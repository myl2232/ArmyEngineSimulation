// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmySpotLightActor.h"
#include "Components/SpotLightComponent.h"
#include "Components/SphereComponent.h"


AArmySpotLightActor::AArmySpotLightActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SpotLightCOM = ObjectInitializer.CreateDefaultSubobject<USpotLightComponent>(this, TEXT("SpotLightCOM"));
	SpotLightCOM->SetupAttachment(RootComponent);
	SpotLightCOM->ShadowBias = 0.15f;
	SpotLightCOM->SetCastShadows(false);

	SphereCOM->SetVisibility(false);
	//ArrowCOM = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ArrowCOM"));
	//ArrowCOM->SetupAttachment(RootComponent);
}

void AArmySpotLightActor::BeginPlay()
{
	Super::BeginPlay();
	//ArrowMID = ArrowCOM->CreateAndSetMaterialInstanceDynamic(0);

	FLinearColor TempColor = SpotLightCOM->GetLightColor();
	TempColor.A = 1;

	//if (SphereMID)
	//{
	//	SphereMID->SetVectorParameterValue(TEXT("GizmoColor"), TempColor);
	//}
	//if (ArrowMID)
	//{
	//	ArrowMID->SetVectorParameterValue(TEXT("GizmoColor"), TempColor);
	//}
	//ArrowCOM->SetHiddenInGame(true);
}

void AArmySpotLightActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AArmySpotLightActor::SetLightColor(FLinearColor _Color)
{
	Super::SetLightColor(_Color);
	SpotLightCOM->SetLightColor(_Color);
	//if (ArrowMID)
	//{
	//	ArrowMID->SetVectorParameterValue(TEXT("GizmoColor"), _Color);
	//}
}

void AArmySpotLightActor::OnSelected(bool _bSelected)
{
	//ArrowCOM->SetVisibility(_bSelected);
	if (_bSelected)
	{
		float NewScale = SpotLightCOM->AttenuationRadius / 114.f;
		FVector NewScale3D;
		NewScale3D.X = NewScale;
		NewScale3D.Y = 1;
		NewScale3D.Z = 1;
		//ArrowCOM->SetWorldScale3D(NewScale3D);
	}
}

