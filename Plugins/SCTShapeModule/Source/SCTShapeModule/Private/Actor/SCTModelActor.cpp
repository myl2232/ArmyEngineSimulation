#include "SCTModelActor.h"
#include "SCTShape.h"
#include "SCTShapeManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/BodySetup.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/CollisionProfile.h"
#include "SCTModelShape.h"

ASCTModelActor::ASCTModelActor()
: ModelMesh(nullptr)
, SkeletalModelMesh(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	//bHasChildren = false;
}

void ASCTModelActor::BeginPlay()
{
	Super::BeginPlay();
}

void ASCTModelActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	/*if (bDelayReBuildBoundBox)
	{
		PrimaryActorTick.bCanEverTick = false;
		bDelayReBuildBoundBox = false;
		DelayReBuildBoundBox();		
	}*/
}

FBox ASCTModelActor::GetCollisionBox() const
{
	FBox CurrentBBox(ForceInitToZero);
	if (ModelMesh != nullptr)
	{
		FKAggregateGeom& agg = ModelMesh->GetBodySetup()->AggGeom;
		CurrentBBox = agg.CalcAABB(ModelMesh->GetComponentToWorld());
	}
	else if (SkeletalModelMesh != nullptr)
	{
		FKAggregateGeom& agg = SkeletalModelMesh->GetBodySetup()->AggGeom;
		CurrentBBox = agg.CalcAABB(SkeletalModelMesh->GetComponentToWorld());
	}

	return CurrentBBox;
}

void ASCTModelActor::UpdateActorDimension()
{
	const float XScaleFactor = (ScaleFilter &= ModelSacleFilter::SCALE_X) ? ShapeData->GetShapeWidth() / MeshWidth : 1.0f;
	const float YScaleFactor = (ScaleFilter &= ModelSacleFilter::SCALE_Y) ? ShapeData->GetShapeDepth() / MeshDepth : 1.0f;
	const float ZScaleFactor = (ScaleFilter &= ModelSacleFilter::SCALE_Z) ? ShapeData->GetShapeHeight() / MeshHeight : 1.0f;
	
	SetActorScale3D(FVector(XScaleFactor, YScaleFactor, ZScaleFactor));
}


void ASCTModelActor::ChangeMaterial(UMaterialInterface * InMaterial)
{
	FSCTShape * BindingShape = GetShape();
	if (!BindingShape) return;	
	if (ModelMesh)
	{
		for (int32 Index = 0; Index < ModelMesh->GetNumMaterials(); ++Index)
		{
			ModelMesh->SetMaterial(Index, InMaterial);
		}
	}
}

FVector ASCTModelActor::GetModelMeshSize()
{
	return FVector(MeshWidth, MeshDepth, MeshHeight);
}
void ASCTModelActor::SetFixedModelMeshCenterSize(const FVector & InCenter, const FVector & InSize)
{
	MeshWidth = InSize.X;
	MeshDepth = InSize.Y;
	MeshHeight = InSize.Z;	
	MeshCenter = InCenter;
}

void ASCTModelActor::SetStaticMesh(UStaticMesh* InStaticMesh, const TArray<TPair<FString, UMaterialInstanceDynamic*>> & InMaterialList)
{
	if (ModelMesh == nullptr)
	{
		ModelMesh = NewObject<UStaticMeshComponent>(this);
		ModelMesh->SetMobility(EComponentMobility::Movable);
		ModelMesh->RegisterComponent();	
		ModelMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		RootComponent = ModelMesh;
	}
	ModelMesh->SetStaticMesh(InStaticMesh);

	FVector SMBoxSize = InStaticMesh->GetBoundingBox().GetSize();
	MeshWidth = SMBoxSize.X * 10;
	MeshDepth = SMBoxSize.Y * 10;
	MeshHeight = SMBoxSize.Z * 10;
	MeshCenter = InStaticMesh->GetBoundingBox().GetCenter() * 10.0f;

	SetMaterialList(InMaterialList, false);
}
void ASCTModelActor::SetMaterialList(const TArray<TPair<FString, UMaterialInstanceDynamic*>> & InMaterialList, const bool InIsSkeletalMesh)
{
	UMeshComponent * MeshCompoent = nullptr;
	if (InIsSkeletalMesh)
	{
		MeshCompoent = Cast<UMeshComponent>(SkeletalModelMesh);		
	}
	else
	{
		MeshCompoent = Cast<UMeshComponent>(ModelMesh);
	}
	if (MeshCompoent)
	{
		for (const auto & IterRef : InMaterialList)
		{									
			MeshCompoent->SetMaterialByName(FName(*(IterRef.Key)),Cast<UMaterialInterface>(IterRef.Value));
		}		
	}	
}
void ASCTModelActor::SetSkeletalMesh(USkeletalMesh* InSkeletalMesh, UAnimationAsset *InAnimationAsset, const TArray<TPair<FString, UMaterialInstanceDynamic*>> & InMaterialList)
{
	if (SkeletalModelMesh == nullptr)
	{
		SkeletalModelMesh = NewObject<USkeletalMeshComponent>(this);
		SkeletalModelMesh->SetMobility(EComponentMobility::Movable);
		SkeletalModelMesh->RegisterComponent();
		SkeletalModelMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
		RootComponent = SkeletalModelMesh;
	}
	SkeletalModelMesh->SetSkeletalMesh(InSkeletalMesh);	
	SkeletalModelMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	SkeletalModelMesh->SetAnimation(InAnimationAsset);		
	FVector SMBoxSize = InSkeletalMesh->GetImportedBounds().GetBox().GetSize();	
	MeshWidth = SMBoxSize.X * 10;
	MeshDepth = SMBoxSize.Y * 10;
	MeshHeight = SMBoxSize.Z * 10;
	MeshCenter = InSkeletalMesh->GetImportedBounds().GetBox().GetCenter() * 10.0f;
	SetMaterialList(InMaterialList, true);
	bDelayReBuildBoundBox = true;
}

void ASCTModelActor::PlayAnimation(bool bForward)
{
	if (SkeletalModelMesh != nullptr)
	{
		//SkeletalModelMesh->SetPlayRate(bForward ? 1.0f : -1.0f);
		SkeletalModelMesh->GetSingleNodeInstance()->SetReverse(!bForward);
		SkeletalModelMesh->Play(false);
	}
}

bool ASCTModelActor::IsAnimationPlaying() const
{
	return SkeletalModelMesh != nullptr ? SkeletalModelMesh->IsPlaying() : false;
}

float ASCTModelActor::GetAnimationLength() const
{
	return SkeletalModelMesh != nullptr ? SkeletalModelMesh->GetSingleNodeInstance()->GetLength() : 0.0f;
}

UMaterialInterface * ASCTModelActor::GetSingleMaterial()
{
	UMaterialInterface * ToRetMaterial = nullptr;
	do
	{	
		if (ModelMesh)
		{
			ToRetMaterial = ModelMesh->GetMaterial(0);
			break;
		}
		if (SkeletalModelMesh)
		{
			ToRetMaterial = SkeletalModelMesh->GetMaterial(0);
			break;
		}

	} while (false);
	return ToRetMaterial;
}

void ASCTModelActor::SetSingleMaterialOffsetAndRepeatParameter(const float InOffsetU, const float InOffsetV, const float InRepeatU, const float InRepeatV)
{
	UMaterialInterface * Material = GetSingleMaterial();
	UMaterialInstanceDynamic * DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
	if (!DynamicMaterial) return;

	auto SetScalarParameterValue = [DynamicMaterial](const FName & InName,const float InValue)
	{			
		float ParaVaule = 0.0f;
		if (DynamicMaterial->GetScalarParameterValue(InName, ParaVaule))
		{
			DynamicMaterial->SetScalarParameterValue(InName, InValue);
		}
	};
	SetScalarParameterValue(FName(TEXT("Diffuse_UV_OffsetU")), InOffsetU);
	SetScalarParameterValue(FName(TEXT("Diffuse_UV_OffsetV")), InOffsetV);
	SetScalarParameterValue(FName(TEXT("NormalMap_UV_Offset_U")), InOffsetU);
	SetScalarParameterValue(FName(TEXT("NormalMap_UV_Offset_V")), InOffsetV);
	SetScalarParameterValue(FName(TEXT("Diffuse_UV_TilingU")), InRepeatU);
	SetScalarParameterValue(FName(TEXT("Diffuse_UV_TilingV")), InRepeatV);
	SetScalarParameterValue(FName(TEXT("NormalMap_UV_Tiling_U")), InRepeatU);
	SetScalarParameterValue(FName(TEXT("NormalMap_UV_Tiling_V")), InRepeatV);
}


void ASCTModelActor::SetSingleMaterialRotateParameter(const float InDegree)
{
	UMaterialInterface * Material = GetSingleMaterial();
	UMaterialInstanceDynamic * DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
	if (!DynamicMaterial) return;

	auto SetScalarParameterValue = [DynamicMaterial](const FName & InName, const float InValue)
	{
		float ParaVaule = 0.0f;
		if (DynamicMaterial->GetScalarParameterValue(InName, ParaVaule))
		{
			DynamicMaterial->SetScalarParameterValue(InName, InValue);
		}
	};	
	// 此名称一定要与DLC common包中的材质参数保持一致
	SetScalarParameterValue(FName(TEXT("NormalMap_UV_Ratote")), InDegree);
	SetScalarParameterValue(FName(TEXT("Diffuse_UV_Rotate")), InDegree);	
}
// int32 ASCTModelActor::GetNumChild() const
// {
// 	return RootComponent->GetNumChildrenComponents();
// }

// UStaticMeshComponent* ASCTModelActor::GetChildSMC(int32 Index) const
// {
// 	return Cast<UStaticMeshComponent>(RootComponent->GetChildComponent(Index));
// }
