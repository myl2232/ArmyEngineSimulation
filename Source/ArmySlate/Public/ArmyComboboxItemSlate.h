/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
*  
*
* @File SArmyComboboxItem.h
* @Description ComboboxItem��ʽ
*
* @Author ������
* @Date 2018��7��30��
* @Version 1.0
*/
#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "SImage.h"
#include "SlateWidgetStyleAsset.h"
#include "ArmyStyle.h"

class ARMYSLATE_API SArmyComboboxItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyComboboxItem)
		: _Height(32)
		, _ImagePadding(FMargin(16, 0, 8, 0))
		, _TextBlockPadding(FMargin(0))
        , _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
	{}

	SLATE_ARGUMENT(float, Height)

	SLATE_ATTRIBUTE(struct FSlateColor, TextNormalColor)

	SLATE_ATTRIBUTE(struct FSlateColor, TextSelectedColor)

	SLATE_ATTRIBUTE(class FText, Text)

	SLATE_ATTRIBUTE(FMargin, ImagePadding)

	SLATE_ATTRIBUTE(FMargin, TextBlockPadding)

    SLATE_STYLE_ARGUMENT(struct FTextBlockStyle, TextStyle)

	SLATE_ATTRIBUTE(const FSlateBrush*, NormalImage)

	SLATE_ATTRIBUTE(const FSlateBrush*, SelectedImage)





	//@ 水平排列方式
	SLATE_ATTRIBUTE(EHorizontalAlignment, ItemHorizontalAlignment)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	//virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	//virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	void SetTextColorAndImageBrushNormal();
    void SetTextColorAndImageBrushSelected();

	FString GetText();

private:
	TAttribute<FSlateColor> TextNormalColor;
	TAttribute<FSlateColor> TextSelectedColor;
	TAttribute< const FSlateBrush* > NormalImage;
	TAttribute< const FSlateBrush* > SelectedImage;
	TAttribute<FText> Text;
	float Height;

	TSharedPtr<SImage> Image;
	TSharedPtr<class STextBlock> TextBlock;

	TAttribute<FMargin> ImagePadding;
	TAttribute<FMargin> TextBlockPadding;
};