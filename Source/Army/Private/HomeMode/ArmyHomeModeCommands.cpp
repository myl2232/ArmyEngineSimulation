#include "ArmyHomeModeCommands.h"

#define LOCTEXT_NAMESPACE "FArmyHomeModeCommands"

void FArmyHomeModeCommands::RegisterCommands()
{
	/**@欧石楠 ToolBar Commands*/
	UI_COMMAND(CommandSave, "保存", "Save", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandUndo, "撤销", "Undo", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandRedo, "重做", "Redo", EUserInterfaceActionType::Button, FInputChord());
	
	UI_COMMAND(CommandHouseType, "户型", "HouseType", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandEmpty, "清空", "Empty", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDelete, "删除", "Delete", EUserInterfaceActionType::Button, FInputChord(EKeys::Delete));
    UI_COMMAND(CommandImportFacsimile, "导入底图", "ImportFacsimile", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandSetFacsimileScale, "设置比例尺", "SetFacsimileScale", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandOffset, "偏移", "Offset", EUserInterfaceActionType::Button, FInputChord(EKeys::O));
	UI_COMMAND(CommandCreateOutWall, "生成外墙", "CreateOutWall", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDeleteOutWall, "删除外墙", "DeleteOutWall", EUserInterfaceActionType::Button, FInputChord());

    // 建筑
	UI_COMMAND(CommandInternalWall, "原始内墙", "InternalWall", EUserInterfaceActionType::Button, FInputChord(EKeys::L));
	UI_COMMAND(CommandBalconyPunch, "开阳台", "Punch", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandPass, "门洞", "Pass", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandSecurityDoor, "防盗门", "SecurityDoor", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CommandOriginalWall, "原始内墙", "OriginalWall", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandBearingWall, "承重墙", "BearingWall", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandSpaceLine, "空间线", "SpaceLine", EUserInterfaceActionType::Button, FInputChord());

    // 门窗
    UI_COMMAND(CommandDoor, "标准门", "Door", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandSlidingDoor, "推拉门", "SlidingDoor", EUserInterfaceActionType::Button, FInputChord());    

    UI_COMMAND(CommandWindow, "标准窗", "Window", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandFloorWindow, "落地窗", "Window", EUserInterfaceActionType::Button, FInputChord());
    UI_COMMAND(CommandBayWindow, "标准飘窗", "BayWindow", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandCornerBayWindow, "转角飘窗", "CornerBayWindow", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandTrapezoidBayWindow, "梯形飘窗", "TrapezoidBayWindow", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandArcWindow, "弧形窗", "ArcWindow", EUserInterfaceActionType::Button, FInputChord());

	// 部件
	UI_COMMAND(CommandPillar, "柱子", "Pillar", EUserInterfaceActionType::Button, FInputChord());		
	UI_COMMAND(CommandAirFlue, "风道", "AirFlue", EUserInterfaceActionType::Button, FInputChord());	
	UI_COMMAND(CommandBeam, "梁", "Beam", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandAirLouver, "空调孔", "AirLouver", EUserInterfaceActionType::Button, FInputChord());

	//点位
	UI_COMMAND(CommandDiversityWaterDevice, "分集水器", "DiversityWaterDevice", EUserInterfaceActionType::Button, FInputChord());
	//UI_COMMAND(CommandMainBlowOffPipe, "排污主管", "MainBlowOffPipe", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandDewateringPipe, "下水管", "DewateringPipe", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandRainPipe, "雨水管", "RainPipe", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandClosestool, "坐便下水", "Closestool", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandGasMeter, "燃气表", "GasMeter", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandGasMainPipe, "燃气主管道", "GasPipe", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandHighElvBox, "强电箱", "HighElvBox", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandLowElvBox, "弱电箱", "LowElvBox", EUserInterfaceActionType::Button, FInputChord());	
	UI_COMMAND(CommandDrainPoint, "下水主管道", "DrainPoint", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandWitch1O2Point, "单联双控开关", "Witch1O2Point", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandWaterChilledPoint, "冷水点位", "WaterChilled", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandWaterHotPoint, "热水点位", "WaterHot", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandWaterSeparatorPoint, "分集水器", "WaterSeparator", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandWaterNormalPoint, "中水点位", "WaterNormal", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandBasin, "地漏下水", "WaterNormal", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CommandEnterRoomWater, "入户水点位", "EnterRoomWater", EUserInterfaceActionType::Button, FInputChord());


}

#undef LOCTEXT_NAMESPACE