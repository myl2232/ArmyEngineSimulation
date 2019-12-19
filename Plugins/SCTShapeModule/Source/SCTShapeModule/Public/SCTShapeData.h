#pragma once
#include "CoreMinimal.h"

#define RefName_WIDTH TEXT("WIDTH")
#define RefName_DEPTH TEXT("DEPTH")
#define RefName_HEIGHT TEXT("HEIGHT")
#define RefName_POSX TEXT("POSX")
#define RefName_POSY TEXT("POSY")
#define RefName_POSZ TEXT("POSZ")
#define RefName_ROTX TEXT("ROTX")
#define RefName_ROTY TEXT("ROTY")
#define RefName_ROTZ TEXT("ROTZ")

 /** 封边材质类型 */
#define RefName_BOARD_EDGE_MATERIAL_TYPE TEXT("封边")
#define RefName_BaseMaterial TEXT("基材")
#define RefName_CoveredEdgeMaterial TEXT("包边")
#define RefName_Veneer TEXT("铝箔")

/** 活动层板 */
#define  RefName_ActiveBoard TEXT("活动层板")

/** 内胆子对象复制 */
#define RefName_COPY_THRESHOLD TEXT("CopyThreshold")
#define RefName_COPY_COUNT TEXT("CopyCount")

/** 轮廓类型 */
#define RefName_OUTLINE_TYPE TEXT("OutlineType")
/** 轮廓参数 */
#define RefName_OUTLINE_GW TEXT("GW")
#define RefName_OUTLINE_GD TEXT("GD")
#define RefName_OUTLINE_GH TEXT("GH")
#define RefName_OUTLINE_PW TEXT("PW")
#define RefName_OUTLINE_PD TEXT("PD")
#define RefName_OUTLINE_PH TEXT("PH")
#define RefName_OUTLINE_PX TEXT("PX")

#define SpaceBoard_Top            TEXT("顶")
#define SpaceBoard_Bottom         TEXT("底")
#define SpaceBoard_Left           TEXT("左")
#define SpaceBoard_Right          TEXT("右")
#define SpaceBoard_Front          TEXT("前")
#define SpaceBoard_Back           TEXT("后")

#define SpaceBoard_WidthSplit     TEXT("水平分割")
#define SpaceBoard_DepthSplit     TEXT("纵深分割")
#define SpaceBoard_HeighthSplit   TEXT("垂直分割")

#define Board_TopExpand            TEXT("上延")
#define Board_BottomExpand         TEXT("下延")
#define Board_LeftExpand           TEXT("左延")
#define Board_RightExpand          TEXT("右延")
#define Board_FrontExpand          TEXT("前延")
#define Board_BackExpand           TEXT("后延")

#define Board_Contraction          TEXT("内移")

//柜体空间中顶层内胆内缩RetractValue
#define Inside_Retract           TEXT("内缩值")
#define ActiveBoard_Retract      TEXT("活层内缩值")
//底部空间高度
#define BottomSpace_Height       TEXT("下部高度")
#define Space_AddBottomSpace     TEXT("添加底部空间")
#define Space_DeleteBottomSpace  TEXT("删除底部空间")
// 门的显隐设置
#define Door_Visiable             TEXT("门的显隐设置")
#define Door_Top_PlateBoard_Height TEXT("门上垫板高度")
#define Door_Bottom_PlateBoard_Height TEXT("门下垫板高度")
#define Door_Top_Slid_Way_List TEXT("移门滑轮组替换")
#define Door_FirstPosition TEXT("移门首扇门内外")
#define Door_Interleaving_mode TEXT("移门内外交错方式")
#define Door_Top_Down_PlateBoard_Show_Condition TEXT("上下垫板的设置")
#define Door_Available_DoorSheetNum TEXT("可选的移门扇数")

//抽屉动画设置
#define Drawer_Animation             TEXT("抽类动画设置")

//抽屉组外延
#define Door_DrawerGroup_UpExtensionValue TEXT("抽屉组上延")
#define Door_DrawerGroup_DownExtensionValue TEXT("抽屉组下延")

// 掩门设置项
#define Door_SideHungDoor_Door_Type TEXT("掩门类型")
#define Door_SideHungDoor_Door_Material TEXT("掩门材质")
#define Door_SideHungDoor_Handle_Type_List TEXT("掩门拉手类型")
#define Door_SideHungDoor_Top_Cover_Way TEXT("掩门顶位置关系")
#define Door_SideHungDoor_Down_Cover_Way TEXT("掩门底位置关系")
#define Door_SideHungDoor_Left_Cover_Way TEXT("掩门左位置关系")
#define Door_SideHungDoor_Right_Cover_Way TEXT("掩门右位置关系")
#define Door_SideHungDoor_PoseBackSize TEXT("掩门门后缝")
#define Door_SideHungDoor_UpDownGapValue TEXT("掩门上下门缝")
#define Door_SideHungDoor_LeftRigheGapValue TEXT("掩门左右门缝")
#define Door_SideHungDoor_UpExtensionValue TEXT("掩门上延")
#define Door_SideHungDoor_DownExtensionValue TEXT("掩门下延")
#define Door_SideHungDoor_LeftExtensionValue TEXT("掩门左延")
#define Door_SideHungDoor_RightExtensionValue TEXT("掩门右延")
#define Door_SideHungDoor_OpenDirection_Single_Door TEXT("掩门单开门")
#define Door_SideHungDoor_OpenDirection_Hinge_Door TEXT("掩门对开门")
#define Door_SideHungDoor_Left_Door_OpenDirection TEXT("掩门左门开门方向")
#define Door_SideHungDoor_Right_Door_OpenDirection TEXT("掩门右门开门方向")
#define Door_SideHungDoor_Left_Door_HingeList_Change TEXT("掩门左门铰链列表")
#define Door_SideHungDoor_Right_Door_HingList_Change TEXT("掩门右门铰链列表")
#define Door_SideHungDoor_Left_Door_HandleList_Change TEXT("掩门左门拉手列表")
#define Door_SideHungDoor_Right_Door_HandleList_Change TEXT("掩门右门拉手列表")
#define Door_SideHungDoor_Left_Door_Handle_X_List     TEXT("掩门左门拉手X位置可选项")
#define Door_SideHungDoor_Left_Door_Handle_Side_Hanle_List     TEXT("掩门左门封边拉手可选项")
#define Door_SideHungDoor_Left_Door_Hanle_X_Position  TEXT("掩门左门拉手X位置值")
#define Door_SideHungDoor_Left_Door_Handle_Z_List     TEXT("掩门左门拉手Z位置可选项")
#define Door_SideHungDoor_Left_Door_Hanle_Z_Position TEXT("掩门左门拉手Z位置值")
#define Door_SideHungDoor_Left_Door_Hanle_RotationDirection TEXT("掩门左门拉手旋转值")
#define Door_SideHungDoor_Right_Door_Handle_X_List     TEXT("掩门右门拉手X设置列表")
#define Door_SideHungDoor_Right_Door_Hanle_X_Position  TEXT("掩门右门拉手X位置值")
#define Door_SideHungDoor_Right_Door_Handle_Side_Hanle_List     TEXT("掩门右门封边拉手可选项")
#define Door_SideHungDoor_Right_Door_Handle_Z_List     TEXT("掩门右门拉手Z位置可选项")
#define Door_SideHungDoor_Right_Door_Hanle_Z_Position TEXT("掩门右门拉手Z位置值")
#define Door_SideHungDoor_Right_Door_Hanle_RotationDirection TEXT("掩门右门拉手旋转值")
#define Door_SideHungDoor_FrontBoardType TEXT("掩门前封板结构")
#define Door_SideHungDoor_FrontBoardPositionType TEXT("掩门前封板位置类型")
#define Door_SideHungDoor_FrontBoardCornerType TEXT("掩门前封板切角类型")
#define Door_SideHungDoor_FrontBoardBaseMaterialName TEXT("掩门前封板基材名称")
#define Door_SideHungDoor_FrontBoardEdgeBandingName TEXT("掩门前封板封边名称")
#define Door_SideHungDoor_FrontBoardWidth TEXT("掩门前封板宽度")
#define Door_SideHungDoor_FrontBoardDepth TEXT("掩门前封板厚度")
#define Door_SideHungDoor_FrontBoardCabinetRelativeMove TEXT("掩门前封板柜体联动")
#define Door_SideHungDoor_FrontBoardTopExpand TEXT("掩门前封板上延")
#define Door_SideHungDoor_FrontBoardBottomExpand TEXT("掩门前封板下延")
#define Door_SideHungDoor_FrontBoardCorner_Cut_Width TEXT("掩门前封板左上切角宽")
#define Door_SideHungDoor_FrontBoardCorner_Cut_Height TEXT("掩门前封板左上切角高")
#define Door_SideHungDoor_AssistBoardWidth TEXT("掩门帮板宽度")
#define Door_SideHungDoor_AssistBoardDepth TEXT("掩门帮板厚度")
#define Door_SideHungDoor_AssistBoardTopExpand TEXT("掩门帮板上延")
#define Door_SideHungDoor_AssistBoardBottomExpand TEXT("掩门帮板下延")
#define Door_SideHungDoor_AssistBoardLenghtToFrontBoard TEXT("掩门帮板距离前封板距离")
#define Door_SideHungDoor_FrontBoardMaterial TEXT("掩门前封板材质")
#define Door_SideHungDoor_AssistBoardMaterial TEXT("掩门帮板材质")
#define Door_SideHungDoor_AssistBoardEdgeBanding TEXT("掩门帮板封板")
#define Door_SideHungDoor_AssistBoardSubstrate TEXT("掩门帮板基材")
#define Door_SideHungDoor_BoxHanleList TEXT("掩门箱体拉手列表")
#define Door_SideHungDoor_BoxHanle_V_Value TEXT("掩门箱体拉手垂直方向偏移值")
#define Door_SideHungDoor_BoxHanle_Left_Extend_Value TEXT("掩门箱体拉手左延")
#define Door_SideHungDoor_BoxHanle_Right_Extend_Value TEXT("掩门箱体拉手右延")

/** 抽面 */
#define Door_DrawerDoor_HanleType_List TEXT("抽面拉手类型列表")
#define Door_DrawerDoor_BoxHanleList TEXT("抽面箱体拉手列表")
#define Door_DrawerDoor_BoxHanle_V_Value TEXT("抽面箱体拉手垂直方向偏移值")
#define Door_DrawerDoor_Side_Hanle_List TEXT("抽面封边拉手列表")
#define Door_DrawerDoor_Side_Handle_Z_List TEXT("抽面封边拉手Z值可选列表") 

/**  电器饰条 */
#define EmbededElectrical_InsertionalBoard_Material TEXT("电器饰条材质")
#define EmbededElectrical_InsertionalBoard_TopExpand  TEXT("电器饰条上延")
#define EmbededElectrical_InsertionalBoard_DownExpand  TEXT("电器饰条下延")
#define EmbededElectrical_InsertionalBoard_LeftExpand  TEXT("电器饰条左延")
#define EmbededElectrical_InsertionalBoard_RightExpand  TEXT("电器饰条右延")
#define EmbededElectrical_InsertionalBoard_InnerExpand  TEXT("电器饰条内移")
#define EmbededElectrical_Electrical_Z_Value  TEXT("电器饰条中电器Z")
#define EmbededElectrical_LateralBoxHanleLenTo_InsertaionBoard_Z_Value  TEXT("箱体拉手距饰条")

/** 见光板组 */
#define None_Model_VeneeredSideBoard_Substarate_Type_List TEXT("非造型见光板厚度列表")
#define None_Model_VeneeredSideBoard_Substarate_Height TEXT("非造型见光板厚度")


/**
 * 型录类型
 */
enum EShapeType : uint8
{
    ST_None = 0,					//未定义
	ST_Board = 1,					//板件
	ST_Decoration = 2,				//饰品模型
	ST_Accessory = 3,				//五金模型
	ST_Base = 4,					//基础组件
	ST_Inside = 5,				    //内胆
	ST_Unit = 6,					//单元柜
	ST_Cabinet = 7,					//柜体
	ST_Space = 8,					//柜体空间
	ST_Frame = 9,					//柜体外框
	ST_SlidingDoor = 10,			//移门\推拉门
	ST_SideHungDoor = 11,			//掩门\平开门
	ST_DrawerDoor = 12,				//抽面
	ST_SpaceDividDoorSheet = 13,	//空间划分式门板
	ST_VariableAreaDoorSheet = 14,	//可变区域划分式门板
	ST_VariableAreaDoorSheet_ForDrawerDoor = 15, // 可变区域划分门板-仅用于抽面
	ST_VariableAreaDoorSheet_ForSideHungDoor = 16, // 可变区域划分门板-仅用于掩门
	ST_SlidingDrawer  = 17,         //抽屉
	ST_DrawerGroup = 18,            //外盖抽屉组
	ST_BoxShape = 19,				//方案框体 
	ST_EmbeddedElectrical = 20,		// 嵌入式电器
	ST_InsertionBoard = 21,			// 饰条板
	ST_EmbeddedElectricalGroup = 22,// 嵌入式电器组
	ST_NoneModelVeneeredBoard = 23, // 非造型见光板
	ST_ModelVeneeredBoard = 24,     // 造型见光板
	ST_VeneerdBoardGroup = 25,      // 见光板组
	ST_Max
};

/** 五金类型 */
enum class EMetalsType : uint8
{
	MT_None						= 0,
	MT_TOP_BOTTOM_RAILS			= 1,	// 移门顶底轨
	MT_SLIPPERY_COURSE			= 2,	// 外挂移门滑轨
	MT_PULLEY_BLOCK				= 3,	// 移门滑轮组
	MT_HINGE					= 4,	// 掩门铰链
	MT_DRAWER_GUIDE_RAIL		= 5,	// 抽屉导轨
	MT_SHAKE_HANDS				= 6,	// 拉手
	MT_LOCK						= 7,	// 锁
	MT_DOOR_BEARING				= 8,	// 门板支承件 
	MT_DOOR_DAMPER				= 9,	// 门板阻尼器
	MT_DOOR_STRAIGHTENER		= 10,	// 门板拉直器
	MT_PLATE_CONNECTION			= 11,	// 板件连接件
	MT_FINISHED_COMPONENTS		= 12,	// 成品组件
	MT_HANGER_ROD_ACCESSORIES	= 13,	// 挂杆及配件
	MT_DRESSING_MIRROR			= 14,	// 试衣镜
	MT_SD_TOP_RAILS				= 15,	// 移门顶轨
	MT_SD_BOTTOM_RAILS			= 16,   // 移门底轨
	MT_WATER_CHANNE				= 17,	// 水槽
	MT_BIBCOCK					= 18,	// 龙头
	MT_FLUE_GAS_TURBINE			= 19,   // 烟机
	MT_KITCHEN_RANGE		    = 20,	// 灶具
	MT_EMBEDDED_APPLIANCE		= 21,	// 嵌入式电器
	MT_LATERAL_BOX_SHAKE_HAND	= 22,   // 横装箱体拉手
	MT_SEALING_SIDE_HANDLE		= 23,	// 封边拉手
	MT_OTHER					= 24,   // 其他
	MT_BATHROOM_BASIN           = 25,   // 卫浴水盆
	MT_BATHROOM_TAP             = 26,   // 卫浴龙头
	MT_Max
};

/**
 * 型录产品状态
 */
enum EProductStatus : uint8
{
    PS_None = 0,      //未定义
	PS_Public = 1,    //已上架
	PS_Private = 2,   //已下架
	PS_Draft = 3,     //草稿
	PS_Useless = 4,   //不可用状态
	PS_Max
};

enum EShapeAttriModificationFlag
{
	EAM_Value = 0x01,
	EAM_Type = 0x02
};

//型录属性类型
enum EShapeAttributeType : uint8
{
	SAT_undefined = 0,       //未定义
	SAT_BoolAssign = 1,      //布尔值
	SAT_NumberAssign = 2,    //浮点值
	SAT_NumberRange = 3,     //范围数值
	SAT_NumberSelect = 4,    //可选数值
	SAT_TextAssign = 5,      //字符值
	SAT_TextSelect = 6       //可选字符值
};

/**
 * 基材类型
 */
enum ESubstrateType : uint8
{
	SsT_None = 0,    //未定义
	SsT_Board = 1,   //板材
	SsT_Stone = 2,   //石材
	SsT_Glass = 3,   //玻璃
	SsT_Max
};

/**
* 板件使用类型
*/
enum EBoardUseType : uint8
{
	BUT_None = 0,              //未定义
	BUT_HorizontalBoard = 1,   //固定层板
	BUT_VerticalBoard = 2,     //中竖板
	BUT_BackBoard = 3,         //背板
	BUT_TopBoard = 4,          //顶板
	BUT_BottomBoard = 5,       //底板
	BUT_LeftBoard = 6,         //左侧板
	BUT_RightBoard = 7,        //右侧板
	BUT_FrontBoard = 8,        //面板
	BUT_MobileBoard = 9,       //活动层板
	BUT_RevealBoard = 10,	   //外露旁板（替代侧板）
	BUT_ExposureBoard = 11,    //外露板（贴在侧板外面）
	BUT_AdjustBoard = 12,      //调整板(43垫板)
	BUT_EnhanceBoard = 13,     //加强板
	BUT_ConvergentBoard = 14,  //收口板
	BUT_SkirtingBoard = 15,    //踢脚板
	BUT_Max
};

/**
 * 板件形状类型
 */
enum EBoardShapeType : uint8
{
    BoST_None = 0,					//未定义
	BoST_Rectangle = 1,				//矩形板
	BoST_LeftCut = 2,				//左切角板
	BoST_RightCut = 3,				//右切角板
	BoST_LeftOblique = 4,			//左五角板
	BoST_RightOblique = 5,			//右五角板
	BoST_LeftOutArc = 6,			//左圆弧板
	BoST_RightOutArc = 7,			//右圆弧板
	BoST_LeftInArc = 8,				//左圆弧转角板
	BoST_RightInArc = 9,			//右圆弧转角板
	BoST_RightCorner = 10,			//右转角板
	BoST_LeftCorner = 11,			//左转角板
	BoST_RightCornerCut = 12,		//右转角切角板
	BoST_LeftCornerCut = 13,		//左转角切角板
	BoST_LeftInArcCut = 14,			//左圆弧转角切角
	BoST_RightInArcCut = 15,		//右圆弧转角切角
	BoST_InnerRectangleHole = 16,	// 内方洞
	BoST_InnerCircleHole = 17,		// 内圆洞
	BoST_LeftRightOutArc = 18,      //双圆弧板
	BoST_Max
};

/**
* 轮廓类型
*/
enum EOutlineType : uint8
{
	OLT_None = 0,           //未定义
	OLT_LeftGirder = 1,     //左侧避梁
	OLT_RightGirder = 2,    //右侧避梁
	OLT_BackGirder = 3,     //后侧避梁
	OLT_LeftPiller = 4,     //左侧避柱
	OLT_RightPiller = 5,    //右侧避柱
	OLT_MiddlePiller = 6,   //中间避柱
	OLT_CornerPiller = 7,   //转角避柱
	OLT_Max
};

/**
 * 基本组件类型（后台确定）
 */
enum EBaseShapeType : uint8
{
	BST_None = 0, //未定义
	BST_Drawer = 1,   //抽屉
	BST_Lattices = 2, //格子抽
	BST_TrousersRack = 3, //裤架
	BST_UTypeDrawer = 4, //U型抽屉
	BST_DampTypeDrawer = 5, //阻尼抽
	BST_WineRack = 6, //酒架
	BST_StorageRack = 7, //博古架
	BST_DiamondTypeWineRack = 8, //菱形酒架菱形酒架
	BST_ClothesRail = 9, //挂衣杆
	BST_Basket = 10, //拉篮
	BST_RiceBucket = 11, //米箱
	BST_LTypeRack = 12, //L架
	BST_KeyboardRack = 13, //键盘架
	BST_LTypeBlocker = 14, //L型收口
	BST_LightRack = 15, //灯带板
	BST_CollectionArea = 16, //收藏区
	BST_Max
};

/**
* 内胆的方向类型（由分割方式确定）
*/
enum EInsideDirectType
{
	IDT_None = 0,       //未定义
	IDT_Vertical = 1,   //垂直方向
	IDT_Horizontal = 2, //水平方向
	IDT_Max
};


/**
* 柜体空间子类型
*/
enum ESpaceSubType : uint8
{
	SST_None = 0,         //未定义
	SST_FrameSpace = 1,   //框体空间
	SST_SplitSpace = 2,   //分割板件空间
	SST_BottomSpace = 3,  //底部空间
	SST_InsideSpace = 4,  //内部空间
	SST_UnitSpace = 5,     //含单元柜的内部空间
	SST_ChildrenSpace = 6, //含子空间的内部空间
	SST_DrawerSpace = 7,  //外盖抽屉空间
	SST_EmbededElectrical = 8, // 嵌入式电器
	SST_Max
};

/**
* 柜体空间的方向类型
*/
enum ESpaceDirectType
{
	SDT_None = 0,      //未定义
	SDT_XDirect = 1,   //水平方向
	SDT_YDirect = 2,   //纵深方向
	SDT_ZDirect = 3,   //垂直方向
	SDT_Max
};


enum EUnitShapeType : uint8
{
	UST_None = 0,		// 未定义
	UST_ShortCab,		// 矮柜
	UST_TallCab,		// 高柜
	UST_Shelf,			// 上架
	UST_TopCab,			// 顶柜
	UST_HangupCab,		// 吊柜
	UST_SideCab,		// 旁架单元柜
	UST_CornerCab,		// 转角柜
	UST_Desk,			// 书桌
	UST_BedCab,			// 床头柜
	UST_Tatami,			// 榻榻米
	UST_WindowCab,		// 飘窗柜
	UST_WardrobeUnit,	// 移门衣柜单元柜下柜
	UST_Cupboard,		// 橱柜下柜
	UST_HangupCupboard,	// 橱柜吊柜
	UST_TallCupboard,	// 橱柜高柜
	UST_MediumCupboard,	// 橱柜半高柜
	UST_DeckCupboard,	// 橱柜台上柜
	UST_Max
};

struct FDoorSubWidgetDefaultInfoBase
{
public:
	EMetalsType	MetalType;
};

/** 明装拉手默认位置 */
struct FDoorHanlerDefaultLocation  : public FDoorSubWidgetDefaultInfoBase
{
	int32 Hpos				= -1;		/*!< 水平位置：1 距左边；2 距右边；3 水平居中；*/
	float HposVal			= -1.0f;	/*!< 水平位置距离*/
	int   Vpos				= -1;		/*!< 垂直位置：1 距上边；2 距下边；3 水平居中；4 公式；*/
	float VposVAl			= -1.0f;	/*!< 垂直位置距离*/
	int32 RotateDirection   = 0;		/*!< 旋转方向：0 保持不变；1 左旋90度；2 右旋90度；*/
	float LeftExtend = 0.0f;			/*!< 箱体拉手左延伸 */
	float RightExtend = 0.0f;			/*!< 箱体拉手右延伸 */
};

/** 箱体拉手默认位置 -- 废弃，这些值都在模型属性里面存储着*/
struct  FLateralBoxShakeHand : public FDoorSubWidgetDefaultInfoBase
{	
	float HorizontalChandleAbove = -1.0f;	/*!< 横装箱体拉手距上*/
	float WallCupBoardValueBottom = -1.0f;	/*!< 地柜箱体拉手距下*/
	float FloorCupboardAbove = -1.0f;		/*!< 吊柜箱体拉手距上*/
};


/** 门默认属性信息 */
struct FDoorDefaultInfo
{
	int32 ID;
	int32 DoorType;	/*!< 门类型 1 移门; 2掩门; 3 抽面*/
	int32 SubstrateId; /*!< 默认基材*/
	int32 DoorSlotValue; /*!< 门缝值*/
	int32 CoverUpWay;  /*!< 掩盖方式 1内嵌；2外盖*/		
	TSharedPtr<TArray<TSharedPtr<FDoorSubWidgetDefaultInfoBase>>> MetalDefaultLocationList;
};

/** 插角封板 */
struct FPinBoardDefaultInfo
{
	float BlankingPlateDefaultWidth; /*!< 封板默认宽度*/
	float BlankingPlateMinWidth;   /*!< 封板最小宽度*/
	float BlankingPlateChafmerWidth; /*!< 切角尺寸-切角宽*/
	float BlankingPlateChafmerHeight;/*!< 切角尺寸-切角高*/
	int32 BlankingPlateDefaultMaterialId;/*!< 封板、帮板默认材质*/
	int32 BlankingPlateDefaultSubstrateId; /*!< 默认基材ID*/
	float thickness; /*!<默认厚度*/
	FString BlankingPlateDefaultMaterialName;/*!< 封板、帮板默认材质Url*/
	FString BlankingPlateDefaultMaterialUrl;/*!< 封板、帮板默认材质Url*/
	FString BlankingPlateDefaultMaterialMd5;/*!< 封板、帮板默认材质Md5*/
	FString BlankingPlateDefaultMaterialThumbnailUrl;/*!< 封板、帮板默认材质缩略图Url*/
	int32 BlankingPlateDefaultEdgeBandingId;/*!< 封板、帮板默认封边ID*/
	float AssistPlateDefaultLength;/*!< 帮板默认长度*/
	float AssistPlateAlignPosition; /*!< 帮板与前封板的默认对齐位置*/
};


struct FCommonPakData
{
	int64 ID = 0;
	FString Name;
	FString Url;
	FString MD5;
	FString ThumbnailUrl;
	FString OptimizeParam;
};

class UStaticMesh;
class USkeletalMesh;
class UAnimationAsset;
class UMaterialInstanceDynamic;

struct FMeshDataEntry
{
	union
	{
		UStaticMesh *Mesh;
		USkeletalMesh *SkeletalMesh;
	} MeshData;

	UAnimationAsset *AnimationAsset;

	int32 Type; // 0 for normal mesh, 1 for mesh with skeletal animation
	TArray<TPair<FString, UMaterialInstanceDynamic*>> MaterialList;
	FVector Center;
	FVector Size;

	FMeshDataEntry()
	: Type(-1)
	, AnimationAsset(nullptr)
	{
		MeshData.Mesh = nullptr;
	}
};