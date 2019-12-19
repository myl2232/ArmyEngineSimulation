#pragma  once 
/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File Segment2d.h
* @Description �߶���
*
* @Author ŷʯ�
* @Date 2018��12��1��
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
		* �߶��๹�캯��
		* @InStart �ߵ�
		* @InEnd �߶��յ�
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