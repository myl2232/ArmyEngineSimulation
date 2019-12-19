#pragma once

#include "SScrollWrapBox.h"
#include "SMultiLineEditableText.h"
#include "Widgets/SCompoundWidget.h"
#include "SArmyOpenPlan.h"

class SArmyGreatPlanDetail : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyGreatPlanDetail) {}

	//缩略图的URL如果传入的不为空，会自动下载图片并填充进去
	//SLATE_ARGUMENT(TArray<FString>, GreatPlanPicURL)

	SLATE_EVENT(FOnClicked, OnNewPlanClicked)
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

public:

    /** 设置项目信息 */
	void SetProjectLeftInfo(const FString& InLeftStr);
	void SetProjectRightInfo(const FString& InRightStr);
    void SetProjectTitle(const FString& InProjectTitle);
	void SetGreatPlanPicURL(TArray<FString> TResURL);
	void SetMaxBrushNum(int32 BrushNum);
    //void SetOwnerName(const FString& InOwnerName);
    //void SetOwnerPhoneNumber(const FString& InOwnerPhoneNumber);
    //void SetMasterDesigner(const FString& InMasterDesigner);
    //void SetRemarks(const FString& InRemarks);

    /** 填充方案列表 */
    void AddPlanItem(TSharedRef<SWidget> PlanItem);

	//生成应用方案和查看全景图按钮
	TSharedRef<SWidget> MakeApplyAndViewButton();

	//切换选择的方案时重置总页数
	void ResetCurrentNumInCount();


private:
	//关闭按钮
    FReply OnClosed();

	//查看前一张缩略图
	FReply GreatPlanBackEvent();
	//查看后一张缩略图
	FReply GreatPlanForwardEvent();

	//应用方案按钮事件
	FReply ApplePlanEvent();
	//查看全景图事件
	FReply ViewPanoramasEvent();

	//获得 当前缩略图的页数/总页数
	FText GetCurrentNumInCount() const { return CurrentNumInCount; }



private:
	//详情里的方案的Text信息
	TSharedPtr<SMultiLineEditableText> TB_ProjectLeftDetail;
    TSharedPtr<SMultiLineEditableText> TB_ProjectRightDetail;

    TSharedPtr<STextBlock> TB_ProjectContractNo;

	//缩略图的Box
	TSharedPtr<SBox> Box_ImageContainer;

	/** 缩略图画刷数组 */
	TArray<TAttribute<const FSlateBrush*>>ThumbnailImageBrush;

	/* 缩略图当前页数*/
	int32 CurrentBrushNum = 0;

	//初始化当前页数/总页数
	FString InitCurrentNumInCount = TEXT("1  / ");
	FString TempCount; 
	//总页数
	FText  CurrentNumInCount; 

	TArray<FString> TGreatPlanPicURL;
	TSharedPtr<SArmyOpenPlan> OpenPlanWidget;

	



};