// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyReflectionCaptureActor.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereReflectionCaptureComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "Components/DrawSphereComponent.h"
#include "Engine/CollisionProfile.h"

AArmyReflectionCaptureActor::AArmyReflectionCaptureActor(const FObjectInitializer& ObjectInitializer)
{
	ReflectionCaptureCOM = ObjectInitializer.CreateDefaultSubobject<USphereReflectionCaptureComponent>(this, TEXT("ReflectionCaptureCOM"));
	MatBillboardCOM = ObjectInitializer.CreateDefaultSubobject<UMaterialBillboardComponent>(this, TEXT("MatBillboardCOM"));
	BillboardCOM = ObjectInitializer.CreateDefaultSubobject<UBillboardComponent>(this, TEXT("BillboardCOM"));
	DrawCaptureRadius = CreateDefaultSubobject<UDrawSphereComponent>(TEXT("DrawRadius1"));

	BillboardCOM->bHiddenInGame = true;

	RootComponent = ReflectionCaptureCOM;
	MatBillboardCOM->SetupAttachment(RootComponent);
	BillboardCOM->SetupAttachment(RootComponent);
	ReflectionCaptureCOM->CaptureOffsetComponent = BillboardCOM;

	DrawCaptureRadius->SetupAttachment(ReflectionCaptureCOM);
	DrawCaptureRadius->bDrawOnlyIfSelected = true;
	DrawCaptureRadius->bUseEditorCompositing = true;
	DrawCaptureRadius->bHiddenInGame = false;
	DrawCaptureRadius->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	DrawCaptureRadius->ShapeColor = FColor(100, 90, 40);
	//ReflectionCaptureCOM->PreviewInfluenceRadius = DrawCaptureRadius;
}

void AArmyReflectionCaptureActor::SetRadius(float _Radius)
{
	DrawCaptureRadius->SetSphereRadius(_Radius);
	ReflectionCaptureCOM->InfluenceRadius = _Radius;
}

void AArmyReflectionCaptureActor::SetBrightness(float InBrightness)
{
	ReflectionCaptureCOM->Brightness = InBrightness;
	ReflectionCaptureCOM->RecreateRenderState_Concurrent();
}

void AArmyReflectionCaptureActor::ShowRCIcon(bool bShow)
{
    MatBillboardCOM->SetVisibility(bShow);
}