#include "ZipOperation.h"
#include "ZipUtilityPrivatePCH.h"
#include "SevenZipCallbackHandler.h"
#include "WFULambdaRunnable.h"


FZipOperation::FZipOperation()
{
	CallbackHandler = nullptr;
}

void FZipOperation::StopOperation()
{
	if (ThreadPoolWork != nullptr)
	{
		FWFULambdaRunnable::RemoveLambdaFromQueue(ThreadPoolWork);
	}

	if (CallbackHandler != nullptr)
	{
 		CallbackHandler->bCancelOperation = true;
		CallbackHandler = nullptr;
	}
}

void FZipOperation::SetCallbackHandler(FSevenZipCallbackHandler* Handler)
{
	CallbackHandler = Handler;
}

void FZipOperation::SetThreadPoolWorker(IQueuedWork* Work)
{
	ThreadPoolWork = Work;
}

