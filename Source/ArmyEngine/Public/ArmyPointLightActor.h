// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ArmyLightActor.h"
#include "ArmyPointLightActor.generated.h"


UCLASS()
class ARMYENGINE_API AArmyPointLightActor : public AArmyLightActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light)
		class UPointLightComponent* PointLightCOM;

	AArmyPointLightActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetLightColor(FLinearColor _Color) override;
	virtual void OnSelected(bool _bSelected) override;

};
