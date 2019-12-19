#pragma once

/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File BspTree2d.h
* @Description  Bsp树，分割多边形成bsp树，加速相交运算
*
* @Author 欧石楠
* @Date 2018年11月24日
* @Version 1.0
*/
#include<vector>
class BspPolygon2d;

class ARMYSCENEDATA_API BspTree2d
{
public:
	/*
	根据多边形构建bsp树
	*/
	BspTree2d(BspPolygon2d& polygon, const TArray<Edge2d>& edges);

	~BspTree2d();
	/*
	 复制bsp树
	*/
	BspTree2d* GetCopy()const;
	/*
	bsp 树取反
	*/
	void Negate();
	/*
	根据两点对bsp树划分
	*/
	void GetPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame, BspPolygon2d& coDiff)const;
	/*
	* 求点和多边形的关系
	*/
	int PointLocation(const BspPolygon2d& polygon, const FVector2D& vertex)const;


private:

	BspTree2d()
	{

	}
	BspTree2d(const BspTree2d&)
	{

	}
	BspTree2d& operator =(const BspTree2d&)
	{
		return *this;
	}

	enum
	{
		TRANSVERSE_POSITIVE,
		TRANSVERSE_NEGATIVE,
		ALL_POSITIVE,
		ALL_NEGATIVE,
		COINCIDENT
	};

	int Classify(const FVector2D& end0, const FVector2D& end1, const FVector2D& v0, const FVector2D& v1, FVector2D& intr)const;

	void GetPosPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg,
		BspPolygon2d& coSame, BspPolygon2d& coDiff)const;

	void GetNegPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame,
		BspPolygon2d& coDiff)const;

	class Interval
	{
	public:
		Interval(double t0, double t1, bool sameDir, bool touching)
		{
			T0 = t0;
			T1 = t1;
			SameDir = sameDir;
			Touching = touching;
		}
		double T0, T1;
		bool SameDir, Touching;
	};

	void GetCoPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg,
		BspPolygon2d& coSame, BspPolygon2d& coDiff)const;

	int Classify(const FVector2D& end0, const FVector2D& end1, const FVector2D& vertex)const;

	int CoPointLocation(const BspPolygon2d& polygon, const FVector2D& vertex)const;

	std::vector<Edge2d> MCoincident;
	BspTree2d* MPosChild;
	BspTree2d* MNegChild;
};