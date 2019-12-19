#pragma once

#include "CoreMinimal.h"
#include "ArmyPreviewScene.h"
#include "ArmyThumbnailHelpers.h"
#include "ArmyThumbnailRenderer.generated.h"

UCLASS(abstract, MinimalAPI)
class UArmyThumbnailRenderer : public UObject
{
	GENERATED_UCLASS_BODY()


public:
	/**
	* Returns true if the renderer is capable of producing a thumbnail for the specified asset.
	*
	* @param Object the asset to attempt to render
	*/
	virtual bool CanVisualizeAsset(UObject* Object) { return true; }

	/**
	* Calculates the size the thumbnail would be at the specified zoom level
	*
	* @param Object the object the thumbnail is of
	* @param Zoom the current multiplier of size
	* @param OutWidth the var that gets the width of the thumbnail
	* @param OutHeight the var that gets the height
	*/
	virtual void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const PURE_VIRTUAL(UThumbnailRenderer::GetThumbnailSize, );

	/**
	* Draws a thumbnail for the object that was specified.
	*
	* @param Object the object to draw the thumbnail for
	* @param X the X coordinate to start drawing at
	* @param Y the Y coordinate to start drawing at
	* @param Width the width of the thumbnail to draw
	* @param Height the height of the thumbnail to draw
	* @param Viewport the viewport being drawn in
	* @param Canvas the render interface to draw with
	*/
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas) PURE_VIRTUAL(UThumbnailRenderer::Draw, );
};

//-------------------------------------------------------------------------VRSMaterialInstanceThumbnailRenderer
UCLASS()
class UArmyMaterialInstanceThumbnailRenderer : public UArmyThumbnailRenderer
{
	GENERATED_UCLASS_BODY()
		virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas) override;
	virtual void BeginDestroy() override;

private:
	class FArmyMaterialThumbnailScene* ThumbnailScene;
};

UCLASS()
class UArmyTextureThumbnailRenderer : public UArmyThumbnailRenderer
{
	GENERATED_UCLASS_BODY()
		virtual void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas) override;
};