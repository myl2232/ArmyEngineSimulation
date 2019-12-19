#pragma once
/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File FArmyRoomEntity.h
 * @Description Room管理类
 *
 * @Author 常远
 * @Date 2019年1月17日
 * @Version 1.0
 */

#include "CoreMinimal.h"
#include "ArmyAutoDesignSettings.h"
#include "ArmyRoom.h"


 
//将room和room中的对象关联起来
class ARMYSCENEDATA_API FArmyRoomEntity : public TSharedFromThis<FArmyRoomEntity>
{
public:

	// 构造函数
	FArmyRoomEntity(TSharedPtr<FArmyRoom> InRoom);
	
	FArmyRoomEntity();

	// 获得当前房间
	TSharedPtr<class FArmyRoom> GetRoom();

	// 通过类型获得房间模型  1：软装 2：配饰
	void GetModelsByType(TArray<TSharedPtr<class FArmyModelEntity>> &OutModelEntityArray,int32 InModelType);

	// 获得所有的模型
	void GetAllModels(TArray<TSharedPtr<class FArmyModelEntity>> &OutModelEntityArray);

	// 是否存在指定类型的模型
	bool IsExistObj(EAIComponentCode InCode);

	// 通过构件Id获得模型列表
	//void GetModelArrayByComponentId(EAIComponentCode ComponentCode, TArray<TSharedPtr<class FArmyActorItem>> &OutModelArray);

	// 通过构件Id获得模型列表
	void GetModelEntityArrayByComponentId(EAIComponentCode ComponentCode, TArray<TSharedPtr<class FArmyModelEntity>> &OutModelArray);

	// 获得门的数据
	TArray<TSharedPtr<class FArmyHardware>> GetDoorsActor();

	// 获得窗户的数据
	TArray<TSharedPtr<class FArmyHardware>> GetWindowsActor();

	// 获得区域的数据
	TArray<TSharedPtr<class FArmyRegion>> GetRegionArray();

	// 获得入户门
	TSharedPtr<class FArmySecurityDoor> GetSecurityDoor();
	// 获得地面面片
	TArray<TSharedPtr<class FArmyRoomSpaceArea>> GetFloorSpaceArray();
	// 获得墙面面片
	TArray<TSharedPtr<class FArmyRoomSpaceArea>> GetWallSpaceArray();
	// 获得顶面面片
	TArray<TSharedPtr<class FArmyRoomSpaceArea>> GetCeilingSpaceArray();
	/**
	* 得到Room类型
	* @return ERoomType - 返回room类型
	*/
	ERoomType GetAiRoomType();

	/**
	* 得到门窗数据
	* @param ObjType - EObjectType - 传入需要得到的类型
	* @return TSharedPtr<FArmyHardware>& - 返回相应的hardware，不存在返回nullPtr
	*/
	TArray<TSharedPtr<class FArmyHardware> > GetDoorByType(EObjectType ObjType);
	// 序列化为json数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter,FString RegionNo="",bool IsRegion=false);

	// 是否区域数据
	bool GetIsRegion();
	// 添加模型
	bool AddModel(TSharedPtr<class FArmyModelEntity> InModelEntity);
	// 移除模型
	bool RemoveModel(TSharedPtr<class FArmyModelEntity> InModelEntity);
	// 创建颜值包
	TSharedPtr<class FArmyDesignPackage> CreateDesignPackage();
	// 初始化硬装相关信息
	void InitHardDataInfo();
	// 获得吊顶下吊高度
	float GetCeilingDropOff();
	// 获得顶角线高度
	float GetCrownMouldingHeight();
	// 获取吊顶布灯位置
	FVector GetCeilingDropLampPos();
	// 设置吊顶布灯位置
	void SetCeilingDropLampPos(FVector Pos);
	// 获取客厅壁灯位置数组
	TArray<FVector> GetWallLampPosArr();
	// 设置客厅壁灯位置数组
	void SetWallLampPosArr(TArray<FVector> PosArr);
	// 获取客厅壁灯方向
	FVector GetWallLampDirection();
	// 设置客厅壁灯方向
	void SetWallLampDirection(FVector Direction);
	// 获取房间的梁
	TArray<TSharedPtr<class FArmyBeam>> GetBeamArray();
	// 获取房间的柱子
	TArray<TSharedPtr<class FArmyPillar>> GetPillarArray();
	// 获取房间的风道
	TArray<TSharedPtr<class FArmyAirFlue>> GetAirFlueArray();
	// 获取房间的包立管
	TArray<TSharedPtr<class FArmyPackPipe>> GetPackPipeArray();
	// 获取房间的点位
	TArray<TSharedPtr<class FArmyFurniture>> GetComponentArray();
private:
	// 初始化入光口
	void InitSunnyOpening();
	// 初始化门转窗户
	void InitDoorWindow();
	// 门是否需要转化为窗户
	bool IsDoorToWindow(TSharedPtr<class FArmyHardware> InDoor);

	/** @纪仁泽 布尔运算合并柱子、风道到房间内 */
	void CalModifyRoom();

protected:

	// 门列表
	TArray<TSharedPtr<class FArmyHardware>> DoorArray;  
	// 窗户列表
	TArray<TSharedPtr<class FArmyHardware>> WindowArray; 
	// 门转窗户列表
	TArray<TSharedPtr<class FArmyHardware>> DoorWindowArray;
	// 梁
	TArray<TSharedPtr<class FArmyBeam>> BeamArray;
	// 柱子
	TArray<TSharedPtr<class FArmyPillar>> PillarArray;
	// 风道
	TArray<TSharedPtr<class FArmyAirFlue>> AirFlueArray;
	// 包立管
	TArray<TSharedPtr<class FArmyPackPipe>> PackPipeArray;
	// 点位信息
	TArray<TSharedPtr<class FArmyFurniture>> ComponentArray;
	// 入光口
	TSharedPtr<class FArmyHardware> SunnyOpening;
	// 当前房间
	TSharedPtr<class FArmyRoom> CurrentRoom; 
	// 变换后的房间数据
	TSharedPtr<class FArmyRoom> ModifyRoom;
	// 区域列表
	TArray<TSharedPtr<class FArmyRegion>> RegionArray; 
	// 入户门
	TSharedPtr<class FArmySecurityDoor> SecurityDoor;
	// 地面面片
	TArray<TSharedPtr<FArmyRoomSpaceArea>> FloorSpaceArray;
	// 墙面面片
	TArray<TSharedPtr<FArmyRoomSpaceArea>> WallSpaceArray;
	// 顶面面片
	TArray<TSharedPtr<FArmyRoomSpaceArea>> CeilingSpaceArray;
	// 所有模型的数组
	//TArray<TSharedPtr<class FArmyActorItem>> ModelArray;
	// 所有模型的数组
	TArray<TSharedPtr<class FArmyModelEntity>> ModelEntityArray;
	// 所有模型的map
	TMultiMap<EAIComponentCode, TSharedPtr<class FArmyModelEntity> > AllModelMap;
	// 吊顶下吊高度
	float CeilingDropOff;
	// 吊顶顶角线高度
	float CrownMouldingHeight;
	// 吊顶布置主灯位置
	FVector CeilingDropLampPos;
	// 客厅壁灯位置数组
	TArray<FVector> WallLampPosArr;
	// 客厅壁灯方向（多个壁灯方向相同）
	FVector WallLampDirection;
};