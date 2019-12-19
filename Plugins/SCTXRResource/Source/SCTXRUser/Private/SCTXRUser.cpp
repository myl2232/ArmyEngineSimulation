#include "SCTXRUser.h"

#include "JsonObject.h"
#include "JsonSerializer.h"
#include "Base64.h"

FSCTXRUser::FSCTXRUser()
    : bRememberMe(false)
    , bAutoLogin(false)
	, CurRelevantHomePlanID(0)
{
}

void FSCTXRUser::RequestLogin(const FString& InUsername, const FString& InPassword)
{
	ClearAll();
	CancelLogin();

    // 记录用户名和密码
    Username = InUsername;
    Password = InPassword;

    FString Url = FString::Printf(TEXT("%s/api/login?phone=%s&password=%s"), *GServerAddress, *Username, *Password);
	LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->SetVerb("POST");
	LoginRequest->SetURL(Url);
	LoginRequest->OnProcessRequestComplete().BindRaw(this, &FSCTXRUser::Callback_RequestLogin);
	LoginRequest->ProcessRequest();
}

SCTXRUSER_API void FSCTXRUser::CancelLogin()
{
	if (LoginRequest.IsValid())
	{
		LoginRequest->OnProcessRequestComplete().Unbind();
		LoginRequest->CancelRequest();
		LoginRequest = nullptr;
	}
}

void FSCTXRUser::Callback_RequestLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        TSharedPtr<FJsonObject> Data;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Data))
        {
            int32 Code = Data->GetIntegerField("code");
            if (Code == 1)
            {
                // 记录loginId和token
                TSharedPtr<FJsonObject> JObject = Data->GetObjectField("data");
                LoginId = JObject->GetStringField("loginId");
                Token = JObject->GetStringField("token");

                // 记录用户信息
                TSharedPtr<FJsonObject> UserInfoObject = JObject->GetObjectField("userInfo");
                CompanyName = UserInfoObject->GetStringField("companyName");
                Profession = UserInfoObject->GetStringField("profession");
				LoginUserName = UserInfoObject->GetStringField("username");

                // 请求一些固定不变的数据
                RequestOrientationList();
                RequestHomeTypeList();
                RequestSpaceTypeList();

                OnLoginSuccess.ExecuteIfBound();
				return;
            }
        }
    }
	OnLoginFailed.ExecuteIfBound();
}

TSharedPtr<IHttpRequest> FSCTXRUser::RequestUpload(const FString& FileName, FHttpRequestCompleteDelegate CompletedDelegate,const TArray<uint8>& Content)
{
    FString Url = FString::Printf(TEXT("%s/%s"), *GOSSServerAddress, *FileName);
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    
    FString Suffix = FileName;
    int32 Index = Suffix.Find(".", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
    Suffix = Suffix.Right(Suffix.Len() - Index);

    const FString ContentType = GetContentType(Suffix);
    const FString ContentLength = FString::FromInt(Content.Num());
    const FString Date = FDateTime::UtcNow().ToHttpDate();
    
    //FMD5 Md5Gen;
    //Md5Gen.Update(Content.GetData(), JStr.Len());
    //uint8 Digest[16];
    //Md5Gen.Final(Digest);
    //FString ContentMD5 = FBase64::Encode(Digest, 16);
    
    const FString Authorization = GenerateAuthorization(FileName, ContentType, Date);
    
    Request->SetVerb("PUT");
    Request->SetURL(Url);
    Request->SetHeader("Date", Date);
    Request->SetHeader("Cache-control", "no-cache");
    Request->SetHeader("Content-Encoding", "utf-8");
    Request->SetHeader("Content-Type", ContentType);
    Request->SetHeader("Content-Length", ContentLength);
    Request->SetHeader("Authorization", Authorization);
    Request->SetContent(Content);
	Request->OnProcessRequestComplete() = CompletedDelegate;
    Request->ProcessRequest();
	return Request;
}

void FSCTXRUser::RequestOrientationList()
{
    FString Url = "/api/home/forward";
    FHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FSCTXRUser::Callback_RequestOrientationList);
    HttpRequest(Url, CompleteDelegate);
}

void FSCTXRUser::Callback_RequestOrientationList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        TSharedPtr<FJsonObject> Data;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Data))
        {
            int32 Code = Data->GetIntegerField("code");
            if (Code == 1)
            {
                const TArray<TSharedPtr<FJsonValue>> JArray = Data->GetArrayField("data");

                OrientationList.Reset();
                for (auto& ArrayIt : JArray)
                {
                    TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
                    int32 Id = JObject->GetIntegerField("id");
                    FString Name = JObject->GetStringField("name");
                    OrientationList.Add(MakeShareable(new FSCTXRKeyValue(Id, Name)));
                }
            }
        }
    }
}

void FSCTXRUser::RequestHomeTypeList()
{
    FString Url = "/api/home/buildingType";
    FHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FSCTXRUser::Callback_RequestHomeTypeList);
    HttpRequest(Url, CompleteDelegate);
}

void FSCTXRUser::Callback_RequestHomeTypeList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        TSharedPtr<FJsonObject> Data;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Data))
        {
            int32 Code = Data->GetIntegerField("code");
            if (Code == 1)
            {
                const TArray<TSharedPtr<FJsonValue>> JArray = Data->GetArrayField("data");

                HomeTypeList.Reset();
                for (auto& ArrayIt : JArray)
                {
                    TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
                    int32 Id = JObject->GetIntegerField("id");
                    FString Name = JObject->GetStringField("name");
                    HomeTypeList.Add(MakeShareable(new FSCTXRKeyValue(Id, Name)));
                }
            }
        }
    }
}

void FSCTXRUser::RequestSpaceTypeList()
{
    FString Url = "/api/home/space";
    FHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FSCTXRUser::Callback_RequestSpaceTypeList);
    HttpRequest(Url, CompleteDelegate);
}

void FSCTXRUser::Callback_RequestSpaceTypeList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        TSharedPtr<FJsonObject> Data;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Data))
        {
            int32 Code = Data->GetIntegerField("code");
            if (Code == 1)
            {
                const TArray<TSharedPtr<FJsonValue>> JArray = Data->GetArrayField("data");

                SpaceTypeList.Reset();
                for (auto& ArrayIt : JArray)
                {
                    TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
                    int32 Id = JObject->GetIntegerField("id");
                    FString Name = JObject->GetStringField("name");
                    SpaceTypeList.Add(MakeShareable(new FSCTXRKeyValue(Id, Name)));
                }
            }
        }
    }
}

void FSCTXRUser::Clear()
{
    Username = "";
    LoginId = "";
    Token = "";
    CompanyName = "";
    Profession = "";
}

const FString FSCTXRUser::GenerateAuthorization(const FString& FileName, const FString& ContentType, const FString& Date)
{
    FString CanonicalizedResource = FString::Printf(TEXT("%s%s"), *GBucketName, *FileName);
    
    FString MsgStr = "PUT\n\n" + ContentType + "\n" + Date + "\n" + CanonicalizedResource;
    
    TArray<uint8> Msg;
    Msg.SetNum(MsgStr.Len());
    memcpy(Msg.GetData(), TCHAR_TO_ANSI(*MsgStr), MsgStr.Len());
    
    TArray<uint8> Key;
    Key.SetNum(GAccessKey.Len());
    memcpy(Key.GetData(), TCHAR_TO_ANSI(*GAccessKey), GAccessKey.Len());
    
    uint8 HashMsg[20];
    FSHA1::HMACBuffer(Key.GetData(), Key.Num(), Msg.GetData(), Msg.Num(), HashMsg);
    FString Signature = FBase64::Encode(HashMsg, 20);
    const FString Result = "OSS " + GAccessId + ":" + Signature;
    
    return Result;
}

const FString FSCTXRUser::GetContentType(const FString& FileSuffix)
{
    if (FileSuffix.Equals(".json"))
    {
        return "application/json";
    }
    else if (FileSuffix.Equals(".jpg"))
    {
        return "image/jpeg";
    }

    return "";
}

void FSCTXRUser::Init()
{
    // 加载配置文件
    if (GConfig)
    {
        FString Section = TEXT("UserData");
        GConfig->GetBool(*Section, TEXT("RememberMe"), bRememberMe, GGameIni);
        GConfig->GetBool(*Section, TEXT("AutoLogin"), bAutoLogin, GGameIni);
        GConfig->GetString(*Section, TEXT("Username"), Username, GGameIni);
        GConfig->GetString(*Section, TEXT("Password"), Password, GGameIni);
    }
}

void FSCTXRUser::ClearAll()
{
	Username.Empty();
	LoginId.Empty();
	Token.Empty();
	CompanyName.Empty();
	Profession.Empty();
	CurProjectData.Reset();
	CurHomePlanData.Reset();
	CurLevelHomeData.Reset();
	CurEditHomeData.Reset();
}

TSharedPtr<FSCTXRUser> FSCTXRUser::XRUserMgr = nullptr;

SCTXRUSER_API FSCTXRUser& FSCTXRUser::Get()
{
	if (!XRUserMgr.IsValid())
	{
		XRUserMgr = MakeShareable(new FSCTXRUser());
	}
	return *XRUserMgr;
}

TSharedPtr<IHttpRequest> FSCTXRUser::HttpRequest(const FString& Url, FHttpRequestCompleteDelegate CompletedDelegate, const FString& Content /*= ""*/)
{
    const FString FullUrl = FString::Printf(TEXT("%s%s"), *GServerAddress, *Url);
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetHeader("Content-Type", "application/json; charset=utf-8");
    Request->SetHeader("xloginId", *LoginId);
    Request->SetHeader("xtoken", *Token);
    Request->SetVerb("POST");
    Request->SetURL(FullUrl);
    if (!Content.IsEmpty())
    {
        Request->SetContentAsString(Content);
    }
    Request->OnProcessRequestComplete() = CompletedDelegate;
    Request->ProcessRequest();

	return Request;
}