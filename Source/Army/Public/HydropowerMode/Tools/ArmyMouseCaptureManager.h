#pragma once
#include "CoreMinimal.h"
#include "Math/Matrix.h"
#include "ArmyLine.h"
#include "ArmyObjectVisualizer.h"

struct EditorPointInfo;

struct HComponentProxy : public HXRObjectVisProxy
{
	DECLARE_HIT_PROXY ();

	HComponentProxy (const FArmyObject* InComponent,int32 InKeyIndex)
		: HXRObjectVisProxy (InComponent),KeyIndex (InKeyIndex)
	{
	}

	int32 KeyIndex;
};

class FArmyMouseCaptureManager : public TSharedFromThis<FArmyMouseCaptureManager>
{	
public:
	/**
	* Description
	*/
	enum EMCTYPE
	{
		EMC_None		= 1,
		EMC_Corner		= 1<<1, // 墙角
		EMC_Line		= 1<<2,	// 线
		EMC_LinePoint	= 1<<3, //线上点
		EMC_Adsorption	= 1<<4, //捕捉点位吸附点
		EMC_Max
	};
public:

	FArmyMouseCaptureManager ();
	~FArmyMouseCaptureManager ();

	/**	获取当前捕获的点，如果返回值false则表示没有捕获点*/
	FVector GetCapturePoint();

	FVector GetMovePoint();

	FVector GetCaptureNormal();

	FVector GetCaptureNormal(FVector& InLocation);

	FVector GetCaptureWallDirection(FVector& InLocation);

	HAdsorbDetect GetCaptureInfo();
	HAdsorbDetect GetCaptureInfo(FVector& InLocation);

	/**	绘制捕捉到的点*/
	void Draw(class UArmyEditorViewportClient* InViewPortClient,FViewport* InViewport, FCanvas* SceneCanvas);

	void Draw (const FSceneView* View,FPrimitiveDrawInterface* PDI);

	/***/
	bool MouseMove (UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y);

	/**	重置辅助容器*/
	void Reset();

	/**	捕获实时计算，输入鼠标屏幕坐标*/
	void Capture(class UArmyEditorViewportClient* InViewPortClient, int32 x, int32 y);

	/**
	* 捕捉墙角
	*/
	bool CaptureCorner(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 捕捉墙面和顶底面
	*/
	bool CapturePlane(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 捕捉线上点
	*/
	bool CaptureLinePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 捕捉线
	*/
	bool CaptureLine(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 捕捉点位吸附点
	*/
	bool CaptureAdsorptionPoint (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 顶视图捕捉墙面
	*/
	bool CaptureWallInTopView(class UArmyEditorViewportClient* InViewPortClient, int32 x, int32 y);

	/**
	* 捕捉坐标轴
	*/
	bool CaptureCoordinatePoint (class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 捕捉X轴
	*/
	bool CaptureXCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation);

	/**
	* 捕捉Y轴
	*/
	bool CaptureYCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation);

	/**
	* 捕捉Z轴
	*/
	bool CaptureZCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation);

	/**
	*捕捉平面视图坐标轴
	*/
	bool CaptureXYCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y,FVector& OutLocation);

	/**
	* 捕捉45°坐标轴
	*/

	bool CaptureAngle45Coordinate(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 捕捉3D坐标轴
	*/
	bool CanCaptureCoordinatePoint(class UArmyEditorViewportClient* InViewPortClient,int32 x,int32 y);

	/**
	* 获取起始点平面的交点
	*/
	bool GetPointPlane(class UArmyEditorViewportClient* InViewPortClient,FPlane& BasePlane,int32 x,int32 y,FVector& OutPoint);

	void SetCaptureModel (uint32 InCaptureModel){CaptureModel = InCaptureModel;};


	//设置选中类型 水电点位。捕捉点需要
	void SetObjectType(EObjectType InObjectType);

	void SetEnable(bool InEnable);

	void GetFurnitures(TArray<TSharedPtr< class FArmyFurniture> > & FunitureObjects);

	bool IsShowEditPoint(TSharedPtr< class FArmyFurniture> Item);

	/**
	* 判断是否在同一房间内
	*/
	bool InSameRoom(FVector& StartPoint,FVector EndPoint);
public:
	static FArmyMouseCaptureManager* Get ();

	void SetStartPoint (FVector InStartPoint,bool InEnableCaptureCoordinate = false);

	void SetTopView(bool InTopView){ bTopView = InTopView; }

	void SetSelectObject(TSharedPtr<FArmyObject> InObject){ SelectObject = InObject; };

	void SetCaputureActor(bool InCaputureActor){bCaputureActor = InCaputureActor;};

	/**
	* 点是否在墙上
	*/
static	bool IsOnWall(FVector& InPoint);

static	bool IsOnWall(FVector& InPoint, FVector&WallDirection,FVector & WallNormal);
	/**
	* 点是否在地面
	*/
static	bool IsOnFloor(FVector& InPoint);

	/**
	* 点是否在顶面
	*/
static	bool IsOnTop(FVector& InPoint);

	/**
	* 点是否在地面or墙面
	*/
static	bool IsOnFloorOrTop(FVector& InPoint);

	/*
	*点是否在墙角
	*/
	bool IsOnCorner(FVector& InPoint);

	void SetCaptureNormal(FVector& InNormal)
	{
		MouseCaptureNormal = InNormal;
	}
public:
	FVector MouseCapturePoint;
	FVector MouseCaptureNormal;
	TMap<FGuid,TArray<EditorPointInfo> > CapturePointMap;
private:
	FVector StartPoint;
	bool bEnableCaptureCoordinate = false;
	uint32 CaptureModel;
	EObjectType ObjectType;

	TSharedPtr<FArmyObject> SelectObject;

	static FArmyMouseCaptureManager* Singleton ;
	bool bEnable;
	/**鼠标捕捉的是否的附着点*/
	bool bCaptureAttachment;
	//是否是3d顶视图
	bool bTopView;
	//是否捕捉选中物体
	bool bCaputureActor;
	//是否捕捉到吸附点
	bool bAdsorptionPoint;//


	FVector ForwardVector;
	FVector RightVector;
	FVector UpVector;

private :
	HAdsorbDetect CurrentInfo;
};