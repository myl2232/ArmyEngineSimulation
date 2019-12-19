#include "ArmyThumbnailItem.h"
#include "SlateOptMacros.h"
#include "ArmyThumbnailRenderer.h"
#include "ArmyEditorModule.h"
#include "CanvasTypes.h"
#include "SViewport.h"

FArmyAssetThumbnail::FArmyAssetThumbnail(FSlateTextureRenderTarget2DResource* InThumbnailRenderTarget,
	UObject* InObject)
	: ThumbnailObject(InObject)
	, ThumbnailTexture(NULL)
	, ThumbnailRenderTarget(InThumbnailRenderTarget)
{
	FIntPoint RTSize = ThumbnailRenderTarget->GetSizeXY();
	ThumbnailTexture = new FSlateTexture2DRHIRef(RTSize.X, RTSize.Y, PF_B8G8R8A8, NULL, TexCreate_Dynamic);
	BeginInitResource(ThumbnailTexture);
}

FArmyAssetThumbnail::~FArmyAssetThumbnail()
{
	BeginReleaseResource(ThumbnailTexture);
	FlushRenderingCommands();
	delete ThumbnailTexture;
	ThumbnailTexture = NULL;
}

FIntPoint FArmyAssetThumbnail::GetSize() const
{
	return ThumbnailRenderTarget->GetSizeXY();
}

FSlateShaderResource* FArmyAssetThumbnail::GetViewportRenderTargetTexture() const
{
	return ThumbnailTexture;
}

bool FArmyAssetThumbnail::RequiresVsync() const
{
	return false;
}

void FArmyAssetThumbnail::UpdateTextureData(FObjectThumbnail* ObjectThumbnail)
{
	check(ThumbnailTexture)
		if (ObjectThumbnail &&
			ObjectThumbnail->GetImageWidth() > 0 &&
			ObjectThumbnail->GetImageHeight() > 0 &&
			ObjectThumbnail->GetUncompressedImageData().Num() > 0)
		{
			// Make bulk data for updating the texture memory later
			FSlateTextureDataPtr ThumbnailBulkData = MakeShareable(new FSlateTextureData(
				ObjectThumbnail->GetImageWidth(),
				ObjectThumbnail->GetImageHeight(),
				GPixelFormats[PF_B8G8R8A8].BlockBytes,
				ObjectThumbnail->AccessImageData())
			);

			// Update the texture RHI
			ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
				UpdateArmyAssetThumbnailResourse,
				FSlateTexture2DRHIRef*, Texture, ThumbnailTexture,
				FSlateTextureDataPtr, BulkData, ThumbnailBulkData,
				{
					Texture->SetTextureData(BulkData);
			Texture->UpdateRHI();
				});
		}
}

void FArmyAssetThumbnail::UpdateThumbnail()
{
	if (ThumbnailObject)
	{
		FObjectThumbnail NewThumbnail;
		FIntPoint RTSize = ThumbnailRenderTarget->GetSizeXY();
		//FSThumbnailTools::RenderThumbnail(
		//	ThumbnailObject,
		//	RTSize.X,
		//	RTSize.Y,
		//	FSThumbnailTools::EFSThumbnailTextureFlushMode::NeverFlush,
		//	//ThePawn->CaptureComponent2D.Get()->TextureTarget->GameThread_GetRenderTargetResource(),
		//	ThumbnailRenderTarget,
		//	&NewThumbnail
		//	);
		UObject* InObject = ThumbnailObject;
		UArmyThumbnailRenderer* TheRender = NULL;
		if (InObject->IsA(UMaterialInterface::StaticClass()))
			TheRender = FArmyEditorModule::Get().ThumbnailRenderer_MaterialInstance;
		else if (InObject->IsA(UTexture::StaticClass()))
			TheRender = FArmyEditorModule::Get().ThumbnailRenderer_Texture;

		const uint32 InImageWidth = RTSize.X;
		const uint32 InImageHeight = RTSize.Y;
		FTextureRenderTargetResource* InTextureRenderTargetResource = ThumbnailRenderTarget;
		FObjectThumbnail* OutThumbnail = &NewThumbnail;


		check(GIsRHIInitialized);
		if (OutThumbnail)
		{
			OutThumbnail->SetImageSize(InImageWidth, InImageHeight);
		}
		FTextureRenderTargetResource* RenderTargetResource = InTextureRenderTargetResource;
		check(RenderTargetResource != NULL);
		FCanvas Canvas(RenderTargetResource, NULL, FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime, GMaxRHIFeatureLevel);
		Canvas.Clear(FLinearColor::Blue);

		// If this object's thumbnail will be rendered to a texture on the GPU.
		bool bUseGPUGeneratedThumbnail = true;

		//if (GFrame->LevelEditorWidget->MaterialSettingsWidget->GetMIRender())
		if (TheRender)
		{
			const float ZoomFactor = 1.0f;

			uint32 DrawWidth = InImageWidth;
			uint32 DrawHeight = InImageHeight;
			if (OutThumbnail)
			{
				// Find how big the thumbnail WANTS to be
				uint32 DesiredWidth = 0;
				uint32 DesiredHeight = 0;
				// Does this thumbnail have a size associated with it?  Materials and textures often do!
				if (DesiredWidth > 0 && DesiredHeight > 0)
				{
					// Scale the desired size down if it's too big, preserving aspect ratio
					if (DesiredWidth > InImageWidth)
					{
						DesiredHeight = (DesiredHeight * InImageWidth) / DesiredWidth;
						DesiredWidth = InImageWidth;
					}
					if (DesiredHeight > InImageHeight)
					{
						DesiredWidth = (DesiredWidth * InImageHeight) / DesiredHeight;
						DesiredHeight = InImageHeight;
					}
					// Update dimensions
					DrawWidth = FMath::Max<uint32>(1, DesiredWidth);
					DrawHeight = FMath::Max<uint32>(1, DesiredHeight);
					OutThumbnail->SetImageSize(DrawWidth, DrawHeight);
				}
			}
			// Draw the thumbnail
			const int32 XPos = 0;
			const int32 YPos = 0;
			//GFrame->LevelEditorWidget->MaterialSettingsWidget->MIRender->Draw(
			TheRender->Draw(
				InObject,
				XPos,
				YPos,
				DrawWidth,
				DrawHeight,
				RenderTargetResource,
				&Canvas
			);
		}
		// GPU based thumbnail rendering only
		if (bUseGPUGeneratedThumbnail)
		{
			// Tell the rendering thread to draw any remaining batched elements
			Canvas.Flush_GameThread();
			{
				if (OutThumbnail)
				{
					const FIntRect InSrcRect(0, 0, OutThumbnail->GetImageWidth(), OutThumbnail->GetImageHeight());
					TArray<uint8>& OutData = OutThumbnail->AccessImageData();
					OutData.Empty();
					OutData.AddUninitialized(OutThumbnail->GetImageWidth() * OutThumbnail->GetImageHeight() * sizeof(FColor));
					RenderTargetResource->ReadPixelsPtr((FColor*)OutData.GetData(), FReadSurfaceDataFlags(), InSrcRect);
				}
			}
		}
		UpdateTextureData(&NewThumbnail);
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SArmyThumbnailItem::SArmyThumbnailItem()
{
	bRefreshed = false;
}

SArmyThumbnailItem::~SArmyThumbnailItem()
{
	BeginReleaseResource(ThumbnailTexture);
	// Wait for all resources to be released
	FlushRenderingCommands();
	delete ThumbnailTexture;
	ThumbnailTexture = NULL;
}

void SArmyThumbnailItem::Construct(const FArguments& InArgs)
{
	ThumbnailObject = InArgs._ThumbnailObject;
	//ThumbnailRenderTarget = InArgs._ThumbnailRenderTarget;
	ThumbnailRenderTarget = FArmyEditorModule::Get().SharedThumbnailRT;

	FIntPoint RTSize = ThumbnailRenderTarget->GetSizeXY();
	ThumbnailTexture = new FSlateTexture2DRHIRef(RTSize.X, RTSize.Y, PF_B8G8R8A8, NULL, TexCreate_Dynamic);
	BeginInitResource(ThumbnailTexture);

	AssetThumbnail = MakeShareable(new FArmyAssetThumbnail(ThumbnailRenderTarget, ThumbnailObject));
	ThumbnailViewport = SNew(SViewport).EnableGammaCorrection(false);
	ThumbnailViewport->SetViewportInterface(AssetThumbnail.ToSharedRef());
	ThumbnailViewport->SetEnabled(true);

	ChildSlot.VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(100)
			.HeightOverride(100)
			[
				ThumbnailViewport.ToSharedRef()
			]
		];
}

void SArmyThumbnailItem::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	UpdateThumbnail();
}

void SArmyThumbnailItem::UpdateThumbnail()
{
	if (!bRefreshed)
	{
		bRefreshed = true;
		AssetThumbnail->UpdateThumbnail();
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
