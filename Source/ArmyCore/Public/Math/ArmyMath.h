#pragma once

#include "CoreMinimal.h"

/** 静态网格三角形顶点 */
struct FClipSMVertex
{
	FVector Pos;
	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;
	FVector2D UVs[8];
	FColor Color;
};

/** 背面消影静态网格物体面的属性 */
struct FClipSMFace
{
	int32 MaterialIndex;
	uint32 SmoothingMask;
	int32 NumUVs;
	bool bOverrideTangentBasis;

	FVector FaceNormal;

	FMatrix TangentXGradient;
	FMatrix TangentYGradient;
	FMatrix TangentZGradient;
	FMatrix UVGradient[8];
	FMatrix ColorGradient;

	void CopyFace(const FClipSMFace& OtherFace)
	{
		FMemory::Memcpy(this, &OtherFace, sizeof(FClipSMFace));
	}
};

/** 背面消影静态网格物体三角形的属性 */
struct FClipSMTriangle : FClipSMFace
{
	FClipSMVertex Vertices[3];

	void ComputeGradientsAndNormal();

	FClipSMTriangle(int32 Init)
	{
		FMemory::Memzero(this, sizeof(FClipSMTriangle));
	}
};

/** 背面消影静态网格物体多边形的属性 */
struct FClipSMPolygon : FClipSMFace
{
	TArray<FClipSMVertex> Vertices;

	FClipSMPolygon(int32 Init)
	{
		FMemory::Memzero(this, sizeof(FClipSMPolygon));
	}
};

class ARMYCORE_API FArmyMath
{
public:
	/** 计算2D下两条线段相交 */
	static bool CalculateLinesIntersection(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint, bool bIgnoreParallel = true/*���Թ���*/);

	static bool CalculateLine2DIntersection(const FVector&P1, const FVector& P2, const FVector& P3, const FVector& P4, bool bIgnoreParallel = true/*���Թ���*/);

	/**@欧石楠 计算2D线段相交，忽略误差版 */
	static bool SegmentIntersection2D(const FVector& SegmentStartA, const FVector& SegmentEndA, const FVector& SegmentStartB, const FVector& SegmentEndB, FVector& out_IntersectionPoint);

	/** 计算3D下任意线段和Z轴为0的线段是否相交 */
	static bool CalculateLinesIntersection3DTo2D(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint);

	/** 计算2D下两条线是否穿过相交，不包括端点在另一条线上的情况 */
	static bool CalculateLinesCross(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint);

	/** 判断两个多边形是否有重叠 */
	static bool ArePolysOverlap(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2);

	/** @欧石楠 判断2D两个多边形是否相交 */
	static bool ArePolysIntersection(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2, bool bSideOverlap = false /*���Ǳ��غ�*/);

	/** 判断多边形是否是简单多边形 */

	static bool IsPolygonSimple( TArray<FVector>& InVertices);

	/** 获得某点在直线上的投影位置 */
	static const FVector GetProjectionPoint(const FVector& Point, const FVector& V0, const FVector& V1);

	/** 获取某点距直线的距离 */
	static const float Distance(const FVector& Point, const FVector& V0, const FVector& V1);

	/** 获取某点到线段上最近的点 */
	static const bool NearlyPoint(const FVector& Point, const FVector& V0, const FVector& V1, FVector& OutLocation, float ErrorTolerance = SMALL_NUMBER);

	/** 点是否在线段上 */
	static const bool IsPointOnLine(const FVector& Point, const FVector& V0, const FVector& V1, float Tolerance = KINDA_SMALL_NUMBER);

	/** 点是否在线段上,排除线段的端点 */
	static const bool IsPointOnLineExcludeEndPoint(const FVector& Point, const FVector& V0, const FVector& V1, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IntersectSegmentWithConvexPolygon(const FVector2D& startPos, const FVector2D& EndPos, const TArray<FVector2D>& ConverxPolygon, TArray<FVector>& intersectionPos);

	/** 获得某点在线段上的投影位置 */
	static const bool GetLineSegmentProjectionPos(const FVector& V0, const FVector& V1, FVector& OutPos);

	/** 计算线段的中点 */
	static const FVector GetLineCenter(const FVector& Start, const FVector& End);

	/** 计算两条线段相交的角平分线方向 */
	static const FVector GetAngularBisectorDirection(const FVector& P1, const FVector P0, const FVector& P2);

	/** 计算两条线段相交的角平分线方向 */
	static const FVector GetAngularBisectorDirection(const FVector& Direction1, const FVector Direction2);

	/** 获取切线的圆心 */
	static const FVector GetTangentCenter(const FVector& P1, const FVector P0, const FVector& P2, float Radius);

	/** 计算线段方向 */
	static const FVector GetLineDirection(const FVector Start, const FVector End);

	/** 判断多边形点集是否是顺时针 */
	static bool IsClockWise(const TArray<FVector2D>& Points);

	static bool IsClockWise(const TArray<FVector>& Points);

	/** 将顶点数组按照顺时针排列 */
	static void ClockwiseSortPoints(TArray<FVector>& Points);
	static bool PointCmp(const FVector& A, const FVector& B, const FVector& Center);

	static bool AreEdgesMergeable2D(const FVector2D& V0, const FVector2D& V1, const FVector2D& V2);

	/** 计算2D下点是否在三角形内 */
	static bool IsPointInTriangle2D(const FVector2D& TestPoint, const FVector2D& A, const FVector2D& B, const FVector2D& C);

	/** 计算点的投影是否在线段上！这个比较精确 */
	static bool IsPointProjectionOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance = KINDA_SMALL_NUMBER);

	/** 计算点是否在线段上 */
	static bool IsPointOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IsPointOnLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IsPointInLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance = KINDA_SMALL_NUMBER);

	/** 计算多边形的面积 */
	static float CalcPolyArea(const TArray<FVector2D>& Points);

	static float CalcPolyArea(const TArray<FVector>& Points);
	// zhx
	static float CalcPolyArea3D(const TArray<FVector>& Points); 

    /** 计算2D下点是否在多边形内 */
    static bool IsPointInPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints, float Tolerance = 0.009f);

	/** 计算2D下点是否在多边形内或者多边形上 */
	static bool IsPointInOrOnPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints, float Tolerance = 0.009f);

	static bool IsPolygonInPolygon(const TArray<FVector>& TestPolygone, const TArray<FVector>& Polygon);

	static bool IsCircleInPolygon2D(const FVector& CirclePositoin, const float Radius, const TArray<FVector>& PolygonPoints);

	/** 计算点到直线的距离(@ 请注意直线与线段的区别) */
	static const float CalcPointToLineDistance(const FVector& Point, const FVector& V0, const FVector& V1);

	/** 三角剖分 */
	static bool Triangulate2DPoly(TArray<FVector2D>& OutTris, const TArray<FVector2D>& InPolyVerts, bool bKeepColinearVertices = false);

	/**	计算两线相交 */
	static bool Line2DIntersection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End, FVector2D& IntersectionPoint);

	/** 计算两线在某个方向是否相交相交 而且焦点在第一条线上 */
	static bool Line2DIntersectionDirection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2Direction, FVector2D& IntersectionPoint);

	/**	计算两线某个方向是否相交相交3D而且焦点在第一条线上 */
	static bool Line3DIntersectionDirection(const FVector& Line1Start, const FVector& Line1End, const FVector& Line2Start, const FVector& Line2End, FVector& IntersectionPoint);


	/**	计算线与四边形相交 */
	static bool LineBox2DIntersection(const FVector2D& LineStart, const FVector2D& LineEnd, const FBox2D& Box);

	/**	计算线与多边形的交点 */
	static bool CalLinePolygonIntersection(const FVector& LineStart, const FVector& LineEnd, const TArray<FVector>& PolygonPoints,TArray<FVector>&Intersections);
	/**	计算线与多边形的重合点，只考虑重合的情况 */
	static bool CalLinePolygonCoincidence(const FVector& LineStart, const FVector& LineEnd, const TArray<FVector>& PolygonPoints, TArray<FVector>&Coincidences);


	/** 判断两条线是否平行 */
	static bool AreLinesParallel(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1);

	/** 判断两条线是否平行 */
	static bool AreLinesParallel(const FVector& Line0, const FVector& Line1);
	// 向内挤传入正数
	static TArray<FVector> Extrude3D(const TArray<FVector>& InPoints, float ExtrudeDistance, bool bOutSide = true);
	static TArray<FVector> Extrude3dPolygon(const TArray<FVector>& InPoints, float ExtrudeDistance, const TArray<FVector>& BoundaryAra);
	static TArray<FVector> ExturdePolygon(const TArray<FVector>& Original, float ExturdeDistance, bool InSide = true);
	static void CleanPolygon(const TArray<FVector2D>& In_Poly, TArray<FVector2D>& Out_Poly, float Tolarence = 0.01f);
	static void CleanPolygon(TArray<FVector>& In_Poly, float Tolarence = 0.01f);
	static bool UpdataBreakLineInfos(const TArray<FVector2D>& InOutLineVerts, TArray<TArray<FVector2D>>& OutResults, const TArray<FVector2D>& BreakLines);

	static void RemoveComPointOnPolygon(TArray<FVector>& Orginal);
	static void ReversePointList(TArray<FVector>& PointList);
	static bool LineBoxIntersectionWithHitPoint(const FBox& Box, const FVector& Start, const FVector& End, const FVector& Direction, FVector& OutHit);
	static void ReverPointList(TArray<FVector2D>& PointList);

	static bool TriangulatePoly(TArray<FClipSMTriangle>& OutTris, const FClipSMPolygon& InPoly, bool bKeepColinearVertices = false);

	static	bool TriangulatePoly(TArray<FVector>& OutTris, TArray<int32>& OutIndexList, const TArray<FVector>& InPointList, const FVector &FaceNormal, bool bKeepColinearVertices = false);

	static bool ArcLineIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, TArray<FVector>& OutInter);

	static bool ArcBoxIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FBox2D& Box, TArray<FVector>& InterPoints);

	static bool ArcPointIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& Point, float tolerance, FVector& InterPoint);

	static bool ArcLineIntersectionWithOutInifinet(const FVector& ArcPoint1, const FVector& ArcPoint2, const FVector& ArcPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, FVector& OutInter);

	static bool Circle(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, FVector& OutCenter, double &OutRadius);

	static int ArcAndArcIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& AcrPoint4, const FVector& AcrPoint5, const FVector& AcrPoint6, TArray<FVector>& OutPoint);

	/** 计算两线之间距离 */
	static float DistanceLineToLine(const FVector& p1, const FVector& p2, const FVector& p3, const FVector& p4, FVector& IntersectPoint);

	/** 两线如果相交计算垂足点 */
	static void FootLineToLine(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1, FVector& OutPoint0, FVector& OutPoint1);


	// 三维点确定圆
	static bool Circle3D(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, FVector& OutCenter, double &OutRadius);

	/* @欧石楠 Z=0,求区域的周长，分为闭合区域和未闭合区域 */
	static float CircumferenceOfPointList(TArray<FVector> PointList, bool bClose);

	/**
	 * 计算2D下两条线是否共线
	 * @param S1 - const FVector & - 第一条线起点
	 * @param E1 - const FVector & - 第一条线终点
	 * @param S2 - const FVector & - 第二条线起点
	 * @param E2 - const FVector & - 第二条线终点
	 * @return bool - true表示共线，否则不共线
	 */
	static bool CalcLines2DCollineation(const FVector& S1, const FVector& E1, const FVector& S2, const FVector& E2);

	/** 判断两条线段是否平行共线并有重合部分，最后一个参数表示是否包括端点 */
	static bool IsLinesCollineationAndIntersection(const FVector& S1, const FVector& E1, const FVector& S2, const FVector& E2, bool bIncludeEndpoint = true);

	//绘制圆环
	/**
	* Center 圆环中心
	* InStart 圆环的起始位置
	* InEnd 圆环的终止位置
	* 圆环的半径
	*/
	static void CreateArc(const FVector& center,FVector& InStart,FVector& InEnd,float Radius,TArray<FVector>& Verties,int32 cout = 6);

	/**
	* 绘制 圆柱
	*/
	static void DrawCylinder(FVector InStart,FVector InEnd,float r,TArray<FVector>& Verties);

	static void DrawCylinder(FVector InStart,FVector InEnd,FVector CylinderDirection, float r,TArray<FVector>& Verties);

	/**
	* 画个球
	*/
	static void DrawWire(FVector& Location,float Raduis,TArray<FVector>& Verties,int32 Cout = 15);

	static FVector GetPoint(float u,float v);

	/** @欧石楠 Tarray反转*/
	template<typename T>
	static void ReverseArray(TArray<T> &InArray);
};

template<typename T>
void FArmyMath::ReverseArray(TArray<T> &InArray)
{
	int size = InArray.Num();
	if (size < 2)
	{
		return;
	}
	else
	{
		for (int i = 0; i < size / 2; ++i)
		{
			T Temp = InArray[i];
			InArray[i] = InArray[size - 1 - i];
			InArray[size - 1 - i] = Temp;
		}
	}
}
