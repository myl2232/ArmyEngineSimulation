#pragma once

#include "CoreMinimal.h"
#include "BatchedElements.h"

class ARMYSHADERS_API FNormalMapBatchedElementParameters : public FBatchedElementParameters
{
public:
	FNormalMapBatchedElementParameters() {}
	/** Binds vertex and pixel shaders for this element */
	virtual void BindShaders(
		FRHICommandList& RHICmdList,
		FGraphicsPipelineStateInitializer& GraphicsPSOInit,
		ERHIFeatureLevel::Type InFeatureLevel,
		const FMatrix& InTransform,
		const float InGamma,
		const FMatrix& ColorWeights,
		const FTexture* Texture) override;
};
