#include "ArmyTextureDerivedDataTask.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/MemoryReader.h"
#include "DerivedDataCacheInterface.h"
void FArmyTextureCachedDerivedDataWorker::FArmyTextureSourceData::Init(UTexture& InTexture, const FArmyTextureBuildSettings& InBuildSettings, bool bAllowAsyncLoading, FArmyTextureSource Source)
{
	switch (Source.GetFormat())
	{
	case TSF_G8:
		ImageFormat = ERawImageFormat::G8;
		break;
	case TSF_BGRA8:
		ImageFormat = ERawImageFormat::BGRA8;
		break;
	case TSF_BGRE8:
		ImageFormat = ERawImageFormat::BGRE8;
		break;
	case TSF_RGBA16:
		ImageFormat = ERawImageFormat::RGBA16;
		break;
	case TSF_RGBA16F:
		ImageFormat = ERawImageFormat::RGBA16F;
		break;
	default:
		return;
		break;
	}

	NumMips = Source.GetNumMips();
	NumSlices = Source.GetNumSlices();

	if (NumMips < 1 || NumSlices < 1)
	{
		return;
	}
	if (InBuildSettings.MipGenSettings != TMGS_LeaveExistingMips)
	{
		NumMips = 1;
	}
	if (!InBuildSettings.bCubemap)
	{
		NumSlices = 1;
	}

	TextureName = InTexture.GetFName();

	GammaSpace = InTexture.SRGB ? (EGammaSpace::sRGB) : EGammaSpace::Linear;

	if (bAllowAsyncLoading && !Source.IsBulkDataLoaded())
	{
		AsyncSource = Source;
	}
	bValid = true;
}

void FArmyTextureCachedDerivedDataWorker::FArmyTextureSourceData::GetSourceMips(FArmyTextureSource& Source, IImageWrapperModule* InImageWrapper)
{
	if (bValid && !Mips.Num())
	{
		/*if (Source.HasHadBulkDataCleared())
		{
			return;
		}*/

		for (int32 MipIndex = 0; MipIndex < NumMips; ++MipIndex)
		{
			FImage* SourceMip = new (Mips)FImage((MipIndex == 0) ? Source.GetSizeX() : FMath::Max(1, Mips[MipIndex - 1].SizeX >> 1),
				(MipIndex == 0) ? Source.GetSizeY() : FMath::Max(1, Mips[MipIndex - 1].SizeY >> 1),
				NumSlices,
				ImageFormat,
				GammaSpace);

			if (!Source.GetMipData(SourceMip->RawData, MipIndex, InImageWrapper))
			{
				ReleaseMemory();
				bValid = false;
				break;
			}
		}
	}


}

void FArmyTextureCachedDerivedDataWorker::FArmyTextureSourceData::GetAsyncSourceMips(IImageWrapperModule* InImageWrapper)
{
	if (bValid && !Mips.Num() && AsyncSource.GetSizeOnDisk())
	{
		if (AsyncSource.LoadBulkDataWithFileReader())
		{
			GetSourceMips(AsyncSource, InImageWrapper);
		}
	}
}

void FArmyTextureCachedDerivedDataWorker::BuildTexture()
{
	

	if (TextureData.Mips.Num())
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("TextureName"), FText::FromString(Texture.GetName()));
		Args.Add(TEXT("TextureFormatName"), FText::FromString(BuildSettings.TextureFormatName.GetPlainNameString()));
		Args.Add(TEXT("TextureResolutionX"), FText::FromString(FString::FromInt(TextureData.Mips[0].SizeX)));
		Args.Add(TEXT("TextureResolutionY"), FText::FromString(FString::FromInt(TextureData.Mips[0].SizeY)));
		//FTextureStatusMessageContext StatusMessage(FText::Format(NSLOCTEXT("Engine", "BuildTextureStatus", "Building textures: {TextureName} ({TextureFormatName}, {TextureResolutionX}X{TextureResolutionY})"), Args));

		check(DerivedData->Mips.Num() == 0);
		DerivedData->SizeX = 0;
		DerivedData->SizeY = 0;
		DerivedData->PixelFormat = PF_Unknown;

		// Compress the texture.
		TArray<FArmyCompressedImage2D> CompressedMips;
		if (FArmyTextureCompressorModule::Get().BuildTexture(TextureData.Mips, CompositeTextureData.Mips, BuildSettings, CompressedMips))
		{
			check(CompressedMips.Num());

			// Build the derived data.
			const int32 MipCount = CompressedMips.Num();
			for (int32 MipIndex = 0; MipIndex < MipCount; ++MipIndex)
			{
				const FArmyCompressedImage2D& CompressedImage = CompressedMips[MipIndex];
				FTexture2DMipMap* NewMip = new(DerivedData->Mips) FTexture2DMipMap();
				NewMip->SizeX = CompressedImage.SizeX;
				NewMip->SizeY = CompressedImage.SizeY;
				NewMip->BulkData.Lock(LOCK_READ_WRITE);
				check(CompressedImage.RawData.GetTypeSize() == 1);
				void* NewMipData = NewMip->BulkData.Realloc(CompressedImage.RawData.Num());
				FMemory::Memcpy(NewMipData, CompressedImage.RawData.GetData(), CompressedImage.RawData.Num());
				NewMip->BulkData.Unlock();

				if (MipIndex == 0)
				{
					DerivedData->SizeX = CompressedImage.SizeX;
					DerivedData->SizeY = CompressedImage.SizeY;
					DerivedData->PixelFormat = (EPixelFormat)CompressedImage.PixelFormat;
				}
				else
				{
					check(CompressedImage.PixelFormat == DerivedData->PixelFormat);
				}
			}
			DerivedData->SetNumSlices(BuildSettings.bCubemap ? 6 : 1);

			// Store it in the cache.
			// @todo: This will remove the streaming bulk data, which we immediately reload below!
			// Should ideally avoid this redundant work, but it only happens when we actually have 
			// to build the texture, which should only ever be once.
			//this->BytesCached = PutDerivedDataInCache(DerivedData, KeySuffix);
		}

		if (DerivedData->Mips.Num())
		{
			bool bInlineMips = (CacheFlags & FArmyETextureCacheFlags::InlineMips) != 0;
			//	bSucceeded = !bInlineMips || DerivedData->TryInlineMipData();
			bSucceeded = !bInlineMips;
		}
		else
		{
			UE_LOG(LogTexture, Warning, TEXT("Failed to build %s derived data for %s"), *BuildSettings.TextureFormatName.GetPlainNameString(), *Texture.GetPathName());
		}
	}
}

FArmyTextureCachedDerivedDataWorker::FArmyTextureCachedDerivedDataWorker(ITextureCompressorModule* InCompressor, FTexturePlatformData* InDerivedData, UTexture* InTexture, const FArmyTextureBuildSettings& InSettings, uint32 InCachedFlags, FArmyTextureSource InSource) :
	Compressor(InCompressor),
	ImageWrapper(nullptr),
	DerivedData(InDerivedData),
	Texture(*InTexture),
	BuildSettings(InSettings),
	CacheFlags(InCachedFlags),
	bSucceeded(false),
	Source(InSource)
{
	check(DerivedData);

	//// At this point, the texture *MUST* have a valid GUID.
	if (!Source.GetId().IsValid())
	{
		UE_LOG(LogTexture, Warning, TEXT("Building texture with an invalid GUID: %s"), *Texture.GetPathName());
		//Texture.Source.ForceGenerateGuid();
	}
	check(Source.GetId().IsValid());

	// Dump any existing mips.
	DerivedData->Mips.Empty();
	UTexture::GetPixelFormatEnum();
	const bool bAllowAsyncBuild = (CacheFlags & FArmyETextureCacheFlags::AllowAsyncBuild) != 0;
	const bool bAllowAsyncLoading = (CacheFlags & FArmyETextureCacheFlags::AllowAsyncLoading) != 0;

	//if (bAllowAsyncLoading)
	//{
	//	ImageWrapper = &FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	//}

	TextureData.Init(Texture, BuildSettings, bAllowAsyncLoading, Source);
	//if (Texture.CompositeTexture && Texture.CompositeTextureMode != CTM_Disabled)
	//{
	//	const int32 SizeX = Texture.CompositeTexture->Source.GetSizeX();
	//	const int32 SizeY = Texture.CompositeTexture->Source.GetSizeY();
	//	if (FMath::IsPowerOfTwo(SizeX) && FMath::IsPowerOfTwo(SizeY))
	//	{
	//		CompositeTextureData.Init(*Texture.CompositeTexture, BuildSettings, bAllowAsyncLoading);
	//	}
	//}

	//// If the bulkdata is loaded and async build is allowed, get the source mips now (safe) to allow building the DDC if required.
	//// If the bulkdata is not loaded, the DDC will be built in Finalize() unless async loading is enabled (which won't allow reuse of the source for later use).
	//if (bAllowAsyncBuild)
	//{
	//	if (TextureData.IsValid() && Source.IsBulkDataLoaded())
	//	{
	//		TextureData.GetSourceMips(Source, ImageWrapper);
	//	}
	//	if (CompositeTextureData.IsValid() && Texture.CompositeTexture && Texture.CompositeTexture->Source.IsBulkDataLoaded())
	//	{
	//		CompositeTextureData.GetSourceMips(Texture.CompositeTexture->Source, ImageWrapper);
	//	}
	//}
}

void FArmyTextureCachedDerivedDataWorker::DoWork()
{
	const bool bForceRebuild = (CacheFlags & FArmyETextureCacheFlags::ForceRebuild) != 0;
	const bool bAllowAsyncBuild = (CacheFlags & FArmyETextureCacheFlags::AllowAsyncBuild) != 0;
	const bool bAllowAsyncLoading = (CacheFlags & FArmyETextureCacheFlags::AllowAsyncLoading) != 0;

	TArray<uint8> RawDerivedData;

	//if (!bForceRebuild && GetDerivedDataCacheRef().GetSynchronous(*DerivedData->DerivedDataKey, RawDerivedData))
	//{
	//	const bool bInlineMips = (CacheFlags & FArmyETextureCacheFlags::InlineMips) != 0;
	//	const bool bForDDC = (CacheFlags & FArmyETextureCacheFlags::ForDDCBuild) != 0;

	//	BytesCached = RawDerivedData.Num();
	//	FMemoryReader Ar(RawDerivedData, /*bIsPersistent=*/ true);
	//	//DerivedData->Serialize(Ar, NULL);
	//	bSucceeded = true;
	//	// Load any streaming (not inline) mips that are necessary for our platform.
	//	if (bForDDC)
	//	{
	//		//bSucceeded = DerivedData->TryLoadMips(0, NULL);
	//	}
	//	else if (bInlineMips)
	//	{
	//		bSucceeded = DerivedData->TryInlineMipData();
	//	}
	//	else
	//	{
	//		//bSucceeded = DerivedData->AreDerivedMipsAvailable();
	//	}
	//	bLoadedFromDDC = true;

	//	// Reset everything derived data so that we can do a clean load from the source data
	//	if (!bSucceeded)
	//	{
	//		DerivedData->Mips.Empty();
	//	}
	//}

	//if (!bSucceeded && bAllowAsyncBuild)
	//{
	//	if (bAllowAsyncLoading)
	//	{
	//		TextureData.GetAsyncSourceMips(ImageWrapper);
	//		CompositeTextureData.GetAsyncSourceMips(ImageWrapper);
	//	}

	//	if (TextureData.Mips.Num() && (!CompositeTextureData.IsValid() || CompositeTextureData.Mips.Num()))
	//	{
	//		BuildTexture();
	//		bSucceeded = true;
	//	}
	//	else
	//	{
	//		bSucceeded = false;
	//	}
	//}

	//if (bSucceeded)
	//{
	//	TextureData.ReleaseMemory();
	//	CompositeTextureData.ReleaseMemory();
	//}
}

void FArmyTextureCachedDerivedDataWorker::Finalize()
{
	check(IsInGameThread());
	// if we couldn't get from the DDC or didn't build synchronously, then we have to build now. 
	// This is a super edge case that should rarely happen.
	if (!bSucceeded)
	{
		TextureData.GetSourceMips(Source, ImageWrapper);

		BuildTexture();
	}
}

