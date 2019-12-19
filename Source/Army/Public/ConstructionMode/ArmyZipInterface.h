#include "ZipUtilityInterface.h"
#include "ArmyTypes.h"

#pragma once

class FArmyCADZipInterface : public IZipUtilityInterface
{
	DECLARE_DELEGATE_ThreeParams(FCadDoneDelegate, const FString&, const FString& , const FString& );
	
public:
	virtual ~FArmyCADZipInterface();

	virtual void OnProgress(const FString& archive, float percentage, int32 bytes) override;
	//压缩完成后执行
	virtual void OnDone(const FString& archive, EZipUtilityCompletionState CompletionState) override;
	virtual void OnStartProcess(const FString& archive, int32 bytes) override;
	virtual void OnFileDone(const FString& archive, const FString& file) override;
	virtual void OnFileFound(const FString& archive, const FString& file, int32 size) override;

	FCadDoneDelegate OnCadDoneDelegate;

	 FString From ;
	 FString To;
	 FString DirPath;

	void SetFromTo( FString TempFrom, FString TempTo);
	void SetDirPath(FString InDirpath);
};