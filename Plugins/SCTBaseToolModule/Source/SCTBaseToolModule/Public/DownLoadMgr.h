#pragma once

#include "CoreMinimal.h"
#include "IHttpRequest.h"
#include "HttpModule.h"
#include "IHttpResponse.h"
#include "Serialization/Archive.h"
#include "Tickable.h"
#include "Job.h"

class SCTBASETOOLMODULE_API FDownLoadFile
{
public:
	DECLARE_DELEGATE_TwoParams(FOnFileLoadSucceed, const FString&, const FString&);
	DECLARE_DELEGATE_TwoParams(FOnFileLoadFailed, const FString&, const FString&);

	FDownLoadFile(const FString &_FileUrl, const FString &_FilePath, const FString &_FileMD5,
		const FOnFileLoadSucceed &SucceedLoadCallback,
		const FOnFileLoadFailed &FailedCallback);
	~FDownLoadFile();
	
public:
	/** ִ���ļ����� */
	void ExecuteDownloadFile();

	/** ��ͣ/�����ļ����� */
	void PauseFileLoad() { bLoadPaused = true; };
	void ContinueFileLoad();

	/** ��ȡ��ǰ���ؽ��� */
	float GetRateOfProgress() { return RateProgress; };

private:
	void RequestDownload(int32 fileOffset);
	void Callback_RequestDownload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//���سɹ�
	void DownloadSucceed();
	//����ʧ��
	void DownloadFailed();

private:
	//���ص�ַ
	FString FileUrl;
	//���浽Ӳ�̵�·�������� ../../DLCPAK/Model/M_001_&0eb24bbe70206ce7b704d00cacab4d65.pak
	FString FilePath;
	//�ļ�δ�������ʱ�����浽Ӳ�̵���ʱ·�������� ../../DLCPAK/Model/M_001_&0eb24bbe70206ce7b704d00cacab4d65.pak.temp
	FString FilePathTemp;
	//�����������ĸ��ļ���MD5�룬����У��������֮���ļ���������
	FString FileMD5;

	//��ǰʹ�õ�Http���󣬲���Ҫʱ��ʱ�������������̻߳��ں�̨����
	TSharedPtr<class IHttpRequest> HttpRequest;
	
	FArchive* FileArchive;    //�ļ���д����ӿ�
	int64 FileTotalSize;      //Ҫ���ص��ļ��ܴ�С���ֽڣ�
	int64 DownloadedFileSize; //�Ѿ����ص����ݴ�С���ֽڣ�
	int64 FileRemainSize;     // 下载文件剩余量

	float RateProgress;  //��ǰ���ؽ���
	bool bLoadPaused;    //��ʶ�Ƿ���ͣ

	FOnFileLoadSucceed OnFileLoadSucceed;
	FOnFileLoadFailed OnFileLoadFailed;
};

class SCTBASETOOLMODULE_API FDownloadTask : public IJob
{
public:
	friend class FDownloadMgr;

	enum ETaskState : uint8
	{
		State_NotStart,
		State_Failed,
		State_Processing,
		State_Finished,
		State_NoNeedToDownload
	};

	DECLARE_DELEGATE_TwoParams(FOnFileDownloadFinished, const FString&, const FString&);
	DECLARE_DELEGATE_TwoParams(FOnFileDownloadFailed, const FString&, const FString&);

	FDownloadTask(const FString &_FileUrl, const FString &_FilePath, const FString &_FileMD5);

	//ִ������
	virtual void DoJob() override;

	ETaskState GetState() const { return TaskState; }

private:
	void RequestDownload(int32 fileOffset);
	void Callback_RequestDownload(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	//�ļ�������ϣ�У���ļ������ԣ������������ɾ������Ϊ����ʧ�ܣ������������Ϊ���سɹ�
	void DownloadFinished();
	//���سɹ�
	void DownloadSuccess();
	//����ʧ��
	void DownloadFailed();

	FString CalcMD5FromFile(const FString &_FilePath);

	//���ص�ַ
	FString FileUrl;
	//���浽Ӳ�̵�·�������� ../../DLCPAK/Model/M_001_&0eb24bbe70206ce7b704d00cacab4d65.pak
	FString FilePath;
	//�ļ�δ�������ʱ�����浽Ӳ�̵���ʱ·�������� ../../DLCPAK/Model/M_001_&0eb24bbe70206ce7b704d00cacab4d65.pak.temp
	FString FilePathTemp;
	//�����������ĸ��ļ���MD5�룬����У��������֮���ļ���������
	FString FileMD5;

	//��ǰʹ�õ�Http���󣬲���Ҫʱ��ʱ�������������̻߳��ں�̨����
	TSharedPtr<class IHttpRequest> HttpRequest;
	//�ļ���д����ӿ�
	FArchive* FileArchive;
	//Ҫ���ص��ļ��ܴ�С���ֽڣ�
	int64 FileTotalSize;
	//�Ѿ����ص����ݴ�С���ֽڣ�
	int64 DownloadedFileSize;
	int64 FileRemainSize;     // 下载文件剩余量
	ETaskState TaskState;

};

class SCTBASETOOLMODULE_API FDownloadMgr
{
public:
	static const int64 DownloadPacketSize;

	static const TSharedRef<FDownloadMgr>& Get();

	void DownloadFile(const FString &_FileUrl, const FString &_FilePath, const FString &_FileMD5, bool bLast = true,
		const IJob::FOnJobFinished &FinishCallback = IJob::FOnJobFinished());
		// const FDownloadTask::FOnFileDownloadFinished &FinishCallback = FDownloadTask::FOnFileDownloadFinished(), 
		// const FDownloadTask::FOnFileDownloadFailed &FailedCallback = FDownloadTask::FOnFileDownloadFailed());

	void DownloadFinished(const FString &_FileUrl, const FString &_FilePath);
	void DownloadFailed(const FString &_FileUrl, const FString &_FilePath);

private:
	TMap<FString, TSharedPtr<FDownloadTask>> DownloadTaskMap;
};