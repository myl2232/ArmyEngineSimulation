#pragma once

#include "ArmyPrimitive.h"

/************************************************************************/
/* 点绘制                                                                     */
/************************************************************************/
class ARMYDRAWCORE_API FArmyPoint : public FArmyPrimitive
{
public:
	FArmyPoint();
	FArmyPoint(FVector Position);
    FArmyPoint(FArmyPoint* Copy);

	virtual void SetState(EOPState InState);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)override;
	bool IsSelected(const FBox2D& box)override;
	bool IsSelected(const FVector& Point, class UArmyEditorViewportClient* InViewportClient)override;

	const FVector& GetPos() const { return Pos; };
	void SetPos(const FVector& InV) { Pos = InV; };

	void Transform(const FVector& PostAdd)override;
	void Rotate(const FRotator& Rotation)override;
	void Scale(const FVector& Scale)override;
	void SetBasePoint(const FVector& InBasePoint)override;

public:
	float Size;

	FVector Pos;

	bool bSprite;

	FVector PivotPosition;
	/*属性标识符*/
	uint32 PropertyFlag = 0;

};