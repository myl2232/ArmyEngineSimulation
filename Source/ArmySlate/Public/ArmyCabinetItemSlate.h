#pragma once

#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"

/**
 * 智能设计模式下，具体厨具项目（例如某种类型（例如顶吸式）的烟机）的UI标准单元
 * 当bCanSelectType为true时，该控件表示某种类型的厨具（例如：吸烟机中的顶吸式烟机）
 * 当bCanSelectType为false时，该控件表示某种类型厨具的具体品牌项（例如：顶式吸烟机中的美的顶式吸烟机）
 */

class ARMYSLATE_API SArmyCabinetItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyCabinetItem)
		: _ItemCode(-1)
		, _ItemHeight(164)
		, _ItemWidth(108) //设计图稿给出的112 但是考虑到有两个滚动条，必须腾出空间来
		, _bCanSelectType(true)
	{}

	SLATE_ARGUMENT(int32, ItemCode)

	SLATE_ARGUMENT(float, ItemWidth)

	SLATE_ARGUMENT(float, ItemHeight)

	SLATE_ARGUMENT(bool, bCanSelectType)

	SLATE_EVENT(FInt32Delegate, OnItemDetailInfoHovered)

	SLATE_EVENT(FInt32Delegate, OnItemDetailInfoUnHovered)

	SLATE_EVENT(FInt32Delegate, OnItemClicked)

	SLATE_EVENT(FInt32Delegate, OnSelectTypeClicked)

	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);

	FReply OnButtonSelectTypeClicked();

	FReply OnButtonSelectCabinetClicked();

public:

	TSharedPtr<SWidget> Owner;

	FInt32Delegate DelegateOnItemDetailInfoHovered;

	FInt32Delegate DelegateOnItemDetailInfoUnHovered;

	FInt32Delegate DelegateOnItemClicked;

	FInt32Delegate DelegateOnSelectTypeClicked;

	FReply OnItemClicked();

	void OnItemDetailInfoHovered();

	void OnItemDetailInfoUnHovered();

	FReply OnSelectTypeClicked();

private:
	
	int32 ItemCode;

	TSharedPtr<class STextBlock> CabinetNameText;

	TSharedPtr<class SImage> ThumbnailImage; //厨具类型略缩图

	TSharedPtr<SImage> DetailInformationImage;

	TSharedPtr<SButton> ButtonSelectType; //选择具体款式，如果bCanSelectType为false时，那么该Button应该隐去

	TSharedPtr<SButton> ButtonSelectItem; //用于选中当前厨具

	TSharedPtr<SImage> ImageToShowCheck; //用于显示厨具是否被选中

	TSharedPtr<class SBorder> Border;
};