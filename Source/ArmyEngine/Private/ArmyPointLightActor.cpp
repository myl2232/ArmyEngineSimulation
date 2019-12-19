// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyPointLightActor.h"
#include "Components/PointLightComponent.h"

AArmyPointLightActor::AArmyPointLightActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PointLightCOM = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, TEXT("PointLightCOM"));
	PointLightCOM->SetupAttachment(RootComponent);
	PointLightCOM->ShadowBias = 0.15f;
	PointLightCOM->SetCastShadows(false);
}

void AArmyPointLightActor::BeginPlay()
{
	Super::BeginPlay();
	//if (SphereMID)
	//{
	//	FLinearColor TempColor = PointLightCOM->GetLightColor();
	//	TempColor.A = 1;
	//	SphereMID->SetVectorParameterValue(TEXT("GizmoColor"), TempColor);
	//}
}

void AArmyPointLightActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void  AArmyPointLightActor::SetLightColor(FLinearColor _Color)
{
	Super::SetLightColor(_Color);
	PointLightCOM->SetLightColor(_Color);
}

void AArmyPointLightActor::OnSelected(bool _bSelected)
{
	Super::OnSelected(_bSelected);
	if (_bSelected)
	{
		//SphereCOM->SetSphereRadius(PointLightCOM->AttenuationRadius);
	}
}


