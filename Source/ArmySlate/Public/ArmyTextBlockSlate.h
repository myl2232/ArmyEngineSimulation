#pragma once

#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"

class ARMYSLATE_API SArmyTextBlock : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyTextBlock)
        : _bShowUnderline(false)
        , _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
        , _ColorAndOpacity(FLinearColor::White)
        , _HighlightColor(FArmyStyle::Get().GetColor("Color.FFFD9800"))
		, _UnderlineNormalImage(FArmyStyle::Get().GetBrush("Splitter.White"))
    {}

    /** 是否显示下划线 */
    SLATE_ARGUMENT(bool, bShowUnderline)

    SLATE_ATTRIBUTE(FText, Text)

    SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)

    SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)

    SLATE_ATTRIBUTE(FLinearColor, HighlightColor)

	SLATE_ATTRIBUTE(const FSlateBrush*, UnderlineNormalImage)

    SLATE_EVENT(FSimpleDelegate, OnClicked)

    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

protected:
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

    virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

protected:
    TSharedPtr<class STextBlock> TextBlock;

    TSharedPtr<class SImage> UnderlineImage;

	TAttribute< const FSlateBrush* > UnderlineNormalImage;

    FSimpleDelegate OnClicked;

	TAttribute<FSlateColor> NormalColor;
};