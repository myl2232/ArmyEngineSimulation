#include "ArmyConstructionModeCommands.h"

#define LOCTEXT_NAMESPACE "FArmyConstructionModeCommands"

void FArmyConstructionCommands::RegisterCommands()
{
	UI_COMMAND(CommandSave, "保存", "Save", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDelete, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandUndo, "撤销", "Undo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandRedo, "重做", "Redo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandLoadFrame, "生成图框", "Load Frame", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandSaveImage, "生成图纸", "Save image", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandFacadeIndex, "生成立面图", "Facade index", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandExportCAD, "导出CAD", "Export CAD", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandReset, "重置标注", "Reset Dismension", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandUpdate, "更新标注", "Update", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandAutoRulerUp, "上", "Size Annotation", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(CommandAutoRulerDown, "下", "Material Annotation", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(CommandAutoRulerLeft, "左", "Text Annotation", EUserInterfaceActionType::Check, FInputChord());
	UI_COMMAND(CommandAutoRulerRight, "右", "Craft Annotation", EUserInterfaceActionType::Check, FInputChord());

    // 尺寸标注
    UI_COMMAND(CommandDimension, "标尺", "Dimension", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandDownLeadLabel_Size, "规格标注", "Size Annotation", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandDownLeadLabel_Material, "材质标注", "Material Annotation", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandDownLeadLabel_Text, "文字标注", "Text Annotation", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDownLeadLabel_Height, "高度标注", "Craft Annotation", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDownLeadLabel_CustomHeight, "手动标高", "Craft CustomAnnotation", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDownLeadLabel_Craft, "工艺标注", "Height Annotation", EUserInterfaceActionType::Button, FInputChord());

    UI_COMMAND(CommandCompass, "指北针", "Compass", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandBoradSplitLine, "台面分割", "BoradSplitLine", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE