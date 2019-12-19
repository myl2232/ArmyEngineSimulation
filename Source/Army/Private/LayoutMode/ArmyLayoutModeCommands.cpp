#include "ArmyLayoutModeCommands.h"

#define LOCTEXT_NAMESPACE "FArmyLayoutModeCommands"

void FArmyLayoutModeCommands::RegisterCommands()
{
	/**@欧石楠 ToolBar Commands*/
	UI_COMMAND(CommandSave, "保存", "Save", EUserInterfaceActionType::Button, FInputChord());
	//UI_COMMAND(CommandUndo, "撤销", "Undo", EUserInterfaceActionType::Button, FInputChord());
	//UI_COMMAND(CommandRedo, "重做", "Redo", EUserInterfaceActionType::Button, FInputChord());
	
	UI_COMMAND(CommandEmpty, "清空", "Empty", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDelete, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord());	

	//拆改
	UI_COMMAND(CommandNewWall, "新建墙体", "NewWall", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDismantleWall, "拆除墙体", "DismantleWall", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandNewPass, "开垭口", "NewPass", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandPackPipe, "包立管", "PackPipe", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandCurtainBox, "窗帘盒", "CurtainBox", EUserInterfaceActionType::Button, FInputChord());

    // 门窗
    UI_COMMAND(CommandDoor, "标准门", "Door", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandSlidingDoor, "推拉门", "SlidingDoor", EUserInterfaceActionType::Button, FInputChord());    
	

    UI_COMMAND(CommandDismantle, "拆改中", "Dismantle", EUserInterfaceActionType::ToggleButton, FInputChord());
    UI_COMMAND(CommandAfterDismantle, "拆改后", "AfterDismantle", EUserInterfaceActionType::ToggleButton, FInputChord());

	/** @纪仁泽 区域分割 */
	UI_COMMAND(CommandSplitRegion, "区域分割", "SplitRegion", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE