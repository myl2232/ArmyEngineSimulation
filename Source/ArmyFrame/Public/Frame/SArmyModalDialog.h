/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyModalDialog.h
 * @Description 模态对话框
 *
 * @Author 欧石楠
 * @Date 2018年7月5日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"

class ARMYFRAME_API SArmyModalDialog : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyModalDialog)
        : _bCloseImmediately(true)
		, _ConfirmText(TEXT("确认"))
		, _CancelText(TEXT("取消"))
    {}

    /** 标题 */
    SLATE_ARGUMENT(FString, Title)

    /** 内容 */
    SLATE_ARGUMENT(TSharedPtr<SWidget>, ContentWidget)

    /** 是否点击完确定按钮立即关闭对话框 */
    SLATE_ARGUMENT(bool, bCloseImmediately)

	/** @刘克祥 确认按钮的文字 */
	SLATE_ARGUMENT(FString, ConfirmText)

	/** @刘克祥 取消按钮的文字 */
	SLATE_ARGUMENT(FString, CancelText)

    /** 确定 */
    SLATE_EVENT(FSimpleDelegate, OnConfirm)

    /** @欧石楠 取消 */
    SLATE_EVENT(FSimpleDelegate, OnCancel)

    /** 立即关闭 */
    SLATE_EVENT(FSimpleDelegate, OnImmediatelyClose)

    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

private:
    FReply OnConfirmButtonClicked();
    FReply OnCloseButtonClicked();
    FReply OnCancelButtonClicked();

    TSharedRef<SWidget> MakeConfirmCancelButtons();

private:
    TSharedPtr<SVerticalBox> VB_Dialog;

    bool bCloseImmediately;

	/**@刘克祥 确认取消按钮的文本*/
	FString ConfirmText;
	FString CancelText;

    FSimpleDelegate OnConfirm;
    FSimpleDelegate OnCancel;
	FSimpleDelegate OnImmediatelyClose;
};