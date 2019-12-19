#pragma  once
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File Box2d.h
* @Description AABB盒子
*
* @Author 欧石楠
* @Date 2018年12月1日
* @Version 1.0
*/
namespace Math
{
	class Box2d
	{
	public:
		Box2d() {}
		~Box2d() {}
		/*
		* 够着带有旋转的+
		*/
		Box2d(const FVector2D& InCenter, const FVector2D Axis[2], const float Extent[2]);
		/*
		* 矩形构造函数
		*/
		Box2d(const FVector2D& InCenter, const FVector2D& Axis1, const FVector2D& Axis2, const float Extent0, const float Extent1);
		/*
		* 快速度判断一个点是否在矩形内部
		* 只需要判断改点是否在上下两条边和左边两条边之间就行
		*/
		bool Contains(const FVector2D& InTestPos);
		/*
		* 计算矩形的四个顶点
		*/
		void ComputeVertices(FVector2D vertex[4])const;
	public:
		FVector2D Center;

		FVector2D Axis[2];

		float Extent[2];
	};
}