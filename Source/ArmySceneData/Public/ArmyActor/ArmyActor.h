#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmyActor.generated.h"

#define ECC_BIMBASEWALL		ECC_GameTraceChannel3

UCLASS()
class ARMYSCENEDATA_API AXRActor :public AActor
{
	GENERATED_BODY()

public:
	AXRActor() {};
	void SetRelevanceObject(TWeakPtr<class FArmyObject> InObj);
	TWeakPtr<class FArmyObject> GetRelevanceObject();
private:
	TWeakPtr<class FArmyObject> RelevanceObject;
};