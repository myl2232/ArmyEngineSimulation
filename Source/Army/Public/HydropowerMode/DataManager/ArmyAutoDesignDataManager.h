/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRAutoDesignDataManager.h
 * @Description 自动布点数据管理器
 *
 * @Author 
 * @Date 2019年1月11日
 * @Version 1.0
 */
#pragma once
#include "ArmyAutoDesignSettings.h"
#include "Tickable.h"
#include "ArmyCommonTypes.h"

class FArmyAutoDesignDataManager : public FTickableGameObject
{
public:
	FArmyAutoDesignDataManager();

	virtual ~FArmyAutoDesignDataManager();

	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	/**
	 * 初始化相关数据
	 * @return void - 
	 */
	void Init();

	/**
	 * 从后台获取开关、插座、点位默认数据
	 * @return bool - true表示请求成功，否则失败
	 */
	bool RequestData();

	//请求自动布点数据
	void ResSyncAutoDesignData(struct FArmyHttpResponse Response, FString InIds);

	/**
	 * 根据类型ID创建Actor
	 * @param TypeID - EAutoDesignModelID - 类型ID
	 * @return AActor * - 返回Actor
	 */
	AActor* CreateActor(EAutoDesignModelID TypeID);

	//获取和设置布点数据
	TArray<FContentItemPtr> GetAutoDesignContentItems() const { return AutoDesignContentItems; }
	//void SetAutoDesignContentItems(TArray<FContentItemPtr> InAutoDesignContentItems) { AutoDesignContentItems = InAutoDesignContentItems; }

	//清空数据
	void Clear();

	//判断是否存在电视背景墙插座
	bool IsExistAutoDesignModel(EAutoDesignModelID AutoDesignModelID);

	//根据默认点位模型获取模型名称
	FString GetModelNameByModelID(EAutoDesignModelID InID);
	/*根据点位依赖模型类型id获取依赖模型类型名称（仅适用于自动布点用到的模型）*/
	FString GetRelyModelNameByModelID(int32 InID);
public:
	FBoolDelegate OnDownLoadFinished;

private:
	//请求布点数据
	bool ReqData();

	/** 更新下载进度 */
	void UpdateDownloadProgress(); 

	/** 布点数据下载进度 */
	FText GetDownloadProgressText() const { return FORMAT_TEXT("下载布点数据中%d%%...", FMath::FloorToInt(DownloadProgress)); }

	//请求自动布点商品列表ID，无商品则获取构件ID
	void ResquestGoodsIDList();
	//回调
	void ResquestGoodsIDListResponse(struct FArmyHttpResponse Response);

	//后台-前端数据类型转化
	EAutoDesignModelID GetModelIdByType(int32 inType);

	//通过构件ID请求获取默认pak白模数据
	void ResquestDXFIDList();
	void ResquestDxfIDListResponse(struct FArmyHttpResponse Response);

	//将商品模型和构件模型合并下载
	void DownloadModelData();

	//取消数据下载回调
	void CancleDataDownloadCallBack();

private:
	//从需要从后台请求的商品数据列表
	TArray<int32> ReqDataFileList;
	TArray<int32> ReqDataDxfFileList;//构件列表
	FString ReqFileListStr;
	float DownloadProgress = 0.f;
	TSharedPtr<class FDownloadFileSet> DownLoadTasks;
	/** 保存自动布点数据 */
	TArray<FContentItemPtr> AutoDesignContentItems;

	TMap<EAutoDesignModelID, int32/*模型ID*/> AutoDesignPointMap;

	TArray<FString> IdsArray;

};