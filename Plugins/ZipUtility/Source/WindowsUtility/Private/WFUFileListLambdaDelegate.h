
#pragma once

#include "CoreMinimal.h"
#include "WFUFileListInterface.h"

class WINDOWSFILEUTILITY_API FWFUFileListLambdaDelegate : public IWFUFileListInterface
{

public:
	FWFUFileListLambdaDelegate();
	virtual ~FWFUFileListLambdaDelegate() = default;
public:
	void SetOnDoneCallback(TFunction<void(const TArray<FString>&, const TArray<FString>&)> InOnDoneCallback);

protected:
	//File List Interface

	virtual void OnListFileFound(const FString& FileName, int32 ByteCount, const FString& FilePath) override;
	virtual void OnListDirectoryFound(const FString& DirectoryName, const FString& FilePath) override;
	virtual void OnListDone(const FString& DirectoryPath, const TArray<FString>& Files, const TArray<FString>& Folders) override;

	TFunction<void(const TArray<FString>&, const TArray<FString>&)> OnDoneCallback;
};