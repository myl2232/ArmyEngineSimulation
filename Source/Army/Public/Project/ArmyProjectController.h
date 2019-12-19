/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRProjectController.h
 * @Description 项目列表视图管理器
 *
 * @Author 欧石楠
 * @Date 2018年6月14日
 * @Version 1.0
 */

#pragma once

#include "ArmyViewController.h"
#include "SArmyProjectList.h"
#include "SArmyPlanList.h"
#include "SArmyGreatPlanDetail.h"
#include "ArmyCommonTypes.h"
#include "SContentItem.h"

#include "IHttpRequest.h"

class FArmyProjectController : public FArmyViewController
{
public:
    FArmyProjectController();

    //~ Begin FArmyViewController Interface
    virtual void Init() override;
    virtual TSharedPtr<SWidget> MakeContentWidget() override;
    virtual void Tick(float DeltaSeconds) override;
//#if SERVER_MODE == DEBUG //@ 大师方案开发正在进行中，暂不上线
	//virtual TSharedPtr<SWidget> MakeCustomTitleBarWidget()override;
//#endif // #if SERVER_MODE == DEBUG //@ 大师方案开发正在进行中，暂不上线

    //~ End FArmyViewController Interface

private:
    /** 请求项目列表 */
    void RequestProjectList();
    void Callback_RequestProjectList(struct FArmyHttpResponse Response);
    void OnProjectListScrollToEnd();
    void FillProjectList(TArray<FContentItemPtr> ItemList, bool ClearProjectList = true,bool bInSelectLast = false);

    /** 请求方案列表 */
    void RequestPlanList(const int32& ProjectId);
    void Callback_RequestPlanList(FArmyHttpResponse Response);
    void FillPlanList(TArray<FContentItemPtr> ItemList);

    /** 请求户型数据 */
    void ReqHome(const int32& PlanId);
    void ResHome(FArmyHttpResponse Response);

    /** 项目交互 */
    FReply OnNewProjectClicked();
    void OnProjectSelected(int32 ProjectId);

    /** 新建项目 */
    void RequestNewProject();
    void Callback_RequestNewProject(FArmyHttpResponse Response);

	FReply OnNewPlanClicked();

    /** 打开方案 */
    void OpenPlan(int32 PlanID);
    void GotoDesignEditor();

    /** 加载方案 */
    void OnPlanReady(bool bWasSucceed, FString FilePath);
    void LoadPlan(bool bWasSucceed);
    FContentItemPtr TempHomeData;
    FContentItemPtr TempPlanData;

	//我的项目
	FReply OnMyProjectClicked();
	void OnMyProjectHovered();
	void OnMyProjectUnHovered();

	//大师方案
	FReply OnGreatPlanClicked();
	void OnGreatPlanHovered();
	void OnGreatPlanUnHovered();
	void FillGreatPlanList(TArray<FContentItemPtr> ItemList, bool ClearProjectList = true, bool bInSelectLast = false);

	//请求大师方案数据
	void RequestGreatPlanList();
	void Callback_RequestGreatPlanList(struct FArmyHttpResponse Response);
	void OnGreatPlanSelected(int32 PlanId);

	//大师方案数据后台暂时无法提供，现构造假数据以供测试
	void CreateTestData(FArmyHttpResponse &TestResponse);

private:
    /** 项目数据列表 */
    TArray<FContentItemPtr> ProjectList;

	/** 方案数据列表 */
	TArray<FContentItemPtr> PlanDataList;

    TArray< TSharedPtr<SContentItem> > ProjectUIList;

    TSharedPtr<SArmyProjectList> ProjectListWidget;

    TSharedPtr<SArmyPlanList> PlanListWidget;

	TSharedPtr<SArmyGreatPlanDetail> GreatPlanDetailWidget;

    TSharedPtr<SArmyNewProject> NewProjectDialog;

	//@ 大师方案列表
	TSharedPtr<class SArmyGreatPlanList> GreatPlanListWidget;
	/** 大师方案数据列表 */
	TArray<FContentItemPtr> GreatPlanList;
	//大师方案瀑布流项
	TArray< TSharedPtr<SContentItem> > GreatPlanUIList;

    /** 瀑布流相关 */
    bool bClearProjectList;
    int32 PageIndex = 1;
    int32 PageSize;
    int32 TotalPageNum = 0;

	/**@欧石楠选中的项目ID*/
	int32 SelectedProjectID;

	//@ 项目内容显示切换
	TSharedPtr<SWidgetSwitcher> ContentSwitcher;

	//@
	TSharedPtr<class SArmyImageTextButton> MyProjectWid;//我的项目
	TSharedPtr<SArmyImageTextButton> GreatPlanWid;//大师方案
	static const FString MyProject;
	static const FString GreatPlan;
};