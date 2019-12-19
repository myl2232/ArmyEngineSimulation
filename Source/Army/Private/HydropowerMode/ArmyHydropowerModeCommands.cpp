#include "ArmyHydropowerModeCommands.h"

#define LOCTEXT_NAMESPACE "FArmyHomeModeCommands"

void FArmyHydropowerModeCommands::RegisterCommands()
{
	UI_COMMAND(CommandSave, "保存", "Save", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandUndo, "撤销", "Undo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandGroup,"相连接","Group", EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandRedo, "重做", "Redo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandEmpty, "清空", "Empty", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDelete, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
	UI_COMMAND(CommandReplace, "替换", "Delete", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandCopy, "复制", "Delete", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandSwitchConnect,"灯控设置","进入灯孔设置",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandExitSwitchConnect,"退出灯控","退出灯控设置",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandPowerSystem,"配电箱设置","进入配电箱设置",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandAutoDesignLine,"自动布线","自动设计配电系统",EUserInterfaceActionType::Button,FInputChord());

	UI_COMMAND(CommandDeleteStrongELe,"清空强电线路","Strong",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandDeleteLighting,"清空照明线路","Strong",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandDeleteWeakELe,"清空弱电线路","Strong",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandDeleteColdWater,"清空冷水管线路","Strong",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandDeleteHotWater,"清空热水管线路","Strong",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandDeleteDrain,"清空排水管线路","Strong",EUserInterfaceActionType::Button,FInputChord());
	UI_COMMAND(CommandDeleteAll,"清空所有","Empty All",EUserInterfaceActionType::Button,FInputChord());

	UI_COMMAND(CommandViewCombo, "平面视图", "ViewModeCombo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandOverallView, "3D视图", "OverallView", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(CommandTopView, "地面视图", "Plane View", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(CommandCeilingView, "顶面视图", "Ceiling View", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandAutoDesignPoint, "自动布点", "自动设计点位", EUserInterfaceActionType::Button, FInputChord());

}

#undef LOCTEXT_NAMESPACE