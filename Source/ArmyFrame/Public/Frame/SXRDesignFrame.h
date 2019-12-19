/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyDesignFrame.h
 * @Description 设计界面UI框架
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"

class ARMYFRAME_API SArmyDesignFrame : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyDesignFrame) {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    void HideDesignPanel(bool bHidden);

public:
    TSharedPtr<SBox> LeftPanel;
    TSharedPtr<SBox> RightPanel;
    TSharedPtr<SBox> ToolBar;
    TSharedPtr<SBox> SettingBar;
};