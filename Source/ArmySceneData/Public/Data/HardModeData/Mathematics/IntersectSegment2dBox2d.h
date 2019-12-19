#pragma once
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File IntersectSegment2dBox2d.h
* @Description 线段和aabb盒子求交点
*
* @Author 欧石楠
* @Date 2018年12月1日
* @Version 1.0
*/
#include "HardModeData/Mathematics/Box2d.h"
#include "HardModeData/Mathematics/Segment2d.h"

namespace Math
{
	class IntersectSegment2dBox2d
	{
	public:
		enum
		{
			IT_EMPTY,
			IT_POINT,
			IT_SEGMENT,
			IT_RAY,
			IT_LINE,
			IT_POLYGON,
			IT_PLANE,
			IT_POLYHEDRON,
			IT_OTHER
		} ;
		/*
		* 判断线段和矩形包围方法，构造函数中传入线段和矩形
		*/
		IntersectSegment2dBox2d(const Segment2d& InSegment, const Box2d& InBox);
		/*
		* 获取线段
		*/
		const Segment2d&  GetSegment()const;
		/*
		* 获取矩形
		*/
		const Box2d& GetBox()const;
		/*
		* 快速判断矩形和线段是否相交
		*/
		virtual bool Test();
		/*
		* 找到矩形和线段相交的交点
		*/
		virtual bool Find();
		/*
		* 获取线段和矩形相交并且在矩形内的点
		*/
		bool GetInBoxSegment(FVector2D& IntersectStart, FVector2D& IntersectEnd);
		/*
		* 获取线段和矩形交点的个数，0表示没有交点，1表示一个交点，2表示两个交点
		*/
		int32 Getquatity() const;
		/*
		* 获取交点坐标
		*/
		const FVector2D GetPoints(int i)const;

	private:
		bool DoClipping(float t0, float t1, const FVector2D& origin, const FVector2D& direction, const Box2d& box, int& quantity, FVector2D point[2], int& intrType);
		
		bool Clip(float  denom, float numer, float& t0, float& t1);
		
		const Segment2d* MSegment;

		const Box2d* MBox;

		int mQuantity;

		int  mIntersectionType;
		
		FVector2D InterPoints[2];

	};
}