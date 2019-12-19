#pragma once

#include "Guid.h"
#include "Input/Events.h"

#define MAKE_TEXT(Text) FText::FromString(TEXT(Text))
#define FORMAT_TEXT(Format, ...) FText::FromString(FString::Printf(TEXT(Format), __VA_ARGS__))

enum EModelType
{
	E_NoneModel = -1,
	E_HomeModel = 0,	    // 户型模式
	E_ModifyModel,			// 拆改模式
	E_LayoutModel,			// 布局模式
	E_HardModel,		    // 硬装模式
	E_SoftModel,		    // 软装模式
	E_HydropowerModel,	    // 水电模式
	E_ConstructionModel,    // 图纸模式
	E_WHCModel,				// 木作模式
    E_OtherModel,           // 其它模式，作为撤销重做用
};

/**@欧石楠视图模式*/
enum EArmyViewMode
{
	EArmyView_None,
	//透视图
	EArmyView_FPS,
	//顶视图
	EArmyView_TOP,
	//吊顶视图
	EArmyView_CEILING,
	//环绕图
	EArmyView_TPS,
};
//施工图图层分类
enum ObjectClassType
{
	Class_Begin,
	Class_BaseWall = 1,//  基础墙
	Class_AddWall = 2,//  新建墙
	Class_BreakWall = 3,//  拆除墙
	Class_Door = 4,
	Class_Pass = 5,
	Class_Window = 6,
	Class_Furniture = 7,
	Class_Switch = 8,
	Class_Socket = 9,
	Class_ElectricBoxH = 10,
	Class_ElectricBoxL = 11,
	Class_Light = 12,
	Class_AirConditionerPoint = 13,
	//Class_ElectricPoint = 14,
	Class_WaterRoutePoint = 15,
	//Class_WaterPipe = 16,
	Class_ElectricWire = 17,
	Class_AnnotationInSide = 18,
	Class_AnnotationOutSide = 19,
	Class_AnnotationDoor = 20,
	Class_AnnotationWindow = 21,
	Class_AnnotationElectricBoxH = 22,
	Class_AnnotationElectricBoxL = 23,
	Class_AnnotationAirConditionerPoint = 24,
	Class_AnnotationFurniture = 25,
	Class_AnnotationSwitch = 26,
	Class_AnnotationSocket = 27,
	Class_AnnotationLamp = 28,
	Class_SpaceName = 29,
	Class_Floorlayout = 30,
	//Class_Ceilinglayout = 31,
	Class_Walllayout = 32,
	Class_ConstructionFrame = 33,
	Class_Flag34 = 34,
	Class_SpaceArea = 35,
	Class_SpacePerimeter = 36,
	Class_SpaceHeight = 37,
	Class_AddWallPost = 38,
	Class_DeleteWallPost = 39,
	Class_AnnotationDeleteWall = 40,
	Class_AnnotationAddWall = 41,
	Class_LampStrip = 42,
	Class_Ceilinglayout = 43,//顶面造型

	Class_AnnotationCeilingObj = 44,//吊顶标注
	Class_Beam = 55,//  梁
	Class_Socket_H = 68,//  强电插座
	Class_Socket_L = 69,//  弱电插座
	Class_DiversityWater = 70,//  分集水器
	Class_WaterPipe = 74,//  下水主管道
	Class_Heater = 75,//  暖气
	Class_GasMeter = 76,//  燃气表
	Class_GasPipe = 77,//  燃气主管
	Class_AnnotationCurtainBox = 86,//  窗帘盒标注
	Class_AnnotationBeam = 87,//  梁标注
	Class_AnnotationElectricPoint = 92,//  电路点位标注
	Class_AnnotationSocket_H = 94,//  强电插座标注
	Class_AnnotationSocket_L = 96,//  弱电插座标注
	Class_AnnotationWaterPoint = 98,//  水路点位标注
	Class_AnnotationSewerPipe = 99,//  下水管道标注
	Class_AnnotationHeater = 100,//  暖气标注
	Class_AnnotationGas = 102, // 燃气点位标注
	Class_SCTCabinet = 104,//木作定制柜
	Class_SCTHardware = 105,//木作定制五金/电器

	Class_BaseGroundHeight = 106,//原始地面标高
	Class_PostGroundHeight = 107,//完成地面标高

	Class_HotWaterPoint = 110,//热水点位
	Class_HotAndColdWaterPoint = 111,//冷热水点位
	Class_ColdWaterPoint = 112,//冷水点位
	Class_FloordRainPoint = 113,//地漏下水
	Class_ClosestoolRainPoint = 114,//马桶下水
	Class_WashBasinRainPoint = 115,//水盆下水
	Class_RecycledWaterPoint = 122,//中水点位
	Class_HomeEntryWaterPoint = 123,//进户水

	Class_AnnotationHotWaterPoint = 116,//热水点位标注
	Class_AnnotationHotAndColdWaterPoint = 117,//冷热水点位标注
	Class_AnnotationColdWaterPoint = 118,//冷水点位标注
	Class_AnnotationFloordRainPoint = 119,//地漏下水标注
	Class_AnnotationClosestoolRainPoint = 120,//马桶下水标注
	Class_AnnotationWashBasinRainPoint = 121,//水盆下水标注
	Class_AnnotationRecycledWaterPoint = 124,//中水点位标注
	Class_AnnotationHomeEntryWaterPoint = 125,//进户水标注

	Class_Other,
	Class_End
};

//施工图图框类型
enum EConstructionFrameType
{
	E_Frame_Default = 0,
	E_Frame_Cupboard = 1,// 橱柜
};

struct ArmyArgument
{
#define ARMY_ARGUMENT( ArgType, ArgName ) \
		ArgType _##ArgName; \
		ArmyArgument& ArgName( ArgType InArg ) \
		{ \
			_##ArgName = InArg; \
			return *this; \
		}

	ArmyArgument(int32 InType = -1) : _ArgUint8(0), _ArgInt32(InType), _ArgUint32(0), _ArgBoolean(false) {}
    ARMY_ARGUMENT(uint8, ArgUint8);
	ARMY_ARGUMENT(int32, ArgInt32);
	ARMY_ARGUMENT(uint32, ArgUint32);
	ARMY_ARGUMENT(bool, ArgBoolean);
	ARMY_ARGUMENT(FString, ArgString);
	ARMY_ARGUMENT(FName, ArgFName);
	ARMY_ARGUMENT(FColor, ArgColor);
	ARMY_ARGUMENT(float, ArgFloat);
	ARMY_ARGUMENT(void*, ArgVoid);
};
struct ItemInfo
{
	ItemInfo(const FGuid& InID, const FName& InName, const FGuid& InParentID):ItemID(InID),ItemName(InName),ParentID(InParentID){};
	FGuid ItemID;
	FName ItemName;
	FGuid ParentID;
};

DECLARE_DELEGATE_OneParam(SCommandOperator, ArmyArgument);
DECLARE_DELEGATE_OneParam(FJsonObjectDelegate, TSharedPtr<class FJsonObject>);
DECLARE_DELEGATE_TwoParams(SItemOperator,const ItemInfo&,int32);
DECLARE_DELEGATE_OneParam(FInputKeyDelegate, const struct FKeyEvent&);
DECLARE_DELEGATE_OneParam(FInt32Delegate, const int32);
DECLARE_DELEGATE_TwoParams(FTwoInt32Delegate, const int32, const int32);
DECLARE_DELEGATE_FourParams(FFourInt32Delegate, const int32, const int32, const int32, const int32);
DECLARE_DELEGATE_OneParam(FStringDelegate, const FString&);
DECLARE_DELEGATE_TwoParams(FTwoStringDelegate, FString, FString);
DECLARE_DELEGATE_TwoParams(FInt32StringDelegate, const int32, const FString&);
DECLARE_DELEGATE_OneParam(FFloatDelegate, const float);
DECLARE_DELEGATE_OneParam(FVectorDelegate, const FVector&);
DECLARE_DELEGATE_OneParam(FVector2DDelegate, const FVector2D&);
DECLARE_DELEGATE_OneParam(FBoolDelegate, bool);
DECLARE_DELEGATE_OneParam(FObjectDelegate, UObject*);
DECLARE_DELEGATE_OneParam(FActorDelegate, class AActor*);
DECLARE_DELEGATE_OneParam(FOnDelegateColor, const FLinearColor&);
DECLARE_DELEGATE_TwoParams(FLine2DDelegate, const FVector2D&, const FVector2D&);
DECLARE_DELEGATE_ThreeParams(FCircle2DDelegate, const FVector2D&, const FVector2D& , const FVector2D&);
DECLARE_DELEGATE_RetVal(TSharedPtr<class SWidget>, FGetWidgetDelegate);
DECLARE_DELEGATE_OneParam(FArmyObjectesDelegate,TArray<TSharedPtr< class FArmyObject>>&);
DECLARE_DELEGATE_TwoParams(FArmyObjectesTransactionDelegate,TArray<TSharedPtr< class FArmyObject>>&,bool);
DECLARE_DELEGATE_ThreeParams(FArmyObjectDelegate, TSharedPtr<class FArmyObject>, bool, bool);
DECLARE_DELEGATE_TwoParams(FArmyObjectAndUObjectDelegate, TSharedPtr<class FArmyObject>, class AActor*);

DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiTwoInt32Delegate, const int32, const int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiActorDelegate, class AActor*);
DECLARE_MULTICAST_DELEGATE(FMultiVoidDelegate);

// 欧石楠 added
DECLARE_DELEGATE_OneParam(FPointerEventDelegate, const FPointerEvent&)

/**
 * 键值对
 */
struct FArmyKeyValue
{
    int32 Key;
    FString Value;
	FArmyKeyValue() {}
    FArmyKeyValue(int32 InKey, FString InValue)
    {
        Key = InKey;
        Value = InValue;
    }

    bool InitFromString(const TCHAR * InSourceString)
    {
        Key = -1;
        Value = TEXT("");

        return FParse::Value(InSourceString, TEXT("Key="), Key) && FParse::Value(InSourceString, TEXT("Value="), Value);
    }

    FString ToString()
    {
        return FString::Printf(TEXT("Key=%d Value=%s"), Key, *Value);
    }
};

/**
 * 下拉框数据数组
 */
struct FArmyComboBoxArray
{
    TArray< TSharedPtr<FArmyKeyValue> > Array;

    void Add(TSharedPtr<FArmyKeyValue> Item) { Array.Add(Item); }
    
    void Reset() { Array.Reset(); }

    /** 通过Key搜索对应的键值对 */
    TSharedPtr<FArmyKeyValue> FindByKey(const int32 Key)
    {
        for (auto& It : Array)
        {
            if (It->Key == Key)
            {
                return It;
            }
        }

        return NULL;
    }

	void Remove(TSharedPtr<FArmyKeyValue> Item)
	{
		if (Array.Contains(Item))
		{
			Array.Remove(Item);
		}
	}

    /** 通过Value搜索对应的键值对 */
    TSharedPtr<FArmyKeyValue> FindByValue(const FString& Value)
    {
        for (auto& It : Array)
        {
            if (It->Value.Equals(Value))
            {
                return It;
            }
        }

        return NULL;
    }
};

enum FrameFillType
{
	FRAME_AREA_Main = -1,//施工图区域
	FRAME_AREA_COMPANYNAME = 0,//公司名称
	FRAME_AREA_DESCRIBE = 1,//描述信息
	FRAME_AREA_DECLARATION = 2,//声明信息
	FRAME_AREA_TITLE_REVISION = 3,//修改
	FRAME_AREA_TITLE_CUSTOMERINFO = 4,//客户信息
	FRAME_AREA_TITLE_CUSTOMERNAME = 5,//客户名称
	FRAME_AREA_TITLE_PROJECTIONADDRESS = 6,//项目地址
	FRAME_AREA_TITLE_CONSTRUCTIONNAME = 7,//图纸名称
	FRAME_AREA_TITLE_REMARKS_E = 8,//备注（英文）
	FRAME_AREA_TITLE_REMARKS_C = 9,//备注（中文）
	FRAME_AREA_TITLE_DESIGNED_E = 10,//设计（英文）
	FRAME_AREA_TITLE_DESIGNED_C = 11,//设计（中文）
	FRAME_AREA_TITLE_DRAW_E = 12,//制图（英文）
	FRAME_AREA_TITLE_DRAW_C = 13,//制图（中文）
	FRAME_AREA_TITLE_APPROVED_E = 14,//审核（英文）
	FRAME_AREA_TITLE_APPROVED_C = 15,//审核（中文）
	FRAME_AREA_TITLE_SCALE_E = 16,//比例（英文）
	FRAME_AREA_TITLE_SCALE_C = 17,//比例（中文）
	FRAME_AREA_TITLE_DRAWINGNO_E = 18,//图号（英文）
	FRAME_AREA_TITLE_DRAWINGNO_C = 19,//图号（中文）
	FRAME_AREA_TITLE_JOBNO_E = 20,//编号（英文）
	FRAME_AREA_TITLE_JOBNO_C = 21,//编号（中文）
	FRAME_AREA_TITLE_DATE = 22,//日期
	FRAME_AREA_TITLE_CONTRACTNO = 23,//合同号
								///
	FRAME_AREA_CONTRACTNO = 24,//合同号
	//FRAME_AREA_CONTRACTNONAME,//合同号
	FRAME_AREA_CUSTOMERNAME = 25,//客户名称
	FRAME_AREA_PROJECTIONADDRESS = 26,//项目地址
	FRAME_AREA_CONSTRUCTIONNAME = 27,//图纸名称
	FRAME_AREA_REMARKS = 28,//备注信息
	FRAME_AREA_DESIGNERNAME = 29,//设计师
	FRAME_AREA_NAME = 30,//公司名称
	FRAME_AREA_DRAWINGNO = 31,//图号

	FRAME_AREA_USERDEFINE = 33
};
struct FrameRectInfo
{
	FrameRectInfo(FrameFillType InType, FString InFillText, uint32 InFontSize)
		:FillType(InType), FillText(InFillText), FontSize(InFontSize)
	{}
	FrameFillType FillType;
	FString FillText;
	uint32 FontSize;
};

static TArray<TSharedPtr<FrameRectInfo>> FrameRectTypeList =
{
	MakeShareable(new FrameRectInfo(FRAME_AREA_USERDEFINE,				TEXT("自定义"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_Main,					TEXT("公司名"),20)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_COMPANYNAME,				TEXT(" "),20)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_DESCRIBE,				TEXT("公司信息描述"),5)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_DECLARATION,				TEXT("声明信息"),8)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_REVISION,			TEXT("修改   REVISION"),5)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_CUSTOMERINFO,		TEXT("客户信息"),12)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_CUSTOMERNAME,		TEXT("客户姓名"),8)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_PROJECTIONADDRESS,	TEXT("项目地址"),8)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_CONSTRUCTIONNAME,	TEXT("TITLE	图纸名称"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_REMARKS_E,			TEXT("REMARKS"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_REMARKS_C,			TEXT("备注"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DESIGNED_E,		TEXT("DESIGNED"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DESIGNED_C,		TEXT("设计"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DRAW_E,			TEXT("DRAW"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DRAW_C,			TEXT("制图"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_APPROVED_E,		TEXT("APPROVED"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_APPROVED_C,		TEXT("审核"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_SCALE_E,			TEXT("SCALE"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_SCALE_C,			TEXT("比例"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DRAWINGNO_E,		TEXT("DRAWING NO"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DRAWINGNO_C,		TEXT("图号"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_JOBNO_E,			TEXT("JOB NO"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_JOBNO_C,			TEXT("编号"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_TITLE_DATE,				TEXT("日期 DATE"),10)),

	MakeShareable(new FrameRectInfo(FRAME_AREA_CONTRACTNO,				TEXT("xxx-xx-xxxx-xxxx-xxxx"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_CUSTOMERNAME,			TEXT("xxx"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_PROJECTIONADDRESS,		TEXT("xxxxxxxxx"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_CONSTRUCTIONNAME,		TEXT("xxxxxxxxx"),15)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_REMARKS,					TEXT("xxxxxxxxx"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_DESIGNERNAME,			TEXT("xxxxxxxxx"),10)),
	MakeShareable(new FrameRectInfo(FRAME_AREA_DRAWINGNO,				TEXT("xxxxxxxxx"),10)),
};