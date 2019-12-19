#include "ArmyToolBarBuilderSlate.h"
#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ArmyStyle.h"

FArmyToolBarBuilder::FArmyToolBarBuilder() : ButtonCount(0), ComboButtonCount(0)
{
	ActionList = MakeShareable(new FUICommandList());
}

FArmyToolBarBuilder::~FArmyToolBarBuilder()
{
}

TSharedRef<SWidget> FArmyToolBarBuilder::CreateToolBar(const ISlateStyle* InStyleSet, const FName& InStyleName, EOrientation Orientation)
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());

	FToolBarBuilder ToolbarBuilder(ActionList, FMultiBoxCustomization::None, MenuExtender, Orientation);
	
	int8 TempCount = 0;
	int8 TempComboIndex = 0;
	// 欧石楠 modified 2018-11-14
	// 1、解决添加两个连续ComboButton时程序崩溃的问题
	// 2、解决最终显示的Button顺序和添加顺序不一致的问题
	TMap<TSharedPtr<FUICommandInfo>, FExecuteAction>::TIterator It(CommandInfoList);
	for (; It; )
	{
		if (ComboButtonCount > 0 && TempCount == ComboButonIndex[TempComboIndex])
        {
            FUIAction Action = FUIAction();
            Action.CanExecuteAction = CanExecuteActionList[TempCount];
			ToolbarBuilder.AddComboButton(Action, ContentList[TempCount], TextList[TempCount], FText::FromString("ToolBarButton"), IconList[TempCount]);
			--ComboButtonCount;
			++TempComboIndex;
			//++TempCount;
		}
		else
		{
			ToolbarBuilder.AddToolBarButton(It.Key());
			++It;
		}
		++TempCount;
	}
	// for (auto It : CommandInfoList)
    // {
    //     if (ComboButtonCount > 0 && TempCount == ComboButonIndex[TempComboIndex])
    //     {
    //         FUIAction Action = FUIAction();
    //         Action.CanExecuteAction = CanExecuteActionList[TempCount];
	// 		ToolbarBuilder.AddComboButton(Action, ContentList[TempCount], TextList[TempCount], FText::FromString("ToolBarButton"), IconList[TempCount]);
	// 		--ComboButtonCount;
	// 		++TempComboIndex;
	// 		//++TempCount;
	// 	}
	// 	ToolbarBuilder.AddToolBarButton(It.Key);
	// 	++TempCount;
	// }
	while(ComboButtonCount > 0)
    {
        FUIAction Action = FUIAction();
        Action.CanExecuteAction = CanExecuteActionList[TempCount];
		ToolbarBuilder.AddComboButton(Action, ContentList[TempCount], TextList[TempCount], FText::FromString("ToolBarButton"), IconList[TempCount]);
		--ComboButtonCount;
		++TempComboIndex;
		++TempCount;
	}

	ToolbarBuilder.SetStyle(InStyleSet, InStyleName);

	ToolBarWidget = ToolbarBuilder.MakeWidget();
	return ToolBarWidget->AsShared();
}

void FArmyToolBarBuilder::AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction &ExecuteAction)
{
	CommandInfoList.Add(CommandInfo, ExecuteAction);
	ActionList->MapAction(CommandInfo, ExecuteAction);
	++ButtonCount;
}

void FArmyToolBarBuilder::AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction & ExecuteAction, const FCanExecuteAction & CanExecuteAction)
{
	CommandInfoList.Add(CommandInfo, ExecuteAction);
	ActionList->MapAction(CommandInfo, ExecuteAction, CanExecuteAction);
	++ButtonCount;
}

void FArmyToolBarBuilder::AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction &ExecuteAction, const FCanExecuteAction &CanExecuteAction, const FGetActionCheckState &GetActionCheckState)
{
	CommandInfoList.Add(CommandInfo, ExecuteAction);
	ActionList->MapAction(CommandInfo, ExecuteAction, CanExecuteAction, GetActionCheckState);
	++ButtonCount;
}

void FArmyToolBarBuilder::AddButton(const TSharedPtr<FUICommandInfo> CommandInfo, const FExecuteAction & ExecuteAction, const FCanExecuteAction & CanExecuteAction, const FIsActionChecked & IsActionChecked)
{
	CommandInfoList.Add(CommandInfo, ExecuteAction);
	ActionList->MapAction(CommandInfo, ExecuteAction, CanExecuteAction, IsActionChecked);
	++ButtonCount;
}

void FArmyToolBarBuilder::AddButton(const TSharedPtr<FUICommandInfo > InUICommandInfo,const FExecuteAction ExecuteAction,const FCanExecuteAction CanExecuteAction,const FIsActionChecked IsCheckedDelegate,FIsActionButtonVisible IsVisibleDelegate,EUIActionRepeatMode RepeatMode)
{
	CommandInfoList.Add(InUICommandInfo,ExecuteAction);
	ActionList->MapAction(InUICommandInfo,ExecuteAction,CanExecuteAction,IsCheckedDelegate,IsVisibleDelegate,RepeatMode);
	++ButtonCount;
}

void FArmyToolBarBuilder::AddComboButton(const FOnGetContent & InMenuContentGenerator, const TAttribute<FText>& InLabelOverride, const TAttribute<FSlateIcon>& InIconOverride)
{
    CanExecuteActionList.Add(ButtonCount, FCanExecuteAction());
	ComboButonIndex.Add(ButtonCount);
	ContentList.Add(ButtonCount, InMenuContentGenerator);
	TextList.Add(ButtonCount, InLabelOverride);
	IconList.Add(ButtonCount, InIconOverride);
	++ButtonCount;
	++ComboButtonCount;
}

void FArmyToolBarBuilder::AddComboButton(const FOnGetContent& InMenuContentGenerator, const FCanExecuteAction &CanExecuteAction, const TAttribute<FText>& InLabelOverride /*= TAttribute<FText>()*/, const TAttribute<FSlateIcon>& InIconOverride /*= TAttribute<FSlateIcon>()*/)
{
    CanExecuteActionList.Add(ButtonCount, CanExecuteAction);
    ComboButonIndex.Add(ButtonCount);
    ContentList.Add(ButtonCount, InMenuContentGenerator);
    TextList.Add(ButtonCount, InLabelOverride);
    IconList.Add(ButtonCount, InIconOverride);
    ++ButtonCount;
    ++ComboButtonCount;
}

void FArmyToolBarBuilder::ClearCommandInfoList()
{
	CommandInfoList.Empty();
}

bool FArmyToolBarBuilder::RemoveCommandInfo(const TSharedPtr<FUICommandInfo> commandInfo)
{
	return ((CommandInfoList.Remove(commandInfo) > 0) ? true : false);
}

TSharedRef<SWidget> FArmyToolBarBuilder::GetToolBarWidget() const
{
	return ToolBarWidget->AsShared();
}

TSharedRef<FUICommandList> FArmyToolBarBuilder::GetCommandList() const
{
	return ActionList->AsShared();
}
