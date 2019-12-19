#include "AliyunOssHttp.h"
#include "AliyunOssUtil.h"

const FString FAliyunOSSRequest::GetUrl() const
{
    return "http://" + BucketName + "." + Endpoint + GetResourcePath();
}

const FString FAliyunOSSRequest::GetCdnUrl() const
{
    return CdnUrl + Dir + "/" + FileName;
}

const FString FAliyunOSSRequest::GetResourcePath() const
{
    return "/" + Dir + "/" + FileName;
}

const FString FAliyunOSSRequest::GetCanonicalizedOssHeaders() const
{
    return "x-oss-security-token:" + SecurityToken + "\n";
}

const FString FAliyunOSSRequest::GetCanonicalizedResource() const
{
    return "/" + BucketName + GetResourcePath();
}

TSharedPtr<IHttpRequest> FAliyunOSSRequest::Build()
{
    const FString Date = FDateTime::UtcNow().ToHttpDate();
    const FString ContentMd5 = FAliyunOssUtil::GenerateContentMd5(Content);
    const FString ContentType = FAliyunOssUtil::GetContentType(FileName);
    const FString Authorization = FAliyunOssUtil::GenerateAuthorization(ContentMd5, ContentType, Date, 
        GetCanonicalizedOssHeaders(), GetCanonicalizedResource(), AccessKeyId, AccessKeySecret);

    TSharedPtr<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("PUT");
    Request->SetURL(GetUrl());
    Request->SetHeader("Date", Date);
    Request->SetHeader("Content-MD5", ContentMd5);
    Request->SetHeader("Content-Type", ContentType);
    Request->SetHeader("x-oss-security-token", SecurityToken);
    Request->SetHeader("Authorization", Authorization);
    Request->SetContent(Content);

    return Request;
}

void FAliyunOssResponse::Receive(bool InbWasSuccessful, FHttpResponsePtr Response)
{
    if (Response.IsValid())
    {
        ResponseCode = Response->GetResponseCode();

        if (InbWasSuccessful && EHttpResponseCodes::IsOk(ResponseCode))
        {
            bWasSuccessful = true;
        }
        else
        {
            bWasSuccessful = false;
            Message = Response->GetContentAsString();
        }
    }
}