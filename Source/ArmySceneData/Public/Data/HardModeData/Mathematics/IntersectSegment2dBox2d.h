#pragma once
/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File IntersectSegment2dBox2d.h
* @Description �߶κ�aabb�����󽻵�
*
* @Author ŷʯ�
* @Date 2018��12��1��
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
		* �ж��߶κ;��ΰ�Χ���������캯���д����߶κ;���
		*/
		IntersectSegment2dBox2d(const Segment2d& InSegment, const Box2d& InBox);
		/*
		* ��ȡ�߶�
		*/
		const Segment2d&  GetSegment()const;
		/*
		* ��ȡ����
		*/
		const Box2d& GetBox()const;
		/*
		* �����жϾ��κ��߶��Ƿ��ཻ
		*/
		virtual bool Test();
		/*
		* �ҵ����κ��߶��ཻ�Ľ���
		*/
		virtual bool Find();
		/*
		* ��ȡ�߶κ;����ཻ�����ھ����ڵĵ�
		*/
		bool GetInBoxSegment(FVector2D& IntersectStart, FVector2D& IntersectEnd);
		/*
		* ��ȡ�߶κ;��ν���ĸ�����0��ʾû�н��㣬1��ʾһ�����㣬2��ʾ��������
		*/
		int32 Getquatity() const;
		/*
		* ��ȡ��������
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