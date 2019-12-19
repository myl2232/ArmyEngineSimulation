#include "SCTXRMath.h"

static bool VectorsOnSameSide(const FVector& Vec, const FVector& A, const FVector& B)
{
	const FVector CrossA = Vec ^ A;
	const FVector CrossB = Vec ^ B;
	return !FMath::IsNegativeFloat(CrossA | CrossB);
}

static bool PointInTriangle(const FVector& A, const FVector& B, const FVector& C, const FVector& P)
{
	// Cross product indicates which 'side' of the vector the point is on
	// If its on the same side as the remaining vert for all edges, then its inside.	
	if (VectorsOnSameSide(B - A, P - A, C - A) &&
		VectorsOnSameSide(C - B, P - B, A - B) &&
		VectorsOnSameSide(A - C, P - C, B - C))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/** Compare all aspects of two verts of two triangles to see if they are the same. */
static bool VertsAreEqual(const FClipSMVertex& A, const FClipSMVertex& B)
{
	if (!A.Pos.Equals(B.Pos, THRESH_POINTS_ARE_SAME))
	{
		return false;
	}

	if (!A.TangentX.Equals(B.TangentX, THRESH_NORMALS_ARE_SAME))
	{
		return false;
	}

	if (!A.TangentY.Equals(B.TangentY, THRESH_NORMALS_ARE_SAME))
	{
		return false;
	}

	if (!A.TangentZ.Equals(B.TangentZ, THRESH_NORMALS_ARE_SAME))
	{
		return false;
	}

	if (A.Color != B.Color)
	{
		return false;
	}

	for (int32 i = 0; i < ARRAY_COUNT(A.UVs); i++)
	{
		if (!A.UVs[i].Equals(B.UVs[i], 1.0f / 1024.0f))
		{
			return false;
		}
	}

	return true;
}

static FClipSMVertex InterpolateVert(const FClipSMVertex& V0, const FClipSMVertex& V1, float Alpha)
{
	FClipSMVertex Result;

	// Handle dodgy alpha
	if (FMath::IsNaN(Alpha) || !FMath::IsFinite(Alpha))
	{
		Result = V1;
		return Result;
	}

	Result.Pos = FMath::Lerp(V0.Pos, V1.Pos, Alpha);
	Result.TangentX = FMath::Lerp(V0.TangentX, V1.TangentX, Alpha);
	Result.TangentY = FMath::Lerp(V0.TangentY, V1.TangentY, Alpha);
	Result.TangentZ = FMath::Lerp(V0.TangentZ, V1.TangentZ, Alpha);
	for (int32 i = 0; i < 8; i++)
	{
		Result.UVs[i] = FMath::Lerp(V0.UVs[i], V1.UVs[i], Alpha);
	}

	Result.Color.R = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.R), float(V1.Color.R), Alpha)), 0, 255);
	Result.Color.G = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.G), float(V1.Color.G), Alpha)), 0, 255);
	Result.Color.B = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.B), float(V1.Color.B), Alpha)), 0, 255);
	Result.Color.A = FMath::Clamp(FMath::TruncToInt(FMath::Lerp(float(V0.Color.A), float(V1.Color.A), Alpha)), 0, 255);
	return Result;
}

static bool AreEdgesMergeable(
	const FClipSMVertex& V0,
	const FClipSMVertex& V1,
	const FClipSMVertex& V2
)
{
	const FVector MergedEdgeVector = V2.Pos - V0.Pos;
	const float MergedEdgeLengthSquared = MergedEdgeVector.SizeSquared();
	if (MergedEdgeLengthSquared > DELTA)
	{
		// Find the vertex closest to A1/B0 that is on the hypothetical merged edge formed by A0-B1.
		const float IntermediateVertexEdgeFraction =
			((V2.Pos - V0.Pos) | (V1.Pos - V0.Pos)) / MergedEdgeLengthSquared;
		const FClipSMVertex InterpolatedVertex = InterpolateVert(V0, V2, IntermediateVertexEdgeFraction);

		// The edges are merge-able if the interpolated vertex is close enough to the intermediate vertex.
		return VertsAreEqual(InterpolatedVertex, V1);
	}
	else
	{
		return true;
	}
}


bool FSCTXRMath::CalculateLinesIntersection(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint)
{
	if (FMath::SegmentIntersection2D(P1, P2, P3, P4, OutPoint))
	{
		FVector2D Line1Dir = FVector2D(P2) - FVector2D(P1);
		FVector2D Line2Dir = FVector2D(P4) - FVector2D(P3);

		Line1Dir.Normalize();
		Line2Dir.Normalize();

		if (Line1Dir.Size() > 0 && Line2Dir.Size() > 0 && Line1Dir.X * Line2Dir.X == 0 && Line1Dir.Y * Line2Dir.Y == 0)
		{
			OutPoint.X = Line1Dir.X == 0 ? P1.X : P3.X;
			OutPoint.Y = Line1Dir.Y == 0 ? P1.Y : P3.Y;
		}
		if ((OutPoint - P1).Size() < 0.0001)
		{
			OutPoint = P1;
		}
		else if ((OutPoint - P2).Size() < 0.0001)
		{
			OutPoint = P2;
		}
		else if ((OutPoint - P3).Size() < 0.0001)
		{
			OutPoint = P3;
		}
		else if ((OutPoint - P4).Size() < 0.0001)
		{
			OutPoint = P4;
		}
		return true;
	}
	return false;
}

bool FSCTXRMath::CalculateLinesIntersection3DTo2D(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint)
{
	FVector P3T2 = P3;
	FVector P4T2 = P4;
	P3T2.Z = 0;
	P4T2.Z = 0;
	return CalculateLinesIntersection(P1, P2, P3T2, P4T2, OutPoint);
}

bool FSCTXRMath::ArePolysOverlap(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2)
{

	FVector CrossPos = FVector::ZeroVector;
	for (int32 i = 0; i < Poly1.Num(); ++i)
	{
		FVector StartA = Poly1[i];
		FVector EndA = (i == Poly1.Num() - 1) ? Poly1[0] : Poly1[i + 1];
		for (int32 j = 0; j < Poly2.Num(); ++j)
		{
			FVector StartB = Poly2[j];
			FVector EndB = (j == Poly2.Num() - 1) ? Poly2[0] : Poly2[j + 1];
			bool bCross = CalculateLinesIntersection(StartA, EndA, StartB, EndB, CrossPos);
			if (bCross)
			{
				return true;
			}
		}
	}

	return false;
}

const FVector FSCTXRMath::GetProjectionPoint(const FVector& Point, const FVector& V0, const FVector& V1)
{
	return (FVector::DotProduct((Point - V0), (V1 - V0))* (V1 - V0)) / (FVector::Distance(V1, V0) * FVector::Distance(V1, V0)) + V0;
}

const float FSCTXRMath::Distance(const FVector& Point, const FVector& V0, const FVector& V1)
{
	//@飞舞轻扬 计算线段V1V0的长度的平方
	float DistSquaredV1V0 = FVector::DistSquared(V1, V0);
	//@飞舞轻扬 计算点乘
	float DotProduct = FVector::DotProduct((Point - V0), (V1 - V0));
	//@飞舞轻扬 计算PV长度的平方
	float DistSquaredPV0 = FVector::DistSquared(V0, Point);
	//@飞舞轻扬 返回长度
	return FMath::Sqrt(DistSquaredPV0 - (DotProduct *DotProduct / DistSquaredV1V0));
}

const bool FSCTXRMath::GetLineSegmentProjectionPos(const FVector& V0, const FVector& V1, FVector& OutPos)
{
	OutPos = GetProjectionPoint(OutPos, V0, V1);

	return IsPointOnLineSegment2D(FVector2D(OutPos), FVector2D(V0), FVector2D(V1), 0.01f);
}

const FVector FSCTXRMath::GetLineCenter(const FVector& Start, const FVector& End)
{
	return (Start + End) / 2.f;
}


const FVector FSCTXRMath::GetAngularBisectorDirection(const FVector& P1, const FVector P0, const FVector& P2)
{
	const FVector P0P1 = (P1 - P0).GetSafeNormal();
	const FVector P0P2 = (P2 - P0).GetSafeNormal();
	return GetAngularBisectorDirection(P0P1, P0P2);
}

const FVector FSCTXRMath::GetAngularBisectorDirection(const FVector& Direction1, const FVector Direction2)
{
	return ((Direction1 + Direction2) / 2).GetSafeNormal();
}

const FVector FSCTXRMath::GetTangentCenter(const FVector& P1, const FVector P0, const FVector& P2, float Radius)
{
	FVector P0P1 = (P1 - P0).GetSafeNormal();
	//  二分线方向
	FVector Direction = GetAngularBisectorDirection(P1, P0, P2);

	FVector Normal = FVector::CrossProduct(P0P1, Direction);
	float SinQ = Normal.Size();
	float Length = Radius / SinQ;

	return P0 + Length *Direction;
}

const FVector FSCTXRMath::GetLineDirection(const FVector Start, const FVector End)
{
	FVector Result = FVector::ZeroVector;
	Result = End - Start;
	Result = Result.GetSafeNormal();

	return Result;
}

bool FSCTXRMath::IsClockWise(const TArray<FVector2D>& Points)
{
	uint32 NumS = 0;
	uint32 NumN = 0;

	for (int32 Index = 0; Index + 2 < Points.Num(); Index++)
	{
		FVector2D Vector1 = Points[Index + 1] - Points[Index];
		FVector2D Vector2 = Points[Index + 2] - Points[Index];
		if (FMath::IsNearlyZero(FVector2D::CrossProduct(Vector1, Vector2)))
		{
			continue;
		}
		else if (FVector2D::CrossProduct(Vector1, Vector2) < 0)
		{
			NumN++;
		}
		else if (FVector2D::CrossProduct(Vector1, Vector2) > 0)
		{
			NumS++;
		}
	}

	return (NumS > NumN);
}


bool FSCTXRMath::IsClockWise(const TArray<FVector>& Points)
{
	uint32 NumS = 0;
	uint32 NumN = 0;

	for (int32 Index = 0; Index + 2 < Points.Num(); Index++)
	{
		FVector2D Vector1 = FVector2D(Points[Index + 1] - Points[Index]);
		FVector2D Vector2 = FVector2D(Points[Index + 2] - Points[Index]);
		if (FMath::IsNearlyZero(FVector2D::CrossProduct(Vector1, Vector2)))
		{
			continue;
		}
		else if (FVector2D::CrossProduct(Vector1, Vector2) < 0)
		{
			NumN++;
		}
		else if (FVector2D::CrossProduct(Vector1, Vector2) > 0)
		{
			NumS++;
		}
	}

	return (NumS > NumN);
}

void FSCTXRMath::ClockwiseSortPoints(TArray<FVector>& Points)
{
	//计算重心
	FVector Center;
	float X = 0, Y = 0;
	for (int32 i = 0; i < Points.Num(); i++)
	{
		X += Points[i].X;
		Y += Points[i].Y;
	}
	Center.X = (int32)X / Points.Num();
	Center.Y = (int32)Y / Points.Num();
	Center.Z = 0;

	//冒泡排序
	for (int32 i = 0; i < Points.Num() - 1; i++)
	{
		for (int32 j = 0; j < Points.Num() - i - 1; j++)
		{
			if (PointCmp(Points[j], Points[j + 1], Center))
			{
				FVector tmp = Points[j];
				Points[j] = Points[j + 1];
				Points[j + 1] = tmp;
			}
		}
	}
}

bool FSCTXRMath::PointCmp(const FVector& A, const FVector& B, const FVector& Center)
{
	if (A.X >= 0 && B.X < 0) return true;
	if (A.X == 0 && B.X == 0) return A.Y > B.Y;

	float Det = (A.X - Center.X) * (B.Y - Center.Y) - (B.X - Center.X) * (A.Y - Center.Y);
	if (Det < 0) return true;
	if (Det > 0) return false;

	//向量OA和向量OB共线，以距离判断大小
	float D1 = (A.X - Center.X) * (A.X - Center.X) + (A.Y - Center.Y) * (A.Y - Center.Y);
	float D2 = (B.X - Center.X) * (B.X - Center.Y) + (B.Y - Center.Y) * (B.Y - Center.Y);

	return D1 > D2;
}

bool FSCTXRMath::AreEdgesMergeable2D(const FVector2D& V0, const FVector2D& V1, const FVector2D& V2)
{
	const FVector2D MergedEdgeVector = V2 - V0;
	const float MergedEdgeLengthSquared = MergedEdgeVector.SizeSquared();
	if (MergedEdgeLengthSquared > DELTA)
	{
		// Find the vertex closest to A1/B0 that is on the hypothetical merged edge formed by A0-B1.
		const float IntermediateVertexEdgeFraction = ((V2 - V0) | (V1 - V0)) / MergedEdgeLengthSquared;
		const FVector2D InterpolatedVertex = V0 + (V2 - V0) * IntermediateVertexEdgeFraction;

		// The edges are merge-able if the interpolated vertex is close enough to the intermediate vertex.
		return InterpolatedVertex.Equals(V1, THRESH_POINTS_ARE_SAME);
	}
	else
	{
		return true;
	}
}

#define VECTOR_SIGN(Vec,A,B) FMath::Sign((B.X - A.X) * (Vec.Y - A.Y) - (B.Y - A.Y) * (Vec.X - A.X))
bool FSCTXRMath::IsPointInTriangle2D(const FVector2D& TestPoint, const FVector2D& A, const FVector2D& B, const FVector2D& C)
{
	float BA = VECTOR_SIGN(B, A, TestPoint);
	float CB = VECTOR_SIGN(C, B, TestPoint);
	float AC = VECTOR_SIGN(A, C, TestPoint);

	return BA == CB && CB == AC;
}

bool FSCTXRMath::IsPointProjectionOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance /*= KINDA_SMALL_NUMBER*/)
{
	if (IsPointOnLineSegment2D(P, A, B, Tolerance))
	{
		return true;
	}
	// 计算点乘
	float DotA = FVector2D::DotProduct(P - A, B - A);
	float DotB = FVector2D::DotProduct(P - B, A - B);
	return DotA >= 0 && DotB >= 0;
}

bool FSCTXRMath::IsPointOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance/* = SMALL_NUMBER*/)
{
	float TestResult = FVector2D::CrossProduct(P - A, P - B);
	if (FMath::Abs(TestResult) < Tolerance)
	{
		if (FMath::Min(A.X, B.X) - Tolerance <= P.X &&
			P.X - Tolerance <= FMath::Max(A.X, B.X) &&
			FMath::Min(A.Y, B.Y) - Tolerance <= P.Y &&
			P.Y - Tolerance <= FMath::Max(A.Y, B.Y)
			)
		{
			return true;
		}
	}

	return false;
}

bool FSCTXRMath::IsPointOnLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance /*= KINDA_SMALL_NUMBER*/)
{
	if ((P0 - P1).IsNearlyZero(Tolerance))
		return true;
	if ((P0 - P2).IsNearlyZero(Tolerance))
		return true;
	if ((P1 - P2).IsNearlyZero(Tolerance))
		return true;
	FVector direction0 = (P1 - P0).GetSafeNormal();
	FVector direction1 = (P2 - P0).GetSafeNormal();
	float reuslt = FMath::Abs(FVector::DotProduct(direction1, direction0));
	if (FMath::IsNearlyEqual(reuslt, 1.0f, Tolerance))
	{
		return true;
	}
	return false;
}

bool FSCTXRMath::IsPointInLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance /*= KINDA_SMALL_NUMBER*/)
{
	if (IsPointOnLineSegment3D(P0, P1, P2, 0.01f))
	{
		if (FMath::Min(P1.X, P2.X) - Tolerance <= P0.X &&
			P0.X - Tolerance <= FMath::Max(P1.X, P2.X) &&
			FMath::Min(P1.Y, P2.Y) - Tolerance <= P0.Y &&
			P0.Y - Tolerance <= FMath::Max(P2.Y, P1.Y) &&
			FMath::Min(P1.Z, P2.Z) - Tolerance <= P0.Z &&
			P0.Z - Tolerance <= FMath::Max(P2.Z, P1.Z)
			)
			return true;
	}
	return false;
}

float FSCTXRMath::CalcPolyArea(const TArray<FVector2D>& Points)
{
	int32 iArea = 0;
	int32 iCount = Points.Num();

	for (int32 iCycle = 0; iCycle < iCount; ++iCycle)
	{
		FVector2D Vec0 = Points[iCycle];
		FVector2D Vec1 = Points[(iCycle + 1) % iCount];
		iArea = iArea + Vec0.X * Vec1.Y - Vec1.X * Vec0.Y;
	}

	iArea = FMath::Abs(0.5f * iArea);

	return iArea;
}

bool FSCTXRMath::IsPointInPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints)
{
	const int NumPoints = PolygonPoints.Num();
	float AngleSum = 0.0f;
	for (int PointIndex = 0; PointIndex < NumPoints; ++PointIndex)
	{
		const FVector2D& VecAB = FVector2D(PolygonPoints[PointIndex] - TestPoint);
		const FVector2D& VecAC = FVector2D(PolygonPoints[(PointIndex + 1) % NumPoints] - TestPoint);
		const float Angle = FMath::Sign(FVector2D::CrossProduct(VecAB, VecAC)) * FMath::Acos(FMath::Clamp(FVector2D::DotProduct(VecAB, VecAC) / (VecAB.Size() * VecAC.Size()), -1.0f, 1.0f));
		AngleSum += Angle;
	}
	return (FMath::Abs(AngleSum) > 0.009f);
}

bool FSCTXRMath::IsPolygonInPolygon(const TArray<FVector>& TestPolygon, const TArray<FVector>& Polygon)
{
	int number = TestPolygon.Num();
	for (int i = 0; i < number; i++)
	{
		if (!IsPointInPolygon2D(TestPolygon[i], Polygon))
		{
			return false;
		}
	}
	return true;
}

bool FSCTXRMath::IsCircleInPolygon2D(const FVector& CirclePositoin, const float Radius, const TArray<FVector>& PolygonPoints)
{
	if (IsPointInPolygon2D(CirclePositoin, PolygonPoints))
	{
		int number = PolygonPoints.Num();
		for (int i = 0; i < number; i++)
		{
			FVector startPoint = PolygonPoints[i%number];
			FVector endPoint = PolygonPoints[(i + 1) % number];
			float distance = Distance(CirclePositoin, startPoint, endPoint);
			if (distance < Radius)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

const float FSCTXRMath::CalcPointToLineDistance(const FVector& Point, const FVector& V0, const FVector& V1)
{
	FVector ProjectPoint = GetProjectionPoint(Point, V0, V1);
	return FVector::Distance(ProjectPoint, Point);
}

bool FSCTXRMath::Triangulate2DPoly(TArray<FVector2D>& OutTris, const TArray<FVector2D>& InPolyVerts, bool bKeepColinearVertices /*= false*/)
{
	if (InPolyVerts.Num() < 3)
		return false;

	TArray<FVector2D> PolyVerts = InPolyVerts;
	if (!IsClockWise(PolyVerts))
	{
		TArray<FVector2D> NewPoints;
		for (int j = PolyVerts.Num() - 1; j >= 0; j--)
		{
			NewPoints.Push(PolyVerts[j]);
		}
		PolyVerts.Empty();
		PolyVerts = NewPoints;
	}

	while (true)
	{
		if (!bKeepColinearVertices)
		{
			for (int32 VertexIndex = 0; VertexIndex < PolyVerts.Num(); VertexIndex++)
			{
				const int32 I0 = (VertexIndex + 0) % PolyVerts.Num();
				const int32 I1 = (VertexIndex + 1) % PolyVerts.Num();
				const int32 I2 = (VertexIndex + 2) % PolyVerts.Num();
				if (AreEdgesMergeable2D(PolyVerts[I0], PolyVerts[I1], PolyVerts[I2]))
				{
					PolyVerts.RemoveAt(I1);
					VertexIndex--;
				}
			}
		}

		if (PolyVerts.Num() < 3)
		{
			break;
		}
		else
		{
			bool bFoundEar = false;
			for (int32 EarVertexIndex = 0; EarVertexIndex < PolyVerts.Num(); EarVertexIndex++)
			{
				const int32 AIndex = (EarVertexIndex == 0) ? PolyVerts.Num() - 1 : EarVertexIndex - 1;
				const int32 BIndex = EarVertexIndex;
				const int32 CIndex = (EarVertexIndex + 1) % PolyVerts.Num();

				const FVector2D ABEdge = PolyVerts[BIndex] - PolyVerts[AIndex];
				const FVector2D ACEdge = PolyVerts[CIndex] - PolyVerts[AIndex];
				float CurrentV = FMath::IsNegativeFloat(ABEdge ^ ACEdge);
				if (CurrentV)
				{
					continue;
				}

				bool bFoundVertInside = false;
				for (int32 VertexIndex = 0; VertexIndex < PolyVerts.Num(); VertexIndex++)
				{
					const FVector2D& CurrentVertex = PolyVerts[VertexIndex];

					if (VertexIndex != AIndex && VertexIndex != BIndex && VertexIndex != CIndex &&
						CurrentVertex != PolyVerts[AIndex] && CurrentVertex != PolyVerts[BIndex] && CurrentVertex != PolyVerts[CIndex] &&
						(IsPointInTriangle2D(CurrentVertex, PolyVerts[AIndex], PolyVerts[BIndex], PolyVerts[CIndex]) || IsPointOnLineSegment2D(CurrentVertex, PolyVerts[CIndex], PolyVerts[AIndex])))
					{
						bFoundVertInside = true;
						break;
					}
				}

				if (!bFoundVertInside)
				{
					// Add to output list..
					OutTris.Add(PolyVerts[AIndex]);
					OutTris.Add(PolyVerts[BIndex]);
					OutTris.Add(PolyVerts[CIndex]);

					PolyVerts.RemoveAt(EarVertexIndex);

					bFoundEar = true;
					break;
				}
			}

			if (!bFoundEar)
			{
				OutTris.Empty();
				return false;
			}
		}
	}

	if (OutTris.Num() <= 0)
	{
		return false;
	}

	return true;
}
bool FSCTXRMath::Line2DIntersection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End, FVector2D& IntersectionPoint)
{
	FVector2D Line1Dir = Line1End - Line1Start;
	FVector2D Line2Dir = Line2End - Line2Start;

	Line1Dir.Normalize();
	Line2Dir.Normalize();
	if ((Line1Dir + Line2Dir).Size() < 0.0001 || (Line1Dir - Line2Dir).Size() < 0.0001)//平行
	{
		return false;
	}

	if (Line1Dir.Size() > 0 && Line2Dir.Size() > 0 && Line1Dir.X * Line2Dir.X == 0 && Line1Dir.Y * Line2Dir.Y == 0)
	{
		IntersectionPoint.X = Line1Dir.X == 0 ? Line1Start.X : Line2Start.X;
		IntersectionPoint.Y = Line1Dir.Y == 0 ? Line1Start.Y : Line2Start.Y;
		return true;
	}

	float x1 = Line1Start.X;
	float x2 = Line1End.X;

	float x3 = Line2Start.X;
	float x4 = Line2End.X;

	float y1 = Line1Start.Y;
	float y2 = Line1End.Y;

	float y3 = Line2Start.Y;
	float y4 = Line2End.Y;

	IntersectionPoint.X = ((x1 - x2) * (x3 * y4 - x4 * y3) - (x3 - x4) * (x1 * y2 - x2 * y1))
		/ ((x3 - x4) * (y1 - y2) - (x1 - x2) * (y3 - y4));

	IntersectionPoint.Y = ((y1 - y2) * (x3 * y4 - x4 * y3) - (x1 * y2 - x2 * y1) * (y3 - y4))
		/ ((y1 - y2) * (x3 - x4) - (x1 - x2) * (y3 - y4));

	return true;
}
bool FSCTXRMath::LineBox2DIntersection(const FVector2D& LineStart, const FVector2D& LineEnd, const FBox2D& Box)
{
	FVector NearPoint = FMath::ClosestPointOnLine(FVector(LineStart, 0), FVector(LineEnd, 0), FVector(Box.Min, 0));
	if (Box.IsInside(FVector2D(NearPoint)))
	{
		return true;
	}
	NearPoint = FMath::ClosestPointOnLine(FVector(LineStart, 0), FVector(LineEnd, 0), FVector(Box.Max, 0));
	if (Box.IsInside(FVector2D(NearPoint)))
	{
		return true;
	}
	NearPoint = FMath::ClosestPointOnLine(FVector(LineStart, 0), FVector(LineEnd, 0), FVector(Box.Min.X, Box.Max.Y, 0));
	if (Box.IsInside(FVector2D(NearPoint)))
	{
		return true;
	}
	NearPoint = FMath::ClosestPointOnLine(FVector(LineStart, 0), FVector(LineEnd, 0), FVector(Box.Max.X, Box.Min.Y, 0));
	if (Box.IsInside(FVector2D(NearPoint)))
	{
		return true;
	}
	NearPoint = FMath::ClosestPointOnLine(FVector(LineStart, 0), FVector(LineEnd, 0), FVector((Box.Min.X + Box.Max.X) / 2, Box.Min.Y, 0));
	if (Box.IsInside(FVector2D(NearPoint)))
	{
		return true;
	}
	NearPoint = FMath::ClosestPointOnLine(FVector(LineStart, 0), FVector(LineEnd, 0), FVector(Box.Max.X, (Box.Min.Y + Box.Max.Y) / 2, 0));
	if (Box.IsInside(FVector2D(NearPoint)))
	{
		return true;
	}
	return false;
}

bool FSCTXRMath::AreLinesParallel(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1)
{
	FVector Dir0 = GetLineDirection(S0, E0);
	FVector Dir1 = GetLineDirection(S1, E1);

	float Result = FMath::Abs<float>(FVector::DotProduct(Dir0, Dir1));

	return FMath::IsNearlyEqual(Result, 1.f, 0.01f);
}

TArray<FVector> FSCTXRMath::Extrude3D(const TArray<FVector>& InPoints, float ExtrudeDistance, bool bOutSide /*= true*/)
{
	TArray<FVector> ResultPoints;
	TArray<FVector2D> UsePoints;
	for (auto& It : InPoints)
	{
		UsePoints.Push(FVector2D(It.X, It.Y));
	}
	for (int32 j = 0; j < UsePoints.Num(); j++)
	{
		FVector2D PrePoint = j == 0 ? UsePoints[UsePoints.Num() - 1] : UsePoints[j - 1];
		FVector2D CurrentPoint = UsePoints[j];
		FVector2D NextPoint = j == UsePoints.Num() - 1 ? UsePoints[0] : UsePoints[j + 1];
		FVector2D DirectCurrentPre = PrePoint - CurrentPoint;
		FVector2D DirectCurrentNext = NextPoint - CurrentPoint;
		DirectCurrentPre.Normalize();
		DirectCurrentNext.Normalize();
		float CrossVector = FVector2D::CrossProduct(DirectCurrentPre, DirectCurrentNext);
		FVector2D HornLine = DirectCurrentPre + DirectCurrentNext;

		FVector2D PointIntersect;
		if (CrossVector > 0)
			HornLine *= -1.f;
		if (FMath::IsNearlyZero(CrossVector, KINDA_SMALL_NUMBER))
		{
			FVector CurrentPoint3D = FVector::CrossProduct(FVector(DirectCurrentPre.X, DirectCurrentPre.Y, 0.f), FVector(0, 0, 1));
			HornLine = FVector2D(CurrentPoint3D.X, CurrentPoint3D.Y);
			HornLine.Normalize();
			PointIntersect = CurrentPoint + ExtrudeDistance * HornLine;
		}
		else
		{
			HornLine.Normalize();
			float Angle = FMath::Acos(FVector2D::DotProduct(DirectCurrentPre, DirectCurrentNext));
			Angle = (Angle * 180 / PI);
			if (CrossVector > 0)//夹角大于180
			{
				Angle = 360.f - Angle;
				Angle = Angle / 2.f;
				Angle -= 90.f;
				Angle = Angle*PI / 180.f;
				PointIntersect = CurrentPoint + (ExtrudeDistance / FMath::Cos(Angle)) * HornLine;
			}
			else
			{
				Angle = Angle / 2.f;
				Angle = Angle*PI / 180.f;
				PointIntersect = CurrentPoint + (ExtrudeDistance / FMath::Sin(Angle)) * HornLine;
			}
		}

		ResultPoints.Push(FVector(PointIntersect.X, PointIntersect.Y, InPoints[j].Z));
	}

	return ResultPoints;

}

void FSCTXRMath::ReversePointList(TArray<FVector>& PointList)
{
	FVector Temp;
	int32 i, c;

	c = PointList.Num() / 2;
	for (i = 0; i < c; i++)
	{
		Temp = PointList[i];

		PointList[i] = PointList[(PointList.Num() - 1) - i];
		PointList[(PointList.Num() - 1) - i] = Temp;
	}
}

bool FSCTXRMath::TriangulatePoly(TArray<FClipSMTriangle>& OutTris, const FClipSMPolygon& InPoly, bool bKeepColinearVertices /*= false*/)
{
	// Can't work if not enough verts for 1 triangle
	if (InPoly.Vertices.Num() < 3)
	{
		// Return true because poly is already a tri
		return true;
	}

	// Vertices of polygon in order - make a copy we are going to modify.
	TArray<FClipSMVertex> PolyVerts = InPoly.Vertices;

	// Keep iterating while there are still vertices
	while (true)
	{
		if (!bKeepColinearVertices)
		{
			// Cull redundant vertex edges from the polygon.
			for (int32 VertexIndex = 0; VertexIndex < PolyVerts.Num(); VertexIndex++)
			{
				const int32 I0 = (VertexIndex + 0) % PolyVerts.Num();
				const int32 I1 = (VertexIndex + 1) % PolyVerts.Num();
				const int32 I2 = (VertexIndex + 2) % PolyVerts.Num();
				if (AreEdgesMergeable(PolyVerts[I0], PolyVerts[I1], PolyVerts[I2]))
				{
					PolyVerts.RemoveAt(I1);
					VertexIndex--;
				}
			}
		}

		if (PolyVerts.Num() < 3)
		{
			break;
		}
		else
		{
			// Look for an 'ear' triangle
			bool bFoundEar = false;
			for (int32 EarVertexIndex = 0; EarVertexIndex < PolyVerts.Num(); EarVertexIndex++)
			{
				// Triangle is 'this' vert plus the one before and after it
				const int32 AIndex = (EarVertexIndex == 0) ? PolyVerts.Num() - 1 : EarVertexIndex - 1;
				const int32 BIndex = EarVertexIndex;
				const int32 CIndex = (EarVertexIndex + 1) % PolyVerts.Num();

				// Check that this vertex is convex (cross product must be positive)
				const FVector ABEdge = PolyVerts[BIndex].Pos - PolyVerts[AIndex].Pos;
				const FVector ACEdge = PolyVerts[CIndex].Pos - PolyVerts[AIndex].Pos;
				const float TriangleDeterminant = (ABEdge ^ ACEdge) | InPoly.FaceNormal;
				if (FMath::IsNegativeFloat(TriangleDeterminant))
				{
					continue;
				}

				bool bFoundVertInside = false;
				// Look through all verts before this in array to see if any are inside triangle
				for (int32 VertexIndex = 0; VertexIndex < PolyVerts.Num(); VertexIndex++)
				{
					if (VertexIndex != AIndex && VertexIndex != BIndex && VertexIndex != CIndex &&
						PointInTriangle(PolyVerts[AIndex].Pos, PolyVerts[BIndex].Pos, PolyVerts[CIndex].Pos, PolyVerts[VertexIndex].Pos))
					{
						bFoundVertInside = true;
						break;
					}
				}

				// Triangle with no verts inside - its an 'ear'! 
				if (!bFoundVertInside)
				{
					// Add to output list..
					FClipSMTriangle NewTri(0);
					NewTri.CopyFace(InPoly);
					NewTri.Vertices[0] = PolyVerts[AIndex];
					NewTri.Vertices[1] = PolyVerts[BIndex];
					NewTri.Vertices[2] = PolyVerts[CIndex];
					OutTris.Add(NewTri);

					// And remove vertex from polygon
					PolyVerts.RemoveAt(EarVertexIndex);

					bFoundEar = true;
					break;
				}
			}

			// If we couldn't find an 'ear' it indicates something is bad with this polygon - discard triangles and return.
			if (!bFoundEar)
			{
				OutTris.Empty();
				return false;
			}
		}
	}

	return true;
}

bool FSCTXRMath::TriangulatePoly(TArray<FVector>& OutTris, TArray<int32>& OutIndexList, const TArray<FVector>& InPointList, const FVector &FaceNormal, bool bKeepColinearVertices /*= false*/)
{
	OutTris.Empty();
	OutIndexList.Empty();

	TArray<FClipSMTriangle> InOutTris;
	FClipSMPolygon CurrentPoly(0);
	CurrentPoly.FaceNormal = FaceNormal;
	for (auto& It : InPointList)
	{
		FClipSMVertex CurrentVertex;
		CurrentVertex.Pos = It;
		CurrentPoly.Vertices.Push(CurrentVertex);
	}
	if (TriangulatePoly(InOutTris, CurrentPoly, bKeepColinearVertices))
	{
		for (auto& It : InOutTris)
		{
			OutTris.Push(It.Vertices[0].Pos);
			OutTris.Push(It.Vertices[1].Pos);
			OutTris.Push(It.Vertices[2].Pos);
		}

		for (auto& It : OutTris)
		{
			for (int i = 0; i < InPointList.Num(); i++)
			{
				if (It.Equals(InPointList[i], KINDA_SMALL_NUMBER))
				{
					OutIndexList.Push(i);
					break;
				}
			}
		}
		return true;
	}
	else
		return false;
}
bool FSCTXRMath::ArcLineIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, TArray<FVector>& OutInter)
{
	float radius = 0;
	FVector Center(0, 0, 0);
	double x1 = AcrPoint1.X, x2 = AcrPoint2.X, x3 = AcrPoint3.X;
	double y1 = AcrPoint1.Y, y2 = AcrPoint2.Y, y3 = AcrPoint3.Y;
	double a = x1 - x2;
	double b = y1 - y2;
	double c = x1 - x3;
	double d = y1 - y3;
	double e = ((x1 * x1 - x2 * x2) + (y1 * y1 - y2 * y2)) / 2.0;
	double f = ((x1 * x1 - x3 * x3) + (y1 * y1 - y3 * y3)) / 2.0;
	double det = b * c - a * d;
	if (FMath::Abs(det) < 1e-5)
	{
		radius = -1;
	}

	double x0 = -(d * e - b * f) / det;
	double y0 = -(a * f - c * e) / det;
	radius = hypot(x1 - x0, y1 - y0);
	Center = FVector(x0, y0, 0);
	if (radius < 0) return false;

	FVector TheLine = LinePointEnd - LinePointStart;

	FVector ClosestPoint;

	FVector TempNearPoint = FMath::ClosestPointOnInfiniteLine(LinePointStart, LinePointEnd, Center);
	float NearDis = (TempNearPoint - Center).Size();
	if (NearDis <= radius)
	{
		float OffSet = FMath::Sqrt(radius * radius - NearDis * NearDis);
		FVector Inter1 = TempNearPoint + TheLine.GetSafeNormal(1) * OffSet;
		FVector Inter2 = TempNearPoint - TheLine.GetSafeNormal(1) * OffSet;
		if ((Inter1 - AcrPoint1).Size() < 0.0001)
		{
			Inter1 = AcrPoint1;
		}
		else if ((Inter1 - AcrPoint2).Size() < 0.0001)
		{
			Inter1 = AcrPoint2;
		}
		else if ((Inter1 - LinePointStart).Size() < 0.0001)
		{
			Inter1 = LinePointStart;
		}
		else if ((Inter1 - LinePointEnd).Size() < 0.0001)
		{
			Inter1 = LinePointEnd;
		}
		if ((Inter2 - AcrPoint1).Size() < 0.0001)
		{
			Inter2 = AcrPoint1;
		}
		else if ((Inter2 - AcrPoint2).Size() < 0.0001)
		{
			Inter2 = AcrPoint2;
		}
		else if ((Inter2 - LinePointStart).Size() < 0.0001)
		{
			Inter2 = LinePointStart;
		}
		else if ((Inter2 - LinePointEnd).Size() < 0.0001)
		{
			Inter2 = LinePointEnd;
		}
		OutInter.Add(Inter1);
		OutInter.Add(Inter2);
		return true;
	}
	return false;
}
bool FSCTXRMath::ArcBoxIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FBox2D& Box, TArray<FVector>& InterPoints)
{
	float radius = 0;
	FVector Center(0, 0, 0);
	double x1 = AcrPoint1.X, x2 = AcrPoint2.X, x3 = AcrPoint3.X;
	double y1 = AcrPoint1.Y, y2 = AcrPoint2.Y, y3 = AcrPoint3.Y;
	double a = x1 - x2;
	double b = y1 - y2;
	double c = x1 - x3;
	double d = y1 - y3;
	double e = ((x1 * x1 - x2 * x2) + (y1 * y1 - y2 * y2)) / 2.0;
	double f = ((x1 * x1 - x3 * x3) + (y1 * y1 - y3 * y3)) / 2.0;
	double det = b * c - a * d;
	if (FMath::Abs(det) < 1e-5)
	{
		radius = -1;
	}

	double x0 = -(d * e - b * f) / det;
	double y0 = -(a * f - c * e) / det;
	radius = hypot(x1 - x0, y1 - y0);
	Center = FVector(x0, y0, 0);
	if (radius < 0) return false;

	Box.Min; Box.Max;
	FVector LT(Box.Min.X, Box.Max.Y, 0);
	FVector LB(Box.Min.X, Box.Min.Y, 0);
	FVector RB(Box.Max.X, Box.Min.Y, 0);
	FVector RT(Box.Max.X, Box.Max.Y, 0);

	FVector NearP1 = FMath::ClosestPointOnLine(LT, LB, Center);
	FVector NearP2 = FMath::ClosestPointOnLine(LB, RB, Center);
	FVector NearP3 = FMath::ClosestPointOnLine(RB, RT, Center);
	FVector NearP4 = FMath::ClosestPointOnLine(RT, LT, Center);

	FVector RefeDir = AcrPoint3 - (AcrPoint1 + AcrPoint2) / 2;

	FVector Dir1 = NearP1 - Center;
	bool Inter = false;
	if (Dir1.Size() <= radius && ((LT - Center).Size() >= radius || (LB - Center).Size() >= radius))
	{
		FVector LDir = LT - LB;
		if (FVector::DotProduct(Dir1, LDir) == 0)
		{
			float HalfL = FMath::Sqrt(radius * radius - Dir1.Size() * Dir1.Size());
			if (HalfL == 0)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP1);
				if (((NearP1 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP1);
					Inter = true;
				}
			}
			else
			{
				FVector InterP1 = NearP1 + LDir.GetSafeNormal() * HalfL;
				FVector InterP2 = NearP1 - LDir.GetSafeNormal() * HalfL;

				FVector TempNearLineP1 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP1);
				FVector TempNearLineP2 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP2);
				if (((InterP1 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP1);
					Inter = true;
				}
				if (((InterP2 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP2);
					Inter = true;
				}
			}
		}
		else
		{
			if (Dir1.Size() == radius)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP1);
				if (((NearP1 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP1);
					Inter = true;
				}
			}
			else
			{
				FVector TempNearP = FMath::ClosestPointOnInfiniteLine(LT, LB, Center);
				FVector OffSet = TempNearP - Center;
				float TempV = FMath::Sqrt(radius * radius - OffSet.Size() * OffSet.Size());

				FVector InterPoint = TempNearP + (LT - TempNearP).GetSafeNormal() * TempV;

				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterPoint);
				if (((InterPoint - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterPoint);
					Inter = true;
				}
			}
		}
	}
	FVector Dir2 = NearP2 - Center;
	if (Dir2.Size() <= radius && ((RB - Center).Size() >= radius || (LB - Center).Size() >= radius))
	{
		FVector LDir = LB - RB;
		if (FVector::DotProduct(Dir2, LDir) == 0)
		{
			float HalfL = FMath::Sqrt(radius * radius - Dir2.Size() * Dir2.Size());
			if (HalfL == 0)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP2);
				if (((NearP2 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP2);
					Inter = true;
				}
			}
			else
			{
				FVector InterP1 = NearP2 + LDir.GetSafeNormal() * HalfL;
				FVector InterP2 = NearP2 - LDir.GetSafeNormal() * HalfL;

				FVector TempNearLineP1 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP1);
				FVector TempNearLineP2 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP2);
				if (((InterP1 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP1);
					Inter = true;
				}
				if (((InterP2 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP2);
					Inter = true;
				}
			}
		}
		else
		{
			if (Dir2.Size() == radius)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP2);
				if (((NearP2 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP2);
					Inter = true;
				}
			}
			else
			{
				FVector TempNearP = FMath::ClosestPointOnInfiniteLine(LB, RB, Center);
				FVector OffSet = TempNearP - Center;
				float TempV = FMath::Sqrt(radius * radius - OffSet.Size() * OffSet.Size());

				FVector InterPoint = TempNearP + (LB - TempNearP).GetSafeNormal() * TempV;

				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterPoint);
				if (((InterPoint - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterPoint);
					Inter = true;
				}
			}
		}
	}
	FVector Dir3 = NearP3 - Center;
	if (Dir3.Size() <= radius && ((RB - Center).Size() >= radius || (RT - Center).Size() >= radius))
	{
		FVector LDir = RB - RT;
		if (FVector::DotProduct(Dir3, LDir) == 0)
		{
			float HalfL = FMath::Sqrt(radius * radius - Dir3.Size() * Dir3.Size());
			if (HalfL == 0)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP3);
				if (((NearP3 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP3);
					Inter = true;
				}
			}
			else
			{
				FVector InterP1 = NearP3 + LDir.GetSafeNormal() * HalfL;
				FVector InterP2 = NearP3 - LDir.GetSafeNormal() * HalfL;

				FVector TempNearLineP1 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP1);
				FVector TempNearLineP2 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP2);
				if (((InterP1 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP1);
					Inter = true;
				}
				if (((InterP2 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP2);
					Inter = true;
				}
			}
		}
		else
		{
			if (Dir3.Size() == radius)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP3);
				if (((NearP3 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP3);
					Inter = true;
				}
			}
			else
			{
				FVector TempNearP = FMath::ClosestPointOnInfiniteLine(RB, RT, Center);
				FVector OffSet = TempNearP - Center;
				float TempV = FMath::Sqrt(radius * radius - OffSet.Size() * OffSet.Size());

				FVector InterPoint = TempNearP + (RB - TempNearP).GetSafeNormal() * TempV;

				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterPoint);
				if (((InterPoint - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterPoint);
					Inter = true;
				}
			}
		}
	}
	FVector Dir4 = NearP4 - Center;
	if (Dir4.Size() <= radius && ((LT - Center).Size() >= radius || (RT - Center).Size() >= radius))
	{
		FVector LDir = RT - LT;
		if (FVector::DotProduct(Dir4, LDir) == 0)
		{
			float HalfL = FMath::Sqrt(radius * radius - Dir4.Size() * Dir4.Size());
			if (HalfL == 0)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP4);
				if (((NearP4 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP4);
					Inter = true;
				}
			}
			else
			{
				FVector InterP1 = NearP4 + LDir.GetSafeNormal() * HalfL;
				FVector InterP2 = NearP4 - LDir.GetSafeNormal() * HalfL;

				FVector TempNearLineP1 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP1);
				FVector TempNearLineP2 = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterP2);
				if (((InterP1 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP1);
					Inter = true;
				}
				if (((InterP2 - TempNearLineP1).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterP2);
					Inter = true;
				}
			}
		}
		else
		{
			if (Dir4.Size() == radius)
			{
				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, NearP4);
				if (((NearP4 - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(NearP4);
					Inter = true;
				}
			}
			else
			{
				FVector TempNearP = FMath::ClosestPointOnInfiniteLine(RT, LT, Center);
				FVector OffSet = TempNearP - Center;
				float TempV = FMath::Sqrt(radius * radius - OffSet.Size() * OffSet.Size());

				FVector InterPoint = TempNearP + (RT - TempNearP).GetSafeNormal() * TempV;

				FVector TempNearLineP = FMath::ClosestPointOnLine(AcrPoint1, AcrPoint2, InterPoint);
				if (((InterPoint - TempNearLineP).GetSafeNormal() - RefeDir.GetSafeNormal()).Size() < 0.00001)
				{
					InterPoints.Add(InterPoint);
					Inter = true;
				}
			}
		}
	}
	return Inter;
}
bool FSCTXRMath::ArcPointIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& Point, float tolerance, FVector& InterPoint)
{
	FVector Center;
	double R;
	if (Circle(AcrPoint1, AcrPoint2, AcrPoint3, Center, R))
	{
		if ((Point - AcrPoint1).Size() <= tolerance)
		{
			InterPoint = AcrPoint1;
			return true;
		}
		else if ((Point - AcrPoint2).Size() <= tolerance)
		{
			InterPoint = AcrPoint2;
			return true;
		}
		else if (FMath::Abs((Point - Center).Size() - R) <= tolerance)
		{
			AcrPoint1 - AcrPoint2;
			FVector NearP3 = FMath::ClosestPointOnInfiniteLine(AcrPoint1, AcrPoint2, AcrPoint3);
			FVector NearPTemp = FMath::ClosestPointOnInfiniteLine(AcrPoint1, AcrPoint2, Point);

			if (((Point - NearPTemp).GetSafeNormal() - (AcrPoint3 - NearP3).GetSafeNormal()).Size() < 0.00001)
			{
				InterPoint = (Point - Center).GetSafeNormal() * R;
				return true;
			}
		}
	}
	return false;
}

bool FSCTXRMath::ArcLineIntersectionWithOutInifinet(const FVector& ArcPoint1, const FVector& ArcPoint2, const FVector& ArcPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, FVector& OutInter)
{
	TArray<FVector> IntersectonPoints;
	if (ArcLineIntersection(ArcPoint1, ArcPoint2, ArcPoint3, LinePointStart, LinePointEnd, IntersectonPoints))
	{
		if (IntersectonPoints.Num() > 0)
		{
			/*for (int i = 0; i < IntersectonPoints.Num(); i++)
			{
				FVector tempPoint;
				if(ArcPointIntersection(ArcPoint1,ArcPoint2,ArcPoint3,IntersectonPoints[i],0.00000f,tempPoint))
				{
					OutInter = tempPoint;
					return true;
				}
				else
				{
					return false;
				}
			}*/
			OutInter = IntersectonPoints[0];
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;

}

bool FSCTXRMath::Circle(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, FVector& OutCenter, double &OutRadius)
{
	double x1 = AcrPoint1.X, x2 = AcrPoint2.X, x3 = AcrPoint3.X;
	double y1 = AcrPoint1.Y, y2 = AcrPoint2.Y, y3 = AcrPoint3.Y;
	double a = x1 - x2;
	double b = y1 - y2;
	double c = x1 - x3;
	double d = y1 - y3;
	double e = ((x1 * x1 - x2 * x2) + (y1 * y1 - y2 * y2)) / 2.0;
	double f = ((x1 * x1 - x3 * x3) + (y1 * y1 - y3 * y3)) / 2.0;
	double det = b * c - a * d;
	if (FMath::Abs(det) < 1e-5)
	{
		OutRadius = -1;
	}

	double x0 = -(d * e - b * f) / det;
	double y0 = -(a * f - c * e) / det;
	OutRadius = FMath::Sqrt(FMath::Square<double>(x1 - x0) + FMath::Square<double>(y1 - y0));
	OutCenter = FVector(x0, y0, 0);
	if (OutRadius < 0) return false;
	return true;
}
int dcmp(double x) {
	if (FMath::Abs(x) < 0.00001) return 0;
	return x < 0 ? -1 : 1;
}
int FSCTXRMath::ArcAndArcIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& AcrPoint4, const FVector& AcrPoint5, const FVector& AcrPoint6, TArray<FVector>& OutPoint)
{
	FVector Circle1Center, Circle2Center;
	double Circle1Radius, Circle2Radius;
	if (!Circle(AcrPoint1, AcrPoint2, AcrPoint3, Circle1Center, Circle1Radius)) return 0;
	if (!Circle(AcrPoint4, AcrPoint5, AcrPoint6, Circle2Center, Circle2Radius)) return 0;

	double d, a, b, c, p, q, r;
	double cos_value[2], sin_value[2];
	d = (Circle1Center - Circle2Center).Size();
	if (d < 0.0001) {
		return -1;
	}

	if (d > Circle1Radius + Circle2Radius
		|| d < FMath::Abs(Circle1Radius - Circle2Radius)) {
		return 0;
	}

	a = 2.0 * Circle1Radius * (Circle1Center.X - Circle2Center.X);
	b = 2.0 * Circle1Radius * (Circle1Center.Y - Circle2Center.Y);
	c = Circle2Radius * Circle2Radius - Circle1Radius * Circle1Radius
		- d * d;
	p = a * a + b * b;
	q = -2.0 * a * c;
	if (FMath::IsNearlyEqual(d, Circle1Radius + Circle2Radius)
		|| FMath::IsNearlyEqual(d, FMath::Abs(Circle1Radius - Circle2Radius))) {
		cos_value[0] = -q / p / 2.0;
		sin_value[0] = sqrt(1 - cos_value[0] * cos_value[0]);

		FVector TempPoint(0, 0, 0);
		TempPoint.X = Circle1Radius * cos_value[0] + Circle1Center.X;
		TempPoint.Y = Circle1Radius * sin_value[0] + Circle1Center.Y;


		if (!FMath::IsNearlyEqual(double((TempPoint - Circle2Center).Size()), Circle2Radius)) {
			TempPoint.Y = Circle1Center.Y - Circle1Radius * sin_value[0];
		}
		if ((TempPoint - AcrPoint1).Size() < 0.0001)
		{
			TempPoint = AcrPoint1;
		}
		if ((TempPoint - AcrPoint2).Size() < 0.0001)
		{
			TempPoint = AcrPoint2;
		}
		if ((TempPoint - AcrPoint3).Size() < 0.0001)
		{
			TempPoint = AcrPoint3;
		}
		if ((TempPoint - AcrPoint4).Size() < 0.0001)
		{
			TempPoint = AcrPoint4;
		}
		OutPoint.Push(TempPoint);
		return 1;
	}

	r = c * c - b * b;
	cos_value[0] = (sqrt(q * q - 4.0 * p * r) - q) / p / 2.0;
	cos_value[1] = (-sqrt(q * q - 4.0 * p * r) - q) / p / 2.0;
	sin_value[0] = sqrt(1 - cos_value[0] * cos_value[0]);
	sin_value[1] = sqrt(1 - cos_value[1] * cos_value[1]);

	FVector TempPoint1(0, 0, 0), TempPoint2(0, 0, 0);

	TempPoint1.X = Circle1Radius * cos_value[0] + Circle1Center.X;
	TempPoint2.X = Circle1Radius * cos_value[1] + Circle1Center.X;
	TempPoint1.Y = Circle1Radius * sin_value[0] + Circle1Center.Y;
	TempPoint2.Y = Circle1Radius * sin_value[1] + Circle1Center.Y;

	float v = (TempPoint1 - Circle2Center).Size();
	float v2 = (TempPoint2 - Circle2Center).Size();
	if (!FMath::IsNearlyEqual(double((TempPoint1 - Circle2Center).Size()), Circle2Radius, 0.0001)) {
		TempPoint1.Y = Circle1Center.Y - Circle1Radius * sin_value[0];
	}
	if (!FMath::IsNearlyEqual(double((TempPoint2 - Circle2Center).Size()), Circle2Radius, 0.0001)) {
		TempPoint2.Y = Circle1Center.Y - Circle1Radius * sin_value[1];
	}
	if (FMath::IsNearlyEqual(TempPoint1.Y, TempPoint2.Y)
		&& FMath::IsNearlyEqual(TempPoint1.X, TempPoint2.X)) {
		if (TempPoint1.Y > 0) {
			TempPoint2.Y = -TempPoint2.Y;
		}
		else {
			TempPoint1.Y = -TempPoint1.Y;
		}
	}
	if ((TempPoint1 - AcrPoint1).Size() < 0.0001)
	{
		TempPoint1 = AcrPoint1;
	}
	if ((TempPoint1 - AcrPoint2).Size() < 0.0001)
	{
		TempPoint1 = AcrPoint2;
	}
	if ((TempPoint1 - AcrPoint3).Size() < 0.0001)
	{
		TempPoint1 = AcrPoint3;
	}
	if ((TempPoint1 - AcrPoint4).Size() < 0.0001)
	{
		TempPoint1 = AcrPoint4;
	}
	if ((TempPoint2 - AcrPoint1).Size() < 0.0001)
	{
		TempPoint2 = AcrPoint1;
	}
	if ((TempPoint2 - AcrPoint2).Size() < 0.0001)
	{
		TempPoint2 = AcrPoint2;
	}
	if ((TempPoint2 - AcrPoint3).Size() < 0.0001)
	{
		TempPoint2 = AcrPoint3;
	}
	if ((TempPoint2 - AcrPoint4).Size() < 0.0001)
	{
		TempPoint2 = AcrPoint4;
	}
	OutPoint.Push(TempPoint1);
	OutPoint.Push(TempPoint2);
	return 2;
}

float FSCTXRMath::DistanceLineToLine(const FVector& p1, const FVector& p2, const FVector& p3, const FVector& p4, FVector& IntersectPoint)
{
	float distance;
	float x1 = p1.X; //A点坐标（x1,y1,z1）
	float y1 = p1.Y;
	float z1 = p1.Z;
	float x2 = p2.X; //B点坐标（x2,y2,z2）
	float y2 = p2.Y;
	float z2 = p2.Z;
	float x3 = p3.X; //C点坐标（x3,y3,z3）
	float y3 = p3.Y;
	float z3 = p3.Z;
	float x4 = p4.X; //D点坐标（x4,y4,z4）
	float y4 = p4.Y;
	float z4 = p4.Z;

	float a = (p2 - p1).SizeSquared();//(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1);
	float b = -(p2 - p1) | (p4 - p3); //-((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
	float c = -(p1 - p2) | (p1 - p3);// -((x1-x2)*(x1-x3)+(y1-y2)*(y1-y3)+(z1-z2)*(z1-z3));

	float d = -(p2 - p1) | (p4 - p3);//-((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
	float e = (p4 - p3).SizeSquared();//(x4-x3)*(x4-x3)+(y4-y3)*(y4-y3)+(z4-z3)*(z4-z3);
	float f = -(p1 - p3) | (p4 - p3);// -((x1-x3)*(x4-x3)+(y1-y3)*(y4-y3)+(z1-z3)*(z4-z3));

	if ((a*e - b*d) == 0 && (b*d - a*e) == 0) //平行
	{
		float d1 = (p1 - p3).Size();
		float d2 = (p1 - p4).Size();
		distance = (d1 < d2) ? d1 : d2;
		IntersectPoint = FVector::ZeroVector;
		return distance;
	}

	float s = (b*f - e*c) / (a*e - b*d);
	float t = (a*f - d*c) / (b*d - a*e);

	if (0 <= s&&s <= 1 && 0 <= t&&t <= 1) //说明P点落在线段AB上,Q点落在线段CD上
	{
		//2条线段的公垂线段PQ;
		//P点坐标
		float X = x1 + s*(x2 - x1);
		float Y = y1 + s*(y2 - y1);
		float Z = z1 + s*(z2 - z1);
		//Q点坐标
		float U = x3 + t*(x4 - x3);
		float V = y3 + t*(y4 - y3);
		float W = z3 + t*(z4 - z3);
		FVector P(X, Y, Z);
		FVector Q(U, V, W);
		distance = (P - Q).Size();
		IntersectPoint = P;
	}
	else
	{
		float d1 = FMath::PointDistToLine(p1, p3 - p4, p3);//DistancePointToLine (p3,p4,p1);
		float d2 = FMath::PointDistToLine(p2, p3 - p4, p3);//DistancePointToLine (p3,p4,p2);
		float d3 = FMath::PointDistToLine(p3, p1 - p2, p1);//DistancePointToLine (p1,p2,p3);
		float d4 = FMath::PointDistToLine(p4, p1 - p2, p1);//DistancePointToLine (p1,p2,p4);
		distance = (d1 < d2) ? d1 : d2;
		IntersectPoint = (d1 < d2) ? p1 : p2;
		distance = (distance < d3) ? distance : d3;
		IntersectPoint = (distance < d3) ? p2 : p3;
		distance = (distance < d4) ? distance : d4;
		IntersectPoint = (distance < d4) ? p3 : p4;
	}
	return distance;
}

void FSCTXRMath::FootLineToLine (const FVector& p1,const FVector& p2,const FVector& p3,const FVector& p4,FVector& OutPoint0,FVector& OutPoint1)
{
	float x1 = p1.X; //A点坐标（x1,y1,z1）
	float y1 = p1.Y;
	float z1 = p1.Z;
	float x2 = p2.X; //B点坐标（x2,y2,z2）
	float y2 = p2.Y;
	float z2 = p2.Z;
	float x3 = p3.X; //C点坐标（x3,y3,z3）
	float y3 = p3.Y;
	float z3 = p3.Z;
	float x4 = p4.X; //D点坐标（x4,y4,z4）
	float y4 = p4.Y;
	float z4 = p4.Z;

	float a = (p2-p1).SizeSquared ();//(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1);
	float b = -(p2-p1)|(p4-p3); //-((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
	float c = -(p1-p2)|(p1-p3);// -((x1-x2)*(x1-x3)+(y1-y2)*(y1-y3)+(z1-z2)*(z1-z3));

	float d = -(p2-p1)|(p4-p3);//-((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
	float e = (p4-p3).SizeSquared ();//(x4-x3)*(x4-x3)+(y4-y3)*(y4-y3)+(z4-z3)*(z4-z3);
	float f = -(p1-p3)|(p4-p3);// -((x1-x3)*(x4-x3)+(y1-y3)*(y4-y3)+(z1-z3)*(z4-z3));

	if((a*e-b*d)==0&&(b*d-a*e)==0) //平行
	{
		OutPoint0 = OutPoint1 = FVector::ZeroVector;
		return;
	}

	float s = (b*f-e*c)/(a*e-b*d);
	float t = (a*f-d*c)/(b*d-a*e);

	if(0<=s&&s<=1&&0<=t&&t<=1) //说明P点落在线段AB上,Q点落在线段CD上
	{
		//2条线段的公垂线段PQ;
		//P点坐标
		float X = x1+s*(x2-x1);
		float Y = y1+s*(y2-y1);
		float Z = z1+s*(z2-z1);
		//Q点坐标
		float U = x3+t*(x4-x3);
		float V = y3+t*(y4-y3);
		float W = z3+t*(z4-z3);
		FVector P (X,Y,Z);
		FVector Q (U,V,W);
		OutPoint0 = P;
		OutPoint1 = Q;
	}
	else
	{
		OutPoint1 = OutPoint0 = FVector::ZeroVector;
	}
}
