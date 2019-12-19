#include "ArmyBoxItemSlate.h"
#include "SImage.h"
#include "SlateOptMacros.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"
#include "SEditableText.h"
#include "ArmySlateModuleSlate.h"
#include "SArmyApplyMasterPlan.h"

void SBoxItem::Construct(const FArguments& InArgs)
{
	ContentImage = InArgs._ContentImage;
	OnBoxClickedDelegate = InArgs._OnBoxClickedDelegate;

	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(InArgs._ItemWidth)
			.HeightOverride(InArgs._ItemHeight)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Top)
			[
				SNew(SOverlay)
				//选择框背景色
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBox)
					.WidthOverride(InArgs._ItemWidth)
					.HeightOverride(InArgs._ItemHeight)
					[
						SNew(SOverlay)
						+SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[	
							SNew(SOverlay)
							+SOverlay::Slot()
							[
								SNew(SBorder)
								.BorderBackgroundColor(InArgs._BackgroundColor)
								.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							]
							+SOverlay::Slot()
							[
								SAssignNew(Border_ContentSBorder,SBorder)
								//.Padding(FMargin(0))
								.BorderImage(FArmyStyle::Get().GetBrush("Border.Orange"))
								.Visibility(EVisibility::Collapsed)  //Collapsed
							]
						]
					]
				]
			
				+SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				.Padding(0, 32, 0, 0)
				[
					//选择框中平面布局，硬装，软装的图片
					SAssignNew(Box_ContentImageContainer,SBox)
					.WidthOverride(InArgs._ContentImageItemWidth)
					.HeightOverride(InArgs._ContentImageItemWidth)
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				.Padding(0, 120, 0, 0)
				[
					//选择框中平面布局，硬装，软装的文字说明
					SNew(STextBlock)
					.Text(InArgs._DesInfoText)//
					.TextStyle(InArgs._TextStyle)
				]
				//边框
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[	
					SNew(SBox)
					.WidthOverride(168)
					.HeightOverride(168)
					[
						SNew(SOverlay)
						+SOverlay::Slot()
						[
							SAssignNew(Border_ContentSBorder, SBorder)
							//.Padding(FMargin(0))
							.BorderImage(FArmyStyle::Get().GetBrush("Border.Orange"))
							.Visibility(EVisibility::Collapsed)  //Collapsed
						]
						+SOverlay::Slot()
						[	
							//选择框逻辑中的按钮
							SNew(SBox)
							.WidthOverride(168)
							.HeightOverride(168)
							[
								SNew(SButton)
								.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
								.ContentPadding(FMargin(0))
								.OnClicked(this, &SBoxItem::OnBoxClicked)
								.OnHovered(this, &SBoxItem::OnBoxHovered)
								.OnUnhovered(this, &SBoxItem::OnBoxUnhovered)
	
							]
						]
						+SOverlay::Slot()
						.VAlign(VAlign_Bottom)
						.HAlign(HAlign_Right)
						[
							SNew(SBox)
							.WidthOverride(28)
							.HeightOverride(28)
							[
								SAssignNew(IsCheckedImage,SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.ResourceReady"))
								.Visibility(EVisibility::Collapsed)
							]
						]
					
					]
				]	
				]
			];
			Box_ContentImageContainer->SetContent(ContentImage.ToSharedRef());
}

FReply SBoxItem::OnBoxClicked()
{
	if (IsChoosed)
	{
		IsCheckedImage->SetVisibility(EVisibility::Collapsed);
		Border_ContentSBorder->SetVisibility(EVisibility::Collapsed);
		IsChoosed = false;
	}
	else
	{
		IsCheckedImage->SetVisibility(EVisibility::HitTestInvisible);
		Border_ContentSBorder->SetVisibility(EVisibility::HitTestInvisible);
		IsChoosed = true;
	}
	OnBoxClickedDelegate.ExecuteIfBound();
	return FReply::Handled();
}

void SBoxItem::OnBoxHovered()
{
	Border_ContentSBorder->SetVisibility(EVisibility::HitTestInvisible);
}

void SBoxItem::OnBoxUnhovered()
{
	if (IsChoosed)
	{
		return;
	}
	Border_ContentSBorder->SetVisibility(EVisibility::Collapsed);
}
