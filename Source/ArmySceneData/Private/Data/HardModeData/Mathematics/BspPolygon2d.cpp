#include "BspPolygon2d.h"
#include "BspTree2d.h"
BspPolygon2d::BspPolygon2d()
{
	MTree = 0;
}

BspPolygon2d::BspPolygon2d(const BspPolygon2d& polygon)
{
	MTree = 0;
	*this = polygon;
}

BspPolygon2d::BspPolygon2d(const TArray<FVector>& InVertices)
{
	MTree = 0;
	int number = InVertices.Num();
	if (number >= 3)
	{
		for (int i0 = number - 1, i1 = 0; i1 < number; i0 = i1++)
		{
			InsertVertex(FVector2D(InVertices[i1].X, InVertices[i1].Y));
			InsertEdge(Edge2d(i0, i1));
		}
	}
	Finalize();
}

BspPolygon2d::BspPolygon2d(const TArray<FVector>& InOutVertices, const TArray<TArray<FVector>> & InHoles)
{
	MTree = 0;
	int outNum = InOutVertices.Num();
	for (int i = 0; i < outNum; i++)
	{
		InsertVertex(FVector2D(InOutVertices[i].X, InOutVertices[i].Y));
		InsertEdge(Edge2d(i%outNum, (i + 1) % outNum));
	}
	int startNumber = outNum;
	for (const TArray<FVector>& iter : InHoles)
	{
		int holeVertsNum = iter.Num();
		for (int i = 0; i < holeVertsNum; i++)
		{
			InsertVertex(FVector2D(iter[i].X, iter[i].Y));
			InsertEdge(Edge2d(i%holeVertsNum + startNumber, (i + 1) % holeVertsNum + startNumber));
		}
		startNumber += holeVertsNum;
	}
	Finalize();
}

BspPolygon2d::BspPolygon2d(const TArray<TArray<FVector>>& InVerts)
{
	MTree = 0;
	int startNumber = 0;
	for (const TArray<FVector>& iter : InVerts)
	{
		int holeVertsNum = iter.Num();
		for (int i = 0; i < holeVertsNum; i++)
		{
			InsertVertex(FVector2D(iter[i].X, iter[i].Y));
			InsertEdge(Edge2d(i%holeVertsNum + startNumber, (i + 1) % holeVertsNum + startNumber));
		}
		startNumber += holeVertsNum;
	}
	Finalize();
}

BspPolygon2d::~BspPolygon2d()
{
	delete MTree;
}
BspPolygon2d& BspPolygon2d::operator=(const BspPolygon2d& polygon)
{
	MVertMap = polygon.MVertMap;
	MVertArray = polygon.MVertArray;
	MEdgeArray = polygon.MEdgeArray;
	MEdgeMap = polygon.MEdgeMap;
	delete MTree;
	MTree = (polygon.MTree ? polygon.MTree->GetCopy() : 0);
	return *this;
}

int BspPolygon2d::InsertVertex(const FVector2D& vertex)
{
	int*  iter = MVertMap.Find(vertex);
	if (iter)
		return *iter;
	int i = (int)(MVertArray.Num());
	MVertMap.Add(vertex, i);
	MVertArray.Push(vertex);
	return i;
}

int BspPolygon2d::InsertEdge(const Edge2d& edge)
{
	std::map<Edge2d, int >::iterator iter = MEdgeMap.find(edge);
	if (iter != MEdgeMap.end())
	{
		return iter->second;
	}
	int i = (int)MEdgeMap.size();
	MEdgeMap.insert(std::make_pair(edge, i));
	MEdgeArray.Push(edge);
	return i;
}

void BspPolygon2d::Finalize()
{
	delete MTree;
	MTree = new BspTree2d(*this, MEdgeArray);
}

int BspPolygon2d::GetNumVertices() const
{
	return (int)MVertArray.Num();
}

bool BspPolygon2d::GetVertex(int i, FVector2D& vertex) const
{
	if (0 <= i&&i < (int)MVertArray.Num())
	{
		vertex = MVertArray[i];
		return true;
	}
	return false;
}

int BspPolygon2d::GetNumEdges() const
{
	return  (int)MEdgeArray.Num();
}

bool BspPolygon2d::GetEdge(int i, Edge2d& edge) const
{
	if (0 <= i&&i < (int)MEdgeArray.Num())
	{
		edge = MEdgeArray[i];
		return true;
	}
	return false;
}
BspPolygon2d BspPolygon2d::operator~()const
{
	BspPolygon2d neg;
	neg.MVertArray = MVertArray;
	neg.MVertMap = MVertMap;

	std::map<Edge2d, int>::const_iterator iter = MEdgeMap.begin();
	std::map<Edge2d, int>::const_iterator end = MEdgeMap.end();

	for (; iter != end; ++iter)
	{
		neg.InsertEdge(Edge2d(iter->first.I1, iter->first.I0));
	}
	neg.MTree = MTree->GetCopy();
	neg.MTree->Negate();

	return neg;
}
BspPolygon2d BspPolygon2d::operator&(const BspPolygon2d& polygon) const
{
	BspPolygon2d intersect;
	GetInsideEdgesFrom(polygon, intersect);
	polygon.GetInsideEdgesFrom(*this, intersect);
	intersect.Finalize();
	return intersect;
}

BspPolygon2d BspPolygon2d::operator-(const BspPolygon2d& polygon) const
{
	const BspPolygon2d& thisPolygon = *this;
	return thisPolygon & ~polygon;
}

int BspPolygon2d::PointLocation(const FVector2D& vertex) const
{
	return MTree->PointLocation(*this, vertex);
}

void BspPolygon2d::GetInsideOrCoSameCoDiff(const BspPolygon2d& polygon, BspPolygon2d& inside, BspPolygon2d& coSame, BspPolygon2d& coDiff) const
{
	assert(MTree != 0);
	BspPolygon2d ignore;

	const int numEdges = polygon.GetNumEdges();
	for (int i = 0; i < numEdges; ++i)
	{
		int v0 = polygon.MEdgeArray[i].I0;
		int v1 = polygon.MEdgeArray[i].I1;
		FVector2D vertex0 = polygon.MVertArray[v0];
		FVector2D vertex1 = polygon.MVertArray[v1];
		MTree->GetPartition(*this, vertex0, vertex1, ignore, inside, coSame, coDiff);
	}
}

void BspPolygon2d::SplitEdge(int v0, int v1, int vMid)
{
	std::map<Edge2d, int>::iterator iter = MEdgeMap.find(Edge2d(v0, v1));
	int eIndex = iter->second;

	MEdgeMap.erase(iter);

	MEdgeArray[eIndex].I1 = vMid;
	MEdgeMap.insert(std::make_pair(MEdgeArray[eIndex], eIndex));

	InsertEdge(Edge2d(vMid, v1));
}

void BspPolygon2d::GetInsideEdgesFrom(const BspPolygon2d& polygon, BspPolygon2d& inside) const
{
	assert(MTree != 0);
	BspPolygon2d ignore;

	const int numEdges = polygon.GetNumEdges();
	for (int i = 0; i < numEdges; ++i)
	{
		int v0 = polygon.MEdgeArray[i].I0;
		int v1 = polygon.MEdgeArray[i].I1;
		FVector2D vertex0 = polygon.MVertArray[v0];
		FVector2D vertex1 = polygon.MVertArray[v1];
		MTree->GetPartition(*this, vertex0, vertex1, ignore, inside, ignore, ignore);
	}
}

void BspPolygon2d::GetInsideEdgesFrom(BspPolygon2d& inside, const FVector& startPos, const FVector& endPos)
{
	BspPolygon2d ignore;

	MTree->GetPartition(*this, FVector2D(startPos), FVector2D(endPos), ignore, inside, inside, ignore);
}



BspPolygon2d BspPolygon2d::operator^(const BspPolygon2d& polygon) const
{
	const BspPolygon2d& thisPolygon = *this;
	return (thisPolygon - polygon) | (thisPolygon - polygon);
}

BspPolygon2d BspPolygon2d::operator|(const BspPolygon2d& polygon) const
{
	const BspPolygon2d& thisPolygon = *this;
	return ~(~thisPolygon&~polygon);
}


