/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File FArmyDownloadCad.h
 * @Description 下载CAD
 *
 * @Author ChangYuan
 * @Date 2019年2月14日
 * @Version 1.0
 */

#pragma once
#include "CoreMinimal.h"
#include "ArmyConstructionLayerManager.h"


#define LineType_01 "CONTINUOUS"
#define LineType_02 "DASHED2"


#define FILL_STYLE_0 "AR-HBONE"  // 轻钢龙骨
#define FILL_STYLE_1 "NET"  // 轻体石膏板
#define FILL_STYLE_2 "ANSI31"  // 轻体砖
#define FILL_STYLE_3 "AR-B816"  // 墙体

// 线性类型
enum ELineWeight
{
	LW_000 = 0,
	LW_005 = 5,
	LW_009 = 9,
	LW_013 = 13,
	LW_015 = 15,
	LW_018 = 18,
	LW_020 = 20,
	LW_025 = 25,
	LW_030 = 30,
	LW_035 = 35,
};

// 图层类型
enum ELayerType
{
	LT_Default = 1,       // 默认图层
	LT_Wall_Solid = 2,	  // DBJ - 承重墙填充
	LT_Wall = 3,          // DBJ - 原始建筑墙体
	LT_Windows = 4,       // DBJ - 窗
	LT_LL = 5,            // DBJ - 顶面梁
	LT_Column = 6,        // DBJ - 柱
	LT_Column_Solid = 7,  // DBJ - 柱填充
	LT_Flue = 8,          // DBJ - 风道
	LT_Flue_Solid = 9,    // DBJ - 风道填充
	LT_Lvtry = 10,        // DBJ - 下水主管道
	LT_Fp_Potential = 11, // DBJ - 强弱电点位
	LT_Fp_Water = 12,     // DBJ - 水路点位
	LT_Fp_Utilities = 13, // DBJ - 燃气点位
	LT_Text = 14,         // DBJ - 原始标注
	LT_Wall_Dim = 15,     // DBJ - 内墙标注
	LT_Grid_Dim = 16,     // DBJ - 外墙标注
	LT_Ceiling_Dim = 17,  // DBJ - 天花标注
	LT_Windows_Dim = 18,  // DBJ - 门窗标注
	LT_Fp_Potential_Dim = 19, // DBJ - 强弱电点位标注
	LT_Fp_Water_Dim = 20, // DBJ - 水路点位标注
	LT_Fp_Gas_Dim = 21,   // DBJ - 燃气点位标注
	LT_Define_Text = 22,  // DBJ - 自定义标注
	LT_Title_Gdg = 23,    // DBJ - 图框
	LT_Wall_Del = 24,     // DBJ - 拆除墙体
	LT_Wall_Del_Solid = 25, // DBJ - 拆除墙体填充
	LT_Wall_Del_Dim = 26, // DBJ - 拆除墙标注
	LT_Wall_Add = 27,     // DBJ - 新建墙体
	LT_Wall_Add_Solid = 28, // DBJ - 新建墙体填充
	LT_Wall_Add_Dim = 29, // DBJ - 新建墙标注
	LT_Fp_Door = 30,      // DBJ - 门
	LT_Fp_Furn = 31,      // DBJ - 平面活动家具
	LT_Fp_Toilet_Furn = 32, // DBJ - 洁具
	LT_Fp_Light = 33,     // DBJ - 平面灯具
	LT_Fp_Curtain = 34,   // DBJ - 窗帘
	LT_Fp_Furn_Fl = 35,   // DBJ - 固定家具
	LT_Fp_Kitchen_Furn = 36, // DBJ - 厨房电器
	LT_Ceil_Light = 37, // DBJ - 天花灯具
	LT_Fp_Floor_M_Hatch = 38, // DBJ-地面拼花填充
	LT_Fp_Floor_M = 39, // DBJ-地面拼花线
	LT_Wall_Hatch = 40, // DBJ-墙面造型
	LT_Ceiling=41, // DBJ-天花结构线
	LT_Ceil_Hatch = 42, // DBJ-天花填充
	LT_Fp_Electrical = 43, // DBJ-开关及连线
	LT_FP_Furn_Dim = 44, // DBJ-家具尺寸标注
	LT_Ceil_Light_Dim = 45, // DBJ-灯具标注
	LT_Fp_Electrical_Dim = 46, // DBJ-开关标注
	LT_Wall_Cabinet_Line = 47, // DBJ-吊柜线条
	LT_Wall_Cabinet_Solid = 48, // DBJ-吊柜填充
	LT_Floor_Cabinet_Line = 49, // DBJ-地柜线条
	LT_Floor_Cabinet_Solid = 50, // DBJ-地柜填充
	LT_High_Cabinet_Line = 51, // DBJ-高柜线条
	LT_High_Cabinet_Solid = 52, // DBJ-高柜填充
	LT_Board_Splitline = 53, // DBJ-分割线
	LT_Cabinet_Countertops = 54, // DBJ-柜体台面
	LT_Cabinet_Symbol = 55, // DBJ-柜体索引号
};

/** 物体绘制模式 */
enum ECadDrawMode
{
	CDM_Normal = 1,     // 普通绘制模式
	CDM_AddWall = 1 << 1,    // 新增墙绘制
	CDM_DelWall = 1 << 2,    // 删除墙绘制模式
	CDM_DoorPass = 1 << 3,   //门-门洞的绘制模式
	CDM_LampStrip= 1<< 4,    // 施工图下灯带绘制
	CDM_OtherCeilingObj = 1 << 5,    // 施工图下顶面排除灯带的其他数据(以后可能会更细化的拆分)
};

// 图层信息
struct LayerInfo
{
	int32 index;
	FString Name;
	FColor LayerColor;
	ELineWeight LayerLineWeight;
	FString LineType;
	LayerInfo(FString InName, FColor InLayerColor, FString InLineType, ELineWeight InLineWeight)
	{
		Name = InName;
		LayerColor = InLayerColor;
		LayerLineWeight = InLineWeight;
		LineType = InLineType;
	}
	~LayerInfo() {};
};


class XR_API FArmyDownloadCad : public TSharedFromThis<FArmyDownloadCad>
{

public:
	static const TSharedRef<FArmyDownloadCad>& Get();
	FArmyDownloadCad();
	~FArmyDownloadCad() {};
	
	// 生成CAD文件测试
	bool CreateCad(const TMap<EConstructionFrameType, TSharedPtr<class FArmyConstructionFrame>>& InFrameMap, const TSharedPtr<class FArmyConstructionLayerManager>& InLayerManager,FString CadFilePath, const TArray<FName>& CadChosenList,FString &OutPath);
	
private:
	// 绘制测试数据
	bool TestCad();
	// 绘制CAD
	bool DrawCAD();
	// 根据bim图纸绘制图框
	bool DrawCadFrame(TSharedPtr<class FArmyLayer> BimLayer,int32 CadIndex);
	// 绘制指北针
	bool DrawCompass(int32 CadIndex);
	// 根据bim的layer绘制每张cad图
	bool DrawCadByLayer(TSharedPtr<class FArmyLayer> BimLayer, int32 CadIndex);
private:
	// 根据bim的layer绘制墙（新建墙体和拆除墙体填充）
	bool DrawCadWall(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制门（推拉门、防盗门、标准门）
	bool DrawCadDoor(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制窗户（标准窗户等5类）
	bool DrawCadWindow(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制风烟道梁
	bool DrawCadHardware(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制点位
	bool DrawCadComponent(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制家具
	bool DrawCadFurniture(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制标尺
	bool DrawCadDimensions(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制文本
	bool DrawCadTextLable(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制地面数据
	bool DrawCadFloor(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// DrawCadFloor内部方法，绘制线框
	bool DrawCadWireFrame(int32 CadIndex, FString InLayerName, TSharedPtr<FArmyObject> InObject);
	// DrawCadFloor内部方法，绘制波打线
	bool DrawCadBodaLine(int32 CadIndex, FString InLayerName, TSharedPtr<class FArmyBodaArea> InBodaArea);
	// DrawCadFloor内部方法，绘制放样线条
	bool DrawCadSkitLine(int32 CadIndex, FString InLayerName, TSharedPtr<class FArmyObject> InObject);
	// 根据bim的layer绘制顶面数据
	bool DrawCadCeiling(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 根据bim的layer绘制顶面填充
	bool DrawCadCeilingFill(int32 CadIndex, FString InLayerName, TSharedPtr<class FArmyObject> InObject);
	// DrawCadCeiling内部方法-共通绘制
	bool DrawCadRectBase(int32 CadIndex, ObjectClassType InClassType, TSharedPtr<class FArmyObject> InObject);
	// 根据bim的layer绘制灯控线
	bool DrawCadLampControlLines(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 绘制定制柜类
	bool DrawCadSCTCabinet(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
	// 绘制定制五金/电器类
	bool DrawCadSCTHardware(int32 CadIndex, ObjectClassType InClassType, TArray<TSharedPtr<FArmyObject>> InObjItList);
private:
	// 初始化图纸的集合
	void InitBimLayerArray(const TArray<FName>& CadChosenList);
	// 初始化图层信息
	void InitLayersMap();
	// 获得图层的名字通过编号
	FString GetLayerName(int32 LayerType);
	//// 根据bim的图纸获得CAD的图纸
	//ECadFrameType GetCadTypeByLayer(int32 Layerindex);
	// 根据BIm的图层获得CAD的图层
	ELayerType GetLayerTypeByObjectClassType(ObjectClassType InClassType);
	// 生成拆改后的户型数据
	TArray<TSharedPtr<FArmyLine>> GenerateChangedData(EModelType Modeltype = EModelType::E_HomeModel);
	// 获取原始户型数据-中间函数
	TArray<TSharedPtr<FArmyLine>> GetHomeModelData(EModelType Modeltype);
	// 获取飘窗、门洞数据-中间函数
	TArray<TSharedPtr<FArmyLine>> GetHardwareData(EModelType Modeltype);
	// 初始化绘图模式通过bim图层
	void InitCadDrawModel(TSharedPtr<class FArmyLayer> BimLayer);
	// 获取绘制模式
	bool GetCadDrawModel(ECadDrawMode InType);
	// 设置绘制模式
	void SetCadDrawModel(ECadDrawMode InType, bool InUse=true);
private:
	// BIM图层管理器
	TSharedPtr<class FArmyConstructionLayerManager> LayerManager;
	// 图框信息
	TMap<EConstructionFrameType, TSharedPtr<class FArmyConstructionFrame>> MapFrameInfo;
	//TSharedPtr<class FArmyConstructionFrame> FrameInfo;
	// CAD图层字典
	TMap<int32, TSharedPtr<LayerInfo>> LayersMap;
	// 需要绘制的CAD图层对应的bim图纸集合
	TArray<TSharedPtr<class FArmyLayer>> BimLayerArray;
	// cad绘制模式
	TSet<ECadDrawMode> CadModeArray;
};