#pragma once

#include "IArmyTextureFormat.h"
#include "ArmyTextureCompressorModule.h"
THIRD_PARTY_INCLUDES_START
#include "nvtt/nvtt.h"
THIRD_PARTY_INCLUDES_END
#include "Containers/IndirectArray.h"
#include "PixelFormat.h"
#include "Misc/ScopeLock.h"
#include "ImageCore.h"

/**
* NVTT output handler.
*/
struct FArmyNVOutputHandler : public nvtt::OutputHandler
{
	explicit FArmyNVOutputHandler(uint8* InBuffer, int32 InBufferSize)
		: Buffer(InBuffer)
		, BufferEnd(InBuffer + InBufferSize)
	{
	}

	~ FArmyNVOutputHandler()
	{
	}

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
	{
	}

	virtual bool writeData(const void* data, int size)
	{
		check(data);
		check(Buffer + size <= BufferEnd);
		FMemory::Memcpy(Buffer, data, size);
		Buffer += size;
		return true;
	}

	virtual void endImage()
	{
	}

	uint8* Buffer;
	uint8* BufferEnd;
};


/**
* NVTT error handler.
*/
struct FArmyNVErrorHandler : public nvtt::ErrorHandler
{
	 FArmyNVErrorHandler() :
		bSuccess(true)
	{}

	virtual void error(nvtt::Error e)
	{
		//	UE_LOG(LogTextureFormatDXT, Warning, TEXT("nvtt::compress() failed with error '%s'"), ANSI_TO_TCHAR(nvtt::errorString(e)));
		bSuccess = false;
	}

	bool bSuccess;
};


/**
* All state objects needed for NVTT.
*/
class FArmyNVTTCompressor
{
	 FArmyNVOutputHandler			OutputHandler;
	 FArmyNVErrorHandler				ErrorHandler;
	nvtt::InputOptions			InputOptions;
	nvtt::CompressionOptions	CompressionOptions;
	nvtt::OutputOptions			OutputOptions;
	nvtt::Compressor			Compressor;

public:
	/** Initialization constructor. */
	 FArmyNVTTCompressor(
		const void* SourceData,
		EPixelFormat PixelFormat,
		int32 SizeX,
		int32 SizeY,
		bool bSRGB,
		bool bIsNormalMap,
		uint8* OutBuffer,
		int32 BufferSize,
		bool bPreview = false)
		: OutputHandler(OutBuffer, BufferSize)
	{
		// CUDA acceleration currently disabled, needs more robust error handling
		// With one core of a Xeon 3GHz CPU, compressing a 2048^2 normal map to DXT1 with NVTT 2.0.4 takes 7.49s.
		// With the same settings but using CUDA and a Geforce 8800 GTX it takes 1.66s.
		// To use CUDA, a CUDA 2.0 capable driver is required (178.08 or greater) and a Geforce 8 or higher.
		const bool bUseCUDAAcceleration = false;

		// DXT1a support is currently not exposed.
		const bool bSupportDXT1a = false;

		// Quality level is hardcoded to production quality for now.
		const nvtt::Quality QualityLevel = bPreview ? nvtt::Quality_Fastest : nvtt::Quality_Production;

		nvtt::Format TextureFormat = nvtt::Format_DXT1;
		if (PixelFormat == PF_DXT1)
		{
			TextureFormat = bSupportDXT1a ? nvtt::Format_DXT1a : nvtt::Format_DXT1;
		}
		else if (PixelFormat == PF_DXT3)
		{
			TextureFormat = nvtt::Format_DXT3;
		}
		else if (PixelFormat == PF_DXT5 && bIsNormalMap)
		{
			TextureFormat = nvtt::Format_DXT5n;
		}
		else if (PixelFormat == PF_DXT5)
		{
			TextureFormat = nvtt::Format_DXT5;
		}
		else if (PixelFormat == PF_B8G8R8A8)
		{
			TextureFormat = nvtt::Format_RGBA;
		}
		else if (PixelFormat == PF_BC4)
		{
			TextureFormat = nvtt::Format_BC4;
		}
		else if (PixelFormat == PF_BC5)
		{
			TextureFormat = nvtt::Format_BC5;
		}
		else
		{

		}

		InputOptions.setTextureLayout(nvtt::TextureType_2D, SizeX, SizeY);

		// Not generating mips with NVTT, we will pass each mip in and compress it individually
		InputOptions.setMipmapGeneration(false, -1);
		verify(InputOptions.setMipmapData(SourceData, SizeX, SizeY));

		if (bSRGB)
		{
			InputOptions.setGamma(2.2f, 2.2f);
		}
		else
		{
			InputOptions.setGamma(1.0f, 1.0f);
		}

		// Only used for mip and normal map generation
		InputOptions.setWrapMode(nvtt::WrapMode_Mirror);
		InputOptions.setFormat(nvtt::InputFormat_BGRA_8UB);

		// Highest quality is 2x slower with only a small visual difference
		// Might be worthwhile for normal maps though
		CompressionOptions.setQuality(QualityLevel);
		CompressionOptions.setFormat(TextureFormat);

		if (bIsNormalMap)
		{
			// For BC5 normal maps we don't care about the blue channel.
			CompressionOptions.setColorWeights(1.0f, 1.0f, 0.0f);

			// Don't tell NVTT it's a normal map. It was producing noticeable artifacts during BC5 compression.
			//InputOptions.setNormalMap(true);
		}
		else
		{
			CompressionOptions.setColorWeights(1, 1, 1);
		}

		Compressor.enableCudaAcceleration(bUseCUDAAcceleration);
		//OutputHandler.ReserveMemory( Compressor.estimateSize(InputOptions, CompressionOptions) );
		check(OutputHandler.BufferEnd - OutputHandler.Buffer <= Compressor.estimateSize(InputOptions, CompressionOptions));

		// We're not outputting a dds file so disable the header
		OutputOptions.setOutputHeader(false);
		OutputOptions.setOutputHandler(&OutputHandler);
		OutputOptions.setErrorHandler(&ErrorHandler);
	}

	/** Run the compressor. */
	bool Compress()
	{
		return Compressor.process(InputOptions, CompressionOptions, OutputOptions) && ErrorHandler.bSuccess;
	}
};
class FArmyAsyncNVTTWorker :public FNonAbandonableTask
{
public:
	 FArmyAsyncNVTTWorker( FArmyNVTTCompressor* InCompressor) :
		Compressor(InCompressor)
	{
		check(Compressor);
	}

	void DoWork()
	{
		bCompressionResults = Compressor->Compress();
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT( FArmyAsyncNVTTWorker, STATGROUP_ThreadPoolAsyncTasks);
	}

	bool GetCompressionResults()const { return bCompressionResults; }

private:

	 FArmyNVTTCompressor* Compressor;
	bool bCompressionResults;

};

typedef FAsyncTask< FArmyAsyncNVTTWorker> FAsyncNVTTTask;

class FArmyTextureFormatDXT : public IArmyTextureFormat
{
public:
	virtual bool AllowParalleBuild()const override { return true; }

	virtual uint16 GetVersion(FName Format, const struct FArmyTextureBuildSettings* BuildSettting /* = nullptr */)const override { return 0; }


	virtual void GetSupportedFormats(TArray<FName>& OutFormats) override;

	virtual FArmyTextureFormatCompressorCaps GetFormatCapabilities() const override
	{
		return FArmyTextureFormatCompressorCaps(); // Default capabilities.
	}
	virtual bool CompressImage(
		const FImage& InImage,
		const struct FArmyTextureBuildSettings& BuildSettings,
		bool bImageHasAlphaChannel,
		 FArmyCompressedImage2D& OutCompressedImage
	) const override;




};