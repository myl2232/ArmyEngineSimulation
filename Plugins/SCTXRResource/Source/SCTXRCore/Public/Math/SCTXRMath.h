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

class SCTXRCORE_API FSCTXRMath
{
public:
	/** 计算2D下两条线段相交 */
	static bool CalculateLinesIntersection(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint);

	/** 计算3D下任意线段和Z轴为0的线段是否相交 */
	static bool CalculateLinesIntersection3DTo2D (const FVector& P1,const FVector& P2,const FVector& P3,const FVector& P4,FVector& OutPoint);

	/** 判断两个多边形是否有重叠 */
	static bool ArePolysOverlap(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2);

	/** 获得某点在直线上的投影位置 */
	static const FVector GetProjectionPoint(const FVector& Point, const FVector& V0, const FVector& V1);

	/** 获取某点距直线的距离*/
	static const float Distance(const FVector& Point,const FVector& V0 ,const FVector& V1);

	/** 获得某点在线段上的投影位置 */
	static const bool GetLineSegmentProjectionPos(const FVector& V0, const FVector& V1, FVector& OutPos);

	/** 计算线段的中点 */
	static const FVector GetLineCenter(const FVector& Start, const FVector& End);

	/** 计算两条线段相交的角平分线方向 */
	static const FVector GetAngularBisectorDirection(const FVector& P1,const FVector P0,const FVector& P2);

	/** 计算两条线段相交的角平分线方向 */
	static const FVector GetAngularBisectorDirection (const FVector& Direction1,const FVector Direction2);

	/** 获取切线的圆心*/
	static const FVector GetTangentCenter(const FVector& P1,const FVector P0,const FVector& P2,float Radius);

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
	static bool IsPointProjectionOnLineSegment2D (const FVector2D& P,const FVector2D& A,const FVector2D& B,float Tolerance = KINDA_SMALL_NUMBER);

	/** 计算点是否在线段上 */
	static bool IsPointOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IsPointOnLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IsPointInLineSegment3D (const FVector& P0,const FVector& P1,const FVector& P2,float Tolerance = KINDA_SMALL_NUMBER);

    /** 计算多边形的面积 */
    static float CalcPolyArea(const TArray<FVector2D>& Points);

	/** 计算2D下点是否在多边形内 */
	static bool IsPointInPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints);

	static bool IsPolygonInPolygon(const TArray<FVector>& TestPolygone, const TArray<FVector>& Polygon);

	static bool IsCircleInPolygon2D(const FVector& CirclePositoin, const float Radius, const TArray<FVector>& PolygonPoints);

	/** 计算点到线段的距离 */
	static const float CalcPointToLineDistance(const FVector& Point, const FVector& V0, const FVector& V1);

	/** 三角剖分 */
	static bool Triangulate2DPoly(TArray<FVector2D>& OutTris, const TArray<FVector2D>& InPolyVerts, bool bKeepColinearVertices = false);

	/**	计算两线相交*/
	static bool Line2DIntersection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End, FVector2D& IntersectionPoint);

	/**	计算线与四边形相交*/
	static bool LineBox2DIntersection(const FVector2D& LineStart, const FVector2D& LineEnd, const FBox2D& Box);

	/** 判断两条线是否平行 */
	static bool AreLinesParallel(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1);

	static TArray<FVector> Extrude3D(const TArray<FVector>& InPoints, float ExtrudeDistance, bool bOutSide = true);
	static void ReversePointList(TArray<FVector>& PointList);

	static bool TriangulatePoly(TArray<FClipSMTriangle>& OutTris, const FClipSMPolygon& InPoly, bool bKeepColinearVertices = false);

	static	bool TriangulatePoly(TArray<FVector>& OutTris, TArray<int32>& OutIndexList, const TArray<FVector>& InPointList, const FVector &FaceNormal, bool bKeepColinearVertices = false);

	static bool ArcLineIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, TArray<FVector>& OutInter);

	static bool ArcBoxIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FBox2D& Box, TArray<FVector>& InterPoints);

	static bool ArcPointIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& Point, float tolerance, FVector& InterPoint);

	static bool ArcLineIntersectionWithOutInifinet(const FVector& ArcPoint1, const FVector& ArcPoint2, const FVector& ArcPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, FVector& OutInter);

	static bool Circle(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, FVector& OutCenter, double &OutRadius);

	static int ArcAndArcIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& AcrPoint4, const FVector& AcrPoint5, const FVector& AcrPoint6, TArray<FVector>& OutPoint);

	/** 计算两线之间距离*/
	static float DistanceLineToLine (const FVector& p1,const FVector& p2,const FVector& p3,const FVector& p4,FVector& IntersectPoint);

	/** 两线如果相交计算垂足点*/
	static void FootLineToLine (const FVector& S0,const FVector& E0,const FVector& S1,const FVector& E1,FVector& OutPoint0 ,FVector& OutPoint1);
};