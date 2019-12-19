/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmySavePlan.h
* @Description 保存方案界面
*
* @Author 欧石楠
* @Date 2018年6月21日
* @Version 1.0
*/

#pragma once

#include "AliyunOss.h"
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SComboBox.h"
#include "Http.h"
#include "ArmyUser.h"

class SArmySavePlan : public SCompoundWidget {

	SLATE_BEGIN_ARGS(SArmySavePlan) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

public:
	FText GetCurrentProject() const { return FText::FromString(CurrentProjectName); }
	FText GetHouseName() const { return FText::FromString(HouseName); }

    /** @欧石楠 截图获取户型缩略图，在切换户型模式或者在户型模式下保存时截取 */
    void TakeHomeScreenshot();

    /** 截图获取方案缩略图*/
    void TakePlanScreentshot();

	bool CheckValid();

public:
	virtual void OnConfirmClicked();

	
	/**
	 * 请求保存方案
	 * @param bTakeHomeScreenshot - bool - 是否在保存的时候立即截取户型缩略图
	 */
	virtual void PreReqSavePlan(bool bTakeHomeScreenshot = false);

protected:
	/**创建方案名称界面*/
	TSharedRef<SWidget> CreatePlanWidget();

	/**创建选择房型界面*/
	TSharedRef<SWidget> CreateHouseTypeWidget();

	/** 初始化获取项目等*/
	void Init();

    void Reset();

    /** @欧石楠 生成各文件名 */
    void GenerateFilenames();

    virtual void ReqSavePlan();
    virtual void ResSavePlan(struct FArmyHttpResponse Response);

    /** 上传回调 */
    void ResUploadHomeThumbnail(TSharedPtr<FAliyunOssResponse> Response);
    void ResUploadPlanThumbnail(TSharedPtr<FAliyunOssResponse> Response);
    void ResUploadHomeFile(TSharedPtr<FAliyunOssResponse> Response);
    void ResUploadPlanFile(TSharedPtr<FAliyunOssResponse> Response);
    void ResUploadFacsimile(TSharedPtr<FAliyunOssResponse> Response);

    /** 查看是否达到提交保存的条件 */
    void CheckToSave();

	/** @欧石楠 计算套内面积*/
	TArray<FVector> CalcInnerArea();
protected:
    /** @欧石楠 户型缩略图文件 */
    FString HomeThumbnailFilename;
    TArray<uint8> HomeThumbnailData;

	/** @欧石楠 方案缩略图文件 */
	FString PlanThumbnailFilename;
	TArray<uint8> PlanThumbnailData;

    /** 户型文件 */
    FString HomeFilename;
    FString HomeFileMD5;
    TArray<uint8> HomeData;

    /** 方案文件 */
    FString PlanFilename;
    FString PlanFileMD5;
    TArray<uint8> PlanData;

    /** 文件上传状态 */
    bool bUploadHomeThumbnailReady = false;
    bool bUploadPlanThumbnailReady = false;
    bool bUploadHomeFileReady = false;
    bool bUploadPlanFileReady = false;
    bool bUploadFacsimileReady = false;

    /** 文件上传所需的oss数据 */
    FString HomeThumbnailUrl;
    FString PlanThumbnailUrl;
    FString HomeFileUrl;
    FString PlanFileUrl;
    FString FacsimileUrl;

	FString ViewImageUrl;

    /** 请求 */
    TSharedPtr<IHttpRequest> SubmitSaveRequest;

	FString CurrentProjectName;
	FString PlanName;
	
	FString HouseName;
	
	TSharedPtr<class SEditableTextBox> ETB_PlanName;
	TSharedPtr<class SArmyEditableNumberBox> ETB_Bedroom;
	TSharedPtr<class SArmyEditableNumberBox> ETB_LivingRoom;
	TSharedPtr<class SArmyEditableNumberBox> ETB_Kitchen;
	TSharedPtr<class SArmyEditableNumberBox> ETB_Bathroom;
	TSharedPtr<class SImage> Img_Thumbnail;
	TSharedPtr<FSlateDynamicImageBrush> ThumbnailBrush;

	/** 错误信息文本 */
	TSharedPtr<class STextBlock> TB_ErrorMessage;
};