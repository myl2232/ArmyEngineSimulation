#include "ArmyRectArea.h"
#include "ArmyFreePolygonArea.h"


#include "Engine/PointLight.h"
#include "Components/PointLightComponent.h"
#include "Primitive2D/XRPolygon.h"
#include "SceneManagement.h"
#include "Data/HardModeData/GridSystemCore/XRBrickUnit.h"
#include "ArmySceneData.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyActorConstant.h"
#include "ArmyPlayerController.h"

FArmyRectArea::FArmyRectArea() :FArmyBaseArea()
{
	BodaThickness = 0.0f;
	M_DodaType = AntiCornerBrick;
	ObjectType = OT_RectArea;
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat();
	MI_BodaTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_BodaTextureMat->AddToRoot();
	MI_CornerTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_CornerTextureMat->AddToRoot();
	RoomDataStatus &= ~FLAG_HASLIGHT;
}

void FArmyRectArea::IntervalDistChange(float InV)
{
	RefreshPolyVertices();
}

void FArmyRectArea::SetCornerType(BodaCornerType InType)
{
	if (M_DodaType != InType)
	{
		M_DodaType = InType;
		RefreshPolyVertices();
		TArray<TArray<FVector>> holes;
		MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	}
}

void FArmyRectArea::RefreshBodaBrick()
{
	BodaSurfaceArea->CaculateBodaArea(OutArea->Vertices, BodaThickness, BodaBrickLength, BodaSurfaceArea->GetBodaThick());
}

void FArmyRectArea::SetBodaBrickAndThickness(float length, float width, TSharedPtr<FContentItemSpace::FContentItem > ContentItem)
{
	if (!ContentItem.IsValid())
		return;

	BodaThickness = width;
	BodaBrickLength = length;
	FVector BodaAreaPlaneOrignPos = FVector(PlaneOrignPos.X, PlaneOrignPos.Y, PlaneOrignPos.Z + M_ExtrusionHeight);
	if (!BodaSurfaceArea.IsValid())
	{
		BodaSurfaceArea = MakeShareable(new FArmyBodaArea(OutArea->Vertices, BodaThickness, length, PlaneXDir, PlaneYDir, BodaAreaPlaneOrignPos));
	}
	BodaSurfaceArea->AttachParentArea = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
	TSharedPtr<FArmySeamlessStyle> M_BodaStyle = MakeShareable(new FArmySeamlessStyle());
	M_BodaStyle->SetBrickWidthAndHeight(length* 10.0f, width*10.0f);
	BodaSurfaceArea->CaculateBodaArea(OutArea->Vertices, BodaThickness, BodaBrickLength, BodaSurfaceArea->GetBodaThick());
	M_BodaStyle->SetCurrentItem(ContentItem);
	BodaSurfaceArea->SetStyle(M_BodaStyle);
	BodaSurfaceArea->SetBodaContentItem(ContentItem);
	BodaSurfaceArea->UpdateBodaAreaBrickDist(0.2);//@梁晓菲 每次替换材质的时候，波打线缝隙重置为0
	if (BodaBrickLength == 0.0f)
		BodaBrickLength = 40.0f;
	RefreshPolyVertices();
	TArray<TArray<FVector>> holes;

	TArray<FVector> OutLine;
	CalculateOutAndInnerHoles(OutLine, holes);
	MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	CalculateWallActor(MatStyle);
}

void FArmyRectArea::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
		{
			if (BodaSurfaceArea.IsValid())
			{
				BodaSurfaceArea->DrawConstructionMode(PDI, View);
			}
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

void FArmyRectArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("leftUpCornerPoint", LeftUpCornerPoint.ToString());
	JsonWriter->WriteValue("areaWidth", AreaWidth);
	JsonWriter->WriteValue("areaHeight", AreaHeight);
	JsonWriter->WriteValue("bodaThickness", BodaThickness);
	JsonWriter->WriteValue("BodaBrickLength", BodaBrickLength);
	JsonWriter->WriteValue("bodaCornerType", (int32)M_DodaType);

	/** @梁晓菲 序列化波打线区域*/
	JsonWriter->WriteObjectStart("BodaSurfaceArea");
	if (BodaSurfaceArea.IsValid())
	{
		BodaSurfaceArea->SerializeToJson(JsonWriter);
	}
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteValue("state", GetState());
	if (BodaThickness > 0)
	{
		JsonWriter->WriteValue("bodaTextureURL", GetBodaTextureURL());
		JsonWriter->WriteValue("mainTextureGoodID", MainBrickID);
		if (M_DodaType == BodaCornerType::CornerBrick)
		{
			JsonWriter->WriteValue("cornerTextureURL", GetCornerTextureURL());
			JsonWriter->WriteValue("cornerBrickID", CornerBrickID);
		}
	}
	JsonWriter->WriteValue("extrudedxfFilePath", ExtrudedxfFilePath);
	JsonWriter->WriteValue("ExturdedxfMaterialFilePath", ExturdedxfMaterialFilePath);
	JsonWriter->WriteValue("m_deltaHeight", m_deltaHeight);
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyRectArea)
		FArmyBaseArea::SerializeToJson(JsonWriter);
	if (m_deltaHeight != 0.0)
	{
		JsonWriter->WriteArrayStart("InnearHoleSurface");
		int tempNumber = InnearHoleSurface.Num();
		for (int i = 0; i < tempNumber; ++i)
		{
			JsonWriter->WriteObjectStart();
			InnearHoleSurface[i]->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}

	//@打扮家 XRLightmass 序列化 按顺序保存灯带每个PointLight的LightGUID
	JsonWriter->WriteArrayStart("LampLightComponentGUIDs");
	for (auto& It : GetLampSlotExtruder()->GetLampLightIDs())
	{
		FString IDStr = It.ToString();
		JsonWriter->WriteValue(*IDStr);
	}
	JsonWriter->WriteArrayEnd();

    // @欧石楠 序列化灯槽
    JsonWriter->WriteObjectStart("lampSlot");
    GetLampSlotExtruder()->SerializeToJson(JsonWriter);
    JsonWriter->WriteObjectEnd();
}

void FArmyRectArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	LeftUpCornerPoint.InitFromString(InJsonData->GetStringField("leftUpCornerPoint"));

	AreaWidth = InJsonData->GetNumberField("areaWidth");
	AreaHeight = InJsonData->GetNumberField("areaHeight");
	BodaThickness = InJsonData->GetNumberField("bodaThickness");
	BodaBrickLength = InJsonData->GetNumberField("BodaBrickLength");
	M_DodaType = BodaCornerType(int32(InJsonData->GetNumberField("bodaCornerType")));

	FArmyBaseArea::Deserialization(InJsonData);
	RefreshPolyVertices();

	TArray<TArray<FVector>> InHoles;
	MatStyle->SetDrawArea(InneraArea->Vertices, InHoles);
	ExtrudedxfFilePath = InJsonData->GetStringField("extrudedxfFilePath");
	ExturdedxfMaterialFilePath = InJsonData->GetStringField("ExturdedxfMaterialFilePath");
	double tempDelta;
	if (InJsonData->TryGetNumberField("m_deltaHeight", tempDelta))
	{
		m_deltaHeight = tempDelta;
		if (m_deltaHeight != 0.0)
		{
			TArray<TSharedPtr<FJsonValue> > surfaceHolesJson = InJsonData->GetArrayField("InnearHoleSurface");
			if (InnearHoleSurface.Num() == 0)
			{
				for (auto& L : surfaceHolesJson)
				{
					const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
					TSharedPtr<FArmyHelpRectArea> tempHelpRect = MakeShareable(new FArmyHelpRectArea());
					tempHelpRect->SurportPlaceArea = SurportPlaceArea;
					tempHelpRect->SurfaceType = SurfaceType;
					tempHelpRect->Deserialization(LJsonObj);
					InnearHoleSurface.Push(tempHelpRect);
				}
			}
			else if (surfaceHolesJson.Num() == InnearHoleSurface.Num())
			{
				for (int i = 0; i < surfaceHolesJson.Num(); ++i)
				{
					const TSharedPtr<FJsonObject> LJsonObj = surfaceHolesJson[i]->AsObject();
					InnearHoleSurface[i]->Deserialization(LJsonObj);
				}
			}
		}
	}
	if (!GetLampSlotExtruder()->IsEmpty())
	{
		//@打扮家 XRLightmass 序列化 先于GenerateLampSlot反序列化，按灯带固定的一圈顺序，加载每个PointLight的LightGUID
		GetLampSlotExtruder()->GetLampLightIDs().Reset();
		const TArray<TSharedPtr<FJsonValue>> LampLightComponentGUIDsData = InJsonData->GetArrayField(TEXT("LampLightComponentGUIDs"));
		for (TSharedPtr<FJsonValue> It : LampLightComponentGUIDsData)
		{
			FString IDStr = It->AsString();
			FGuid ID;
			FGuid::Parse(IDStr, ID);
            GetLampSlotExtruder()->GetLampLightIDs().Add(ID);
		}
	}
	if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
	{
		GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
	}
	CalculateWallActor(MatStyle);

	/** @梁晓菲 反序列化波打线区域*/
	TSharedPtr<FJsonObject> BodaSurfaceData = InJsonData->GetObjectField(TEXT("BodaSurfaceArea"));
	if (BodaSurfaceData.IsValid())
	{
		TSharedPtr<class FArmyBodaArea> TempBodaSurfaceArea = MakeShareable(new FArmyBodaArea(OutArea->Vertices, BodaThickness, BodaBrickLength, PlaneXDir, PlaneYDir, PlaneOrignPos));
		TempBodaSurfaceArea->Deserialization(BodaSurfaceData);
		if (TempBodaSurfaceArea.IsValid() && TempBodaSurfaceArea->GetBodaContentItem().IsValid())
		{
			FVector BodaAreaPlaneOrignPos = FVector(PlaneOrignPos.X, PlaneOrignPos.Y, PlaneOrignPos.Z + M_ExtrusionHeight);
			//BodaSurfaceArea = MakeShareable(new FArmyBodaArea(OutArea->Vertices, BodaThickness, BodaBrickLength, PlaneXDir, PlaneYDir, BodaAreaPlaneOrignPos));
			BodaSurfaceArea = TempBodaSurfaceArea;
			float BrickDist = BodaSurfaceArea->GetBodaThick();//先记录波打线缝隙，SetBodaBrickAndThickness会重置缝隙值，因为每次替换材质的时候会调用此函数
			SetBodaBrickAndThickness(BodaSurfaceArea->GetBodaContentItem()->ProObj->Length / 10.f, BodaSurfaceArea->GetBodaContentItem()->ProObj->Width / 10.f, BodaSurfaceArea->GetBodaContentItem());
			UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
			UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(BodaSurfaceArea->GetBodaContentItem());
			UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
			SetBodaMaterial(MID, BodaSurfaceArea->GetBodaContentItem()->ID);
			BodaSurfaceArea->UpdateBodaAreaBrickDist(BrickDist);
		}
	}
	if (BodaThickness > 0)
	{
		BodaTextureUrl = InJsonData->GetStringField("bodaTextureURL");
		MainBrickID = InJsonData->GetIntegerField("mainTextureGoodID");
		if (BodaBrickLength == 0.0f)
			BodaBrickLength = 40.0f;
	}
	ExtrusionBodaArea();


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
			SubArea->Deserialization(SubAreaData);
			FArmySceneData::Get()->Add(SubArea, arg, this->AsShared());
		}
	}

    // @欧石楠 反序列化灯槽
    const TSharedPtr<FJsonObject>* LampSlotObj = nullptr;
    if (InJsonData->TryGetObjectField("lampSlot", LampSlotObj))
    {
        GetLampSlotExtruder()->Deserialization(*LampSlotObj);
    }
}

bool FArmyRectArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	FVector tempStart, tempEnd;
	if (FArmyBaseArea::IsSelected(Pos, ViwePortClient, dist) || HoverSingleEdge(FVector2D(Pos), ViwePortClient, tempStart, tempEnd))
	{
		return true;
	}
	return false;
}

TSharedPtr<FArmyBaseArea> FArmyRectArea::ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		TArray<TempRoomInstersection> tempIntersection;
		for (TSharedPtr<FArmyBaseArea> iter : InnearHoleSurface)
		{
			if (iter->IsSelected(Pos, ViwePortClient, dist))
			{
				tempIntersection.Push(TempRoomInstersection(iter, dist));
			}

		}
		if (IsSelected(Pos, ViwePortClient, dist))
		{
			tempIntersection.Push(TempRoomInstersection(StaticCastSharedRef<FArmyBaseArea>(this->AsShared()), dist));
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

bool FArmyRectArea::HoverSingleEdge(const FVector2D& Pos, class UArmyEditorViewportClient* ViewPortClient, FVector& outStart, FVector& outEnd)
{
	int number = OutArea->Vertices.Num();
	for (int i = 0; i < number; ++i)
	{
		FVector start = OutArea->Vertices[i%number].X * PlaneXDir + OutArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
		FVector end = OutArea->Vertices[(i + 1) % number].X * PlaneXDir + OutArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos;

		FVector2D ScreenStart, ScreenEnd;
		ViewPortClient->WorldToPixel(start, ScreenStart);
		ViewPortClient->WorldToPixel(end, ScreenEnd);

		float dist = FMath::PointDistToSegment(FVector(Pos, 0.0), FVector(ScreenStart, 0.0f), FVector(ScreenEnd, 0.0f));
		float delta = FVector::DotProduct(FVector(ScreenEnd - Pos, 0.0f), FVector(ScreenStart - Pos, 0.0f));

		if (dist < 16 && delta <= 0.0)
		{
			float tempOffset = SurfaceType == 2 ? -10.0f : 10.0f;
			outStart = start + PlaneNormal * tempOffset;
			outEnd = end + PlaneNormal* tempOffset;

			//设定边界偏移方向
			switch (i)
			{
			case 0:
				TransformFlag = 4;
				break;
			case 1:
				TransformFlag = 1;
				break;
			case 2:
				TransformFlag = 3;
				break;
			case 3:
				TransformFlag = 2;
				break;
			}


			return true;
		}
	}
	return false;
}

void FArmyRectArea::ApplyTransform(const FTransform& tempTrans)
{
	FVector	tempLeftUp = LeftUpCornerPoint;
	float tempAreaWidth = AreaWidth;
	float tempAreaHeight = AreaHeight;

	FVector delta = tempTrans.GetTranslation();

	

	if (DriveByHoleOrBoundary == 0)
	{
		tempLeftUp = tempTrans.TransformPosition(LeftUpCornerPoint);

	}
	else if (DriveByHoleOrBoundary == 1)
	{
		//@郭子阳
		//测试数据
		UE_LOG(LogTemp, Warning, TEXT("flag %d  delta %s"), CurrentTransformFlag, *delta.ToString())

		if (CurrentTransformFlag == 1)
		{
			tempAreaWidth += delta.X;
		}
		else if (CurrentTransformFlag == 2)
		{
			//tempLeftUp = tempTrans.TransformPosition(LeftUpCornerPoint);
			tempAreaWidth -= delta.X;
			tempLeftUp.X+= delta.X;
		}
		else if (CurrentTransformFlag == 3)
		{
			tempAreaHeight += delta.Y;
		}
		else if (CurrentTransformFlag == 4)
		{
			//tempLeftUp = tempTrans.TransformPosition(LeftUpCornerPoint);
			tempAreaHeight -= delta.Y;
			tempLeftUp.Y += delta.Y;
		}
	}

	SCOPE_TRANSACTION(TEXT("六轴坐标尺移动矩形位置"));
	LeftUpCornerPoint = tempLeftUp;
	AreaWidth = tempAreaWidth;
	AreaHeight = tempAreaHeight;
	RefreshPolyVertices();
	TArray<TArray<FVector>> holes;
	MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	CalculateWallActor(MatStyle);
	//PositionChanged.Broadcast();
	//BoundaryDriveEvent.ExecuteIfBound();
	RefreshExtrusionPlane();
	if (SurfaceType == 1)
	{
		if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
		{
			FArmyBaseArea::GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
		}
	}
	this->Modify();
	GXREditor->SelectNone(true, true, false);

	/* @梁晓菲 有波打线，需要更新波打线*/
	if (HasBodaThickness())
	{
		RefreshBodaBrick();
	}
}

void FArmyRectArea::SetVertice(const TArray<FVector>& InVertices)
{
	FBox box(InVertices);
	LeftUpCornerPoint = box.Min;
	AreaWidth = (box.Max - box.Min).X;
	AreaHeight = (box.Max - box.Min).Y;
	RefreshPolyVertices();

}

void FArmyRectArea::SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)
{
	TArray<FVector> OutLine;
	TArray<TArray<FVector>> innear;
	CalculateOutAndInnerHoles(OutLine, innear);
	
	if (GetLampSlotExtruder()->IsEmpty())
	{
		MatStyle = InStyle;
		MatStyle->SetDrawArea(InneraArea->Vertices, innear);
		MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		MatStyle->StyleDirty.BindRaw(this, &FArmyRectArea::RefreshPolyVertices);
	}
	else
	{
		if (m_deltaHeight != 0)
		{
			MatStyle = InStyle;
			TArray<FVector> tempNewVerts = InneraArea->Vertices;
			tempNewVerts = FArmyMath::Extrude3D(tempNewVerts, -LampSlotWidth, true);
			MatStyle->SetDrawArea(tempNewVerts, innear);
			MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		}
	}
	MatStyle->StyleDirty.BindRaw(this, &FArmyRectArea::UpdateWallActor);
	CalculateWallActor(MatStyle);

}

void FArmyRectArea::PostModify(bool bTransaction/* =false */)
{
	PositionChanged.Broadcast();
	BoundaryDriveEvent.ExecuteIfBound();
}

const FBox FArmyRectArea::GetBounds()
{
	FBox box(OutArea->Vertices);
	return box;
}

void FArmyRectArea::SetRectWidth(const float InWidth)
{
	if (AreaWidth != InWidth)
	{
		AreaWidth = InWidth;
		RefreshPolyVertices();
	}
}

void FArmyRectArea::SetRectHeight(const float InHeight)
{
	if (AreaHeight != InHeight)
	{
		AreaHeight = InHeight;
		RefreshPolyVertices();
	}
}

void FArmyRectArea::SetLeftUpCornerPos(const FVector InCenterPosition)
{
	if (LeftUpCornerPoint != InCenterPosition)
	{
		LeftUpCornerPoint = InCenterPosition;
		RefreshPolyVertices();
	}
}

void FArmyRectArea::SetCenterPosition(const FVector InPosition)
{
	LeftUpCornerPoint = InPosition - FVector(1, 0, 0)*AreaWidth / 2.0f - FVector(0, 1, 0)*AreaHeight / 2.0f;
	RefreshPolyVertices();
}

FVector FArmyRectArea::GetCenterPosition()
{
	return LeftUpCornerPoint + FVector(1, 0, 0)*AreaWidth / 2.0f + FVector(0, 1, 0)*AreaHeight / 2.0f;
}

void FArmyRectArea::DeleteBodaArea()
{
	if (BodaSurfaceArea.IsValid())
	{
		BodaBrickLength = 0;
		BodaThickness = 0;

		TArray<FVector> OutLine;
		TArray<TArray<FVector>> InHoles;
		CalculateOutAndInnerHoles(OutLine, InHoles);
		MatStyle->SetDrawArea(OutArea->Vertices, InHoles);
		CalculateWallActor(MatStyle);
		BodaSurfaceArea->Destroy();
		BodaSurfaceArea = NULL;
	}
}

const TArray<FVector>& FArmyRectArea::GetOutVertices() const
{
	return OutArea->Vertices;
}
void FArmyRectArea::SetBodaMaterial(UMaterialInstanceDynamic* InBodaMaterial, int32 mainID)
{
	MainBrickID = mainID;
	if (BodaSurfaceArea.IsValid())
	{
		BodaSurfaceArea->SetBodaMaterial(InBodaMaterial);
	}
}

void FArmyRectArea::SetExtrusionHeight(float height)
{
	if (M_ExtrusionHeight != height)
	{
		M_ExtrusionHeight = height;
		RefreshExtrusionPlane();
		MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		ExtrusionBodaArea();
		CalculateWallActor(MatStyle);
	}

	FArmyBaseArea::SetExtrusionHeight(height);
}

void FArmyRectArea::RefreshExtrusionPlane()
{
	if (GetParents().Num() != 0)
	{
		TWeakPtr<FArmyObject> parent = GetParents()[0];
		if (!parent.IsValid())
			return;
		FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
		m_deltaHeight = temp->GetExtrusionHeight() - M_ExtrusionHeight;
		CalculateVerticalHole(m_deltaHeight, LampSlotWidth);
	}
	/* @梁晓菲 有波打线，需要更新波打线*/
	if (HasBodaThickness())
	{
		RefreshBodaBrick();
	}
}

void FArmyRectArea::CalculateVerticalHole(float deltaHeight, float lampSlotWidth)
{
	if (deltaHeight == 0.0)
	{
		for (TSharedPtr<FArmyHelpRectArea> iter : InnearHoleSurface)
		{
			iter->BeginDestroy();
		}
		InnearHoleSurface.Empty();
		return;
	}
	TArray<FVector> newInnearVerts = OutArea->Vertices;
    if (lampSlotWidth != 0)
    {
        newInnearVerts = FArmyMath::Extrude3D(newInnearVerts, -lampSlotWidth, true);
    }
	if (!FArmyMath::IsClockWise(newInnearVerts))
		FArmyMath::ReversePointList(newInnearVerts);

	int number = newInnearVerts.Num();

	struct TempRectEdge
	{
		FVector startPos;
		FVector endPos;
		TempRectEdge(const FVector& InStartPos, const FVector& InEndPos)
		{
			startPos = InStartPos;
			endPos = InEndPos;
		}
		TempRectEdge& operator()(const FVector& InStartPos, const FVector& InEndPos)
		{
			startPos = InStartPos;
			endPos = InEndPos;
			return *this;
		}
	};
	TArray<TempRectEdge> InvalidEdges;
	//if (GetParents().Num() == 1)
	//{
	//	FArmyRoomSpaceArea* tempParent = GetParents()[0].Pin()->AsassignObj<FArmyRoomSpaceArea>();
	//	if (tempParent)
	//	{
	//		const TArray<FVector>& OutVerts = tempParent->GetOutArea()->Vertices;
	//		for (int i = 0; i < number; i++)
	//		{
	//			const FVector& tempStart = newInnearVerts[i%number];
	//			const FVector& tempEnd = newInnearVerts[(i + 1) % number];
	//			if (!Translate::IsLineOnPolygonArea(tempStart, tempEnd, OutVerts))
	//			{
	//				const FVector& contvertStart = tempStart.X* PlaneXDir + tempStart.Y * PlaneYDir + PlaneOrignPos;
	//				const  FVector& convetEnd = tempEnd.X * PlaneXDir + tempEnd.Y * PlaneYDir + PlaneOrignPos;
	//				TempRectEdge tempEdge(contvertStart, convetEnd);
	//				InvalidEdges.Emplace(tempEdge);
	//			}
	//		}

	//	}
	//}
	//else
	{
		for (int i = 0; i < number; i++)
		{
			const FVector& tempStart = newInnearVerts[i%number];
			const FVector& tempEnd = newInnearVerts[(i + 1) % number];
			const FVector& contvertStart = tempStart.X* PlaneXDir + tempStart.Y * PlaneYDir + PlaneOrignPos;
			const FVector& convetEnd = tempEnd.X * PlaneXDir + tempEnd.Y * PlaneYDir + PlaneOrignPos;
			TempRectEdge tempEdge(contvertStart, convetEnd);
			InvalidEdges.Emplace(tempEdge);
		}
	}

	if (InnearHoleSurface.Num() <= InvalidEdges.Num())
	{
		int number0 = InnearHoleSurface.Num();
		int number1 = InvalidEdges.Num();
		for (int i = 0; i < number0; i++)
		{
			FVector xdir, yDir, tempCenter;
			TArray<FVector> rectVertics;
			CalculatePlaneInfo(InvalidEdges[i].startPos + PlaneNormal* M_ExtrusionHeight, InvalidEdges[i].endPos + PlaneNormal*M_ExtrusionHeight, deltaHeight, xdir, yDir, tempCenter, rectVertics);
			InnearHoleSurface[i]->GetStyle()->SetPlaneInfo(tempCenter, xdir, yDir);
			InnearHoleSurface[i]->SetPlaneInfo(xdir, yDir, tempCenter);
			InnearHoleSurface[i]->SetVertices(rectVertics);
		}
		for (int i = number0; i < number1; ++i)
		{
			TSharedPtr<FArmyHelpRectArea> innearRect = CalculateInnerRectArea(InvalidEdges[i].startPos + PlaneNormal* M_ExtrusionHeight, InvalidEdges[i].endPos + PlaneNormal*M_ExtrusionHeight, deltaHeight);
			InnearHoleSurface.Push(innearRect);
            innearRect->SurportPlaceArea = SurfaceType;
		}
	}
	else
	{
		while (InnearHoleSurface.Num() > InvalidEdges.Num())
		{
			TSharedPtr<FArmyHelpRectArea> top = InnearHoleSurface.Pop();
			top->Destroy();
		}
		int number0 = InvalidEdges.Num();
		for (int i = 0; i < number0; i++)
		{
			FVector xdir, yDir, tempCenter;
			TArray<FVector> rectVertics;
			CalculatePlaneInfo(InvalidEdges[i].startPos + PlaneNormal* M_ExtrusionHeight, InvalidEdges[i].endPos + PlaneNormal*M_ExtrusionHeight, deltaHeight, xdir, yDir, tempCenter, rectVertics);
			InnearHoleSurface[i]->GetStyle()->SetPlaneInfo(tempCenter, xdir, yDir);
			InnearHoleSurface[i]->SetPlaneInfo(xdir, yDir, tempCenter);
			InnearHoleSurface[i]->SetVertices(rectVertics);
		}
	}
}

void FArmyRectArea::CalculateVerticalHole(float deltaHeight, float lampSlotWidth, TArray<FVector> NewVertexes)
{
    if (deltaHeight == 0.0)
    {
        for (TSharedPtr<FArmyHelpRectArea> iter : InnearHoleSurface)
        {
            iter->BeginDestroy();
        }
        InnearHoleSurface.Empty();
        return;
    }

    int number = NewVertexes.Num();

    struct TempRectEdge
    {
        FVector startPos;
        FVector endPos;
        TempRectEdge(const FVector& InStartPos, const FVector& InEndPos)
        {
            startPos = InStartPos;
            endPos = InEndPos;
        }
        TempRectEdge& operator()(const FVector& InStartPos, const FVector& InEndPos)
        {
            startPos = InStartPos;
            endPos = InEndPos;
            return *this;
        }
    };
    TArray<TempRectEdge> InvalidEdges;
    //if (GetParents().Num() == 1)
    //{
    //	FArmyRoomSpaceArea* tempParent = GetParents()[0].Pin()->AsassignObj<FArmyRoomSpaceArea>();
    //	if (tempParent)
    //	{
    //		const TArray<FVector>& OutVerts = tempParent->GetOutArea()->Vertices;
    //		for (int i = 0; i < number; i++)
    //		{
    //			const FVector& tempStart = NewVertexes[i%number];
    //			const FVector& tempEnd = NewVertexes[(i + 1) % number];
    //			if (!Translate::IsLineOnPolygonArea(tempStart, tempEnd, OutVerts))
    //			{
    //				const FVector& contvertStart = tempStart.X* PlaneXDir + tempStart.Y * PlaneYDir + PlaneOrignPos;
    //				const  FVector& convetEnd = tempEnd.X * PlaneXDir + tempEnd.Y * PlaneYDir + PlaneOrignPos;
    //				TempRectEdge tempEdge(contvertStart, convetEnd);
    //				InvalidEdges.Emplace(tempEdge);
    //			}
    //		}

    //	}
    //}
    //else
    {
        for (int i = 0; i < number; i++)
        {
            const FVector& tempStart = NewVertexes[i%number];
            const FVector& tempEnd = NewVertexes[(i + 1) % number];
            const FVector& contvertStart = tempStart.X* PlaneXDir + tempStart.Y * PlaneYDir + PlaneOrignPos;
            const FVector& convetEnd = tempEnd.X * PlaneXDir + tempEnd.Y * PlaneYDir + PlaneOrignPos;
            TempRectEdge tempEdge(contvertStart, convetEnd);
            InvalidEdges.Emplace(tempEdge);
        }
    }

    if (InnearHoleSurface.Num() <= InvalidEdges.Num())
    {
        int number0 = InnearHoleSurface.Num();
        int number1 = InvalidEdges.Num();
        for (int i = 0; i < number0; i++)
        {
            FVector xdir, yDir, tempCenter;
            TArray<FVector> rectVertics;
            CalculatePlaneInfo(InvalidEdges[i].startPos + PlaneNormal* M_ExtrusionHeight, InvalidEdges[i].endPos + PlaneNormal*M_ExtrusionHeight, deltaHeight, xdir, yDir, tempCenter, rectVertics);
            InnearHoleSurface[i]->GetStyle()->SetPlaneInfo(tempCenter, xdir, yDir);
            InnearHoleSurface[i]->SetPlaneInfo(xdir, yDir, tempCenter);
            InnearHoleSurface[i]->SetVertices(rectVertics);
        }
        for (int i = number0; i < number1; ++i)
        {
            TSharedPtr<FArmyHelpRectArea> innearRect = CalculateInnerRectArea(InvalidEdges[i].startPos + PlaneNormal* M_ExtrusionHeight, InvalidEdges[i].endPos + PlaneNormal*M_ExtrusionHeight, deltaHeight);
            InnearHoleSurface.Push(innearRect);
            if (SurfaceType == 2)
            {
                innearRect->SurportPlaceArea = 2;
            }
            else if (SurfaceType == 0)
            {
                innearRect->SurportPlaceArea = 0;
            }
        }
    }
    else
    {
        while (InnearHoleSurface.Num() > InvalidEdges.Num())
        {
            TSharedPtr<FArmyHelpRectArea> top = InnearHoleSurface.Pop();
            top->Destroy();
        }
        int number0 = InvalidEdges.Num();
        for (int i = 0; i < number0; i++)
        {
            FVector xdir, yDir, tempCenter;
            TArray<FVector> rectVertics;
            CalculatePlaneInfo(InvalidEdges[i].startPos + PlaneNormal* M_ExtrusionHeight, InvalidEdges[i].endPos + PlaneNormal*M_ExtrusionHeight, deltaHeight, xdir, yDir, tempCenter, rectVertics);
            InnearHoleSurface[i]->GetStyle()->SetPlaneInfo(tempCenter, xdir, yDir);
            InnearHoleSurface[i]->SetPlaneInfo(xdir, yDir, tempCenter);
            InnearHoleSurface[i]->SetVertices(rectVertics);
        }
    }
}

void FArmyRectArea::GenerateLampSlot(
    UWorld* InWorld,
    TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
    float InLampSlotWidth,
    bool bIsLoad /*= false*/)
{
    // @欧石楠 如果从方案加载，直接生成灯槽，不需要重新计算
    if (SurfaceType == 2 && bCanGenerateLampSlot(InLampSlotWidth) && bIsLoad && !GetLampSlotExtruder()->IsEmpty())
    {
        GetLampSlotExtruder()->Generate();
        UpdateWallArea(GetLampSlotExtruder()->GetAreaVertexes());
        return;
    }

    if (SurfaceType == 2 && bCanGenerateLampSlot(InLampSlotWidth) && ContentItem.IsValid())
    {
        TArray<FVector> OutList;
        GetLampSlotExtruder()->Clear();
        CalculateLampSlotPaths(GetLampSlotExtruder()->GetPaths(), OutList, ContentItem, InLampSlotWidth);
        GetLampSlotExtruder()->Generate();
        RecalculateArea(InLampSlotWidth);
    }
}


bool FArmyRectArea::TestAreaCanAddOrNot(const TArray<FVector>& InTestArea, FGuid ObjId, TSharedPtr<FArmyBaseArea> OutParent /*= nullptr*/)
{
	
	//@郭子阳
	//考虑波打线
	if (Translate::ArePolygonInPolygon(OutArea->Vertices, InTestArea)
		&& !Translate::ArePolygonInPolygon(InTestArea, InneraArea->Vertices)
		&& !Translate::ArePolygonInPolygon(InneraArea->Vertices, InTestArea)
		)
	{
		if (BodaSurfaceArea->TestAreaCanAddOrNot(InTestArea, ObjId, OutParent))
		{
			return true;
		}
		OutParent = nullptr;
		return false;
	}

	OutParent = nullptr;
	if (!Translate::ArePolygonInPolygon(InneraArea->Vertices, InTestArea))
		return false;

	return  FArmyBaseArea::TestAreaCanAddOrNot(InTestArea, ObjId, OutParent);
	
}

void FArmyRectArea::GetConstructionAreas(TArray<FArmyGoods>& ArtificialData)
{
	FArmyBaseArea::GetConstructionAreas(ArtificialData);

	if (BodaThickness > 0.0f)
	{
		// 波打线施工项
		
		FArmyGoods goods;
		goods.GoodsId = BodaSurfaceArea->GetBodaMatStyle()->GetContentItem()->ID;
		goods.Type = 1;
		goods.PaveID = ConvertStyleToPavingID(BodaSurfaceArea->GetBodaMatStyle());

		// 施工项
		ConstructionPatameters Parameter;
		Parameter.SetPaveInfo(BodaSurfaceArea->GetBodaMatStyle(), EWallType::All);
		TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(BodaSurfaceArea->GetUniqueID(), Parameter);
		//TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = BodaSurfaceArea->GetBodaMatStyle()->ConstructionItemData;
		for (auto It : tempConstructoinData->CheckedId)
		{

			int32 Quotaindex = goods.QuotaData.Add(FArmyQuota());
			FArmyQuota &Quota = goods.QuotaData[Quotaindex];
			Quota.QuotaId = It.Key;
			for (auto PropertyIt : It.Value)
			{
				Quota.QuotaProperties.Add(FArmyPropertyValue(PropertyIt.Key, PropertyIt.Value));
			}
		}

		int32 GoodsIndex;
		if (!ArtificialData.Find(goods, GoodsIndex))
		{
			GoodsIndex = ArtificialData.Add(goods);
		}
		FArmyGoods& Goods = ArtificialData[GoodsIndex];
		//合并施工项数量
		for (int32 i = 0; i < ArtificialData[GoodsIndex].QuotaData.Num(); i++)
		{
			ArtificialData[GoodsIndex].QuotaData[i].Dosage += 1;
		}

		Goods.Param.D += BodaSurfaceArea->GetBrickNums();
		Goods.Param.L += FArmyMath::CircumferenceOfPointList(OutArea->Vertices, true) / 100;
		Goods.Param.C += FArmyMath::CircumferenceOfPointList(OutArea->Vertices, true) / 100;
		//Goods.Param.H += M_ExtrusionHeight;
		Goods.Param.S+= BodaSurfaceArea->GetAreaDimension();

	}

	if (m_deltaHeight != 0.0f) //添加吊顶下吊侧面区域施工项算量
	{
		int number = InnearHoleSurface.Num();
		for (int i = 0; i < number; i++)
		{
			InnearHoleSurface[i]->GetConstructionAreas(ArtificialData);
		}
	}

}


void FArmyRectArea::DestroyAllLights()
{
	//for (APointLight* light : LampLights)
	//{
	//	light->Destroy();
	//}
	//LampLights.Empty();
	//RoomDataStatus &= ~FLAG_HASLIGHT;
}

void FArmyRectArea::GenerateAllLights()
{
	//float offset = m_deltaHeight > 0 ? m_deltaHeight : 0.0f;
	//TArray<FVector> OutList = OutArea->Vertices;
	//OutList = FArmyMath::ExturdePolygon(OutList, LampSlotLength + 0.2f, false);
	//if (!FArmyMath::IsClockWise(OutList))
	//	FArmyMath::ReversePointList(OutList);
	//for (auto& iter : OutList)
	//{
	//	iter = iter.X * PlaneXDir + iter.Y * PlaneYDir + PlaneOrignPos + PlaneNormal * (M_ExtrusionHeight + offset - 0.01f);
	//}
	//FArmyMath::ReversePointList(OutList);
	//const float xOffset = 5.0f;
	//const float yOffset = 10.0f;
	//TArray<FVector>	lightTrackPoints = FArmyMath::Extrude3D(OutList, xOffset, false);
	//GenerateLampSlotLightFromTrackingPoints(lightTrackPoints);
	//RoomDataStatus |= FLAG_HASLIGHT;
}

void FArmyRectArea::RemoveLampSlot(AArmyExtrusionActor* InLampSlotActor)
{
    //FArmyBaseArea::RemoveLampSlot();
    //for (auto iter : LampLights)
    //{
    //    iter->Destroy();
    //}
    //LampLights.Empty();
    //CalculateVerticalHole(m_deltaHeight, 0.0);

    GetLampSlotExtruder()->DeleteActor(InLampSlotActor);
}

bool FArmyRectArea::HasLight()
{
	return (RoomDataStatus & FLAG_HASLIGHT) > 0;

}

void FArmyRectArea::Destroy()
{
	FArmyBaseArea::Destroy();
	DestroyAllLights();
	for (TSharedPtr<FArmyHelpRectArea> rectArea : InnearHoleSurface)
	{
		rectArea->Destroy();
	}
	InnearHoleSurface.Empty();
	if (BodaSurfaceArea.IsValid())
	{
		BodaSurfaceArea->Destroy();
		BodaSurfaceArea = NULL;
	}
}

void FArmyRectArea::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	if (IsAbs)
	{
		FArmyBaseArea::GetLines(OutLines, IsAbs);
		if (BodaThickness > 0)
		{
			int number = InneraArea->Vertices.Num();
			for (int i = 0; i < number; ++i)
			{
				FVector start = InneraArea->Vertices[i%number].X* PlaneXDir + InneraArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
				FVector end = InneraArea->Vertices[(i + 1) % number].X* PlaneXDir + InneraArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos;

				OutLines.AddUnique(MakeShareable(new FArmyLine(start, end)));
			}
		}

	}
	else
	{
		return OutArea->GetLines(OutLines);
	}
}

void FArmyRectArea::GetConstructionOutLines(TArray< TSharedPtr<FArmyLine> >& OutLines)
{
	FArmyBaseArea::GetLines(OutLines, true);
}

void FArmyRectArea::GetConstructionInnerLines(TArray< TSharedPtr<FArmyLine> >& OutLines)
{
	if (BodaThickness > 0)
	{
		int number = InneraArea->Vertices.Num();
		for (int i = 0; i < number; ++i)
		{
			FVector start = InneraArea->Vertices[i%number].X* PlaneXDir + InneraArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
			FVector end = InneraArea->Vertices[(i + 1) % number].X* PlaneXDir + InneraArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos;

			OutLines.AddUnique(MakeShareable(new FArmyLine(start, end)));
		}
	}
}

 void FArmyRectArea::StartTransformByEdge() 
 {
	 CurrentTransformFlag = TransformFlag;
 }

//TSharedPtr<FArmyBaseArea> FArmyRectArea::FindArea(const FVector & InPoint)
//{
//	return TSharedPtr<FArmyBaseArea>();
//}

void FArmyRectArea::MoveByEdge(FVector offset)
{
	FTransform trans(offset);
	ApplyTransform(trans);
}

void FArmyRectArea::Move(const FVector& Offset)
{
	this->SetLeftUpCornerPos(this->GetLeftUpCornerPos()+ Offset);
    OffsetExtrusionActors(Offset);

	//移动子区域
	for (auto & SubArea : RoomEditAreas)
	{
		SubArea->Move(Offset);
	}
}

void FArmyRectArea::UpdateWallArea(TArray<FVector> NewVertexes)
{
    TArray<FVector> OutLine;
    TArray<TArray<FVector>> InHoles;
    CalculateOutAndInnerHoles(OutLine, InHoles);
    MatStyle->SetDrawArea(NewVertexes, InHoles);
    MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
    CalculateWallActor(MatStyle);
    CalculateVerticalHole(m_deltaHeight, 7.5f, NewVertexes);
}

bool FArmyRectArea::HasBodaThickness() const
{
	return BodaThickness > 0;
}

void FArmyRectArea::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyBaseArea::SetPropertyFlag(InType, InUse);

	//for (APointLight* iter : LampLights)
	//{
	//	iter->SetActorHiddenInGame(!InUse);
	//}
	for (TSharedPtr<FArmyHelpRectArea> iter : InnearHoleSurface)
	{
		iter->SetActorVisible(InUse);
	}
}

void FArmyRectArea::SetActorVisible(bool InVisible)
{
	FArmyBaseArea::SetActorVisible(InVisible);
	//for (APointLight* iter : LampLights)
	//{
	//	iter->SetActorHiddenInGame(!InVisible);
	//}
	for (TSharedPtr<FArmyHelpRectArea> iter : InnearHoleSurface)
	{
		iter->SetActorVisible(InVisible);
	}
}

void FArmyRectArea::RecalculateArea(float InLampSlotWidth)
{
    TArray<FVector> tempNewOut = OutArea->Vertices;
    tempNewOut = FArmyMath::ExturdePolygon(tempNewOut, InLampSlotWidth + 0.2f, false);
    TArray<FVector> OutLine;
    TArray<TArray<FVector>> InHoles;
    CalculateOutAndInnerHoles(OutLine, InHoles);
    MatStyle->SetDrawArea(tempNewOut, InHoles);
    MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);

    LampSlotWidth = InLampSlotWidth;
    CalculateWallActor(MatStyle);
    CalculateVerticalHole(m_deltaHeight, InLampSlotWidth, tempNewOut);

    RoomDataStatus |= FLAG_HASLIGHT;
}

TSharedPtr<FArmyHelpRectArea> FArmyRectArea::CalculateInnerRectArea(FVector InStart, FVector InEnd, float InDepth)
{
	FVector xdir, yDir, tempCenter;
	TArray<FVector> rectVertics;
	CalculatePlaneInfo(InStart, InEnd, InDepth, xdir, yDir, tempCenter, rectVertics);
	TSharedPtr<FArmyHelpRectArea> tempRect = MakeShareable(new FArmyHelpRectArea());
    tempRect->SurportPlaceArea = SurportPlaceArea;
    tempRect->SurfaceType = SurfaceType;
	tempRect->SetPlaneInfo(xdir, yDir, tempCenter);
	tempRect->SetVertices(rectVertics);
	TSharedPtr<FArmySeamlessStyle> tempStyle = MakeShareable(new FArmySeamlessStyle());
	/* @梁晓菲 抬高或者下吊后的默认材质*/
	if (SurfaceType == 0)
	{
		tempStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureFloor(), 1024, 1024);
		FString Path = TEXT("/Game/XRCommon/HardMode/FloorTexture.FloorTexture");
		tempStyle->SetMainTextureLocalPath(Path);
	}
	else if (SurfaceType == 1 || SurfaceType == 2)
	{
		tempStyle->SetMainTextureInfo(FArmyEngineModule::Get().GetEngineResource()->GetDefaultTextureCeiling(), 520, 520);
		FString Path = TEXT("/Game/XRCommon/HardMode/CeilingTexture.CeilingTexture");
		tempStyle->SetMainTextureLocalPath(Path);
	}

	tempStyle->SetPlaneInfo(tempRect->GetPlaneCenter(), tempRect->GetXDir(), tempRect->GetYDir());
	tempRect->SetStyle(tempStyle);

	return tempRect;

}

void FArmyRectArea::CalculatePlaneInfo(const FVector& InStart, const FVector& InEnd, const float InDepth, FVector& OutPlaneXDir, FVector& OutPlaneYDir, FVector& OutPlaneCenter, TArray<FVector>& OutVertices)
{
    // 地面或者顶面，矩形区域凸起或者凹陷的时候，侧面的法相方向应该是指向不同
    // 凸起时候法相指向外面，凹陷时候应该指向里面
	FVector XDir = FVector::ForwardVector;
	if (InDepth < 0.0f)
	{
		XDir = (InEnd - InStart).GetSafeNormal();

	}
	else if (InDepth > 0.0)
	{
		XDir = (InStart - InEnd).GetSafeNormal();
	}
	FVector YDir = PlaneNormal;

	TArray<FVector> rectVertics = { InStart,InStart + PlaneNormal * InDepth, InEnd + PlaneNormal * InDepth, InEnd };

	FBox tempBox(rectVertics);
	FVector tempCenter = tempBox.GetCenter();
	for (FVector& iter : rectVertics)
	{
        float X = FVector::DotProduct((iter - tempCenter), XDir);
        float Y = FVector::DotProduct((iter - tempCenter), YDir);
		iter = FVector(X, Y, 0.f);
	}
	OutPlaneXDir = XDir;
	OutPlaneYDir = YDir;
	OutPlaneCenter = tempCenter;
	OutVertices = rectVertics;
}

void FArmyRectArea::ExtrusionBodaArea()
{
	if (BodaSurfaceArea.IsValid())
	{
		FVector BodaAreaPlaneOrignPos = FVector(PlaneOrignPos.X, PlaneOrignPos.Y, PlaneOrignPos.Z + M_ExtrusionHeight);
		BodaSurfaceArea->SetPlaneOrignPos(BodaAreaPlaneOrignPos);
		BodaSurfaceArea->CaculateBodaArea(OutArea->Vertices, BodaThickness, BodaBrickLength, BodaSurfaceArea->GetBodaThick());
	}
}

void FArmyRectArea::RefreshPolyVertices()
{
	TArray<FVector> OutVertices;
	FVector second = LeftUpCornerPoint + FVector(1, 0, 0)*AreaWidth;
	FVector third = second + FVector(0, 1, 0)*AreaHeight;
	FVector fourth = third + FVector(-1, 0, 0)*AreaWidth;
	OutVertices = { LeftUpCornerPoint,second,third,fourth };
	FVector InnRectFirst = LeftUpCornerPoint + FVector(1, 0, 0)*BodaThickness + FVector(0, 1, 0)*BodaThickness;
	FVector InnRectSecond = InnRectFirst + FVector(1, 0, 0)*(AreaWidth - 2 * BodaThickness);
	FVector InnRectThird = InnRectSecond + FVector(0, 1, 0)*(AreaHeight - 2 * BodaThickness);
	FVector InnRectFourth = InnRectThird + FVector(-1, 0, 0)*(AreaWidth - 2 * BodaThickness);
	TArray<FVector> innerVertices = { InnRectFirst,InnRectSecond,InnRectThird,InnRectFourth };
	OutArea->SetVertices(OutVertices);
	InneraArea->SetVertices(innerVertices);

	if (!BeDrawOutLine)
	{
		TArray<TArray<FVector>> InnerHoles;
		CalculateBackGroundSurface(OutArea->Vertices, InnerHoles);
	}

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
		MatStyle->SetDrawArea(innerVertices, Holes);
		//if (M_ExtrusionHeight != 0.0f)
			//MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
        SetExtrusionHeight(M_ExtrusionHeight);
		CalculateWallActor(MatStyle);
	}
	//CalculateBackGroundSurface(OutArea->Vertices, Holes);
    if (SurfaceType == 2)
    {
        RecalculateArea(LampSlotWidth);
    }
	//@郭子阳 刷新子面
	for (auto & SubArea : RoomEditAreas)
	{
		//SubArea->setpla
		SubArea->RefreshPolyVertices();
	}

}
