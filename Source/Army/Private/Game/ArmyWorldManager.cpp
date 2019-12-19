#include "ArmyWorldManager.h"
#include "ArmyStaticLighting/Public/StaticLightingPrivate.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyHardModeController.h"
#include "Components/LightComponentBase.h"
#include "Components/SkyLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "ArmyPointLightActor.h"

TSharedPtr<FArmyWorldManager> FArmyWorldManager::Singleton = nullptr;

FArmyWorldManager::FArmyWorldManager()
{

}

void FArmyWorldManager::Init(UWorld* InWorld)
{
	MyWorld = InWorld;

	for (TActorIterator<AActor> ActorItr(InWorld); ActorItr; ++ActorItr)
	{
		FString ActorName = (*ActorItr)->GetName();
		if (ActorName == "DynamicSunLight")
			DynamicSunLight = Cast<ADirectionalLight>(*ActorItr);
		else if (ActorName == "DynamicSkyLight")
			DynamicSkyLight = Cast<ASkyLight>(*ActorItr);
		else if (ActorName == "StaticSkyLight")
			StaticSkyLight = Cast<ASkyLight>(*ActorItr);
		else if (ActorName == "StaticSunLight")
			StaticSunLight = Cast<ADirectionalLight>(*ActorItr);
		else if (ActorName == "PPV_DynamicLighting")
			PPV_DynamicLighting = Cast<APostProcessVolume>(*ActorItr);
		else if (ActorName == "PPV_StaticLighting")
			PPV_StaticLighting = Cast<APostProcessVolume>(*ActorItr);
		else if (ActorName.Contains("SM_Sorround"))
			SM_OutdoorPanorama = Cast<AStaticMeshActor>(*ActorItr);
	}

	//默认动态环境
	SetEnvironmentMode(true);

	//为外景模型创建动态材质实例，用来修改外景纹理
	if (SM_OutdoorPanorama)
	{
		MID_OutdoorPanorama = UMaterialInstanceDynamic::Create(SM_OutdoorPanorama->GetStaticMeshComponent()->GetStaticMesh()->GetMaterial(0), SM_OutdoorPanorama);
		SM_OutdoorPanorama->GetStaticMeshComponent()->SetMaterial(0, MID_OutdoorPanorama);
	}
}

void FArmyWorldManager::SetOutdoorMeshVisibile(bool bInVisible)
{
	SM_OutdoorPanorama->SetActorHiddenInGame(!bInVisible);
}

void FArmyWorldManager::TryChangeEnvironmentModeConditionally(FString InCurrentMode)
{
	if (InCurrentMode == DesignMode::HydropowerMode)
	{
		if (FStaticLightingManager::Get()->IsValidLightMap())
			SetEnvironmentMode(true);
	}
	else if (InCurrentMode == DesignMode::HardMode || InCurrentMode == DesignMode::WHCMode)
	{
		SetEnvironmentMode(!FStaticLightingManager::Get()->IsValidLightMap());
	}
}

void FArmyWorldManager::SetEnvironmentMode(bool bInDynamic)
{
	for (TActorIterator<APointLight> It(GWorld); It; ++It)
	{
		UPointLightComponent* LM = (*It)->PointLightComponent;
		if (LM->GetName() == "LampSlotPointLightComponent")
		{
			LM->SetMobility(bInDynamic ? EComponentMobility::Movable : EComponentMobility::Static);
			LM->SetIntensity(bInDynamic ? 600.f : 400.f);
			LM->SetCastShadows(!bInDynamic);
		}
	}

	for (TActorIterator<AXRPointLightActor> It(GWorld); It; ++It)
	{
		UPointLightComponent* LM = (*It)->PointLightCOM;
		LM->SetCastShadows(!bInDynamic);
	}

	for (TActorIterator<AXRSpotLightActor> It(GWorld); It; ++It)
	{
		USpotLightComponent* LM = (*It)->SpotLightCOM;
		LM->SetCastShadows(!bInDynamic);
	}
	

	//静态太阳光
	StaticSunLight->GetLightComponent()->bAffectsWorld = !bInDynamic;
	StaticSunLight->GetLightComponent()->RecreateRenderState_Concurrent();
	//动态太阳光
	DynamicSunLight->GetLightComponent()->bAffectsWorld = bInDynamic;
	DynamicSunLight->GetLightComponent()->RecreateRenderState_Concurrent();
	//动态天光
	DynamicSkyLight->GetLightComponent()->bAffectsWorld = bInDynamic;
	DynamicSkyLight->GetLightComponent()->RecreateRenderState_Concurrent();
	//静态天光
	StaticSkyLight->GetLightComponent()->bAffectsWorld = !bInDynamic;
	StaticSkyLight->GetLightComponent()->RecreateRenderState_Concurrent();
	//切换PPV
	PPV_DynamicLighting->bEnabled = bInDynamic;
	PPV_StaticLighting->bEnabled = !bInDynamic;

	if(bInDynamic)
		MyWorld->GetGameViewport()->ConsoleCommand("showflag.volumetriclightmap 0");
	else
		MyWorld->GetGameViewport()->ConsoleCommand("showflag.volumetriclightmap 1");
}

int32 FArmyWorldManager::GetCameraFOV()
{
	CameraFOV = GVC->ViewFOV;
	return CameraFOV;
}

void FArmyWorldManager::SetGlobalStaticLightingResScale(float InValue)
{
	GlobalStaticLightingResScale = InValue;
}

void FArmyWorldManager::SetSkirtLineStaticLightingRes(int32 InValue)
{
	SkirtLineStaticLightingRes = InValue;
	for (TObjectIterator<UXRProceduralMeshComponent> It; It; ++It)
	{
		if (It->GetName() == "SkirtLineComponent")
		{
			It->SetLightmapResolution(InValue);
		}
	}
}

void FArmyWorldManager::SetMoldingLineStaticLightingRes(int32 InValue)
{
	MoldingLineStaticLightingRes = InValue;
	for (TObjectIterator<UXRProceduralMeshComponent> It; It; ++It)
	{
		if (It->GetName() == "MoldingLineComponent")
		{
			It->SetLightmapResolution(InValue);
		}
	}
}

void FArmyWorldManager::SetWindowStaticLightingRes(int32 InValue)
{
	WindowStaticLightingRes = InValue;
	for (TObjectIterator<UXRProceduralMeshComponent> It; It; ++It)
	{
		if (It->GetName() == "WindowComponent")
		{
			It->SetLightmapResolution(InValue);
		}
	}
}

void FArmyWorldManager::SetCameraFOV(int32 InValue)
{
	CameraFOV = InValue;

	GVC->ViewFOV = InValue;
}

void FArmyWorldManager::SetOutdoorPanoramaIndex(int32 InIndex)
{
	OutdoorPanoramaIndex = InIndex;

	if (MID_OutdoorPanorama)
	{
		MID_OutdoorPanorama->SetTextureParameterValue("Texture", FArmyEngineModule::Get().GetEngineResource()->GetOutdoorPanoramaTexture(InIndex));
	}
}

void FArmyWorldManager::SetOutdoorPanoramaYaw(int32 InYaw)
{
	OutdoorPanoramaYaw = InYaw;

	if (SM_OutdoorPanorama)
	{
		FRotator Rot = SM_OutdoorPanorama->GetActorRotation();
		Rot.Yaw = InYaw;
		SM_OutdoorPanorama->SetActorRotation(Rot);
	}
}

void FArmyWorldManager::DirectionalLight_Intensity(float InValue)
{
	StaticSunLight->GetLightComponent()->SetIntensity(InValue);
}

void FArmyWorldManager::DirectionalLight_Color(int32 R, int32 G, int32 B)
{
	StaticSunLight->GetLightComponent()->SetLightColor(FLinearColor(FColor(R, G, B)));
}

void FArmyWorldManager::DirectionalLight_IndirectIntensity(float InValue)
{
	StaticSunLight->GetLightComponent()->SetIndirectLightingIntensity(InValue);
}

void FArmyWorldManager::DirectionalLight_SourceAngle(float InValue)
{
	UDirectionalLightComponent* DC = Cast<UDirectionalLightComponent>(StaticSunLight->GetLightComponent());
	if (DC)
		DC->LightSourceAngle = InValue;
}

void FArmyWorldManager::DirectionalLight_ShadowExponent(float InValue)
{
	UDirectionalLightComponent* DC = Cast<UDirectionalLightComponent>(StaticSunLight->GetLightComponent());
	if (DC)
		DC->LightmassSettings.ShadowExponent = InValue;
}

void FArmyWorldManager::DirectionalLight_Rotation(int32 InPitch, int32 InYaw, int32 InRoll)
{
	StaticSunLight->SetActorRotation(FRotator(InPitch, InYaw, InRoll).Quaternion());
}

void FArmyWorldManager::Postprocess_AutoExposure(float InMin, float InMax, float InUp, float InDown)
{
	PPV_StaticLighting->Settings.bOverride_AutoExposureMinBrightness = true;
	PPV_StaticLighting->Settings.bOverride_AutoExposureMaxBrightness = true;
	PPV_StaticLighting->Settings.bOverride_AutoExposureSpeedUp = true;
	PPV_StaticLighting->Settings.bOverride_AutoExposureSpeedDown = true;

	PPV_StaticLighting->Settings.AutoExposureMinBrightness = InMin;
	PPV_StaticLighting->Settings.AutoExposureMaxBrightness = InMax;
	PPV_StaticLighting->Settings.AutoExposureSpeedUp = InUp;
	PPV_StaticLighting->Settings.AutoExposureSpeedDown = InDown;
}

void FArmyWorldManager::Postprocess_AutoExposureBias(float InValue)
{
	PPV_StaticLighting->Settings.AutoExposureBias = InValue;
}

void FArmyWorldManager::Postprocess_IndirectLightingIntensity(float InValue)
{
	PPV_StaticLighting->Settings.IndirectLightingIntensity = InValue;
}

void FArmyWorldManager::WorldSetting_StaticLightingLevelScale(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.StaticLightingLevelScale = InValue;
}

void FArmyWorldManager::WorldSetting_NumIndirectLightingBounces(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.NumIndirectLightingBounces = InValue;
}

void FArmyWorldManager::WorldSetting_NumSkyLightingBounces(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.NumSkyLightingBounces = InValue;
}

void FArmyWorldManager::WorldSetting_IndirectLightingQuality(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.IndirectLightingQuality = InValue;
}

void FArmyWorldManager::WorldSetting_IndirectLightingSmoothness(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.IndirectLightingSmoothness = InValue;
}

void FArmyWorldManager::WorldSetting_EnvironmentColor(int32 R, int32 G, int32 B)
{
	MyWorld->GetWorldSettings()->LightmassSettings.EnvironmentColor = FColor(R, G, B);
}

void FArmyWorldManager::WorldSetting_EnvironmentIntensity(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.EnvironmentIntensity = InValue;
}

void FArmyWorldManager::WorldSetting_DiffuseBoost(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.DiffuseBoost = InValue;
}

void FArmyWorldManager::WorldSetting_VolumetricLightmapDetailCellSize(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.VolumetricLightmapDetailCellSize = InValue;
}

void FArmyWorldManager::WorldSetting_VolumetricLightmapMaximumBirckMemoryMb(float InValue)
{
	MyWorld->GetWorldSettings()->LightmassSettings.VolumetricLightmapMaximumBrickMemoryMb = InValue;
}

void FArmyWorldManager::WorldSetting_AO(float InDirectillOFraction, float InIndirectillOFraction, float InOExponent, float InFullyOSimplesFraction, float InMaxODistance)
{
	MyWorld->GetWorldSettings()->LightmassSettings.DirectIlluminationOcclusionFraction = InDirectillOFraction;
	MyWorld->GetWorldSettings()->LightmassSettings.IndirectIlluminationOcclusionFraction = InIndirectillOFraction;
	MyWorld->GetWorldSettings()->LightmassSettings.OcclusionExponent = InOExponent;
	MyWorld->GetWorldSettings()->LightmassSettings.FullyOccludedSamplesFraction = InFullyOSimplesFraction;
	MyWorld->GetWorldSettings()->LightmassSettings.MaxOcclusionDistance = InMaxODistance;

}

void FArmyWorldManager::WorldSetting_SSR(float InIntensity, float InQuality, float InMaxRoughness)
{
	PPV_StaticLighting->Settings.bOverride_ScreenSpaceReflectionIntensity = 1;
	PPV_StaticLighting->Settings.bOverride_ScreenSpaceReflectionQuality = 1;
	PPV_StaticLighting->Settings.bOverride_ScreenSpaceReflectionMaxRoughness = 1;

	PPV_StaticLighting->Settings.ScreenSpaceReflectionIntensity = InIntensity;
	PPV_StaticLighting->Settings.ScreenSpaceReflectionQuality = InQuality;
	PPV_StaticLighting->Settings.ScreenSpaceReflectionMaxRoughness = InMaxRoughness;
}

void FArmyWorldManager::CeilingLampIntensity(float InValue)
{
	for (TActorIterator<APointLight> It(GWorld); It; ++It)
	{
		UPointLightComponent* LM = (*It)->PointLightComponent;
		if (LM->GetName() == "LampSlotPointLightComponent")
		{
			LM->SetIntensity(400.f);
		}
	}
}
