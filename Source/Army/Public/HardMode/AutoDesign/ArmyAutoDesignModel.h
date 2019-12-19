// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File FArmyAutoDesignModel.h
* @Description 自动设计
*
* @Author yuan.chang
* @Date 2019年01月10日
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Online/ImageDownload/Public/WebImage.h"
#include "Json.h"
#include "Http.h"
#include "ArmyCommonTypes.h"
#include "IArmyHttp.h"

struct AutoDesignInfo
{
	bool DesignLayout;
	bool DesignHard;
	bool DesignAccessory;
	bool IsDefine;
	TArray<FGuid> RoomGuidList;

	AutoDesignInfo() {
		DesignLayout = false; 
		DesignHard = false;
		DesignAccessory = false;
		IsDefine = true;
	}

	AutoDesignInfo(bool InDesignLayout,bool InDesignAccessory,bool InDesignHard,bool InIsDefine) {
		DesignLayout = InDesignLayout;
		DesignHard = InDesignHard;
		DesignAccessory = InDesignAccessory;
		IsDefine = InIsDefine;
	}
};

// 布局方案
struct ModelLayout
{
	TArray<TSharedPtr<class FArmyModelEntity>> ItemLayoutArray; // 模型列表
	ModelLayout() {};
};

// 房间信息
struct RoomItemInfo
{
	FGuid RoomGuid;    // 房间编号
	FString RoomName;  // 房间名称
	RoomItemInfo() {};
	RoomItemInfo(FGuid InRoomGuid,  FString InRoomName) :RoomGuid(InRoomGuid), RoomName(InRoomName) {}
};

// 单房间设计方案
struct SingleDesignModel :public TSharedFromThis<SingleDesignModel>
{
	TArray<TSharedPtr<class FArmyModelEntity>> ItemArray; // 模型列表
	TArray<TSharedPtr<ModelLayout>> LayoutList; // 方案列表
	SingleDesignModel() {};
};

class XR_API FArmyAutoDesignModel : public TSharedFromThis<FArmyAutoDesignModel>
{
public:
	static const TSharedRef<FArmyAutoDesignModel>& Get();
	FArmyAutoDesignModel();
	~FArmyAutoDesignModel() {};

	// 执行自动设计
	bool AutoDesignExecute(TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap, AutoDesignInfo DesignInfo);
	// 读取颜值包
	TMap<int32, TSharedPtr< class FArmyDesignPackage>>  ReadDesignPackage(FString FileName);
	// 根据字符串生成颜值包
	TMap<int32, TSharedPtr< class FArmyDesignPackage>> GetDesignPackageMap(FString ContentStr);
	// 写颜值包
	void WriteDesignPackage(FString FileName);
	// 获取颜值包json字符串，主要用于保存到服务器（大师方案另存为功能）
	bool CreateJsonDesignPackage(FString& DesignPackageJson);
	// 检测硬装
	bool CheckDesignHard(TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap);
	// 检测配饰
	bool CheckDesignAccessory(TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap);
	// 获得可以布局的房间数据
	void GetEffectiveRoomArray(TArray<int32> InRoomIdList, TArray<RoomItemInfo> & EffectiveRooms, TArray<RoomItemInfo> & OutEffectiveRooms);
	
	// 初始化铺贴类施工项
	bool InitPasteConstructionItem(TSharedPtr<class FArmyBaseArea>  InBaseArea, TSharedPtr<class FArmyConstructionItemInterface> InConstructionItemData);
	// 初始化放样类施工项
	bool InitExtrusionConstructionItem(int32 GoodsId, FString UniqueCodeString, TSharedPtr<class FArmyConstructionItemInterface> InConstructionItemData);
	// 初始化软装类施工项
	//bool SetActorConstructionItem(AActor* InActor);

protected:
	// 中台自动设计软装（含区域）
	bool AutoDesignLayout(TSharedPtr<class FArmyRoomEntity> Room, TMap<int32, TSharedPtr< class FArmyDesignPackage>> InDesignPackageMap);
	// bim布置软装
	bool AutoDesignBimLayout(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FArmyDesignPackage> InDesignPackage);
	// 自动布置配饰
	bool AutoDesignAccessory(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<FArmyDesignPackage> InDesignPackage);
	// 自动布置硬装
	bool AutoDesignHard(TSharedPtr<class FArmyRoomEntity> Room, TSharedPtr<class FArmyDesignPackage > DesignPackage);
	// 获得当前户型的房间数据
	TArray<TSharedPtr<class FArmyRoomEntity>> GetCurrentRooms(bool IsEffective=false);
	// 获得当前户型要布局的房间数据
	TArray<TSharedPtr<class FArmyRoomEntity>> GetCurrentLayoutRooms(AutoDesignInfo InDesignInfo);
	
private:
	// 自动布局单个空间的软装-软装布局
	bool AutoDesignSingleLayout(TSharedPtr<class FArmyRoomEntity> InRoom, TSharedPtr<class FArmyDesignPackage> DesignPackage, FString InRegionNo = "", bool InIsRegion = false);
	// 获得户型json数据-软装布局
	FString GetSingleHouse(TSharedPtr<class FArmyRoomEntity> RoomEntity,FString InRegionNo = "", bool InIsRegion = false);
	// 获得设计数据-软装布局
	FString GetSingleDesign(TSharedPtr<SingleDesignModel> InSingleDesign);
	// 从中台转BIM反序列化-软装布局
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);
	// 中台调用成功回调-软装布局
	void OnHttpSuccess(FHttpRequestPtr htppReqPtr, FHttpResponsePtr httpRespondPtr, bool unknown);
	// 组装中台调用参数-软装布局
	FString CombinePostParam(const FString&rSingleHouse, const FString&rSingleDesign);
	// 调用dll库后进行解析布局-软装布局
	bool LayoutSoftModel(FString OutResult,TSharedPtr<class FArmyRoomEntity> InRoomEntity);
	// 添加商品到场景中
	bool AddItemToScene(TSharedPtr<class FArmyModelEntity> InModelEntity);
	/** 长宽高和宽深高转换 @纪仁泽 */
	void LWHTransferWDH(TArray<TSharedPtr<class FArmyModelEntity>>& InModelArray);
	// 获得颜值包数据
	void GetDesignPackageArray(TArray<TSharedPtr<class FArmyDesignPackage>> &OutPackageArray);
	// 工具方法，对房间的样本数据（颜值包）数据进行优先级排序
	void SortAutoDesignModelEntity(TSharedPtr<FArmyRoomEntity>& Room, TArray<TSharedPtr<FArmyModelEntity>>& ModelArray, TArray<TSharedPtr<FArmyModelEntity>>& ModelArr);
	//  请求施工项相关数据
	void ReqConstructionData(int32 GoodsId);
	void ResConstructionData(FArmyHttpResponse Response, int32 GoodsId);

private:
	TSharedPtr<SingleDesignModel> SingleDesign;
	/*TSharedPtr<class FArmyHardwareData> HardwareData;*/
	int32 LivingRoomIndex;

	bool  HardWareIsShow;// zhx : gloal Hard Ware Should Show Only Once,this is Switch
};
