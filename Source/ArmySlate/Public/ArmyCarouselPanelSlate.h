/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 *  
 *
 * @File SArmyCarouselPanel.h
 * @Description 轮播面板
 *
 * @Author 欧石楠
 * @Date 2018年12月17日
 * @Version 1.0
 */

#pragma once


#include "Widgets/SCompoundWidget.h"

class SArmyCarouselButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyCarouselButton)
		: _RelatedIndex(0)
	{}
	SLATE_ARGUMENT(int, RelatedIndex)
		SLATE_EVENT(FSimpleDelegate, OnHovered)
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

	bool GetIsHovered() { return bIsHovered; }

	/**设为当前播放中，即需要按钮变色*/
	void SetCurrent();

	/**设为播放结束，即需要按钮变色*/
	void SetUnCarousel();
public:
	int RelatedIndex = 0;

private:
	void OnCarouseButtonHovered();

	void OnCarouseButtonUnHover();

private:
	TSharedPtr<SButton> CarouselButton;

	bool bIsHovered = false;

	FSimpleDelegate OnHovered;
};

class ARMYSLATE_API SArmyCarouselPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyCarouselPanel) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	/**添加一个轮播内容,添加到轮播数组中*/
	void AddCarouselItem(TSharedPtr<SWidget> InWidget);

	/**添加一个轮播内容数组*/
	void AddCarouselItemArray(TArray<TSharedPtr<SWidget>> InWidgetArray);

	/**通过索引切换轮播内容*/
	void SwitchByIndex(int Index);

	/**切换到下一个内容*/
	void SwitchToNext();	

private:
	/**通过按钮切换轮播内容*/
	void SwitchByButton();

private:
	/**轮播内容切换*/
	TSharedPtr<class SScrollBox> ContentWidgetSwitcher;

	/**承载轮播按钮的面板*/
	TSharedPtr<class SHorizontalBox> CarouselButtonsContiner;

	/**轮播内容数组*/
	TArray<TSharedPtr<SWidget>> AllCarouselWidgets;

	/**与内容相关联的按钮数组*/
	TArray<TSharedPtr<SArmyCarouselButton>> AllCarouselButtons;

	/**轮播间隔(s)*/
	float CarouseInteral = 3.f;

	/**当前播放内容的索引值*/
	int CurrentIndex = 0;

	FTimerHandle MessageTimeHandle;

	FTimerHandle TickTimeHandle;
	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
};
