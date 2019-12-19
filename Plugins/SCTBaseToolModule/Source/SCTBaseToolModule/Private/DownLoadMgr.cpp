#include "DownLoadMgr.h"
#include "HttpMgr.h"
#include "Misc/Paths.h"
#include "Misc/SecureHash.h"
#include "HAL/FileManager.h"

FDownLoadFile::FDownLoadFile(const FString &InFileUrl, const FString &InFilePath, const FString &InFileMD5,
	const FOnFileLoadSucceed &SucceedCallback,
	const FOnFileLoadFailed &FailedCallback)
	: DownloadedFileSize(0),
	FileTotalSize(-1),
	FileArchive(nullptr),
	FileRemainSize(-1)
{
	FileUrl = InFileUrl;
	FilePath = InFilePath;
	FileMD5 = InFileMD5;
	OnFileLoadSucceed = SucceedCallback;
	OnFileLoadFailed = FailedCallback;

	RateProgress = 0.0;
	bLoadPaused = false;
}

FDownLoadFile::~FDownLoadFile()
{
	//关闭请求
	if (HttpRequest.IsValid())
	{
		if (HttpRequest->OnProcessRequestComplete().IsBound())
		{
			HttpRequest->OnProcessRequestComplete().Unbind();
		}
		HttpRequest->CancelRequest();
		HttpRequest = NULL;
	}

	//关闭文件流
	if (FileArchive)
	{
		FileArchive->Close();
		delete FileArchive;
		FileArchive = NULL;
	}
	
}

void FDownLoadFile::ExecuteDownloadFile()
{
	FilePathTemp = FilePath + TEXT(".temp");
	DownloadedFileSize = 0;
	FileTotalSize = -1;
	FileRemainSize = -1;

	//如果文件存在，尝试继续下载
	if (FPaths::FileExists(FilePathTemp))
	{
		DownloadedFileSize = IFileManager::Get().FileSize(*FilePathTemp);
	}
	else if (FPaths::FileExists(FilePath))
	{
		DownloadSucceed();
		return;
	}

	if (FileArchive == nullptr)
	{
		if (DownloadedFileSize > 0)
		{
			FileArchive = IFileManager::Get().CreateFileWriter(*FilePathTemp, FILEWRITE_Append);
			FileArchive->Seek(DownloadedFileSize);		
		}
		else
		{
			FileArchive = IFileManager::Get().CreateFileWriter(*FilePathTemp, 0);
		}
	}

	//创建成功，则从文件的末尾位置新开始请求一段数据流
	if (FileArchive)
	{
		RequestDownload(DownloadedFileSize);
	}
	else
	{
		DownloadFailed();
	}
}

void FDownLoadFile::ContinueFileLoad()
{
	bLoadPaused = false;
	if (FileArchive == nullptr)
	{
		ExecuteDownloadFile();
	}
	else
	{
		RequestDownload(DownloadedFileSize);
	}
};

void FDownLoadFile::RequestDownload(int32 fileOffset)
{
	HttpRequest = FHttpModule::Get().CreateRequest();
	
	HttpRequest->SetURL(FileUrl);
	
	// 第一次数据请求，先获取文件大小
	if (FileTotalSize == -1)
	{
		HttpRequest->SetVerb(TEXT("Head"));
	}
	else if (FileRemainSize > FDownloadMgr::DownloadPacketSize)
	{
		HttpRequest->SetVerb(TEXT("Get"));
		HttpRequest->SetHeader(TEXT("Range"), FString::Printf(TEXT("bytes=%i-%i"), fileOffset, fileOffset + FDownloadMgr::DownloadPacketSize));
	}
	else
	{
		HttpRequest->SetVerb(TEXT("Get"));
		HttpRequest->SetHeader(TEXT("Range"), FString::Printf(TEXT("bytes=%i-%i"), fileOffset, fileOffset + FileRemainSize - 1));
	}
	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FDownLoadFile::Callback_RequestDownload);
	HttpRequest->ProcessRequest();
}

void FDownLoadFile::Callback_RequestDownload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (FHttpMgr::Get()->CheckHttpResponseValid(Response, bWasSuccessful))
	{
		int32 RetutnCode = Response->GetResponseCode();
		if (FileTotalSize == -1)
		{
#if 0
			FString range = Response->GetHeader(TEXT("Content-Range"));
			if (range.IsEmpty())
			{
				FileTotalSize = Response->GetContentLength();
			}
			else
			{
				int32 pos = range.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				FileTotalSize = FCString::Atoi(*(range.RightChop(pos + 1)));
			}
#else
			FileTotalSize = Response->GetContentLength();
#endif
			// 文件大小获取错误，直接认为下载失败
			if (FileTotalSize <= 0)
			{
				DownloadFailed();
				return;
			}
			RateProgress = (double)DownloadedFileSize / FileTotalSize;
			FileRemainSize = FileTotalSize - DownloadedFileSize;
			RequestDownload(DownloadedFileSize);
			return;
		}		
		//写入刚得到的数据块
		TArray<uint8> Data = Response->GetContent();
		FileArchive->Serialize(const_cast<uint8*>(Data.GetData()), Data.Num());
		FileArchive->FlushCache();

		//DownSize累加，进度累加
		DownloadedFileSize += Response->GetContentLength();
		RateProgress = (double)DownloadedFileSize / FileTotalSize;
		FileRemainSize = FileTotalSize - DownloadedFileSize;

		//判断是否暂停下载
		if (bLoadPaused)
		{
			return;
		}

		//如果依然没有下完，继续请求下一个数据块
		if (DownloadedFileSize < FileTotalSize)
		{
			RequestDownload(DownloadedFileSize);
		}
		else//下载完成
		{
			DownloadSucceed();
		}
	}
	else
	{		
		// 请求失败，标记下载失败
		DownloadFailed();
	}
}

void FDownLoadFile::DownloadSucceed()
{
	//关闭请求
	if (HttpRequest.IsValid())
	{
		if (HttpRequest->OnProcessRequestComplete().IsBound())
		{
			HttpRequest->OnProcessRequestComplete().Unbind();
		}
		HttpRequest->CancelRequest();
		HttpRequest = nullptr;
	}

	//关闭文件流
	if (FileArchive)
	{
		FileArchive->Close();
		delete FileArchive;
		FileArchive = nullptr;
	}
	//拷贝临时文件
	if (FPaths::FileExists(*FilePathTemp))
	{
		bool bMove = IFileManager::Get().Move(*FilePath, *FilePathTemp);
	}
	RateProgress = 1.0f;	
	//执行代理
	OnFileLoadSucceed.ExecuteIfBound(FileUrl, FilePath);
}

void FDownLoadFile::DownloadFailed()
{
	//关闭请求
	if (HttpRequest.IsValid())
	{
		HttpRequest->CancelRequest();
		HttpRequest->OnProcessRequestComplete().Unbind();
		HttpRequest = NULL;
	}

	//关闭文件流
	if (FileArchive)
	{
		FileArchive->Close();
		delete FileArchive;
		FileArchive = NULL;
	}
	
	//删除这个错误文件
	IFileManager::Get().Delete(*FilePathTemp, true, true);
	//执行代理
	OnFileLoadFailed.ExecuteIfBound(FileUrl, FilePath);
}


FDownloadTask::FDownloadTask(const FString &_FileUrl, const FString &_FilePath, const FString &_FileMD5)
	: DownloadedFileSize(0),
	FileTotalSize(-1),
	FileArchive(nullptr),
	FileRemainSize(-1),
	TaskState(State_NotStart)
{
	FileUrl = _FileUrl;
	FilePath = _FilePath;
	FileMD5 = _FileMD5;

	JobDescription = TEXT("下载数据...");
}

void FDownloadTask::DoJob()
{
	//JobFinished = 0;  //开始任务
	JobProgress = 0.0f;  //进度条比例为0.0

	FilePathTemp = FilePath;
	FilePathTemp.Append(TEXT(".temp"));
	DownloadedFileSize = 0;
	FileTotalSize = -1;
	FileRemainSize = -1;

	//如果文件存在，尝试继续下载
	if (FPaths::FileExists(FilePathTemp))
	{
		DownloadedFileSize = IFileManager::Get().FileSize(*FilePathTemp);
	}
	else if (FPaths::FileExists(FilePath))
	{
		DownloadSuccess();
		TaskState = State_NoNeedToDownload;
		return;
	}

	if (FileArchive == nullptr)
	{
		if (DownloadedFileSize > 0)
		{
			FileArchive = IFileManager::Get().CreateFileWriter(*FilePathTemp, FILEWRITE_Append);
			FileArchive->Seek(DownloadedFileSize);
		}
		else
		{
			FileArchive = IFileManager::Get().CreateFileWriter(*FilePathTemp, 0);
		}
	}

	//创建成功，则从文件的末尾位置新开始请求一段数据流
	if (FileArchive)
	{
		RequestDownload(DownloadedFileSize);
	}
	else
	{
		DownloadFailed();
	}
}

void FDownloadTask::RequestDownload(int32 fileOffset)
{
	TaskState = State_Processing;

	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(FileUrl);
	HttpRequest->SetVerb(TEXT("Get"));	
	if (FileRemainSize > FDownloadMgr::DownloadPacketSize || FileRemainSize == -1)
	{
		HttpRequest->SetHeader(TEXT("Range"), FString::Printf(TEXT("bytes=%i-%i"), fileOffset, fileOffset + FDownloadMgr::DownloadPacketSize));
	}
	else
	{
		HttpRequest->SetHeader(TEXT("Range"), FString::Printf(TEXT("bytes=%i-%i"), fileOffset, fileOffset + FileRemainSize - 1));
	}

	HttpRequest->OnProcessRequestComplete().BindRaw(this, &FDownloadTask::Callback_RequestDownload);
	HttpRequest->ProcessRequest();
}

void FDownloadTask::Callback_RequestDownload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (FHttpMgr::Get()->CheckHttpResponseValid(Response, bWasSuccessful))
	{
		//获取真实的文件大小
		if (FileTotalSize == -1)
		{
			FString range = Response->GetHeader(TEXT("Content-Range"));
			if (range.IsEmpty())
			{
				FileTotalSize = Response->GetContentLength();
			}
			else
			{
				int32 pos = range.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
				FileTotalSize = FCString::Atoi(*(range.RightChop(pos + 1)));
			}

			// 文件大小获取错误，直接认为下载失败
			if (FileTotalSize <= 0)
			{
				DownloadFailed();
				return;
			}
		}

		//写入刚得到的数据块
		TArray<uint8> Data = Response->GetContent();
		FileArchive->Serialize(const_cast<uint8*>(Data.GetData()), Data.Num());
		FileArchive->FlushCache();

		//DownSize累加，进度累加
		DownloadedFileSize += Response->GetContentLength();
		JobProgress = (double)DownloadedFileSize / FileTotalSize;
		FileRemainSize = FileTotalSize - DownloadedFileSize;
		//如果依然没有下完，继续请求下一个数据块
		if (DownloadedFileSize < FileTotalSize)
			RequestDownload(DownloadedFileSize);
		else//下载完成
			DownloadFinished();
	}
	else
	{
		// 请求失败，标记下载失败
		DownloadFailed();
	}
}

void FDownloadTask::DownloadFinished()
{
	bool bDownloadSuccess = true;

	//关闭文件流
	if (FileArchive)
	{
		FileArchive->Close();
		delete FileArchive;
		FileArchive = NULL;
	}

	if (!FileMD5.IsEmpty())
	{
		FString LocalMD5 = CalcMD5FromFile(FilePathTemp);

		//下载完之后只对比MD5
		if (FileMD5 == LocalMD5)
		{
			//如果重命名失败 有可能是目标文件存在且被占用
			bDownloadSuccess = IFileManager::Get().Move(*FilePath, *FilePathTemp);
		}
		else
		{
			bDownloadSuccess = false;
		}
	}
	else
	{
		bDownloadSuccess = IFileManager::Get().Move(*FilePath, *FilePathTemp);
	}
	
	if (bDownloadSuccess)
	{
		DownloadSuccess();
	}
	else
	{
		DownloadFailed();
	}
}

void FDownloadTask::DownloadSuccess()
{
	TaskState = State_Finished;
	JobState = JS_Succeed; //任务结束

	//OnFileDownloadFinished.ExecuteIfBound(FileUrl, FilePath);
	FDownloadMgr::Get()->DownloadFinished(FileUrl, FilePath);
}

void FDownloadTask::DownloadFailed()
{
	TaskState = State_Failed;
	JobState = JS_Failed; //任务结束

	//关闭文件流
	if (FileArchive)
	{
		FileArchive->Close();
		delete FileArchive;
		FileArchive = NULL;
	}

	//关闭请求
	if (HttpRequest.IsValid())
	{
		HttpRequest->CancelRequest();
		HttpRequest->OnProcessRequestComplete().Unbind();
		HttpRequest = NULL;
	}

	//删除这个错误文件
	IFileManager::Get().Delete(*FilePathTemp, true, true);

	//OnFileDownloadFailed.ExecuteIfBound(FileUrl, FilePath);
	FDownloadMgr::Get()->DownloadFailed(FileUrl, FilePath);
}

FString FDownloadTask::CalcMD5FromFile(const FString &_FilePath)
{
	FArchive* Ar = IFileManager::Get().CreateFileReader(*_FilePath);
	if (Ar == NULL)
	{
		return TEXT("");
	}
	
	TArray<uint8> Result;
	Result.Reset();
	Result.AddUninitialized(Ar->TotalSize());
	Ar->Serialize(Result.GetData(), Result.Num());
	delete Ar;

	FMD5 Md5Gen;
	Md5Gen.Update(Result.GetData(), Result.Num());

	uint8 Digest[16];
	Md5Gen.Final(Digest);

	FString MD5;
	for (int32 i = 0; i < 16; i++)
	{
		MD5 += FString::Printf(TEXT("%02x"), Digest[i]);
	}

	return MD5;
}


const int64 FDownloadMgr::DownloadPacketSize = 512 * 1024 - 1;

const TSharedRef<FDownloadMgr>& FDownloadMgr::Get()
{
	static const TSharedRef<FDownloadMgr> Instance = MakeShareable(new FDownloadMgr);
	return Instance;
}

void FDownloadMgr::DownloadFile(const FString &_FileUrl, const FString &_FilePath, const FString &_FileMD5, bool bLast/* = true*/,
	const IJob::FOnJobFinished &FinishCallback /* = IJob::FOnJobFinished() */)
	// const FDownloadTask::FOnFileDownloadFinished &FinishCallback /* = FDownloadTask::FOnFileDownloadFinished() */,
	// const FDownloadTask::FOnFileDownloadFailed &FailedCallback /* = FDownloadTask::FOnFileDownloadFailed() */)
{
	if (DownloadTaskMap.Find(_FileUrl) == nullptr)
	{
		TSharedPtr<FDownloadTask> NewTask = MakeShareable(new FDownloadTask(_FileUrl, _FilePath, _FileMD5));
		NewTask->SetJobFinishedDelegate(FinishCallback);
		DownloadTaskMap.Add(_FileUrl, NewTask);
		FJobQueue::Get()->EnqueueJob(NewTask);
	}

	if (bLast)
	{
		//FJobQueue::Get()->GetQueuedJobExecutionFinishedDelegate()
		FJobQueue::Get()->ExecuteJobQueue();
	}
}

void FDownloadMgr::DownloadFinished(const FString &_FileUrl, const FString &_FilePath)
{
	if (DownloadTaskMap.Contains(_FileUrl))
	{
		DownloadTaskMap.Remove(_FileUrl);
	}
}

void FDownloadMgr::DownloadFailed(const FString &_FileUrl, const FString &_FilePath)
{
	if (DownloadTaskMap.Contains(_FileUrl))
	{
		DownloadTaskMap.Remove(_FileUrl);
	}
}