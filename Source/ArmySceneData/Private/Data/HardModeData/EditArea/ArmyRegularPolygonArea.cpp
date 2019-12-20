#include "Data/HardModeData/EditArea/ArmyRegularPolygonArea.h"
#include "Primitive2D/ArmyPolygon.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyEngineModule.h"
#include "ArmyPlayerController.h"
#include "SceneManagement.h"
#include "ArmyBodaArea.h"
FArmyRegularPolygonArea::FArmyRegularPolygonArea() : FArmyBaseArea()
{
	Position = FVector::ZeroVector;
	Radius = 0.0f;
	NumSide = 12;
	BodaThickness = 0.0f;
	M_RegularPolyType = OutCircle;
	ObjectType = OT_PolygonArea;
	bBoundingBox = false;
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat();
	MI_BodaTextureMat = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_BodaTextureMat->AddToRoot();
	OutStartPoint = FVector::ZeroVector;
	ExturesionActor = NULL;

}

void FArmyRegularPolygonArea::SetRegularPolyType(RegularPolyType InType)
{
	if (M_RegularPolyType != InType)
	{
		M_RegularPolyType = InType;
		RefreshPolyVertices();
	}
}

void FArmyRegularPolygonArea::SetPosition(FVector InPosition)
{
	if (Position != InPosition)
	{
		Position = InPosition;
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

void FArmyRegularPolygonArea::Move(const FVector& Offset)
{
	OutStartPoint += Offset;
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

void FArmyRegularPolygonArea::SetRadius(float InRadius)
{
	if (Radius != InRadius)
	{
		Radius = InRadius;
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

void FArmyRegularPolygonArea::SetBodaThickness(float InThickness)
{
	if (BodaThickness != InThickness)
	{
		BodaThickness = InThickness;
		RefreshPolyVertices();
		TArray<TArray<FVector>> holes;
		MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	}
}

void FArmyRegularPolygonArea::SetNumSide(int32 InNumSide,bool FreshVertice)
{
	if (NumSide != InNumSide)
	{
		NumSide = InNumSide;
		if (FreshVertice)
		{
			RefreshPolyVertices();
		}
	}

}

void FArmyRegularPolygonArea::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
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

void FArmyRegularPolygonArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("position", Position.ToString());
	JsonWriter->WriteValue("startPoint", OutStartPoint.ToString());
	JsonWriter->WriteValue("radius", Radius);
	JsonWriter->WriteValue("numSide", NumSide);
	JsonWriter->WriteValue("bodaThickness", BodaThickness);
	JsonWriter->WriteValue("regularPolyType", (int32)GetRegularPolyType());
	JsonWriter->WriteValue("bodaTextureURL", BodaTextureUrl);
	JsonWriter->WriteValue("m_deltaHeight", m_deltaHeight);
	JsonWriter->WriteValue("extrudedxfFilePath", ExtrudedxfFilePath);
	JsonWriter->WriteValue("ExturdedxfMaterialFilePath", ExturdedxfMaterialFilePath);
	JsonWriter->WriteValue("MainBrickID", MainBrickID);
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyRegularPolygonArea)
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
}

void FArmyRegularPolygonArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	Position.InitFromString(InJsonData->GetStringField("position"));
	OutStartPoint.InitFromString(InJsonData->GetStringField("startPoint"));
	Radius = (InJsonData->GetNumberField("radius"));
	NumSide = (InJsonData->GetNumberField("numSide"));
	BodaThickness = (InJsonData->GetNumberField("bodaThickness"));
	int32 TypeValue = (int32)InJsonData->GetNumberField("regularPolyType");
	if (BodaThickness > 0.0)
	{
		BodaTextureUrl = InJsonData->GetStringField("bodaTextureURL");
		MainBrickID = InJsonData->GetNumberField("MainBrickID");

		UXRResourceManager * res = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface * mat = res->CreateCustomMaterial(BodaTextureUrl);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
		SetBodaMaterial(MID, MainBrickID);
	}
	FArmyBaseArea::Deserialization(InJsonData);
	M_RegularPolyType = RegularPolyType(TypeValue);
	RefreshPolyVertices();
	TArray<TArray<FVector>> InHoles;
	MatStyle->SetDrawArea(InneraArea->Vertices, InHoles);
	m_deltaHeight = InJsonData->GetNumberField("m_deltaHeight");
	double tempDelta;
	if (InJsonData->TryGetNumberField("m_deltaHeight", tempDelta))
	{
		if (FMath::Abs(tempDelta) <= 100)
		{
			m_deltaHeight = tempDelta;
			if (tempDelta != 0.0)
			{
				TArray<TSharedPtr<FJsonValue> > surfaceHolesJson = InJsonData->GetArrayField("InnearHoleSurface");
				if (InnearHoleSurface.Num() == 0)
				{
					for (auto& L : surfaceHolesJson)
					{
						const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
						TSharedPtr<FArmyHelpRectArea> tempHelpRect = MakeShareable(new FArmyHelpRectArea());
						tempHelpRect->Deserialization(LJsonObj);
						tempHelpRect->SurfaceType = SurfaceType;
						tempHelpRect->SurportPlaceArea = SurportPlaceArea;
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
	}
	ExtrudedxfFilePath = InJsonData->GetStringField("extrudedxfFilePath");
	ExturdedxfMaterialFilePath = InJsonData->GetStringField("ExturdedxfMaterialFilePath");
	if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
	{
		GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
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



TSharedPtr<FArmyBaseArea> FArmyRegularPolygonArea::ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
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

void FArmyRegularPolygonArea::GenerateLampSlot(
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

void FArmyRegularPolygonArea::ApplyTransform(const FTransform& Translation)
{
	FVector newPosition = Translation.TransformPosition(Position);
	FVector	tempOutStartPoint = Translation.TransformPosition(OutStartPoint);

	SCOPE_TRANSACTION(TEXT("多边形六轴标尺移动"));
	OutStartPoint = tempOutStartPoint;
	SetPosition(newPosition);
	TArray<TArray<FVector>> holes;
	MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	CalculateWallActor(MatStyle);
	RefreshExtrusionPlane();
	PositionChanged.Broadcast();
	this->Modify();
	GXREditor->SelectNone(true, true, false);
}



void FArmyRegularPolygonArea::SetExtrusionHeight(float InHeight)
{
	if (M_ExtrusionHeight != InHeight)
	{
		M_ExtrusionHeight = InHeight;
		RefreshExtrusionPlane();
		MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		CalculateWallActor(MatStyle);
	}
	FArmyBaseArea::SetExtrusionHeight(InHeight);

}

void FArmyRegularPolygonArea::RefreshExtrusionPlane()
{
	if (GetParents().Num() != 0)
	{
		TWeakPtr<FArmyObject> parent = GetParents()[0];
		if (!parent.IsValid())
			return;
		FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
		m_deltaHeight = temp->GetExtrusionHeight() - M_ExtrusionHeight;
		CalculateVerticalHole(m_deltaHeight, LampSlotWidth);

		/* @梁晓菲 面的高度差为0，删除灯槽*/
		if (m_deltaHeight == 0 && !GetLampSlotExtruder()->IsEmpty())
		{
			DestroyLampLight();
			return;
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
}

const FBox FArmyRegularPolygonArea::GetBounds()
{
	return FBox(OutArea->Vertices);
}

void FArmyRegularPolygonArea::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	if (IsAbs)
	{
		FArmyBaseArea::GetLines(OutLines, IsAbs);
	}
	else
	{
		int number = OutArea->Vertices.Num();
		for (int i = 0; i < number; i++)
		{
			TSharedPtr<FArmyLine> line = MakeShareable(new FArmyLine());
			line->SetStart(OutArea->Vertices[i%number]);
			line->SetEnd(OutArea->Vertices[(i + 1) % number]);
			OutLines.Push(line);
		}
	}

}

TArray<FVector> FArmyRegularPolygonArea::GetOutVertices() const
{
	return OutArea->Vertices;
}

void FArmyRegularPolygonArea::SetBodaTexture(UTexture2D* InTexture, int32 brickID)
{
	MI_BodaTextureMat->SetTextureParameterValue("MainTex", InTexture);
	MainBrickID = brickID;
}

void FArmyRegularPolygonArea::SetBodaMaterial(UMaterialInstanceDynamic* InTexture, int32 brickID)
{
	MI_BodaTextureMat = InTexture;
	MainBrickID = brickID;
}

void FArmyRegularPolygonArea::SetStartPoint(FVector InPosition)
{
	OutStartPoint = InPosition;
	RefreshPolyVertices();
}

void FArmyRegularPolygonArea::SetStartPointAndCenterPoint(const FVector& InStartPos, const FVector& InCenterPos)
{
	OutStartPoint = InStartPos;
	Position = InCenterPos;
	RefreshPolyVertices();
	if (SurfaceType == 1)
	{
		if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
		{
			FArmyBaseArea::GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
		}
	}
}

void FArmyRegularPolygonArea::GetConstructionAreas(TArray<FArmyGoods>& ArtificialData)
{
	FArmyBaseArea::GetConstructionAreas(ArtificialData);

	//不支持在正多边形绘制波打线
	return;


}


void FArmyRegularPolygonArea::Destroy()
{
	for (TSharedPtr<FArmyHelpRectArea> rectArea : InnearHoleSurface)
	{
        if (rectArea.IsValid())
        {
		    rectArea->Destroy();
            rectArea = nullptr;
        }
	}
	FArmyBaseArea::Destroy();
	InnearHoleSurface.Empty();
}

void FArmyRegularPolygonArea::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyBaseArea::SetPropertyFlag(InType, InUse);
	for (TSharedPtr<FArmyHelpRectArea> iter : InnearHoleSurface)
	{
		iter->SetActorVisible(InUse);
	}
}


void FArmyRegularPolygonArea::RecalculateArea(float InLampSlotWidth)
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
    CalculateVerticalHole(m_deltaHeight, LampSlotWidth);

    RoomDataStatus |= FLAG_HASLIGHT;
}

void FArmyRegularPolygonArea::CalculateVerticalHole(float deltaHeight, float InLampSlotWidth)
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

	TArray<FVector> outList = OutArea->Vertices;
	if (InLampSlotWidth != 0.0f)
	{
		outList = FArmyMath::ExturdePolygon(outList, InLampSlotWidth + 0.2f, false);
	}
	int number = outList.Num();
	TArray<FVector> tempOutVertices;
	tempOutVertices.AddUninitialized(number);
	for (int i = 0; i < number; i++)
	{
		tempOutVertices[i] = outList[i].X *PlaneXDir + outList[i].Y *PlaneYDir + PlaneOrignPos;
	}

	if (!FArmyMath::IsClockWise(tempOutVertices))
		FArmyMath::ReversePointList(tempOutVertices);
	if (InnearHoleSurface.Num() == 0)
	{
		for (int i = 0; i < number; ++i)
		{
			TSharedPtr<FArmyHelpRectArea> innearRect = CalculateInnerRectArea(tempOutVertices[i%number] + PlaneNormal* M_ExtrusionHeight, tempOutVertices[(i + 1) % number] + PlaneNormal*M_ExtrusionHeight, deltaHeight);
			InnearHoleSurface.Push(innearRect);
		}
	}
	else
	{
		for (int i = 0; i < number; ++i)
		{
			FVector xdir, yDir, tempCenter;
			TArray<FVector> rectVertics;
			CalculatePlaneInfo(tempOutVertices[i%number] + PlaneNormal* M_ExtrusionHeight, tempOutVertices[(i + 1) % number] + PlaneNormal*M_ExtrusionHeight, deltaHeight, xdir, yDir, tempCenter, rectVertics);
			InnearHoleSurface[i]->GetStyle()->SetPlaneInfo(tempCenter, xdir, yDir);
			InnearHoleSurface[i]->SetPlaneInfo(xdir, yDir, tempCenter);
			InnearHoleSurface[i]->SetVertices(rectVertics);
		}
	}
}

TSharedPtr<FArmyHelpRectArea> FArmyRegularPolygonArea::CalculateInnerRectArea(FVector InStart, FVector InEnd, float InDepth)
{

	FVector xdir, yDir, tempCenter;
	TArray<FVector> rectVertics;
	CalculatePlaneInfo(InStart, InEnd, InDepth, xdir, yDir, tempCenter, rectVertics);
	TSharedPtr<FArmyHelpRectArea> tempRect = MakeShareable(new FArmyHelpRectArea());
	tempRect->SurfaceType = SurfaceType;
	tempRect->SurportPlaceArea = SurportPlaceArea;
	tempRect->SetPlaneInfo(xdir, yDir, tempCenter);
	tempRect->SetVertices(rectVertics);
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
	tempStyle->SetPlaneInfo(tempRect->GetPlaneCenter(), tempRect->GetXDir(), tempRect->GetYDir());
	tempRect->SetStyle(tempStyle);

	return tempRect;
}

void FArmyRegularPolygonArea::CalculatePlaneInfo(const FVector& InStart, const FVector& InEnd, const float InDepth, FVector& OutPlaneXDir, FVector& OutPlaneYDir, FVector& OutPlaneCenter, TArray<FVector>& OutVertices)
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

	TArray<FVector> rectVertics = { InStart, InStart + PlaneNormal * InDepth, InEnd + PlaneNormal * InDepth, InEnd };

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

void FArmyRegularPolygonArea::RefreshPolyVertices()
{
	TArray<FVector> TotalInnearVertices, TotalOutVertices;
	if (M_RegularPolyType == InneraCircle)
	{
		FVector direction = (OutStartPoint - Position).GetSafeNormal();
		float dist = BodaThickness / FMath::Cos(FMath::DegreesToRadians(180.0 / NumSide));
		FVector InnerStartPoint = OutStartPoint - dist * direction;
		float startAngle = 0.0f;
		for (int i = 0; i < NumSide; i++)
		{
			FVector tempPoint = FRotator(0, startAngle, 0).RotateVector(InnerStartPoint - Position) + Position;
			FVector tempPoint1 = FRotator(0, startAngle, 0).RotateVector(OutStartPoint - Position) + Position;

			startAngle += 360 / NumSide;

			TotalInnearVertices.Push(tempPoint + FVector(0, 0, POLYGONOFFSETDIS));
			TotalOutVertices.Push(tempPoint1 + FVector(0, 0, POLYGONOFFSETDIS));
		}
	}
	else if (M_RegularPolyType == OutCircle)
	{
		FVector direction = (OutStartPoint - Position).GetSafeNormal();
		FVector direction0 = direction.RotateAngleAxis(90, FVector(0, 0, 1));
		float radius = (OutStartPoint - Position).Size();

		FVector OutStartPos = OutStartPoint + direction0 * (radius * FMath::Tan(FMath::DegreesToRadians(180 / NumSide)));
		FVector InnearStartPos = (OutStartPoint - direction * BodaThickness) + direction0 * ((radius - BodaThickness)*FMath::Tan(FMath::DegreesToRadians(180 / NumSide)));
		float startAngle = 0.0f;

		for (int i = 0; i < NumSide; i++)
		{
			FVector tempPoint = FRotator(0, startAngle, 0).RotateVector(InnearStartPos - Position) + Position;
			FVector tempPoint1 = FRotator(0, startAngle, 0).RotateVector(OutStartPos - Position) + Position;

			startAngle += 360 / NumSide;

			TotalInnearVertices.Push(tempPoint + FVector(0, 0, POLYGONOFFSETDIS));
			TotalOutVertices.Push(tempPoint1 + FVector(0, 0, POLYGONOFFSETDIS));
		}

	}


	if (BodaThickness > 0)
	{
		BodaAreaVertices.Empty();
		BodaAreaPositionVertices.Empty();
		FVector Normal = FVector(0, 0, 1);
		FVector Tangent = FVector(1, 0, 0);
		for (int i = 0; i < NumSide; i++)
		{
			FVector InnearStartPoint = TotalInnearVertices[i%NumSide];
			FVector InnearEndPoint = TotalInnearVertices[(i + 1) % NumSide];

			FVector OutStartPoint = TotalOutVertices[i%NumSide];
			FVector OutEndPoint = TotalOutVertices[(i + 1) % NumSide];

			TArray<FVector> UnitPolygons = { InnearStartPoint,OutStartPoint,OutEndPoint,InnearEndPoint };

			FVector projectionDirection = (OutEndPoint - OutStartPoint).GetSafeNormal();
			FVector direction0 = (UnitPolygons[0] - UnitPolygons[1]);
			FVector dirction1 = (UnitPolygons[3] - UnitPolygons[1]);
			float width = (UnitPolygons[2] - UnitPolygons[0]).Size();
			float U0 = FVector::DotProduct(direction0, projectionDirection) / width;
			float U3 = FVector::DotProduct(dirction1, projectionDirection) / width;
			float UCoord = U0;
			float VCoord = 0;
			BodaAreaVertices.Push(FDynamicMeshVertex(UnitPolygons[0], Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
			UCoord = 0;
			VCoord = 1;
			BodaAreaVertices.Push(FDynamicMeshVertex(UnitPolygons[1], Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
			UCoord = 1;
			VCoord = 1;
			BodaAreaVertices.Push(FDynamicMeshVertex(UnitPolygons[2], Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
			UCoord = U0;
			VCoord = 0;
			BodaAreaVertices.Push(FDynamicMeshVertex(UnitPolygons[0], Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
			UCoord = 1;
			VCoord = 1;
			BodaAreaVertices.Push(FDynamicMeshVertex(UnitPolygons[2], Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
			UCoord = U3;
			VCoord = 0;
			BodaAreaVertices.Push(FDynamicMeshVertex(UnitPolygons[3], Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));

		}

		for (FDynamicMeshVertex& iter : BodaAreaVertices)
		{
			BodaAreaPositionVertices.Push(iter.Position);
			iter.Position = iter.Position.X *PlaneXDir + iter.Position.Y * PlaneYDir + PlaneNormal* 0.2f + PlaneOrignPos;
		}


		//BodaSurfaceArea->AttachParentArea = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());

	}
	InneraArea->SetVertices(TotalInnearVertices);
	InneraArea->SetPolygonOffset(POLYGONOFFSETDIS);
	OutArea->SetVertices(TotalOutVertices);

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
			MatStyle->SetDrawArea(TotalInnearVertices, Holes);
			if (M_ExtrusionHeight != 0.0f)
				MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
			CalculateWallActor(MatStyle);
		}
		//CalculateBackGroundSurface(OutArea->Vertices, Holes);

		//@郭子阳 刷新子面
		for (auto & SubArea : RoomEditAreas)
		{
			//SubArea->setpla
			SubArea->RefreshPolyVertices();
		}
	}



	if (!BeDrawOutLine && Radius != 0.0f)
	{
		TArray<TArray<FVector>> InnerHoles;
		CalculateBackGroundSurface(OutArea->Vertices, InnerHoles);
	}

	if (SurfaceType == 2 && (!IsTempArea))
	{
		RecalculateArea(LampSlotWidth);
	}
}
