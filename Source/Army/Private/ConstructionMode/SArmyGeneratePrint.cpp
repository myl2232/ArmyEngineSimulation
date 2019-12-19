#include "SArmyGeneratePrint.h"
#include "ArmyStyle.h"
#include "ArmyGameInstance.h"

void SArmyGeneratePrint::Construct(const FArguments& InArgs)
{
	OnGeneratePrintStart = InArgs._OnGeneratePrintStart;

	ChildSlot
		[
			SNew(SVerticalBox)
	
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Fill)
			.Padding(0)
			[
				SNew(SBox)
				.WidthOverride(700)
				.HeightOverride(270)
				[
					SNew(SScrollBox)
					.Orientation(Orient_Vertical)
					.Style(FArmyStyle::Get(), "ScrollBar.Style.Gray")
					.ScrollBarStyle(FArmyStyle::Get(), "ScrollBar.BarStyle.Gray")
					+ SScrollBox::Slot()
					.HAlign(HAlign_Fill)
					//.VAlign(VAlign_Fill)
					.Padding(0)
					[
						SAssignNew(LayerListBox, SGridPanel)
					]
				]
			]

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			//.HAlign(HAlign_Right)
			.AutoHeight()
			.Padding(0)
			[
				SNew(SBox)
				.HeightOverride(1)
				[
					SNew(SImage)
					.Image(FArmyStyle::Get().GetBrush("Splitter.Gray.FF424447"))
				]
			]

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			.AutoHeight()
			.Padding(10, 20, 10, 20)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.Padding(10,0,10,0)
				[
					SNew(SBox)
					.WidthOverride(80)
					.HeightOverride(30)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
						//.ButtonColorAndOpacity(this,&SArmyGeneratePrint::GetOkButtonColor)
						.OnClicked(this, &SArmyGeneratePrint::OnCommand, 1)
						.Content()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("确定")))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.HighlightColor(FLinearColor(FColor(0XFFFFFFFF)))
						]
					]
				]
				+ SHorizontalBox::Slot()
				.Padding(10, 0, 10, 0)
				[
					SNew(SBox)
					.WidthOverride(80)
					.HeightOverride(30)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
						.OnClicked(this, &SArmyGeneratePrint::OnCommand, 0)
						.Content()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("取消")))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
							.HighlightColor(FLinearColor(FColor(0XFFFFFFFF)))
						]
					]
				]
			]
		];
}
void SArmyGeneratePrint::InitContent(const TArray<FName>& InLayerList)
{
	AllLayerList = InLayerList;
	
	//LayerListBox->AddSlot()
	LayerListBox->AddSlot(0, 0)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(30, 30, 0, 10)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SCheckBox)
				.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
				.OnCheckStateChanged(this, &SArmyGeneratePrint::OnCheckStateChanged, FName())
				.IsChecked(this, &SArmyGeneratePrint::IsItemChecked, FName())
			]
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(5,0,0,0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("全部")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
			]
		];

	int32 Count = InLayerList.Num();
	for (int32 i = 0;i < Count; ++i)
	{
		FName V = InLayerList[i];

		int32 row = i / 4 + 1;
		int32 cul =  i % 4;
		LayerListBox->AddSlot(cul, row)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(30,30,20,10)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SCheckBox)
				.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
				.OnCheckStateChanged(this, &SArmyGeneratePrint::OnCheckStateChanged, V)
				.IsChecked(this, &SArmyGeneratePrint::IsItemChecked, V)
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SNew(STextBlock)
				.WrapTextAt(170.0f)
				.AutoWrapText(true)
				.Text(FText::TrimPreceding(FText::FromName(V)))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
			]
		];
	}
}
FReply SArmyGeneratePrint::OnCommand(int32 InV)
{
	if (InV == 1)
	{
		OnGeneratePrintStart.ExecuteIfBound(SelectedLayerList);
	}
	else
	{
		GGI->Window->DismissModalDialog();
	}
	return FReply::Handled();
}
FSlateColor SArmyGeneratePrint::GetOkButtonColor() const
{
	return SelectedLayerList.Num() > 0
		? FLinearColor::White
		: FArmyStyle::Get().GetColor("Color.FFFFC266");
}
void SArmyGeneratePrint::OnCheckStateChanged(ECheckBoxState InCheck, FName InName)
{
	if (InName.IsNone())
	{
		if (InCheck == ECheckBoxState::Checked)
		{
			SelectedLayerList = (AllLayerList);
		}
		else
		{
			SelectedLayerList.Empty();
		}
	}
	else
	{
		if (InCheck == ECheckBoxState::Checked)
		{
			SelectedLayerList.AddUnique(InName);
		}
		else
		{
			SelectedLayerList.Remove(InName);
		}
	}
}
ECheckBoxState SArmyGeneratePrint::IsItemChecked(FName InName) const
{
	if (InName.IsNone())
	{
		return SelectedLayerList.Num() == AllLayerList.Num() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
	}
	return SelectedLayerList.Contains(InName) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}