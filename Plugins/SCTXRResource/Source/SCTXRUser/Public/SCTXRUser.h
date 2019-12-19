#pragma once

#include "SCTXRTypes.h"
#include "SCTXRCommonTypes.h"
#include "CoreMinimal.h"
#include "Http.h"

/** 开发服务器 */
#define DEBUG 0

/** 测试服务器 */
#define STAGE 1

/** 当前服务器模式 */
#define SERVER_MODE DEBUG

/** 服务器地址 */
#if SERVER_MODE == DEBUG
    const static FString GServerAddress = "http://101.201.78.170:8080";
    const static FString GBackendUrl = "http://47.93.164.20";
#elif SERVER_MODE == STAGE
    const static FString GServerAddress = "http://api.v7.stage.dabanjia.com";
    const static FString GBackendUrl = "http://magina.dabanjia.com";
#endif // SERVER_MODE

const static FString GOSSServerAddress = "http://test.ali.res.dabanjia.com";
const static FString GBucketName = "/dbj-test/";
const static FString GAccessId = "LTAIVNPppai5XhAu";
const static FString GAccessKey = "iGkaPXhWYlOeELuRVepYp4qgEXKSmE";

struct AppendInfo
{
	FString CompanyName;
	FString DesignerName;
	FString HomeAddress;
	FString UserName;
};
class FSCTXRUser
{
public:

public:
	FSCTXRUser();
    ~FSCTXRUser() {}

	/** 单例获取 */
	SCTXRUSER_API static FSCTXRUser& Get();

    /** 初始化用户数据，加载配置文件 */
    SCTXRUSER_API void Init();

    SCTXRUSER_API TSharedPtr<IHttpRequest> HttpRequest(const FString& Url, FHttpRequestCompleteDelegate CompletedDelegate, const FString& Content = "");

	SCTXRUSER_API void RequestLogin(const FString& InUsername, const FString& InPassword);
	SCTXRUSER_API void CancelLogin();
    void Callback_RequestLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    SCTXRUSER_API TSharedPtr<IHttpRequest> RequestUpload(const FString& FileName, FHttpRequestCompleteDelegate CompletedDelegate,const TArray<uint8>& Content);

    void RequestOrientationList();
    void Callback_RequestOrientationList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void RequestHomeTypeList();
    void Callback_RequestHomeTypeList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    void RequestSpaceTypeList();
    void Callback_RequestSpaceTypeList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    /** 用户退出时清空缓存数据 */
    SCTXRUSER_API void Clear();

    FORCEINLINE FString GetCompanyName() const { return CompanyName; }
	FORCEINLINE FString GetUserRealName() const { return LoginUserName;}
    FORCEINLINE FString GetUsername() const { return Username; }
    FORCEINLINE FString GetProfession() const { return Profession; }
    FORCEINLINE FString GetToken() const { return Token; }
    FORCEINLINE FString GetPassword() const { return Password; }
	/** 获取相关ID */
	int32 GetCurProjectID() { return CurProjectData.IsValid() ? CurProjectData->ID : 0; }
	int32 GetCurHomePlanID() { return CurHomePlanData.IsValid() ? CurHomePlanData->ID : 0; }
	int32 GetCurLevelHomeID() { return CurLevelHomeData.IsValid() ? CurLevelHomeData->ID : 0; }
	int32 GetCurEditHomeID() { return CurEditHomeData.IsValid() ? CurEditHomeData->ID : 0; }
private:
    /** 计算签名 */
    const FString GenerateAuthorization(const FString& FileName, const FString& ContentType, const FString& Date);

    /** 通过文件名后缀返回Content-Type */
    const FString GetContentType(const FString& FileSuffix);

	/** 重置所有数据 */
	void ClearAll();

public:
    /** 房屋朝向列表 */
    FSCTXRComboBoxArray OrientationList;

    /** 房屋类型列表 */
    FSCTXRComboBoxArray HomeTypeList;

    /** 空间类型列表 */
    FSCTXRComboBoxArray SpaceTypeList;

    /** 登录成功回调 */
	FSimpleDelegate OnLoginSuccess;

	/** 登录失败回调 */
	FSimpleDelegate OnLoginFailed;

	/** 登录请求 */
	TSharedPtr<IHttpRequest> LoginRequest;

    /** 是否在本地记录本次登录信息 */
    bool bRememberMe;

    /** 是否自动登录 */
    bool bAutoLogin;

	/** 当前载入的项目数据 */
	TSharedPtr<FSCTContentItemSpace::FContentItem> CurProjectData;

	/** 当前场景编辑器载入的方案数据 */
	TSharedPtr<FSCTContentItemSpace::FContentItem> CurHomePlanData;

	/** 当前场景编辑中放置的户型数据 */
	TSharedPtr<FSCTContentItemSpace::FContentItem> CurLevelHomeData;

	/** 当前户型编辑器编辑的户型数据 */
	TSharedPtr<FSCTContentItemSpace::FContentItem> CurEditHomeData;

	/** 当前户型编辑器正在编辑的户型是否关联了方案，方案ID大于0的时候认为关联，在保存户型的时候会把方案ID写入Json */
	int32 CurRelevantHomePlanID;

	/**	当前扩展信息，目前在施工图图框获取信息用*/
	AppendInfo CurrentAppendInfo;
private:
    /** 用户名 */
    FString Username;

    /** 密码 */
    FString Password;

    /** 登录id */
    FString LoginId;

    /** 用户token */
    FString Token;

    /** 公司名称 */
    FString CompanyName;

	/**	用户真实名称*/
	FString LoginUserName;

    /** 职位 */
    FString Profession;

private:
	/** 单例 */
	static TSharedPtr<FSCTXRUser> XRUserMgr;
};