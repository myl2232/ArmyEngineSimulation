#pragma once

#include "CoreMinimal.h"
#include "7zpp.h"
#include "ListCallback.h"
#include "ProgressCallback.h"
#include "HAL/ThreadSafeBool.h"

class IZipUtilityInterface;
using namespace SevenZip;
/**
 * Forwards events from the 7zpp library to the UE4 listener.
 */
class ZIPUTILITY_API FSevenZipCallbackHandler : public ListCallback, public ProgressCallback
{
public:
	virtual ~FSevenZipCallbackHandler() = default;
public:
	
	virtual void OnProgress(const TString& archivePath, uint64 bytes) override;
	virtual void OnDone(const TString& archivePath) override;
	virtual void OnFileDone(const TString& archivePath, const TString& filePath, uint64 bytes) override;
	virtual void OnStartWithTotal(const TString& archivePath, unsigned __int64 totalBytes) override;
	virtual void OnFileFound(const TString& archivePath, const TString& filePath, int size) override;
	virtual void OnListingDone(const TString& archivePath) override;
	virtual bool OnCheckBreak() override;
	
	uint64 BytesLeft = 0;
	uint64 TotalBytes = 0;
	TSharedPtr<IZipUtilityInterface> ProgressDelegate = nullptr;
	FThreadSafeBool bCancelOperation = false;
};
