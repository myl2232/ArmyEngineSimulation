#include "ArmyWayButtonSlate.h"
#include "SImage.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "SBorder.h"
#include "STextBlock.h"

void SArmyWayButton::Construct(const FArguments & InArgs)
{
	bIsSelected = false;
	BorderSelectedColor = FArmyStyle::Get().GetColor("Color.FFFD9800");
	BorderNormalColor = FArmyStyle::Get().GetColor("Color.FF343539");
	BorderColor = BorderNormalColor;

	OnDelegate_Clicked = InArgs._OnDelegate_Clicked;

	ChildSlot
	[
		SNew(SBox)
		.WidthOverride(InArgs._Width)
		.HeightOverride(InArgs._Height)
		[
			SNew(SBorder)
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Fill)
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			.BorderBackgroundColor(this, &SArmyWayButton::GetBorderColor)
			.Padding(2)
			[
				SNew(SBorder)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.BorderBackgroundColor(BorderNormalColor)
				.Padding(FMargin(56,40,56,40))
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					.VAlign(VAlign_Top)
					[
						SNew(SImage)
						.Image(InArgs._ButtonImage)
					]
					+ SVerticalBox::Slot()
					.VAlign(VAlign_Bottom)
					.Padding(0, 20, 0, 0)
					[
						SNew(STextBlock)
						.Text(InArgs._ButtonText)
                        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
					]
				]
			]
		]
	];
}

void SArmyWayButton::OnMouseEnter(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	BorderColor = BorderSelectedColor;
}

void SArmyWayButton::OnMouseLeave(const FPointerEvent & MouseEvent)
{
	bIsSelected = false;
	BorderColor = BorderNormalColor;
}

FReply SArmyWayButton::OnMouseButtonDown(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	bIsSelected = true;
	return FReply::Handled();
}

FReply SArmyWayButton::OnMouseButtonUp(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	if (bIsSelected) {
		OnDelegate_Clicked.ExecuteIfBound();
		bIsSelected = false;
	}	
	return FReply::Handled();
}
