#pragma once
#include "IHttpRequest.h"

class SCTBASETOOLMODULE_API FAliYunServer
{
private:
	FAliYunServer();

public:
	static const TSharedRef<FAliYunServer>& Get();

public:
	const FString& GetALiYunDNS() const;
	void SetALiYunDNS(const FString &InDNS);
	void SetALiYunBucketName(const FString &InName);
	void SetALiYunAccessId(const FString &InId);
	void SetALiYunAccessKey(const FString &InKey);

public:
	/** �ϴ��ļ��������� */
	void RequestUploadFileAliYun(
		const FHttpRequestCompleteDelegate& RequestCompleteCallback, 
		const TArray<uint8>& Data, const FString& FileName);

	/** ��顢�����������е��ļ��� */
	void CheckServerFolder(const FString &Folder);

private:
	/** �жϰ������Ƿ��Ѿ�����ָ���ļ��� */
	void RequestFileIfExistInAliYun(
		const FHttpRequestCompleteDelegate& RequestCompleteCallback, 
		const FString& FileName);

	/** �ڰ����Ʒ������ϴ����ļ��� */
	void CreateFileFolderOnServer(const FString &Folder);

private:
	FString ALIYUN_DNS;
	FString BUCKET_NAME;
	FString ACCESS_ID;
	FString ACCESS_KEY;
};
