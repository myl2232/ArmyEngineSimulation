// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ArmyLightActor.generated.h"

UCLASS()
class ARMYENGINE_API AArmyLightActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AArmyLightActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light)
	class USphereComponent* SphereCOM;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Light)
    class UMaterialBillboardComponent* MaterialBillboardCOM;

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	virtual void SetLightColor(FLinearColor _Color);
	virtual void OnSelected(bool _bSelected);
	virtual bool UpdateData() { return false; }

    /** �Ƿ���ʾ��Դͼ�� */
    void ShowLightIcon(bool bShow);

	//UPROPERTY()
	//	UMaterialInstanceDynamic* SphereMID;
};
