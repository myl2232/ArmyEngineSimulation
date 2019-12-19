#include "ArmyKitchenItemSlate.h"
#include "SImage.h"
#include "SButton.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "STextBlock.h"
#include "SCheckBox.h"
#include "ArmyStyle.h"

void SArmyKitchenItem::Construct(const FArguments& InArgs)
{
	ItemCode = InArgs._ItemCode;

	TSharedPtr<SWidget> OutWidget = NULL;

	OutWidget =
	SNew(SBox).HeightOverride(InArgs._ItemHeight).WidthOverride(InArgs._ItemWidth) //每条槽的高度
	[	
		SNew(SButton)
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
		[
	        SNew(SBorder)
			.Padding(0.f)
		    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF424447"))
		    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
	        [
	            SNew(SHorizontalBox)
	            
	            +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(16.f,0.f,0.f,0.f)
	            [
					SNew(SBox).HeightOverride(14).WidthOverride(14)
					[
						SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.Radio_Selected_AutoDesign"))
					]
	            ]
	            
	            +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(32.f, 0.f, 0.f, 0.f)
	            [
					SNew(STextBlock).Text(FText::FromString(TEXT("厨房1"))).TextStyle(&FArmyStyle::GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
	            ]

				+SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(48.f, 0.f, 0.f, 0.f)
	            [
					SNew(STextBlock).Text(FText::FromString(TEXT("13M2"))).TextStyle(&FArmyStyle::GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
	            ]
	        ]
		]
	];

	ChildSlot[OutWidget.ToSharedRef()];
}