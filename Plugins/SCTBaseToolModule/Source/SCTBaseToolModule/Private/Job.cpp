#include "Job.h"
#include "SCTProgressBar.h"

const TSharedRef<FJobQueue>& FJobQueue::Get()
{
	static const TSharedRef<FJobQueue> Instance = MakeShareable(new FJobQueue);
	return Instance;
}

TStatId FJobQueue::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FJobQueue, STATGROUP_Tickables);
}

void FJobQueue::Tick(float DeltaTime)
{
	ElapsedTime += DeltaTime;
	if (ElapsedTime > TimeThreshold)
	{
		ElapsedTime = 0.0f;
		TDoubleLinkedList<TSharedPtr<IJob>>::TDoubleLinkedListNode *FirstNode = JobList.GetHead();
		const TSharedPtr<IJob> &Job = FirstNode->GetValue();
		if (Job->IsJobFinished())
		{
			++NumFinishedJob;
			Job->NotifyJobFinished();
			if (Job->IsJobSucceed())
			{
				RemoveJob(FirstNode);
				ExecuteNextJob();
			}
			else
			{
// 				ClearQueue();
// 				OnLoadMassage.Unbind();
// 				JobProgressBar.Reset();

				RemoveJob(FirstNode);
				ExecuteNextJob();
			}
		}
		else
		{
			OnLoadMassage.ExecuteIfBound(Job->GetJobDescription());
			if (JobProgressBar.IsValid())
			{
				JobProgressBar->SetMessage(FText::FromString(Job->GetJobDescription()), Job->GetJobProgress(), NumFinishedJob, NumTotalJob);
			}
		}
	}
}

FSimpleDelegate& FJobQueue::GetJobQueueFinishedDelegate()
{
	// if (JobQueueFinished.IsBound())
	// 	JobQueueFinished.Unbind();
	return JobQueueFinished;
}

void FJobQueue::EnqueueJob(TSharedPtr<IJob> InJob)
{
	JobList.AddTail(InJob);
}

void FJobQueue::ExecuteJobQueue()
{
	if (!JobProgressBar.IsValid())
	{
		JobProgressBar = MakeShareable(new FProgressBar(ProgressBarImage));
	}
	ExecuteNextJob();
}

void FJobQueue::ExecuteJobQueueInLoadWindow(FDelegateLoadMessage delegateLoadMessage)
{
	OnLoadMassage = delegateLoadMessage;

	ExecuteNextJob();
}

void FJobQueue::ExecuteNextJob()
{
	TDoubleLinkedList<TSharedPtr<IJob>>::TDoubleLinkedListNode *FirstNode = JobList.GetHead();
	if (FirstNode)
	{
		if (FirstNode->GetValue()->JobState == IJob::JS_NotStarted)
		{
			OnLoadMassage.ExecuteIfBound(FirstNode->GetValue()->GetJobDescription());
			if (JobProgressBar.IsValid())
			{
				JobProgressBar->SetMessage(FText::FromString(FirstNode->GetValue()->GetJobDescription()), 0.0f, NumFinishedJob, NumTotalJob);
			}
			FirstNode->GetValue()->JobState = IJob::JS_Executing;
			FirstNode->GetValue()->DoJob();
		}
	}
	else
	{
		OnLoadMassage.Unbind();
		JobProgressBar.Reset();
		JobQueueFinished.ExecuteIfBound();
		JobQueueFinished.Unbind();
		NumFinishedJob = 0;
		NumTotalJob = 0;
	}
}

void FJobQueue::SetProgressBarImage(const FSlateBrush *Image)
{
	ProgressBarImage = Image;
}

void FJobQueue::RemoveJob(TDoubleLinkedList<TSharedPtr<IJob>>::TDoubleLinkedListNode *InNode)
{
	JobList.RemoveNode(InNode);
}

void FJobQueue::ClearQueue()
{
	JobList.Empty();
	NumFinishedJob = 0;
	NumTotalJob = 0;
}