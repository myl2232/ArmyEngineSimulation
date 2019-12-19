#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "GizmoPrimitive.generated.h"

UCLASS()
class SCTSHAPEMODULE_API UGizmoPrimitive : public UPrimitiveComponent
{
    GENERATED_UCLASS_BODY()

public:
    // UPrimitiveComponent Methods
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

    // USceneComponent Method
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
};