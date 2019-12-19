#include "ArmyGameMode.h"
#include "ArmyPlayerController.h"
#include "ArmyPawn.h"

AXRGameMode::AXRGameMode(const FObjectInitializer &ObjectInitializer)
{
	PlayerControllerClass = AXRPlayerController::StaticClass();
	DefaultPawnClass = AXRPawn::StaticClass();
}
