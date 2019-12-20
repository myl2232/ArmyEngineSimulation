#include "ArmyTextureSource.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
FArmyTextureSource::FArmyTextureSource() :
	LockedMipData(NULL),
	LockedMips(0),
	bHasHadBulkDataCleared(false),
	SizeX(0),
	SizeY(0),
	NumMips(0),
	bPNGCompressed(false),
	bGuidIsHash(false),
	Format(TSF_Invalid)
{

}

void FArmyTextureSource::Init(int32 NewSizeX, int32 NewSizeY, int32 NewNumSlices, int32 NewNumMips, ETextureSourceFormat NewFormat, const uint8* NewData /*= NULL */)
{
	RemoveSourceData();
	SizeX = NewSizeX;
	SizeY = NewSizeY;
	NumSlices = NewNumSlices;
	NumMips = NewNumMips;
	Format = NewFormat;

	int32 TotalBytes = 0;
	int32 BytesPerPixel = GetBytesPerPixel();
	int32 MipSizeX = SizeX;
	int32 MipSizeY = SizeY;

	while (NewNumMips-- > 0)
	{
		TotalBytes += MipSizeX * MipSizeY * NumSlices * BytesPerPixel;
		MipSizeX = FMath::Max(MipSizeX >> 1, 1);
		MipSizeY = FMath::Max(MipSizeY >> 1, 1);
	}

	BulkData.Lock(LOCK_READ_WRITE);
	uint8* DestData = (uint8*)BulkData.Realloc(TotalBytes);
	if (NewData)
	{
		FMemory::Memcpy(DestData, NewData, TotalBytes);
	}
	BulkData.Unlock();
}

void FArmyTextureSource::ForceGenerateGuid()
{
	Id = FGuid::NewGuid();
	bGuidIsHash = false;
}

void FArmyTextureSource::RemoveSourceData()
{
	SizeX = 0;
	SizeY = 0;
	NumSlices = 0;
	NumMips = 0;
	Format = TSF_Invalid;
	bPNGCompressed = false;
	LockedMipData = NULL;
	LockedMips = 0;
	if (BulkData.IsLocked())
	{
		BulkData.Unlock();
	}
	BulkData.RemoveBulkData();
	ForceGenerateGuid();
}

uint8* FArmyTextureSource::LockMip(int32 MipIndex)
{
	uint8* MipData = NULL;
	if (MipIndex < NumMips)
	{
		if (LockedMipData == NULL)
		{
			LockedMipData = (uint8*)BulkData.Lock(LOCK_READ_WRITE);
			if (bPNGCompressed)
			{
				bool bCanPngCompressFormat = (Format == TSF_G8 || Format == TSF_RGBA8 || Format == TSF_BGRA8 || Format == TSF_RGBA16);
				check(NumSlices == 1 && bCanPngCompressFormat);
				if (MipIndex != 0)
				{
					return NULL;
				}

				IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
				TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(LockedMipData, BulkData.GetBulkDataSize()))
				{
					check(ImageWrapper->GetWidth() == SizeX);
					check(ImageWrapper->GetHeight() == SizeY);
					const TArray<uint8>* RawData = NULL;
					// TODO: TSF_BGRA8 is stored as RGBA, so the R and B channels are swapped in the internal png. Should we fix this?
					ERGBFormat RawFormat = (Format == TSF_G8) ? ERGBFormat::Gray : ERGBFormat::RGBA;
					if (ImageWrapper->GetRaw(RawFormat, Format == TSF_RGBA16 ? 16 : 8, RawData))
					{
						if (RawData->Num() > 0)
						{
							LockedMipData = (uint8*)FMemory::Malloc(RawData->Num());
							// PVS-Studio does not understand that IImageWrapper::GetRaw's return value validates the pointer, so we disable
							// the warning that we are using the RawData pointer before checking for null:
							FMemory::Memcpy(LockedMipData, RawData->GetData(), RawData->Num()); //-V595
						}
					}
					if (RawData == NULL || RawData->Num() == 0)
					{
						UE_LOG(LogTexture, Warning, TEXT("PNG decompression of source art failed"));
					}
				}
				else
				{
					UE_LOG(LogTexture, Log, TEXT("Only pngs are supported"));
				}
			}
		}

		MipData = LockedMipData + CalcMipOffset(MipIndex);
		LockedMips |= (1 << MipIndex);
	}
	return MipData;
}

void FArmyTextureSource::UnlockMip(int32 MipIndex)
{
	check(MipIndex < MAX_TEXTURE_MIP_COUNT);

	uint32 LockBit = 1 << MipIndex;
	if (LockedMips & LockBit)
	{
		LockedMips &= (~LockBit);
		if (LockedMips == 0)
		{
			if (bPNGCompressed)
			{
				check(MipIndex == 0);
				int32 MipSize = CalcMipSize(0);
				uint8* UncompressedData = (uint8*)BulkData.Realloc(MipSize);
				FMemory::Memcpy(UncompressedData, LockedMipData, MipSize);
				FMemory::Free(LockedMipData);
				bPNGCompressed = false;
			}
			LockedMipData = NULL;
			BulkData.Unlock();
			ForceGenerateGuid();
		}
	}
}

int32 FArmyTextureSource::GetBytesPerPixel() const
{
	int32 BytesPerPixel = 0;
	switch (Format)
	{
	case TSF_G8:		BytesPerPixel = 1; break;
	case TSF_BGRA8:		BytesPerPixel = 4; break;
	case TSF_BGRE8:		BytesPerPixel = 4; break;
	case TSF_RGBA16:	BytesPerPixel = 8; break;
	case TSF_RGBA16F:	BytesPerPixel = 8; break;
	default:			BytesPerPixel = 0; break;
	}
	return BytesPerPixel;
}

bool FArmyTextureSource::GetMipData(TArray64<uint8>& OutMipData, int32 MipIndex, class IImageWrapperModule* ImageWrapperModule /*= nullptr*/)
{
	bool bSuccess = false;
	if (MipIndex < NumMips && BulkData.GetBulkDataSize() > 0)
	{
		void* RawSourceData = BulkData.Lock(LOCK_READ_ONLY);
		if (bPNGCompressed)
		{
			bool bCanPngCompressFormat = (Format == TSF_G8 || Format == TSF_RGBA8 || Format == TSF_BGRA8 || Format == TSF_RGBA16);
			if (MipIndex == 0 && NumSlices == 1 && bCanPngCompressFormat)
			{
				if (!ImageWrapperModule) // Optional if called from the gamethread, see FModuleManager::WarnIfItWasntSafeToLoadHere()
				{
					ImageWrapperModule = &FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
				}

				TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule->CreateImageWrapper(EImageFormat::PNG);

				if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawSourceData, BulkData.GetBulkDataSize()))
				{
					if (ImageWrapper->GetWidth() == SizeX
						&& ImageWrapper->GetHeight() == SizeY)
					{
						const TArray<uint8>* RawData = NULL;
						// TODO: TSF_BGRA8 is stored as RGBA, so the R and B channels are swapped in the internal png. Should we fix this?
						ERGBFormat RawFormat = (Format == TSF_G8) ? ERGBFormat::Gray : ERGBFormat::RGBA;
						if (ImageWrapper->GetRaw(RawFormat, Format == TSF_RGBA16 ? 16 : 8, RawData))
						{
							OutMipData = *RawData;
							bSuccess = true;
						}
						else
						{
							UE_LOG(LogTexture, Warning, TEXT("PNG decompression of source art failed"));
							OutMipData.Empty();
						}
					}
					else
					{
						UE_LOG(LogTexture, Warning,
							TEXT("PNG decompression of source art failed. ")
							TEXT("Source image should be %dx%d but is %dx%d"),
							SizeX, SizeY,
							ImageWrapper->GetWidth(), ImageWrapper->GetHeight()
						);
					}
				}
				else
				{
					UE_LOG(LogTexture, Log, TEXT("Only pngs are supported"));
				}
			}
		}
		else
		{
			int32 MipOffset = CalcMipOffset(MipIndex);
			int32 MipSize = CalcMipSize(MipIndex);
			if (BulkData.GetBulkDataSize() >= MipOffset + MipSize)
			{
				OutMipData.Empty(MipSize);
				OutMipData.AddUninitialized(MipSize);
				FMemory::Memcpy(
					OutMipData.GetData(),
					(uint8*)RawSourceData + MipOffset,
					MipSize
				);
			}
			bSuccess = true;
		}
		BulkData.Unlock();
	}
	return bSuccess;
}

int32 FArmyTextureSource::CalcMipSize(int32 MipIndex) const
{
	int32 MipSizeX = FMath::Max(SizeX >> MipIndex, 1);
	int32 MipSizeY = FMath::Max(SizeY >> MipIndex, 1);
	int32 BytesPerPixel = GetBytesPerPixel();
	return MipSizeX * MipSizeY * NumSlices * BytesPerPixel;
}

int32 FArmyTextureSource::CalcMipOffset(int32 MipIndex) const
{
	int32 MipOffset = 0;
	int32 BytesPerPixel = GetBytesPerPixel();
	int32 MipSizeX = SizeX;
	int32 MipSizeY = SizeY;

	while (MipIndex-- > 0)
	{
		MipOffset += MipSizeX * MipSizeY * BytesPerPixel * NumSlices;
		MipSizeX = FMath::Max(MipSizeX >> 1, 1);
		MipSizeY = FMath::Max(MipSizeY >> 1, 1);
	}

	return MipOffset;
}

