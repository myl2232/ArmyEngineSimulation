
#include "ArmyWHTextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AXRWHTextRenderActor::AXRWHTextRenderActor(const FObjectInitializer & ObjectInitializer) : Super (ObjectInitializer)
{
	//将这个字符设置为每帧调用Tick()。如果不需要，可以关闭此选项来提高性能。
	PrimaryActorTick.bCanEverTick = true;
}

void AXRWHTextRenderActor::BeginPlay()
{
	Super::BeginPlay();
}

void AXRWHTextRenderActor::PostActorCreated()
{
	Super::PostActorCreated();

	FVector Location = GetActorLocation();
	FRotator LookAtRot = GetActorRotation();
	SetActorRotation(LookAtRot + FRotator(0, 90, 0));
	GetTextRender()->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	GetTextRender()->SetWorldSize(14);
}

void AXRWHTextRenderActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AXRWHTextRenderActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Timer += DeltaSeconds;
	if (Timer >= ShowTextMaxTime) {
		Destroy();
	}
}

void AXRWHTextRenderActor::SetColor(FColor textColor) {
	GetTextRender()->SetTextRenderColor(textColor);
}

void AXRWHTextRenderActor::SetText(const FString & TextString)
{
	GetTextRender()->SetText(FText::FromString(TextString));
}
 
void AXRWHTextRenderActor::SetTextRelativeLocation(FVector& relativeLoc) {
	SetActorRelativeLocation(relativeLoc);
}
