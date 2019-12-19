#include "SArmyGreatPlanDetail.h"
#include "ArmyStyle.h"
#include "SImage.h"
#include "ArmySlateModule.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"
#include "ArmyGameInstance.h"

void SArmyGreatPlanDetail::Construct(const FArguments& InArgs)
{

    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(624)
        .HeightOverride(1016)
        [
            SNew(SOverlay)
            
            + SOverlay::Slot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF343539"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                .Padding(FMargin(0))
                [
                    SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.Padding(32,32,0,0)
						.AutoHeight()
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("夏日微风")))
// 						    SAssignNew(TB_ProjectContractNo, STextBlock)
 						    .TextStyle(FArmyStyle::Get(), "ArmyText_24_Bold")
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(HAlign_Fill)
							.Padding(32,20,0,0)
							[
								SAssignNew(TB_ProjectLeftDetail, SMultiLineEditableText)
								.AutoWrapText(true)
								.IsReadOnly(true)
								.TextStyle(FArmyStyle::Get(), "ArmyText_14")
                                .AllowContextMenu(false)
							]
							+ SHorizontalBox::Slot()
							.Padding(FMargin(16,20,0,0))
							.HAlign(HAlign_Fill)
							[
								SAssignNew(TB_ProjectRightDetail, SMultiLineEditableText)
								.IsReadOnly(true)
								.AutoWrapText(true)
								.TextStyle(FArmyStyle::Get(), "ArmyText_14")
                                .AllowContextMenu(false)
							]
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Top)
						[
							MakeApplyAndViewButton()
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SOverlay)
							+SOverlay::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Top)
							.Padding(32, 260, 0, 0)
							[
								SNew(SBox)
 								.WidthOverride(40.f)
 								.HeightOverride(40.f)
 								[
 									SNew(SButton)
 									.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.GreatPlanBack"))
 									.OnClicked(this, &SArmyGreatPlanDetail::GreatPlanBackEvent)
 								]
							]

							 + SOverlay::Slot()
 							.HAlign(HAlign_Left)
 							.VAlign(VAlign_Top)
 							.Padding(88, 65, 0, 0)
							[
								//图片
								SAssignNew(Box_ImageContainer, SBox)
								.WidthOverride(448.f)
								.HeightOverride(446.f)
							]
  	

							 + SOverlay::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Top)
							.Padding(470, 471, 0, 0)
							[
								SNew(SBox)
								.WidthOverride(50)
							.HeightOverride(24)
							[
								SNew(SBorder)
								.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
								.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							]
						]

							 + SOverlay::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Top)
							.Padding(484, 476, 0, 0)
							[
								SNew(STextBlock)
								.Text(this, &SArmyGreatPlanDetail::GetCurrentNumInCount)
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
							]

							+ SOverlay::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Top)
							.Padding(0, 260, 32, 0)
							[
								SNew(SBox)
								.WidthOverride(40.f)
								.HeightOverride(40.f)
								[
									SNew(SButton)
									.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.GreatPlanForward"))
									.OnClicked(this, &SArmyGreatPlanDetail::GreatPlanForwardEvent)
								]
							]
						]
					]
				]
                
            ]

            + SOverlay::Slot()
            .Padding(0, 11, 11, 0)
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Top)
            [
                SNew(SButton)
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
                .OnClicked(this, &SArmyGreatPlanDetail::OnClosed)
            ]
        ]
	];

// 	if (InArgs._ThumbnailURL.IsEmpty()) // 如果URL传入的为空，则使用传入的ImageBrush
// 	{
// 		if (ThumbnailImage.IsValid())
// 		{
// 			Box_ImageContainer->SetContent(ThumbnailImage.ToSharedRef());
// 		}
// 	}
// 	else // 如果URL传入的不为空，则使用网络下载的WebImage
// 	{
		
/*	}*/
}



void SArmyGreatPlanDetail::SetProjectLeftInfo(const FString& InLeftStr)
{
	TB_ProjectLeftDetail->SetText(FText::FromString(InLeftStr));
}

void SArmyGreatPlanDetail::SetProjectRightInfo(const FString& InRightStr)
{
	TB_ProjectRightDetail->SetText(FText::FromString(InRightStr));
}

void SArmyGreatPlanDetail::SetProjectTitle(const FString& InProjectTitle)
{
	TB_ProjectContractNo->SetText(FText::FromString(InProjectTitle));
}

void SArmyGreatPlanDetail::SetGreatPlanPicURL(TArray<FString> TResURL)
{
	TResURL.Add(TEXT("http://mvfurniture.dabanjia.com/Admin_4_1504689240783_0.jpg"));
	TResURL.Add(TEXT("http://mvfurniture.dabanjia.com/Admin_4_1498110787186_0.jpg"));
	TResURL.Add(TEXT("http://mvfurniture.dabanjia.com/SJHT_358_1530845358304_0.jpg"));
	TResURL.Add(TEXT("http://mvfurniture.dabanjia.com/SJHT_358_1526625134588_0.jpg"));
	TResURL.Add(TEXT("http://mvfurniture.dabanjia.com/SJHT_358_1522225149133_0.jpg"));
	
	TGreatPlanPicURL.Empty();
	ThumbnailImageBrush.Empty();
	for (auto TResURLIt : TResURL)
	{
		TGreatPlanPicURL.Add(TResURLIt);
	}

	for (auto GreatPlanIt : TGreatPlanPicURL)
	{
		TAttribute<const FSlateBrush*> TempBrush = FArmySlateModule::Get().WebImageCache->Download(GreatPlanIt).Get().Attr();
		ThumbnailImageBrush.Add(TempBrush);
	}

	if (ThumbnailImageBrush.Num() > 0)
	{
		Box_ImageContainer->SetContent(SNew(SImage).Image(ThumbnailImageBrush[CurrentBrushNum]));
	}
}


TSharedRef<SWidget> SArmyGreatPlanDetail::MakeApplyAndViewButton()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(32, 16, 0, 0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SNew(SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.OnClicked(FOnClicked::CreateRaw(this, &SArmyGreatPlanDetail::ApplePlanEvent))
				[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("应用方案")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(16,16,0,0)
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SNew(SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
				.OnClicked(FOnClicked::CreateRaw(this, &SArmyGreatPlanDetail::ViewPanoramasEvent))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("查看全景图")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		];
}

FReply SArmyGreatPlanDetail::OnClosed()
{
    SetVisibility(EVisibility::Hidden);

    return FReply::Handled();
}

FReply SArmyGreatPlanDetail::GreatPlanBackEvent()
{
	CurrentBrushNum = (CurrentBrushNum - 1) > 0 ? (CurrentBrushNum - 1) : 0;
	Box_ImageContainer->SetContent(SNew(SImage).Image(ThumbnailImageBrush[CurrentBrushNum]));
	FString SCurrentBrushNum = FString::FromInt(CurrentBrushNum + 1);
	FString SLineBreak = TEXT(" / ");
	FString SBrushCountNum = FString::FromInt(ThumbnailImageBrush.Num());
	FString TempString= SCurrentBrushNum.Append(SLineBreak);
	CurrentNumInCount = FText::FromString(TempString.Append(SBrushCountNum));
	return FReply::Handled();
}

FReply SArmyGreatPlanDetail::GreatPlanForwardEvent()
{
	CurrentBrushNum = (CurrentBrushNum + 1) > ThumbnailImageBrush.Num() -1  ? (ThumbnailImageBrush.Num()-1) : (CurrentBrushNum + 1);
	Box_ImageContainer->SetContent(SNew(SImage).Image(ThumbnailImageBrush[CurrentBrushNum]));
	FString SCurrentBrushNum = FString::FromInt(CurrentBrushNum + 1);
	FString SLineBreak = TEXT(" / ");
	FString SBrushCountNum = FString::FromInt(ThumbnailImageBrush.Num());
	FString TempString = SCurrentBrushNum.Append(SLineBreak);
	CurrentNumInCount = FText::FromString(TempString.Append(SBrushCountNum));
	return FReply::Handled();
}

FReply SArmyGreatPlanDetail::ApplePlanEvent()
{
	if (GGI)
	{
// 		SAssignNew(GreatProjectDetail, SArmyGreatPlanDetail);
// 		GGI->Window->PresentModalDialog(TEXT("应用方案")),
// 		GreatProjectDetail->AsShared();
		SAssignNew(OpenPlanWidget, SArmyOpenPlan);
		GGI->Window->PresentModalDialog(TEXT("应用方案"), OpenPlanWidget->AsShared(), FSimpleDelegate::CreateRaw(OpenPlanWidget.Get(), &SArmyOpenPlan::OnGreatPlanConfirmClicked), false);
	}

	return FReply::Handled();
}

FReply SArmyGreatPlanDetail::ViewPanoramasEvent()
{
	FString TheURL = TEXT("https://www.baidu.com/");
	FPlatformProcess::LaunchURL(*TheURL, nullptr, nullptr);
	return FReply::Handled();
}

void SArmyGreatPlanDetail::ResetCurrentNumInCount()
{
	FString SLineBreak = TEXT("1 / ");
	CurrentNumInCount = FText::FromString(TEXT(""));
	FString SBrushCountNum = FString::FromInt(ThumbnailImageBrush.Num());
	CurrentNumInCount = FText::FromString(SLineBreak.Append(SBrushCountNum));
}

