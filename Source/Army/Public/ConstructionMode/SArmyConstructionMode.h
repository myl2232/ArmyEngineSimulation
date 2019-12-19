/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyConstructionMode.h
 * @Description 施工图绘制
 *
 * @Author HUAEN
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "SArmyOperationBrowser.h"

class SArmyConstructionBrowser : public SArmyOperationBrowser
{
public:
    SLATE_BEGIN_ARGS(SArmyConstructionBrowser) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
};