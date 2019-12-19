// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyLightActor.h"
#include "Components/SphereComponent.h"
#include "Components/MaterialBillboardComponent.h"

AArmyLightActor::AArmyLightActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//StaticMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StaticMeshCOM"));
	SphereCOM = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereCOM"));

	RootComponent = SphereCOM;
	//StaticMeshComponent->SetupAttachment(RootComponent);
	SphereCOM->SetHiddenInGame(true);
	SphereCOM->SetVisibility(false);
	SphereCOM->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MaterialBillboardCOM = ObjectInitializer.CreateDefaultSubobject<UMaterialBillboardComponent>(this, TEXT("MaterialBillboardCOM"));
    MaterialBillboardCOM->AttachToComponent(SphereCOM, FAttachmentTransformRules::KeepRelativeTransform);
}

void AArmyLightActor::BeginPlay()
{
	Super::BeginPlay();
	//SphereMID = StaticMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	//SphereMID = UMaterialInstanceDynamic::Create(StaticMeshComponent->GetMaterial(0), GetWorld()->GetCurrentLevel());
	//StaticMeshComponent->SetMaterial(0, SphereMID);
}

void AArmyLightActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AArmyLightActor::SetLightColor(FLinearColor _Color)
{
	//if (SphereMID)
	//{  
	//	SphereMID->SetVectorParameterValue(TEXT("GizmoColor"), _Color);
	//}
}

void AArmyLightActor::OnSelected(bool _bSelected)
{
	SphereCOM->SetVisibility(_bSelected);
	if (_bSelected)
	{
	}
}

void AArmyLightActor::ShowLightIcon(bool bShow)
{
    MaterialBillboardCOM->SetVisibility(bShow);
}