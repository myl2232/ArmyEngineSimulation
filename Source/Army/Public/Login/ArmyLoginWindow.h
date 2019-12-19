/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRLoginWindow.h
 * @Description 登录窗口
 *
 * @Author 欧石楠
 * @Date 2018年12月13日
 * @Version 1.0
 */
#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "IHttpRequest.h"

class SArmyLoginWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyLoginWindow) {}	
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);	

	void Init();

	void ShowThrobber(const TAttribute<FText>& Message, bool bCanceable = false, TSharedPtr<IHttpRequest> CanceableRequest = nullptr);		
	void HideThrobber();
private:
	/** 登录请求 */
	void RequestLogin(FString Username, FString Password);

	void OnLoginSuccess();

	void OnLoginFailed(const int32 ErrorCode, const FString& ErrorMsg);

	/** 记住密码 */
	void RememberMe();

	/** 自动登录 */
	void AutoLogin();

	/** 关闭窗口 */
	FReply OnCloseButtonClicked();

    void PresentModalDialog(
        const FString& InMessage, 
        FSimpleDelegate OnConfirm, 
        FSimpleDelegate OnClose, 
        const FString& ConfirmString, 
        const FString& CancelString, 
        bool bCloseImmediately = true, 
        const FString& InTitle = TEXT("提醒"));

private:
	TSharedPtr<class SArmyLogin> LoginWidget;

    TSharedPtr<class SOverlay> Overlay;

    TSharedPtr<class SArmyModalDialog> ModalDialog = nullptr;

	/** 加载提示消息 */
	TSharedPtr<class SArmyThrobber> Throbber;

	/** 轮播控件*/
	TSharedPtr<class SArmyCarouselPanel> LoginCarouselPanel;
};