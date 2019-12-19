#include "ArmyAutoDesignHttp.h"

const TSharedRef<FArmyAutoDesignHttp>& FArmyAutoDesignHttp::Get()
{
	static const TSharedRef<FArmyAutoDesignHttp> Instance = MakeShareable(new FArmyAutoDesignHttp);
	return Instance;
}

void FArmyAutoDesignHttp::HttpGet(const FString& url, FHttpRequestCompleteDelegate complete)
{
	this->HttpRequestHandle(url, TEXT(""), TEXT("GET"), complete);
}

void FArmyAutoDesignHttp::HttpGet(const FString& url, const FString& param, FHttpRequestCompleteDelegate complete)
{
	this->HttpRequestHandle(url, param, TEXT("GET"), complete);
}

void FArmyAutoDesignHttp::HttpPost(const FString& url, const FString& param, FHttpRequestCompleteDelegate complete)
{
	this->HttpRequestHandle(url, param, TEXT("POST"), complete);
}


//////////////////////////////////////////////////

void FArmyAutoDesignHttp::HttpRequestHandle(const FString& url, const FString& param, const FString& method, FHttpRequestCompleteDelegate complete)
{
	//FHttpModule httpModule = FHttpModule::Get();
	//httpModule.SetHttpTimeout(60);
	FHttpRequestPtr httpRequest = FHttpModule::Get().CreateRequest();
	httpRequest->SetVerb(method);
	if (method.Equals(TEXT("GET"), ESearchCase::IgnoreCase))
	{
		if (param.IsEmpty() == false && param.Len() != 0)
		{
			FString urlStr = url + "?" + param;
			httpRequest->SetURL(urlStr);
		}
		else
		{
			httpRequest->SetURL(url);
		}
	} 
	else if (method.Equals(TEXT("POST"), ESearchCase::IgnoreCase))
	{
		httpRequest->SetURL(url);
		httpRequest->SetContentAsString(param);
	} 
	else 
	{
		httpRequest->SetURL(url);
	}
	httpRequest->OnProcessRequestComplete().BindRaw(this, &FArmyAutoDesignHttp::OnRequestComplete, complete);
	httpRequest->OnRequestProgress().BindRaw(this, &FArmyAutoDesignHttp::OnRequestProgress);
	// 开始请求服务器
	httpRequest->ProcessRequest();
}

//
void FArmyAutoDesignHttp::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FHttpRequestCompleteDelegate complete)
{
	UE_LOG(LogHttp, Log, TEXT("URL: %s"), *HttpRequest->GetURL());
	//
	if (bSucceeded)
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			// 成功，仅代表网络正常返回数据
			//至于数据是否是正常的业务数据，则需要在业务代码中判断对应的业务code，一般10000表示业务数据正常
			complete.ExecuteIfBound(HttpRequest, HttpResponse, true);
		}
		else
		{
			const FString Payload = HttpResponse->GetContentAsString();
			complete.ExecuteIfBound(HttpRequest, HttpResponse, false);
			UE_LOG(LogHttp, Log, TEXT("失败: %s"), *Payload);
			//GGI->Window->ShowMessage(MT_Warning, Payload);
		}
	}
	else
	{
		// 失败
		complete.ExecuteIfBound(HttpRequest, HttpResponse, false);
		UE_LOG(LogHttp, Log, TEXT("失败: %s"), TEXT("网络错误，请联系管理员"));
		//GGI->Window->ShowMessage(MT_Warning, TEXT("网络错误，请联系管理员"));
	}
}

// 请求过程回调
void FArmyAutoDesignHttp::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	//UE_LOG(XRLogHttp, Log, TEXT("URL: %s"), *HttpRequest->GetURL());
	//这部分回调仅在需要知道进度信息的时候有用处，大部分表示下载或者上传的进度百分比信息
}
