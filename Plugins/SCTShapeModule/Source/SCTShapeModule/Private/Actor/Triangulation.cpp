#include "Triangulation.h"
#include "GeomTools.h"

namespace SCT
{
	/** Compare all aspects of two verts of two triangles to see if they are the same. */
	bool VertsAreEqual(const FClipSMVertex& A, const FClipSMVertex& B)
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

	FClipSMVertex InterpolateVert(const FClipSMVertex& V0, const FClipSMVertex& V1, float Alpha)
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

	bool AreEdgesMergeable(
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

	bool VectorsOnSameSide(const FVector& Vec, const FVector& A, const FVector& B)
	{
		const FVector CrossA = Vec ^ A;
		const FVector CrossB = Vec ^ B;
		return !FMath::IsNegativeFloat(CrossA | CrossB);
	}

	bool PointInTriangle(const FVector& A, const FVector& B, const FVector& C, const FVector& P)
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

	bool Earcut(TArray<FClipSMTriangle>& OutTris, const FClipSMPolygon& InPoly, bool bKeepColinearVertices)
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

	bool TriangulatePoly(TArray<uint16>& OutIndexList, const TArray<FVector>& InPointList, const FVector& InFaceNormal, bool bKeepColinearVertices /*= false*/)
	{
		OutIndexList.Empty();

		TArray<FClipSMTriangle> InOutTris;
		FClipSMPolygon CurrentPoly(0);
		CurrentPoly.FaceNormal = InFaceNormal;
		for (auto& It : InPointList)
		{
			FClipSMVertex CurrentVertex;
			CurrentVertex.Pos = It;
			CurrentPoly.Vertices.Emplace(CurrentVertex);
		}
		if (Earcut(InOutTris, CurrentPoly, bKeepColinearVertices))
		{
			//for (auto& It : InOutTris)
			//{
			//	OutTris.Emplace(It.Vertices[0].Pos);
			//	OutTris.Emplace(It.Vertices[1].Pos);
			//	OutTris.Emplace(It.Vertices[2].Pos);
			//}

			for (auto& It : InOutTris)
			{
				for (int32 i = 0; i < 3; ++i)
				{
					for (int32 j = 0; j < InPointList.Num(); ++j)
					{
						if (It.Vertices[i].Pos.Equals(InPointList[j], KINDA_SMALL_NUMBER))
						{
							OutIndexList.Add(j);
							break;
						}
					}
				}
			}
			return true;
		}
		else
			return false;
	}

}