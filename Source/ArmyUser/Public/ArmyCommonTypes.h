﻿/**
 * Copyright 2019 北京北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File ArmyCommonTypes.h
 * @Description 全局通用的结构定义，通常只需要引入该模块就可以直接使用
 *
 * @Author 欧石楠
 * @Date 2018年4月3日
 * @Version 1.0
 */

#pragma once
#include "ArmyTypes.h"
#include "ArmyAutoDesignSettings.h"
#include "Json.h"
 /*
 *	一个Actor在场景中的类型：静态MeshActor，可移动MeshActor，蓝图Actor，环境组件Actor（光源，反射球，热点球）
 */
enum EActorType
{
	//烘焙户型中的导入自FBX的静态模型
	Static,
	//可移动的家具
	Moveable,
	//蓝图类型
	Blueprint,
	//窗户
	Blueprint_Window,
	//光源，反射球等环境组件
	EnvironmentAsset,
	//管线
	PipeLine,
	// 波打线
	BodaAreaActor,
	// 墙体
	WallAreaActor,
	Other,
};

/** 业务资源类型 */
enum EArmyBusinessType
{
    /** 项目 */
    BT_PROJECT = 1,

    /** 方案 */
    BT_PLAN,

    /** 户型 */
    BT_HOME,
    
    /** 素材 */
    BT_GOODS,
};

/** 文件资源类型 */
enum EArmyResourceType
{
    /** 3D */
    RT_PAK = 1,

    /** 铺贴 */
    RT_IMG,

    /** 放样 */
    RT_DXF,

    /** 构件俯视图 */
    RT_MEMBER_VERTICAL_DXF,

    /** 构件正视图 */
    RT_MEMBER_FRONT_DXF,

    /** 构件侧视图 */
    RT_MEMBER_SIDE_DXF,

    /** 构件PAK */
    RT_MEMBER_PAK,

    /** JSON文件 */
    RT_JSON,
};

//硬盘上的文件类型：户型，模型，材质，音频，蓝图类，蒙皮网格，交互灯，方案文件等；该类型直接取决于加载过程的解析方式
enum EResourceType
{
	//带动画的模型
	SkeletalMesh = -3,
	//可交互的灯
	Light = -2,
	//无
	None = -1,
	//绘制户型
	Home = 0,
	//烘焙户型
	CookedHome = 1,
	//可移动家的具模型
	MoveableMesh = 2,
	//蓝图
	BlueprintClass = 3,
	//壁纸瓷砖
	Material = 4,
	//户型方案
	Plan = 5,
	//组方案
	GroupPlan = 6,
	//项目
	Project = 9,
	//施工图Json
	Construction = 10,
	//施工图图片包
	ConstructionImages = 11,
	//地板拼花
	CustomlizedFloor = 12,
	//吊顶
	CustomlizedCeling = 13,
	//造型墙
	CustomlizedWall = 14,
	//墙顶地纹理
	Texture = 15,
	//水电素材
	Hydropower = 16,
	//硬装工具中的dxf模型数据
	HardDXF = 17,
	//硬装工具中的pak模型数据
	HardPAK = 18,
	//构件信息
	ComponentDXF = 19,
	//大师方案
	GreatPlan = 20,

    // @欧石楠 底图
    Facsimile = 21,
};

enum EModeIdent //所属模块
{
	MI_None = 0,
	MI_SoftHardMode = 1,
	MI_HydropowerMode = 2
};

//吸附面类型
enum EMeshSnapType
{
	MST_None = 0,
	//墙面
	MST_Wall,
	//地面
	MST_Bottom,
	//顶面
	MST_Top,
	//台面
	MST_TableFace,
};

//模型旋转轴点类型
//enum EMeshOriginType
//{
//	//0
//	MOT_None = 0,
//	//1.底面主接触面中心
//	MOT_BottomContactCenter,
//	//2.墙面主接触面中心
//	MOT_WallContactCenter,
//	//3.顶面主接触面中心
//	MOT_TopContactCenter,
//	//4.墙和地主接触面中心
//	MOT_WallBottomContactCenter,
//	//5.墙和顶接触面中心
//	MOT_WallTopContactCenter,
//	//6.墙和墙接触面中心
//	MOT_WallCornerContactCenter,
//	//7.接触面内顶角
//	MOT_InnerTopContactCorner,
//	//8.对齐面中心
//	MOT_AlignPlaneContactCenter,
//	//9.杆的中心点
//	MOT_PoleCenter,
//	//10.与杆接触面的中心点
//	MOT_PoleContactCenter,
//	//11.台面和墙面的接触面中心点
//	MOT_TableFaceWallContactCenter,
//	//12.主接触面中心底部
//	MOT_MainBottomContactCenter,
//	//13.合页轴心底部接触面
//	MOT_BottomAxis,
//	//—————以下木作——————
//	//14.钉子轴心与对齐面交点
//	MOT_NailAxisContactIntersectionPoint,
//	//15.安装孔中心
//	MOT_MountingHoleCenter,
//	//16.轴中心
//	MOT_AxisCenter,
//	//17.柜角位置
//	MOT_CabinetCorner,
//	//18.主接触面中心
//	MOT_MainContactCenter,
//	//19.接触面与安装孔中心
//	MOT_MountingHoleContactCenter,
//	//20.人工判断
//	MOT_Manually,
//	//21.左下后
//	MOT_LeftBottomBack,
//};

//硬盘上的文件的完整性状态：不存在，正在下载中，已经完整下载完
enum EFileExistenceState
{
	//不存在硬盘上
	NotExist,
	//正在下载等待队列
	PendingDownload,
	//已存在，但不确定完整性
	Exist,
	//完整合法
	Complete,
};

enum EContentItemFlag
{
	PF_NoFlags = 0x00000000,
	//是否为家具
	PF_Furniture = 0x00000001,
	//是否为户型
	PF_Home = 0x00000002,
	//是否为方案
	PF_Plan = 0x00000004,
	//户型方案
	PF_LevelPlan = 0x00000008,
	//家具组
	PF_GroupPlan = 0x00000010,
	//是否同时加载户型
	PF_LoadHome = 0x00000020,
	//是否是最新的物品
	PF_Newest = 0x00000040,
	//是否可编辑
	PF_Editable = 0x00000080,
	//是否为已收藏的物品
	PF_InFavorite = 0x00000100,
	//是否属套餐物品
	PF_InPackage = 0x00000200,
	//是否为自动设计组
	PF_AutoDesignGroup = 0x00000400,
	//可替换物品
	PF_Replacement = 0x00000800,
	PF_OrignalCenter = 0x00001000
};

/**
* 水电构件id
*/
enum EComponentID
{
	EC_None,
	
	EC_Socket_Five_Point = 20,//五孔插座
	EC_Socket_Five_On_Point = 40,//五孔开关插座
	EC_Socket_Five_On_Dislocation = 1100,//五孔错位插座
	EC_Socket_Five_Point_Dislocation,//五孔错位开关插座
	EC_Socket_Four_Point = 63, //四孔插座
	EC_Socket_Four_On_Point = 64,//四孔开关插座
	EC_Socket_Three_On_Point = 1000, //三孔开关插座
	EC_Socket_Three, //三孔插座
	EC_Socket_Three_On_Point_3A = 71,  //16A三孔开关插座
	EC_Socket_Three_3A = 70, //16A三孔插座
	EC_Socket_Three_On_Point_10A = 69,  //10A三孔开关插座
	EC_Socket_Air_Conditioner = 50,//空调挂机三孔插座
	EC_Socket_USB = 68,//五孔USB插座
	EC_Socket_Air_Conditionner_Normal = 53,//空调柜机插座
	EC_Socket_HotWater = 52,//热水器五孔插座
	EC_Socket_PaiYan = 51,//排烟机五孔插座

	

	EC_Socket_White,//白板
	EC_Socket_Spatter = 72,//防溅盒
	EC_Socket_IT = 57,//网线插座
	EC_Socket_IT_TV = 65,//电视电脑插座
	EC_Socket_TV = 56,//电视插座
	EC_Socket_Phone = 58,//电话插座
	EC_Socket_IT_Phone = 67,//电脑电话插座
	EC_Socket_TV_Phone = 66,//电视电话插座
	EC_Socket_TP = 54, //厨房橱柜专用联排插座
	EC_Socket_TV_Couplet = 42, //电视墙专用联排插座

	EC_EleBoxL_Point = 15,//弱电箱
	EC_EleBoxH_Point = 16,//强电箱

	
	EC_Switch_BathHeater = 49,//浴霸开关
	EC_Switch_Double = 48,//双键无线开关
	EC_Switch_Single = 47,//单键无线开关
	EC_Switch4O2_Point = 46,//四联双控开关
	EC_Switch4O1_Point = 45,//四联单控开关
	EC_Switch3O2_Point = 44,//三联双控开关
	EC_Switch3O1_Point = 43,//三联单控开关
	EC_Switch2O2_Point = 30,//双联双控开关
	EC_Switch2O1_Point = 25,//双联单控开关
	EC_Switch1O2_Point = 221,//单连双控开关
	EC_Switch1O1_Point = 24,//单连单控开关

	
	EC_Water_Chilled_Point = 17,//冷水点位
	EC_Water_Hot_Chilled_Point = 18,//冷热水
	EC_Dewatering_Point = 400,//排水点位
	EC_Water_Supply = 62, // 给水点位,
	EC_Drain_Point = 61,//下水主管道
	EC_Basin = 60,//地漏下水
	EC_Water_Basin = 59,//水盆下水
	EC_Water_Hot_Point = 55,//热水点位
	EC_Closestool = 31,//马桶下水
	//EC_Gas = 32,//燃气点位

	EC_Supply_Same_Tee = 73,//给水管等径正三通
	EC_Supply_Diff_Tee = 75,//给水管异径正三通
	EC_Supply_Same_45_Tee = 94,//给水管等径45斜三通
	EC_Supply_Diff_45_Tee = 95,//给水管异径45斜三通
	EC_Supply_NoPlane_Same_Tee = 117, //给水管立体等径三通
	EC_Supply_NoPlane_Diff_Tee = 116, //给水管立体异径三通

	EC_Supply_Same_90_Flexure = 96,//给水管等径90弯头
	EC_Supply_Diff_90_Flexure = 97,//给水管异径90弯头
	EC_Supply_Same_45_Flexure = 98,//给水管异径45弯头
	EC_Supply_Diff_45_Flexure = 99,//给水管异径45弯头

	EC_Supply_Same_Direct = 108,//给水管等径直接
	EC_Supply_Diff_Direct = 109,//给水管异径直接

	EC_Supply_Plane_Same_FourLinks = 114, //给水管平面等径四通
	EC_Supply_Plane_Diff_FourLinks = 115, //给水管平面异径四通
	EC_Supply_NoPlane_Same_FourLinks = 118, //给水管立体等径四通
	EC_Supply_NoPlane_Diff_FourLinks = 119, //给水管立体异径四通

	EC_Drain_Bottle_Tee = 113,// 排水管瓶型三通
	EC_Drain_Same_Tee = 76,//排水管等径正三通
	EC_Drain_Diff_Tee = 74,//排水管异径正三通
	EC_Drain_Same_45_Tee = 77,//排水管等径45°斜三通
	EC_Drain_Diff_45_Tee = 78,//排水管异径45°斜三通

	EC_Drain_Same_Direct = 79,//排水管等径直接
	EC_Drain_Diff_Direct = 80,//排水管异径直接

	EC_Drain_Same_45_Flexure = 81, //排水管等径45弯头
	EC_Drain_Diff_45_Flexure = 82, //排水管异径45弯头
	EC_Drain_Same_90_Flexure = 83, //排水管等径90弯头
	EC_Drain_Diff_90_Flexure = 84, //排水管异径90弯头

	EC_Drain_Plane_Same_FourLinks = 85, //排水管平面等径四通
	EC_Drain_Plane_Diff_FourLinks = 86, //排水管平面异径四通
	EC_Drain_Same_45_FourLinks = 87,//排水管等径45四通
	EC_Drain_Diff_45_FourLinks = 88,//排水管异径45四通
	EC_Drain_NoPlane_Same_FourLinks = 89, //排水管立体等径四通
	EC_Drain_NoPlane_Diff_FourLinks = 92, //排水管立体异径四通

	EC_Trap_P = 90,//P型存水弯
	EC_Trap_S = 91,//S型存水弯
	EC_Trap_U = 93,//U型存水弯

	EC_Bridge_Same = 120, //等径过桥弯
	EC_Bridge_Diff = 121, //异径过桥弯

	EC_Gas_MainPipe = 1200,//燃气主管
	EC_Gas_Meter = 1201,//燃气表
	EC_Water_Normal_Point=1202,//中水点位
	EC_Water_Separator_Point=1203,//分集水器

	EC_300_300_LED_Light = 205,//300x300集成LED灯
	EC_300_600_LED_Light = 207,//300x600集成led灯
	EC_Wall_Light = 226,//壁灯
	EC_Spot_Light = 225,//射灯
	EC_Ceiling_Light = 266,//吸顶灯
	EC_Floor_Light = 256,//地灯
	EC_Down_Light = 209,//筒灯
	EC_Anti_Fog_Down_Light = 252,//防雾筒灯
	EC_Eight_Chandeliers_Light = 136,//八头吊灯（客厅用）
	EC_Four_Chandeliers_Light = 203,//八头吊灯（餐厅用）
	EC_300_600_301_Bath_Heater = 208,//300x600三合一浴霸
	EC_300_300_301_Bath_Heater = 206,//300x300三合一浴霸
	EC_Exhaust_Fan = 253,//排气扇
	EC_Single_Bed = 243,//单人床
	EC_Double_Bed = 193,//双人床
	EC_Double_Basin_Flume = 163,//双盆水槽
	EC_Single_Basin_Flume = 162,//单盆水槽
	EC_Dresser = 245,//梳妆台
	EC_Double_Sofa = 181,//双人沙发
	EC_Multiplayer_Sofa = 180,//双人沙发
	EC_Corner_Sofa = 234,//转角沙发
	EC_Desk = 202,//书桌
	EC_RCTtangular_Table = 210,//长方形餐桌
	EC_Lampblack_Machine = 215,//油烟机
	EC_ElCTtric_Water_Heater = 249,//电热水器
	EC_Wall_Air_Conditioner = 257,//挂机空调
	EC_Cabinet_Air_Conditioner = 258,//柜机空调
	EC_Single_Open_Refrigerator = 164,//单开电冰箱
	EC_Double_Open_Refrigerator = 247,//双开冰箱
	EC_TV_Cabinet = 176,//电视柜
	EC_Bathroom_Cabinet = 173,//浴室柜
	EC_Bathtub = 172,//浴缸
	EC_Washing_Machine = 171,//洗衣机
	EC_Shower = 167,//花洒

	EC_Gas_Water_Heater = 250,//燃气热水器
	EC_Water_Dispenser = 251,//饮水机
	EC_Squatting_Pan = 161,//蹲便器
	EC_Round_Table = 213,//圆形餐桌
	EC_Floor_Lamp = 179,//落地灯
	EC_Track_Light = 254,//轨道灯
	EC_Fishing_Lights = 255,//钓鱼灯
	EC_Round_Bedside_Cabinet = 229,//圆形床头柜
	EC_Rect_Bedside_Cabinet = 194,//矩形床头柜
	EC_Ground_Socket = 220,//地面插座


	EC_Socket_H_Defualt=2000 , //默认强电插座，用于没有构件的情况 //@欧石楠
	EC_Socket_L_Defualt=2001 , //默认弱电插座，用于没有构件的情况 //@欧石楠
	EC_Water_Defualt=2002 , //默认冷热水点位，用于没有构件的情况 //@欧石楠
	EC_Switch_Defualt=2003, //默认开关，用于没有构件的情况 //@欧石楠

	EC_Max
};
enum CategryType
{
	CT_None = 0,
	//地面
	CT_Floor = 1,
	//墙面
	CT_Wall = 2,
	//顶面
	CT_Ceiling = 3,
	//水电用材
	CT_HydroelectricMaterials = 4,
	//绘制管线
	CT_DrawPipeline = 5,
	//瓷砖
	CT_FloorTile = 6,
	//地板
	CT_FloorFloor = 7,
	//水刀拼花
	CT_FloorWaterKnife = 8,
	//波打线
	CT_FloorWaveLine = 9,
	//踢脚线
	CT_FloorKickingLine = 10,
	//连续直铺
	CT_Continue = 11,
	//工字铺法
	CT_Worker = 12,
	//旋风铺法
	CT_Wind = 13,
	//间隔铺法
	CT_Space = 14,
	//交错铺法
	CT_Interlace = 15,
	//人字铺
	CT_People = 18,
	//三六九铺
	CT_TrapeZoid = 19,
	//有角砖
	CT_CornerBrickMethod = 20,
	//无角砖
	CT_AntiCornerBrickMethod = 21,
	//墙砖
	CT_WallTile = 22,
	//墙板
	CT_WallFloor = 23,
	//墙漆
	CT_WallPaint = 24,
	//背景墙
	CT_WallBackground = 25,
	//灯槽
	CT_LampSlot = 26,
	//顶角线
	CT_CrownMoulding = 27,
	//壁纸
	CT_WallPaper = 28,
	//铝扣板
	CT_AluminousGussetPlate = 30,
	//吊顶灯
	CT_CeilingLamp = 43,
	//浴霸
	CT_BathHeater = 44,
	//排风扇
	CT_VentilatingFan = 45,
	//轻钢龙骨
	CT_SteelKeel = 47,
	//木龙骨
	CT_WoodKeel = 48,
	//造型线
	CT_MoldingLine = 58,
	// 配电箱
	CT_EleBox = 31,
	// 开关
	CT_SWITCH = 32,
	// 强电插座
	CT_Socket_Strong = 33,
	//弱电插座
	CT_Socket_Weak = 34,
	// 水路点位
	CT_Water_Point = 600180004,
	// 预装点位
	CT_Preload = 60028,
	//2.5平方强电线路
	CT_Strong_25h = 400190001,
	//4平方强电线路
	CT_Strong_4 = 400190002,
	//单控强电线路
	CT_Strong_Single = 400190003,
	//双控强电线路
	CT_Strong_Double = 400190004,
	//弱电 电视线
	CT_WeakElectricity_TV = 4002200001,
	//弱电， 网线
	CT_WeakElectricity_Net = 4002200003,
	//弱电，电话线
	CT_WeakElectricity_Phone = 4002200002,
	//冷水管
	CT_ColdWaterTube = 600180001,
	//热水管
	CT_HotWaterTube = 600180002,
	//排水管
	CT_Drain = 600180003,
};


//模型放置位置
struct PlacePosition
{
	//true地面可放置，false地面不可放置
	bool bFloor;
	//true墙面可放置，false墙面不可放置
	bool bWall;
	//true顶面可放置，false顶面不可放置
	bool bCeiling;
	//true台面可放置，false台面不可放置
	bool bMesa;
};

//@欧石楠
//墙类型
enum class EWallType
{
	Floor = 0, //地
	Wall = 1, //墙
	Roof = 2, //顶
	All //所有类型
};

//@欧石楠
//窗户类型
enum class EArmyWindowType
{
	NotWindow = -1, //不是窗户
	
	Standard = 1, //标准窗
	Floor=2, //落地窗
	
	StandardBayWindow=3,//标准飘窗
	TrapezoidBayWindow =4, //梯形飘窗

};

enum CategryApplicationType
{
	CAT_None = 0,
	//连续直铺
	CAT_Continue = 1,
	//工字铺法
	CAT_Worker = 2,
	//旋风铺法
	CAT_Wind = 3,
	//间隔铺法
	CAT_Space = 4,
	//交错铺法
	CAT_Interlace = 5,
	//人字铺
	CAT_People = 8,
	//三六九铺
	CAT_TrapeZoid = 9,
	//有角砖
	CAT_CornerBrickMethod = 10,
	//无角砖
	CAT_AntiCornerBrickMethod = 11,
	//墙纸
	CAT_WallPaper = 12,
	// 墙漆
	CAT_WallPaint = 13,
	//灯槽
	CAT_LampSlot = 14,
	//顶角线
	CAT_CrownMoulding = 15,
	//造型线
	CAT_MoldingLine = 16,
	//踢脚线
	CAT_FloorKickingLine = 17,
	//PAK模型
	CAT_PAKModel = 18,
	//垭口
	CAT_Pass = 19,
	//扣条
	CAT_Buckle = 20,
	//@欧石楠 2019-4-11 斜铺
	CAT_SlopeContinue = 21
};

// 模型dxf文件的类型
enum EItemDxfType
{
	E_Dxf_Overlook = 0, // 俯视图
	E_Dxf_Front = 1, // 正视图
	E_Dxf_Left = 2, // 左视图
	E_Dxf_Right = 3, // 右视图
};

static TMap<FString, EComponentID> GlobalComponentCodeMap;

static TMap<FString, CategryApplicationType> GlobalHardModelCodeMap;


//FORCEINLINE EContentItemFlag operator|(EContentItemFlag Arg1, EContentItemFlag Arg2)
//{
//	return EContentItemFlag(uint32(Arg1) | uint32(Arg2));
//}
//
//FORCEINLINE EContentItemFlag operator&(EContentItemFlag Arg1, EContentItemFlag Arg2)
//{
//	return EContentItemFlag(uint32(Arg1) & uint32(Arg2));
//}
//
//FORCEINLINE EContentItemFlag operator~(EContentItemFlag Arg)
//{
//	return EContentItemFlag(RF_AllFlags & ~uint32(Arg));
//}
//
//FORCEINLINE void operator&=(EContentItemFlag& Dest, EContentItemFlag Arg)
//{
//	Dest = EContentItemFlag(Dest & Arg);
//}
//FORCEINLINE void operator|=(EContentItemFlag& Dest, EContentItemFlag Arg)
//{
//	Dest = EContentItemFlag(Dest | Arg);
//}

namespace FContentItemSpace
{
	//商品信息结构体
	class FProductObj :public TSharedFromThis<FProductObj>
	{
	public:
		FProductObj() {}
		FProductObj(float _Price, float _PriceDiscount, FString _URL, float _Length, float _Width, float _Height)
			:Price(_Price), PriceDiscount(_PriceDiscount), URL(_URL), Length(_Length), Width(_Width), Height(_Height)
		{}

		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			JsonWriter->WriteValue("price", FString::Printf(TEXT("%.1f"), Price));
			JsonWriter->WriteValue("priceDiscount", FString::Printf(TEXT("%.1f"), PriceDiscount));
			JsonWriter->WriteValue("url", URL);
			JsonWriter->WriteValue("model", modelType);
			JsonWriter->WriteValue("length", FString::Printf(TEXT("%.1f"), Length));
			JsonWriter->WriteValue("width", FString::Printf(TEXT("%.1f"), Width));
			JsonWriter->WriteValue("height", FString::Printf(TEXT("%.1f"), Height));



		}

		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			Price = InJsonData->GetNumberField("price");
			double reuslt;
			if (InJsonData->TryGetNumberField("priceDiscount", reuslt))
			{
				PriceDiscount = reuslt;
			}
			else
				PriceDiscount = 1;
			URL = InJsonData->GetStringField("url");
			Length = InJsonData->GetNumberField("length");
			modelType = InJsonData->GetStringField("model");
			Width = InJsonData->GetNumberField("width");
			Height = InJsonData->GetNumberField("height");
		}
		float Price;//价格
		float purchasePrice = 0.0f;//采购价格
		FString unit;//单位
		FString brandName;//品牌
		FString modelType;//型号
		FString introduce;//备注信息

		float PriceDiscount;
		FString URL;

		float Length = 0;
		float Width = 0;
		float Height = 0;

	};
	/**
	* 水电商品信息
	*/
	class FHydropowerProductObj : public FProductObj
	{
	public:
		float Radius = 20;
	public:
		FHydropowerProductObj() {}
		FHydropowerProductObj(float _Price, float _PriceDiscount, FString _URL, float _Length, float _Width, float _Height, float InRadius)
			:FProductObj(_Price, PriceDiscount, _URL, _Length, _Width, _Height), Radius(InRadius)
		{
		}

		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			FProductObj::SerializeToJson(JsonWriter);
			JsonWriter->WriteValue("radius", FString::Printf(TEXT("%.1f"), Radius));
		}

		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			FProductObj::Deserialization(InJsonData);
			Radius = InJsonData->GetNumberField("radius");
		}

	};

	//资源信息结构体
	class FResObj : public TSharedFromThis<FResObj>
	{
	public:
		FResObj() {}
		FResObj(FString _FileName, FString _FilePath, FString _FileURL, FString _FileMD5, EResourceType _ResourceType, PlacePosition _placePosition = { false })
			: FileName(_FileName), FilePath(_FilePath), FileURL(_FileURL), FileMD5(_FileMD5), ResourceType(_ResourceType), placePosition(_placePosition)
		{}
		virtual ~FResObj() {}

		virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			JsonWriter->WriteValue("fileName", *FileName);
			JsonWriter->WriteValue("filePath", *FilePath);
			JsonWriter->WriteValue("fileURL", *FileURL);
			JsonWriter->WriteValue("fileMD5", *FileMD5);
			JsonWriter->WriteValue("version", FString::Printf(TEXT("%d"), Version));
			JsonWriter->WriteValue("fileState", FString::Printf(TEXT("%d"), (int32)FileState));
			JsonWriter->WriteValue("resourceType", FString::Printf(TEXT("%d"), (int32)ResourceType));
		}

		virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			FileName = InJsonData->GetStringField("fileName");
			FilePath = InJsonData->GetStringField("filePath");
			FileURL = InJsonData->GetStringField("fileURL");
			FileMD5 = InJsonData->GetStringField("fileMD5");
			Version = InJsonData->GetIntegerField("version");
			FileState = (EFileExistenceState)(InJsonData->GetIntegerField("fileState"));
			ResourceType = (EResourceType)(InJsonData->GetIntegerField("resourceType"));
		}

		FString FileName;
		FString FilePath;
		FString FileURL;
		FString FileMD5;
		int32 Version;
		EFileExistenceState FileState;

		////存放构件信息
		//TSharedPtr<FContentItemSpace::FComponentRes> ComponentObj;

		//资源类型，用来对应的解析ResObj数据
		EResourceType ResourceType;
		//放置位置
		PlacePosition placePosition;
	};

	//模型信息
	class FModelRes : public FResObj
	{
	public:
		FModelRes(FString FileName,
			FString FilePath,
			FString FileURL,
			FString FileMD5,
			EResourceType _ResourceType,
			FString _MaterialParameter,
			FString _LightParameter,
			PlacePosition _placePosition = { false })
			: FResObj(FileName, FilePath, FileURL, FileMD5, _ResourceType, _placePosition)
			, MaterialParameter(_MaterialParameter)
			, LightParameter(_LightParameter)
		{}
		FModelRes()
		{
		}
		FString MaterialParameter;
		FString LightParameter;



		virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			FResObj::SerializeToJson(JsonWriter);
			JsonWriter->WriteValue("placeposition_floor", placePosition.bFloor);
			JsonWriter->WriteValue("placeposition_wall", placePosition.bWall);
			JsonWriter->WriteValue("placeposition_ceiling", placePosition.bCeiling);
			JsonWriter->WriteValue("placeposition_mesa", placePosition.bMesa);

			JsonWriter->WriteValue("MaterialParameter", *MaterialParameter);
			JsonWriter->WriteValue("LightParameter", *LightParameter);

		}

		virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			FResObj::Deserialization(InJsonData);
			placePosition.bFloor = InJsonData->GetBoolField("placeposition_floor");
			placePosition.bWall = InJsonData->GetBoolField("placeposition_wall");
			placePosition.bCeiling = InJsonData->GetBoolField("placeposition_ceiling");
			placePosition.bMesa = InJsonData->GetBoolField("placeposition_mesa");

			MaterialParameter = InJsonData->GetStringField("MaterialParameter");
			LightParameter = InJsonData->GetStringField("LightParameter");
		}
	};
	/**
	* 构件信息
	*/
	class FComponentRes : public FResObj
	{
	public:
		FComponentRes()
			: FResObj()
			, TypeID(0)
			, ComponentID(0)
		{}
		FComponentRes(FString FileName, FString FilePath, FString FileURL, FString FileMD5, EResourceType _resourceType, uint32 InTypeID, uint32 InID = 0)
			: FResObj(FileName, FilePath, FileURL, FileMD5, _resourceType)
			, TypeID(InTypeID), ComponentID(InID)
		{}

		virtual ~FComponentRes() {}

		virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			FResObj::SerializeToJson(JsonWriter);

			JsonWriter->WriteValue("typeID", FString::Printf(TEXT("%d"), TypeID));
			JsonWriter->WriteValue("componentID", FString::Printf(TEXT("%d"), ComponentID));
			JsonWriter->WriteValue(TEXT("SwitchCoupletNum"), SwitchCoupletNum);
			JsonWriter->WriteArrayStart("altitudes");
			for (auto& It : Altitudes)
			{
				JsonWriter->WriteValue(FString::Printf(TEXT("%d"), It));
			}
			JsonWriter->WriteArrayEnd();
		}

		virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			FResObj::Deserialization(InJsonData);

			TypeID = InJsonData->GetIntegerField("typeID");
			ComponentID = InJsonData->GetIntegerField("componentID");
			SwitchCoupletNum = InJsonData->GetIntegerField(TEXT("SwitchCoupletNum"));
			TArray<FString> AltitudesArray;
			InJsonData->TryGetStringArrayField("altitudes", AltitudesArray);
			for (auto& It : AltitudesArray)
			{
				Altitudes.Add(FCString::Atoi(*It));
			}
		}

		uint32 TypeID = 0;
		uint32 ComponentID = 0;
		float ProductLength = 0;
		float ProductWidth = 0;
		float ProductHeight = 0;

		// 其他图例信息
		TMap<EItemDxfType, TSharedPtr<FResObj>> MapResObj;

		int32 SwitchCoupletNum = -1;// 点位的按键数

		TArray<int32> Altitudes;
	};

	/**
	 * 项目信息
	 */
	class FProjectRes : public FResObj
	{
	public:
		FProjectRes()
			: FResObj("", "", "", "", EResourceType::Project)
		{}

		/**	套餐编号*/
		int32 SetMealId = -1;

		/**	套餐名称*/
		FString SetMealName;

		/** 合同编号 */
		FString Code;

		/** 业主姓名 */
		FString CustomerName;

		/** 业主联系电话 */
		FString CustomerPhone;

        /** 小区 */
		FString Villages;

        /** 详细地址 */
        FString DetailAddress;

		/** 省Name */
		FString ProvinceName;

		/** 市Name */
		FString CityName;

		/** 区Name */
		FString AreaName;
	};

	/**
	 * 方案信息
	 */
	class FArmyPlanRes : public FResObj
	{
	public:
        FArmyPlanRes()
            : FResObj("", "", "", "", EResourceType::Plan)
        {}

        /** 方案名 */
        FString Name;

		/** 卧室数量 */
		int32 BedroomNum = 0;

		/** 客厅数量 */
		int32 LivingRoomNum = 0;

		/** 厨房数量 */
		int32 KitchenNum = 0;

		/** 卫生间数量 */
		int32 BathroomNum = 0;

		/** 户型面积 */
		double FloorArea = 0.f;
	};

    class FArmyHomeRes : public FResObj
    {
    public:
        FArmyHomeRes()
            : FResObj("", "", "", "", EResourceType::Home)
        {}

        /** 当前户型最大空间Id */
        int32 MaxVrSpaceId = -1;

        /** @欧石楠 底图Url */
        FString FacsimileUrl;
    };

	class FArmyHardModeRes;
	/**
	* 硬装模型信息
	*/
	class FArmyHardModeRes : public FModelRes
	{
	public:
		FArmyHardModeRes(
			const FString _FileName,
			const FString _FilePath,
			const FString _FileURL,
			const FString _FileMD5,
			const EResourceType _ResourceType,
			const FString _MaterialParameter = "",
			const FString _LightParameter = "",
			const PlacePosition _placePosition = { false },
			const int32 _DefaultPavingMethod = 0,
			//const TArray<TSharedPtr<FArmyKeyValue> > _PavingMethodArr = NULL,
			const CategryApplicationType _ResourceCategryType = CategryApplicationType::CAT_None)
			: FModelRes(_FileName, _FilePath, _FileURL, _FileMD5, _ResourceType, _MaterialParameter, _LightParameter, _placePosition)
			, DefaultPavingMethod(_DefaultPavingMethod)
			//,PavingMethodArr(_PavingMethodArr)
			, ResourceCategryType(_ResourceCategryType)
		{}
		FArmyHardModeRes()
		{}

		//本商品支持的铺法列表
		TArray<TSharedPtr<FArmyKeyValue> > PavingMethodArr;
		//默认铺法
		int32 DefaultPavingMethod;
		//@add by weixiaokun 
		// 默认用了几张贴图,
		int32 UseTextureNum = 1;
		//资源所在类目的最底层枚举，如连续直铺、水刀拼花、墙漆
		CategryApplicationType ResourceCategryType;

		virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			FModelRes::SerializeToJson(JsonWriter);
			JsonWriter->WriteValue("ResourceCategryType", (int32)ResourceCategryType);
			JsonWriter->WriteValue("UseTextureNum", UseTextureNum);
			JsonWriter->WriteValue("DefaultPavingMethod", FString::Printf(TEXT("%d"), DefaultPavingMethod));
			JsonWriter->WriteArrayStart("PavingMethodArr");
			for (int32 j = 0; j < PavingMethodArr.Num(); j++)
			{
				JsonWriter->WriteObjectStart();
				JsonWriter->WriteValue("PavingMethod", *PavingMethodArr[j]->ToString());
				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayEnd();
		}

		virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			FModelRes::Deserialization(InJsonData);
			ResourceCategryType = CategryApplicationType(InJsonData->GetIntegerField("ResourceCategryType"));

			DefaultPavingMethod = InJsonData->GetIntegerField("DefaultPavingMethod");
			UseTextureNum = InJsonData->GetIntegerField("UseTextureNum");
			const TArray<TSharedPtr<FJsonValue> > JPavingMethod = InJsonData->GetArrayField("PavingMethodArr");
			for (auto & PavingMethodIt : JPavingMethod)
			{
				TSharedPtr<FJsonObject> ItPavingMethodIt = PavingMethodIt->AsObject();
				if (ItPavingMethodIt.IsValid())
				{
					TSharedPtr<FArmyKeyValue> PavingMethod = MakeShareable(new FArmyKeyValue());
					FString PavingMethodStr = ItPavingMethodIt->GetStringField("PavingMethod");
					PavingMethod->InitFromString(*PavingMethodStr);
					PavingMethodArr.Add(PavingMethod);
				}
			}

		}
	};

	/**
	* 硬装产品属性信息
	*/
	class FArmyHardModeProduct : public FProductObj
	{
		//后续硬装的属性信息在这里添加，暂时为空
		//TODO
	};
	/**
	* 水电信息
	*/
	class FArmyPipeRes : public FModelRes
	{
	public:
		FArmyPipeRes(
			const FString& FileName,
			const FString& FilePath,
			const FString& FileURL,
			const FString& FileMD5,
			const EResourceType _resourcetype,
			const FString& MaterialParameter,
			const FString& LightParameter,
			const FString& InClass,
			const FString& InName,
			const PlacePosition _placePosition = { false }
		) : FModelRes(FileName, FilePath, FileURL, FileMD5, _resourcetype, MaterialParameter, LightParameter, _placePosition),
			ClassName(InClass),
			Raduis(0.f),
			Name(InName)
		{
		}

		virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			FModelRes::SerializeToJson(JsonWriter);
			JsonWriter->WriteValue("Categry0Type", Categry0Type);
			JsonWriter->WriteValue("ClassType", ClassType);
			JsonWriter->WriteValue("ObjectType", ObjectType);
			JsonWriter->WriteValue("ClassName", ClassName);
			JsonWriter->WriteValue("Name", Name);
			JsonWriter->WriteValue("FileThumbnailURL", FileThumbnailURL);
			JsonWriter->WriteValue("ID", ID);

			JsonWriter->WriteArrayStart("Altitudes");
			for (int32 j = 0; j < Altitudes.Num(); j++)
			{
				JsonWriter->WriteObjectStart();
				JsonWriter->WriteValue("Altitude", Altitudes[j]);
				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteValue("Length", Length);
			JsonWriter->WriteValue("Width", Width);
			JsonWriter->WriteValue("Height", Height);
			JsonWriter->WriteValue("ComponentID", ComponentID);
			JsonWriter->WriteValue("Color", Color.ToString());
			JsonWriter->WriteValue("RefromColor", RefromColor.ToString());
			JsonWriter->WriteValue("PointColor", PointColor.ToString());
			JsonWriter->WriteValue("PointReformColor", PointReformColor.ToString());
			JsonWriter->WriteValue("Raduis", Raduis);
		}

		virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			FModelRes::Deserialization(InJsonData);

			Categry0Type = InJsonData->GetIntegerField("Categry0Type");
			ClassType = InJsonData->GetIntegerField("ClassType");
			ObjectType = InJsonData->GetIntegerField("ObjectType");
			ClassName = InJsonData->GetStringField("ClassName");
			Name = InJsonData->GetStringField("Name");
			FileThumbnailURL = InJsonData->GetStringField("FileThumbnailURL");
			ID = InJsonData->GetIntegerField("ID");

			const TArray<TSharedPtr<FJsonValue> > AltArr = InJsonData->GetArrayField("Altitudes");
			for (auto & AltIT : AltArr)
			{
				TSharedPtr<FJsonObject> Alt = AltIT->AsObject();
				if (Alt.IsValid())
				{
					float Altitude = Alt->GetNumberField("Altitude");
					Altitudes.Add(Altitude);
				}
			}

			Length = InJsonData->GetIntegerField("Length");
			Width = InJsonData->GetIntegerField("Width");
			Height = InJsonData->GetIntegerField("Height");
			Color.InitFromString(InJsonData->GetStringField("Color"));
			RefromColor.InitFromString(InJsonData->GetStringField("RefromColor"));
			PointColor.InitFromString(InJsonData->GetStringField("PointColor"));
			PointReformColor.InitFromString(InJsonData->GetStringField("PointReformColor"));
			Raduis = InJsonData->GetNumberField("Raduis");
		}

	public:
		int32 Categry0Type;//1级菜单
		int32 ClassType; // 资源类型
		int32 ObjectType; // 用途
		FString ClassName; //类型名称
		FString Name; //名字
		FString FileThumbnailURL;//缩略图
		int32 ID; //商品ID

		TArray<float> Altitudes; // 离地高度
								 /** 素材长度 */
		int32 Length;
		/** 素材宽度*/
		int32 Width;
		/** 素材高度 */
		int32 Height;
		int32 ComponentID;//构件ID
		FColor Color; //管子颜色
		FColor RefromColor;//断电改颜色
		FColor PointColor;//管子节点颜色
		FColor PointReformColor;//管子节点断电改颜色
		float  Raduis=20.0f;// 管径 毫米    //@欧石楠 设置了初值
	};

	/**
	* 大师方案信息
	*/
	class FArmyGreatPlanRes : public FResObj
	{
	public:
		FArmyGreatPlanRes() {}

		FArmyGreatPlanRes(
			const FString& FileName,
			const FString& FilePath,
			const FString& FileURL,
			const FString& FileMD5,
			EResourceType _ResourceType,
			int32 _AreaRangeId,
			FString _AreaRangeName,
			int32 _BathroomCount,
			int32 _BedroomCount,
			FString _Description,
			TArray<FString> _DesignPictureList,
			int32 _DesignStyleId,
			FString _DesignStyleName,
			FString _DesignerAvatar,
			int32 _DesignerId,
			FString _DesignerName,
			FString _GmtCreate,
			FString _GmtModified,
			int32 _ID,
			int32 _KitchenCount,
			int32 _LivingRoomCount,
			FString _Name,
			FString _PanoUrl,
			FString _ThumbnailUrl,
			TArray<int32> _RoomIdList
		)
			: FResObj(FileName, FilePath, FileURL, FileMD5, _ResourceType)
			, AreaRangeId(_AreaRangeId)
			, AreaRangeName(_AreaRangeName)
			, BathroomCount(_BathroomCount)
			, BedroomCount(_BedroomCount)
			, Description(_Description)
			, DesignPictureList(_DesignPictureList)
			, DesignStyleId(_DesignStyleId)
			, DesignStyleName(_DesignStyleName)
			, DesignerAvatar(_DesignerAvatar)
			, DesignerId(_DesignerId)
			, DesignerName(_DesignerName)
			, GmtCreate(_GmtCreate)
			, GmtModified(_GmtModified)
			, ID(_ID)
			, KitchenCount(_KitchenCount)
			, LivingRoomCount(_LivingRoomCount)
			, Name(_Name)
			, PanoUrl(_PanoUrl)
			, ThumbnailUrl(_ThumbnailUrl)
			, RoomIdList(_RoomIdList)
		{}
		// RoomID List
		TArray<int32> RoomIdList;
		//面积范围ID
		int32 AreaRangeId;
		//面积范围名称
		FString AreaRangeName;
		//卫生间数量
		int32 BathroomCount;
		//卧室数量
		int32 BedroomCount; 
		//方案描述
		FString Description;
		//效果图列表
		TArray<FString> DesignPictureList;
		//设计风格ID
		int32 DesignStyleId;
		//设计风格名称
		FString DesignStyleName;
		//设计师头像
		FString DesignerAvatar;
		//设计师ID
		int32 DesignerId;
		//设计师头像
		FString DesignerName;
		//创建时间
		FString GmtCreate;
		//最后修改时间
		FString GmtModified;
		//智能方案ID
		int32 ID;
		//厨房数量
		int32 KitchenCount;
		//客厅数量
		int32 LivingRoomCount;
		//智能方案名称
		FString Name;
		//全景图地址
		FString PanoUrl;
		//缩略图地址
		FString ThumbnailUrl;

		//颜值包
		FString DesignPackageStr;

		////设计名称
		//FString Name;
		////设计风格
		//int32 DesignStyle;
		////房屋类型
		//int32 HouseType;
		////房屋面积
		//float Area;
		////面积类型
		//int32 AreaType;
		////全景图
		//FString PanoUrl;
		////设计师ID
		//int32 DesignerID;
		////设计师名字
		//FString DesignerName;
		////设计描述
		//FString Description;
		////设计图片列表
		//TArray<FString> DesignPictureList;
	};

	//标准内容单元
	class FContentItem :public TSharedFromThis<FContentItem>
	{
	public:
		FContentItem(int32 InID)
		{
			ItemFlag = EContentItemFlag::PF_NoFlags;
			ID = InID;
		}

		FContentItem()
		{
			ItemFlag = EContentItemFlag::PF_NoFlags;
			ID = -1;
		}

		~FContentItem()
		{

		}

		FContentItem(
			EResourceType _ResourceType,
			int32 _ID,
			FString _Name,
			FString _ThumbnailURL,
			TArray<TSharedPtr<FResObj> > _ResObjArr,
			bool _isSale,
			TSharedPtr<FProductObj> _ProObj = NULL,
			EContentItemFlag _ItemFlag = EContentItemFlag::PF_NoFlags
		)
			: ResourceType(_ResourceType), ID(_ID), Name(_Name), ThumbnailURL(_ThumbnailURL), ResObjArr(_ResObjArr), ProObj(_ProObj), bIsSale(_isSale)/*,ComponentObj(_ComponentObj)*/
		{
			ItemFlag = EContentItemFlag::PF_NoFlags;
			ItemFlag |= _ItemFlag;
		}

		FContentItem(
			EResourceType _ResourceType,
			int32 _ID,
			FString _Name,
			FString _ThumbnailURL,
			TArray<TSharedPtr<FResObj> > _ResObjArr,
			TSharedPtr<FProductObj> _ProObj = NULL,
			EContentItemFlag _ItemFlag = EContentItemFlag::PF_NoFlags
		)
			: ResourceType(_ResourceType), ID(_ID), Name(_Name), ThumbnailURL(_ThumbnailURL), ResObjArr(_ResObjArr), ProObj(_ProObj), bIsSale(false)
		{
			ItemFlag = EContentItemFlag::PF_NoFlags;
			ItemFlag |= _ItemFlag;
		}

		/** 根据不同的资源类型创建默认的ResObj */
		TSharedPtr<FResObj> CreateDefaultResObj(EResourceType InResourceType)
		{
			/* @欧石楠 材质参数*/
			if (InResourceType == EResourceType::Texture || InResourceType == EResourceType::Material)
			{
				return MakeShareable(new FArmyHardModeRes());
			}
			else if (InResourceType == EResourceType::ComponentDXF)
			{
				return MakeShareable(new FComponentRes("", "", "", "", EResourceType::ComponentDXF, 0, 0));
			}
			else if ((InResourceType == EResourceType::HardPAK || InResourceType == EResourceType::MoveableMesh) && ModeIdent == MI_HydropowerMode)
			{
				return MakeShareable(new FArmyPipeRes("", "", "", "", EResourceType::HardPAK, "", "", "", ""));
			}

			return MakeShareable(new FResObj("", "", "", "", EResourceType::None));
		}

		void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
		{
			JsonWriter->WriteValue("bIsSale", bIsSale);
			JsonWriter->WriteValue("id", FString::Printf(TEXT("%d"), ID));
			JsonWriter->WriteValue("name", Name);
			JsonWriter->WriteValue("thumbnailURL", ThumbnailURL);
			JsonWriter->WriteValue("codeStrId", codeStrId);
			JsonWriter->WriteValue("itemFlag", FString::Printf(TEXT("%d"), (int32)ItemFlag));
			JsonWriter->WriteValue("resourceType", FString::Printf(TEXT("%d"), (int32)ResourceType));
			JsonWriter->WriteValue("categryid", FString::Printf(TEXT("%d"), CategryID));
			JsonWriter->WriteValue("aiCode", FString::Printf(TEXT("%d"), (int32)AiCode));
			JsonWriter->WriteValue("modeIdent", (int32)ModeIdent);

			JsonWriter->WriteArrayStart("resObjArr");
			for (auto& it : ResObjArr)
			{
				JsonWriter->WriteObjectStart();
				it->SerializeToJson(JsonWriter);
				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayEnd();

			if (ProObj.IsValid())
			{
				JsonWriter->WriteObjectStart("proObj");
				ProObj->SerializeToJson(JsonWriter);
				JsonWriter->WriteObjectEnd();
			}
		}

		void Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
		{
			bIsSale = InJsonData->GetBoolField("bIsSale");
			ID = InJsonData->GetIntegerField("id");
			Name = InJsonData->GetStringField("name");
			ThumbnailURL = InJsonData->GetStringField("thumbnailURL");
			codeStrId = InJsonData->GetStringField("codeStrId");
			ItemFlag = (InJsonData->GetIntegerField("itemFlag"));
			ResourceType = (EResourceType)(InJsonData->GetIntegerField("resourceType"));
			ModeIdent = (EModeIdent)(InJsonData->GetIntegerField("modeIdent"));
			CategryID = InJsonData->GetIntegerField("categryid");
			int32 TmpCode  = InJsonData->GetIntegerField("aiCode");
			if (TmpCode <= 0)
			{
				AiCode = AI_None;
			}
			else
			{
				AiCode = (EAIComponentCode)TmpCode;
			}

			const TArray<TSharedPtr<FJsonValue> > ResObj = InJsonData->GetArrayField("resObjArr");
			for (auto & it : ResObj)
			{
				TSharedPtr<FJsonObject> itRes = it->AsObject();
				EResourceType temPResourceType = EResourceType::None;
				if (itRes.IsValid())
				{
					temPResourceType = (EResourceType)(itRes->GetIntegerField("resourceType"));
					TSharedPtr<FResObj> tempResObj = CreateDefaultResObj(temPResourceType);
					tempResObj->Deserialization(itRes);
					ResObjArr.Add(tempResObj);
				}
			}
			ProObj = MakeShareable(new FContentItemSpace::FProductObj());
			TSharedPtr<FJsonObject> TempProbj = InJsonData->GetObjectField("proObj");
			ProObj->Deserialization(TempProbj);

		}

		//按资源类型获取资源数据
		TArray<TSharedPtr<FResObj> >& GetResObjByType(EResourceType type)
		{
			TArray<TSharedPtr<FResObj> > resArr;
			for (auto & It : ResObjArr)
			{
				if (It->ResourceType == type)
					resArr.Add(It);
			}
			return resArr;
		}

		//获取除构件外的所有资源
		TArray<TSharedPtr<FResObj> > GetResObjNoComponent()
		{
			TArray<TSharedPtr<FResObj> > resArr;
			for (auto & It : ResObjArr)
			{
				if (It->ResourceType != EResourceType::ComponentDXF)
					resArr.Add(It);
			}
			return resArr;
		}

		//如果存在构件，返回此商品构件
		TSharedPtr<FComponentRes> GetComponent()
		{
			TArray<TSharedPtr<FResObj> > resArr;
			for (auto & It : ResObjArr)
			{
				if (It->ResourceType == EResourceType::ComponentDXF)
				{
					TSharedPtr<FComponentRes> CurrentComponentRes = StaticCastSharedPtr<FComponentRes>(It);
					return CurrentComponentRes;
				}
			}
			return nullptr;
		}

        /** @欧石楠 是否包含资源内容 */
        bool HasResources()
        {
            return GetResObjNoComponent().Num() > 0;
        }

		/**@欧石楠是否是商品的标识*/
		bool bIsSale;
		//资源唯一ID
		int32 ID;
		//显示名称
		FString Name;
		//显示缩略图地址
		FString ThumbnailURL;
		//该资源的属性
		uint32 ItemFlag;
		//资源类型，用来对应的解析ResObj数据
		EResourceType ResourceType;

		//存放资源数据
		TArray<TSharedPtr<FResObj> > ResObjArr;
		//存放对应的商品信息
		TSharedPtr<FProductObj> ProObj;
		//存放构件信息
		//TSharedPtr<FContentItemSpace::FComponentRes> ComponentObj;

		//存放商品所在类目ID
		int32 CategryID;

		FString codeStrId;
		//EModeIdent
		EModeIdent ModeIdent;
		//存放商品属于的aicode
		EAIComponentCode AiCode;
	};


}

typedef TSharedPtr<class FContentItemSpace::FContentItem> FContentItemPtr;

/*
*	类目层级数据
*/
struct FCategoryContentItem
{
	FCategoryContentItem() {}
	FCategoryContentItem(FString _Name, int32 _Key, int32 _Value)
		: Name(_Name), Key(_Key), Value(_Value)
	{}

	FCategoryContentItem(FString _Name, int32 _leval, int32 _id, int32 _parentId) :
		Name(_Name), Key(_leval), Value(_id), ParentMenuID(_parentId)
	{

	}
	~FCategoryContentItem() {}

	FString Name;
	// 目录层级
	int32 Key;
	// 菜单id
	int32 Value;
	// 父级菜单ID
	int32 ParentMenuID;
	TArray<FCategoryContentItem> CategoryList;
};
struct FObjectThirdItem
{
	FString DirectoryName;
	TArray<int32> CategoryList;
};
struct FObjectGroupContentItem
{
	FObjectGroupContentItem() {}
	~FObjectGroupContentItem() {}

	FString ModelName;
	int32 ModelID;
	int32 VisibleInOtherModel;

	FString ItemPath;
	FObjectThirdItem ThirdItem;
	TArray<int32> CategoryList;
};

//@ 存放构件以及关联的模型属性信息
class ARMYUSER_API FFurnitureProperty
{
public:
	FFurnitureProperty();
	FFurnitureProperty(EResourceType _Type);
	FFurnitureProperty(TSharedPtr<FFurnitureProperty> Copy);
	~FFurnitureProperty() {}
	//设置和获取半径
	bool SetRadius(float _Radius);
	float GetRadius();

	//设置和获取长度
	bool SetLength(float _Length);
	float GetLength();

	//设置和获取宽度
	bool SetWidth(float _Width);
	float GetWidth();

	//设置和获取高度
	bool SetHeight(float _Height);
	float GetHeight();

	//设置和获取可选离地高度
	bool SetOptionsAltitude(const TArray<float> _Altitude);
	TArray<float> GetOptionsAltitude();

	//设置和获取离地高度
	void SetAltitude(float _Altitude);
	float GetAltitude();

	//设置和获取构件的路径
	bool SetComponentPath(FString _ComponentPath);
	FString GetComponentPath();

	//设置和获取模型的路径
	bool SetModelPath(FString _ModelPath);
	FString GetModelPath();

	//设置和获取构件的类型
	bool SetType(EResourceType _Type);
	EResourceType GetType();

	//设置和获取构件的类目（此处类目主要分开关、插座、强电、弱电、水路）
	bool SetBelongClass(int32 _Type);
	int32 GetBelongClass();

	//设置和获取构件关联模型性质(ture表示为pak模型，否则为数字化建模)
	bool GetbIsPakModel() const { return bIsPakModel; }
	void SetbIsPakModel(bool InbIsPakModel) { bIsPakModel = InbIsPakModel; }

	//设置和获取模型的ObjectType
	bool SetObjectType(int32 _Type);
	int32 GetObjectType();

	//设置和获取模型的优化参数
	bool SetOptimizeParam(FString _ParamFile);
	// @刘克祥 获得模型参数 0:材质参数 1：灯光参数
	FString GetOptimizeParam(int32 ParamType =0);
	//int32 GetOptimizeParam();

	//设置资源的模式类型，如立面、水电
	void SetModeType(EModeIdent _type);
	EModeIdent GetModelType();

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);
	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	// 添加构件的图例
	void AddItemLegend(EItemDxfType InType, TSharedPtr<class FArmyFurniture> InFurniture);
	// 获得构件的图例
	TSharedPtr<class FArmyFurniture>  GetItemLegend(EItemDxfType InType);
	// 是否存在图例
	bool IsExitItemLegend(EItemDxfType InType);
	TMap<EItemDxfType, TSharedPtr<class FArmyFurniture>> GetLegendMap() { return LegendMap; }
private:
	//根据系统时间生成contentItem唯一ID
	void CalContentItemID();
private:
	bool bIsPakModel = true;  //true表示为PAK模型，否则为数字化模型(如主下水管)
	float Altitude = 0; //离地高度
	// 各个视图的构件
	TMap<EItemDxfType, TSharedPtr<class FArmyFurniture>> LegendMap;

public:
	FContentItemPtr FurContentItem;
	//后期可扩展其他属性
	
	
};
DECLARE_DELEGATE_ThreeParams(FArmyFComponentResDelegate, TSharedPtr<FContentItemSpace::FComponentRes>, AActor*, EModeIdent);
//@欧石楠 用于在没有构件的情况下生成Armyobj
DECLARE_DELEGATE_FourParams(FArmyFComponentDelegate, FContentItemPtr, AActor*, TSharedPtr<FArmyObject>, EModeIdent);