#pragma once

/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File BspPolygon2d.h
* @Description  ����в������㷽��
*
* @Author ŷʯ�
* @Date 2018��11��24��
* @Version 1.0
*/
#include "Edge2d.h"
#include<map>
#include<vector>

class BspTree2d;

class ARMYSCENEDATA_API BspPolygon2d
{
public:

	BspPolygon2d();

	BspPolygon2d(const BspPolygon2d& polygon);
	/*
	* ����һ�ѵ㹹��bsp�����
	*/
	BspPolygon2d(const TArray<FVector>& InVertices);
	/*
	* ��������������ڶ���
	*/
	BspPolygon2d(const TArray<FVector>& InOutVertices, const TArray<TArray<FVector>> & InHoles);

	BspPolygon2d(const TArray<TArray<FVector>>& InVerts);

	~BspPolygon2d();
	/*
	* �����
	*/
	int InsertVertex(const FVector2D& vertex);
	/*
	* �����
	*/
	int InsertEdge(const Edge2d& edge);
	/*
	* ����bsp��
	*/
	void Finalize();
	/*
	* ��ֵ���캯��
	*/
	BspPolygon2d& operator=(const BspPolygon2d& polygon);
	/*
	* ���������ȡ�����㹹����߶��ڶ�����ڲ���
	*/
	void GetInsideEdgesFrom( BspPolygon2d& inside,const FVector& startPos,const FVector& endPos);

	/*
	* ��ö���εĶ�����Ŀ
	*/
	int GetNumVertices()const;
	/*
	* ����������ȡ����εĶ�������
	*/
	bool GetVertex(int i, FVector2D& vertex)const;
	/*
	* ��ȡ����αߵ���Ŀ
	*/
	int GetNumEdges()const;
	/*
	* ��ȡ����εı�
	*/
	bool GetEdge(int i, Edge2d& edge)const;

	// negation 
	/*
	* bsp �����ȡ��
	*/
	BspPolygon2d operator~()const;
	/*
	* �����������
	*/
	BspPolygon2d operator&(const BspPolygon2d& polygon)const;
	/*
	* �����������
	*/
	BspPolygon2d operator|(const BspPolygon2d& polygon)const;
	/*
	* ������������
	*/
	BspPolygon2d operator-(const BspPolygon2d& polygon)const;
	/*
	* �����������
	*/
	BspPolygon2d operator^(const BspPolygon2d& polygon)const;
	/*
	����Ƿ��ڶ�����ڲ�
	*/
	int PointLocation(const FVector2D& vertex)const;
	/*
	��һ�����������һ��������ڲ��ı�
	*/
	void GetInsideEdgesFrom(const BspPolygon2d& polygon, BspPolygon2d& inside) const;


	void GetInsideOrCoSameCoDiff(const BspPolygon2d& polygon, BspPolygon2d& inside,BspPolygon2d& coSame,BspPolygon2d& coDiff)const;

protected:

	void SplitEdge(int v0, int v1, int vMid);
	// vertices;
	TArray<FVector2D> MVertArray;

	TMap<FVector2D, int> MVertMap;
	// edges
	std::map<Edge2d, int> MEdgeMap;

	TArray<Edge2d> MEdgeArray;

	friend class BspTree2d;
	BspTree2d* MTree;
};