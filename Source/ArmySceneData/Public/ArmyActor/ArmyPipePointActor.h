#pragma once

#include "ArmyShapeBoardActor.h"
#include "ArmyActor.h"
#include "ArmyTypes.h"
#include "ProceduralMeshComponent.h"
#include "ArmyPipePointActor.generated.h"

UCLASS()
class ARMYSCENEDATA_API AXRPipePointActor : public AXRShapeActor
{
	GENERATED_BODY()

public:
	AXRPipePointActor ();
	virtual void BeginPlay ()override;
	virtual void Tick (float DeltaSeconds)override;
	virtual void EditorApplyTranslation (const FVector& DeltaTranslation,bool bAltDown,bool bShiftDown,bool bCtrlDown) override;
	void Destroy ();
	virtual void PostEditMove (bool bFinished) override;
public:
	FVectorDelegate DelteaDelegate;
	FArmyObjectesDelegate PostEditDelegate;
	//class USplineMeshComponent* SplineMeshComponet;
	UPROPERTY()
	class UProceduralMeshComponent* MeshComponent;
};