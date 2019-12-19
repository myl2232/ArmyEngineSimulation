/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRHardModeCommands.cpp
 * @Description 硬装工具相关命令注册类
 *
 * @Author 
 * @Date 2018年6月23日
 * @Version 1.0
 */
#include "ArmyHardModeCommands.h"

#define LOCTEXT_NAMESPACE "FArmyHomeModeCommands"

void FArmyHardModeCommands::RegisterCommands()
{
	UI_COMMAND(CommandSave, "保存", "Save", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandUndo, "撤销", "Undo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandRedo, "重做", "Redo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandEmpty, "清空", "Empty", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandEmptyPlan, "清空所有", "Empty", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandEmptyStaticLighting, "清空渲染效果", "EmptyStaticLighting", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDelete, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandReplace, "替换", "Delete", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandCopy, "复制", "Delete", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandArea, "自由绘制", "Area", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandRectArea, "矩形", "RectArea", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandCircleArea, "圆形", "CircleArea", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandRegularPolygonArea, "多边形", "RegularPolygonArea", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandLight, "光源", "Light", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandPointLight, "点光源", "PointLight", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandSpotLight, "聚光灯", "SpotLight", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandReflectionCapture, "反射球", "ReflectionCapture", EUserInterfaceActionType::Button, FInputChord());

    UI_COMMAND(CommandDisplay, "显示", "Display", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandDisplayLightIcon, "光源图标", "DisplayLightIcon", EUserInterfaceActionType::Check, FInputChord());

	UI_COMMAND(CommandViewCombo, "平面视图", "ViewModeCombo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandHardOverallView, "3D视图", "OverallView", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(CommandHardTopView, "地面视图", "Plane View", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(CommandHardCeilingView, "顶面视图", "Ceiling View", EUserInterfaceActionType::Check, FInputChord());

	UI_COMMAND(CommandBuildStaticLighting, "全屋渲染", "BuildStaticLighting", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandInteractiveHovered, "交互", "Interactive", EUserInterfaceActionType::Button, FInputChord(EKeys::F));
	
	UI_COMMAND(CommandMaterialBrush, "材质刷", "MaterialBrush", EUserInterfaceActionType::Button, FInputChord());
	
}

#undef LOCTEXT_NAMESPACE