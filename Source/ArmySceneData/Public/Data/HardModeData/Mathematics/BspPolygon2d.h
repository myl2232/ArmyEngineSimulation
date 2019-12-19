#pragma once

/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File BspPolygon2d.h
* @Description  多变行布尔运算方法
*
* @Author 欧石楠
* @Date 2018年11月24日
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
	* 根据一堆点构建bsp多边形
	*/
	BspPolygon2d(const TArray<FVector>& InVertices);
	/*
	* 传入外轮廓点和内洞点
	*/
	BspPolygon2d(const TArray<FVector>& InOutVertices, const TArray<TArray<FVector>> & InHoles);

	BspPolygon2d(const TArray<TArray<FVector>>& InVerts);

	~BspPolygon2d();
	/*
	* 插入点
	*/
	int InsertVertex(const FVector2D& vertex);
	/*
	* 插入边
	*/
	int InsertEdge(const Edge2d& edge);
	/*
	* 构造bsp树
	*/
	void Finalize();
	/*
	* 赋值构造函数
	*/
	BspPolygon2d& operator=(const BspPolygon2d& polygon);
	/*
	* 根据两点获取这两点构造的线段在多边形内部点
	*/
	void GetInsideEdgesFrom( BspPolygon2d& inside,const FVector& startPos,const FVector& endPos);

	/*
	* 获得多边形的顶点数目
	*/
	int GetNumVertices()const;
	/*
	* 根据索引获取多边形的顶点坐标
	*/
	bool GetVertex(int i, FVector2D& vertex)const;
	/*
	* 获取多边形边的数目
	*/
	int GetNumEdges()const;
	/*
	* 获取多边形的边
	*/
	bool GetEdge(int i, Edge2d& edge)const;

	// negation 
	/*
	* bsp 多边行取反
	*/
	BspPolygon2d operator~()const;
	/*
	* 两个多边形求交
	*/
	BspPolygon2d operator&(const BspPolygon2d& polygon)const;
	/*
	* 两个多边形求并
	*/
	BspPolygon2d operator|(const BspPolygon2d& polygon)const;
	/*
	* 两个多边形求差
	*/
	BspPolygon2d operator-(const BspPolygon2d& polygon)const;
	/*
	* 两个多边形求
	*/
	BspPolygon2d operator^(const BspPolygon2d& polygon)const;
	/*
	求点是否在多边形内部
	*/
	int PointLocation(const FVector2D& vertex)const;
	/*
	求一个多边形在另一个多边形内部的边
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