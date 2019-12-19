#pragma once

#define MAKE_TEXT(Text) FText::FromString(TEXT(Text))
#define FORMAT_TEXT(Format, ...) FText::FromString(FString::Printf(TEXT(Format), __VA_ARGS__))

struct SCTXRArgument
{
#define SCT_XR_ARGUMENT( ArgType, ArgName ) \
		ArgType _##ArgName; \
		SCTXRArgument& ArgName( ArgType InArg ) \
		{ \
			_##ArgName = InArg; \
			return *this; \
		}

	SCTXRArgument(int32 InType = -1) :_ArgInt32(InType),_ArgUint32(0),_ArgBoolean(true){}
	SCT_XR_ARGUMENT(int32, ArgInt32);
	SCT_XR_ARGUMENT(uint32, ArgUint32);
	SCT_XR_ARGUMENT(bool, ArgBoolean);
	SCT_XR_ARGUMENT(FString, ArgString);
	SCT_XR_ARGUMENT(FName, ArgFName);
	SCT_XR_ARGUMENT (FColor,ArgColor);
	SCT_XR_ARGUMENT (float,ArgFloat);
	SCT_XR_ARGUMENT (void*,ArgVoid);
};

DECLARE_DELEGATE(SCT_FVoidDelegate);
DECLARE_DELEGATE_OneParam(SCT_SCommandOperator, SCTXRArgument);
DECLARE_DELEGATE_OneParam(SCT_FInputKeyDelegate, const struct FKeyEvent&);
DECLARE_DELEGATE_TwoParams(SCT_FInputKeysDelegate,const struct FGeometry& ,const struct FKeyEvent&);
DECLARE_DELEGATE_OneParam(SCT_FInt32Delegate, const int32);
DECLARE_DELEGATE_FourParams(SCT_FFourInt32Delegate, const int32, const int32, const int32, const int32);
DECLARE_DELEGATE_OneParam(SCT_FFStringDelegate, const FString);
DECLARE_DELEGATE_OneParam(SCT_FFloatDelegate, const float);
DECLARE_DELEGATE_OneParam(SCT_FVectorDelegate, const FVector&);
DECLARE_DELEGATE_OneParam(SCT_FVector2DDelegate, const FVector2D&);
DECLARE_DELEGATE_OneParam(SCT_FBoolDelegate, bool);
DECLARE_DELEGATE_OneParam(SCT_FObjectDelegate, UObject*);
DECLARE_DELEGATE_OneParam(SCT_FActorDelegate, class AActor*);
DECLARE_DELEGATE_OneParam(SCT_FOnDelegateColor, const FLinearColor&);
DECLARE_DELEGATE_TwoParams(SCT_FLine2DDelegate, const FVector2D&, const FVector2D&);
DECLARE_DELEGATE_ThreeParams(SCT_FCircle2DDelegate, const FVector2D&, const FVector2D& , const FVector2D&);
DECLARE_DELEGATE_RetVal(TSharedPtr<class SWidget>, SCT_FGetWidgetDelegate);
DECLARE_DELEGATE_OneParam (SCT_FArmyObjectesDelegate,TArray<TSharedPtr< class FArmyObject>>);
/**
 * 键值对
 */
struct FSCTXRKeyValue
{
    int32 Key;
    FString Value;

    FSCTXRKeyValue(int32 InKey, FString InValue)
    {
        Key = InKey;
        Value = InValue;
    }
};

/**
 * 下拉框数据数组
 */
struct FSCTXRComboBoxArray
{
    TArray< TSharedPtr<FSCTXRKeyValue> > Array;

    void Add(TSharedPtr<FSCTXRKeyValue> Item) { Array.Add(Item); }
    
    void Reset() { Array.Reset(); }

    /** 通过index搜索对应的键值对 */
    TSharedPtr<FSCTXRKeyValue> Find(int32 Index)
    {
        for (auto& It : Array)
        {
            if (It->Key == Index)
            {
                return It;
            }
        }

        return NULL;
    }
};

enum class SCTFrameFillType
{
	FRAME_AREA_Main = -1,//施工图区域
	FRAME_AREA_COMPANYNAME = 0,//公司名称
	FRAME_AREA_DESCRIBE,//描述信息
	FRAME_AREA_DECLARATION,//声明信息
	FRAME_AREA_TITLE_REVISION,//修改
	FRAME_AREA_TITLE_CUSTOMERINFO,//客户信息
	FRAME_AREA_TITLE_CUSTOMERNAME,//客户名称
	FRAME_AREA_TITLE_PROJECTIONADDRESS,//项目地址
	FRAME_AREA_TITLE_CONSTRUCTIONNAME,//图纸名称
	FRAME_AREA_TITLE_REMARKS_E,//备注（英文）
	FRAME_AREA_TITLE_REMARKS_C,//备注（中文）
	FRAME_AREA_TITLE_DESIGNED_E,//设计（英文）
	FRAME_AREA_TITLE_DESIGNED_C,//设计（中文）
	FRAME_AREA_TITLE_DRAW_E,//制图（英文）
	FRAME_AREA_TITLE_DRAW_C,//制图（中文）
	FRAME_AREA_TITLE_APPROVED_E,//审核（英文）
	FRAME_AREA_TITLE_APPROVED_C,//审核（中文）
	FRAME_AREA_TITLE_SCALE_E,//比例（英文）
	FRAME_AREA_TITLE_SCALE_C,//比例（中文）
	FRAME_AREA_TITLE_DRAWINGNO_E,//图号（英文）
	FRAME_AREA_TITLE_DRAWINGNO_C,//图号（中文）
	FRAME_AREA_TITLE_JOBNO_E,//编号（英文）
	FRAME_AREA_TITLE_JOBNO_C,//编号（中文）
	FRAME_AREA_TITLE_DATE,//日期
	FRAME_AREA_TITLE_CONTRACTNO,//合同号
								///
	FRAME_AREA_CONTRACTNO,//合同号
	FRAME_AREA_CUSTOMERNAME,//客户名称
	FRAME_AREA_PROJECTIONADDRESS,//项目地址
	FRAME_AREA_CONSTRUCTIONNAME,//图纸名称
	FRAME_AREA_REMARKS,//备注信息
	FRAME_AREA_DRAWINGNO,//图号
	FRAME_AREA_USERDEFINE
};
struct SCTFrameRectInfo
{
	SCTFrameRectInfo(SCTFrameFillType InType, FString InFillText, uint32 InFontSize)
		:FillType(InType), FillText(InFillText), FontSize(InFontSize)
	{}
	SCTFrameFillType FillType;
	FString FillText;
	uint32 FontSize;
};

static TArray<TSharedPtr<SCTFrameRectInfo>> SCTFrameRectTypeList =
{
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_USERDEFINE,				TEXT("自定义"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_Main,					TEXT("公司名"),20)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_COMPANYNAME,				TEXT("打扮家"),20)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_DESCRIBE,				TEXT("公司信息描述"),5)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_DECLARATION,				TEXT("声明信息"),8)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_REVISION,			TEXT("修改   REVISION"),5)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_CUSTOMERINFO,		TEXT("客户信息"),12)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_CUSTOMERNAME,		TEXT("客户姓名"),8)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_PROJECTIONADDRESS,	TEXT("项目地址"),8)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_CONSTRUCTIONNAME,	TEXT("TITLE	图纸名称"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_REMARKS_E,			TEXT("REMARKS"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_REMARKS_C,			TEXT("备注"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DESIGNED_E,		TEXT("DESIGNED"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DESIGNED_C,		TEXT("设计"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DRAW_E,			TEXT("DRAW"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DRAW_C,			TEXT("制图"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_APPROVED_E,		TEXT("APPROVED"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_APPROVED_C,		TEXT("审核"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_SCALE_E,			TEXT("SCALE"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_SCALE_C,			TEXT("比例"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DRAWINGNO_E,		TEXT("DRAWING NO"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DRAWINGNO_C,		TEXT("图号"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_JOBNO_E,			TEXT("JOB NO"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_JOBNO_C,			TEXT("编号"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_TITLE_DATE,				TEXT("日期 DATE"),10)),

	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_CONTRACTNO,				TEXT("xxx-xx-xxxx-xxxx-xxxx"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_CUSTOMERNAME,			TEXT("xxx"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_PROJECTIONADDRESS,		TEXT("xxxxxxxxx"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_CONSTRUCTIONNAME,		TEXT("xxxxxxxxx"),15)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_REMARKS,					TEXT("xxxxxxxxx"),10)),
	MakeShareable(new SCTFrameRectInfo(SCTFrameFillType::FRAME_AREA_DRAWINGNO,				TEXT("xxxxxxxxx"),10)),
};