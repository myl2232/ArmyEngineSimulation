
#pragma once

#include "CoreMinimal.h"
#include "WFUFolderWatchInterface.h"

class WINDOWSFILEUTILITY_API FWFUFolderWatchLambdaDelegate :  public IWFUFolderWatchInterface
{
public:
	virtual ~FWFUFolderWatchLambdaDelegate() = default;
	FWFUFolderWatchLambdaDelegate();
public:
	void SetOnFileChangedCallback(TFunction<void(FString, FString)> InOnFileChangedCallback);

protected:
	TFunction<void(FString, FString)> OnFileChangedCallback;
	
	//IWFUFolderWatchInterface
	virtual void OnFileChanged(const FString& FileName, const FString& FilePath) override;
	virtual void OnDirectoryChanged(const FString& DirectoryName, const FString& DirectoryPath) override;
};