#include "ArmyCrownMouldingExtruder.h"
#include "ArmyBaseArea.h"

FString FArmyCrownMouldingExtruder::GetUniqueCode()
{
    return SurfaceArea.IsValid() ? TEXT("RoofExtrusion") + SurfaceArea->GetUniqueID().ToString() : TEXT("RoofExtrusion");
}
