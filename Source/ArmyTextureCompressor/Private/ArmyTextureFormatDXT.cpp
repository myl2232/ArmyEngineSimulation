#include "ArmyTextureFormatDXT.h"
#include "Stats/Stats.h"
/**
* Macro trickery for supported format names.
*/
#define ENUM_SUPPORTED_FORMATS(op) \
	op(DXT1) \
	op(DXT3) \
	op(DXT5) \
	op(AutoDXT) \
	op(DXT5n) \
	op(BC4)	\
	op(BC5)

#define DECL_FORMAT_NAME(FormatName) static FName GTextureFormatName##FormatName = FName(TEXT(#FormatName));
ENUM_SUPPORTED_FORMATS(DECL_FORMAT_NAME);
#undef DECL_FORMAT_NAME

#define DECL_FORMAT_NAME_ENTRY(FormatName) GTextureFormatName##FormatName ,
static FName GSupportedTextureFormatNames[] =
{
	ENUM_SUPPORTED_FORMATS(DECL_FORMAT_NAME_ENTRY)
};
#undef DECL_FORMAT_NAME_ENTRY

#undef ENUM_SUPPORTED_FORMATS
void FArmyTextureFormatDXT::GetSupportedFormats(TArray<FName>& OutFormats) 
{
	for (int32 i = 0; i < ARRAY_COUNT(GSupportedTextureFormatNames); ++i)
	{
		OutFormats.Add(GSupportedTextureFormatNames[i]);
	}
}

/** Critical section to isolate construction of nvtt objects */
FCriticalSection GNVCompressionCriticalSection;
namespace CompressionSettings
{
	int32 BlocksPerBatch = 2048;
	FAutoConsoleVariableRef BlocksPerBatch_CVar(
		TEXT("Tex.AsyncDXTBlocksPerBatch"),
		BlocksPerBatch,
		TEXT("The number of blocks to compress in parallel for DXT compression.")
	);
}
static bool CompressImageUsingNVTT(
	const void* SourceData,
	EPixelFormat PixelFormat,
	int32 SizeX,
	int32 SizeY,
	bool bSRGB,
	bool bIsNormalMap,
	bool bIsPreview,
	TArray<uint8>& OutCompressedData
)
{
	check(PixelFormat == PF_DXT1 || PixelFormat == PF_DXT3 || PixelFormat == PF_DXT5 || PixelFormat == PF_BC4 || PixelFormat == PF_BC5);
	const int32 BlockSizeX = 4;
	const int32 BlockSizeY = 4;
	const int32 BlockBytes = (PixelFormat == PF_DXT1 || PixelFormat == PF_BC4) ? 8 : 16;
	const int32 ImageBlocksX = FMath::Max(SizeX / BlockSizeX, 1);
	const int32 ImageBlocksY = FMath::Max(SizeY / BlockSizeY, 1);
	const int32 BlocksPerBatch = FMath::Max<int32>(ImageBlocksX, FMath::RoundUpToPowerOfTwo(CompressionSettings::BlocksPerBatch));
	const int32 RowsPerBatch = BlocksPerBatch / ImageBlocksX;
	const int32 NumBatches = ImageBlocksY / RowsPerBatch;

	// Allocate space to store compressed data.
	OutCompressedData.Empty(ImageBlocksX * ImageBlocksY * BlockBytes);
	OutCompressedData.AddUninitialized(ImageBlocksX * ImageBlocksY * BlockBytes);

	if (ImageBlocksX * ImageBlocksY <= BlocksPerBatch ||
		BlocksPerBatch % ImageBlocksX != 0 ||
		RowsPerBatch * NumBatches != ImageBlocksY)
	{
		 FArmyNVTTCompressor* Compressor = NULL;

		{
			FScopeLock ScopeLock(&GNVCompressionCriticalSection);
			Compressor = new FArmyNVTTCompressor(
				SourceData,
				PixelFormat,
				SizeX,
				SizeY,
				bSRGB,
				bIsNormalMap,
				OutCompressedData.GetData(),
				OutCompressedData.Num(),
				bIsPreview
			);
		}

		bool bSuccess = Compressor->Compress();
		{
			FScopeLock ScopeLock(&GNVCompressionCriticalSection);
			delete Compressor;
			Compressor = NULL;
		}
		return bSuccess;
	}

	int32 UncompressedStride = RowsPerBatch * BlockSizeY * SizeX * sizeof(FColor);
	int32 CompressedStride = RowsPerBatch * ImageBlocksX * BlockBytes;

	// Create compressors for each batch.
	TIndirectArray< FArmyNVTTCompressor> Compressors;
	Compressors.Empty(NumBatches);
	{
		FScopeLock ScopeLock(&GNVCompressionCriticalSection);
		const uint8* Src = (const uint8*)SourceData;
		uint8* Dest = OutCompressedData.GetData();
		for (int32 BatchIndex = 0; BatchIndex < NumBatches; ++BatchIndex)
		{
			new(Compressors) FArmyNVTTCompressor(
				Src,
				PixelFormat,
				SizeX,
				RowsPerBatch * BlockSizeY,
				bSRGB,
				bIsNormalMap,
				Dest,
				CompressedStride
			);
			Src += UncompressedStride;
			Dest += CompressedStride;
		}
	}

	// Asynchronously compress each batch.
	bool bSuccess = true;
	{
		TIndirectArray<FAsyncNVTTTask> AsyncTasks;
		for (int32 BatchIndex = 0; BatchIndex < NumBatches; ++BatchIndex)
		{
			FAsyncNVTTTask* AsyncTask = new(AsyncTasks) FAsyncNVTTTask(&Compressors[BatchIndex]);
#if WITH_EDITOR
			AsyncTask->StartBackgroundTask(GLargeThreadPool);
#else
			AsyncTask->StartBackgroundTask();
#endif
		}
		for (int32 BatchIndex = 0; BatchIndex < NumBatches; ++BatchIndex)
		{
			FAsyncNVTTTask& AsyncTask = AsyncTasks[BatchIndex];
			AsyncTask.EnsureCompletion();
			bSuccess = bSuccess && AsyncTask.GetTask().GetCompressionResults();
		}
	}

	// Release compressors
	{
		FScopeLock ScopeLock(&GNVCompressionCriticalSection);
		Compressors.Empty();
	}

	return bSuccess;

    
}
bool FArmyTextureFormatDXT::CompressImage(const FImage& InImage, const struct FArmyTextureBuildSettings& BuildSettings, bool bImageHasAlphaChannel, FArmyCompressedImage2D& OutCompressedImage) const
{
	FImage Image;
	InImage.CopyTo(Image, ERawImageFormat::BGRA8, BuildSettings.GetGammaSpace());

	bool bIsNormalMap = false;
	EPixelFormat CompressedPixelFormat = PF_Unknown;
	if (BuildSettings.TextureFormatName == GTextureFormatNameDXT1)
	{
		CompressedPixelFormat = PF_DXT1;
	}
	else if (BuildSettings.TextureFormatName == GTextureFormatNameDXT3)
	{
		CompressedPixelFormat = PF_DXT3;
	}
	else if (BuildSettings.TextureFormatName == GTextureFormatNameDXT5)
	{
		CompressedPixelFormat = PF_DXT5;
	}
	else if (BuildSettings.TextureFormatName == GTextureFormatNameAutoDXT)
	{
		CompressedPixelFormat = bImageHasAlphaChannel ? PF_DXT5 : PF_DXT1;
	}
	else if (BuildSettings.TextureFormatName == GTextureFormatNameDXT5n)
	{
		CompressedPixelFormat = PF_DXT5;
		bIsNormalMap = true;
	}
	else if (BuildSettings.TextureFormatName == GTextureFormatNameBC5)
	{
		CompressedPixelFormat = PF_BC5;
		bIsNormalMap = true;
	}
	else if (BuildSettings.TextureFormatName == GTextureFormatNameBC4)
	{
		CompressedPixelFormat = PF_BC4;
	}

	bool bCompressionSucceeded = true;
	int32 SliceSize = Image.SizeX * Image.SizeY;
	for (int32 SliceIndex = 0; SliceIndex < Image.NumSlices && bCompressionSucceeded; ++SliceIndex)
	{
		TArray<uint8> CompressedSliceData;
		bCompressionSucceeded = CompressImageUsingNVTT(
			Image.AsBGRA8() + SliceIndex * SliceSize,
			CompressedPixelFormat,
			Image.SizeX,
			Image.SizeY,
			Image.IsGammaCorrected(),
			bIsNormalMap,
			false, // Daniel Lamb: Testing with this set to true didn't give large performance gain to lightmaps.  Encoding of 140 lightmaps was 19.2seconds with preview 20.1 without preview.  11/30/2015
			CompressedSliceData
		);
		OutCompressedImage.RawData.Append(CompressedSliceData);
	}

	if (bCompressionSucceeded)
	{
		OutCompressedImage.SizeX = FMath::Max(Image.SizeX, 4);
		OutCompressedImage.SizeY = FMath::Max(Image.SizeY, 4);
		OutCompressedImage.PixelFormat = CompressedPixelFormat;
	}
	return bCompressionSucceeded;
}

