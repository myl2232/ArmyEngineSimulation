/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyWindow.h
 * @Description 全局唯一的窗口
 *
 * @Author 欧石楠
 * @Date 2018年6月6日
 * @Version 1.0
 */

#pragma once

#include "SArmySystemTitleBar.h"
#include "ArmyViewController.h"
#include "ArmyTypes.h"
#include "SArmyMessage.h"
#include "IHttpRequest.h"

#include "Widgets/SCompoundWidget.h"

class ARMYFRAME_API SArmyWindow : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyWindow) {}

	SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandList)

	SLATE_ARGUMENT(TArray<TSharedPtr<FArmyMenuInfo>>, MenuConstrctionInfos)

	SLATE_ARGUMENT(TArray<TSharedPtr<FString>>, ExtendComboList)

	SLATE_EVENT(FOnComboBoxSelectionChanged, OnComboItemChanged)

    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    /** 显示视图控制器 */
    void PresentViewController(TSharedPtr<class FArmyViewController> ViewController);

    /** 以模态形式显示视图控制器 */
    void PresentModalViewController(TSharedPtr<FArmyViewController> ViewController);

    /** 弹出上一个模态视图控制器 */
    void DismissModalViewController();

    /** 移除所有模态视图控制器 */
    void DismissAllModalViewControllers();

	/** 移除所有视图控制器，注销用 */
	void DismissAllViewControllers();

	/** @欧石楠 获取标题栏widget*/
    TSharedPtr<SArmySystemTitleBar> GetSystemTitleBar() { return SystemTitleBar; }

    /** 显示模态对话框 */
    void PresentModalDialog(const FString& InTitle, TSharedRef<SWidget> InContentWidget, FSimpleDelegate OnConfirm = FSimpleDelegate(), bool bCloseImmediately = true);
    void PresentModalDialog(const FString& InMessage, FSimpleDelegate OnConfirm, bool bCloseImmediately = true, const FString& InTitle = TEXT("提醒"));//梁晓菲 默认显示“提醒”
	void PresentModalDialog(const FString& InMessage, FSimpleDelegate OnConfirm, FSimpleDelegate OnClose, bool bCloseImmediately = true, const FString& InTitle = TEXT("提醒"));
	void PresentModalDialog(TSharedRef<SWidget> InContentWidget);

	void PresentModalDialog(
        const FString& InTitle,
        TSharedRef<SWidget> InContentWidget,
        FSimpleDelegate OnConfirm,
        FSimpleDelegate OnCancel,
        FSimpleDelegate OnClose,
        bool bCloseImmediately = false,
        const FString& ConfirmString = TEXT("确认"),
        const FString& CancelString = TEXT("取消"));

	/**@欧石楠 指定确认和取消的文字内容*/
	void PresentModalDialog(
        const FString& InMessage,
        FSimpleDelegate OnConfirm,
        FSimpleDelegate OnCancel,
        FSimpleDelegate OnClose,
        const FString& ConfirmString,
        const FString& CancelString,
        bool bCloseImmediately = true,
        const FString& InTitle = TEXT("提醒"));

    /** 隐藏模态对话框 */
    void DismissModalDialog();

    /**
     * 显示和隐藏加载提示消息
     * @param Message - const TAttribute<FText> & - 消息内容
     * @param bCanceable - bool - 是否支持取消
     * @param CanceableRequest - TSharedPtr<IHttpRequest> - 需要取消的HTTP请求
     */
    void ShowThrobber(const TAttribute<FText>& Message, bool bCanceable = false, TSharedPtr<IHttpRequest> CanceableRequest = nullptr);
    void ShowThrobber(const TAttribute<FText>& Message, FSimpleDelegate OnCancel);
	void UpdateThrobberMessage(const TAttribute<FText>& Message);
	// zengy added
	void AlwaysShowThrobber(const TAttribute<FText>& Message, bool bCanceable = false, TSharedPtr<IHttpRequest> CanceableRequest = nullptr);
	void AlwaysShowThrobber(const TAttribute<FText>& Message, FSimpleDelegate OnCancel);
    //void ShowThrobber(const FString& Message, bool bCanceable = false, TSharedPtr<IHttpRequest> CanceableRequest = nullptr);
    //void ShowThrobber(const FString& Message, FSimpleDelegate OnCancel);
    void HideThrobber();
	// zengy added
	void AlwaysHideThrobber();

    /**
     * 显示提示消息
     * @param MessageType - EXRMessageType - 消息类型
     * @param Message - const FString & - 消息内容
     */
	void ShowMessage(enum EXRMessageType MessageType, const FString& Message);

    // zengy added
    void ShowMessageWithoutTimeout(EXRMessageType MessageType, const FString& Message);
    void HideMessage();

	/** @马云龙 显示屏幕底部信息，通常为“按F键交互”*/
	void ShowInteractiveMessage(bool bInShow);

    /** 设置标题栏 */
    void SetTitleBar(TSharedPtr<SWidget> InTitleBar);

    /** 设置窗口内容 */
    void SetWindowContent(TSharedPtr<SWidget> Widget);

    /** 获取当前活动的控制器 */
    TSharedPtr<FArmyViewController> GetActiveController();

	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);

	/** @马云龙 切换设计模式和VR全屏模式 */
	void SetViewportFullscreen(bool bFullscreen);

    // @zengy 2019-4-16
    void NotifyQuitCurrentViewController();

public:
    TArray< TSharedPtr<FArmyViewController> > ControllerStack;

    /** 窗口层叠 */
    TSharedPtr<SOverlay> WindowOverlay;

private:
    /** 自定义标题栏容器 */
    TSharedPtr<SBox> CustomTitleBarContainer;

    /** 系统标题栏 */
    TSharedPtr<SArmySystemTitleBar> SystemTitleBar;

    /** 窗口容器 */
    TSharedPtr<SBox> WindowContainer;

    /** 内容层叠 */
    TSharedPtr<SOverlay> ModalViewOverlay;

    /** 消息提示 */
    TSharedPtr<class SArmyMessage> MessageWidget;

    /** 加载提示消息 */
    TSharedPtr<class SArmyThrobber> Throbber;
	// zengy added
	bool AlwaysShow = false;

	/** @欧石楠 标题栏widget */
	TSharedPtr<SWidget> CustomTitleBarWidget;

	/** @马云龙 交互信息提示（“按F键交互”），位于屏幕中底部 */
	TSharedPtr<SBorder> InteractiveMessageWidget;

	/** @马云龙 窗口整体切换，用于设计模式和VR模式，其中索引0位设计模式的Widget，1为Viewport全屏VR模式 */
	TSharedPtr<class SWidgetSwitcher> WindowSwitcher;
};