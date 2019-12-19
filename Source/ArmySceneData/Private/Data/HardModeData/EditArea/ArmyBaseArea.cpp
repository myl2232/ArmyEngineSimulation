#include "ArmyBaseArea.h"
#include "ArmyBodaArea.h"
#include "Primitive2D/XRPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyEditPoint.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyContinueStyle.h"
#include "ArmyCrossStyle.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyIntervalStyle.h"
#include "ArmyTrapezoidStyle.h"
#include "ArmyWhirlwindStyle.h"
#include "ArmyWorkerStyle.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyExtrusionActor.h"
#include "ArmyViewportClient.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyExtrusionActor.h"
#include "ArmyWallActor.h"
#include "../Mathematics/RectangleManager.h"
#include "ArmyActorConstant.h"
#include "ArmyGameInstance.h"
#include "ArmyHttpModule.h"
#include "ArmyUser.h"
#include "ArmySeamlessStyle.h"
#include "SArmyMulitCategory.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"
#include "ArmyClipper.h"
#include "ArmySceneData/Private/Data/HardModeData/Mathematics/EarcutTesselator.h"
#include "ArmyResourceModule.h"
#include "ArmyConstructionManager.h"
#include "ArmyLayingPoint.h"
#include "ArmyFreePolygonArea.h"

FArmyBaseArea::FArmyBaseArea()
{
	RoomDataStatus |= FLAG_DEFAULTSTYLE;
	RoomDataStatus &= ~FLAG_HAVESKITLINE;
	RoomDataStatus &= ~FLAG_CLOSESKITLINE;

	ExturesionActor = NULL;
	LampSlotActor = NULL;
	WallActor = NULL;
	SetPropertyFlag(FArmyObject::FLAG_COLLISION, true);
	InneraArea = MakeShareable(new FArmyPolygon());
	OutArea = MakeShareable(new FArmyPolygon());
	MI_BackGroundColor = UMaterialInstanceDynamic::Create(FArmyEngineModule::Get().GetEngineResource()->GetDefaultFloorTextureMat(), NULL);
	MI_BackGroundColor->AddToRoot();
	VectorCode = -1;
	GapColor = FLinearColor(1.f, 1.f, 1.f, 1.0f);

	MatStyle = MakeShareable(new FArmyContinueStyle());
	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
	LayingPointPos = FVector::ZeroVector;
}

void FArmyBaseArea::SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)
{
	TArray<FVector> OutLine;
	TArray<TArray<FVector>> innear;
	CalculateOutAndInnerHoles(OutLine, innear);
	if (GetLampSlotExtruder()->IsEmpty())
	{
		MatStyle = InStyle;
		MatStyle->SetDrawArea(InneraArea->Vertices, innear);
		MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
	}
	else
	{
		if (m_deltaHeight != 0)
		{
			MatStyle = InStyle;
			TArray<TArray<FVector>> innear;
			TArray<FVector> tempNewVerts = InneraArea->Vertices;
			tempNewVerts = FArmyMath::ExturdePolygon(tempNewVerts, LampSlotWidth, false);
			MatStyle->SetDrawArea(tempNewVerts, innear);
			MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		}
	}
	MatStyle->StyleDirty.BindRaw(this, &FArmyBaseArea::UpdateWallActor);
	CalculateWallActor(MatStyle);
}

const TSharedPtr<FArmyBaseEditStyle> FArmyBaseArea::GetStyle(AreaType m_defualtType /*= M_InnearArea*/)
{
	if (m_defualtType == M_BodaArea && BodaSurfaceArea.IsValid())
	{
		//@王志超
		return BodaSurfaceArea->GetStyle();
	}
	else if (m_defualtType == M_InnearArea)
	{
		return MatStyle;
	}

	return nullptr;
}

void FArmyBaseArea::DrawConstructionMode(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
    // @欧石楠 绘制灯带
    if (SurfaceType == 2)
    {
        TSparseArray<FArmyLightPath> LightConstructionInfo = GetLampSlotExtruder()->GetLightConstructionInfo();
        for (const auto& It : LightConstructionInfo)
        {
            DrawDashedLine(PDI, It.StartPos, It.EndPos, FLinearColor(FColor(0XFFFFF700)), 2.0f, 1.0f);
        }
    }

	//如果不绘制顶面除灯带外的其他东西，直接停止绘制
	if (SurfaceType == 2 && !FArmyObject::GetDrawModel(MODE_OTHERCEILINGOBJ)) return;

	// @是顶&&无缝铺 
	if (SurfaceType == 2 && MatStyle->GetEditType() == S_SeamlessStyle)
	{
		//&&进行了下吊  ，斜线绘制下吊区域
		if (M_ExtrusionHeight != 0.0f)
			DrawRoof(PDI, View);
	}
	else
		MatStyle->DrawWireFrame(PDI, View);//按照放置的物体的样式进行绘制

	//放样线条颜色
	FLinearColor LineColor;
	if (SurfaceType == 0)
	{
		LineColor = FLinearColor(FColor(0XFF3D3D3D));

		//踢脚线两段线在门洞处闭合
		if (ConstructionSkitLineVerts.Num() > 0)
		{
			for (int i = 0; i < ConstructionSkitLineVerts[0].Num(); i++)
			{
				PDI->DrawLine(ConstructionSkitLineVerts[0][i][0], ConstructionSkitLineVerts[ConstructionSkitLineVerts.Num() - 1][i][0], LineColor, 1);
				PDI->DrawLine(ConstructionSkitLineVerts[0][i][ConstructionSkitLineVerts[0][i].Num() - 1], ConstructionSkitLineVerts[ConstructionSkitLineVerts.Num() - 1][i][ConstructionSkitLineVerts[0][i].Num() - 1], LineColor, 1);
			}
		}
	}
	else if (SurfaceType == 2)
		LineColor = FLinearColor(FColor(0XFF00FFFF));
	else
		LineColor = FLinearColor(FColor(0XFFFF0000));
	//在图纸模式下绘制放样线条
	if (ConstructionSkitLineVerts.Num() > 0)
	{
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

	// @梁晓菲 对于绘制的区域，画边界线
	DrawOutLine(PDI, View, LineColor, 1.0f);
}

void FArmyBaseArea::DrawRoof(FPrimitiveDrawInterface* PDI, const FSceneView* View)
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
	MeshBuilder.Draw(PDI, FMatrix::Identity, FArmyEngineModule::Get().GetEngineResource()->GetObliqueLineMat()->GetRenderProxy(false), 0, true);
	PDI->SetHitProxy(NULL);
}

TSharedPtr<class FArmyLampSlotExtruder> FArmyBaseArea::GetLampSlotExtruder()
{
    if (!LampSlotExtruder.IsValid())
    {
        LampSlotExtruder = MakeShareable(new FArmyLampSlotExtruder(GVC->GetWorld(), StaticCastSharedRef<FArmyBaseArea>(this->AsShared())));
    }

    return LampSlotExtruder;
}

void FArmyBaseArea::GetCommonConstruction(TArray<FArmyGoods>&  ArtificialData)
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

		{
			FArmyGoods goods;
			goods.GoodsId = MatStyle->GetGoodsID();
			goods.Type = 1;
			goods.PaveID = ConvertStyleToPavingID(MatStyle);

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

	

			// 施工项
		/*	ConstructionPatameters Parameter;
			bool HasHung = false;
			if (SurfaceType == 2 && GetExtrusionHeight() != 0)
			{
				HasHung = true;
			}
			Parameter.SetPaveInfo(GetStyle(M_InnearArea), (EWallType)SurportPlaceArea, HasHung);*/

			//Parameter.SetPaveInfo(MatStyle, (EWallType)SurportPlaceArea);
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

	/*		if (tempStyle->SeamlesStyleType == 1)
			{
				
			}*/

			if (tempStyle->SeamlesStyleType == 1)
			{
				//找到或添加乳胶漆色号和面积
				FArmyLatexPaintColor PaintColor;
				PaintColor.LatexPaintColorNumber = tempStyle->ColorPaintValueKey;
				Goods.LatexPaintColorData.AddUnique(PaintColor);

				int32 index=Goods.LatexPaintColorData.Find(PaintColor);
				Goods.LatexPaintColorData[index].Dosage += GetAreaDimension();
			}
		}

	}

}

int32 FArmyBaseArea::ConvertStyleToPavingID(TSharedPtr<FArmyBaseEditStyle> InStyle)
{
	switch (InStyle->GetEditType())
	{
	case S_ContinueStyle:
		return 1;
	case S_CrossStyle:
		return 0;
	case S_HerringBoneStyle:
		return 4;
	case S_TrapeZoidStyle:
		return 5;
	case S_WorkerStyle:
		return 2;
	case S_WhirlwindStyle:
		return 3;
	case S_SlopeContinueStyle:
		return 7;
	default:
		return 0;
	}
}

void FArmyBaseArea::DestroyAttachActor()
{
    ClearExtrusionActors();

	if (BuckleActors.Num() > 0)
	{
		for (auto & Actor : BuckleActors)
		{
			if (Actor != nullptr)
			{
				FArmySceneData::Get()->DeleteExtrusionConstructionItemData(Actor->UniqueCodeExtrusion);
				Actor->Destroy();
				Actor = NULL;
			}
		}
		BuckleEdges.Empty();
		BuckleActors.Empty();
		BuckleMap.Empty();
		BuckleActorMap.Empty();
	}
}

void FArmyBaseArea::DestroyLampLight()
{
    GetLampSlotExtruder()->Clear();

	TArray<TArray<FVector>> InHoles;
	MatStyle->SetDrawArea(OutArea->Vertices, InHoles);
	CalculateWallActor(MatStyle);
	if (ContentItemList.Num() > 0)
	{
		GenerateExtrudeActor(GVC->GetWorld(), ContentItemList[0]);
	}
}

bool FArmyBaseArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	float Dis = 0;
	FVector2D PixPos;
	InViewportClient->WorldToPixel(Pos, PixPos);
	return IsSelected(FVector(PixPos, 0), InViewportClient, Dis);
}

bool FArmyBaseArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	return IsSelected(Pos, ViwePortClient, dist, OutArea->Vertices);
}

bool FArmyBaseArea::IsSelected(FVector InStart, FVector InEnd, FVector& IntersectionPoint, FVector& IntersectionNormal)
{
	float tempOffset = M_ExtrusionHeight == 0 ? 0.01 : (M_ExtrusionHeight - 0.2f);
	if (MatStyle.IsValid())
	{
		tempOffset = MatStyle->GetHeightToFloor();
		if (tempOffset == 0)
		{
			tempOffset = tempOffset == 0.0f ? 0.02f : tempOffset;;
		}
	}


	FVector  MWorldDir = (InEnd - InStart).GetSafeNormal();
	FPlane BasePlane(PlaneOrignPos + PlaneNormal * tempOffset, PlaneNormal);
	FVector MouseWorldPoint = FMath::LinePlaneIntersection(InStart, InStart + MWorldDir * 1000000.0f, BasePlane);
	FVector dir1 = -(MouseWorldPoint - InStart).GetSafeNormal();
	if (MWorldDir.Equals(dir1, 0.01f))
		return false;

	IntersectionPoint = MouseWorldPoint;
	IntersectionNormal = PlaneNormal;
	FVector2D relativeMousePos = TranlateToPlane(MouseWorldPoint);

	return FArmyMath::IsPointInOrOnPolygon2D(FVector(relativeMousePos, 0.0), OutArea->Vertices);
}

bool FArmyBaseArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient, float& dist, const TArray<FVector>& PolygonVertices)
{
	FVector MWorldStart, MWorldDir;

	InViewportClient->DeprojectFVector2D(FVector2D(Pos.X, Pos.Y), MWorldStart, MWorldDir);

	FPlane BasePlane(PlaneOrignPos + PlaneNormal*M_ExtrusionHeight, PlaneNormal);
	FVector MouseWorldPoint = FMath::LinePlaneIntersection(MWorldStart, MWorldStart + MWorldDir * ((InViewportClient->GetViewLocation() - FVector::PointPlaneProject(InViewportClient->GetViewLocation(), BasePlane)).Size() + 100000), BasePlane);

	FVector rayDirection = MWorldDir.GetSafeNormal();
	FVector dir1 = -(MouseWorldPoint - MWorldStart).GetSafeNormal();
	if (rayDirection.Equals(dir1, 0.01f))
		return false;
	dist = (MouseWorldPoint - MWorldStart).Size();

	FVector2D relativeMousePos = TranlateToPlane(MouseWorldPoint);

	return FArmyMath::IsPointInOrOnPolygon2D(FVector(relativeMousePos, 0.0), PolygonVertices);
}

void FArmyBaseArea::SetState(EObjectState InState)
{
	FArmyObject::SetState(InState);
}

void FArmyBaseArea::SetStateRecursively(EObjectState InState)
{
	SetState(InState);
	for (auto& SubArea : RoomEditAreas)
	{
		SubArea->SetStateRecursively(InState);
	}
}
AreaType FArmyBaseArea::SelectAreaType(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	float dist = 0.0f;
	if (IsSelected(Pos, InViewportClient, dist, OutArea->Vertices))
	{
		if (IsSelected(Pos, InViewportClient, dist, InneraArea->Vertices))
		{
			return M_InnearArea;
		}
		else
		{
			return M_BodaArea;
		}
	}
	else
		return M_OutArea;
}

TSharedPtr<FArmyBaseArea> FArmyBaseArea::ForceSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
{
	if (IsSelected(Pos, ViwePortClient, dist))
	{
		return  StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
	}
	return NULL;
}




void FArmyBaseArea::SetDrawOutLine(bool Enable)
{
	BeDrawOutLine = Enable;
}

void FArmyBaseArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	if (MatStyle.IsValid())
	{
		JsonWriter->WriteValue("StyleType", (int32)MatStyle->GetEditType());
		MatStyle->SerializeToJson(JsonWriter);
		JsonWriter->WriteValue("planeNormal", GetPlaneNormal().ToString());
		JsonWriter->WriteValue("planeOrignPos", GetPlaneCenter().ToString());
		JsonWriter->WriteValue("planeXDir", GetXDir().ToString());
		JsonWriter->WriteValue("planeYDir", GetYDir().ToString());
		JsonWriter->WriteValue("GapColor", GapColor.ToString());
		JsonWriter->WriteValue("parentGUID", GetParentGuid().ToString());
		JsonWriter->WriteValue("M_ExtrusionHeight", M_ExtrusionHeight);
		JsonWriter->WriteValue("SurfaceType", (int32)SurfaceType);
		JsonWriter->WriteValue("SurportPlaceArea", (int32)SurportPlaceArea);
		JsonWriter->WriteValue("SkitLineGoodsID", (int32)SkitLineID);
		JsonWriter->WriteValue("M_LampSlotWidth", LampSlotWidth);

		if (ContentItemList.Num() > 0)
		{
			JsonWriter->WriteArrayStart(TEXT("ContentItemList"));
			for (auto& ContentItem : ContentItemList)
			{
				JsonWriter->WriteObjectStart();
				ContentItem->SerializeToJson(JsonWriter);
				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayEnd();
		}
		//@打扮家 XRLightmass 序列化LightMapID
		if (WallActor && WallActor->MeshComponent->bBuildStaticLighting)
		{
			LightMapID = WallActor->GetLightMapGUIDFromSMC();
			JsonWriter->WriteValue("LightMapID", LightMapID.ToString());
		}
		//if (LampSlotActor && LampSlotActor->MeshComponent->bBuildStaticLighting)
		//{
		//	ExtrusionLightMapID = LampSlotActor->GetLightMapGUIDFromSMC();
		//	JsonWriter->WriteValue("ExtrusionLightMapID", ExtrusionLightMapID.ToString());
		//}
		//else if (ExturesionActor && ExturesionActor->MeshComponent->bBuildStaticLighting)
		//{
		//	ExtrusionLightMapID = ExturesionActor->GetLightMapGUIDFromSMC();
		//	JsonWriter->WriteValue("ExtrusionLightMapID", ExtrusionLightMapID.ToString());
		//}
	}
	//if (LampContentItem.IsValid())
	//{
	//	JsonWriter->WriteObjectStart(TEXT("LampContentItem"));
	//	LampContentItem->SerializeToJson(JsonWriter);
	//	JsonWriter->WriteObjectEnd();
	//}
	if (BuckleMap.Num() > 0)
	{
		JsonWriter->WriteArrayStart(TEXT("KouTiaoMap"));
		for (auto & temp : BuckleMap)
		{
			JsonWriter->WriteObjectStart();
			temp.Key.SerializeToJson(JsonWriter);
			temp.Value->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}



	/** 如果该面没有被点击过，就需要给这个面设置施工项默认值*/
	if (!ConstructionItemData->bHasSetted)
	{
		TSharedPtr<SArmyMulitCategory> ConstructionDetail = MakeShareable(new SArmyMulitCategory);
		if (SurfaceType == 0)
		{
			ConstructionDetail->Init(FArmyUser::Get().ConstructionFloorData);
		}
		else if (SurfaceType == 1)
		{
			ConstructionDetail->Init(FArmyUser::Get().ConstructionWallData);
		}
		else
		{
			ConstructionDetail->Init(FArmyUser::Get().ConstructionTopData);
		}
		ConstructionItemData->SetConstructionItemCheckedId(ConstructionDetail->GetCheckedData());
		ConstructionItemData->bHasSetted = true;
	}
	JsonWriter->WriteObjectStart(TEXT("RoomConstructionItemData"));
	ConstructionItemData->SerializeToJson(JsonWriter);
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectStart("layingPoint");
	JsonWriter->WriteValue("pos", LayingPointPos.ToString());
	JsonWriter->WriteObjectEnd();

    JsonWriter->WriteValue("surfaceType", (int32)SurfaceType);

    if (SurfaceType == 0)
    {
        JsonWriter->WriteObjectStart("baseboard");
        GetBaseboardExtruder()->SerializeToJson(JsonWriter);
        JsonWriter->WriteObjectEnd();

        // @马云龙 踢脚线GUID序列化
        JsonWriter->WriteArrayStart("SkirtLineGUIDs");
        for (auto& It : GetBaseboardExtruder()->GetActorMap())
        {
            JsonWriter->WriteValue(It.Key->GetLightMapGUIDFromSMC().ToString());
        }
        JsonWriter->WriteArrayEnd();
    }
    else if (SurfaceType == 2)
    {
        JsonWriter->WriteObjectStart("crownMoulding");
        GetCrownMouldingExtruder()->SerializeToJson(JsonWriter);
        JsonWriter->WriteObjectEnd();

        // @马云龙 踢脚线GUID序列化
        JsonWriter->WriteArrayStart("SkirtLineGUIDs");
        for (auto& It : GetCrownMouldingExtruder()->GetActorMap())
        {
            JsonWriter->WriteValue(It.Key->GetLightMapGUIDFromSMC().ToString());
        }
        JsonWriter->WriteArrayEnd();
    }

	//测试代码
	if (SurfaceType == 0)
	{
		SurfaceType = 0;
	}
	//@郭子阳
	//序列化子面
	if (IsPlotArea())
	{
		JsonWriter->WriteArrayStart("SubAreas");
		for (auto & SubArea : RoomEditAreas)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue<int32>(TEXT("SubAreaType"), (int32)SubArea->GetDrawAreaType());
			SubArea->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
	}





}

void FArmyBaseArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	//@打扮家 XRLightmass 反序列化LightMapID
	FString LightGUIDStr = "";
	InJsonData->TryGetStringField("LightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, LightMapID);
	LightGUIDStr = "";
	InJsonData->TryGetStringField("ExtrusionLightMapID", LightGUIDStr);
	FGuid::Parse(LightGUIDStr, ExtrusionLightMapID);

	int32 editStyle = (int32)InJsonData->GetNumberField("StyleType");
	switch (editStyle)
	{
	case 1:
		MatStyle = MakeShareable(new FArmyContinueStyle());
		break;
	case 2:
		MatStyle = MakeShareable(new FArmyHerringBoneStyle());
		break;
	case 3:
		MatStyle = MakeShareable(new FArmyTrapezoidStyle());
		break;
	case 4:
		MatStyle = MakeShareable(new FArmyWhirlwindStyle());
		break;
	case 5:
		MatStyle = MakeShareable(new FArmyWorkerStyle());
		break;
	case 6:
		MatStyle = MakeShareable(new FArmyCrossStyle());
		break;
	case 7:
		MatStyle = MakeShareable(new FArmyIntervalStyle());
		break;
	case 8:
		MatStyle = MakeShareable(new FArmySeamlessStyle());
		break;
	case 10:
		MatStyle = MakeShareable(new FArmySlopeContinueStyle());
		break;
	default:
		MatStyle = MakeShareable(new FArmyContinueStyle());
		return;
		break;
	}
	MatStyle->Deserialization(InJsonData);
	InneraArea->MaterialRenderProxy = MatStyle->GetMaterial()->GetRenderProxy(false);
	TSharedPtr<FContentItemSpace::FContentItem> MatStyleTempContentItem = MakeShareable(new FContentItemSpace::FContentItem());
	//MatStyleTempContentItem = MatStyle->GetCurrentItem();
	//if (MatStyleTempContentItem.IsValid())
	//{
	//	TSharedPtr<FJsonObject> TempJsonObj = FArmySceneData::Get()->GetJsonDataByItemID(MatStyleTempContentItem->ID);
	//	if (!TempJsonObj.IsValid())
	//	{
	//		ReqConstructionData(MatStyleTempContentItem->ID);
	//	}
	//}

	const TArray<TSharedPtr<FJsonValue>> ContentItemList1 = InJsonData->GetArrayField("ContentItemList");
	for (auto& ContentItem1 : ContentItemList1)
	{
		TSharedPtr<FJsonObject> JObject = ContentItem1->AsObject();
		TSharedPtr<FContentItemSpace::FContentItem> temp = MakeShareable(new FContentItemSpace::FContentItem());
		temp->Deserialization(JObject);
		ContentItemList.Add(temp);
		//TSharedPtr<FJsonObject> TempJsonObj = FArmySceneData::Get()->GetJsonDataByItemID(temp->ID);
		//if (!TempJsonObj.IsValid())
		//{
		//	ReqConstructionData(temp->ID);
		//}
	}

	const TSharedPtr<FJsonObject> * lampContentItemJson;

	if (InJsonData->TryGetObjectField(TEXT("LampContentItem"), lampContentItemJson))
	{
		LampContentItem = MakeShareable(new FContentItemSpace::FContentItem());
		LampContentItem->Deserialization(*lampContentItemJson);
	}

	const TArray<TSharedPtr<FJsonValue>> BuckleMap1 = InJsonData->GetArrayField("KouTiaoMap");
	if (BuckleMap1.Num() > 0)
	{
		SetbIsSingleBuckle(true);
	}
	for (auto& tempJson : BuckleMap1)
	{
		TSharedPtr<FJsonObject> JObject = tempJson->AsObject();
		FArmyAreaEdge tempedge(FVector(0, 0, 0), FVector(0, 0, 0));
		tempedge.Deserialization(JObject);
		TSharedPtr<FContentItemSpace::FContentItem> temp = MakeShareable(new FContentItemSpace::FContentItem());
		temp->Deserialization(JObject);
		BuckleMap.Add(tempedge, temp);
		//TSharedPtr<FJsonObject> TempJsonObj = FArmySceneData::Get()->GetJsonDataByItemID(temp->ID);
		//if (!TempJsonObj.IsValid())
		//{
		//	ReqConstructionData(temp->ID);
		//}
	}

	FString strParentGUID = InJsonData->GetStringField("parentGUID");
	FGuid::Parse(strParentGUID, ParentGUID);

	PlaneNormal.InitFromString(InJsonData->GetStringField("planeNormal"));
	PlaneOrignPos.InitFromString(InJsonData->GetStringField("planeOrignPos"));
	PlaneXDir.InitFromString(InJsonData->GetStringField("planeXDir"));
	PlaneYDir.InitFromString(InJsonData->GetStringField("planeYDir"));
	SurfaceType = (uint32)InJsonData->GetNumberField("SurfaceType");
	SurportPlaceArea = (uint32)InJsonData->GetNumberField("SurportPlaceArea");
	SkitLineID = InJsonData->GetNumberField("SkitLineGoodsID");

	GapColor.InitFromString(InJsonData->GetStringField("GapColor"));
	MI_BackGroundColor->SetVectorParameterValue("MainColor", GapColor);

	M_ExtrusionHeight = InJsonData->GetNumberField("M_ExtrusionHeight");
	LampSlotWidth = InJsonData->GetNumberField("M_LampSlotWidth");
	if (PlaneNormal.Equals(FVector(0, 0, 1), 0.01f))
		SurfaceType = 0;
	else if (PlaneNormal.Equals(FVector(0, 0, -1), 0.01f))
		SurfaceType = 2;
	else
		SurfaceType = 1;
	MatStyle->SetPlaneInfo(PlaneOrignPos, PlaneXDir, PlaneYDir);
	MatStyle->StyleDirty.BindRaw(this, &FArmyBaseArea::UpdateWallActor);

	/**@欧石楠 读取施工项*/
	TSharedPtr<FJsonObject> SubstrateObj = InJsonData->GetObjectField(TEXT("RoomConstructionItemData"));
	ConstructionItemData->Deserialization(SubstrateObj);
	if (!SubstrateObj.IsValid() || SubstrateObj->Values.Num() == 0) {
		TArray<FString> ConstructionStrArray;
		InJsonData->TryGetStringArrayField("constructionItem", ConstructionStrArray);
		for (auto& ArrayIt : ConstructionStrArray)
		{
			ConstructionItemData->CheckedId.Add(FCString::Atoi(*ArrayIt));
		}
		ConstructionItemData->bHasSetted = true;
	}

	//@郭子阳
	//读取1.5版本以下铺贴类的数据到施工项管理器
	if (GetStyle(M_InnearArea).IsValid() && GetStyle(M_InnearArea)->ConstructionItemData.IsValid() && GetStyle(M_InnearArea)->ConstructionItemData->bHasSetted && GetStyle(M_InnearArea)->HasGoodID())
	{
		ConstructionPatameters Parameter;
		Parameter.SetPaveInfo(GetStyle(M_InnearArea)->GetGoodsID(), (EWallType)SurportPlaceArea, GetStyle(M_InnearArea)->GetEditType(), false, EConstructionVersion::V140);
		XRConstructionManager::Get()->SaveConstructionData(GetUniqueID(), Parameter, GetStyle(M_InnearArea)->ConstructionItemData, true);
		XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), Parameter, nullptr);
	}

	// @欧石楠 读取起铺点相关数据
	const TSharedPtr<FJsonObject>* LayingPointObj = nullptr;
	InJsonData->TryGetObjectField("layingPoint", LayingPointObj);
	if (LayingPointObj)
	{
		FString LayingPointPosStr;
		if (InJsonData->TryGetStringField("pos", LayingPointPosStr))
		{
			LayingPointPos.InitFromString(LayingPointPosStr);
		}
	}

	if (SurfaceType == 0)
	{
		SurfaceType = 0;
	}
	
    // @欧石楠 加载数据生成踢脚线
    if (SurfaceType == 0)
    {
        const TSharedPtr<FJsonObject>* BaseboardObj = nullptr;
        if (InJsonData->TryGetObjectField("baseboard", BaseboardObj))
        {
            GetBaseboardExtruder()->Deserialization(*BaseboardObj);
        }
    }
    else if (SurfaceType == 2)
    {
        const TSharedPtr<FJsonObject>* CrownMouldingObj = nullptr;
        if (InJsonData->TryGetObjectField("crownMoulding", CrownMouldingObj))
        {
            GetCrownMouldingExtruder()->Deserialization(*CrownMouldingObj);
        }
    }

    // @欧石楠 放样类分段生成和删除功能针对老方案的兼容
    if (GetBaseboardExtruder()->IsEmpty() && GetCrownMouldingExtruder()->IsEmpty() && ContentItemList.Num() > 0)
    {
        TArray<FArmyExtrusionPath> Paths;

        TArray<TSharedPtr<FJsonValue> > skirtingLineInfoObj = InJsonData->GetArrayField("SkirtingLineInfo");
        for (auto & It : skirtingLineInfoObj)
        {
            TArray<FVector> Vertexes;
            const TArray< TSharedPtr<FJsonValue> > VertexesArray = It->AsObject()->GetArrayField("vertices");
            for (auto & VertexIt : VertexesArray)
            {
                FVector Vertex;
                Vertex.InitFromString(VertexIt->AsString());
                Vertexes.Add(Vertex);
            }

            bool bClosed = false;
            It->AsObject()->TryGetBoolField("bClosed", bClosed);

            Paths.Add(FArmyExtrusionPath(Vertexes, bClosed, ContentItemList[0]));
        }

        if (SurfaceType == 0)
        {
            GetBaseboardExtruder()->SetPaths(Paths);
        }
        else if (SurfaceType == 2)
        {
            GetCrownMouldingExtruder()->SetPaths(Paths);
        }
    }

    GetBaseboardExtruder()->Generate();
    GetCrownMouldingExtruder()->Generate();
    GenerateConstructionInfo();
}

void FArmyBaseArea::SetPlaneInfo(const FVector& XDir, const FVector& Ydir, const FVector& PlaneCenter)
{
	PlaneXDir = XDir;
	PlaneYDir = Ydir;
	PlaneOrignPos = PlaneCenter;
	PlaneNormal = XDir ^ Ydir;
}

void FArmyBaseArea::PostModify(bool bTransaction/* =false */)
{
	PositionChanged.Broadcast();
	if (ExtrudedxfFilePath.IsEmpty())
	{
		if (ExturesionActor)
		{
			ExturesionActor->Destroy();
			ExturesionActor = NULL;
		}
	}

    GetLampSlotExtruder()->Clear();
}

void FArmyBaseArea::DrawOutLine(class FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor Color, float thickness)
{
	int number = OutArea->Vertices.Num();
	float tempOffset = SurfaceType == 2 ? -2 : 2;
	for (int i = 0; i < number; ++i)
	{
		FVector start = OutArea->Vertices[i%number].X* PlaneXDir + OutArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos + PlaneNormal* tempOffset;
		FVector end = OutArea->Vertices[(i + 1) % number].X* PlaneXDir + OutArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos + PlaneNormal * tempOffset;

		PDI->DrawLine(start, end, Color, thickness);
	}
}

void FArmyBaseArea::DrawOutLineRecursion(class FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor NormalColor /*= FLinearColor::White*/, FLinearColor SelectedColor /*= FLinearColor::White*/, float thickness /*= 2.0f*/)
{
	if (GetState() != OS_Selected)
	{
		DrawOutLine(PDI, View, NormalColor, thickness);
	}
	else
	{
		DrawOutLine(PDI, View, SelectedColor, thickness);
	}

	for (auto iter0 : RoomEditAreas)
	{
		iter0->DrawOutLineRecursion(PDI, View, NormalColor, SelectedColor, thickness);
	}
}

void FArmyBaseArea::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	if (IsAbs)
	{
		int number = OutArea->Vertices.Num();
		for (int i = 0; i < number; ++i)
		{
			FVector start = OutArea->Vertices[i%number].X* PlaneXDir + OutArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
			FVector end = OutArea->Vertices[(i + 1) % number].X* PlaneXDir + OutArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos;

			OutLines.AddUnique(MakeShareable(new FArmyLine(start, end)));
		}
	}
}

void FArmyBaseArea::GetOutlineCenter(FVector& CenterPos) const
{
	TArray<FVector> Vertices;

	int number = OutArea->Vertices.Num();
	for (int i = 0; i < number; ++i)
	{
		FVector Position = OutArea->Vertices[i%number].X* PlaneXDir + OutArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
		Vertices.Emplace(Position);
	}

	CenterPos = FBox(Vertices).GetCenter();
}

void FArmyBaseArea::GetExtrusionLines(TMap<int32, TArray< TSharedPtr<FArmyLine> >>& ExtrusionLinesMap, ExtrusionType TypeOfExtrusion) const
{
	TArray< TSharedPtr<FArmyLine> >ExtrusionLines0;
	TArray< TSharedPtr<FArmyLine> >ExtrusionLines1;

	if (TypeOfExtrusion == 0 || TypeOfExtrusion == 1)
	{
		// 返回踢脚线顶角线内圈的线,现在有两圈，所以是数组第二个元素
		//如果有的线段数量超过两段，那么一定是不闭合的，不需要首尾相连
		if (ConstructionSkitLineVerts.Num() > 0)
		{
			if (ConstructionSkitLineVerts[1].Num() > 1)
			{
				for (int i = 0; i < ConstructionSkitLineVerts[1].Num(); i++)
				{
					for (int j = 0; j < ConstructionSkitLineVerts[1][i].Num() - 1; j++)
					{
						TSharedPtr<FArmyLine> ExtrusionLine0 = MakeShareable(new FArmyLine(ConstructionSkitLineVerts[0][i][j], ConstructionSkitLineVerts[0][i][j + 1]));
						ExtrusionLines0.Add(ExtrusionLine0);
						TSharedPtr<FArmyLine> ExtrusionLine1 = MakeShareable(new FArmyLine(ConstructionSkitLineVerts[1][i][j], ConstructionSkitLineVerts[1][i][j + 1]));
						ExtrusionLines1.Add(ExtrusionLine1);
					}
				}
			}
			//如果只有一段线。那有两种情况，闭合的或者只有一个洞
			else if (ConstructionSkitLineVerts[1].Num() == 1)
			{
				int32 Num = ConstructionSkitLineVerts[1][0].Num();
				for (int j = 0; j < Num - 1; j++)
				{
					TSharedPtr<FArmyLine> ExtrusionLine = MakeShareable(new FArmyLine(ConstructionSkitLineVerts[0][0][j], ConstructionSkitLineVerts[0][0][j + 1]));
					ExtrusionLines0.Add(ExtrusionLine);
					TSharedPtr<FArmyLine> ExtrusionLine1 = MakeShareable(new FArmyLine(ConstructionSkitLineVerts[1][0][j], ConstructionSkitLineVerts[1][0][j + 1]));
					ExtrusionLines1.Add(ExtrusionLine1);
				}
				//如果是闭合的，需要首尾相连
				if (RoomDataStatus & FLAG_CLOSESKITLINE)
				{
					TSharedPtr<FArmyLine> ExtrusionLine = MakeShareable(new FArmyLine(ConstructionSkitLineVerts[0][0][Num - 1], ConstructionSkitLineVerts[0][0][0]));
					ExtrusionLines0.Add(ExtrusionLine);
					TSharedPtr<FArmyLine> ExtrusionLine1 = MakeShareable(new FArmyLine(ConstructionSkitLineVerts[1][0][Num - 1], ConstructionSkitLineVerts[1][0][0]));
					ExtrusionLines1.Add(ExtrusionLine1);
				}
			}
			else
			{
				ExtrusionLines0.Empty();
				ExtrusionLines1.Empty();
			}
			ExtrusionLinesMap.Add(0, ExtrusionLines0);//顶角线与面重合层
			ExtrusionLinesMap.Add(1, ExtrusionLines1);//顶角线内层
		}
		else
		{
			ExtrusionLines0.Empty();
			ExtrusionLines1.Empty();
		}
	}
}

void FArmyBaseArea::SetExtrusionHeight(float InHeight)
{
	M_ExtrusionHeight = InHeight;

	//@郭子阳 递归的刷新子面
	std::function< void(TSharedPtr<FArmyBaseArea>)>
		FreashArea = [&FreashArea](TSharedPtr<FArmyBaseArea> InArea)
	{
		for (TSharedPtr<FArmyBaseArea> iter : InArea->GetEditAreas())
		{
			FreashArea(iter);
		}
		InArea->RefreshExtrusionPlane();
	};
	FreashArea(StaticCastSharedRef<FArmyBaseArea>(this->AsShared()));

    // @欧石楠 重新设置顶角线离地高度
    if (SurfaceType == 2)
    {
        GetCrownMouldingExtruder()->SetDistanceToFloor(FArmySceneData::WallHeight - InHeight);
    }
}

void FArmyBaseArea::GenerateExtrudeActor(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem)
{
    if (!ContentItem.IsValid() || !ContentItem->HasResources())
    {
        return;
    }

    if (SurfaceType == 0)
    {
        GetBaseboardExtruder()->Clear();
        CalculateBaseboardPaths(GetBaseboardExtruder()->GetPaths(), ContentItem);
        GetBaseboardExtruder()->Generate();
    }
    else if (SurfaceType == 2)
    {
        GetCrownMouldingExtruder()->Clear();
        CalculateCrownMouldingPaths(GetCrownMouldingExtruder()->GetPaths(), ContentItem);
        GetCrownMouldingExtruder()->Generate();
    }

    GenerateConstructionInfo();
}

bool FArmyBaseArea::bCanGenerateLampSlot(float LampSlotWidth)
{

	/** 如果没有这三个必要信息，不许放置灯槽*/
	if (!OutArea.IsValid() || !ParentGUID.IsValid())
	{
		//GGI->Window->ShowMessage(MT_Warning, TEXT("Lack of OutArea Or ParentGUID!"));
		return false;
	}
	/** 放样后的边界点*/
	TArray<FVector> OutList = FArmyMath::ExturdePolygon(OutArea->Vertices, (LampSlotWidth + 0.2f), false);

	/** 判断灯槽是否超出房间*/
	TWeakPtr<FArmyObject> ParentObjectWeak = FArmySceneData::Get()->GetObjectByGuidAlone(ParentGUID);
	TWeakPtr<FArmyBaseArea> ParentArea = StaticCastSharedPtr<FArmyBaseArea>(ParentObjectWeak.Pin());
	TArray<FVector> ParentOutList;//房间区域
	ParentOutList = ParentArea.Pin()->OutArea->Vertices;
	if (!FArmyMath::IsPolygonInPolygon(OutList, ParentOutList))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("灯槽已超出房间，请修改区域！"));
		return false;
	}


	/** 判断灯槽是否与其他区域相交，梁柱风道*/
	TWeakPtr<FArmyRoomSpaceArea> ParentRoomSpaceArea = StaticCastSharedPtr<FArmyRoomSpaceArea>(ParentObjectWeak.Pin());
	if (!ParentRoomSpaceArea.Pin()->TestAreaCanAddOrNot(OutList, GetUniqueID()))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("灯槽与空间其他区域相交！"));
		return false;
	}

	/** 判断灯槽是否与其他区域相交*/
	TArray<FObjectWeakPtr> BaseAreaArray;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, BaseAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_CircleArea, BaseAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_PolygonArea, BaseAreaArray);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_FreePolygonArea, BaseAreaArray);
	for (auto iter : BaseAreaArray)
	{
		TSharedPtr<FArmyBaseArea> Area = StaticCastSharedPtr<FArmyBaseArea>(iter.Pin());
		if (Area->GetUniqueID() != GetUniqueID() && Area->GetParentGuid() == ParentGUID)//与当前区域同一顶面的其他区域
		{
			TArray<FVector> AreaOutList = Area->OutArea->Vertices;
			if (Area->GetLampSlotExtruder()->IsEmpty())//其他区域没有灯槽，直接判断相交
			{
				if (FArmyMath::ArePolysOverlap(OutList, AreaOutList))
				{
					GGI->Window->ShowMessage(MT_Warning, TEXT("与其他区域重叠，请修改区域！"));
					return false;
				}
			}
			else//其他区域没有灯槽，先计算灯槽，再判断相交
			{
				AreaOutList = FArmyMath::ExturdePolygon(AreaOutList, (Area->GetLampSlotWidth() + 0.2f), false);
				if (FArmyMath::ArePolysOverlap(OutList, AreaOutList))
				{
					GGI->Window->ShowMessage(MT_Warning, TEXT("与其他区域重叠，请修改区域！"));
					return false;
				}
			}
		}
	}

	return true;
}

void FArmyBaseArea::CalculateLampSlotPaths(
    TArray<FArmyExtrusionPath>& OutPaths,
    TArray<FVector>& OutVertexes,
    TSharedPtr<FContentItemSpace::FContentItem> ContentItem,
    float InlampSlotWidth)
{
    LampSlotWidth = InlampSlotWidth;
    OutVertexes = OutArea->Vertices;
    if (FArmyMath::IsClockWise(OutVertexes))
    {
        FArmyMath::ReversePointList(OutVertexes);
    }
    //OutVertexes = FArmyMath::ExturdePolygon(OutVertexes, (InlampSlotWidth + 0.2f), false);
    for (auto& iter : OutVertexes)
    {
        iter = iter.X * PlaneXDir + iter.Y * PlaneYDir + PlaneOrignPos + PlaneNormal * (M_ExtrusionHeight + FMath::Max(m_deltaHeight, 0.f) - 0.01f);
    }

    OutPaths.Add(FArmyExtrusionPath(OutVertexes, true, ContentItem));
}

void FArmyBaseArea::GenerateWallMoldingLine(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem)
{
	if (!ContentItem.IsValid())
		return;
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	ContentItemList.Empty();
	ContentItemList.Add(ContentItem);
	FString InPath;
	for (auto& iter : resArr)
	{
		if (iter->ResourceType == HardDXF)
		{
			InPath = iter->FilePath;
			break;
		}
	}

	if (!FPaths::FileExists(InPath))
		return;
	TArray<FVector> OutList = OutArea->Vertices;
	if (!FArmyMath::IsClockWise(OutList))
		FArmyMath::ReversePointList(OutList);
	TArray<float> deltas;
	if (ExturesionActor == NULL)
	{
		ExturesionActor = InWorld->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		ExturesionActor->MeshComponent->Rename(TEXT("MoldingLineComponent"));
		//ExturesionActor->MeshComponent->SetLightmapResolution(FArmySceneData::Get()->GetMoldingLineStaticLightingRes());
		ExturesionActor->MeshComponent->bBuildStaticLighting = true;
		ExturesionActor->Tags.Add(TEXT("Immovable"));
		ExturesionActor->Tags.Add(TEXT("WallMoldingLine"));
		ExturesionActor->SetFolderPath(FArmyActorPath::GetWallPath());
		ExturesionActor->AttachSurfaceArea = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
	}
	FString UniqueCodeString = TEXT("Extrusion") + GetUniqueID().ToString();
	ExturesionActor->UniqueCodeExtrusion = UniqueCodeString;
	TArray<AArmyExtrusionActor*> ExturesionActorArray;
	ExturesionActorArray.Add(ExturesionActor);

	ExturesionActor->GenerateMeshFromFilePath(InPath, OutList, false, true, false, PlaneXDir, PlaneYDir, PlaneNormal, PlaneOrignPos);
	ExturesionActor->SetSynID(ContentItem->ID);
	ExturesionActor->dxfName = ContentItem->Name;
	ExturesionActor->SetActorLabel(ContentItem->Name);
	ExturesionActor->dxfLength = ContentItem->ProObj->Length;
	ExturesionActor->dxfWidth = ContentItem->ProObj->Width;
	ExturesionActor->dxfHeight = ContentItem->ProObj->Height;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	UMaterialInterface* MI = ResMgr->CreateContentItemMaterial(ContentItem);
	UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(MI);
	if (MID)
		ExturesionActor->SetMaterial(MID);

	ExtrudedxfFilePath = InPath;
	ExturdedxfMaterialFilePath = resArr[1]->FilePath;//梁晓菲 放样类的材质路径是第二个元素存储的路径

													 
	//@郭子阳 转换老方案(V1.9之前)施工项
	TSharedPtr<FArmyConstructionItemInterface> temp = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(UniqueCodeString);//加载
	if (temp.IsValid())
	{
		XRConstructionManager::Get()->SaveConstructionData(GetUniqueID()
			, ExturesionActor->GetConstructionParameter(), temp);

		FArmySceneData::Get()->DeleteExtrusionConstructionItemData(UniqueCodeString);
	}
	//@郭子阳 获取施工项
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), ExturesionActor->GetConstructionParameter(), nullptr);
	
	/*if (!temp.IsValid())
	{
		FArmySceneData::Get()->AddExtrusionConstructionData(UniqueCodeString, MakeShareable(new FArmyConstructionItemInterface));
	}*/
	 
}

void FArmyBaseArea::GenerateBuckleActorLine(UWorld* InWorld, TSharedPtr<FContentItemSpace::FContentItem> ContentItem)
{
	if (!ContentItem.IsValid())
		return;
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;

	FString InPath;
	for (auto& iter : resArr)
	{
		if (iter->ResourceType == HardDXF)
		{
			InPath = iter->FilePath;
			break;
		}
	}
	if (!FPaths::FileExists(InPath))
		return;

	/* 扣条为单边放置*/
	if (bIsSingleBuckle && VectorCode != -1)
	{
		int32 Num = OutArea->Vertices.Num();
		FVector HoverEdgeBuckleStart = OutArea->Vertices[VectorCode % Num].X* PlaneXDir + OutArea->Vertices[VectorCode % Num].Y * PlaneYDir;
		FVector HoverEdgeBuckleEnd = OutArea->Vertices[(VectorCode + 1) % Num].X* PlaneXDir + OutArea->Vertices[(VectorCode + 1) % Num].Y * PlaneYDir;
		if (!CheckEdgeCouldPlaceBuckle(HoverEdgeBuckleStart, HoverEdgeBuckleEnd))
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("此方向不支持放置扣条！"));
			return;
		}
		class AArmyExtrusionActor* BuckleActor = InWorld->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		FString UniqueCodeString = TEXT("Buckle") + FString::Printf(TEXT("%d"), VectorCode) + GetUniqueID().ToString();
		BuckleActor->UniqueCodeExtrusion = UniqueCodeString;
		TArray<AArmyExtrusionActor*> BuckleActorArray;
		BuckleActorArray.Add(BuckleActor);
		//TSharedPtr<FArmyConstructionItemInterface> temp = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(UniqueCodeString);//加载
		//if (!temp.IsValid())
		//{
		//	FArmySceneData::Get()->AddExtrusionConstructionData(UniqueCodeString, MakeShareable(new FArmyConstructionItemInterface));
		//}
		BuckleActor->MeshComponent->bBuildStaticLighting = false;
		BuckleActor->Tags.Add(TEXT("Immovable"));
		BuckleActor->SetFolderPath(FArmyActorPath::GetFloorPath());
		BuckleActor->AttachSurfaceArea = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
		BuckleActor->dxfName = ContentItem->Name;
		BuckleActor->SetActorLabel(ContentItem->Name);
		BuckleActor->dxfLength = ContentItem->ProObj->Length;
		BuckleActor->dxfWidth = ContentItem->ProObj->Width;
		BuckleActor->dxfHeight = ContentItem->ProObj->Height;
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface* MI = ResMgr->CreateContentItemMaterial(ContentItem);
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(MI);
		if (MID)
			BuckleActor->SetMaterial(MID);
		BuckleActor->GenerateMeshFromFilePath(InPath, { HoverEdgeBuckleStart,HoverEdgeBuckleEnd }, false, false, false, PlaneXDir, PlaneYDir, PlaneNormal, PlaneOrignPos);
		BuckleActor->SetSynID(ContentItem->ID);

		//@郭子阳 转换老方案(V1.9之前)施工项
		TSharedPtr<FArmyConstructionItemInterface> temp = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(UniqueCodeString);//加载
		if (temp.IsValid())
		{
			XRConstructionManager::Get()->SaveConstructionData(GetUniqueID()
				, BuckleActor->GetConstructionParameter(), temp);

			FArmySceneData::Get()->DeleteExtrusionConstructionItemData(UniqueCodeString);
		}
		//@郭子阳 获取施工项
		XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), BuckleActor->GetConstructionParameter(), nullptr);

		FArmyAreaEdge BuckleEdge(OutArea->Vertices[VectorCode % Num], OutArea->Vertices[(VectorCode + 1) % Num]);
		if (BuckleMap.Contains(BuckleEdge))
		{
			BuckleMap[BuckleEdge] = ContentItem;
			BuckleActorMap[BuckleEdge] = BuckleActor;
			int32 tempNum = BuckleEdges.Num();
			for (int32 i = 0; i < tempNum; i++)
			{
				if (BuckleEdges[i] == BuckleEdge)
				{
					if (BuckleActors[i] != nullptr)
						BuckleActors[i]->Destroy();
					BuckleActors[i] = BuckleActor;
					break;
				}
			}
		}
		else
		{
			BuckleEdges.Add(BuckleEdge);
			BuckleActors.Add(BuckleActor);
			BuckleMap.Emplace(BuckleEdge, ContentItem);
			BuckleActorMap.Add(BuckleEdge, BuckleActor);
		}
	}
	else
	{
		/* 扣条为整圈放置,V1.2不涉及*/
		GGI->Window->ShowMessage(MT_Warning, TEXT("请选择您要放置扣条的边！"));
	}
}
void FArmyBaseArea::SetVectorCode(int32 vectorCode)
{
	VectorCode = vectorCode;
}
TSharedPtr<FContentItemSpace::FContentItem> FArmyBaseArea::GetEdgeVectorCodeCloseToPos(TSharedPtr<FArmyBaseArea> FloorSpace, int32 & vectorCode)
{
	float dis = FLT_MAX;
	FVector startPos;
	FVector endPos;
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = nullptr;

	int number = OutArea->Vertices.Num();
	int num = FloorSpace->GetOutArea()->Vertices.Num();

	for (int i = 0; i < number; ++i)
	{
		FVector start = OutArea->Vertices[i%number].X * PlaneXDir + OutArea->Vertices[i%number].Y * PlaneYDir + PlaneOrignPos;
		FVector end = OutArea->Vertices[(i + 1) % number].X * PlaneXDir + OutArea->Vertices[(i + 1) % number].Y * PlaneYDir + PlaneOrignPos;
		FVector lCenter = FVector((start + end) * 0.5f);

		for (int j = 0; j < num; j++)
		{
			FVector FloorStart =
				FloorSpace->GetOutArea()->Vertices[j%num].X * FloorSpace->GetXDir() +
				FloorSpace->GetOutArea()->Vertices[j%num].Y * FloorSpace->GetYDir() + FloorSpace->GetPlaneCenter();
			FVector FloorEnd =
				FloorSpace->GetOutArea()->Vertices[(j + 1) % num].X * FloorSpace->GetXDir() +
				FloorSpace->GetOutArea()->Vertices[(j + 1) % num].Y * FloorSpace->GetYDir() + FloorSpace->GetPlaneCenter();
			if (FMath::PointDistToSegment(lCenter, FloorStart, FloorEnd) < dis)
			{
				dis = FMath::PointDistToSegment(lCenter, FloorStart, FloorEnd);
				vectorCode = i;
			}
		}
	}
	if (vectorCode != -1)
	{
		FArmyAreaEdge BuckleEdge(OutArea->Vertices[vectorCode % number], OutArea->Vertices[(vectorCode + 1) % number]);
		if (BuckleMap.Contains(BuckleEdge))
		{
			ContentItem = BuckleMap.FindRef(BuckleEdge);
		}
	}
	return ContentItem;
}

bool FArmyBaseArea::CheckEdgeCouldPlaceBuckle(const FVector& startEdge, const FVector& endEdge)
{
	return false;
}

bool FArmyBaseArea::BHoverSingleEdge(const FVector2D& Pos, class UArmyEditorViewportClient* ViewPortClient, FVector& OutStart, FVector& OutEnd)
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

		if (dist < 16)
		{
			OutStart = start;
			OutEnd = end;
			VectorCode = i;
			return true;
		}
	}
	return false;
}

void FArmyBaseArea::SetbIsSingleBuckle(bool BIsSingle)
{
	bHasBuckle = true;
	bIsSingleBuckle = BIsSingle;
}

void FArmyBaseArea::RemoveBuckleLine(class AArmyExtrusionActor* BuckleActor)
{
	if (BuckleActor)
	{
		int32 Num = BuckleActors.Num();
		for (int i = 0; i < Num; i++)
		{
			if (BuckleActors[i] != nullptr && BuckleActors[i] == BuckleActor)
			{
				BuckleMap.Remove(BuckleEdges[i]);
				BuckleActorMap.Remove(BuckleEdges[i]);
				BuckleActors.RemoveAt(i);
				BuckleEdges.RemoveAt(i);
				FArmySceneData::Get()->DeleteExtrusionConstructionItemData(BuckleActor->UniqueCodeExtrusion);
				break;
			}
		}
	}
}

void FArmyBaseArea::RemoveSkitLineActor(AArmyExtrusionActor* skitActor)
{
    if (SurfaceType == 0)
    {
        GetBaseboardExtruder()->DeleteActor(skitActor);
    }
    else if (SurfaceType == 2)
    {
        GetCrownMouldingExtruder()->DeleteActor(skitActor);
    }

    GenerateConstructionInfo();
}

void FArmyBaseArea::RemoveWallModingLine()
{
	if (ExturesionActor)
	{
		ExturesionActor->Destroy();
		ExturesionActor = NULL;
		ConstructionSkitLineVerts.Empty();
		SkitLineID = -1;
		ContentItemList.Empty();
	}
}

void FArmyBaseArea::RemoveLampSlot(AArmyExtrusionActor* InLampSlotActor)
{
	//if (LampSlotActor)
	//{
	//	LampSlotActor->Destroy();
	//	LampSlotActor = NULL;
	//}
}

float FArmyBaseArea::GetAreaDimension() const 
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
		holeArea += iter0->GetAreaDimension();
	}
	float area = FArmyMath::CalcPolyArea(OutArea->Vertices) / 10000.0f - holeArea;
	return area;
}
float FArmyBaseArea::GetAreaDimensionWithVisibility()
{
	return GetAreaDimension();
}
float FArmyBaseArea::GetAreaDimensionWhithoutHoles() const
{
	return FArmyMath::CalcPolyArea(OutArea->Vertices) / 10000.0f;
}

float FArmyBaseArea::GetAreaCircumference() const
{
	return FArmyMath::CircumferenceOfPointList(OutArea->Vertices, true);
}

float FArmyBaseArea::GetAreaLength() const
{
	float length = 0;
	int number = OutArea->Vertices.Num();
	for (int i = 0; i < number; ++i)
	{
		FVector start = OutArea->Vertices[i%number];
		FVector end = OutArea->Vertices[(i + 1) % number];

		length += (end - start).Size();
	}
	return length / 100.0f;
}

void FArmyBaseArea::Destroy()
{
	DestroyAttachActor();
	if (WallActor)
	{
		WallActor->SetActorHiddenInGame(true);
		WallActor->Destroy();
		WallActor = NULL;
	}

	for (auto& SubArea : RoomEditAreas)
	{
		SubArea->Destroy();
	}
}

void FArmyBaseArea::RecaculateRelatedArea()
{
	TArray<FVector> tempOutVertices;
	TArray<TArray<FVector>> Holes;
	CalculateOutAndInnerHoles(tempOutVertices, Holes);
	MatStyle->SetDrawArea(InneraArea->Vertices, Holes);
	CalculateWallActor(MatStyle);

	PositionChanged.Broadcast();
    RefreshPolyVertices();

	if (SurfaceType == 1)
	{
		if (!ExtrudedxfFilePath.IsEmpty() && ContentItemList.Num() > 0)
		{
			FArmyBaseArea::GenerateWallMoldingLine(GVC->GetWorld(), ContentItemList[0]);
		}
	}
	/* @梁晓菲 无论高度差是多少，面都应该进行更新*/
	RefreshExtrusionPlane();

	//@郭子阳 刷新子面
	for (auto & SubArea : RoomEditAreas)
	{
		SubArea->RecaculateRelatedArea();
	}
}

void FArmyBaseArea::GetConstructionAreas(TArray<FArmyGoods>& ArtificialData)
{
	GetCommonConstruction(ArtificialData);

    TMap<AArmyExtrusionActor*, FArmyExtrusionPath> ActorMap;
    if (SurfaceType == 0)
    {
        ActorMap = GetBaseboardExtruder()->GetActorMap();
    }
    else if (SurfaceType == 2)
    {
        ActorMap = GetCrownMouldingExtruder()->GetActorMap();
    }

    //if (ActorMap.Num() <= 0)
    //{
    //    return;
    //}

    for (auto& It : ActorMap)
    {
        AArmyExtrusionActor* ExturesionActor = It.Key;

        //闭合放样类 （踢脚线 顶角线等）
        FArmyGoods goods;
        goods.GoodsId = SkitLineID;
        goods.Type = 1;

        // 施工项
        TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), ExturesionActor->GetConstructionParameter());
			//= FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(ExturesionActor->UniqueCodeExtrusion);
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
        int tempC = ExturesionActor->Circumference / 100.0f * 100;
        Goods.Param.D += tempC / 100.0f;
        Goods.Param.L += ExturesionActor->Circumference / 100.0f;
        Goods.Param.C += ExturesionActor->Circumference / 100.0f;
        Goods.Param.H = M_ExtrusionHeight;
    }

    // @欧石楠 灯槽施工项
	if (!GetLampSlotExtruder()->IsEmpty() && GetLampSlotExtruder()->GetSaleID() != -1)
	{
		FArmyGoods goods;
		goods.GoodsId = SkitLineID;
		goods.Type = 1;

		// 施工项
		ConstructionPatameters P;
		P.SetNormalGoodsInfo(GetLampSlotExtruder()->GetSaleID(), GetRoomSpaceID());
		TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), P);
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

		int32 Circumference = GetLampSlotExtruder()->GetCircumference();
		Goods.Param.D += Circumference / 100.0f;
		Goods.Param.L += Circumference / 100.0f;
		Goods.Param.C += Circumference / 100.0f;
		Goods.Param.H = M_ExtrusionHeight;
		
	}
	//原始墙面施工项
	GetConstructionData(ArtificialData);

	for (auto& iter0 : GetEditAreas())
	{
		iter0->GetConstructionAreas(ArtificialData);
	}
}

void FArmyBaseArea::GetConstructionData(TArray<FArmyGoods>& ArtificialData)
{
	if (!ConstructionItemData.IsValid())
	{
		return;
	}

	FArmyGoods goods;
	goods.GoodsId = 0;
	goods.Type = 4;
	//goods.PaveID = ConvertStyleToPavingID(MatStyle);

	// 施工项
	ConstructionPatameters Parameter;
	EWallType wallType = (EWallType)SurportPlaceArea;
	Parameter.SetOriginalSurface(wallType, GetExtrusionHeight() != 0, GetRoomSpaceID());
	TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(GetUniqueID(), Parameter);
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

	Goods.Param.S += GetAreaDimension(); //GetAreaDimensionWhithoutHoles();
}
ConstructionPatameters  FArmyBaseArea::GetMatConstructionParameter()
{
	// 施工项参数
	ConstructionPatameters Parameter;
	
	//空间ID
	int32 SpaceID = -1;
	auto Room = GetRoom();
	if (Room.IsValid())
	{
		SpaceID = Room->GetSpaceId();
	}

	Parameter.SetPaveInfo2(GetStyle(M_InnearArea), (EWallType)SurportPlaceArea, SpaceID);
	return Parameter;
}

TSharedPtr<FArmyRoom> FArmyBaseArea::GetRoom()
{
	auto ParentArea = GetParentArea();
	if (ParentArea.IsValid())
	{
		return 	ParentArea->GetRoom();
	}
	return nullptr;
}

void FArmyBaseArea::OnRoomSpaceIDChanged(int32 NewSpaceID)
{
	//原始面施工项
	ConstructionPatameters Parameter;
	EWallType wallType = (EWallType)SurportPlaceArea;
	Parameter.SetOriginalSurface(wallType, GetExtrusionHeight() != 0,GetRoomSpaceID());
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), Parameter,nullptr);
	//面上铺贴的材质的施工项
	XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), GetMatConstructionParameter(), nullptr);
	//不管放样类，放样类自己管自己
}


void FArmyBaseArea::SetGapColor(const FLinearColor& InColor)
{
	if (GapColor != InColor)
	{
		GapColor = InColor;
		MI_BackGroundColor->SetVectorParameterValue("MainColor", GapColor);
		if (WallActor)
		{
			WallActor->SetBrickMaterial(MI_BackGroundColor);
		}

		if (MatStyle.IsValid())
		{
			MatStyle->DefaultGapColor = false;
			MatStyle->GapColor = GapColor;
		}
	}

}

void FArmyBaseArea::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyObject::SetPropertyFlag(InType, InUse);
	if (ExturesionActor)
	{
		ExturesionActor->SetActorHiddenInGame(!InUse);
	}
	if (LampSlotActor)
	{
		LampSlotActor->SetActorHiddenInGame(!InUse);
	}
}

void FArmyBaseArea::SetActorVisible(bool InVisible)
{
	if (ExturesionActor)
	{
		ExturesionActor->SetActorHiddenInGame(!InVisible);
	}
	if (LampSlotActor)
	{
		LampSlotActor->SetActorHiddenInGame(!InVisible);
	}
	if (WallActor)
	{
		WallActor->SetActorHiddenInGame(!InVisible);
	}
	if (BodaSurfaceArea.IsValid())
	{
		if (BodaSurfaceArea->GetBodaWallActor())
			BodaSurfaceArea->GetBodaWallActor()->SetActorHiddenInGame(!InVisible);
	}

	int number = RoomEditAreas.Num();
	for (int i = 0; i < number; ++i)
	{
		RoomEditAreas[i]->SetActorVisible(InVisible);
	}
}

TArray<FVector> FArmyBaseArea::GetLampLightDashLines()
{
    TArray<FVector> Result;

    TSparseArray<FArmyLightPath> LightConstructionInfo = GetLampSlotExtruder()->GetLightConstructionInfo();
    for (const auto& It : LightConstructionInfo)
    {
        Result.AddUnique(It.StartPos);
        Result.AddUnique(It.EndPos);
    }

    return Result;
}

void FArmyBaseArea::CaculateConstructionSkitLineInfo(const TArray<TArray<FVector>>& SkintLines, const TArray<float>& SkitLineDelta)
{
    if (SkintLines.Num() == 0)
    {
        return;
    }

	ConstructionSkitLineVerts.Empty();

    if (RoomDataStatus & FLAG_CLOSESKITLINE)
    {
        const TArray<FVector>& HoleCloseLineInfo = SkintLines[0];
        for (auto& iter : SkitLineDelta)
        {
            TArray<FVector> result = FArmyMath::Extrude3D(HoleCloseLineInfo, iter);
            TArray<TArray<FVector>> tempSkit = { result };
            ConstructionSkitLineVerts.Push(tempSkit);
        }
    }
    else
    {
        int number = OutArea->Vertices.Num();
        TArray<FVector> boundaryVerts;
        boundaryVerts.AddUninitialized(number);
        for (int i = 0; i < number; i++)
        {
            boundaryVerts[i] = OutArea->Vertices[i].X * PlaneXDir + OutArea->Vertices[i].Y * PlaneYDir + PlaneOrignPos;
        }
        for (float iter0 : SkitLineDelta)
        {
            TArray<TArray<FVector>> tempNewLines;
            for (const TArray<FVector>& iter : SkintLines)
            {
                TArray<FVector> extrusionVerts = FArmyMath::Extrude3dPolygon(iter, iter0, boundaryVerts);
                tempNewLines.Push(extrusionVerts);
            }
            ConstructionSkitLineVerts.Push(tempNewLines);
        }
    }
}

void FArmyBaseArea::ConvertRelativeToWorldPos(TArray<FVector>& InPoints)
{
	for (auto& iter : InPoints)
	{
		iter = iter.X * PlaneXDir + iter.Y * PlaneYDir + PlaneOrignPos + PlaneNormal * (M_ExtrusionHeight == 0 ? 0.2f : M_ExtrusionHeight);
	}
}
#define  SCALE2 100.0f
TArray<TSharedPtr<AreaCompare>> FArmyBaseArea::CombineOutAreaAndInnearHole(const TArray<FVector>& OutArea, const TArray<TArray<FVector>>& Holes)
{
	Clipper c;
	Paths ClipperAreas;
	Paths HolePaths;
	Paths ClipperSolution;
	int holeNumbers = Holes.Num();
	ClipperAreas.resize(1);
	for (int i = 0; i < OutArea.Num(); i++)
	{
		cInt x = (cInt)(OutArea[i].X * SCALE2);
		cInt y = (cInt)(OutArea[i].Y * SCALE2);
		ClipperAreas[0].push_back(IntPoint(x, y));
	}
	for (int i = 0; i < holeNumbers; i++)
	{
		Path holePath;
		for (int j = 0; j < Holes[i].Num(); j++)
		{
			cInt x = (cInt)(Holes[i][j].X * SCALE2);
			cInt y = (cInt)(Holes[i][j].Y * SCALE2);
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
		const Path& tempPath = ClipperSolution[i];
		TArray<FVector> tempOutVertices;
		for (int j = 0; j < tempPath.size(); ++j)
		{
			IntPoint point = tempPath[j];
			FVector x = point.X / SCALE2 * PlaneXDir;
			FVector y = point.Y / SCALE2*PlaneYDir;
			tempOutVertices.Push(FVector(point.X / SCALE2, point.Y / SCALE2, 0.0f));
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

bool FArmyBaseArea::TestAreaCanAddOrNot(const TArray<FVector>& InTestArea, FGuid ObjId, TSharedPtr<FArmyBaseArea> OutParent)
{
	OutParent = nullptr;
	TArray<FVector> testVertices = InTestArea;
	FVector testPoint = Translate::CaculatPlaneCenter(InTestArea);
	float testAreaDim = FArmyMath::CalcPolyArea(testVertices);
	float outAreaDim = FArmyMath::CalcPolyArea(OutArea->Vertices);


	if (!Translate::ArePolygonInPolygon(OutArea->Vertices, InTestArea))
		return false;

	//先和面上的绘制区域做相交判断 @郭子阳注
	for (auto iter : RoomEditAreas)
	{
		if (iter->GetUniqueID() != ObjId)
		{
			/** @梁晓菲 判断灯槽是否存在，如果存在，需要用灯槽边界点确认是否可以添加在新位置*/
			TArray<FVector> tempOutVertices = iter->GetOutArea()->Vertices;
            if (!iter->GetLampSlotExtruder()->IsEmpty())
            {
				tempOutVertices = FArmyMath::ExturdePolygon(tempOutVertices, (iter->GetLampSlotWidth() + 0.2f), false);
            }

			FVector centerPos = Translate::CaculatPlaneCenter(tempOutVertices);

			//所有点都在原有面内部
			bool AllPointIn = false;
			//所有点都在原有面内部
			bool AllPointOut = false;

			//@郭子阳 判断是不是完全在内部
			for (auto & Point : InTestArea)
			{
				bool Inside = FArmyMath::IsPointInOrOnPolygon2D(Point, tempOutVertices);

				if ((AllPointOut && Inside)
					|| (AllPointIn && !Inside)
					)
				{
					AllPointIn = false;
					AllPointOut = false;
					break;
				}
				AllPointIn = Inside;
				AllPointOut = !Inside;
			}

			if (!AllPointOut && !AllPointIn)
			{
				return false;
			}

			//判断有没有和内部区域交叉
			if (FArmyMath::ArePolysIntersection(tempOutVertices, testVertices, false))
			{
				return false;
			}


			if (AllPointIn)
			{
				if (TestAreaCanAddOrNot(tempOutVertices, iter->GetUniqueID(), OutParent))
				{

					OutParent = iter;
					return true;
				}
			}

		}
	}
	for (auto iterHole : SurfaceHoles)
	{
		FVector centerPos = Translate::CaculatPlaneCenter(iterHole);
		if (FArmyMath::IsPointInOrOnPolygon2D(testPoint, iterHole) || FArmyMath::IsPointInOrOnPolygon2D(centerPos, testVertices))
			return false;
		if (FArmyMath::ArePolysIntersection(InTestArea, iterHole))
			return false;
	}

	if (!OutParent.IsValid())
	{
		OutParent = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
	}
	return true;
}

void FArmyBaseArea::AddArea(TSharedPtr<FArmyBaseArea> InArea)
{
	InArea->PositionChangedHandles = InArea->PositionChanged.AddRaw(this, &FArmyBaseArea::RefreshPolyVertices);
	InArea->ParentGUID = this->GetUniqueID();

	RoomEditAreas.Add(InArea);
	RefreshPolyVertices();
	RoomDataStatus &= ~FLAG_DEFAULTSTYLE;
}

void FArmyBaseArea::MoveChildAreaToParent()
{
	auto ParentArea = GetParentArea();
	if (!ParentArea.IsValid())
	{
		//没有父面 不转移
		return;
	}

	auto TempSubAreas = RoomEditAreas;
	for (auto& SubArea : TempSubAreas)
	{
		ParentArea->AddChild(SubArea);

		SubArea->RemoveFromParents(AsShared());

		RemoveArea(SubArea);
		ParentArea->AddArea(SubArea);

		SubArea->ParentGUID = ParentArea->GetUniqueID();
	}
	RefreshPolyVertices();
	ParentArea->RefreshPolyVertices();
}



void FArmyBaseArea::RemoveArea(TSharedPtr<FArmyBaseArea> InArea)
{
	auto SubAreaIndex = RoomEditAreas.Find(InArea);
	if (InArea.IsValid() && InArea->PositionChangedHandles.IsValid())
	{

		InArea->PositionChanged.Remove(InArea->PositionChangedHandles); \
			InArea->PositionChangedHandles.Reset();
	}

    //InArea->Destroy();
	RoomEditAreas.Remove(InArea);
	InArea->ParentGUID = FGuid::NewGuid();
	RefreshPolyVertices();
}

TSharedPtr<FArmyBaseArea> FArmyBaseArea::GetParentArea()
{
	if (GetParents().Num() != 0)
	{
		TWeakPtr<FArmyObject> parent = GetParents()[0];
		if (!parent.IsValid())
		{
			return nullptr;
		}
		FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
		return StaticCastSharedRef<FArmyBaseArea>(temp->AsShared());
	}
	return nullptr;
}

void FArmyBaseArea::SetParentArea(TSharedPtr<FArmyBaseArea> NewParent)
{

	if (!NewParent.IsValid())
	{
		return;
	}

	auto ParentArea = GetParentArea();
	if (ParentArea.IsValid())
	{
		RemoveFromParents(ParentArea);
		ParentArea->RoomEditAreas.Remove(StaticCastSharedRef<FArmyBaseArea>(AsShared()));
	}

	NewParent->AddChild(AsShared());
	NewParent->RoomEditAreas.Add(StaticCastSharedRef<FArmyBaseArea>(AsShared()));
}

bool FArmyBaseArea::IsPointIn(const FVector & InPoint, bool ConsiderHole)
{
	//外轮廓
	TArray<FVector> OutArea;
	//内轮廓
	TArray<TArray<FVector>> InAreas;

	CalculateOutAndInnerHoles(OutArea, InAreas);

	if (!FArmyMath::IsPointInOrOnPolygon2D(InPoint, OutArea))
	{
		return false;
	}

	if (!ConsiderHole)
	{
		return true;
	}

	for (auto& InArea : InAreas)
	{
		if (FArmyMath::IsPointInOrOnPolygon2D(InPoint, InArea))
		{
			return false;
		}
	}

	return true;
}

TSharedPtr<FArmyBaseArea> FArmyBaseArea::FindArea(const FVector& InPoint)
{
	if (!IsPointIn(InPoint, false))
	{
		return nullptr;
	}

	for (auto& SubArea : RoomEditAreas)
	{
		auto Ret = SubArea->FindArea(InPoint);
		if (Ret.IsValid())
		{
			return Ret;
		}

	}

	/* @梁晓菲 判断是否选中了波打线*/
	FArmyRectArea* RectArea = dynamic_cast<FArmyRectArea*>(this);
	if (RectArea && RectArea->HasBodaThickness())
	{
		TSharedPtr<FArmyBodaArea> BodaArea = RectArea->GetBodaSurfaceArea();
		if (BodaArea.IsValid() && BodaArea->IsPointIn(InPoint, true))
		{
			return BodaArea;
		}
	}

	return StaticCastSharedRef<FArmyBaseArea>(this->AsShared());

}

void FArmyBaseArea::Move(const FVector & Delta)
{
}

void FArmyBaseArea::CalculateOutAndInnerHoles(TArray<FVector>& InOutArea, TArray<TArray<FVector>>& InnearHoles)
{
	InOutArea = OutArea->Vertices;
	for (int i = 0; i < RoomEditAreas.Num(); i++)
	{
		const  TArray<FVector>& tempHoleVerts = RoomEditAreas[i]->GetOutArea()->Vertices;
		InnearHoles.Add(tempHoleVerts);
	}
	for (int i = 0; i < SurfaceHoles.Num(); ++i)
	{
		const TArray<FVector>& tempHoleVerts0 = SurfaceHoles[i];
		InnearHoles.Add(tempHoleVerts0);
	}
}

void FArmyBaseArea::RefreshExtrusionPlane()
{
	if (GetParents().Num() != 0)
	{
		TWeakPtr<FArmyObject> parent = GetParents()[0];
		if (!parent.IsValid())
			return;
		FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
		m_deltaHeight = temp->GetExtrusionHeight() - M_ExtrusionHeight;

		/* @梁晓菲 面的高度差为0，删除灯槽*/
		if (m_deltaHeight == 0 && !GetLampSlotExtruder()->IsEmpty())
		{
			DestroyLampLight();
		}
	}
}

void FArmyBaseArea::CalculateBackGroundSurface(const TArray<FVector>& outLine, const TArray<TArray<FVector>>& AreaHoles)
{
	TArray <TSharedPtr<AreaCompare>> results = CombineOutAreaAndInnearHole(outLine, AreaHoles);
	BackGroundSurface.Empty();
	TempHighLightVertices.Empty();
	for (TSharedPtr<AreaCompare> iter : results)
	{
		TArray<TArray<FVector>> tempHoles;
		for (int i = 0; i < iter->InnearHoles.Num(); ++i)
		{
			tempHoles.Push(iter->InnearHoles[i].M_Area);
		}
		TriangleArea(iter->M_Area, tempHoles);
	}
}
void FArmyBaseArea::TriangleArea(const TArray<FVector>& OutAreas, const TArray<TArray<FVector>>& AreaHoles)
{
	TArray<FVector2D> points;
	Translate::TriangleAreaWithHole(OutAreas, AreaHoles, points);
	FVector Tangent = PlaneXDir;
	FVector Normal = FVector::CrossProduct(PlaneXDir, PlaneYDir);
	int number0 = points.Num();
	FBox2D box(points);
	minPos = box.Min;
	for (int i = 0; i < number0; ++i)
	{
		TempHighLightVertices.Push(PlaneXDir * points[i].X + PlaneYDir * points[i].Y + PlaneNormal * M_ExtrusionHeight + PlaneOrignPos);
	}
	float tempOffset = M_ExtrusionHeight == 0 ? -FArmySceneData::Get()->FinishWallThick / 2.0f : (M_ExtrusionHeight - FArmySceneData::Get()->FinishWallThick / 2.0f);
	for (int index = 0; index < number0; ++index)
	{
		float UCoord = (points[index] - minPos).X;
		float VCoord = (points[index] - minPos).Y;
		BackGroundSurface.Push(FDynamicMeshVertex(PlaneXDir * points[index].X + PlaneYDir * points[index].Y + PlaneOrignPos + PlaneNormal * tempOffset, Tangent, Normal, FVector2D(UCoord, VCoord), FColor::White));
	}
}

void FArmyBaseArea::CalculateWallActor(TSharedPtr<FArmyBaseEditStyle> InStyle)
{
	//@打扮家 XRLightmass 临时 不生成墙体和地板
	//if (SurfaceType == 1 || SurfaceType == 2)
	//	return;

	if (InStyle.IsValid())
	{
		if (WallActor == NULL)
		{
			if (IsTempArea)
			{
				FActorSpawnParameters SpawnParam;
				SpawnParam.Name = FName(*(FString("NOLIST-")));
				WallActor = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity, SpawnParam);
			}
			else
			{
				WallActor = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity);
			}
			
			WallActor->MeshComponent->bBuildStaticLighting = true;
			WallActor->AttachSurface = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
			WallActor->ActorType = EActorType::WallAreaActor;
			WallActor->Tags.Add(TEXT("Immovable"));
			WallActor->Tags.Add(TEXT("CanNotDelete"));
			WallActor->Tags.Add(TEXT("Wall"));

			WallActor->bIsSelectable = true;
			switch (SurportPlaceArea)
			{
			case 0:
				WallActor->SetFolderPath(FArmyActorPath::GetFloorPath());
				WallActor->SetActorLabel(FArmyActorLabel::MakeFloorLabel());
				break;
			case 1:
				WallActor->SetFolderPath(FArmyActorPath::GetWallPath());
				WallActor->SetActorLabel(FArmyActorLabel::MakeWallLabel());
				break;
			case 2:
				WallActor->SetFolderPath(FArmyActorPath::GetRoofPath());
				WallActor->SetActorLabel(FArmyActorLabel::MakeRoofLabel());
				break;
			default:
				break;
			}
		}
		WallActor->ResetMeshTriangles();

		//@打扮家 XRLightmass 临时 地面直接展平2UV
		if (WallActor && (SurfaceType == 0 || SurfaceType == 1 || SurfaceType == 2))
			WallActor->GetMeshComponent()->bAutoWrapFlatLightUV = true;

		TArray<FDynamicMeshVertex> allVertexs;
		TArray<FDynamicMeshVertex> AllGapVerts;
		InStyle->GetVertexInfo(allVertexs);
		InStyle->GetGapVertsInfo(AllGapVerts);
		WallActor->AddVerts(allVertexs);
		WallActor->AddGapVerts(AllGapVerts);
		WallActor->UpdateAllVetexBufferIndexBuffer();
		WallActor->SetMaterial(MatStyle->GetMaterial());
		WallActor->SetBrickMaterial(MI_BackGroundColor);
	}
}

void FArmyBaseArea::RefreshPolyVertices()
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
		MatStyle->SetDrawArea(tempOutVertices, Holes);
		if (M_ExtrusionHeight != 0.0f)
			MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		CalculateWallActor(MatStyle);
	}
	//CalculateBackGroundSurface(OutArea->Vertices, Holes);

	//@郭子阳 刷新子面
	for (auto & SubArea : RoomEditAreas)
	{
		SubArea->RefreshPolyVertices();
	}
}

void FArmyBaseArea::OffsetExtrusionActors(const FVector& InOffset)
{
    FVector Offset = InOffset;
    Offset.X = -Offset.X;
    Offset.Z = 0.f;

    if (SurfaceType == 0)
    {
        GetBaseboardExtruder()->ApplyOffset(Offset);
    }
    else if (SurfaceType == 2)
    {
        GetCrownMouldingExtruder()->ApplyOffset(Offset);
        GetLampSlotExtruder()->ApplyOffset(Offset);
    }
}

void FArmyBaseArea::UpdateWallActor()
{
	if (MatStyle.IsValid())
		CalculateWallActor(MatStyle);
}

FVector2D FArmyBaseArea::TranlateToPlane(const FVector& World)
{
	float x = FVector::DotProduct((World - (PlaneOrignPos + PlaneNormal*M_ExtrusionHeight)), PlaneXDir);
	float y = FVector::DotProduct((World - (PlaneOrignPos + PlaneNormal*M_ExtrusionHeight)), PlaneYDir);

	return FVector2D(x, y);
}

// 平面坐标系转世界坐标系
FVector FArmyBaseArea::PlaneToTranlate(const FVector& Plane)
{
	FVector result = Plane.X * PlaneXDir + Plane.Y * PlaneYDir + PlaneOrignPos;
	return result;
}

FVector FArmyBaseArea::GetPlaneLocalFromWorld(const FVector& WorldPos)
{
	float x = FVector::DotProduct((WorldPos - PlaneOrignPos), PlaneXDir.GetSafeNormal());
	float y = FVector::DotProduct((WorldPos - PlaneOrignPos), PlaneYDir.GetSafeNormal());
	float z = FVector::DotProduct((WorldPos - PlaneOrignPos), PlaneNormal.GetSafeNormal());

	return FVector(x, y, z);
}

void FArmyBaseArea::SetLayingPointPos(FVector InLayingPointPos)
{
	LayingPointPos = InLayingPointPos;

	if (GetStyle().IsValid())
	{
		GetStyle()->SetLayingPointPos(InLayingPointPos);
	}
}

void FArmyBaseArea::OffsetLayingPointPos(const FVector& InDelta)
{
	SetLayingPointPos(LayingPointPos + InDelta);
}

const FVector& FArmyBaseArea::GetLayingPointPos()
{
	if (LayingPointPos == FVector::ZeroVector && OutArea->Vertices.Num() > 0)
	{
		SetLayingPointPos(OutArea->Vertices[0]);
	}

	return LayingPointPos;
}

TSharedPtr<class FArmyBaseEditStyle> FArmyBaseArea::GetMatStyle()
{
	if (MatStyle.IsValid())
	{
		return MatStyle;
	}
	else
	{
		return nullptr;
	}
}

//void FArmyBaseArea::ReqConstructionData(int32 GoodsId)
//{
//	FString url = FString::Printf(TEXT("/api/quota/goods/%d"), GoodsId);
//	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyBaseArea::ResConstructionData, GoodsId));
//	Request->ProcessRequest();
//}
//
//void FArmyBaseArea::ResConstructionData(FArmyHttpResponse Response, int32 GoodsId)
//{
//	if (Response.bWasSuccessful && Response.Data.IsValid())
//	{
//		FArmySceneData::Get()->AddItemIDConstructionData(GoodsId, Response.Data);
//	}
//}

void FArmyBaseArea::CalculateBuckle()
{
	/* @梁晓菲 生成扣条*/
	if (bHasBuckle && bIsSingleBuckle)
	{
		/** @梁晓菲 利用Map填充BuckleEdges*/
		for (auto &temp : BuckleMap)
		{
			BuckleEdges.Add(temp.Key);
		}

		int32 VerticesNum = OutArea->Vertices.Num();
		for (auto&It : BuckleMap)
		{
			bool bFoundMatchingBuckle = false;
			for (int i = 0; i < VerticesNum; i++)
			{
				if (It.Key == FArmyAreaEdge(OutArea->Vertices[i % VerticesNum], OutArea->Vertices[(i + 1) % VerticesNum]))
				{
					AArmyExtrusionActor* BuckleActor = GVC->GetWorld()->SpawnActor<AArmyExtrusionActor>(AArmyExtrusionActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
					BuckleActor->MeshComponent->bBuildStaticLighting = false;
					BuckleActor->SetSynID(It.Value->ID);
					BuckleActorMap.Emplace(It.Key, BuckleActor);/** @梁晓菲 利用BuckleEdges和BuckleActor填充Map*/
					BuckleActors.Add(BuckleActor);
					VectorCode = i;
					FString UniqueCodeString = TEXT("Buckle") + FString::Printf(TEXT("%d"), VectorCode) + GetUniqueID().ToString();
					BuckleActor->UniqueCodeExtrusion = UniqueCodeString;
					TArray<AArmyExtrusionActor*> BuckleActorArray;
					BuckleActorArray.Add(BuckleActor);

					GenerateBuckleActorLine(GVC->GetWorld(), It.Value);
					bFoundMatchingBuckle = true;
					//@郭子阳 转换老方案(V1.9之前)施工项
					TSharedPtr<FArmyConstructionItemInterface> temp = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(UniqueCodeString);//加载
					if (temp.IsValid())
					{
						XRConstructionManager::Get()->SaveConstructionData(GetUniqueID()
							, BuckleActor->GetConstructionParameter(), temp);

						FArmySceneData::Get()->DeleteExtrusionConstructionItemData(UniqueCodeString);
					}
					//@郭子阳 获取施工项
					XRConstructionManager::Get()->TryToFindConstructionData(GetUniqueID(), BuckleActor->GetConstructionParameter(), nullptr);

					//TSharedPtr<FArmyConstructionItemInterface> temp = FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(UniqueCodeString);//加载
					//if (!temp.IsValid())
					//{
					//	FArmySceneData::Get()->AddExtrusionConstructionData(UniqueCodeString, MakeShareable(new FArmyConstructionItemInterface));
					//}
					break;
				}
			}
			// @zengy 临时解决方法，如果在OutArea这个区域中未找到能够匹配It指向的扣条的边，则置一个空值占位
			// 为了保证在GenerateBuckleActorLine函数中通过遍历BuckleEdges时，能够和BuckleActors对应上
			if (!bFoundMatchingBuckle)
				BuckleActors.Add(nullptr);
		}
	}
}

void FArmyBaseArea::GenerateConstructionInfo()
{
    TArray<float> Deltas = { 0.0f, 1.2f }; // 挤出的线的间距，厘米为单位

    if (SurfaceType == 0)
    {
        TArray< TArray<FVector> > ExtrusionVertexes;
        TArray<FArmyExtrusionPath> Paths = GetBaseboardExtruder()->GetPaths();
        for (auto& It : Paths)
        {
            ExtrusionVertexes.Add(It.Vertexes);
        }

        if (Paths.Num() == 1 && Paths[0].bClosed)
        {
            RoomDataStatus |= FLAG_CLOSESKITLINE;
        }
        else if (Paths.Num() > 0)
        {
            RoomDataStatus &= ~FLAG_CLOSESKITLINE;
            RoomDataStatus &= ~FLAG_DEFAULTSTYLE;
            RoomDataStatus |= FLAG_HAVESKITLINE;
        }

        CaculateConstructionSkitLineInfo(ExtrusionVertexes, Deltas);
    }
    else if (SurfaceType == 2)
    {
        TArray< TArray<FVector> > ExtrusionVertexes;
        TArray<FArmyExtrusionPath> Paths = GetCrownMouldingExtruder()->GetPaths();
        for (auto& It : Paths)
        {
            ExtrusionVertexes.Add(It.Vertexes);
        }

        if (Paths.Num() == 1 && Paths[0].bClosed)
        {
            RoomDataStatus |= FLAG_CLOSESKITLINE;
        }
        else if (Paths.Num() > 0)
        {
            RoomDataStatus &= ~FLAG_CLOSESKITLINE;
            RoomDataStatus &= ~FLAG_DEFAULTSTYLE;
            RoomDataStatus |= FLAG_HAVESKITLINE;
        }

        CaculateConstructionSkitLineInfo(ExtrusionVertexes, Deltas);
    }
}

void FArmyBaseArea::CalculateBaseboardPaths(TArray<FArmyExtrusionPath>& OutPaths, TSharedPtr<FContentItemSpace::FContentItem> ContentItem)
{
    // @欧石楠 计算房间轮廓并以此生成踢脚线
	TArray<FVector2D> HoleVertices;
	if (FArmySceneData::Get()->GetRoomHoleInfoById(AttachRoomID, HoleVertices))
	{
		TArray<TArray<FVector>> TempHoles;
		for (TSharedPtr<FArmyBaseArea> iter : RoomEditAreas)
		{
			if (iter->M_ExtrusionHeight > 0)
			{
				TempHoles.Add(iter->OutArea->Vertices);
			}
		}

		HoleVertices = Translate::UniqueHoles(HoleVertices);

		// 计算包立管
		TArray<FVector> OutlineWordPositions;
		for (FVector& iter : OutArea->Vertices)
		{
			FVector worldPos = PlaneXDir * iter.X + PlaneYDir * iter.Y + PlaneOrignPos;
			OutlineWordPositions.Emplace(worldPos);
		}
		TArray<TWeakPtr<FArmyObject>> AllPackPipe;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, AllPackPipe);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, AllPackPipe);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, AllPackPipe);

		TArray<TWeakPtr<FArmyObject>> AllHalfWalls;
		TArray<TWeakPtr<FArmyObject>> AllAddWalls;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AllAddWalls);
		TArray<TWeakPtr<FArmyObject>> allRelatedHards;
		for (TWeakPtr<FArmyObject>iter : AllAddWalls)
		{
			FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
			if (tempAddWall->GetIsHalfWall())
			{
				AllHalfWalls.Emplace(iter);
				tempAddWall->GetRelatedHardwares(allRelatedHards);
			}
		}

		for (TWeakPtr<FArmyObject> iter : allRelatedHards)
		{
			FArmyHardware* tempHard = iter.Pin()->AsassignObj<FArmyHardware>();
			if (tempHard)
			{
				FVector startPos = tempHard->GetStartPos();
				FVector endPos = tempHard->GetEndPos();
				FVector vertic = tempHard->GetDirection();
				float width = tempHard->GetWidth();
				TArray<FVector2D> hardHoles = { FVector2D(startPos + vertic* width / 2),FVector2D(endPos + vertic* width / 2),
												FVector2D(startPos - vertic* width / 2),FVector2D(endPos - vertic* width / 2) };
				HoleVertices.Append(hardHoles);
			}
		}

		AllPackPipe.Append(AllHalfWalls);
		for (TWeakPtr<FArmyObject> iter : AllPackPipe)
		{
			TArray<FVector> result;
			iter.Pin()->GetVertexes(result);
			//if (Translate::ArePolygonInPolygon(OutlineWordPositions, result))
			if (FArmyMath::ArePolysOverlap(OutlineWordPositions, result))
			{
				TArray<FVector> combineVerts;
				if (iter.Pin()->GetType() == OT_IndependentWall)
					Translate::CaculatePackPipeOutLineVerts(OutlineWordPositions, result, combineVerts);
				else
					combineVerts = result;
				TArray<FVector> ConvertPackOutLines;
				for (const FVector& pos : combineVerts)
				{
					float x = FVector::DotProduct(pos - PlaneOrignPos, PlaneXDir);
					float  y = FVector::DotProduct(pos - PlaneOrignPos, PlaneYDir);
					ConvertPackOutLines.Emplace(FVector(x, y, 0.0f));
				}
				TempHoles.Emplace(ConvertPackOutLines);
			}
		}

		TArray< TSharedPtr<AreaCompare> > outResults = CombineOutAreaAndInnearHole(OutArea->Vertices, TempHoles);
        for (TSharedPtr<AreaCompare> areIter : outResults)
        {
            TArray<FVector> NewOutVertices = areIter->M_Area;
            for (auto& iter : NewOutVertices)
            {
                iter = iter.X * PlaneXDir + iter.Y * PlaneYDir + PlaneOrignPos + PlaneNormal * (M_ExtrusionHeight + m_deltaHeight);
            }

            int number = NewOutVertices.Num();
            TArray<FVector2D> RoomVertices;
            RoomVertices.AddUninitialized(number);
            for (int i = 0; i < number; ++i)
            {
                RoomVertices[i] = FVector2D(NewOutVertices[i]);
            }

            TArray< TArray<FVector2D> > ResultLines;
            if (FArmyMath::UpdataBreakLineInfos(RoomVertices, ResultLines, HoleVertices))
            {
                for (int32 i = 0; i < ResultLines.Num(); ++i)
                {
                    TArray<FVector> TempSkitLine;
                    for (int32 j = 0; j < ResultLines[i].Num(); ++j)
                    {
                        TempSkitLine.Add(FVector(ResultLines[i][j], 0.f));
                    }
                    OutPaths.Add(FArmyExtrusionPath(TempSkitLine, false, ContentItem));
                }
            }
            else
            {
                OutPaths.Add(FArmyExtrusionPath(NewOutVertices, true, ContentItem));
            }
        }
	}
}

void FArmyBaseArea::CalculateCrownMouldingPaths(TArray<FArmyExtrusionPath>& OutPaths, TSharedPtr<FContentItemSpace::FContentItem> ContentItem)
{
    TArray<TArray<FVector>> TempHoles;
    for (TSharedPtr<FArmyBaseArea> iter : RoomEditAreas)
    {
        if (iter->M_ExtrusionHeight > 0)
        {
            TempHoles.Add(iter->OutArea->Vertices);
        }
    }
    TempHoles.Append(SurfaceHoles);

    // @欧石楠 如果有灯槽，顶角线需要沿着灯槽内圈放样
    TArray<FVector> AreaVertexes = OutArea->Vertices;
    if (!GetLampSlotExtruder()->IsEmpty() && SurfaceType == 2)
    {
        AreaVertexes = GetLampSlotExtruder()->GetAreaVertexes();
    }

    TArray< TSharedPtr<AreaCompare> > AreaCompares = CombineOutAreaAndInnearHole(AreaVertexes, TempHoles);
    for (TSharedPtr<AreaCompare> AreaIt : AreaCompares)
    {
        TArray<FVector> NewOutVertices = AreaIt->M_Area;
        FArmyMath::CleanPolygon(NewOutVertices, 0.02f);
        for (auto& iter : NewOutVertices)
        {
            iter = iter.X * PlaneXDir + iter.Y * PlaneYDir + PlaneOrignPos + PlaneNormal * M_ExtrusionHeight;
        }

        if (NewOutVertices.Num() < 3)
        {
            continue;
        }

        if (!FArmyMath::IsClockWise(NewOutVertices))
        {
            FArmyMath::ReversePointList(NewOutVertices);
        }

        OutPaths.Add(FArmyExtrusionPath(NewOutVertices, true, ContentItem));
    }
}

void FArmyBaseArea::ClearExtrusionActors()
{
    // @欧石楠 递归清除踢脚线
    std::function< void(TSharedPtr<FArmyBaseArea>) >
        ClearBaseboard = [&ClearBaseboard](TSharedPtr<FArmyBaseArea> InArea) {
        for (auto & SubArea : InArea->GetEditAreas())
        {
            ClearBaseboard(SubArea);
        }

        InArea->GetBaseboardExtruder()->Clear();
    };
    ClearBaseboard(SharedThis(this));

    // @欧石楠 递归清除顶角线
    std::function< void(TSharedPtr<FArmyBaseArea>) >
        ClearCrownMoulding = [&ClearCrownMoulding](TSharedPtr<FArmyBaseArea> InArea) {
        for (auto & SubArea : InArea->GetEditAreas())
        {
            ClearCrownMoulding(SubArea);
        }

        InArea->GetCrownMouldingExtruder()->Clear();
    };
    ClearCrownMoulding(SharedThis(this));

    // @欧石楠 递归清除灯槽
    std::function< void(TSharedPtr<FArmyBaseArea>) >
        ClearLampSlot = [&ClearLampSlot](TSharedPtr<FArmyBaseArea> InArea) {
        for (auto & SubArea : InArea->GetEditAreas())
        {
            ClearLampSlot(SubArea);
        }

        InArea->GetLampSlotExtruder()->Clear();
    };
    ClearLampSlot(SharedThis(this));

	//@郭子阳 删除造型线
	std::function< void(TSharedPtr<FArmyBaseArea>) >
		ClearModelLine = [&ClearModelLine](TSharedPtr<FArmyBaseArea> InArea) {
		for (auto & SubArea : InArea->GetEditAreas())
		{
            ClearModelLine(SubArea);
		}

		if (InArea->ExturesionActor&&InArea->ExturesionActor->IsValidLowLevel())
		{
			InArea->ExturesionActor->Destroy();
			InArea->ExturesionActor = nullptr;
		}
	};
	ClearModelLine(SharedThis(this));
	
}

TSharedPtr<class FArmyBaseboardExtruder> FArmyBaseArea::GetBaseboardExtruder()
{
    if (!BaseboardExtruder.IsValid())
    {
        if (GetType() == OT_RoomSpaceArea)
        {
            BaseboardExtruder = MakeShareable(new FArmyBaseboardExtruder(
                GVC->GetWorld(), StaticCastSharedRef<FArmyBaseArea>(this->AsShared()), StaticCastSharedRef<FArmyRoomSpaceArea>(this->AsShared())));
        }
        else
        {
            BaseboardExtruder = MakeShareable(new FArmyBaseboardExtruder(
                GVC->GetWorld(), StaticCastSharedRef<FArmyBaseArea>(this->AsShared()), nullptr));
        }
    }

    return BaseboardExtruder;
}

TSharedPtr<class FArmyCrownMouldingExtruder> FArmyBaseArea::GetCrownMouldingExtruder()
{
    if (!CrownMouldingExtruder.IsValid())
    {
        if (GetType() == OT_RoomSpaceArea)
        {
            CrownMouldingExtruder = MakeShareable(new FArmyCrownMouldingExtruder(
                GVC->GetWorld(), StaticCastSharedRef<FArmyBaseArea>(this->AsShared()), StaticCastSharedRef<FArmyRoomSpaceArea>(this->AsShared())));
        }
        else
        {
            CrownMouldingExtruder = MakeShareable(new FArmyCrownMouldingExtruder(
                GVC->GetWorld(), StaticCastSharedRef<FArmyBaseArea>(this->AsShared()), nullptr));
        }
    }

    return CrownMouldingExtruder;
}
