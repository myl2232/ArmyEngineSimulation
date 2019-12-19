#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 * 智能设计模式，多个厨房的情况下，弹框中的单个厨房UI
 */

class ARMYSLATE_API SArmyKitchenItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyKitchenItem)
		: _ItemCode(-1)
		, _ItemHeight(40)
		, _ItemWidth(391)
	{}

	SLATE_ARGUMENT(int32, ItemCode)

	SLATE_ARGUMENT(float, ItemWidth)

	SLATE_ARGUMENT(float, ItemHeight)

	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);

public:

	TSharedPtr<SWidget> Owner;

private:
	
	int32 ItemCode;

	TSharedPtr<SImage> ImageToShowCheck;
};