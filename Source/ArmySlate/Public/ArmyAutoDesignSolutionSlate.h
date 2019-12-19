#pragma once

#include "Widgets/SCompoundWidget.h"



class ARMYSLATE_API SArmyAutoDesignSolution : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyAutoDesignSolution)
		: _ItemCode(-1)
		, _ItemHeight(142)
		, _ItemWidth(112)
	{}

	SLATE_ARGUMENT(int32, ItemCode)

	SLATE_ARGUMENT(float, ItemHeight)

	SLATE_ARGUMENT(float, ItemWidth)

	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);

public:

	TSharedPtr<SWidget> Owner;

private:
	
	int32 ItemCode;

	TSharedPtr<class SImage> ThumbnailImage; //方案结果略缩图

	TSharedPtr<SButton> ButtonApply; //应用当前方案

	TSharedPtr<class SBorder> Border; 
};