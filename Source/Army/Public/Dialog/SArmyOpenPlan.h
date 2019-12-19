/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyOpenPlan.h
* @Description 打开方案界面
*
* @Author 欧石楠
* @Date 2018年6月27日
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyCommonTypes.h"
#include "SContentItem.h"
#include "ArmyTypes.h"
#include "Http.h"
#include "SComboBox.h"

class SArmyOpenPlan : public SCompoundWidget {

public:
	SLATE_BEGIN_ARGS(SArmyOpenPlan) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

public:
	void OnConfirmClicked();

	void OnGreatPlanConfirmClicked();

	FText GetSelectedProject() const { return FText::FromString(SelectedProjectName); }

private:
	/** 初始化获取项目列表等*/
	void Init();	

	/**向后台数据请求项目列表信息*/
	void ReqGetProjectList();
	void ResGetProjectList(struct FArmyHttpResponse Response);

	/** 请求方案列表 */
	void ReqGetPlanList(const int32& ProjectId);
	void ResGetPlanList(struct FArmyHttpResponse Response);
	void FillPlanList(TArray<FContentItemPtr> ItemList);

	/** 项目交互 */	
	void OnPlanSelected(int32 PlanListIndex);

	/** 请求筛选的项目列表*/
	void ReqGetSearchProjectList();
	void ResGetSearchProjectList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/** 点击搜索*/
	FReply OnSearchClicked();
	void OnSearchTextCommited(const FText& _NewText, ETextCommit::Type _Committype);

    /** 请求户型数据 */
    void ReqHome(const int32& PlanId);
    void ResHome(struct FArmyHttpResponse Response);

	/** 打开方案 */
	void OpenPlan(int32 PlanListIndex);

	/* 大师方案打开方案*/
	void GreatPlanOpenPlan(int32 PlanListIndex);

    /** 加载方案 */
    void OnPlanReady(bool bWasSucceed, FString FilePath);
    void LoadPlan(bool bWasSucceed);
    FContentItemPtr TempHomeData;
    FContentItemPtr TempPlanData;

	/** @梁晓菲 根据项目ID获取项目详情 */
	void ReqProject();
	void ResProject(struct FArmyHttpResponse Response);
	
private:
	/**创建项目下拉界面*/
	TSharedRef<SWidget> CreateProjectComboWidget();

	/**创建搜索框界面*/
	TSharedRef<SWidget> CreateSearchWidget();

	/**创建方案列表界面*/
	TSharedRef<SWidget> CreatePlanListWidget();

	void OnSlectedProjectChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> OnGenerateComoboWidget(TSharedPtr<FArmyKeyValue> InItem);

private:
	TSharedPtr<SComboBox<TSharedPtr<FArmyKeyValue>>> ProjectComboBox;
	TSharedPtr< class SScrollWrapBox > PlanListWidget;

	/** 搜索相关*/
	TSharedPtr<class SEditableTextBox> ETB_SearchTextBox;

	/** 项目数据列表 */
	FArmyComboBoxArray ProjectList;

	TArray<FContentItemPtr> PlanDataList;
	FContentItemPtr SelectedProjectData;

	TArray< TSharedPtr<SContentItem> > PlanUIList;

	/**选中的项目ID*/
	int32 SelectedProjectID;

	/** 选中的方案的index（非唯一标识ID）*/
	int32 SelectedPlanIndex;	

	/** @梁晓菲 项目下方案ID数组*/
	TArray<int32> PlanIdList;

	FString SelectedProjectName;
};