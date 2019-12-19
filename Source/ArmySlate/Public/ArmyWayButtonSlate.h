/**
* Copyright 2019 ����������Ƽ����޹�˾.
* All Rights Reserved.
*  
*
* @File SArmyWayButton.h
* @Description ��ʽѡ���ͼƬ���ְ�ť
*
* @Author ŷʯ�
* @Date 2018��7��2��
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "ArmyStyle.h"

class ARMYSLATE_API SArmyWayButton : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SArmyWayButton) 
	: _ButtonImage(FCoreStyle::Get().GetDefaultBrush())
	, _ButtonText(FText::FromString(TEXT("ѡ��")))
	, _Width(64)
	, _Height(64) {}
	SLATE_ARGUMENT(const FSlateBrush*, ButtonImage)
	SLATE_ARGUMENT(FText, ButtonText)
	SLATE_ARGUMENT(float, Width)
	SLATE_ARGUMENT(float, Height)
	SLATE_EVENT(FSimpleDelegate, OnDelegate_Clicked)
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

private:
	FSlateColor GetBorderColor() const { return BorderColor; }
	
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	bool bIsSelected;	
	FText ButtonText;
	FSlateColor BorderColor;
	FSlateColor BorderSelectedColor;
	FSlateColor BorderNormalColor;
	TAttribute< const FSlateBrush* > ButtonImage;

	FSimpleDelegate OnDelegate_Clicked;
};