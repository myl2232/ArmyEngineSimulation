#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ArmyThumbnailManager.generated.h"


UCLASS()
class UArmyThumbnailManager : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	UArmyThumbnailManager();
	~UArmyThumbnailManager();
	ARMYEDITOR_API static UArmyThumbnailManager& Get();

	UPROPERTY(Transient)
		class UStaticMesh* EditorCube;

	UPROPERTY(Transient)
		class UStaticMesh* EditorSphere;

	UPROPERTY(Transient)
		class UStaticMesh* EditorCylinder;

	UPROPERTY(Transient)
		class UStaticMesh* EditorPlane;

	UPROPERTY(Transient)
		class UStaticMesh* EditorSkySphere;

	UPROPERTY(Transient)
		class UMaterial* FloorPlaneMaterial;

	UPROPERTY(Transient)
		class UTextureCube* AmbientCubemap;

	UPROPERTY(Transient)
		class UTexture2D* CheckerboardTexture;

protected:
	static class UArmyThumbnailManager* ThumbnailManagerSingleton;

private:
	void SetupCheckerboardTexture();
};