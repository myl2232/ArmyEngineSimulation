/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File ARMYFrameModule.h
 * @Description ArmyFrame模块接口
 *
 * @Author 欧石楠
 * @Date 2019年7月28日
 * @Version 1.0
 */

#pragma once

#include "Modules/ModuleInterface.h"

class FArmyFrameModule : public IModuleInterface
{
public:
    // IModuleInterface Interface Begin
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // IModuleInterface Interface End

    ARMYFRAME_API static FArmyFrameModule& Get();

public:
    /** 程序关闭的代理 */
    FSimpleDelegate OnApplicationClosed;

    /** 返回主页代理 */
    FSimpleDelegate OnGotoHomePage;

	//@欧石楠
	/** 查看方案估价代理 */
	FSimpleDelegate OnShowPricePage;

private:
    static FArmyFrameModule* Singleton;
};