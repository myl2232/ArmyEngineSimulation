#include "ArmyAutoDesignModel.h"
#include "ArmyCommonTypes.h"
#include "ArmySceneData.h"
#include "ArmyRoom.h"
#include "ArmyRegion.h"
#include "ArmyViewportClient.h"
#include "ArmyDataTools.h"
#include "JsonResourceParser.h"
#include "JsonObjectConverter.h"
#include "ArmyAutoDesignHttp.h"
#include "ArmyDesignPackage.h"
#include "ArmyModelEntity.h"
#include "ArmyRoomEntity.h"
#include "ArmyActorItem.h"
#include "ArmyResourceModule.h"
#include "ArmyAutoAccessory.h"
#include "ArmyAutoLayoutPatch.h"
#include "ArmyHardModeExecute.h"
#include "SArmyModelContentBrowser.h"
#include "SArmyHardModeContentBrowser.h"
#include "AutoDesignEngine.h"
#include "ArmyHardModeExecute.h"
#include "ArmyUser.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyHardModeController.h"
#include "ArmyEditorEngine.h"
#include "ArmyConstructionManager.h"

#include "ArmyPlayerController.h"


const TSharedRef<FArmyAutoDesignModel>& FArmyAutoDesignModel::Get()
{
	static const TSharedRef<FArmyAutoDesignModel> Instance = MakeShareable(new FArmyAutoDesignModel);
	return Instance;
}

FArmyAutoDesignModel::FArmyAutoDesignModel()
{
}

/////////////////////////////// 公开方法////////////////////
#pragma region 公开方法

// 执行自动设计
bool FArmyAutoDesignModel::AutoDesignExecute(TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap, AutoDesignInfo DesignInfo)
{
	HardWareIsShow = false;
	// 获得布局的房间数据
	TArray<TSharedPtr<FArmyRoomEntity>> RoomList = GetCurrentLayoutRooms(DesignInfo);

	for (auto RoomEntity : RoomList)
	{
		if (RoomEntity.IsValid())
		{
			// 颜值包中是否有需要布局空间对象的颜值包
			if (!InDesignPackageMap.Contains(RoomEntity->GetAiRoomType()))
			{
				continue;
			}

			// 获得当前房间的颜值包
			TSharedPtr<FArmyDesignPackage> Packgae = *InDesignPackageMap.Find(RoomEntity->GetAiRoomType());
			// 布局软装
			if (DesignInfo.DesignLayout)
			{
				AutoDesignLayout(RoomEntity, InDesignPackageMap);
			}

			// 布局硬装
			if (DesignInfo.DesignHard)
			{
				AutoDesignHard(RoomEntity, Packgae);
			}

			// 布局配饰
			if (DesignInfo.DesignAccessory)
			{
				AutoDesignAccessory(RoomEntity, Packgae);
			}
		}
	}


	GXREditor->SelectNone(true, true, false);
	GVC->CancelDrop();

	// 如果当前是地面模式，需要隐藏顶面的物体，比如：主灯、吊顶等
	if (GXRPC) {
		EXRViewMode ViewMode = GXRPC->GetXRViewMode();
		if (ViewMode == EXRView_TOP)
		{
			GXRPC->SetXRViewMode(EXRView_TOP);
		}
		else if (ViewMode == EXRView_CEILING)
		{
			GXRPC->SetXRViewMode(EXRView_CEILING);
		}
	}
	//结束

	return true;
}

// 读取颜值包
TMap<int32, TSharedPtr< class FArmyDesignPackage>> FArmyAutoDesignModel::ReadDesignPackage(FString FileName)
{
	TMap<int32, TSharedPtr<FArmyDesignPackage>> DesignMap;
	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("DesignPackages") / FileName;
	FString ContentStr = "";
	if (!FFileHelper::LoadFileToString(ContentStr, *FilePath))
	{
		return DesignMap;
	}

	TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(ContentStr), Object);

	const TArray<TSharedPtr<FJsonValue>> Packgaes = Object->GetArrayField(TEXT("packageList"));
	TSharedPtr<class FArmyHardwareData> HardwareDataObj;
	for (auto& It : Packgaes)
	{
		TSharedPtr<FArmyDesignPackage> TmpPackgae = MakeShareable(new FArmyDesignPackage);
		TmpPackgae->Deserialization(It->AsObject());
		DesignMap.Add(TmpPackgae->GetSpaceId(), TmpPackgae);

		// 处理其他空间的窗台石和过门石
		if (TmpPackgae->GetSpaceId() == RT_Other && !HardwareDataObj.IsValid())
		{
			HardwareDataObj = MakeShareable(new FArmyHardwareData(TmpPackgae->GetGlobalModelList()));
			TmpPackgae->SetHardwareData(HardwareDataObj);
		}
	}

	// 初始化所有颜值包中的全局硬装数据
	if (HardwareDataObj.IsValid())
	{
		for (auto &It : DesignMap)
		{
			It.Value->SetHardwareData(HardwareDataObj);
		}
	}
	
	return DesignMap;
}

// 根据字符串生成颜值包
TMap<int32, TSharedPtr< class FArmyDesignPackage>> FArmyAutoDesignModel::GetDesignPackageMap(FString ContentStr)
{
	TMap<int32, TSharedPtr<FArmyDesignPackage>> DesignMap;
	if (ContentStr.Equals(""))
	{
		return DesignMap;
	}

	TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(ContentStr), Object);

	const TArray<TSharedPtr<FJsonValue>> Packgaes = Object->GetArrayField(TEXT("packageList"));
	TSharedPtr<class FArmyHardwareData> HardwareDataObj;
	for (auto& It : Packgaes)
	{
		TSharedPtr<FArmyDesignPackage> TmpPackgae = MakeShareable(new FArmyDesignPackage);
		TmpPackgae->Deserialization(It->AsObject());
		DesignMap.Add(TmpPackgae->GetSpaceId(), TmpPackgae);

		// 处理其他空间的窗台石和过门石
		if (TmpPackgae->GetSpaceId() == RT_Other && !HardwareDataObj.IsValid())
		{
			HardwareDataObj = MakeShareable(new FArmyHardwareData(TmpPackgae->GetGlobalModelList()));
			TmpPackgae->SetHardwareData(HardwareDataObj);
		}
	}

	// 为每个颜值包初始化全局数据
	if (HardwareDataObj.IsValid())
	{
		for (auto It : DesignMap)
		{
			if (It.Value.IsValid() && It.Key != RT_Other)
			{
				It.Value->SetHardwareData(HardwareDataObj);
				It.Value->SetGlobalModelList(HardwareDataObj->HardModeArray);
			}
		}
	}
	return DesignMap;
}
// 写颜值包
void FArmyAutoDesignModel::WriteDesignPackage(FString FileName)
{
	FString PackageJStr;
	CreateJsonDesignPackage(PackageJStr);

	const FString FilePath = FPaths::ProjectSavedDir() / TEXT("DesignPackages") / FileName;
	FFileHelper::SaveStringToFile(PackageJStr, *FilePath);
}

// 获取颜值包json字符串，主要用于保存到服务器（大师方案另存为功能）
bool FArmyAutoDesignModel::CreateJsonDesignPackage(FString& DesignPackageJson)
{
	// 颜值包列表
	TArray<TSharedPtr<class FArmyDesignPackage>> PackageList;
	GetDesignPackageArray(PackageList);

	// 如果颜值中只包含一个其他空间
	if (PackageList.Num() == 1)
	{
		return false;
	}

	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > PackageJsonWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&DesignPackageJson);
	PackageJsonWriter->WriteObjectStart();
	PackageJsonWriter->WriteValue("version", *FArmyUser::Get().GetApplicationVersion());
	PackageJsonWriter->WriteArrayStart(TEXT("packageList"));
	for (auto It : PackageList)
	{
		if (It.IsValid())
		{
			PackageJsonWriter->WriteObjectStart();
			It->SerializeToJson(PackageJsonWriter);
			PackageJsonWriter->WriteObjectEnd();
		}
	}
	PackageJsonWriter->WriteArrayEnd();
	PackageJsonWriter->WriteObjectEnd();
	PackageJsonWriter->Close();

	return true;
}

// 检测硬装 true = 有 | false = 没有
bool FArmyAutoDesignModel::CheckDesignHard(TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
	 
	for (auto room : RoomList)
	{
		/** 获得房间 */
		TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(room.Pin());
		TSharedPtr<FArmyRoomEntity> RoomEntity = MakeShareable(new FArmyRoomEntity(TempRoom));

		if (RoomEntity.IsValid())
		{
			/** 客厅无电视柜 */
			if (RoomEntity->GetAiRoomType() == RT_LivingRoom)
			{
				/** 检测若无此房间id的颜值包 */
				if (InDesignPackageMap.Find(RT_LivingRoom) == nullptr)
				{
					return true;
				}
				/** 获得客厅所有的墙 */
				TArray<TSharedPtr<FModellingWall>> ModellingWallList = InDesignPackageMap[RT_LivingRoom]->GetHardModeData()->GetModellingWallList();
				for (auto& temp_wall : ModellingWallList)
				{
					//0:普通铺贴，1:卧室床头，2:卧室床尾，3:客厅电视柜后面，4:靠近沙发后面
					if (temp_wall->ModellingWallType == 3)
					{
						/** 获得电视柜 */
						TArray<TSharedPtr<class FArmyModelEntity>> OutTVBenchArray;
						RoomEntity->GetModelEntityArrayByComponentId(AI_TVBench, OutTVBenchArray);

						if (OutTVBenchArray.Num() == 0)
						{
							return false;
						}
					}
				}
			}
			/** 主卧无双人床 */
			if (RoomEntity->GetAiRoomType() == RT_Bedroom_Master)
			{
				/** 检测若无此房间id的颜值包 */
				if (InDesignPackageMap.Find(RT_Bedroom_Master) == nullptr)
				{
					return true;
				}
				/** 获得客厅所有的墙 */
				TArray<TSharedPtr<FModellingWall>> ModellingWallList = InDesignPackageMap[RT_Bedroom_Master]->GetHardModeData()->GetModellingWallList();
				for (auto& temp_wall : ModellingWallList)
				{
					//0:普通铺贴，1:卧室床头，2:卧室床尾，3:客厅电视柜后面，4:靠近沙发后面
					if (temp_wall->ModellingWallType == 2)
					{
						/** 获得双人床 */
						TArray<TSharedPtr<class FArmyModelEntity>> OutDoubleBedArray;
						RoomEntity->GetModelEntityArrayByComponentId(AI_DoubleBed, OutDoubleBedArray);

						if (OutDoubleBedArray.Num() == 0)
						{
							return false;
						}
					}
				}
			}
		} 
	} 
	return true;
}
// 检测配饰 true = 有 | false = 没有
bool FArmyAutoDesignModel::CheckDesignAccessory(TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
 	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto room : RoomList)
	{
		/** 获得房间 */
		TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(room.Pin());
		TSharedPtr<FArmyRoomEntity> RoomEntity = MakeShareable(new FArmyRoomEntity(TempRoom));
		if (RoomEntity.IsValid())
		{
			/** 客厅 */
			if (RoomEntity->GetAiRoomType() == RT_LivingRoom)
			{
				/** 检测若无此房间id的颜值包 */
				if (InDesignPackageMap.Find(RT_LivingRoom) == nullptr)
				{
					return true;
				}
				/** 获得颜值包内的配饰 */
				TArray<TSharedPtr<class FArmyModelEntity>> OutAccessoryArray = InDesignPackageMap[RT_LivingRoom]->GetOrnamentModelList();
				if (OutAccessoryArray.Num() != 0)
				{
					/** 电视柜 */
					TArray<TSharedPtr<class FArmyModelEntity>> OutTVBenchArray;
					RoomEntity->GetModelEntityArrayByComponentId(AI_TVBench, OutTVBenchArray);
					/** 茶几 */
					TArray<TSharedPtr<class FArmyModelEntity>> OutTeaTableArray;
					RoomEntity->GetModelEntityArrayByComponentId(AI_TeaTable, OutTeaTableArray);
					/** 三人沙发 */
					TArray<TSharedPtr<class FArmyModelEntity>> OutSofa_MasterArray;
					RoomEntity->GetModelEntityArrayByComponentId(AI_Sofa_Master, OutSofa_MasterArray);

					if (OutTVBenchArray.Num() == 0 || OutTeaTableArray.Num() == 0 || OutSofa_MasterArray.Num() == 0)
					{
						return false;
					}
				}
			}

			/** 餐厅 */
			if (RoomEntity->GetAiRoomType() == RT_DiningRoom)
			{
				/** 检测若无此房间id的颜值包 */
				if (InDesignPackageMap.Find(RT_DiningRoom) == nullptr)
				{
					return true;
				}
				/** 获得颜值包内的配饰 */
				TArray<TSharedPtr<class FArmyModelEntity>> OutAccessoryArray = InDesignPackageMap[RT_DiningRoom]->GetOrnamentModelList();
				if (OutAccessoryArray.Num() != 0)
				{	
					/** 餐桌 */
					TArray<TSharedPtr<class FArmyModelEntity>> OutDiningTableArray;
					RoomEntity->GetModelEntityArrayByComponentId(AI_DiningTable, OutDiningTableArray);
					if (OutDiningTableArray.Num() == 0)
					{
						return false;
					}
				}
			}

			/** 书房 */
			if (RoomEntity->GetAiRoomType() == RT_StudyRoom)
			{
				/** 检测若无此房间id的颜值包 */
				if (InDesignPackageMap.Find(RT_StudyRoom) == nullptr)
				{
					return true;
				}
				/** 获得颜值包内的配饰 */
				TArray<TSharedPtr<class FArmyModelEntity>> OutAccessoryArray = InDesignPackageMap[RT_StudyRoom]->GetOrnamentModelList();
				if (OutAccessoryArray.Num() != 0)
				{
					/** 书桌 */
					TArray<TSharedPtr<class FArmyModelEntity>> OutDeskArray;
					RoomEntity->GetModelEntityArrayByComponentId(AI_Desk, OutDeskArray);
					if (OutDeskArray.Num() == 0)
					{
						return false;
					}
				}
			}

			/** 卧室 */
			if (RoomEntity->GetAiRoomType() == RT_Bedroom_Master || RoomEntity->GetAiRoomType() == RT_Bedroom_Second)
			{
				/** 检测若无此房间id的颜值包 */
				if (InDesignPackageMap.Find(RT_Bedroom_Master) == nullptr && InDesignPackageMap.Find(RT_Bedroom_Second) == nullptr)
				{
					return true;
				}
				/** 获得颜值包内的配饰 */
				TArray<TSharedPtr<class FArmyModelEntity>> OutAccessoryArray;
				if (InDesignPackageMap.Find(RT_Bedroom_Master) != nullptr)
				{
					OutAccessoryArray.Append(InDesignPackageMap[RT_Bedroom_Master]->GetOrnamentModelList());
				}
				if (InDesignPackageMap.Find(RT_Bedroom_Second) != nullptr)
				{
					OutAccessoryArray.Append(InDesignPackageMap[RT_Bedroom_Second]->GetOrnamentModelList());
				}
				//OutAccessoryArray.Append(InDesignPackageMap[RT_Bedroom_Second]->GetOrnamentModelList());
				if (OutAccessoryArray.Num() != 0)
				{
					/** 双人床 */
					TArray<TSharedPtr<class FArmyModelEntity>> OutDoubleBedArray;
					RoomEntity->GetModelEntityArrayByComponentId(AI_DoubleBed, OutDoubleBedArray);
					if (OutDoubleBedArray.Num() == 0)
					{
						return false;
					}
				}
			}

			/** 卫生间 */
			//if (RoomEntity->GetAiRoomType() == RT_Bathroom)
			//{
			//	/** 检测若无此房间id的颜值包 */
			//	if (InDesignPackageMap.Find(RT_Bathroom) == nullptr)
			//	{
			//		return true;
			//	}
			//	/** 获得颜值包内的配饰 */
			//	TArray<TSharedPtr<class FArmyModelEntity>> OutAccessoryArray = InDesignPackageMap[RT_Bathroom]->GetOrnamentModelList();
			//	if (OutAccessoryArray.Num() != 0)
			//	{
			//		/** 马桶 */
			//		TArray<TSharedPtr<class FArmyModelEntity>> OutClosestoolArray;
			//		RoomEntity->GetModelEntityArrayByComponentId(AI_Closestool, OutClosestoolArray);
			//		/** 浴室柜 */
			//		TArray<TSharedPtr<class FArmyModelEntity>> OutBathroomArk_WallArray;
			//		RoomEntity->GetModelEntityArrayByComponentId(AI_BathroomArk_Wall, OutBathroomArk_WallArray);
			//		if (OutClosestoolArray.Num() == 0 || OutBathroomArk_WallArray.Num() == 0)
			//		{
			//			return false;
			//		}
			//	}
			//}
		}
		
	}
	return true;
}

// 获得可以布局的房间数据
void  FArmyAutoDesignModel::GetEffectiveRoomArray(TArray<int32> InRoomIdList, TArray<RoomItemInfo> & EffectiveRooms, TArray<RoomItemInfo> & OutEffectiveRooms)
{
	TArray<TSharedPtr<FArmyRoomEntity>> CurrentRooms = GetCurrentRooms(true);
	for (auto It : CurrentRooms)
	{
		int32 CurrentRoomId = It->GetAiRoomType();
		if (InRoomIdList.Contains(CurrentRoomId))
		{
			RoomItemInfo CurrentRoomItem = RoomItemInfo(It->GetRoom()->GetUniqueID(),It->GetRoom()->GetSpaceName());
			EffectiveRooms.Add(CurrentRoomItem);
		}
		else
		{
			RoomItemInfo CurrentRoomItem = RoomItemInfo(It->GetRoom()->GetUniqueID(), It->GetRoom()->GetSpaceName());
			OutEffectiveRooms.Add(CurrentRoomItem);
		}
	}
}


// 初始化铺贴类施工项
bool FArmyAutoDesignModel::InitPasteConstructionItem(TSharedPtr<class FArmyBaseArea>  InBaseArea, TSharedPtr<class FArmyConstructionItemInterface> InConstructionItemData)
{
	if (!InBaseArea.IsValid())
	{
		return false;
	}

	if (!InBaseArea->GetStyle().IsValid())
	{
		return false;
	}

	// 请求施工项数据
	//ReqConstructionData(GoodsId);
	ConstructionPatameters Parameter;
	
	//这个面有没有下吊
	bool HasHung = false;
	if (InBaseArea->SurfaceType == 2 && InBaseArea->GetExtrusionHeight() != 0)
	{
		HasHung = true;
	}

	Parameter.SetPaveInfo(InBaseArea->GetStyle(), (EWallType)InBaseArea->SurfaceType, HasHung);
	XRConstructionManager::Get()->SaveConstructionData(InBaseArea->GetUniqueID(), Parameter,InConstructionItemData);
	// 请求施工项
	XRConstructionManager::Get()->TryToFindConstructionData(InBaseArea->GetUniqueID(), Parameter,nullptr);
	return true;
}

// 初始化放样类施工项
bool FArmyAutoDesignModel::InitExtrusionConstructionItem(int32 GoodsId, FString UniqueCodeString, TSharedPtr<class FArmyConstructionItemInterface> InConstructionItemData)
{
	// 请求施工项数据
	ReqConstructionData(GoodsId);

	// 关联施工项
	if (InConstructionItemData.IsValid())
	{
		FArmySceneData::Get()->AddExtrusionConstructionData(UniqueCodeString, InConstructionItemData);
	}
	else
	{
		FArmySceneData::Get()->AddExtrusionConstructionData(UniqueCodeString, MakeShareable(new FArmyConstructionItemInterface));
	}
	return true;

}
#pragma endregion 公开方法
/////////////////////////////// 保护型方法////////////////////
#pragma region 保护型方法
// 自动设计软装（含区域）
bool FArmyAutoDesignModel::AutoDesignLayout(TSharedPtr<class FArmyRoomEntity> Room, TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap)
{

	// 删除软装数据
	TArray<TSharedPtr<class FArmyModelEntity>> ModelItemArray;
	Room->GetModelsByType(ModelItemArray, 1);
	for (auto It : ModelItemArray)
	{
		// 从room对象中清空
		Room->RemoveModel(It);
		// 从户型数据删除软装
		//FArmySceneData::Get()->Delete(It);
		// 销毁actor对象
		AActor* NewActor = It->Actor;
		if (NewActor->IsValidLowLevel())
		{
			NewActor->Destroy();
		}
	}

	// 区域布局
	if (Room->GetRegionArray().Num() > 0)
	{
		//区域遍历布局
		for (auto &Region : Room->GetRegionArray())
		{
			int32 RegionType = FArmyDataTools::GetRoomType(Region->GetRegionId());
			// 如果是客厅布局
			if (RegionType == RT_LivingRoom)
			{
				LivingRoomIndex = 1;
			}
			if (InDesignPackageMap.Contains(RegionType))
			{
				TSharedPtr<FArmyDesignPackage> Package = *InDesignPackageMap.Find(RegionType);
				if (!AutoDesignSingleLayout(Room, Package, Region->GetUniqueID().ToString(), true))
				{
					if (LivingRoomIndex == 2)
					{
						AutoDesignSingleLayout(Room, Package, Region->GetUniqueID().ToString(), true);
					}
				}

				// bim布局软装
				AutoDesignBimLayout(Room, Package);
			}

		}
	}
	else
	{
		// 正常布局
		if (InDesignPackageMap.Contains(Room->GetAiRoomType()))
		{
			if (Room->GetAiRoomType() == RT_LivingRoom)
			{
				LivingRoomIndex = 1;
			}
			TSharedPtr<FArmyDesignPackage> Package = *InDesignPackageMap.Find(Room->GetAiRoomType());
			if (!AutoDesignSingleLayout(Room, Package))
			{
				if (LivingRoomIndex == 2)
				{
					AutoDesignSingleLayout(Room, Package);
				}
			} 
			// bim布局软装
			AutoDesignBimLayout(Room,Package);
		}
	}

	return true;
}

void FArmyAutoDesignModel::SortAutoDesignModelEntity(TSharedPtr<FArmyRoomEntity>& Room, TArray<TSharedPtr<FArmyModelEntity>>& ModelArray, TArray<TSharedPtr<FArmyModelEntity>>& ModelArr)
{
	//对颜值包构件进行简单排序：有些构件需要先布置，有些构件需要最后布置等
	// 特殊处理卫生间的数据（排序）：马桶、浴室柜、花洒、淋浴房、洗衣机、百叶帘
	//非卫生间房间，窗帘和地毯最后放置（优先级别最低）
	TArray<EAIComponentCode> FirstCodeArr = { AI_Closestool , AI_BathroomArk , AI_BathroomArk_Wall , AI_ShowerRoom , AI_Sprinkler , AI_WashingMachine , AI_Blind ,AI_SideTable,AI_LampFloorDown };
	TArray<EAIComponentCode> LastCodeArr = { AI_Carpet , AI_CurtainStd };
	TArray<TSharedPtr<FArmyModelEntity>> FirstArr;
	TArray<TSharedPtr<FArmyModelEntity>> LastArr;
	for (auto code : FirstCodeArr)
	{
		for (auto& model : ModelArray)
		{
			int32 CCode = model->ComponentId;
			if (CCode == code)
			{
				FirstArr.Add(model);
				break;
			}
		}
	}
	for (auto code : LastCodeArr)
	{
		for (auto& model : ModelArray)
		{
			int32 CCode = model->ComponentId;
			if (CCode == code)
			{
				LastArr.Add(model);
				break;
			}
		}
	}
	//
	for (auto& model : FirstArr)
	{
		ModelArr.Add(model);
	}
	for (auto& model : ModelArray)
	{
		if (!FirstArr.Contains(model) && !LastArr.Contains(model))
		{
			ModelArr.Add(model);
		}
	}
	for (auto& model : LastArr)
	{
		ModelArr.Add(model);
	}
}

// bim布置软装
bool FArmyAutoDesignModel::AutoDesignBimLayout(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FArmyDesignPackage> InDesignPackage)
{
	TArray<TSharedPtr<class FArmyModelEntity>> ModelArray = InDesignPackage->GetModelArrayByType(ELT_Bim_Layout);

	if (ModelArray.Num() < 1)
	{
		return true;
	}

	TArray<TSharedPtr<class FArmyModelEntity>> ModelArr;
	SortAutoDesignModelEntity(Room, ModelArray, ModelArr);
	TArray<TSharedPtr<FArmyModelEntity>> EntityArray;
	for (auto ModelIt : ModelArr)
	{
		/*if (!ModelIt->FurContentItem.IsValid())
		{
			continue;
		}*/
		EntityArray.Empty();
		TSharedRef<FArmyAutoLayoutPatch> AutoLayoutBim = FArmyLayoutPatchFactory::Get()->GetLayoutPatchRoom(Room);
		AutoLayoutBim->CalcLayoutPatch(ModelIt, Room, EntityArray);//可以进到具体的分发函数
		for (auto &It : EntityArray)
		{
			// 添加actor到场景中
			if (!AddItemToScene(It))
			{
				continue;
			}
			// 把布局后的模型添加到模型数组
			Room->AddModel(It);
		}
	}

	return true;
}

// 自动布置配饰
bool FArmyAutoDesignModel::AutoDesignAccessory(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FArmyDesignPackage> InDesignPackage)
{
	//
	TArray<TSharedPtr<class FArmyModelEntity>> ModelArray = InDesignPackage->GetModelArrayByType(ELT_Bim_Ornament);
	// 初始化硬装数据
	Room->InitHardDataInfo();
	//主灯下吊位置和壁灯位置方向，需要基于硬装设计之后（需要在配饰布置算法里面进行有效性判断：是否等于默认值）
	Room->SetCeilingDropLampPos(InDesignPackage->GetCeilingLampPos(Room));
	FVector WallLampDirection = FVector::ZeroVector;
	Room->SetWallLampPosArr(InDesignPackage->GetWallLampPos(Room->GetRoom(), WallLampDirection));
	Room->SetWallLampDirection(WallLampDirection);

	// 删除软装数据
	TArray<TSharedPtr<FArmyModelEntity>> ActorItemArray;
	Room->GetModelsByType(ActorItemArray, 2);//获取当前房间的所有配饰
	for (auto It : ActorItemArray)
	{
		// 从room对象中清空
		Room->RemoveModel(It);
		// 从户型数据删除配饰
		//FArmySceneData::Get()->Delete(It);
		// 销毁actor对象
		AActor* NewActor = It->Actor;
		if (NewActor->IsValidLowLevel())
		{
			NewActor->Destroy();
		}
	}

	TArray<TSharedPtr<class FArmyModelEntity>> ModelArr;
	SortAutoDesignModelEntity(Room, ModelArray, ModelArr);
	TArray<TSharedPtr<FArmyModelEntity>> EntityArray;
	for (auto ModelIt : ModelArr)
	{
		if (!ModelIt->FurContentItem.IsValid())
		{
			continue;
		}
		EntityArray.Empty();
		TSharedRef<FArmyAutoAccessory> AutoAccessoryObj = FArmyAccessoryFactory::Get()->GetAccessoryRoom(Room);
		AutoAccessoryObj->CalcAccessoryLayout(ModelIt, Room, EntityArray);//可以进到具体的分发函数
		for (auto &It : EntityArray)
		{
			// 添加actor到场景中
			if (!AddItemToScene(It))
			{
				continue;
			}
			// 把布局后的模型添加到模型数组
			Room->AddModel(It);
		}
	}

	return true;
}

// 自动布置硬装
bool FArmyAutoDesignModel::AutoDesignHard(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<class FArmyDesignPackage > DesignPackage)
{
	TSharedPtr<FArmyHardModeExecute> HardModeExecute = MakeShareable(new FArmyHardModeExecute);
	//由于门洞的硬装铺贴要依赖墙面的铺贴样式，所以必须先铺贴硬装 @文海涛
	if (!HardWareIsShow && DesignPackage->GetHardwareData().IsValid())
	{
		HardWareIsShow = true;
		HardModeExecute->ExecHardwareMatching(DesignPackage->GetHardwareData());
	}
	HardModeExecute->ExecHardMatching(Room, DesignPackage->GetHardModeData());
	HardModeExecute->ExecDoors();
	
	return true;
}

// 获得当前户型的房间数据,如果有效的就去除未命名的房间
TArray<TSharedPtr<class FArmyRoomEntity>> FArmyAutoDesignModel::GetCurrentRooms(bool IsEffective)
{
	TArray<TSharedPtr<FArmyRoomEntity>> OutRooms;
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto room : RoomList)
	{
		TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(room.Pin());
		TSharedPtr<FArmyRoomEntity> RoomEntity = MakeShareable(new FArmyRoomEntity(TempRoom));
		if (RoomEntity.IsValid())
		{
			if (IsEffective)
			{
				if (TempRoom->GetSpaceId() != RT_NoName)
				{
					OutRooms.Add(RoomEntity);
				}
			}
			else
			{
				OutRooms.Add(RoomEntity);
			}
			
		}
	}

	return OutRooms;
}

// 获得当前户型要布局的房间数据
TArray<TSharedPtr<class FArmyRoomEntity>> FArmyAutoDesignModel::GetCurrentLayoutRooms(AutoDesignInfo InDesignInfo)
{
	TArray<TSharedPtr<FArmyRoomEntity>> AllRooms = GetCurrentRooms();
	if (InDesignInfo.IsDefine)
	{
		return AllRooms;
	}


	TArray<TSharedPtr<FArmyRoomEntity>> OutRooms;
	for (FGuid &TmpGuid : InDesignInfo.RoomGuidList)
	{
		for (auto It : AllRooms)
		{
			if (TmpGuid == It->GetRoom()->GetUniqueID())
			{
				OutRooms.Add(It);
				break;
			}
		}
	}

	return OutRooms;
}
#pragma endregion 保护型方法


/////////////////////////////// 私有方法////////////////////
#pragma region 私有方法
// 自动布局单个空间的软装
bool FArmyAutoDesignModel::AutoDesignSingleLayout(TSharedPtr<class FArmyRoomEntity> InRoom, TSharedPtr<class FArmyDesignPackage> DesignPackage, FString InRegionNo, bool InIsRegion)
{
	if (!DesignPackage.IsValid())
	{
		return false;
	}

	if (!InRoom.IsValid())
	{
		return false;
	}

	// 初始化设计数据
	SingleDesign = MakeShareable(new SingleDesignModel);
	TArray<TSharedPtr<FArmyModelEntity>> TempLayoutModelArray = DesignPackage->GetModelArrayByType(ELT_Mid_Layout);
	//LWHTransferWDH(TempLayoutModelArray);
	for (auto& Model : TempLayoutModelArray)
	{
		if (Model->FurContentItem.IsValid())
		{
			SingleDesign->ItemArray.Add(Model);
		}
	}

	// 不存在布局模型
	if (SingleDesign->ItemArray.Num() < 1)
	{
		return false;
	}

	// 获得户型数据
	FString StrSingeHouse = GetSingleHouse(InRoom,InRegionNo,InIsRegion);
	// 获得设计数据
	FString StrSingeDesign = GetSingleDesign(SingleDesign);

	if (StrSingeHouse.Equals("") || StrSingeDesign.Equals(""))
	{
		return false;
	}


	TSharedPtr<AutoDesignEngine> DesignEngine = MakeShareable(new AutoDesignEngine);
	FString OutResult = DesignEngine->AIDesign(StrSingeHouse, StrSingeDesign);
	return LayoutSoftModel(OutResult, InRoom);

	/*FString Context = CombinePostParam(StrSingeHouse, StrSingeDesign);
	FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb("POST");
	Request->SetURL(L"http://192.168.1.64:8012/ai_auto_design/");
	Request->SetContentAsString(Context);
	Request->OnProcessRequestComplete().BindSP(this, &FArmyAutoDesignModel::OnHttpSuccess);
	Request->SetHeader(L"Content-Type", L"application/x-www-form-urlencoded;charset=utf-8");
	Request->ProcessRequest();
	return true;*/
}

// 获得户型json数据
FString FArmyAutoDesignModel::GetSingleHouse(TSharedPtr<class FArmyRoomEntity> RoomEntity, FString InRegionNo, bool InIsRegion)
{
	// 户型数据
	FString HomeJStr;
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > HomeJsonWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&HomeJStr);
	HomeJsonWriter->WriteObjectStart();
	HomeJsonWriter->WriteObjectStart("layoutMode");
	RoomEntity->SerializeToJson(HomeJsonWriter, InRegionNo,InIsRegion);
	HomeJsonWriter->WriteObjectEnd();
	HomeJsonWriter->WriteObjectEnd();
	HomeJsonWriter->Close();
	return HomeJStr;
}

// 获得设计数据
FString FArmyAutoDesignModel::GetSingleDesign(TSharedPtr<SingleDesignModel> InSingleDesign)
{
	FString DesignJStr;
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > DesignJsonWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&DesignJStr);
	DesignJsonWriter->WriteObjectStart();
	DesignJsonWriter->WriteObjectStart(TEXT("hardMode"));
	DesignJsonWriter->WriteValue("Version", *FArmyUser::Get().GetApplicationVersion());

	if (SingleDesign.IsValid())
	{
		DesignJsonWriter->WriteArrayStart(TEXT("moveableMeshList"));
		for (auto Item : SingleDesign->ItemArray)
		{
			if (!Item->FurContentItem.IsValid())
			{
				continue;
			}

			if (LivingRoomIndex == 2 && Item->ComponentId == AI_SofaAuxiliary_R)
			{
				continue;
			}
			if (LivingRoomIndex == 2 && Item->ComponentId == AI_SofaAuxiliary_L)
			{
				continue;
			}
			DesignJsonWriter->WriteObjectStart();
			Item->SerializeToJson(DesignJsonWriter,0);
			DesignJsonWriter->WriteObjectEnd();
		}
		DesignJsonWriter->WriteArrayEnd();
	}

	DesignJsonWriter->WriteObjectEnd();
	DesignJsonWriter->WriteObjectEnd();
	DesignJsonWriter->Close();

	return DesignJStr;
}

// 反序列化中台返回的数据
void FArmyAutoDesignModel::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	if (InJsonData.IsValid())
	{
		SingleDesign->LayoutList.Empty();
		/** 读取移动家具信息 list */
		const TSharedPtr<FJsonObject> MoveableDataValue = InJsonData->GetObjectField(TEXT("data"));
		const TArray<TSharedPtr<FJsonValue>> MoveableMeshListData = MoveableDataValue->GetArrayField(TEXT("list"));
		for (auto& It : MoveableMeshListData)
		{
			/** moveableMeshList */
			TSharedPtr<FJsonObject> ItObject = It->AsObject();
			if (ItObject.IsValid())
			{
				const TArray<TSharedPtr<FJsonValue>> TempObjList = ItObject->GetArrayField(TEXT("moveableMeshList"));

				TSharedPtr<ModelLayout> TempModelLayout = MakeShareable(new ModelLayout);
				for (auto& TempObj : TempObjList)
				{
					TSharedPtr<FArmyModelEntity> Model = MakeShareable(new FArmyModelEntity);
					Model->Deserialization(TempObj->AsObject(),0);
					TempModelLayout->ItemLayoutArray.Add(Model);
				}
				SingleDesign->LayoutList.Add(TempModelLayout);
			}
		}
	}
}

// 构造自动设计参数
FString FArmyAutoDesignModel::CombinePostParam(const FString&rSingleHouse, const FString&rSingleDesign)
{
	FString Out;
	//original_design = {} &single_house ={}
	Out += L"original_design=";
	Out += rSingleDesign;
	Out += "&single_house=";
	Out += rSingleHouse;
	return Out;
}

// 调用中台成功后的回调
void FArmyAutoDesignModel::OnHttpSuccess(FHttpRequestPtr htppReqPtr, FHttpResponsePtr httpRespondPtr, bool unknown)
{
	// 调用中台失败
	if (!httpRespondPtr.IsValid()) {
		GGI->Window->ShowMessage(MT_Warning, L"Http Error!\n");
		return;
	}

	FString result = httpRespondPtr->GetContentAsString();
	TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());

	FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(result), Object);
	auto Reader = FJsonStringReader::Create(result);
	Deserialization(Object);

	// 存在布局
	if (this->SingleDesign->LayoutList.Num())
	{
		int index = (FArmyDataTools::BimLayoutIndex + 1) % (this->SingleDesign->LayoutList.Num());
		FArmyDataTools::BimLayoutIndex = index;
		for (auto It : SingleDesign->ItemArray)
		{
			if (!It->FurContentItem.IsValid())
			{
				continue;
			}

			for (auto ComponentLayout : SingleDesign->LayoutList[index]->ItemLayoutArray)
			{
				if (It->ItemID == ComponentLayout->ItemID&& It->ComponentId == ComponentLayout->ComponentId)
				{
					FVector ItScale = FVector(1, 1, 1);
					if (ComponentLayout->IsMaker)
					{
						float OldWidth = It->Width;
						float NewWidth = ComponentLayout->Width;
						float Scale = NewWidth / OldWidth;
						ItScale.X = Scale;
					}

					// 添加actor到场景中
					ComponentLayout->ConstructionItemData = It->ConstructionItemData;
					if (!AddItemToScene(ComponentLayout))
					{
						continue;
					}

					/*TSharedPtr<FArmyModelEntity> ActorItem = AddItemToScene(It->FurContentItem, ComponentLayout->Location, ComponentLayout->Rotation, ComponentLayout->Scale3D);
					if (!ActorItem.IsValid())
					{
						continue;
					}
					ActorItem->Location = ComponentLayout->Location;
					ActorItem->Rotation = ComponentLayout->Rotation;
					ActorItem->Scale3D = ComponentLayout->Scale3D;*/
					//FArmySceneData::Get()->Add(ActorItem, XRArgument().ArgUint32(E_OtherModel));
					// 把布局后的模型添加到模型数组
					//InRoomEntity->AddModel(ActorItem);

					//AActor* NewActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GGI->GetWorld(), It->FurContentItem, ComponentLayout->Location, ComponentLayout->Rotation, ComponentLayout->Scale3D);
					//NewActor->SetActorLabel(It->FurContentItem->Name);
					//NewActor->SetFolderPath(FName(*(SArmyModelContentBrowser::FindObjectGroup(It->FurContentItem->CategryID))));
					////添加是否是商品的标记
					//if (It->FurContentItem->bIsSale)
					//{
					//	NewActor->Tags.Add(FName("IsSale"));
					//}
					//TSharedPtr<FArmyActorItem> ActorItem = MakeShareable(new FArmyActorItem(It->FurContentItem, NewActor));
					//FArmySceneData::Get()->Add(ActorItem, XRArgument().ArgUint32(E_OtherModel));
					//SingleHouse->AddModel(ActorItem);
					// 修正花洒坐标
					/*if (model->ContentItem->ID == 289790)
					{
					auto vectorCorrect = Rotator.RotateVector(FVector(0, -1, 0));
					vectorCorrect *= GVC->GetActorCollisionBox(model->Actor).GetExtent().Y;
					Actor->SetActorLocation(ComponentLayout->Location + vectorCorrect);
					}*/

				}
			}
		}
	}
}

// 调用dll库后进行解析布局
bool FArmyAutoDesignModel::LayoutSoftModel(FString OutResult, TSharedPtr<class FArmyRoomEntity> InRoomEntity)
{
	// 调用中台失败
	if (OutResult.IsEmpty())
	{
		GGI->Window->ShowMessage(MT_Warning, L"布局失败!\n");
		return false;
	}
	TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());
	FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OutResult), Object);
	int Code = Object->GetIntegerField(TEXT("code"));

	// 客厅没有方案，并且是第一次布局
	if(Code == 10009 && InRoomEntity->GetAiRoomType() == RT_LivingRoom)
	{
		if (LivingRoomIndex == 1)
		{
			LivingRoomIndex = 2;
			return false;
		}	
	}

	if (Code == 10009)
	{
		GGI->Window->ShowMessage(MT_Warning, L"没有合适的布局方案!\n");
		return false;
	}

	if (Code != 10000)
	{
		GGI->Window->ShowMessage(MT_Warning, L"布局失败!\n");
		return false;
	}

	Deserialization(Object);

	// 存在布局
	if (this->SingleDesign->LayoutList.Num())
	{
		int index = (FArmyDataTools::BimLayoutIndex + 1) % (this->SingleDesign->LayoutList.Num());
		FArmyDataTools::BimLayoutIndex = index;
		index = 0;
		for (auto It : SingleDesign->ItemArray)
		{
			if (!It->FurContentItem.IsValid())
			{
				continue;
			}

			for (auto ComponentLayout : SingleDesign->LayoutList[index]->ItemLayoutArray)
			{
				if (It->ItemID == ComponentLayout->ItemID && It->ComponentId == ComponentLayout->ComponentId)
				{
					FVector ItScale = FVector(1, 1, 1);
					if (ComponentLayout->IsMaker)
					{
						float OldWidth = It->Width;
						float NewWidth = ComponentLayout->Width;
						float Scale = NewWidth / OldWidth;
						ItScale.X = Scale;
					}

					// 添加actor到场景中
					It->Location = ComponentLayout->Location;
					It->Rotation = ComponentLayout->Rotation;
					It->Scale3D = ComponentLayout->Scale3D;
					//ComponentLayout->ConstructionItemData = It->ConstructionItemData;
					if (!AddItemToScene(It))
					{
						continue;
					}
					// 把布局后的模型添加到模型数组
					InRoomEntity->AddModel(It);
					
					/*TSharedPtr<FArmyModelEntity> ActorItem = AddItemToScene(It->FurContentItem, ComponentLayout->Location, ComponentLayout->Rotation, ComponentLayout->Scale3D);
					if (!ActorItem.IsValid())
					{
						continue;
					}*/

					//SingleHouse->AddModel(ActorItem);
					// 修正花洒坐标
					/*if (model->ContentItem->ID == 289790)
					{
					auto vectorCorrect = Rotator.RotateVector(FVector(0, -1, 0));
					vectorCorrect *= GVC->GetActorCollisionBox(model->Actor).GetExtent().Y;
					Actor->SetActorLocation(ComponentLayout->Location + vectorCorrect);
					}*/
				}
			}
		}
		return true;
	}
	else
	{
		return false;
	}

}

// 添加商品到场景中
bool FArmyAutoDesignModel::AddItemToScene(TSharedPtr<class FArmyModelEntity> InModelEntity)
{
	if (!InModelEntity.IsValid())
	{
		return false;
	}

	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = InModelEntity->FurContentItem;
	if (!ContentItem.IsValid())
	{
		return false;
	}

	FVector NewLoaction = FArmyDataTools::GetNewLocationByAiCode(InModelEntity);
	AActor* NewActor = FArmyResourceModule::Get().GetResourceManager()->CreateActor(GVC->GetWorld(), ContentItem, NewLoaction, InModelEntity->Rotation, InModelEntity->Scale3D);
	if (!NewActor)
		return  false;

	//@欧石楠修改actor label
	NewActor->SetActorLabel(ContentItem->Name);
	NewActor->SetFolderPath(FName(*(SArmyModelContentBrowser::FindObjectGroup(ContentItem->CategryID))));

	//添加是否是商品的标记
	if (ContentItem->bIsSale)
	{
		NewActor->Tags.Add(FName("IsSale"));
	}
	if (ContentItem->ItemFlag& EContentItemFlag::PF_OrignalCenter)
	{
		NewActor->Tags.Add(FName("POINTOBJECT"));
	}
	InModelEntity->Actor = NewActor;

	TSharedPtr<FArmyHardModeController> HardModeController = StaticCastSharedPtr<FArmyHardModeController>(GGI->DesignEditor->CurrentController);
	// 构件操作
	if (ContentItem->GetComponent().IsValid())
	{
		HardModeController->On3DTo2DGanged(ContentItem->GetComponent(), NewActor, EModeIdent::MI_SoftHardMode);
	}

	////请求施工项数据
	//HardModeController->ReqConstructionData(InModelEntity->ItemID);
	//// 关联施工项
	//if (InModelEntity->ConstructionItemData.IsValid())
	//{
	//	FArmySceneData::Get()->AddActorConstructionItemData(NewActor->GetUniqueID(), InModelEntity->ConstructionItemData);
	//}
	//else
	//{
	//	FArmySceneData::Get()->AddActorConstructionItemData(NewActor->GetUniqueID(), MakeShareable(new FArmyConstructionItemInterface));
	//}

	return true;
}

// 获得颜值包数据
void FArmyAutoDesignModel::GetDesignPackageArray(TArray<TSharedPtr<class FArmyDesignPackage>> &OutPackageArray)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_InternalRoom, RoomList);
	TMap<int32, TSharedPtr<FArmyDesignPackage>> PackageMap;
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TSharedPtr<FArmyRoomEntity> RoomEntity = MakeShareable(new FArmyRoomEntity(TempRoom));

		// 未命名的空间不能生成颜值包
		if (RoomEntity->GetAiRoomType() == RT_NoName)
		{
			continue;
		}
		TSharedPtr<FArmyDesignPackage> DesignPackage = RoomEntity->CreateDesignPackage();
		if (DesignPackage.IsValid())
		{
			// 颜值包至少包含一个模型
			if (DesignPackage->GetAllModelList().Num()<1)
			{
				continue;
			}

			if (PackageMap.Contains(RoomEntity->GetAiRoomType()))
			{
				TSharedPtr<FArmyDesignPackage> TmpObj = PackageMap.FindRef(RoomEntity->GetAiRoomType());
				if (TmpObj->GetRoomArea() > DesignPackage->GetRoomArea())
				{
					// 硬装使用面积大的，软装合并
					for (auto ModelIt : DesignPackage->GetLayoutModelList())
					{
						TmpObj->AddLayoutModelList(ModelIt);
					}

					// 硬装使用面积大的，软装合并
					for (auto ModelIt : DesignPackage->GetOrnamentModelList())
					{
						TmpObj->AddOrnamentModelList(ModelIt);
					}

					PackageMap.Add(RoomEntity->GetAiRoomType(), TmpObj);
				}
				else
				{
					// 硬装使用面积大的，软装合并
					for (auto ModelIt : TmpObj->GetLayoutModelList())
					{
						DesignPackage->AddLayoutModelList(ModelIt);
					}

					// 硬装使用面积大的，软装合并
					for (auto ModelIt : TmpObj->GetOrnamentModelList())
					{
						DesignPackage->AddOrnamentModelList(ModelIt);
					}

					PackageMap.Add(RoomEntity->GetAiRoomType(), DesignPackage);
				}
			}
			else
			{
				PackageMap.Add(RoomEntity->GetAiRoomType(), DesignPackage);
			}

		}
	}

	// 其他类型颜值包处理
	if (PackageMap.Contains(RT_Other))
	{
		TSharedPtr<FArmyDesignPackage> TmpObj = PackageMap.FindRef(RT_Other);
		// 添加过门石和窗台石
		TSharedPtr<FArmyHardwareData> HardwareDataObj = MakeShareable(new FArmyHardwareData);
		if (HardwareDataObj.IsValid())
		{
			for (auto It : HardwareDataObj->GetHardModelList())
			{
				TmpObj->AddGlobalModelList(It);
			}
		}
	}
	else
	{
		TSharedPtr<FArmyDesignPackage> ResultPackage = MakeShareable(new FArmyDesignPackage());
		// 设置房间类型
		ResultPackage->SetSpaceId(RT_Other);
		ResultPackage->SetRoomName(TEXT("其他"));
		ResultPackage->SetRoomArea(0);
		// 添加过门石和窗台石
		TSharedPtr<FArmyHardwareData> HardwareDataObj = MakeShareable(new FArmyHardwareData);
		if (HardwareDataObj.IsValid())
		{
			for (auto It : HardwareDataObj->GetHardModelList())
			{
				ResultPackage->AddGlobalModelList(It);
			}
		}
		PackageMap.Add(RT_Other, ResultPackage);
	}

	for (auto It : PackageMap)
	{
		OutPackageArray.Add(It.Value);
	}
}

//  请求施工项相关数据
void FArmyAutoDesignModel::ReqConstructionData(int32 GoodsId)
{
	FString url = FString::Printf(TEXT("/api/quota/goods/%d"), GoodsId);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyAutoDesignModel::ResConstructionData, GoodsId));
	Request->ProcessRequest();
}

void FArmyAutoDesignModel::ResConstructionData(FArmyHttpResponse Response, int32 GoodsId)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		FArmySceneData::Get()->AddItemIDConstructionData(GoodsId, Response.Data);
	}
}
#pragma endregion 私有方法


void FArmyAutoDesignModel::LWHTransferWDH(TArray<TSharedPtr<class FArmyModelEntity>>& InModelArray)
{
	for (auto& InModel : InModelArray)
	{
		Swap(InModel->Length, InModel->Width);
	}
}

