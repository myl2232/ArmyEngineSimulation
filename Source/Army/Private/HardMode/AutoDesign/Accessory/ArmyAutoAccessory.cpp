#include "ArmyAutoAccessory.h"

#include "ArmyViewportClient.h"
#include "ArmySecurityDoor.h"
#include "ArmySceneData.h"
#include "ArmyWallLine.h"
#include "ArmyWindow.h"
#include "ArmyRegion.h"

DEFINE_LOG_CATEGORY(AutoAccessory);

const TSharedRef<FArmyAccessoryFactory>& FArmyAccessoryFactory::Get()
{
	static const TSharedRef<FArmyAccessoryFactory> Instance = MakeShareable(new FArmyAccessoryFactory);
	return Instance;
}

TSharedRef<FArmyAutoAccessory>& FArmyAccessoryFactory::GetAccessoryRoom(TSharedPtr<FArmyRoomEntity>& InRoom)
{
	UE_LOG(AutoAccessory, Log, TEXT("RoomName: %s"), *(InRoom->GetRoom()->GetSpaceName()));

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
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryBedroom);
		return accessory;
	}

	/** 书房 */
	case RT_StudyRoom:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryStudyroom);
		return accessory;
	}

	/** 客厅 */
	case RT_LivingRoom:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryLivingroom);
		return accessory;
	}

	/** 卫生间 */
	case RT_Bathroom:
	case RT_Bathroom_Mester:
	case RT_Bathroom_Quest:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryBathRoom);
		return accessory;
	}

	/** 阳台 */
	case RT_Balcony:
	case RT_Balcony_After:
	case RT_Balcony_Bedroom:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryBalcony);
		return accessory;
	}

	/** 厨房 */
	case RT_Kitchen:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryKitchen);
		return accessory;
	}

	/** 餐厅 */
	case RT_DiningRoom:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryDiningroom);
		return accessory;
	}

	/** 玄关 */
	case RT_Hallway:
	{
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryHallway);
		return accessory;
	}
	default:
		//默认空间
		static TSharedRef<FArmyAutoAccessory> accessory = MakeShareable(new FArmyAutoAccessoryRoom);
		return accessory;
	}
}

/*------------------------------------ 基类方法实现 Start ------------------------------------*/

//工具方法
//获取进光墙
TSharedPtr<FArmyWallLine> FArmyAutoAccessory::LightAreaMaxWallLine(const TSharedPtr<FArmyRoom> Room)
{
	//获取进光墙，包含最大总面积的窗户或垭口
	TSharedPtr<FArmyWallLine> WallLineLightMax;
	float LightAreaMax = 0;
	TArray<TSharedPtr<FArmyWallLine>> WallLineArr = Room->GetWallLines();
	for (auto& WallLine : WallLineArr)
	{
		//
		float LightAreaValue = 0;
		TArray<FObjectWeakPtr> WallModels;//墙上的模型（门和窗）
		WallLine->GetAppendObjects(WallModels);
		for (auto& Model : WallModels)
		{
			TSharedPtr<FArmyHardware> ModelObject = StaticCastSharedPtr<FArmyHardware>(Model.Pin());
			//窗户
			if (ModelObject->GetType() == OT_Window || ModelObject->GetType() == OT_FloorWindow || ModelObject->GetType() == OT_RectBayWindow || ModelObject->GetType() == OT_TrapeBayWindow)
			{
				float length = ModelObject->GetLength();
				float height = ModelObject->GetHeight();
				LightAreaValue += length * height;
			}
			//透光的门或者洞
			if (ModelObject->GetType() == OT_DoorHole || ModelObject->GetType() == OT_Pass || ModelObject->GetType() == OT_NewPass || ModelObject->GetType() == OT_SlidingDoor)
			{
				float length = ModelObject->GetLength();
				float height = ModelObject->GetHeight();
				LightAreaValue += length * height;
			}
		}
		if (LightAreaValue > LightAreaMax)
		{
			LightAreaMax = LightAreaValue;
			WallLineLightMax = WallLine;
		}
	}

	return WallLineLightMax;
}

// 检测某个模型与柱子、包立管、风道的碰撞，碰撞了返回true
bool FArmyAutoAccessory::CheckWallCornerCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
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
bool FArmyAutoAccessory::CheckCeilingCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
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
bool FArmyAutoAccessory::CheckWallCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
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
bool FArmyAutoAccessory::CheckWallObjectCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom)
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
bool FArmyAutoAccessory::CheckOtherActorCollision(AActor* ActorItr, TSharedPtr<FArmyRoomEntity> InRoom, const TSet<AActor*>& IgnoreActorArr)
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
	//是否存在其他模型，如衣柜等
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
//工具方法结束

//房间吊灯和吸顶灯 Test OK
void FArmyAutoAccessory::CalcLampMain(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//主灯：吊灯、吸顶灯、集成LED灯，通用布置函数
	FVector Center = FVector::ZeroVector;
	TArray<FVector> PointArr;
	TArray<FVector> Points = InRoom->GetRoom()->GetWorldPoints(true);
	if (Points.Num() == 4)
	{
		PointArr.Add(Points[0]);
		PointArr.Add(Points[1]);
		PointArr.Add(Points[2]);
		Center = InRoom->GetRoom()->GetBounds().GetCenter();
	}
	else if (Points.Num() > 4)
	{
		PointArr.Add(Points[0]);
		PointArr.Add(Points[1]);
		PointArr.Add(Points[2]);
		FBox Box;
		FArmyDataTools::GetMaxQuadInPolygon(Points, Center, PointArr, Box);
	}
	if (PointArr.Num() >= 3)
	{
		//找出长的墙
		FVector vecCP = PointArr[0] - PointArr[1];
		FVector vecCN = PointArr[2] - PointArr[1];
		FVector vecMax = vecCP;
		if (vecCN.Size() > vecCP.Size())
		{
			vecMax = vecCN;
		}
		//矩形中心点
		Center.Z = FArmySceneData::WallHeight - InRoom->GetCeilingDropOff();//减去下吊高度

		TSharedPtr<FArmyModelEntity> Entity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(Entity);
		Entity->Location = InRoom->GetCeilingDropLampPos();
		//判断是否有效（基于硬装是否勾选）
		if (FVector::PointsAreSame(Entity->Location, FVector::ZeroVector) || Entity->Location.Z == 0 || Entity->Location.Z < FArmySceneData::WallHeight / 2)
		{
			//无效，使用默认值
			Entity->Location = Center;
		}
		Entity->Rotation = vecMax.Rotation();
		
		//碰撞检测：主灯由硬装进行计算处理，软配不进行碰撞检测
		EntityArray.Add(Entity);
	}
}

//房间普通窗帘
void FArmyAutoAccessory::CalcCurtainStd(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	// 普通窗帘（除了卫生间，其他空间都一样）
	TArray<TSharedPtr<FArmyHardware>> WindowArr = InRoom->GetWindowsActor();
	for (auto TempWindow : WindowArr)
	{
		TSharedPtr<FArmyWindow> Window = StaticCastSharedPtr<FArmyWindow>(TempWindow);
		//无效的窗户
		if (Window->HoleWindow == nullptr)
		{
			continue;
		}
		//结束
		//暂时只支持标准窗、落地窗、矩形飘窗和梯形飘窗
		//OT_Window, OT_FloorWindow, OT_RectBayWindow, OT_TrapeBayWindow,
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempEntity);
		//
		TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByHardware(Window, InRoom->GetRoom());
		FVector Direction = InRoom->GetRoom()->GetWallLineNormal(WallLine);
		Direction = Direction.GetSafeNormal();
		//
		//先旋转后平移
		float height = FArmySceneData::WallHeight;
		TempEntity->Scale3D = FVector((Window->GetLength() + 20) / InEntity->Length, 1, (height - 10 - InRoom->GetCeilingDropOff() - InRoom->GetCrownMouldingHeight()) / InEntity->Height);
		FVector Postion = Window->GetPos();
		//TempEntity->Location = FVector(Postion.X, Postion.Y, (height - 10 - InRoom->GetCeilingDropOff() - InRoom->GetCrownMouldingHeight())) + (InEntity->Width / 2 + Window->GetWidth() / 2 + 5) * Direction;
		TempEntity->Location = FVector(Postion.X, Postion.Y, 0) + (InEntity->Width / 2 + Window->GetWidth() / 2 + 5) * Direction;
		TempEntity->Rotation = Direction.Rotation().Add(0, -90, 0);
		//
		//碰撞检测：不检测碰撞
		EntityArray.Add(TempEntity);
	}
}

/*------------------------------------ 基类方法实现 End ------------------------------------*/

/*------------------------------------ 默认空间方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryRoom::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//不做任何分发
}

/*------------------------------------ 默认方法实现 End ------------------------------------*/

/*------------------------------------ 卧室方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryBedroom::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 卧室 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 窗帘 */
	case AI_CurtainStd:
		CalcCurtainStd(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 挂画 */
	case AI_Titanic:
		CalcTitanic(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 地毯 */
	case AI_Carpet:
		CalcCarpetLayout(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 台灯 */
	case AI_LampTable:
		CalcLampTable(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 壁灯 */
	case AI_LampWall:
		CalcLampWall(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 壁挂空调 */
	case AI_AirConditionedHang:
		CalcAirConditionedHang(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 壁挂电视 */
	case AI_TVWall:
		CalcTVWall(InEntity, InRoom, EntityArray);
		break;

		/** 卧室 立式电视 */
	case AI_TVVert:
		CalcTVVert(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}

}

//卧室地毯
void FArmyAutoAccessoryBedroom::CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 单人床地毯摆放 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> SingleBedArray;
	InRoom->GetModelEntityArrayByComponentId(AI_SingleBed, SingleBedArray);
	/** 双人床地毯摆放 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> DoubleBedArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DoubleBed, DoubleBedArray);
	//
	TArray<TSharedPtr<class FArmyModelEntity>> BedArray;
	BedArray.Append(SingleBedArray);
	BedArray.Append(DoubleBedArray);

	for (auto& Bed : BedArray)
	{
		/** 构造出新的Entity */
		TSharedPtr<FArmyModelEntity> TempCarpetEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempCarpetEntity);

		/** 进行Rotation转换 */
		TempCarpetEntity->Rotation = Bed->Rotation;
		FVector TempRotationY = Bed->Rotation.RotateVector(FVector(0, 1, 0));

		/** bed.center + bed.width*0.5f + 50cm - tempentity.width*0.5f */
		TempCarpetEntity->Location = Bed->Location + (Bed->Width*0.5f + 50.0f - TempCarpetEntity->Width*0.5f)*TempRotationY;

		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempCarpetEntity->FurContentItem, TempCarpetEntity->Location, TempCarpetEntity->Rotation, TempCarpetEntity->Scale3D);
		if (EntityActor)
		{
			/** 初始化Entity中的Actor */
			TempCarpetEntity->Actor = EntityActor;

			/** 计算地毯能否调整到房间内 */
			bool bInRoom = FArmyDataTools::CalModelAdjustToRoom(TempCarpetEntity, InRoom);
			if (bInRoom)
			{
				TempCarpetEntity->Location = EntityActor->GetActorLocation();
				TempCarpetEntity->Rotation = EntityActor->GetActorRotation();
				TempCarpetEntity->Scale3D = EntityActor->GetActorScale3D();

				//碰撞检测：不检测碰撞
				EntityArray.Emplace(TempCarpetEntity);
			}

			TempCarpetEntity->Actor->Destroy();
		}
	}
}

// 卧室 台灯
void FArmyAutoAccessoryBedroom::CalcLampTable(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//1、卧室床头柜台灯
	TArray<TSharedPtr<class FArmyModelEntity>> TableLArray;
	InRoom->GetModelEntityArrayByComponentId(AI_BedsideTable_L, TableLArray);//AI_BedsideTable_L
	TArray<TSharedPtr<class FArmyModelEntity>> TableRArray;
	InRoom->GetModelEntityArrayByComponentId(AI_BedsideTable_R, TableRArray);//AI_BedsideTable_R
	TArray<TSharedPtr<class FArmyModelEntity>> TableArray;
	TableArray.Append(TableLArray);
	TableArray.Append(TableRArray);
	for (auto &desk : TableArray)
	{
		FVector Position = FVector::ZeroVector;
		FVector Size = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FArmyDataTools::PosOnDirectionOfModel(*desk->Actor, Size.Z*1.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		//
		TempEntity->Location = Position;
		TempEntity->Rotation = desk->Actor->GetActorRotation();
		//
		//碰撞检测：不检测碰撞
		EntityArray.Add(TempEntity);
	}

	//2、 卧室书桌台灯
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Desk, DeskArray);
	for (auto &desk : DeskArray)
	{
		//获取床头柜子上面的距离床头柜一半高度
		FVector Size = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector Right = FArmyDataTools::GetModelLeftVector(desk->Actor) * -1;
		FVector Back = FArmyDataTools::GetModelForwardVector(desk->Actor) * -1;

		FVector Position = FVector::ZeroVector;
		FArmyDataTools::PosOnDirectionOfModel(*desk->Actor, Size.Z, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		Position += Back * (Size.Y / 2 - InEntity->Width / 2 - 10);
		Position += Right * (Size.X / 2 - InEntity->Length / 2 - 10);

		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		TempEntity->Location = Position;
		TempEntity->Rotation = desk->Actor->GetActorRotation();

		//碰撞检测：不检测碰撞
		EntityArray.Add(TempEntity);
	}
}

// 卧室 壁灯
void FArmyAutoAccessoryBedroom::CalcLampWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{

	TArray<TSharedPtr<class FArmyModelEntity>> DeskLArray;
	InRoom->GetModelEntityArrayByComponentId(AI_BedsideTable_L, DeskLArray);//AI_BedsideTable_L
	TArray<TSharedPtr<class FArmyModelEntity>> DeskRArray;
	InRoom->GetModelEntityArrayByComponentId(AI_BedsideTable_R, DeskRArray);//AI_BedsideTable_R
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	DeskArray.Append(DeskLArray);
	DeskArray.Append(DeskRArray);

	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	for (auto &desk : DeskArray)
	{
		TSharedPtr<FArmyWallLine> DeskBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*desk->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (DeskBackWardWall.IsValid())
		{
			FVector Direction = InRoom->GetRoom()->GetWallLineNormal(DeskBackWardWall);
			Direction = Direction.GetSafeNormal();
			FVector Position = FVector::ZeroVector;
			float Height = 160;//距地1.6m
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*desk->Actor, Height, Position))
			{
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				TempEntity->Location = Position;
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				//
				EntityArr.Add(TempEntity);
			}
		}
	}

	//检测碰撞：检测柱子类碰撞
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallCornerCollision(EntityActor, InRoom) && 
				!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
				!CheckWallObjectCollision(EntityActor, InRoom))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

//卧室挂画
void FArmyAutoAccessoryBedroom::CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 卧室挂画 @纪仁泽 */
	/*高度小于1300的床，挂画底部距地1400；高度大于1300的床，挂画底部距床顶面高100mm,若挂画离顶高度小于300则不布置*/
	TArray<TSharedPtr<class FArmyModelEntity>> DoubleBedArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DoubleBed, DoubleBedArray);
	for (auto& DoubleBed : DoubleBedArray)
	{
		/** 床背面的墙长度需要大于100cm */
		TSharedPtr<FArmyWallLine> BedBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*DoubleBed->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (BedBackWardWall.IsValid() && BedBackWardWall->GetCoreLine()->Size() >= 100.f)
		{
			/** bed.height + 15 + temp_entity.height*0.5f */
			FVector TempCenter(0, 0, 0);
			float Dist = DoubleBed->Height + 10 + InEntity->Height*0.5f;
			float BedHeight = DoubleBed->Height;
			if (BedHeight <= 130)
			{
				Dist = 140 + InEntity->Height*0.5f;
			}
			else if (BedHeight > 130)
			{
				Dist = DoubleBed->Height + 10 + InEntity->Height*0.5f;
			}
			/** 床背后的墙体 */
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*DoubleBed->Actor, Dist, TempCenter))
			{
				TSharedPtr<FArmyModelEntity> TempTitanicEntity = MakeShareable(new FArmyModelEntity);
				InEntity->CopyTo(TempTitanicEntity);
				TempTitanicEntity->Rotation = DoubleBed->Rotation;
				TempTitanicEntity->Location = TempCenter;
				//
				//检测碰撞：检测柱子类、墙体、门窗、顶面、其他墙物件碰撞（全部5种碰撞类型）
				AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntity->FurContentItem, TempTitanicEntity->Location, TempTitanicEntity->Rotation, TempTitanicEntity->Scale3D);
				if (EntityActor)
				{
					FBox EntityBox = GVC->GetActorCollisionBox(EntityActor);
					if (!CheckWallCornerCollision(EntityActor, InRoom) &&
						!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
						!CheckWallObjectCollision(EntityActor, InRoom) &&
						!CheckWallCollision(EntityActor, InRoom) &&
						!CheckCeilingCollision(EntityActor, InRoom) &&
						(FArmySceneData::WallHeight - 30) >= (EntityBox.GetCenter().Z + EntityBox.GetSize().Z*0.5f))
					{
						TempTitanicEntity->Location = EntityActor->GetActorLocation();
						TempTitanicEntity->Rotation = EntityActor->GetActorRotation();
						TempTitanicEntity->Scale3D = EntityActor->GetActorScale3D();
						EntityArray.Emplace(TempTitanicEntity);
					}
					EntityActor->Destroy();
				}
			}
		}
	}
}

// 卧室 壁挂空调
void FArmyAutoAccessoryBedroom::CalcAirConditionedHang(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//床
	TSharedPtr<FArmyModelEntity> BedEntity;
	TArray<TSharedPtr<FArmyModelEntity>> BedArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DoubleBed, BedArray);
	if (BedArray.Num() > 0)
	{
		BedEntity = BedArray[0];
	}
	else
	{
		InRoom->GetModelEntityArrayByComponentId(AI_SingleBed, BedArray);
		if (BedArray.Num() > 0)
		{
			BedEntity = BedArray[0];
		}
	}
	//找到了床
	if (BedEntity.IsValid())
	{
		TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
		TSharedPtr<FArmyWallLine> LightWallLine = LightAreaMaxWallLine(InRoom->GetRoom());
		if (LightWallLine.IsValid())
		{
			//
			FVector Start = LightWallLine->GetCoreLine()->GetStart();
			FVector End = LightWallLine->GetCoreLine()->GetEnd();
			TArray<TSharedPtr<FArmyWallLine>> StartLines;
			InRoom->GetRoom()->GetSamePointWalls(Start, StartLines);
			TArray<TSharedPtr<FArmyWallLine>> EndLines;
			InRoom->GetRoom()->GetSamePointWalls(End, EndLines);
			TArray<TSharedPtr<FArmyWallLine>> ConnectWallLineArr;
			for (auto& WallLine : StartLines)
			{
				if (WallLine != LightWallLine)
				{
					ConnectWallLineArr.Add(WallLine);
				}
			}
			for (auto& WallLine : EndLines)
			{
				if (WallLine != LightWallLine)
				{
					ConnectWallLineArr.Add(WallLine);
				}
			}
			//
			for (auto& ACWallLine : ConnectWallLineArr)
			{
				if (ACWallLine->GetCoreLine()->Size() < 25 + InEntity->Length)
				{
					continue;
				}
				FVector Position = FVector::ZeroVector;
				FVector ACWallCenter = (ACWallLine->GetCoreLine()->GetStart() + ACWallLine->GetCoreLine()->GetEnd()) / 2;
				if (StartLines.Contains(ACWallLine))
				{
					FVector VecSC = ACWallCenter - Start;
					VecSC = VecSC.GetSafeNormal();
					Position = Start + VecSC * (25 + InEntity->Length / 2);
				}
				if (EndLines.Contains(ACWallLine))
				{
					FVector VecEC = ACWallCenter - End;
					VecEC = VecEC.GetSafeNormal();
					Position = End + VecEC * (25 + InEntity->Length / 2);
				}
				float Dist = 180 + InEntity->Height*0.5f;//空调底面距地1800mm, 侧面距边墙250mm
				Position.Z = Dist;
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(ACWallLine);
				Direction = Direction.GetSafeNormal();
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				TempEntity->Location = Position;
				//
				EntityArr.Add(TempEntity);
			}
		}

		//碰撞检测：检测顶面、柱子类碰撞
		TArray<TSharedPtr<FArmyModelEntity>> EntityUnIntersectArr;
		for (auto& TempEntity : EntityArr)
		{
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckCeilingCollision(EntityActor, InRoom) &&
					!CheckWallCornerCollision(EntityActor, InRoom) &&
					!CheckWallObjectCollision(EntityActor, InRoom))
				{
					EntityUnIntersectArr.Add(TempEntity);
				}
				EntityActor->Destroy();
			}
		}

		//选择离床头最远的空调
		TSharedPtr<FArmyWallLine> BedForWardWall = FArmyDataTools::GetWallLineByModelInDir(*BedEntity->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (BedForWardWall.IsValid())
		{
			FVector TempCenter(0, 0, 0);
			float Dist = 180 + InEntity->Height*0.5f;//空调底面距地1800mm, 侧面距边墙250mm
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*BedEntity->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
			{
				TSharedPtr<FArmyModelEntity> MaxEntity;
				float MaxDis = 0;
				for (auto& TempEntity : EntityUnIntersectArr)
				{
					float dis = FVector::Distance(TempCenter, TempEntity->Location);
					if (dis > MaxDis)
					{
						MaxDis = dis;
						MaxEntity = TempEntity;
					}
				}
				//
				if (MaxEntity.IsValid() && MaxDis > 0)
				{
					EntityArray.Add(MaxEntity);
				}
			}
		}
	}
}

// 卧室 壁挂电视
void FArmyAutoAccessoryBedroom::CalcTVWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//首先寻找电视柜
	TArray<TSharedPtr<class FArmyModelEntity>> TVBenchArray;
	InRoom->GetModelEntityArrayByComponentId(AI_TVBench, TVBenchArray);
	if (TVBenchArray.Num() > 0)
	{
		auto TVBench = TVBenchArray[0];
		TSharedPtr<FArmyWallLine> TVCabinetBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*TVBench->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (TVCabinetBackWardWall.IsValid() && TVCabinetBackWardWall->GetCoreLine()->Size() >= InEntity->Length)
		{
			FVector TempCenter(0, 0, 0);
			float Dist = 130 /*- InEntity->Height*0.5f*/;//电视中心距地1300
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*TVBench->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
			{
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(TVCabinetBackWardWall);
				Direction = Direction.GetSafeNormal();
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				TempEntity->Location = TempCenter + Direction * 1;
				
				//碰撞检测：检测柱子类碰撞
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
	else
	{
		//床
		TSharedPtr<FArmyModelEntity> BedEntity;
		TArray<TSharedPtr<FArmyModelEntity>> BedArray;
		InRoom->GetModelEntityArrayByComponentId(AI_DoubleBed, BedArray);
		if (BedArray.Num() > 0)
		{
			BedEntity = BedArray[0];
		}
		else
		{
			InRoom->GetModelEntityArrayByComponentId(AI_SingleBed, BedArray);
			if (BedArray.Num() > 0)
			{
				BedEntity = BedArray[0];
			}
		}
		//找到了床
		if (BedEntity.IsValid())
		{
			//找床对面的墙
			TSharedPtr<FArmyWallLine> BedForWardWall = FArmyDataTools::GetWallLineByModelInDir(*BedEntity->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
			if (BedForWardWall.IsValid() && BedForWardWall->GetCoreLine()->Size() >= InEntity->Length)
			{
				FVector TempCenter(0, 0, 0);
				float Dist = 130 /*- InEntity->Height*0.5f*/;//电视中心距地1300

				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*BedEntity->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD))
				{
					//参考方向
					FVector Direction = BedForWardWall->GetDirection() * -1;//顺时针的
					FVector2D VecDir = FVector2D(Direction.X, Direction.Y);
					VecDir = VecDir.GetSafeNormal();//顺时针的
													//
					FVector TempLength = Direction * InEntity->Length / 2;
					FVector Start = BedForWardWall->GetCoreLine()->GetEnd();
					FVector End = BedForWardWall->GetCoreLine()->GetStart();
					FVector2D Center2D = FVector2D(TempCenter.X, TempCenter.Y);
					FVector2D CenterMin2D = Center2D + FVector2D(TempLength.X, TempLength.Y) * -1;
					FVector2D CenterMax2D = Center2D + FVector2D(TempLength.X, TempLength.Y) * 1;
					FVector2D Start2D = FVector2D(Start.X, Start.Y);
					FVector2D End2D = FVector2D(End.X, End.Y);
					/** 构造出新的Entity */
					TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempEntity);
					//
					FVector DirectionE = InRoom->GetRoom()->GetWallLineNormal(BedForWardWall);
					DirectionE = DirectionE.GetSafeNormal();
					TempEntity->Rotation = DirectionE.Rotation() + FRotator(0, -90, 0);
					TempEntity->Location = TempCenter + DirectionE * 1;
					//碰撞检测：检测柱子类碰撞
					AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
					if (EntityActor)
					{
						if (!CheckWallCornerCollision(EntityActor, InRoom) &&
							!CheckWallObjectCollision(EntityActor, InRoom) &&
							!CheckWallCollision(EntityActor, InRoom) &&
							(FArmyMath::IsPointProjectionOnLineSegment2D(CenterMin2D + VecDir * 1 * 30, Start2D, End2D) && FArmyMath::IsPointProjectionOnLineSegment2D(CenterMax2D + VecDir*-1 * 30, Start2D, End2D)))
						{
							//获取墙上所有的点
							TArray<FVector2D> Point2DArr;
							Point2DArr.Add(Start2D);
							Point2DArr.Add(End2D);
							TArray<FObjectWeakPtr> WallModels;
							BedForWardWall->GetAppendObjects(WallModels);
							for (auto& Model : WallModels)
							{
								TSharedPtr<FArmyHardware> ModelObject = StaticCastSharedPtr<FArmyHardware>(Model.Pin());
								FVector ModelStart = ModelObject->GetEndPos();
								FVector ModelEnd = ModelObject->GetStartPos();
								FArmyMath::GetLineSegmentProjectionPos(Start, End, ModelStart);
								FArmyMath::GetLineSegmentProjectionPos(Start, End, ModelEnd);
								Point2DArr.Add(FVector2D(ModelStart.X, ModelStart.Y));
								Point2DArr.Add(FVector2D(ModelEnd.X, ModelEnd.Y));
							}
							//找出距离最小的点
							FVector2D Point2DMin = Start2D;
							float DisMin = BedForWardWall->GetCoreLine()->Size();
							for (FVector2D Point2D : Point2DArr)
							{
								float dis = FVector2D::Distance(CenterMin2D, Point2D);
								FVector2D vecCP = Point2D - CenterMin2D;
								if (dis < DisMin)
								{
									if (!(FVector2D::DotProduct(VecDir, vecCP) > 0 && dis > InEntity->Length))
									{
										DisMin = dis;
										Point2DMin = Point2D;
									}
								}
							}
							//
							FVector2D VecMinPoint = (Point2DMin - CenterMin2D);
							VecMinPoint = VecMinPoint.GetSafeNormal();
							if (FVector2D::DotProduct(VecDir, VecMinPoint) > 0)
							{
								//同向,向VecDir平移DisMin距离
								if (DisMin <= 30)
								{
									//要求NewCenterMax2D不跟任何门窗重叠
									FVector2D NewCenter2D = Center2D + VecDir * (DisMin + 5);
									//
									bool IsCheck = true;
									for (FVector2D Point2D : Point2DArr)
									{
										float dis = FVector2D::Distance(NewCenter2D, Point2D);
										if (dis < InEntity->Length / 2)
										{
											IsCheck = false;
											break;
										}
									}
									//
									if (IsCheck)
									{
										TempEntity->Location = FVector(NewCenter2D.X, NewCenter2D.Y, TempEntity->Location.Z);
										EntityArray.Add(TempEntity);
									}
								}
							}
							if (FVector2D::DotProduct(VecDir, VecMinPoint) < 0)
							{
								//反向
								FVector2D Point2DMinMin = Start2D;
								float DisMinMin = BedForWardWall->GetCoreLine()->Size();
								for (FVector2D Point2D : Point2DArr)
								{
									float dis = FVector2D::Distance(CenterMax2D, Point2D);
									FVector2D vecCP = Point2D - CenterMax2D;
									if (dis < DisMinMin)
									{
										if (!(FVector2D::DotProduct(VecDir, vecCP) < 0 && dis > InEntity->Length))
										{
											DisMinMin = dis;
											Point2DMinMin = Point2D;
										}
									}
								}
								//
								FVector2D VecMinMinPoint = (Point2DMinMin - CenterMax2D);
								VecMinMinPoint = VecMinMinPoint.GetSafeNormal();
								if (FVector2D::DotProduct(VecDir, VecMinMinPoint) < 0)
								{
									//反向,向VecDir反方向平移DisMinMin距离
									if (DisMinMin <= 30)
									{
										FVector2D NewCenter2D = Center2D + VecDir * (DisMinMin + 5) * -1;
										//
										bool IsCheck = true;
										for (FVector2D Point2D : Point2DArr)
										{
											float dis = FVector2D::Distance(NewCenter2D, Point2D);
											if (dis < InEntity->Length / 2)
											{
												IsCheck = false;
												break;
											}
										}
										//
										if (IsCheck)
										{
											TempEntity->Location = FVector(NewCenter2D.X, NewCenter2D.Y, TempEntity->Location.Z);
											EntityArray.Add(TempEntity);
										}
									}
								}
								else
								{
									//同向
									bool IsCheck = true;
									for (FVector2D Point2D : Point2DArr)
									{
										float dis = FVector2D::Distance(Center2D, Point2D);
										if (dis < InEntity->Length / 2)
										{
											IsCheck = false;
											break;
										}
									}
									//
									if (IsCheck)
									{
										TempEntity->Location = FVector(Center2D.X, Center2D.Y, TempEntity->Location.Z);
										EntityArray.Add(TempEntity);
									}
								}
							}
						}
						EntityActor->Destroy();
					}
				}
			}
		}
	}
}

// 卧室 立式电视
void FArmyAutoAccessoryBedroom::CalcTVVert(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//首先寻找电视柜
	TArray<TSharedPtr<class FArmyModelEntity>> TVBenchArray;
	InRoom->GetModelEntityArrayByComponentId(AI_TVBench, TVBenchArray);
	if (TVBenchArray.Num() > 0)
	{
		auto TVBench = TVBenchArray[0];
		//
		FVector Size = FArmyDataTools::GetContextItemSize(TVBench->FurContentItem);
		FVector Position = FVector::ZeroVector;
		//获取地面的位置
		FArmyDataTools::PosOnDirectionOfModel(*TVBench->Actor, Size.Z*1.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		FVector Forward = FArmyDataTools::GetModelForwardVector(TVBench->Actor);

		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		//
		FVector Direction = Forward;
		TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
		TempEntity->Location = Position;
		
		//碰撞检测：不进行碰撞检测
		EntityArray.Add(TempEntity);
	}
}

/*------------------------------------ 卧室方法实现 End ------------------------------------*/


/*------------------------------------ 书房方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryStudyroom::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 书房 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

		/** 书房 窗帘 */
	case AI_CurtainStd:
		CalcCurtainStd(InEntity, InRoom, EntityArray);
		break;

		/** 书房 挂画 */
	case AI_Titanic:
		CalcTitanic(InEntity, InRoom, EntityArray);
		break;

		/** 书房 地毯 */
	case AI_Carpet:
		CalcCarpetLayout(InEntity, InRoom, EntityArray);
		break;

		/** 书房 台灯 */
	case AI_LampTable:
		CalcLampTable(InEntity, InRoom, EntityArray);
		break;

		/** 书房 壁挂空调 */
	case AI_AirConditionedHang:
		CalcAirConditionedHang(InEntity, InRoom, EntityArray);

	default:
		break;
	}
}

void FArmyAutoAccessoryStudyroom::CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 书房地毯摆放 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Desk, DeskArray);

	for (auto& Desk : DeskArray)
	{
		/** 构造出新的Entity */
		TSharedPtr<FArmyModelEntity> TempCarpetEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempCarpetEntity);

		/** 进行Rotation转换 */
		TempCarpetEntity->Rotation = Desk->Rotation;

		/** temp_entity.center = desk.center */
		TempCarpetEntity->Location = Desk->Location;

		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempCarpetEntity->FurContentItem, TempCarpetEntity->Location, TempCarpetEntity->Rotation, TempCarpetEntity->Scale3D);
		if (EntityActor)
		{
			/** 初始化Entity中的Actor */
			TempCarpetEntity->Actor = EntityActor;

			/** 计算地毯能否调整到房间内 */
			bool bInRoom = FArmyDataTools::CalModelAdjustToRoom(TempCarpetEntity, InRoom);
			if (bInRoom)
			{
				TempCarpetEntity->Location = EntityActor->GetActorLocation();
				TempCarpetEntity->Rotation = EntityActor->GetActorRotation();
				TempCarpetEntity->Scale3D = EntityActor->GetActorScale3D();

				//碰撞检测：不进行碰撞检测
				EntityArray.Emplace(TempCarpetEntity);
			}
			TempCarpetEntity->Actor->Destroy();
		}
	}
}

void FArmyAutoAccessoryStudyroom::CalcLampTable(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Desk, DeskArray);
	for (auto &desk : DeskArray)
	{
		//获取床头柜子上面的距离床头柜一半高度
		FVector Size = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector Right = FArmyDataTools::GetModelLeftVector(desk->Actor) * -1;
		FVector Back = FArmyDataTools::GetModelForwardVector(desk->Actor) * -1;

		FVector Position = FVector::ZeroVector;
		FArmyDataTools::PosOnDirectionOfModel(*desk->Actor, Size.Z, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		Position += Back * (Size.Y / 2 - InEntity->Width / 2 - 10);
		Position += Right * (Size.X / 2 - InEntity->Length / 2 - 10);

		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		TempEntity->Location = Position;
		TempEntity->Rotation = desk->Actor->GetActorRotation();

		//碰撞检测：不进行碰撞检测
		EntityArray.Add(TempEntity);
	}
}

void FArmyAutoAccessoryStudyroom::CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 书房挂画 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Desk, DeskArray);
	for (auto& Desk : DeskArray)
	{
		/** 书桌背面的墙长度需要大于100cm */
		TSharedPtr<FArmyWallLine> DeskBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*Desk->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
		if (DeskBackWardWall.IsValid() && DeskBackWardWall->GetCoreLine()->Size() >= 100.f)
		{
			/** temp_entity.center = height=150 */
			FVector TempCenter(0, 0, 0);
			float Dist = 150;
			/** 书桌背后的墙体 150.f */
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*Desk->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD))
			{
				/** 构造出新的Entity */
				TSharedPtr<FArmyModelEntity> TempTitanicEntity = MakeShareable(new FArmyModelEntity);
				InEntity->CopyTo(TempTitanicEntity);
				TempTitanicEntity->Location = TempCenter;
				/** 进行Rotation转换 */
				FVector Forward = FArmyDataTools::GetModelForwardVector(Desk->Actor) * -1;
				TempTitanicEntity->Rotation = Forward.Rotation() + FRotator(0, -90, 0);
				//
				// 碰撞检测：检测柱子类、墙体、门窗、顶面和物件的碰撞（全5种）
				AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntity->FurContentItem, TempTitanicEntity->Location, TempTitanicEntity->Rotation, TempTitanicEntity->Scale3D);
				if (EntityActor)
				{
					FBox EntityBox = GVC->GetActorCollisionBox(EntityActor);
					if (!CheckWallCornerCollision(EntityActor, InRoom) &&
						!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
						!CheckWallObjectCollision(EntityActor, InRoom) &&
						!CheckWallCollision(EntityActor, InRoom) &&
						!CheckCeilingCollision(EntityActor, InRoom) &&
						(FArmySceneData::WallHeight - 30) >= (EntityBox.GetCenter().Z + EntityBox.GetSize().Z*0.5f))
					{
						TempTitanicEntity->Location = EntityActor->GetActorLocation();
						TempTitanicEntity->Rotation = EntityActor->GetActorRotation();
						TempTitanicEntity->Scale3D = EntityActor->GetActorScale3D();
						EntityArray.Emplace(TempTitanicEntity);
					}
					EntityActor->Destroy();
				}
			}
		}
	}
}

// 书房 壁挂空调
void FArmyAutoAccessoryStudyroom::CalcAirConditionedHang(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	TSharedPtr<FArmyWallLine> LightWallLine = LightAreaMaxWallLine(InRoom->GetRoom());
	if (LightWallLine.IsValid())
	{
		//
		FVector Start = LightWallLine->GetCoreLine()->GetStart();
		FVector End = LightWallLine->GetCoreLine()->GetEnd();
		TArray<TSharedPtr<FArmyWallLine>> StartLines;
		InRoom->GetRoom()->GetSamePointWalls(Start, StartLines);
		TArray<TSharedPtr<FArmyWallLine>> EndLines;
		InRoom->GetRoom()->GetSamePointWalls(End, EndLines);
		TArray<TSharedPtr<FArmyWallLine>> ConnectWallLineArr;
		for (auto& WallLine : StartLines)
		{
			if (WallLine != LightWallLine)
			{
				ConnectWallLineArr.Add(WallLine);
			}
		}
		for (auto& WallLine : EndLines)
		{
			if (WallLine != LightWallLine)
			{
				ConnectWallLineArr.Add(WallLine);
			}
		}
		//
		for (auto& ACWallLine : ConnectWallLineArr)
		{
			if (ACWallLine->GetCoreLine()->Size() < 25 + InEntity->Length)
			{
				continue;
			}
			FVector Position = FVector::ZeroVector;
			FVector ACWallCenter = (ACWallLine->GetCoreLine()->GetStart() + ACWallLine->GetCoreLine()->GetEnd()) / 2;
			if (StartLines.Contains(ACWallLine))
			{
				FVector VecSC = ACWallCenter - Start;
				VecSC = VecSC.GetSafeNormal();
				Position = Start + VecSC * (25 + InEntity->Length / 2);
			}
			if (EndLines.Contains(ACWallLine))
			{
				FVector VecEC = ACWallCenter - End;
				VecEC = VecEC.GetSafeNormal();
				Position = End + VecEC * (25 + InEntity->Length / 2);
			}
			float Dist = 180 + InEntity->Height*0.5f;//空调底面距地1800mm, 侧面距边墙250mm
			Position.Z = Dist;
			//
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			//
			FVector Direction = InRoom->GetRoom()->GetWallLineNormal(ACWallLine);
			Direction = Direction.GetSafeNormal();
			TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
			TempEntity->Location = Position;
			//
			EntityArr.Add(TempEntity);
		}
	}

	//碰撞检测：检测顶面和柱子类碰撞
	TArray<TSharedPtr<FArmyModelEntity>> EntityUnIntersectArr;
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckCeilingCollision(EntityActor, InRoom) &&
				!CheckWallCornerCollision(EntityActor, InRoom) &&
				!CheckWallObjectCollision(EntityActor, InRoom))
			{
				EntityUnIntersectArr.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckCeilingCollision(EntityActor, InRoom) &&
								  !CheckWallCornerCollision(EntityActor, InRoom) &&
								  !CheckWallObjectCollision(EntityActor, InRoom);
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArray.Add(TempEntity);
				break;
			}
		}
	}
}

/*------------------------------------ 书房方法实现 End ------------------------------------*/


/*------------------------------------ 客厅方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryLivingroom::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/*
	//客厅划分区域，临时解决方案
	if (InRoom->GetRegionArray().Num() >= 1)
	{
	static TArray<ERoomType> ValidRegionArr = { RT_DiningRoom , RT_Hallway };
	for (TSharedPtr<FArmyRegion> It : InRoom->GetRegionArray())
	{
	int32 RegionId = FArmyDataTools::GetRoomType(It->GetSplitRegionId());
	FString RegionName = It->GetSplitRegionName();
	if (It.IsValid() && ValidRegionArr.Contains(RegionId))
	{
	TSharedPtr<FArmyRoom> room = InRoom->GetRoom();
	FString name = room->GetSpaceName();
	int32 spaceId = room->GetSpaceId();
	room->SetSpaceName(RegionName);
	room->SetSpaceId(RegionId);
	TSharedRef<FArmyAutoAccessory> AutoAccessory = FArmyAccessoryFactory::Get()->GetAccessoryRoom(InRoom);
	room->SetSpaceName(name);
	room->SetSpaceId(spaceId);
	AutoAccessory->CalcAccessoryLayout(InEntity, InRoom, EntityArray);//可以进到具体的分发函数
	}
	}
	}
	//客厅划分区域，临时解决方案结束
	*/
	switch (InEntity->ComponentId)
	{
		/** 客厅 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 窗帘 */
	case AI_CurtainStd:
		CalcCurtainStd(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 挂画 */
	case AI_Titanic:
		CalcTitanic(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 地毯 */
	case AI_Carpet:
		CalcCarpetLayout(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 台灯 */
	case AI_LampTable:
		CalcLampTable(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 壁灯 */
	case AI_LampWall:
		CalcLampWall(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 落地灯 */
	case AI_LampFloorDown:
		CalcLampFloorDown(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 绿植 */
	case AI_GreenPlants:
		CalcGreenPlants(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 立式空调 */
	case AI_AirConditionedVert:
		CalcAirConditionedVert(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 壁挂空调 */
	case AI_AirConditionedHang:
		CalcAirConditionedHang(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 壁挂电视 */
	case AI_TVWall:
		CalcTVWall(InEntity, InRoom, EntityArray);
		break;

		/** 客厅 立式电视 */
	case AI_TVVert:
		CalcTVVert(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

void FArmyAutoAccessoryLivingroom::CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 客厅地毯摆放 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> DoubleSofaArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, DoubleSofaArray);

	for (auto& DoubleSofa : DoubleSofaArray)
	{
		/** 构造出新的Entity */
		TSharedPtr<FArmyModelEntity> TempCarpetEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempCarpetEntity);

		/** 进行Rotation转换 */
		TempCarpetEntity->Rotation = DoubleSofa->Rotation;
		FVector TempRotation = DoubleSofa->Rotation.RotateVector(FVector(0, 1, 0));

		/** temp_entity.center = double_sofa.center + 30 */
		TempCarpetEntity->Location = DoubleSofa->Location + 30 * TempRotation;

		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempCarpetEntity->FurContentItem, TempCarpetEntity->Location, TempCarpetEntity->Rotation, TempCarpetEntity->Scale3D);
		if (EntityActor)
		{
			/** 初始化Entity中的Actor */
			TempCarpetEntity->Actor = EntityActor;
			/** 计算地毯能否调整到房间内 */
			bool bInRoom = FArmyDataTools::CalModelAdjustToRoom(TempCarpetEntity, InRoom);
			if (bInRoom)
			{
				TempCarpetEntity->Location = EntityActor->GetActorLocation();
				TempCarpetEntity->Rotation = EntityActor->GetActorRotation();
				TempCarpetEntity->Scale3D = EntityActor->GetActorScale3D();

				//碰撞检测：不进行碰撞检测
				EntityArray.Emplace(TempCarpetEntity);
			}
			TempCarpetEntity->Actor->Destroy();
		}
	}
}

// 客厅台灯
void FArmyAutoAccessoryLivingroom::CalcLampTable(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_SideTable, DeskArray);
	for (auto &desk : DeskArray)
	{
		FVector Size = FArmyDataTools::GetContextItemSize(desk->FurContentItem);
		FVector Position = FVector::ZeroVector;
		FArmyDataTools::PosOnDirectionOfModel(*desk->Actor, Size.Z*1.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		//
		TempEntity->Location = Position;
		//
		//碰撞检测：不进行碰撞检测
		EntityArray.Add(TempEntity);
	}
}

// 客厅壁灯
void FArmyAutoAccessoryLivingroom::CalcLampWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	//客厅壁灯的位置和方向是硬装计算出来的
	TArray<FVector> LampWallPosArr = InRoom->GetWallLampPosArr();
	FVector LampWallDirectrion = InRoom->GetWallLampDirection();
	//判断是否有效（基于硬装是否勾选）
	if (!FVector::PointsAreSame(LampWallDirectrion, FVector::ZeroVector))
	{
		//有效的位置和方向
		for (auto& Pos : LampWallPosArr)
		{
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			//
			TempEntity->Location = Pos;
			TempEntity->Rotation = LampWallDirectrion.Rotation() + FRotator(0, -90, 0);
			//
			EntityArr.Add(TempEntity);
		}
	}
	
	//碰撞检测：检测柱子类碰撞
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallCornerCollision(EntityActor, InRoom) &&
								!CheckWallObjectCollision(EntityActor, InRoom) &&
								!CheckWallCollision(EntityActor, InRoom);
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArray.Add(TempEntity);
			}
		}
	}
}

// 客厅 落地灯
void FArmyAutoAccessoryLivingroom::CalcLampFloorDown(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//获取边几的个数
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_SideTable, DeskArray);

	int  sideTableNum = DeskArray.Num();

	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;

	//获取主位沙发
	TArray<TSharedPtr<class FArmyModelEntity>> SofaArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, SofaArray);

	if (sideTableNum == 2) return;
	if (sideTableNum == 1)
	{
		for (auto &sofa : SofaArray)
		{
			FVector Size = FArmyDataTools::GetContextItemSize(sofa->FurContentItem);
			FVector Position = FVector::ZeroVector;
			//获取地面的位置
			FArmyDataTools::PosOnDirectionOfModel(*sofa->Actor, Size.Z*0.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);
			FVector Left = FArmyDataTools::GetModelLeftVector(sofa->Actor);
			FVector Right = -Left;
			FVector Back = FArmyDataTools::GetModelForwardVector(sofa->Actor) * -1;
			//两边伸展
			Left = Position + Left*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);;
			Right = Position + Right*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);;

			//计算边几的位置
			FVector tablePostion = FVector::ZeroVector;
			FArmyDataTools::PosOnDirectionOfModel(*(DeskArray[0]->Actor), Size.Z * 0, tablePostion, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);

			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			if (FVector::Dist2D(tablePostion, Right) < FVector::Dist2D(tablePostion, Left))
			{
				TempEntity->Location = Left;
			}
			else
			{
				TempEntity->Location = Right;
			}

			TempEntity->Rotation = sofa->Actor->GetActorRotation();
			EntityArr.Add(TempEntity);
		}
	}
	// 布置两个落地灯
	if (sideTableNum == 0)
	{
		for (auto &sofa : SofaArray)
		{
			FVector Size = FArmyDataTools::GetContextItemSize(sofa->FurContentItem);
			FVector Position = FVector::ZeroVector;
			//获取地面的位置
			FArmyDataTools::PosOnDirectionOfModel(*sofa->Actor, Size.Z*0.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);
			FVector Left = FArmyDataTools::GetModelLeftVector(sofa->Actor);
			FVector Right = -Left;
			FVector Back = FArmyDataTools::GetModelForwardVector(sofa->Actor) * -1;
			//两边伸展
			Left = Position + Left*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);
			Right = Position + Right*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);

			//计算茶几的位置
			TSharedPtr<FArmyModelEntity>  LeftEntity = MakeShareable(new FArmyModelEntity());
			TSharedPtr<FArmyModelEntity>  RigthEntity = MakeShareable(new FArmyModelEntity());

			InEntity->CopyTo(LeftEntity);
			InEntity->CopyTo(RigthEntity);
			LeftEntity->Location = Left;
			RigthEntity->Location = Right;

			EntityArr.Add(LeftEntity);
			EntityArr.Add(RigthEntity);
		}
	}

	//碰撞检测：检测柱子类碰撞
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallCornerCollision(EntityActor, InRoom) &&
								!CheckWallCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>());
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArray.Add(TempEntity);
			}
		}
	}
}

// 客厅 绿植
void FArmyAutoAccessoryLivingroom::CalcGreenPlants(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//
	TSet<AActor*> OtherActorSet;
	// 1. 绿植布置到主位沙发旁边
	//获取边几的个数
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_SideTable, DeskArray);

	int  sideTableNum = DeskArray.Num();

	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;

	//获取主位沙发
	TArray<TSharedPtr<class FArmyModelEntity>> SofaArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, SofaArray);

	//布置1棵绿植
	if (sideTableNum == 1)
	{
		for (auto &sofa : SofaArray)
		{
			FVector Size = FArmyDataTools::GetContextItemSize(sofa->FurContentItem);
			FVector Position = FVector::ZeroVector;
			//获取地面的位置
			FArmyDataTools::PosOnDirectionOfModel(*sofa->Actor, Size.Z*0.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);
			FVector Left = FArmyDataTools::GetModelLeftVector(sofa->Actor);
			FVector Right = -Left;
			FVector Back = FArmyDataTools::GetModelForwardVector(sofa->Actor) * -1;
			//两边伸展
			Left = Position + Left*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);;
			Right = Position + Right*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);;

			//计算边几的位置
			FVector tablePostion = FVector::ZeroVector;
			FArmyDataTools::PosOnDirectionOfModel(*(DeskArray[0]->Actor), Size.Z * 0, tablePostion, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);

			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			if (FVector::Dist2D(tablePostion, Right) < FVector::Dist2D(tablePostion, Left))
			{
				TempEntity->Location = Left;
			}
			else
			{
				TempEntity->Location = Right;
			}

			TempEntity->Rotation = sofa->Actor->GetActorRotation();
			EntityArr.Add(TempEntity);
			OtherActorSet.Add(sofa->Actor);
		}
	}
	// 布置2棵绿植
	if (sideTableNum == 0)
	{
		for (auto &sofa : SofaArray)
		{
			FVector Size = FArmyDataTools::GetContextItemSize(sofa->FurContentItem);
			FVector Position = FVector::ZeroVector;
			//获取地面的位置
			FArmyDataTools::PosOnDirectionOfModel(*sofa->Actor, Size.Z*0.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);
			FVector Left = FArmyDataTools::GetModelLeftVector(sofa->Actor);
			FVector Right = -Left;
			FVector Back = FArmyDataTools::GetModelForwardVector(sofa->Actor) * -1;
			//两边伸展
			Left = Position + Left*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);
			Right = Position + Right*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Back*(FArmyDataTools::GetContextItemSize(sofa->FurContentItem).Y / 2 - 50);

			//计算茶几的位置
			TSharedPtr<FArmyModelEntity>  LeftEntity = MakeShareable(new FArmyModelEntity());
			TSharedPtr<FArmyModelEntity>  RigthEntity = MakeShareable(new FArmyModelEntity());

			InEntity->CopyTo(LeftEntity);
			InEntity->CopyTo(RigthEntity);
			LeftEntity->Location = Left;
			RigthEntity->Location = Right;

			EntityArr.Add(LeftEntity);
			EntityArr.Add(RigthEntity);
			OtherActorSet.Add(sofa->Actor);
		}
	}

	// 2. 绿植布置到视听柜旁边
	//获取视听柜
	TArray<TSharedPtr<class FArmyModelEntity>> TVCabinetArray;
	InRoom->GetModelEntityArrayByComponentId(AI_TVBench, TVCabinetArray);
	for (auto &tvcabinet : TVCabinetArray)
	{
		FVector Size = FArmyDataTools::GetContextItemSize(tvcabinet->FurContentItem);
		FVector Position = FVector::ZeroVector;
		//获取地面的位置
		FArmyDataTools::PosOnDirectionOfModel(*tvcabinet->Actor, Size.Z*0.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_DOWN);
		FVector Left = FArmyDataTools::GetModelLeftVector(tvcabinet->Actor);
		FVector Right = -Left;
		FVector Forward = FArmyDataTools::GetModelForwardVector(tvcabinet->Actor);
		//两边伸展
		Left = Position + Left*(FArmyDataTools::GetContextItemSize(tvcabinet->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Forward*(InEntity->Width / 2 - FArmyDataTools::GetContextItemSize(tvcabinet->FurContentItem).Y / 2 + 10);
		Right = Position + Right*(FArmyDataTools::GetContextItemSize(tvcabinet->FurContentItem).X / 2 + InEntity->Length / 2 + 10) + Forward*(InEntity->Width / 2 - FArmyDataTools::GetContextItemSize(tvcabinet->FurContentItem).Y / 2 + 10);

		//计算茶几的位置
		TSharedPtr<FArmyModelEntity>  LeftEntity = MakeShareable(new FArmyModelEntity());
		TSharedPtr<FArmyModelEntity>  RigthEntity = MakeShareable(new FArmyModelEntity());

		InEntity->CopyTo(LeftEntity);
		InEntity->CopyTo(RigthEntity);
		LeftEntity->Location = Left;
		RigthEntity->Location = Right;

		EntityArr.Add(LeftEntity);
		EntityArr.Add(RigthEntity);
		OtherActorSet.Add(tvcabinet->Actor);
	}

	//碰撞检测：检测柱子类、墙体、物件碰撞
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallCornerCollision(EntityActor, InRoom) &&
								!CheckWallCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>());
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArray.Add(TempEntity);
			}
		}
	}
}

// 客厅 挂画
void FArmyAutoAccessoryLivingroom::CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 客厅挂画 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> SofeMasterArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, SofeMasterArray);

	for (auto& SofaMaster : SofeMasterArray)
	{
		/** 沙发背面的墙长度需要大于100cm */
		TSharedPtr<FArmyWallLine> SofaMasterBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*SofaMaster->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (SofaMasterBackWardWall.IsValid() && SofaMasterBackWardWall->GetCoreLine()->Size() >= 210.f)
		{
			FVector TempCenter(0, 0, 0);
			float Dist = 150;
			/** temp_entity.width >= 90.f */
			if (InEntity->Length >= 90.f)
			{
				/** temp_entity.center = height=150 */
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*SofaMaster->Actor, Dist, TempCenter))
				{
					TSharedPtr<FArmyModelEntity> TempTitanicEntity = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntity);
					TempTitanicEntity->Rotation = SofaMaster->Rotation;
					TempTitanicEntity->Location = TempCenter;
					//
					AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntity->FurContentItem, TempTitanicEntity->Location, TempTitanicEntity->Rotation, TempTitanicEntity->Scale3D);
					if (EntityActor)
					{
						for (int i = 0; i < 8; ++i)
						{
							if (!CheckWallCornerCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(EntityActor, InRoom) &&
								!CheckWallCollision(EntityActor, InRoom) &&
								!CheckCeilingCollision(EntityActor, InRoom))
							{
								TempTitanicEntity->Location = EntityActor->GetActorLocation();
								TempTitanicEntity->Rotation = EntityActor->GetActorRotation();
								TempTitanicEntity->Scale3D = EntityActor->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntity);
								break;
							}
							EntityActor->SetActorLocation(EntityActor->GetActorLocation() + FVector(0, 0, 10));
						}
						EntityActor->Destroy();
					}
				}
			}

			/** 60.f <= temp_entity.width < 90.f */
			else if (InEntity->Length < 90.f && InEntity->Length >= 60.f)
			{
				/**
				* temp_entity.center1 = FVector(5.f + Width*0.5f,0,150)
				* temp_entity2.center = FVector(-5.f - Width*0.5f,0,150)
				*/
				/** 书桌背后的墙体 */
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*SofaMaster->Actor, Dist, TempCenter))
				{
					/** X方向 */
					FVector TempRotationX = SofaMaster->Rotation.RotateVector(FVector(1, 0, 0));
					/** 构造出新的Entity */
					TSharedPtr<FArmyModelEntity> TempTitanicEntityFirst = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntityFirst);
					TSharedPtr<FArmyModelEntity> TempTitanicEntitySecond = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntitySecond);
					TempTitanicEntityFirst->Rotation = SofaMaster->Rotation;
					TempTitanicEntityFirst->Location = TempCenter - (5.f + TempTitanicEntityFirst->Length*0.5f)*TempRotationX;
					TempTitanicEntitySecond->Rotation = SofaMaster->Rotation;
					TempTitanicEntitySecond->Location = TempCenter + (5.f + TempTitanicEntityFirst->Length*0.5f)*TempRotationX;
					//
					AActor* TitanicActorFirst = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntityFirst->FurContentItem, TempTitanicEntityFirst->Location, TempTitanicEntityFirst->Rotation, TempTitanicEntityFirst->Scale3D);
					AActor* TitanicActorSecond = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntitySecond->FurContentItem, TempTitanicEntitySecond->Location, TempTitanicEntitySecond->Rotation, TempTitanicEntitySecond->Scale3D);
					if (TitanicActorFirst && TitanicActorSecond)
					{
						for (int i = 0; i < 8; ++i)
						{
							if ((!CheckWallCornerCollision(TitanicActorFirst, InRoom) &&
								!CheckOtherActorCollision(TitanicActorFirst, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(TitanicActorFirst, InRoom) &&
								!CheckWallCollision(TitanicActorFirst, InRoom) &&
								!CheckCeilingCollision(TitanicActorFirst, InRoom))
								&& 
								(!CheckWallCornerCollision(TitanicActorSecond, InRoom) &&
								!CheckOtherActorCollision(TitanicActorSecond, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(TitanicActorSecond, InRoom) &&
								!CheckWallCollision(TitanicActorSecond, InRoom) &&
								!CheckCeilingCollision(TitanicActorSecond, InRoom)))
							{
								TempTitanicEntityFirst->Location = TitanicActorFirst->GetActorLocation();
								TempTitanicEntityFirst->Rotation = TitanicActorFirst->GetActorRotation();
								TempTitanicEntityFirst->Scale3D = TitanicActorFirst->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntityFirst);
								//
								TempTitanicEntitySecond->Location = TitanicActorSecond->GetActorLocation();
								TempTitanicEntitySecond->Rotation = TitanicActorSecond->GetActorRotation();
								TempTitanicEntitySecond->Scale3D = TitanicActorSecond->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntitySecond);
								break;
							}
							TitanicActorFirst->SetActorLocation(TitanicActorFirst->GetActorLocation() + FVector(0, 0, 10));
							TitanicActorSecond->SetActorLocation(TitanicActorSecond->GetActorLocation() + FVector(0, 0, 10));
						}
						TitanicActorFirst->Destroy();
						TitanicActorSecond->Destroy();
					}
				}
			}

			/** temp_entity.width < 60.f */
			else if (InEntity->Length < 60.f)
			{
				/**
				* temp_entity1.center = FVector(0,0,150)
				* temp_entity2.center = FVector(10.f + Width,0,150)
				* temp_entity3.center = FVector(- 10.f - Width,0,150)
				*/
				/** 书桌背后的墙体 */
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*SofaMaster->Actor, Dist, TempCenter))
				{
					/** X方向 */
					FVector TempRotationX = SofaMaster->Rotation.RotateVector(FVector(1, 0, 0));
					/** 构造出新的Entity */
					TSharedPtr<FArmyModelEntity> TempTitanicEntityFirst = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntityFirst);
					TSharedPtr<FArmyModelEntity> TempTitanicEntitySecond = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntitySecond);
					TSharedPtr<FArmyModelEntity> TempTitanicEntityThird = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntityThird);
					/** 进行Rotation Location转换 */
					TempTitanicEntityFirst->Rotation = SofaMaster->Rotation;
					TempTitanicEntityFirst->Location = TempCenter;
					TempTitanicEntitySecond->Rotation = SofaMaster->Rotation;
					TempTitanicEntitySecond->Location = TempCenter - (10.f + TempTitanicEntityFirst->Length)*TempRotationX;
					TempTitanicEntityThird->Rotation = SofaMaster->Rotation;
					TempTitanicEntityThird->Location = TempCenter + (10.f + TempTitanicEntityFirst->Length)*TempRotationX;
					//
					AActor* TitanicActorFirst = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntityFirst->FurContentItem, TempTitanicEntityFirst->Location, TempTitanicEntityFirst->Rotation, TempTitanicEntityFirst->Scale3D);
					AActor* TitanicActorSecond = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntitySecond->FurContentItem, TempTitanicEntitySecond->Location, TempTitanicEntitySecond->Rotation, TempTitanicEntitySecond->Scale3D);
					AActor* TitanicActorThird = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntityThird->FurContentItem, TempTitanicEntityThird->Location, TempTitanicEntityThird->Rotation, TempTitanicEntityThird->Scale3D);
					if (TitanicActorFirst && TitanicActorSecond && TitanicActorThird)
					{
						for (int i = 0; i < 8; ++i)
						{
							if ((!CheckWallCornerCollision(TitanicActorFirst, InRoom) &&
								!CheckOtherActorCollision(TitanicActorFirst, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(TitanicActorFirst, InRoom) &&
								!CheckWallCollision(TitanicActorFirst, InRoom) &&
								!CheckCeilingCollision(TitanicActorFirst, InRoom))
								&&
								(!CheckWallCornerCollision(TitanicActorSecond, InRoom) &&
								!CheckOtherActorCollision(TitanicActorSecond, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(TitanicActorSecond, InRoom) &&
								!CheckWallCollision(TitanicActorSecond, InRoom) &&
								!CheckCeilingCollision(TitanicActorSecond, InRoom))
								&&
								(!CheckWallCornerCollision(TitanicActorThird, InRoom) &&
								!CheckOtherActorCollision(TitanicActorThird, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(TitanicActorThird, InRoom) &&
								!CheckWallCollision(TitanicActorThird, InRoom) &&
								!CheckCeilingCollision(TitanicActorThird, InRoom)))
							{
								TempTitanicEntityFirst->Location = TitanicActorFirst->GetActorLocation();
								TempTitanicEntityFirst->Rotation = TitanicActorFirst->GetActorRotation();
								TempTitanicEntityFirst->Scale3D = TitanicActorFirst->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntityFirst);
								//
								TempTitanicEntitySecond->Location = TitanicActorSecond->GetActorLocation();
								TempTitanicEntitySecond->Rotation = TitanicActorSecond->GetActorRotation();
								TempTitanicEntitySecond->Scale3D = TitanicActorSecond->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntitySecond);
								//
								TempTitanicEntityThird->Location = TitanicActorThird->GetActorLocation();
								TempTitanicEntityThird->Rotation = TitanicActorThird->GetActorRotation();
								TempTitanicEntityThird->Scale3D = TitanicActorThird->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntityThird);
								break;
							}
							TitanicActorFirst->SetActorLocation(TitanicActorFirst->GetActorLocation() + FVector(0, 0, 10));
							TitanicActorSecond->SetActorLocation(TitanicActorSecond->GetActorLocation() + FVector(0, 0, 10));
							TitanicActorThird->SetActorLocation(TitanicActorThird->GetActorLocation() + FVector(0, 0, 10));
						}
						TitanicActorFirst->Destroy();
						TitanicActorSecond->Destroy();
						TitanicActorThird->Destroy();
					}
				}
			}
		}
	}
}

// 客厅 立式空调
void FArmyAutoAccessoryLivingroom::CalcAirConditionedVert(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//主位沙发
	TArray<TSharedPtr<FArmyModelEntity>> MasterSofaArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, MasterSofaArray);
	if (MasterSofaArray.Num() > 0)
	{
		TSharedPtr<FArmyModelEntity> MasterSofaEntity = MasterSofaArray[0];
		TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
		TSharedPtr<FArmyWallLine> LightWallLine = LightAreaMaxWallLine(InRoom->GetRoom());
		if (LightWallLine.IsValid())
		{
			//
			FVector Start = LightWallLine->GetCoreLine()->GetStart();
			FVector End = LightWallLine->GetCoreLine()->GetEnd();
			TArray<TSharedPtr<FArmyWallLine>> StartLines;
			InRoom->GetRoom()->GetSamePointWalls(Start, StartLines);
			TArray<TSharedPtr<FArmyWallLine>> EndLines;
			InRoom->GetRoom()->GetSamePointWalls(End, EndLines);
			TArray<TSharedPtr<FArmyWallLine>> ConnectWallLineArr;
			for (auto& WallLine : StartLines)
			{
				if (WallLine != LightWallLine)
				{
					ConnectWallLineArr.Add(WallLine);
				}
			}
			for (auto& WallLine : EndLines)
			{
				if (WallLine != LightWallLine)
				{
					ConnectWallLineArr.Add(WallLine);
				}
			}
			//
			for (auto& ACWallLine : ConnectWallLineArr)
			{
				if (ACWallLine->GetCoreLine()->Size() < 25 + InEntity->Length)
				{
					continue;
				}
				FVector Position = FVector::ZeroVector;
				FVector ACWallCenter = (ACWallLine->GetCoreLine()->GetStart() + ACWallLine->GetCoreLine()->GetEnd()) / 2;
				if (StartLines.Contains(ACWallLine))
				{
					FVector VecSC = ACWallCenter - Start;
					VecSC = VecSC.GetSafeNormal();
					Position = Start + VecSC * (25 + InEntity->Length / 2);
				}
				if (EndLines.Contains(ACWallLine))
				{
					FVector VecEC = ACWallCenter - End;
					VecEC = VecEC.GetSafeNormal();
					Position = End + VecEC * (25 + InEntity->Length / 2);
				}
				float Dist = 0;//空调底面距地0mm, 侧面距边墙250mm，背里面距墙30mm，跟背立墙成45度夹角向房间吹风
				Position.Z = Dist;
				Position = Position + ACWallLine->GetNormal() * (3 + InEntity->Width / 2);
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				FVector ACWallDirection = InRoom->GetRoom()->GetWallLineNormal(ACWallLine);
				ACWallDirection = ACWallDirection.GetSafeNormal();
				FVector LightWallDirection = InRoom->GetRoom()->GetWallLineNormal(LightWallLine);
				LightWallDirection = LightWallDirection.GetSafeNormal();
				FVector Direction = ACWallDirection + LightWallDirection;
				Direction = Direction.GetSafeNormal();
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);//需要考虑旋转45度角度
				TempEntity->Location = Position;
				//
				EntityArr.Add(TempEntity);
			}
		}

		//碰撞检测：检测柱子类碰撞
		TArray<TSharedPtr<FArmyModelEntity>> EntityUnIntersectArr;
		for (auto& TempEntity : EntityArr)
		{
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				bool isNotIntersect = !CheckWallCornerCollision(EntityActor, InRoom) &&
									!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>());
				EntityActor->Destroy();
				if (isNotIntersect)
				{
					EntityUnIntersectArr.Add(TempEntity);
				}
			}
		}

		//选择离主位沙发最远的空调
		TSharedPtr<FArmyWallLine> BedForWardWall = FArmyDataTools::GetWallLineByModelInDir(*MasterSofaEntity->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (BedForWardWall.IsValid())
		{
			FVector TempCenter(0, 0, 0);
			float Dist = 180 + InEntity->Height*0.5f;//空调底面距地1800mm, 侧面距边墙250mm
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*MasterSofaEntity->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
			{
				TSharedPtr<FArmyModelEntity> MaxEntity;
				float MaxDis = 0;
				for (auto& TempEntity : EntityUnIntersectArr)
				{
					float dis = FVector::Distance(TempCenter, TempEntity->Location);
					if (dis > MaxDis)
					{
						MaxDis = dis;
						MaxEntity = TempEntity;
					}
				}
				//
				if (MaxEntity.IsValid() && MaxDis > 0)
				{
					EntityArray.Add(MaxEntity);
				}
			}
		}
	}
}

// 客厅 壁挂空调
void FArmyAutoAccessoryLivingroom::CalcAirConditionedHang(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//主位沙发
	TArray<TSharedPtr<FArmyModelEntity>> MasterSofaArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, MasterSofaArray);
	if (MasterSofaArray.Num() > 0)
	{
		TSharedPtr<FArmyModelEntity> MasterSofaEntity = MasterSofaArray[0];
		TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
		TSharedPtr<FArmyWallLine> LightWallLine = LightAreaMaxWallLine(InRoom->GetRoom());
		if (LightWallLine.IsValid())
		{
			//
			FVector Start = LightWallLine->GetCoreLine()->GetStart();
			FVector End = LightWallLine->GetCoreLine()->GetEnd();
			TArray<TSharedPtr<FArmyWallLine>> StartLines;
			InRoom->GetRoom()->GetSamePointWalls(Start, StartLines);
			TArray<TSharedPtr<FArmyWallLine>> EndLines;
			InRoom->GetRoom()->GetSamePointWalls(End, EndLines);
			TArray<TSharedPtr<FArmyWallLine>> ConnectWallLineArr;
			for (auto& WallLine : StartLines)
			{
				if (WallLine != LightWallLine)
				{
					ConnectWallLineArr.Add(WallLine);
				}
			}
			for (auto& WallLine : EndLines)
			{
				if (WallLine != LightWallLine)
				{
					ConnectWallLineArr.Add(WallLine);
				}
			}
			//
			for (auto& ACWallLine : ConnectWallLineArr)
			{
				if (ACWallLine->GetCoreLine()->Size() < 25 + InEntity->Length)
				{
					continue;
				}
				FVector Position = FVector::ZeroVector;
				FVector ACWallCenter = (ACWallLine->GetCoreLine()->GetStart() + ACWallLine->GetCoreLine()->GetEnd()) / 2;
				if (StartLines.Contains(ACWallLine))
				{
					FVector VecSC = ACWallCenter - Start;
					VecSC = VecSC.GetSafeNormal();
					Position = Start + VecSC * (25 + InEntity->Length / 2);
				}
				if (EndLines.Contains(ACWallLine))
				{
					FVector VecEC = ACWallCenter - End;
					VecEC = VecEC.GetSafeNormal();
					Position = End + VecEC * (25 + InEntity->Length / 2);
				}
				float Dist = 180 + InEntity->Height*0.5f;//空调底面距地1800mm, 侧面距边墙250mm
				Position.Z = Dist;
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(ACWallLine);
				Direction = Direction.GetSafeNormal();
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				TempEntity->Location = Position;
				//
				EntityArr.Add(TempEntity);
			}
		}

		//碰撞检测：检测顶面和柱子类碰撞
		TArray<TSharedPtr<FArmyModelEntity>> EntityUnIntersectArr;
		for (auto& TempEntity : EntityArr)
		{
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				bool isNotIntersect = !CheckCeilingCollision(EntityActor, InRoom) &&
									!CheckWallCornerCollision(EntityActor, InRoom) &&
									!CheckWallObjectCollision(EntityActor, InRoom);
				EntityActor->Destroy();
				if (isNotIntersect)
				{
					EntityUnIntersectArr.Add(TempEntity);
				}
			}
		}

		//选择离主位沙发最远的空调
		TSharedPtr<FArmyWallLine> BedForWardWall = FArmyDataTools::GetWallLineByModelInDir(*MasterSofaEntity->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (BedForWardWall.IsValid())
		{
			FVector TempCenter(0, 0, 0);
			float Dist = 180 + InEntity->Height*0.5f;//空调底面距地1800mm, 侧面距边墙250mm
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*MasterSofaEntity->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
			{
				TSharedPtr<FArmyModelEntity> MaxEntity;
				float MaxDis = 0;
				for (auto& TempEntity : EntityUnIntersectArr)
				{
					float dis = FVector::Distance(TempCenter, TempEntity->Location);
					if (dis > MaxDis)
					{
						MaxDis = dis;
						MaxEntity = TempEntity;
					}
				}
				//
				if (MaxEntity.IsValid() && MaxDis > 0)
				{
					EntityArray.Add(MaxEntity);
				}
			}
		}
	}
}

// 客厅 壁挂电视
void FArmyAutoAccessoryLivingroom::CalcTVWall(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	//首先寻找电视柜
	TArray<TSharedPtr<class FArmyModelEntity>> TVBenchArray;
	InRoom->GetModelEntityArrayByComponentId(AI_TVBench, TVBenchArray);
	if (TVBenchArray.Num() > 0)
	{
		auto TVBench = TVBenchArray[0];
		TSharedPtr<FArmyWallLine> TVCabinetBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*TVBench->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (TVCabinetBackWardWall.IsValid() && TVCabinetBackWardWall->GetCoreLine()->Size() >= InEntity->Length)
		{
			FVector TempCenter(0, 0, 0);
			float Dist = 130 /*- InEntity->Height*0.5f*/;//电视中心距地1300
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*TVBench->Actor, Dist, TempCenter, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
			{
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(TVCabinetBackWardWall);
				Direction = Direction.GetSafeNormal();
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				TempEntity->Location = TempCenter + Direction * 1;
				//
				EntityArr.Add(TempEntity);
			}
		}
	}
	else
	{
		//主位沙发规则布置
		TArray<TSharedPtr<class FArmyModelEntity>> MasterSofaArray;
		InRoom->GetModelEntityArrayByComponentId(AI_Sofa_Master, MasterSofaArray);
		if (MasterSofaArray.Num() > 0)
		{
			auto SofaMaster = MasterSofaArray[0];
			TSharedPtr<FArmyWallLine> SofaMasterForWardWall = FArmyDataTools::GetWallLineByModelInDir(*SofaMaster->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
			if (SofaMasterForWardWall.IsValid() && SofaMasterForWardWall->GetCoreLine()->Size() >= InEntity->Length)
			{
				float Dist = 130 /*- InEntity->Height*0.5f*/;//电视中心距地1300
				FVector Position = (SofaMasterForWardWall->GetCoreLine()->GetStart() + SofaMasterForWardWall->GetCoreLine()->GetEnd()) / 2;
				Position.Z = Dist;
				//
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				//
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(SofaMasterForWardWall);
				Direction = Direction.GetSafeNormal();
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				TempEntity->Location = Position + Direction * 1;
				//
				EntityArr.Add(TempEntity);
			}
		}
	}

	//碰撞检测：检测柱子类碰撞
	for (auto& TempEntity : EntityArr)
	{
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

// 客厅 立式电视
void FArmyAutoAccessoryLivingroom::CalcTVVert(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//首先寻找电视柜
	TArray<TSharedPtr<class FArmyModelEntity>> TVBenchArray;
	InRoom->GetModelEntityArrayByComponentId(AI_TVBench, TVBenchArray);
	if (TVBenchArray.Num() > 0)
	{
		auto TVBench = TVBenchArray[0];
		//
		FVector Size = FArmyDataTools::GetContextItemSize(TVBench->FurContentItem);
		FVector Position = FVector::ZeroVector;
		//获取地面的位置
		FArmyDataTools::PosOnDirectionOfModel(*TVBench->Actor, Size.Z*1.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		FVector Forward = FArmyDataTools::GetModelForwardVector(TVBench->Actor);

		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		//
		FVector Direction = Forward;
		TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
		TempEntity->Location = Position;
		
		//碰撞检测：不进行碰撞检测
		EntityArray.Add(TempEntity);
	}
}

/*------------------------------------ 客厅方法实现 End ------------------------------------*/

/*------------------------------------ 卫生间方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryBathRoom::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 卫生间 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 百叶窗 */
	case AI_Blind:
		CalcShutters(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 浴巾架 */
	case AI_BathTowelHolder:
		CalcBathTowelHolder(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 毛巾架 */
	case AI_FaceclothHolder:
		CalcFaceclothHolder(InEntity, InRoom, EntityArray);
		break;

		/** 卫生间 厕纸架 */
	case AI_ToiletPaperHolder:
		CalcToiletPaperHolder(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

// 卫生间百叶帘 Test OK
void FArmyAutoAccessoryBathRoom::CalcShutters(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	// 百叶帘（卫生间）
	TArray<TSharedPtr<FArmyHardware>> WindowArr = InRoom->GetWindowsActor();
	for (auto TempWindow : WindowArr)
	{
		TSharedPtr<FArmyWindow> Window = StaticCastSharedPtr<FArmyWindow>(TempWindow);
		//暂时只支持标准窗
		if (Window->GetType() != OT_Window)
		{
			continue;
		}
		//无效的窗户
		if (Window->HoleWindow == nullptr)
		{
			continue;
		}
		//结束
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempEntity);
		//
		TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByHardware(Window, InRoom->GetRoom());
		FVector Direction = InRoom->GetRoom()->GetWallLineNormal(WallLine);
		Direction = Direction.GetSafeNormal();
		//
		TempEntity->Scale3D = FVector((Window->GetLength() + 0) / InEntity->Length, 1, (Window->GetHeight() + 5) / InEntity->Height);
		FVector Postion = Window->GetPos();
		//TempEntity->Location = FVector(Postion.X, Postion.Y, Window->GetHeightToFloor() + (Window->GetHeight() + 5)) + (InEntity->Width / 2 + Window->GetWidth() / 2 + 5) * Direction;
		TempEntity->Location = FVector(Postion.X, Postion.Y, Window->GetHeightToFloor()) + (InEntity->Width / 2 + Window->GetWidth() / 2 + 5) * Direction;
		TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
		
		//碰撞检测：不进行碰撞检测
		EntityArray.Add(TempEntity);
	}
}

/** 卫生间 浴巾架 */
void FArmyAutoAccessoryBathRoom::CalcBathTowelHolder(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<class FArmyModelEntity>> ClosestoolArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Closestool, ClosestoolArray);//马桶（座便器）

	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	for (auto &closestool : ClosestoolArray)
	{
		TSharedPtr<FArmyWallLine> BackWardWall = FArmyDataTools::GetWallLineByModelInDir(*closestool->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
		if (BackWardWall.IsValid())
		{
			FVector Direction = InRoom->GetRoom()->GetWallLineNormal(BackWardWall);
			Direction = Direction.GetSafeNormal();
			FVector Position = FVector::ZeroVector;
			float Height = 170;//距地1.7m
			if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*closestool->Actor, Height, Position))
			{
				//拷贝新对象
				TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
				InEntity->CopyTo(TempEntity);
				TempEntity->Location = Position;
				TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
				//获取墙的两个方向
				FVector VecDirH = BackWardWall->GetDirection();
				FVector VecDirV = InRoom->GetRoom()->GetWallLineNormal(BackWardWall);
				VecDirV = VecDirV.GetSafeNormal();
				//处理柱子、包立管和风道
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
						FVector ClosestPoint = ModelBox.GetClosestPointTo(Location);
						FVector vecCL = Location - ClosestPoint;
						if (FVector::DotProduct(vecCL, VecDirH) < 0) {
							VecDirH *= -1;
						}
						FBox OverlapBox = ModelBox.Overlap(ActorBox);
						float size = OverlapBox.GetSize().ProjectOnToNormal(VecDirH).Size() + 10;
						TempEntity->Location += VecDirH * size;
					}
				}
				EntityArr.Add(TempEntity);
			}
		}
	}

	//检测碰撞：检测门窗和物件碰撞
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			if (!CheckWallObjectCollision(EntityActor, InRoom) &&
				!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()))
			{
				EntityArray.Add(TempEntity);
			}
			EntityActor->Destroy();
		}
	}
}

/** 卫生间 毛巾架 */
void FArmyAutoAccessoryBathRoom::CalcFaceclothHolder(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<class FArmyModelEntity>> BathroomArkArray;
	InRoom->GetModelEntityArrayByComponentId(AI_BathroomArk, BathroomArkArray);//浴室柜（有脚）
	TArray<TSharedPtr<class FArmyModelEntity>> BathroomArkWallArray;
	InRoom->GetModelEntityArrayByComponentId(AI_BathroomArk_Wall, BathroomArkWallArray);//浴室柜（无脚）
	TArray<TSharedPtr<class FArmyModelEntity>> ArkArray;
	ArkArray.Append(BathroomArkArray);
	ArkArray.Append(BathroomArkWallArray);
																			 //
	TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
	TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
	TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
	TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
	ComponentObjArr.Append(PillarArr);
	ComponentObjArr.Append(PackPipeArr);
	ComponentObjArr.Append(AirFlueArr);
	//

	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	for (auto& bathark : ArkArray)
	{
		float Dist = 0;//距地750mm
		FVector BatharkSize = FArmyDataTools::GetContextItemSize(bathark->FurContentItem);
		FVector BatharkPos = FVector::ZeroVector;
		FArmyDataTools::PosOnDirectionOfModel(*bathark->Actor, Dist, BatharkPos, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		BatharkPos.Z = 0;
		FVector BatharkForwrad = FArmyDataTools::GetModelForwardVector(bathark->Actor);
		FVector BatharkLeft = FArmyDataTools::GetModelLeftVector(bathark->Actor);
		//
		//1、浴室柜侧立面墙
		TArray<FArmyDataTools::EDirection> DirArr = { FArmyDataTools::EDirection::AI_DT_MODEL_LEFT ,  FArmyDataTools::EDirection::AI_DT_MODEL_RIGHT };
		for (FArmyDataTools::EDirection dir : DirArr)
		{
			TSharedPtr<FArmyWallLine> Wall = FArmyDataTools::GetWallLineByModelInDir(*bathark->Actor, dir);
			if (Wall.IsValid() && Wall->GetCoreLine()->Size() >= InEntity->Length && Wall->GetCoreLine()->Size() >= 90)
			{
				FVector VecDirH = Wall->GetDirection();
				FVector WallPos = Wall->GetCoreLine()->GetStart();
				if (FVector::DotProduct(BatharkForwrad, VecDirH) < 0) {
					VecDirH *= -1;
					WallPos = Wall->GetCoreLine()->GetEnd();
				}
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(Wall);
				Direction = Direction.GetSafeNormal();
				FVector WallPosTemp(0, 0, 0);
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*bathark->Actor, Dist, WallPosTemp, dir))//PointIntervalGroundOnDirProjPoint存在bug
				{
					float dis_CP = FVector::Dist2D(BatharkPos, WallPos);
					if (dis_CP <= 50 + BatharkSize.X / 2)
					{
						//
						TArray<FVector> BoxPointArr;
						BoxPointArr.Add(BatharkPos);
						BoxPointArr.Add(WallPos);
						BoxPointArr.Add(WallPos + BatharkForwrad * 20);
						BoxPointArr.Add(BatharkPos + BatharkForwrad * 20);
						FBox ActorBox = FBox(BoxPointArr);

						bool isIntersect = false;
						for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
						{
							FBox ModelBox = model->GetBounds();
							if (ModelBox.Intersect(ActorBox))
							{
								//存在相交的柱子类组件
								isIntersect = true;
								FBox OverlapBox = ModelBox.Overlap(ActorBox);
								float MinSize = (OverlapBox.Min - WallPos).ProjectOnToNormal(VecDirH).Size();
								float MaxSize = (OverlapBox.Max - WallPos).ProjectOnToNormal(VecDirH).Size();
								float size0 = MaxSize > MinSize ? MaxSize : MinSize;
								MinSize = (ModelBox.Min - WallPos).ProjectOnToNormal(VecDirH).Size();
								MaxSize = (ModelBox.Max - WallPos).ProjectOnToNormal(VecDirH).Size();
								float size1 = MaxSize > MinSize ? MaxSize : MinSize;
								float size = size0 > size1 ? size0 : size1;
								if (size + 10 + InEntity->Length / 2 <= 20)
								{
									size = 20;
									TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
									InEntity->CopyTo(TempEntity);
									TempEntity->Location = WallPos + size*VecDirH + FVector(0, 0, 130);
									TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
									EntityArr.Add(TempEntity);
								}
								else
								{
									if (size <= (BatharkSize.Y *  BatharkForwrad).ProjectOnToNormal(VecDirH).Size())
									{
										size = size + 10 + InEntity->Length / 2;
										TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
										InEntity->CopyTo(TempEntity);
										TempEntity->Location = WallPos + size*VecDirH + FVector(0, 0, 130);
										TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
										EntityArr.Add(TempEntity);
									}
									else
									{
										//不布置
									}
								}
							}
						}
						if (!isIntersect)
						{
							//不跟柱子等相交
							TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
							InEntity->CopyTo(TempEntity);
							TempEntity->Location = WallPos + (20 + InEntity->Length / 2)*VecDirH + FVector(0, 0, 130);
							TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
							EntityArr.Add(TempEntity);
						}
					}
				}
			}
		}
		//检测碰撞：门窗和物件
		TSet<AActor*> OtherSet;
		OtherSet.Add(bathark->Actor);
		TArray<TSharedPtr<FArmyModelEntity>> EntityArr1;
		for (auto& TempEntity : EntityArr)
		{
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckWallObjectCollision(EntityActor, InRoom) &&
					!CheckOtherActorCollision(EntityActor, InRoom, OtherSet))
				{
					EntityArr1.Add(TempEntity);
				}
				EntityActor->Destroy();
			}
		}

		TSharedPtr<FArmyModelEntity> PointMinEntity;
		float dis_min = 100000000000;
		for (auto& TempEntity : EntityArr1)
		{
			float dis = FVector::Dist2D(BatharkPos, TempEntity->Location);
			if (dis < dis_min)
			{
				dis_min = dis;
				PointMinEntity = TempEntity;
			}
		}
		if (PointMinEntity.IsValid())
		{
			//优先侧立面
			EntityArray.Add(PointMinEntity);
		}
		else
		{
			//2、马桶背立面墙上方1200mm
			TArray<TSharedPtr<class FArmyModelEntity>> ClosestoolArray;
			InRoom->GetModelEntityArrayByComponentId(AI_Closestool, ClosestoolArray);//马桶（座便器）
			//
			for (auto& closestool : ClosestoolArray)
			{
				TSharedPtr<FArmyWallLine> Wall = FArmyDataTools::GetWallLineByModelInDir(*closestool->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
				if (Wall.IsValid() && Wall->GetCoreLine()->Size() >= InEntity->Length)
				{
					FVector VecDirH = Wall->GetDirection();
					FVector Direction = InRoom->GetRoom()->GetWallLineNormal(Wall);
					Direction = Direction.GetSafeNormal();
					FVector WallPos(0, 0, 0);
					if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*closestool->Actor, Dist, WallPos, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
					{
						//Position
						TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
						InEntity->CopyTo(TempEntity);
						TempEntity->Location = WallPos + FVector(0, 0, 130);
						TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
						//
						TArray<FVector> BoxPointArr;
						FVector Location = TempEntity->Location;
						Location.Z = 0;
						BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2);
						BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2);
						BoxPointArr.Add(Location - VecDirH * InEntity->Length / 2 + Direction * InEntity->Width);
						BoxPointArr.Add(Location + VecDirH * InEntity->Length / 2 + Direction * InEntity->Width);
						FBox ActorBox = FBox(BoxPointArr);
						for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
						{
							FBox ModelBox = model->GetBounds();
							if (ModelBox.Intersect(ActorBox))
							{
								FVector ClosestPoint = ModelBox.GetClosestPointTo(Location);
								FVector vecCL = Location - ClosestPoint;
								if (FVector::DotProduct(vecCL, VecDirH) < 0) {
									VecDirH *= -1;
								}
								FBox OverlapBox = ModelBox.Overlap(ActorBox);
								float size = OverlapBox.GetSize().ProjectOnToNormal(VecDirH).Size() + 10;
								TempEntity->Location += VecDirH * size;
							}
						}
						TSet<AActor*> OtherSet1;
						OtherSet1.Add(bathark->Actor);
						OtherSet1.Add(closestool->Actor);
						//检测碰撞：检测门窗和物件碰撞
						AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
						if (EntityActor)
						{
							if (!CheckWallObjectCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, OtherSet1))
							{
								EntityArray.Add(TempEntity);
							}
							EntityActor->Destroy();
						}
					}
				}
			}
		}
	}
}

/** 卫生间 厕纸架 */
void FArmyAutoAccessoryBathRoom::CalcToiletPaperHolder(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<class FArmyModelEntity>> ClosestoolArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Closestool, ClosestoolArray);//马桶（座便器）
	//
	TArray<TSharedPtr<FArmyPillar>> PillarArr = InRoom->GetPillarArray();//柱子
	TArray<TSharedPtr<FArmyPackPipe>> PackPipeArr = InRoom->GetPackPipeArray();//包立管
	TArray<TSharedPtr<FArmyAirFlue>> AirFlueArr = InRoom->GetAirFlueArray();//风道
	TArray<TSharedPtr<FArmyObject>> ComponentObjArr;//柱子类物件数组
	ComponentObjArr.Append(PillarArr);
	ComponentObjArr.Append(PackPipeArr);
	ComponentObjArr.Append(AirFlueArr);
	//

	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	for (auto& closestool : ClosestoolArray)
	{
		float Dist = 0;//距地750mm
		FVector ClosestoolSize = FArmyDataTools::GetContextItemSize(closestool->FurContentItem);
		FVector ClosestoolPos = FVector::ZeroVector;
		FArmyDataTools::PosOnDirectionOfModel(*closestool->Actor, Dist, ClosestoolPos, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		FVector ClosestoolForwrad = FArmyDataTools::GetModelForwardVector(closestool->Actor);
		FVector ClosestoolLeft = FArmyDataTools::GetModelLeftVector(closestool->Actor);
		//
		//1、马桶侧立面墙
		TArray<FArmyDataTools::EDirection> DirArr = { FArmyDataTools::EDirection::AI_DT_MODEL_LEFT ,  FArmyDataTools::EDirection::AI_DT_MODEL_RIGHT };
		for (FArmyDataTools::EDirection dir : DirArr)
		{
			TSharedPtr<FArmyWallLine> Wall = FArmyDataTools::GetWallLineByModelInDir(*closestool->Actor, dir);
			if (Wall.IsValid() && Wall->GetCoreLine()->Size() >= InEntity->Length)
			{
				FVector VecDirH = Wall->GetDirection();
				FVector WallPos = Wall->GetCoreLine()->GetStart();
				if (FVector::DotProduct(ClosestoolForwrad, VecDirH) < 0) {
					VecDirH *= -1;
					WallPos = Wall->GetCoreLine()->GetEnd();
				}
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(Wall);
				Direction = Direction.GetSafeNormal();
				FVector WallPosTemp(0, 0, 0);
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*closestool->Actor, Dist, WallPosTemp, dir))
				{
					float dis_CP = FVector::Dist2D(ClosestoolPos, WallPos);
					if (dis_CP <= 70 + ClosestoolSize.X / 2)
					{
						//
						TArray<FVector> BoxPointArr;
						BoxPointArr.Add(ClosestoolPos);
						BoxPointArr.Add(WallPos);
						BoxPointArr.Add(WallPos + ClosestoolForwrad * 40);
						BoxPointArr.Add(ClosestoolPos + ClosestoolForwrad * 40);
						FBox ActorBox = FBox(BoxPointArr);
						
						bool isIntersect = false;
						for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
						{
							FBox ModelBox = model->GetBounds();
							if (ModelBox.Intersect(ActorBox))
							{
								//存在相交的柱子类组件
								isIntersect = true;
								FBox OverlapBox = ModelBox.Overlap(ActorBox);
								float MinSize = (OverlapBox.Min - WallPos).ProjectOnToNormal(VecDirH).Size();
								float MaxSize = (OverlapBox.Max - WallPos).ProjectOnToNormal(VecDirH).Size();
								float size0 = MaxSize > MinSize ? MaxSize : MinSize;
								MinSize = (ModelBox.Min - WallPos).ProjectOnToNormal(VecDirH).Size();
								MaxSize = (ModelBox.Max - WallPos).ProjectOnToNormal(VecDirH).Size();
								float size1 = MaxSize > MinSize ? MaxSize : MinSize;
								float size = size0 > size1 ? size0 : size1;
								if (size + 10 + InEntity->Length / 2 <= 40)
								{
									size = 40;
									TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
									InEntity->CopyTo(TempEntity);
									TempEntity->Location = WallPos + size*VecDirH + FVector(0, 0, 75);
									TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
									EntityArr.Add(TempEntity);
								}
								else
								{
									if (size <= (ClosestoolSize.Y * ClosestoolForwrad).ProjectOnToNormal(VecDirH).Size())
									{
										size = size + 10 + InEntity->Length / 2;
										TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
										InEntity->CopyTo(TempEntity);
										TempEntity->Location = WallPos + size*VecDirH + FVector(0, 0, 75);
										TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
										EntityArr.Add(TempEntity);
									}
									else 
									{
										//不布置
									}
								}
							}
						}
						if (!isIntersect)
						{
							//不跟柱子等相交
							TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
							InEntity->CopyTo(TempEntity);
							TempEntity->Location = WallPos + (40 + InEntity->Length / 2)*VecDirH + FVector(0, 0, 75);
							TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
							EntityArr.Add(TempEntity);
						}
					}
				}
			}
		}
		//检测碰撞：检测门窗和物件碰撞
		TArray<TSharedPtr<FArmyModelEntity>> EntityArr1;
		for (auto& TempEntity : EntityArr)
		{
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckWallObjectCollision(EntityActor, InRoom) &&
					!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()))
				{
					EntityArr1.Add(TempEntity);
				}
				EntityActor->Destroy();
			}
		}

		TSharedPtr<FArmyModelEntity> PointMinEntity;
		float dis_min = 100000000000;
		for (auto& TempEntity : EntityArr1)
		{
			float dis = FVector::Dist2D(ClosestoolPos, TempEntity->Location);
			if (dis < dis_min)
			{
				dis_min = dis;
				PointMinEntity = TempEntity;
			}
		}
		if (PointMinEntity.IsValid())
		{
			//优先侧立面
			EntityArray.Add(PointMinEntity);
		}
		else 
		{
			//2、马桶背立面墙
			TSharedPtr<FArmyWallLine> Wall = FArmyDataTools::GetWallLineByModelInDir(*closestool->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
			if (Wall.IsValid() && Wall->GetCoreLine()->Size() >= InEntity->Length)
			{
				FVector VecDirH = Wall->GetDirection();
				if (FVector::DotProduct(ClosestoolLeft, VecDirH) < 0) {
					VecDirH *= -1;
				}
				FVector Direction = InRoom->GetRoom()->GetWallLineNormal(Wall);
				Direction = Direction.GetSafeNormal();
				FVector WallPos(0, 0, 0);
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*closestool->Actor, Dist, WallPos, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
				{
					TArray<TSharedPtr<FArmyModelEntity>> EntityArr_L;
					//Left
					FVector LeftPos = WallPos + (ClosestoolSize.X / 2 + 20) * VecDirH;
					TArray<FVector> LeftBoxPointArr;
					FVector LeftLocation = LeftPos;
					LeftLocation.Z = 0;
					LeftBoxPointArr.Add(LeftLocation - VecDirH * InEntity->Length / 2);
					LeftBoxPointArr.Add(LeftLocation + VecDirH * InEntity->Length / 2);
					LeftBoxPointArr.Add(LeftLocation - VecDirH * InEntity->Length / 2 + Direction * InEntity->Width);
					LeftBoxPointArr.Add(LeftLocation + VecDirH * InEntity->Length / 2 + Direction * InEntity->Width);
					FBox LeftActorBox = FBox(LeftBoxPointArr);
					bool isLeftIntersect = false;
					for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
					{
						FBox ModelBox = model->GetBounds();
						if (ModelBox.Intersect(LeftActorBox))
						{
							isLeftIntersect = true;
							break;
						}
					}
					if (!isLeftIntersect)
					{
						TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
						InEntity->CopyTo(TempEntity);
						TempEntity->Location = LeftPos + FVector(0, 0, 75);
						TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
						EntityArr_L.Add(TempEntity);
					}
					//Right
					FVector RightPos = WallPos + (ClosestoolSize.X / 2 + 20) * VecDirH * -1;
					TArray<FVector> RightBoxPointArr;
					FVector RightLocation = RightPos;
					RightLocation.Z = 0;
					RightBoxPointArr.Add(RightLocation - VecDirH * InEntity->Length / 2);
					RightBoxPointArr.Add(RightLocation + VecDirH * InEntity->Length / 2);
					RightBoxPointArr.Add(RightLocation - VecDirH * InEntity->Length / 2 + Direction * InEntity->Width);
					RightBoxPointArr.Add(RightLocation + VecDirH * InEntity->Length / 2 + Direction * InEntity->Width);
					FBox RightActorBox = FBox(RightBoxPointArr);
					bool isRightIntersect = false;
					for (TSharedPtr<FArmyObject>& model : ComponentObjArr)
					{
						FBox ModelBox = model->GetBounds();
						if (ModelBox.Intersect(RightActorBox))
						{
							isRightIntersect = true;
							break;
						}
					}
					if (!isRightIntersect)
					{
						TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
						InEntity->CopyTo(TempEntity);
						TempEntity->Location = RightPos + FVector(0, 0, 75);
						TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
						EntityArr_L.Add(TempEntity);
					}
					//检测碰撞：门窗和物件
					TArray<TSharedPtr<FArmyModelEntity>> EntityArr_L1;
					for (auto& TempEntity : EntityArr_L)
					{
						AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
						if (EntityActor)
						{
							if (!CheckWallObjectCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()))
							{
								EntityArr_L1.Add(TempEntity);
							}
							EntityActor->Destroy();
						}
					}

					TSharedPtr<FArmyModelEntity> PointMinEntity1;
					float dis_min = 100000000000;
					for (auto& TempEntity : EntityArr_L1)
					{
						float dis = FVector::Dist2D(ClosestoolPos, TempEntity->Location);
						if (dis < dis_min)
						{
							dis_min = dis;
							PointMinEntity1 = TempEntity;
						}
					}
					if (PointMinEntity1.IsValid())
					{
						EntityArray.Add(PointMinEntity1);
					}
				}
			}
		}
	}
}

/*------------------------------------ 卫生间方法实现 End ------------------------------------*/


/*------------------------------------ 阳台方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryBalcony::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 阳台 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

		/** 阳台 绿植 */
	case AI_GreenPlants:
		CalcGreenPlants(InEntity, InRoom, EntityArray);
		break;

		/** 阳台 晾衣杆 */
	case AI_ClotheslinePole:
		CalcClotheslinePole(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

//阳台 绿植
void FArmyAutoAccessoryBalcony::CalcGreenPlants(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//从阳台找出空闲的位置，然后布置绿植，需要修改location
	TArray<TSharedPtr<FArmyEditPoint>> Points = InRoom->GetRoom()->GetPoints();
	TArray<TSharedPtr<class FArmyWallLine>> WallLines = InRoom->GetRoom()->GetWallLines();
	//
	TArray<TSharedPtr<FArmyModelEntity>> EntityArr;
	for (auto& Point : Points)
	{
		//
		TArray<TSharedPtr<FArmyEditPoint>> OtherPoints;
		for (auto& WallLine : WallLines)
		{
			TArray<FObjectWeakPtr> WallModels;
			WallLine->GetAppendObjects(WallModels);
			bool hasDoor = false;
			for (auto& Model : WallModels)
			{
				TSharedPtr<FArmyObject> ModelObject = StaticCastSharedPtr<FArmyObject>(Model.Pin());
				if (ModelObject->GetType() == OT_Door || ModelObject->GetType() == OT_SecurityDoor || ModelObject->GetType() == OT_SlidingDoor || ModelObject->GetType() == OT_Pass || ModelObject->GetType() == OT_NewPass || ModelObject->GetType() == OT_DoorHole)
				{
					hasDoor = true;
					break;
				}
			}
			if (!hasDoor)
			{
				TSharedPtr<FArmyEditPoint> StartPoint = WallLine->GetCoreLine()->GetStartPointer();
				TSharedPtr<FArmyEditPoint> EndPoint = WallLine->GetCoreLine()->GetEndPointer();
				if (Point == StartPoint)
				{
					OtherPoints.Add(EndPoint);
				}
				if (Point == EndPoint)
				{
					OtherPoints.Add(StartPoint);
				}
			}
		}
		if (OtherPoints.Num() == 2)
		{
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
			InEntity->CopyTo(TempEntity);
			//
			FVector Point0 = Point->GetPos();
			FVector Point1 = OtherPoints[0]->GetPos();
			FVector Point2 = OtherPoints[1]->GetPos();
			FVector vec1 = FVector(Point1.X - Point0.X, Point1.Y - Point0.Y, Point1.Z - Point0.Z);
			FVector vec2 = FVector(Point2.X - Point0.X, Point2.Y - Point0.Y, Point2.Z - Point0.Z);
			vec1 = vec1.GetSafeNormal(); vec1 *= InEntity->Length;
			vec2 = vec2.GetSafeNormal(); vec2 *= InEntity->Width;
			FVector vec3 = vec1 + vec2;
			//
			TempEntity->Location = Point->GetPos() + vec3;
			//
			if (InRoom->GetRoom()->IsPointInRoom(TempEntity->Location))
			{
				EntityArr.Add(TempEntity);
			}
		}
	}

	//碰撞检测：墙体、柱子类和物件
	for (auto& TempEntity : EntityArr)
	{
		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
		if (EntityActor)
		{
			bool isNotIntersect = !CheckWallCornerCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
								!CheckWallCollision(EntityActor, InRoom);
			EntityActor->Destroy();
			if (isNotIntersect)
			{
				EntityArray.Add(TempEntity);
			}
		}
	}
}

//阳台 晾衣杆
void FArmyAutoAccessoryBalcony::CalcClotheslinePole(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
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
	if (MaxWindow.IsValid() && MaxWallLine.IsValid())
	{
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
			TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByHardware(MaxWindow, InRoom->GetRoom());
			FVector Direction = InRoom->GetRoom()->GetWallLineNormal(WallLine);
			Direction = Direction.GetSafeNormal();
			TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);//设置Rotation
																			  //
			FVector Centre = (MaxWallLine->GetCoreLine()->GetStart() + MaxWallLine->GetCoreLine()->GetEnd()) / 2;
			Centre += Direction * (min_length / 2);
			Centre.Z = FArmySceneData::WallHeight;
			TempEntity->Location = Centre;//设置Location

										  //碰撞检测：柱子类
			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempEntity->FurContentItem, TempEntity->Location, TempEntity->Rotation, TempEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckWallCornerCollision(EntityActor, InRoom))
				{
					EntityArray.Add(TempEntity);
				}
				EntityActor->Destroy();
			}
		}
	}
}

/*------------------------------------ 阳台方法实现 End ------------------------------------*/


/*------------------------------------ 厨房方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryKitchen::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 厨房 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

/*------------------------------------ 厨房方法实现 End ------------------------------------*/


/*------------------------------------ 餐厅方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryDiningroom::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 餐厅 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

		/** 餐厅 窗帘 */
	case AI_CurtainStd:
		CalcCurtainStd(InEntity, InRoom, EntityArray);
		break;

		/** 餐厅 挂画 */
	case AI_Titanic:
		CalcTitanic(InEntity, InRoom, EntityArray);
		break;

		/** 餐厅 地毯 */
	case AI_Carpet:
		CalcCarpetLayout(InEntity, InRoom, EntityArray);
		break;

		/** 餐厅 餐具 */
	case AI_Dinnerware:
		CalcDinnerware(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

void FArmyAutoAccessoryDiningroom::CalcLampMain(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//
	TArray<TSharedPtr<class FArmyModelEntity>> DeskArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DiningTable, DeskArray);
	for (auto &desk : DeskArray)
	{
		//找出最长的墙
		TSharedPtr<FArmyWallLine> MaxWallLine;
		float MaxLength = 0;
		TArray<TSharedPtr<FArmyWallLine>> WallLines = InRoom->GetRoom()->GetWallLines();
		for (auto& WallLine : WallLines)
		{
			float length = WallLine->GetCoreLine()->Size();
			if (length > MaxLength)
			{
				MaxLength = length;
				MaxWallLine = WallLine;
			}
		}
		//
		FVector Position = FVector::ZeroVector;
		FArmyDataTools::PosOnDirectionOfModel(*desk->Actor, FArmySceneData::WallHeight - InRoom->GetCeilingDropOff(), Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
		//
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
		InEntity->CopyTo(TempEntity);
		TempEntity->Location = Position;
		TempEntity->Rotation = MaxWallLine->GetDirection().Rotation();

		//碰撞检测：不进行碰撞检测！？
		EntityArray.Add(TempEntity);
	}
}

void FArmyAutoAccessoryDiningroom::CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 餐厅地毯摆放 @纪仁泽 */
	TArray<TSharedPtr<class FArmyModelEntity>> DiningTableArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DiningTable, DiningTableArray);

	for (auto& DiningTable : DiningTableArray)
	{
		/** 构造出新的Entity */
		TSharedPtr<FArmyModelEntity> TempCarpetEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempCarpetEntity);

		/** 进行Rotation转换 */
		TempCarpetEntity->Rotation = DiningTable->Rotation;

		/** temp_entity.center = dining_table_square.center */
		TempCarpetEntity->Location = DiningTable->Location;

		AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempCarpetEntity->FurContentItem, TempCarpetEntity->Location, TempCarpetEntity->Rotation, TempCarpetEntity->Scale3D);
		if (EntityActor)
		{
			/** 初始化Entity中的Actor */
			TempCarpetEntity->Actor = EntityActor;
			/** 计算地毯能否调整到房间内 */
			bool bInRoom = FArmyDataTools::CalModelAdjustToRoom(TempCarpetEntity, InRoom);
			if (bInRoom)
			{
				TempCarpetEntity->Location = EntityActor->GetActorLocation();
				TempCarpetEntity->Rotation = EntityActor->GetActorRotation();
				TempCarpetEntity->Scale3D = EntityActor->GetActorScale3D();

				//碰撞检测：不进行碰撞检测
				EntityArray.Emplace(TempCarpetEntity);
			}
			TempCarpetEntity->Actor->Destroy();
		}
	}
}

void FArmyAutoAccessoryDiningroom::CalcTitanic(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	/** 餐厅挂画 @纪仁泽 */

	/** 有餐边柜 */
	TArray<TSharedPtr<class FArmyModelEntity>> SideboardArray;
	InRoom->GetModelEntityArrayByComponentId(AI_Sideboard, SideboardArray);
	/** 检测是否有餐边柜 */
	if (SideboardArray.Num() > 0)
	{
		for (auto& Sideboard : SideboardArray)
		{
			/** 餐边柜背面的墙长度需要大于100cm */
			TSharedPtr<FArmyWallLine> SideboardBackWardWall = FArmyDataTools::GetWallLineByModelInDir(*Sideboard->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);
			if (SideboardBackWardWall.IsValid() && SideboardBackWardWall->GetCoreLine()->Size() >= 100.f)
			{
				/** temp_entity.center = height=150 */
				FVector TempCenter(0, 0, 0);
				float Dist = 150;
				/** 餐边柜背后的墙体 150.f */
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*Sideboard->Actor, Dist, TempCenter))
				{
					TSharedPtr<FArmyModelEntity> TempTitanicEntity = MakeShareable(new FArmyModelEntity);
					InEntity->CopyTo(TempTitanicEntity);
					TempTitanicEntity->Location = TempCenter;
					TempTitanicEntity->Rotation = Sideboard->Rotation;
					//
					AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntity->FurContentItem, TempTitanicEntity->Location, TempTitanicEntity->Rotation, TempTitanicEntity->Scale3D);
					if (EntityActor)
					{
						for (int i = 0; i < 8; ++i)
						{
							if (!CheckWallCornerCollision(EntityActor, InRoom) &&
								!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
								!CheckWallObjectCollision(EntityActor, InRoom) &&
								!CheckWallCollision(EntityActor, InRoom) &&
								!CheckCeilingCollision(EntityActor, InRoom))
							{
								TempTitanicEntity->Location = EntityActor->GetActorLocation();
								TempTitanicEntity->Rotation = EntityActor->GetActorRotation();
								TempTitanicEntity->Scale3D = EntityActor->GetActorScale3D();
								EntityArray.Emplace(TempTitanicEntity);
								break;
							}
							EntityActor->SetActorLocation(EntityActor->GetActorLocation() + FVector(0, 0, 10));
						}
						EntityActor->Destroy();
					}
				}
			}
		}
	}

	/** 没有餐边柜，根据餐桌来布置 */
	else
	{
		/** 有餐桌 */
		TArray<TSharedPtr<class FArmyModelEntity>> DiningTableArray;
		InRoom->GetModelEntityArrayByComponentId(AI_DiningTable, DiningTableArray);
		for (auto& DiningTable : DiningTableArray)
		{
			/** 构造出新的Entity */
			TSharedPtr<FArmyModelEntity> TempTitanicEntity = MakeShareable(new FArmyModelEntity);
			InEntity->CopyTo(TempTitanicEntity);

			/** 寻找距离餐桌最近的墙 */
			TSharedPtr<class FArmyWallLine> LeftWall = FArmyDataTools::GetWallLineByModelInDir(*DiningTable->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_LEFT);
			TSharedPtr<class FArmyWallLine> ForwardWall = FArmyDataTools::GetWallLineByModelInDir(*DiningTable->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD);
			TSharedPtr<class FArmyWallLine> RightWall = FArmyDataTools::GetWallLineByModelInDir(*DiningTable->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_RIGHT);
			TSharedPtr<class FArmyWallLine> BackwardWall = FArmyDataTools::GetWallLineByModelInDir(*DiningTable->Actor, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD);

			/** 模型离墙距离 */
			float ModelWallDistance = 999999.0f;

			/** 左 */
			if (FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), LeftWall->GetCoreLine()->GetStart(), LeftWall->GetCoreLine()->GetEnd())<ModelWallDistance)
			{
				ModelWallDistance = FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), LeftWall->GetCoreLine()->GetStart(), LeftWall->GetCoreLine()->GetEnd());
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*DiningTable->Actor, 150.f, TempTitanicEntity->Location, FArmyDataTools::EDirection::AI_DT_MODEL_LEFT))
				{
					TempTitanicEntity->Rotation = DiningTable->Actor->GetActorRotation() + FRotator(0, 270, 0);
				}
			}

			/** 前 */
			if (FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), ForwardWall->GetCoreLine()->GetStart(), ForwardWall->GetCoreLine()->GetEnd()) < ModelWallDistance)
			{
				ModelWallDistance = FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), ForwardWall->GetCoreLine()->GetStart(), ForwardWall->GetCoreLine()->GetEnd());
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*DiningTable->Actor, 150.f, TempTitanicEntity->Location, FArmyDataTools::EDirection::AI_DT_MODEL_FORWARD))
				{
					TempTitanicEntity->Rotation = DiningTable->Actor->GetActorRotation() + FRotator(0, 180, 0);
				}
			}

			/** 右 */
			if (FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), RightWall->GetCoreLine()->GetStart(), RightWall->GetCoreLine()->GetEnd()) < ModelWallDistance)
			{
				ModelWallDistance = FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), RightWall->GetCoreLine()->GetStart(), RightWall->GetCoreLine()->GetEnd());
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*DiningTable->Actor, 150.f, TempTitanicEntity->Location, FArmyDataTools::EDirection::AI_DT_MODEL_RIGHT))
				{
					TempTitanicEntity->Rotation = DiningTable->Actor->GetActorRotation() + FRotator(0, 90, 0);
				}
			}

			/** 后 */
			if (FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), BackwardWall->GetCoreLine()->GetStart(), BackwardWall->GetCoreLine()->GetEnd()) < ModelWallDistance)
			{
				ModelWallDistance = FArmyMath::Distance(DiningTable->Actor->GetActorLocation(), BackwardWall->GetCoreLine()->GetStart(), BackwardWall->GetCoreLine()->GetEnd());
				if (FArmyDataTools::PointIntervalGroundOnDirProjPoint(*DiningTable->Actor, 150.f, TempTitanicEntity->Location, FArmyDataTools::EDirection::AI_DT_MODEL_BACKWARD))
				{
					TempTitanicEntity->Rotation = DiningTable->Actor->GetActorRotation() + FRotator(0, 0, 0);
				}
			}

			AActor* EntityActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), TempTitanicEntity->FurContentItem, TempTitanicEntity->Location, TempTitanicEntity->Rotation, TempTitanicEntity->Scale3D);
			if (EntityActor)
			{
				if (!CheckWallCornerCollision(EntityActor, InRoom) &&
					!CheckOtherActorCollision(EntityActor, InRoom, TSet<AActor*>()) &&
					!CheckWallObjectCollision(EntityActor, InRoom) &&
					!CheckWallCollision(EntityActor, InRoom) &&
					!CheckCeilingCollision(EntityActor, InRoom))
				{
					TempTitanicEntity->Location = EntityActor->GetActorLocation();
					TempTitanicEntity->Rotation = EntityActor->GetActorRotation();
					TempTitanicEntity->Scale3D = EntityActor->GetActorScale3D();
					EntityArray.Emplace(TempTitanicEntity);
				}
				EntityActor->Destroy();
			}
		}
	}
}

//餐厅 餐具
void FArmyAutoAccessoryDiningroom::CalcDinnerware(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	TArray<TSharedPtr<class FArmyModelEntity>> ChairArray;
	InRoom->GetModelEntityArrayByComponentId(AI_DiningChair, ChairArray);
	//获取对应的桌子
	TSharedPtr<FArmyModelEntity> DeskEntity;
	if (ChairArray.Num() > 0)
	{
		TArray<TSharedPtr<FArmyModelEntity>> DeskArray;
		InRoom->GetModelEntityArrayByComponentId(AI_DiningTable, DeskArray);
		FBox InActorBox = GVC->GetActorCollisionBox(ChairArray[0]->Actor);
		for (auto& Desk : DeskArray)
		{
			AActor* ModelActor = Desk->Actor;
			if (ModelActor)
			{
				FBox ModelActorBox = GVC->GetActorCollisionBox(ModelActor);
				if (InActorBox.Intersect(ModelActorBox))
				{
					DeskEntity = Desk;
					break;
				}
			}
		}
	}

	if (DeskEntity.IsValid())
	{
		for (auto& chair : ChairArray)
		{
			FVector Forward = FArmyDataTools::GetModelForwardVector(chair->Actor);
			//
			FVector Position = FVector::ZeroVector;
			FVector Size = FArmyDataTools::GetContextItemSize(DeskEntity->FurContentItem);
			FArmyDataTools::PosOnDirectionOfModel(*chair->Actor, Size.Z*1.0f, Position, FArmyDataTools::EDirection::AI_DT_MODEL_UP);
			FVector ChairSize = FArmyDataTools::GetContextItemSize(chair->FurContentItem);
			Position += Forward * (InEntity->Width * 0.5 + ChairSize.Y * 0.25 + 5);
			//
			TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity());
			InEntity->CopyTo(TempEntity);
			TempEntity->Location = Position;
			TempEntity->Rotation = (Forward * -1).Rotation() + FRotator(0, -90, 0);

			//碰撞检测：不进行碰撞检测
			EntityArray.Add(TempEntity);
		}
	}
}

/*------------------------------------ 餐厅方法实现 End ------------------------------------*/


/*------------------------------------ 玄关方法实现 Start ------------------------------------*/

void FArmyAutoAccessoryHallway::CalcAccessoryLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	switch (InEntity->ComponentId)
	{
		/** 玄关 地毯 */
	case AI_Carpet:
		CalcCarpetLayout(InEntity, InRoom, EntityArray);
		break;

		/** 玄关 吊灯 吸顶灯 集成LED灯 主灯*/
	case AI_LampDrop:
	case AI_LampCeiling:
	case AI_LedLight:
	case AI_LampMain:
		CalcLampMain(InEntity, InRoom, EntityArray);
		break;

	default:
		break;
	}
}

void FArmyAutoAccessoryHallway::CalcCarpetLayout(TSharedPtr<FArmyModelEntity>& InEntity, TSharedPtr<FArmyRoomEntity>& InRoom, TArray<TSharedPtr<FArmyModelEntity>>& EntityArray)
{
	//从玄关找出门的位置，然后布置玄关地毯，需要修改location和rotation
	TSharedPtr<FArmySecurityDoor> door = InRoom->GetSecurityDoor();
	if (door.IsValid())
	{
		TSharedPtr<FArmyWallLine> WallLine = FArmyDataTools::GetWallLineByHardware(door, InRoom->GetRoom());
		FVector Direction = InRoom->GetRoom()->GetWallLineNormal(WallLine);
		Direction = Direction.GetSafeNormal();
		//先旋转后平移
		TSharedPtr<FArmyModelEntity> TempEntity = MakeShareable(new FArmyModelEntity);
		InEntity->CopyTo(TempEntity);
		float width = InEntity->Width < InEntity->Length ? InEntity->Width : InEntity->Length;
		TempEntity->Location = door->GetPos() + (width / 2 + door->GetWidth() / 2) * Direction;
		TempEntity->Rotation = Direction.Rotation() + FRotator(0, -90, 0);
		
		//碰撞检测：不进行碰撞检测
		EntityArray.Add(TempEntity);
	}
}

/*------------------------------------ 玄关方法实现 End ------------------------------------*/

