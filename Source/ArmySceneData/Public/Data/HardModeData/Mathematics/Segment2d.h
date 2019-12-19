#pragma  once 
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File Segment2d.h
* @Description 线段类
*
* @Author 欧石楠
* @Date 2018年12月1日
* @Version 1.0
*/
namespace Math
{
	class Segment2d
	{
	public:
		Segment2d() {}
		~Segment2d() {}
		/*
		* 线段类构造函数
		* @InStart 七点
		* @InEnd 线段终点
		*/
		Segment2d(const FVector2D& InStart, const FVector2D& InEnd);

		Segment2d(const FVector2D& InCenter, const FVector2D& InDirection, float InExtent);

		void ComputeCenterDirectionExtent();

		void ComputeEndPoints();
	
	public:
		FVector2D P0, P1;

		FVector2D Center;
		FVector2D Direction;

		float Extent;



	};
}