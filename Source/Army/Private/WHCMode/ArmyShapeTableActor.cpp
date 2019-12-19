#include "ArmyShapeTableActor.h"
#include "TypedPrimitive.h"
#include "Engine/CollisionProfile.h"
#include "Components/StaticMeshComponent.h"
#include "ArmyActorConstant.h"

AXRShapeTableActor::AXRShapeTableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>("ArmyShapeTableRoot");

    mPrimitive = CreateDefaultSubobject<UPNTUVPrimitive>("ArmyShapeTable");
	// mPrimitive->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	RootComponent = mPrimitive;

	Tags.AddUnique(XRActorTag::WHCActor);
}

void AXRShapeTableActor::RefreshMesh(const TArray<FPUVVertex>& Vertices, const TArray<uint16>& Indices )
{
	if (Vertices.Num() > 0 && Indices.Num() > 0)
		mPrimitive->SetPrimitiveInfo(Vertices, Indices);
}

void AXRShapeTableActor::SetMaterial(UMaterialInterface* pMaterial)
{
	if (pMaterial)
		mPrimitive->SetMaterial(0, pMaterial);
}

void AXRShapeTableActor::SetIsSelected(bool bSelected)
{ 
	bOverrideSelectionFlag = bSelected; 
	mPrimitive->PushSelectionToProxy();
}

AXRWhcGapActor::AXRWhcGapActor()
{
	bIsSelectable = 0;
	Tags.AddUnique(XRActorTag::WHCActor);
}

void AXRWhcGapActor::SetIsSelected(bool bSelected)
{ 
	bOverrideSelectionFlag = bSelected; 
	GetStaticMeshComponent()->PushSelectionToProxy();
}

AXRElecDeviceActor::AXRElecDeviceActor()
{
	bIsSelectable = 0;
	Tags.AddUnique(XRActorTag::WHCActor);
}

void AXRElecDeviceActor::SetIsSelected(bool bSelected)
{ 
	bOverrideSelectionFlag = bSelected; 
	GetStaticMeshComponent()->PushSelectionToProxy();
}