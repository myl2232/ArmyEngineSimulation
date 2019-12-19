#pragma once

#include "ArmyExtruder.h"

class FArmyBaseboardExtruder : public FArmyExtruder
{
public:
    FArmyBaseboardExtruder(UWorld* InWorld, TSharedPtr<FArmyBaseArea> InSurfaceArea, TSharedPtr<FArmyRoomSpaceArea> InRoomSpaceArea)
        : FArmyExtruder(InWorld, InSurfaceArea, InRoomSpaceArea)
    {}

protected:
    virtual FString GetUniqueCode() override;
    virtual FName GetFolderPath() override { return FArmyActorPath::GetFloorPath(); }
};