#include "ArmyWHCModeCommands.h"

#define LOCTEXT_NAMESPACE "FArmyWHCModeCommands"

void FArmyWHCModeCommands::RegisterCommands()
{
	UI_COMMAND(SaveCommand, "保存", "Save", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeleteCommand, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(HideCommand, "隐藏", "Hide", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(HideCabinetDoorCommand, "橱柜门板", "HideCabinetDoor", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(HideOnGroundCabCommand, "橱柜地柜", "HideOnGroundCab", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(HideHangingCabCommand, "橱柜吊柜", "HideHangingCab", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(HideFurnitureCommand, "软装", "HideFurniture", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(HideHydropowerCommand, "水电", "HideHydropower", EUserInterfaceActionType::Check, FInputChord());

	UI_COMMAND(StyleCommand, "风格", "Style", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(RulerCommand, "标尺", "Ruler", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CabinetRulerCommand, "柜体标尺", "CabinetRuler", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(EnvRulerCommand, "环境标尺", "EnvRuler", EUserInterfaceActionType::Check, FInputChord());

	UI_COMMAND(GeneratingCommand, "生成", "Generating", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(PlatformGeneratingCommand, "橱柜台面", "PlatformGenerating", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ToeGeneratingCommand, "橱柜踢脚线", "ToeGenerating", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(TopBlockerGeneratingCommand, "橱柜上线条", "TopBlockerGenerating", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(BottomBlockerGeneratingCommand, "橱柜下线条", "BottomBlockerGenerating", EUserInterfaceActionType::Button, FInputChord());

    UI_COMMAND(ClearSceneCommand, "清空", "ClearScene", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(ClearCabinetCommand, "清空橱柜", "ClearCabinet", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(ClearWardrobeCommand, "清空衣柜", "ClearWardrobe", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(ClearOtherCommand, "清空其它柜子", "ClearOther", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(ClearAllCommand, "清空所有", "ClearAll", EUserInterfaceActionType::Button, FInputChord());

    UI_COMMAND(TopdownViewCommand, "平面视图", "TopdownView", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(RegularViewCommand, "3D视图", "RegularView", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE