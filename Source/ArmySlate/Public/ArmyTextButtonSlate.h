#pragma once

#include "Widgets/SCompoundWidget.h"

class ARMYSLATE_API SArmyTextButton : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyTextButton) {}

    SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)

    SLATE_ARGUMENT(FText, Text)

    SLATE_ARGUMENT(FMargin, TextPadding)

    SLATE_EVENT(FOnClicked, OnClicked)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    //virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    //virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    //virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    //virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
};