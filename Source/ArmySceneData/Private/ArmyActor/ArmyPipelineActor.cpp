#include "ArmyPipelineActor.h"
#include "ArmyPipeline.h"
#include "ArmyActorConstant.h"

AXRPipelineActor::AXRPipelineActor ()
{
	Tags.Add(XRActorTag::IgnoreCollision);
	//bGenerateOverlapEventsDuringLevelStreaming = true;
	//GetMeshComponent()->bShouldUpdatePhysicsVolume = true;
	//GetMeshComponent()->SetCollisionProfileName ("BlockAllDynamic");
	//GetMeshComponent ()->SetCollisionEnabled (ECollisionEnabled::QueryAndPhysics);
	//GetMeshComponent ()->SetCollisionObjectType (ECC_WorldDynamic);
	//GetMeshComponent ()->SetCollisionResponseToChannel (ECC_GameTraceChannel2,ECR_Block);
	//GetMeshComponent ()->SetCollisionResponseToChannel (ECC_WorldDynamic,ECR_Overlap);
}

void AXRPipelineActor::BeginPlay ()
{
	Super::BeginPlay();
}

void AXRPipelineActor::Tick (float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRPipelineActor::EditorApplyTranslation (const FVector& DeltaTranslation,bool bAltDown,bool bShiftDown,bool bCtrlDown)
{
	Super::EditorApplyTranslation(DeltaTranslation,bAltDown,bShiftDown,bCtrlDown);
	if(GetRelevanceObject().IsValid ())
	{
		if (DeltaDelegate.IsBound())
		{
			DeltaDelegate.ExecuteIfBound(DeltaTranslation);
		}
	}
}

void AXRPipelineActor::Destroy ()
{
	Super::Destroy();
}

void AXRPipelineActor::PostInitializeComponents ()
{
	Super::PostInitializeComponents();
	//FScriptDelegate beginOverlap,endOverlap;
	//beginOverlap.BindUFunction (this,FName ("BeginOverlap"));
	//endOverlap.BindUFunction (this,FName ("EndOverlap"));

	//GetMeshComponent()->OnComponentBeginOverlap.Add (beginOverlap);
	//GetMeshComponent()->OnComponentEndOverlap.Add (endOverlap);
}

void AXRPipelineActor::BeginOverlap (UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult)
{
	//AXRPipelineActor* OtherPipeActor = Cast<AXRPipelineActor>(OtherActor);
	//if (OtherPipeActor && OtherPipeActor->IsValidLowLevel()&& OtherActor->IsA(AXRPipelineActor::StaticClass()))
	//{
	//	TSharedPtr<FArmyPipeline>  Pipeline = StaticCastSharedPtr<FArmyPipeline> (this->XROwner.Pin ());
	//	TSharedPtr<FArmyPipeline> OtherPipeline = StaticCastSharedPtr<FArmyPipeline>(OtherPipeActor->XROwner.Pin());
	//	if(Pipeline.IsValid() && OtherPipeline.IsValid())
	//		Pipeline->Intersect( OtherPipeline ,SweepResult.ImpactPoint );
	//}
	
}

void AXRPipelineActor::EndOverlap (UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{
	int a = 0;
}

void AXRPipelineActor::PostEditMove (bool bFinished)
{
	Super::PostEditMove(bFinished);
	if(bFinished)
	{
		if (PostEditDelegate.IsBound())
		{
			TSharedPtr<FArmyPipeline> PipeLine = StaticCastSharedPtr<FArmyPipeline> (GetRelevanceObject().Pin ());
			if (PipeLine->IsChanged)
			{
				TArray<TSharedPtr<FArmyObject>> Pointes;
				Pointes.Add(PipeLine->StartPoint);
				Pointes.Add(PipeLine->EndPoint);
				PostEditDelegate.ExecuteIfBound(Pointes);
				PipeLine->IsChanged = false;

				
			}
		}
	}
}
