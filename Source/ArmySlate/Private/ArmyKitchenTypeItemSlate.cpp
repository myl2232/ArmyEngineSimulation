#include "ArmyKitchenTypeItemSlate.h"
#include "SImage.h"
#include "SButton.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "STextBlock.h"
#include "ArmyStyle.h"
#include "SOverlay.h"

void SArmyKitchenTypeItem::Construct(const FArguments& InArgs)
{
	ItemCode = InArgs._ItemCode;

	TSharedPtr<SWidget> OutWidget = NULL;

	OutWidget =
	SNew(SBox)
	.WidthOverride(InArgs._ItemWidth)
	.HeightOverride(InArgs._ItemHeight)
	[
		SNew(SButton)
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
		.ContentPadding(FMargin(0))
		.OnHovered_Lambda(
			[this]() {
		        Border->SetVisibility(EVisibility::Visible);
	        }
		)
		.OnUnhovered_Lambda(
			[this]() {
			    Border->SetVisibility(EVisibility::Collapsed);
		    }
		)
		[
			SNew(SOverlay)
			
			+SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		    [
		        SNew(SOverlay)
		        
		        + SOverlay::Slot().VAlign(VAlign_Fill).HAlign(HAlign_Fill)
	            [
		        
		        	//形状略所图
		        	SAssignNew(ButtonSelectItem, SButton)
		        	.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
	                .ContentPadding(FMargin(0))
	                [
		        	    SAssignNew(ThumbnailImage, SImage).Image(FArmyStyle::Get().GetBrush("Icon.KitchenType_Default_AutoDesign"))					
		        	]	        
		        ]
		        
	            + SOverlay::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(4.f)
		        [
		        	SNew(SBox)
		        	.WidthOverride(14)
		        	.HeightOverride(14)
		        	[
		        		//选中状态图
		        		SAssignNew(ImageToShowCheck,SImage).Image(FArmyStyle::Get().GetBrush("Icon.Radio_Selected_AutoDesign"))
		        	]
		        ]
			]

			+ SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		    [
				SAssignNew(Border, SBorder)
				.Padding(FMargin(0))
				.BorderImage(FArmyStyle::Get().GetBrush("Border.Orange"))
				.Visibility(EVisibility::Collapsed)
		    ]
		]
	];

	ChildSlot[OutWidget.ToSharedRef()];
}