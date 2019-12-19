#include "HardModeData/Mathematics/IntersectSegment2dBox2d.h"

namespace Math
{

	IntersectSegment2dBox2d::IntersectSegment2dBox2d(const Segment2d& InSegment, const Box2d& InBox) :
		MSegment(&InSegment),
		MBox(&InBox)
	{

	}

	const Segment2d& IntersectSegment2dBox2d::GetSegment() const
	{
		return *MSegment;
	}

	const Box2d& IntersectSegment2dBox2d::GetBox() const
	{
		return *MBox;
	}

	bool IntersectSegment2dBox2d::Test()
	{
		FVector2D diff = MSegment->Center - MBox->Center;


		float AWdU[2], ADdU[2], RHS;
		AWdU[0] = FMath::Abs(MSegment->Direction | (MBox->Axis[0]));
		ADdU[0] = FMath::Abs(diff | (MBox->Axis[0]));
		RHS = MBox->Extent[0] + MSegment->Extent*AWdU[0];
		if (ADdU[0] > RHS)
		{
			return false;
		}

		AWdU[1] = FMath::Abs(MSegment->Direction | (MBox->Axis[1]));
		ADdU[1] = FMath::Abs(diff | (MBox->Axis[1]));
		RHS = MBox->Extent[1] + MSegment->Extent*AWdU[1];
		if (ADdU[1] > RHS)
		{
			return false;
		}

		FVector2D perp = FVector2D(MSegment->Direction.Y, -MSegment->Direction.X);
		float LHS = FMath::Abs(perp | (diff));
		float part0 = FMath::Abs(perp | (MBox->Axis[0]));
		float part1 = FMath::Abs(perp | (MBox->Axis[1]));
		RHS = MBox->Extent[0] * part0 + MBox->Extent[1] * part1;
		return LHS <= RHS;
	}

	bool IntersectSegment2dBox2d::Find()
	{
		float t0 = -MSegment->Extent, t1 = MSegment->Extent;
		return DoClipping(t0, t1, MSegment->Center,
			MSegment->Direction, *MBox, mQuantity, InterPoints,
			mIntersectionType);
	}

	bool IntersectSegment2dBox2d::GetInBoxSegment(FVector2D& IntersectStart, FVector2D& IntersectEnd)
	{
		if (Test())
		{
			if (Find())
			{
				if (mQuantity == 2)
				{
					IntersectStart = InterPoints[0];
					IntersectEnd = InterPoints[1];
				}
				else if (mQuantity == 1)
				{
					IntersectStart = InterPoints[0];
				}
				return true;
			}

		}

		return false;
	}

	int32 IntersectSegment2dBox2d::Getquatity() const
	{
		return 0;
	}

	const FVector2D IntersectSegment2dBox2d::GetPoints(int i) const
	{
		return InterPoints[i];
	}

	bool IntersectSegment2dBox2d::DoClipping(float t0, float t1, const FVector2D& origin, const FVector2D& direction, const Box2d& box, int& quantity, FVector2D point[2], int& intrType)
	{
		FVector2D diff = origin - box.Center;
		FVector2D BOrigin(
			diff | (box.Axis[0]),
			diff | (box.Axis[1])
		);
		FVector2D BDirection(
			direction | (box.Axis[0]),
			direction | (box.Axis[1])
		);

		float saveT0 = t0, saveT1 = t1;
		bool notAllClipped =
			Clip(+BDirection.X, -BOrigin.X - box.Extent[0], t0, t1) &&
			Clip(-BDirection.X, +BOrigin.X - box.Extent[0], t0, t1) &&
			Clip(+BDirection.Y, -BOrigin.Y - box.Extent[1], t0, t1) &&
			Clip(-BDirection.Y, +BOrigin.Y - box.Extent[1], t0, t1);

		if (notAllClipped && (t0 != saveT0 || t1 != saveT1))
		{
			if (t1 > t0)
			{
				intrType = IT_SEGMENT;
				quantity = 2;
				point[0] = origin + t0*direction;
				point[1] = origin + t1*direction;
			}
			else
			{
				intrType = IT_POINT;
				quantity = 1;
				point[0] = origin + t0*direction;
			}
		}
		else
		{
			intrType = IT_EMPTY;
			quantity = 0;
		}

		return intrType != IT_EMPTY;
	}

	bool IntersectSegment2dBox2d::Clip(float denom, float numer, float& t0, float& t1)
	{

		if (denom > (float)0)
		{
			if (numer > denom*t1)
			{
				return false;
			}
			if (numer > denom*t0)
			{
				t0 = numer / denom;
			}
			return true;
		}
		else if (denom < (float)0)
		{
			if (numer > denom*t0)
			{
				return false;
			}
			if (numer > denom*t1)
			{
				t1 = numer / denom;
			}
			return true;
		}
		else
		{
			return numer <= (float)0;
		}
	}

}