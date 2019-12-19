/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File ArmyUpdatedNotes.h
 * @Description 版本更新内容说明界面
 *
 * @Author 欧石楠
 * @Date 2019年3月1日
 * @Version 1.0
 */

#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SlateDelegates.h"
#include "Widgets/SCompoundWidget.h"

class SArmyUpdatedNotes : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyUpdatedNotes) {}

    /** @欧石楠 发版公告内容 */
    SLATE_ARGUMENT(FString, Content)

    /** 确认事件 */
    SLATE_EVENT(FOnClicked, OnConfirm)
    
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

private:
    TSharedRef<SWidget> CreateTitleWidget();
};