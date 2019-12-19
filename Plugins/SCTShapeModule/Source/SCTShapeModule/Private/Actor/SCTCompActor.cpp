#include "SCTCompActor.h"
#include "SCTShapeActor.h"
#include "SCTBoardActor.h"
#include "SCTModelActor.h"
#include "SCTShape.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "SCTBaseShape.h"
#include "SCTBoardShape.h"
#include "SCTModelShape.h"

#define CURVE_STEP 128

ASCTCompActor::ASCTCompActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
}

void ASCTCompActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASCTCompActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ASCTCompActor::UpdateActorDimension()
{
	ASCTShapeActor::UpdateActorDimension();
// 	TArray<AActor*> AttachedActors;
// 	GetAttachedActors(AttachedActors);
// 	for (int32 i = 0; i < AttachedActors.Num(); ++i)
// 	{
// 		ASCTShapeActor* CurShapeActor = Cast<ASCTShapeActor>(AttachedActors[i]);
// 		if (CurShapeActor)
// 		{
// 			CurShapeActor->UpdateActorDimension();
// 		}
// 	}
}
void ASCTCompActor::UpdateActorPosition()
{
	ASCTShapeActor::UpdateActorPosition();
}
void ASCTCompActor::UpdateActorRotation()
{
	ASCTShapeActor::UpdateActorRotation();
}
