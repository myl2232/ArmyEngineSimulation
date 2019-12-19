//#pragma once
//#include "Widgets/SCompoundWidget.h"
//#include "ArmyStyle.h"
//
//class ARMYSLATE_API SArmyTab : public SCompoundWidget
//{
//public:
//
//	SLATE_BEGIN_ARGS(SArmyTab)
//	{}
//	SLATE_END_ARGS()
//
//public:
//	void Construct(const FArguments& InArgs);
//
//	void AddTab(FText InLabel, FString InUniqueID, TSharedPtr<SWidget> InWidget);
//	
//	bool RemoveTab(FString InUniqueID);
//
//	bool InvokeTab(FString InUniqueID);
//
//	TSharedPtr<SWidget> GetTabWidget(FString InUniqueID);
//
//	void SetTabVisible(bool InVisible, FString InUniqueID);
//
//private:
//	TArray<TSharedPtr<SWidget>> Widgets;
//
//	TSharedPtr<class SHorizontalBox> TabWellContainer;
//	TSharedPtr<class SWidgetSwitcher> TabContentSwitcher;
//
//	//注册弹出窗口为不断更新的Widget
//	TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;
//	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
//};