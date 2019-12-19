#pragma once

#include "CoreMinimal.h"

/** ��̬���������ζ��� */
struct FClipSMVertex
{
	FVector Pos;
	FVector TangentX;
	FVector TangentY;
	FVector TangentZ;
	FVector2D UVs[8];
	FColor Color;
};

/** ������Ӱ��̬��������������� */
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

/** ������Ӱ��̬�������������ε����� */
struct FClipSMTriangle : FClipSMFace
{
	FClipSMVertex Vertices[3];

	void ComputeGradientsAndNormal();

	FClipSMTriangle(int32 Init)
	{
		FMemory::Memzero(this, sizeof(FClipSMTriangle));
	}
};

/** ������Ӱ��̬�����������ε����� */
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
	/** ����2D�������߶��ཻ */
	static bool CalculateLinesIntersection(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint);

	/** ����3D�������߶κ�Z��Ϊ0���߶��Ƿ��ཻ */
	static bool CalculateLinesIntersection3DTo2D (const FVector& P1,const FVector& P2,const FVector& P3,const FVector& P4,FVector& OutPoint);

	/** �ж�����������Ƿ����ص� */
	static bool ArePolysOverlap(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2);

	/** ���ĳ����ֱ���ϵ�ͶӰλ�� */
	static const FVector GetProjectionPoint(const FVector& Point, const FVector& V0, const FVector& V1);

	/** ��ȡĳ���ֱ�ߵľ���*/
	static const float Distance(const FVector& Point,const FVector& V0 ,const FVector& V1);

	/** ���ĳ�����߶��ϵ�ͶӰλ�� */
	static const bool GetLineSegmentProjectionPos(const FVector& V0, const FVector& V1, FVector& OutPos);

	/** �����߶ε��е� */
	static const FVector GetLineCenter(const FVector& Start, const FVector& End);

	/** ���������߶��ཻ�Ľ�ƽ���߷��� */
	static const FVector GetAngularBisectorDirection(const FVector& P1,const FVector P0,const FVector& P2);

	/** ���������߶��ཻ�Ľ�ƽ���߷��� */
	static const FVector GetAngularBisectorDirection (const FVector& Direction1,const FVector Direction2);

	/** ��ȡ���ߵ�Բ��*/
	static const FVector GetTangentCenter(const FVector& P1,const FVector P0,const FVector& P2,float Radius);

	/** �����߶η��� */
	static const FVector GetLineDirection(const FVector Start, const FVector End);

	/** �ж϶���ε㼯�Ƿ���˳ʱ�� */
	static bool IsClockWise(const TArray<FVector2D>& Points);

	static bool IsClockWise(const TArray<FVector>& Points);

	/** ���������鰴��˳ʱ������ */
	static void ClockwiseSortPoints(TArray<FVector>& Points);
	static bool PointCmp(const FVector& A, const FVector& B, const FVector& Center);

	static bool AreEdgesMergeable2D(const FVector2D& V0, const FVector2D& V1, const FVector2D& V2);

	/** ����2D�µ��Ƿ����������� */
	static bool IsPointInTriangle2D(const FVector2D& TestPoint, const FVector2D& A, const FVector2D& B, const FVector2D& C);

	/** ������ͶӰ�Ƿ����߶��ϣ�����ȽϾ�ȷ */
	static bool IsPointProjectionOnLineSegment2D (const FVector2D& P,const FVector2D& A,const FVector2D& B,float Tolerance = KINDA_SMALL_NUMBER);

	/** ������Ƿ����߶��� */
	static bool IsPointOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IsPointOnLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance = KINDA_SMALL_NUMBER);

	static bool IsPointInLineSegment3D (const FVector& P0,const FVector& P1,const FVector& P2,float Tolerance = KINDA_SMALL_NUMBER);

    /** �������ε���� */
    static float CalcPolyArea(const TArray<FVector2D>& Points);

	/** ����2D�µ��Ƿ��ڶ������ */
	static bool IsPointInPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints);

	static bool IsPolygonInPolygon(const TArray<FVector>& TestPolygone, const TArray<FVector>& Polygon);

	static bool IsCircleInPolygon2D(const FVector& CirclePositoin, const float Radius, const TArray<FVector>& PolygonPoints);

	/** ����㵽�߶εľ��� */
	static const float CalcPointToLineDistance(const FVector& Point, const FVector& V0, const FVector& V1);

	/** �����ʷ� */
	static bool Triangulate2DPoly(TArray<FVector2D>& OutTris, const TArray<FVector2D>& InPolyVerts, bool bKeepColinearVertices = false);

	/**	���������ཻ*/
	static bool Line2DIntersection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End, FVector2D& IntersectionPoint);

	/**	���������ı����ཻ*/
	static bool LineBox2DIntersection(const FVector2D& LineStart, const FVector2D& LineEnd, const FBox2D& Box);

	/** �ж��������Ƿ�ƽ�� */
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

	/** ��������֮�����*/
	static float DistanceLineToLine (const FVector& p1,const FVector& p2,const FVector& p3,const FVector& p4,FVector& IntersectPoint);

	/** ��������ཻ���㴹���*/
	static void FootLineToLine (const FVector& S0,const FVector& E0,const FVector& S1,const FVector& E1,FVector& OutPoint0 ,FVector& OutPoint1);
};