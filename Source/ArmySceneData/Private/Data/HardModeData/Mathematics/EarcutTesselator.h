#pragma  once
#include "earcut.hpp"
#include <array>
#include <memory>
#include <vector>
#include "ArmyTools/Public/XRClipper.h"
namespace mapbox
{
	template<typename Coord, typename Polygon>
	class FArmyEarcutTesselator
	{
	public:
		using Vertex = std::array<Coord, 2>;
		using Vertices = std::vector<Vertex>;
		FArmyEarcutTesselator(const Polygon& polygon_) :polygon(polygon_)
		{
			for (const auto& ring : polygon_)
			{
				for (auto& vertex : ring)
				{
					vertices_.emplace_back(Vertex{ { Coord(std::get<0>(vertex)),
						Coord(std::get<1>(vertex)) } });
				}
			}
		}
		FArmyEarcutTesselator& operator=(const FArmyEarcutTesselator&) = delete;
		void Run()
		{
			indices_ = mapbox::earcut(polygon);
		}
		std::vector<uint32_t> const & Indices()const
		{
			return indices_;
		}
		Vertices const& GetVertices()
		{
			return vertices_;
		}

	private:
		const Polygon& polygon;
		Vertices vertices_;
		std::vector<uint32_t> indices_;
	};

}
namespace Translate
{
	template<typename T> using Polygon = std::vector<std::vector<T>>;
	static void Triangle(const TArray<FVector2D>& SidePoints, const FVector& rightDir, const FVector originalPos, TArray<int32>& outIndex, TArray<FVector>& outResults)
	{
		Translate::Polygon<std::pair<float, float>> tempVertices;
		std::vector<std::pair<float, float>> OutConvertexs;

		for (FVector2D iter : SidePoints)
		{
			std::pair<float, float> converVert{ iter.X,iter.Y };
			OutConvertexs.push_back(converVert);
		}
		tempVertices.push_back(OutConvertexs);
		mapbox::FArmyEarcutTesselator<float, decltype(tempVertices)> tesslator(tempVertices);
		tesslator.Run();
		std::vector<uint32_t> indexs = tesslator.Indices();
		for (uint32_t index : indexs)
		{
			FVector tempVert = SidePoints[index].X * rightDir + originalPos;
			tempVert.Z += SidePoints[index].Y;
			outResults.Push(tempVert);
			outIndex.Push(index);
		}
	}
	static FVector CaculatPlaneCenter(const TArray<FVector>& InVers)
	{

		int number = InVers.Num();
		FVector totalSum = FVector::ZeroVector;
		for (int i = 0; i < number; i++)
		{
			totalSum += FVector(InVers[i].X, InVers[i].Y, 0.0f);
		}
		return totalSum / number;
	}
	static bool IsLineOnPolygonArea(const FVector& LineStart, const FVector& LineEnd, const TArray<FVector>& PolygonAreas)
	{
		int number = PolygonAreas.Num();
		for (int i = 0; i < number; i++)
		{
			const FVector& tempStart = PolygonAreas[i%number];
			const FVector& tempEnd = PolygonAreas[(i + 1) % number];
			if (FArmyMath::IsPointInLineSegment3D(LineStart, tempStart, tempEnd, 0.01f) && FArmyMath::IsPointInLineSegment3D(LineEnd, tempStart, tempEnd, 0.01f))
			{
				return true;
			}
		}
		return false;

	}
	static double CalcPolyArea(const Path& InArea)
	{
		double iArea = 0.0f;
		int32 count = InArea.size();
		for (int i = 0; i < count; ++i)
		{
			IntPoint vec0 = InArea[i];
			IntPoint vec1 = InArea[(i + 1) % count];

			iArea = iArea + vec0.X * vec1.Y - vec1.X * vec0.Y;
		}
		iArea = FMath::Abs(0.5f * iArea);
		return iArea;
	}
	static bool ArePolysOverlap(const TArray<FVector>& outPoly, const TArray<FVector>& InsidePoly, TArray<FVector>& result)
	{
		const float SCALE0 = 1000.0f;
		Clipper c;
		Paths ClipperAreas;

		Paths ClipperSolution;

		ClipperAreas.resize(1);
		for (int i = 0; i < outPoly.Num(); i++)
		{
			cInt x = (cInt)(outPoly[i].X * SCALE0);
			cInt y = (cInt)(outPoly[i].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}

		Path holePath;
		int number = InsidePoly.Num();
		for (int j = 0; j < number; j++)
		{
			cInt x = (cInt)(InsidePoly[j].X * SCALE0);
			cInt y = (cInt)(InsidePoly[j].Y * SCALE0);
			holePath.push_back(IntPoint(x, y));
		}


		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPath(holePath, ptClip, true);
		c.Execute(ctIntersection, ClipperSolution, pftEvenOdd, pftEvenOdd);
		int newNumber = ClipperSolution.size();
		if (newNumber >= 1)
		{
			Path& tempPath = ClipperSolution[0];
			number = tempPath.size();
			for (int i = 0; i < number; i++)
			{
				result.Emplace(FVector(tempPath[i].X / SCALE0, tempPath[i].Y / SCALE0, 0.0f));
			}
			return true;
		}
		return false;
	}
	static bool ArePolygonInPolygon(const TArray<FVector>& outPoly, const TArray<FVector>& InsidePoly)
	{
		const float SCALE0 = 10.0f;
		Clipper c;
		Paths ClipperAreas;

		Paths ClipperSolution;

		ClipperAreas.resize(1);
		for (int i = 0; i < outPoly.Num(); i++)
		{
			cInt x = (cInt)(outPoly[i].X * SCALE0);
			cInt y = (cInt)(outPoly[i].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}

		Path holePath;
		int number = InsidePoly.Num();
		for (int j = 0; j < number; j++)
		{
			cInt x = (cInt)(InsidePoly[j].X * SCALE0);
			cInt y = (cInt)(InsidePoly[j].Y * SCALE0);
			holePath.push_back(IntPoint(x, y));
		}


		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPath(holePath, ptClip, true);
		c.Execute(ctIntersection, ClipperSolution, pftEvenOdd, pftEvenOdd);
		int newNumber = ClipperSolution.size();
		if (newNumber == 1)
		{

			double area0 = CalcPolyArea(ClipperSolution[0]);
			double area1 = CalcPolyArea(holePath);
			if (FMath::IsNearlyEqual(area0, area1, 0.01))
			{
				return true;
			}
			else
				return false;
		}
		else
		{
			return false;
		}
	}
	static bool IsLineOnPolygon(const FVector2D& Inpos, const FVector2D& InEnd, const TArray<FVector2D>& outArea)
	{
		int number = outArea.Num();
		for (int i = 0; i < number; i++)
		{
			const	FVector2D& p1 = outArea[i%number];
			const  	FVector2D& p2 = outArea[(i + 1) % number];
			if (FArmyMath::IsPointOnLineSegment2D(Inpos, p1, p2, 0.01f) && FArmyMath::IsPointOnLineSegment2D(InEnd, p1, p2, 0.01f))
				return true;

		}
		return false;
	}
	static bool  IntersectSegmentWithConvexPolygon(const FVector2D& startPos, const FVector2D& EndPos, const TArray<FVector2D>& ConverxPolygon, TArray<FVector>& intersectionPos)
	{


		TArray<FVector> outArea;
		for (const FVector2D& iter : ConverxPolygon)
		{
			outArea.Push(FVector(iter, 0.0f));
		}
		if (IsLineOnPolygon(startPos, EndPos, ConverxPolygon))
			return false;
		if (FArmyMath::IsPointInOrOnPolygon2D(FVector(startPos, 0.0f), outArea) && FArmyMath::IsPointInOrOnPolygon2D(FVector(EndPos, 0.0f), outArea))
		{
			intersectionPos.Push(FVector(startPos, 0.0));
			intersectionPos.Push(FVector(EndPos, 0.0f));
			return true;
		}

		if (outArea.Num() < 4)
			return false;
		float angle = 90;
		FVector dir = (outArea[1] - outArea[0]).GetSafeNormal();
		FVector outDir = dir.RotateAngleAxis(angle, FVector(0, 0, 1));
		FVector centerPos = (outArea[1] + outArea[0]) / 2.0f;
		centerPos = centerPos + outDir * 3.0f;
		if (FArmyMath::IsPointInOrOnPolygon2D(centerPos, outArea))
			angle = -90;
		float tE = 0;
		float tL = 1;
		float t, N, D;
		FVector2D direction = (EndPos - startPos);
		FVector2D e;
		int number = ConverxPolygon.Num();

		for (int i = 0; i < number; ++i)
		{
			FVector2D tempStart = ConverxPolygon[i%number];
			FVector2D tempEnd = ConverxPolygon[(i + 1) % number];
			e = (tempEnd - tempStart).GetSafeNormal();
			FVector normal = FVector(e, 0.0f).RotateAngleAxis(angle, FVector(0, 0, 1));
			D = FVector::DotProduct(normal, FVector(direction, 0.0));
			N = FVector::DotProduct(FVector((tempStart - startPos), 0.0), normal);
			if (FMath::Abs(D) < 0.009f)
			{
				continue;
			}

			t = N / D;
			FVector interPos = FVector(startPos + t * direction, 0.0f);
			FVector tempDir0 = (FVector(startPos, 0.0f) - interPos).GetSafeNormal();
			FVector tempDir1 = (FVector(EndPos, 0.0f) - interPos).GetSafeNormal();
			FVector tempDir2 = (FVector(tempStart, 0.0f) - interPos).GetSafeNormal();
			FVector tempDir3 = (FVector(tempEnd, 0.0f) - interPos).GetSafeNormal();
			float result = FVector::DotProduct(tempDir0, tempDir1);
			float result1 = FVector::DotProduct(tempDir2, tempDir3);
			if (interPos.Equals(FVector(startPos, 0.0), 0.1f) || interPos.Equals(FVector(EndPos, 0.0f), 0.1f) && result1 <= 0.0f)
			{
				intersectionPos.Push(interPos);
			}
			else if (result < 0.0f && result1 < 0.0f)
			{
				intersectionPos.Push(interPos);
			}


		}

		if (intersectionPos.Num() > 0)
		{
			if (intersectionPos.Num() == 1)
			{
				if (FArmyMath::IsPointInOrOnPolygon2D(FVector(startPos, 0.0f), outArea))
				{
					intersectionPos.Push(FVector(startPos, 0.0f));
				}
				else if (FArmyMath::IsPointInOrOnPolygon2D(FVector(EndPos, 0.0f), outArea))
				{
					intersectionPos.Push(FVector(EndPos, 0.0f));
				}
			}
			if (intersectionPos.Num() == 2)
			{
				int TempNumber = ConverxPolygon.Num();
				for (int i = 0; i < TempNumber; i++)
				{
					FVector tempStart = FVector(ConverxPolygon[i%TempNumber], 0.0f);
					FVector tempEnd = FVector(ConverxPolygon[(i + 1) % TempNumber], 0.0f);
					if (FArmyMath::IsPointOnLine(intersectionPos[0], tempStart, tempEnd, 0.01f) && FArmyMath::IsPointOnLine(intersectionPos[1], tempStart, tempEnd, 0.01f))
					{
						return false;
					}
				}
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	static bool CaculatePackPipeOutLineVerts(const TArray<FVector>& InRoomVerts, const TArray<FVector>& pipeVerts, TArray<FVector>& results)
	{

		const float SCALE0 = 1000.0f;
		Clipper c;
		Paths ClipperAreas;
		Paths HolePaths;

		ClipperAreas.resize(1);
		for (int iter = 0; iter < InRoomVerts.Num(); iter++)
		{
			cInt x = (cInt)(InRoomVerts[iter].X * SCALE0);
			cInt y = (cInt)(InRoomVerts[iter].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}

		Path holePath;
		for (int j = 0; j < pipeVerts.Num(); j++)
		{
			cInt x = (cInt)(pipeVerts[j].X * SCALE0);
			cInt y = (cInt)(pipeVerts[j].Y * SCALE0);
			holePath.push_back(IntPoint(x, y));
		}
		HolePaths.push_back(holePath);

		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPaths(HolePaths, ptClip, true);
		Paths ClipperSolution;
		c.Execute(ctDifference, ClipperSolution, pftNonZero, pftNonZero);
		if (ClipperSolution.size() == 2)
		{
			Path path0 = ClipperSolution[0];
			Path path1 = ClipperSolution[1];
			int number0 = path0.size();
			int number1 = path1.size();
			TArray<FVector> tempArry0, tempArray1;
			tempArry0.AddUninitialized(number0);
			tempArray1.AddUninitialized(number1);
			for (int i = 0; i < number0; ++i)
			{
				tempArry0[i] = FVector(path0[i].X / SCALE0, path0[i].Y / SCALE0, 0.0f);
			}
			for (int i = 0; i < number1; ++i)
			{
				tempArray1[i] = FVector(path1[i].X / SCALE0, path1[i].Y / SCALE0, 0.0f);
			}
			Path minPath;
			if (FArmyMath::CalcPolyArea(tempArry0) < FArmyMath::CalcPolyArea(tempArray1))
			{
				minPath = path0;
			}
			else
				minPath = path1;

			c.Clear();

			c.AddPath(holePath, ptSubject, true);
			c.AddPath(minPath, ptClip, true);
			ClipperSolution.clear();
			c.Execute(ClipType::ctUnion, ClipperSolution, pftNonZero, pftNonZero);
			if (ClipperSolution.size() == 1)
			{
				int TotalNum = ClipperSolution[0].size();
				for (int i = 0; i < TotalNum; i++)
				{
					results.Push(FVector(ClipperSolution[0][i].X / SCALE0, ClipperSolution[0][i].Y / SCALE0, 0.0f));
				}
				return true;
			}
			return false;
		}
		return false;
	}
	static void ClippPolygonHoleArea(const TArray<FVector>& InPolygonArea, const TArray<TArray<FVector>>& InHoles, TArray < TArray<FVector>>& outResults)
	{
		const float SCALE0 = 100.0f;
		Clipper c;
		Paths ClipperAreas;
		Paths HolePaths;
		Paths ClipperSolution;
		int holeNumbers = InHoles.Num();
		ClipperAreas.resize(1);
		for (int i = 0; i < InPolygonArea.Num(); i++)
		{
			cInt x = (cInt)(InPolygonArea[i].X * SCALE0);
			cInt y = (cInt)(InPolygonArea[i].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}
		for (int i = 0; i < holeNumbers; i++)
		{
			Path holePath;
			for (int j = 0; j < InHoles[i].Num(); j++)
			{
				cInt x = (cInt)(InHoles[i][j].X * SCALE0);
				cInt y = (cInt)(InHoles[i][j].Y * SCALE0);
				holePath.push_back(IntPoint(x, y));
			}
			HolePaths.push_back(holePath);
		}
		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPaths(HolePaths, ptClip, true);
		c.Execute(ctIntersection, ClipperSolution, pftEvenOdd, pftEvenOdd);
		int newNumber = ClipperSolution.size();
		for (int i = 0; i < newNumber; ++i)
		{
			TArray<FVector> newVertices;
			Path holePath = ClipperSolution[i];
			int pathNum = holePath.size();
			for (int j = 0; j < pathNum; ++j)
			{
				FVector newPos = FVector(holePath[j].X / SCALE0, holePath[j].Y / SCALE0, 0.0f);
				newVertices.Push(newPos);
			}
			outResults.Push(newVertices);
		}
	}

	static void PolygonIntersect(const TArray<FVector>& InPolygonOne, const TArray<TArray<FVector>>& PolygonWithHoles, TArray <TArray<FVector>>& result)
	{
		if (PolygonWithHoles.Num() == 0)
			return;

		const float scale = 100000.0f;
		Clipper c;
		Paths ClipperAreas;
		Paths HolePaths;
		TArray<TArray<FVector>> Holes;
		int totalNumber = PolygonWithHoles.Num();
		if (totalNumber > 1)
		{
			for (int i = 1; i < totalNumber; ++i)
			{
				Holes.Push(PolygonWithHoles[i]);
			}
		}
		int holeNumbers = Holes.Num();
		ClipperAreas.resize(1);
		for (int i = 0; i < PolygonWithHoles[0].Num(); i++)
		{
			cInt x = (cInt)(PolygonWithHoles[0][i].X * scale);
			cInt y = (cInt)(PolygonWithHoles[0][i].Y * scale);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}
		for (int i = 0; i < holeNumbers; i++)
		{
			Path holePath;
			for (int j = 0; j < Holes[i].Num(); j++)
			{
				cInt x = (cInt)(Holes[i][j].X * scale);
				cInt y = (cInt)(Holes[i][j].Y * scale);
				holePath.push_back(IntPoint(x, y));
			}
			HolePaths.push_back(holePath);
		}
		c.AddPaths(ClipperAreas, ptSubject, true);

		Paths sub;
		sub.resize(1);
		for (int j = 0; j < InPolygonOne.Num(); j++)
		{
			cInt x = (cInt)(InPolygonOne[j].X * scale);
			cInt y = (cInt)(InPolygonOne[j].Y * scale);
			sub[0].push_back(IntPoint(x, y));
		}
		c.AddPaths(sub, ptClip, true);
		Paths ClipperSolution, tempResults;
		c.Execute(ctIntersection, tempResults, pftEvenOdd, pftEvenOdd);
		if (HolePaths.size() > 0)
		{
			c.Clear();
			c.AddPaths(tempResults, ptSubject, true);
			c.AddPaths(HolePaths, ptClip, true);
			c.Execute(ctDifference, ClipperSolution, pftEvenOdd, pftEvenOdd);
		}
		else
		{
			ClipperSolution = tempResults;
		}

		for (int i = 0; i < ClipperSolution.size(); ++i)
		{
			const Path& tempPath = ClipperSolution[i];
			TArray<FVector> vertices;
			for (int j = 0; j < tempPath.size(); ++j)
			{

				const IntPoint& point = tempPath[j];
				float x = point.X / scale;
				float y = point.Y / scale;

				vertices.Push(FVector(x, y, 0.0f));
			}
			result.Push(vertices);
		}
	}
	static void TriangleAreaWithHole(const TArray<FVector>& outVertices, const TArray<TArray<FVector>>& innearHoles, TArray<FVector2D>& OutResults)
	{
		Translate::Polygon<std::pair<float, float>> tempVertices;
		std::vector<std::pair<float, float>> OutConvertexs;
		for (FVector iter : outVertices)
		{
			std::pair<float, float> converVert{ iter.X,iter.Y };
			OutConvertexs.push_back(converVert);
		}
		TArray<FVector> totalVetices;
		totalVetices.Append(outVertices);
		tempVertices.push_back(OutConvertexs);
		for (TArray<FVector> iter : innearHoles)
		{
			std::vector<std::pair<float, float>> innerHole;
			for (FVector& iter0 : iter)
			{
				std::pair<float, float> converVert{ iter0.X,iter0.Y };
				innerHole.push_back(converVert);
			}
			tempVertices.push_back(innerHole);
			totalVetices.Append(iter);
		}
		mapbox::FArmyEarcutTesselator<float, decltype(tempVertices)> tesslator(tempVertices);
		tesslator.Run();
		std::vector<uint32_t> indexs = tesslator.Indices();
		for (uint32_t index : indexs)
		{
			OutResults.Push(FVector2D(totalVetices[index]));
		}
	}
	static void ClipperGapVertsAndTriangle(const TArray<FVector>& InPolygonArea, const TArray<TArray<FVector>>& InHoles, TArray<FVector2D>& OutResults)
	{
		struct TempTPathNode
		{
			bool IsPoly;
			TArray<FVector>	mOutLine;
			TArray<TempTPathNode*>		mHoles;
			PolyNode*					mNode;
		};

		TArray<TempTPathNode*> mPathNodes;
		const float SCALE0 = 100.0f;
		Clipper c;
		Paths ClipperAreas;
		Paths HolePaths;
		Paths ClipperSolution;
		int holeNumbers = InHoles.Num();
		ClipperAreas.resize(1);
		for (int i = 0; i < InPolygonArea.Num(); i++)
		{
			cInt x = (cInt)(InPolygonArea[i].X * SCALE0);
			cInt y = (cInt)(InPolygonArea[i].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}
		for (int i = 0; i < holeNumbers; i++)
		{
			Path holePath;
			for (int j = 0; j < InHoles[i].Num(); j++)
			{
				cInt x = (cInt)(InHoles[i][j].X * SCALE0);
				cInt y = (cInt)(InHoles[i][j].Y * SCALE0);
				holePath.push_back(IntPoint(x, y));
			}
			HolePaths.push_back(holePath);
		}
		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPaths(HolePaths, ptClip, true);
		PolyTree RetPolys;
		c.Execute(ctDifference, RetPolys, pftNonZero, pftNonZero);

		TMap<PolyNode*, TempTPathNode*> TempNodes;
		for (PolyNode* p = RetPolys.GetFirst(); p != NULL; p = p->GetNext())
		{

			TempTPathNode* pNode = new TempTPathNode;
			pNode->mNode = p;
			pNode->IsPoly = !p->IsHole();
			mPathNodes.Add(pNode);
			TempNodes.Add(p, pNode);
		}

		for (auto& pNode : mPathNodes)
		{
			Path& ph = pNode->mNode->Contour;
			for (Path::iterator p = ph.begin(); p != ph.end(); ++p)
			{
				pNode->mOutLine.Emplace(FVector(p->X / SCALE0, p->Y / SCALE0, 0.0f));
			}
			for (auto& p : pNode->mNode->Childs)
			{
				auto pChild = *TempNodes.Find(p);
				pNode->mHoles.Emplace(pChild);
			}
		}
		for (auto& pNode : mPathNodes)
		{
			if (pNode->IsPoly)
			{
				TArray<TArray<FVector>> mHoles;
				for (auto& p : pNode->mHoles)
				{
					mHoles.Emplace(p->mOutLine);
				}
				TriangleAreaWithHole(pNode->mOutLine, mHoles, OutResults);
			}
		}
		for (auto p : mPathNodes)
		{
			delete(p);
		}
		mPathNodes.Empty();

	}
	static TArray<FVector2D> UniqueHoles(const TArray<FVector2D>& InHoles)
	{
		struct TempEdge
		{
			TempEdge(FVector2D InStart, FVector2D InEnd) :Start(InStart), End(InEnd) {}
			bool operator ==(const TempEdge& other)  const
			{
				bool result0 = other.Start.Equals(Start, 0.1f) && other.End.Equals(End, 0.1f);
				bool reuslt1 = other.End.Equals(Start, 0.1f) && other.Start.Equals(End, 0.1f);
				if (result0 || reuslt1)
					return true;
				return false;
			}

			FVector2D Start;
			FVector2D End;
		};
		checkSlow(InHoles.Num() % 2 == 0);
		int number = InHoles.Num();
		TArray<TempEdge> UniqueEdges;
		for (int i = 0; i < number; i += 2)
		{
			TempEdge temp = TempEdge(InHoles[i], InHoles[i + 1]);
			if (!UniqueEdges.Contains(temp))
			{
				UniqueEdges.Emplace(temp);
			}
		}
		number = UniqueEdges.Num();
		TArray<FVector2D> uniqueArray;
		for (int i = 0; i < number; i++)
		{
			uniqueArray.Emplace(UniqueEdges[i].Start);
			uniqueArray.Emplace(UniqueEdges[i].End);
		}
		return uniqueArray;
	}
	static bool CombineArea(const TArray<FVector>& InPolygonArea, const TArray<TArray<FVector>>& InHoles, TArray<FVector>& results)
	{
		const float SCALE0 = 100000.0f;
		Clipper c;
		Paths ClipperAreas;
		Paths HolePaths;
		Paths solution;
		int holeNumbers = InHoles.Num();
		ClipperAreas.resize(1);
		for (int iter = 0; iter < InPolygonArea.Num(); iter++)
		{
			cInt x = (cInt)(InPolygonArea[iter].X * SCALE0);
			cInt y = (cInt)(InPolygonArea[iter].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}
		for (int iter = 0; iter < holeNumbers; iter++)
		{
			Path holePath;
			for (int j = 0; j < InHoles[iter].Num(); j++)
			{
				cInt x = (cInt)(InHoles[iter][j].X * SCALE0);
				cInt y = (cInt)(InHoles[iter][j].Y * SCALE0);
				holePath.push_back(IntPoint(x, y));
			}
			HolePaths.push_back(holePath);
		}
		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPaths(HolePaths, ptClip, true);
		c.Execute(ctUnion, solution, pftNonZero, pftNonZero);

		if (solution.size() == 1)
		{
			const Path& p = solution[0];
			results.Empty();
			for (auto& iter : p)
			{
				results.Push(FVector(iter.X / SCALE0, iter.Y / SCALE0, 0.0f));
			}
			return true;
		}
		return false;
	}
	static void ClipperIntersecionAndTriangle(const TArray<FVector>& InPolygonArea, const TArray<TArray<FVector>>& InHoles, TArray<FVector2D>& results)
	{
		const float SCALE0 = 100000.0f;
		Clipper c;
		Paths ClipperAreas;
		Paths HolePaths;
		int holeNumbers = InHoles.Num();
		ClipperAreas.resize(1);
		for (int iter = 0; iter < InPolygonArea.Num(); iter++)
		{
			cInt x = (cInt)(InPolygonArea[iter].X * SCALE0);
			cInt y = (cInt)(InPolygonArea[iter].Y * SCALE0);
			ClipperAreas[0].push_back(IntPoint(x, y));
		}
		for (int iter = 0; iter < holeNumbers; iter++)
		{
			Path holePath;
			for (int j = 0; j < InHoles[iter].Num(); j++)
			{
				cInt x = (cInt)(InHoles[iter][j].X * SCALE0);
				cInt y = (cInt)(InHoles[iter][j].Y * SCALE0);
				holePath.push_back(IntPoint(x, y));
			}
			HolePaths.push_back(holePath);
		}
		c.AddPaths(ClipperAreas, ptSubject, true);
		c.AddPaths(HolePaths, ptClip, true);
		PolyTree RetPolys;
		c.Execute(ctDifference, RetPolys, pftNonZero, pftNonZero);
		struct TempPathNode
		{
			bool IsPoly;
			TArray<FVector> m_OutLine;
			TArray<TempPathNode*> m_Holes;
			PolyNode* mNode;
		};
		int totalVertsNum = 0;
		TMap<PolyNode*, TempPathNode*> TempNodes;
		TArray<TempPathNode*> mPathNodes;
		for (PolyNode* p = RetPolys.GetFirst(); p != NULL; p = p->GetNext())
		{
			totalVertsNum += p->Contour.size();
			TempPathNode* pNode = new TempPathNode;
			pNode->mNode = p;
			pNode->IsPoly = !p->IsHole();
			mPathNodes.Add(pNode);
			TempNodes.Add(p, pNode);
		}
		for (auto& pNode : mPathNodes)
		{
			Path& ph = pNode->mNode->Contour;
			for (Path::iterator p = ph.begin(); p != ph.end(); ++p)
			{
				pNode->m_OutLine.Push(FVector(p->X / SCALE0, p->Y / SCALE0, 0.0f));
			}
			for (auto& p : pNode->mNode->Childs)
			{
				auto pChild = *TempNodes.Find(p);
				pNode->m_Holes.Push(pChild);
			}
		}
		results.Empty();
		for (auto& pNode : mPathNodes)
		{
			if (pNode->IsPoly)
			{
				TArray<FVector>& outArea = pNode->m_OutLine;
				TArray<TArray<FVector>> tempHoles;
				for (auto& p : pNode->m_Holes)
				{
					tempHoles.Push(p->m_OutLine);
				}
				TriangleAreaWithHole(outArea, InHoles, results);
			}
		}
		for (auto p : mPathNodes)
			delete p;
		mPathNodes.Empty();

	}
}

