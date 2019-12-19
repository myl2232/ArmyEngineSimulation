#pragma once

#include "ArmyObject.h"
#include "ArmyPipePoint.h"
#include "ArmyCommonTypes.h"
#include "../Data/ConstructionData/IArmyConstructionHelper.h"
using namespace FContentItemSpace;

class FArmyPipePoint;
class FArmyPipeline;
DECLARE_DELEGATE_TwoParams (FArmyLineVectorDelegate,TSharedPtr<FArmyPipeline>,const FVector&);

/**
* 管线类型
*/
enum EPipeType
{
  	PT_None              = 0x0000000000000000,
	PT_WeakElectricity   = 0x0000000000000001,// 弱电
	PT_StrongElectricity = 0x0000000000000002,// 强电
	PT_ColdWaterTube     = 0x0000000000000004, // 冷水管
	PT_HotWaterTube      = 0x0000000000000008,// 热水管
	PT_Drain             = 0x0000000000000010,// 排水管
	PT_Max               = 0x0000000000000020
}; 

/**
* 排水管类型
*/
enum EDrainType
{
	DT_None = 0, // 无
	DT_P,// p型管
	DT_S,//s型管
	DT_U,//U型管
};


//过桥弯，弯曲避让线的信息
struct FBridgeOverlyLineInfo
{
	FGuid LineID;
	FVector IntersectPoint;
	float LineRaduis;
	FVector Normal;

	FBridgeOverlyLineInfo(FGuid InLineID,FVector InIntersectPoint,float InRadius,FVector InNormal)
		: LineID(InLineID)
		,IntersectPoint(InIntersectPoint)
		,LineRaduis(InRadius)
		,Normal(InNormal)
	{

	}
};

// 过桥弯节点信息
struct ARMYSCENEDATA_API FBridgePointInfo
{
	TArray<TSharedPtr<FBridgeOverlyLineInfo>> LineInfoes; //跨越的线信息
	float OverlyLineRadius; //跨越的线半径
	FVector Start;
	FVector End;
	FGuid StartObjectID;
	FGuid EndObjectID;
	EObjectType LinkerType;
	FColor NormalColor;
	FColor ReformColor;
	FBridgePointInfo(FVector InStart,FVector InEnd,FGuid InStartID,FGuid InEndID,float InOverlyRadius,FColor InNormalColor,FColor InReformColor,EObjectType InLinkerType)
		:Start(InStart),
		End(InEnd),
		StartObjectID(InStartID),
		EndObjectID(InEndID),
		OverlyLineRadius(InOverlyRadius),
		NormalColor(InNormalColor),
		ReformColor(ReformColor),
		LinkerType(InLinkerType)
	{

	}
	//判断截断点信息是否咋该节点范围内
	bool Contain(TSharedPtr<FBridgeOverlyLineInfo>& LineInfo); 
	void AddBridgeOverlyLineInfo(TSharedPtr<FBridgeOverlyLineInfo>& LineInfo);
	FVector GetBottomLocation();
	FVector GetNearlyStart();
	FVector GetNearlyEnd();

};

/**
* 水电管线
*/
class ARMYSCENEDATA_API FArmyPipeline : public FArmyObject,public IArmyConstructionHelper
{
public:
	FArmyPipeline ();
	FArmyPipeline (FArmyPipeline* Copy);
	FArmyPipeline(TSharedPtr<FArmyPipePoint>& InStart , TSharedPtr<FArmyPipePoint>& InEnd,EObjectType ObjectType);
	virtual ~FArmyPipeline ();

	// FObject2D Interface Begin
	virtual void SerializeToJson (TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization (const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void Draw (FPrimitiveDrawInterface* PDI,const FSceneView* View) override;
	virtual bool IsSelected (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> SelectPoint (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient) override;
	virtual TSharedPtr<FArmyEditPoint> HoverPoint (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient) override;
	virtual bool Hover (const FVector& Pos,class UArmyEditorViewportClient* InViewportClient) override;
	virtual void GetVertexes (TArray<FVector>& OutVertexes) override;
	virtual void GetLines (TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs = false) override;
	virtual void ApplyTransform (const FTransform& Trans) override;
	virtual const FBox GetBounds () override;
	virtual void Refresh ()override;
	void SetReformState(bool bInReform);

	friend bool operator==(const FArmyPipeline& A,const FArmyPipeline& B)
	{
		return (A.StartPoint == B.StartPoint && A.EndPoint == B.EndPoint ) ||
			(A.EndPoint==B.StartPoint && A.StartPoint==B.EndPoint);
	}

	/** 判断当前管线是否创建ShapeActor */
	virtual bool IsCreate();

	void GeneratePipelineModel(UWorld* World, bool InBaseCenter = true);

	virtual void CreateMatrial();

	virtual void Update();
	
	virtual void UpdateWithPoints(bool bTrans = false);

    virtual void Create() override;

	virtual void PostModify(bool bTransaction=false) override;

public:
	TSharedPtr<FArmyPipeline> CopyPipeLine ();

	/** 移动线代理*/
	void  EditorApplyTranslation (const FVector& deltaVector);

	void  PostEditPointes (TArray<TSharedPtr<FArmyObject>>& Objectes);

	/**
	* 添加过桥弯点！顺序为距离起始点由近及远
	*/
	void AddBridgeBendPoint(TSharedPtr<FArmyPipeline> Line,TSharedPtr<FArmyPipePoint> point);
	void AddBridgeBendPoint(TSharedPtr<FArmyPipePoint> point);

	void GetBridgePointInfo(TArray<TSharedPtr<FBridgePointInfo>>& OutPointInfoes);

	/**
	* 删除过桥弯点
	*/
	void RemoveOverlayLine(TSharedPtr<FArmyPipeline>& Line);

	/**
	* 获取所有过桥弯的点
	*/
	void GetAllBridgeBendPoints(TArray<TSharedPtr<FArmyPipePoint>>& OutArray);

	/** 绘制 电线*/
	virtual void CreateElectric();

	/** 绘制 过桥弯缺口*/
	virtual bool CreateBridgeBend(FVector& InStart,FVector& InEnd, TArray<FVector>& Verties);

	/** 圆柱数据*/
	virtual void DrawCylinder(FVector InStart, FVector InEnd, TArray<FVector>& Verties);

	/** 绘制拐角返回切线点 */
	virtual FVector CreateElectricTurn (TSharedPtr<FArmyPipePoint> Point,TArray<FVector>& Verties);

	/** 判断点位是否是线的端点 */
	bool IsPointOnLine(TSharedPtr<FArmyPipePoint> Point);

	/** 判断是否和另一个线相交*/
	bool IsIntersect(TSharedPtr<FArmyPipeline>& OtherLine);

	/** 判断是否和另一个线相交*/
	void Intersect(TSharedPtr<FArmyPipeline> OtherLine,FVector& OutPoint0,FVector& OutPoint1);

	/**获取该线和另一条线的法线*/
	static FVector CrossProduct(TSharedPtr<FArmyPipeline>& Line1,TSharedPtr<FArmyPipeline>& Line2);

	virtual void PostDelete()override;

	virtual void DeleteSelf(bool bTrans = false);

	/**
	* 改变管线颜色
	*/
	void ChangeColor();

	/**
	* 获取线的方向
	*/
	FVector GetDirection();
	//删除actor
	void DestroyPipelineModel();

	// FObject2D Interface End

	FORCEINLINE const FVector GetStart ()
	{
		return GetPointLocation(StartPoint);
	}
	FORCEINLINE const FVector GetEnd ()
	{
		return GetPointLocation (EndPoint);
	}

	const FVector GetRealStart();
	const FVector GetRealEnd();
	FORCEINLINE bool IsValid () const
	{
		return !StartPoint->GetLocation() .Equals ( EndPoint->GetLocation() );
	}

	void SetColor ( const FColor InColor );
	FORCEINLINE FColor GetColor ()
	{
		return Color;
	}


	FORCEINLINE void SetReformColor (const FColor InColor)
	{
		RefromColor = InColor;
	}

	FORCEINLINE FColor GetReformColor ()
	{
		return RefromColor;
	}

	FORCEINLINE void SetPointColor (const FColor InColor)
	{
		PointColor = InColor;
	}

	FORCEINLINE FColor GetPointColor ()
	{
		return PointColor;
	}

	FORCEINLINE void SetPointRefromColor (const FColor InColor)
	{
		PointRefromColor = InColor;
	}

	FORCEINLINE FColor GetPointRefromColor ()
	{
		return PointRefromColor;
	}

	FORCEINLINE void SetRadius (const float InRadius)
	{
		Diameter = InRadius;
		Radius = InRadius / 2;
	}

	FORCEINLINE float GetRadius ()
	{
		return Radius;
	}

	FORCEINLINE int32 GetDiameter ()
	{
		return Diameter;
	}

	FORCEINLINE float GetLenght ()
	{
		FVector Director = (GetEnd()-GetStart());
		return Director.Size();
	}

	FString GetRadiusStr ()
	{
		return FString::FromInt(GetDiameter());
	};

	FORCEINLINE EPipeType GetPipeType(){  return PipeType; }

	void SetStart (const FVector Pos);
	void SetEnd (const FVector Pos);

	void SetStartV (const FVector Pos)
	{
		if(!StartPoint->IsBridgeBend ())
		{
			StartPoint->SetLocation (Pos);
		}
	}
	void SetEndV (const FVector Pos)
	{
		if(!EndPoint->IsBridgeBend ())
		{
			EndPoint->SetLocation (Pos);
		}
	}

	void SetStartPoint(TSharedPtr<FArmyPipePoint>& InPoint);
	void SetEndPoint (TSharedPtr<FArmyPipePoint>& InPoint);

	FVector GetNormal(FVector Start);
	FVector GetOtherPoint(FVector InPoint);
	TSharedPtr<FArmyPipePoint> GetOtherPoint(TSharedPtr<FArmyPipePoint> Point);
	bool ModifyStartPoint(TSharedPtr<FArmyPipePoint> NewPoint);
	bool ModifyEndPoint (TSharedPtr<FArmyPipePoint> NewPoint);
	bool ModifyPoint(TSharedPtr<FArmyPipePoint> OldPoint,TSharedPtr<FArmyPipePoint> NewPoint);

	FVector GetPointLocation(TSharedPtr<FArmyPipePoint> Point);

	float GetOffsetGroundHeight (){		return OffsetGroundHeight;	}

	void SetOffsetGroundHeight (float InOffsetGroundHeight){OffsetGroundHeight = InOffsetGroundHeight;}

	virtual void SetType(EObjectType InType) override;

	void GetConnectLines(TArray<FObjectPtr>& Lines);

	AActor* GetRelevanceActor()override;
private:
	/** 取消选中操作点 */
	void DeselectPoints ();
private:
	UMaterialInstanceDynamic* AxisMaterial;
	
public:
	TSharedPtr<FArmyPipePoint> StartPoint;
	TSharedPtr<FArmyPipePoint> EndPoint;

	bool IsChanged = false;
	/**
	* 相交的线以及相交点
	*/
	//TMap< FGuid,TSharedPtr<FArmyPipePoint>> AttachmentPoints; // 过桥弯点在该管线上
	//TMap< FGuid,TSharedPtr<FArmyPipePoint>> OverlayPoints; // 过桥弯点不在该管线上，越过
	TArray<FGuid> OverlayLines; // 过桥弯不在该线上
	TArray<TSharedPtr<FBridgeOverlyLineInfo>> BridgeInfoes;
	TArray<TSharedPtr<FArmyPipePoint>> AttachmentPoints;

	//三维管线
	class AXRPipelineActor* PipeLineActor;

	/** 管线离地高度*/
	float OffsetGroundHeight;

	/** 是否显示操作点 */
	bool bShowPoint;

	/** 颜色*/
	FColor Color; 

	/** 断电改颜色*/
	FColor RefromColor;

	/** 节点断电改颜色*/
	FColor PointRefromColor;

	/** 节点颜色*/
	FColor PointColor;

	/** 半径 */
	int32 Radius;

	/**直径*/
	int32 Diameter;

	/**排水管管型*/
	EDrainType DrainType;

	/**
	*商品代码
	*/
	int32 ProductID;

	/**
	* 强弱电，冷热水管，排水管
	*/
	EPipeType PipeType; 

	bool bReform;// 是否是断电该

	/** 材质*/
	class UMaterialInstanceDynamic* MaterialInsDy;

	FArmyObjectesDelegate PostEditDelegate;

	FArmyObjectesDelegate PostDeleteDelegate;

	FArmyObjectesTransactionDelegate ModifyDelegate;
	//@郭子阳 获得所在的房间，用于施工项
	TSharedPtr<class FArmyRoom> GetRoom() override { return nullptr; };
	virtual void OnRoomSpaceIDChanged(int32 NewSpaceID) {};
};

REGISTERCLASS(FArmyPipeline)