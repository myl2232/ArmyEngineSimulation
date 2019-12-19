#include "ArmyCabinetItemSlate.h"
#include "SImage.h"
#include "SButton.h"
#include "SCheckBox.h"
#include "SBoxPanel.h"
#include "SOverlay.h"
#include "SBox.h"
#include "STextBlock.h"
#include "SBorder.h"
#include "ArmyStyle.h"

void SArmyCabinetItem::Construct(const FArguments& InArgs)
{
	float TextHeight = 52.f;
	float ImagePadding = 0.f;

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

			+ SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Fill)
			[
		        SNew(SVerticalBox)
		        +SVerticalBox::Slot()
		        .AutoHeight()
		        .Padding(0.f)
		        [
		        	SNew(SBox)
		        	.WidthOverride(InArgs._ItemWidth - ImagePadding * 2) //Box的宽度 - Padding
		            .HeightOverride(InArgs._ItemHeight - TextHeight) //TextHeight为预留给字体的高度
		        	[
		        		SNew(SOverlay)
		        
		        		+ SOverlay::Slot().VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		                [
		        	        SNew(SBox)
		        	        [
		        				//厨具略所图
		        				SAssignNew(ButtonSelectItem, SButton)
		        				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
		                        .ContentPadding(FMargin(0))
		                        .OnClicked(this, &SArmyCabinetItem::OnItemClicked)
		                        [
		        				    SAssignNew(ThumbnailImage, SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser"))
		        			    ]
		        		    ]
		        		]
		        
	                    + SOverlay::Slot().VAlign(VAlign_Top).HAlign(HAlign_Left).Padding(8.f,8.f,0.f,0.f)
		        		[
		        			SNew(SBox)
		        		    .Visibility(InArgs._bCanSelectType ? EVisibility::Visible : EVisibility::Collapsed)
		        			.WidthOverride(14)
		        			.HeightOverride(14)
		        			[
		        				//选中状态图
		        				SAssignNew(ImageToShowCheck,SImage).Image(FArmyStyle::Get().GetBrush("Icon.Radio_Selected_AutoDesign"))
		        			]
		        		]
		        
		        		+SOverlay::Slot().VAlign(VAlign_Top).HAlign(HAlign_Right).Padding(0.f, 8.f, 8.f, 0.f)
		        		[
		        			SNew(SBox)
		        			.WidthOverride(12)
		        			.HeightOverride(12)
		        			[
								SNew(SButton)
								.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
							    .OnHovered(this,&SArmyCabinetItem::OnItemDetailInfoHovered)
							    .OnUnhovered(this,&SArmyCabinetItem::OnItemDetailInfoUnHovered)
							    [
									//详细信息图示					
									SAssignNew(DetailInformationImage, SImage).Image(FArmyStyle::Get().GetBrush("Icon.Information_Normal_AutoDesign"))
								]
				
		        			]
		        		]
		        
		        	]
		        ]
		        
	            +SVerticalBox::Slot()
		        .FillHeight(1.0)
		        .VAlign(VAlign_Fill)
		        .HAlign(HAlign_Fill)
		        [
		        	SNew(SBorder)
		        	.VAlign(VAlign_Center)
		        	.HAlign(HAlign_Fill)
		        	.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF383A3D"))
		        	.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		        	.Padding(0.f)
		        	[
		        		SNew(SVerticalBox)
		        
		        		+SVerticalBox::Slot().FillHeight(1.0).VAlign(VAlign_Center).HAlign(HAlign_Left).Padding(8.f, 8.f, 0.f, 6.f)
		        	    [
		        			//某种厨具的具体类型
		        			SNew(STextBlock)
		        			.Text(FText::FromString(TEXT("顶吸式")))
		        	        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		        	        .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFFFFFF"))
		        		]
		        
		                + SVerticalBox::Slot().FillHeight(1.0).VAlign(VAlign_Center).Padding(8.f, 0.f, 0.f, 6.f)
		        	    [
		        	        SNew(SHorizontalBox)
		        	    
		        	        +SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center)
		        	        [
		        	        	
		        	        	SAssignNew(CabinetNameText, STextBlock)
		        	        	.Text(FText::FromString(TEXT("￥1200")))
		        	            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		        			    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
		        	        ]
		        	        			        
		        	        +SHorizontalBox::Slot().FillWidth(1.0).HAlign(HAlign_Right).VAlign(VAlign_Center).Padding(FMargin(8.f,0.f))
		        	        [
		        	        	//选择款式Button
		        				SNew(SBox).HeightOverride(16).WidthOverride(16).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		        				[
		        					SAssignNew(ButtonSelectType, SButton).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		        					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
		        				    .ContentPadding(FMargin(0))
		        				    .OnClicked(this, &SArmyCabinetItem::OnSelectTypeClicked)
		        				    .Visibility(InArgs._bCanSelectType ? EVisibility::Visible : EVisibility::Collapsed)
		        				    [
		        				    	SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.SelectType_Default_AutoDesign"))
		        				    ]
		        				]
		        	        ]
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

FReply SArmyCabinetItem::OnItemClicked()
{
	DelegateOnItemClicked.ExecuteIfBound(ItemCode);
	return FReply::Handled();
}

void SArmyCabinetItem::OnItemDetailInfoHovered()
{
	DelegateOnItemDetailInfoHovered.ExecuteIfBound(ItemCode);
}

void SArmyCabinetItem::OnItemDetailInfoUnHovered()
{
	DelegateOnItemDetailInfoUnHovered.ExecuteIfBound(ItemCode);
}

FReply SArmyCabinetItem::OnSelectTypeClicked()
{
	DelegateOnSelectTypeClicked.ExecuteIfBound(ItemCode);
	return FReply::Handled();
}