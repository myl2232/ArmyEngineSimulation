#include "ArmyMath.h"
#include "ArmyClipper.h"

//@欧石楠 水电模式管线圆上的顶点数量
static const int32 HyPipeVerticeCount = 32;

static Clipper C;

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


bool FArmyMath::CalculateLinesIntersection(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint, bool bIgnoreParallel/* = true*/ /*忽略共线*/)
{
	const float eplision = 0.01f;
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
		if ((OutPoint - P1).Size() < eplision)
		{
			OutPoint = P1;
		}
		else if ((OutPoint - P2).Size() < eplision)
		{
			OutPoint = P2;
		}
		else if ((OutPoint - P3).Size() < eplision)
		{
			OutPoint = P3;
		}
		else if ((OutPoint - P4).Size() < eplision)
		{
			OutPoint = P4;
		}
		return true;
	}
	else
	{
		if (!bIgnoreParallel)
		{
			//@  考虑两条线段共线
			if (FArmyMath::AreLinesParallel(P1, P2, P3, P4))
			{
				//如果两条线平行，进一步判断是否有部分重合
				if (FArmyMath::IsPointOnLine(P3, P1, P2)
					|| FArmyMath::IsPointOnLine(P4, P1, P2)
					|| FArmyMath::IsPointOnLine(P1, P3, P4)
					|| FArmyMath::IsPointOnLine(P2, P3, P4))
				{
					return true;
				}
			}
		}

		if (P1.Equals(P3, eplision) || P1.Equals(P4, eplision))
		{
			OutPoint = P1;
			return true;
		}
		else if (P2.Equals(P3, eplision) || P2.Equals(P4, eplision))
		{
			OutPoint = P2;
			return true;
		}
	}
	return false;
}

bool FArmyMath::CalculateLine2DIntersection(const FVector&P1, const FVector& P2, const FVector& P3, const FVector& P4, bool bIgnoreParallel/* = true*/ /*忽略共线*/)
{
	FVector outPoint;
	const float eplision = 0.01f;
	if (FMath::SegmentIntersection2D(P1, P2, P3, P4, outPoint))
	{
		FVector2D Line1Dir = FVector2D(P2) - FVector2D(P1);
		FVector2D Line2Dir = FVector2D(P4) - FVector2D(P3);

		Line1Dir.Normalize();
		Line2Dir.Normalize();

		if (Line1Dir.Size() > 0 && Line2Dir.Size() > 0 && Line1Dir.X * Line2Dir.X == 0 && Line1Dir.Y * Line2Dir.Y == 0)
		{
			outPoint.X = Line1Dir.X == 0 ? P1.X : P3.X;
			outPoint.Y = Line1Dir.Y == 0 ? P1.Y : P3.Y;
		}
		if ((outPoint - P1).Size() < eplision)
		{
			return false;
		}
		else if ((outPoint - P2).Size() < eplision)
		{
			return false;
		}
		else if ((outPoint - P3).Size() < eplision)
		{
			return false;
		}
		else if ((outPoint - P4).Size() < eplision)
		{
			return false;
		}
		return true;
	}
	else
	{
		if (!bIgnoreParallel)
		{
			//@  考虑两条线段共线
			if (FArmyMath::AreLinesParallel(P1, P2, P3, P4))
			{
				//如果两条线平行，进一步判断是否有部分重合
				if (FArmyMath::IsPointOnLine(P3, P1, P2)
					|| FArmyMath::IsPointOnLine(P4, P1, P2)
					|| FArmyMath::IsPointOnLine(P1, P3, P4)
					|| FArmyMath::IsPointOnLine(P2, P3, P4))
				{
					return true;
				}
			}
		}

		if (P1.Equals(P3, eplision) || P1.Equals(P4, eplision))
		{
			return false;
		}
		else if (P2.Equals(P3, eplision) || P2.Equals(P4, eplision))
		{
			return false;
		}
	}
	return false;
}

bool FArmyMath::SegmentIntersection2D(const FVector & SegmentStartA, const FVector & SegmentEndA, const FVector & SegmentStartB, const FVector & SegmentEndB, FVector & out_IntersectionPoint)
{
	const FVector VectorA = SegmentEndA - SegmentStartA;
	const FVector VectorB = SegmentEndB - SegmentStartB;

	const float S = (-VectorA.Y * (SegmentStartA.X - SegmentStartB.X) + VectorA.X * (SegmentStartA.Y - SegmentStartB.Y)) / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);
	const float T = (VectorB.X * (SegmentStartA.Y - SegmentStartB.Y) - VectorB.Y * (SegmentStartA.X - SegmentStartB.X)) / (-VectorB.X * VectorA.Y + VectorA.X * VectorB.Y);

	const bool bIntersects = ((FMath::IsNearlyEqual(S, 0) || S >= 0) && (FMath::IsNearlyEqual(S, 1) || S <= 1) && (FMath::IsNearlyEqual(T, 0) || T >= 0) && (FMath::IsNearlyEqual(T, 1) || T <= 1));

	if (bIntersects)
	{
		out_IntersectionPoint.X = SegmentStartA.X + (T * VectorA.X);
		out_IntersectionPoint.Y = SegmentStartA.Y + (T * VectorA.Y);
		out_IntersectionPoint.Z = SegmentStartA.Z + (T * VectorA.Z);
	}

	return bIntersects;
}

bool FArmyMath::CalculateLinesIntersection3DTo2D(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint)
{
	FVector P3T2 = P3;
	FVector P4T2 = P4;
	P3T2.Z = 0;
	P4T2.Z = 0;
	return CalculateLinesIntersection(P1, P2, P3T2, P4T2, OutPoint);
}

bool FArmyMath::CalculateLinesCross(const FVector& P1, const FVector& P2, const FVector& P3, const FVector& P4, FVector& OutPoint)
{
	if (CalculateLinesIntersection3DTo2D(P1, P2, P3, P4, OutPoint))
	{
		if (IsPointOnLine(P1, P3, P4) || IsPointOnLine(P2, P3, P4) || IsPointOnLine(P3, P1, P2) || IsPointOnLine(P4, P1, P2))
		{
			return false;
		}
	}
	else
		return false;

	return true;
}

#define  SCALE 10000.0f
bool FArmyMath::ArePolysOverlap(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2)
{
	C.Clear();

	Path PolyPath;
	for (auto& It : Poly1)
	{
		PolyPath.push_back(IntPoint(It.X * SCALE, It.Y * SCALE));
	}
	C.AddPath(PolyPath, ptSubject, true);

	PolyPath.clear();

	for (auto& It : Poly2)
	{
		PolyPath.push_back(IntPoint(It.X * SCALE, It.Y * SCALE));
	}
	C.AddPath(PolyPath, ptClip, true);

	Paths ResultPaths;
	if (C.Execute(ctIntersection, ResultPaths, pftEvenOdd, pftEvenOdd))
	{
		return ResultPaths.size() > 0;
	}

	return false;
}

bool FArmyMath::ArePolysIntersection(const TArray<FVector>& Poly1, const TArray<FVector>& Poly2, bool bSideOverlap /*= false*/ /*考虑边重合*/)
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
			StartA.Z = 0;
			StartB.Z = 0;
			EndA.Z = 0;
			EndB.Z = 0;
			bool bCross = CalculateLine2DIntersection(StartA, EndA, StartB, EndB, !bSideOverlap);
			if (bCross)
			{
				return true;
			}
		}
	}

	return false;
}

bool FArmyMath::IsPolygonSimple(TArray<FVector>& InVertices)
{
	CleanPolygon(InVertices);
	if (InVertices.Num() == 3)
		return true;
	const int number = InVertices.Num();
	for (int i0 = 0; i0 < number; ++i0)
	{
		int i0P1 = (i0 + 1) % number;
		FVector startPos = InVertices[i0];
		FVector endPos = InVertices[i0P1];
		int i1min = (i0 + 2) % number;
		int i1max = (i0 - 2 + number) % number;
		for (int i1 = i1min; i1 <= i1max; ++i1)
		{
			int i1p1 = (i1 + 1) % number;
			FVector tempStartPos = InVertices[i1];
			FVector tempEndps = InVertices[i1p1];
			FVector intersection;
			if (CalculateLinesIntersection(startPos, endPos, tempStartPos, tempEndps, intersection))
				return false;
		}
	}
	return true;
}

const FVector FArmyMath::GetProjectionPoint(const FVector& Point, const FVector& V0, const FVector& V1)
{
	return (FVector::DotProduct((Point - V0), (V1 - V0))* (V1 - V0)) / (FVector::Distance(V1, V0) * FVector::Distance(V1, V0)) + V0;
}

const float FArmyMath::Distance(const FVector& Point, const FVector& V0, const FVector& V1)
{
	//@刘克祥 计算线段V1V0的长度的平方
	float DistSquaredV1V0 = FVector::DistSquared(V1, V0);
	//@刘克祥 计算点乘
	float DotProduct = FVector::DotProduct((Point - V0), (V1 - V0));
	//@刘克祥 计算PV长度的平方
	float DistSquaredPV0 = FVector::DistSquared(V0, Point);
	/**@欧石楠 可能存在负数*/
	float TempValue = DistSquaredPV0 - (DotProduct *DotProduct / DistSquaredV1V0);
	if (TempValue < 0)
	{
		TempValue = 0;
	}
	//@刘克祥 返回长度
	return FMath::Sqrt(TempValue);
}

const bool FArmyMath::NearlyPoint(const FVector& Point, const FVector& V0, const FVector& V1, FVector& OutLocation, float ErrorTolerance)
{
	//投影的点
	FVector ProjectionPoint = FArmyMath::GetProjectionPoint(Point, V0, V1);
	//点到最近点向量
	FVector Direction = FVector::ZeroVector;
	float distance = 0.f;
	if (FArmyMath::IsPointOnLine(ProjectionPoint, V0, V1))
	{
		Direction = ProjectionPoint - Point;
		distance = Direction.Size();
		if (distance <= ErrorTolerance && !ProjectionPoint.ContainsNaN())
		{
			OutLocation = ProjectionPoint;
			return true;
		}
	}
	else
	{
		Direction = Point - V0;
		distance = Direction.Size();
		if (distance <= ErrorTolerance && !V0.ContainsNaN())
		{
			OutLocation = V0;
			return true;
		}
		Direction = Point - V1;
		distance = Direction.Size();
		if (distance <= ErrorTolerance && !V1.ContainsNaN())
		{
			OutLocation = V1;
			return true;
		}
	}
	return false;
}

const bool FArmyMath::IsPointOnLine(const FVector& Point, const FVector& V0, const FVector& V1, float Tolerance)
{
	if (Point.Equals(V0, Tolerance) || Point.Equals(V1, Tolerance))
		return true;
	FVector P0Normal = (Point - V0).GetSafeNormal();
	FVector P1Normal = (Point - V1).GetSafeNormal();
	if ((P0Normal + P1Normal).IsNearlyZero(Tolerance))
	{
		return true;
	}
	return false;

}

const bool FArmyMath::IsPointOnLineExcludeEndPoint(const FVector& Point, const FVector& V0, const FVector& V1, float Tolerance /*= KINDA_SMALL_NUMBER*/)
{
	if (Point.Equals(V0) || Point.Equals(V1))
		return false;

	return IsPointOnLine(Point, V0, V1, Tolerance);
}

float Perp(const FVector2D& u, const FVector2D& v)
{
	return u.X * v.Y - u.Y * v.X;
}
bool FArmyMath::IntersectSegmentWithConvexPolygon(const FVector2D& startPos, const FVector2D& EndPos, const TArray<FVector2D>& ConverxPolygon, TArray<FVector>& intersectionPos)
{
	TArray<FVector> IntersectPos;

	TArray<FVector> outArea;
	for (const FVector2D& iter : ConverxPolygon)
		outArea.Emplace(iter, 0.0f);

	if (startPos.Equals(EndPos))
	{
		if (IsPointInPolygon2D(FVector(startPos, 0.0f), outArea))
		{
			IntersectPos.Emplace(startPos, 0.0);
			return true;
		}
		else
			return false;
	}

	if (outArea.Num() < 4)
		return false;

	// float angle = 90;
	// FVector dir = (outArea[1] - outArea[0]).GetSafeNormal();
	// FVector outDir = dir.RotateAngleAxis(angle, FVector(0, 0, 1));
	// FVector centerPos = (outArea[1] + outArea[0]) / 2.0f;
	// centerPos = centerPos + outDir * 3.0f;
	// if (IsPointInPolygon2D(centerPos, outArea))
	// angle = -90;
	// float tE = 0;
	// float tL = 1;
	// float t, N, D;
	// FVector2D direction = (EndPos - startPos);
	// FVector2D e;
	int number = outArea.Num();

	FVector StartPos3D(startPos, 0.0f);
	FVector EndPos3D(EndPos, 0.0f);
	FVector InterPos;

	for (int i = 0; i < number; ++i)
	{
		// FVector2D tempStart = ConverxPolygon[i%number];
		// FVector2D tempEnd = ConverxPolygon[(i + 1) % number];

		FVector tempStart = outArea[i%number];
		FVector tempEnd = outArea[(i + 1) % number];
		if (SegmentIntersection2D(StartPos3D, EndPos3D, tempStart, tempEnd, InterPos))
			IntersectPos.Emplace(InterPos);


		// e = (tempEnd - tempStart).GetSafeNormal();
		// FVector normal = FVector(e, 0.0f).RotateAngleAxis(angle, FVector(0, 0, 1));
		// D = FVector::DotProduct(normal, FVector(direction, 0.0));
		// N = FVector::DotProduct(FVector((tempStart - startPos), 0.0), normal);
		// if (FMath::Abs(D) < 0.009f)
		// {
		// 	continue;
		// }

		// t = N / D;
		// FVector interPos = FVector(startPos + t * direction, 0.0f);
		// FVector tempDir0 = (FVector(startPos, 0.0f) - interPos).GetSafeNormal();
		// FVector tempDir1 = (FVector(EndPos, 0.0f) - interPos).GetSafeNormal();
		// FVector tempDir2 = (FVector(tempStart, 0.0f) - interPos).GetSafeNormal();
		// FVector tempDir3 = (FVector(tempEnd, 0.0f) - interPos).GetSafeNormal();
		// float result = FVector::DotProduct(tempDir0, tempDir1);
		// float result1 = FVector::DotProduct(tempDir2, tempDir3);
		// if ((interPos.Equals(FVector(tempStart, 0.0), 0.2f) || interPos.Equals(FVector(tempEnd, 0.0f), 0.2f)) && result <= 0.0f)
		// {
		// 	IntersectPos.Emplace(interPos);
		// }
		// else
		// {
		// 	if (interPos.Equals(FVector(startPos, 0.0), 0.1f) || interPos.Equals(FVector(EndPos, 0.0f), 0.1f))
		// 	{
		// 		if (result1 <= 0.0f)
		// 			IntersectPos.Emplace(interPos);
		// 		else if (interPos.Equals(FVector(tempStart, 0.0f), 0.2f) || interPos.Equals(FVector(tempEnd, 0.0f), 0.2f))
		// 			IntersectPos.Emplace(interPos);
		// 	}
		// 	else if (result < 0.0f && result1 < 0.0f)
		// 	{
		// 		IntersectPos.Emplace(interPos);
		// 	}
		// }
	}

	intersectionPos = IntersectPos;
	if (intersectionPos.Num() > 0)
	{
		if (intersectionPos.Num() == 1)
		{
			if (IsPointInOrOnPolygon2D(FVector(startPos, 0.0f), outArea))
			{
				intersectionPos.Emplace(startPos, 0.0f);
			}
			else if (IsPointInOrOnPolygon2D(FVector(EndPos, 0.0f), outArea))
			{
				intersectionPos.Emplace(EndPos, 0.0f);
			}
		}
		return true;
	}
	else if (intersectionPos.Num() == 0)
	{
		// @欧石楠 解决墙线完全包含在Polygon中的情况
		if (IsPointInPolygon2D(FVector(startPos, 0.0f), outArea))
			intersectionPos.Emplace(startPos, 0.0f);
		if (IsPointInPolygon2D(FVector(EndPos, 0.0f), outArea))
			intersectionPos.Emplace(EndPos, 0.0f);
		return true;
	}
	else
		return false;
}

const bool FArmyMath::GetLineSegmentProjectionPos(const FVector& V0, const FVector& V1, FVector& OutPos)
{
	OutPos = GetProjectionPoint(OutPos, V0, V1);

	return IsPointOnLineSegment2D(FVector2D(OutPos), FVector2D(V0), FVector2D(V1), 0.01f);
}

const FVector FArmyMath::GetLineCenter(const FVector& Start, const FVector& End)
{
	return (Start + End) / 2.f;
}


const FVector FArmyMath::GetAngularBisectorDirection(const FVector& P1, const FVector P0, const FVector& P2)
{
	const FVector P0P1 = (P1 - P0).GetSafeNormal();
	const FVector P0P2 = (P2 - P0).GetSafeNormal();
	return GetAngularBisectorDirection(P0P1, P0P2);
}

const FVector FArmyMath::GetAngularBisectorDirection(const FVector& Direction1, const FVector Direction2)
{
	return ((Direction1 + Direction2) / 2).GetSafeNormal();
}

const FVector FArmyMath::GetTangentCenter(const FVector& P1, const FVector P0, const FVector& P2, float Radius)
{
	FVector P0P1 = (P1 - P0).GetSafeNormal();
	//  二分线方向
	FVector Direction = GetAngularBisectorDirection(P1, P0, P2);

	FVector Normal = FVector::CrossProduct(P0P1, Direction);
	float SinQ = Normal.Size();
	float Length = Radius / SinQ;

	return P0 + Length *Direction;
}

const FVector FArmyMath::GetLineDirection(const FVector Start, const FVector End)
{
	FVector Result = FVector::ZeroVector;
	Result = End - Start;
	Result = Result.GetSafeNormal();

	return Result;
}

bool FArmyMath::IsClockWise(const TArray<FVector2D>& Points)
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


bool FArmyMath::IsClockWise(const TArray<FVector>& Points)
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

void FArmyMath::ClockwiseSortPoints(TArray<FVector>& Points)
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

bool FArmyMath::PointCmp(const FVector& A, const FVector& B, const FVector& Center)
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

bool FArmyMath::AreEdgesMergeable2D(const FVector2D& V0, const FVector2D& V1, const FVector2D& V2)
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
bool FArmyMath::IsPointInTriangle2D(const FVector2D& TestPoint, const FVector2D& A, const FVector2D& B, const FVector2D& C)
{
	float BA = VECTOR_SIGN(B, A, TestPoint);
	float CB = VECTOR_SIGN(C, B, TestPoint);
	float AC = VECTOR_SIGN(A, C, TestPoint);

	return BA == CB && CB == AC;
}

bool FArmyMath::IsPointProjectionOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance /*= KINDA_SMALL_NUMBER*/)
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

bool FArmyMath::IsPointOnLineSegment2D(const FVector2D& P, const FVector2D& A, const FVector2D& B, float Tolerance/* = SMALL_NUMBER*/)
{
	auto closetPoint = FMath::ClosestPointOnSegment2D(P, A, B);
	return (closetPoint - P).Size() < Tolerance;
	// //@刘克祥
	// FVector2D DirectPA = (P - A).GetSafeNormal();
	// FVector2D DirectPB = (P - B).GetSafeNormal();
	// float TestResult = FVector2D::CrossProduct(DirectPA, DirectPB);
	// if (FMath::Abs(TestResult) < Tolerance)
	// {
	// 	if (FMath::Min(A.X, B.X) - Tolerance <= P.X &&
	// 		P.X - Tolerance <= FMath::Max(A.X, B.X) &&
	// 		FMath::Min(A.Y, B.Y) - Tolerance <= P.Y &&
	// 		P.Y - Tolerance <= FMath::Max(A.Y, B.Y)
	// 		)
	// 	{
	// 		return true;
	// 	}
	// }


	// return false;
}

bool FArmyMath::IsPointOnLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance /*= KINDA_SMALL_NUMBER*/)
{
	if ((P0 - P1).IsNearlyZero(Tolerance))
		return true;
	if ((P0 - P2).IsNearlyZero(Tolerance))
		return true;
	if ((P1 - P2).IsNearlyZero(Tolerance))
		return true;
	FVector direction0 = (P1 - P0).GetSafeNormal();
	FVector direction1 = (P2 - P0).GetSafeNormal();
	float X0 = FMath::Abs(direction0.X);
	float Y0 = FMath::Abs(direction0.Y);
	float Z0 = FMath::Abs(direction0.Z);
	float X1 = FMath::Abs(direction1.X);
	float Y1 = FMath::Abs(direction1.Y);
	float Z1 = FMath::Abs(direction1.Z);
	if (FMath::IsNearlyEqual(X0, X1, Tolerance) && FMath::IsNearlyEqual(Y0, Y1, Tolerance) && FMath::IsNearlyEqual(Z0, Z1, Tolerance))
	{
		return true;
	}
	return false;
}

bool FArmyMath::IsPointInLineSegment3D(const FVector& P0, const FVector& P1, const FVector& P2, float Tolerance /*= KINDA_SMALL_NUMBER*/)
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

float FArmyMath::CalcPolyArea(const TArray<FVector2D>& Points)
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

float FArmyMath::CalcPolyArea(const TArray<FVector>& Points)
{
	TArray<FVector2D> ConvertPoints;
	for (int i = 0; i < Points.Num(); ++i)
	{
		ConvertPoints.Push(FVector2D(Points[i].X, Points[i].Y));
	}
	return	CalcPolyArea(ConvertPoints);
}
float FArmyMath::CalcPolyArea3D(const TArray<FVector>& Points)
{
	if (Points.Num() <= 2)
	{
		return 0.f;
	}
	const float fPreX = Points[0].X, fPreY = Points[0].Y, fPreZ = Points[0].Z;
	bool  bChooseX = true, bChooseY = true, bChooseZ = true;
	for (auto inputPoint : Points)
	{
		if (fPreX != inputPoint.X && bChooseX)
		{
			bChooseX = false;
		}
		if (fPreY != inputPoint.Y && bChooseY)
		{
			bChooseY = false;
		}
		if (fPreZ != inputPoint.Z && bChooseZ)
		{
			bChooseZ = false;
		}
	}
	TArray<FVector2D> ConvertPoints;
	if (bChooseZ)
	{
		for (int i = 0; i < Points.Num(); ++i)
		{
			ConvertPoints.Push(FVector2D(Points[i].X, Points[i].Y));
		}
	}else if (bChooseX)
	{
		for (int i = 0; i < Points.Num(); ++i)
		{
			ConvertPoints.Push(FVector2D(Points[i].Y, Points[i].Z));
		}
	}else if (bChooseY)
	{
		for (int i = 0; i < Points.Num(); ++i)
		{
			ConvertPoints.Push(FVector2D(Points[i].X, Points[i].Z));
		}
	}
	return CalcPolyArea(ConvertPoints);
}
bool  FArmyMath::LineBoxIntersectionWithHitPoint(
	const FBox& Box,
	const FVector& Start,
	const FVector& End,
	const FVector& Direction,
	FVector& OutHit)
{
	FVector OneOverDirection = Direction.Reciprocal();

	FVector	Time;
	bool bStartIsOutside = false;

	if (Start.X < Box.Min.X)
	{
		bStartIsOutside = true;
		if (End.X >= Box.Min.X)
		{
			Time.X = (Box.Min.X - Start.X) * OneOverDirection.X;
		}
		else
		{
			return false;
		}
	}
	else if (Start.X > Box.Max.X)
	{
		bStartIsOutside = true;
		if (End.X <= Box.Max.X)
		{
			Time.X = (Box.Max.X - Start.X) * OneOverDirection.X;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Time.X = 0.0f;
	}

	if (Start.Y < Box.Min.Y)
	{
		bStartIsOutside = true;
		if (End.Y >= Box.Min.Y)
		{
			Time.Y = (Box.Min.Y - Start.Y) * OneOverDirection.Y;
		}
		else
		{
			return false;
		}
	}
	else if (Start.Y > Box.Max.Y)
	{
		bStartIsOutside = true;
		if (End.Y <= Box.Max.Y)
		{
			Time.Y = (Box.Max.Y - Start.Y) * OneOverDirection.Y;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Time.Y = 0.0f;
	}

	if (Start.Z < Box.Min.Z)
	{
		bStartIsOutside = true;
		if (End.Z >= Box.Min.Z)
		{
			Time.Z = (Box.Min.Z - Start.Z) * OneOverDirection.Z;
		}
		else
		{
			return false;
		}
	}
	else if (Start.Z > Box.Max.Z)
	{
		bStartIsOutside = true;
		if (End.Z <= Box.Max.Z)
		{
			Time.Z = (Box.Max.Z - Start.Z) * OneOverDirection.Z;
		}
		else
		{
			return false;
		}
	}
	else
	{
		Time.Z = 0.0f;
	}

	if (bStartIsOutside)
	{
		const float	MaxTime = FMath::Max3(Time.X, Time.Y, Time.Z);

		if (MaxTime >= 0.0f && MaxTime <= 1.0f)
		{
			OutHit = Start + Direction * MaxTime;
			const float BOX_SIDE_THRESHOLD = 0.1f;
			if (OutHit.X > Box.Min.X - BOX_SIDE_THRESHOLD && OutHit.X < Box.Max.X + BOX_SIDE_THRESHOLD &&
				OutHit.Y > Box.Min.Y - BOX_SIDE_THRESHOLD && OutHit.Y < Box.Max.Y + BOX_SIDE_THRESHOLD &&
				OutHit.Z > Box.Min.Z - BOX_SIDE_THRESHOLD && OutHit.Z < Box.Max.Z + BOX_SIDE_THRESHOLD)
			{
				return true;
			}
		}

		return false;
	}
	else
	{
		return true;
	}
}

bool FArmyMath::IsPointInPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints, float Tolerance)
{
	int32 count = PolygonPoints.Num();

	if (count < 3)
	{
		return false;
	}

	float MaxX = PolygonPoints[0].X;
	float MaxY = PolygonPoints[0].Y;
	float MinX = MaxX;
	float MinY = MaxY;

	for (auto PolygonPoint : PolygonPoints)
	{
		if (PolygonPoint.X > MaxX)
		{
			MaxX = PolygonPoint.X;
		}

		if (PolygonPoint.Y > MaxY)
		{
			MaxY = PolygonPoint.Y;
		}

		if (PolygonPoint.X < MinX)
		{
			MinX = PolygonPoint.X;
		}

		if (PolygonPoint.Y < MinY)
		{
			MinY = PolygonPoint.Y;
		}
	}

	if (!(MinX - Tolerance <= TestPoint.X
		&& MinY - Tolerance <= TestPoint.Y
		&& MaxX + Tolerance >= TestPoint.X
		&& MaxY + Tolerance >= TestPoint.Y)
		)
	{
		return false;
	}

	bool result = false;

	for (int i = 0, j = count - 1; i < count; i++)
	{
		FVector p1 = PolygonPoints[i];
		FVector p2 = PolygonPoints[j];

		//@刘克祥 判断测试点是否在边上
		if (IsPointOnLineSegment2D(FVector2D(TestPoint), FVector2D(p1), FVector2D(p2), Tolerance))
		{
			return false;
		}

		if ((p1.Y > TestPoint.Y) != (p2.Y > TestPoint.Y))
		{
			if (TestPoint.X < ((p2.X - p1.X)*(TestPoint.Y - p1.Y) / (p2.Y - p1.Y) + p1.X))
			{
				result = !result;
			}
		}
		j = i;
	}

	return result;
}

bool FArmyMath::IsPointInOrOnPolygon2D(const FVector& TestPoint, const TArray<FVector>& PolygonPoints, float Tolerance)
{
	int32 count = PolygonPoints.Num();

	if (count < 3)
	{
		return false;
	}

	float MaxX = PolygonPoints[0].X;
	float MaxY = PolygonPoints[0].Y;
	float MinX = MaxX;
	float MinY = MaxY;

	for (auto PolygonPoint : PolygonPoints)
	{
		if (PolygonPoint.X > MaxX)
		{
			MaxX = PolygonPoint.X;
		}

		if (PolygonPoint.Y > MaxY)
		{
			MaxY = PolygonPoint.Y;
		}

		if (PolygonPoint.X < MinX)
		{
			MinX = PolygonPoint.X;
		}

		if (PolygonPoint.Y < MinY)
		{
			MinY = PolygonPoint.Y;
		}

		//	if(FMath::IsNearlyEqual(PolygonPoint.Y,TestPoint.Y,))
	}

	if (!(MinX - Tolerance <= TestPoint.X
		&& MinY - Tolerance <= TestPoint.Y
		&& MaxX + Tolerance >= TestPoint.X
		&& MaxY + Tolerance >= TestPoint.Y)
		)
	{
		return false;
	}

	bool result = false;

	for (int i = 0, j = count - 1; i < count; i++)
	{
		FVector p1 = PolygonPoints[i];
		FVector p2 = PolygonPoints[j];

		//@刘克祥 判断测试点是否在边上
		if (IsPointOnLineSegment2D(FVector2D(TestPoint), FVector2D(p1), FVector2D(p2), Tolerance))
			return true;


		if ((p1.Y > TestPoint.Y) != (p2.Y > TestPoint.Y))
		{
			if (TestPoint.X < ((p2.X - p1.X)*(TestPoint.Y - p1.Y) / (p2.Y - p1.Y) + p1.X))
				//if (p1.Y + (TestPoint.X - p1.X) / (p2.X - p1.X) * (p2.Y - p1.Y) < TestPoint.Y)
			{
				result = !result;
			}
		}
		j = i;
	}

	return result;
}

bool FArmyMath::IsPolygonInPolygon(const TArray<FVector>& TestPolygon, const TArray<FVector>& Polygon)
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

bool FArmyMath::IsCircleInPolygon2D(const FVector& CirclePositoin, const float Radius, const TArray<FVector>& PolygonPoints)
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

const float FArmyMath::CalcPointToLineDistance(const FVector& Point, const FVector& V0, const FVector& V1)
{
	FVector ProjectPoint = GetProjectionPoint(Point, V0, V1);
	return FVector::Distance(ProjectPoint, Point);
}

bool FArmyMath::Triangulate2DPoly(TArray<FVector2D>& OutTris, const TArray<FVector2D>& InPolyVerts, bool bKeepColinearVertices /*= false*/)
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
bool FArmyMath::Line2DIntersection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2End, FVector2D& IntersectionPoint)
{
	FVector2D Line1Dir = Line1End - Line1Start;
	FVector2D Line2Dir = Line2End - Line2Start;

	if (Line1Dir.Size() < 0.0001 || Line2Dir.Size() < 0.0001)
	{
		return false;
	}

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

bool FArmyMath::Line2DIntersectionDirection(const FVector2D& Line1Start, const FVector2D& Line1End, const FVector2D& Line2Start, const FVector2D& Line2Direction, FVector2D& IntersectionPoint)
{
	FVector2D Line2End = Line2Start + Line2Direction.GetSafeNormal();
	FVector2D Line1Dir = (Line1End - Line1Start);
	FVector2D Line2Dir = Line2End - Line2Start;

	if (Line1Dir.Size() < 0.0001 || Line2Dir.Size() < 0.0001)
	{
		return false;
	}

	Line1Dir.Normalize();
	Line2Dir.Normalize();
	if ((Line1Dir + Line2Dir).Size() < 0.0001 || (Line1Dir - Line2Dir).Size() < 0.0001)//平行
	{
		return false;
	}

	FVector2D TempInterPoint;
	if (Line1Dir.Size() > 0 && Line2Dir.Size() > 0 && Line1Dir.X * Line2Dir.X == 0 && Line1Dir.Y * Line2Dir.Y == 0)
	{
		TempInterPoint.X = Line1Dir.X == 0 ? Line1Start.X : Line2Start.X;
		TempInterPoint.Y = Line1Dir.Y == 0 ? Line1Start.Y : Line2Start.Y;
	}
	else
	{
		float x1 = Line1Start.X;
		float x2 = Line1End.X;

		float x3 = Line2Start.X;
		float x4 = Line2End.X;

		float y1 = Line1Start.Y;
		float y2 = Line1End.Y;

		float y3 = Line2Start.Y;
		float y4 = Line2End.Y;



		TempInterPoint.X = ((x1 - x2) * (x3 * y4 - x4 * y3) - (x3 - x4) * (x1 * y2 - x2 * y1))
			/ ((x3 - x4) * (y1 - y2) - (x1 - x2) * (y3 - y4));

		TempInterPoint.Y = ((y1 - y2) * (x3 * y4 - x4 * y3) - (x1 * y2 - x2 * y1) * (y3 - y4))
			/ ((y1 - y2) * (x3 - x4) - (x1 - x2) * (y3 - y4));
	}

	bool bOnLine1 = IsPointOnLineSegment2D(TempInterPoint, Line1Start, Line1End, 0.001f);
	if (!bOnLine1)
		return false;
	FVector2D Direction3 = (TempInterPoint - Line2Start).GetSafeNormal();

	if (Direction3.IsNearlyZero(0.001f) || Direction3.Equals(Line2Direction.GetSafeNormal()))
	{
		IntersectionPoint = TempInterPoint;
		return true;
	}
	return false;
}

bool FArmyMath::Line3DIntersectionDirection(const FVector& Line1Start, const FVector& Line1End, const FVector& Line2Start, const FVector& Line2Direction, FVector& IntersectionPoint)
{
	bool bOneLine = IsPointOnLine(Line2Start, Line1Start, Line1End);

	if (bOneLine)
	{
		//IntersectionPoint = Line2Start;
		return false;
	}
	FVector Line2StartTemp = Line2Start;

	FVector Line2End = Line2StartTemp + 2 * Line2Direction.GetSafeNormal();
	FVector Line1Dir = (Line1End - Line1Start).GetSafeNormal();
	FVector Line2Dir = Line2Direction.GetSafeNormal();
	FVector Line3Dir = (Line1Start - Line2StartTemp).GetSafeNormal();
	FVector Line4Dir = (Line1Start - Line2End).GetSafeNormal();

	FPlane Plane(Line1Start, Line1End, Line2StartTemp);
	FVector PlaneNormal = Plane.GetSafeNormal();

	if (Line1Dir.Size() < 0.0001 || Line2Dir.Size() < 0.0001)
	{
		return false;
	}

	Line1Dir.Normalize();
	Line2Dir.Normalize();
	if ((Line1Dir + Line2Dir).Size() < 0.0001 || (Line1Dir - Line2Dir).Size() < 0.0001)//平行
	{
		return false;
	}

	FVector planeCenterPos = Line1Start;//(Line1Start + Line1End + Line2StartTemp)/3;
	FVector planeYDir = Line1Dir;
	FVector planeXDir = (PlaneNormal ^ planeYDir).GetSafeNormal();

	FVector sD1 = (Line1Start - planeCenterPos);
	float s1x = FVector::DotProduct(sD1, planeXDir);
	float s1y = FVector::DotProduct(sD1, planeYDir);

	FVector eD1 = (Line1End - planeCenterPos);
	float e1x = FVector::DotProduct(eD1, planeXDir);
	float e1y = FVector::DotProduct(eD1, planeYDir);

	FVector sD2 = (Line2StartTemp - planeCenterPos);
	float s2x = FVector::DotProduct(sD2, planeXDir);
	float s2y = FVector::DotProduct(sD2, planeYDir);

	FVector eD2 = (Line2End - planeCenterPos);
	float e2x = FVector::DotProduct(eD2, planeXDir);
	float e2y = FVector::DotProduct(eD2, planeYDir);

	FVector2D Line1Start2D(s1x, s1y), Line1End2D(e1x, e1y), Line2Start2D(s2x, s2y), Line2End2D(e2x, e2y), IntersectionPoint2D;

	FVector2D Linedir2d = (Line2End2D - Line2Start2D).GetSafeNormal();
	bool bInter = Line2DIntersectionDirection(Line1Start2D, Line1End2D, Line2Start2D, Linedir2d, IntersectionPoint2D);

	if (!bInter)
		return false;
	IntersectionPoint = IntersectionPoint2D.X * planeXDir + IntersectionPoint2D.Y * planeYDir + planeCenterPos;

	return true;
}


bool FArmyMath::LineBox2DIntersection(const FVector2D& LineStart, const FVector2D& LineEnd, const FBox2D& Box)
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

/**	计算线与多边形的交点 */
bool FArmyMath::CalLinePolygonIntersection(const FVector& LineStart, const FVector& LineEnd, const TArray<FVector>& PolygonPoints, TArray<FVector>&Intersections)
{
	int Count = PolygonPoints.Num();
	if (Count < 3)
	{
		return false;
	}
	for (int i = 0; i < Count; i++)
	{
		FVector CurrentPoint = PolygonPoints[i];
		FVector NextPoint = PolygonPoints[(i+1)%Count];
		FVector Intersection;
		if (CalculateLinesIntersection(LineStart, LineEnd, CurrentPoint, NextPoint, Intersection))
		{
			Intersections.Emplace(Intersection);
		}
	}

	if (Intersections.Num() > 0)
	{
		return true;
	}

	return false;
}

/**	计算线与多边形的重合点，只考虑重合的情况 */
bool FArmyMath::CalLinePolygonCoincidence(const FVector& LineStart, const FVector& LineEnd, const TArray<FVector>& PolygonPoints, TArray<FVector>&Coincidences)
{
	int Count = PolygonPoints.Num();
	if (Count < 3)
	{
		return false;
	}
	float Tolerance = 1.0f;
	for (int i = 0; i < Count; i++)
	{
		FVector CurrentPoint = PolygonPoints[i];
		FVector NextPoint = PolygonPoints[(i + 1) % Count];
		if (AreLinesParallel(LineStart, LineEnd, CurrentPoint, NextPoint))
		{
			if (Distance(CurrentPoint, LineStart, LineEnd) > Tolerance)
			{
				continue;
			}
			else
			{
				Coincidences.Add(GetProjectionPoint(CurrentPoint, LineStart, LineEnd));
				Coincidences.Add(GetProjectionPoint(NextPoint, LineStart, LineEnd));
			}
		}
	}

	if (Coincidences.Num() > 0)
	{
		return true;
	}

	return false;
}
bool FArmyMath::AreLinesParallel(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1)
{
	FVector Dir0 = GetLineDirection(S0, E0);
	FVector Dir1 = GetLineDirection(S1, E1);

	return AreLinesParallel(Dir0, Dir1);
}


bool FArmyMath::AreLinesParallel(const FVector& Line0, const FVector& Line1)
{
	float Result = FMath::Abs<float>(FVector::DotProduct(Line0, Line1));

	return FMath::IsNearlyEqual(Result, 1.f, 0.01f);
}

TArray<FVector> FArmyMath::Extrude3D(const TArray<FVector>& InPoints, float ExtrudeDistance, bool bOutSide /*= true*/)
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
struct TempEdge
{
	FVector StartPos;
	FVector EndPos;
	TempEdge(FVector start, FVector end)
	{
		StartPos = start;
		EndPos = end;
	}
	bool Intersect(const TempEdge& Other, FVector& IntersectPoint) const
	{
		FVector2D resut = FVector2D::ZeroVector;
		if (FArmyMath::Line2DIntersection(FVector2D(StartPos), FVector2D(EndPos), FVector2D(Other.StartPos), FVector2D(Other.EndPos), resut))
		{
			IntersectPoint = FVector(resut, 0.0);
			return true;
		}
		return false;
	}
};
TArray<FVector> FArmyMath::Extrude3dPolygon(const TArray<FVector>& InPoints, float ExtrudeDistance, const TArray<FVector>& BoundaryAra)
{


	if (InPoints.Num() < 2)
		return InPoints;
	float angle = 90;
	FVector direction = (InPoints[1] - InPoints[0]).GetSafeNormal();
	FVector extrudir = direction.RotateAngleAxis(angle, FVector(0, 0, 1));
	FVector pos0 = InPoints[0] + extrudir * ExtrudeDistance;
	//if (!IsPointInPolygon2D(pos0, BoundaryAra))
	//	angle = -angle;
	TArray<TempEdge> NewOutEdge;
	for (int i = 0; i < InPoints.Num() - 1; i++)
	{
		direction = (InPoints[i + 1] - InPoints[i]).GetSafeNormal();
		extrudir = direction.RotateAngleAxis(angle, FVector(0, 0, 1));
		FVector start = InPoints[i] + extrudir * ExtrudeDistance;
		FVector end = InPoints[i + 1] + extrudir * ExtrudeDistance;
		TempEdge temp(start, end);
		NewOutEdge.Push(temp);
	}
	TArray<FVector> result;
	if (NewOutEdge.Num() == 1)
		result = { NewOutEdge[0].StartPos,NewOutEdge[0].EndPos };
	else
	{
		int numberOfEdge = NewOutEdge.Num();
		result.Push(NewOutEdge[0].StartPos);
		for (int i = 0; i < numberOfEdge - 1; ++i)
		{
			const TempEdge& firstEdge = NewOutEdge[i];
			const TempEdge& seconEdge = NewOutEdge[i + 1];
			FVector interPos;
			if (firstEdge.Intersect(seconEdge, interPos))
				result.Push(interPos);
		}
		result.Push(NewOutEdge[numberOfEdge - 1].EndPos);
	}
	return result;


}


TArray<FVector> FArmyMath::ExturdePolygon(const TArray<FVector>& Original, float ExtrudeDistance, bool InSide /*= true*/)
{
	if (Original.Num() <= 2)
		return Original;
	float angle = 90;
	FVector direction = (Original[0] - Original[1]).GetSafeNormal();
	FVector extrudir = direction.RotateAngleAxis(angle, FVector(0, 0, 1));
	FVector pos0 = (Original[0] + Original[1]) / 2.0f + extrudir * ExtrudeDistance;
	if (!IsPointInPolygon2D(pos0, Original))
	{
		if (InSide)
			angle = -90;
		else
			angle = 90;
	}
	else
	{
		if (!InSide)
			angle = -90;
	}
	TArray<TempEdge> NewOutEdge;
	int num = Original.Num();
	for (int i = 0; i < num; i++)
	{
		direction = (Original[i%num] - Original[(i + 1) % num]).GetSafeNormal();
		extrudir = direction.RotateAngleAxis(angle, FVector(0, 0, 1));
		FVector start = Original[i%num] + extrudir * ExtrudeDistance;
		FVector end = Original[(i + 1) % num] + extrudir * ExtrudeDistance;
		TempEdge temp(start, end);
		NewOutEdge.Push(temp);
	}
	TArray<FVector> result;

	int numberOfEdge = NewOutEdge.Num();

	for (int i = 0; i < numberOfEdge; ++i)
	{
		const TempEdge& firstEdge = NewOutEdge[(i + numberOfEdge - 1) % numberOfEdge];
		const TempEdge& seconEdge = NewOutEdge[(i + numberOfEdge) % numberOfEdge];
		FVector interPos;
		if (firstEdge.Intersect(seconEdge, interPos))
			result.Push(interPos);
	}

	return result;
}
struct ArmyOutPt
{
	int Idx;

	int pointType = 0; // 0 表示点，1 表示门洞起点，2表示门洞终点
	FVector2D Pt;
	ArmyOutPt* Next;
	ArmyOutPt* Prev;
};
ArmyOutPt* ExcludeOp(ArmyOutPt* op)
{
	ArmyOutPt* result = op->Prev;
	result->Next = op->Next;
	op->Next->Prev = result;
	result->Idx = 0;
	return result;
}
bool SlopesNearCollinear(const FVector2D& point0, const FVector2D& point1, const FVector2D& point2, float Tolarence)
{
	float dist = FMath::PointDistToSegmentSquared(FVector(point1, 0.0f), FVector(point0, 0.0f), FVector(point2, 0.0f));
	if (dist <= Tolarence)
		return true;
	return false;
}
// @欧石楠 added 判断三点共线
bool Colinear(const FVector2D& point0, const FVector2D& point1, const FVector2D& point2, float Tolarence)
{
	FVector2D line1 = point1 - point0;
	FVector2D line2 = point2 - point0;
	line2.Normalize();
	// 计算投影长度
	float ProjDis = FVector2D::DotProduct(line1, line2);
	// 计算line1在line2上的投影点和point1的距离
	return (point0 + line2 * ProjDis - point1).Size() < Tolarence;
}
void FArmyMath::CleanPolygon(const TArray<FVector2D>& In_Poly, TArray<FVector2D>& Out_Poly, float Tolarence /*= 0.01f*/)
{
	int size = In_Poly.Num();
	if (size == 0)
	{
		Out_Poly.Empty();
		return;
	}
	ArmyOutPt* outPts = new ArmyOutPt[size];
	for (int i = 0; i < size; ++i)
	{
		outPts[i].Pt = In_Poly[i];
		outPts[i].Next = &outPts[(i + 1) % size];
		outPts[i].Next->Prev = &outPts[i];
		outPts[i].Idx = 0;
	}
	double distSqrd = Tolarence* Tolarence;
	ArmyOutPt* op = &outPts[0];

	while (op->Idx == 0 && op->Next != op->Prev)
	{
		if (op->Pt.Equals(op->Next->Pt, FMath::Abs(Tolarence)))
		{
			op = ExcludeOp(op);
			size--;
		}
		else if (op->Prev->Pt.Equals(op->Next->Pt, FMath::Abs(Tolarence)))
		{
			ExcludeOp(op->Next);
			op = ExcludeOp(op);
			size -= 2;
		}
		else if (SlopesNearCollinear(op->Prev->Pt, op->Pt, op->Next->Pt, distSqrd))
		{
			op = ExcludeOp(op);
			size--;
		}
		else if (Colinear(op->Prev->Pt, op->Pt, op->Next->Pt, Tolarence))
		{
			op = ExcludeOp(op);
			size--;
		}
		else
		{
			op->Idx = 1;
			op = op->Next;
		}
	}
	if (size < 3)size = 0;
	Out_Poly.AddUninitialized(size);
	for (int i = 0; i < size; ++i)
	{
		Out_Poly[i] = op->Pt;
		op = op->Next;
	}
	delete[] outPts;
}

void FArmyMath::CleanPolygon(TArray<FVector>& In_Poly, float Tolarence /*= 0.01f*/)
{
	TArray<FVector2D> tempInVetices, tempReulst;
	int number = In_Poly.Num();
	for (int i = 0; i < number; ++i)
	{
		tempInVetices.Push(FVector2D(In_Poly[i]));
	}
	CleanPolygon(tempInVetices, tempReulst, Tolarence);
	In_Poly.Empty();
	number = tempReulst.Num();
	for (int i = 0; i < number; ++i)
	{
		In_Poly.Push(FVector(tempReulst[i], 0.0f));
	}
}

struct HoleInfo
{
	HoleInfo(FVector2D InStart, FVector2D InEnd)
	{
		startHolePos = InStart;
		endHolePos = InEnd;
	}
	bool operator==(const HoleInfo& other)
	{
		if (other.startHolePos == startHolePos&&other.endHolePos == other.endHolePos)
			return true;
		else
			return false;
	}
	bool operator< (const HoleInfo& other) const
	{
		float dist1 = (endHolePos - orginalPos).Size();
		float dist2 = (other.startHolePos - orginalPos).Size();
		if (dist1 <= dist2)
			return true;
		else
			return false;
	}
	FVector2D startHolePos;
	FVector2D endHolePos;

	FVector2D orginalPos = FVector2D::ZeroVector;
};
struct RoomEdge
{
	FVector2D startPos;
	FVector2D endPos;
	TArray<HoleInfo> HoleInfos;
	bool IsHoleInEdge(HoleInfo& InInfo)
	{
		if (FArmyMath::AreLinesParallel(FVector(InInfo.startHolePos, 0.0f), FVector(InInfo.endHolePos, 0.0f), FVector(startPos, 0.0f), FVector(endPos, 0.0f)) &&
            FArmyMath::IsPointProjectionOnLineSegment2D(InInfo.startHolePos, startPos, endPos) &&
            FArmyMath::IsPointProjectionOnLineSegment2D(InInfo.endHolePos, startPos, endPos))
		{
			float dist = FArmyMath::CalcPointToLineDistance(FVector(InInfo.startHolePos, 0.0f), FVector(startPos, 0.0), FVector(endPos, 0.0f));
			if (dist < 3.0f)
			{
				FVector tempStart = FArmyMath::GetProjectionPoint(FVector(InInfo.startHolePos, 0.0f), FVector(startPos, 0.0f), FVector(endPos, 0.0f));
				FVector tempEnd = FArmyMath::GetProjectionPoint(FVector(InInfo.endHolePos, 0.0f), FVector(startPos, 0.0f), FVector(endPos, 0.0f));

				InInfo.startHolePos = ClampPoint(FVector2D(tempStart));
				InInfo.endHolePos = ClampPoint(FVector2D(tempEnd));
				bool IsStartOnEdge = FArmyMath::IsPointOnLine(FVector(InInfo.startHolePos, 0.0f), FVector(startPos, 0.0f), FVector(endPos, 0.0f), 0.2f);
				bool IsEndOnEdge = FArmyMath::IsPointOnLine(FVector(InInfo.endHolePos, 0.0f), FVector(startPos, 0.0f), FVector(endPos, 0.0f), 0.2f);
				if (IsStartOnEdge&&IsEndOnEdge)
				{
					FVector2D dir = (endPos - startPos).GetSafeNormal();
					FVector2D dir1 = (InInfo.endHolePos - InInfo.startHolePos);
					float result = FVector2D::DotProduct(dir1, dir);
					if (result < 0)
					{
						FVector2D tempStart = InInfo.startHolePos;
						InInfo.startHolePos = InInfo.endHolePos;
						InInfo.endHolePos = tempStart;
					}
					return true;
				}
			}
			else
			{
				return false;
			}

		}


		return false;
	}
	FVector2D ClampPoint(const FVector2D& InPos)
	{
		float edgeLength = (endPos - startPos).Size();
		float dist1 = (InPos - endPos).Size();
		float dist2 = (InPos - startPos).Size();
		if (dist1 > (edgeLength + 0.2f) || dist2 > (edgeLength + 0.2f))
		{
			if (dist1 <= dist2)
			{
				return endPos;
			}
			else if (dist2 <= dist1)
			{
				return startPos;
			}
		}
		return InPos;

	}
	void SortHoleWidthLine()
	{
		HoleInfos.Sort();
	}
};
void CalcuEdgeHole(RoomEdge& inEdge, const TArray<HoleInfo>& HoleInfos)
{
	FVector2D dir = (inEdge.endPos - inEdge.startPos).GetSafeNormal();
	int number = HoleInfos.Num();
	TArray<HoleInfo> onEdge;
	for (int i = 0; i < number; ++i)
	{
		HoleInfo tempHole = HoleInfos[i];
		if (inEdge.IsHoleInEdge(tempHole))
		{
			tempHole.orginalPos = inEdge.startPos;
			onEdge.Push(tempHole);
		}
	}
	inEdge.HoleInfos = onEdge;
	inEdge.SortHoleWidthLine();

}
bool  FArmyMath::UpdataBreakLineInfos(const TArray<FVector2D>& InOutLineVerts, TArray<TArray<FVector2D>>& OutResults, const TArray<FVector2D>& BreakLines)
{
	if (BreakLines.Num() == 0)
		return false;
	TArray<FVector2D>  clearPolygons;
	CleanPolygon(InOutLineVerts, clearPolygons, 0.1f);
	if (BreakLines.Num() < 2)
	{
		OutResults.Push(InOutLineVerts);
		return false;
	}

	TArray<HoleInfo> TotalHoles;
	int HoleVertsNum = BreakLines.Num();
	if (HoleVertsNum % 2 != 0)
		return false;
	for (int i = 0; i < HoleVertsNum; i += 2)
	{
		TotalHoles.Push(HoleInfo(BreakLines[i], BreakLines[i + 1]));
	}

	TArray<RoomEdge> TotalEdges;
	int orginalNumber = clearPolygons.Num();
	if (orginalNumber < 3)
		return false;
	for (int i = 0; i < orginalNumber; ++i)
	{
		FVector2D start = clearPolygons[i%orginalNumber];
		FVector2D end = clearPolygons[(i + 1) % orginalNumber];
		RoomEdge tempEdge;
		tempEdge.startPos = start;
		tempEdge.endPos = end;
		CalcuEdgeHole(tempEdge, TotalHoles);
		TotalEdges.Push(tempEdge);
	}
	int totalNumber = 0;
	struct PointInfo
	{
		PointInfo(FVector2D InPos, int InType)
		{
			Pos = InPos;
			pointType = InType;
		}
		int pointType = 0; // 0 表示点，1 表示门洞起点，2表示门洞终点
		FVector2D Pos;
	};
	TArray<PointInfo> TotalPointInfo;
	for (int i = 0; i < orginalNumber; ++i)
	{
		RoomEdge temEdge = TotalEdges[i];
		TotalPointInfo.Push(PointInfo(temEdge.startPos, 0.0f));
		totalNumber += 1;
		if (temEdge.HoleInfos.Num() > 0)
		{
			for (int j = 0; j < temEdge.HoleInfos.Num(); ++j)
			{
				TotalPointInfo.Push(PointInfo(temEdge.HoleInfos[j].startHolePos, 1));
				TotalPointInfo.Push(PointInfo(temEdge.HoleInfos[j].endHolePos, 2));
			}
			totalNumber += temEdge.HoleInfos.Num() * 2;
		}
	}
	if (totalNumber == TotalPointInfo.Num())
	{
		int firstIndex = 0;
		bool hasDoor = false;
		for (int i = 0; i < totalNumber; ++i)
		{
			if (TotalPointInfo[i].pointType == 2)
			{
				firstIndex = i;
				hasDoor = true;
				break;
			}
		}
		TArray<PointInfo> tempPointInfo;
		if (hasDoor)
		{
			for (int i = firstIndex; i < totalNumber; ++i)
			{
				tempPointInfo.Push(TotalPointInfo[i]);
			}
			for (int i = 0; i < firstIndex; ++i)
			{
				tempPointInfo.Push(TotalPointInfo[i]);
			}
			TArray<FVector2D> SkitLines;
			for (int i = 0; i < totalNumber; ++i)
			{
				if (tempPointInfo[i].pointType == 2)
				{
					SkitLines.Empty();
					SkitLines.Push(tempPointInfo[i].Pos);
				}
				else if (tempPointInfo[i].pointType == 0)
				{
					SkitLines.Push(tempPointInfo[i].Pos);
				}
				else if (tempPointInfo[i].pointType == 1)
				{
					SkitLines.Push(tempPointInfo[i].Pos);
					OutResults.Push(SkitLines);
					SkitLines.Empty();
				}

				//@欧石楠 临时
				/*if (tempPointInfo[i].pointType == 2)
				{
					SkitLines.Empty();
					SkitLines.Push(tempPointInfo[i].Pos);
				}
				else if (tempPointInfo[i].pointType == 0)
				{
					if (SkitLines.Num() == 0)
						SkitLines.Push(tempPointInfo[i].Pos);
					else if (SkitLines.Num() == 1)
					{
						SkitLines.Push(tempPointInfo[i].Pos);
						OutResults.Push(SkitLines);
						SkitLines.Empty();
						i--;
					}
				}
				else if (tempPointInfo[i].pointType == 1)
				{
					SkitLines.Push(tempPointInfo[i].Pos);
					OutResults.Push(SkitLines);
					SkitLines.Empty();
				}*/
			}

			return true;
		}
		else
		{
			OutResults.Emplace(InOutLineVerts);
			return false;
		}
	}

	return false;

}

void FArmyMath::RemoveComPointOnPolygon(TArray<FVector>& Orginal)
{
	TArray<FVector> result;
	if (Orginal.Num() > 1)
	{
		result = Orginal;

		int number = result.Num();
		for (int i = 0; i < number; i++)
		{
			FVector start = result[i%number];
			FVector end = result[(i + 1) % number];
			if (end.Equals(start, 0.01f))
				Orginal.RemoveAt((i + 1) % number);
		}
	}
}

void FArmyMath::ReversePointList(TArray<FVector>& PointList)
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


void FArmyMath::ReverPointList(TArray<FVector2D>& PointList)
{
	FVector2D Temp;
	int32 i, c;

	c = PointList.Num() / 2;
	for (i = 0; i < c; i++)
	{
		Temp = PointList[i];

		PointList[i] = PointList[(PointList.Num() - 1) - i];
		PointList[(PointList.Num() - 1) - i] = Temp;
	}
}

bool FArmyMath::TriangulatePoly(TArray<FClipSMTriangle>& OutTris, const FClipSMPolygon& InPoly, bool bKeepColinearVertices /*= false*/)
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

bool FArmyMath::TriangulatePoly(TArray<FVector>& OutTris, TArray<int32>& OutIndexList, const TArray<FVector>& InPointList, const FVector &FaceNormal, bool bKeepColinearVertices /*= false*/)
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
bool FArmyMath::ArcLineIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, TArray<FVector>& OutInter)
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
bool FArmyMath::ArcBoxIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FBox2D& Box, TArray<FVector>& InterPoints)
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
bool FArmyMath::ArcPointIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& Point, float tolerance, FVector& InterPoint)
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

bool FArmyMath::ArcLineIntersectionWithOutInifinet(const FVector& ArcPoint1, const FVector& ArcPoint2, const FVector& ArcPoint3, const FVector& LinePointStart, const FVector& LinePointEnd, FVector& OutInter)
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

bool FArmyMath::Circle(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, FVector& OutCenter, double &OutRadius)
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


bool FArmyMath::Circle3D(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, FVector& OutCenter, double &OutRadius)
{
	FVector Direction1 = AcrPoint1 - AcrPoint2;
	FVector Direction2 = AcrPoint1 - AcrPoint3;
	if (AreLinesParallel(Direction1, Direction2))
		return false;
	FPlane plane(AcrPoint1, AcrPoint2, AcrPoint3);
	FVector Normal = plane.GetSafeNormal();
	FVector planeXDir = Direction1.GetSafeNormal();
	FVector planeYDir = (planeXDir ^ Normal).GetSafeNormal();
	FVector BasePos = AcrPoint1;

	float x1 = FVector::DotProduct((AcrPoint1 - BasePos), planeXDir);
	float y1 = FVector::DotProduct((AcrPoint1 - BasePos), planeYDir);
	float z1 = 0;

	float x2 = FVector::DotProduct((AcrPoint2 - BasePos), planeXDir);
	float y2 = FVector::DotProduct((AcrPoint2 - BasePos), planeYDir);
	float z2 = 0;

	float x3 = FVector::DotProduct((AcrPoint3 - BasePos), planeXDir);
	float y3 = FVector::DotProduct((AcrPoint3 - BasePos), planeYDir);
	float z3 = 0;

	FVector P1(x1, y1, z1);
	FVector P2(x2, y2, z2);
	FVector P3(x3, y3, z3);

	FVector Center;
	if (Circle(P1, P2, P3, Center, OutRadius))
	{
		OutCenter = Center.X * planeXDir + Center.Y * planeYDir + BasePos;
		return true;
	}
	return false;
}


float FArmyMath::CircumferenceOfPointList(TArray<FVector> PointList, bool bClose)
{
	float Length = 0;
	int Num = PointList.Num();
	for (int i = 0; i < Num; i++)
	{
		PointList[i].Z = 0;
	}
	if (Num > 2)
	{
		for (int i = 0; i < Num - 1; i++)
		{
			Length += (PointList[i + 1] - PointList[i]).Size();
		}
		if (bClose)
		{
			Length += (PointList[0] - PointList[Num - 1]).Size();
		}
	}
	else if (Num == 2)
	{
		Length = (PointList[1] - PointList[0]).Size();
	}
	return Length;
}


bool FArmyMath::CalcLines2DCollineation(const FVector& S1, const FVector& E1, const FVector& S2, const FVector& E2)
{
	if (FArmyMath::AreLinesParallel(S1, E1, S2, E2) && S1.Z == E1.Z && S1.Z == S2.Z && S1.Z == E2.Z)
	{
		float CenterDis = FVector::Dist2D((S1 + E1) / 2, (S2 + E2) / 2);
		float Line1Length = FVector::Dist2D(S1, E1);
		float Line2Length = FVector::Dist2D(S2, E2);
		FVector normal = (S1 - S2).GetSafeNormal();
		FVector NewS1 = S1 + normal * (CenterDis + Line2Length + Line1Length);
		FVector NewE1 = E1 - normal * (CenterDis + Line2Length + Line1Length);
		float Dis1 = FArmyMath::CalcPointToLineDistance(S2, NewS1, NewE1);
		float Dis2 = FArmyMath::CalcPointToLineDistance(E2, NewS1, NewE1);
		if (FMath::Abs(Dis1) < 0.001 && FMath::Abs(Dis2) < 0.001)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool FArmyMath::IsLinesCollineationAndIntersection(const FVector& S1, const FVector& E1, const FVector& S2, const FVector& E2, bool bIncludeEndpoint /*= true*/)
{
	if (IsPointOnLine(S1, S2, E2) || IsPointOnLine(E1, S2, E2)
		|| IsPointOnLine(S2, S1, E1) || IsPointOnLine(E2, S1, E1))
	{
		//排除共端点的情况
		/*if (!bIncludeEndpoint)
		{
			if ((S1.Equals(S2, 0.001) && !IsPointOnLine(E1, S2, E2))
				|| (E1.Equals(S2, 0.001) && !IsPointOnLine(S1, S2, E2))
				|| (S1.Equals(E2, 0.001) && !IsPointOnLine(E1, S2, E2))
				|| (E1.Equals(E2, 0.001) && !IsPointOnLine(S1, S2, E2)))
				return false;
		}*/

		return true;
	}
	else
		return false;
}

void FArmyMath::CreateArc(const FVector& center, FVector& InStart, FVector& InEnd, float r, TArray<FVector>& Verties, int32 Count)
{
	// 圆心位置
	FVector Center = center;
	FVector Direction1 = (InStart - Center).GetSafeNormal();
	FVector Direction2 = (InEnd - Center).GetSafeNormal();
	float Radius = (InStart - Center).Size();
	// 起始方向和终止方向的法线向量
	FVector Normal = FVector::CrossProduct(Direction1, Direction2).GetSafeNormal();
	//两个向量的弧度
	float CosQ = FVector::DotProduct(Direction1, Direction2);
	//起始向量
	FVector BeginVector = InStart - Center;

	//终止向量
	FVector EndDirt = InEnd - Center;
	// 圆心的需要旋转的弧度
	float Angle = FMath::Acos(CosQ) * 180 / PI;
	float theta = Angle / Count;
	for (int32 i = 0; i < Count; i++)
	{

		FVector StartDirt = BeginVector.RotateAngleAxis(theta * i, Normal).GetSafeNormal();
		FVector EndDirt = BeginVector.RotateAngleAxis(theta * (i + 1), Normal).GetSafeNormal();

		FVector Start = Center + Radius*StartDirt;
		FVector End = Center + Radius*EndDirt;
		FVector DirectVector = End - Start;
		float halfLength = DirectVector.Size() / 2;

		FVector SDirectNormal = StartDirt.RotateAngleAxis(90, Normal); // 起始单位法线
		FVector EDirectNormal = EndDirt.RotateAngleAxis(90, Normal); // 终止单位法线

		float AngleCycle = 360;
		int32 CountCycle = HyPipeVerticeCount; // 圆弧分成2


		float theta = AngleCycle / CountCycle;
		for (int32 j = 0; j < CountCycle + 1; j++)
		{
			FVector StartDirtCycle0 = StartDirt.RotateAngleAxis(theta * j, SDirectNormal).GetSafeNormal();
			FVector StartDirtCycle1 = StartDirt.RotateAngleAxis(theta * (j + 1), SDirectNormal).GetSafeNormal();
			FVector EndDirtCycle0 = EndDirt.RotateAngleAxis(theta * j, EDirectNormal).GetSafeNormal();
			FVector EndDirtCycle1 = EndDirt.RotateAngleAxis(theta * (j + 1), EDirectNormal).GetSafeNormal();

			FVector SP0 = Start + StartDirtCycle0* r;
			FVector SP1 = Start + StartDirtCycle1* r;
			FVector EP0 = End + EndDirtCycle0*r;
			FVector EP1 = End + EndDirtCycle1*r;
			if (SP0.ContainsNaN() || SP1.ContainsNaN() || EP0.ContainsNaN() || EP1.ContainsNaN())
			{
				continue;
			}
			Verties.Push(SP0);
			Verties.Push(EP0);
			Verties.Push(EP1);

			Verties.Push(SP0);
			Verties.Push(EP1);
			Verties.Push(SP1);
		}
		//PipeLineActor->AddVetries (Verties);
	}
}

void FArmyMath::DrawCylinder(FVector InStart, FVector InEnd, float r, TArray<FVector>& Verties)
{
	FVector DirectVector = InEnd - InStart;
	float halfLength = DirectVector.Size() / 2;
	FVector DirectNormal = DirectVector.GetSafeNormal(); // 单位法线
	FVector U(DirectNormal.Y, -DirectNormal.X, 0); // 圆所在平面上的向量
	if (FMath::IsNearlyZero(FVector::DistSquaredXY(U, FVector::ZeroVector))) //如果y*y + x*x ==0换个向量
		U = FVector(DirectNormal.Z, 0, 0);
	FVector UN = U.GetSafeNormal();
	float Angle = 360;
	int32 Count = HyPipeVerticeCount; // 圆弧分成2

	float theta = Angle / Count;
	FVector Location = (InStart + InEnd) / 2;
	TArray<FVector> CycleVertexs;
	for (int32 i = 0; i < Count; i++)
	{
		FVector StartDirt = UN.RotateAngleAxis(theta * i, DirectNormal).GetSafeNormal();
		FVector Start1 = Location + StartDirt* r;
		FVector EndDirt = UN.RotateAngleAxis(theta * (i + 1), DirectNormal).GetSafeNormal();
		FVector End1 = Location + EndDirt* r;



		FVector SP0 = Start1 - halfLength*DirectNormal;
		FVector EP0 = End1 - halfLength*DirectNormal;
		FVector SP1 = Start1 + halfLength*DirectNormal;
		FVector EP1 = End1 + halfLength*DirectNormal;
		FVector CP0 = Location - halfLength * DirectNormal;
		FVector CP1 = Location + halfLength * DirectNormal;
		if (SP0.ContainsNaN() || SP1.ContainsNaN() || EP0.ContainsNaN() || EP1.ContainsNaN() || CP0.ContainsNaN() || CP1.ContainsNaN())
		{
			continue;
		}
		Verties.Push(SP0);
		Verties.Push(SP1);
		Verties.Push(EP1);

		Verties.Push(SP0);
		Verties.Push(EP1);
		Verties.Push(EP0);

		Verties.Push(CP0);
		Verties.Push(SP0);
		Verties.Push(EP0);

		Verties.Push(CP1);
		Verties.Push(SP1);
		Verties.Push(EP1);

	}
}


void FArmyMath::DrawCylinder(FVector InStart, FVector InEnd, FVector CylinderDirection, float r, TArray<FVector>& Verties)
{
	FVector DirectVector = CylinderDirection.GetSafeNormal();
	float halfLength = DirectVector.Size() / 2;
	FVector DirectNormal = DirectVector.GetSafeNormal(); // 单位法线
	FVector U(DirectNormal.Y, -DirectNormal.X, 0); // 圆所在平面上的向量
	if (FMath::IsNearlyZero(FVector::DistSquaredXY(U, FVector::ZeroVector))) //如果y*y + x*x ==0换个向量
		U = FVector(DirectNormal.Z, 0, 0);
	FVector UN = U.GetSafeNormal();
	float Angle = 360;
	int32 Count = HyPipeVerticeCount; // 圆弧分成2

	float theta = Angle / Count;
	TArray<FVector> CycleVertexs;
	for (int32 i = 0; i < Count; i++)
	{
		FVector StartDirt = UN.RotateAngleAxis(theta * i, DirectNormal).GetSafeNormal();
		FVector Start1 = InStart + StartDirt* r;
		FVector Start2 = InEnd + StartDirt*r;
		FVector EndDirt = UN.RotateAngleAxis(theta * (i + 1), DirectNormal).GetSafeNormal();
		FVector End1 = InStart + EndDirt* r;
		FVector End2 = InEnd + EndDirt* r;




		if (Start1.ContainsNaN() || Start2.ContainsNaN() || End1.ContainsNaN() || End2.ContainsNaN())
		{
			continue;
		}
		Verties.Push(Start1);
		Verties.Push(Start2);
		Verties.Push(End1);

		Verties.Push(End1);
		Verties.Push(Start2);
		Verties.Push(End2);
	}
}

void FArmyMath::DrawWire(FVector& Location1, float Radius, TArray<FVector>& Verties, int32 Cout)
{
	int32 uStepsNum = Cout, vStepNum = Cout;
	float ustep = 1 / (float)uStepsNum, vstep = 1 / (float)vStepNum;
	float u = 0, v = 0;
	//绘制下端三角形组
	for (int32 i = 0; i < uStepsNum; i++)
	{
		FVector a = Location1 + GetPoint(0, 0) * Radius;
		FVector b = Location1 + GetPoint(u, vstep) * Radius;
		FVector c = Location1 + GetPoint(u + ustep, vstep) * Radius;
		u += ustep;
		Verties.Add(a);
		Verties.Add(b);
		Verties.Add(c);
	}
	//绘制中间四边形组
	u = 0, v = vstep;
	for (int32 i = 1; i < vStepNum - 1; i++)
	{
		for (int32 j = 0; j < uStepsNum; j++)
		{
			FVector a = Location1 + GetPoint(u, v)* Radius;
			FVector b = Location1 + GetPoint(u + ustep, v)* Radius;
			FVector c = Location1 + GetPoint(u + ustep, v + vstep)* Radius;
			FVector d = Location1 + GetPoint(u, v + vstep)* Radius;
			u += ustep;

			Verties.Add(a);
			Verties.Add(b);
			Verties.Add(c);

			Verties.Add(a);
			Verties.Add(c);
			Verties.Add(d);
		}
		v += vstep;
	}
	//绘制下端三角形组
	u = 0;
	for (int32 i = 0; i < uStepsNum; i++)
	{
		FVector a = Location1 + GetPoint(0, 1)* Radius;
		FVector b = Location1 + GetPoint(u, 1 - vstep)* Radius;
		FVector c = Location1 + GetPoint(u + ustep, 1 - vstep)* Radius;
		Verties.Add(a);
		Verties.Add(b);
		Verties.Add(c);
	}
}

FVector FArmyMath::GetPoint(float u, float v)
{
	float sin = FMath::Sin(PI *v);
	float cos = FMath::Cos(PI* v);
	float sin2 = FMath::Sin(PI * 2 * u);
	float cos2 = FMath::Cos(PI * 2 * u);
	float x = sin*cos2;
	float y = sin*sin2;
	float z = cos;
	return FVector(x, y, z).GetSafeNormal();
}

int dcmp(double x) {
	if (FMath::Abs(x) < 0.00001) return 0;
	return x < 0 ? -1 : 1;
}
int FArmyMath::ArcAndArcIntersection(const FVector& AcrPoint1, const FVector& AcrPoint2, const FVector& AcrPoint3, const FVector& AcrPoint4, const FVector& AcrPoint5, const FVector& AcrPoint6, TArray<FVector>& OutPoint)
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

float FArmyMath::DistanceLineToLine(const FVector& p1, const FVector& p2, const FVector& p3, const FVector& p4, FVector& IntersectPoint)
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

void FArmyMath::FootLineToLine(const FVector& p1, const FVector& p2, const FVector& p3, const FVector& p4, FVector& OutPoint0, FVector& OutPoint1)
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

	float a = (p2 - p1).SizeSquared();//(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1);
	float b = -(p2 - p1) | (p4 - p3); //-((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
	float c = -(p1 - p2) | (p1 - p3);// -((x1-x2)*(x1-x3)+(y1-y2)*(y1-y3)+(z1-z2)*(z1-z3));

	float d = -(p2 - p1) | (p4 - p3);//-((x2-x1)*(x4-x3)+(y2-y1)*(y4-y3)+(z2-z1)*(z4-z3));
	float e = (p4 - p3).SizeSquared();//(x4-x3)*(x4-x3)+(y4-y3)*(y4-y3)+(z4-z3)*(z4-z3);
	float f = -(p1 - p3) | (p4 - p3);// -((x1-x3)*(x4-x3)+(y1-y3)*(y4-y3)+(z1-z3)*(z4-z3));

	if ((a*e - b*d) == 0 && (b*d - a*e) == 0) //平行
	{
		OutPoint0 = OutPoint1 = FVector::ZeroVector;
		return;
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
		OutPoint0 = P;
		OutPoint1 = Q;
	}
	else
	{
		OutPoint1 = OutPoint0 = FVector::ZeroVector;
	}
}