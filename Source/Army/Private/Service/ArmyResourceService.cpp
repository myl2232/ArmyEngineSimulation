#include "ArmyResourceService.h"
#include "ArmySlateModule.h"
#include "ResTools.h"
#include "ArmyCommonTools.h"
#include "ArmyEngineTools.h"
#include "ArmyDownloadModule.h"
#include "ArmyGameInstance.h"
#include "ArmyHttpModule.h"

#define ResourceNewURL "/api/newResource/10"
#define ResourceSaveURL "/api/saveResource/10"
#define ResourceSaveImageURL "/api/work-images"
#define ResourceSaveAsURL "/api/saveAsResource/10"
#define ResourceUpdateAsURL "/api/hydropower/joint"

#define ResourceLogoURL "/api/logo/picture_frame"

#define RequestConstructionLayer "/api/drawing"

#define RequestFrameFillContentURL "/api/projects/" + FString::FromInt(FArmyUser::Get().GetCurProjectID()) + "/plan/" + FString::FromInt(FArmyUser::Get().GetCurPlanID()) + "/drawing-side-msg"
#define RequestConstructionURL "/api/queryWork/"

#define CONSTRUCTIONFRAMEFILE TEXT("ConstructionFrame_Template.json")
#define CONSTRUCTIONFRAMEFILENEW TEXT("ConstructionFrame_TemplateNew.json")

FArmyResourceService::FArmyResourceService()
{
	IFileManager::Get().Delete(*(FArmyCommonTools::GetConstructionDir() + CONSTRUCTIONFRAMEFILENEW));//每次启动程序都 删除本地缓存的图框文件（因为图框文件暂时没有md5码，导致无法判断是否有更新，所以每次启动都删除缓存。但是考虑爱空间或以后其他用户无法连接图框服务器（打扮家自己的阿里云），所以用一个备份的本地缓存作为下载不成功后使用的默认图框）
}

bool FArmyResourceService::SetUploadNum(int32 InCount)
{
	if (UploadCount != 0)
	{
		return false;
	}
	UploadCount = InCount;
	return true;
}

bool FArmyResourceService::UploadConstructionImage(const FName& InName, const TArray<uint8>& InData)
{
	FString FileName = FString::Printf(TEXT("Construction_%s.jpg"), *FArmyEngineTools::GetTimeStampString());

	// 执行请求
    FOnUploadComplete UploadDelegate;
    UploadDelegate.BindRaw(this, &FArmyResourceService::Callback_RequestUploadImageFile, InName);
    FAliyunOss::Get().PutObject(EOssDir::OD_Picture, FileName, InData, UploadDelegate);

	return true;
}

void FArmyResourceService::OnUploadCanceled()
{
	SaveFailed(-2);
}

void FArmyResourceService::Callback_RequestUploadImageFile(TSharedPtr<FAliyunOssResponse> Response, FName Name)
{
    if (Response->bWasSuccessful)
    {
        ConstructionImageMap.Add(Response->CdnUrl, Name);

        CurrentUploadIndex++;
        OnUploadImageStep.ExecuteIfBound(CurrentUploadIndex + 1, Name.ToString());
        if (CurrentUploadIndex >= (UploadCount - 1))
        {
            CurrentUploadIndex = -1;
            UploadCount = 0;
            RequestSaveConstructionImage();
        }
    }
}

void FArmyResourceService::CancelUploadImage()
{
	CurrentUploadIndex = -1;
	UploadCount = 0;
	ConstructionImageMap.Empty();
}

void FArmyResourceService::UpdateAutoComponentProductInfo(int32 ComponentID,FString& PipeDiameter,FString& ObjectIDStr)
{
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter=TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
	JsonWriter->WriteObjectStart();
	int32 MealID = FArmyUser::Get().GetMealID();
	int32 MealModel = 1;
	FString ComponentCode = FArmyCommonTools::GetComponentCodeByLocalID(EComponentID(ComponentID));

	JsonWriter->WriteValue(TEXT("setMealId"),MealID);
	JsonWriter->WriteValue(TEXT("setMealModel"),MealModel);
	JsonWriter->WriteValue(TEXT("memberClientCode"), ComponentCode);
	JsonWriter->WriteValue(TEXT("pipeDiameter"),PipeDiameter);
	JsonWriter->WriteValue(TEXT("objectId"),ObjectIDStr); 
	JsonWriter->WriteObjectEnd();

	JsonWriter->Close();

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyResourceService::Callback_UpdateAutoComponentProductInfo);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(ResourceUpdateAsURL, CompleteDelegate, JStr);
    Request->ProcessRequest();
}

void FArmyResourceService::Callback_UpdateAutoComponentProductInfo(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        if (JObject.IsValid())
        {
            FString FGUID = JObject->GetStringField("objectId");
            int32 ProductID = JObject->GetNumberField("id");
            if (ProductID > 0)
            {
                OnUpdateProductInfoDelegate.ExecuteIfBound(FGUID, ProductID);
            }
        }
    }
}

void FArmyResourceService::SaveFailed(int32 InErrorCode)
{
	if (SubmitSaveRequest.IsValid())
	{
		SubmitSaveRequest->OnProcessRequestComplete().Unbind();
		SubmitSaveRequest->CancelRequest();
		SubmitSaveRequest = nullptr;
	}
	{
		CancelUploadImage();
	}
	GGI->Window->ShowMessage(MT_Warning, FString::Printf(TEXT("保存失败！(Code:%d)"), InErrorCode));
}

void FArmyResourceService::RequestFailed(int32 InErrorCode)
{
	//FArmyModalManager::Get().ShowWindow(ECommonWindowType::E_MessageConfirm, FText::FromString(TEXT("提示")));
	//FArmyModalManager::Get().SetMessage(FText::FromString(FString::Printf(TEXT("请求施工图失败！(Code:%d)"), InErrorCode)));
}
bool FArmyResourceService::RequestLoadConstructionFrameLogo()
{
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyResourceService::Callback_RequestLoadConstructionLogo);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(ResourceLogoURL, CompleteDelegate);
	Request->ProcessRequest();

	return true;
}
void FArmyResourceService::RequestSaveConstructionImage()
{
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
	JsonWriter->WriteObjectStart();
	JsonWriter->WriteValue(TEXT("version"), 1.0);

	JsonWriter->WriteValue(TEXT("planId"), FArmyUser::Get().GetCurPlanID());

	JsonWriter->WriteArrayStart(TEXT("imageList"));
	for (auto ImageInfo : ConstructionImageMap)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("name", ImageInfo.Value.ToString());
		JsonWriter->WriteValue("thumbnailUrl", ImageInfo.Key);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	//如果之前请求残留，取消
	if (SubmitSaveRequest.IsValid())
	{
		SubmitSaveRequest->OnProcessRequestComplete().Unbind();
		SubmitSaveRequest->CancelRequest();
		SubmitSaveRequest = nullptr;
	}

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyResourceService::Callback_RequestSaveConstructionImage);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(ResourceSaveImageURL, CompleteDelegate, JStr);
    Request->ProcessRequest();
}
void FArmyResourceService::Callback_RequestLoadConstructionLogo(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        if (JObject.IsValid())
        {
            FString FileMD5 = JObject->GetStringField("fileMd5");
            FString FileUrl = JObject->GetStringField("fileUrl");

            FString FileName = FString::Printf(TEXT("%d_%s.%s"), 88889, *FileMD5, *FArmyCommonTools::GetFileExtension(FileUrl));
            FString FilePath = FArmyCommonTools::GetPathFromFileName(ComponentDXF, FileName);

            TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddTask(
                FDownloadSpace::FDownloadFileInfo(88889, FilePath, FileUrl, FileMD5, true));
            NewTask->OnDownloadFinished.BindRaw(this, &FArmyResourceService::Callback_DownloadFileFinished, FilePath, 2);
        }
    }
}
void FArmyResourceService::Callback_RequestSaveConstructionImage(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        ConstructionImageMap.Empty();
        GGI->Window->ShowMessage(MT_Success, TEXT("图纸生成成功！"));

        FString Url = FString::Printf(TEXT("%s?token=%s"), *FArmyHttpModule::Get().GetBackendUrl(), *FArmyHttpModule::Get().GetToken());
        FPlatformProcess::LaunchURL(*Url, NULL, NULL);
    }
    else
    {
        //SaveFailed(ResponseCode);
    }
}

bool FArmyResourceService::DownLoadConstructionFrame()
{
	FString FileUrl = FString::Printf(TEXT("%s/%s"), *FArmyHttpModule::Get().GetOssUrl(), CONSTRUCTIONFRAMEFILE);

	TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddTask(
		FDownloadSpace::FDownloadFileInfo(88888, FArmyCommonTools::GetConstructionDir() + CONSTRUCTIONFRAMEFILENEW, FileUrl, TEXT(""), true));
		NewTask->OnDownloadFinished.BindRaw(this, &FArmyResourceService::Callback_DownloadFileFinished, FArmyCommonTools::GetConstructionDir() + CONSTRUCTIONFRAMEFILENEW,1);

	return true;
}

bool FArmyResourceService::RequestFrameFillInfo()
{
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyResourceService::Callback_RequestFrameContent);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(RequestFrameFillContentURL, CompleteDelegate);
    Request->ProcessRequest();

	return true;
}

void FArmyResourceService::Callback_RequestFrameContent(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        if (JObject.IsValid())
        {
            FArmyUser::Get().CurrentAppendInfo.ContractNo = JObject->GetStringField(TEXT("contractNo"));
            FArmyUser::Get().CurrentAppendInfo.DesignerName = JObject->GetStringField(TEXT("designerName"));
            FArmyUser::Get().CurrentAppendInfo.HomeAddress = JObject->GetStringField(TEXT("addressDetails"));
            FArmyUser::Get().CurrentAppendInfo.CustomerName = JObject->GetStringField(TEXT("customerName"));
        }
    }

	OnFrameFillContentCallBack.ExecuteIfBound();
}
void FArmyResourceService::Callback_DownloadFileFinished(bool bWasSucceed, FString InFilePath, int32 InFileType)
{
    if (bWasSucceed)
    {
		switch (InFileType)
		{
		case 1:
			OnLoadConstructionFrame.ExecuteIfBound(InFilePath);
			break;
		case 2:
			OnLoadFrameLogo.ExecuteIfBound(InFilePath);
			break;
		default:
			break;
		}
    }
    else
    {
        RequestFailed(-2);
		switch (InFileType)
		{
		case 1:
		{
			FString PreFile = FArmyCommonTools::GetConstructionDir() + CONSTRUCTIONFRAMEFILE;
			if (IFileManager::Get().FileExists(*PreFile))
			{
				OnLoadConstructionFrame.ExecuteIfBound(PreFile);
			}
			else
			{
				OnLoadConstructionFrame.ExecuteIfBound(TEXT(""));
			}
		}
			break;
		case 2:
			OnLoadFrameLogo.ExecuteIfBound(TEXT(""));
			break;
		default:
			break;
		}
    }
}
bool FArmyResourceService::RequestConstructionLayers()
{
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyResourceService::Callback_RequestConstructionLayers);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(RequestConstructionLayer, CompleteDelegate);
    Request->ProcessRequest();

	return true;
}
void FArmyResourceService::Callback_RequestConstructionLayers(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        OnLoadConstructionLayersCallBack.ExecuteIfBound(JObject);
    }
}