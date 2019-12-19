#pragma once

#include "SScrollWrapBox.h"
#include "SMultiLineEditableText.h"
#include "Widgets/SCompoundWidget.h"

class SArmyPlanList : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyPlanList) {}
	SLATE_EVENT(FOnClicked, OnNewPlanClicked)
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

public:
    void Empty();

    /** 设置项目信息 */
    void SetProjectName(const FString& InProjectName);
    void SetMealID(const FString& InMealID);
    void SetContractID(const FString& InContractID);
    void SetOwnerName(const FString& InOwnerName);
    void SetOwnerPhoneNumber(const FString& InOwnerPhoneNumber);
    void SetLocation(const FString& InLocation);
    void SetVillageName(const FString& InVillageName);
    void SetDetailAddress(const FString& InDetailAddress);

    /** 填充方案列表 */
    void AddPlanItem(TSharedRef<SWidget> PlanItem);

private:
    FReply OnClosed();

private:
    TSharedPtr<STextBlock> TB_ProjectName;
    TSharedPtr<STextBlock> TB_MealID;
    TSharedPtr<STextBlock> TB_ContractID;
    TSharedPtr<STextBlock> TB_OwnerName;
    TSharedPtr<STextBlock> TB_OwnerPhoneNumber;
    TSharedPtr<STextBlock> TB_Location;
    TSharedPtr<STextBlock> TB_VillageName;
    TSharedPtr<SMultiLineEditableText> TB_DetailAddress;

	TSharedPtr<SScrollWrapBox> Container;
	TSharedPtr<SVerticalBox> NoResource;

	/** @梁晓菲 创建新建方案按钮 */
	TSharedRef<SWidget> MakeNewPlanButton();
	/** @梁晓菲 新建按钮交互 */
	void OnNewPlanHovered();
	void OnNewPlanUnhovered();
	TSharedPtr<SBorder> NewPlanBorder;
	FOnClicked OnNewProjectClicked;
};