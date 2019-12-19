// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmyReflectionCaptureActor.generated.h"

UCLASS()
class ARMYENGINE_API AArmyReflectionCaptureActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ReflectionCapture)
	class USphereReflectionCaptureComponent* ReflectionCaptureCOM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LevelAssetBoard)
	class UMaterialBillboardComponent* MatBillboardCOM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ReflectionCapture)
	class UBillboardComponent* BillboardCOM;

	UPROPERTY()
	class UDrawSphereComponent* DrawCaptureRadius;

	AArmyReflectionCaptureActor(const FObjectInitializer& ObjectInitializer);

	void SetRadius(float _Radius);

	void SetBrightness(float InBrightness);

    /** 是否显示反射球图标 */
    void ShowRCIcon(bool bShow);
};
