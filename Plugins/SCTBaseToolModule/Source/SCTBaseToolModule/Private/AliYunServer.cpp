#include "AliYunServer.h"
#include "HttpMgr.h"
#include "Base64.h"

const TSharedRef<FAliYunServer>& FAliYunServer::Get()
{
	static const TSharedRef<FAliYunServer> Instance = MakeShareable(new FAliYunServer);
	return Instance;
}

FAliYunServer::FAliYunServer()
{
	ALIYUN_DNS = TEXT("http://test.ali.res.dabanjia.com");
	BUCKET_NAME = TEXT("/dbj-test/");
	ACCESS_ID = "LTAIVNPppai5XhAu";
	ACCESS_KEY = "iGkaPXhWYlOeELuRVepYp4qgEXKSmE";
}

const FString& FAliYunServer::GetALiYunDNS() const
{
	return ALIYUN_DNS;
}
void FAliYunServer::SetALiYunDNS(const FString &InDNS)
{
	ALIYUN_DNS = InDNS;
}
void FAliYunServer::SetALiYunBucketName(const FString &InName)
{
	BUCKET_NAME = InName;
}
void FAliYunServer::SetALiYunAccessId(const FString &InId)
{
	ACCESS_ID = InId;
}
void FAliYunServer::SetALiYunAccessKey(const FString &InKey)
{
	ACCESS_KEY = InKey;
}

void FAliYunServer::RequestFileIfExistInAliYun(const FHttpRequestCompleteDelegate &RequestCompleteCallback, const FString &FileName)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	FString URL = FString::Printf(TEXT("%s/%s"), *ALIYUN_DNS, *FileName);
	FString Date = FDateTime::UtcNow().ToHttpDate();

	// 生成授权字符串
	FString CanonicalizedResource = FString::Printf(TEXT("%s%s"), *BUCKET_NAME, *FileName);
	FString MsgStr = "GET\n\n\n" + Date + "\n" + CanonicalizedResource;

	TArray<uint8> Msg;
	Msg.SetNum(MsgStr.Len());
	memcpy(Msg.GetData(), TCHAR_TO_ANSI(*MsgStr), MsgStr.Len());

	TArray<uint8> Key;
	Key.SetNum(ACCESS_KEY.Len());
	memcpy(Key.GetData(), TCHAR_TO_ANSI(*ACCESS_KEY), ACCESS_KEY.Len());

	uint8 HashMsg[20];
	FSHA1::HMACBuffer(Key.GetData(), Key.Num(), Msg.GetData(), Msg.Num(), HashMsg);
	FString Signature = FBase64::Encode(HashMsg, 20);
	FString Result = "OSS " + ACCESS_ID + ":" + Signature;
	// 生成授权字符串结束

	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(URL);
	HttpRequest->SetHeader("Date", Date);
	HttpRequest->SetHeader("Cache-control", "no-cache");
	HttpRequest->SetHeader("Content-Encoding", "utf-8");
	HttpRequest->SetHeader("Authorization", Result);

	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
	HttpRequest->ProcessRequest();
}

FString GetContentType(const FString &Suffix)
{
	if (Suffix == TEXT("json"))
	{
		return TEXT("application/json");
	}
	else if (Suffix == TEXT("xml"))
	{
		return TEXT("application/xml");
	}
	else if (Suffix == TEXT("dae"))
	{
		return TEXT("application/octet-stream");
	}
	else if (Suffix == TEXT("jpeg") || Suffix == TEXT("jpg"))
	{
		return TEXT("image/jpeg");
	}
	return FString();
}

void FAliYunServer::RequestUploadFileAliYun(const FHttpRequestCompleteDelegate &RequestCompleteCallback, const TArray<uint8> &Data, const FString &FileName)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	FString URL = FString::Printf(TEXT("%s/%s"), *ALIYUN_DNS, *FileName);

	int32 Index = FileName.Find(TEXT("."), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	FString Suffix;
	if (Index != INDEX_NONE)
	{
		Suffix = FileName.Right(FileName.Len() - Index - 1);
	}
	FString ContentType = GetContentType(Suffix);

	FString ContentLength = FString::FromInt(Data.Num());

	FString Date = FDateTime::UtcNow().ToHttpDate();

	FString base64;
	/*if (Data.Num() > 0) {
	FMD5 md5;
	md5.Update(Data.GetData(), Data.Num());
	uint8 digest[16];
	md5.Final(digest);
	base64 = FBase64::Encode(digest, 16);
	}*/

	// 生成授权字符串
	FString CanonicalizedResource = FString::Printf(TEXT("%s%s"), *BUCKET_NAME, *FileName);
	FString MsgStr = "PUT\n" + base64 + "\n" + ContentType + "\n" + Date + "\n" + CanonicalizedResource;

	TArray<uint8> Msg;
	Msg.SetNum(MsgStr.Len());
	memcpy(Msg.GetData(), TCHAR_TO_ANSI(*MsgStr), MsgStr.Len());

	TArray<uint8> Key;
	Key.SetNum(ACCESS_KEY.Len());
	memcpy(Key.GetData(), TCHAR_TO_ANSI(*ACCESS_KEY), ACCESS_KEY.Len());

	uint8 HashMsg[20];
	FSHA1::HMACBuffer(Key.GetData(), Key.Num(), Msg.GetData(), Msg.Num(), HashMsg);
	FString Signature = FBase64::Encode(HashMsg, 20);
	FString Result = "OSS " + ACCESS_ID + ":" + Signature;
	// 生成授权字符串结束

	HttpRequest->SetVerb("PUT");
	HttpRequest->SetURL(URL);
	HttpRequest->SetHeader("Date", Date);
	HttpRequest->SetHeader("Cache-control", "no-cache");
	HttpRequest->SetHeader("Content-Encoding", "utf-8");
	HttpRequest->SetHeader("Content-Type", ContentType);
	HttpRequest->SetHeader("Content-Length", ContentLength);
	HttpRequest->SetHeader("Authorization", Result);
	HttpRequest->SetContent(Data);

	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
	HttpRequest->ProcessRequest();
}

void FAliYunServer::CheckServerFolder(const FString &Folder)
{
	RequestFileIfExistInAliYun(
		FHttpRequestCompleteDelegate::CreateLambda(
			[this, Folder](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessfull) {
				if (Response.IsValid())
				{
					int32 Code = Response->GetResponseCode();
					if (Code == 404)
					{
						CreateFileFolderOnServer(Folder);
					}
					else if (Code == 200)
					{
						//已经创建
					}
				}
				else
				{
					//
				}
			}
		),
		Folder
	);
}

void FAliYunServer::CreateFileFolderOnServer(const FString &Folder)
{
	TArray<uint8> Data;
	RequestUploadFileAliYun(
		FHttpRequestCompleteDelegate::CreateLambda(
			[this](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessfull) {
				FHttpMgr::Get()->CheckHttpResponseValid(Response, bWasSuccessfull);
				if (Response->GetResponseCode() == 200)
				{
					//创建成功
				}
				else
				{
					//创建失败
				}
			}
		),
		Data,
		Folder
	);
}

