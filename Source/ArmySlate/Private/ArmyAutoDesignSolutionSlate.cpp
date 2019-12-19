#include "ArmyAutoDesignSolutionSlate.h"
#include "SImage.h"
#include "SButton.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "STextBlock.h"
#include "SOverlay.h"
#include "SBorder.h"
#include "ArmyStyle.h"

void SArmyAutoDesignSolution::Construct(const FArguments& InArgs)
{
	float TextHeight = 30.f;

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
		        SNew(SVerticalBox)
		        
		        +SVerticalBox::Slot()
		        .AutoHeight()
		        .Padding(0.f)
		        [
		        	SNew(SBox)
		        	.WidthOverride(InArgs._ItemWidth) //Box的宽度 - Padding
		            .HeightOverride(InArgs._ItemHeight - TextHeight) //TextHeight为预留给字体的高度
		        	[
		        		SNew(SOverlay)
		        		
		        		+SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		        	    [
		        			//方案结果图片
		        			SAssignNew(ThumbnailImage, SImage).Image(FArmyStyle::Get().GetBrush("Icon.KitchenType_Default_AutoDesign"))
		        		]
		        		
	                    +SOverlay::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom)
	                    [
		        			SNew(SBox).WidthOverride(24).HeightOverride(24)
		        			[
		        				SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.ResourceReady"))
		        			]
		        		]
		        	]
		        ]
		        
	            +SVerticalBox::Slot()
				.FillHeight(1.0)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
		        [
	              	SNew(SBorder).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
                    .Padding(0.f)
		        	.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF383A3D"))
		        	.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		        	[
		        		//应用Button
		        		SAssignNew(ButtonApply, SButton)
		        		.VAlign(VAlign_Center)
		        		.HAlign(HAlign_Center)
		        		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
		        	    .ContentPadding(FMargin(0))
		        	    [
		        			SNew(STextBlock).Text(FText::FromString(TEXT("应用")))
		        			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		        	        .ColorAndOpacity(FSlateColor(FLinearColor::White))
		        	    ]
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