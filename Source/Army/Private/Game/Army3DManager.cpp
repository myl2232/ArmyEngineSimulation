#include "Army3DManager.h"
#include "ArmySceneData.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyResourceModule.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyRoom.h"
#include "ArmyPipePoint.h"
#include "ArmyPipeline.h"
#include "ArmyAddWall.h"
#include "ArmyFurniture.h"
#include "ArmyActor.h"
#include "ArmyMath.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "ArmyFurnitureActor.h"
#include "ArmyHydropowerDataManager.h"
#include "ArmyActorConstant.h"
#include "ArmyPipelineActor.h"


UXR3DManager* G3DM = nullptr;

UXR3DManager::UXR3DManager()
{
	G3DM = this;

//	if (!(GetFlags() & RF_ArchetypeObject))
	{
		FArmySceneData::Get()->PointPosGenerateDelegate.BindUObject(this, &UXR3DManager::BindObjectAndActor);
		FArmyResourceModule::Get().GetResourceManager()->OnLevelActorAdded.AddUObject(this, &UXR3DManager::OnLevelActorsAdded);
	}
	
}
void UXR3DManager::OnOwnerDestroyed(AActor* DestroyedActor)
{
	AXRActor* Owner = Cast<AXRActor>(DestroyedActor->GetOwner());
	if (Owner)
	{
		if (Owner)
		{
			FObjectWeakPtr RObj = Owner->GetRelevanceObject();
			if (RObj.IsValid())
			{
				
				//断点改造,修改相关的构件
				TSharedPtr<FArmyFurniture> Furniture= StaticCastSharedPtr<FArmyFurniture>(RObj.Pin());
				if (Furniture->IsSustitute())
				{
					auto Native = Furniture->GetRepresentedFurniture(true);
					Native->Replace(nullptr);
					//选中构件actor
					//HydropowerDetail->ShowSelectedDetial(Native->GetRelevanceActor());
				}
				
				
				//删除灯具的灯控关系
				FGuid ObjectID = RObj.Pin()->GetUniqueID();
				TArray<TWeakPtr<FArmyObject>> OutArr;
				FArmySceneData::Get()->GetObjects(E_HydropowerModel,OT_ComponentBase,OutArr);
				for (int32 i = 0;i<OutArr.Num();i++)
				{
					TWeakPtr<FArmyObject> ObjectItem = OutArr[i];
					TSharedPtr<FArmyFurniture> Item = StaticCastSharedPtr<FArmyFurniture>(ObjectItem.Pin());
					if (Item.IsValid())
					{
						TArray<int32> Keys;
						Item->RelatedFurnitureMap.GetKeys(Keys);
						for (auto key:Keys)
						{
							Item->RelatedFurnitureMap.RemoveSingle(key,ObjectID);
						}
					}
				}
				FArmySceneData::Get()->Delete(RObj.Pin());
			}
			for (int32 i = 0 ;i<Owner->Children.Num();i++)
			{
				if (Owner->Children[i] != DestroyedActor)
				{
					Owner->Children[i]->Destroy();
				}
			}
			Owner->Destroy();
		}
	}
}

struct HAdsorbDetect UXR3DManager::CaptureWallInfo(FVector MousePoint)
{
	return CaptureWallInfoRelace(MousePoint,10.f);
}

struct HAdsorbDetect UXR3DManager::CaptureWallInfoRelace(FVector MousePoint,float releace /*= 0.001f*/)
{
	//static FName TraceTag = FName(TEXT("CaptureWall"));
	//FCollisionQueryParams TraceParams(TraceTag, false);
	//TraceParams.bTraceAsyncScene = true;
	//FVector2D MousePos;
	//GVC->WorldToPixel(MousePoint, MousePos);
	//FVector TraceStart, TraceEnd, TraceDirection;
	//GVC->DeprojectFVector2D(MousePos, TraceStart, TraceDirection);
	//TraceEnd = TraceStart + TraceDirection * 10000.f;

	//FHitResult Hit(ForceInit);
	////-----------------------------------------------------------Trace---------------------------------------------
	//HAdsorbDetect WallInfo;
	//WallInfo.bAdsorbDetect = false;
	//if (GVC->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_BIMBASEWALL, TraceParams))
	//{
	//	WallInfo.bAdsorbDetect = true;
	//	WallInfo.AdsPoint = Hit.ImpactPoint;
	//	WallInfo.WallNormal = Hit.ImpactNormal;
	//	WallInfo.WallDirection = ((Hit.ImpactNormal + FVector::UpVector).Size() < 0.0001 || (Hit.ImpactNormal - FVector::UpVector).Size() < 0.0001) ? FVector(1,0,0) : Hit.ImpactNormal ^ FVector::UpVector;
	//}
	//return WallInfo;

	float Hight = MousePoint.Z;
	FVector ZMousePoint = MousePoint;
	ZMousePoint.Z = 0;
	HAdsorbDetect WallInfo;
	WallInfo.bAdsorbDetect = false;
	WallInfo.AdsPoint = MousePoint;
	WallInfo.OnFloor = FMath::IsNearlyZero(MousePoint.Z, releace);
	WallInfo.OnTop = FMath::IsNearlyEqual(MousePoint.Z,FArmySceneData::WallHeight, releace);


	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	//FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_IndependentWall, InObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, InObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pillar, InObjects);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_AirFlue, InObjects);
	if (InObjects.Num ()==0)
		return WallInfo;
	bool bCapture = false;
	for (int32 j = 0;j<InObjects.Num ();j++)
	{
		FObjectPtr object = InObjects[j].Pin ();
		bool ShouldInside = false;
		if (object->GetType() == OT_InternalRoom)
		{
			ShouldInside = true;
		}
	/*	TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(object);
		if (Room.IsValid())
		{
			ReverseNormal = true;
		}*/
		TArray< TSharedPtr<FArmyLine> > OutLines;
		TArray<FVector> Vertex;
		object->GetLines(OutLines);
		object->GetVertexes(Vertex);

		FBox Bound = object->GetBounds();
		for (int32 i = 0;i<OutLines.Num();i++)
		{
			TSharedPtr<FArmyLine> Line = OutLines[i];
			FVector Start = Line->GetStart();
			FVector End = Line->GetEnd();
			FVector ProjectPoint = FArmyMath::GetProjectionPoint(ZMousePoint,Start,End);
			FVector Direction = ProjectPoint-ZMousePoint;
			bool bOnLine = FArmyMath::IsPointOnLine(ProjectPoint,Start,End);
			if (Direction.Size()<releace&& bOnLine) 
			{
				FVector Normal = FVector::CrossProduct(FVector::UpVector, Start - End).GetSafeNormal();
			//FMath::in
				FVector TempPoint = (Start + End) / 2 + Normal * 2;
				//FArmyMath::IsPointInOrOnPolygon2D(TempPoint, Vertex, 0.1f)
				bool Inside = FArmyMath::IsPointInOrOnPolygon2D(TempPoint, Vertex, 0.1f);
			/*	TempPoint.X >= Bound.Min.X
					&&TempPoint.Y >= Bound.Min.Y
					&&TempPoint.X <= Bound.Max.X
					&&TempPoint.Y <= Bound.Max.Y;*/
				if (ShouldInside ^Inside)
				{
					Normal *= -1.0f;
				}

				//FVector MiddlePoint = (Start + End) / 2 + Normal;
				if (!WallInfo.bAdsorbDetect)
				{
					bCapture = true;
					ProjectPoint.Z = Hight;
					WallInfo.bAdsorbDetect = true;
					WallInfo.AdsPoint = ProjectPoint;
					WallInfo.WallNormal = Normal;// : -Normal;
					WallInfo.WallDirection = (End - Start).GetSafeNormal();
				}
				else
				{
					FVector temp = Normal ^	WallInfo.WallNormal;
					if (temp.Z < 0)
					{
						WallInfo.bCornerDetect = true;
						WallInfo.WallNormal2 = Normal;// object->IsPointInRoom(MiddlePoint) ? Normal : -Normal;
						WallInfo.WallDirection2 = (End - Start).GetSafeNormal();
					}
				}
			}
		}
		if (bCapture)
			break;
	}
	if (!bCapture)
	{
		TArray<TWeakPtr<FArmyObject>> InWallObjectes;
		FArmySceneData::Get ()->GetObjects (E_HomeModel,OT_AddWall,InWallObjectes);
		for (int32 i = 0;i<InWallObjectes.Num ();i++)
		{
			FObjectPtr object = InWallObjectes[i].Pin ();
			if (object->GetType ()!=OT_AddWall)
				continue;
			TSharedPtr<FArmyAddWall> Wall = StaticCastSharedPtr<FArmyAddWall>(object);
			TArray< TSharedPtr<FArmyLine> > OutLines;
			Wall->GetLines(OutLines);
			TArray<FVector> OutPointes;
			Wall->GetVertexes(OutPointes);
			for (int32 i = 0;i<OutLines.Num();i++)
			{
				TSharedPtr<FArmyLine> Line = OutLines[i];
				FVector Start = Line->GetStart();
				FVector End = Line->GetEnd();
				Start.Z = End.Z = 0;
				FVector ProjectPoint = FArmyMath::GetProjectionPoint(ZMousePoint,Start,End);
				bool bOnLine = FArmyMath::IsPointOnLine(ProjectPoint,Start,End);
				FVector Direction = ProjectPoint-ZMousePoint;
				if (Direction.Size()<releace&&bOnLine)
				{
					//bCapture = true;
					//ProjectPoint.Z = Hight;
					//FVector Normal = FVector::CrossProduct(FVector::UpVector,Start-End).GetSafeNormal();
					//FVector MiddlePoint = (Start+End)/2+Normal;
					//WallInfo.bAdsorbDetect = true;
					//WallInfo.AdsPoint = ProjectPoint;
					//WallInfo.WallNormal = FArmyMath::IsPointInOrOnPolygon2D(MiddlePoint,OutPointes) ? -Normal : Normal;
					//WallInfo.WallDirection = (End-Start).GetSafeNormal();
					//break;

					FVector Normal = -FVector::CrossProduct(FVector::UpVector, Start - End).GetSafeNormal();
					FVector MiddlePoint = (Start + End) / 2 + Normal;
					if (!WallInfo.bAdsorbDetect)
					{
						bCapture = true;
						ProjectPoint.Z = Hight;
						WallInfo.bAdsorbDetect = true;
						WallInfo.AdsPoint = ProjectPoint;
						WallInfo.WallNormal = FArmyMath::IsPointInOrOnPolygon2D(MiddlePoint, OutPointes) ? -Normal : Normal;
						WallInfo.WallDirection = (End - Start).GetSafeNormal();
					}
					else
					{
						WallInfo.bCornerDetect = true;
						WallInfo.WallNormal2 = FArmyMath::IsPointInOrOnPolygon2D(MiddlePoint, OutPointes) ? -Normal : Normal;
						WallInfo.WallDirection2 = (End - Start).GetSafeNormal();
						break;
					}
				}
			}
			if (bCapture)
				break;
		}
	}

	return WallInfo;
}

void UXR3DManager::OnXRBrushVisibleChange(AActor* InActor)
{
	for (auto &It : FArmyObject::DynamicActorMap)
	{
		if (It.Value == InActor)
		{
			for (auto Obj : FArmySceneData::Get()->GetObjects(E_HardModel))
			{
				if (Obj.Pin()->GetType() == OT_RoomSpaceArea)
				{
					TSharedPtr<FArmyRoomSpaceArea> RoomAreaObj = StaticCastSharedPtr<FArmyRoomSpaceArea>(Obj.Pin());
					if (RoomAreaObj->AttachBspID == It.Key)
					{
						RoomAreaObj->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, !InActor->bHidden);
						break;
					}
				}
			}
		}
	}
}
void UXR3DManager::OnLevelActorsAdded(AActor* InActor)
{
	InActor->OnDestroyed.AddDynamic(this, &UXR3DManager::OnActorDestroyed);

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(InActor->GetSynID());
	if (ActorItemInfo.IsValid())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ActorItemInfo->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		bool OnlyCeiling = resArr[0]->placePosition.bCeiling && !resArr[0]->placePosition.bFloor &&
			!resArr[0]->placePosition.bWall && !resArr[0]->placePosition.bMesa;

		if (OnlyCeiling)
		{
			TypeActorMap.Add(AT_CEILING,InActor);
		}
	}
}
void UXR3DManager::OnActorDestroyed(AActor* InActor)
{
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(InActor->GetSynID());
	if (ActorItemInfo.IsValid())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ActorItemInfo->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		bool OnlyCeiling = resArr[0]->placePosition.bCeiling && !resArr[0]->placePosition.bFloor &&
			!resArr[0]->placePosition.bWall && !resArr[0]->placePosition.bMesa;

		if (OnlyCeiling)
		{
			TypeActorMap.Remove(AT_CEILING, InActor);
		}
	}
}

void UXR3DManager::SetActorVisibleByType(EXRActorType InType,bool InVisible)
{
	TArray<AActor*> OutArray;
	TypeActorMap.MultiFind(InType, OutArray);
	for (auto A : OutArray)
	{
		A->SetActorHiddenInGame(!InVisible);
	}
}

void UXR3DManager::BindObjectAndActor(FObjectPtr ObjectPtr,AActor* InActor)
{
	if (ObjectPtr->GetType() == OT_ComponentBase)
	{
		
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ObjectPtr);


		//普通模型
		//1.关联actor和object需要AssociatedObjectAndActor
		//2.如果横面显示如要CreateSurfaceComponent（创建横截面）
		//3.如果水电（主下水管）（AssociatedObjectAndPipeLine）
		if (Furniture->GetFurniturePro()->GetbIsPakModel() && InActor != nullptr)
		{
			InActor->OnDestroyed.AddUniqueDynamic(G3DM, &UXR3DManager::OnOwnerDestroyed);
		
			//@郭子阳
			//****************断点改造相关***********
			if (Furniture->IsOrignalPoint())
			{
			//默认不显示坐标轴
				InActor->Tags.AddUnique(TEXT("NoWidget"));
				//原始点位Actor不允许被删除,只能在原始户型下删除
				InActor->Tags.AddUnique(TEXT("CanNotDelete"));

			
			}

			AssociatedObjectAndActor(ObjectPtr, InActor, XRActorTag::OriginalPoint.ToString());

			if (Furniture->IsOrignalPoint())
			{
				if (!Furniture->isNativeLocationSet)
				{
					//设置构建默认的原始位置
					Furniture->SetNativeLocation(Furniture->GetRelevanceActor()->GetActorLocation());
				}
			}
				//****************断点改造结束***********

			//生成水电模型的2维视图模型 
			switch (Furniture->GetFurniturePro()->GetBelongClass())
			{
				case Class_Switch:
				case Class_Socket:
				case Class_ElectricBoxH:
				case Class_ElectricBoxL:
				case Class_WaterRoutePoint:
				case Class_HotWaterPoint://热水点位
				case Class_HotAndColdWaterPoint://冷热水点位
				case Class_ColdWaterPoint://冷水点位
				//case Class_FloordRainPoint://地漏下水
				//case Class_ClosestoolRainPoint://马桶下水
				case Class_WashBasinRainPoint://水盆下水
				case Class_RecycledWaterPoint://中水点位
				case Class_HomeEntryWaterPoint://进户水
				{

					AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(InActor);
					if (StaticMeshActor)
					{
						bool bCreate = CreateSurfaceComponent(InActor, Furniture->GetAltitude());
						if (bCreate)
						{
							for (auto& ItemComp : InActor->GetComponents())
							{
								if (ItemComp->ComponentHasTag(TEXT("Transverse")))
								{
									UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ItemComp);
									StaticMeshComp->SetHiddenInGame(GXRPC->GetXRViewMode() != EXRView_TOP);
								}
							}
						}
					}
					
				}
			}
		}
		else
		{
			AssociatedObjectAndPipeLine(ObjectPtr);
		}
	}
}

bool UXR3DManager::CreateSurfaceComponent(AActor* InActor,float attibute)
{
	AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(InActor);
	if (StaticMeshActor)
	{
		UStaticMeshComponent* ParentMesh = StaticMeshActor->GetStaticMeshComponent();
		UStaticMesh* Mesh = ParentMesh->GetStaticMesh();
		UStaticMeshComponent* TransverseStaticMeshComponent = NewObject<UStaticMeshComponent>(StaticMeshActor,TEXT("Transverse"));
		TransverseStaticMeshComponent->ComponentTags.Add(TEXT("Transverse"));
		TransverseStaticMeshComponent->Mobility = EComponentMobility::Movable;
		TransverseStaticMeshComponent->bGenerateOverlapEvents = false;
		TransverseStaticMeshComponent->bUseDefaultCollision = false;
		TransverseStaticMeshComponent->SetStaticMesh(Mesh);
		TransverseStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		TArray<FName> MaterialNames = ParentMesh->GetMaterialSlotNames();
		for (int32 i = 0; i<MaterialNames.Num(); i++)
			TransverseStaticMeshComponent->SetMaterialByName(MaterialNames[i],ParentMesh->GetMaterial(i));
		FBox Box = TransverseStaticMeshComponent->GetBodySetup()->AggGeom.CalcAABB(FTransform(FVector(0,0,0)));
		FVector MeshPos,Extent;
		Box.GetCenterAndExtents(MeshPos,Extent);
		FRotator Rotator(0.f,0.0,-90.0);
		FVector Location(0,Extent.Z,FArmySceneData::WallHeight+20-attibute);
		FVector InScale(1,1,1);
		FTransform Transform(Rotator,Location,InScale);
		TransverseStaticMeshComponent->SetRelativeTransform(Transform);
		TransverseStaticMeshComponent->AttachToComponent(StaticMeshActor->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		TransverseStaticMeshComponent->RegisterComponent();
		return true;
	}
	return false;
}

bool UXR3DManager::AssociatedObjectAndActor(FObjectPtr ObjectPtr,AActor* InActor,FString Tag)
{
	FName name(*Tag);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Name = MakeUniqueObjectName(GVC->GetOuter(),AXRFurnitureActor::StaticClass(),TEXT("NOLIST"));
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AXRFurnitureActor* Owner = GVC->GetWorld ()->SpawnActor<AXRFurnitureActor>(SpawnInfo);
	Owner->bIsSelectable = InActor->bIsSelectable;//点位模型不可选择
	InActor->SetOwner(Owner);
	InActor->Tags.Add(name);
	Owner->Tags.Add(name);
	Owner->SetRelevanceObject (ObjectPtr);
	ObjectPtr->SetRelevanceActor (InActor);
	return true;
}

bool UXR3DManager::AssociatedObjectAndPipeLine(FObjectPtr ObjectPtr)
{
	if (ObjectPtr.IsValid()&&ObjectPtr->GetType() == OT_ComponentBase)
	{
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ObjectPtr);
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = Furniture->GetFurniturePro()->FurContentItem->GetResObjNoComponent();
        if (resArr.Num() == 0)
        {
            return false;
        }

		TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes>(resArr[0]);
		FVector InStart = Furniture->GetBaseEditPoint()->GetPos();
		FVector InEnd = Furniture->GetBaseEditPoint()->GetPos();
		InEnd.Z += Furniture->GetFurniturePro()->GetHeight();
		EObjectType LineType =(EObjectType) Res->ObjectType;
		TSharedPtr<FArmyPipePoint> StartPoint = FArmyHydropowerDataManager::Get()->MakeShareablePoint(InStart,LineType,Res->PointColor,Res->PointReformColor);
		TSharedPtr<FArmyPipePoint> EndPoint = FArmyHydropowerDataManager::Get()->MakeShareablePoint(InEnd,LineType,Res->PointColor,Res->PointReformColor);

		TSharedPtr<FArmyPipeline> Result = FArmyHydropowerDataManager::Get()->CreatePipeline(StartPoint,EndPoint,Res,ObjectPtr);
		if (Result.IsValid())
		{
			Result->GeneratePipelineModel(GVC->GetWorld());
            Result->PipeLineActor->SetActorLabel(ObjectPtr->GetName());
			Result->UpdateWithPoints(true);

			//@郭子阳
			//燃气主管道和下水主管道全局不可选
			if (Furniture->IsGasMainPipe() || Furniture->IsDownWarterMainPipe())
			{
				Result->PipeLineActor->Tags.AddUnique(XRActorTag::UnSelectable);
			}

			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool UXR3DManager::IsPointInRoom(FVector& InPoint)
{
	TArray<TWeakPtr<FArmyObject>> InObjects;
	FArmySceneData::Get ()->GetObjects (E_LayoutModel,OT_InternalRoom,InObjects);
	if (InObjects.Num ()==0)
		return false;
	for (int32 i = 0;i<InObjects.Num ();i++)
	{
		FObjectPtr object = InObjects[i].Pin ();
		if (object->GetType ()!=OT_InternalRoom)
			continue;
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom> (object);
		if (Room->IsPointInRoom(InPoint))
			return true;
	}
	return false;
}
