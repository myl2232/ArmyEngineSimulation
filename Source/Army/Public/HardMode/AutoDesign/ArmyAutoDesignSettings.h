/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRAutoDesignComponentBase.h
* @Description 智能设计构件库
*
* @Author 纪仁泽
* @Date 2019年1月14日
* @Version 1.0
*/

#pragma once

enum ERoomType
{
	RT_NoName = 1,                // 未命名-AI未命名
	RT_Bedroom_Master= 2,         // 主卧-AI主卧
	RT_Bedroom_Second = 3,        // 次卧-AI次卧
	RT_ChildrenRoom = 4,		  // 儿童房-AI次卧
	RT_StudyRoom = 5,			  // 书房-AI书房
	RT_LivingRoom = 6,			  // 客厅-AI客厅
	RT_StorageRoom = 7,			  // 储物间-AI其他
	RT_Bathroom = 8,			  // 卫生间-AI卫生间
	RT_Balcony = 9,				  // 阳台-AI阳台
	RT_Kitchen = 10,			  // 厨房-AI厨房
	RT_DiningRoom =11,			  // 餐厅-AI餐厅
	RT_Hallway = 12,			  // 玄关-AI玄关
	RT_WashingRoom = 13,		  // 洗衣间-AI其他
	RT_Corridor = 14,			  // 走廊-AI走廊
	RT_Aisle = 15,				  // 过道-AI走廊
	RT_Bedroom_Quest = 16,		  // 客卧-AI次卧
	RT_Bathroom_Mester = 17,	  // 主卫生间-AI卫生间
	RT_Bathroom_Quest = 18,	      // 客卫生间-AI卫生间
	RT_Balcony_Bedroom = 19,      // 卧室阳台-AI阳台
	RT_Balcony_After = 20,        // 后阳台-AI阳台
	RT_LivingDiningRoom = 24,     // 客餐厅-AI客厅
	RT_ClosedKitchen = 26,		  // 封闭厨房-AI厨房
	RT_OpenKitchen = 27,		  // 开放厨房
	RT_Balcony_Habit = 28,		  // 居室阳台-AI阳台
	RT_BathroomDryArea = 29,	  // 卫生间干区-AI其他
	RT_Balcony_Kitchen = 30,	  // 厨房阳台
	RT_Other = 99,	  // 其他
};


enum EAIComponentCode
{
	/**
	* S Standard   标准
	* C Customized 定制
	* L Left       左
	* R Right      右
	*/


	AI_None = -1,          // 默认

	// 中台软装布局
	AI_SingleBed = 1001,          // 单人床-双人床
 	AI_DoubleBed = 1002,          // 双人床-双人床
	AI_BedsideTable_L = 1003,     // 左床头柜-左床头柜
	AI_BedsideTable_R = 1004,     // 右床头柜-左床头柜
	AI_Wardrobe_C = 1005,         // 定制衣柜-无
	AI_Wardrobe = 1006,           // 衣柜
	AI_Desk = 1007,               // 书桌
	AI_Dresser = 1008,            // 梳妆台
	AI_BedChair = 1009,           // 床尾凳
	AI_TVBench = 1010,            // 电视柜
	AI_Bookcase_C = 1011,         // 定制书柜-无
	AI_Bookcase_S = 1012,         // 标准书柜
	AI_BalconyCabinet_C = 1013,   // 定制阳台柜-无
	AI_BalconyCabinet_S = 1014,   // 标准阳台柜
	AI_LeisureChair = 1015,       // 休闲椅
	AI_Sofa_Master = 1016,        // 主位沙发
	AI_SofaAuxiliary_L = 1017,    // 辅位沙发左
	AI_SofaAuxiliary_R = 1018,    // 辅位沙发右
	AI_TeaTable = 1019,           // 茶几
	AI_SideTable = 1020,          // 边几
	AI_Sprinkler = 1021,          // 花洒
	AI_ShowerRoom = 1022,         // 淋浴房
	AI_Bathtub = 1023,            // 浴缸 
	AI_BathroomArk = 1024,        // 浴室柜（有脚）
	AI_Closestool = 1025,         // 马桶（坐便器）
	AI_ShowerRoomFirst = 1026,    // 一字型淋浴房
	AI_EnterArk_C = 1027,         // 入户柜
	AI_DiningTable = 1028,        // 餐桌
	AI_Sideboard = 1029,          // 餐边柜
	AI_WashingMachine = 1030,     // 洗衣机
	AI_Refrigerator = 1031,       // 冰箱
	AI_Stove = 1032,              // 炉灶
	AI_Sink = 1033,               // 水槽
	AI_SquattingPan = 1034,       // 马桶（蹲便器）
	AI_DiamondShowerRoom = 1035,  // 钻石淋浴房
	AI_BookChair = 1036,          // 书椅
	AI_DiningChair =1037,         // 餐椅
	AI_DressingStool = 1038,      // 梳妆凳
	AI_BathroomArk_Wall = 1039,   // 浴室柜（无脚）



	// 配饰模型
	AI_Carpet = 3001,		      // 地毯
	AI_CurtainStd = 3002,		  // 标准窗帘
	AI_Blind = 3003,		      // 百叶帘
	AI_Mural = 3004,              // 壁画
	AI_LedLight = 3005,           // Led灯
	AI_LampFloorDown = 3006,      // 落地灯
	AI_LampTable = 3007,          // 台灯
	AI_LampCeiling = 3008,        // 吸顶灯
	AI_LampTube = 3009,           // 筒灯
	AI_LampReflector = 3010,      // 射灯
	AI_LampWall = 3011,           // 壁灯
	AI_LampFloor = 3012,          // 地灯
	AI_LampDrop = 3013,           // 吊灯
	AI_LampTubeFog = 3014,        // 防雾筒灯
	AI_CBase_LampTrack = 3015,    // 轨道灯
	AI_Titanic = 3016,            // 挂画
	AI_GreenPlants = 3017,        // 绿植
	AI_DrinkingFountain = 3019,   // 饮水机
	AI_AirConditionedVert = 3020, // 立式空调
	AI_AirConditionedHang = 3021, // 壁挂空调
	AI_TVWall = 3022,             // 壁挂电视
	AI_TVVert = 3018,             // 立式电视
	AI_ClotheslinePole = 3024,    // 晾衣杆
	AI_LampMain = 3025,           // 主灯
	AI_Dinnerware = 3026,           //餐具
	AI_BathTowelHolder = 3027,      //浴巾架
	AI_FaceclothHolder = 3028,      //毛巾架
	AI_ToiletPaperHolder = 3029,    //厕纸架
	AI_CommonAccessory = 3030,      //通用配饰（小配饰）

	// 其他模型
	AI_CabinetsFloor = 5001,      // 地柜
	AI_CabinetsWall = 5002,       // 吊柜
	AI_CabinetsHigh = 5003,       // 高柜
	AI_Hood = 5004,               // 油烟机
	AI_LampBathHeater = 5005,     // 浴霸
	AI_ExhaustFan = 5006,         // 排风扇
	AI_300_600_301_Bath_Heater = 5007,//300x600三合一浴霸-无
	AI_300_300_301_Bath_Heater = 5008,//300x300三合一浴霸-无
	AI_Exhaust_Fan = 5009,        //排气扇
	AI_IntelClosestool = 5010,    //智能马桶
	AI_Electric_Heater = 5011,    //电热水器
	AI_Gas_Heater = 5012,         //燃气热水器

	// 硬装
	AI_Material = 7001,           // 墙顶地材质
	AI_Lamp_Trough = 7002,        // 灯槽
	AI_Skirting = 7003,           // 踢脚线
	AI_Ceiling_Line = 7004,       // 顶角线
	AI_Window_Stone = 7001,       // 窗台石-7005
	AI_Door_Stone = 7001,         // 过门石-7006

};


//自动设计点位默认模型ID
enum EAutoDesignModelID
{
	ADM_None = 0,
	/** 开关 */
	ADM_Switch1O1_Point /*= 164*/,//单联单控开关
	ADM_Switch2O1_Point /*= 165*/,//双联单控开关
	ADM_Switch3O1_Point /*= 166*/,//三联单控开关
	ADM_Switch4O1_Point /*= 154*/,//四联单控开关
	ADM_Switch1O2_Point /*= 169*/,//单联双控开关
	ADM_Switch2O2_Point /*= 168*/,//双联双控开关
	ADM_Switch3O2_Point/* = 167*/,//三联双控开关
	ADM_Switch4O2_Point /*= 153*/,//四联双控
	ADM_Switch_BathHeater /*= 149*/,//浴霸开关

	/** 插座 */
	ADM_Socket_Three_Point_16A,//16A三孔插座
	ADM_Socket_Three_Point_10A,//10A三孔插座
	ADM_Socket_Five_Point_10A,//10A五孔插座
	ADM_Socket_Five_Point_16A,//16A五孔插座
	ADM_Socket_Three_Spatter_Point_16A,//带防溅盒16A三孔插座
	ADM_Socket_Five_Spatter_Point_10A,//带防溅盒10A五孔插座
	//ADM_Socket_Five_Spatter_Point_16A,//带防溅盒16A五孔插座
	//ADM_Socket_IT_TV /*= 150*/,//电脑+电视弱电插座(网络/电视插座)
	//ADM_Socket_Three_Point_With_Switch_16A,//16A三孔插座带开关
	//ADM_Socket_Five_Point_With_Switch_10A,//10A五孔插座带开关
	//ADM_Socket_Five_Point_With_Switch_16A,//16A五孔插座带开关
	ADM_Socket_TP/* = 148*/,//厨房台面联排插座(厨房联排插座)
	ADM_Socket_Living_Couplet/* = 148*/,//客厅联排插座(厨房联排插座)电视联排插座(电视背景墙联排插座)
	ADM_Socket_Net_TV_Couplet /*= 150*/,//网络+电视插座
	ADM_Socket_Computer_Tel_Couplet /*= 150*/,//电脑+电话插座
	ADM_Socket_TV_Tel_Couplet /*= 150*/,//电视+电话插座
	ADM_Socket_Computer /*= 150*/,//电脑插座
	ADM_Socket_TV /*= 150*/,//电视插座
	ADM_Socket_Tel /*= 150*/,//电话插座
	ADM_Socket_Audio /*= 150*/,//音频插座
	ADM_Socket_Five_Point_USB_10A,//10A五孔插座带USB

	/** 水位 */
	ADM_Water_Cold_Water_OutLet,//冷水口
	ADM_Water_Hot_Water_OutLet, //热水口
	ADM_Water_The_Floor_Drain, //普通地漏
	ADM_Water_Floor_Drain, //地漏下水
	ADM_Water_Launching_Of_A_Basin,//水盆下水
	ADM_Water_Closestool_Drain,//马桶下水
	ADM_WashingMachine_Brain, //洗衣机地漏 未对应
	//ADM_tap //洗衣机冷水口所需龙头, 暂无模型 未对应
};

//自动设计点位默认模型ID
enum EAutoDesignTag
{
	EAT_Other = 1022,   // 其他
	EAT_OneFont = 1026, // 一字型
	EAT_BiGua = 3021,
	EAT_LiGui = 3020,
	EAT_GuaBi = 3022,
	EAT_DiZuo = 3018,
	EAT_ZNMaTong = 5010,
	EAT_Dian = 5011,
	EAT_RanQi = 5012,
};


// BIM构件编码
/*enum EComponentCode
{
	CC_None = -1,
	CC_300_300_LED_Light = 205,//300x300集成LED灯
	CC_300_600_LED_Light = 207,//300x600集成led灯
	CC_Wall_Light = 226,//壁灯
	CC_Spot_Light = 225,//射灯
	CC_Ceiling_Light = 266,//吸顶灯
	CC_Floor_Light = 256,//地灯
	CC_Down_Light = 31,//筒灯
	CC_Anti_Fog_Down_Light = 252,//防雾筒灯
	CC_Eight_Chandeliers_Light = 22,//八头吊灯（客厅用）
	CC_Four_Chandeliers_Light = 203,//八头吊灯（餐厅用）
	CC_300_600_301_Bath_Heater = 208,//300x600三合一浴霸
	CC_300_300_301_Bath_Heater = 206,//300x300三合一浴霸
	CC_Exhaust_Fan = 253,//排气扇
	CC_Single_Bed = 243,//单人床
	CC_Double_Bed = 21,//双人床
	CC_Double_Basin_Flume = 163,//双盆水槽
	CC_Single_Basin_Flume = 162,//单盆水槽
	CC_Dresser = 245,//梳妆台
	CC_Double_Sofa = 181,//双人沙发
	CC_Multiplayer_Sofa = 180,//双人沙发
	CC_Corner_Sofa = 234,//转角沙发
	CC_Desk = 202,//书桌
	CC_RCTtangular_Table = 210,//长方形餐桌
	CC_Lampblack_Machine = 215,//油烟机
	CC_ElCTtric_Water_Heater = 249,//电热水器
	CC_Wall_Air_Conditioner = 257,//挂机空调
	CC_Cabinet_Air_Conditioner = 258,//柜机空调
	CC_Single_Open_Refrigerator = 164,//单开电冰箱
	CC_Double_Open_Refrigerator = 247,//双开冰箱
	CC_TV_Cabinet = 176,//电视柜
	CC_Bathroom_Cabinet = 173,//浴室柜
	CC_Bathtub = 172,//浴缸
	CC_Washing_Machine = 171,//洗衣机
	CC_Shower = 167,//花洒
	CC_Closestool = 136,//马桶
	CC_Gas_Water_Heater = 250,//燃气热水器
	CC_Water_Dispenser = 251,//饮水机
	CC_Squatting_Pan = 161,//蹲便器
	CC_Round_Table = 213,//圆形餐桌
	CC_Floor_Lamp = 179,//落地灯
	CC_Track_Light = 254,//轨道灯
	CC_Fishing_Lights = 255,//钓鱼灯
	CC_Round_Bedside_Cabinet = 229,//圆形床头柜
	CC_Rect_Bedside_Cabinet = 194,//矩形床头柜
	CC_Ground_Socket = 220,//地面插座
};*/
