#include "ArmyRoomSpaceArea.h"
#include "ArmyStyle.h"
#include "ArmyEngineModule.h"
#include "ArmyMath.h"
#include "ArmyTools/Public/XRClipper.h"
#include<vector.h>
#include "ArmyViewportClient.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyExtrusionActor.h"
#include "ArmyRoom.h"
#include "ArmyActor/XRWallActor.h"
#include "ArmyWallLine.h"
#include "ArmyBaseboardExtruder.h"
#include "ArmyCrownMouldingExtruder.h"
#include "ArmyExtruder.h"

#define SCALE0 100.0f

FArmyRoomSpaceArea::FArmyRoomSpaceArea() :FArmyBaseArea()
{
	bBoundingBox = false;
	ObjectType = OT_RoomSpaceArea;
	ExturesionActor = NULL;
	WallActor = NULL;
}

void FArmyRoomSpaceArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("AttachRoomID", AttachRoomID);
	JsonWriter->WriteValue("AttachBspID", AttachBspID);
	JsonWriter->WriteValue("RoomStatus", (int32)RoomDataStatus);
	JsonWriter->WriteValue("GenerateFromObjectType", (int32)GenerateFromObjectType);

	JsonWriter->WriteArrayStart(TEXT("vertexes"));

	for (FVector& Vertex : InneraArea->Vertices)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	if (SurfaceType == 1)
	{
		JsonWriter->WriteArrayStart(TEXT("DropVerts"));
		for (FVector& iter : dropVerts)
		{
			JsonWriter->WriteValue(iter.ToString());
		}
		JsonWriter->WriteArrayEnd();
	}
	/// 原始墙体顶点数据
	JsonWriter->WriteArrayStart(TEXT("OrginalSurfaceVerts"));
	for (FVector& vert : OrginalSurfaceVerts)
	{
		JsonWriter->WriteValue(vert.ToString());
	}
	JsonWriter->WriteArrayEnd();
	///
	JsonWriter->WriteArrayStart("AttachWindowId");
	for (FString nameIter : CurrentAttachWindowName)
	{
		JsonWriter->WriteValue(nameIter);
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("surfaceHoles");
	for (auto& It : SurfaceHoles)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteArrayStart("vertices");
		for (auto& item : It)
		{
			JsonWriter->WriteValue(item.ToString());
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	///写原始墙洞
	JsonWriter->WriteArrayStart(TEXT("OrginalSurfaceHoles"));
	for (auto& It : OrginalSurfaceHoles)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteArrayStart(TEXT("OrginalHoles"));
		for (auto& item : It)
		{
			JsonWriter->WriteValue(item.ToString());
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	///
	/// 写入原始墙体世界坐标中的中心点
	JsonWriter->WriteValue("OrignalSurfaceCenter", OrignalSurfaceCenter.ToString());

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyRoomSpaceArea)
		FArmyBaseArea::SerializeToJson(JsonWriter);
}

void FArmyRoomSpaceArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	RoomDataStatus = InJsonData->GetNumberField("RoomStatus");
	InJsonData->TryGetStringField("AttachRoomID", AttachRoomID);
	InJsonData->TryGetStringField("AttachBspID", AttachBspID);
	int32 TypeValue = (int32)InJsonData->GetNumberField("GenerateFromObjectType");
	GenerateFromObjectType = EObjectType(TypeValue);

	InJsonData->TryGetStringArrayField("AttachWindowId", CurrentAttachWindowName);
	TArray<FVector> Vertexes;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("vertexes", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		Vertexes.Add(Vertex);
	}
	SurfaceHoles.Empty();
	TArray<TSharedPtr<FJsonValue> > surfaceHolesJson = InJsonData->GetArrayField("surfaceHoles");
	for (auto & L : surfaceHolesJson)
	{
		TArray<FVector> tempV;
		const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
		const TArray< TSharedPtr<FJsonValue> > VJsonArray = LJsonObj->GetArrayField("vertices");
		for (auto & V : VJsonArray)
		{
			FVector PV;
			PV.InitFromString(V->AsString());
			tempV.Add(PV);
		}
		SurfaceHoles.Add(tempV);
	}

    SurfaceType = InJsonData->GetIntegerField("surfaceType");

	//@马云龙 XRLightmass 反序列化踢脚线GUID
	SkirtLineGUIDs.Empty();
	TArray<TSharedPtr<FJsonValue> > SkirtLineGUIDsArray = InJsonData->GetArrayField("SkirtLineGUIDs");
	for (auto & JValue : SkirtLineGUIDsArray)
	{
		FString GUIDStr = JValue->AsString();
		FGuid ParsedGUID;
		FGuid::Parse(GUIDStr, ParsedGUID);
		SkirtLineGUIDs.Add(ParsedGUID);
	}

	FArmyBaseArea::Deserialization(InJsonData);

	if (SurfaceType == 1)
	{
		TArray<FString> dropVertsStrArray;
		InJsonData->TryGetStringArrayField("DropVerts", dropVertsStrArray);
		for (auto& ArrayIt : dropVertsStrArray)
		{
			FVector Vertex;
			Vertex.InitFromString(ArrayIt);
			dropVerts.Add(Vertex);
		}
	}

#pragma region OriginalSurfaceArea
	OrginalSurfaceVerts.Empty();
	TArray<FString> OrginalSurfaceVertsStrArray;
	InJsonData->TryGetStringArrayField("OrginalSurfaceVerts", OrginalSurfaceVertsStrArray);
	for (auto& ArrayIt : OrginalSurfaceVertsStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		OrginalSurfaceVerts.Add(Vertex);
	}


	TArray<TSharedPtr<FJsonValue> > OrginalsurfaceHolesJson = InJsonData->GetArrayField("OrginalSurfaceHoles");
	for (auto & L : OrginalsurfaceHolesJson)
	{
		TArray<FVector> tempV;
		const TSharedPtr<FJsonObject> LJsonObj = L->AsObject();
		const TArray< TSharedPtr<FJsonValue> > VJsonArray = LJsonObj->GetArrayField("OrginalHoles");
		for (auto & V : VJsonArray)
		{
			FVector PV;
			PV.InitFromString(V->AsString());
			tempV.Add(PV);
		}
		OrginalSurfaceHoles.Add(tempV);
	}
	OrignalSurfaceCenter.InitFromString(InJsonData->GetStringField("OrignalSurfaceCenter"));

	GenerateOrignalActor(OrginalSurfaceVerts, OrginalSurfaceHoles, PlaneXDir, PlaneYDir, OrignalSurfaceCenter);
#pragma endregion
	InneraArea->SetVertices(Vertexes);
	OutArea->SetVertices(Vertexes);
	RefreshPolyVertices();
	CalculateBuckle();
}

void FArmyRoomSpaceArea::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
		{
			DrawConstructionMode(PDI, View);
		}
	}
}

bool FArmyRoomSpaceArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	if (FArmyBaseArea::IsSelected(Pos, ViwePortClient, dist))
	{
		for (auto iter : RoomEditAreas)
		{
			if (iter->IsSelected(Pos, ViwePortClient, dist))
				return false;

		}

		return true;
	}
	return false;
}

bool FArmyRoomSpaceArea::IsSelected(FVector InStart, FVector InEnd, FVector& IntersectionPoint, FVector& IntersectionNormal)
{
	if (FArmyBaseArea::IsSelected(InStart, InEnd, IntersectionPoint, IntersectionNormal))
	{
		TArray<TSharedPtr<FArmyBaseArea>> tempTotoalHoles;
		for (auto& iterHole : SurfaceHoles)
		{
			TSharedPtr<FArmyBaseArea> tempHole = MakeShareable(new FArmyBaseArea());
			tempHole->SetPlaneInfo(PlaneXDir, PlaneYDir, PlaneOrignPos);
			tempHole->GetOutArea()->Vertices = iterHole;
			tempTotoalHoles.Push(tempHole);
		}
		for (TSharedPtr<FArmyBaseArea>& iter : RoomEditAreas)
		{
			tempTotoalHoles.Push(iter);
		}
		FVector InnearIntersectionPoint, InnerIntersectionNormal;
		for (TSharedPtr<FArmyBaseArea>& iter : tempTotoalHoles)
		{
			if (iter->IsSelected(InStart, InEnd, InnearIntersectionPoint, InnerIntersectionNormal))
				return false;
		}
		return true;
	}
	return false;
}

bool FArmyRoomSpaceArea::IsPointOneSurface(const FVector& InPos)
{
	FPlane plane(PlaneOrignPos, PlaneNormal);

	float dist = FMath::Abs(plane.PlaneDot(InPos));
	if (dist < 0.1f)
	{
		float x = FVector::DotProduct((InPos - PlaneOrignPos), PlaneXDir);
		float y = FVector::DotProduct((InPos - PlaneOrignPos), PlaneYDir);
		if (FArmyMath::IsPointInOrOnPolygon2D(FVector(x, y, 0.0), OutArea->Vertices))
		{
			return true;
		}
		else
			return false;
	}
	else
	{
		return false;
	}

}

bool FArmyRoomSpaceArea::IsSelectEditPlane(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist, bool IncludeInnearAreas)
{
	{
		if (FArmyBaseArea::IsSelected(Pos, ViwePortClient, dist))
		{
			TArray<TSharedPtr<FArmyBaseArea>> tempTotalHoles;
			for (auto iterHole : SurfaceHoles)
			{
				TSharedPtr<FArmyBaseArea> tempHole = MakeShareable(new FArmyBaseArea());
				tempHole->SetPlaneInfo(PlaneXDir, PlaneYDir, PlaneOrignPos);
				tempHole->SetExtrusionHeight(M_ExtrusionHeight);
				tempHole->GetOutArea()->Vertices = iterHole;
				tempTotalHoles.Push(tempHole);
			}

			for (auto iter : tempTotalHoles)
			{
				if (iter->IsSelected(Pos, ViwePortClient, dist))
					return false;
			}
			if (IncludeInnearAreas)
			{
				for (auto iter : RoomEditAreas)
				{
					TSharedPtr<FArmyBaseArea> tempHole = MakeShareable(new FArmyBaseArea());
					tempHole->SetPlaneInfo(iter->GetXDir(), iter->GetYDir(), iter->GetPlaneCenter());
					tempHole->SetExtrusionHeight(M_ExtrusionHeight);
					tempHole->GetOutArea()->Vertices = iter->GetOutArea()->Vertices;
					if (tempHole->IsSelected(Pos, ViwePortClient, dist))
					{
						return false;
					}
				}
			}
			return true;
		}
		return false;
	}

}

void FArmyRoomSpaceArea::SetVerticesAndHoles(const TArray<FVector>& InVertices, const TArray<TArray<FVector>>& InHoles)
{
	SurfaceHoles.Empty();
	Translate::ClippPolygonHoleArea(InVertices, InHoles, SurfaceHoles);
	InneraArea->SetVertices(InVertices);
	OutArea->SetVertices(InVertices);
	if (!(RoomDataStatus & FLAG_DEFAULTSTYLE) && RoomDataStatus & FLAG_HAVESKITLINE)
	{
		if (ContentItemList.Num() > 0)
		{
			GenerateExtrudeActor(GVC->GetWorld(), ContentItemList[0]);
		}
	}

}

void FArmyRoomSpaceArea::GenerateOrignalActor(const TArray<FVector>& InVertices, const TArray<TArray<FVector>>& InHoles, const FVector& xDir, const FVector& yDir, const FVector& center)
{
	if (OrignalSurfaceActor == NULL)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Name = FName(*(TEXT("NOLIST-ORIGNALWALL") + FGuid::NewGuid().ToString()));

		OrignalSurfaceActor = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity, SpawnInfo);
		OrignalSurfaceActor->Tags.Add(TEXT("Immovable"));
		OrignalSurfaceActor->Tags.Add(TEXT("Wall"));
		OrignalSurfaceActor->Tags.Add(TEXT("OriginalWall"));
		OrignalSurfaceActor->Tags.Add(TEXT("CanNotDelete"));
		OrignalSurfaceActor->GetMeshComponent()->SetCollisionObjectType(ECC_BIMBASEWALL);
		OrignalSurfaceActor->GetMeshComponent()->SetCollisionResponseToChannel(ECC_BIMBASEWALL, ECR_Block);

		//@郭子阳
		//填充AttachSurface
		OrignalSurfaceActor->AttachSurface = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
		OrignalSurfaceActor->bIsSelectable = true;
	}
	TArray<FVector2D> results;
	Translate::ClipperGapVertsAndTriangle(InVertices, InHoles, results);
	FBox box(InVertices);
	FArmyMath::ReverPointList(results);
	int number = results.Num();
	TArray<FDynamicMeshVertex> TotalVertexs;
	for (int index = 0; index < number; index++)
	{
		float VCoord, UCoord;
		UCoord = (results[index].X - box.Min.X) * 10.0f / 1024;
		VCoord = (results[index].Y - box.Min.Y) *10.0f / 1024;
		TotalVertexs.Push(FDynamicMeshVertex(xDir * results[index].X + yDir * results[index].Y + center, PlaneXDir, PlaneNormal, FVector2D(UCoord, VCoord), FColor::White));
	}
	OrignalSurfaceActor->ResetMeshTriangles();
	OrignalSurfaceActor->AddVerts(TotalVertexs);
	OrignalSurfaceActor->UpdateAllVetexBufferIndexBuffer();
	if (SurfaceType == 0)
	{
		UMaterialInstanceDynamic* temp = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial();
		temp->SetScalarParameterValue("PixelDepthOffset", 0.1);
		temp->AddToRoot();
		OrignalSurfaceActor->SetMaterial(temp);
	}
	else
	{
		UMaterialInstanceDynamic* temp = FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial();
		temp->SetScalarParameterValue("PixelDepthOffset", 0.1);
		temp->AddToRoot();
		OrignalSurfaceActor->SetMaterial(temp);
	}
	OrginalSurfaceVerts = InVertices;
	OrginalSurfaceHoles = InHoles;
	OrignalSurfaceCenter = center;

	//@郭子阳  请求施工项
	ConstructionPatameters Parameter;
	EWallType wallType = (EWallType)SurportPlaceArea;
	Parameter.SetOriginalSurface(wallType, GetExtrusionHeight() != 0,GetRoomSpaceID());
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), Parameter, nullptr);

}

void FArmyRoomSpaceArea::SetOriginalActorVisible(bool bVisible)
{
	if (OrignalSurfaceActor)
	{
		OrignalSurfaceActor->SetActorHiddenInGame(!bVisible);
	}
}

void FArmyRoomSpaceArea::SetOrignalFinishWallVisible(bool bVisible)
{
	if (OrignalSurfaceActor)
		OrignalSurfaceActor->SetActorHiddenInGame(!bVisible);
	if (WallActor)
		WallActor->SetActorHiddenInGame(!bVisible);
}

const FBox FArmyRoomSpaceArea::GetBounds()
{
	return FBox(OutArea->Vertices);
}

void FArmyRoomSpaceArea::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	// @ 梁晓菲 放样线条捕捉
	//TMap<int32, TArray< TSharedPtr<FArmyLine> >> ExtrusionLinesMap;
	//switch (this->SurfaceType)
	//{
	//case 0://地
	//	GetExtrusionLines(ExtrusionLinesMap, ExtrusionType::FloorExtrusionLine);
	//	break;
	//case 1://墙
	//	GetExtrusionLines(ExtrusionLinesMap, ExtrusionType::WallExtrusionLine);
	//	break;
	//case 2://顶
	//	GetExtrusionLines(ExtrusionLinesMap, ExtrusionType::RoofExtrusionLine);
	//	break;
	//default:
	//	break;
	//}
	//if (ExtrusionLinesMap.Num() > 0)
	//{
	//	TArray< TSharedPtr<FArmyLine> > ExtrusionLines;
	//	ExtrusionLines.Append(*ExtrusionLinesMap.Find(0));
	//	ExtrusionLines.Append(*ExtrusionLinesMap.Find(1));
	//	OutLines.Append(ExtrusionLines);
	//}

	if (IsAbs)
	{
		int number = InneraArea->Vertices.Num();
		for (int i = 0; i < number; i++)
		{
			FVector startPos = InneraArea->Vertices[i%number].X* PlaneXDir + InneraArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
			FVector endPos = InneraArea->Vertices[(i + 1) % number].X* PlaneXDir + InneraArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos;

			TSharedPtr<FArmyLine> line = MakeShareable(new FArmyLine(startPos, endPos));
			OutLines.Push(line);
		}
		int HoleNumber = SurfaceHoles.Num();
		for (int i = 0; i < HoleNumber; ++i)
		{
			int vertexNumber = SurfaceHoles[i].Num();
			for (int j = 0; j < vertexNumber; ++j)
			{
				FVector start = SurfaceHoles[i][j%vertexNumber];
				FVector end = SurfaceHoles[i][(j + 1) % vertexNumber];

				FVector startPos = start.X* PlaneXDir + start.Y * PlaneYDir + PlaneOrignPos;
				FVector endPos = end.X* PlaneXDir + end.Y * PlaneYDir + PlaneOrignPos;

				TSharedPtr<FArmyLine> line = MakeShareable(new FArmyLine(startPos, endPos));
				OutLines.Push(line);
			}
		}
	}
	else
	{
		int number = InneraArea->Vertices.Num();
		for (int i = 0; i < number; i++)
		{
			const FVector& startPos = InneraArea->Vertices[i%number];
			const FVector& endPos = InneraArea->Vertices[(i + 1) % number];
			TSharedPtr<FArmyLine> line = MakeShareable(new FArmyLine(startPos, endPos));
			OutLines.Push(line);
		}
		int HoleNumber = SurfaceHoles.Num();
		for (int i = 0; i < HoleNumber; ++i)
		{
			int vertexNumber = SurfaceHoles[i].Num();
			for (int j = 0; j < vertexNumber; ++j)
			{
				const FVector& start = SurfaceHoles[i][j%vertexNumber];
				const FVector& end = SurfaceHoles[i][(j + 1) % vertexNumber];
				TSharedPtr<FArmyLine> line = MakeShareable(new FArmyLine(start, end));
				OutLines.Push(line);
			}
		}
	}
}

void FArmyRoomSpaceArea::SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)
{
	MatStyle = InStyle;
	RefreshPolyVertices();
	MatStyle->StyleDirty.BindRaw(this, &FArmyRoomSpaceArea::UpdateWallActor);

	RoomDataStatus &= ~FLAG_DEFAULTSTYLE;
}

void FArmyRoomSpaceArea::DrawConstructionMode(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	//如果不绘制顶面除灯槽之外的其他东西，直接停止绘制
	if (SurfaceType == 2 && !FArmyObject::GetDrawModel(MODE_OTHERCEILINGOBJ)) return;

	// @是顶&&无缝铺 
	if (SurfaceType == 2 && MatStyle->GetEditType() == S_SeamlessStyle)
	{
		//&&进行了下吊，斜线绘制下吊区域
		if (M_ExtrusionHeight != 0.0f)
			DrawRoof(PDI, View);
	}
	else
		MatStyle->DrawWireFrame(PDI, View);

	//在图纸模式下绘制放样线条
	if (ConstructionSkitLineVerts.Num() > 0)
	{
		//放样线条颜色
		FLinearColor LineColor;
		if (SurfaceType == 0)
		{
			LineColor = FLinearColor(FColor(0XFF3D3D3D));
		}
        else if (SurfaceType == 2)
        {
            LineColor = FLinearColor(FColor(0XFF00FFFF));
        }
        else
        {
			LineColor = FLinearColor(FColor(0XFFFF0000));
        }

        //踢脚线两段线在门洞处闭合
        if (ConstructionSkitLineVerts.Num() > 0)
        {
            for (int i = 0; i < ConstructionSkitLineVerts[0].Num(); i++)
            {
                PDI->DrawLine(ConstructionSkitLineVerts[0][i][0], ConstructionSkitLineVerts[ConstructionSkitLineVerts.Num() - 1][i][0], LineColor, 1);
                PDI->DrawLine(ConstructionSkitLineVerts[0][i][ConstructionSkitLineVerts[0][i].Num() - 1], ConstructionSkitLineVerts[ConstructionSkitLineVerts.Num() - 1][i][ConstructionSkitLineVerts[0][i].Num() - 1], LineColor, 1);
            }
        }

		for (TArray<TArray<FVector>>& iter0 : ConstructionSkitLineVerts)
		{
			if ((iter0.Num() == 1) && (RoomDataStatus & FLAG_CLOSESKITLINE))
			{
				TArray<FVector>& skitVerts = iter0[0];
				int number = skitVerts.Num();
				for (int i = 0; i < number; ++i)
					PDI->DrawLine(skitVerts[i%number], skitVerts[(i + 1) % number], LineColor, 1);
			}
			else
			{
				for (TArray<FVector>& iter : iter0)
				{
					int number = iter.Num();
					for (int i = 0; i < number - 1; ++i)
						PDI->DrawLine(iter[i], iter[i + 1], LineColor, 1);
				}
			}
		}
	}
	if (GenerateFromObjectType != OT_Beam)
		DrawOutLine(PDI, View, FLinearColor(FColor(0XFF666666)), 1.0f);
}

TSharedPtr< FArmyRoom> FArmyRoomSpaceArea::GetRoom()
{

	switch (GenerateFromObjectType)
	{
	case OT_InternalRoom:
	case OT_WallLine:
	{	//找到roomID对应的房间即可
		FGuid RoomID;
		FGuid::Parse(this->AttachRoomID, RoomID);


		auto Room = FArmySceneData::Get()->GetObjectByGuid(EModelType::E_LayoutModel, RoomID);
			//GetObjectByGuidAlone(RoomID);
		if (Room.IsValid())
		{
			return StaticCastSharedPtr<FArmyRoom>(Room.Pin());
		}
		else
		{
			return nullptr;
		}
	}

	case OT_Pass:
	case OT_NewPass:
		//门洞没有房间
		return nullptr;

	case OT_Beam:
	{
		TArray<FObjectWeakPtr> RoomObjs;
		FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, RoomObjs);

		TSharedPtr<FArmyRoomSpaceArea> Me = StaticCastSharedRef<FArmyRoomSpaceArea>(this->AsShared());

		for (auto & RoomObj : RoomObjs)
		{

			auto Room = StaticCastSharedPtr<FArmyRoom>(RoomObj.Pin());
			if (Room->IsPointInRoom(GetPlaneCenter()))
			{
				return Room;
			}
		}
		//不应当走到这一步
		return nullptr;
	}
	
	default:

		TArray<FObjectWeakPtr> RoomObjs;
		FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, RoomObjs);

		TSharedPtr<FArmyRoomSpaceArea> Me = StaticCastSharedRef<FArmyRoomSpaceArea>(this->AsShared());

		for (auto & RoomObj : RoomObjs)
		{
			auto Room= StaticCastSharedPtr<FArmyRoom>(RoomObj.Pin());
			
			auto AllComponentSurfaces = FArmySceneData::Get()->GetRoomAttachedComponentSurfaceWithRoomId(Room->GetUniqueID().ToString());

			if (AllComponentSurfaces.Contains(Me))
			{
				return Room;
			}

		}
		break;
	};

	return nullptr;
}

float FArmyRoomSpaceArea::GetAreaDimensionWithVisibility()
{
	float holeArea = 0.0f;
	for (auto iter : SurfaceHoles)
	{
		holeArea += FArmyMath::CalcPolyArea(iter) / 10000.0f;
	}
	if (SurfaceType == 1)
	{
		holeArea += FArmyMath::CalcPolyArea(dropVerts) / 10000.0f;
	}
	for (auto iter0 : RoomEditAreas)
	{
		if (iter0.IsValid() && iter0->WallActor &&!iter0->WallActor->bHidden)
		{
			holeArea += iter0->GetAreaDimension();
		}
	}
	float area = FArmyMath::CalcPolyArea(OutArea->Vertices) / 10000.0f - holeArea;
	return area;
}

void FArmyRoomSpaceArea::DrawBridgeStoneArea(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (FArmyObject::GetDrawModel(MODE_TOPVIEW))
		return;
	PDI->SetHitProxy(new HHitProxy());
	int	number = BackGroundSurface.Num();
	const float SizeX = View->ViewRect.Width();
	const float Zoom = (1.0f / View->ViewMatrices.GetProjectionMatrix().M[0][0])*2.0f / SizeX;
	FDynamicMeshBuilder MeshBuilder;
	for (int i = 0; i < number; i++)
	{
		FDynamicMeshVertex& tempVertex = BackGroundSurface[i];
		const FVector2D& localPos = TranlateToPlane(tempVertex.Position);
		float UCoord = (localPos.X - minPos.X) / (Zoom * 10.0f);
		float VCoord = (localPos.Y - minPos.Y) / (Zoom * 10.0f);
		tempVertex.TextureCoordinate = FVector2D(UCoord, VCoord);
		MeshBuilder.AddVertex(tempVertex);
		if (i % 3 == 0)
		{
			int index = i / 3;
			MeshBuilder.AddTriangle(index * 3, index * 3 + 1, index * 3 + 2);
		}
	}
	MeshBuilder.Draw(PDI, FMatrix::Identity, FArmyEngineModule::Get().GetEngineResource()->GetDefaultBridgeStoneMat()->GetRenderProxy(false), 0, true);
	PDI->SetHitProxy(NULL);
}

void FArmyRoomSpaceArea::ResetSurfaceStyle(bool cleanEditAreas /*= false*/)
{
	MatStyle = MakeShareable(new FArmySeamlessStyle());
	if (SurfaceType == 0)
	{
		UMaterialInstanceDynamic* temp = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial();
		temp->AddToRoot();
		MatStyle->SetMainMaterial(temp, 1024, 1024);
		//@打扮家 XRLightmass
		//FString Path = TEXT("/Game/XRCommon/Material/Default/M_Test1.M_Test1");
		FString Path = TEXT("/Game/XRCommon/Material/Default/M_Concrete_Poured1.M_Concrete_Poured1");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	else
	{
		UMaterialInstanceDynamic* temp = FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial();
		temp->AddToRoot();
		MatStyle->SetMainMaterial(temp, 1024, 1024);
		//@打扮家 XRLightmass
		//FString Path = TEXT("/Game/XRCommon/Material/Default/M_Test1.M_Test1");
		FString Path = TEXT("/Game/XRCommon/Material/Default/M_Concrete_Poured2.M_Concrete_Poured2");
		MatStyle->SetMainTextureLocalPath(Path);
	}
	M_ExtrusionHeight = 0;
	MatStyle->SetPlaneInfo(PlaneOrignPos, PlaneXDir, PlaneYDir);
	SetGapColor(FLinearColor(1.f, 1.f, 1.f, 1.0f));
	if (SurfaceType == 2 && GenerateFromObjectType == OT_InternalRoom)
	{
		TArray < TSharedPtr<FArmyRoomSpaceArea>> results = FArmySceneData::Get()->GetCurrentRoofAtttachWallRoomSpace(AttachRoomID);
		for (TSharedPtr<FArmyRoomSpaceArea> iter : results)
		{
			iter->SetRoofDropDownHeight(0);
		}
	}

    ClearExtrusionActors();

	if (cleanEditAreas && RoomEditAreas.Num() > 0)
	{
		//@郭子阳
		//递归地删除绘制区域
		std::function<void(TSharedPtr<FArmyBaseArea>& Area)>
		DeleteSubAreas = [&DeleteSubAreas](TSharedPtr<FArmyBaseArea> & Area) {
			for (auto & SubArea : Area->GetEditAreas())
			{
				DeleteSubAreas(SubArea);
			}
			//Area->RoomEditAreas.Empty();

			FArmySceneData::Get()->Delete(Area);
		};

		for (auto& SubArea : this->GetEditAreas())
		{
			DeleteSubAreas(SubArea);
		}
		RoomEditAreas.Empty();

		////梁晓菲  重置面的时候把面上做的造型一并重置
		//while (RoomEditAreas.Num() > 0)
		//{
		//	FArmySceneData::Get()->Delete(RoomEditAreas.Top());
		//}
		//RoomEditAreas.Empty();
	}
	RefreshPolyVertices();
	RoomDataStatus |= FLAG_DEFAULTSTYLE;
	SkitLineID = -1;
	DestroyAttachActor();
	ContentItemList.Empty();
}

//float FArmyRoomSpaceArea::GetAreaDimension() const
//{
//	float holeArea = 0.0f;
//	for (auto iter : SurfaceHoles)
//	{
//		holeArea += FArmyMath::CalcPolyArea(iter) / 10000.0f;
//	}
//	if (SurfaceType == 1)
//	{
//		holeArea += FArmyMath::CalcPolyArea(dropVerts) / 10000.0f;
//	}
//	for (auto iter0 : RoomEditAreas)
//	{
//		holeArea += iter0->GetAreaDimension();
//	}
//	float area = FArmyMath::CalcPolyArea(OutArea->Vertices) / 10000.0f - holeArea;
//	return area;
//}

/* @郭子陽 重载，设置施工项区域*/
void FArmyRoomSpaceArea::GetConstructionAreas(TArray<FArmyGoods>& ArtificialData)
{
	GetCommonConstruction(ArtificialData);
	//原始墙面施工项
	GetConstructionData(ArtificialData); 
	for (auto& iter0 : GetEditAreas())
	{
		iter0->GetConstructionAreas(ArtificialData);
	}

    TMap<AArmyExtrusionActor*, FArmyExtrusionPath> ActorMap;
    if (SurfaceType == 0)
    {
        ActorMap = GetBaseboardExtruder()->GetActorMap();
    }
    else if (SurfaceType == 2)
    {
        ActorMap = GetCrownMouldingExtruder()->GetActorMap();
    }

	if (ActorMap.Num() <= 0)
	{
		return;
	}

	// 多段线放样施工项
	for (auto& SkirtingActor : ActorMap)
	{
		TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData= XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), SkirtingActor.Key->GetConstructionParameter());
			//= FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(SkirtingActor.Key->UniqueCodeExtrusion);
		FArmyRoom::ItemIDCheckId TempItemIDCheckId;
		TempItemIDCheckId.ItemID = SkitLineID;

		FArmyGoods goods;
		goods.GoodsId = SkitLineID;
		goods.Type = 1;

		if (tempConstructoinData.IsValid())
		{
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

		Goods.Param.L += SkirtingActor.Key->Circumference / 100;
		Goods.Param.C += SkirtingActor.Key->Circumference / 100;
		Goods.Param.D += SkirtingActor.Key->Circumference / 100;
	}
}
void FArmyRoomSpaceArea::GetCommonConstruction(TArray<FArmyGoods>& ArtificialData)
{
	if (!MatStyle->HasGoodID())
	{
		return;
	}

	if (MatStyle->GetEditType() == S_IntervalStyle || MatStyle->GetEditType() == S_CrossStyle)
	{
		//现在没有这两种类型 不予考虑
	}
	else if (MatStyle->GetEditType() != S_SeamlessStyle)
	{
		//TSharedPtr<FArmySeamlessStyle> tempStyle = StaticCastSharedPtr<FArmySeamlessStyle>(MatStyle);
		{
			FArmyGoods goods;
			goods.GoodsId = MatStyle->GetGoodsID();
			goods.Type = 1;
			goods.PaveID = ConvertStyleToPavingID(MatStyle);

			
			// 施工项
			//ConstructionPatameters Parameter;
			//bool HasHung = false;
			//if (SurfaceType == 2 && GetExtrusionHeight() != 0)
			//{
			//	HasHung = true;
			//}
			//	Parameter.SetPaveInfo(GetStyle(M_InnearArea), (EWallType)SurportPlaceArea, HasHung, , GetRoomSpaceID());
			TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), GetMatConstructionParameter());
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

			//为过门石添加空间

			if (this->GenerateFromObjectType == OT_Pass ||
				this->GenerateFromObjectType == OT_DoorHole ||
				this->GenerateFromObjectType == OT_NewPass ||
				this->GenerateFromObjectType == OT_Door ||
				this->GenerateFromObjectType == OT_SecurityDoor ||
				this->GenerateFromObjectType == OT_SlidingDoor)
			{
				//	this->GetConstructionAreas(HardwareData.ArtificialData);
				bool HaveSpaceName1 = false;
				bool HaveSpaceName2 = false;

				TArray<FString> RelatedHardWareId;
				TArray<TWeakPtr<FArmyObject>> TotalInnearRoom;
				FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRoom);

				for (TWeakPtr<FArmyObject> iter : TotalInnearRoom)
				{
					if (HaveSpaceName1 && HaveSpaceName2)
					{
						break;
					}
					FArmyRoom* currentRoom = iter.Pin()->AsassignObj<FArmyRoom>();
					if (currentRoom)
					{
						TArray<TSharedPtr<FArmyHardware>> relatedObjects;
						currentRoom->GetHardwareListRelevance(relatedObjects);
						for (TWeakPtr<FArmyObject> iter0 : relatedObjects)
						{

							if (this->AttachRoomID == iter0.Pin()->GetUniqueID().ToString())
							{
								if (!HaveSpaceName1)
								{
									goods.SpaceMsg.SpaceName1 = currentRoom->GetSpaceName();
									HaveSpaceName1 = true;
								}
								else if (!HaveSpaceName2)
								{
									goods.SpaceMsg.SpaceName2 = currentRoom->GetSpaceName();
									HaveSpaceName2 = true;
								}
								else
								{
									break;
								}
							}

						}
					}


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

			Goods.Param.D += MatStyle->GetApplyBrickNum();
			Goods.Param.S += GetAreaDimension();
			Goods.Param.L += GetAreaCircumference() / 100;
			Goods.Param.C += GetAreaCircumference() / 100;
		}
	}
	else if (MatStyle->GetEditType() == S_SeamlessStyle)
	{
		TSharedPtr<FArmySeamlessStyle> tempStyle = StaticCastSharedPtr<FArmySeamlessStyle>(MatStyle);
		{
			FArmyGoods goods;
			goods.GoodsId = MatStyle->GetGoodsID();
			goods.Type = 1;
			goods.PaveID = ConvertStyleToPavingID(MatStyle);

			if (tempStyle->SeamlesStyleType == 1)
			{
				FArmyLatexPaintColor PaintColor;
				PaintColor.LatexPaintColorNumber = tempStyle->ColorPaintValueKey;
				goods.LatexPaintColorData.Add(PaintColor);
			}

			// 施工项
			//ConstructionPatameters Parameter;
			//bool HasHung = false;
			//if (SurfaceType == 2 && GetExtrusionHeight() != 0)
			//{
			//	HasHung = true;
			//}
			//	Parameter.SetPaveInfo(GetStyle(M_InnearArea), (EWallType)SurportPlaceArea, HasHung);
				TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), GetMatConstructionParameter());
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

			//为过门石添加空间

			if (this->GenerateFromObjectType == OT_Pass ||
				this->GenerateFromObjectType == OT_DoorHole ||
				this->GenerateFromObjectType == OT_NewPass ||
				this->GenerateFromObjectType == OT_Door ||
				this->GenerateFromObjectType == OT_SecurityDoor ||
				this->GenerateFromObjectType == OT_SlidingDoor)
			{
				//	this->GetConstructionAreas(HardwareData.ArtificialData);
				bool HaveSpaceName1 = false;
				bool HaveSpaceName2 = false;

				TArray<FString> RelatedHardWareId;
				TArray<TWeakPtr<FArmyObject>> TotalInnearRoom;
				FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRoom);

				for (TWeakPtr<FArmyObject> iter : TotalInnearRoom)
				{
					if (HaveSpaceName1 && HaveSpaceName2)
					{
						break;
					}
					FArmyRoom* currentRoom = iter.Pin()->AsassignObj<FArmyRoom>();
					if (currentRoom)
					{
						TArray<TSharedPtr<FArmyHardware>> relatedObjects;
						currentRoom->GetHardwareListRelevance(relatedObjects);
						for (TWeakPtr<FArmyObject> iter0 : relatedObjects)
						{

							if (this->AttachRoomID == iter0.Pin()->GetUniqueID().ToString())
							{
								if (!HaveSpaceName1)
								{
									goods.SpaceMsg.SpaceName1 = currentRoom->GetSpaceName();
									HaveSpaceName1 = true;
								}
								else if (!HaveSpaceName2)
								{
									goods.SpaceMsg.SpaceName2 = currentRoom->GetSpaceName();
									HaveSpaceName2 = true;
								}
								else
								{
									break;
								}
							}

						}
					}


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
			Goods.Param.D += 1;
			Goods.Param.S += GetAreaDimension();
			Goods.Param.L += GetAreaCircumference() / 100;
			Goods.Param.C += GetAreaCircumference() / 100;
			if (tempStyle->SeamlesStyleType == 1)
			{
				Goods.LatexPaintColorData[0].Dosage += GetAreaDimension();
			}
		}
	}
}

void FArmyRoomSpaceArea::SetExtrusionHeight(float InHeight)
{
	if (M_ExtrusionHeight != InHeight)
	{
        FArmyBaseArea::SetExtrusionHeight(InHeight);

		MatStyle->SetStylePlaneOffset(InHeight);
		RoomDataStatus &= ~FLAG_DEFAULTSTYLE;
		TArray<FVector> tempOutVertices;
		TArray<TArray<FVector>> Holes;
		CalculateOutAndInnerHoles(tempOutVertices, Holes);
		CalculateBackGroundSurface(tempOutVertices, Holes);
		CalculateWallActor(MatStyle);
		if (SurfaceType == 2 && GenerateFromObjectType == OT_InternalRoom)
		{
			TArray < TSharedPtr<FArmyRoomSpaceArea>> results = FArmySceneData::Get()->GetCurrentRoofAtttachWallRoomSpace(AttachRoomID);
			for (TSharedPtr<FArmyRoomSpaceArea> iter : results)
			{
				iter->SetRoofDropDownHeight(InHeight);
			}
		}
	}
}

void FArmyRoomSpaceArea::SetRoofDropDownHeight(float InHeight)
{
	// 房顶下掉后引起墙面瓷砖地板从新进行计算
	if (SurfaceType == 1 /*&& GenerateFromObjectType == OT_WallLine*/)
	{
		FBox box(OutArea->Vertices);
		float width = box.GetSize().X;
		float height = box.GetSize().Y;
		FVector LeftDown = PlaneOrignPos - PlaneXDir * width / 2 - PlaneYDir * (height / 2 - InHeight);
		FVector leftUp = LeftDown - PlaneYDir * InHeight;
		FVector RightUp = leftUp + width * PlaneXDir;
		FVector RightDown = RightUp + PlaneYDir *InHeight;
		dropVerts.Empty();
		TArray<FVector>	RoofFallOffRegionVerts = { LeftDown,leftUp,RightUp,RightDown };

		for (FVector& iter : RoofFallOffRegionVerts)
		{
			float x = FVector::DotProduct((iter - PlaneOrignPos), PlaneXDir);
			float y = FVector::DotProduct((iter - PlaneOrignPos), PlaneYDir);
			dropVerts.Push(FVector(x, y, 0.0f));
		}

		TArray<FVector> tempOutverts;
		TArray<TArray<FVector>> tmepHoles;
		CalculateOutAndInnerHoles(tempOutverts, tmepHoles);
		tmepHoles.Emplace(dropVerts);

		if (MatStyle.IsValid())
		{
			MatStyle->SetDrawArea(tempOutverts, tmepHoles);
			if (M_ExtrusionHeight != 0.0f)
				MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
			CalculateWallActor(MatStyle);
		}
	}
}

void FArmyRoomSpaceArea::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyBaseArea::SetPropertyFlag(InType, InUse);
	int number = RoomEditAreas.Num();
	for (int i = 0; i < number; ++i)
	{
		RoomEditAreas[i]->SetPropertyFlag(InType, InUse);
	}

    if (SurfaceType == 0)
    {
        for (auto& It : GetBaseboardExtruder()->GetActorMap())
        {
            It.Key->SetActorHiddenInGame(!InUse);
        }
    }
    else if (SurfaceType == 2)
    {
        for (auto& It : GetCrownMouldingExtruder()->GetActorMap())
        {
            It.Key->SetActorHiddenInGame(!InUse);
        }
    }
}

void FArmyRoomSpaceArea::SetActorVisible(bool InVisible)
{
	FArmyBaseArea::SetActorVisible(InVisible);
	int number = RoomEditAreas.Num();

    if (SurfaceType == 0)
    {
        for (auto& It : GetBaseboardExtruder()->GetActorMap())
        {
            It.Key->SetActorHiddenInGame(!InVisible);
        }
    }
    else if (SurfaceType == 2)
    {
        for (auto& It : GetCrownMouldingExtruder()->GetActorMap())
        {
            It.Key->SetActorHiddenInGame(!InVisible);
        }
    }

	for (AArmyExtrusionActor* Buckle : BuckleActors)
	{
		if (Buckle != nullptr)
			Buckle->SetActorHiddenInGame(!InVisible);
	}
}

void FArmyRoomSpaceArea::PostModify(bool bTransaction)
{
	if (ExtrudedxfFilePath.IsEmpty())
	{
		if (ExturesionActor)
		{
			ExturesionActor->Destroy();
			ExturesionActor = NULL;
		}
		
        if (SurfaceType == 0)
        {
            GetBaseboardExtruder()->Clear();
        }
        else if (SurfaceType == 2)
        {
            GetCrownMouldingExtruder()->Clear();
        }
	}
	// 通知区域内区域更新
	for (TSharedPtr<FArmyBaseArea> iter : RoomEditAreas)
	{
		iter->RefreshExtrusionPlane();
	}
}

void FArmyRoomSpaceArea::Destroy()
{
	FArmyBaseArea::Destroy();
    ClearExtrusionActors();

	if (OrignalSurfaceActor)
	{
		OrignalSurfaceActor->Destroy();
		OrignalSurfaceActor = NULL;
	}
}

void  FArmyRoomSpaceArea::GetBuckleConstructionItems(TArray<FArmyGoods>& ArtificialData, const TSharedPtr<FArmyRoomSpaceArea> RelateRoom)
{
	if (RelateRoom.IsValid() )
	{
		for (auto& iter : BuckleMap)
		{

			auto BuckleActor = BuckleActorMap.Find(iter.Key);
			if (!BuckleActor || !(*BuckleActor))
			{
				//扣条不存在
				return;
			}

			FArmyAreaEdge& edge = iter.Key;
			FVector edgeStart = edge.GetStartPos().X * PlaneXDir + edge.GetStartPos().Y*PlaneYDir + PlaneOrignPos;
			FVector edgeEnd = edge.GetEndPos().X * PlaneXDir + edge.GetEndPos().Y * PlaneYDir + PlaneOrignPos;
			edgeStart = FVector(edgeStart.X, edgeStart.Y, 0.0f);
			edgeEnd = FVector(edgeEnd.X, edgeEnd.Y, 0.0f);
			TArray<FVector> ConvertEdges;
			for (FVector& iter : RelateRoom->GetOutArea()->Vertices)
			{
				ConvertEdges.Emplace(iter.X * RelateRoom->GetXDir() + iter.Y * RelateRoom->GetYDir() + RelateRoom->GetPlaneCenter());
			}
			int number = ConvertEdges.Num();
			for (int i = 0; i < number; ++i)
			{
				FVector TestEdgeStart = FVector(ConvertEdges[i%number].X, ConvertEdges[i%number].Y, 0.0f);
				FVector TestEdgeEnd = FVector(ConvertEdges[(i + 1) % number].X, ConvertEdges[(i + 1) % number].Y, 0.0f);;
				if (FArmyMath::AreLinesParallel(edgeStart, edgeEnd, TestEdgeStart, TestEdgeEnd) && FArmyMath::CalcPointToLineDistance(edgeStart, TestEdgeStart, TestEdgeEnd) < 5.0f)
				{

					FArmyGoods goods;
					goods.GoodsId = iter.Value->ID;
					goods.Type = 1;

					// 施工项
					TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), (*BuckleActor)->GetConstructionParameter());
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
					//if (!ArtificialData.Find(goods, GoodsIndex))
					//{
					GoodsIndex = ArtificialData.Add(goods);
					//}
					FArmyGoods& Goods = ArtificialData[GoodsIndex];
					//合并施工项数量
					for (int32 i = 0; i < ArtificialData[GoodsIndex].QuotaData.Num(); i++)
					{
						ArtificialData[GoodsIndex].QuotaData[i].Dosage += 1;
					}
					Goods.Param.D = (edge.GetStartPos() - edge.GetEndPos()).Size() / 100.0f;
					Goods.Param.L = (edge.GetStartPos() - edge.GetEndPos()).Size() / 100.0f;
					Goods.Param.C = (edge.GetStartPos() - edge.GetEndPos()).Size() / 100.0f;

					break;
				}
			}
		}
	}


}



bool FArmyRoomSpaceArea::IsBuckleConnected(AArmyExtrusionActor * Buckle, const TSharedPtr<FArmyRoomSpaceArea> AnotherArea)
{

	if (!AnotherArea.IsValid() || !Buckle)
	{
		return false;
	}
	auto EdgePtr = BuckleActorMap.FindKey(Buckle);
	if (!EdgePtr)
	{
		return false;
	}


	FVector edgeStart = EdgePtr->GetStartPos().X * PlaneXDir + EdgePtr->GetStartPos().Y*PlaneYDir + PlaneOrignPos;
	FVector edgeEnd = EdgePtr->GetEndPos().X * PlaneXDir + EdgePtr->GetEndPos().Y * PlaneYDir + PlaneOrignPos;
	edgeStart = FVector(edgeStart.X, edgeStart.Y, 0.0f);
	edgeEnd = FVector(edgeEnd.X, edgeEnd.Y, 0.0f);
	TArray<FVector> ConvertEdges;
	for (FVector& iter : AnotherArea->GetOutArea()->Vertices)
	{
		ConvertEdges.Emplace(iter.X * AnotherArea->GetXDir() + iter.Y * AnotherArea->GetYDir() + AnotherArea->GetPlaneCenter());
	}
	int number = ConvertEdges.Num();
	for (int i = 0; i < number; ++i)
	{
		FVector TestEdgeStart = FVector(ConvertEdges[i%number].X, ConvertEdges[i%number].Y, 0.0f);
		FVector TestEdgeEnd = FVector(ConvertEdges[(i + 1) % number].X, ConvertEdges[(i + 1) % number].Y, 0.0f);;
		if (FArmyMath::AreLinesParallel(edgeStart, edgeEnd, TestEdgeStart, TestEdgeEnd) && FArmyMath::CalcPointToLineDistance(edgeStart, TestEdgeStart, TestEdgeEnd) < 5.0f)
		{
			return true;
		}
	}
	return false;

}

bool FArmyRoomSpaceArea::CheckEdgeCouldPlaceBuckle(const FVector& startEdge, const FVector& endEdge)
{
	if (GenerateFromObjectType == OT_Pass ||
		GenerateFromObjectType == OT_SlidingDoor ||
		GenerateFromObjectType == OT_SecurityDoor ||
		GenerateFromObjectType == OT_NewPass ||
		GenerateFromObjectType == OT_Door)
	{
		TArray<TWeakPtr<FArmyObject>> TotalBridgeStoneAreas;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, TotalBridgeStoneAreas);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_NewPass, TotalBridgeStoneAreas);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, TotalBridgeStoneAreas);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, TotalBridgeStoneAreas);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, TotalBridgeStoneAreas);
		FArmyHardware* BridgeStoneArea = NULL;
		for (TWeakPtr<FArmyObject> iter : TotalBridgeStoneAreas)
		{
			if (iter.Pin()->GetUniqueID().ToString() == AttachRoomID)
			{
				BridgeStoneArea = iter.Pin()->AsassignObj<FArmyHardware>();
				break;
			}
		}
		if (BridgeStoneArea)
		{
			FVector startPos = FVector(BridgeStoneArea->GetStartPos().X, BridgeStoneArea->GetStartPos().Y, 0.0f);
			FVector endPos = FVector(BridgeStoneArea->GetEndPos().X, BridgeStoneArea->GetEndPos().Y, 0.0f);
			if (FArmyMath::AreLinesParallel(startEdge, endEdge, startPos, endPos))
			{
				return true;
			}
		}
	}
	return false;
}

FGuid FArmyRoomSpaceArea::GetExtrusionLightMapID(class AArmyExtrusionActor* InActor)
{
    TMap<AArmyExtrusionActor*, FArmyExtrusionPath> ActorMap;
    if (SurfaceType == 0)
    {
        ActorMap = GetBaseboardExtruder()->GetActorMap();
    }
    else if (SurfaceType == 2)
    {
        ActorMap = GetCrownMouldingExtruder()->GetActorMap();
    }

    int32 Index = 0;
	for (auto& It : ActorMap)
	{
		if (It.Key == InActor)
		{
			if (SkirtLineGUIDs.IsValidIndex(Index))
			{
				return SkirtLineGUIDs[Index];
			}
		}

        ++Index;
	}

	return FGuid();
}

TArray<TSharedPtr<AreaCompare>> FArmyRoomSpaceArea::CombineOutAreaAndInnearHole(const TArray<FVector>& OutArea, const TArray<TArray<FVector>>& Holes)
{
	Clipper c;
	Paths ClipperAreas;
	Paths HolePaths;
	Paths ClipperSolution;
	int holeNumbers = Holes.Num();
	ClipperAreas.resize(1);
	for (int i = 0; i < OutArea.Num(); i++)
	{
		cInt x = (cInt)(OutArea[i].X * SCALE0);
		cInt y = (cInt)(OutArea[i].Y * SCALE0);
		ClipperAreas[0].push_back(IntPoint(x, y));
	}
	for (int i = 0; i < holeNumbers; i++)
	{
		Path holePath;
		for (int j = 0; j < Holes[i].Num(); j++)
		{
			cInt x = (cInt)(Holes[i][j].X * SCALE0);
			cInt y = (cInt)(Holes[i][j].Y * SCALE0);
			holePath.push_back(IntPoint(x, y));
		}
		HolePaths.push_back(holePath);
	}
	c.AddPaths(ClipperAreas, ptSubject, true);
	c.AddPaths(HolePaths, ptClip, true);
	c.Execute(ctDifference, ClipperSolution, pftEvenOdd, pftEvenOdd);

	TArray<AreaCompare> tempAreas;
	for (int i = 0; i < ClipperSolution.size(); ++i)
	{
		Path tempPath = ClipperSolution[i];
		TArray<FVector> tempOutVertices;
		for (int j = 0; j < tempPath.size(); ++j)
		{
			IntPoint point = tempPath[j];
			FVector x = point.X / SCALE0 * PlaneXDir;
			FVector y = point.Y / SCALE0*PlaneYDir;
			tempOutVertices.Push(FVector(point.X / SCALE0, point.Y / SCALE0, 0.0f));
		}
		tempAreas.Push(AreaCompare(tempOutVertices));
	}
	tempAreas.Sort();
	TArray<TSharedPtr<AreaCompare>> OrginalAreas;
	for (int i = 0; i < tempAreas.Num(); ++i)
	{
		OrginalAreas.Push(MakeShareable(new AreaCompare(tempAreas[i].M_Area)));
	}
	TArray<TSharedPtr<AreaCompare>> Results;
	while (OrginalAreas.Num() > 0)
	{

		TSharedPtr<AreaCompare> start = OrginalAreas[0];

		OrginalAreas.Remove(start);
		TArray<TSharedPtr<AreaCompare>> RemoveAreas;
		for (int i = 0; i < OrginalAreas.Num(); ++i)
		{
			if (start->IsContainOther(OrginalAreas[i]))
			{
				RemoveAreas.Push(OrginalAreas[i]);
			}
		}
		for (int i = 0; i < RemoveAreas.Num(); ++i)
		{
			OrginalAreas.Remove(RemoveAreas[i]);
		}
		Results.Push(start);

	}

	return Results;
}

// 获得墙面片对应的墙线
TSharedPtr<class FArmyWallLine> FArmyRoomSpaceArea::GetAttachLine()
{
	if (GenerateFromObjectType != OT_WallLine)
	{
		return nullptr;
	}

	TSharedPtr<FArmyRoom> CurrentAttachRoom;
	if (FArmySceneData::Get()->GetInnearRoomByRoomID(AttachRoomID, CurrentAttachRoom))
	{
		//获取命中墙体
		TArray< TSharedPtr<FArmyWallLine>> WallLinesArray = CurrentAttachRoom->GetWallLines();
		for (auto Line : WallLinesArray)
		{
			if (AttachBspID.Contains(Line->GetUniqueID().ToString()))
			{
				return Line;
			}
		}
	}
	else
	{
		return nullptr;
	}

	return nullptr;
}

// 获得绘制立面图的线数据  0 表示地，1表示墙，2 表示顶
TArray<struct FLinesInfo> FArmyRoomSpaceArea::GetFacadeBox(int32 InSurfaceType)
{
	TArray<struct FLinesInfo> OutLinesArray;
	if (InSurfaceType != 1)
	{
		return OutLinesArray;
	}

	FLinearColor NewColor = FLinearColor(FColor(0xFF383A3D));
	// 当前墙面的铺贴数据
	if (MatStyle.IsValid())
	{
		FVector RoomPlaneCenter = GetPlaneCenter();
		for (auto& iter : MatStyle->GetClipperBricks())
		{
			for (auto& iter0 : iter.Value)
			{
				TArray<TArray<FVector>> CutResults = iter0->GetCutResults();
				int number = CutResults.Num();
				for (int i = 0; i < number; ++i)
				{
					OutLinesArray.Add(FLinesInfo(CutResults[i],NewColor,0,0));
				}
			}
		}
	}

	// 当前墙面包含的编辑面铺贴数据
	for (auto It : RoomEditAreas)
	{
		if (!It.IsValid())
		{
			continue;
		}

		if (It->GetMatStyle().IsValid())
		{
			FVector RoomPlaneCenter = It->GetPlaneCenter();
			for (auto& iter : It->GetMatStyle()->GetClipperBricks())
			{
				for (auto& iter0 : iter.Value)
				{
					TArray<TArray<FVector>> CutResults = iter0->GetCutResults();
					int number = CutResults.Num();
					for (int i = 0; i < number; ++i)
					{
						OutLinesArray.Add(FLinesInfo(CutResults[i], NewColor, 0, 0));
					}
				}
			}
		}
	}

	return OutLinesArray;
}

const float FArmyRoomSpaceArea::GetZ() const
{
    if (SurfaceType == 0)
    {
        return M_ExtrusionHeight;
    }
    else if (SurfaceType == 2)
    {
        return FArmySceneData::WallHeight + M_ExtrusionHeight;
    }

    return 0.f;
}
