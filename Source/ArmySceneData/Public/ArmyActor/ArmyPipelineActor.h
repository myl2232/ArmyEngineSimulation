#pragma once

#include "ArmyShapeBoardActor.h"
#include "ArmyTypes.h"
#include "ArmyPipelineActor.generated.h"

UCLASS()
class ARMYSCENEDATA_API AXRPipelineActor : public AXRShapeActor
{
	GENERATED_BODY()

public:
	AXRPipelineActor ();
	virtual void BeginPlay ()override;
	virtual void Tick (float DeltaSeconds)override;
	virtual void EditorApplyTranslation (const FVector& DeltaTranslation,bool bAltDown,bool bShiftDown,bool bCtrlDown) override;
	void Destroy ();
	void PostInitializeComponents () override;
	UFUNCTION()
	void BeginOverlap (UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult & SweepResult);
	UFUNCTION ()
	void EndOverlap (UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);
	virtual void PostEditMove (bool bFinished) override;

public:
	FVectorDelegate DeltaDelegate;
	FArmyObjectesDelegate PostEditDelegate;
};