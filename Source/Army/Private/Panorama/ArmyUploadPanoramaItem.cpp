// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyUploadPanoramaItem.h"
#include "ArmyUser.h"

FArmyPanoramaUploadItem::FArmyPanoramaUploadItem(TArray<uint8> _UploadData, FString _UploadFileName)
{
	UploadData = _UploadData;
	UploadFileName = _UploadFileName;
	UploadState = EUpDownloadState::NotStarted;
	RetryTimes = 0;
}

void FArmyPanoramaUploadItem::StartUpload(FString Dir)
{
	UploadState = EUpDownloadState::Processing;
	dir = Dir;
	//绑定回调
	FOnUploadComplete Delegate_UploadComplete;
	Delegate_UploadComplete.BindRaw(this, &FArmyPanoramaUploadItem::Callback_RequestUpload);
    FAliyunOss::Get().PutObject(EOssDir::OD_Panorama, Dir, UploadFileName, UploadData, Delegate_UploadComplete);
}

void FArmyPanoramaUploadItem::Callback_RequestUpload(TSharedPtr<FAliyunOssResponse> Response)
{
	if (Response->bWasSuccessful)
	{
        UploadSuccess();
        return;
	}

	//如果失败尝试重新上传
    if (RetryTimes >= 3)
    {
        UploadFailed();
    }
    else
    {
        RetryTimes++;
        StartUpload(dir);
    }
}

void FArmyPanoramaUploadItem::CancelUpload()
{
	UploadState = EUpDownloadState::Failed;
}

void FArmyPanoramaUploadItem::UploadSuccess()
{
	UploadState = EUpDownloadState::Succeeded;
	UploadData.Reset();
}

void FArmyPanoramaUploadItem::UploadFailed()
{
	UploadState = EUpDownloadState::Failed;
}
