// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ArmyLightActor.h"
#include "ArmySpotLightActor.generated.h"

/**
 * 
 */
UCLASS()
class ARMYENGINE_API AArmySpotLightActor : public AArmyLightActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light)
		class USpotLightComponent* SpotLightCOM;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light)
	//	UStaticMeshComponent* ArrowCOM;

	AArmySpotLightActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetLightColor(FLinearColor _Color) override;
	virtual void OnSelected(bool _bSelected) override;

	UMaterialInstanceDynamic* ArrowMID;
};
