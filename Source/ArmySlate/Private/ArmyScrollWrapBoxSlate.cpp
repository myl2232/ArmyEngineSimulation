// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyScrollWrapBoxSlate.h"
#include "SScrollBox.h"
#include "SWrapBox.h"
#include "SlateOptMacros.h"

#include "ArmyStyle.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SScrollWrapBox::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	float CBOffset = MyScrollBox->GetScrollOffset();
	if (!bWaitTick && !bNotifyScrollEnd && !MyScrollBox->IsGrabingScrollBar() && !MyScrollBox->IsRightClickScrolling())
	{
		float ScrollPanelSize = MyScrollBox->GetScrollPanelSize();
		float ContentSize = MyScrollBox->GetContentSize();
		if (ContentSize != 0.f && CBOffset + ScrollPanelSize >= ContentSize && ScrollPanelSize < ContentSize)
		{
			bNotifyScrollEnd = true;
			Delegate_ScrollToEnd.ExecuteIfBound();
		}
	}
	else
	{
		bWaitTick = false;
	}
}

void SScrollWrapBox::Construct(const FArguments& InArgs)
{
	bVerticalType = InArgs._VerticalType;
	Delegate_ScrollToEnd = InArgs._Delegate_ScrollToEnd;

	ChildSlot
	[
		SAssignNew(MyScrollBox, SScrollBox)
        .Style(&FArmyStyle::Get().GetWidgetStyle<FScrollBoxStyle>("ScrollBar.Style.Gray"))
        .ScrollBarStyle(&FArmyStyle::Get().GetWidgetStyle<FScrollBarStyle>("ScrollBar.BarStyle.Gray"))
		.Orientation(InArgs._VerticalType ? Orient_Vertical : Orient_Horizontal)

		+ SScrollBox::Slot()
        .Padding(InArgs._WrapBoxPadding)
		[
			SAssignNew(MyWrapBox, SWrapBox)
			.Visibility(bVerticalType ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed)
			.InnerSlotPadding(InArgs._InnerSlotPadding)
			.UseAllottedWidth(true)
		]
	];
	MyScrollBox->SetScrollBarRightClickDragAllowed(false);

	bNotifyScrollEnd = false;
	bWaitTick = true;
}

void SScrollWrapBox::ResetScrollNotify()
{
	bNotifyScrollEnd = false;
	bWaitTick = true;
}

void SScrollWrapBox::AddItem(TSharedPtr<SWidget> _ItemWidget)
{
	if (bVerticalType)
	{
		MyWrapBox->AddSlot()
			[
				_ItemWidget.ToSharedRef()
			];
	}
	else
	{
		MyScrollBox->AddSlot()
			[
				_ItemWidget.ToSharedRef()
			];
	}
	
}

void SScrollWrapBox::ClearChildren()
{
	MyScrollBox->ScrollToStart();
	if (bVerticalType)
		MyWrapBox->ClearChildren();
	else
		MyScrollBox->ClearChildren();
}

void SScrollWrapBox::RemoveSlot(const TSharedRef<SWidget>& WidgetToRemove)
{
	if (bVerticalType)
		MyWrapBox->RemoveSlot(WidgetToRemove);
	else
		MyScrollBox->RemoveSlot(WidgetToRemove);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

