#include "ArmyRoomEntity.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyRegion.h"
#include "ArmyCommonTypes.h"
#include "ArmyHardware.h"
#include "ArmyDataTools.h"
#include "ArmyResourceModule.h"
#include "JsonResourceParser.h"
#include "ArmyCommonTools.h"
#include "ArmyAutoDesignHttp.h"
#include "ArmyDesignPackage.h"
#include "ArmyModelEntity.h"
#include "ArmyActorItem.h"
#include "ArmyHardModeData.h"
#include "ArmySecurityDoor.h"
#include "ArmyRect.h"
#include "ArmyUser.h"
#include "ArmyBeam.h"
#include "ArmyPillar.h"
#include "ArmyAirFlue.h"
#include "ArmyFurniture.h"
#include "ArmyPackPipe.h"
#include "ArmyClipper.h"
#include "ArmyWallLine.h"

// 构造函数
FArmyRoomEntity::FArmyRoomEntity(TSharedPtr<FArmyRoom> InRoom)
{
	CeilingDropOff = 0;
	CrownMouldingHeight = 0;
	// 初始化房间数据
	if (!InRoom.IsValid())
	{
		return;
	}
	CurrentRoom = InRoom;

	// 初始化区域数据
	TArray<TWeakPtr<FArmyObject>>	  RegionList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Region, RegionList);
	for (auto It : RegionList)
	{
		if (It.Pin().IsValid())
		{
			TSharedPtr<FArmyRegion> RegionObj = StaticCastSharedPtr<FArmyRegion>(It.Pin());
			if (CurrentRoom->GetUniqueID().ToString() == RegionObj->GetSpaceObjectId())
			{
				RegionArray.Add(StaticCastSharedPtr<FArmyRegion>(It.Pin()));
			}
		}
	}

	// 初始化门窗数据
	TArray<TSharedPtr<FArmyHardware>> HardwareArray;
	CurrentRoom->GetHardwareListRelevance(HardwareArray);
	for (auto& Hardware : HardwareArray)
	{
		// 初始化门
		if (Hardware->GetType() == OT_Door || Hardware->GetType() == OT_SlidingDoor || Hardware->GetType() == OT_SecurityDoor || Hardware->GetType() == OT_Pass || Hardware->GetType() == OT_NewPass)
		{
			DoorArray.Push(Hardware);
		}

		// 初始化窗户
		if (Hardware->GetType() == OT_Window || Hardware->GetType() == OT_FloorWindow || Hardware->GetType() == OT_RectBayWindow || Hardware->GetType() == OT_TrapeBayWindow || Hardware->GetType() == OT_CornerBayWindow)
		{
			WindowArray.Push(Hardware);
		}
	}

	// 初始化梁
	TArray<TWeakPtr<FArmyObject>> AllBeamArray;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_Beam, AllBeamArray);
	for (auto It : AllBeamArray)
	{
		TSharedPtr<FArmyBeam> Beam = StaticCastSharedPtr<FArmyBeam>(It.Pin());
		if (CurrentRoom->IsPointInRoom(Beam->GetPos()))
		{
			BeamArray.Add(Beam);
		}
	}

	// 初始化柱子
	TArray<TWeakPtr<FArmyObject>> AllPillarArray;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_Pillar, AllPillarArray);
	for (auto It : AllPillarArray)
	{
		TSharedPtr<FArmyPillar> Pillar = StaticCastSharedPtr<FArmyPillar>(It.Pin());
		if (CurrentRoom->IsPointInRoom(Pillar->GetPos()))
		{
			PillarArray.Add(Pillar);
		}
		else
		{
			// 处理含有柱面为墙体的柱子
			if (FArmyDataTools::IsObjectOfRoom(InRoom, Pillar))
			{
				PillarArray.Add(Pillar);
			}
		}
	}

	// 初始化风道
	TArray<TWeakPtr<FArmyObject>> AllAirFlueArray;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_AirFlue, AllAirFlueArray);
	for (auto It : AllAirFlueArray)
	{
		TSharedPtr<FArmyAirFlue> AirFlue = StaticCastSharedPtr<FArmyAirFlue>(It.Pin());
		if (CurrentRoom->IsPointInRoom(AirFlue->GetPos()))
		{
			AirFlueArray.Add(AirFlue);
		}
		else
		{
			// 处理含有柱面为墙体的柱子
			if (FArmyDataTools::IsObjectOfRoom(InRoom, AirFlue))
			{
				AirFlueArray.Add(AirFlue);
			}
		}
	}

	// 构件
	TArray<TWeakPtr<FArmyObject>> AllComponentArray;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_ComponentBase, AllComponentArray);
	for (auto It : AllComponentArray)
	{
		TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(It.Pin());
		if (CurrentRoom->IsPointInRoom(Furniture->GetBasePos()))
		{
			ComponentArray.Add(Furniture);
		}
	}

	// 初始化包立管
	TArray<TWeakPtr<FArmyObject>> AllPackPipeArray;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_PackPipe, AllPackPipeArray);
	for (auto It : AllPackPipeArray)
	{
		TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(It.Pin());
		
		if (CurrentRoom->IsPointInRoom(PackPipe->GetBounds().GetCenter()))
		{
			PackPipeArray.Add(PackPipe);
		}
		else
		{
			// 处理含有柱面为墙体的柱子
			if (FArmyDataTools::IsObjectOfRoom(InRoom, PackPipe))
			{
				PackPipeArray.Add(PackPipe);
			}
		}
	}

	// 初始化入户门
	TArray<TWeakPtr<FArmyObject>> SecurityDoorArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, SecurityDoorArray);
	if (SecurityDoorArray.Num() > 0)
	{
		SecurityDoor = StaticCastSharedPtr<FArmySecurityDoor>(SecurityDoorArray[0].Pin());
	}

	// 初始化软装相关模型
	TArray<TSharedPtr<FArmyModelEntity> > OutModelArr;
	FArmyDataTools::GetAllModelsByRoom(CurrentRoom, OutModelArr);
	for (auto &OutModel : OutModelArr)
	{
		if (OutModel.IsValid())
		{
			FContentItemPtr ContentIt = OutModel->FurContentItem;
			if (ContentIt.IsValid())
			{
				AllModelMap.Add(ContentIt->AiCode, OutModel);
				ModelEntityArray.Add(OutModel);
			}
		}
	}
	//  初始化硬装-地面面片
	TArray<TSharedPtr<class FArmyRoomSpaceArea>>  RoomSpaceArea = FArmySceneData::Get()->GetRoomAttachedSurfacesWidthRoomID(CurrentRoom->GetUniqueID().ToString());
	for (auto It : RoomSpaceArea)
	{
		// 0 表示地，1表示墙，2 表示顶
		if (It.IsValid())
		{
			if (It->SurfaceType == 0)
			{
				FloorSpaceArray.Add(It);
			}
			else if (It->SurfaceType == 1)
			{
				WallSpaceArray.Add(It);
			}
			else if(It->SurfaceType == 2)
			{
				CeilingSpaceArray.Add(It);
			}
		}
	}
	
	// 初始化入光口
	InitSunnyOpening();
	// 初始化们转窗户
	InitDoorWindow();
	// 进行房间的变换
	CalModifyRoom();
}

FArmyRoomEntity::FArmyRoomEntity()
{

}

// 获得当前房间
TSharedPtr<class FArmyRoom> FArmyRoomEntity::GetRoom()
{
	return CurrentRoom;
}

// 获得所有的模型
void FArmyRoomEntity::GetAllModels(TArray<TSharedPtr<class FArmyModelEntity>> &OutModelEntityArray)
{
	OutModelEntityArray = ModelEntityArray;
}

// 通过类型获得房间模型
void FArmyRoomEntity::GetModelsByType(TArray<TSharedPtr<class FArmyModelEntity>> &OutModelEntityArray, int32 InModelType)
{
	for (auto It : ModelEntityArray)
	{
		if (It.IsValid() && It->FurContentItem.IsValid())
		{
			if (RegionArray.Num() > 0)
			{
				for (auto It2 : RegionArray)
				{
					if (!It2.IsValid())
					{
						continue;
					}

					if (FArmyDataTools::GetLayoutModelType(It, FArmyDataTools::GetRoomType(It2->GetSplitRegionId())) == InModelType)
					{
						OutModelEntityArray.Add(It);
						break;
					}
				}
			}
			else
			{
				if (FArmyDataTools::GetLayoutModelType(It, GetAiRoomType()) == InModelType)
				{
					OutModelEntityArray.Add(It);
				}
			}
		}
	}
}

// 是否存在指定类型的模型
bool FArmyRoomEntity::IsExistObj(EAIComponentCode InCode)
{
	return AllModelMap.Contains(InCode);
}

//// 通过构件Id获得模型列表
//void FArmyRoomEntity::GetModelArrayByComponentId(EAIComponentCode ComponentCode, TArray<TSharedPtr<class FArmyActorItem>> &OutModelArray)
//{
//	AllModelMap.MultiFind(ComponentCode, OutModelArray);
//}

// 通过构件Id获得模型列表
void FArmyRoomEntity::GetModelEntityArrayByComponentId(EAIComponentCode ComponentCode, TArray<TSharedPtr<class FArmyModelEntity>> &OutModelArray)
{
	AllModelMap.MultiFind(ComponentCode, OutModelArray);
}

// 获得门的数据
TArray<TSharedPtr<class FArmyHardware>> FArmyRoomEntity::GetDoorsActor()
{
	return DoorArray;
}

// 获得窗户的数据
TArray<TSharedPtr<class FArmyHardware>> FArmyRoomEntity::GetWindowsActor()
{
	return WindowArray;
}

// 获得区域的数据
TArray<TSharedPtr<class FArmyRegion>> FArmyRoomEntity::GetRegionArray()
{
	return RegionArray;
}
// 获得入户门
TSharedPtr<class FArmySecurityDoor> FArmyRoomEntity::GetSecurityDoor()
{
	return SecurityDoor;
}

// 获得地面面片
TArray<TSharedPtr<FArmyRoomSpaceArea>> FArmyRoomEntity::GetFloorSpaceArray()
{
	return FloorSpaceArray;
}
// 获得墙面面片
TArray<TSharedPtr<FArmyRoomSpaceArea>> FArmyRoomEntity::GetWallSpaceArray()
{
	return WallSpaceArray;
}
// 获得顶面面片
TArray<TSharedPtr<FArmyRoomSpaceArea>> FArmyRoomEntity::GetCeilingSpaceArray()
{
	return CeilingSpaceArray;
}
//// 是否区域数据
bool FArmyRoomEntity::GetIsRegion()
{
	if (RegionArray.Num() > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 添加模型
bool FArmyRoomEntity::AddModel(TSharedPtr<class FArmyModelEntity> InModelEntity)
{
	if (InModelEntity.IsValid())
	{
		ModelEntityArray.Add(InModelEntity);
		AllModelMap.Add(InModelEntity->FurContentItem->AiCode, InModelEntity);
		return true;
	}
	else
	{
		return false;
	}
}
// 移除模型
bool FArmyRoomEntity::RemoveModel(TSharedPtr<class FArmyModelEntity> InModelEntity)
{
	if (InModelEntity.IsValid())
	{
		ModelEntityArray.Remove(InModelEntity);
		AllModelMap.Remove(InModelEntity->FurContentItem->AiCode);
		return true;
	}
	return false;
}


TArray<TSharedPtr<class FArmyHardware> > FArmyRoomEntity::GetDoorByType(EObjectType ObjType)
{
	TArray<TSharedPtr<FArmyHardware> > HardWareArrTemp;
	TArray<TSharedPtr<FArmyHardware> > HardWareArr;
	CurrentRoom->GetHardwareListRelevance(HardWareArrTemp);
	//AActor * SecurityDoor = nullptr;
	for (TSharedPtr<FArmyHardware> & HardWareIt : HardWareArrTemp)
	{
		if (HardWareIt->GetType() == ObjType)
		{
			HardWareArr.Add(HardWareIt);
		}
	}
	return HardWareArr;
}

ERoomType FArmyRoomEntity::GetAiRoomType()
{
	return FArmyDataTools::GetRoomType(CurrentRoom->GetSpaceId());
}

// 序列化为json数据
void FArmyRoomEntity::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter, FString RegionNo, bool IsRegion)
{
	// 版本号
	JsonWriter->WriteValue("Version", *FArmyUser::Get().GetApplicationVersion());

	// 房间
	if (CurrentRoom.IsValid())
	{
		JsonWriter->WriteArrayStart(TEXT("roomList"));
		JsonWriter->WriteObjectStart();
		if (ModifyRoom.IsValid())
		{
			ModifyRoom->SerializeToJson(JsonWriter);
		}
		else
		{
			CurrentRoom->SerializeToJson(JsonWriter);
		}
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteArrayEnd();
	}

	//普通门
	JsonWriter->WriteArrayStart(TEXT("singleDoorList"));
	for (auto It : DoorArray)
	{
		if (IsDoorToWindow(It))
		{
			continue;
		}
		if (It->GetType() == OT_Door)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 推拉门
	JsonWriter->WriteArrayStart(TEXT("slidingDoorList"));
	for (auto It : DoorArray)
	{
		if (IsDoorToWindow(It))
		{
			continue;
		}
		if (It->GetType() == OT_SlidingDoor)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 垭口
	JsonWriter->WriteArrayStart(TEXT("passList"));
	for (auto It : DoorArray)
	{
		if (IsDoorToWindow(It))
		{
			continue;
		}
		if (It->GetType() == OT_Pass)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 新垭口
	JsonWriter->WriteArrayStart(TEXT("newPassList"));
	for (auto It : DoorArray)
	{
		if (IsDoorToWindow(It))
		{
			continue;
		}
		if (It->GetType() == OT_NewPass)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	//// 门洞
	//JsonWriter->WriteArrayStart(TEXT("doorHole"));
	//for (auto It : DoorArray)
	//{
	//	if (It->GetType() == OT_DoorHole)
	//	{
	//		JsonWriter->WriteObjectStart();
	//		It->SerializeToJson(JsonWriter);
	//		JsonWriter->WriteObjectEnd();
	//	}
	//}
	//JsonWriter->WriteArrayEnd();


	// 防盗门
	if (SecurityDoor.IsValid())
	{
		JsonWriter->WriteArrayStart(TEXT("securityDoorList"));
		JsonWriter->WriteObjectStart();
		SecurityDoor->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteArrayEnd();
	}


	//标准窗
	JsonWriter->WriteArrayStart(TEXT("windowList"));
	for (auto It : WindowArray)
	{
		if (It->GetType() == OT_Window)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 落地窗
	JsonWriter->WriteArrayStart(TEXT("floorWindowList"));
	for (auto It : WindowArray)
	{
		if (It->GetType() == OT_FloorWindow)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	//飘窗
	JsonWriter->WriteArrayStart(TEXT("bayFloorWindowList"));
	for (auto It : WindowArray)
	{
		if (It->GetType() == OT_RectBayWindow)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	//梯形飘窗
	JsonWriter->WriteArrayStart(TEXT("TrapeBayWindowList"));
	for (auto It : WindowArray)
	{
		if (It->GetType() == OT_TrapeBayWindow)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 柱子
	JsonWriter->WriteArrayStart(TEXT("pillarList"));
	for (auto It : PillarArray)
	{
		if (It->GetType() == OT_Pillar)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 风道
	JsonWriter->WriteArrayStart(TEXT("airFlueList"));
	for (auto It : AirFlueArray)
	{
		if (It->GetType() == OT_AirFlue)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 包立管
	JsonWriter->WriteArrayStart(TEXT("packPipeList"));
	for (auto It : PackPipeArray)
	{
		if (It->GetType() == OT_PackPipe)
		{
			JsonWriter->WriteObjectStart();
			It->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 区域
	JsonWriter->WriteArrayStart(TEXT("regionList"));
	for (auto &It : RegionArray)
	{
		if (IsRegion && RegionNo == It->GetUniqueID().ToString())
		{
			It->SetIsVirtual(false);
		}
		else
		{
			It->SetIsVirtual(true);
		}
		JsonWriter->WriteObjectStart();
		It->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}

	JsonWriter->WriteArrayEnd();


	// 初始化入光口
	if (SunnyOpening.IsValid())
	{
		JsonWriter->WriteArrayStart(TEXT("sunnyOpeningList"));
		JsonWriter->WriteObjectStart();
		SunnyOpening->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteArrayEnd();
	}

	// 门转窗户
	JsonWriter->WriteArrayStart(TEXT("doorWindowList"));
	for (auto It : DoorWindowArray)
	{
		JsonWriter->WriteObjectStart();
		It->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}


// 创建颜值包
TSharedPtr<class FArmyDesignPackage> FArmyRoomEntity::CreateDesignPackage()
{
	// 是否可以创建颜值包
	if (CurrentRoom.IsValid())
	{
		TSharedPtr<FArmyDesignPackage> ResultPackage = MakeShareable(new FArmyDesignPackage());
		// 设置房间类型
		ResultPackage->SetSpaceId(GetAiRoomType());
		ResultPackage->SetRoomName(CurrentRoom->GetSpaceName());
		float RoomArea, RoomPerimeter;
		CurrentRoom->GetRoomAreaAndPerimeter(RoomArea, RoomPerimeter);
		ResultPackage->SetRoomArea(RoomArea);

		// 创建布局模型包
		for (auto &It : ModelEntityArray)
		{
			// 布局模型
			if (FArmyDataTools::GetLayoutModelType(It, GetAiRoomType()) == 1)
			{
				ResultPackage->AddLayoutModelList(It);
			}
			// 配饰模型
			else if (FArmyDataTools::GetLayoutModelType(It, GetAiRoomType()) == 2)
			{
				ResultPackage->AddOrnamentModelList(It);
			}
		}

		// 创建硬装
		TSharedPtr<FArmyHardModeData> HardModeData = MakeShareable(new FArmyHardModeData(this->AsShared()));
		ResultPackage->SetHardModeData(HardModeData);

		return ResultPackage;
	}
	else
	{
		return nullptr;
	}

}

// 获得吊顶下吊高度
float FArmyRoomEntity::GetCeilingDropOff()
{
	return CeilingDropOff;
}
// 获得顶角线高度
float FArmyRoomEntity::GetCrownMouldingHeight()
{
	return CrownMouldingHeight;
}

// 获取吊顶布灯位置
FVector FArmyRoomEntity::GetCeilingDropLampPos()
{
	return CeilingDropLampPos;
}

// 设置吊顶布灯位置
void FArmyRoomEntity::SetCeilingDropLampPos(FVector Pos)
{
	CeilingDropLampPos = Pos;
}

// 获取客厅壁灯位置数组
TArray<FVector> FArmyRoomEntity::GetWallLampPosArr()
{
	return WallLampPosArr;
}
// 设置客厅壁灯位置数组
void FArmyRoomEntity::SetWallLampPosArr(TArray<FVector> PosArr)
{
	WallLampPosArr = PosArr;
}
// 获取客厅壁灯方向
FVector FArmyRoomEntity::GetWallLampDirection()
{
	return WallLampDirection;
}
// 设置客厅壁灯方向
void FArmyRoomEntity::SetWallLampDirection(FVector Direction)
{
	WallLampDirection = Direction;
}

// 获取房间的梁
TArray<TSharedPtr<class FArmyBeam>> FArmyRoomEntity::GetBeamArray()
{
	return BeamArray;
}

// 获取房间的柱子
TArray<TSharedPtr<class FArmyPillar>> FArmyRoomEntity::GetPillarArray()
{
	return PillarArray;
}

// 获取房间的风道
TArray<TSharedPtr<class FArmyAirFlue>> FArmyRoomEntity::GetAirFlueArray()
{
	return AirFlueArray;
}

// 获取房间的包立管
TArray<TSharedPtr<class FArmyPackPipe>> FArmyRoomEntity::GetPackPipeArray()
{
	return PackPipeArray;
}

// 获取房间的点位
TArray<TSharedPtr<class FArmyFurniture>> FArmyRoomEntity::GetComponentArray()
{
	return ComponentArray;
}
////////////////////私有化方法/////////////////////////////////

// 初始化入光口
void FArmyRoomEntity::InitSunnyOpening()
{
	// 如果为客厅或客餐厅则进行入光口处理
	if (GetAiRoomType() != RT_LivingRoom && GetAiRoomType() != RT_LivingDiningRoom)
	{
		return;
	}

	TSharedPtr<FArmyHardware> Hardware = nullptr;
	float MaxLength = 0;

	// 获得有效的窗户
	for (auto &It : WindowArray)
	{
		if (It->GetWidth() > MaxLength)
		{
			MaxLength = It->GetWidth();
			Hardware = It;
		}
	}
	// 获得有效的门
	for (auto &It : DoorArray)
	{
		if (It->GetWidth() > MaxLength)
		{
			// 获得与这个门有关的所有房间
			TArray<TSharedPtr<FArmyRoom>>RoomList = FArmyDataTools::GetRoomsByHardware(It);
			if (RoomList.Num() != 2)
			{
				continue;
			}

			TSharedPtr<FArmyRoom> TmpRoom = RoomList[0];
			if (TmpRoom->GetUniqueID() == CurrentRoom->GetUniqueID())
			{
				TmpRoom = RoomList[1];
			}

			if (FArmyDataTools::GetRoomType(TmpRoom->GetSpaceId()) == RT_Balcony)
			{
				MaxLength = It->GetWidth();
				Hardware = It;
			}
		}
	}

	if (Hardware.IsValid())
	{
		SunnyOpening = Hardware;
	}
	else
	{
		SunnyOpening = nullptr;
	}
}

// 初始化门转窗户
void FArmyRoomEntity::InitDoorWindow()
{
	// 如果为卧室则进行门转窗处理
	if (GetAiRoomType() != RT_Bedroom_Master && GetAiRoomType() != RT_Bedroom_Second && GetAiRoomType() != RT_StudyRoom)
	{
		return;
	}

	TSharedPtr<FArmyHardware> Hardware = nullptr;
	float MaxLength = 0;

	// 获得有效的门
	for (auto &It : DoorArray)
	{
		// 获得与这个门有关的所有房间
		TArray<TSharedPtr<FArmyRoom>>RoomList = FArmyDataTools::GetRoomsByHardware(It);
		if (RoomList.Num() != 2)
		{
			continue;
		}

		TSharedPtr<FArmyRoom> TmpRoom = RoomList[0];
		if (TmpRoom->GetUniqueID() == CurrentRoom->GetUniqueID())
		{
			TmpRoom = RoomList[1];
		}

		if (FArmyDataTools::GetRoomType(TmpRoom->GetSpaceId()) == RT_Balcony)
		{
			DoorWindowArray.Add(It);
		}
	}
}

// 门是否需要转化为窗户
bool FArmyRoomEntity::IsDoorToWindow(TSharedPtr<class FArmyHardware> InDoor)
{
	for (auto It : DoorWindowArray)
	{
		if (It->GetUniqueID() == InDoor->GetUniqueID())
		{
			return true;
		}
	}

	return false;
}

// 布尔运算合并柱子、风道到房间内
void FArmyRoomEntity::CalModifyRoom()
{
	if (PillarArray.Num() == 0)
	{
		ModifyRoom = CurrentRoom;
		return;
	}

	if (CurrentRoom.IsValid())
	{
		Clipper Cli;
		/** 房间的点要为顺时针返回  柱子的点也为顺时针返回 统一便于使用ptfNonZero @see enum PolyFillType */
		TArray<FVector> RoomPoints = CurrentRoom->GetWorldPoints(true);
		Paths ClipperPath;
		Path RoomPath;
		/** 精度控制 */
		const int32 PrecisionCtrlInt = 1000;
		const float PrecisionCtrlFloat = 1000.f;
		for (int i = 0; i < RoomPoints.Num(); i++)
		{
			RoomPath.push_back(IntPoint(RoomPoints[i].X * PrecisionCtrlInt, RoomPoints[i].Y * PrecisionCtrlInt));
		}
		ClipperPath.push_back(RoomPath);
		Cli.AddPaths(ClipperPath, ptSubject, true);

		//把柱子加入布尔运算
		for (auto Pillar: PillarArray)
		{
			// 只考虑柱子属于一个房间的情况
			if (FArmyDataTools::GetObjectRelateRooms(Pillar).Num() != 1)
			{
				continue;
			}

			TArray<FVector> ComponentPoints;
			Pillar->Get4Vertexes(ComponentPoints);
			
			ClipperPath.clear();
			RoomPath.clear();
			for (int i = 0; i < ComponentPoints.Num(); i++)
			{
				RoomPath.push_back(IntPoint(ComponentPoints[i].X * PrecisionCtrlInt, ComponentPoints[i].Y * PrecisionCtrlInt));
			}
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptClip, true);
			
		}

		/** 把风道加入布尔运算 */
		for (auto AirFlue : AirFlueArray)
		{
			TArray<FVector> ComponentPoints;
			AirFlue->Get4Vertexes(ComponentPoints);

			ClipperPath.clear();
			RoomPath.clear();
			for (int i = 0; i < ComponentPoints.Num(); i++)
			{
				RoomPath.push_back(IntPoint(ComponentPoints[i].X * PrecisionCtrlInt, ComponentPoints[i].Y * PrecisionCtrlInt));
			}
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptClip, true);

		}

		/** 构造ModifyRoom */
		Paths ResultPoints;
		if (Cli.Execute(ctUnion, ResultPoints, pftNonZero, pftNonZero))
		{
			FObjectPtr CopyRoom = CurrentRoom->CopySelf();
			ModifyRoom = StaticCastSharedPtr<FArmyRoom>(CopyRoom);
			ModifyRoom->Clear();
			for (int i = 0; i < ResultPoints.size(); i++)
			{
				TSharedPtr<FArmyEditPoint> FirstPoint = nullptr;
				TSharedPtr<FArmyEditPoint> PrePoint = nullptr;
				for (int j = 0; j < ResultPoints[i].size(); j++)
				{
					FVector CurrentPoint(ResultPoints[i][j].X / PrecisionCtrlFloat, ResultPoints[i][j].Y / PrecisionCtrlFloat, 0);
					TSharedPtr<FArmyEditPoint> NewPoint = MakeShareable(new FArmyEditPoint(CurrentPoint));
					if (j > 0)
					{
						TSharedPtr<FArmyWallLine> WallLine = MakeShareable(new FArmyWallLine(PrePoint, NewPoint));
						ModifyRoom->AddLine(WallLine);
						PrePoint->AddReferenceLine(WallLine->GetCoreLine());
						NewPoint->AddReferenceLine(WallLine->GetCoreLine());
					}
					else
					{
						FirstPoint = NewPoint;
					}
					PrePoint = NewPoint;
					if (j == ResultPoints[i].size() - 1)
					{
						TSharedPtr<FArmyWallLine> LastLine = MakeShareable(new FArmyWallLine(NewPoint, FirstPoint));
						NewPoint->AddReferenceLine(LastLine->GetCoreLine());
						FirstPoint->AddReferenceLine(LastLine->GetCoreLine());
						ModifyRoom->AddLine(LastLine);
					}
				}
			}
		}
	}
}

// 初始化硬装相关信息
void FArmyRoomEntity::InitHardDataInfo()
{
	TSharedPtr<FArmyHardModeData> HardModeData = MakeShareable(new FArmyHardModeData(this->AsShared()));
	
	// 设置吊顶下吊高度
	CeilingDropOff = HardModeData->GetCeilingDropOff();

	// 设置顶角线高度
	CrownMouldingHeight = HardModeData->GetCrownMouldingHeight();
}
