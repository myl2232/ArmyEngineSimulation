#include "ArmyCircleArea.h"
#include "Primitive2D/XRPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyClinderArea.h"
#include "ArmyExtrusionActor.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"
#include "ArmySeamlessStyle.h"
#include "ArmyEditorEngine.h"
#include "ArmyPlayerController.h"

FArmyCircleArea::FArmyCircleArea() :FArmyBaseArea()
{
	Position = FVector::ZeroVector;
	Radius = 0.0f;
	MCircle = MakeShareable(new FArmyCircle());
	bBoundingBox = false;
	InneraArea->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial()->GetRenderProxy(false);
	ObjectType = OT_CircleArea;
}

FArmyCircleArea::~FArmyCircleArea()
{

}

void FArmyCircleArea::SetPosition(FVector InPosition)
{
	if (Position != InPosition)
	{
		Position = InPosition;
		MCircle->SetPosition(InPosition);
		RefreshPolyVertices();
		if (SurfaceType == 1)
		{
			if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
			{
				FArmyBaseArea::GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
			}
		}
	}
}

void FArmyCircleArea::Move(const FVector& Offset)
{
	SetPosition(GetPosition() + Offset);
    OffsetExtrusionActors(Offset);

	//移动子区域
	for (auto & SubArea : RoomEditAreas)
	{
		SubArea->Move(Offset);
	}
	RecaculateRelatedArea();
	Modify();
}

FVector FArmyCircleArea::GetPosition3D() const
{
	return Position.X * GetXDir() + Position.Y * GetYDir() + GetPlaneCenter();
}

void FArmyCircleArea::SetRadius(float InRadius)
{
	if (Radius != InRadius)
	{
		Radius = InRadius;
		MCircle->Radius = InRadius;
		RefreshPolyVertices();
		if (SurfaceType == 1)
		{
			if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
			{
				FArmyBaseArea::GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
			}
		}
	}

}

void FArmyCircleArea::SetCenterAndRadius(const FVector& InCenter, float InRadius)
{
	Radius = InRadius;
	MCircle->Radius = InRadius;
	Position = InCenter;
	MCircle->SetPosition(InCenter);
	RefreshPolyVertices();
}

void FArmyCircleArea::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
		{
			DrawConstructionMode(PDI, View);
		}
		else
		{
			if (BeDrawOutLine || GXRPC->GetXRViewMode() == EXRView_CEILING)
			{
				DrawOutLine(PDI, View, FLinearColor(FColor(0XFF666666)));
			}
		}
	}
}

void FArmyCircleArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("position", GetPosition().ToString());
	JsonWriter->WriteValue("radius", GetRadius());
	JsonWriter->WriteValue("extrudedxfFilePath", ExtrudedxfFilePath);
	JsonWriter->WriteValue("m_deltaHeight", m_deltaHeight);
	if (m_deltaHeight != 0)
	{
		ClinderArea->SerializeToJson(JsonWriter);
	}
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyCircleArea)
		FArmyBaseArea::SerializeToJson(JsonWriter);
}

void FArmyCircleArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	Position.InitFromString(InJsonData->GetStringField("position"));
	Radius = InJsonData->GetNumberField("radius");
	FArmyBaseArea::Deserialization(InJsonData);
	RefreshPolyVertices();
	TArray<TArray<FVector>> InHoles;
	MatStyle->SetDrawArea(OutArea->Vertices, InHoles);
	ExtrudedxfFilePath = InJsonData->GetStringField("extrudedxfFilePath");
	m_deltaHeight = InJsonData->GetNumberField("m_deltaHeight");
	if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
	{
		GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
	}
	
	if (m_deltaHeight != 0)
	{
		ClinderArea = MakeShareable(new FArmyClinderArea());
		ClinderArea->Deserialization(InJsonData);
	}
	CalculateWallActor(MatStyle);


	//@郭子阳
	//反序列化子面
	if (IsPlotArea())
	{
		auto SubAreaArray = InJsonData->GetArrayField("SubAreas");

		for (auto & SubAreaValue : SubAreaArray)
		{
			auto SubAreaData = SubAreaValue->AsObject();

			TSharedPtr<FArmyBaseArea> SubArea = nullptr;
			EDrawAreaType SubAreaType = (EDrawAreaType)SubAreaData->GetIntegerField(TEXT("SubAreaType"));
			switch (SubAreaType)
			{
			case EDrawAreaType::Circle:
				SubArea = MakeShared<FArmyCircleArea>();
				break;
			case EDrawAreaType::CuastomPolygon:
				SubArea = MakeShared<FArmyFreePolygonArea>();
				break;
			case EDrawAreaType::RegularPolygon:
				SubArea = MakeShared<FArmyRegularPolygonArea>();
				break;
			case EDrawAreaType::Rectangle:
				SubArea = MakeShared<FArmyRectArea>();
				break;
			default:
				//发生异常
				continue;
			}

			if (!SubArea.IsValid())
			{
				//发生异常
				return;
			}
			XRArgument arg;
			arg._ArgUint32 = E_HardModel;
			FArmySceneData::Get()->Add(SubArea, arg, this->AsShared());
			SubArea->Deserialization(SubAreaData);
		}
	}
}

TSharedPtr<FArmyBaseArea> FArmyCircleArea::ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		TArray<TempRoomInstersection> tempIntersection;
		if (IsSelected(Pos, ViwePortClient, dist))
		{
			tempIntersection.Push(TempRoomInstersection(StaticCastSharedRef<FArmyBaseArea>(this->AsShared()), dist));
		}
		if (ClinderArea.IsValid() && ClinderArea->IsSelected(Pos, ViwePortClient, dist))
		{
			tempIntersection.Push(TempRoomInstersection(ClinderArea, dist));
		}
		tempIntersection.Sort();
		if (tempIntersection.Num() > 0)
		{
			dist = tempIntersection[0].dist;
			return tempIntersection[0].roomSpace.Pin();
		}
		return NULL;
	}
	return NULL;
}

void FArmyCircleArea::ApplyTransform(const FTransform& Translation)
{
	FVector newPosition = Translation.TransformPosition(Position);

	SCOPE_TRANSACTION(TEXT("六轴坐标尺移动圆形位置"));
	SetPosition(newPosition);
	TArray<TArray<FVector>> holes;
	MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	CalculateWallActor(MatStyle);
	RefreshExtrusionPlane();
	PositionChanged.Broadcast();
	this->Modify();
	GXREditor->SelectNone(true, true, false);
}
void FArmyCircleArea::GetCircleLines(TArray<TSharedPtr<FArmyCircle>>& OutCircle)
{
	OutCircle.Push(MCircle);
}

const FBox FArmyCircleArea::GetBounds()
{
	FVector first = Position + Radius * FVector(1, 0, 0) + Radius * FVector(0, 1, 0);
	FVector second = Position + Radius * FVector(-1, 0, 0) + Radius*FVector(0, -1, 0);
	FVector third = Position + Radius* FVector(-1, 0, 0) + Radius * FVector(0, 1, 0);
	FVector fourth = Position + Radius * FVector(1, 0, 0) + Radius*FVector(0, -1, 0);
	TArray<FVector> box = { first,second,third,fourth };

	return FBox(box);
}

void FArmyCircleArea::SetExtrusionHeight(float InHeight)
{
	if (M_ExtrusionHeight != InHeight)
	{
		M_ExtrusionHeight = InHeight;
		TWeakPtr<FArmyObject> parent = GetParents()[0];
		if (!parent.IsValid())
			return;
		FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
		m_deltaHeight = temp->GetExtrusionHeight() - M_ExtrusionHeight;
		RefreshExtrusionPlane();
		MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		CalculateWallActor(MatStyle);
	}
	FArmyBaseArea::SetExtrusionHeight(InHeight);
}

void FArmyCircleArea::RefreshExtrusionPlane()
{
	if (GetParents().Num() > 0)
	{
		TWeakPtr<FArmyObject> parent = GetParents()[0];
		if (parent.IsValid())
		{
			FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
			m_deltaHeight = temp->GetExtrusionHeight() - M_ExtrusionHeight;
		}
	}
	if (m_deltaHeight == 0.0f)
	{
		if (ClinderArea.IsValid())
		{
			ClinderArea->Destroy();
			ClinderArea = nullptr;
		}
		/* @梁晓菲 面的高度差为0，删除灯槽*/
		if (!GetLampSlotExtruder()->IsEmpty())
		{
			DestroyLampLight();
		}
	}
	FVector tempPos = PlaneOrignPos + PlaneNormal * M_ExtrusionHeight;

	if (!ClinderArea.IsValid() )
	{
		if (m_deltaHeight != 0.0f)
		{
		ClinderArea = MakeShareable(new FArmyClinderArea());
		ClinderArea->SetPlaneInfo(PlaneXDir, PlaneYDir, tempPos);
		ClinderArea->SetClinderPositionInfo(Position, Radius, m_deltaHeight);
		ClinderArea->SurfaceType = SurfaceType;
		ClinderArea->SurportPlaceArea = SurportPlaceArea;

		TSharedPtr<FArmySeamlessStyle> tempStyle = MakeShareable(new FArmySeamlessStyle());
		if (SurfaceType == 0)
		{
			tempStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureFloor(), 1024, 1024);
			FString Path = TEXT("/Game/XRCommon/HardMode/FloorTexture.FloorTexture");
			tempStyle->SetMainTextureLocalPath(Path);
		}
		else if (SurfaceType == 2)
		{
			tempStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureCeiling(), 520, 520);
			FString Path = TEXT("/Game/XRCommon/HardMode/CeilingTexture.CeilingTexture");
			tempStyle->SetMainTextureLocalPath(Path);
		}
		tempStyle->SetPlaneInfo(ClinderArea->GetPlaneCenter(), ClinderArea->GetXDir(), ClinderArea->GetYDir());
		ClinderArea->SetStyle(tempStyle);
		}
	}
	else
	{
		ClinderArea->SetPlaneInfo(PlaneXDir, PlaneYDir, tempPos);
		ClinderArea->SetClinderPositionInfo(Position, Radius, m_deltaHeight);
	}
    if (!GetLampSlotExtruder()->IsEmpty())
    {
		GenerateLampSlot(GVC->GetWorld(), LampContentItem, LampSlotWidth);
    }
    else if (ContentItemList.Num() > 0 && SurfaceType != 1)
    {
		GenerateExtrudeActor(GVC->GetWorld(), ContentItemList[0]);
    }
}

void FArmyCircleArea::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyBaseArea::SetPropertyFlag(InType, InUse);
}

void FArmyCircleArea::GenerateLampSlot(
    UWorld* InWorld,
    TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
    float InLampSlotWidth,
    bool bIsLoad /*= false*/)
{
	// @欧石楠 如果没有灯槽则不生成
	if (SurfaceType != 2)
	{
		return;
	}
	if (bCanGenerateLampSlot(InLampSlotWidth))
	{
		FArmyBaseArea::GenerateLampSlot(InWorld, ContentItem, InLampSlotWidth);
		if (ClinderArea.IsValid())
			ClinderArea->SetClinderPositionInfo(Position, Radius + LampSlotWidth + 0.2f, FMath::Abs(m_deltaHeight));
	}
}

void FArmyCircleArea::Destroy()
{
	FArmyBaseArea::Destroy();
	if (ClinderArea.IsValid())
		ClinderArea->Destroy();
}

void FArmyCircleArea::SetIsTempArea()
{
	IsTempArea = true;

}

void FArmyCircleArea::RefreshPolyVertices()
{

	FVector startPos = Position + FVector(-1, 0, 0)*Radius;
	TArray<FVector> TotalVertices;
	int number = 64;
	float deltaAngle = 360.0f / number;
	float startAngle = 0.0f;
	for (int i = 0; i < number; i++)
	{
		FVector tempPoint = FRotator(0, startAngle, 0).RotateVector(startPos - Position) + Position;
		startAngle += deltaAngle;
		TotalVertices.Push(tempPoint);
	}

	InneraArea->SetVertices(TotalVertices);
	InneraArea->SetPolygonOffset(POLYGONOFFSETDIS);
	OutArea->SetVertices(TotalVertices);
	OutArea->SetPolygonOffset(POLYGONOFFSETDIS);

	if (!IsTempArea)
	{
		TArray<FVector> tempOutVertices;
		TArray<TArray<FVector>> Holes;
		CalculateOutAndInnerHoles(tempOutVertices, Holes);
		if (SurfaceType == 1)
		{
			if (dropVerts.Num() > 0)
				Holes.Emplace(dropVerts);
		}

		if (MatStyle.IsValid())
		{

			MatStyle->SetDrawArea(OutArea->Vertices, Holes);
			if (M_ExtrusionHeight != 0.0f)
				MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
			CalculateWallActor(MatStyle);
		}

		if (!BeDrawOutLine && Radius != 0.0f)
		{
			TArray<TArray<FVector>> InnerHoles;
			CalculateBackGroundSurface(OutArea->Vertices, InnerHoles);
		}
		//@郭子阳 刷新子面
		for (auto & SubArea : RoomEditAreas)
		{
			SubArea->RefreshPolyVertices();
		}
	}
}