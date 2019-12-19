#include "HardModeData/Mathematics/Segment2d.h"

namespace Math
{


	Segment2d::Segment2d(const FVector2D& InStart, const FVector2D& InEnd)
	{
		P0 = InStart;
		P1 = InEnd;

		ComputeCenterDirectionExtent();
	}

	Segment2d::Segment2d(const FVector2D& InCenter, const FVector2D& InDirection, float InExtent) :
		Center(InCenter),
		Direction(InDirection),
		Extent(InExtent)
	{
		ComputeEndPoints();
	}

	void Segment2d::ComputeCenterDirectionExtent()
	{
		Center = (0.5f) * (P1 + P0);

		Extent = (0.5f) *(P1 - P0).Size();

		Direction = P1 - P0;

	}

	void Segment2d::ComputeEndPoints()
	{
		P0 = Center - Extent * Direction;
		P1 = Center + Extent * Direction;
	}

}