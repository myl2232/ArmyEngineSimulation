// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File FArmyModelEntity.h
* @Description 自动设计颜值包
*
* @Author yuan.chang
* @Date 2019年01月14日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "ArmyCommonTypes.h"

// 模型类型
enum EItemType
{
	IT_None = -1,      // 默认
	IT_HardMode = 0,   // 硬装
	IT_Layout = 1,     // 软装
	IT_Ornament = 2,   // 配饰
	IT_Global = 3,     // 全局
};

// 模型布局类型
enum ELayoutType
{
	ELT_None = -1,          // 默认
	ELT_Bim_HardMode = 0,   // BIM硬装
	ELT_Mid_Layout = 1,     // 中台软装
	ELT_Bim_Layout = 2,     // 中台软装
	ELT_Bim_Ornament = 3,   // 配饰
	ELT_ALL = 9,            // 所有
};

// 模型硬装类型
enum EHardModeType
{
	HMT_None = -1,             // 默认
	HMT_Floor_Material = 1,    // 地面材质
	HMT_Skirting = 2,          // 踢脚线
	HMT_Ceiling_Material = 3,  //顶面材质
	HMT_Ceiling_Line = 4,      // 顶角线
	HMT_Wall_Material = 5,     // 墙面材质
	HMT_Main_Wall = 6,         // 主视觉墙
	HMT_Lamp_Trough = 7,       // 灯槽
	HMT_Window_Stone = 8,      // 窗台石
	HMT_Door_Stone = 9,        // 过门石
	HMT_Door_Buckle = 10,      // 扣条'
	HMT_Pass_Model = 11,      // 垭口包边
};

class XR_API FArmyModelEntity : public TSharedFromThis<FArmyModelEntity>
{
public:
	FArmyModelEntity();
	~FArmyModelEntity() {};

	FArmyModelEntity(int32 InItemID);

	FArmyModelEntity(int32 InComponentId, int32 InItemID, FVector InLocation, FRotator InRotation, FVector InScale3D, int InWidth, int InLength, int InHeight, bool InIsMaker = false);
	
	FArmyModelEntity(TSharedPtr<class FContentItemSpace::FContentItem> InContentItem);

	FArmyModelEntity(TSharedPtr<class FArmyActorItem> InActorItem);

	// 序列化为josn数据 JsonType 0:中台序列化 1:颜值包序列化 9：所有序列化
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter,int32 JsonType=0);
	// 反序列化模型json数据 JsonType 0:中台序列化 1:颜值包序列化 9：所有序列化
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData,int32 JsonType=0);
	//
	void CopyTo(TSharedPtr<FArmyModelEntity>&Other);

	// 设置铺贴类施工项
	bool SetPasteConstructionItem(TSharedPtr<class FArmyBaseArea> InBaseArea);
	// 设置放样类施工项
	bool SetExtrusionConstructionItem(FString UniqueCodeString);
	// 设置软装类施工项
	bool SetActorConstructionItem(AActor* InActor);
public:
	// 模型编号
	FString ItemNo;
	// 模型编号
	int32 ItemID;
	// 构件编号
	int32 ComponentId;
	// 位置
	FVector Location;
	// 旋转
	FRotator Rotation;
	// 缩放
	FVector Scale3D;
	// 宽度
	int Width;
	// 长度
	int Length;
	// 高度
	int Height;
	// 是否定制
	bool IsMaker;
	// 模型类型
	EItemType ItemType;
	// 模型布局
	ELayoutType LayoutType;
	// 硬装类型
	EHardModeType HardModeType;
	// 模型信息
	TSharedPtr<class FContentItemSpace::FContentItem> FurContentItem;
	// 模型actor
	AActor* Actor;
	// 是否默认模型
	bool IsDefault;
	// 施工项数据
	TSharedPtr<class FArmyConstructionItemInterface> ConstructionItemData;
	// 是否关联硬装
	bool IsInEditArea;

};
