/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHardModeFloorController.h
 * @Description 硬装工具地面模块控制类
 *
 * @Author 
 * @Date 2018年6月27日
 * @Version 1.0
 */

#pragma once

#include "ArmyBaseSurfaceController.h"
class FArmyHardModeCeilingController : public FArmyBaseSurfaceController
{
public:
	virtual void Init() override;

	virtual TSharedPtr<SWidget> MakeLeftPanelWidget() override;
	virtual TSharedPtr<SWidget> MakeRightPanelWidget() override;
	virtual TSharedPtr<SWidget> MakeToolBarWidget() override;
	virtual TSharedPtr<SWidget> MakeSettingBarWidget() override;

	//TSharedPtr<SWidget> CreateToolBarRightArea();

private:
	TSharedRef<SWidget> OnAreaCommand();
};