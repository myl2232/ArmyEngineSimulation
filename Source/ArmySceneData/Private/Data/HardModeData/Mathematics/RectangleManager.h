#pragma once
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File RectangleManager.h
* @Description 平面上很多矩形碰撞检测
*
* @Author 欧石楠
* @Date 2018年11月15日
* @Version 1.0
*/
template<int N, bool Ordered>
class FeatureKey
{
protected:

	FeatureKey();
public:

	bool operator < (FeatureKey const& key)const;
	bool operator ==(FeatureKey const& key)const;

	int V[N];
};

template<int N, bool Ordered>
FeatureKey<N, Ordered> ::FeatureKey()
{

}
template<int N, bool Ordered>
bool FeatureKey <N, Ordered>::operator<(FeatureKey const& key)const
{
	for (int i = N - 1; i >= 0; --i)
	{
		if (V[i] < key.V[i])
		{
			return true;
		}
		if (V[i] > key.V[i])
		{
			return false;
		}
	}
	return false;
}
template<int N, bool Ordered>
bool FeatureKey< N, Ordered>::operator ==(FeatureKey const& key)const
{
	for (int i = 0; i < N; ++i)
	{
		if (V[i] != key.V[i])
		{
			return false;
		}
	}
	return true;
}

template <bool Ordered>
class EdgeKey :public FeatureKey<2, Ordered>
{
public:
	EdgeKey(int v0 = -1, int v1 = -1);
};
template<>
EdgeKey<true> ::EdgeKey(int v0, int v1)
{
	V[0] = v0;
	V[1] = v1;
}
template<>
EdgeKey<false>::EdgeKey(int v0, int v1)
{
	if (v0 < v1)
	{
		V[0] = v0;
		V[1] = v1;
	}
	else
	{
		V[0] = v1;
		V[1] = v0;
	}
}
class AlignedBox2D
{
public:
	AlignedBox2D() { MinPos = FVector2D::ZeroVector; MaxPos == FVector2D::ZeroVector; }

	AlignedBox2D(const FVector2D& InMin, const FVector2D& InMax);
	void GetCenterFrom(FVector2D&& OutCenter, FVector2D& OutExtent);

	FVector2D MinPos;
	FVector2D MaxPos;

	bool Intersect(const AlignedBox2D& InOther);
public:

	bool operator==(const AlignedBox2D&  box)const;
	bool operator!=(const AlignedBox2D&  box)const;
	bool operator<(const AlignedBox2D&  box)const;
	bool operator<=(const AlignedBox2D&  box)const;
	bool operator>(const AlignedBox2D&  box)const;
	bool operator>=(const AlignedBox2D&  box)const;

};

AlignedBox2D::AlignedBox2D(const FVector2D& InMin, const FVector2D& InMax)
{
	MinPos = InMin;
	MaxPos = InMax;
}
void AlignedBox2D::GetCenterFrom(FVector2D&& OutCenter, FVector2D& OutExtent)
{
	OutCenter = (MinPos + MaxPos) * 0.5f;
	OutExtent = (MaxPos - MinPos) * 0.5f;
}
bool AlignedBox2D::operator==(AlignedBox2D const& box)const
{
	return MinPos == box.MinPos && MaxPos == box.MaxPos;
}
bool AlignedBox2D::operator!=(AlignedBox2D const& box)const
{
	return !operator==(box);
}
bool AlignedBox2D::operator<(AlignedBox2D const& box)const
{
	if (MinPos < box.MinPos)
	{
		return true;
	}
	if (MinPos > box.MinPos)
	{
		return false;
	}
	return MaxPos < box.MaxPos;
}

bool AlignedBox2D::operator<=(AlignedBox2D const& box)const
{
	return operator<(box) || operator==(box);
}
bool AlignedBox2D::operator>(AlignedBox2D const& box)const
{
	return !operator<=(box);
}
bool AlignedBox2D::operator>=(AlignedBox2D const& box)const
{
	return !operator<(box);
}
bool AlignedBox2D::Intersect(const AlignedBox2D& InOther)
{
	if ((MinPos.X > InOther.MinPos.X) || (InOther.MinPos.X > MaxPos.X))
		return false;
	if ((MinPos.Y > InOther.MaxPos.Y) || (InOther.MinPos.Y > MaxPos.Y))
		return false;

	return true;
}
class RectangleManager
{
public:

	RectangleManager(TArray<AlignedBox2D>&  InRectangles);

	RectangleManager() = delete;

	RectangleManager(const RectangleManager& InManger) = delete;

	RectangleManager& operator==(const RectangleManager& InMager) = delete;

	void Initialize();

	void SetRectangle(int i, const AlignedBox2D& InRectangle);

	void GetRectangle(int i, AlignedBox2D& OuRectangle)const;

	void Update();

	TArray<EdgeKey<false>> const & GetOverLap()const;

private:

	class EndPoint
	{
	public:
		float Value;
		int Type;
		int Index;

		bool operator<(const EndPoint& InEndPoint)const;

	};
	void InsertionSort(TArray<EndPoint>& endPoints, TArray<int>& LookUp);
	TArray<AlignedBox2D>& MRectangles;
	TArray<EndPoint> M_XEndPoint, M_YEndPoint;
	TArray<EdgeKey<false>> MOverLap;
	
	TArray<int> MXLookup, MYLookup;
};
RectangleManager::RectangleManager(TArray<AlignedBox2D>& InRectangles) :
	MRectangles(InRectangles)
{
	Initialize();
}
void RectangleManager::Initialize()
{
	int intrSize = MRectangles.Num();
	int endpSize = 2 * intrSize;
	M_XEndPoint.AddUninitialized(endpSize);
	M_YEndPoint.AddUninitialized(endpSize);

	for (int i = 0, j = 0; i < intrSize; ++i)
	{
		M_XEndPoint[j].Type = 0;
		M_XEndPoint[j].Value = MRectangles[i].MinPos.X;
		M_XEndPoint[j].Index = i;

		M_YEndPoint[j].Type = 0;
		M_YEndPoint[j].Value = MRectangles[i].MinPos.Y;
		M_YEndPoint[j].Index = 1;
		++j;

		M_XEndPoint[j].Type = 1;
		M_XEndPoint[j].Value = MRectangles[i].MaxPos.X;
		M_XEndPoint[j].Index = i;
		M_YEndPoint[j].Type = 1;

		M_YEndPoint[j].Value = MRectangles[i].MaxPos.Y;
		M_YEndPoint[j].Index = i;
		++j;

	}
	M_XEndPoint.Sort();
	M_YEndPoint.Sort();

	MXLookup.AddUninitialized(endpSize);
	MYLookup.AddUninitialized(endpSize);

	for (int j = 0; j < endpSize; ++j)
	{
		MXLookup[2 * M_XEndPoint[j].Index + M_XEndPoint[j].Type] = j;
		MYLookup[2 * M_YEndPoint[j].Index + M_YEndPoint[j].Type] = j;
	}
	TSet<int> active;

	MOverLap.Empty();

	for (int i = 0; i < endpSize; ++i)
	{
		EndPoint& endPoint = M_XEndPoint[i];
		int index = endPoint.Index;

		if (endPoint.Type == 0)
		{
			for (auto activeIndex : active)
			{
				const AlignedBox2D& r0 = MRectangles[activeIndex];
				const AlignedBox2D& r1 = MRectangles[index];

				if (r0.MaxPos.Y >= r1.MinPos.Y && r0.MinPos.Y <= r1.MinPos.Y)
				{
					if (activeIndex < index)
						MOverLap.Add(EdgeKey<false>(activeIndex, index));
					else
						MOverLap.Add(EdgeKey<false>(index, activeIndex));
				}
			}
			active.Add(index);
		}
		else
			active.Remove(index);
	}
}

void RectangleManager::SetRectangle(int i, const AlignedBox2D& InRectangle)
{
	MRectangles[i] = InRectangle;
	M_XEndPoint[MXLookup[2 * i]].Value = InRectangle.MinPos.X;
	M_XEndPoint[MXLookup[2 * i + 1]].Value = InRectangle.MaxPos.X;

	M_YEndPoint[MYLookup[2 * i]].Value = InRectangle.MinPos.Y;
	M_YEndPoint[MYLookup[2 * i]].Value = InRectangle.MaxPos.Y;

}
void RectangleManager::GetRectangle(int i, AlignedBox2D& OutRectangle)const
{
	OutRectangle = MRectangles[i];
}

void RectangleManager::InsertionSort(TArray<EndPoint>& endPoint, TArray<int>& InLookUp)
{
	int endSize = endPoint.Num();

	for (int j = 1; j < endSize; ++j)
	{
		EndPoint key = endPoint[j];
		int i = j - 1;
		while (i >= 0 && key < endPoint[i])
		{
			EndPoint e0 = endPoint[i];
			EndPoint e1 = endPoint[i + 1];

			if (e0.Type == 0)
			{
				if (e1.Type == 1)
				{
					MOverLap.Remove(EdgeKey<false>(e0.Index, e1.Index));
				}
			}
			else
			{
				if (e1.Type == 0)
				{
					if (MRectangles[e0.Index].Intersect(MRectangles[e1.Index]))
					{
						MOverLap.Add(EdgeKey<false>(e0.Index, e1.Index));
					}
				}
			}
			endPoint[i] = e1;
			endPoint[i + 1] = e0;
			InLookUp[2 * e1.Index + e1.Type] = i;
			InLookUp[2 * e0.Index + e0.Type] = i + 1;
			--i;
		}

		endPoint[i + 1] = key;
		InLookUp[2 * key.Index + key.Type] = i + 1;
	}
}

void RectangleManager::Update()
{
	InsertionSort(M_XEndPoint, MXLookup);
	InsertionSort(M_YEndPoint, MYLookup);
}
TArray<EdgeKey<false>> const& RectangleManager::GetOverLap()const
{
	return MOverLap;
}
bool RectangleManager::EndPoint::operator<(const EndPoint& InEndPoint)const {
	if (Value < InEndPoint.Value)
	{
		return true;
	}
	if (Value > InEndPoint.Value)
	{
		return false;
	}
	return Type < InEndPoint.Type;
}