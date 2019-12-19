#include "ArmyBodaArea.h"
#include "ArmyWallActor.h"
#include "ArmyPolygon.h"
#include "ArmyViewportClient.h"
#include "ArmySceneData/Private/Data/HardModeData/Mathematics/EarcutTesselator.h"
#include "ArmyActorConstant.h"
#include "../Mathematics/EarcutTesselator.h"
#include "ResManager.h"
#include "ArmyResourceModule.h"


FArmyBodaArea::FArmyBodaArea(const TArray<FVector>& InOutArea, const float InWidth, const float InLenght, const FVector& InXdir, const FVector& InYDir, const FVector& InCenter) :FArmyBaseArea()
{
	OutVerts = InOutArea;
	brickWidth = InWidth;
	brickLength = InLenght;
	BrickDist = 0.2f;
	PlaneXDir = InXdir;
	PlaneYDir = InYDir;
	PlaneOrignPos = InCenter;
	PlaneNormal = PlaneXDir^PlaneYDir;
	//@王志超 18.12.18
	ObjectType = OT_BodaArea;

	InneraVerts = FArmyMath::ExturdePolygon(InOutArea, InWidth, true);
	InneraArea->SetVertices(InneraVerts);
	OutArea->SetVertices(InOutArea);
}

void FArmyBodaArea::SetBodaBrickInfo(const float InWidth, const float InLength, float InbrickDist)
{
	brickWidth = InWidth;
	brickLength = InLength;
	BrickDist = InbrickDist;

	CaculateBodaArea(OutVerts, brickWidth, brickLength, BrickDist);
}

void FArmyBodaArea::DrawConstructionMode(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	for (auto& iter : TotoalBricks)
	{
		iter->Draw(PDI, View);
	}
}

void FArmyBodaArea::UpdateBodaAreaBrickDist(const float InBrickDist)
{
	BrickDist = InBrickDist;
	if (MatStyle.IsValid())
		MatStyle->SetInternalDist(BrickDist);
	CaculateBodaArea(OutVerts, brickWidth, brickLength, BrickDist);
}

void FArmyBodaArea::SetExtrusionHeight(float height)
{
	for (TSharedPtr<FArmyBrickUnit> brick : TotoalBricks)
	{
		brick->SetPlaneOffset(height);
	}
	UpdateBodaActor();
}

void FArmyBodaArea::SetBodaMaterial(UMaterialInstanceDynamic* InMaterial)
{
	if (WallActor)
		WallActor->SetMaterial(InMaterial);
}

void FArmyBodaArea::CaculateBodaArea(const TArray<FVector>& InVerts, const float InWidth, const float InLenght, const float InBrickDist)
{
	TArray<FVector> InnverVerts = FArmyMath::ExturdePolygon(InVerts, InWidth, true);
	int number0 = InVerts.Num();
	if (number0 != InnverVerts.Num())
		return;
	TotoalBricks.Empty();
	OutVerts = InVerts;
	InneraVerts = InnverVerts;

	/* @梁晓菲 高亮区域*/
	TArray<TArray<FVector>> innearHoles;
	innearHoles.Add(InneraVerts);
	TArray<FVector2D> points;
	Translate::TriangleAreaWithHole(OutVerts, innearHoles, points);
	int32 number = points.Num();
	FBox2D box(points);
	minPos = box.Min;
	TempHighLightVertices.Empty();
	for (int i = 0; i < number; ++i)
	{
		TempHighLightVertices.Push(PlaneXDir * points[i].X + PlaneYDir * points[i].Y + PlaneNormal * M_ExtrusionHeight + PlaneOrignPos);
	}

	// 外部轮廓为顺时针
	for (int i = 0; i < number0; i++)
	{
		const FVector& OutStart = InVerts[i%number0];
		const FVector& OutEnd = InVerts[(i + 1) % number0];
		const FVector& InStart = InnverVerts[i%number0];
		const FVector& InEnd = InnverVerts[(i + 1) % number0];

		FVector dir0 = (InStart - OutStart).GetSafeNormal().RotateAngleAxis(-90, FVector(0, 0, 1));
		FVector newOutStart = dir0	 * InBrickDist / 2.0f + OutStart;
		FVector newInStart = dir0 * InBrickDist / 2.0f + InStart;
		FVector dir1 = (OutEnd - InEnd).GetSafeNormal().RotateAngleAxis(-90, FVector(0, 0, 1));
		FVector newInEnd = dir1 * InBrickDist / 2.0f + InEnd;
		FVector newOutEnd = dir1 * InBrickDist / 2.0f + OutEnd;
		FVector2D first, second, third, fourth;
		FArmyMath::Line2DIntersection(FVector2D(newInStart), FVector2D(newOutStart), FVector2D(OutStart), FVector2D(OutEnd), first);
		FArmyMath::Line2DIntersection(FVector2D(newInEnd), FVector2D(newOutEnd), FVector2D(OutStart), FVector2D(OutEnd), second);
		FArmyMath::Line2DIntersection(FVector2D(newInEnd), FVector2D(newOutEnd), FVector2D(InStart), FVector2D(InEnd), third);
		FArmyMath::Line2DIntersection(FVector2D(newInStart), FVector2D(newOutStart), FVector2D(InStart), FVector2D(InEnd), fourth);

		TArray<FVector> ClipperArea = { FVector(first,0.0f),FVector(second,0.0f),FVector(third,0.0),FVector(fourth,0.0f) };

		float length = (OutEnd - OutStart).Size();
		int HalfEdgeBrickNumber = FMath::CeilToInt(length / (InLenght * 2)) + 1;
		FVector diretion = (OutEnd - OutStart).GetSafeNormal();
		float angle = diretion.ToOrientationRotator().Yaw;
		FBox box(ClipperArea);
		FVector center = box.GetCenter();
		TArray<TArray<FVector>> tempHoles;
		FVector startPos = center + (InLenght / 2 + InBrickDist)*(-diretion) + (HalfEdgeBrickNumber) * (InLenght + InBrickDist) *(-diretion);
		for (int i = 0; i < HalfEdgeBrickNumber * 2; i++)
		{
			TSharedPtr<FArmyBrickUnit> tempBrick = MakeShareable(new FArmyBrickUnit());
			tempBrick->PlaneXDir = PlaneXDir;
			tempBrick->PlaneYDir = PlaneYDir;
			tempBrick->PlaneCenterPos = PlaneOrignPos;
			tempBrick->polyOffset = 0.0f;
			tempBrick->SetBrickWidth(InLenght);
			tempBrick->SetBrickHeight(InWidth);
			tempBrick->RotateAroundCenterPos(angle);

			FVector brickPos = startPos + (InLenght + InBrickDist)* i *diretion;
			tempBrick->SetPoisition(brickPos);
			tempBrick->ApplyClipper(ClipperArea, tempHoles,1,1,GetMatStyle()->GetInternalDist());
			if (tempBrick->IsClipper())
			{
				TotoalBricks.Push(tempBrick);
			}
		}
	}
	UpdateBodaActor();
}

 bool FArmyBodaArea::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* ViwePortClient, float& dist)
 {
 	if (FArmyBaseArea::IsSelected(Pos, ViwePortClient, dist, OutVerts))
 	{
 		if (!FArmyBaseArea::IsSelected(Pos, ViwePortClient, dist, InneraVerts))
 		{
 			return true;
 		}
 	}
 	return false;
 }


 void  FArmyBodaArea::CalculateOutAndInnerHoles(TArray<FVector>& InOutArea, TArray<TArray<FVector>>& InnearHoles)
{
	 //FArmyBaseArea::CalculateOutAndInnerHoles(InOutArea, InnearHoles);
	 InnearHoles.Empty();
	 InnearHoles.Add(InneraVerts);
	 InOutArea = OutVerts;
}


void FArmyBodaArea::Destroy()
{
	if (WallActor)
	{
		WallActor->Destroy();
		WallActor = NULL;
	}
}

TSharedPtr<FContentItemSpace::FContentItem > FArmyBodaArea::GetBodaContentItem()
{
	if (MatStyle.IsValid() && MatStyle->GetCurrentItem().IsValid())
		return MatStyle->GetCurrentItem();
	return NULL;
}

void FArmyBodaArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//@郭子阳 
	//序列化ObjID
	JsonWriter->WriteValue("objectId", GetUniqueID().ToString());
	FArmyBaseArea::SerializeToJson(JsonWriter);
	JsonWriter->WriteObjectStart("BodaBrickContentItem");
	if (BodaBrickContentItem.IsValid())
	{
		BodaBrickContentItem->SerializeToJson(JsonWriter);
	}
	JsonWriter->WriteObjectEnd();
	JsonWriter->WriteValue("BrickDist", BrickDist);
}

void FArmyBodaArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	//@郭子阳 
	//序列化ObjID
	FString IDstr;
	if (InJsonData->TryGetStringField("objectId", IDstr))
	{
		FGuid::Parse(IDstr, ObjID);
	}

	FArmyBaseArea::Deserialization(InJsonData);
	TSharedPtr<FJsonObject> itemObject = InJsonData->GetObjectField("BodaBrickContentItem");
	if (itemObject.IsValid())
	{
		if (itemObject->Values.Num() > 0)
		{
			BodaBrickContentItem = MakeShareable(new FContentItemSpace::FContentItem());
			BodaBrickContentItem->Deserialization(itemObject);
		}
	}
	BrickDist = InJsonData->GetNumberField("BrickDist");
}

bool FArmyBodaArea::TestAreaCanAddOrNot(const TArray<FVector>& InTestArea, FGuid ObjId, TSharedPtr<FArmyBaseArea> OutParent)
{
	//波打线暂不支持子区域
	OutParent = nullptr;
	return false;
}



void FArmyBodaArea::UpdateBodaActor()
{
	if (WallActor == NULL)
	{
		WallActor = GVC->GetWorld()->SpawnActor<AXRWallActor>(AXRWallActor::StaticClass(), FTransform::Identity);
		WallActor->MeshComponent->bBuildStaticLighting = true;
		WallActor->ActorType = EActorType::BodaAreaActor;
		WallActor->AttachSurface = StaticCastSharedRef<FArmyBaseArea>(this->AsShared());
		WallActor->Tags.Add(XRActorTag::Immovable);
		WallActor->Tags.Add(XRActorTag::CanNotDelete);
		WallActor->SetFolderPath(FArmyActorPath::GetFloorPath());
	}
	if (MatStyle.IsValid() && MatStyle->GetCurrentItem().IsValid())
	{
		WallActor->SetActorLabel(MatStyle->GetCurrentItem()->Name);
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(MatStyle->GetCurrentItem());
		WallActor->SetMaterial(mat);
	}
	WallActor->ResetMeshTriangles();
	TArray<FDynamicMeshVertex> totalVertices;
	for (auto& iter : TotoalBricks)
	{
		totalVertices.Append(iter->GetVertices());
	}
	WallActor->AddVerts(totalVertices);
	WallActor->UpdateAllVetexBufferIndexBuffer();
}

//TSharedPtr<struct ConstructionPatameters> FArmyBodaArea::GetConstructionParameter()
//{
//	auto  P = MakeShared<ConstructionPatameters>();
//	P->SetPaveInfo(GetBodaMatStyle()->GetGoodsID(),EWallType::All,false);
//	return P;
//}
