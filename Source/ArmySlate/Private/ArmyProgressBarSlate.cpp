#include "ArmyProgressBarSlate.h"
#include "ArmyStyle.h"
#include "Widgets/Notifications/SProgressBar.h"
//SArmyProgressBar

void SArmyProgressBar::Construct(const FArguments& InArgs)
{
	OnCancelDelegate = InArgs._OnCanceled;

	ChildSlot
		[
			SNew(SOverlay)
			+SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(420)
				.HeightOverride(170)
				[
					SNew(SBorder)
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					.Padding(0)
					.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.AutoHeight()
						.Padding(40,40,40,10)
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Bottom)
							.Padding(0)
							[
								SNew(STextBlock)
								.Text(this,&SArmyProgressBar::GetCurrentLabel)
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
								.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
							]
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Bottom)
							.Padding(0)
							[
								SNew(STextBlock)
								.Text(this, &SArmyProgressBar::GetCurrentValue)
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
								.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
							]
						]
						+SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Top)
						.Padding(40,0,40,0)
						[
							SNew(SProgressBar)
							.FillColorAndOpacity(FLinearColor::White)
							.Visibility(EVisibility::Visible)
							.Percent(this, &SArmyProgressBar::GetProgressBarPercent)
						]
						+ SVerticalBox::Slot()
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Bottom)
						.Padding(0,0,20,20)
						[
							SNew(SBox)
							.WidthOverride(80)
							.HeightOverride(30)
							[
								SNew(SButton)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
								.OnClicked(this, &SArmyProgressBar::OnCancel)
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
				]
			]
		];
}
void SArmyProgressBar::SetCurrentLabel(const FText& InLabel)
{
	CurrentLabel = InLabel;
}
void SArmyProgressBar::SetCurrentText(const FText& InText)
{
	CurrentText = InText;
}
void SArmyProgressBar::SetCurrentPercent(float InPercent)
{
	CurrentPercent = InPercent;
}
FText SArmyProgressBar::GetCurrentLabel() const
{
	return CurrentLabel;
}
FText SArmyProgressBar::GetCurrentValue() const
{
	return CurrentText;
}
TOptional<float> SArmyProgressBar::GetProgressBarPercent() const
{
	return CurrentPercent;
}
FReply SArmyProgressBar::OnCancel()
{
	OnCancelDelegate.ExecuteIfBound();
	return FReply::Handled();
}