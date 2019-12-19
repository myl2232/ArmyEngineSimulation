#pragma once

#include "Http.h"

class FArmyAutoDesignHttp : public TSharedFromThis<FArmyAutoDesignHttp>
{
public:
	static const TSharedRef<FArmyAutoDesignHttp>& Get();
	FArmyAutoDesignHttp() {};
	~FArmyAutoDesignHttp() {};

	void HttpGet(const FString& url, FHttpRequestCompleteDelegate complete);
	void HttpGet(const FString& url, const FString& param, FHttpRequestCompleteDelegate complete);
	void HttpPost(const FString& url, const FString& param, FHttpRequestCompleteDelegate complete);

private:
	void HttpRequestHandle(const FString& url, const FString& param, const FString& method, FHttpRequestCompleteDelegate complete);
	void OnRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived);
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FHttpRequestCompleteDelegate complete);
};