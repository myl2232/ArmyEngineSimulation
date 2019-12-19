// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ArmyLevelAssetBoardActor.generated.h"

UCLASS()
class ARMYENGINE_API AArmyLevelAssetBoardActor : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LevelAssetBoard)
		USceneComponent* RootCOM;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LevelAssetBoard)
		class UMaterialBillboardComponent* MatBillboardCOM;

	AArmyLevelAssetBoardActor(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	void SetVisibility(bool _bVisible);
	void UpdateData(FText _Title, FText _Des0, FText _Des1, FText _Des2, FText _Des3, FText _Des4);

	FText Tilte;
	FText Des0;
	FText Des1;
	FText Des2;
	FText Des3;
	FText Des4;
};
