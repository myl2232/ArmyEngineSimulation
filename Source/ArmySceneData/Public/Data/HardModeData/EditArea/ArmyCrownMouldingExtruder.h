#pragma once

#include "ArmyExtruder.h"

class FArmyCrownMouldingExtruder : public FArmyExtruder
{
public:
    FArmyCrownMouldingExtruder(UWorld* InWorld, TSharedPtr<FArmyBaseArea> InSurfaceArea, TSharedPtr<FArmyRoomSpaceArea> InRoomSpaceArea)
        : FArmyExtruder(InWorld, InSurfaceArea, InRoomSpaceArea)
    {}

protected:
    virtual FString GetUniqueCode() override;
    virtual FName GetFolderPath() override { return FArmyActorPath::GetRoofPath(); }
    virtual bool NeedReverse() override { return true; }
};