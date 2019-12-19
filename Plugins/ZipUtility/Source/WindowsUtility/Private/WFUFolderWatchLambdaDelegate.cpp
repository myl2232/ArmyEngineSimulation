
#include "WFUFolderWatchLambdaDelegate.h"
#include "WindowsFileUtilityPrivatePCH.h"
#include "WindowsFileUtilityFunctionLibrary.h"


FWFUFolderWatchLambdaDelegate::FWFUFolderWatchLambdaDelegate()
{
	OnFileChangedCallback = nullptr;
}


void FWFUFolderWatchLambdaDelegate::SetOnFileChangedCallback(TFunction<void(FString, FString)> InOnFileChangedCallback)
{
	OnFileChangedCallback = InOnFileChangedCallback;
}


void FWFUFolderWatchLambdaDelegate::OnFileChanged(const FString& FileName, const FString& FilePath)
{
	if (OnFileChangedCallback != nullptr)
	{
		OnFileChangedCallback(FileName, FilePath);
	}
}

void FWFUFolderWatchLambdaDelegate::OnDirectoryChanged(const FString& DirectoryName, const FString& DirectoryPath)
{
	if (OnFileChangedCallback != nullptr)
	{
		OnFileChangedCallback(DirectoryName, DirectoryPath);
	}
}