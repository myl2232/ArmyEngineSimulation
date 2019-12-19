#pragma once
#include "CoreMinimal.h"

struct FArmyTextureFormatCompressorCaps
{
	 FArmyTextureFormatCompressorCaps() :
		MaxTextureDimension(TNumericLimits<uint32>::Max())
	{

	}

	uint32 MaxTextureDimension;
};
class IArmyTextureFormat
{
public:

	virtual bool AllowParalleBuild()const
	{
		return false;
	}

	virtual uint16 GetVersion(FName Format, const struct FArmyTextureBuildSettings* BuildSettting = nullptr)const = 0;

	virtual FString GetDerivedDataKeyString(const class UTexture& Texture)const
	{
		return TEXT("");
	}

	virtual void GetSupportedFormats(TArray<FName>& OutFormats) = 0;

	virtual FArmyTextureFormatCompressorCaps GetFormatCapabilities()const = 0;

	virtual bool CompressImage(
		const struct FImage& Image,
		const struct FArmyTextureBuildSettings& BuildSettings,
		bool bImageHasAlphaChannel,
		struct FArmyCompressedImage2D& OutCompressedImage
	) const = 0;

public:
	virtual ~IArmyTextureFormat() {}
};