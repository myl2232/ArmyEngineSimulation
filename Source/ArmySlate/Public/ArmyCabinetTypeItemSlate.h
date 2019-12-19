#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 * 智能设计模式下，厨具类型的UI单元，包含某种厨具（例如烟机）的各种类型UI
 */
class ARMYSLATE_API SArmyCabinetTypeItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyCabinetTypeItem)
		: _ItemCode(-1)
		, _ItemWidth(264)
		, _ItemHeight(350)
		, _bHasMultiValidItem(false)
	{}

	SLATE_ARGUMENT(int32, ItemCode)

	SLATE_ARGUMENT(int32, ItemWidth)

	SLATE_ARGUMENT(int32, ItemHeight)

	SLATE_ARGUMENT(bool, bHasMultiValidItem)

	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);

	void AddItem(TSharedPtr<SWidget> ItemWidget);

public:

	TArray<TSharedPtr<class SArmyCabinetItem>> CabinetItems;

	TArray<TSharedPtr<SArmyCabinetItem>> SelectedCabinetItems;

public:

	TSharedPtr<SWidget> Owner;

private:
	
	int32 ItemCode;

	TSharedPtr<class STextBlock> CabinetTypeText; //某种厨具类型 （例如 ： 厨具中的吸烟机）

	TSharedPtr<class SScrollWrapBox> CabinetTypeListWidget;
};