#include "ArmyAutoLayoutPatch.h"

#include "ArmyViewportClient.h"
#include "ArmySecurityDoor.h"
#include "ArmySceneData.h"
#include "ArmyWallLine.h"
#include "ArmyWindow.h"
#include "ArmyRegion.h"

const TSharedRef<FArmyLayoutPatchFactory>& FArmyLayoutPatchFactory::Get()
{
	static const TSharedRef<FArmyLayoutPatchFactory> Instance = MakeShareable(new FArmyLayoutPatchFactory);
	return Instance;
}

TSharedRef<FArmyAutoLayoutPatch>& FArmyLayoutPatchFactory::GetLayoutPatchRoom(TSharedPtr<FArmyRoomEntity>& InRoom)
{
	/** 房间ID */
	const int32 spaceId = InRoom->GetAiRoomType();
	switch (spaceId)
	{
		/** 卧室 */
	case RT_Bedroom_Master:
	case RT_Bedroom_Second:
	case RT_ChildrenRoom:
	case RT_Bedroom_Quest:
	{
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchBedroom);
		return layoutpatch;
	}

	/** 书房 */
	case RT_StudyRoom:
	{
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchStudyroom);
		return layoutpatch;
	}

	/** 客厅 */
	case RT_LivingRoom:
	{
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchLivingroom);
		return layoutpatch;
	}

	/** 卫生间 */
	case RT_Bathroom:
	case RT_Bathroom_Mester:
	case RT_Bathroom_Quest:
	{
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchBathRoom);
		return layoutpatch;
	}

	/** 阳台 */
	case RT_Balcony:
	case RT_Balcony_After:
	case RT_Balcony_Bedroom:
	{
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchBalcony);
		return layoutpatch;
	}

	/** 餐厅 */
	case RT_DiningRoom:
	{
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchDiningroom);
		return layoutpatch;
	}

	default:
		//默认空间
		static TSharedRef<FArmyAutoLayoutPatch> layoutpatch = MakeShareable(new FArmyAutoLayoutPatchRoom);
		return layoutpatch;
	}
}

/*------------------------------------ 基类方法实现 Start ------------------------------------*/

// 检测某个模型与柱子、包立管、风道的碰撞，碰撞了返回true
bool FArmyAutoLayoutPatch::CheckWallCornerCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
{
	if (ActorItr == nullptr)
	{
		return true;
	}
	FBox ActorBox = GVC->GetActorCollisionBox(ActorItr);
	ActorBox.Min.Z = 0;
	ActorBox.Max.Z = 0;
	//收集柱子等墙角物件
	TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
	TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
	TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
	TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
	ComponentObjArr.Append(PillarArr);
	ComponentObjArr.Append(PackPipeArr);
	ComponentObjArr.Append(AirFlueArr);
	//碰撞检测
	for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
	{
		FBox ModelBox = model->GetBounds();
		if (ActorBox.Intersect(ModelBox))
		{
			return true;
		}
	}
	return false;
}

// 检测某个模型与顶面（下吊、顶角线和梁）的碰撞，碰撞了返回true
bool FArmyAutoLayoutPatch::CheckCeilingCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
{
	if (ActorItr == nullptr)
	{
		return true;
	}
	//
	FBox ActorBox = GVC->GetActorCollisionBox(ActorItr);
	//
	TArray<TSharedPtr<FArmyObject>> ComponentObjArr;
	TArray<TSharedPtr<FArmyBeam>> BeamArr = InRoom->GetBeamArray();//梁
	ComponentObjArr.Append(BeamArr);
	//
	float CeilingZ = FArmySceneData::WallHeight - InRoom->GetCeilingDropOff() - InRoom->GetCrownMouldingHeight();
	FBox ModelBox = FBox(FVector(0, 0, CeilingZ), FVector(FLT_MAX, FLT_MAX, CeilingZ));//顶面
																					   //碰撞检测
	if (ActorBox.Intersect(ModelBox))
	{
		return true;
	}
	for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
	{
		FBox ModelBox = model->GetBounds();
		if (ActorBox.Intersect(ModelBox))
		{
			return true;
		}
	}
	return false;
}

// 检测某个模型与墙体的碰撞，碰撞了返回true
bool FArmyAutoLayoutPatch::CheckWallCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
{
	if (ActorItr == nullptr)
	{
		return true;
	}
	//
	FBox ActorBox = GVC->GetActorCollisionBox(ActorItr);
	//
	FVector p0 = ActorBox.Min; p0.Z = 0;
	FVector p3 = ActorBox.Max; p3.Z = 0;
	FVector p1 = FVector(p3.X, p0.Y, 0);
	FVector p2 = FVector(p0.X, p3.Y, 0);
	bool p0f = InRoom->GetRoom()->IsPointInRoom(p0);
	bool p1f = InRoom->GetRoom()->IsPointInRoom(p1);
	bool p2f = InRoom->GetRoom()->IsPointInRoom(p2);
	bool p3f = InRoom->GetRoom()->IsPointInRoom(p3);
	if (p0f && p1f && p2f && p3f)
	{
		// 都在房间内则没有跟墙体碰撞
		return false;
	}
	return true;
}

// 检测某个模型与墙上物件的碰撞，碰撞了返回true
bool FArmyAutoLayoutPatch::CheckWallObjectCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
{
	if (ActorItr == nullptr)
	{
		return true;
	}
	//
	FBox ActorBox = GVC->GetActorCollisionBox(ActorItr);
	//
	TArray<TSharedPtr<FArmyHardware>> HardwareDoorArr = InRoom->GetDoorsActor();
	TArray<TSharedPtr<FArmyHardware>> HardwareWinArr = InRoom->GetWindowsActor();
	TArray<TSharedPtr<FArmyHardware>> HardwareObjArr;
	HardwareObjArr.Append(HardwareDoorArr);
	HardwareObjArr.Append(HardwareWinArr);
	for (TSharedPtr<FArmyHardware>& HardwareObj : HardwareObjArr)
	{
		TArray<FVector> WindowVec1 = HardwareObj->RectImagePanel->GetVertices();
		for (FVector & VecIT : WindowVec1)
		{
			VecIT.Z = HardwareObj->GetHeightToFloor();
		}
		TArray<FVector> WindowVec2 = WindowVec1;
		for (FVector & VecIT : WindowVec2)
		{
			VecIT.Z += HardwareObj->GetHeight();
		}
		WindowVec1.Append(WindowVec2);
		FBox ModelBox(WindowVec1);
		ModelBox = ModelBox.ExpandBy(2);//将窗的包围盒适当增大，以便和模型进行碰撞检测
		if (ActorBox.Intersect(ModelBox))
		{
			return true;
		}
	}
	return false;
}

// 检测某个模型与其他模型的碰撞，碰撞了返回true
bool FArmyAutoLayoutPatch::CheckOtherActorCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom, const TSet<AActor*>& IgnoreActorArr)
{
	if (ActorItr == nullptr)
	{
		return true;
	}
	//
	FBox ActorBox = GVC->GetActorCollisionBox(ActorItr);
	//
	TArray <TSharedPtr<FArmyModelEntity>> ModelArr;
	InRoom->GetAllModels(ModelArr);
	for (TSharedPtr<FArmyModelEntity>& Model : ModelArr)
	{
		AActor* ModelActor = Model->Actor;
		if (ModelActor && !IgnoreActorArr.Contains(ModelActor))
		{
			FBox ModelBox = GVC->GetActorCollisionBox(ModelActor);
			if (ActorBox.Intersect(ModelBox))
			{
				return true;
			}
		}
	}
	return false;
}

/*------------------------------------ 基类方法实现 End ------------------------------------*/

/*------------------------------------ 默认空间方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchRoom::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//不做任何分发
}

/*------------------------------------ 默认方法实现 End ------------------------------------*/

/*------------------------------------ 卧室方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchBedroom::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 卧室 梳妆台椅子 */
	case AI_DressingStool:
		CalcChairLayout(InEntity, InRoom, EntityArray);
		break;

		/* 卧室 书桌书椅 */
	case AI_BookChair:
		CalcDeskChairLayout(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}

}

// 卧室 梳妆台椅子
void FArmyAutoLayoutPatchBedroom::CalcChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	//
	TSet<AActor*> OtherActorSet;
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Dresser, DeskArray);
	for (auto &desk : DeskArray)
	{
		FVector deskSize = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector deskPos = desk->Actor->GetActorLocation();
		FVector Forward = FArmyDataTools::GetModelForwardVector(desk->Actor);
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		TempEntity->Location = deskPos + Forward * (deskSize.Y / 2);
		TempEntity->Rotation = (Forward * -1).Rotation() + FRotator(0, -90, 0);

		EntityArr.Add(TempEntity);
		OtherActorSet.Add(desk->Actor);
	}

	//
	//碰撞检测：墙体、柱子类和物件
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCollision(EntityActor, InRoom) && 
				!CheckWallCornerCollision(EntityActor, InRoom))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

// 卧室 书桌椅子
void FArmyAutoLayoutPatchBedroom::CalcDeskChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	//
	TSet<AActor*> OtherActorSet;
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Desk, DeskArray);
	for (auto &desk : DeskArray)
	{
		FVector deskSize = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector deskPos = desk->Actor->GetActorLocation();
		FVector Forward = FArmyDataTools::GetModelForwardVector(desk->Actor);
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		TempEntity->Location = deskPos + Forward * (deskSize.Y / 2);
		TempEntity->Rotation = (Forward * -1).Rotation() + FRotator(0, -90, 0);

		EntityArr.Add(TempEntity);
		OtherActorSet.Add(desk->Actor);
	}

	//
	//碰撞检测：墙体、柱子类和物件
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}


/*------------------------------------ 卧室方法实现 End ------------------------------------*/


/*------------------------------------ 书房方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchStudyroom::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 书房 椅子 */
	case AI_BookChair:
		CalcChairLayout(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

// 书房 椅子
void FArmyAutoLayoutPatchStudyroom::CalcChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	//
	TSet<AActor*> OtherActorSet;
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Desk, DeskArray);
	for (auto &desk : DeskArray)
	{
		FVector deskSize = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector deskPos = desk->Actor->GetActorLocation();
		FVector Forward = FArmyDataTools::GetModelForwardVector(desk->Actor);
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		TempEntity->Location = deskPos + Forward * (deskSize.Y / 2);
		TempEntity->Rotation = (Forward * -1).Rotation() + FRotator(0, -90, 0);

		EntityArr.Add(TempEntity);
		OtherActorSet.Add(desk->Actor);
	}

	//
	//碰撞检测：墙体、柱子类和物件
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

/*------------------------------------ 书房方法实现 End ------------------------------------*/


/*------------------------------------ 客厅方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchLivingroom::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 客厅 边几（软装） */
	case AI_SideTable:
		CalcSideTableLayout(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

// 客厅 边几（软装）
void FArmyAutoLayoutPatchLivingroom::CalcSideTableLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TSet<AActor*> OtherActorSet;

	//获取主位沙发
	TArray<TSharedPtr<class FArmyModelEntity>> SofaArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, SofaArray);
	// 布置两个边几
	for (auto &sofa : SofaArray)
	{
		FVector sofaSize = FArmyDataTools::GetContextItemSize(sofa->FurContentItem);
		FVector sofaPos = sofa->Actor->GetActorLocation();
		FVector Left = FArmyDataTools::GetModelLeftVector(sofa->Actor);
		FVector Right = -Left;
		FVector Backward = FArmyDataTools::GetModelForwardVector(sofa->Actor) * -1;
		//
		FRotator RotationConst = FRotator(0, -90, 0);
		TArray<FVector> DirArr = { Left, Right };
		for (auto& Dir : DirArr)
		{
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			TempEntity->Location = sofaPos + Dir * (sofaSize.X / 2 + InEntity->Length / 2 + 10) + Backward*(sofaSize.Y / 2 - 50);
			TempEntity->Rotation = (Backward * -1).Rotation() + RotationConst;
			EntityArr.Add(TempEntity);
		}
		//
		OtherActorSet.Add(sofa->Actor);
	}

	//碰撞检测：墙体、柱子类和物件
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

/*------------------------------------ 客厅方法实现 End ------------------------------------*/

/*------------------------------------ 卫生间方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchBathRoom::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 卫生间 马桶 */
	case AI_Closestool:
		CalcToilet(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 花洒 */
	case AI_Sprinkler:
		CalcShowerSprinkler(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 淋浴房 */
	case AI_ShowerRoom:
		CalcShower(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 浴室柜（有脚） */
	case AI_BathroomArk:
		CalcCabinet(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 浴室柜（无脚） */
	case AI_BathroomArk_Wall:
		CalcCabinet(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 洗衣机 */
	case AI_WashingMachine:
		CalcWasher(InEntity, InRoom, EntityArray);
		break;


	default:
		break;
	}
}

//马桶、花洒、淋浴房、浴室柜、洗衣机，作为配饰来做
/** 卫生间 马桶 */
void FArmyAutoLayoutPatchBathRoom::CalcToilet(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//马桶下水点
	TSharedPtr<FArmyFurniture> RoomClosestoolPoint;
	TArray<TSharedPtr<FArmyFurniture>> RoomPointArr = InRoom->GetComponentArray();
	for (auto& RoomPoint : RoomPointArr)
	{
		if (RoomPoint->ComponentType == EC_Closestool)
		{
			//马桶下水
			RoomClosestoolPoint = RoomPoint;
			break;
		}
	}
	if (RoomClosestoolPoint.IsValid())
	{
		//
		FVector Position = RoomClosestoolPoint->GetBasePos();
		TSharedPtr<FArmyWallLine> WallLineMin;
		float DisMin = FLT_MAX;
		TArray<TSharedPtr<class FArmyWallLine>> WallLines = InRoom->GetRoom()->GetWallLines();
		for (auto& WallLine : WallLines)
		{
			FVector OutPos = Position;
			bool IsValidPos = FArmyMath::GetLineSegmentProjectionPos(WallLine->GetCoreLine()->GetStart(), WallLine->GetCoreLine()->GetEnd(), OutPos);
			if (IsValidPos)
			{
				float dis = FArmyMath::CalcPointToLineDistance(Position, WallLine->GetCoreLine()->GetStart(), WallLine->GetCoreLine()->GetEnd());
				if (dis < DisMin)
				{
					DisMin = dis;
					WallLineMin = WallLine;
				}
			}
		}
		if (WallLineMin.IsValid())
		{
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
			InEntity->CopyTo(TempEntity);
			//
			FVector Direction = InRoom->GetRoom()->GetWallLineNormal(WallLineMin);
			Direction = Direction.GetSafeNormal();
			//
			TempEntity->Location = Position + Direction * (-DisMin+1);//靠墙
			TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
			//
			//检测碰撞：柱子类、门窗
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckWallCornerCollision(EntityActor, InRoom) &&
					!CheckWallObjectCollision(EntityActor, InRoom) && 
					!CheckWallCollision(EntityActor, InRoom))
				{
					EntityArray.Add(TempEntity);
				}
				EntityActor->Destroy();
			}
		}
	}
}

/** 卫生间 花洒 */
void FArmyAutoLayoutPatchBathRoom::CalcShowerSprinkler(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	// 1 跟淋浴房联合布置花洒（前提：淋浴房是先于花洒布置的）
	TArray<TSharedPtr<FArmyModelEntity>> ShowerRoomArray;
	InRoom->GetModelEntityArrayByComponentId(AI_ShowerRoom, ShowerRoomArray);
	if (ShowerRoomArray.Num() > 0)
	{
		TSharedPtr<FArmyModelEntity> ShowerRoom = ShowerRoomArray[0];//取得淋浴房
		if (ShowerRoom->Actor)
		{
			FVector ForwardDir = FArmyDataTools::GetModelForwardVector(ShowerRoom->Actor);
			ForwardDir = ForwardDir.GetSafeNormal();
			FVector Position = FVector::ZeroVector;
			FVector Size = FArmyDataTools::GetContextItemSize(ShowerRoom->FurContentItem);
			FArmyDataTools::PosOnDirectionOfModel(*ShowerRoom->Actor, Size.X*0.5f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_RIGHT);
			Position += FVector(0, 0, 115) + ForwardDir * 1;
			//
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
			InEntity->CopyTo(TempEntity);
			TempEntity->Location = Position;
			TempEntity->Rotation = ForwardDir.Rotation() + FRotator(0, -90, 0);
			
			//碰撞检测：门窗和顶面
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				bool isNotIntersect = !CheckWallObjectCollision(EntityActor, InRoom) &&
					!CheckCeilingCollision(EntityActor, InRoom) &&
					!CheckWallCollision(EntityActor, InRoom) &&
					!CheckWallCornerCollision(EntityActor, InRoom);
				EntityActor->Destroy();
				if (isNotIntersect)
				{
					EntityArray.Add(TempEntity);
				}
			}
			return;
		}
	}
	// 2 单独布置花洒
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TArray<FVector> Points = InRoom->GetRoom()->GetWorldPoints(true);
	for (auto Point : Points)
	{
		//
		TArray<TSharedPtr<FArmyWallLine>> WallLines;
		InRoom->GetRoom()->GetSamePointWalls(Point, WallLines);
		if (WallLines.Num() >= 2)
		{
			TSharedPtr<FArmyWallLine> Line1 = WallLines[0];
			TSharedPtr<FArmyWallLine> Line2 = WallLines[1];
			//过滤L型的外墙角
			FVector Vec1DirH = Line1->GetDirection();
			if (FVector::PointsAreSame(Line1->GetCoreLine()->GetEnd(), Point))
			{
				Vec1DirH = Vec1DirH * -1;
			}
			FVector Vec2DirH = Line2->GetDirection();
			if (FVector::PointsAreSame(Line2->GetCoreLine()->GetEnd(), Point))
			{
				Vec2DirH = Vec2DirH * -1;
			}
			FVector Vec12DirH = (Vec1DirH + Vec2DirH).GetSafeNormal();
			FVector TestPoint = Point + Vec12DirH;
			if (!InRoom->GetRoom()->IsPointInRoom(TestPoint))
			{
				continue;
			}
			//
			TArray<TSharedPtr<FArmyWallLine>> LineCurArr;
			if (Line1->GetCoreLine()->Size() >= InEntity->Length)
			{
				LineCurArr.Add(Line1);
			}
			if (Line2->GetCoreLine()->Size() >= InEntity->Length)
			{
				LineCurArr.Add(Line2);
			}
			for (auto& LineCur : LineCurArr)
			{
				//排除存在窗户的墙体
				bool IsValidFlag = true;
				TArray<FObjectWeakPtr> InObjects;
				LineCur->GetAppendObjects(InObjects);
				for (auto& Model : InObjects)
				{
					TSharedPtr<FArmyHardware> ModelObject = StaticCastSharedPtr<FArmyHardware>(Model.Pin());
					if (ModelObject->GetType() == OT_Window || ModelObject->GetType() == OT_FloorWindow || ModelObject->GetType() == OT_RectBayWindow || ModelObject->GetType() == OT_TrapeBayWindow)
					{
						IsValidFlag = false;
						break;
					}
				}
				if (!IsValidFlag)
				{
					continue;
				}
				//
				FVector VecDirH = LineCur->GetDirection();
				if (FVector::PointsAreSame(LineCur->GetCoreLine()->GetEnd(), Point))
				{
					VecDirH = VecDirH * -1;
				}
				FVector VecDirV = InRoom->GetRoom()->GetWallLineNormal(LineCur);
				VecDirV = VecDirV.GetSafeNormal();
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
				InEntity->CopyTo(TempEntity);
				//
				TempEntity->Location = Point + VecDirH * (InEntity->Length / 2 < 40 ? 40 : InEntity->Length / 2) + VecDirV * 1/*+ VecDirV * InEntity->Width / 2*/;
				TempEntity->Rotation = VecDirV.Rotation() + FRotator(0, -90, 0);
				//
				TArray<FVector> BoxPointArr;
				FVector Location = TempEntity->Location;
				Location.Z = 0;
				BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2);
				BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2);
				BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width);
				BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width);
				FBox ActorBox = FBox(BoxPointArr);
				TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
				TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
				TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
				TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
				ComponentObjArr.Append(PillarArr);
				ComponentObjArr.Append(PackPipeArr);
				ComponentObjArr.Append(AirFlueArr);
				for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
				{
					FBox ModelBox = model->GetBounds();
					if (ModelBox.Intersect(ActorBox))
					{
						float MaxSize = (ModelBox.Max - Point).ProjectOnToNormal(VecDirH).Size();
						float MinSize = (ModelBox.Min - Point).ProjectOnToNormal(VecDirH).Size();
						FVector MaxPoint = MaxSize > MinSize ? ModelBox.Max : ModelBox.Min;
						float MaxPointSize = (MaxPoint - Point).ProjectOnToNormal(VecDirH).Size();
						FVector MaxLocation = Location + VecDirH * InEntity->Length / 2;
						float MaxLocationSize = (MaxLocation - Point).ProjectOnToNormal(VecDirH).Size();
						//
						if (MaxPointSize <= MaxLocationSize)
						{
							FBox OverlapBox = ModelBox.Overlap(ActorBox);
							float size = OverlapBox.GetSize().ProjectOnToNormal(VecDirH).Size() + 0;
							TempEntity->Location += VecDirH * size;
						}
						else {
							TempEntity->Location = Point + (MaxPoint - Point).ProjectOnToNormal(VecDirH) + InEntity->Length / 2 * VecDirH;
						}
					}
				}
				//
				if ((TempEntity->Location + VecDirH * (InEntity->Length / 2) - Point).ProjectOnToNormal(VecDirH).Size() <= LineCur->GetCoreLine()->Size())
				{
					EntityArr.Add(TempEntity);
				}
			}
		}
	}
	//检验碰撞
	TArray<TSharedPtr<FArmyModelEntity>> EntityArrTemp;
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallObjectCollision(EntityActor, InRoom) &&
				!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
				!CheckCeilingCollision(EntityActor, InRoom) &&
				!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom);
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				TempEntity->Location = TempEntity->Location + FVector(0, 0, 115);
				EntityArrTemp.Add(TempEntity);
			}
		}
	}
	//取离门最远的位置
	TArray<TSharedPtr<class FArmyHardware>> RoomDoorArr = InRoom->GetDoorsActor();
	if (RoomDoorArr.Num() > 0)
	{
		TSharedPtr<class FArmyHardware> Door = RoomDoorArr[0];
		float DisMax = FLT_MIN;
		TSharedPtr<FArmyModelEntity> EntityMax;
		for (auto& TempEntity : EntityArrTemp)
		{
			float dis = FVector::Distance(TempEntity->Location, Door->GetPos());
			if (dis > DisMax)
			{
				DisMax = dis;
				EntityMax = TempEntity;
			}
		}
		if (EntityMax.IsValid())
		{
			EntityArray.Add(EntityMax);
		}
	}
}

/** 卫生间 淋浴房 */
void FArmyAutoLayoutPatchBathRoom::CalcShower(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TSharedPtr<FArmyRoom> Room = InRoom->GetRoom();
	TArray<FVector> Points = Room->GetWorldPoints(true);
	for (auto Point : Points)
	{
		//
		TArray<TSharedPtr<FArmyWallLine>> WallLines;
		Room->GetSamePointWalls(Point, WallLines);
		if (WallLines.Num() >= 2)
		{
			TSharedPtr<FArmyWallLine> Line1 = WallLines[0];
			TSharedPtr<FArmyWallLine> Line2 = WallLines[1];
			//过滤L型的外墙角
			FVector Vec1DirH = Line1->GetDirection();
			if (FVector::PointsAreSame(Line1->GetCoreLine()->GetEnd(), Point))
			{
				Vec1DirH = Vec1DirH * -1;
			}
			FVector Vec2DirH = Line2->GetDirection();
			if (FVector::PointsAreSame(Line2->GetCoreLine()->GetEnd(), Point))
			{
				Vec2DirH = Vec2DirH * -1;
			}
			FVector Vec12DirH = (Vec1DirH + Vec2DirH).GetSafeNormal();
			FVector TestPoint = Point + Vec12DirH;
			if (!Room->IsPointInRoom(TestPoint))
			{
				continue;
			}
			//
			TArray<TSharedPtr<FArmyWallLine>> LineCurArr;
			if (Line1->GetCoreLine()->Size() >= InEntity->Length && Line2->GetCoreLine()->Size() >= InEntity->Width)
			{
				LineCurArr.Add(Line1);
			}
			if (Line2->GetCoreLine()->Size() >= InEntity->Length && Line1->GetCoreLine()->Size() >= InEntity->Width)
			{
				LineCurArr.Add(Line2);
			}
			for (auto& LineCur : LineCurArr)
			{
				FVector VecDirH = LineCur->GetDirection();
				if (FVector::PointsAreSame(LineCur->GetCoreLine()->GetEnd(), Point))
				{
					VecDirH = VecDirH * -1;
				}
				FVector VecDirV = Room->GetWallLineNormal(LineCur);
				VecDirV = VecDirV.GetSafeNormal();
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
				InEntity->CopyTo(TempEntity);
				//
				TempEntity->Location = Point + VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width / 2;
				TempEntity->Rotation = VecDirV.Rotation() + FRotator(0, -90, 0);
				//
				FVector VecPL = TempEntity->Location - Point;
				FVector VecPLUN = VecPL;
				VecPL = VecPL.GetSafeNormal();
				if (FVector::CrossProduct(VecPL, VecDirH).Z < 0)
				{
					//
					bool isCornerValid = true;
					TArray<FVector> BoxPointArr;
					FVector Location = TempEntity->Location;
					Location.Z = 0;
					BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2);
					BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2);
					BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2 - VecDirV * InEntity->Width / 2);
					BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width / 2);
					FBox ActorBox = FBox(BoxPointArr);
					TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
					TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
					TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
					TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
					ComponentObjArr.Append(PillarArr);
					ComponentObjArr.Append(PackPipeArr);
					ComponentObjArr.Append(AirFlueArr);
					for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
					{
						FBox ModelBox = model->GetBounds();
						if (ModelBox.Intersect(ActorBox))
						{
							float length = (ModelBox.Max - ModelBox.Min).X;
							float width = (ModelBox.Max - ModelBox.Min).Y;
							if (length * width > (InEntity->Length * InEntity->Width) / 16)
							{
								isCornerValid = false;
								break;
							}
						}
					}
					//
					if (isCornerValid)
					{
						TempEntity->Location += VecPLUN * -1;
						EntityArr.Add(TempEntity);
					}
				}
			}
		}
	}
	//检验碰撞：门窗、物件和顶面
	TArray<TSharedPtr<FArmyModelEntity>> EntityArrTemp;
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallObjectCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
								!CheckCeilingCollision(EntityActor, InRoom);

			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArrTemp.Add(TempEntity);
			}
		}
	}
	//取离门最远的位置
	TArray<TSharedPtr<class FArmyHardware>> RoomDoorArr = InRoom->GetDoorsActor();
	if (RoomDoorArr.Num() > 0)
	{
		TSharedPtr<class FArmyHardware> Door = RoomDoorArr[0];
		float DisMax = FLT_MIN;
		TSharedPtr<FArmyModelEntity> EntityMax;
		for (auto& TempEntity : EntityArrTemp)
		{
			float dis = FVector::Distance(TempEntity->Location, Door->GetPos());
			if (dis > DisMax)
			{
				DisMax = dis;
				EntityMax = TempEntity;
			}
		}
		if (EntityMax.IsValid())
		{
			EntityArray.Add(EntityMax);
		}
	}
}

/** 卫生间 浴室柜 */
void FArmyAutoLayoutPatchBathRoom::CalcCabinet(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TSharedPtr<FArmyWallLine> ToiletForwardWallLine;
	FVector ToiletForwardPosition = FVector::ZeroVector;
	TArray<TSharedPtr<FArmyModelEntity>> ToiletModelArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Closestool, ToiletModelArray);
	if (ToiletModelArray.Num() > 0)
	{
		TSharedPtr<FArmyModelEntity> ToiletModel = ToiletModelArray[0];
		ToiletForwardWallLine = FArmyDataTools::GetWallLineByModelInDir(*ToiletModel->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
		FArmyDataTools::PointIntervalGroundOnDirProjPoint(*ToiletModel->Actor, 0, ToiletForwardPosition, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
	}
	//
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TArray<FVector> Points = InRoom->GetRoom()->GetWorldPoints(true);
	for (auto Point : Points)
	{
		//
		TArray<TSharedPtr<FArmyWallLine>> WallLines;
		InRoom->GetRoom()->GetSamePointWalls(Point, WallLines);
		if (WallLines.Num() >= 2)
		{
			TSharedPtr<FArmyWallLine> Line1 = WallLines[0];
			TSharedPtr<FArmyWallLine> Line2 = WallLines[1];
			//过滤L型的外墙角
			FVector Vec1DirH = Line1->GetDirection();
			if (FVector::PointsAreSame(Line1->GetCoreLine()->GetEnd(), Point))
			{
				Vec1DirH = Vec1DirH * -1;
			}
			FVector Vec2DirH = Line2->GetDirection();
			if (FVector::PointsAreSame(Line2->GetCoreLine()->GetEnd(), Point))
			{
				Vec2DirH = Vec2DirH * -1;
			}
			FVector Vec12DirH = (Vec1DirH + Vec2DirH).GetSafeNormal();
			FVector TestPoint = Point + Vec12DirH;
			if (!InRoom->GetRoom()->IsPointInRoom(TestPoint))
			{
				continue;
			}
			//
			TArray<TSharedPtr<FArmyWallLine>> LineCurArr;
			if (Line1->GetCoreLine()->Size() >= InEntity->Length)
			{
				LineCurArr.Add(Line1);
			}
			if (Line2->GetCoreLine()->Size() >= InEntity->Length)
			{
				LineCurArr.Add(Line2);
			}
			for (auto& LineCur : LineCurArr)
			{
				//避开跟马桶正对
				if (ToiletForwardWallLine.IsValid() && LineCur == ToiletForwardWallLine)
				{
					if (FVector::PointsAreSame(LineCur->GetCoreLine()->GetStart(), Point))
					{
						if (FVector::Distance(ToiletForwardPosition, Point) < FVector::Distance(ToiletForwardPosition, LineCur->GetCoreLine()->GetEnd()))
						{
							continue;
						}
					}
					else
					{
						if (FVector::Distance(ToiletForwardPosition, Point) < FVector::Distance(ToiletForwardPosition, LineCur->GetCoreLine()->GetStart()))
						{
							continue;
						}
					}
				}
				FVector VecDirH = LineCur->GetDirection();
				if (FVector::PointsAreSame(LineCur->GetCoreLine()->GetEnd(), Point))
				{
					VecDirH = VecDirH * -1;
				}
				FVector VecDirV = InRoom->GetRoom()->GetWallLineNormal(LineCur);
				VecDirV = VecDirV.GetSafeNormal();
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
				InEntity->CopyTo(TempEntity);
				//
				TempEntity->Location = Point + VecDirH * (InEntity->Length / 2 + 1)  + VecDirV * 1/*+ VecDirV * InEntity->Width / 2*/ /*+ FVector(0, 0, InEntity->Height/2)*/;
				if (InEntity->ComponentId == AI_BathroomArk)
				{
					TempEntity->Location = Point + VecDirH * (InEntity->Length / 2 + 1)  + VecDirV * 1/*+ VecDirV * InEntity->Width / 2*/ /*+ FVector(0, 0, InEntity->Height/2)*/;
				}
				else if (InEntity->ComponentId == AI_BathroomArk_Wall)
				{
					TempEntity->Location = Point + VecDirH * (InEntity->Length / 2 + 1) + VecDirV * 1/*+ VecDirV * InEntity->Width / 2*/ + FVector(0, 0, 80);
				}
				TempEntity->Rotation = VecDirV.Rotation() + FRotator(0, -90, 0);
				//
				TArray<FVector> BoxPointArr;
				FVector Location = TempEntity->Location;
				Location.Z = 0;
				BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2);
				BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2);
				BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width);
				BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width);
				FBox ActorBox = FBox(BoxPointArr);
				TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
				TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
				TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
				TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
				ComponentObjArr.Append(PillarArr);
				ComponentObjArr.Append(PackPipeArr);
				ComponentObjArr.Append(AirFlueArr);
				for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
				{
					FBox ModelBox = model->GetBounds();
					if (ModelBox.Intersect(ActorBox))
					{
						float MaxSize = (ModelBox.Max - Point).ProjectOnToNormal(VecDirH).Size();
						float MinSize = (ModelBox.Min - Point).ProjectOnToNormal(VecDirH).Size();
						FVector MaxPoint = MaxSize > MinSize ? ModelBox.Max : ModelBox.Min;
						float MaxPointSize = (MaxPoint - Point).ProjectOnToNormal(VecDirH).Size();
						FVector MaxLocation = Location + VecDirH * InEntity->Length / 2;
						float MaxLocationSize = (MaxLocation - Point).ProjectOnToNormal(VecDirH).Size();
						//
						if (MaxPointSize <= MaxLocationSize)
						{
							FBox OverlapBox = ModelBox.Overlap(ActorBox);
							float size = OverlapBox.GetSize().ProjectOnToNormal(VecDirH).Size() + 0;
							TempEntity->Location += VecDirH * size;
						} else {
							TempEntity->Location = Point + (MaxPoint - Point).ProjectOnToNormal(VecDirH) + InEntity->Length / 2 * VecDirH;
						}
					}
				}
				if ((TempEntity->Location + VecDirH * (InEntity->Length / 2) - Point).ProjectOnToNormal(VecDirH).Size() <= LineCur->GetCoreLine()->Size())
				{
					EntityArr.Add(TempEntity);
				}
			}
		}
	}
	//检验碰撞：门窗、物件和顶面
	TSet<AActor*> OtherActorSet;
	TArray<TSharedPtr<FArmyModelEntity>> FaceclothHolderArray;
	InRoom->GetModelEntityArrayByComponentId(AI_FaceclothHolder, FaceclothHolderArray);//毛巾架
	for (auto& Entity : FaceclothHolderArray)
	{
		if (Entity->Actor != nullptr)
		{
			OtherActorSet.Add(Entity->Actor);
		}
	}
	TArray<TSharedPtr<FArmyModelEntity>> EntityArrTemp;
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallObjectCollision(EntityActor, InRoom) &&
				!CheckOtherActorCollision(EntityActor, InRoom, OtherActorSet) &&
				!CheckCeilingCollision(EntityActor, InRoom) &&
				!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom);
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArrTemp.Add(TempEntity);
			}
		}
	}
	//取离门最近的位置
	TArray<TSharedPtr<class FArmyHardware>> RoomDoorArr = InRoom->GetDoorsActor();
	if (RoomDoorArr.Num() > 0)
	{
		TSharedPtr<class FArmyHardware> Door = RoomDoorArr[0];
		float DisMin = FLT_MAX;
		TSharedPtr<FArmyModelEntity> EntityMin;
		for (auto& TempEntity : EntityArrTemp)
		{
			float dis = FVector::Distance(TempEntity->Location, Door->GetPos());
			if (dis < DisMin)
			{
				DisMin = dis;
				EntityMin = TempEntity;
			}
		}
		if (EntityMin.IsValid())
		{
			EntityArray.Add(EntityMin);
		}
	}
}

/** 卫生间 洗衣机 */
void FArmyAutoLayoutPatchBathRoom::CalcWasher(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TArray<FVector> Points = InRoom->GetRoom()->GetWorldPoints(true);
	for (auto Point : Points)
	{
		//
		TArray<TSharedPtr<FArmyWallLine>> WallLines;
		InRoom->GetRoom()->GetSamePointWalls(Point, WallLines);
		if (WallLines.Num() >= 2)
		{
			TSharedPtr<FArmyWallLine> Line1 = WallLines[0];
			TSharedPtr<FArmyWallLine> Line2 = WallLines[1];
			//过滤L型的外墙角
			FVector Vec1DirH = Line1->GetDirection();
			if (FVector::PointsAreSame(Line1->GetCoreLine()->GetEnd(), Point))
			{
				Vec1DirH = Vec1DirH * -1;
			}
			FVector Vec2DirH = Line2->GetDirection();
			if (FVector::PointsAreSame(Line2->GetCoreLine()->GetEnd(), Point))
			{
				Vec2DirH = Vec2DirH * -1;
			}
			FVector Vec12DirH = (Vec1DirH + Vec2DirH).GetSafeNormal();
			FVector TestPoint = Point + Vec12DirH;
			if (!InRoom->GetRoom()->IsPointInRoom(TestPoint))
			{
				continue;
			}
			//
			TArray<TSharedPtr<FArmyWallLine>> LineCurArr;
			if (Line1->GetCoreLine()->Size() >= InEntity->Length)
			{
				LineCurArr.Add(Line1);
			}
			if (Line2->GetCoreLine()->Size() >= InEntity->Length)
			{
				LineCurArr.Add(Line2);
			}
			for (auto& LineCur : LineCurArr)
			{
				FVector VecDirH = LineCur->GetDirection();
				if (FVector::PointsAreSame(LineCur->GetCoreLine()->GetEnd(), Point))
				{
					VecDirH = VecDirH * -1;
				}
				FVector VecDirV = InRoom->GetRoom()->GetWallLineNormal(LineCur);
				VecDirV = VecDirV.GetSafeNormal();
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
				InEntity->CopyTo(TempEntity);
				//
				TempEntity->Location = Point + VecDirH * (InEntity->Length / 2 + 1) + VecDirV * (InEntity->Width / 2 + 1) ;
				TempEntity->Rotation = VecDirV.Rotation() + FRotator(0, -90, 0);
				//
				TArray<FVector> BoxPointArr;
				FVector Location = TempEntity->Location;
				Location.Z = 0;
				BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2);
				BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2);
				BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2 - VecDirV * InEntity->Width / 2);
				BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2 + VecDirV * InEntity->Width / 2);
				FBox ActorBox = FBox(BoxPointArr);
				TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
				TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
				TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
				TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
				ComponentObjArr.Append(PillarArr);
				ComponentObjArr.Append(PackPipeArr);
				ComponentObjArr.Append(AirFlueArr);
				for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
				{
					FBox ModelBox = model->GetBounds();
					if (ModelBox.Intersect(ActorBox))
					{
						float MaxSize = (ModelBox.Max - Point).ProjectOnToNormal(VecDirH).Size();
						float MinSize = (ModelBox.Min - Point).ProjectOnToNormal(VecDirH).Size();
						FVector MaxPoint = MaxSize > MinSize ? ModelBox.Max : ModelBox.Min;
						float MaxPointSize = (MaxPoint - Point).ProjectOnToNormal(VecDirH).Size();
						FVector MaxLocation = Location + VecDirH * InEntity->Length / 2;
						float MaxLocationSize = (MaxLocation - Point).ProjectOnToNormal(VecDirH).Size();
						//
						if (MaxPointSize <= MaxLocationSize)
						{
							FBox OverlapBox = ModelBox.Overlap(ActorBox);
							float size = OverlapBox.GetSize().ProjectOnToNormal(VecDirH).Size() + 0;
							TempEntity->Location += VecDirH * size;
						}
						else {
							TempEntity->Location = Point + (MaxPoint - Point).ProjectOnToNormal(VecDirH) + InEntity->Length / 2 * VecDirH + VecDirV * (InEntity->Width / 2 + 1);
						}
					}
				}
				//
				if ((TempEntity->Location + VecDirH * (InEntity->Length / 2) - Point).ProjectOnToNormal(VecDirH).Size() <= LineCur->GetCoreLine()->Size())
				{
					EntityArr.Add(TempEntity);
				}
			}
		}
	}
	
	//检验碰撞：门窗和物件
	TArray<TSharedPtr<FArmyModelEntity>> EntityUnIntersectArr;
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallObjectCollision(EntityActor, InRoom) &&
				!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
				!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom);
			if (isNotIntersect)
			{
				bool isIntersectExp = false;
				TArray < TSharedPtr<FArmyModelEntity>> ModelArr;
				InRoom->GetAllModels(ModelArr);
				FBox InActorBox = GVC->GetActorCollisionBox(EntityActor);
				FVector InActorForward = FArmyDataTools::GetModelForwardVector(EntityActor);
				//正方向扩展30cm，作为净空
				FVector InActorBoxCenter = InActorBox.GetCenter();
				FVector VecCMin = InActorBox.Min - InActorBoxCenter;
				FVector VecCMax = InActorBox.Max - InActorBoxCenter;
				if ((VecCMin | InActorForward) > 0)
				{
					InActorBox.Min += InActorForward * 30;
				}
				if ((VecCMax | InActorForward) > 0)
				{
					InActorBox.Max += InActorForward * 30;
				}
				for (TSharedPtr<FArmyModelEntity>& MapPair : ModelArr)
				{
					AActor* ModelActor = MapPair->Actor;
					if (ModelActor)
					{
						FBox ModelActorBox = GVC->GetActorCollisionBox(ModelActor);
						if (InActorBox.Intersect(ModelActorBox))
						{
							isIntersectExp = true;
							break;
						}
					}
				}
				//
				if (!isIntersectExp)
				{
					float Z = TempEntity->Location.Z;
					TempEntity->Location.Z = 115;
					AActor* EntityActor1 = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
					if (EntityActor1)
					{
						bool isNotIntersect1 = !CheckWallObjectCollision(EntityActor1, InRoom) &&
							!CheckOtherActorCollision(EntityActor1, InRoom, TSet<AActor*>()) &&
							!CheckWallCollision(EntityActor1, InRoom) &&
							!CheckWallCornerCollision(EntityActor1, InRoom);
						if (isNotIntersect1)
						{
							TempEntity->Location.Z = Z;
							EntityUnIntersectArr.Add(TempEntity);
						}
						EntityActor1->Destroy();
					}
				}
			}
			EntityActor->Destroy();
		}
	}

	//检验洗衣机朝向，洗衣机正面的射线最短（正面射线最短，指向长边）
	TSharedPtr<FArmyModelEntity> MinModelEntity;
	float MinDis = FLT_MAX;
	for (auto& TempEntity : EntityUnIntersectArr)
	{
		float Height = InEntity->Height * 0.5;
		FVector Location = TempEntity->Location + FVector(0, 0, Height);
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			TSharedPtr<FArmyWallLine> WMForwardWall = FArmyDataTools::GetWallLineByModelInDir(*EntityActor, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
			if (WMForwardWall.IsValid())
			{
				FVector Position = FVector::ZeroVector;
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*EntityActor, Height, Position, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD))
				{
					float Dis = FVector::Dist2D(Location, Position);
					if (Dis < MinDis)
					{
						MinDis = Dis;
						MinModelEntity = TempEntity;
					}
				}
			}
			EntityActor->Destroy();
		}
	}
	if (MinModelEntity.IsValid())
	{
		EntityArray.Add(MinModelEntity);
	}
	else
	{
		if (EntityUnIntersectArr.Num() > 0)
		{
			EntityArray.Add(EntityUnIntersectArr[0]);
		}
	}
}

/*------------------------------------ 卫生间方法实现 End ------------------------------------*/


/*------------------------------------ 阳台方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchBalcony::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 阳台 边几（软装） */
	case AI_SideTable:
		CalcSideTableLayout(InEntity, InRoom, EntityArray);
		break;

		/** 阳台 休闲椅（软装） */
	case AI_LeisureChair:
		CalcLeisureChairLayout(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

// 阳台 边几（软装）
void FArmyAutoLayoutPatchBalcony::CalcSideTableLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//修改location和rotation
	float window_length = 0;
	TSharedPtr<FArmyHardware> MaxWindow;
	TSharedPtr<FArmyWallLine> MaxWallLine;
	TArray<TSharedPtr<FArmyWallLine>> WallLines = InRoom->GetRoom()->GetWallLines();
	for (auto& WallLine : WallLines)
	{
		TArray<FObjectWeakPtr> WallModels;
		WallLine->GetAppendObjects(WallModels);
		for (auto& Model : WallModels)
		{
			TSharedPtr<FArmyHardware> ModelObject = StaticCastSharedPtr<FArmyHardware>(Model.Pin());
			if (ModelObject->GetType() == OT_Window || ModelObject->GetType() == OT_FloorWindow || ModelObject->GetType() == OT_RectBayWindow || ModelObject->GetType() == OT_TrapeBayWindow)
			{
				float length = ModelObject->GetLength();
				if (length > window_length)
				{
					window_length = length;
					MaxWindow = ModelObject;
					MaxWallLine = WallLine;
				}
			}
		}
	}
	//
	if (MaxWallLine.IsValid())
	{
		TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByHardware(MaxWindow, InRoom->GetRoom());
		FVector Direction = InRoom->GetRoom()->GetWallLineNormal(WallLine);
		Direction = Direction.GetSafeNormal();
		//
		int32 max_index = WallLines.Find(MaxWallLine);
		TSharedPtr<FArmyWallLine> PreMaxWallLine = max_index >= 1 ? WallLines[max_index - 1] : WallLines.Last();
		TSharedPtr<FArmyWallLine> NextMaxWallLine = max_index < WallLines.Num() - 1 ? WallLines[max_index + 1] : WallLines[0];
		float min_length = PreMaxWallLine->GetCoreLine()->Size() < NextMaxWallLine->GetCoreLine()->Size() ? PreMaxWallLine->GetCoreLine()->Size() : NextMaxWallLine->GetCoreLine()->Size();
		float wall_length = MaxWallLine->GetCoreLine()->Size();
		float entity_length = InEntity->Width < InEntity->Length ? InEntity->Length : InEntity->Width;
		float entity_length_min = InEntity->Width < InEntity->Length ? InEntity->Width : InEntity->Length;
		if (wall_length >= entity_length && min_length >= entity_length_min)
		{
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			//
			TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);//设置Rotation
			//
			FVector Centre = (MaxWallLine->GetCoreLine()->GetStart() + MaxWallLine->GetCoreLine()->GetEnd()) / 2;
			Centre += Direction * (min_length / 2);
			Centre.Z = 0;
			TempEntity->Location = Centre;//设置Location
			
			//碰撞检测：墙体、柱子类和物件
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckWallCollision(EntityActor, InRoom) &&
					!CheckWallCornerCollision(EntityActor, InRoom) &&
					!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()))
				{
					EntityArray.Add(TempEntity);
				}
				EntityActor->Destroy();
			}
		}
	}
}

// 阳台 休闲椅（软装）
void FArmyAutoLayoutPatchBalcony::CalcLeisureChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TSet<AActor*> OtherActorSet;
	//获取边几
	TArray<TSharedPtr<class FArmyModelEntity>> SideTableArray;
	InRoom->GetModelEntityArrayByComponentId(AI_SideTable, SideTableArray);
	// 布置两个休闲椅
	for (auto &SideTable : SideTableArray)
	{
		FVector tableSize = FArmyDataTools::GetContextItemSize(SideTable->FurContentItem);
		FVector tablePos = SideTable->Actor->GetActorLocation();
		FVector Left = FArmyDataTools::GetModelLeftVector(SideTable->Actor);
		FVector Right = -Left;
		//
		FRotator RotationConst = FRotator(0, -90, 0);
		TArray<FVector> DirArr = {Left, Right};
		for (auto& Dir : DirArr)
		{
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			TempEntity->Location = tablePos + Dir * (tableSize.X / 2 + InEntity->Width / 2 + 10);
			TempEntity->Rotation = (Dir * -1).Rotation() + RotationConst;
			EntityArr.Add(TempEntity);
		}
		//
		OtherActorSet.Add(SideTable->Actor);
	}

	//碰撞检测：墙体、柱子类和物件
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom) &&
				!CheckOtherActorCollision(EntityActor, InRoom, OtherActorSet))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

/*------------------------------------ 阳台方法实现 End ------------------------------------*/


/*------------------------------------ 餐厅方法实现 Start ------------------------------------*/

void FArmyAutoLayoutPatchDiningroom::CalcLayoutPatch(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 餐厅 椅子 */
	case AI_DiningChair:
		CalcChairLayout(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

// 餐厅 椅子
void FArmyAutoLayoutPatchDiningroom::CalcChairLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	//
	TSet<AActor*> OtherActorSet;
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DiningTable, DeskArray);
	for (auto &desk : DeskArray)
	{
		//获取正确的长和宽，以及前和左方向
		FVector Size = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector Forward = FArmyDataTools::GetModelForwardVector(desk->Actor);
		FVector Left = FArmyDataTools::GetModelLeftVector(desk->Actor);
		FVector Backward = Forward * -1;
		FVector Right = Left * -1;
		FVector DeskPos = desk->Actor->GetActorLocation();
		float ChairGap = InEntity->Width * 0.25;
		FRotator RotationConst = FRotator(0, -90, 0);
		float ChairLength = InEntity->Length;
		// 1 判断桌子的类型
		if (fabs(Size.X - Size.Y) < 10)
		{
			// 圆桌或者方桌，前后左右各方一张椅子
			TArray<FVector> DirectionArr = {Forward, Backward, Left, Right};
			TArray<double> DirectionDisArr = { Size.Y * 0.5 + ChairGap, Size.Y * 0.5 + ChairGap, Size.X * 0.5 + ChairGap, Size.X * 0.5 + ChairGap };
			for (int i = 0; i < DirectionArr.Num(); i++)
			{
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				TempEntity->Location = DeskPos + DirectionDisArr[i] * DirectionArr[i];
				TempEntity->Rotation = (DirectionArr[i] * -1).Rotation() + RotationConst;
				EntityArr.Add(TempEntity);
			}
		}
		else
		{
			TArray<FVector> DirectionArr = { Forward, Backward };
			TArray<double> DirectionDisArr = { Size.Y * 0.5 + ChairGap, Size.Y * 0.5 + ChairGap };
			float ChairHGap = 10;
			//
			TArray<FVector> DirectionHDisArr;
			float max = 1;
			while (max * ChairLength + (max - 1) * ChairHGap <= Size.X - 20) max++;
			max--;//max为放置椅子的最大数量
			FVector LeftMax = (((max - 1) / 2) * (ChairHGap + ChairLength)) * Left;
			for (int n = 0; n < max; n++)
			{
				DirectionHDisArr.Add(LeftMax + n * (ChairHGap + ChairLength) * Right);
			}
			for (int i = 0; i < DirectionArr.Num(); i++)
			{
				for (int j = 0; j < DirectionHDisArr.Num(); j++)
				{
					TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
					InEntity->CopyTo(TempEntity);
					TempEntity->Location = DeskPos + DirectionDisArr[i] * DirectionArr[i] + DirectionHDisArr[j];
					TempEntity->Rotation = (DirectionArr[i] * -1).Rotation() + RotationConst;
					EntityArr.Add(TempEntity);
				}
			}
		}
		//
		OtherActorSet.Add(desk->Actor);
	}

	//
	//碰撞检测：墙体和柱子类
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCollision(EntityActor, InRoom) && 
				!CheckWallCornerCollision(EntityActor, InRoom))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

/*------------------------------------ 餐厅方法实现 End ------------------------------------*/

