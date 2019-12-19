#include "ArmySlateMessageSlate.h"
#include "SImage.h"

void SArmySlateMessage::Construct(const FArguments& InArgs)
{
	RollingVisibility = InArgs._Rolling ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed;
	ConfirmVisibility = InArgs._Confirm ? EVisibility::Visible : EVisibility::Collapsed;
	CancelVisibility = InArgs._Cancel ? EVisibility::Visible : EVisibility::Collapsed;
	//MessageText = InArgs._MessageText;
    ImgRotDegree = 0;

	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(273)
		.HeightOverride(170)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
            .FillHeight(1)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
				.AutoWidth()
				[
                    SAssignNew(Img_Throbble, SImage)
                    .Image(FArmyStyle::Get().GetBrush("Icon.Throbber"))
                    .Visibility(this, &SArmySlateMessage::GetRollingVisibility)
				]

				+ SHorizontalBox::Slot()
                .HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(this, &SArmySlateMessage::GetMessageText)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]

			+ SVerticalBox::Slot()
            .AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(FMargin(0, 0, 20, 20))
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
                .Padding(FMargin(0, 0, 20, 0))
				[
                    SNew(SBox)
                    .WidthOverride(80)
                    .HeightOverride(30)
                    [
                        SNew(SButton)
                        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
					    .OnClicked(this, &SArmySlateMessage::OnConfirmClicked)
					    .Visibility(this, &SArmySlateMessage::GetConfirmVisibility)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
					    [
						    SNew(STextBlock)
						    .Text(FText::FromString(TEXT("确认")))
						    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					    ]
                    ]
				]

				+ SHorizontalBox::Slot()
                .AutoWidth()
				[
                    SNew(SBox)
                    .WidthOverride(80)
                    .HeightOverride(30)
                    [
                        SNew(SButton)
                        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
					    .OnClicked(this, &SArmySlateMessage::OnCancelClicked)
					    .Visibility(this, &SArmySlateMessage::GetCancelVisibility)
                        .HAlign(HAlign_Center)
                        .VAlign(VAlign_Center)
					    [
						    SNew(STextBlock)
						    .Text(FText::FromString(TEXT("取消")))
						    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					    ]
                    ]
				]
			]
		]
	];

	ActiveTimerHandle = RegisterActiveTimer(1.f / 60.f, FWidgetActiveTimerDelegate::CreateSP(this, &SArmySlateMessage::ActiveTick));
}

void SArmySlateMessage::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    ImgRotDegree = (ImgRotDegree + 5) % 360;
    Img_Throbble->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    Img_Throbble->SetRenderTransform(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(ImgRotDegree))));
}

FReply SArmySlateMessage::OnConfirmClicked()
{
	ConfirmDelegate.ExecuteIfBound();
	return FReply::Handled();
}

FReply SArmySlateMessage::OnCancelClicked()
{
	CancelDelegate.ExecuteIfBound();
	return FReply::Handled();
}

EActiveTimerReturnType SArmySlateMessage::ActiveTick(double InCurrentTime, float InDeltaTime)
{
    return EActiveTimerReturnType::Continue;
}
