#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 * 智能设计模式下，厨房厨具摆放形状的UI单元
 */

class ARMYSLATE_API SArmyKitchenTypeItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyKitchenTypeItem)
		: _ItemCode(-1)
		, _ItemHeight(112)
		, _ItemWidth(112)
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

	TSharedPtr<class SImage> ThumbnailImage; //方案结果略缩图

	TSharedPtr<SButton> ButtonSelectItem;

	TSharedPtr<SImage> ImageToShowCheck;

	TSharedPtr<class SBorder> Border;
};