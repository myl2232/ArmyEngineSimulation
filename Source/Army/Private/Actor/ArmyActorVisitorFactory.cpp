#include "ArmyActorVisitorFactory.h"
#include "ArmyActorVisitor.h"

TSharedPtr<FArmyActorVisitorFactory> FArmyActorVisitorFactory::Instance = nullptr;

FArmyActorVisitorFactory& FArmyActorVisitorFactory::Get()
{
    if (!Instance.IsValid())
    {
        Instance = MakeShareable(new FArmyActorVisitorFactory());
    }
    return *Instance;
}

TSharedPtr<IArmyActorVisitor> FArmyActorVisitorFactory::CreateVisitor(EXRActorVisitor ActorVisitor)
{
    TSharedPtr<IArmyActorVisitor> Result = nullptr;

    switch (ActorVisitor)
    {
    case AV_HardMode:
        Result = MakeShareable(new FArmyHardModeActorVisitor);
        break;
    case AV_HardSubMode:
        Result = MakeShareable(new FArmyHardSubModeActorVisitor);
        break;
    case AV_HydropowerMode:
        Result = MakeShareable(new FArmyHydropowerModeActorVisitor);
        break;
    case AV_WHCMode:
        Result = MakeShareable(new FArmyWHCModeActorVisitor);
        break;
    case AV_Model:
        Result = MakeShareable(new FArmyModelActorVisitor);
        break;
    case AV_TopHardMode:
        Result = MakeShareable(new FArmyTopHardModeActorVisitor);
		break;
    case AV_TopHydropowerMode:
        Result = MakeShareable(new FArmyTopHydropowerModeActorVisitor);
        break;
	case AV_HardModeCeilingMode:
		Result = MakeShareable(new FArmyHardModeCeilingModeActorVisitor);
		break;
    case AV_OriginalPoint:
        Result = MakeShareable(new FArmyOriginalPointVisitor);
		break;
	case AV_OuterWall:
		Result = MakeShareable(new FArmyOuterWallVisitor);
		break;
	case AV_OriginalWall:
		Result = MakeShareable(new FArmyOriginalWallVisitor);
		break;
	case AV_WHCActor:
		Result = MakeShareable(new FArmyWHCActorVisitor);
		break;
	case AV_TranslucentRoofActor:
		Result = MakeShareable(new FArmyTranslucentRoofActorVisitor);
		break;
    case AV_HardwareActor:
        Result = MakeShareable(new FArmyHardwareActorVisitor);
        break;
    case AV_LightIcon:
        Result = MakeShareable(new FArmyLightIconVisitor);
        break;
    default:
        break;
    }

    if (Result.IsValid())
    {
        Result->Refresh();
    }

    return Result;
}
