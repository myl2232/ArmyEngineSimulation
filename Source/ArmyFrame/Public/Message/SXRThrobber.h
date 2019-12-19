/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyThrobber.h
 * @Description 加载提示消息
 *
 * @Author 欧石楠
 * @Date 2018年7月5日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"

class ARMYFRAME_API SArmyThrobber : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyThrobber) {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    //~ Begin SCompoundWidget Interface
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    //~ End SCompoundWidget Interface

    void SetMessage(const TAttribute<FText>& Message) { TB_Message->SetText(Message); }
    void SetMessage(const FString& Message) { TB_Message->SetText(FText::FromString(Message)); }
    void SetCanceable(bool bCanceable, TSharedPtr<class IHttpRequest> InCanceableRequest);
    void SetOnCancel(FSimpleDelegate InOnCancel);

private:
    FReply OnCancelClicked();

private:
    TSharedPtr<STextBlock> TB_Message;
    TSharedPtr<SButton> Btn_Cancel;

    TSharedPtr<class SImage> Img_Throbble;
    int32 ImgRotDegree;

    TSharedPtr<IHttpRequest> CanceableRequest;

    /** 取消回调 */
    FSimpleDelegate OnCancel;

    /** 注册弹出窗口为不断更新的Widget */
    TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;
    EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
};