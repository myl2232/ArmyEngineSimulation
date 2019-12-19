/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHardModeCommands.h
 * @Description 硬装工具相关命令注册类
 *
 * @Author 
 * @Date 2018年6月23日
 * @Version 1.0
 */

#pragma once

#include "ArmyStyle.h"
#include "Commands.h"

class FArmyHardModeCommands : public TCommands<FArmyHardModeCommands>
{
public:
	FArmyHardModeCommands()
        : TCommands<FArmyHardModeCommands>(
            TEXT("HardMode"),
            NSLOCTEXT("Contexts", "HardMode", "Hard Mode"),
            NAME_None,
            FArmyStyle::Get().GetStyleSetName()
            )
    {}
    virtual void RegisterCommands() override;
public:
	TSharedPtr<FUICommandInfo> CommandSave;		// 保存
	TSharedPtr<FUICommandInfo> CommandUndo;		// 撤销
	TSharedPtr<FUICommandInfo> CommandRedo;		// 重做
	TSharedPtr<FUICommandInfo> CommandEmpty;	// 清空（顶部工具栏的“清空”按钮，下拉菜单，含有图标）
	TSharedPtr<FUICommandInfo> CommandEmptyPlan;	// 清空下拉菜单的“清空所有”命令，不含图标（里面模式下的方案数据）
	TSharedPtr<FUICommandInfo> CommandEmptyStaticLighting;	// 清空下拉菜单的“清空光照烘焙数据”命令，不含图标
	TSharedPtr<FUICommandInfo> CommandDelete;	// 删除
	TSharedPtr<FUICommandInfo> CommandReplace;	// 替换
	TSharedPtr<FUICommandInfo> CommandCopy;		// 复制

	TSharedPtr<FUICommandInfo> CommandReturnViewAngle;		// 返回视角

	TSharedPtr<FUICommandInfo> CommandArea;		// 区域
	TSharedPtr<FUICommandInfo> CommandRectArea;		// 矩形
	TSharedPtr<FUICommandInfo> CommandCircleArea;		// 圆形
	TSharedPtr<FUICommandInfo> CommandRegularPolygonArea;		// 规则多边形

	/**@欧石楠 平面视图下拉*/
	TSharedPtr<FUICommandInfo> CommandViewCombo;//视图下拉
	/**@欧石楠 三种视图*/
	TSharedPtr<FUICommandInfo> CommandHardOverallView;	//透视图
	TSharedPtr<FUICommandInfo> CommandHardTopView;		//平面视图
	TSharedPtr<FUICommandInfo> CommandHardCeilingView;	//吊顶视图

	/**@马云龙 烘焙光照按钮，弹出烘焙设置界面*/
	TSharedPtr<FUICommandInfo> CommandBuildStaticLighting;//构建光照

	/** 光源 */
	TSharedPtr<FUICommandInfo> CommandLight;

	/** 点光源 */
	TSharedPtr<FUICommandInfo> CommandPointLight;

	/** 聚光灯 */
	TSharedPtr<FUICommandInfo> CommandSpotLight;

	/** 反射球 */
	TSharedPtr<FUICommandInfo> CommandReflectionCapture;

    /** 显示 */
    TSharedPtr<FUICommandInfo> CommandDisplay;

    /** 显示光源图标 */
    TSharedPtr<FUICommandInfo> CommandDisplayLightIcon;

	/**@马云龙 视口中按空格键执行鼠标当前触碰到的Actor交互功能 */
	TSharedPtr<FUICommandInfo> CommandInteractiveHovered;

	/** @郭子阳 材质刷 */
	TSharedPtr<FUICommandInfo> CommandMaterialBrush;
};