#include "HttpMgr.h"
#include "JsonReader.h"
//#include "JsonWriter.h"
//#include "Misc/SecureHash.h"
//#include "Misc/Base64.h"
#include "JsonSerializer.h"

// const FString DBJ_DNS = TEXT("http://designtool.lsmuyprt.com/");
// const FString ACCESS_ID = "LTAIR5IzYIEhRF27";
// const FString ACCESS_KEY = "j3ur9hO6tehCDIHLTvA9mmOG8oUp5N";

const TSharedRef<FHttpMgr>& FHttpMgr::Get()
{
	static const TSharedRef<FHttpMgr> Instance = MakeShareable(new FHttpMgr);
	return Instance;
}

FHttpMgr::FHttpMgr()
{

}

FHttpMgr::FOnHttpError& FHttpMgr::GetHttpErrorDelegate(bool bNeedUnbind)
{
	if (bNeedUnbind && HttpError.IsBound())
		HttpError.Unbind();
	return HttpError;
}

void FHttpMgr::SetShapeLibraryDomainName(const FString &InShapeLibraryDomainName)
{
	ShapeLibraryDomainName = InShapeLibraryDomainName;
}

void FHttpMgr::SetDomainName(const FString &InDomainName)
{
	DomainName = InDomainName;
}

void FHttpMgr::SetLoginIdAndToken(const FString &InLoginId, const FString &InLoginToken)
{
	LoginId = InLoginId;
	LoginToken = InLoginToken;
}

void FHttpMgr::SetRequestTimeout(float InTimeoutInSecond)
{
	FHttpModule::Get().SetHttpTimeout(InTimeoutInSecond);
}

TSharedRef<IHttpRequest> FHttpMgr::CreateHttpRequest(const FString &_URL, const FString &_Verb)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(DomainName + _URL);
	HttpRequest->SetVerb(_Verb);
	HttpRequest->SetHeader(TEXT("xtoken"), LoginToken);
	HttpRequest->SetHeader(TEXT("xloginid"), LoginId);
	return HttpRequest;
}

bool FHttpMgr::CheckHttpResponseValid(const FHttpResponsePtr &_Response, bool _bWasSuccessful) const
{
	return _bWasSuccessful ? EHttpResponseCodes::IsOk(_Response->GetResponseCode()) : false;
}

TSharedPtr<FJsonObject> FHttpMgr::GetContentAsJsonObject(const FHttpResponsePtr &_Response, FString *OutErrorMsg) const
{
	FString Content = _Response->GetContentAsString();
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Content);
	TSharedPtr<FJsonObject> Data;
	if (FJsonSerializer::Deserialize(JsonReader, Data))
	{
		if (Data->HasField(TEXT("code")))
		{
			int32 retCode = Data->GetIntegerField(TEXT("code"));
			if (retCode == 1)
				return Data;
		}
		//HttpError.ExecuteIfBound(Data->GetStringField(TEXT("message")));
		*OutErrorMsg = Data->GetStringField(TEXT("message"));
	}
	else
		*OutErrorMsg = TEXT("数据不是一个有效的JSON对象");
		//HttpError.ExecuteIfBound(TEXT("数据错误"));
	return nullptr;
}

// TSharedPtr<FJsonObject> FHttpMgr::CheckHttpResponseValid(FHttpRequestPtr _Request, FHttpResponsePtr _Response, bool _bWasSuccessful)
// {
// 	if (!_Response.IsValid() || !_bWasSuccessful)
// 	{
// 		return nullptr;
// 	}
// 	FString Content = _Response->GetContentAsString();
// 	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(Content);
// 	TSharedPtr<FJsonObject> Data;
// 	if (FJsonSerializer::Deserialize(JsonReader, Data))
// 	{
// 		if (Data->HasField(TEXT("code")))
// 		{
// 			int32 retCode = Data->GetIntegerField(TEXT("code"));
// 			if (retCode == 1)
// 				return Data;
// 			else {
// 				HttpError.ExecuteIfBound(Data->GetStringField(TEXT("message")));
// 				return nullptr;
// 			}
// 		}
// 	}
// 	return nullptr;
// }

// void FHttpMgr::RequestLogin(const FHttpRequestCompleteDelegate& RequestCompleteCallback, const FString &LoginId, const FString &PassWord)
// {
// 	FString URLStr = DBJ_DNS + FString::Printf(TEXT("api/login?loginId=%s&password=%s"), LoginId.GetCharArray().GetData(), PassWord.GetCharArray().GetData());
// 	TSharedPtr<IHttpRequest> HttpRequest = CreateHttpRequest(URLStr, TEXT("POST"));
// 	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
// 	HttpRequest->ProcessRequest();
// }

// void FHttpMgr::RequestLoginCheck(const FHttpRequestCompleteDelegate& RequestCompleteCallback, const FString &LoginId, const FString &LoginToken)
// {
// 	FString URLStr = DBJ_DNS + FString::Printf(TEXT("api/lin/loginCheck?loginId=%s&loginToken=%s"), LoginId.GetCharArray().GetData(), LoginToken.GetCharArray().GetData());
// 	TSharedPtr<IHttpRequest> HttpRequest = CreateHttpRequest(URLStr, TEXT("POST"));
// 	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
// 	HttpRequest->ProcessRequest();
// }

// void FHttpMgr::RequestAllBoardShape(const FHttpRequestCompleteDelegate& RequestCompleteCallback)
// {
// 	FString URLStr = TEXT("http://60.205.56.190:8282/api/plank/list");
// 	TSharedPtr<IHttpRequest> HttpRequest = CreateHttpRequest(URLStr, TEXT("GET"));
// 	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
// 	HttpRequest->ProcessRequest();
// }

// void FHttpMgr::RequestAllDecorationModel(const FHttpRequestCompleteDelegate& RequestCompleteCallback)
// {
	
// }

// void FHttpMgr::RequestAllAccessoryModel(const FHttpRequestCompleteDelegate& RequestCompleteCallback)
// {
// 	FString URLStr = TEXT("http://60.205.56.190:8282/api/materials/metals/list");
// 	TSharedPtr<IHttpRequest> HttpRequest = CreateHttpRequest(URLStr, TEXT("GET"));
// 	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
// 	HttpRequest->ProcessRequest();
// }

// void FHttpMgr::RequestFileData(FString FileUrl, int32 fileOffset, int64 PacketSize, 
// 	const FHttpRequestCompleteDelegate& RequestCompleteCallback)
// {
// 	FString HeaderValue = FString::Printf(TEXT("bytes=%i-%i"), fileOffset, fileOffset + PacketSize);
// 	TSharedPtr<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(FileUrl, TEXT("Get"), TEXT("Range"), HeaderValue);
// 	HttpRequest->OnProcessRequestComplete() = RequestCompleteCallback;
// 	HttpRequest->ProcessRequest();
// }
