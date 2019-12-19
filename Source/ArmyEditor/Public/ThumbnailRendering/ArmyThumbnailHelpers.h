#pragma once

#include "CoreMinimal.h"
#include "ArmyPreviewScene.h"

class FSceneViewFamily;

class FArmyThumbnailPreviewScene : public FArmyPreviewScene
{
public:
	/** Constructor */
	FArmyThumbnailPreviewScene();

	/** Allocates then adds an FSceneView to the ViewFamily. */
	void GetView(FSceneViewFamily* ViewFamily, int32 X, int32 Y, uint32 SizeX, uint32 SizeY) const;

protected:
	/** Helper function to get the bounds offset to display an asset */
	float GetBoundsZOffset(const FBoxSphereBounds& Bounds) const;

	/**
	* Gets parameters to create a view matrix to be used by GetView(). Implemented in children classes.
	* @param InFOVDegrees  The FOV used to display the thumbnail. Often used to calculate the output parameters.
	* @param OutOrigin	 The origin of the orbit view. Typically the center of the bounds of the target object.
	* @param OutOrbitPitch The pitch of the orbit cam around the object.
	* @param OutOrbitYaw	 The yaw of the orbit cam around the object.
	* @param OutOrbitZoom  The camera distance from the object.
	*/
	virtual void GetViewMatrixParameters(const float InFOVDegrees, FVector& OutOrigin, float& OutOrbitPitch, float& OutOrbitYaw, float& OutOrbitZoom) const = 0;
};

class FArmyMaterialThumbnailScene : public FArmyThumbnailPreviewScene
{
public:
	/** Constructor */
	FArmyMaterialThumbnailScene();

	/** Sets the material to use in the next GetView() */
	void SetMaterialInterface(class UMaterialInterface* InMaterial);

protected:
	// FThumbnailPreviewScene implementation
	virtual void GetViewMatrixParameters(const float InFOVDegrees, FVector& OutOrigin, float& OutOrbitPitch, float& OutOrbitYaw, float& OutOrbitZoom) const override;

protected:
	/** The static mesh actor used to display all material thumbnails */
	AStaticMeshActor* PreviewActor;
};