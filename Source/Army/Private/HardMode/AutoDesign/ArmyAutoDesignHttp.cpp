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
	// ��ʼ���������
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
			// �ɹ�������������������������
			//���������Ƿ���������ҵ�����ݣ�����Ҫ��ҵ��������ж϶�Ӧ��ҵ��code��һ��10000��ʾҵ����������
			complete.ExecuteIfBound(HttpRequest, HttpResponse, true);
		}
		else
		{
			const FString Payload = HttpResponse->GetContentAsString();
			complete.ExecuteIfBound(HttpRequest, HttpResponse, false);
			UE_LOG(LogHttp, Log, TEXT("ʧ��: %s"), *Payload);
			//GGI->Window->ShowMessage(MT_Warning, Payload);
		}
	}
	else
	{
		// ʧ��
		complete.ExecuteIfBound(HttpRequest, HttpResponse, false);
		UE_LOG(LogHttp, Log, TEXT("ʧ��: %s"), TEXT("�����������ϵ����Ա"));
		//GGI->Window->ShowMessage(MT_Warning, TEXT("�����������ϵ����Ա"));
	}
}

// ������̻ص�
void FArmyAutoDesignHttp::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived)
{
	//UE_LOG(XRLogHttp, Log, TEXT("URL: %s"), *HttpRequest->GetURL());
	//�ⲿ�ֻص�������Ҫ֪��������Ϣ��ʱ�����ô����󲿷ֱ�ʾ���ػ����ϴ��Ľ��Ȱٷֱ���Ϣ
}
