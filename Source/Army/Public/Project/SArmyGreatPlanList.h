/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyProjectList.h
 * @Description 大师方案列表
 *
 * @Author 
 * @Date 2019年2月13日
 * @Version 1.0
 */

#pragma once

//#include "SScrollWrapBox.h"

#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SComboBox.h"
#include "SScrollWrapBox.h"
#include "SEditableTextBox.h"
#include "ArmyCommonTypes.h"


class SArmyGreatPlanList : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyGreatPlanList) {}

	///** 项目列表瀑布流加载 */
	//SLATE_EVENT(FSimpleDelegate, OnProjectListScrollToEnd)

	///** 新建项目事件 */
	//SLATE_EVENT(FOnClicked, OnNewProjectClicked)

	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

	void AddPlanWidget(TSharedRef<SWidget> Widget);

	///** 清空方案列表 */
	void EmptyProjectList();

	/**
	 * 设置大师方案总数量
	 * @param InPlanCount - int32 - 输入数量
	 * @return void -
	 */
//	void SetPlanCount(int32 InPlanCount) { PlanCount = InPlanCount; }

	//更新页码
	void UpData(int32 InPlanCount);
private:
    ///** 创建新建项目按钮 */
    //TSharedRef<SWidget> MakeNewProjectButton();

    ///** 新建按钮交互 */
    //void OnNewProjectHovered();
    //void OnNewProjectUnhovered();
    TSharedPtr<SBorder> NewProjectBorder;
    FOnClicked OnNewProjectClicked;

	//大师方案总数
	//int32 PlanCount;

	TSharedPtr<class SArmyPageQuery> PageQuery;
private:
    TSharedPtr<SScrollWrapBox> Container;

    TSharedPtr<class SArmyNewProject> NewProjectWidget;

	TSharedPtr<SEditableTextBox> MasterSearchEditableText;

	//输入框内容
	TArray<FString> SearchText;
	FText SearchBoxText;
	//选中的分类种类名字
	//全屋？
	FString SelectedAllHouseName;
	//风格
	FString SelectedStyle;
	//面积
	FString SelectedMeasure;
	//房间数
	FString SelectedHouseNum;
	//选中的种类ID
	int32 SelectedAllHouseID = 0;
	int32 SelectedStyleID = 0;
	int32 SelectedMeasureID = 0;
	int32 SelectedHouseNumID = 0;

	TArray<TSharedPtr<FArmyKeyValue>> AllHouseList;
	TSharedPtr<FArmyKeyValue> CurrentAllHouseValue = MakeShareable(new FArmyKeyValue(-1, TEXT("全屋")));//全屋

	TArray<TSharedPtr<FArmyKeyValue>> StyleList;
	TSharedPtr<FArmyKeyValue> CurrentStyle = MakeShareable(new FArmyKeyValue(-1, TEXT("不限风格")));//不限风格

	TArray<TSharedPtr<FArmyKeyValue>> MeasureList;
	TSharedPtr<FArmyKeyValue> CurrentMeasure = MakeShareable(new FArmyKeyValue(-1, TEXT("不限户型面积")));//不限户型面积

	TArray<TSharedPtr<FArmyKeyValue>> HouseNumList;
	TSharedPtr<FArmyKeyValue> CurrentHouseNum = MakeShareable(new FArmyKeyValue(-1, TEXT("不限居室数")));//不限居室数

	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> ETB_HouseComboBox;
	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> ETB_StyleComboBox;
	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> ETB_MeasureComboBox;
	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> ETB_HouseNumComboBox;

	TSharedRef<SWidget> CreateHouseComboBox();
	TSharedRef<SWidget> CreateStyleComboBox();
	TSharedRef<SWidget> CreateMeasureComboBox();
	TSharedRef<SWidget> CreateHouseNumComboBox();

	TSharedRef<SWidget> GenerateComboItem(TSharedPtr<FArmyKeyValue> InItem, int32 InType);

	FText GetCurrentComboText(int32 InType) const;

	void  CallBack_HouseList(struct FArmyHttpResponse Response);
	void  CallBack_StyleList(struct FArmyHttpResponse Response);
	void  CallBack_MeasureList(struct FArmyHttpResponse Response);
	void  CallBack_HouseNumList(struct FArmyHttpResponse Response);

	void  RequestServerHouseDate();
	void  RequestServerStyleDate();
	void  RequestServerMeasureDate();
	void  RequestServerHouseNumDate();


	void HandleComboItemChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo, int32 InType);

	void ProcessSearch();

	//搜索文字发生变化
	void OnSearchTextCommited(const FText& _NewText, ETextCommit::Type _Committype);
	void OnTextChange(const FText& text);
	FReply OnSearchClicked();
	FText GetSearchTest()const { return SearchBoxText; }

	//请求方案列表
	void ReqGetMasterPlanList(const int32& AllHouseID, const int32& StyleID, const int32& MeasureID, const int32& HouseNumID);
	void ResGetGreatPlanList(struct FArmyHttpResponse Response);
	void FillPlanList(TArray<FContentItemPtr> ItemList);
};