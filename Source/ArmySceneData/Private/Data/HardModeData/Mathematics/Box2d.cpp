#include "HardModeData/Mathematics/Box2d.h"

namespace Math
{
	Box2d::Box2d(const FVector2D& InCenter, const FVector2D InAxis[2], const float InExtent[2])
	{
		Center = InCenter;
		Axis[0] = InAxis[0];
		Axis[1] = InAxis[1];
	}

	bool Box2d::Contains(const FVector2D& InTestPos)
	{
		return true;
	}

	void Box2d::ComputeVertices(FVector2D vertex[4]) const
	{
		FVector2D extAxis0 = Axis[0] * Extent[0];
		FVector2D extAxis1 = Axis[1] * Extent[1];
		vertex[0] = Center - extAxis0 - extAxis1;
		vertex[1] = Center + extAxis0 - extAxis1;
		vertex[2] = Center + extAxis0 + extAxis1;
		vertex[3] = Center - extAxis0 + extAxis1;
	}

	Box2d::Box2d(const FVector2D& InCenter, const FVector2D& Axis1, const FVector2D& Axis2, const float Extent0, const float Extent1)
	{
		Center = InCenter;
		Axis[0] = Axis1;
		Axis[1] = Axis2;
		Extent[0] = Extent0;
		Extent[1] = Extent1;
	}

}

