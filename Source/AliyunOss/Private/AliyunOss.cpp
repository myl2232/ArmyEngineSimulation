#include "AliyunOss.h"
#include "AliyunOssModule.h"
#include "AliyunOssUtil.h"
#include "Base64.h"
#include "FileHelper.h"
#include "HttpModule.h"
#include "Json.h"
#include "ModuleManager.h"

TSharedPtr<FAliyunOss> FAliyunOss::Singleton = nullptr;

FAliyunOss& FAliyunOss::Get()
{
    if (!Singleton.IsValid())
    {
        Singleton = MakeShareable(new FAliyunOss());
    }

    return *Singleton;
}

void FAliyunOss::InitUserConfig(TSharedPtr<FUserConfig> InUserConfig)
{
    InUserConfig->CheckValidation();

    UserConfig = InUserConfig;

    // 加载Content-Type对应表 
    FAliyunOssUtil::LoadContentTypeMap();
}

void FAliyunOss::PutObject(EOssDir OssDir, const FString& FileName, const TArray<uint8>& Content, FOnUploadComplete OnUploadComplete)
{
    // 构造上传请求
    TSharedPtr<FAliyunOSSRequest> OssRequest = MakeShareable(new FAliyunOSSRequest());
    OssRequest->FileName = FileName;
    OssRequest->Content = Content;
    OssRequest->OnUploadComplete = OnUploadComplete;
    OssRequestMap.Add(FileName, OssRequest);

    // 获取阿里云oss配置
    ReqOssConfig(OssDir, "", FileName);
}

ALIYUNOSS_API void FAliyunOss::PutObject(EOssDir OssDir, const FString& Dir, const FString& FileName, const TArray<uint8>& Content, FOnUploadComplete OnUploadComplete)
{
    // 已经存在的上传任务不能重复添加
    if (OssRequestMap.Find(FileName))
    {
        return;
    }

    // 构造上传请求
    TSharedPtr<FAliyunOSSRequest> OssRequest = MakeShareable(new FAliyunOSSRequest());
    OssRequest->FileName = FileName;
    OssRequest->Content = Content;
    OssRequest->OnUploadComplete = OnUploadComplete;
    OssRequestMap.Add(FileName, OssRequest);

    // 获取阿里云oss配置
    ReqOssConfig(OssDir, Dir, FileName);
}

void FAliyunOss::ReqOssConfig(EOssDir OssDir, const FString& Dir, FString FileName)
{
    const FString Url = Dir.IsEmpty() ? FString::Printf(TEXT("%s/api/oss/token?dirType=%d&tokenName=%s"), *UserConfig->ServerAddress, (int32)OssDir, *UserConfig->LoginId) :
		FString::Printf(TEXT("%s/api/oss/token?dirType=%d&tokenName=%s&dir=%s"), *UserConfig->ServerAddress, (int32)OssDir, *UserConfig->LoginId, *Dir);

    TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("GET");
    Request->SetHeader("Content-Type", "application/json; charset=utf-8");
    Request->SetHeader("xloginId", *UserConfig->LoginId);
    Request->SetHeader("xtoken", *UserConfig->Token);
    Request->SetURL(Url);
    Request->OnProcessRequestComplete().BindRaw(this, &FAliyunOss::ResOssConfig, FileName);
    Request->ProcessRequest();
}

void FAliyunOss::ResOssConfig(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString FileName)
{
    if (bWasSuccessful)
    {
        if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
        {
            TSharedPtr<FJsonObject> Data;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Data))
            {
                int32 Code = Data->GetIntegerField("code");
                if (Code == 1)
                {
                    const TSharedPtr<FJsonObject> JData = Data->GetObjectField("data");
                    if (JData.IsValid() && OssRequestMap.Contains(FileName))
                    {
                        // 获取阿里云oss临时访问凭证，构造上传请求
                        TSharedPtr<FAliyunOSSRequest> OssRequest = *OssRequestMap.Find(FileName);
                        OssRequest->AccessKeyId = JData->GetStringField("accessKeyId");
                        OssRequest->AccessKeySecret = JData->GetStringField("accessKeySecret");
                        OssRequest->SecurityToken = JData->GetStringField("securityToken");
                        OssRequest->Endpoint = JData->GetStringField("endPoint");
                        OssRequest->BucketName = JData->GetStringField("bucketName");
                        OssRequest->CdnUrl = JData->GetStringField("cdnUrl");
                        OssRequest->Dir = JData->GetStringField("dir");

                        // 上传文件到oss
                        TSharedPtr<IHttpRequest> UploadRequest = OssRequest->Build();
                        UploadRequest->OnProcessRequestComplete().BindRaw(this, &FAliyunOss::ResUploadOss, OssRequest->FileName);
                        UploadRequest->ProcessRequest();
                    }
                }
            }
        }
    }
    else
    {
        TSharedPtr<FAliyunOssResponse> OssReponse = MakeShareable(new FAliyunOssResponse());
        OssReponse->Receive(bWasSuccessful, Response);

        if (OssRequestMap.Contains(FileName))
        {
            TSharedPtr<FAliyunOSSRequest> OssRequest = *OssRequestMap.Find(FileName);
            if (OssRequest.IsValid())
            {
                OssReponse->CdnUrl = OssRequest->GetCdnUrl();
                OssRequest->OnUploadComplete.ExecuteIfBound(OssReponse);
                OssRequestMap.Remove(FileName);
            }
        }
    }
}

void FAliyunOss::ResUploadOss(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString FileName)
{
    TSharedPtr<FAliyunOssResponse> OssReponse = MakeShareable(new FAliyunOssResponse());
    OssReponse->Receive(bWasSuccessful, Response);

    if (OssRequestMap.Contains(FileName))
    {
        TSharedPtr<FAliyunOSSRequest> OssRequest = *OssRequestMap.Find(FileName);
        if (OssRequest.IsValid())
        {
            OssReponse->CdnUrl = OssRequest->GetCdnUrl();
            OssRequest->OnUploadComplete.ExecuteIfBound(OssReponse);
            OssRequestMap.Remove(FileName);
        }
    }
}
