#include "ArmyBaseboardExtruder.h"
#include "ArmyBaseArea.h"

FString FArmyBaseboardExtruder::GetUniqueCode()
{
    return SurfaceArea.IsValid() ? TEXT("FloorExtrusion") + SurfaceArea->GetUniqueID().ToString() : TEXT("FloorExtrusion");
}
