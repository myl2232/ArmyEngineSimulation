#include "ZULambdaDelegate.h"
#include "ZipUtilityPrivatePCH.h"
#include "WindowsFileUtilityFunctionLibrary.h"
#include "ZipFileFunctionLibrary.h"


FZULambdaDelegate::FZULambdaDelegate()
{
	OnDoneCallback = nullptr;
	OnProgressCallback = nullptr;
}

void FZULambdaDelegate::SetOnDoneCallback(TFunction<void()> InOnDoneCallback)
{
	OnDoneCallback = InOnDoneCallback;
}

void FZULambdaDelegate::SetOnProgessCallback(TFunction<void(float)> InOnProgressCallback)
{
	OnProgressCallback = InOnProgressCallback;
}

void FZULambdaDelegate::OnProgress(const FString& archive, float percentage, int32 bytes)
{
	if (OnProgressCallback != nullptr)
	{
		OnProgressCallback(percentage);
	}
}

void FZULambdaDelegate::OnDone(const FString& archive, EZipUtilityCompletionState CompletionState)
{
	if (OnDoneCallback != nullptr)
	{
		OnDoneCallback();
	}

}

void FZULambdaDelegate::OnStartProcess(const FString& archive, int32 bytes)
{

}

void FZULambdaDelegate::OnFileDone(const FString& archive, const FString& file)
{

}

void FZULambdaDelegate::OnFileFound(const FString& archive, const FString& file, int32 size)
{

}

