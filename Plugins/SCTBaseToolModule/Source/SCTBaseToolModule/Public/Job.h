/**
 * Copyright 2018 ����������(���)�Ƽ����޹�˾.
 * All Rights Reserved.
 * 
 *
 * @File Job.h
 * @Description �������
 *
 * @Author ��Ԯ
 * @Date 2018��5��18��
 * @Version 1.0
 */
#pragma once
#include "CoreMinimal.h"
#include "Tickable.h"
#include "Containers/List.h"


DECLARE_DELEGATE_OneParam(FDelegateLoadMessage,FString);

class IJob
{
public:
	friend class FJobQueue;
	
	enum EJobState
	{
		JS_NotStarted = 0,
		JS_Executing,
		JS_Failed,
		JS_Succeed
	};

	DECLARE_DELEGATE_OneParam(FOnJobFinished, EJobState)

	IJob() : JobState(JS_NotStarted), JobProgress(0.0f) {}
	virtual ~IJob() {}
	bool IsJobExecuting() const { return JobState == JS_Executing; }
	bool IsJobFinished() const { return JobState == JS_Failed || JobState == JS_Succeed; }
	bool IsJobSucceed() const { return JobState == JS_Succeed; }
	float GetJobProgress() const { return JobProgress; }
	const FString& GetJobDescription() const { return JobDescription; }
	void SetJobFinishedDelegate(const FOnJobFinished &InJobFinishedDelegate) {
		JobFinishedDelegate = InJobFinishedDelegate;
	}
	void NotifyJobFinished() { JobFinishedDelegate.ExecuteIfBound(JobState); }
	virtual void DoJob() = 0;

protected:
	EJobState JobState;
	float JobProgress;
	FString JobDescription;
	FOnJobFinished JobFinishedDelegate;
};

class FJobQueue : public FTickableGameObject
{
public:
	SCTBASETOOLMODULE_API static const TSharedRef<FJobQueue>& Get();

public:
	virtual bool IsTickable() const override { return JobList.Num() > 0; }
	virtual TStatId GetStatId() const override;
	virtual void Tick(float DeltaTime) override;

	SCTBASETOOLMODULE_API FSimpleDelegate& GetJobQueueFinishedDelegate();

	SCTBASETOOLMODULE_API void EnqueueJob(TSharedPtr<IJob> InJob);
	SCTBASETOOLMODULE_API void ExecuteJobQueue();
	SCTBASETOOLMODULE_API void ExecuteJobQueueInLoadWindow(FDelegateLoadMessage delegateLoadMessage);
	SCTBASETOOLMODULE_API void SetProgressBarImage(const struct FSlateBrush *Image);

private:
	void ExecuteNextJob();
	void RemoveJob(TDoubleLinkedList<TSharedPtr<IJob>>::TDoubleLinkedListNode *InNode);
	void ClearQueue();

private:
	FDelegateLoadMessage OnLoadMassage;

private:
	TDoubleLinkedList<TSharedPtr<IJob>> JobList;
	FSimpleDelegate JobQueueFinished;
	TSharedPtr<class FProgressBar> JobProgressBar;
	const struct FSlateBrush *ProgressBarImage;
	int32 NumFinishedJob = 0;
	int32 NumTotalJob = 0;
	float ElapsedTime = 0.0f;
	float TimeThreshold = 0.5f;
};