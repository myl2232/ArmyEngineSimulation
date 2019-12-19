#pragma once
#include "CoreMinimal.h"
#include "SlateTextures.h"
#include "Widgets/SCompoundWidget.h"

class FArmyAssetThumbnail : public ISlateViewport, public TSharedFromThis<FArmyAssetThumbnail>
{
public:
	FArmyAssetThumbnail(FSlateTextureRenderTarget2DResource* InThumbnailRenderTarget, UObject* InObject);
	~FArmyAssetThumbnail();

	virtual FIntPoint GetSize() const override;
	virtual class FSlateShaderResource* GetViewportRenderTargetTexture() const override;
	virtual bool RequiresVsync() const override;

	void UpdateThumbnail();

private:
	void UpdateTextureData(FObjectThumbnail* ObjectThumbnail);

private:
	UObject*	ThumbnailObject;
	FSlateTexture2DRHIRef*					ThumbnailTexture;
	FSlateTextureRenderTarget2DResource*	ThumbnailRenderTarget;
};

class ARMYEDITOR_API SArmyThumbnailItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyThumbnailItem)
		: _ThumbnailObject(NULL)
	{}
	SLATE_ARGUMENT(UObject*, ThumbnailObject)
	//SLATE_ARGUMENT(FSlateTextureRenderTarget2DResource*, ThumbnailRenderTarget)
	SLATE_END_ARGS()

	SArmyThumbnailItem();
	~SArmyThumbnailItem();
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void Construct(const FArguments& InArgs);
	void UpdateThumbnail();

	UObject*								ThumbnailObject;
	FSlateTexture2DRHIRef*					ThumbnailTexture;
	FSlateTextureRenderTarget2DResource*	ThumbnailRenderTarget;
	TSharedPtr<SViewport>					ThumbnailViewport;
	TSharedPtr<FArmyAssetThumbnail>			AssetThumbnail;
	bool bRefreshed;
};
