#pragma once
/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRAutoDesignPoint.h
 * @Description 自动布点
 *
 * @Author 
 * @Date 2019年1月11日
 * @Version 1.0
 */
//#include "ArmyAutoDesignDataManager.h"

#include "ArmyAutoDesignSettings.h"
#include "ArmyTypes.h"

#include "ArmyModelEntity.h"
#include "ArmyRoomEntity.h"

struct DoorState
{
	DoorState(TSharedPtr<class FArmyHardware> InDoorInfo, int32 InDoorPriority, bool InUse) : DoorInfo(InDoorInfo), bUsed(InUse), DoorPriority(InDoorPriority)
	{}

	TSharedPtr<FArmyHardware> DoorInfo; //门对应的地址
	bool bUsed = false; //门是否被布置过开关，如果布置过了，就不再布置了
	int32 DoorPriority = 0; //门布置时，与被布置的门在同一房间下的优先级计数-1；目的是将优先级提前

};
class FArmyAutoDesignPoint : public TSharedFromThis<FArmyAutoDesignPoint>
{
public:
	FArmyAutoDesignPoint() {};
	~FArmyAutoDesignPoint() {}

	//单例方法
	static TSharedPtr<FArmyAutoDesignPoint> GetInstance()
	{
		static TSharedPtr<FArmyAutoDesignPoint> Instance = MakeShareable(new FArmyAutoDesignPoint());
		return Instance;
	}
	/**
	 * 自动布点执行函数
	 * @return bool - 
	 */
	bool AutoDesign();
	/**
	 * 清理自动布点相关数据
	 * @return bool - 
	 */
	bool Empty();

private:
	
	/**
	 * 配置界面确定取消响应函数
	 * @param inN - int32 - 1表示确定，0表示取消
	 * @return void - 
	 */
	void OnClick( int32 inN);

	/**
	 * 数据下载状态
	 * @param bSuccess - bool - true 表示下载成功，否则下载失败
	 * @return void - 
	 */
	void DataDownloadState(bool bSuccess);

	/**
	* 检测全局开关、插座、水位与柱子、包立管、风道的碰撞，如果碰撞了则移除
	* @return bool -
	*/
	bool CheckWallCornerCollision();

	/**
	* 检测单个开关、插座、水位与柱子、包立管、风道的碰撞，如果碰撞了则移除
	* @return bool -
	*/
	bool CheckWallCornerCollision(AActor* ActorItr);

	/**
	 * 执行自动布点
	 * @return bool - 
	 */
	bool ExecuteAutoPointDesign();

	/**
	 * 执行开关自动布点
	 * @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 传入要布置的房间
	 * @return bool - true表示布点成功，否则失败
	 */
	bool RunSwitchAutoDesign();

	/**
	 * 执行插座自动布点
	 * @return bool - true表示布点成功，否则失败
	 */
	bool RunSocketAutoDesign();

	/**
	 * 执行水位自动布点
	 * @param InRoomTable - TSharedPtr<class class FArmyRoomEntity> - 传入要布置的房间
	 * @return bool - true表示布点成功，否则失败
	 */
	bool RunWaterPosAutoDesign();

	/** 布点进度 */
	FText GetAutoDesignProgressText() const { return FORMAT_TEXT("%s布点进度%d%%...", *CurrentAutoDesignStr, FMath::FloorToInt(0)); }

	/**
	 * 在门口放置开关
	 * @param InDoor - const TSharedPtr<FArmyHardware> & - 传入要放置开关的门
	 * @param InRoomTable - const TSharedPtr<class FArmyRoomEntity> & - 传入当前房间的roomTable
	 * @param InModelID - EAutoDesignModelID - 传入开关类型
	 * @param isLinkLights - bool - 是否关联灯
	 * @return bool - true表示放置成功，否则失败
	 */
	bool PlaceSwitchInDoor(const TSharedPtr<FArmyHardware> & InDoor, const TSharedPtr<FArmyRoomEntity> & InRoomTable, EAutoDesignModelID InModelID, bool isLinkLights = true);

	/**
	* 在门口外侧放置开关
	* @param InDoor - const TSharedPtr<FArmyHardware> & - 传入要放置开关的门
	* @param InRoomTable - const TSharedPtr<class FArmyRoomEntity> & - 传入当前房间的roomTable
	* @param InModelID - EAutoDesignModelID - 传入开关类型
	* @param isLinkLights - bool - 是否关联灯
	* @return bool - true表示放置成功，否则失败
	*/
	bool PlaceSwitchOutDoor(const TSharedPtr<class FArmyHardware> & InDoor, const TSharedPtr<class FArmyRoomEntity> & InRoomTable, EAutoDesignModelID InModelID, bool isLinkLights = true);

	/**
	 * 在床侧放置开关
	 * @param InDoor - const TSharedPtr<FArmyHardware> & - 开关放置所参考的门，距离门最远侧
	 * @param InRoomTable - const TSharedPtr<class FArmyRoomEntity> & - 放置的房间
	 * @param InModelID - EAutoDesignModelID - 开关类型
	 * @param isLinkLights - bool - 是否关联灯
	 * @return bool - true表示放置成功，否则失败
	 */
	bool PlaceSwitchInBed(const TSharedPtr<FArmyHardware> & InDoor, const TSharedPtr<FArmyRoomEntity> & InRoomTable, EAutoDesignModelID InModelID, bool isLinkLights = true);

	/**
	* 布置开关灯控
	* @param SwitchActor - const TSharedPtr<FArmyHardware> & - 开关Actor
	* @param InModelID - EAutoDesignModelID - 开关类型
	* @param InRoom - const TSharedPtr<class FArmyRoomEntity> & - 放置的房间
	* @return bool - true表示布置成功，否则失败
	*/
	bool PlaceSwitchConnect(TSharedPtr<FArmyFurniture> SwitchFurniture, TSharedPtr<FArmyRoomEntity> InRoom);
	bool PlaceSwitchConnectCouplet(AActor* SwitchActor, EAutoDesignModelID InModelID, TSharedPtr<FArmyRoomEntity> InRoom);
	TArray<TArray<TSharedPtr<FArmyModelEntity>>> GetLightCategoryArrInRoom(TSharedPtr<FArmyRoomEntity> InRoom);

	/**
	* 根据灯的种类和是否存在床获取相应的开关类型
	* @param InLightNum - int32 - 等的种类
	* @param InIsExistBed - bool - 是否存在床
	* @return EAutoDesignModelID - 返回模型ID
	*/
	EAutoDesignModelID GetModelTypeByLightAndBedNum(int32 InLightNum, bool InIsExistBed);

	////根据灯的数量生成对应的几联单控的开关
	//AActor* CreateAutoSwitchByLightNum(int LightNum);

	//根据浴霸的种类生成对应的开关
	AActor* CreateAutoSwitchByBathHeaterType(TSharedPtr<class FArmyRoomEntity>  InRoomTable);

	///**
	// * 计算模型是否去房间内其他模型相交
	// * @param InActor - const AActor * - 
	// * @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 
	// * @return bool - true表示与其他模型碰撞，否则不碰撞
	// */
	//bool CalModelIntersectWithOtherModel(AActor * InActor, TSharedPtr<class FArmyRoomEntity> InRoomTable, const TSet<AActor*> & IgnoreActorArr);
	//
// 	/**
// 	* 获取房间内的门，并且根据门的优先级进行排序
// 	* @param InRoomAllDoorInfo - 存储房间内门的信息
// 	* @param InRoomTable - TSharedPtr<class FArmyRoomEntity> -
// 	*
// 	*/
// 	void GetRoomAndSortSetPriority(TMultiMap<int32, TSharedPtr<FArmyHardware>> InRoomAllDoorInfo, TSharedPtr<class FArmyRoomEntity> InRoomTable);
// 	
	/**
	 * 在房间中放置开关
	 * @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 房间信息
	 * @return bool - true表示放置成功，否则失败
	 */
	bool PlaceSwitch(TSharedPtr<class FArmyRoomEntity> InRoomTable);
	bool PlaceSwitchToilet(TSharedPtr<FArmyRoomEntity> InRoomTable);
	
	bool IsExistLight(TSharedPtr<class FArmyRoomEntity> InRoomTable);


	/**
	* 得到灯的种类
	* @return int32 - 返回种类
	*/
	int32 GetLightTypeNum(TSharedPtr<class FArmyRoomEntity> InRoomTable);

	/**
	* 是否存在床
	* @return bool - true表示存在床，否则不存在
	*/
	bool IsExistBed(TSharedPtr<class FArmyRoomEntity> InRoomTable);
	
		/**
	* 在房间外侧放置开关（卫生间、阳台用）
	* @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 房间信息
	* @return bool - true表示放置成功，否则失败
	*/
	bool PlaceSwitchOutWall(TSharedPtr<class FArmyRoomEntity> InRoomTable);

	/**
	* 在房间中放置浴霸开关
	* @param InRoomTable - TSharedPtr<class FArmyRoomEntity> - 房间信息
	* @return bool - true表示放置成功，否则失败
	*/
	bool PlaceBathHeaterSwitch(TSharedPtr<class FArmyHardware> InDoor, TSharedPtr<class FArmyRoomEntity> InRoomTable);

	/**
	* 获取数组中第一个门，也就是方法中算出的优先级最高的门
	* @param TSharedPtr<FArmyRoomEntity> RoomIt  房间信息的指针
	*/
	TSharedPtr<class FArmyHardware> GetFirstPriorityDoorInRoom(TSharedPtr<class FArmyRoomEntity> RoomIt);

	void SetDoorUsedAndOtherPrority(TSharedPtr<class FArmyHardware> FirstDoor);

	/**
	 * 床侧放置插座
	 * @param RoomEntity - TSharedPtr<class FArmyRoomEntity> - 床所在的房间
	 * @return void - 
	 */
	void PalceSocketInBed(TSharedPtr<class FArmyRoomEntity> RoomEntity);

	/**
	* 洗衣机放置水位或者插座
	* @return void -
	*/
	void PlaceWaterPointOrSocketInWashing(EAutoDesignModelID ModelID);

	/** 自动布置普通水位*/
	void PlaceNormalWaterPoint(EAIComponentCode AICompnentCode, float AIDisFloor, float AILorR, EAutoDesignModelID AutoDesignId, TSharedPtr<class FArmyRoomEntity> RoomIt);
	/** 自动布置冷热水位*/
	void PlaceColdHotWaterPoint(EAIComponentCode AICompnentCode, float AIDisFloor, float AILorR, EAutoDesignModelID ColdWaterPointID, EAutoDesignModelID HotWaterPointID, TSharedPtr<class FArmyRoomEntity> RoomIt);
	
	/** 自动布置水位的地漏和下水*/
	void PlaceFloorDrain(EAIComponentCode AICompnentCode, float AIDisWall, float AILorR, EAutoDesignModelID FloorDrainID, EAutoDesignModelID FloorDrainID2, TSharedPtr<class FArmyRoomEntity> RoomIt);

	/*************布点失败提示信息相关*************/
	///**
	// * 计算模型是否去房间内其他模型相交
	// * @param EPointType - EAutoDesignModelID - 布置点位类型
	// * @param RoomName - FString - 所布置空间的名称
	// * @param ModelName - FString - 模型名称
	// * @return bool - true表示与其他模型碰撞，否则不碰撞
	// */
	void AddFailInfoDownload(TSharedPtr<FArmyRoomEntity> Room, int32 RelyModelType, EAutoDesignModelID PointType);
	void AddFailInfoDesign(TSharedPtr<FArmyRoomEntity> Room, int32 RelyModelType, EAutoDesignModelID PointType);

	TArray<FString> FailInfoArr;
	int32 FailInfoType = 0;

	//对话框显示布置失败信息
	void ShowDialogFailInfo();
	/**********************************************/

	/** 设置插座的标记字段，以便进行自动布线(主要设置冰箱和空调插座，其他插座位普通默认插座)*/
	void SetSocketPipeLineFlag(AActor* InActor, ESocketFlag InFlag = ESF_Normal);

private:
	//自动布点配置界面
	TSharedPtr<class SArmyAutoDesignPoint> AutoDesignWid;

	TSharedPtr<class FArmyAutoDesignDataManager> AutoDesignPointManager;

	TArray<TSharedPtr<class FArmyRoomEntity> > RoomTableArr;

	float AutoDesignProgress = 0.f;

	FString CurrentAutoDesignStr;

	//根据房间类型布置开关
	void ArrangeSwitchByRoomType(TSharedPtr<class FArmyRoomEntity> RoomIt);

	//单个房间中门的信息和优先级
	TMap<TSharedPtr<class FArmyHardware>, int32> InHouseAllDoorInfo;
	////正在布置的模式
	//EAutoDesignMode CurrentAutoDesign = DM_None;

	//整套房子所有的门，依据每个房间门的数量
	TMultiMap<int32, TSharedPtr<class FArmyRoomEntity>> RoomTableDoorInfo;

	

// 	//整个屋内门的信息Center
// 	TMap<TSharedPtr<FArmyHardware>, int32> SingeDoorInfo;
// 
// 	//房间内各类型的门
// 	TSharedPtr<FArmyHardware> SecurityDoor;
// 	TSharedPtr<FArmyHardware> Door;
// 	TSharedPtr<FArmyHardware> SlidingDoor;
// 	TSharedPtr<FArmyHardware> DoorType;
	//整个屋子所有门的状态数组
	TArray<DoorState> DoorStateArray;

	//防盗门只布置一次
	bool bSecurityPlaceSuccess = false;
};