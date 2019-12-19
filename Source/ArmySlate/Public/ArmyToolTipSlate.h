/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyToolTip.h
 * @Description 控件提示
 *
 * @Author 欧石楠
 * @Date 2019年1月10日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SToolTip.h"

class ARMYSLATE_API SArmyToolTip : public SToolTip
{
public:
    SLATE_BEGIN_ARGS(SArmyToolTip) {}

    SLATE_ATTRIBUTE(FText, Text)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
};