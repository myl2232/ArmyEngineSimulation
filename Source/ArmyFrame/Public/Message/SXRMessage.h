/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyMessage.h
 * @Description 顶部消息提示
 *
 * @Author 欧石楠
 * @Date 2018年7月5日
 * @Version 1.0
 */

#pragma once

#include "SImage.h"
#include "Widgets/SCompoundWidget.h"

enum EXRMessageType
{
    /** 成功 */
    MT_Success,

    /** 警告 */
    MT_Warning,

	/** 梁晓菲 正常*/
	MT_Normal,
};

class SArmyMessage : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyMessage) {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    void ShowMessage(EXRMessageType MessageType, const FString& Message);
    // zengy added
    void ShowMessageWithoutTimeout(EXRMessageType MessageType, const FString& Message);
    void HideMessage();

private:
    void HideMessageImpl();

private:
	TSharedPtr<SBorder> BackgroundBorder;
    TSharedPtr<SBorder> MessageBorder;
    TSharedPtr<SImage> Img_Icon;
    TSharedPtr<STextBlock> TB_Message;

    FTimerHandle MessageTimeHandle;
};