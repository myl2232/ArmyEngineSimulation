#pragma once

#include "ArmyTypes.h"
#include "ArmyCommonTypes.h"
#include "CoreMinimal.h"
#include "Http.h"

/** 客户端版本号 */
static FString GArmyVersion;

struct AppendInfo
{
	FString ContractNo;
	FString DesignerName;
	FString HomeAddress;
	FString CustomerName;
};

class ARMY_USER_API FArmyUser
{
public:
    FArmyUser();
    ~FArmyUser() {}

	/** 单例获取 */
	static FArmyUser& Get();

    /** 初始化用户数据，加载配置文件 */
    void Init();

	void RequestLogin(const FString& InUsername, const FString& InPassword);
	void CancelLogin();
    void ResponseLogin(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessfull);

    /** @欧石楠 重新登录 */
    void Relogin();
    void ReqRelogin();

    /** @欧石楠 请求发版公告 */
    void RequestUpgradeNotes();

    void RequestSpaceTypeList();
    void ResponseSpaceTypeList(struct FArmyHttpResponse Response);

	/**@欧石楠 请求施工项相关数据*/

	/** 墙*/
	void ReqConstructionWallData();
	void ResConstructionWallData(FArmyHttpResponse Response);
	/** 顶*/
	void ReqConstructionTopData();
	void ResConstructionTopData(FArmyHttpResponse Response);
	/** 地*/
	void ReqConstructionFloorData();
	void ResConstructionFloorData(FArmyHttpResponse Response);
	/** 拆墙*/
	void ReqConstructionModifyWallData();
	void ResConstructionModifyWallData(FArmyHttpResponse Response);
	/** 新建墙*/
	void ReqConstructionAddWallData();
	void ResConstructionAddWallData(FArmyHttpResponse Response);
	/** 补门洞*/
	void ReqConstructionAddPassData();
	void ResConstructionAddPassData(FArmyHttpResponse Response);
	/** 拆门洞*/
	void ReqConstructionModifyPassData();
	void ResConstructionModifyPassData(FArmyHttpResponse Response);
	/** 填门洞*/
	void ReqConstructionFillPassData();
	void ResConstructionFillPassData(FArmyHttpResponse Response);
	/** 开垭口*/
	void ReqConstructionNewPassData();
	void ResConstructionNewPassData(FArmyHttpResponse Response);
	/** 开垭口分材质*/
	void ReqConstructionNewPassByMatData();
	void ResConstructionNewPassByMatData(FArmyHttpResponse Response);	
	//原始门洞数据
	void ReqConstructionOriginPassData();
	void ResConstructionOriginPassData(FArmyHttpResponse Response);
	//包立管数据
	void ReqConstructionPackPipeData();
	void ResConstructionPackPipeData(FArmyHttpResponse Response);
	/** 垂直拆*/
	void ReqConstructionVerticalModifyData();
	void ResConstructionVerticalModifyData(FArmyHttpResponse Response);
	/** 垂直补*/
	void ReqConstructionVerticalAddData();
	void ResConstructionVerticalAddData(FArmyHttpResponse Response);


    /** 用户退出时清空缓存数据 */
    void Clear();

    FORCEINLINE FString GetCompanyName() const { return CompanyName; }
	FORCEINLINE FString GetUserRealName() const { return LoginUserName;}
    FORCEINLINE FString GetUsername() const { return Username; }
    FORCEINLINE FString GetProfession() const { return Profession; }
    FORCEINLINE FString GetPassword() const { return Password; }
    FORCEINLINE FString GetAppIconUrl() const { return AppLogoUrl; }

	/** 获取相关数据 */
    int32 GetCurProjectID() { return CurProjectData.IsValid() ? CurProjectData->ID : 0; }
    int32 GetCurPlanID() { return CurPlanData.IsValid() ? CurPlanData->ID : 0; }
    int32 GetCurHomeID() { return CurHomeData.IsValid() ? CurHomeData->ID : 0; }
    int32 GetMealID() const { return MealID; }
    int32 GetCurrentMealID() const { return CurrentMealID; }

    void SetMealID(int32 InMealID) { MealID = CurrentMealID = InMealID; }
    void SetCurrentMealID(int32 InCurrentMealID) { CurrentMealID = InCurrentMealID; }

    /** 获取程序版本号 */
    const FString GetApplicationVersion();

	/** Int版本的获取程序版本号，可用于比较版本大小 */
	int32 GetApplicationVersionInt();

    /** 检查是否有新的版本更新 */
    bool CheckHasNewUpdates();

    /** @欧石楠 是否已登录 */
    bool IsLoggedIn() { return bLoggedIn; }

private:
	/** 重置所有数据 */
	void ClearAll();

public:
    /** 空间类型列表 */
    TSharedPtr<FArmyComboBoxArray> SpaceTypeList;

	/**@欧石楠 存储墙顶地施工项的json对象*/
	/** 墙*/
	TSharedPtr<class FJsonObject> ConstructionWallData;
	/** 顶*/
	TSharedPtr<class FJsonObject> ConstructionTopData;
	/** 地*/
	TSharedPtr<class FJsonObject> ConstructionFloorData;
	/** 拆墙*/
	TMap<int/*材质类型*/, TSharedPtr<class FJsonObject>> ConstructionModifyWallData;
	/** 新建墙*/
	TMap<int/*材质类型*/, TSharedPtr<class FJsonObject>> ConstructionAddWallData;
	/** 补门洞*/
	TSharedPtr<class FJsonObject> ConstructionAddPassData;
	/** 拆门洞*/
	TSharedPtr<class FJsonObject> ConstructionModifyPassData;
	/** 填门洞*/
	TMap<int/*材质类型*/, TSharedPtr<class FJsonObject>> ConstructionFillPassData;
	/** 根据材质开垭口数据*/
	TMap<int/*材质类型*/, TSharedPtr<class FJsonObject>> ConstructionNewPassByMatData;
	/** 开垭口*/
	TSharedPtr<class FJsonObject> ConstructionNewPassData;	
	/** 原始门洞*/
	TSharedPtr<class FJsonObject> ConstructionOriginPassData;
	/** 包立管*/
	TSharedPtr<class FJsonObject> ConstructionPackPipeData;
	/** 垂直拆*/
	TSharedPtr<class FJsonObject> ConstructionVerticalModify;
	/** 垂直补*/
	TSharedPtr<class FJsonObject> ConstructionVerticalAdd;

    /** 登录成功回调 */
	FSimpleDelegate OnLoginSuccess;

	/** 登录失败回调 */
    FInt32StringDelegate OnLoginFailed;

	/** 登录请求 */
	TSharedPtr<IHttpRequest> LoginRequest;

    /** 是否在本地记录本次登录信息 */
    bool bRememberMe;

    /** 是否自动登录 */
    bool bAutoLogin;

	/** 当前载入的项目数据 */
	TSharedPtr<FContentItemSpace::FContentItem> CurProjectData;

	/** 当前场景编辑器载入的方案数据 */
	TSharedPtr<FContentItemSpace::FContentItem> CurPlanData;

	/** 当前场景编辑中放置的户型数据 */
	TSharedPtr<FContentItemSpace::FContentItem> CurHomeData;

	/** @欧石楠 从方案Json文件中读取的版本号 */
	int32 CurPlanVersion = 0;

	/** @欧石楠 从户型Json文件中读取的版本号 */
	int32 CurHomeVersion = 0;

	/**	当前扩展信息，目前在施工图图框获取信息用*/
	AppendInfo CurrentAppendInfo;

    /** 空间类型列表加载完毕 */
	FSimpleMulticastDelegate OnSpaceTypeListReady;

	/** 空间唯一标识id */
	int32 RoomUniqueId;

	/** @欧石楠 请求施工项墙顶地数据回调 */
	FSimpleDelegate OnConstructionWallDataReady;

	FSimpleDelegate OnConstructionTopDataReady;

	FSimpleDelegate OnConstructionFloorDataReady;

	FSimpleDelegate OnConstructionModifyWallDataReady;

	FSimpleDelegate OnConstructionAddWallDataReady;		

	FSimpleDelegate OnConstructionNewPassDataReady;	

	FSimpleDelegate OnConstructionOriginPassDataReady;

	FSimpleDelegate OnConstructionPackPipeDataReady;

private:
    /** 用户名 */
    FString Username;

    /** 密码 */
    FString Password;

    FString Token;

    /** 公司名称 */
    FString CompanyName;

    /** 程序logo */
    FString AppLogoUrl;

	/**	用户真实名称*/
	FString LoginUserName;

    /** 职位 */
    FString Profession;

	/** 单例 */
	static TSharedPtr<FArmyUser> ArmyUserMgr;

    /** 当前套餐ID */
    int32 MealID;

    /** 临时选中的套餐ID */
    int32 CurrentMealID = -1;

    /** 程序版本 */
    FString ApplicationVersion;

    /** 是否有新的版本更新 */
    bool bHasNewUpdates;

    /** @欧石楠 是否已登录 */
    bool bLoggedIn = false;

    /** @欧石楠 是否显示重新登录提示 */
    bool bShowReloginMessage = true;
};