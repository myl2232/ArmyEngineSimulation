
#include "WFUFileListLambdaDelegate.h"
#include "WindowsFileUtilityPrivatePCH.h"
#include "WindowsFileUtilityFunctionLibrary.h"


FWFUFileListLambdaDelegate::FWFUFileListLambdaDelegate()
{
	OnDoneCallback = nullptr;
}

void FWFUFileListLambdaDelegate::SetOnDoneCallback(TFunction<void(const TArray<FString>&, const TArray<FString>&)> InOnDoneCallback)
{
	OnDoneCallback = InOnDoneCallback;
}

void FWFUFileListLambdaDelegate::OnListFileFound(const FString& FileName, int32 ByteCount, const FString& FilePath)
{

}

void FWFUFileListLambdaDelegate::OnListDirectoryFound(const FString& DirectoryName, const FString& FilePath)
{

}

void FWFUFileListLambdaDelegate::OnListDone(const FString& DirectoryPath, const TArray<FString>& Files, const TArray<FString>& Folders)
{
	if (OnDoneCallback != nullptr)
	{
		OnDoneCallback(Files, Folders);
	}
}