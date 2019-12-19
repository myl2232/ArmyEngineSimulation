/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRConstructionQuantity.h
 * @Description 施工算量相关数据结构
 *
 * @Author 欧石楠
 * @Date 2018年7月17日
 * @Version 1.0
 */

#pragma once

#include "ArmyConstructionQuantity.generated.h"

/**
 * 乳胶漆色号信息
 */
USTRUCT()
struct FArmyLatexPaintColor
{
    GENERATED_BODY()
    
    /** 乳胶漆色号 */
    UPROPERTY()
    FString LatexPaintColorNumber;

    /** 用量 */
    UPROPERTY()
    float Dosage=0;

	//@郭子阳
	bool operator==(const FArmyLatexPaintColor& InOther) const
	{
		return LatexPaintColorNumber == InOther.LatexPaintColorNumber;
	}
};

/*
*施工项属性
*/
USTRUCT()
struct FArmyPropertyValue
{
	GENERATED_BODY()

	FArmyPropertyValue() {}

	FArmyPropertyValue(int32 NewpropertyId, int32 NewvalueId)
		: propertyId(NewpropertyId)
		, valueId(NewvalueId)
	{}

	/** 属性 */
	UPROPERTY()
	int32 propertyId = 0;

	/** 属性值 */
	UPROPERTY()
	int32 valueId = 0;

	//@郭子阳
	bool operator==(const FArmyPropertyValue& InOther) const
	{
		return propertyId == InOther.propertyId && valueId == InOther.valueId;
	}

};

/* @郭子阳
*施工参数
*/
USTRUCT()
struct FArmyConstructionParameter
{
	GENERATED_BODY()

	/** 施工面积 */
	UPROPERTY()
	float S = 0;

	/** 施工周长 */
	UPROPERTY()
	float C = 0;

	/** 施工长度 */
	UPROPERTY()
	float L = 0;

	/** 施工高度 */
	UPROPERTY()
	float H = 0;

	/** 直接量 */
	UPROPERTY()
	float D = 0;

	//@郭子阳
	FArmyConstructionParameter& operator+(const FArmyConstructionParameter& Rv) const
	{
		FArmyConstructionParameter NewParameter;
		NewParameter.S += Rv.S;
		NewParameter.C += Rv.C;
		NewParameter.L += Rv.L;
		NewParameter.H += Rv.H;
		NewParameter.D += Rv.D;
		return NewParameter;
	}

	//@郭子阳
	void operator+=(const FArmyConstructionParameter& Rv) 
	{
        S += Rv.S;
		C += Rv.C;
		L += Rv.L;
		H += Rv.H;
		D += Rv.D;
	}
};

/*@郭子阳
*施工项
*/
USTRUCT()
struct FArmyQuota
{
	GENERATED_BODY()

	//施工项ID
	UPROPERTY()
	int32 QuotaId = 0;

	// 施工项属性及属性值
	UPROPERTY()
	TArray<FArmyPropertyValue> QuotaProperties;

	UPROPERTY()
	int32 Dosage = 0;

	bool operator==(const FArmyQuota& InOther) const
	{
		return	QuotaId == InOther.QuotaId
				&& QuotaProperties == InOther.QuotaProperties;
	}
		
	//使用之前必须判断两个施工项数据相等
	void operator +=(const FArmyQuota& InOther)
	{
		Dosage += InOther.Dosage;
	}
};

/*@郭子阳
*空间信息
*/
USTRUCT()
struct FArmySpaceMsg
{
	GENERATED_BODY()

	// 商品相邻空间1
	UPROPERTY()
	FString SpaceName1;

	// 商品相邻空间2
	UPROPERTY()
	FString SpaceName2;

	bool operator==(const FArmySpaceMsg& InOrther) const
	{
		return SpaceName1 == InOrther.SpaceName1
			   &&SpaceName2 == InOrther.SpaceName2
			   ;
	
	}
};


/*@郭子阳
*门信息
*/
USTRUCT()
struct FArmyDoorMsg
{
	GENERATED_BODY()

	// 门宽
	UPROPERTY()
	float DoorWidth = 0;

	// 门高
	UPROPERTY()
	float DoorHeight = 0;

	// 墙体厚度
	UPROPERTY()
	float WallThickness = 0;

	// 开合方向
	UPROPERTY()
	float SwitchingDirection = 0;
};

//@郭子阳
//方案估价数据项
USTRUCT()
struct FArmyPlanPriceItem
{
	GENERATED_BODY()

		UPROPERTY()
		int32	Number=0;// 子项数
		UPROPERTY()
		int32	SaleOrQuotaNumber=0;// 商品 / 施工项数
		UPROPERTY()
		float	SaleOrQuotaTotal=0;// 商品 / 施工项类型总计
		UPROPERTY()
		float	Total=0;//总计
		UPROPERTY()
		int32	Type=0;//	该项类型  1	主材2	定制商品3	软装4	施工项
};


//@郭子阳
//方案估价数据
USTRUCT()
struct FArmyPlanPrice
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FArmyPlanPriceItem> Items;
	UPROPERTY()
		float	SaleOrQuotaTotal = 0;// 商品 / 施工项类型总计
	UPROPERTY()
		float	Total = 0;//总计
};

/* @郭子阳 V1.3新增
*商品信息
*/
USTRUCT()
struct FArmyGoods
{
	GENERATED_BODY()

	//@郭子阳 V1.3新增
	/** 子项类型: 1 素材/商品 4 施工项 7 构件  */
	UPROPERTY()
	int32 Type = 0;

	//@郭子阳 V1.3新增
	// 该字段仅在拆改建下起作用，1 新建墙 2 拆除墙 3 原始门洞 4 拆门洞 5 补门洞 6 填门洞 7 开垭口 8 包立管
	UPROPERTY()
	int32 DemolitionOrRebuilding = 0;

	//@郭子阳 V1.3新增
	/**  商品Id */
	UPROPERTY()
	int32 GoodsId = 0;

	//@郭子阳 V1.3新增
	// 构件ID
	UPROPERTY()
	int32 MemberId = 0;

	/** 铺法ID */
	UPROPERTY()
	int32 PaveID = 0;

	//墙体材质
	UPROPERTY()
	int32 wallMaterial = 0;

	//@郭子阳 V1.3新增
	//施工参数
	UPROPERTY()
	FArmyConstructionParameter Param;

	//@郭子阳 V1.3新增
	//施工项数据
	UPROPERTY()
	TArray<	FArmyQuota> QuotaData;

	/** 若为乳胶漆，提供乳胶漆色号 */
	UPROPERTY()
	TArray<FArmyLatexPaintColor> LatexPaintColorData;

	/** 标记 水电下用来标记是否为断点改1：是 0 否*/
	UPROPERTY()
	int32 BreakpointReform = 0;

	//@郭子阳 V1.3新增
	//空间信息
	UPROPERTY()
	FArmySpaceMsg SpaceMsg;

	//@郭子阳 V1.3新增
	//门信息
	UPROPERTY()
	FArmyDoorMsg DoorMsg;

	bool operator==(const FArmyGoods& InOrther) const
	{
		return Type == InOrther.Type
			&&DemolitionOrRebuilding == InOrther.DemolitionOrRebuilding
			&&GoodsId == InOrther.GoodsId
			&&MemberId == InOrther.MemberId
			&&PaveID == InOrther.PaveID
			//&&Param == InOrther.Param
			&&QuotaData == InOrther.QuotaData
			//&&LatexPaintColorData == InOrther.LatexPaintColorData
			&&BreakpointReform == InOrther.BreakpointReform
			&&SpaceMsg== InOrther.SpaceMsg
			;
	}
};


enum DataType
{
	M_LoftingLine,
	M_RegionalPaving,
	M_WallPainting,
	M_OriginalConstructionArea
};


/* @郭子阳
* 空间参数
*/
USTRUCT()
struct FArmySpaceParameters
{
	GENERATED_BODY()

    // 空间面积
	UPROPERTY()
	float Rs = 0.f;

    // 空间周长
	UPROPERTY()
	float Rc = 0.f;

    // 空间高度
	UPROPERTY()
	float Rh = 0.f;

    // 空间墙面积
	UPROPERTY()
	float Ws = 0.f;
};


/** 
 * 区域 
 */
USTRUCT()
struct FArmyArea
{
	GENERATED_BODY()

	//@郭子阳
	bool operator==(const FArmyArea& InOther) const
	{
		return AreaName == InOther.AreaName;//&& AreaType == InOther.AreaType;
	}

	UPROPERTY()
	FString AreaName;

	//商品数据
	UPROPERTY()
	TArray<FArmyGoods>  ArtificialData;

	FArmyArea() {};

};

/**
 * 空间内的施工算量
 */
USTRUCT()
struct FArmySpaceData
{
	GENERATED_BODY()

		FArmySpaceData() {};

	/** 标识空间的功能作用 */
	UPROPERTY()
	int32 SpaceID;

    /** 空间名称 */
    UPROPERTY()
    FString SpaceName;

	//@郭子阳 V1.3新增
	/** 空间类型：1 普通空间如：主卧、客厅等，2 水电 3 门窗工程 4 拆改建 */
	UPROPERTY()
		int32 SpaceType=0;

	//@郭子阳 V1.3新增
	//空间参数
	UPROPERTY()
	FArmySpaceParameters Param;

	/**V1.3不用，将来可能会用 区域数据，当施工量参数存在区域划分时，相关参数在此处提供，区域内只存在施工相关参数及商品 */
	UPROPERTY()
	TArray<FArmyArea> AreaData;

	//@郭子阳 V1.3新增
	//商品数据
	UPROPERTY()
	TArray<FArmyGoods>  ArtificialData;

};

// 木作柜子附属数据
USTRUCT()
struct FArmyWHCListCabinetElemData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Type;

	UPROPERTY()
	int32 SingleType; // 住范临时使用，用来区分Elem是否是门板 1-是门板/抽面 0-不是

	UPROPERTY()
	FString Code;

	UPROPERTY()
	FString ThumbnailUrl;

	UPROPERTY()
	FString SubstrateName;

	UPROPERTY()
	int32 MaterialId;

	UPROPERTY()
	FString MaterialName;

	UPROPERTY()
	FString BrandName;

	UPROPERTY()
	FString Version;

	UPROPERTY()
	FString Wdh;

	UPROPERTY()
	float Quantity;

	UPROPERTY()
	FString QuantityStr;

	UPROPERTY()
	FString QuantityUnit;

	UPROPERTY()
	TArray<FString> OpenDoorDirection;

	FArmyWHCListCabinetElemData()
	: Quantity(0)
	, SingleType(0)
	, MaterialId(0) {}
};

// 木作门信息
USTRUCT()
struct FArmyWHCListCabinetDoorItem
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id;

	UPROPERTY()
	int32 MaterialId;

	UPROPERTY()
	int32 OpenDoorDirection;

	UPROPERTY()
	int32 Type;

	FArmyWHCListCabinetDoorItem()
	: Id(0)
	, MaterialId(0)
	, OpenDoorDirection(0)
	, Type(0) {}
};

// 木作柜子数据
USTRUCT()
struct FArmyWHCListCabinetData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int32 Type;

	UPROPERTY()
	int32 Category;

	UPROPERTY()
	int32 VrSpaceId;

	UPROPERTY()
	FString Code;

	UPROPERTY()
	FString ThumbnailUrl;

	UPROPERTY()
	FString SubstrateName;

	UPROPERTY()
	int32 MaterialId;

	UPROPERTY()
	FString MaterialName;

	UPROPERTY()
	FString Wdh;

	UPROPERTY()
	float HollowWidth;

	UPROPERTY()
	int32 Quantity;

	UPROPERTY()
	FString QuantityUnit;

	UPROPERTY()
	FString SpaceName;

	UPROPERTY()
	TArray<FArmyWHCListCabinetElemData> Cabinetelems;

	UPROPERTY()
	TArray<FArmyWHCListCabinetDoorItem> DoorItems;

	UPROPERTY()
	TArray<int32> CabinetLabels;

	FArmyWHCListCabinetData()
	: Quantity(0)
	, Type(0)
	, VrSpaceId(0)
	, MaterialId(0) {}
};

// 木作台面信息
USTRUCT()
struct FArmyWHCModeTableData
{
	GENERATED_BODY()

	UPROPERTY()
	FString MaterialName;

	UPROPERTY()
	int32 MaterialId;

	FArmyWHCModeTableData()
	: MaterialId(0) {}
};

// 木作柜体清单
USTRUCT()
struct FArmyWHCModeListData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FArmyWHCListCabinetData> Cabinets;

	UPROPERTY()
	TArray<FArmyWHCListCabinetElemData> Kitchens;

	UPROPERTY()
	FArmyWHCModeTableData Worktop;
};

/**
 * 总体施工算量
 */
USTRUCT()
struct FArmyConstructionQuantity
{
	GENERATED_BODY()

	/** 各个空间的数据 */
	UPROPERTY()
	TArray<FArmySpaceData> SpaceData;

	UPROPERTY()
	FArmyWHCModeListData CustomCupboard;

	UPROPERTY()
	FArmyWHCModeListData CustomWardrobe;

	UPROPERTY()
	FArmyWHCModeListData CustomOtherCabinet;
};