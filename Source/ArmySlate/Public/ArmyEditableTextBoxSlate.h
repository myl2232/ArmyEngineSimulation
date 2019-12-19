#pragma once

#include "SImage.h"
#include "SlateDelegates.h"
#include "Widgets/SCompoundWidget.h"

class ARMYSLATE_API SArmyEditableTextBox : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyEditableTextBox)
        : _IconImage(nullptr)
        , _IsPassword(false)
        , _MaxTextLength(0)
    {}

    SLATE_ARGUMENT(TSharedPtr<SImage>, IconImage)

    SLATE_ARGUMENT(FText, HintText)

    SLATE_ARGUMENT(bool, IsPassword)

    /** 最大输入文本长度，0默认不做限制 */
    SLATE_ARGUMENT(int32, MaxTextLength)

    SLATE_EVENT(FOnTextChanged, OnTextChanged)

	SLATE_EVENT(FOnTextCommitted, OnTextCommitted)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	FText GetText();
    void SetText(const FText& InText);

private:
	TSharedPtr<SImage> IconImage;
	TSharedPtr<class SEditableTextBox> ETB;

    int32 MaxTextLength;

    FOnTextChanged OnTextChanged;
};

class ARMYSLATE_API SArmyPropertyEditableTextBox : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyPropertyEditableTextBox)
        : _IsReadOnly(false)
    {}

    SLATE_ATTRIBUTE(FText, Text)

    SLATE_ATTRIBUTE(bool, IsReadOnly)

    SLATE_EVENT(FOnTextChanged, OnTextChanged)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
};