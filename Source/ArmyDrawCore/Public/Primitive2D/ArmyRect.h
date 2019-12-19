#pragma once

#include "Primitive2D/ArmyPrimitive.h"

class ARMYDRAWCORE_API FArmyRect :public FArmyPrimitive
{
public:
	FArmyRect();
	FArmyRect(FArmyRect* Copy);
	FArmyRect(FVector InPosition, float InWidth, float InHeight);
	~FArmyRect();

	bool IsSelected(const FBox2D& Box)override;
	bool IsSelected(const FVector& Point, class UArmyEditorViewportClient* InViewportClient)override;

	/**@欧石楠 判断点是否在区域内*/
	bool IsPointInRect(const FVector& Point);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;

	TArray<FVector> GetVertices()const ;

	void SetCalcUVs(bool bUse) { bCalcUVs = bUse; }
	bool GetIfCalcUVs() { return bCalcUVs; }
public:
	bool bIsFilled;
	FVector Pos;
	float Width;
	float Height;
	FLinearColor FillColor;
	FMaterialRenderProxy* MaterialRenderProxy;
	FVector XDirection;
	FVector YDirection;
	float LineThickness = 1.0f;

	float XUVRepeatDist = 40.f;
	float YUVRepeatDist = 40.f;
	bool DrawBoundray = true;
	/**@欧石楠 外部传入的中心点*/
	FVector InCenterPos = FVector::ZeroVector;
private:
	bool bCalcUVs = false;
};

