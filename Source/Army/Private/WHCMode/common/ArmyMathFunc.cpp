#include "ArmyMathFunc.h"
#include "Math/XRMath.h"

namespace XRWHCMode
{
    int32 RoundIndex(int32 InStart, int32 InOffset, int32 InTotalNum)
    {
        return (InStart + InOffset + InTotalNum) % InTotalNum;
    }

    bool FloatGreater(float InSrc, float InDest, float InTolerance)
    {
        if (FMath::IsNearlyEqual(InSrc, InDest, InTolerance))
            return false;
        return InSrc > InDest;
    }

    bool FloatLess(float InSrc, float InDest, float InTolerance)
    {
        if (FMath::IsNearlyEqual(InSrc, InDest, InTolerance))
            return false;
        return InSrc < InDest;
    }

    bool FloatGreaterEqual(float InSrc, float InDest, float InTolerance)
    {
        if (FMath::IsNearlyEqual(InSrc, InDest, InTolerance))
            return true;
        return InSrc > InDest;
    }

    bool FloatLessEqual(float InSrc, float InDest, float InTolerance)
    {
        if (FMath::IsNearlyEqual(InSrc, InDest, InTolerance))
            return true;
        return InSrc < InDest;
    }

    bool CheckDoublication(const FVector &InDir1, const FVector &InDir2)
    {
        float Dot = FVector::DotProduct(InDir1, InDir2);
        return FMath::IsNearlyEqual(FMath::Abs<float>(Dot), 1.0f, 1.e-2f);
    }

    bool GetIntersectionDistance(const FVector2D &InFirstOrigin, const FVector2D &InFirstDir, const FVector2D &InSecondOrigin, const FVector2D &InSecondDir, float &OutFirstDistance, float &OutSecondDistance)
    {
        if (FMath::IsNearlyZero(InFirstDir ^ InSecondDir, 1.e-2f))
            return false;
        FVector2D Delta = InSecondOrigin - InFirstOrigin;
        float Dot = InFirstDir ^ InSecondDir;
        OutFirstDistance = (Delta ^ InSecondDir) / Dot;
        OutSecondDistance = (Delta ^ InFirstDir) / Dot;
        return true;
    }

    bool PointInPolygon(const FVector &InTestPoint, const TArray<FVector> &InPolygonPoints, bool bIncludeEdge)
    {
        int32 count = InPolygonPoints.Num();

        if (count < 3)
        {
            return false;
        }

        bool result = false;

        for (int i = 0, j = count - 1; i < count; i++)
        {
            FVector p1 = InPolygonPoints[i];
            FVector p2 = InPolygonPoints[j];

            if (bIncludeEdge && FArmyMath::IsPointOnLineSegment2D(FVector2D(InTestPoint), FVector2D(p1), FVector2D(p2), 1.e-2f))
                return true;

            if (p1.X < InTestPoint.X && p2.X >= InTestPoint.X || p2.X < InTestPoint.X && p1.X >= InTestPoint.X)
            {
                if (p1.Y + (InTestPoint.X - p1.X) / (p2.X - p1.X) * (p2.Y - p1.Y) < InTestPoint.Y)
                {
                    result = !result;
                }
            }
            j = i;
        }

        return result;
    }

    bool PointInCube(const FVector &InTestPoint, const TArray<FVector> &InCubeOutline, float InTestHeight, float InCubeHeightMin, float InCubeHeightMax)
    {
        bool bInHeightRange = FloatGreaterEqual(InTestHeight, InCubeHeightMin) && FloatLessEqual(InTestHeight, InCubeHeightMax);
        return bInHeightRange && PointInPolygon(InTestPoint, InCubeOutline, false);
    }
}