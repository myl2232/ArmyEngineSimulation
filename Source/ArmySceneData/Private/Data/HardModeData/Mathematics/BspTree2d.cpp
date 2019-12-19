#include "BspTree2d.h"
#include "BspPolygon2d.h"
BspTree2d::BspTree2d(BspPolygon2d& polygon, const TArray<Edge2d>& edges)
{
	// 从第一条边构建分割线
	FVector2D end0 = polygon.MVertArray[edges[0].I0];
	FVector2D end1 = polygon.MVertArray[edges[0].I1];

	MCoincident.push_back(edges[0]);
	TArray<Edge2d> PosArray, NegArray;

	int iMax = edges.Num();

	for (int i = 1; i < iMax; ++i)
	{
		int v0 = edges[i].I0;
		int v1 = edges[i].I1;
		FVector2D vertex0 = polygon.MVertArray[v0];
		FVector2D vertex1 = polygon.MVertArray[v1];

		FVector2D intr;

		int vMid;
		switch (Classify(end0, end1, vertex0, vertex1, intr))
		{
		case TRANSVERSE_POSITIVE:
			vMid = polygon.InsertVertex(intr);
			polygon.SplitEdge(v0, v1, vMid);
			PosArray.Push(Edge2d(vMid, v1));
			NegArray.Push(Edge2d(v0, vMid));
			break;
		case TRANSVERSE_NEGATIVE:
			vMid = polygon.InsertVertex(intr);
			polygon.SplitEdge(v0, v1, vMid);
			PosArray.Push(Edge2d(v0, vMid));
			NegArray.Push(Edge2d(vMid, v1));
			break;
		case ALL_POSITIVE:
			PosArray.Push(edges[i]);
			break;
		case ALL_NEGATIVE:
			NegArray.Push(edges[i]);
			break;

		default:
			MCoincident.push_back(edges[i]);
			break;
		}
	}
	if (PosArray.Num() > 0)
	{
		MPosChild = new BspTree2d(polygon, PosArray);
	}
	else
		MPosChild = 0;
	if (NegArray.Num() > 0)
	{
		MNegChild = new BspTree2d(polygon, NegArray);
	}
	else
		MNegChild = 0;

}
// 判断两条线段的相交情况
int BspTree2d::Classify(const FVector2D& end0, const FVector2D& end1, const FVector2D& v0, const FVector2D& v1, FVector2D& intr) const
{
	const float epsilon0 = 0.01f;
	const float epsilon1 = 0.99f;

	FVector2D dir = end1 - end0;
	float x = end1.X - end0.X;
	float y = end1.Y - end0.Y;
	FVector2D nor = FVector2D(y, -x);

	FVector2D diff0 = v0 - end0;
	FVector2D diff1 = v1 - end0;
	float d0 = nor.X * diff0.X + nor.Y * diff0.Y;
	float d1 = nor.X * diff1.X + nor.Y * diff1.Y;

	FVector2D normlizeNormal = nor.GetSafeNormal();
	float tempD0 = FVector2D::DotProduct(diff0, normlizeNormal);
	float tempD1 = FVector2D::DotProduct(diff1, normlizeNormal);
	if (FMath::IsNearlyZero(tempD0, 0.09f) && FMath::IsNearlyZero(tempD1, 0.09f))
		return COINCIDENT;
	if (d0 * d1 < 0.0f)
	{
		double t = d0 / (d0 - d1);
		if (t > epsilon0)
		{
			if (t < epsilon1)
			{
				intr = v0 + t*(v1 - v0);
				if (d1 > 0.0f)
				{
					return TRANSVERSE_POSITIVE;
				}
				else
					return TRANSVERSE_NEGATIVE;
			}
			else
			{
				d1 = 0.0f;
			}
		}
		else
		{
			d0 = 0.0f;
		}
	}
	if (d0 > 0.0 || d1 > 0.0)
	{
		return ALL_POSITIVE;
	}
	if (d0 < 0.0 || d1 < 0.0)
	{
		return ALL_NEGATIVE;
	}
	return COINCIDENT;
}

int BspTree2d::Classify(const FVector2D& end0, const FVector2D& end1, const FVector2D& vertex) const
{
	const double epsilon = 0.001f;
	FVector2D dir = end1 - end0;
	FVector2D nor = FVector2D(dir.Y, -dir.X);
	FVector2D diff = vertex - end0;
	double c = nor | diff;

	if (c > epsilon)
	{
		return ALL_POSITIVE;
	}
	if (c < -epsilon)
	{
		return ALL_NEGATIVE;
	}
	return COINCIDENT;

}

int BspTree2d::CoPointLocation(const BspPolygon2d& polygon, const FVector2D& vertex) const
{
	const double epsilon = 0.0001f;
	const int numEdges = (int)MCoincident.size();
	for (int i = 0; i < numEdges; ++i)
	{
		FVector2D end0 = polygon.MVertArray[MCoincident[i].I0];
		FVector2D end1 = polygon.MVertArray[MCoincident[i].I1];

		FVector2D dir = end1 - end0;
		FVector2D diff = vertex - end0;
		double tmax = dir | dir;
		double t = dir | diff;

		if (-epsilon <= t && t <= tmax + epsilon)
		{
			return 0;
		}
	}
	if (MPosChild)
	{
		return MPosChild->PointLocation(polygon, vertex);
	}
	if (MNegChild)
	{
		return MNegChild->PointLocation(polygon, vertex);
	}
	return 0;
}

void BspTree2d::GetPosPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame, BspPolygon2d& coDiff) const
{
	if (MPosChild)
	{
		MPosChild->GetPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
	}
	else
	{
		int i0 = pos.InsertVertex(v0);
		int i1 = pos.InsertVertex(v1);
		pos.InsertEdge(Edge2d(i0, i1));
	}
}

void BspTree2d::GetNegPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame, BspPolygon2d& coDiff) const
{
	if (MNegChild)
	{
		MNegChild->GetPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
	}
	else
	{
		int i0 = neg.InsertVertex(v0);
		int i1 = neg.InsertVertex(v1);
		neg.InsertEdge(Edge2d(i0, i1));
	}
}

void BspTree2d::GetCoPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame, BspPolygon2d& coDiff) const
{
	const double epsilon = 0.001f;

	FVector2D dir = v1 - v0;

	double tmax = dir | dir;

	FVector2D end0, end1;

	double t0, t1;
	bool sameDir;

	std::list<Interval> intervalList;
	std::list<Interval>::iterator iter;

	const int numEdges = (int)MCoincident.size();

	for (int i = 0; i < numEdges; ++i)
	{
		end0 = polygon.MVertArray[MCoincident[i].I0];
		end1 = polygon.MVertArray[MCoincident[i].I1];

		t0 = dir | (end0 - v0);
		if (FMath::Abs(t0) <= epsilon)
		{
			t0 = 0.0;
		}
		else if (FMath::Abs(t0 - tmax) <= epsilon)
		{
			t0 = tmax;
		}
		t1 = dir | (end1 - v0);
		if (FMath::Abs(t1) <= epsilon)
		{
			t1 = 0.0f;
		}
		else if (FMath::Abs(t1 - tmax) <= epsilon)
		{
			t1 = tmax;
		}

		sameDir = (t1 > t0);
		if (!sameDir)
		{
			double save = t0;
			t0 = t1;
			t1 = save;
		}

		if (t1 > 0.0 && t0 < tmax)
		{
			if (intervalList.empty())
			{
				intervalList.push_front(Interval(t0, t1, sameDir, true));
			}
			else
			{
				iter = intervalList.begin();
				for (; iter != intervalList.end(); ++iter)
				{
					if (FMath::Abs(t1 - iter->T0) <= epsilon)
					{
						t1 = iter->T0;
					}
					if (t1 <= iter->T0)
					{
						intervalList.insert(iter, Interval(t0, t1, sameDir, true));
						break;
					}
					if (FMath::Abs(t0 - iter->T1) <= epsilon)
					{
						t0 = iter->T1;
					}

					std::list<Interval>::iterator last = intervalList.end();
					--last;

					if (iter == last)
					{
						intervalList.push_front(Interval(t0, t1, sameDir, true));
						break;
					}
				}
			}
		}
	}

	if (intervalList.empty())
	{
		GetPosPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
		GetNegPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
		return;
	}

	Interval& front = intervalList.front();

	if (front.T0 > 0.0)
	{
		intervalList.push_front(Interval(0.0, front.T0, front.SameDir, false));
	}
	else
	{
		front.T0 = 0.0;
	}
	Interval& back = intervalList.back();
	if (back.T1 < tmax)
	{
		intervalList.push_front(Interval(back.T1, tmax, back.SameDir, false));
	}
	else
	{
		back.T1 = tmax;
	}

	std::list<Interval>::iterator iter0 = intervalList.begin();
	std::list<Interval>::iterator iter1 = intervalList.begin();

	for (++iter1; iter1 != intervalList.end(); ++iter0, ++iter1)
	{
		t0 = iter0->T1;
		t1 = iter1->T0;
		if (t1 - t0 > epsilon)
		{
			iter0 = intervalList.insert(iter1, Interval(t0, t1, true, false));
		}
	}

	double invTMax = 1.0 / tmax;

	t0 = intervalList.front().T0 * invTMax;

	end1 = v0 + (intervalList.front().T0* invTMax) * dir;
	iter = intervalList.begin();

	for (; iter != intervalList.end(); ++iter)
	{
		end0 = end1;
		t1 = iter->T1 * invTMax;
		end1 = v0 + (iter->T1* invTMax) * dir;

		if (iter->Touching)
		{
			Edge2d edge;
			if (iter->SameDir)
			{
				edge.I0 = coSame.InsertVertex(end0);
				edge.I1 = coSame.InsertVertex(end1);
				if (edge.I0 != edge.I1)
				{
					coSame.InsertEdge(edge);
				}
			}
			else
			{
				edge.I0 = coDiff.InsertVertex(end1);
				edge.I1 = coDiff.InsertVertex(end0);
				if (edge.I0 != edge.I1)
				{
					coDiff.InsertEdge(edge);
				}
			}
		}
		else
		{
			GetPosPartition(polygon, end0, end1, pos, neg, coSame, coDiff);
			GetNegPartition(polygon, end0, end1, pos, neg, coSame, coDiff);
		}
	}


}

BspTree2d::~BspTree2d()
{
	delete MNegChild;
	delete MPosChild;
}

BspTree2d* BspTree2d::GetCopy() const
{
	BspTree2d* tree = new BspTree2d();
	tree->MCoincident = MCoincident;

	if (MPosChild)
	{
		tree->MPosChild = MPosChild->GetCopy();
	}
	else
		tree->MPosChild = 0;

	if (MNegChild)
	{
		tree->MNegChild = MNegChild->GetCopy();
	}
	else
		tree->MNegChild = 0;

	return tree;
}

void BspTree2d::Negate()
{
	const int numEdges = (int)MCoincident.size();
	for (int i = 0; i < numEdges; ++i)
	{
		Edge2d& edge = MCoincident[i];
		int save = edge.I0;
		edge.I0 = edge.I0;
		edge.I0 = save;
	}
	BspTree2d* save = MPosChild;
	MPosChild = MNegChild;
	MNegChild = save;
	if (MPosChild)
	{
		MPosChild->Negate();
	}
	if (MNegChild)
	{
		MNegChild->Negate();
	}
}

void BspTree2d::GetPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame, BspPolygon2d& coDiff) const
{
	// 建立第一条分离线
	FVector2D end0 = polygon.MVertArray[MCoincident[0].I0];
	FVector2D end1 = polygon.MVertArray[MCoincident[0].I1];

	FVector2D intr;
	switch (Classify(end0, end1, v0, v1, intr))
	{
	case TRANSVERSE_POSITIVE:
		GetPosPartition(polygon, intr, v1, pos, neg, coSame, coDiff);
		GetNegPartition(polygon, v0, intr, pos, neg, coSame, coDiff);
		break;
	case TRANSVERSE_NEGATIVE:
		GetPosPartition(polygon, v0, intr, pos, neg, coSame, coDiff);
		GetNegPartition(polygon, intr, v1, pos, neg, coSame, coDiff);
		break;
	case ALL_POSITIVE:
		GetPosPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
		break;
	case ALL_NEGATIVE:
		GetNegPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
		break;
	default:
		GetCoPartition(polygon, v0, v1, pos, neg, coSame, coDiff);
		break;
	}
}

int BspTree2d::PointLocation(const BspPolygon2d& polygon, const FVector2D& vertex) const
{
	FVector2D end0 = polygon.MVertArray[MCoincident[0].I0];
	FVector2D end1 = polygon.MVertArray[MCoincident[0].I1];

	switch (Classify(end0, end1, vertex))
	{
	case ALL_POSITIVE:
		if (MPosChild)
		{
			return MPosChild->PointLocation(polygon, vertex);
		}
		else
			return 1;
	case ALL_NEGATIVE:
		if (MNegChild)
		{
			return MNegChild->PointLocation(polygon, vertex);
		}
		else
			return 1;
	default:
		return CoPointLocation(polygon, vertex);
	}

}

