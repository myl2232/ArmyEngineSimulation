/**
* Copyright 2019 北京北京伯睿科技有限公司.
* All Rights Reserved.
*  
*
* @File XRToolBarBuilder.h
* @Description 工具栏按钮创建封装
*
* @Author 欧石楠
* @Date 2018-06-08
* @Version 1.0
*/
#pragma once

#include "ArmyTypes.h"
#include "Widgets/SCompoundWidget.h"

class ARMYSLATE_API FArmyToolBarBuilder {

public:

	FArmyToolBarBuilder();
	~FArmyToolBarBuilder();
public:

	TSharedRef<SWidget> CreateToolBar(const ISlateStyle* InStyleSet, const FName& InStyleName, EOrientation Orientation = Orient_Horizontal);

	void AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction &CxecuteAction);

	void AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction &ExecuteAction, const FCanExecuteAction &CanExecuteAction);

	void AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction &ExecuteAction, const FCanExecuteAction &CanExecuteAction, const FGetActionCheckState &GetActionCheckState);

	void AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction &ExecuteAction, const FCanExecuteAction &CanExecuteAction, const FIsActionChecked &IsActionChecked);

	void AddButton(TSharedPtr<FUICommandInfo > InUICommandInfo, FExecuteAction ExecuteAction, FCanExecuteAction CanExecuteAction, FIsActionChecked IsCheckedDelegate,FIsActionButtonVisible IsVisibleDelegate,EUIActionRepeatMode RepeatMode = EUIActionRepeatMode::RepeatDisabled);

	void AddComboButton(const FOnGetContent& InMenuContentGenerator, const TAttribute<FText>& InLabelOverride = TAttribute<FText>(), const TAttribute<FSlateIcon>& InIconOverride = TAttribute<FSlateIcon>());

    void AddComboButton(const FOnGetContent& InMenuContentGenerator, const FCanExecuteAction &CanExecuteAction, const TAttribute<FText>& InLabelOverride = TAttribute<FText>(), const TAttribute<FSlateIcon>& InIconOverride = TAttribute<FSlateIcon>());

	//void AddCheckBoxButton(const TSharedPtr<FUICommandInfo> CommandInfo,const FExecuteAction &ExecuteAction,const FCanExecuteAction &CanExecuteAction);

	void ClearCommandInfoList();

	bool RemoveCommandInfo(const TSharedPtr<FUICommandInfo> CommandInfo);

	TSharedRef<SWidget> GetToolBarWidget() const;

	TSharedRef<FUICommandList> GetCommandList() const;
private:

	TSharedPtr< FUICommandList > ActionList;

	TMap<TSharedPtr<FUICommandInfo>, FExecuteAction> CommandInfoList;

	TSharedPtr<SWidget> ToolBarWidget;

	TMap<int32, FOnGetContent> ContentList;
	TMap<int32, TAttribute<FText>> TextList;
	TMap<int32, TAttribute<FSlateIcon>> IconList;
    TMap<int32, FCanExecuteAction> CanExecuteActionList;

private:
	int8 ButtonCount;
	int8 ComboButtonCount;
	TArray<int32> ComboButonIndex;
};