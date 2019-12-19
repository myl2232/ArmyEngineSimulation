#pragma once

#include "ArmyTypes.h"
#include "CoreMinimal.h"
#include "ArmyActor.h"
#include "ArmyFurnitureActor.generated.h"

UCLASS()
class ARMYSCENEDATA_API AXRFurnitureActor : public AXRActor
{
	GENERATED_BODY() 

public:
	AXRFurnitureActor ();
	virtual void BeginPlay ()override;
	virtual void Tick (float DeltaSeconds)override;
	virtual void PostInitializeComponents()override;
	virtual void EditorApplyTranslation (const FVector& DeltaTranslation,bool bAltDown,bool bShiftDown,bool bCtrlDown) override;
	void Destroy ();
	virtual void PostEditMove (bool bFinished) override;

	class UStaticMeshComponent* GetStandStaticMeshComponent () const
	{
		return StandStaticMeshComponent;
	}

	class UStaticMeshComponent* GetTransverseStaticMeshComponent () const
	{
		return TransverseStaticMeshComponent;
	}

	//class UMaterialBillboardComponent* GetMaterialBillboardComponent()const;

	void SetTransverse(bool bTransverse);

	void SetNormal (FVector& InNormal)
	{
		Normal = InNormal;
	}

	FVector GetNormal(){ return Normal; }
private:
	FVector Normal;
	UPROPERTY ()
	class UStaticMeshComponent* StandStaticMeshComponent;

	UPROPERTY ()
	class UStaticMeshComponent* TransverseStaticMeshComponent;

		/*	UPROPERTY (Category = StaticMeshActor,VisibleAnywhere,BlueprintReadOnly,meta = (ExposeFunctionCategories = "Mesh,Rendering,Physics,Components|StaticMesh",AllowPrivateAccess = "true"))
		class UMaterialBillboardComponent* BillboardComponent;*/
};