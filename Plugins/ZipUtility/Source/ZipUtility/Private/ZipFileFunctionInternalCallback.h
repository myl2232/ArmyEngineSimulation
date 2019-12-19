#pragma once

#include "ZipUtilityInterface.h"
#include "ZipFileFunctionLibrary.h"

class IZipUtilityInterface;

/**
* This is used to provide a callback for unzipping single files, it ends up getting called from the ListFiles lambda.
*/
class ZIPUTILITY_API FZipFileFunctionInternalCallback : public IZipUtilityInterface
{

private:

	/** Compression format used to unzip */	
	TEnumAsByte<EZipUtilityCompressionFormat> CompressionFormat;

	/** Path of the file */	
	FString File;

	FString DestinationFolder;

	/** Current File index parsed */	
	int32 FileIndex = 0;

	/** Callback object */	
	TSharedPtr<IZipUtilityInterface> Callback;

	bool bSingleFile;	
	bool bFileFound;	
	bool bUnzipto;

public:
	FZipFileFunctionInternalCallback();
	virtual ~FZipFileFunctionInternalCallback() = default;

	//IZipUtilityInterface overrides
	virtual void OnProgress(const FString& archive, float percentage, int32 bytes) override {};

	virtual void OnDone(const FString& archive, EZipUtilityCompletionState CompletionState) override {};

	virtual void OnStartProcess(const FString& archive, int32 bytes) override {};

	virtual void OnFileDone(const FString& archive, const FString& file) override {};

	virtual void OnFileFound(const FString& archive, const FString& fileIn, int32 size) override;

	void SetCallback(const FString& FileName, TSharedPtr<IZipUtilityInterface> CallbackIn, TEnumAsByte<EZipUtilityCompressionFormat> CompressionFormatIn = EZipUtilityCompressionFormat::COMPRESSION_FORMAT_UNKNOWN);
	void SetCallback(const FString& FileName, const FString& DestinationFolder, TSharedPtr<IZipUtilityInterface> CallbackIn, TEnumAsByte<EZipUtilityCompressionFormat> CompressionFormatIn = EZipUtilityCompressionFormat::COMPRESSION_FORMAT_UNKNOWN);

	FORCEINLINE bool GetSingleFile() const { return bSingleFile; }
	FORCEINLINE void SetSingleFile(bool val) { bSingleFile = val; }
};