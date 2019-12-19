/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 服务接口
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "IHttpRequest.h"
#include "HttpModule.h"
#include "IHttpResponse.h"


class SCTBASETOOLMODULE_API FHttpMgr
{
private:
	FHttpMgr();

public:
	static const TSharedRef<FHttpMgr>& Get();

public:
	DECLARE_DELEGATE_OneParam(FOnHttpError, const FString &);
	FOnHttpError& GetHttpErrorDelegate(bool bNeedUnbind = true);

	void SetShapeLibraryDomainName(const FString &ShapeLibraryDomainName);
	const FString& GetShapeLibraryDomainName() const { return ShapeLibraryDomainName; }
	void SetDomainName(const FString &InDomainName);
	const FString& GetDomainName() const { return DomainName; }
	void SetLoginIdAndToken(const FString &InLoginId, const FString & OutLoginToken);
	void GetLoginIdAndToken(FString & OutId, FString & OutToken) { OutId = LoginId; OutToken = LoginToken; }
	void SetRequestTimeout(float InTimeoutInSecond);

	TSharedRef<IHttpRequest> CreateHttpRequest(const FString &_URL, const FString &_Verb);
	bool CheckHttpResponseValid(const FHttpResponsePtr &_Response, bool _bWasSuccessful) const;
	TSharedPtr<class FJsonObject> GetContentAsJsonObject(const FHttpResponsePtr &_Response, FString *OutErrorMsg = nullptr) const;

private:
	FString ShapeLibraryDomainName;   //前端地址
	FString DomainName;               //后台地址
	FString LoginId;
	FString LoginToken;

	FOnHttpError HttpError;
};