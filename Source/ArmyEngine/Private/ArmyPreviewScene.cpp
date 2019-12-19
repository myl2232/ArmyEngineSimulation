// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyPreviewScene.h"
#include "SoundDefinitions.h"
#include "FXSystem.h"
#include "Components/MeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/LineBatchComponent.h"

FArmyPreviewScene::FArmyPreviewScene(FArmyPreviewScene::ConstructionValues CVS)
: PreviewWorld(NULL)
, bForceAllUsedMipsResident(CVS.bForceMipsResident)
{
	EObjectFlags NewObjectFlags = RF_NoFlags;
	if (CVS.bTransactional)
	{
		NewObjectFlags = RF_Transactional;
	}

	PreviewWorld = NewObject<UWorld>(GetTransientPackage(), NAME_None, NewObjectFlags);
	PreviewWorld->WorldType = EWorldType::Game;

	FWorldContext& WorldContext = GEngine->CreateNewWorldContext(PreviewWorld->WorldType);
	WorldContext.SetCurrentWorld(PreviewWorld);

	//PreviewWorld->SetGameInstance(GEngine->GameViewport->GetGameInstance());

	PreviewWorld->InitializeNewWorld(UWorld::InitializationValues()
		.AllowAudioPlayback(CVS.bAllowAudioPlayback)
		.CreatePhysicsScene(CVS.bCreatePhysicsScene)
		.RequiresHitProxies(false)
		.CreateNavigation(false)
		.CreateAISystem(false)
		.ShouldSimulatePhysics(CVS.bShouldSimulatePhysics)
		.SetTransactional(CVS.bTransactional));
	PreviewWorld->InitializeActorsForPlay(FURL());

	//GetScene()->UpdateDynamicSkyLight(FLinearColor::White * CVS.SkyBrightness, FLinearColor::Black);

	DirectionalLight = NewObject<UDirectionalLightComponent>(GetTransientPackage());
	DirectionalLight->Intensity = CVS.LightBrightness;
	DirectionalLight->LightColor = FColor::White;
	AddComponent(DirectionalLight, FTransform(CVS.LightRotation));

	SkyLight = NewObject<USkyLightComponent>(GetTransientPackage(), NAME_None, RF_Transient);
	SkyLight->bLowerHemisphereIsBlack = false;
	SkyLight->SourceType = ESkyLightSourceType::SLS_SpecifiedCubemap;
	SkyLight->Intensity = CVS.SkyBrightness;
	SkyLight->Mobility = EComponentMobility::Movable;
	AddComponent(SkyLight, FTransform::Identity);

	LineBatcher = NewObject<ULineBatchComponent>(GetTransientPackage());
	LineBatcher->bCalculateAccurateBounds = false;
	AddComponent(LineBatcher, FTransform::Identity);
}

FArmyPreviewScene::~FArmyPreviewScene()
{
	// Stop any audio components playing in this scene
	if (GEngine)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			if (FAudioDevice* AudioDevice = World->GetAudioDevice())
			{
				AudioDevice->Flush(GetWorld(), false);
			}
		}
	}

	// Remove all the attached components
	for (int32 ComponentIndex = 0; ComponentIndex < Components.Num(); ComponentIndex++)
	{
		UActorComponent* Component = Components[ComponentIndex];

		if (bForceAllUsedMipsResident)
		{
			// Remove the mip streaming override on the mesh to be removed
			UMeshComponent* pMesh = Cast<UMeshComponent>(Component);
			if (pMesh != NULL)
			{
				pMesh->SetTextureForceResidentFlag(false);
			}
		}

		Component->UnregisterComponent();
	}

	PreviewWorld->CleanupWorld();
	GEngine->DestroyWorldContext(GetWorld());
}

void FArmyPreviewScene::AddComponent(UActorComponent* Component, const FTransform& LocalToWorld)
{
	Components.AddUnique(Component);

	USceneComponent* SceneComp = Cast<USceneComponent>(Component);
	if (SceneComp && SceneComp->GetAttachParent() == NULL)
	{
		SceneComp->SetRelativeTransform(LocalToWorld);
	}

	Component->RegisterComponentWithWorld(GetWorld());

	if (bForceAllUsedMipsResident)
	{
		// Add a mip streaming override to the new mesh
		UMeshComponent* pMesh = Cast<UMeshComponent>(Component);
		if (pMesh != NULL)
		{
			pMesh->SetTextureForceResidentFlag(true);
		}
	}

	GetScene()->UpdateSpeedTreeWind(0.0);
}

void FArmyPreviewScene::RemoveComponent(UActorComponent* Component)
{
	Component->UnregisterComponent();
	Components.Remove(Component);

	if (bForceAllUsedMipsResident)
	{
		// Remove the mip streaming override on the old mesh
		UMeshComponent* pMesh = Cast<UMeshComponent>(Component);
		if (pMesh != NULL)
		{
			pMesh->SetTextureForceResidentFlag(false);
		}
	}
}

void FArmyPreviewScene::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (int32 Index = 0; Index < Components.Num(); Index++)
	{
		Collector.AddReferencedObject(Components[Index]);
	}
	Collector.AddReferencedObject(DirectionalLight);
	Collector.AddReferencedObject(PreviewWorld);
}

void FArmyPreviewScene::ClearLineBatcher()
{
	if (LineBatcher != NULL)
	{
		LineBatcher->Flush();
	}
}

/** Accessor for finding the current direction of the preview scene's DirectionalLight. */
FRotator FArmyPreviewScene::GetLightDirection()
{
	return DirectionalLight->GetComponentTransform().GetUnitAxis(EAxis::X).Rotation();
}

/** Function for modifying the current direction of the preview scene's DirectionalLight. */
void FArmyPreviewScene::SetLightDirection(const FRotator& InLightDir)
{
#if WITH_EDITOR
	DirectionalLight->PreEditChange(NULL);
#endif // WITH_EDITOR
	DirectionalLight->SetAbsolute(true, true, true);
	DirectionalLight->SetRelativeRotation(InLightDir);
#if WITH_EDITOR
	DirectionalLight->PostEditChange();
#endif // WITH_EDITOR
}

void FArmyPreviewScene::SetLightBrightness(float LightBrightness)
{
#if WITH_EDITOR
	DirectionalLight->PreEditChange(NULL);
#endif // WITH_EDITOR
	DirectionalLight->Intensity = LightBrightness;
#if WITH_EDITOR
	DirectionalLight->PostEditChange();
#endif // WITH_EDITOR
}

void FArmyPreviewScene::SetLightColor(const FColor& LightColor)
{
#if WITH_EDITOR
	DirectionalLight->PreEditChange(NULL);
#endif // WITH_EDITOR
	DirectionalLight->LightColor = LightColor;
#if WITH_EDITOR
	DirectionalLight->PostEditChange();
#endif // WITH_EDITOR
}

void FArmyPreviewScene::SetSkyBrightness(float SkyBrightness)
{
	//GetScene()->UpdateDynamicSkyLight(FLinearColor::White * SkyBrightness, FLinearColor::Black);
}

void FArmyPreviewScene::LoadSettings(const TCHAR* Section)
{
	FRotator LightDir;
	if (GConfig->GetRotator(Section, TEXT("LightDir"), LightDir, GEditorPerProjectIni))
	{
		SetLightDirection(LightDir);
	}
}

void FArmyPreviewScene::SaveSettings(const TCHAR* Section)
{
	GConfig->SetRotator(Section, TEXT("LightDir"), GetLightDirection(), GEditorPerProjectIni);
}