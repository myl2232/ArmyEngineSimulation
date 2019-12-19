#pragma  once

#include "CoreMinimal.h"


#include "Async/AsyncWork.h"
#include "ImageCore.h"
#include "ArmyTextureCompressorModule.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"
#include "ArmyTextureSource.h"
namespace FArmyETextureCacheFlags
{
	enum Type
	{
		None = 0x00,
		Async = 0x01,
		ForceRebuild = 0x02,
		InlineMips = 0x08,
		AllowAsyncBuild = 0x10,
		ForDDCBuild = 0x20,
		RemoveSourceMipDataAfterCache = 0x40,
		AllowAsyncLoading = 0x80,
	};
};

class FArmyTextureCachedDerivedDataWorker : public FNonAbandonableTask
{
	struct FArmyTextureSourceData
	{
		FArmyTextureSourceData() :NumMips(0), NumSlices(0), ImageFormat(ERawImageFormat::BGRA8), GammaSpace(EGammaSpace::sRGB), bValid(false) {}

		void Init(UTexture& InTexture, const FArmyTextureBuildSettings& InBuildSettings, bool bAllowAsyncLoading, FArmyTextureSource Source);

		bool IsValid()const { return bValid; }

		void GetSourceMips(FArmyTextureSource& Source, IImageWrapperModule* InImageWrapper);

		void GetAsyncSourceMips(IImageWrapperModule* InImageWrapper);

		void ReleaseMemory()
		{

			// Unload BulkData loaded with LoadBulkDataWithFileReader
			AsyncSource.RemoveBulkData();
			Mips.Empty();

		}
		FName TextureName;

		FArmyTextureSource AsyncSource;

		TArray<FImage> Mips;
		int32 NumMips;
		int32 NumSlices;
		ERawImageFormat::Type ImageFormat;

		EGammaSpace GammaSpace;
		bool bValid;
	};

	ITextureCompressorModule* Compressor;
	IImageWrapperModule* ImageWrapper;
	FTexturePlatformData* DerivedData;

	UTexture& Texture;

	/** Compression settings. */
	FArmyTextureBuildSettings BuildSettings;
	/** Derived data key suffix. */
	FString KeySuffix;
	/** Source mip images. */
	FArmyTextureSourceData TextureData;
	/** Source mip images of the composite texture (e.g. normal map for compute roughness). Not necessarily in RGBA32F, usually only top mip as other mips need to be generated first */
	FArmyTextureSourceData CompositeTextureData;
	/** Texture cache flags. */
	uint32 CacheFlags;
	/** Have many bytes were loaded from DDC or built (for telemetry) */
	uint32 BytesCached = 0;

	/** true if caching has succeeded. */
	bool bSucceeded;
	/** true if the derived data was pulled from DDC */
	bool bLoadedFromDDC = false;

	/** Build the texture. This function is safe to call from any thread. */
	void BuildTexture();

public:

	FArmyTextureCachedDerivedDataWorker(ITextureCompressorModule* InCompressor, FTexturePlatformData* InDerivedData,
		UTexture* InTexture,
		const FArmyTextureBuildSettings& InSettings,
		uint32 InCachedFlags,FArmyTextureSource InSource);

	void DoWork();

	void Finalize();

	uint32 GetBytesCached() const
	{
		return BytesCached;
	}

	/** Expose how the resource was returned for telemetry. */
	bool WasLoadedFromDDC() const
	{
		return bLoadedFromDDC;
	}

	FArmyTextureSource Source;

};