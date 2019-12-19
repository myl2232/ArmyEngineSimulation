#pragma once
#include "CoreMinimal.h"

struct FPVertex 
{
	FVector Position;

	FPVertex() {}
	FPVertex(const FVector &InPosition) : Position(InPosition) {}
};

struct FPUVVertex 
{
	FVector Position;
	FVector2D Texcoord0;

	FPUVVertex() {}
	FPUVVertex(const FVector &InPosition, const FVector2D &InTexcoord0) : Position(InPosition), Texcoord0(InTexcoord0) {}
};