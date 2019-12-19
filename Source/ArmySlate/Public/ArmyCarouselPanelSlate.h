/**
 * Copyright 2019 ������Ƽ����޹�˾.
 * All Rights Reserved.
 *  
 *
 * @File SArmyCarouselPanel.h
 * @Description �ֲ����
 *
 * @Author ŷʯ�
 * @Date 2018��12��17��
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

	/**��Ϊ��ǰ�����У�����Ҫ��ť��ɫ*/
	void SetCurrent();

	/**��Ϊ���Ž���������Ҫ��ť��ɫ*/
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
	/**���һ���ֲ�����,��ӵ��ֲ�������*/
	void AddCarouselItem(TSharedPtr<SWidget> InWidget);

	/**���һ���ֲ���������*/
	void AddCarouselItemArray(TArray<TSharedPtr<SWidget>> InWidgetArray);

	/**ͨ�������л��ֲ�����*/
	void SwitchByIndex(int Index);

	/**�л�����һ������*/
	void SwitchToNext();	

private:
	/**ͨ����ť�л��ֲ�����*/
	void SwitchByButton();

private:
	/**�ֲ������л�*/
	TSharedPtr<class SScrollBox> ContentWidgetSwitcher;

	/**�����ֲ���ť�����*/
	TSharedPtr<class SHorizontalBox> CarouselButtonsContiner;

	/**�ֲ���������*/
	TArray<TSharedPtr<SWidget>> AllCarouselWidgets;

	/**������������İ�ť����*/
	TArray<TSharedPtr<SArmyCarouselButton>> AllCarouselButtons;

	/**�ֲ����(s)*/
	float CarouseInteral = 3.f;

	/**��ǰ�������ݵ�����ֵ*/
	int CurrentIndex = 0;

	FTimerHandle MessageTimeHandle;

	FTimerHandle TickTimeHandle;
	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
};
