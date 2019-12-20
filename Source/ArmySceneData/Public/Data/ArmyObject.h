#pragma once

#include "ArmyPoint.h"
#include "Armyline.h"
#include "ArmyArcLine.h"
#include "ArmyCircle.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Json.h"
//#include "ArmyTransaction.h"
#include "ArmyTypes.h"
#include "FArmyConstructionItemInterface.h"

class FArmyEditPoint;
struct FLinearColor;
struct ObjectConstructionKey;

#define POLYGONOFFSETDIS 0.2
#define WALLLINEWIDTH 2.0f

enum EObjectType
{
	OT_None = -1,       // 无
	OT_Wall,            // 墙面
	OT_ArcWall,
	OT_ArcWindow, // 弧形窗
	OT_Door,            // 门
	OT_SecurityDoor,     // 防盗门
	OT_SlidingDoor,     // 推拉门
	OT_Pass,            // 垭口
	OT_NewPass,            // 新开垭口
	OT_DoorHole,		// 门洞
	OT_Window,          // 窗
	OT_FloorWindow,
	OT_RectBayWindow,
	OT_TrapeBayWindow,
	OT_CornerBayWindow,
	OT_Punch,				//开洞 @欧石楠
	OT_CrossDoorStone, //过门石

	OT_InternalRoom,//内墙区域
	OT_OutRoom,//外墙区域
	OT_WallLine,//墙线
	OT_BaseWall, //原始墙体
	OT_ModifyWall, // 拆改墙
	OT_AddWall,
	OT_IndependentWall, //独立墙
	OT_PackPipe,		//包立管
	OT_SplitLine, // 分割线 by常远
	OT_Region, // 区域 by常远
	OT_BaseBoard,   // 踢脚线
	OT_Dimensions, // 标注
	OT_InSideWallDimensions, // 标注
	OT_OutSideWallDimensions,
	OT_AddWallDimensions, //新增墙标尺
	OT_DeleteWallDimensions,//拆除墙标尺
	OT_TextLabel,//文本框
	OT_ObjectRuler, // 
	OT_ReferenceImage, // 底图
    OT_LayingPoint, // @欧石楠 起铺点

	OT_ActorItem, // 3D模型

	OT_EleBox_Begin, //@欧石楠 电箱开始
	OT_EleBox_Point, //电箱,
	OT_EleBoxL_Point,//弱电箱
	OT_EleBoxH_Point,//强电箱
	OT_EleBox_End, //@欧石楠 电箱结束

	OT_Water_Point,//水路点位,	


	
	OT_Component_Begin, //构件开始
	OT_ComponentBase,

	OT_Water_ChilledAndHot_Point,

	OT_Socket_Begin, //@欧石楠 插座开始
	OT_Socket_Point,//插座
	OT_Socket_Weak,//弱电插座
	OT_Socket_Strong,//强电插座
	OT_Socket_Five_Point,//五孔插座
	OT_Socket_Five_On_Point,//五孔开关插座
	OT_Socket_Five_On_Dislocation,//五孔错位插座
	OT_Socket_Five_Point_Dislocation,//五孔错位开关插座
	OT_Socket_Four_Point, //四孔插座
	OT_Socket_Four_On_Point,//四孔开关插座
	OT_Socket_Three_On_Point, //三孔开关插座
	OT_Socket_Three, //三孔插座
	OT_Socket_Three_On_Point_3A, //16A三孔开关插座
	OT_Socket_Three_3A, //16A三孔插座

	OT_Socket_White,//白板
	OT_Socket_Spatter,//防溅盒
	OT_Socket_IT,//网线插座
	OT_Socket_IT_TV,//电视电脑插座
	OT_Socket_TV,//电视插座
	OT_Socket_Phone,//电话插座
	OT_Socket_IT_Phone,//电脑电话插座

	OT_Socket_TP, //厨房橱柜专用联排插座
	OT_Socket_TV_Couplet, //电视墙专用联排插座	
	OT_Socket_End, //@欧石楠 插座结束


	OT_ElectricWire,//

	OT_Switch_Begin, //@欧石楠 开关开始
	OT_Switch,//强电开关
	OT_Switch_BathHeater,//浴霸开关
	OT_Switch_Double,//双键无线开关
	OT_Switch_Single,//单键无线开关
	OT_Switch4O2_Point,//四联双控开关
	OT_Switch4O1_Point,//四联单控开关
	OT_Switch3O2_Point,//三联双控开关
	OT_Switch3O1_Point,//三联单控开关
	OT_Switch2O2_Point,//双联双控开关
	OT_Switch2O1_Point,//双联单控开关
	OT_Switch1O2_Point,//单连双控开关
	OT_Switch1O1_Point,//单连单控开关
	OT_Switch_End, //@欧石楠 开关结束

	OT_Water_Hot_Point,//热水点位
	OT_Water_Chilled_Point,//冷水点位
	OT_Water_Hot_Chilled_Point,//冷熱水
	OT_Water_Normal_Point,//中水

	OT_Water_Separator_Point,//分集水器

	OT_PrePT_Begin, //@欧石楠 预装点位开始
	OT_Dewatering_Point,//排水点位
	OT_FloorDrain_Point,//地漏
	OT_Closestool,//马桶下水
	OT_Basin,//地漏下水
	OT_Water_Basin,//水盆下水
	OT_Water_Supply, // 给水点位,
	OT_PrePT_End, //@欧石楠 预装点位结束



	OT_Lamp_Pendant_Point,//吊灯
	OT_Lamp_Flower_Point,//花灯
	OT_Lamp_Down_Point,//筒灯
	OT_Lamp_Ceiling_Point,//吸顶灯
	OT_Furniture_TVStand,//电视柜
	OT_Furniture_TV,//电视
	OT_Furniture_TeaTable,//茶几
	OT_Furniture_SquattingPan,//蹲便器
	OT_Furniture_SofaSet,//沙发组
	OT_Furniture_Sofas,//多人沙发
	OT_Furniture_Sink,//厨房水槽
	OT_Furniture_SingleBed,//单人床
	OT_Furniture_ShowerHead,//花洒
	OT_Furniture_PedestalPan,//坐便器
	OT_Furniture_FloorCabinet,//地柜
	OT_Furniture_DoubleSofa,//双人沙发
	OT_Furniture_DoubleBed18X20,//双人床1800*
	OT_Furniture_DoubleBed15X19,//双人沙发150
	OT_Furniture_DiningTable,//餐桌
	OT_Furniture_CommonCabinet,//通用柜
	OT_Furniture_Closet,//衣柜
	OT_Furniture_Chair,//椅子
	OT_Furniture_Botany,//植物
	OT_Furniture_BedSideTable,//床头柜
	OT_Furniture_BedChair,//床尾凳
	OT_Furniture_Bathtub,//浴缸
	OT_Furniture_BathroomCabinet,//浴室柜
	OT_Furniture_Armchair,//单人沙发
	OT_Funiture_GasAppliance,//灶具
	OT_OtherComponent,//其他构件
	OT_Gas_Meter,//燃气表

	OT_Pillar,
	OT_Beam,
	OT_CurtainBox,
	OT_AirFlue, //风道
	OT_Furniture_CasualSofa, // 休闲沙发
	OT_Furniture_CornerTable, // 角几
	OT_Furniture_AirConditioner, // 空调
	OT_Furniture_Curtains, // 窗帘
	OT_Furniture_WritingDesk, // 书桌
	OT_Furniture_WritingChair, // 书椅
	OT_Furniture_SingleSink, // 厨房水槽(单槽)
	OT_Furniture_Fridge, // 冰箱
	OT_Furniture_WashingMachine, // 洗衣机
	OT_Component_End, //构件结束

	/**	施工图模块Begin*/
	OT_ConstructionFrame, //施工图图框
	OT_Compass,//指北针
	OT_Entrance,//入口图
	OT_LampControlLine,//灯控图中的连线
	/**	施工图模块End*/


	OT_Space,      // 空间

	OT_Hydropower_Begin, //水路布线开始
	OT_Pipe_Begin,
	OT_StrongElectricity_25,            //强电2.5
	OT_StrongElectricity_4,            //强电4
	OT_StrongElectricity_Single,            //强电单控
	OT_StrongElectricity_Double,            //强电双控
	OT_WeakElectricity_TV,//弱电 电视线
	OT_WeakElectricity_Net,//弱电， 网线
	OT_WeakElectricity_Phone,//弱电，电话线
	OT_ColdWaterTube,//冷水管
	OT_HotWaterTube,//热水管
	OT_Drain,//排水管
	OT_Pipe_End,

	OT_Linker_Begin, //@欧石楠 连接器开始
	OT_StrongElectricity_25_Linker,            //强电2.5
	OT_StrongElectricity_4_Linker,            //强电4
	OT_StrongElectricity_Single_Linker,            //强电单控
	OT_StrongElectricity_Double_Linker,            //强电双控
	OT_WeakElectricity_TV_Linker,//弱电 电视线
	OT_WeakElectricity_Net_Linker,//弱电， 网线
	OT_WeakElectricity_Phone_Linker,//弱电，电话线
	OT_ColdWaterTube_Linker,//冷水管
	OT_HotWaterTube_Linker,//热水管
	OT_Drain_Linker,//排水管
	OT_WaterLinker,   //水管连接器
	OT_ElectricLinker,   //电线连接器
	OT_Linker_End, //@欧石楠 连接器结束

	OT_Preload_Begin, // 预装点位(绘制)
	OT_Drain_Point,//下水主管道
	OT_Gas_MainPipe,//燃气主管
	OT_Preload_End,

	OT_Water,   //水
	OT_Hydropower_End, //水路布线结束

	OT_TextureEditorBegin,
	OT_HelpRectArea,//矩形辅助区域
	OT_ClinderArea,//圆柱辅助区域
	OT_RectArea, //矩形绘制
	OT_CircleArea, //矩形绘制
	OT_PolygonArea, //矩形绘制
	OT_FreePolygonArea,// 自由多边形绘制区域
	OT_BridgeStoneArea,//过门石区域
	//@王志超
	OT_BodaArea,		//波打线区域
	OT_WaterKnifeArea,//水刀拼花
	OT_RoomSpaceArea,
	OT_TextureEditorEnd,
	OT_DownLeadLabel,//引线标注
	OT_BearingWall,//承重墙 
	OT_CupboardTable,//橱柜台面
	OT_BoardSplitline,//台面分割线
	OT_Symbol,//立面索引符号
	OT_UserDefine,//用戶自定義樣式
	OT_Component_None, //@郭子阳 空构件 用于水电模式丢失构件的对象

	OT_CabinetFace,//柜体立面图
	OT_DoorSheetFace,//门板立面图
	OT_CupboardTableFace,//台面立面图
	OT_SunBoardFace,//见光板
	OT_ResizeFace,//调整板立面
	OT_TopLineFace,//上线条立面图
	OT_FootLineFace,//踢脚板立面图


	OT_StrongElectricity_6,      //@郭子阳      //强电6
	OT_StrongElectricity_6_Linker,  //@郭子阳          //强电6
	OT_LightElectricity,      //@郭子阳      //照明线路
	OT_LightElectricity_Linker,  //@郭子阳          ///照明线路

	OT_AirLouver,//@欧石楠 空调孔

	OT_TheEnd	  //类型的最大值（有新类型请在上边加）
	/************************************************************************/
	/*						注意：不要在下边添加新类型                        */
	/************************************************************************/
};



/** 物体状态 */
enum EObjectState
{
	OS_Normal,
	OS_Hovered,
	OS_Selected,
	OS_Disable
};
/** 物体绘制模式 */
enum EObjectDrawMode
{
	MODE_NORAMAL = 1, //普通绘制模式
	MODE_TOPVIEW = 1 << 1, //顶视图绘制模式
	MODE_CEILING = 1 << 2, //吊顶视图模式
	MODE_CONSTRUCTION = 1 << 3, //施工图绘制模式
	MODE_MODIFYADD = 1 << 4, //新增墙绘制模式（用于门洞的增墙）
	MODE_MODIFYDELETE = 1 << 5, //拆除墙绘制模式（用于门洞的拆墙）
	MODE_ADDWALLPOST = 1 << 6, //增墙墙后无填充绘制模式
	MODE_DELETEWALLPOST = 1 << 7, //拆墙后无填充绘制模式
	MODE_LAMPSTRIP = 1 << 8, //施工图下灯带绘制
	MODE_OTHERCEILINGOBJ = 1 << 9, //施工图下顶面排除灯带的其他数据(以后可能会更细化的拆分)
	MODE_DOORPASS = 1 << 10 //门-门洞的绘制模式
};

// 绘制线的类型
struct FLinesInfo
{
	TArray<FVector> Vertices;
	FLinearColor Color;
	int32 LineType; // 0 实线  1：虚线
	int32 DrawType; // 0 首尾相连 1：首尾不相连

	FLinesInfo() { LineType = 0; DrawType = 0; Color = FLinearColor(1, 1, 1); }

	FLinesInfo(const TArray<FVector> &InVertices, const FLinearColor& InColor, int32 InLineType,int32 InDrawType) :
		Vertices(InVertices),
		Color(InColor),
		LineType(InLineType),
		DrawType(InDrawType)
	{}
};
class FScaleCanvasText : public FCanvasTextItem
{
public:
	FScaleCanvasText();
	void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, const FSceneView* View, FCanvas* InCanvas);

	FORCEINLINE void SetPosition(const FVector& InPos) { WorldPosition = InPos; }
	FORCEINLINE void SetRotator(const FRotator& InRot) { Rotation = InRot; };
	FORCEINLINE void SetTextSize(int32 InSize) { TextSize = InSize; };
	FORCEINLINE int32 GetCanvasTextSize() { return TextSize; };
	FORCEINLINE int32 GetScaleTextSize() { return TextSize; };
	FORCEINLINE FVector GetPosition() { return WorldPosition; };

public:
	FVector2D BaseScale = FVector2D(1, 1);
private:
	FVector WorldPosition;
	FRotator Rotation = FRotator(0, 0, 0);
	int32 TextSize = 12;
};

typedef TSharedPtr<class FArmyObject> FObjectPtr;
typedef TWeakPtr<class FArmyObject> FObjectWeakPtr;

DECLARE_DELEGATE_RetVal_OneParam(FObjectWeakPtr, GetObjectByID, const FGuid&)

typedef FObjectPtr(*fun_ptr) ();

static TMap<FString, fun_ptr> ClassRegisterMap;

class ARMYSCENEDATA_API ClassFactory
{
public:
	static void ArmyRegisterClass(FString InName, fun_ptr InFun)
	{
		ClassRegisterMap.Add(InName, InFun);
	}
	static FObjectPtr GetInstance(const FString& InName)
	{
		return ClassRegisterMap.FindRef(InName)();
	}
};

class RegisterOperation
{
public:
	RegisterOperation(const FString& InName, fun_ptr InFun)
	{
		ClassFactory::ArmyRegisterClass(InName, InFun);
	}
};

#define XRCLASSINSTANCE(Class_name) \
ClassFactory::GetInstance(TEXT(#Class_name));

#define REGISTERCLASS( Class_name ) \
class Register##Class_name \
{\
public:\
	static FObjectPtr Instance()     \
	{\
		return MakeShareable(new Class_name);  \
	};\
};\
static RegisterOperation Temp##Class_name(TEXT(#Class_name), Register##Class_name::Instance);

#define SERIALIZEREGISTERCLASS(JsonWriter,ClassName)\
	JsonWriter->WriteValue("RegisterClass", TEXT(#ClassName));
/**
 * 户型绘制元素基类
 */
class ARMYSCENEDATA_API FArmyObject : public TSharedFromThis<FArmyObject>
{
public:
	enum PropertyType
	{
		FLAG_COLLISION = 1, //碰撞
		FLAG_POINTOBJ = 1 << 1, //点类型物体例如 开关、水位等,点类型数据的六轴坐标通过基点发出
		FLAG_MODIFY = 1 << 2, //欧石楠 属于拆改中的标记
		FLAG_LAYOUT = 1 << 3, //欧石楠 属于拆改后的标记
		FLAG_VISIBILITY = 1 << 6, //可见性(为了与旧版本数据的对接)
		FLAG_PICKPRIMITIVE = 1 << 7, //可见性(为了与旧版本数据的对接)
		FLAG_ORIGINALPOINT = 1 << 8, //点位性质(原始点位标记)
		FLAG_STOPPED =1<<9, //点位停用标志
	};

public:
	FArmyObject()
	{
		FPlatformMisc::CreateGuid(ObjID);
		SetPropertyFlag(FLAG_VISIBILITY, true);

		//初始化施工项
		ConstructionData = MakeShareable(new FArmyConstructionItemInterface);
	}

	virtual ~FArmyObject() {}

	virtual void Destroy() {}

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	//~ Begin Transaction Record
	virtual void Create();
	virtual void Delete();
	virtual void Modify();
	virtual void PostModify(bool bTransaction = false) {}
	//void SaveTransaction(ETransType TransType);
	virtual void PostDelete() {}
	//~ End Transaction Record

	//添加数据更新到户型模式（主要为了户型和硬装数据对接）
	//void PushHomeModeModifyData(ETransType TransType);

	FObjectPtr CopySelf(bool bCopyGUID = false);

    virtual void SetState(EObjectState InState) { State = InState; }

	/** 刷新 */
	virtual void Refresh() {};

	/** 绘制 */
	virtual void Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) {}

	/** HUD绘制 */
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) {}

	/** 选中 */
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) { return false; }

	/** 选中操作点 */
	virtual TSharedPtr<FArmyEditPoint> SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) { return NULL; }

	/** 高亮操作点 */
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) { return NULL; }

	/** 高亮操作线 */
	virtual TSharedPtr<FArmyLine> HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) { return NULL; }

	/** 高亮 */
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) { return false; }

	/**	获取所有顶点*/
	virtual void GetVertexes(TArray<FVector>& OutVertexes);

	/**	获取所有线*/
	virtual void GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) {}

	/**	获取弧形线*/
	virtual void GetArcLines(TArray<TSharedPtr<FArmyArcLine>>& OutArcLines) {}

	/**	获取弧形线*/
	virtual void GetCircleLines(TArray<TSharedPtr<FArmyCircle>>& OutCircle) {}

	/**	删除线*/
	virtual void RemoveLine(TSharedPtr<FArmyLine> InLine) {}

	/**	删除弧形线*/
	virtual void RemoveArcLine(TSharedPtr<FArmyArcLine> InArcLine) {}

	/** @欧石楠 当被删除时的调用*/
	virtual void OnDelete() {}

	/**	获取弧形线*/
	virtual void RemoveCircleLine(TSharedPtr<FArmyCircle> InCircle) {}

	/**	获取所有单独的点*/
	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints) {}

	/**	设置应用变换的标记*/
	virtual void SetTransformFlag(uint32 InFlag) 
	{ 
		TransformFlag = InFlag;
	};
	

	/** 移动位置 */
	virtual void ApplyTransform(const FTransform& Trans) {}

	virtual void GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const {};

	/** 相对object原点的坐标变换*/
	virtual FVector TranfromVertex(const FVector& InV) const { return InV; };

	/** 获取包围盒*/
	virtual const FBox GetBounds() { return FBox(ForceInitToZero); }

	/** 获取原始包围盒（未进行变换的包围盒）*/
	virtual const FBox GetPreBounds() { return FBox(ForceInitToZero); }

	/**	获取基点，默认为包围盒中心点，构件需要自定义基点*/
	virtual const FVector GetBasePos() { return GetBounds().GetCenter(); }

	/**	设置属性标识*/
	virtual void SetPropertyFlag(PropertyType InType, bool InUse);
	/**	获取属性标识*/
	virtual bool GetPropertyFlag(PropertyType InType);
	// 获得绘制CAD的数据
	virtual void GetCadLineArray(TArray<TPair<FVector,FVector>>& OutLineList) const {};

	/**	设置绘制模式*/
	static void SetDrawModel(EObjectDrawMode InType, bool InUse);
	/**	获取绘制模式*/
	static bool GetDrawModel(EObjectDrawMode InType);

	void SetPropertyFlag(uint32 InPropertyFlag) { PropertyFlag = InPropertyFlag; }

	const uint32 GetPropertyFlag() const { return PropertyFlag; }

	virtual void SetForceVisible(bool InV) { ForceVisible = InV; }

	FORCEINLINE EObjectState GetState() const { return State; }

	virtual void SetType(EObjectType InType) { ObjectType = InType; }

	virtual EObjectType GetType() const { return ObjectType; }

	virtual const FString& GetName() const;

	virtual void SetName(const FString& InName) { ObjectName = InName; }

	virtual const FGuid& GetUniqueID() const { return ObjID; }
	virtual void SetUniqueID(FGuid GUID) { ObjID = GUID; }

	virtual void SetDrawDepth(uint8 depthId) {}

	//virtual void SetDrawConstructionMode(bool EnableModel) { ConstructioMode = EnableModel; }

	virtual const TArray<TWeakPtr<FArmyObject>>& GetParents() const;

	virtual const TArray<FObjectPtr>& GetChildren() const;

	virtual void Generate(UWorld* InWorld) {}

	virtual void SetRelevanceActor(class AActor* InActor);
	virtual class AActor* GetRelevanceActor()
	{
		return RelevanceActor;
	}

	template<typename T> T* AsassignObj()
	{
		T* v = dynamic_cast<T*>(this);
		return v;
	}
public:
	static TMap<FString, AActor*> DynamicActorMap;

	static GetObjectByID OnGetObjectByID;
protected:
	friend class FArmySceneData;

	virtual void AddChild(FObjectPtr InObj);

	virtual void RemoveChild(FObjectPtr InObj);

	virtual void ClearChildren();

	virtual void AttachToParent(FObjectPtr InParent);

	/**	InParent = NULL remove from all parent*/
	virtual void RemoveFromParents(FObjectPtr InParent = NULL);

public:
	/**	是否应用包围盒*/
	bool bBoundingBox = true;

	/** 创建XRObject对象需要的参数 */
	ArmyArgument Args;

	/** 用来记录修改之前的XRObject的状态 */
	FString RecordBeforeModify;

	/** 用来记录XRObject的状态 */
	FString Record;

protected:
	static uint32 ObjectDrawMode;

	/** 状态 */
	EObjectState State = OS_Normal;
	/**	唯一标识*/
	FGuid ObjID;
	/**	属性标识*/
	uint32 PropertyFlag = 0;

	/**	应用变换的标识(变换方向 1:+x,2:-x,3:+y,4:-y)*/
	uint32 TransformFlag = 0;

	EObjectType ObjectType = OT_None;

	/**	用于包含的actor的显示隐藏控制*/
	bool ForceVisible;

	TArray<FObjectPtr> Children;

	TArray<TWeakPtr<FArmyObject>> ParentList;

	class AActor* RelevanceActor = NULL;

	/**@欧石楠 标记是否是拷贝数据*/
	bool bIsCopyData = false;
private:
	/** Object名称 */
	FString ObjectName;
//******************************3D模式施工项相关***************************
public:
	//@郭子阳
	//3D施工项数据
	TSharedPtr<FArmyConstructionItemInterface> ConstructionData;

	//@郭子阳
	//是不是水电模式的管子
	bool IsPipeLine();
	//是不是水电模式的管子接头
	bool IsPipeLineLinker();
	//@郭子阳
	//获取linker对应的管线的类型
	static EObjectType GetPipeType(EObjectType PipeLinkerType);
	//获取管线对应的Linker类型
	static EObjectType GetPipeLinkerType(EObjectType PipeType);
	
};