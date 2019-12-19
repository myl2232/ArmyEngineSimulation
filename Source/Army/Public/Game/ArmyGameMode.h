#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArmyGameMode.generated.h"

UCLASS()
class XR_API AXRGameMode : public AGameModeBase {

	GENERATED_BODY()

public:
	AXRGameMode(const FObjectInitializer &ObjectInitializer);
};