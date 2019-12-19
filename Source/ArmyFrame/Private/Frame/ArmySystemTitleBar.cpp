#include "SArmySystemTitleBar.h"
#include "ArmyStyle.h"
#include "SArmyTextBlock.h"
#include "ArmyFrameModule.h"
#include "ArmySlateModule.h"
#include "SArmyImageTextButton.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"

#define LOCTEXT_NAMESPACE "SArmySystemTitleBar"

#define MAX_ICON_WIDTH 120
#define MAX_ICON_HEIGHT 32

void SArmySystemTitleBar::Construct(const FArguments& InArgs)
{
	MenuConstrctionInfos = InArgs._MenuConstrctionInfos;

	GlobalCommandsList = InArgs._CommandsList;

    ChildSlot
    [
        SAssignNew(TitleBar, SBox)
        .HeightOverride(TitleBarHeight)
        .Padding(FMargin(16, 0))
        [
            SNew(SHorizontalBox)

            // 应用程序图标
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SAssignNew(Box_AppIcon, SBox)
                .WidthOverride_Lambda([this]() {
                    if (AppIconBrush.IsSet())
                    {
                        FVector2D IconSize = AppIconBrush.Get()->ImageSize;
                        float Scale = FMath::Max<float>(IconSize.X / MAX_ICON_WIDTH, IconSize.Y / FMath::Min<float>(TitleBarHeight, MAX_ICON_HEIGHT));
                        return IconSize.X / Scale;
                    }
                    return 100.f;
                })
                .HeightOverride_Lambda([this]() {
                    if (AppIconBrush.IsSet())
                    {
                        FVector2D IconSize = AppIconBrush.Get()->ImageSize;
                        float Scale = FMath::Max<float>(IconSize.X / MAX_ICON_WIDTH, IconSize.Y / FMath::Min<float>(TitleBarHeight, MAX_ICON_HEIGHT));
                        return IconSize.Y / Scale;
                    }
                    return 20.f;
                })
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(AppIcon, SImage)
                    .Image(FArmyStyle::Get().GetBrush("Icon.Application"))
                ]
            ]

            // 公司名称
            + SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            [
                SAssignNew(HB_CompanyName, SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(0, 0, 14, 0)
                [
                    SNew(SBox)
                    .HeightOverride(12)
                    [
                        SNew(SImage)
                        .Image(FArmyStyle::Get().GetBrush("Splitter.Gray.FF424447"))
                    ]
                ]
                
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(TB_Company, STextBlock)
                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
                ]
            ]

            // 菜单栏
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            .VAlign(VAlign_Center)			
            [
                SNew(SHorizontalBox)

                // 菜单
                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
                [
                    CreateMenu()
                ]


				// 查看方案估价按钮
				+SHorizontalBox::Slot()
					.AutoWidth()
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Center)
					[
						//SNew(SHorizontalBox)
						SAssignNew(HB_PlanPrice, SHorizontalBox)
						.Visibility(EVisibility::Collapsed)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SAssignNew(ITB_PlanPrice, SArmyImageTextButton)
							.LayoutOrientation(LO_Horizontal)
							.ButtonStyle(FArmyStyle::Get(), "Button.None")
							.Image(FArmyStyle::Get().GetBrush("Icon.PlanPrice.Normal"))
							.Text(MAKE_TEXT("查看方案估价"))
							.TextColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.OnClicked_Lambda([this]() {
							// 查看方案估价
							FArmyFrameModule::Get().OnShowPricePage.ExecuteIfBound();
							return FReply::Handled();
							})
							.OnHovered_Lambda([this]() {
								ITB_PlanPrice->SetImage(FArmyStyle::Get().GetBrush("Icon.PlanPrice.Hovered"));
								ITB_PlanPrice->SetTextColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
							})
							.OnUnhovered_Lambda([this]() {
								ITB_PlanPrice->SetImage(FArmyStyle::Get().GetBrush("Icon.PlanPrice.Normal"));
								ITB_PlanPrice->SetTextColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"));
							})
						]

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(14, 0)
						[
							SNew(SBox)
							.HeightOverride(12)
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Splitter.Gray.FF424447"))
							]
						]
					]

                // 返回首页按钮
                + SHorizontalBox::Slot()
                .AutoWidth()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(HB_GotoHomePage, SHorizontalBox)
                    .Visibility(EVisibility::Collapsed)

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        SAssignNew(ITB_GotoHomePage, SArmyImageTextButton)
                        .LayoutOrientation(LO_Horizontal)
                        .ButtonStyle(FArmyStyle::Get(), "Button.None")
                        .Image(FArmyStyle::Get().GetBrush("Icon.HomePage.Normal"))
                        .Text(MAKE_TEXT("返回首页"))
                        .TextColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"))
                        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                        .OnClicked_Lambda([this]() {
                            // 返回主页
                            FArmyFrameModule::Get().OnGotoHomePage.ExecuteIfBound();
                            return FReply::Handled();
                        })
                        .OnHovered_Lambda([this]() {
                            ITB_GotoHomePage->SetImage(FArmyStyle::Get().GetBrush("Icon.HomePage.Hovered"));
                            ITB_GotoHomePage->SetTextColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
                        })
                        .OnUnhovered_Lambda([this]() {
                            ITB_GotoHomePage->SetImage(FArmyStyle::Get().GetBrush("Icon.HomePage.Normal"));
                            ITB_GotoHomePage->SetTextColor(FArmyStyle::Get().GetColor("Color.Gray.FFC8C9CC"));
                        })
                    ]

                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(14, 0)
                    [
                        SNew(SBox)
                        .HeightOverride(12)
                        [
                            SNew(SImage)
                            .Image(FArmyStyle::Get().GetBrush("Splitter.Gray.FF424447"))
                        ]
                    ]
                ]


			


                // 账号相关
                + SHorizontalBox::Slot()
                .AutoWidth()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(AccountComboBox, SArmyTextPulldownBox)
                    .ComboItemList(InArgs._ExtendComboList)
                    .OnComboBoxSelectionChanged(InArgs._OnComboItemChanged)
                    .Visibility(EVisibility::Hidden)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Center)
                .Padding(8, 0, 14, 0)
                [
                    SNew(SBox)
                    .HeightOverride(12)
                    [
                        SNew(SImage)
                        .Image(FArmyStyle::Get().GetBrush("Splitter.Gray.FF424447"))
                    ]
                ]
            ]
				
            + SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign(HAlign_Right)
            [
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
                .VAlign(VAlign_Center)
				[
					SNew(SHorizontalBox)

					// 最小化按钮
					+ SHorizontalBox::Slot()
                    .AutoWidth()
					[
						SNew(SButton)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Minimize"))
						.OnClicked(this, &SArmySystemTitleBar::OnMinimizeButtonClicked)
					]

					// 关闭按钮
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SButton)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
						.OnClicked(this, &SArmySystemTitleBar::OnCloseButtonClicked)
					]
				]
            ]
        ]
    ];
}

void SArmySystemTitleBar::SetStyle(const FArmySystemTitleBarStyle& SystemTitleBarStyle)
{
    if (SystemTitleBarStyle.bShowApplicationIcon)
    {
        AppIcon->SetVisibility(EVisibility::HitTestInvisible);
    }
    else
    {
        AppIcon->SetVisibility(EVisibility::Collapsed);
    }

    if (SystemTitleBarStyle.bShowMenuBar)
    {
        MenuBar->SetVisibility(EVisibility::Visible);
    }
    else
    {
        MenuBar->SetVisibility(EVisibility::Hidden);
    }

    if (SystemTitleBarStyle.bShowAccountMenu)
    {
        AccountComboBox->SetVisibility(EVisibility::Visible);
    }
    else
    {
        AccountComboBox->SetVisibility(EVisibility::Hidden);
    }

    TitleBarHeight = SystemTitleBarStyle.TitleBarHeight;
    TitleBar->SetHeightOverride(SystemTitleBarStyle.TitleBarHeight);
}

TSharedRef<SWidget> SArmySystemTitleBar::CreateMenu()
{
    const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	
	FMenuBarBuilder MenuBuilder(GlobalCommandsList, MenuExtender);
	{		
		for (auto MInfo : MenuConstrctionInfos)
		{
			MenuBuilder.AddPullDownMenu(
				MInfo->M_Label,
				MInfo->M_Tip,
				FNewMenuDelegate::CreateStatic(&SArmySystemTitleBar::FillMenu, MenuExtender, MInfo)
			);
		}
	}

    MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	// Create the menu bar!
	return
        SAssignNew(MenuBar, SBox)
        .HeightOverride(24)
        .VAlign(VAlign_Bottom)
        [
            MenuBuilder.MakeWidget()
        ];
}
void SArmySystemTitleBar::FillMenu(FMenuBuilder& MenuBuilder, const TSharedRef<FExtender> Extender, TSharedPtr<FArmyMenuInfo> InMenuInfo)
{
	for (auto & SectionInfo : InMenuInfo->SectionInfos)
	{
		MenuBuilder.BeginSection(SectionInfo.ExtensionHook, SectionInfo.HeadingText);
		{
			for (auto Command : SectionInfo.ActionList)
			{
				MenuBuilder.AddMenuEntry(Command);
			}
		}
		MenuBuilder.EndSection();
	}
}

void SArmySystemTitleBar::SetAccountName(FString InAccountName)
{	
	AccountComboBox->SetDisplayContent(InAccountName);
	AccountComboBox->SetVisibility(EVisibility::Visible);
}

void SArmySystemTitleBar::SetCompanyName(FString InCompanyName)
{
    TB_Company->SetText(FText::FromString(InCompanyName));
}

void SArmySystemTitleBar::SetAppIcon(FString InIconUrl)
{
    if (!InIconUrl.IsEmpty())
    {
        AppIconBrush = FArmySlateModule::Get().WebImageCache->Download(InIconUrl).Get().Attr();
        AppIcon->SetImage(AppIconBrush);
    }
}

void SArmySystemTitleBar::ShowGotoHomePageButton(bool bShow)
{
    // @欧石楠 显示返回主页的时候，也意味着不显示公司名称
    if (bShow)
    {
		HB_PlanPrice->SetVisibility(EVisibility::Visible);
        HB_GotoHomePage->SetVisibility(EVisibility::Visible);
        HB_CompanyName->SetVisibility(EVisibility::Collapsed);
    }
    else
    {
		HB_PlanPrice->SetVisibility(EVisibility::Collapsed);
        HB_GotoHomePage->SetVisibility(EVisibility::Collapsed);
        HB_CompanyName->SetVisibility(EVisibility::Visible);
    }
}

FReply SArmySystemTitleBar::OnMinimizeButtonClicked()
{
    TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();

    if (RootWindow.IsValid())
    {
        TSharedPtr<FGenericWindow> NativeWindow = RootWindow->GetNativeWindow();

        if (NativeWindow.IsValid())
        {
            NativeWindow->Minimize();
        }
    }

    return FReply::Handled();
}

FReply SArmySystemTitleBar::OnCloseButtonClicked()
{
    FArmyFrameModule::Get().OnApplicationClosed.ExecuteIfBound();

    return FReply::Handled();
}

void SArmySystemTitleBar::OnOpenVideoTutorial()
{
    FString URL = "https://dabanjia.com/html/dbj_yy_video.html";
    FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
}

#undef  LOCTEXT_NAMESPACE