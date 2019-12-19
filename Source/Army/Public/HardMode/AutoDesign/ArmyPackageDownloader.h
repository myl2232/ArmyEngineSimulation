// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File FArmyPakageDownloader.h
* @Description 自动设计
*
* @Author 冯小翼
* @Date 2019年01月15日
* @Version 1.0
*/
#pragma once
#include "ArmyGameInstance.h"
#include "CoreMinimal.h"
#include "Tickable.h"
#include "ArmyCommonTools.h"
#include "ArmyResourceModule.h"

struct DownloadInfo
{
	bool IsDownLoad;
	bool DesignLayout;
	bool DesignHard;
	bool DesignAccessory;
};

DECLARE_DELEGATE_OneParam(FDownloadItemsDelegate, DownloadInfo);

class FArmyPakageDownloader : public FTickableGameObject
{
public:
	FArmyPakageDownloader();
	virtual ~FArmyPakageDownloader();
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
 
 
	/** 
	*@ 添加下载列表（ModelEntityList）
	*/
	void AddComponentsList(const TArray<TSharedPtr<class FArmyModelEntity>>  &InModelList);

	/** 
	*@ 添加下载模型Id	 
	*/
	void AddComponent(const  int32   id);

	/* 
	*  执行下载任务，参数为回调函数
	*/
	void DownloadPakage(FDownloadItemsDelegate CompeleDelegate, const DownloadInfo InLoadInfo);


	/**
	*   获取下载包的数量
	*/
	TArray<FContentItemPtr> GetPakageContentItems();

	/**
	*  获取模型列表 ，返回FArmyModelEntityList
	*/
	TArray<TSharedPtr<class FArmyModelEntity>> GetModelList();

	/**
	*  第一次Http请求 完成的回调函数
	*/
	void OnCompeleDownload(FArmyHttpResponse Response);
	 
	/**
	*   隐藏下载进度的动作，执行下载完成后的回调
	*   @ 最终都会执行的回调  1. 包含模型已经缓存在本地等特殊情况
	*/
	void OnHideProgressExecuteCallback(bool param);
private:
	/**
	* @ 更新下载进度
	*/
	void UpdateDownloadProgress();

	/**
	* @下载提示文本
	*/
	FText GetDownloadProgressText() const { return FORMAT_TEXT("Download：%d%%...", FMath::FloorToInt(DownloadProgress)); }
	
	/**
	* @ id List
	*/
	TArray<int32>  ComponentList;
 
	/**
	* @ 请求文件字符串
	*/
	FString		FileStrings;
	/**   
	* @ 回调用户下载完成的委托
	*/
	FDownloadItemsDelegate OnCompeleDelegate;

	/** 
	* @执行用户委托的委托，隐藏下载进度条
	*/
	FBoolDelegate OnHideDelegate;
	
	/** 下载进度 */
	float DownloadProgress = 0.f;
	
	/** 下载文件任务  */
	TSharedPtr<class FDownloadFileSet> DownLoadTasks;
	 
	/** 文件内容列表  */
	TArray<FContentItemPtr> PakageContentItems;

	TArray<FContentItemPtr> PakageArray;
	/** Tick */
	bool bTickable = true;
	// 下载信息
	DownloadInfo LoadInfo;
};