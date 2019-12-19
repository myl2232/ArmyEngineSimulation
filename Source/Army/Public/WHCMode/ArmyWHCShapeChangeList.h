#pragma once
#include "CoreMinimal.h"
#include "Json.h"

#define SHAPE_CHANGE_SINGLE_DOOR_GROUP TEXT("SingleDoorGroup")
#define SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL TEXT("SingleDoorGroupMtl")
#define SHAPE_CHANGE_SINGLE_DRAWER_GROUP TEXT("SingleDrawerGroup")
#define SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL TEXT("SingleDrawerGroupMtl")
#define SHAPE_CHANGE_CABINET_MTL TEXT("CabinetMtl")
#define SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE TEXT("SingleDoorGroupHandle")
#define SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE TEXT("SingleDrawerGroupHandle")
#define SHAPE_CHANGE_DOOR_OPEN_DIR TEXT("DoorOpenDir")
#define SHAPE_CHANGE_CABINET_INSERTION_BOARD_MTL TEXT("CabinetInsertionBoardMtl")
#define SHAPE_CHANGE_FRONT_BOARD_CAB TEXT("FrontBoardCabinet")
#define SHAPE_CHANGE_EMBBED_ELEC_DEV TEXT("EmbbedElecDev")
#define SHAPE_CHANGE_DEL_EMBBED_ELEC_DEV TEXT("DelEmbbedElecDev")
#define SHAPE_CHANGE_SWARDROBE_BOTTOM_BOARD_VIS TEXT("SWardrobeBottomBoardVis")
#define SHAPE_CHANGE_SLIDINGWAY TEXT("Slidingway")

class IArmyWHCModeShapeChange
{
public:
    IArmyWHCModeShapeChange(class FSCTShape *InShape) : Shape(InShape) {}
    virtual ~IArmyWHCModeShapeChange() {}
    virtual void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) = 0;
    virtual void Deserialize(const TSharedPtr<FJsonObject>& JsonObject) = 0;
    virtual void ProcessChange() = 0;
    virtual bool IsSameType(const FString &InType) const { return false; }

    static IArmyWHCModeShapeChange* CreateInstance(const FString &InType, class FSCTShape *InShape);

protected:
    class FSCTShape *Shape;
};