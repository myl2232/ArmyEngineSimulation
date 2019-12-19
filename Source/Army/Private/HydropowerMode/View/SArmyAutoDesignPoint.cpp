#include "SArmyAutoDesignPoint.h"
#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"
#include "SBoxItem.h"
#include "ArmyGameInstance.h"

void SArmyAutoDesignPoint::Construct(const FArguments& InArgs)
{
	/*FName AutoWaterPicInfo = TEXT("HydropowerMode.autowater");
	FString AutoWaterTipsInfo = TEXT("自动生成水位");

	FName AutoSwitchPicInfo = TEXT("HydropowerMode.autoswitch");
	FString AutoSwitchTipsInfo = TEXT("自动生成开关");

	FName AutoSocketPicInfo = TEXT("HydropowerMode.autosocket");
	FString AutoSocketTipsInfo = TEXT("自动生成插座");*/
	OnClickDelegate = InArgs._OnClickDelegate;

	/*IsAutoInfo.Add(autoWater);
	IsAutoInfo.Add(autoSwitch);
	IsAutoInfo.Add(autoSocket);*/

	ChildSlot
		[
			SNew(SBorder)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.99000000"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			//.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.99000000"))
			//.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			[
				SNew(SBorder)
				.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()

				[
					SNew(SBox)
					.HeightOverride(48)
					.WidthOverride(704)
					[
						SNew(SBorder)
						.Padding(0)
						.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
						.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						[
							SNew(SHorizontalBox)
							//标题
							+SHorizontalBox::Slot()
							.Padding(20, 0, 0, 0)
							.AutoWidth()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("自动布点应用")))//InArgs._Title
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
							]
							
							//关闭按钮
							+SHorizontalBox::Slot()
							.Padding(0,0,5,0)
							.FillWidth(1)
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							[
								SNew(SBox)
								.WidthOverride(30)
								.HeightOverride(30)
								[
									SNew(SButton)
									.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
									.OnClicked(this, &SArmyAutoDesignPoint::OnCommand, 0)
								]
							]
						]
					]
				]
				
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					.WidthOverride(704)
					.HeightOverride(334)
					[
						SNew(SOverlay)
						+SOverlay::Slot()
						[
							SNew(SBorder)
							.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
							.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						]
						
						+SOverlay::Slot()
 						.Padding(80,40,0,0)
 						[
							MakeAutoWaterBoxItem().ToSharedRef()
 							//MakeWaterCheckWidget(AutoWaterPicInfo,AutoWaterTipsInfo)
 						]
						+SOverlay::Slot()
						.VAlign(VAlign_Top)
						.HAlign(HAlign_Left)
						.Padding(268, 40, 0, 0)
						[
							MakeAutoSwitchBoxItem().ToSharedRef()
						]

						+SOverlay::Slot()
						.VAlign(VAlign_Top)
						.HAlign(HAlign_Left)
						.Padding(455, 40, 0, 0)
						[
							MakeAutoSoketBoxItem().ToSharedRef()
						]

						+SOverlay::Slot()
						.VAlign(VAlign_Top)
						.HAlign(HAlign_Left)
						.Padding(80, 232, 0, 0)
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("注意：自动布点将会清空水电模式与自动布点相关的开关、插座")))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
						]
								
						+SOverlay::Slot()
						.Padding(0, 20, 20, 20)
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Bottom)
						[
							MakeConfirmCancelButton()
						]
					]
				]
			]
				
		]
		];

}

bool SArmyAutoDesignPoint::GetIsAutoWaterInfo()
{
	return AutoWaterBoxItem->IsChoosed;
	//return false;
}

bool SArmyAutoDesignPoint::GetIsAutoSwitchInfo()
{
	return AutoSwitchBoxItem->IsChoosed;
}

bool SArmyAutoDesignPoint::GetIsAutoSocketInfo()
{
	return AutoSocketBoxItem->IsChoosed;
}

FReply SArmyAutoDesignPoint::OnCommand(int32 InV)
{
	OnClickDelegate.ExecuteIfBound(InV);

	return FReply::Handled();
	
}


TSharedPtr<SWidget> SArmyAutoDesignPoint::MakeAutoWaterBoxItem()
{
	return
		AutoWaterBoxItem = SNew(SBoxItem)
		.ContentImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("HydropowerMode.autowater")))
		.OnBoxClickedDelegate(this, &SArmyAutoDesignPoint::ChangeCurrentButtonStyle)
		.DesInfoText(FText::FromString(TEXT("自动生成水位")));
}

TSharedPtr<SWidget> SArmyAutoDesignPoint::MakeAutoSwitchBoxItem()
{
	return
		AutoSwitchBoxItem = SNew(SBoxItem)
		.ContentImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("HydropowerMode.autoswitch")))
		.OnBoxClickedDelegate(this, &SArmyAutoDesignPoint::ChangeCurrentButtonStyle)
		.DesInfoText(FText::FromString(TEXT("自动生成开关")));
}

TSharedPtr<SWidget> SArmyAutoDesignPoint::MakeAutoSoketBoxItem()
{
	return
		AutoSocketBoxItem = SNew(SBoxItem)
		.ContentImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("HydropowerMode.autosocket")))
		.OnBoxClickedDelegate(this, &SArmyAutoDesignPoint::ChangeCurrentButtonStyle)
		.DesInfoText(FText::FromString(TEXT("自动生成插座")));
}

void SArmyAutoDesignPoint::ChangeCurrentButtonStyle()
{
	if ((GetIsAutoWaterInfo() || GetIsAutoSwitchInfo() || GetIsAutoSocketInfo()))
	{
		ConfirmBtn->SetButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"));
	}
	else
	{
		ConfirmBtn->SetButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.MasterPlanGray"));
	}
}

TSharedRef<SWidget> SArmyAutoDesignPoint::MakeConfirmCancelButton()
{
	return
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(0,0,20,0)
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SAssignNew( ConfirmBtn,SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.MasterPlanGray"))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.OnClicked(this, &SArmyAutoDesignPoint::OnCommand, 1)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("确定")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		]

		+SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SBox)
			.WidthOverride(80)
			.HeightOverride(30)
			[
				SNew(SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
				.OnClicked(this, &SArmyAutoDesignPoint::OnCommand, 0)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("取消")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				]
			]
		];
}

void SArmyAutoDesignFailWid::Construct(const FArguments& InArgs)
{
	FString TitleStr = TEXT("还有") + FString::FromInt(InArgs._FailInfoNum) + TEXT("个点位未布置：");
	FString HandleStr = TEXT("请联系管理员在企业后台-智能设计-自动布点配置相关商品。");
	if (InArgs._FailInfoType == 0)
	{
		HandleStr = TEXT("自动布点未找到合适的布点位置，请手动添加点位。");
	}
	else
	{
		HandleStr = TEXT("请联系管理员在企业后台-智能设计-自动布点配置相关商品。");
	}
	
	ChildSlot
		[
			SNew(SBorder)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(0)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.99000000"))
			.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
			[
				SNew(SBorder)
				.Padding(0)
				.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				[
					 SNew(SVerticalBox)
				  // 标题栏
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.HeightOverride(48)
						[
							SNew(SBorder)
							.Padding(0)
							.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
							.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
							[
								SNew(SHorizontalBox)

								// 标题
								+ SHorizontalBox::Slot()
								.Padding(16, 0, 0, 0)
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("提醒")))
									.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
								]

								// 关闭按钮
								+ SHorizontalBox::Slot()
								.Padding(0, 0, 5, 0)
								.FillWidth(1)
								.HAlign(HAlign_Right)
								.VAlign(VAlign_Center)
								[
									SNew(SBox)
									.WidthOverride(30)
									.HeightOverride(30)
									[
										SNew(SButton)
										.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
										.OnClicked(FOnClicked::CreateLambda([this]() {
										GGI->Window->DismissModalDialog();
										return FReply::Handled();
									}))
									]
								]
							]
						]
					]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(0)
					.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Left)
						.VAlign(VAlign_Top)
						.Padding(20,40, 16, 0)

						.AutoWidth()
						[
							SNew(SBox)
							.HeightOverride(32)
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.ModalDialog_Warning"))
							]
						]

						// 失败详情
						+ SHorizontalBox::Slot()
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.Padding(16, 0, 20, 0)
						.AutoWidth()
						[
							SNew(SVerticalBox)  
								
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0, 0, 0, 8)
							[
								SNew(SBox)
								.WidthOverride(332)
								.HeightOverride(73)
								.VAlign(VAlign_Bottom)
									
								[
									SNew(STextBlock)
									.Text(FText::FromString(TitleStr))
									.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))

								]
							]
							//滚动详情
							+ SVerticalBox::Slot()
							.FillHeight(1.f)
							[
								SNew(SBorder)
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.Padding(0)
								.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF424447"))
								.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
								[
									SNew(SBox)
									.WidthOverride(332)
									.HeightOverride(108)
									[
										SNew(SScrollBox)
										.Orientation(Orient_Vertical)
										.Style(FArmyStyle::Get(), "ScrollBar.Style.Gray")
										.ScrollBarStyle(FArmyStyle::Get(), "ScrollBar.BarStyle.Gray")

										+ SScrollBox::Slot()
										.HAlign(HAlign_Fill)
										.VAlign(VAlign_Fill)
										.Padding(16, 4, 40, 4)
										[
											SNew(SMultiLineEditableText)
											.TextStyle(FArmyStyle::Get(), "ArmyText_12")
											.IsReadOnly(true)
											.Text(FText::FromString(InArgs._FailInfo))
											.AutoWrapText(true)
											.LineHeightPercentage(1.5f)
											.AllowContextMenu(false)
										]
									]
								]
							]

							// 分割线
							+ SVerticalBox::Slot()
							.AutoHeight()

							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Splitter.FF343538"))
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0, 8, 0, 0)
							[
								SNew(STextBlock)
								.Text(FText::FromString(HandleStr))
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
							]

							// 确认按钮
							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							.Padding(0, 20, 20, 20)
							[
								SNew(SBox)
								.WidthOverride(80)
								.HeightOverride(30)
								[
									SNew(SButton)
									.ButtonStyle(FArmyStyle::Get(), "Button.Gray.FF353638")
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Center)
									.OnClicked(InArgs._OnConfirm)
									[
										SNew(STextBlock)
										.Text(MAKE_TEXT("我知道了"))
										.TextStyle(FArmyStyle::Get(), "ArmyText_12")
									]
								]
							]
						]
					]
				]
			]
		]
	];
}
