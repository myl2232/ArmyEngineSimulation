#pragma once
#include "CoreMinimal.h"

namespace XRWHCMode
{
    int32 RoundIndex(int32 InStart, int32 InOffset, int32 InTotalNum);
    bool FloatGreater(float InSrc, float InDest, float InTolerance = 0.01f);
    bool FloatLess(float InSrc, float InDest, float InTolerance = 0.01f);
    bool FloatGreaterEqual(float InSrc, float InDest, float InTolerance = 0.01f);
    bool FloatLessEqual(float InSrc, float InDest, float InTolerance = 0.01f);
    bool CheckDoublication(const FVector &InDir1, const FVector &InDir2);
    bool GetIntersectionDistance(const FVector2D &InFirstOrigin, const FVector2D &InFirstDir, const FVector2D &InSecondOrigin, const FVector2D &InSecondDir, float &OutFirstDistance, float &OutSecondDistance);
    bool PointInPolygon(const FVector &InTestPoint, const TArray<FVector> &InPolygonPoints, bool bIncludeEdge);
    bool PointInCube(const FVector &InTestPoint, const TArray<FVector> &InCubeOutline, float InTestHeight, float InCubeHeightMin, float InCubeHeightMax);
}