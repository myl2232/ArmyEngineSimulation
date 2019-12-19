#include "ArmyPipePointActor.h"
#include "ArmyPipePoint.h"
#include "ArmyPipeLine.h"
#include "ArmyActorConstant.h"

AXRPipePointActor::AXRPipePointActor ()
{
	Tags.Add(XRActorTag::IgnoreCollision);
	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("AXRPipePointActor"));
	RootComponent = MeshComponent;
}

void AXRPipePointActor::BeginPlay ()
{
	Super::BeginPlay();
}

void AXRPipePointActor::Tick (float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRPipePointActor::EditorApplyTranslation (const FVector& DeltaTranslation,bool bAltDown,bool bShiftDown,bool bCtrlDown)
{
	Super::EditorApplyTranslation(DeltaTranslation,bAltDown,bShiftDown,bCtrlDown);
	if(GetRelevanceObject().IsValid ())
	{
		if(DelteaDelegate.IsBound ())
		{
			DelteaDelegate.ExecuteIfBound (DeltaTranslation);
		}
	}
}

void AXRPipePointActor::Destroy ()
{
	Super::Destroy();
}

void AXRPipePointActor::PostEditMove (bool bFinished)
{
	Super::PostEditMove (bFinished);
	if(bFinished)
	{
		if(PostEditDelegate.IsBound ())
		{
			TArray<TSharedPtr<FArmyObject>> Pointes;
			Pointes.Add (GetRelevanceObject().Pin ());
			PostEditDelegate.ExecuteIfBound(Pointes);
		}
	}
}
