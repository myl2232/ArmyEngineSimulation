/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmySystemTitleBar.h
 * @Description 标题栏基类
 *
 * @Author 欧石楠
 * @Date 2018年6月9日
 * @Version 1.0
 */

#pragma once

#include "SImage.h"
#include "Widgets/SCompoundWidget.h"
#include "SArmyComboBox.h"

/**	菜单定义*/
struct FArmyMenuInfo
{
	struct FArmyMenuSectionInfo
	{
		FName ExtensionHook = NAME_None;
		FText HeadingText = FText();
		TArray<TSharedPtr<FUICommandInfo>> ActionList;
	};
	FText M_Label = FText();
	FText M_Tip = FText();

	TArray<FArmyMenuSectionInfo> SectionInfos;
};
/**
 * 系统标题栏样式
 */
struct FArmySystemTitleBarStyle
{
    /** 是否显示app图标 */
    bool bShowApplicationIcon;

    /** 是否显示菜单栏 */
    bool bShowMenuBar;

    /** 是否显示账号菜单 */
    bool bShowAccountMenu;

    /** 标题栏高度 */
    int32 TitleBarHeight;

    FArmySystemTitleBarStyle()
        : bShowApplicationIcon(false)
        , bShowMenuBar(false)
        , bShowAccountMenu(false)
        , TitleBarHeight(32)
    {}

    FArmySystemTitleBarStyle(bool InbShowApplicationIcon, bool InbShowMenuBar, bool InbShowAccountMenu, int32 InTitleBarHeight = 32)
        : bShowApplicationIcon(InbShowApplicationIcon)
        , bShowMenuBar(InbShowMenuBar)
        , bShowAccountMenu(InbShowAccountMenu)
        , TitleBarHeight(InTitleBarHeight)
    {}
};

class ARMYFRAME_API SArmySystemTitleBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmySystemTitleBar) {}

	SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandsList)

	SLATE_ARGUMENT(TArray<TSharedPtr<FArmyMenuInfo>>, MenuConstrctionInfos)

	SLATE_ARGUMENT(TArray<TSharedPtr<FString>>, ExtendComboList)

	SLATE_EVENT(FOnComboBoxSelectionChanged,OnComboItemChanged)

    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    /** 设置系统菜单栏样式 */
    void SetStyle(const FArmySystemTitleBarStyle& SystemTitleBarStyle);

	void SetAccountName(FString InAccountName);
    void SetCompanyName(FString InCompanyName);
    void SetAppIcon(FString InIconUrl);

    /** 是否显示返回主页按钮 */
    void ShowGotoHomePageButton(bool bShow);

private:
    /** 最小化窗口 */
    FReply OnMinimizeButtonClicked();

    /** 关闭窗口 */
    FReply OnCloseButtonClicked();
    
    /** 打开视频教程 */
    void OnOpenVideoTutorial();

    /** 创建菜单 */
    TSharedRef<SWidget> CreateMenu();

	static void FillMenu(FMenuBuilder& MenuBuilder, const TSharedRef<FExtender> Extender, TSharedPtr<FArmyMenuInfo> InMenuInfo);

private:
    TSharedPtr<SBox> TitleBar;
    float TitleBarHeight = 32.f;

    TSharedPtr<SBox> Box_AppIcon;

    /** app图标 */
    TSharedPtr<SImage> AppIcon;

    /** 菜单栏 */
    TSharedPtr<SBox> MenuBar;

    /** 公司名称 */
    TSharedPtr<SHorizontalBox> HB_CompanyName;
    TSharedPtr<STextBlock> TB_Company;

	//@欧石楠
	//方案估价
	TSharedPtr<SHorizontalBox> HB_PlanPrice;
	TSharedPtr<class SArmyImageTextButton> ITB_PlanPrice;
    /** 返回主页 */
    TSharedPtr<SHorizontalBox> HB_GotoHomePage;
    TSharedPtr<class SArmyImageTextButton> ITB_GotoHomePage;

    TSharedPtr<FUICommandList> GlobalCommandsList;

	TArray<TSharedPtr<FArmyMenuInfo>> MenuConstrctionInfos;

	TSharedPtr<class SArmyTextPulldownBox> AccountComboBox;

    TAttribute<const FSlateBrush*> AppIconBrush;
};