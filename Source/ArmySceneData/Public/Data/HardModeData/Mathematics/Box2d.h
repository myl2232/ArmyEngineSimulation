#pragma  once
/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File Box2d.h
* @Description AABB����
*
* @Author ŷʯ�
* @Date 2018��12��1��
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
		* ���Ŵ�����ת��+
		*/
		Box2d(const FVector2D& InCenter, const FVector2D Axis[2], const float Extent[2]);
		/*
		* ���ι��캯��
		*/
		Box2d(const FVector2D& InCenter, const FVector2D& Axis1, const FVector2D& Axis2, const float Extent0, const float Extent1);
		/*
		* ���ٶ��ж�һ�����Ƿ��ھ����ڲ�
		* ֻ��Ҫ�жϸĵ��Ƿ������������ߺ����������֮�����
		*/
		bool Contains(const FVector2D& InTestPos);
		/*
		* ������ε��ĸ�����
		*/
		void ComputeVertices(FVector2D vertex[4])const;
	public:
		FVector2D Center;

		FVector2D Axis[2];

		float Extent[2];
	};
}