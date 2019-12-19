#pragma once

/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File BspTree2d.h
* @Description  Bsp�����ָ����γ�bsp���������ཻ����
*
* @Author ŷʯ�
* @Date 2018��11��24��
* @Version 1.0
*/
#include<vector>
class BspPolygon2d;

class ARMYSCENEDATA_API BspTree2d
{
public:
	/*
	���ݶ���ι���bsp��
	*/
	BspTree2d(BspPolygon2d& polygon, const TArray<Edge2d>& edges);

	~BspTree2d();
	/*
	 ����bsp��
	*/
	BspTree2d* GetCopy()const;
	/*
	bsp ��ȡ��
	*/
	void Negate();
	/*
	���������bsp������
	*/
	void GetPartition(const BspPolygon2d& polygon, const FVector2D& v0, const FVector2D& v1, BspPolygon2d& pos, BspPolygon2d& neg, BspPolygon2d& coSame, BspPolygon2d& coDiff)const;
	/*
	* ���Ͷ���εĹ�ϵ
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