// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"
/**
 * 瀑布流回调通用组件，由ScrollBox内嵌一个WrapBox组成
 */
class ARMYSLATE_API SScrollWrapBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SScrollWrapBox)
		: _InnerSlotPadding(FVector2D::ZeroVector)
		//可以设置竖直，或者水平方向
		, _VerticalType(true)
	{}

    SLATE_ARGUMENT(FMargin, WrapBoxPadding)

	SLATE_ARGUMENT(FVector2D, InnerSlotPadding)

	SLATE_ARGUMENT(bool, VerticalType)

	SLATE_EVENT(FSimpleDelegate, Delegate_ScrollToEnd)

	SLATE_END_ARGS()

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void Construct(const FArguments& InArgs);
	//Delegate_ScrollToEnd触发之后，需要执行此函数，Delegate_ScrollToEnd才会再次被触发，一般是Http请求的回调执行时调用。
	void ResetScrollNotify();
	//添加一个单元
	void AddItem(TSharedPtr<SWidget> _ItemWidget);
	//清除所有单元
	void ClearChildren();
	//@刘克祥 删除子控件
	void RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove);

public:
	//绑定此代理，当滑动条滑动到尾部会被执行
	FSimpleDelegate Delegate_ScrollToEnd;

private:
	TSharedPtr<class SScrollBox> MyScrollBox;
	TSharedPtr<class SWrapBox> MyWrapBox;

	bool bWaitTick;
	bool bNotifyScrollEnd;
	bool bVerticalType;

};
