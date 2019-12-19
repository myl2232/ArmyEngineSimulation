#include "ArmyUser.h"

#include "JsonObject.h"
#include "JsonSerializer.h"
#include "Base64.h"
#include "ArmyCommonTypes.h"
#include "ArmyCoreModule.h"
#include "ArmyHttpModule.h"
#include "ArmyGameInstance.h"
#include "ArmyWindow.h"
#include "AliyunOss.h"
#include "ArmyGlobalActionCallBack.h"
#include "ArmyCommonTools.h"
#include "SArmyTextBlock.h"
#include "ArmyStyle.h"
#include "SArmyUpdatedNotes.h"

FArmyUser::FArmyUser()
    : bRememberMe(false)
    , bAutoLogin(false)
    , SpaceTypeList(MakeShareable(new FArmyComboBoxArray))
	, RoomUniqueId(-1)
{
}

void FArmyUser::RequestLogin(const FString& InUsername, const FString& InPassword)
{
	ClearAll();
	CancelLogin();

    // 记录用户名和密码
    Username = InUsername;
    Password = InPassword;

    FString Content;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Content);
    JsonWriter->WriteObjectStart();
    JsonWriter->WriteValue("phone", *Username);
    JsonWriter->WriteValue("password", *Password);
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    const FString Url = FString::Printf(TEXT("%s/api/login"), *FArmyHttpModule::Get().GetServerAddress());
    FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("POST");
    Request->SetURL(Url);
    Request->SetContentAsString(Content);
    Request->OnProcessRequestComplete().BindRaw(this, &FArmyUser::ResponseLogin);
    Request->SetHeader("Content-Type", "application/json");
    Request->ProcessRequest();
}

ARMY_USER_API void FArmyUser::CancelLogin()
{
	if (LoginRequest.IsValid())
	{
		LoginRequest->OnProcessRequestComplete().Unbind();
		LoginRequest->CancelRequest();
		LoginRequest = nullptr;
	}
}

void FArmyUser::ResponseLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessfull)
{
    GGI->Window->HideThrobber();
    GGI->Window->DismissModalDialog();

    if (bWasSuccessfull && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
    {
        TSharedPtr<FJsonObject> Content;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Content))
        {
            TSharedPtr<FJsonObject> Data = Content->GetObjectField("data");
            Data->TryGetStringField("token", Token);

            FArmyHttpModule::Get().Init(Username, Token);

            // 记录用户信息
            TSharedPtr<FJsonObject> UserData = Data->GetObjectField("user");
            UserData->TryGetStringField("companyName", CompanyName);
            UserData->TryGetStringField("logo", AppLogoUrl);
            UserData->TryGetStringField("username", LoginUserName);

            // 初始化阿里云oss用户配置
            FAliyunOss::Get().InitUserConfig(MakeShareable(new FUserConfig(
                FArmyHttpModule::Get().GetOssTokenUrl(), FArmyHttpModule::Get().GetLoginId(), FArmyHttpModule::Get().GetToken())));

            // 请求一些固定不变的数据
            RequestUpgradeNotes();
            ReqConstructionWallData();
            ReqConstructionTopData();
            ReqConstructionFloorData();
            ReqConstructionModifyWallData();
            ReqConstructionAddWallData();
            ReqConstructionNewPassData();
			ReqConstructionNewPassByMatData();
            ReqConstructionFillPassData();
            ReqConstructionAddPassData();
            ReqConstructionModifyPassData();
            ReqConstructionOriginPassData();            
            ReqConstructionPackPipeData();
			ReqConstructionVerticalAddData();
			ReqConstructionVerticalModifyData();
            if (!IsLoggedIn())
            {
                RequestSpaceTypeList();
                OnLoginSuccess.ExecuteIfBound();
                bLoggedIn = true;
            }
        }
    }
    else
    {
        TSharedPtr<FJsonObject> Content;
        if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Response->GetContentAsString()), Content))
        {
            const int32 Code = Content->GetNumberField("code");
            const FString Message = Content->GetStringField("message");

            if (Code == ArmyHttp::ALREADY_LOGGED_IN) // @欧石楠 其它设备已登录
            {
                TSharedPtr<FJsonObject> Data = Content->GetObjectField("data");
                Data->TryGetStringField("token", Token);
            }

            if (!IsLoggedIn())
            {
                OnLoginFailed.ExecuteIfBound(Code, Message);
            }
        }

        bLoggedIn = false;
    }
}

void FArmyUser::Relogin()
{
    if (bShowReloginMessage)
    {
        TSharedPtr<SWidget> DialogWidget =
            SNew(SBox)
		    .MinDesiredWidth(350)
		    .MinDesiredHeight(100)
		    .Padding(FMargin(20, 30, 20, 20))
		    .HAlign(HAlign_Left)
		    .VAlign(VAlign_Center)
		    [
			    SNew(SHorizontalBox)

			    + SHorizontalBox::Slot()
			    .HAlign(HAlign_Left)
                .VAlign(VAlign_Center)
			    .AutoWidth()
			    [
				    SNew(SBox)
				    .HeightOverride(32)
				    .WidthOverride(32)
				    [
					    SNew(SImage)
					    .Image(FArmyStyle::Get().GetBrush("Icon.ModalDialog_Warning"))
				    ]
			    ]

			    + SHorizontalBox::Slot().Padding(FMargin(16, 0, 0, 0))
			    .VAlign(EVerticalAlignment::VAlign_Center)
			    [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(STextBlock)
				        .Text(MAKE_TEXT("您的账号已经在其他设备登录，当前设备已被迫下线，"))
				        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
				        .Justification(ETextJustify::Center)
                    ]

				    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 16, 0, 0)
                    [
                        SNew(SHorizontalBox)
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
				            .Text(MAKE_TEXT("如果不是您本人操作，请及时"))
				            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
				            .Justification(ETextJustify::Center)
                        ]
                        
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(4, 0)
                        [
                            SNew(SBox)
                            .HeightOverride(24)
                            [
                                SNew(SArmyTextBlock)
                                .bShowUnderline(true)
                                .Text(MAKE_TEXT("修改密码"))
                                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFD9800"))
                                .OnClicked_Lambda([this]() {
                                    // @欧石楠 修改密码
                                    FString URL = FArmyHttpModule::Get().GetConsoleAddress() + "/passwordReset.html";
	                                FPlatformProcess::LaunchURL(*URL, nullptr, nullptr);
                                })
                            ]
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew(STextBlock)
				            .Text(MAKE_TEXT("以保证账号安全"))
				            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
				            .Justification(ETextJustify::Center)
                        ]
                    ]
			    ]
		    ];

        FSimpleDelegate OnConfirm, OnCancel, OnClose;
        OnConfirm.BindLambda([this]() {
            FArmyUser::Get().ReqRelogin();
            GGI->Window->ShowThrobber(MAKE_TEXT("重新登录中..."));
        });
        OnCancel.BindLambda([this]() {
            FArmyGlobalActionCallBack::Get().Logout();
        });
        OnClose.BindLambda([this]() {
            bShowReloginMessage = true;
        });
        GGI->Window->PresentModalDialog(TEXT("下线提醒"), DialogWidget.ToSharedRef(), OnConfirm, OnCancel, OnClose, true,
            TEXT("重新登录"), TEXT("退出"));
    }
    
    bShowReloginMessage = false;
}

void FArmyUser::ReqRelogin()
{
    FString Content;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Content);
    JsonWriter->WriteObjectStart();
    JsonWriter->WriteValue("token", *Token);
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    const FString Url = FString::Printf(TEXT("%s/api/override_login"), *FArmyHttpModule::Get().GetServerAddress());
    FHttpRequestPtr Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("POST");
    Request->SetURL(Url);
    Request->SetContentAsString(Content);
    Request->OnProcessRequestComplete().BindRaw(this, &FArmyUser::ResponseLogin);
    Request->SetHeader("Content-Type", "application/json");
    Request->ProcessRequest();
}

void FArmyUser::RequestUpgradeNotes()
{
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindLambda([this](FArmyHttpResponse Response) {
        if (Response.bWasSuccessful)
        {
            TSharedPtr<FJsonObject> DataObj = Response.Data->GetObjectField("data");
            FString Content = DataObj->GetStringField("description");

            // 显示版本更新内容
            if (Content.Len() > 0 && CheckHasNewUpdates())
            {
                GGI->Window->PresentModalDialog(SNew(SArmyUpdatedNotes)
                    .Content(Content)
                    .OnConfirm_Lambda([this]() {
                    GGI->Window->DismissModalDialog();
                    return FReply::Handled();
                }));

                // 版本更新内容只在第一次更新后打开显示一次，之后不再显示，除非有新的更新
                GConfig->SetString(TEXT("UserData"), TEXT("OldVersion"), *(GetApplicationVersion()), GGameIni);
                GConfig->Flush(false, GGameIni);
            }
        }
    });

    const FString Url = FString::Printf(TEXT("/common/client-upgrade/%d"), (int32)(FArmyHttpModule::Get().GetClientType()));
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyUser::RequestSpaceTypeList()
{
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(
        "/common/spaces/1", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResponseSpaceTypeList));
    Request->ProcessRequest();
}

void FArmyUser::ResponseSpaceTypeList(struct FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful && Response.Data.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");

        SpaceTypeList->Reset();
        for (auto& ArrayIt : JArray)
        {
            TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
            int32 Id = JObject->GetIntegerField("id");
            FString Name = JObject->GetStringField("name");
            SpaceTypeList->Add(MakeShareable(new FArmyKeyValue(Id, Name)));
        }

        OnSpaceTypeListReady.Broadcast();
    }
}

void FArmyUser::ReqConstructionWallData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/1", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionWallData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionWallData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionWallData = Response.Data;
		OnConstructionWallDataReady.ExecuteIfBound();
	}
}

void FArmyUser::ReqConstructionTopData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/2", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionTopData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionTopData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionTopData = Response.Data;
		OnConstructionTopDataReady.ExecuteIfBound();
	}
}

void FArmyUser::ReqConstructionFloorData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/3", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionFloorData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionFloorData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionFloorData = Response.Data;
		OnConstructionFloorDataReady.ExecuteIfBound();
	}
}

void FArmyUser::ReqConstructionModifyWallData()
{
	FArmyAsyncHttpRequestSet RequsetSet;
	for (int i = 1; i <= 6; ++i)
	{
		FString Url = "/api/quota/construction_object/demolition_wall?wall_material=" + FString::FormatAsNumber(i);
		IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionModifyWallData));
		RequsetSet.AddRequest(Request);
	}
	RequsetSet.ProcessRequest();
}

void FArmyUser::ResConstructionModifyWallData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		int TempIndex = 0;
		int TempCount = -1;
		if (Response.FullUrl.FindLastChar('=', TempIndex))
		{
			FString Temp = Response.FullUrl.Right(Response.FullUrl.Len() - TempIndex - 1);
			TempCount = FCString::Atoi(*Temp);
			ConstructionModifyWallData.Add(TempCount, Response.Data);
		}		
	}
}

void FArmyUser::ReqConstructionAddWallData()
{
	FArmyAsyncHttpRequestSet RequsetSet;
	for (int i = 1; i <= 6; ++i)
	{
		FString Url = "/api/quota/construction_object/new_wall?wall_material=" + FString::FormatAsNumber(i);
		IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionAddWallData));
		RequsetSet.AddRequest(Request);
	}
	RequsetSet.ProcessRequest();
}

void FArmyUser::ResConstructionAddWallData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{		
		int TempIndex = 0;
		int TempCount = -1;
		if (Response.FullUrl.FindLastChar('=', TempIndex))
		{
			TempCount = FCString::Atoi(*Response.FullUrl.Right(Response.FullUrl.Len() - TempIndex - 1));
			ConstructionAddWallData.Add(TempCount, Response.Data);
		}
	}
}

void FArmyUser::ReqConstructionAddPassData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/horizontal_build", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionAddPassData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionAddPassData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionAddPassData = Response.Data;		
	}
}

void FArmyUser::ReqConstructionModifyPassData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/horizontal_dismantling", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionModifyPassData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionModifyPassData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionModifyPassData = Response.Data;		
	}
}

void FArmyUser::ReqConstructionFillPassData()
{	
	FArmyAsyncHttpRequestSet RequsetSet;
	for (int i = 1; i <= 6; ++i)
	{
		FString Url = "/api/quota/construction_object/fill_the_door?wall_material=" + FString::FormatAsNumber(i);
		IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionFillPassData));
		RequsetSet.AddRequest(Request);
	}
	RequsetSet.ProcessRequest();
}

void FArmyUser::ResConstructionFillPassData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		int TempIndex = 0;
		int TempCount = -1;
		if (Response.FullUrl.FindLastChar('=', TempIndex))
		{
			TempCount = FCString::Atoi(*Response.FullUrl.Right(Response.FullUrl.Len() - TempIndex - 1));
			ConstructionFillPassData.Add(TempCount, Response.Data);
		}
	}
}

void FArmyUser::ReqConstructionNewPassData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/new_door_opening", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionNewPassData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionNewPassData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionNewPassData = Response.Data;
		OnConstructionNewPassDataReady.ExecuteIfBound();
	}
}

void FArmyUser::ReqConstructionNewPassByMatData()
{
	FArmyAsyncHttpRequestSet RequsetSet;
	for (int i = 1; i <= 6; ++i)
	{
		FString Url = "/api/quota/construction_object/new_door_opening?wall_material=" + FString::FormatAsNumber(i);
		IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionNewPassByMatData));
		RequsetSet.AddRequest(Request);
	}
	RequsetSet.ProcessRequest();
}

void FArmyUser::ResConstructionNewPassByMatData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		int TempIndex = 0;
		int TempCount = -1;
		if (Response.FullUrl.FindLastChar('=', TempIndex))
		{
			TempCount = FCString::Atoi(*Response.FullUrl.Right(Response.FullUrl.Len() - TempIndex - 1));
			ConstructionNewPassByMatData.Add(TempCount, Response.Data);
		}
	}
}

void FArmyUser::ReqConstructionOriginPassData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/door_opening", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionOriginPassData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionOriginPassData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionOriginPassData = Response.Data;
		OnConstructionOriginPassDataReady.ExecuteIfBound();
	}
}

void FArmyUser::ReqConstructionPackPipeData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/new_packet_riser", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionPackPipeData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionPackPipeData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionPackPipeData = Response.Data;
		OnConstructionPackPipeDataReady.ExecuteIfBound();
	}
}

void FArmyUser::ReqConstructionVerticalModifyData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/vertical_demolition", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionVerticalModifyData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionVerticalModifyData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionVerticalModify = Response.Data;		
	}
}

void FArmyUser::ReqConstructionVerticalAddData()
{
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/quota/construction_object/vertical_build", FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyUser::ResConstructionVerticalAddData));
	Request->ProcessRequest();
}

void FArmyUser::ResConstructionVerticalAddData(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful && Response.Data.IsValid())
	{
		ConstructionVerticalAdd = Response.Data;
	}
}

void FArmyUser::Clear()
{
    Username = "";
    Token = "";
    FArmyHttpModule::Get().Reset();
    CompanyName = "";
    Profession = "";
    bRememberMe = false;
    bAutoLogin = false;
    bLoggedIn = false;

	OnSpaceTypeListReady.Clear();
}

const FString FArmyUser::GetApplicationVersion()
{
    if (ApplicationVersion.IsEmpty())
    {
        // 读取版本号
        FConfigFile LaunchConfig;
        FString InPath = FPaths::ProjectContentDir() + TEXT("External");
        FConfigCacheIni::LoadExternalIniFile(LaunchConfig, TEXT("Launch"), *FPaths::EngineDir(), *InPath, false);
        LaunchConfig.GetString(TEXT("Launch"), TEXT("Version"), ApplicationVersion);
    }
    
    return ApplicationVersion;
}


int32 FArmyUser::GetApplicationVersionInt()
{
	return FArmyCommonTools::ConvertVersionStrToInt(GetApplicationVersion());
}

bool FArmyUser::CheckHasNewUpdates()
{
    if (GConfig)
    {
        FString OldVersion;
        if (GConfig->GetString(TEXT("UserData"), TEXT("OldVersion"), OldVersion, GGameIni))
        {
            return !OldVersion.Equals(GetApplicationVersion());
        }
    }

    return true;
}

void FArmyUser::Init()
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

void FArmyUser::ClearAll()
{
	Username.Empty();
    FArmyHttpModule::Get().Reset();
	CompanyName.Empty();
	Profession.Empty();
	CurProjectData.Reset();
	CurPlanData.Reset();
	CurHomeData.Reset();
	CurPlanVersion = 0;
	CurHomeVersion = 0;
}

TSharedPtr<FArmyUser> FArmyUser::ArmyUserMgr = nullptr;

ARMY_USER_API FArmyUser& FArmyUser::Get()
{
	if (!ArmyUserMgr.IsValid())
	{
		ArmyUserMgr = MakeShareable(new FArmyUser());
	}
	return *ArmyUserMgr;
}