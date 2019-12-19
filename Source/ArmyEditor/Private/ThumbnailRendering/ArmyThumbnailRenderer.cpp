#include "ArmyThumbnailRenderer.h"
#include "ArmyNormalMapPreview.h"
#include "Runtime/Engine/Classes/Engine/TextureLightProfile.h"
#include "Runtime/Engine/Public/CubemapUnwrapUtils.h"
#include "EngineModule.h"
#include "CanvasItem.h"

UArmyThumbnailRenderer::UArmyThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


//-------------------------------------------------------------------------VRSMaterialInstanceThumbnailRenderer
UArmyMaterialInstanceThumbnailRenderer::UArmyMaterialInstanceThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ThumbnailScene = nullptr;
}

void UArmyMaterialInstanceThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas)
{
	UMaterialInterface* MatInst = Cast<UMaterialInterface>(Object);
	if (MatInst != nullptr)
	{
		if (ThumbnailScene == nullptr)
		{
			ThumbnailScene = new FArmyMaterialThumbnailScene();
		}

		ThumbnailScene->SetMaterialInterface(MatInst);
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(RenderTarget, ThumbnailScene->GetScene(), FEngineShowFlags(ESFIM_Game))
			.SetWorldTimes(FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime));

		ViewFamily.EngineShowFlags.DisableAdvancedFeatures();
		ViewFamily.EngineShowFlags.MotionBlur = 0;
		ViewFamily.EngineShowFlags.AntiAliasing = 0;

		ThumbnailScene->GetView(&ViewFamily, X, Y, Width, Height);

		if (ViewFamily.Views.Num() > 0)
		{
			GetRendererModule().BeginRenderingViewFamily(Canvas, &ViewFamily);
		}

		ThumbnailScene->SetMaterialInterface(nullptr);
	}
}

void UArmyMaterialInstanceThumbnailRenderer::BeginDestroy()
{
	if (ThumbnailScene != nullptr)
	{
		delete ThumbnailScene;
		ThumbnailScene = nullptr;
	}

	Super::BeginDestroy();
}

//-------------------------------------------------------------------------VRSTextureThumbnailRenderer
UArmyTextureThumbnailRenderer::UArmyTextureThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UArmyTextureThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	/*UTexture* Texture = Cast<UTexture>(Object);
	if (Texture != nullptr)
	{
		OutWidth = FMath::TruncToInt(Zoom * (float)Texture->GetSurfaceWidth());
		OutHeight = FMath::TruncToInt(Zoom * (float)Texture->GetSurfaceHeight());
	}
	else
	{
		OutWidth = OutHeight = 0;
	}*/
	UTexture* Texture = Cast<UTexture>(Object);
	UTextureLightProfile* TextureLightProfile = Cast<UTextureLightProfile>(Object);

	if (TextureLightProfile)
	{
		// otherwise a 1D texture would result in a very boring thumbnail
		OutWidth = 192;
		OutHeight = 192;
		return;
	}

	if (Texture != nullptr)
	{
		OutWidth = 128;
		OutHeight = 128;
	}
}

void UArmyTextureThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas)
{
	UTexture* Texture = Cast<UTexture>(Object);
	if (Texture != nullptr && Texture->Resource != nullptr)
	{
		UTexture2D* Texture2D = Cast<UTexture2D>(Texture);

		// Take the alpha channel into account for textures that have one.
		// This provides a much better preview than just showing RGB,
		// Because the RGB content in areas with an alpha of 0 is often garbage that will not be seen in normal conditions.
		// Non-UI textures often have uncorrelated data in the alpha channel (like a skin mask, specular power, etc) so we only preview UI textures this way.
		const bool bUseTranslucentBlend = Texture2D && Texture2D->HasAlphaChannel() && ((Texture2D->LODGroup == TEXTUREGROUP_UI) || (Texture2D->LODGroup == TEXTUREGROUP_Pixels2D));

		UTextureLightProfile* TextureLightProfile = Cast<UTextureLightProfile>(Texture);

		TRefCountPtr<FBatchedElementParameters> BatchedElementParameters;

 		if (TextureLightProfile)
 		{
 			BatchedElementParameters = new FIESLightProfileBatchedElementParameters(TextureLightProfile->Brightness);
 		}
 		else if (Texture2D && Texture2D->IsNormalMap())
 		{
 			BatchedElementParameters = new FNormalMapBatchedElementParameters();
 		}

 		//if (bUseTranslucentBlend)
 		//{
 		//	// If using alpha, draw a checkerboard underneath first.
 		//	const int32 CheckerDensity = 8;
 		//	auto* Checker = UThumbnailManager::Get().CheckerboardTexture;
 		//	Canvas->DrawTile(
 		//		0.0f, 0.0f, Width, Height,							// Dimensions
 		//		0.0f, 0.0f, CheckerDensity, CheckerDensity,			// UVs
 		//		FLinearColor::White, Checker->Resource);			// Tint & Texture
 		//}

		// Use A canvas tile item to draw
		FCanvasTileItem CanvasTile(FVector2D(X, Y), Texture->Resource, FVector2D(Width, Height), FLinearColor::White);
		CanvasTile.BlendMode = bUseTranslucentBlend ? SE_BLEND_Translucent : SE_BLEND_Opaque;
		CanvasTile.BatchedElementParameters = BatchedElementParameters;
		CanvasTile.Draw(Canvas);

		if (TextureLightProfile)
		{
			float Brightness = TextureLightProfile->Brightness;

			// Brightness in Lumens
			FText BrightnessText = FText::AsNumber(Brightness);
			FCanvasTextItem TextItem(FVector2D(5.0f, 5.0f), BrightnessText, GEngine->GetLargeFont(), FLinearColor::White);
			TextItem.EnableShadow(FLinearColor::Black);
			TextItem.Scale = FVector2D(Width / 128.0f, Height / 128.0f);
			TextItem.Draw(Canvas);
		}
	}
}
