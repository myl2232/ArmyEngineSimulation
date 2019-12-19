#include "ArmyWHCShapeChangeList.h"
#include "ArmyWHCShapeDoorChangeList.h"
#include "ArmyWHCCabinetChangeList.h"

IArmyWHCModeShapeChange* IArmyWHCModeShapeChange::CreateInstance(const FString &InType, FSCTShape *InShape)
{
    if (InType == SHAPE_CHANGE_SINGLE_DOOR_GROUP)
        return new FArmyWHCSingleDoorGroupChange(InShape);

    else if (InType == SHAPE_CHANGE_SINGLE_DRAWER_GROUP)
        return new FArmyWHCSingleDrawerGroupChange(InShape);

    else if (InType == SHAPE_CHANGE_CABINET_MTL)
        return new FArmyWHCCabinetMtlChange(InShape);

    else if (InType == SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL)
        return new FArmyWHCSingleDoorGroupMtlChange(InShape);

    else if (InType == SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL)
        return new FArmyWHCSingleDrawerGroupMtlChange(InShape);

    else if (InType == SHAPE_CHANGE_DOOR_OPEN_DIR)
        return new FArmyWHCDoorOpenDirChange(InShape);

    else if (InType == SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE)
        return new FArmyWHCSingleDoorGroupHandleChange(InShape);

    else if (InType == SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE)
        return new FArmyWHCSingleDrawerGroupHandleChange(InShape);

    else if (InType == SHAPE_CHANGE_CABINET_INSERTION_BOARD_MTL)
        return new FArmyWHCCabinetInsertionBoardMtlChange(InShape);

    else if (InType == SHAPE_CHANGE_FRONT_BOARD_CAB)
        return new FArmyWHCFrontBoardCabinetChange(InShape);
        
    else if (InType == SHAPE_CHANGE_EMBBED_ELEC_DEV)
        return new FArmyWHCEmbbedElecDevChange(InShape);

    else if (InType == SHAPE_CHANGE_DEL_EMBBED_ELEC_DEV)
        return new FArmyWHCDelEmbbedElecDevChange(InShape);

    else if (InType == SHAPE_CHANGE_SWARDROBE_BOTTOM_BOARD_VIS)
        return new FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange(InShape);

    else if (InType == SHAPE_CHANGE_SLIDINGWAY)
        return new FArmyWHCSlidingwayChange(InShape);
        
    else
        return nullptr;
}