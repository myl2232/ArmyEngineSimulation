#pragma once
#include "WFULambdaRunnable.h"
#include "WindowsFileUtilityPrivatePCH.h"


uint64 FWFULambdaRunnable::ThreadNumber = 0;

FQueuedThreadPool* FWFULambdaRunnable::ThreadPool = nullptr;

FWFULambdaRunnable::~FWFULambdaRunnable()
{
	ThreadPool->Destroy();
}

TFuture<void> FWFULambdaRunnable::RunLambdaOnBackGroundThread(TFunction< void()> InFunction)
{
	return Async(EAsyncExecution::Thread, InFunction);
}

TFuture<void> FWFULambdaRunnable::RunLambdaOnBackGroundThreadPool(TFunction< void()> InFunction)
{
	return Async(EAsyncExecution::ThreadPool, InFunction);
}

FGraphEventRef FWFULambdaRunnable::RunShortLambdaOnGameThread(TFunction< void()> InFunction)
{
	return FFunctionGraphTask::CreateAndDispatchWhenReady(InFunction, TStatId(), nullptr, ENamedThreads::GameThread);
}

void FWFULambdaRunnable::InitThreadPool(int32 NumberOfThreads)
{
	if (ThreadPool == nullptr)
	{
		ThreadPool = FQueuedThreadPool::Allocate();
		int32 NumThreadsInThreadPool = NumberOfThreads;
		ThreadPool->Create(NumThreadsInThreadPool, 32 * 1024);
	}
}


IQueuedWork* FWFULambdaRunnable::AddLambdaToQueue(TFunction< void()> InFunction)
{
	if (ThreadPool == nullptr)
	{
		FWFULambdaRunnable::InitThreadPool(FPlatformMisc::NumberOfIOWorkerThreadsToSpawn());
	}

	if (ThreadPool)
	{
		return AsyncLambdaPool(*ThreadPool, InFunction);
	}
	return nullptr;
}

bool FWFULambdaRunnable::RemoveLambdaFromQueue(IQueuedWork* Work)
{
	if (ThreadPool)
	{
		return ThreadPool->RetractQueuedWork(Work);
	}
	return false;
}

