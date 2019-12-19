#include "ZipFileFunctionInternalCallback.h"
#include "ZipUtilityPrivatePCH.h"

FZipFileFunctionInternalCallback::FZipFileFunctionInternalCallback()
{
	CompressionFormat = EZipUtilityCompressionFormat::COMPRESSION_FORMAT_UNKNOWN;
	DestinationFolder = FString();
	File = FString();
	FileIndex = 0;
}

void FZipFileFunctionInternalCallback::OnFileFound(const FString& archive, const FString& fileIn, int32 size)
{
	if (!bFileFound && fileIn.ToLower().Contains(File.ToLower()))
	{
		TArray<int32> FileIndices = { FileIndex };
		
		if (bUnzipto)
		{
			FZipFileFunctionLibrary::UnzipFilesTo(FileIndices, archive, DestinationFolder, Callback, CompressionFormat);
		}
		else
		{
			FZipFileFunctionLibrary::UnzipFiles(FileIndices, archive, Callback, CompressionFormat);
		}

		if (bSingleFile)
		{
			bFileFound = true;
		}
	}

	FileIndex++;
}

void FZipFileFunctionInternalCallback::SetCallback(const FString& FileName, TSharedPtr<IZipUtilityInterface> CallbackIn, TEnumAsByte<EZipUtilityCompressionFormat> CompressionFormatIn /*= ZipUtilityCompressionFormat::COMPRESSION_FORMAT_UNKNOWN*/)
{
	File = FileName;
	Callback = CallbackIn;
	CompressionFormat = CompressionFormatIn;
	FileIndex = 0;
}

void FZipFileFunctionInternalCallback::SetCallback(const FString& FileName, const FString& DestinationFolderIn, TSharedPtr<IZipUtilityInterface> CallbackIn, TEnumAsByte<EZipUtilityCompressionFormat> CompressionFormatIn /*= ZipUtilityCompressionFormat::COMPRESSION_FORMAT_UNKNOWN*/)
{
	SetCallback(FileName, CallbackIn, CompressionFormatIn);

	bUnzipto = true;
	DestinationFolder = DestinationFolderIn;
}