#pragma once
#include "CoreMinimal.h"
#include "Math/Matrix.h"
#include "ArmyLine.h"

class ARMYTOOLS_API FArmyMouseCapture : public TSharedFromThis<FArmyMouseCapture>
{
public:
	enum CaptruePointType
	{
		PT_None,
		PT_VertexCaptrue, //端点
		PT_Perpendicular, //垂足
		PT_Intersection,  //交点
		PT_Midpoint,	  //中点
		PT_Closest		  //最近点
	};
	enum DrawModel
	{
		Model_VertexCaptrue = 1,//端点捕捉
		Model_Ortho = 1 << 1,  //正交模式
		Model_Perpendicular = 1 << 2, //垂足捕捉
		Model_Auxiliary = 1 << 3, //辅助线
		Model_LineRange = 1 << 4, //线捕捉，对应CAD范围捕捉
		Model_Closest = 1 << 5, //最近点捕捉
		Model_Midpoint = 1 << 6, //中点捕捉
		Model_IntersectionPoint = 1 << 7, //交点捕捉
	};
	//构建辅助线的信息
	struct  AuxiliaryLineInfo
	{
		AuxiliaryLineInfo(FVector2D InStart, FVector2D InEnd, FVector2D InAux, bool InAuxDraw = false,bool InFullScreen = false)
			:StartPoint(InStart), EndPoint(InEnd), AuxiliaryPoint(InAux), AuxiliaryDraw(InAuxDraw), IsFullScreen(InFullScreen)
		{}
		FVector2D StartPoint = FVector2D(ForceInitToZero);
		FVector2D EndPoint = FVector2D(ForceInitToZero);
		FVector2D AuxiliaryPoint = FVector2D(ForceInitToZero);//辅助点
		bool AuxiliaryDraw = false;//绘制辅助线时，是否绘制参考点标记
		bool IsFullScreen = false;//辅助线是否满屏
	};
	struct CapturePointInfo
	{
		CaptruePointType PointType = PT_None;

		TSharedPtr<"FArmyEditPoint> CapturePointPtr;

		FVector2D CapturePoint = FVector2D(0, 0);

		bool AuxiliaryLineOK = false;

		TArray<AuxiliaryLineInfo> RelatedLines;

		void Reset()
		{
			PointType = PT_None;
			AuxiliaryLineOK = false;
			RelatedLines.Empty();
		}

		bool operator ==(const CapturePointInfo& Other)const
		{
			return (PointType == Other.PointType && CapturePoint == Other.CapturePoint);
		}
		bool operator!=(const CapturePointInfo& Other)
		{
			return (PointType != Other.PointType || CapturePoint != Other.CapturePoint);
		}
		void operator=(const CapturePointInfo& Other)
		{
			PointType = Other.PointType;
			CapturePoint = Other.CapturePoint;
			CapturePointPtr = Other.CapturePointPtr;
			AuxiliaryLineOK = Other.AuxiliaryLineOK;
			RelatedLines = Other.RelatedLines;
		}
		bool operator<(const CapturePointInfo& InOther) const
		{
			return true;
		}
	};
	FArmyMouseCapture();
	~FArmyMouseCapture();

	const TSharedPtr<"FArmyEditPoint> GetCapturePointPtr() const;

	/**	获取当前捕获的点，如果返回值false则表示没有捕获点*/
	FVector2D GetCapturePoint();

	FVector2D GetMovePoint();

	TSharedPtr<"FArmyLine> GetCaptureLine() { return CurrentCaptrueLines.Num() > 0 ? CurrentCaptrueLines.Top().Pin() : NULL; };

	/**	绘制捕捉到的点*/
	void Draw(class UArmyEditorViewportClient* InViewPortClient,FViewport* InViewport, FCanvas* SceneCanvas);

	/**	设置当前绘制点，正交判断的起点
	*@IsContinuous 是不是接着上一条线绘制
	*/
	void SetCurrentPoint(FVector2D InCurrent,bool IsContinuous=true);

	void AddAuxiliaryPoint(TSharedPtr<"FArmyEditPoint> InPoint);

	void ClearAuxiliaryPoints();

	/**	设置捕捉平面，该平面的物体的点线信息都是相对该平面的坐标值*/
	void SetRefCoordinateSystem(const FVector& InBasePos, const FVector& InXDir, const FVector& InYDir, const FVector& InNormal);

	/**	获取捕捉基面*/
	const FPlane& GetPlane() const;

	/**	重置辅助容器*/
	void Reset();

	/**	捕获实时计算，输入鼠标屏幕坐标*/
	bool Capture(class UArmyEditorViewportClient* InViewPortClient, int32 x, int32 y, const TArray< TSharedPtr<class "FArmyLine> >& InData, const TArray< TSharedPtr<class "FArmyPoint> >& InPointData);

private:
	/**	正交计算*/
	bool OrthoAmend(class UArmyEditorViewportClient* InViewPortClient,int32& x, int32& y);

	/**	用函数来控制捕获状态*/
	//void SetCapture(bool InCapture);

	FVector ToBasePlane(const FVector& InV);
	FVector FromBasePlane(const FVector& InV);
public:
	uint32 CaptureModel;

	bool EnableCapture = false;
private:
	FVector BasePoint;
	FVector BaseNormal;
	FVector BaseXDir;
	FVector BaseYDir;
	FPlane BasePlane;//基面

	CapturePointInfo CurrentCaptruePoint;

	bool IsCaptrueChange;//是否捕捉点变化过

	bool CurrentFixPointInit;

	FVector2D CurrentFixPoint;//当前固定的绘制点
	FVector2D CurrentMovePoint;//正交或鼠标点

	TArray<TWeakPtr<"FArmyLine>> CurrentCaptrueLines;//捕捉到的线

	TArray<CapturePointInfo> AuxiliaryPoints;//辅助参考点，有数量限制CAD是7个,包括端点、垂足、交点等
	TArray<AuxiliaryLineInfo> AuxiliaryLines;//绘制辅线列表

    /** 线段颜色 */
    FLinearColor LineColor;

	/* @刘克祥 以当前点为终止点，上一个点为起始点的线段的垂线 */
	void AddAuxiliaryPerpendicular(FVector2D Start, FVector2D End);

};