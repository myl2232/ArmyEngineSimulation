#pragma once

#include "ArmyPoint.h"

/**
 * 操作点
 */
enum FArmyEditType
{
	ArmyOP_NONE,
	ArmyOP_MOVE,
	ArmyOP_ROTATE,
	ArmyOP_SCALE
};

class ARMYDRAWCORE_API FArmyEditPoint : public FArmyPoint
{
public:
    FArmyEditPoint();
	FArmyEditPoint(const FVector& InV);
    FArmyEditPoint(FArmyEditPoint* Copy);
    virtual ~FArmyEditPoint() {}

	bool Hover(const FVector& InPoint, class UArmyEditorViewportClient* InViewportClient);

	void AddReferenceLine(TSharedPtr<class FArmyLine> InLine);//添加点被线的引用次数（以现在的绘制方式，最大值是2）
	void MinusReferenceLine(TSharedPtr<class FArmyLine> InLine);
	uint8 GetReferenceNum() const;
	const TArray<TSharedPtr<class FArmyLine>>& GetReferenceLines() const;

    FBox GetBounds();

public:
	FArmyEditType OperationType;

private:
	TArray<TSharedPtr<class FArmyLine>> ReferenceLines;
};