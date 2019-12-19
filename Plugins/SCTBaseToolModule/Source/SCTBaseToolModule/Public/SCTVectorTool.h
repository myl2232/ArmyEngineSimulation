#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"


SCTBASETOOLMODULE_API bool  LineBoxIntersectionWithHitPoint(
	const FBox& Box,
	const FVector& Start,
	const FVector& End,
	const FVector& Direction,
	FVector& OutHit);

SCTBASETOOLMODULE_API bool  GetPositionOnBBoxFromScreenPos(const FVector2D &ScreenLocation, float InBBoxWidth, float InBBoxDepth, float InBBoxHeight, FVector &OutHit);

SCTBASETOOLMODULE_API int32  GetCameraMostlyFacingPlane(float InBBoxWidth, float InBBoxDepth, float InBBoxHeight, FPlane &OutPlane);