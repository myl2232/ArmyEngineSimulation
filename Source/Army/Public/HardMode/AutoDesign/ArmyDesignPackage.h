// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File FArmyAutoDesignModel.h
* @Description 自动设计颜值包
*
* @Author yuan.chang
* @Date 2019年01月14日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "ArmyHardModeData.h"


class XR_API FArmyDesignPackage : public TSharedFromThis<FArmyDesignPackage>
{
public:
	FArmyDesignPackage(FString InNo, FString InRoomName,int32 InSpaceId);
	FArmyDesignPackage();
	~FArmyDesignPackage() {};

	int32 GetSpaceId();
	void SetSpaceId(int32 InSpaceId);


	FString GetRoomName();
	void SetRoomName(FString InRoomName);

	FString GetNo();
	void SetNo(FString InNo);

	void SetRoomArea(float InRoomArea);
	float GetRoomArea();
	// 设置软装模型
	void AddLayoutModelList(TSharedPtr<class FArmyModelEntity> InLayoutModel);
	// 设置配饰模型
	void AddOrnamentModelList(TSharedPtr<class FArmyModelEntity> InOrnamentModel);
	// 设置全局模型
	void AddGlobalModelList(TSharedPtr<class FArmyModelEntity> InGlobalModel);
	// 所有的模型列表
	TArray<TSharedPtr<class FArmyModelEntity>> GetAllModelList(bool IsAll = true);
	// 获得需软装模型列表
	TArray<TSharedPtr<class FArmyModelEntity>> GetLayoutModelList();
	// 获得配饰模型列表
	TArray<TSharedPtr<class FArmyModelEntity>> GetOrnamentModelList();
	// 获得全局模型
	TArray<TSharedPtr<class FArmyModelEntity>> GetGlobalModelList();
	// 设置全局模型
	void SetGlobalModelList(TArray<TSharedPtr<class FArmyModelEntity>> InGlobalModelArray);
	// 获得硬装的模型列表
	TArray<TSharedPtr<class FArmyModelEntity>> GetHardModelList(bool IsAll=true);
	// 获得不同布局方式的模型
	TArray<TSharedPtr<class FArmyModelEntity>> GetModelArrayByType(ELayoutType InLayoutType);
	// 获得硬装对象
	TSharedPtr<class FArmyHardModeData> GetHardModeData();
	// 设置硬装对象
	void  SetHardModeData(TSharedPtr<class FArmyHardModeData> InHardModeData);
	// 获得全局硬装对象
	TSharedPtr<class FArmyHardwareData> GetHardwareData();
	// 设置全局硬装对象
	void  SetHardwareData(TSharedPtr<class FArmyHardwareData> InHardwareData);

	FVector GetCeilingLampPos(TSharedPtr<class FArmyRoomEntity> Room);
	TArray<FVector> GetWallLampPos(TSharedPtr<FArmyRoom> InRoom, FVector& OutDirection);

	// 序列化json数据
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	// 反序列化颜值包
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

private:
	// 是否软装模型走配饰规则
	bool CheckLayoutToOrnament(TSharedPtr<class FArmyModelEntity> InModel);
	// 获得模型的布局类型
	ELayoutType GetMoldeLayoutType(TSharedPtr<class FArmyModelEntity> InModel);
private:
	// 颜值包编号
	FString No;
	// 房间编号
	FString RoomName;
	// 空间id
	int32 SpaceId;
	// 房间面积
	float RoomArea;
	// 布局数组
	TArray<TSharedPtr<FArmyModelEntity>> LayoutModelArray;
	// 配饰数组
	TArray<TSharedPtr<FArmyModelEntity>> OrnamentModelArray;
	// 硬装对象
	TSharedPtr<class FArmyHardModeData>  HardModeData;
	// 全局硬装模型
	TArray<TSharedPtr<FArmyModelEntity>> GlobalModelArray;
	// 全局硬装数据
	TSharedPtr<class FArmyHardwareData> HardwareData;
};
