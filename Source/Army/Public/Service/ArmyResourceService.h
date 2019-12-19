#pragma once

#include "AliyunOss.h"
#include "ArmyTypes.h"
#include "ArmyUser.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"

DECLARE_DELEGATE_OneParam(FOnDelegateParamString, const FString&);
DECLARE_DELEGATE_OneParam(FOnDelegateParamJsonObject, const TSharedPtr<FJsonObject>);
DECLARE_DELEGATE_TwoParams(FOnProductInfo,FString,int32);

class FArmyResourceService
{
public:
	FArmyResourceService();
	bool SetUploadNum(int32 InCount);

	bool UploadConstructionImage(const FName& InName, const TArray<uint8>& InData);

	bool RequestLoadConstructionFrameLogo();

	bool DownLoadConstructionFrame();

	bool RequestFrameFillInfo();

	bool RequestConstructionLayers();

	void CancelUploadImage();

	void UpdateAutoComponentProductInfo(int32 ComponentID,FString& PipeDiameter,FString& ObjectIDStr);
	void Callback_UpdateAutoComponentProductInfo(struct FArmyHttpResponse Response);
private:
	void RequestFailed(int32 InErrorCode);

	void SaveFailed(int32 InErrorCode);

	void OnUploadCanceled();

	void Callback_RequestFrameContent(FArmyHttpResponse Response);

	void Callback_DownloadFileFinished(bool bWasSucceed, FString InFilePath,int32 InFileType);

	//上传文件回调
	void Callback_RequestUploadImageFile(TSharedPtr<FAliyunOssResponse> Response, FName Name);

	//提交保存
	void RequestSaveConstructionImage();

	void Callback_RequestLoadConstructionLogo(struct FArmyHttpResponse Response);

	void Callback_RequestSaveConstructionImage(struct FArmyHttpResponse Response);

	void Callback_RequestConstructionLayers(FArmyHttpResponse Response);
public:
	FOnDelegateParamString OnLoadConstructionFrame;
	FOnDelegateParamString OnLoadFrameLogo;
	FSimpleDelegate OnFrameFillContentCallBack;
	FOnDelegateParamJsonObject OnLoadConstructionLayersCallBack;
	FInt32StringDelegate OnUploadImageStep;

	FOnProductInfo OnUpdateProductInfoDelegate;
private:
	int32 UploadCount = 0;
	int32 CurrentUploadIndex = -1;
	TMap<FString, FName> ConstructionImageMap;

	//请求
	TSharedPtr<IHttpRequest> SubmitSaveRequest;
};