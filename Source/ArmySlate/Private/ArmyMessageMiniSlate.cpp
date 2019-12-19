#include "ArmyMessageMiniSlate.h"
#include "SImage.h"

void SArmyMessageMini::Construct(const FArguments& InArgs)
{
	ErrorVisibility = InArgs._bError ? EVisibility::Visible : EVisibility::Collapsed;
	RollingVisibility = InArgs._bRolling ? EVisibility::Visible : EVisibility::Collapsed;
	CancelVisibility = InArgs._bCancel ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed;
	//MessageText = InArgs._MessageText;
	CancelDelegate = InArgs._CancelDelegate;
    ImgRotDegree = 0;

	ChildSlot
	[
		SNew(SBox)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.Padding(FMargin(20,10,20,10))
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			[
				SNew(SHorizontalBox)
						
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(24)
					.HeightOverride(24)
					.Visibility(this, &SArmyMessageMini::GetRollingVisibility)
					[
						SAssignNew(Img_Throbble, SImage)
						.Image(FArmyStyle::Get().GetBrush("Icon.Throbber"))
					]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(24)
					.HeightOverride(24)
					.Visibility(this, &SArmyMessageMini::GetErrorVisibility)
					[
						SNew(SImage)
						.Image(FArmyStyle::Get().GetBrush("Icon.Error"))
					]
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(10,0,0,0))
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(this, &SArmyMessageMini::GetMessageText)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(10, 0, 0, 0))
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(24)
					.HeightOverride(24)
					.Visibility(this, &SArmyMessageMini::GetCancelVisibility)
					[
						SNew(SButton)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Close"))
						.OnClicked(this, &SArmyMessageMini::OnCancelClicked)
					]
				]
			]
		]
	];

	ActiveTimerHandle = RegisterActiveTimer(1.f / 60.f, FWidgetActiveTimerDelegate::CreateSP(this, &SArmyMessageMini::ActiveTick));
}

void SArmyMessageMini::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    ImgRotDegree = (ImgRotDegree + 5) % 360;
    Img_Throbble->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    Img_Throbble->SetRenderTransform(FSlateRenderTransform(FQuat2D(FMath::DegreesToRadians(ImgRotDegree))));
}

FReply SArmyMessageMini::OnCancelClicked()
{
	CancelDelegate.ExecuteIfBound();
	return FReply::Handled();
}

EActiveTimerReturnType SArmyMessageMini::ActiveTick(double InCurrentTime, float InDeltaTime)
{
    return EActiveTimerReturnType::Continue;
}
