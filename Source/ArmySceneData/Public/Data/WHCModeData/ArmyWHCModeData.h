#pragma once
#include "CoreMinimal.h"
#include "Serialization/JsonWriter.h"
#include "Json.h"
#include "Policies/CondensedJsonPrintPolicy.h"

enum class ECabinetClassify
{
    EClassify_Other = 0, // 其它类型柜子分类
    EClassify_Wardrobe = 1, // 衣柜分类
    EClassify_Cabinet = 2, // 橱柜分类
    EClassify_BathroomCab = 4 // 浴室柜分类
};

enum class ECabinetType
{
    EType_Unknown = -1,
    EType_CoverDoorWardrobe = 1, // 掩门衣柜
    EType_SlidingDoorWardrobe = 2, // 移门衣柜
    EType_TopCab = 3, // 衣柜顶柜
    EType_HangCab = 4, // 橱柜吊柜
    EType_OnGroundCab = 5, // 橱柜地柜
    EType_TallCab = 6, // 橱柜高柜
    EType_OnCabCab = 7, // 橱柜台上柜
    EType_Other = 8, // 其它柜子
    EType_BathroomOnGroundCab = 9, // 浴室地柜
    EType_BathroomHangCab = 10, // 浴室吊柜
    EType_BathroomStorageCab = 11 // 浴室收纳柜
};

enum class ECabinetLabelType
{
    ELabelType_Basin = 1,
    ELabelType_Cooker,
    ELabelType_BathroomBasin,
    ELabelType_Ventilator
};

enum class ECabinetResourceState
{
    CabRc_None,
    CabRc_FetchingJson,
    CabRc_FetchingRc,
    CabRc_Complete
};

enum class EAdjustmentType : int8
{
    AdjType_OnlyAdjustment = 1,
    AdjType_AdjAndTopBlocker,
    AdjType_AdjAndBottomBlocker,
    AdjType_AdjWithBoth
};

struct ARMYSCENEDATA_API FDimInfo {
    float Current, Min, Max;
    TArray<float> Intervals;

    FDimInfo() { Current = Min = Max = 0.0f; }
    void Serialize(const FString & InObjectName, TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialize(const TSharedPtr<class FJsonObject>& JsonObject);
};

// 全局参数信息
struct FWHCGlobalParams {
    // 台面突出量，厘米单位
    float PlatformBulge;
    // 调整板横装范围，厘米单位
    FVector2D AdjustmentVerticalRange;
    // 调整板侧装范围，厘米单位
    FVector2D AdjustmentLateralRange;
    // 调整板侧装时默认深度，厘米单位
    float AdjustmentLateralDepth;
    // 调整板侧装时封板默认深度，厘米单位
    float AdjustmentLateralBlockerDepth;

    float ToeFBShrink;
    float ToeLRShrink;

    struct FWHCCabGlobalParams
    {
        float AboveGround;
        EAdjustmentType AdjType;
    };
    TMap<ECabinetType, FWHCCabGlobalParams> CabGlobalParamsMap;

    FWHCGlobalParams()
    : PlatformBulge(2.0f)
    , AdjustmentVerticalRange(FVector2D(2.4f, 10.0f))
    , AdjustmentLateralRange(FVector2D(1.8f, 2.4f))
    , AdjustmentLateralDepth(10.0f)
    , AdjustmentLateralBlockerDepth(2.0f)
    , ToeFBShrink(5.0f)
    , ToeLRShrink(3.0f)
    {
        FWHCCabGlobalParams OnGroundCab;
        OnGroundCab.AboveGround = 10.0f;
        OnGroundCab.AdjType = EAdjustmentType::AdjType_OnlyAdjustment;
        CabGlobalParamsMap.Emplace(ECabinetType::EType_OnGroundCab, OnGroundCab);

        FWHCCabGlobalParams TallCab;
        TallCab.AboveGround = 10.0f;
        TallCab.AdjType = EAdjustmentType::AdjType_OnlyAdjustment;
        CabGlobalParamsMap.Emplace(ECabinetType::EType_TallCab, TallCab);

        FWHCCabGlobalParams HangCab;
        HangCab.AboveGround = 148.0f;
        HangCab.AdjType = EAdjustmentType::AdjType_AdjWithBoth;
        CabGlobalParamsMap.Emplace(ECabinetType::EType_HangCab, HangCab);

        FWHCCabGlobalParams OnCabCab;
        OnCabCab.AboveGround = 84.5f;
        OnCabCab.AdjType = EAdjustmentType::AdjType_AdjAndTopBlocker;
        CabGlobalParamsMap.Emplace(ECabinetType::EType_OnCabCab, OnCabCab);
    }
};

// 柜子自身构件信息
struct FCabinetComponent
{
    int32 Id;
	int32 Type;

	FString TopResourceUrl;
	FString TopResourceMd5;

	FString FrontResourceUrl;
    FString FrontResourceMd5;

	FString SideResourceUrl;
	FString SideResourceMd5;
	//InType 1 Top,2 Front,3 Side
    ARMYSCENEDATA_API FString GetCachePath(int32 InType = 1) const;
};

// 柜子附属件构件信息
struct FCabinetAccessoryComponent
{
    FCabinetComponent Self;
    TArray<FCabinetComponent> Subs;
};

// 柜子完整构件信息
struct FCabinetWholeComponent
{
    TSharedPtr<FCabinetComponent> Cabinet;
	TArray<TSharedPtr<FCabinetComponent>> Accessorys;
	TArray<TSharedPtr<FCabinetComponent>> DoorSheets;
    //TSharedPtr<FCabinetAccessoryComponent> Accessory;
};

struct ARMYSCENEDATA_API FCabinetAccInfo
{
    int32 Id = 0;
    float AboveGround = 0.0f;
    uint8 Type = 0;
    bool bBoundToSocket = false;
    // 如果bBoundToSocket为true，会忽略RelativeLocation这个数据
    FVector RelativeLocation;
    FString Name;
    FString Code;
    FString PakUrl;
    FString PakMd5;
    FString OptimizedParam;
    FString ThumbnailUrl;
    FString BrandName;
    FString Version;

    FDimInfo WidthInfo;
    FDimInfo DepthInfo;
    FDimInfo HeightInfo;

	TArray<TSharedPtr<FCabinetComponent>> ComponentArray;

    FCabinetAccInfo& operator = (const FCabinetAccInfo &InOther)
    {
        Id = InOther.Id;
        AboveGround = InOther.AboveGround;
        Type = InOther.Type;
        bBoundToSocket = InOther.bBoundToSocket;
        RelativeLocation = InOther.RelativeLocation;
        Name = InOther.Name;
        Code = InOther.Code;
        PakUrl = InOther.PakUrl;
        PakMd5 = InOther.PakMd5;
        OptimizedParam = InOther.OptimizedParam;
        ThumbnailUrl = InOther.ThumbnailUrl;
        BrandName = InOther.BrandName;
        Version = InOther.Version;
        WidthInfo = InOther.WidthInfo;
        DepthInfo = InOther.DepthInfo;
        HeightInfo = InOther.HeightInfo;
        return *this;
    }

    FCabinetAccInfo() { RelativeLocation.Set(0.0f, 0.0f, 0.0f); }
    void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialize(const TSharedPtr<class FJsonObject>& JsonObject);
    FString GetCachePath() const;
};

struct FStandaloneCabAccInfo
{
    ECabinetResourceState CabRcState = ECabinetResourceState::CabRc_None;
    TSharedPtr<class FAccessShapeForBim> AccShape;
};

// 柜子附属件信息
struct ARMYSCENEDATA_API FCabinetWholeAccInfo
{
    ECabinetResourceState CabRcState = ECabinetResourceState::CabRc_None;

    TSharedPtr<FCabinetAccInfo> Self;
    TArray<TSharedPtr<FCabinetAccInfo>> SubAccInfos;

    void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialize(const TSharedPtr<class FJsonObject>& JsonObject);
};

struct FCabinetAccInfoFunctor
{
    int32 Id;

    FCabinetAccInfoFunctor(int32 InId) : Id(InId) {}
    bool operator()(const TSharedPtr<FCabinetWholeAccInfo> &InData)
    {
        return Id == InData->Self->Id;
    }
};

struct FPlatformInfo;
// 柜子列表信息
struct ARMYSCENEDATA_API FCabinetInfo
{
    int32 Id;
    int32 Type;
    float AboveGround;
    ECabinetResourceState CabRcState;
    FString Name;
    FString ThumbnailUrl;
    FString JsonUrl;
    FString JsonMd5;
    FString DownloadTaskFlag;

    FDimInfo WidthInfo;
    FDimInfo DepthInfo;
    FDimInfo HeightInfo;

    TMap<FName, FDimInfo> OtherDimInfo;

    TArray<ECabinetLabelType> Labels;

    // 柜子独立关联的台面信息
	TSharedPtr<FPlatformInfo> PlatformInfo;

    TSharedPtr<FCabinetWholeAccInfo> AccInfo;

    FCabinetInfo() : Id(0), Type(0), AboveGround(0.0f), CabRcState(ECabinetResourceState::CabRc_None) {}
    FCabinetInfo(int32 InId, int32 InType, const FString &InName, const FString &InThumbnailUrl, const FString &InJsonUrl, const FString &InJsonMd5)
    : Id(InId), Type(InType), AboveGround(0.0f), CabRcState(ECabinetResourceState::CabRc_None), Name(InName), ThumbnailUrl(InThumbnailUrl), JsonUrl(InJsonUrl), JsonMd5(InJsonMd5) 
    {
        // UpdateTime = InUpdateTime.Replace(TEXT("-"), TEXT("_"));
        // UpdateTime.ReplaceInline(TEXT(":"), TEXT("_"));
        // UpdateTime.ReplaceInline(TEXT(" "), TEXT("_"));
    }

    bool IsCabinetHasValidCache() const;
    FString GetJsonFileCachePath() const;
};

struct FCabinetInfoFunctor
{
    int32 Id;

    FCabinetInfoFunctor(int32 InId) : Id(InId) {}
    bool operator()(FCabinetInfo &InData)
    {
        return Id == InData.Id;
    }
};

struct FCabinetListInfo
{
    int32 TotalDataPage = 0;
    TArray<int32> CabinetIds;

    void Reset()
    {
        TotalDataPage = 0;
        CabinetIds.Empty();
    }
};

// 线条类结构信息
struct ARMYSCENEDATA_API FTableInfo
{
	int32 Id;
	FString Name;
	FString ThumbnailUrl;
	FString cadUrl;
	int32 mtlId = 0;
	ECabinetResourceState RcState = ECabinetResourceState::CabRc_None;
    float CurrentHeight = 100.0f;
    TArray<float> AvailableHeights;

    void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialize(const TSharedPtr<class FJsonObject>& JsonObject);
	FString GetCachedPath() const;
};

// 材质信息
struct ARMYSCENEDATA_API FMtlInfo
{
    ECabinetResourceState RcState = ECabinetResourceState::CabRc_None;
    int32 Id = 0;
    FString MtlName;
    FString MtlThumbnailUrl;
    FString MtlUrl;
    FString MtlMd5;
    FString MtlParam;
    class UMaterialInterface *Mtl = nullptr;

    void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialize(const TSharedPtr<class FJsonObject>& JsonObject);
    FString GetCachedPath() const;
};

// 台面信息
struct ARMYSCENEDATA_API FPlatformInfo 
{
    int32 Id;
    TSharedPtr<FMtlInfo> PlatformMtl;
    TArray<TSharedPtr<FTableInfo>> FrontSections;
    TArray<TSharedPtr<FTableInfo>> BackSections;

    void Serialize(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
    void Deserialize(const TSharedPtr<class FJsonObject>& JsonObject);
};

// 门组信息
struct ARMYSCENEDATA_API FCoverDoorTypeInfo
{
    ECabinetResourceState RcState = ECabinetResourceState::CabRc_None;
    int32 Id;
    TArray<int32> SKUOpenDoorDirections;
    FString Name;
    FString ThumbnailUrl;
    FString DownloadTaskFlag;

	TArray<TSharedPtr<FCabinetComponent>> ComponentArray;

    TSharedPtr<class FSCTShape> DoorGroupShape;

    bool IsDoorTypeHasValidCache() const;
    FString GetJsonFileCachePath() const;
};

struct FWHCModeGlobalData
{
    // 当前柜子类型
    ARMYSCENEDATA_API static int32 CurrentWHCClassify;
    
    ARMYSCENEDATA_API static FWHCGlobalParams GlobalParams;

    // 保存柜子信息的数组，由柜子顶层类型索引(橱柜、衣柜、系统柜等)
    using TCabinetInfoArr = TArray<FCabinetInfo>;
    ARMYSCENEDATA_API static TMap<int32, TCabinetInfoArr> CabinetInfoArrCollection;

    // 保存每种柜子信息的数组，由具体的柜子类型索引(如橱柜吊柜、橱柜地柜等)
    ARMYSCENEDATA_API static FCabinetListInfo CabinetListInfo;

    // 保存已解析的柜子模板，由每个柜子的Id索引
    using TCabinetTemplateMap = TMap<int32, TSharedPtr<class FSCTShape>>;
    ARMYSCENEDATA_API static TCabinetTemplateMap CabinetTemplateMap;

    // 保存已解析的柜子的构件数据，由每个柜子的Id索引
    using TCabinetComponentMap = TMap<int32, TSharedPtr<FCabinetWholeComponent>>;
    ARMYSCENEDATA_API static TCabinetComponentMap CabinetComponentMap;

    // 保存已获取的五金数据
    using TStandaloneAccessoryMap = TMap<int32, TSharedPtr<FStandaloneCabAccInfo>>;
    ARMYSCENEDATA_API static TStandaloneAccessoryMap StandaloneAccessoryMap;

    // 保存已获取的掩门门型，有掩门门型Id索引
    using TCoverDoorTypeMap = TMap<int32, TSharedPtr<FCoverDoorTypeInfo>>;
    ARMYSCENEDATA_API static TCoverDoorTypeMap CoverDoorTypeMap;

    ARMYSCENEDATA_API static TCoverDoorTypeMap SlidingDoorTypeMap;

    // 保存已获取的抽面门型
    ARMYSCENEDATA_API static TCoverDoorTypeMap DrawerTypeMap;

    using TMtlMap = TMap<int32, TSharedPtr<FMtlInfo>>;
    using TTableMap = TMap<int32, TSharedPtr<FTableInfo>>;
    using TPlatformMap = TMap<int32, TSharedPtr<FPlatformInfo>>;

    // 保存柜体材质，由柜体材质Id索引
    ARMYSCENEDATA_API static TMtlMap CabinetMtlMap;

    // 保存已获取的台面材质，由台面的Id索引
    ARMYSCENEDATA_API static TPlatformMap PlatformMap;

    // 踢脚板
    ARMYSCENEDATA_API static TTableMap ToeSections;
    // 保存已获取的踢脚板材质，由踢脚板的Id索引
    ARMYSCENEDATA_API static TMtlMap ToeMtlMap;

	// 上线条
	ARMYSCENEDATA_API static TTableMap TopSections;
    // 保存已获取的上线条材质，由上线条的Id索引
	ARMYSCENEDATA_API static TMtlMap TopMtlMap;

    // 保存附属件信息
    using TCabinetAccMap = TMap<int32, TSharedPtr<FCabinetWholeAccInfo>>;
    ARMYSCENEDATA_API static TCabinetAccMap CabinetAccMap;

    // TODO: 临时解决方法，保存场景中选中信息
    ARMYSCENEDATA_API static int32 SelectionState;
};

#define WHC_ADD_DOWNLOAD_TASK(Files, Id, CachePath, PakUrl, PakMd5) \
    if (!PakUrl.IsEmpty()) \
    { \
        Files.Emplace( \
            Id, \
            CachePath, \
            PakUrl, \
            PakMd5, \
            PakMd5.IsEmpty() ? true : false); \
    }