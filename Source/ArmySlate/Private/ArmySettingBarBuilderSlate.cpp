#include "ArmySettingBarBuilderSlate.h"
#include "SBoxPanel.h"
#include "STextBlock.h"


FArmySettingBarBuilder::FArmySettingBarBuilder() : WidgetCount(-1)
{
}

FArmySettingBarBuilder::~FArmySettingBarBuilder()
{
}

TSharedRef<SWidget> FArmySettingBarBuilder::CreateSettingBar()
{
	TSharedPtr<SHorizontalBox> HorizontalBox;
	SAssignNew(HorizontalBox, SHorizontalBox);

	for (auto It : CommandWidgetList) {
		HorizontalBox->AddSlot()
		.Padding(0, 0, 20, 0)
		[
			It.Value.Get()->AsShared()
		];
	}
	return HorizontalBox->AsShared();
}

void FArmySettingBarBuilder::AddItem(int32 CommandIndex, const FText & LocText, const FOnCheckStateChanged &OnStateChanged, ECheckBoxState State)
{
	SAssignNew(CommandWidgetList.FindOrAdd(CommandIndex), SCheckBox)
	.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
	.IsChecked(State)
	.OnCheckStateChanged(OnStateChanged)
	.Padding(FMargin(10, 0, 0, 0))
	[
		SNew(STextBlock)
		.Text(LocText)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
	];
}

void FArmySettingBarBuilder::AddItem(const TSharedRef<SWidget>& Widget)
{
	CommandWidgetList.Add(WidgetCount--, Widget);
}

TSharedRef<SWidget> FArmySettingBarBuilder::GetWidgetByIndex(int32 Index)
{
	return CommandWidgetList.FindOrAdd(Index).Get()->AsShared();
}
