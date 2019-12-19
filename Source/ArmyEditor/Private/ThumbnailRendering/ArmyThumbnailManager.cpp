#include "ArmyThumbnailManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/TextureCube.h"
#include "ImageUtils.h"

UArmyThumbnailManager* UArmyThumbnailManager::ThumbnailManagerSingleton = nullptr;

UArmyThumbnailManager::UArmyThumbnailManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	if (!IsRunningCommandlet())
	{
		// Structure to hold one-time initialization
		struct FConstructorStatics
		{
			ConstructorHelpers::FObjectFinder<UStaticMesh> EditorCubeMesh;
			ConstructorHelpers::FObjectFinder<UStaticMesh> EditorSphereMesh;
			ConstructorHelpers::FObjectFinder<UStaticMesh> EditorCylinderMesh;
			ConstructorHelpers::FObjectFinder<UStaticMesh> EditorPlaneMesh;
			ConstructorHelpers::FObjectFinder<UStaticMesh> EditorSkySphereMesh;
			ConstructorHelpers::FObjectFinder<UMaterial> FloorPlaneMaterial;
			ConstructorHelpers::FObjectFinder<UTextureCube> DaylightAmbientCubemap;
			FConstructorStatics()
				: EditorCubeMesh(TEXT("/Game/ArmyCommon/Mesh/EditorCube"))
				, EditorSphereMesh(TEXT("/Game/ArmyCommon/Mesh/EditorSphere"))
				, EditorCylinderMesh(TEXT("/Game/ArmyCommon/Mesh/EditorCylinder"))
				, EditorPlaneMesh(TEXT("/Game/ArmyCommon/Mesh/EditorPlane"))
				, EditorSkySphereMesh(TEXT("/Game/ArmyCommon/Mesh/EditorSkySphere"))
				, FloorPlaneMaterial(TEXT("/Game/ArmyCommon/Material/FloorPlaneMaterial"))
				, DaylightAmbientCubemap(TEXT("/Game/ArmyCommon/Texture/DaylightAmbientCubemap"))
			{
			}
		};
		static FConstructorStatics ConstructorStatics;

		EditorCube = ConstructorStatics.EditorCubeMesh.Object;
		EditorSphere = ConstructorStatics.EditorSphereMesh.Object;
		EditorCylinder = ConstructorStatics.EditorCylinderMesh.Object;
		EditorPlane = ConstructorStatics.EditorPlaneMesh.Object;
		EditorSkySphere = ConstructorStatics.EditorSkySphereMesh.Object;
		FloorPlaneMaterial = ConstructorStatics.FloorPlaneMaterial.Object;
		AmbientCubemap = ConstructorStatics.DaylightAmbientCubemap.Object;

		SetupCheckerboardTexture();
	}
}

UArmyThumbnailManager::~UArmyThumbnailManager()
{

}

ARMYEDITOR_API UArmyThumbnailManager& UArmyThumbnailManager::Get()
{
	if (ThumbnailManagerSingleton == nullptr)
		ThumbnailManagerSingleton = NewObject<UArmyThumbnailManager>();

	ThumbnailManagerSingleton->AddToRoot();

	return *ThumbnailManagerSingleton;
}

void UArmyThumbnailManager::SetupCheckerboardTexture()
{
	if (CheckerboardTexture)
	{
		return;
	}

	CheckerboardTexture = FImageUtils::CreateCheckerboardTexture(FColor(128, 128, 128), FColor(64, 64, 64), 32);
}
