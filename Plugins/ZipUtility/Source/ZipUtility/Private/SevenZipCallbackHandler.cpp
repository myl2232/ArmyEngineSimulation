#include "SevenZipCallbackHandler.h"
#include "ZipUtilityPrivatePCH.h"
#include "ZipFileFunctionLibrary.h"
#include "ZipUtilityInterface.h"


void FSevenZipCallbackHandler::OnProgress(const TString& archivePath, uint64 bytes)
{
	const TSharedPtr<IZipUtilityInterface> interfaceDelegate = ProgressDelegate;
	const uint64 bytesConst = bytes;
	const FString pathConst = FString(archivePath.c_str());
	if (bytes > 0)
	{
		const float ProgressPercentage = ((double)((TotalBytes)-(BytesLeft - bytes)) / (double)TotalBytes);
		FZipFileFunctionLibrary::RunLambdaOnGameThread([interfaceDelegate, pathConst, ProgressPercentage, bytesConst]
		{			
			interfaceDelegate->OnProgress(pathConst, ProgressPercentage, bytesConst);
		});
	}
}

void FSevenZipCallbackHandler::OnDone(const TString& archivePath)
{
	const TSharedPtr<IZipUtilityInterface> interfaceDelegate = ProgressDelegate;
	const FString pathConst = FString(archivePath.c_str());
	FZipFileFunctionLibrary::RunLambdaOnGameThread([pathConst, interfaceDelegate]
	{		
		interfaceDelegate->OnDone(pathConst, EZipUtilityCompletionState::SUCCESS);
	});
}

void FSevenZipCallbackHandler::OnFileDone(const TString& archivePath, const TString& filePath, uint64 bytes)
{
	const TSharedPtr<IZipUtilityInterface> interfaceDelegate = ProgressDelegate;
	const FString pathConst = FString(archivePath.c_str());
	const FString filePathConst = FString(filePath.c_str());
	const uint64 bytesConst = bytes;
	FZipFileFunctionLibrary::RunLambdaOnGameThread([interfaceDelegate, pathConst, filePathConst, bytesConst]
	{		
		interfaceDelegate->OnFileDone(pathConst, filePathConst);
	});	
	if (bytes > 0)
	{
		BytesLeft -= bytes;
		const float ProgressPercentage = ((double)(TotalBytes - BytesLeft) / (double)TotalBytes);

		FZipFileFunctionLibrary::RunLambdaOnGameThread([interfaceDelegate, pathConst, ProgressPercentage, bytes]
		{			
			interfaceDelegate->OnProgress(pathConst, ProgressPercentage, bytes);
		});
	}
}
void FSevenZipCallbackHandler::OnStartWithTotal(const TString& archivePath, unsigned __int64 totalBytes)
{
	TotalBytes = totalBytes;
	BytesLeft = TotalBytes;

	const TSharedPtr<IZipUtilityInterface> interfaceDelegate = ProgressDelegate;
	const uint64 bytesConst = TotalBytes;
	const FString pathConst = FString(archivePath.c_str());
	FZipFileFunctionLibrary::RunLambdaOnGameThread([interfaceDelegate, pathConst, bytesConst]
	{		
		interfaceDelegate->OnStartProcess(pathConst, bytesConst);
	});
}
void FSevenZipCallbackHandler::OnFileFound(const TString& archivePath, const TString& filePath, int size)
{
	const TSharedPtr<IZipUtilityInterface> interfaceDelegate = ProgressDelegate;
	const uint64 bytesConst = TotalBytes;
	const FString pathString = FString(archivePath.c_str());
	const FString fileString = FString(filePath.c_str());

	FZipFileFunctionLibrary::RunLambdaOnGameThread([interfaceDelegate, pathString, fileString, bytesConst]
	{
		interfaceDelegate->OnFileFound(pathString, fileString, bytesConst);
	});
}
void FSevenZipCallbackHandler::OnListingDone(const TString& archivePath)
{
	const TSharedPtr<IZipUtilityInterface> interfaceDelegate = ProgressDelegate;
	const FString pathString = FString(archivePath.c_str());

	FZipFileFunctionLibrary::RunLambdaOnGameThread([interfaceDelegate, pathString]
	{
		interfaceDelegate->OnDone(pathString, EZipUtilityCompletionState::SUCCESS);
	});
}

bool FSevenZipCallbackHandler::OnCheckBreak()
{
	return bCancelOperation;
}
