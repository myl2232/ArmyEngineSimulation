/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File AliyunOssModule.h
 * @Description 阿里云oss对接
 *
 * @Author 欧石楠
 * @Date 2018年11月19日
 * @Version 1.0
 */

#pragma once

#include "Modules/ModuleInterface.h"
#include "IHttpRequest.h"
#include "IHttpResponse.h"

class FAliyunOssModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};