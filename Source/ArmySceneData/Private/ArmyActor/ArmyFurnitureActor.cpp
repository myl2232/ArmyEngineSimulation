#include "ArmyFurnitureActor.h"
#include "ArmyFurniture.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MaterialBillboardComponent.h"
#include "Engine/StaticMesh.h"

AXRFurnitureActor::AXRFurnitureActor ()
{
	//StandStaticMeshComponent= CreateDefaultSubobject<UStaticMeshComponent> (TEXT ("ArmyFurnitureActor"));
	//StandStaticMeshComponent->Mobility = EComponentMobility::Movable;
	//StandStaticMeshComponent->bGenerateOverlapEvents = false;
	//StandStaticMeshComponent->bUseDefaultCollision = true;

	//RootComponent =StandStaticMeshComponent; 


	//TransverseStaticMeshComponent=CreateDefaultSubobject<UStaticMeshComponent> (TEXT ("ArmyFurnitureActor1"));
	//TransverseStaticMeshComponent->Mobility=EComponentMobility::Movable;
	//TransverseStaticMeshComponent->bGenerateOverlapEvents=false;
	//TransverseStaticMeshComponent->bUseDefaultCollision=true;

	//TransverseStaticMeshComponent->SetupAttachment(RootComponent);

	//FRotator Rotator(0.f,0.0,-90.0);
	//TransverseStaticMeshComponent->SetRelativeRotation(Rotator);

	/*BillboardComponent = CreateDefaultSubobject<UMaterialBillboardComponent>(TEXT("ArmyBillboard"));
	BillboardComponent->SetupAttachment(RootComponent);*/

}

void AXRFurnitureActor::BeginPlay ()
{
	Super::BeginPlay();
}

void AXRFurnitureActor::Tick (float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AXRFurnitureActor::EditorApplyTranslation (const FVector& DeltaTranslation,bool bAltDown,bool bShiftDown,bool bCtrlDown)
{
	Super::EditorApplyTranslation(DeltaTranslation,bAltDown,bShiftDown,bCtrlDown);
	/*if(XROwner.IsValid ())
	{
		TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture>(XROwner.Pin());
		FVector location = furniture->GetBasePos() + DeltaTranslation;
		FVector LocationZ = FVector(location.X,location.Y,0);
		float Altitude = furniture->GetAltitude() + DeltaTranslation.Z;
		furniture->SetAltitude(Altitude);
		furniture->SetPosition (LocationZ);
		furniture->UpdateData();
	}*/
}

void AXRFurnitureActor::Destroy ()
{
	Super::Destroy();
}

void AXRFurnitureActor::PostInitializeComponents ()
{
	Super::PostInitializeComponents();
}

void AXRFurnitureActor::PostEditMove (bool bFinished)
{
	Super::PostEditMove(bFinished);
	if(bFinished)
	{
		
	}
}
//
//class UMaterialBillboardComponent* AXRFurnitureActor::GetMaterialBillboardComponent () const
//{
//	return BillboardComponent;
//}

void AXRFurnitureActor::SetTransverse (bool bTransverse)
{
	//if(XROwner.IsValid ())
	//{
	//	TSharedPtr<FArmyFurniture> furniture = StaticCastSharedPtr<FArmyFurniture> (XROwner.Pin ());
	//	furniture->SetTransverse(bTransverse);
	//	furniture->UpdateData ();
	//}
}

//bool AXRFurnitureActor::SetActorLocation(const FVector& NewLocation,bool bSweep /*= false*/,FHitResult* OutSweepHitResult /*= nullptr*/,ETeleportType Teleport /*= ETeleportType::None*/)
//{
//	TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(GetRelevanceObject().Pin());
//	if (Furniture.IsValid())
//	{
//		Furniture->SetAltitude (NewLocation.Z);
//		Furniture->UpdateData ();
//	}
//	return Super::SetActorLocation(NewLocation,bSweep,OutSweepHitResult,Teleport);
//}
