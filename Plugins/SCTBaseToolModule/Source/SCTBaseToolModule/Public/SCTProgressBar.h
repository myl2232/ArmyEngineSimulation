#pragma once
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
//#include "Widgets/SLeafWidget.h"

class SCTBASETOOLMODULE_API SProgressBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProgressBox)
		{}
		SLATE_ARGUMENT(FVector2D, BoxSize)
		SLATE_EVENT(FSimpleDelegate, OnSucceedDownload)
		SLATE_EVENT(FSimpleDelegate, OnFailedDownload)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	void SetDownloadTasts(TArray<FString> InFileCachePaths, TArray<FString> InResourceUrls);
	void ExecuteProgress();
	void PauseProgress();
	void ContinuProgress();
	bool DetectCacheFiles();
	void OnFileDownloadFinished(const FString&, const FString&);
	void OnFileDownloadFailed(const FString&, const FString&);

	//test
	void SetProgressRate(float InRate);

private:
	float GetProgress() const;
	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);

private:
	TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;
	TSharedPtr<class SBox> Progress; 
	FVector2D BoxSize;

	FSimpleDelegate Callback_Succeed;
	FSimpleDelegate Callback_Failed;

	int32 TaskCount = 0;
	TArray<FString> FileCachePaths; //本地缓存
	TArray<FString> ResourceUrls;   //下载Url
	TArray<TSharedPtr<class FDownLoadFile>> LoadFileTasks;  //资源加载任务
};

class SCTBASETOOLMODULE_API SProgressBarContent : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProgressBarContent)
		: _Image(nullptr)
		{}
		SLATE_ARGUMENT(const FSlateBrush*, Image)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	//void SetMessage(const FText& InJobDescription, float InJobProgress, int32 InNumFinishedJob, int32 InNumTotalJob);
	void SetMessage(const FText& InJobDescription);

private:
	FText GetDescription() const { return DescriptionText; }
	FText GetProgress() const { return ProgressText; }
	//EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);

private:
	//TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;
	FText DescriptionText;
	FText ProgressText;
};

class SCTBASETOOLMODULE_API FProgressBar
{
public:
	FProgressBar(const FSlateBrush *InImage);
	~FProgressBar();

	void SetMessage(const FText& InJobDescription, float InJobProgress, int32 InNumFinishedJob, int32 InNumTotalJob);

private:
	TSharedPtr<class SWindow> ProgressBarWidget;
	TSharedPtr<SProgressBarContent> ProgressBarContentWidget;
};