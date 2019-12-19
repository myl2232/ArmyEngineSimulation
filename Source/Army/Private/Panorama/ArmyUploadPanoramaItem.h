// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AliyunOss.h"
#include "Http.h"

namespace EUpDownloadState
{
	enum Type
	{
		NotStarted,
		Processing,
		Succeeded,
		Failed,
	};
}

/**
*
*/
class FArmyPanoramaUploadItem
{
public:

	FArmyPanoramaUploadItem(TArray<uint8> _UploadData, FString _UploadFileName);

	void StartUpload(FString Dir);
	void Callback_RequestUpload(TSharedPtr<FAliyunOssResponse> Response);
	void CancelUpload();
	void UploadSuccess();
	void UploadFailed();

	TArray<uint8> UploadData;
	FString UploadFileName;
	EUpDownloadState::Type UploadState;
	int32 RetryTimes;

private:
	FString dir;
};
