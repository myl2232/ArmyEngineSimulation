#pragma  once
#include "Engine/Texture.h"

class FArmyTextureSource
{
public:
	FArmyTextureSource();
	void Init(
		int32 NewSizeX,
		int32 NewSizeY,
		int32 NewNumSlices,
		int32 NewNumMips,
		ETextureSourceFormat NewFormat,
		const uint8* NewData = NULL
	);
	/** Force the GUID to change even if mip data has not been modified. */
	void ForceGenerateGuid();
	void RemoveSourceData();
	/** Lock a mip for editing. */
	uint8* LockMip(int32 MipIndex);
	ETextureSourceFormat GetFormat() const { return Format; }
	/** Unlock a mip. */
	void UnlockMip(int32 MipIndex);
	int32 GetBytesPerPixel() const;
	bool GetMipData(TArray64<uint8>& OutMipData, int32 MipIndex, class IImageWrapperModule* ImageWrapperModule = nullptr);
	FORCEINLINE FGuid GetId() const { return Id; }
	FORCEINLINE int32 GetSizeX() const { return SizeX; }
	FORCEINLINE int32 GetSizeY() const { return SizeY; }
	FORCEINLINE int32 GetNumSlices() const { return NumSlices; }
	FORCEINLINE int32 GetNumMips() const { return NumMips; }
	FORCEINLINE void RemoveBulkData() { BulkData.RemoveBulkData(); }
	FORCEINLINE bool IsBulkDataLoaded() const { return BulkData.IsBulkDataLoaded(); }
	FORCEINLINE bool HasHadBulkDataCleared() const { return bHasHadBulkDataCleared; }
	FORCEINLINE int32 GetSizeOnDisk() const { return BulkData.GetBulkDataSize(); }
	FORCEINLINE bool LoadBulkDataWithFileReader() { return BulkData.LoadBulkDataWithFileReader(); }
	int32 CalcMipSize(int32 MipIndex) const;
	int32 CalcMipOffset(int32 MipIndex) const;
private:
	/** The bulk source data. */
	FByteBulkData BulkData;
	/** Pointer to locked mip data, if any. */
	uint8* LockedMipData;
	/** Which mips are locked, if any. */
	uint32 LockedMips;
	/** Used while cooking to clear out unneeded memory after compression */
	bool bHasHadBulkDataCleared;
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		FGuid Id;

	/** Width of the texture. */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		int32 SizeX;

	/** Height of the texture. */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		int32 SizeY;

	/** Depth (volume textures) or faces (cube maps). */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		int32 NumSlices;

	/** Number of mips provided as source data for the texture. */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		int32 NumMips;

	/** RGBA8 source data is optionally compressed as PNG. */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		bool bPNGCompressed;

	/** Legacy textures use a hash instead of a GUID. */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		bool bGuidIsHash;

	/** Format in which the source data is stored. */
	UPROPERTY(VisibleAnywhere, Category = TextureSource)
		TEnumAsByte<enum ETextureSourceFormat> Format;

};