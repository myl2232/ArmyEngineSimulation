/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyHomeMode.h
 * @Description 户型模式类目
 *
 * @Author 欧石楠
 * @Date 2018年6月22日
 * @Version 1.0
 */

#pragma once

#include "SArmyOperationBrowser.h"

class SArmyHomeBrowser : public SArmyOperationBrowser
{
public:
    SLATE_BEGIN_ARGS(SArmyHomeBrowser) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
};