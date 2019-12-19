
#pragma once

#include "CoreMinimal.h"
#include "ZipUtilityInterface.h"

class ZIPUTILITY_API FZULambdaDelegate : public IZipUtilityInterface
{

public:
	FZULambdaDelegate();
	virtual ~FZULambdaDelegate() = default;
public:
	void SetOnDoneCallback(TFunction<void()> InOnDoneCallback);
	void SetOnProgessCallback(TFunction<void(float)> InOnProgressCallback);

protected:
	//Zip utility interface
	virtual void OnProgress(const FString& archive, float percentage, int32 bytes) override;
	virtual void OnDone(const FString& archive, EZipUtilityCompletionState CompletionState) override;
	virtual void OnStartProcess(const FString& archive, int32 bytes) override;
	virtual void OnFileDone(const FString& archive, const FString& file) override;
	virtual void OnFileFound(const FString& archive, const FString& file, int32 size) override;

	TFunction<void()> OnDoneCallback;
	TFunction<void(float)> OnProgressCallback;
};