/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHardModeCommands.h
 * @Description 水电工具相关命令注册类
 *
 * @Author 朱同宽
 * @Date 2018年6月23日
 * @Version 1.0
 */

#pragma once

#include "ArmyStyle.h"
#include "Commands.h"

class FArmyHydropowerModeCommands : public TCommands<FArmyHydropowerModeCommands>
{
public:
	FArmyHydropowerModeCommands()
        : TCommands<FArmyHydropowerModeCommands>(
            TEXT("HydropowerMode"),
            NSLOCTEXT("Contexts", "HydropowerMode", "HydropowerMode"),
            NAME_None,
            FArmyStyle::Get().GetStyleSetName()
            )
    {}

    //~ Begin TCommand<> interface
    virtual void RegisterCommands() override;
    //~ End TCommand<> interface

public:
	TSharedPtr<FUICommandInfo> CommandSave;		// 保存
	TSharedPtr<FUICommandInfo> CommandUndo;		// 撤销
	TSharedPtr<FUICommandInfo> CommandRedo;		// 重做
	TSharedPtr<FUICommandInfo> CommandGroup;    // 相连接
	TSharedPtr<FUICommandInfo> CommandSwitchConnect;    // 开关控制图
	TSharedPtr<FUICommandInfo> CommandExitSwitchConnect;    // 退出开关控制图
	TSharedPtr<FUICommandInfo> CommandPowerSystem;    //配电系统
	TSharedPtr<FUICommandInfo> CommandAutoDesignLine;    //自动配电
	TSharedPtr<FUICommandInfo> CommandAutoDesignPoint;    //自动布点
	TSharedPtr<FUICommandInfo> CommandEmpty;	// 清空
	TSharedPtr<FUICommandInfo> CommandReplace;	// 替换
	TSharedPtr<FUICommandInfo> CommandCopy;		// 复制
	TSharedPtr<FUICommandInfo> CommandDelete;	// 删除
	TSharedPtr<FUICommandInfo> CommandDeleteStrongELe;	//强电删除
	TSharedPtr<FUICommandInfo> CommandDeleteLighting;	//照明删除
	TSharedPtr<FUICommandInfo> CommandDeleteWeakELe;	//弱点删除
	TSharedPtr<FUICommandInfo> CommandDeleteColdWater;	//冷水管删除
	TSharedPtr<FUICommandInfo> CommandDeleteHotWater;	//热水管删除
	TSharedPtr<FUICommandInfo> CommandDeleteDrain;	//排水管删除
	TSharedPtr<FUICommandInfo> CommandDeleteAll;	//排水管删除

	/**@欧石楠 平面视图下拉*/
	TSharedPtr<FUICommandInfo> CommandViewCombo;//视图下拉
	/**@欧石楠 三种视图*/
	TSharedPtr<FUICommandInfo> CommandOverallView;	//透视图
	TSharedPtr<FUICommandInfo> CommandTopView;		//平面视图
	TSharedPtr<FUICommandInfo> CommandCeilingView;	//吊顶视图
};