#pragma once
#include "ArmyObject.h"
#include "ArmyCommonTypes.h"
#include "ArmyActorConstant.h"
#include "IArmyConstructionHelper.h"
using namespace FContentItemSpace;

class FArmyPoint;
class FArmyPolyline;
class FArmyEditPoint;

 /**
 * 插座标记
 */
 enum ESocketFlag
 {
   	ESF_None, 
	ESF_Normal = 0, //普通
	ESF_AirConditioner = 1, //空调
	ESF_Refrigerator = 2, //冰箱
	ESF_Power = 3, //动力
 	ESF_Max
 };

 //点位放置位置标记
 enum EPointLocationFlag
 {
	 EPLF_NORMAL = 1, //无放置限制
	 EPLF_ROOMSPACE = 1 << 1, //点位仅可放置在房间内
	 EPLF_INNERNALWALLLINE = 1 << 2, //点位仅可放置在内墙线上
 };

 //断点改造标记
 enum EBreakPointType
 {
	 NativePoint=0, //原始点位
	 SubstitutePoint=1, //替换后的点位
	 NewPoint=2 //新增点位
 };
 //可替换的断点构件ID列表
 static EComponentID ReplaceableBreakPointIDs[] = {
	 EComponentID::EC_EleBoxL_Point,   //弱电箱
	 EComponentID::EC_EleBoxH_Point, //强电箱
	 EComponentID::EC_Water_Hot_Point,  //热水点位
	 EComponentID::EC_Gas_Meter ,// = 1201 //燃气表
	// EComponentID::EC_Water_Normal_Point, //中水点位
	 EComponentID::EC_Water_Separator_Point,//分集水器
	 EComponentID::EC_Water_Chilled_Point, //= 17,//冷水点位
	 EComponentID::EC_Water_Hot_Chilled_Point, //= 18,//冷热水	
	// EComponentID::EC_Water_Supply, // 给水点位 入户上水
 };
 //可移动的断点构件ID列表
 static EComponentID MoveableBreakPointIDs[] = {
	 EComponentID::EC_EleBoxL_Point,
	 EComponentID::EC_EleBoxH_Point,
	 EComponentID::EC_Water_Hot_Point,
	 EComponentID::EC_Gas_Meter,// = 1201 //燃气表
	 EComponentID::EC_Water_Normal_Point, //中水点位
	 EComponentID::EC_Water_Separator_Point,//分集水器
	 EComponentID::EC_Water_Chilled_Point, //= 17,//冷水点位
	 EComponentID::EC_Water_Hot_Chilled_Point, //= 18,//冷热水
	 EComponentID::EC_Basin,//= 60,//地漏下水
	 EComponentID::EC_Water_Basin,// = 59,//水盆下水
	 EComponentID::EC_Closestool,// = 31,//马桶下水
	// EComponentID:: EC_Water_Supply, // 给水点位 入户上水


 };


// 附着点信息
struct EditorPointInfo : TSharedFromThis<EditorPointInfo>
{ 
	uint32 EPIID; //  id
	FVector EPIPoint3D; // 3d下显示的附着点
	FVector EPIPoint2D;//2d下显示的附着点
	FVector CEPIPoint3D;//3d下捕捉的附着点
	bool bEnable;//能否使用
	uint32 FurnitureCompentType; //点位的组件类型
	FGuid FurnitureID;

	bool operator==( const EditorPointInfo& InOther )const
	{
		return EPIID == InOther.EPIID &&FurnitureID == InOther.FurnitureID && FurnitureCompentType == InOther.FurnitureCompentType;
	}
};

struct GeometryPoint : TSharedFromThis<GeometryPoint>
{
public:
	FVector2D Start;
	FVector2D End;
	FVector2D SwtichPoint;
	float Angle;
public:
	GeometryPoint(FVector2D InStart,FVector2D InEnd,FVector2D InSwtichPoint) :Start(InStart),End(InEnd),SwtichPoint(InSwtichPoint)
	{
		FVector2D Direction = (End - Start).GetSafeNormal();
		if (Direction.IsZero())
		{
			Direction.X = 0;
			Direction.Y = 1;
		}
		FVector2D Normal = (InStart -SwtichPoint).GetSafeNormal();
		Normal =FVector2D( FVector::CrossProduct(FVector(Normal,0),FVector::UpVector));
		float dot = FVector2D::DotProduct(Direction,Normal);
		Angle = FMath::Acos( FVector2D::CrossProduct(Direction,Normal));
		Angle *= (180.f/PI);
		if (dot>0)
		{
			Angle = 360 - Angle;
		}
	}

	float GetSize()
	{
		FVector2D Direction = End - SwtichPoint;
		return Direction.Size();
	}
};

class ARMYSCENEDATA_API FArmyFurniture :public FArmyObject ,public IArmyConstructionHelper
{
public:
	FArmyFurniture();
	TSharedPtr<FArmyFurniture> CopyFurniture();

	virtual ~FArmyFurniture();

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	//////////////////////////////////////////////////////////////////////////
	virtual TSharedPtr<FArmyEditPoint> SelectPoint (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient) override;
	virtual	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	virtual void SetState(EObjectState InState)override;
	virtual bool IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)override;
	virtual bool Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)override;
	virtual const FBox GetBounds()override;
	virtual const FBox GetPreBounds() override;
	virtual const FVector GetBasePos()override;
	virtual const void GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)override;
	virtual void ApplyTransform(const FTransform& Trans) override;
	virtual void GetTransForm(FRotator& InRot, FVector& InTran, FVector& InScale) const override;
	virtual FVector TranfromVertex(const FVector& InV) const;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) override;
    virtual void Generate(UWorld* InWorld) override;
    virtual void Destroy() override;
	
	void SetPosition(const FVector& InPos);
	void SetRotation(const FRotator& InRot);
	void SetScale(const FVector& InScale);
	void SetTransform(const FTransform& InTrans);

	void  SetPolys(TArray<TSharedPtr<FArmyPolyline>>& InPolys);
	void  SetPoints(TArray<TSharedPtr<FArmyPoint>>& InPoints);
	void  AddPoly(TSharedPtr<FArmyPolyline> InPoly);
	void  AddPoint(TSharedPtr<FArmyPoint> InPoint);
	void  BaseSizeChanged();
	void  InitOperationPoint();

	FVector GetLocation(FVector& InLocation);

	float GetAltitude();
	void SetAltitude(float InAltitude);

	void SetId(int32 id) { FurnitureId = id; }
	int32 GetId()const { return FurnitureId; }

	void Generate3DModel(const FString& InModelPath,UWorld* InWorld);

	/**  更新数据*/
	void UpdateData();

	/**
	*InRelativeLocation 相对位置
	*/
	void ConversionLocation(class UStaticMeshComponent* InComponent, FVector& InRelativeLocation,FRotator& furnitureRotator,FVector& OutLocation);


	TSharedPtr<FArmyEditPoint> GetBaseEditPoint() const { return BaseEditPoint; };
	TSharedPtr<FArmyEditPoint> GetOperationPoint(uint32 Index) const;
	uint32 GetOperationIndex(TSharedPtr<FArmyEditPoint> InPoint) const;

	const TMap<uint32, FVector>& GetCapturePointIn3DModel() const { return CapturePoints3D; };
	const TMap<uint32,FVector>& GetEditPoints3D ()const
	{
		return EditPoints3D;
	}

	const TArray<struct FSimpleElementVertex>& GetElementVertex() const { return LineVertices; };

	void SetBeamWidth(float InWidth);
	float GetBeamWidth()const { return BeamWidth; }
	
	void SetBeamLength(float InLength);
	float GetBeamLength()const { return BeamLength; }

	void SetThickness(float InThickness);
	float GetThickness()const { return BeamThickness; }

	void RebuildLenghtWidth();

	void GenerateBeamModel(UWorld* InWorld);
	void BuildModel(float InBeamThickness, float Height, UWorld* InWorld);
	
	const TArray<TSharedPtr<FArmyPolyline>>& GetPolys() const;

	void SetFurnitureModelActor(class AXRFurnitureActor* NewActor);

	class AActor* GetActor ();

	void SetTransverse (bool InTransverse){bTransverse = InTransverse;};//是否横向显示 

	//燃气表
	bool IsGasMeter();
	//分集水器
	bool IsWaterSeparator();
	//燃气主管道
	bool IsGasMainPipe();
	//下水主管道
	bool IsDownWarterMainPipe();

	//中水点位
	bool IsWaterNormalPoint();
	
	//入户上水 给水点位
	bool IsWaterSupply();

	//地漏下水
	bool IsBasin();

	//水盆下水
	bool IsWaterBasin();

	//马桶下水
	bool IsClosestool();
	
	//是否是电箱
	bool IsEleBox();
	//是否是开关
	bool IsSwitch();
	//是否是插座
	bool IsSocket();
	//是否是冷热水点位
	bool IsToilet();
	//是否是预装点位
	bool IsPreLoad();
	//是否是强电
	bool IsStrongElectirc();
	//是否是弱电
	bool IsWeakElectirc ();
	//是否是热水管
	bool IsHotWater ();
	//是否是冷水管
	bool IsColdWater();
	//是否是排水管
	bool IsDrain();
	//是否是普通强电插座
	bool IsNormalSocket();

	void GetPointMap(class UStaticMeshComponent* StaticMeshComp,TMap<uint32,FVector>& OutMap);

	void GetPointMapes(TMap<uint32,FVector>& OutMap3,TMap<uint32,FVector>& OutMap2);

	void GetPoint3DMap(TMap<uint32,FVector>& OutMap3);

	TMap<uint32,TSharedPtr<FArmyEditPoint>> GetBaseEditPoints(){	return BaseEditPoints;	}

	TArray<FVector> GetBottomVerticeList(EObjectType InType);

	//是否显示关联
	void EnableShowRelate(bool InRelate){bShowRelate = InRelate;}

	/**
	 *@ 获取或者设置是否使用自定义颜色
	 * @return bool - true 表示使用自定义颜色，否则不使用
	 */
	bool GetbUseCustomColor() const { return bUseCustomColor; }
	void SetbUseCustomColor(bool InbUseCustomColor) { bUseCustomColor = InbUseCustomColor; }


	void SetSocketType(ESocketFlag InESFType){ eSockType = InESFType; }
	ESocketFlag GetSocketType(){return eSockType;}

	/**
	 * @ 获取或者设置自定义颜色
	 * @return FLinearColor - 颜色值
	 */
	FLinearColor GetCustomoColor() const { return CustomoColor; }
	void SetCustomoColor(FLinearColor InCustomoColor) { CustomoColor = InCustomoColor; }

	///** 查找相关联的房间*/
	void CalcRelatedRoom();

	/**@ 设置相关联的房间*/
	void SetRelatedRoom(TWeakPtr<class FArmyRoom> InRoom) { RelatedRoom = InRoom; }
	TWeakPtr<class FArmyRoom> GetRelateRoom()const { return RelatedRoom; }

	/**	@ 设置点位放置位置标记*/
	void SetPlaceLocationFlag(EPointLocationFlag Location) { EPlaceLocationFlag = Location; }
	/**	获取点位放置位置标记*/
	EPointLocationFlag GetPlaceLocationFlag() const{ return EPlaceLocationFlag; }

	bool GetbDragModify() const { return bDragModify; }
	void SetbDragModify(bool InbDragModify) { bDragModify = InbDragModify; }

	//是否为创建操作
	bool GetbCreateOperation() const { return bCreateOperation; }
	void SetbCreateOperation(bool InbCreateOperation) { bCreateOperation = InbCreateOperation; }
	
	/**
	* 预计算点位标尺修改是否越界，此操作不修改点位数据
	* @param trans - FTransform - 变换矩阵
	* @param Flag - int32 -
	* @return bool - true表示操作越界，否则不越界
	*/
	bool PreCalTransformIllegal(FTransform trans, int32 Flag);


	//获取和设置构件相关信息
	TSharedPtr<FFurnitureProperty> GetFurniturePro() const { return FurniturePro; }
	void SetFurniturePro(TSharedPtr<FFurnitureProperty> InFurniturePro) { FurniturePro = InFurniturePro; }

	//修改对应原始户型的拆改Furniture的属性数据(InType:0-长度，1-厚度，2-高度，3-离地高度，4-管径)
	void ModifyFurniturePro_LayoutData(uint8 InType, float InValue);

	/**
	 * 设置关联的模型可选性
	 * @param bIsSelected - bool - true表示模型可被选择，否则不可被选择
	 * @return bool - 
	 */
	void SetRelatedActorSelected(bool bIsSelected);

	/**
	* 同步修改构件关联的actor位置、大小、角度，此处仅用于原始点位
	* @param _RelatedActor - AActor * - 关联的Actor
	* @return void - true表示修改成功，否则失败
	*/
	bool ModifyRelatedActorTransform(AActor * _RelatedActor);
	/**
	* 同步修改构件关联的FurnitureActor(数字化模型)位置、大小、角度，此处仅用于原始点位
	* @return bool - true表示修改成功，否则失败
	*/
	bool ModifyRelatedFurnitureActorTransform();

	FVector GetPivortPoint() { return PivotPoint; }
private:
	FVector Vertexe(const FVector& InV,bool Invert = false) const;

	TArray<struct FSimpleElementVertex> LineVertices;

	void UpdateSimpleElementVertex();

	bool Culler(const FSceneView* InView);

	void SetReform (bool InReform){	bReform = InReform;}

	/**
	 * 获取墙线点位所在的线
	 * @param OutLine - TSharedPtr<FArmyLine> & - 得到的线
	 * @return void - 
	 */
	void GetRelatedLine(TSharedPtr<FArmyLine> & OutLine);

public:
   // float Altitude = 0; //高度
	int32 ComponentID = 0;
	TArray<float> Altitudes;

    FTransform LocalTransform;

	uint32 ComponentType;

	EComponentID GetComponentID()
	{
		return (EComponentID)ComponentType;
	}

	uint32 BelongClass = 0;

	bool bReform;// 是否是断电该

	int32 SwitchCoupletNum;

	//开关关联的普通灯具
	//int32 开关的联数  @郭子阳注
	//FGuid 灯具对应的Object ID
	TMultiMap<int32,FGuid> RelatedFurnitureMap;

	//@郭子阳
	//开关关联的灯带
	//int32 开关的联数 
	//FGuid 灯带对应的Path ID
	TMultiMap<int32, FGuid> RelatedExtrusionLightMap;

protected:
	int32 FurnitureId;
	/*构建模型*/
	class AXRFurnitureActor* FurnitureModelActor;

	float BeamWidth = 40;
	float BeamThickness;
	float BeamLength = 100;

	FBox BondingBox = FBox(ForceInitToZero);
	TArray<TSharedPtr<FArmyPolyline>> Polys;
	TArray<TSharedPtr<FArmyPoint>> Points;
	class AXRShapeActor* BeamModel;
	FVector PivotPoint;//原始基点
	TSharedPtr<FArmyEditPoint> BaseEditPoint;//操作基点

	TMap<uint32, TSharedPtr<FArmyEditPoint>> BaseEditPoints;//基点 索引值-操作点（捕捉点 id从11开始，与3D对应点的索引值为：垂直方向从左到右：2D（15,16...），3D从上到下从左到右排序（15,16...）；水平方向从上到下从左到右：2D（11,12），3D（11,12,13,14））
	TMap<uint32,FVector> CapturePoints3D;//3D 模式下模型对应的捕捉点（根据模型包围盒，按一定比例计算求得）
	TMap<uint32,FVector> EditPoints3D;//3D 模式下模型对应的捕捉点（根据模型包围盒，按一定比例计算求得）
	TArray<TSharedPtr<EditorPointInfo>> EditPointArr;

	static TMap<FString, int32> TypeNameMap;

	bool bTransverse;//是否横向显示

	bool bShowRelate; //是否显示关联

	FVector NowLocatoin;

	//@
	bool bUseCustomColor;//是否使用自定义颜色
	FLinearColor CustomoColor;//自定义颜色
	//设置标记
	ESocketFlag eSockType;

	/**@ 关联的房间*/
	TWeakPtr<class FArmyRoom> RelatedRoom;

	/**@欧石楠 更新虚线标注*/
	void UpdateDashLine();
	/**@欧石楠 绘制标尺标注*/
	void DrawDashLine(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	//墙线点位标尺线，不同于四轴标尺
	TSharedPtr<class FArmyRulerLine> FirstRulerLine;
	void OnFirstInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class FArmyRulerLine> SecondRulerLine;
	void OnSecondInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	float OffsetDis = 30.f;//标尺偏移距离
private:
	//点位放置位置
	EPointLocationFlag EPlaceLocationFlag = EPLF_NORMAL;

//	FString RelatedActorPath;

	bool bDragModify = false;//修改
	bool bCreateOperation = false;//创建
	////-------------------------PointLocation-------------------------	

	//float PointPositionAltitude;//离地高度
	//float PipeRadius;//管径

	//构件属性相关信息
	TSharedPtr<class FFurnitureProperty> FurniturePro;


//**************断点改造相关**************
	

public:
	//构件actor显隐刷新代理
	DECLARE_DELEGATE_TwoParams(FFurnitureActorShowUpdated, TSharedRef<FArmyFurniture>, bool);

	FFurnitureActorShowUpdated OnFurnitureActorShowUpdated;

	virtual void SetRelevanceActor(class AActor* InActor);

	//构建偏移事件
	DECLARE_DELEGATE_OneParam(FFurnitureOffseted, FVector)
	FFurnitureOffseted FurnitureOffsetedDelegate;
	//设置断点类型
	void SetBreakPointType(EBreakPointType NewType);
	//获取断点类型
	EBreakPointType GetBreakPointType() { return BreakPointType; };

	//是否是原始点位
	bool IsOrignalPoint() { return BreakPointType == EBreakPointType::NativePoint; };
	//是否是替代品
	bool IsSustitute() { return BreakPointType == EBreakPointType::SubstitutePoint; };

	//是否已停用
	bool IsBPStopped();

	//设置是否停用
	void SetBPStop(bool value);

	//构建停用事件
	DECLARE_DELEGATE_OneParam(FFurnitureStopped, bool)
	FFurnitureOffseted FurnitureStoppedDelegate;

	//获取偏移
	FVector GetOffset();

	//设置偏移量 单位cm
	/*@
	*  InfluenceSubstitute 是不是同样也对替代品设置偏移
	*/
	void SetOffset(FVector NewOffset,bool InfluenceSubstitute=true);

	//开启断点改造
	void StartReforming();

	void StopReforming();

	bool IsBPreforming();

	void OnActorMove(AActor * actor);

	bool bIsBPreforming = false;
	bool StopUsing = false;
private:

	//原始actor的位置
	FVector NativeLocation;

	//断点类型,默认为新增点位
	EBreakPointType BreakPointType= EBreakPointType::NewPoint;

	bool bisReplaced=false;
public:
	//相关联的构件。 原始点位与替代者互相关联；
	TWeakPtr<FArmyFurniture> RelativeFurniture=nullptr;

	FGuid RelativeFurnitureID;

	bool IsReplaced();

	void SetNativeLocation(FVector Location);
	bool isNativeLocationSet = false;

	/*获取应当显示的构件
	* @MustShowNativeFurniture 是不是一定要原本的构件
	*/
	TSharedRef<FArmyFurniture> GetRepresentedFurniture(bool MustShowNativeFurniture = false);
	
	//在3d和右侧物体列表的显示
	void UpdateShow();

	//是否可以断点改造
	bool CanNativeBPReform();
	//断点是否可移动
	bool CanBPMove();
	//断点是否可停用
	bool CanBPStopUsing();
	//断点是否可替换
	bool CanBPReplace();

	//替换，若果是二次替换,返回被替换掉的构件，否则返回null
	TWeakPtr<FArmyFurniture> Replace(TWeakPtr<FArmyFurniture> NewRelativeFurniture);

	//是否可以导出算量清单
	bool CanOutPutCalcList();

private:
		bool HaveMoveHandle = false;
		FDelegateHandle MoveHandle;
	//**************断点改造结束**************
public:
    //****************施工项相关******************

	//获取施工项参数
	struct ConstructionPatameters GetConstructionParameter();

	TSharedPtr<FArmyRoom> GetRoom() override;

	//@郭子阳 当房间ID改变时发生
	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) override;
};
REGISTERCLASS(FArmyFurniture)
