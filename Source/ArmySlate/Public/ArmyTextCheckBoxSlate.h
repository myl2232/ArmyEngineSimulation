#pragma once

#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"
#include "ArmyTypes.h"
#include "SCheckBox.h"

class ARMYSLATE_API SArmyTextCheckBox : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyTextCheckBox)
        : _Width(0)
        , _Height(0)
        , _InnerPadding(FMargin(0))
		, _CheckBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
        , _TextColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
        , _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		,_OnCheckStateChanged()
		,_IsChecked(ECheckBoxState::Unchecked)
    {}

    /** 宽度 */
    SLATE_ARGUMENT(float, Width)

    /** 高度 */
    SLATE_ARGUMENT(float, Height)

    /** 内边距 */
    SLATE_ARGUMENT(FMargin, InnerPadding)

    /** 按钮样式 */
    SLATE_STYLE_ARGUMENT(FCheckBoxStyle, CheckBoxStyle)

    /** 文字 */
    SLATE_ARGUMENT(FText, NormalText)
	
	/** 文字 */
	SLATE_ARGUMENT(FText,CheckText)

    /** 文字颜色 */
    SLATE_ARGUMENT(FLinearColor, TextColor)

    /** 文字样式 */
    SLATE_ARGUMENT(const FTextBlockStyle*, TextStyle)

	/** Called when the checked state has changed */
	SLATE_EVENT(FOnCheckStateChanged,OnCheckStateChanged)

	/** Whether the check box is currently in a checked state */
	SLATE_ATTRIBUTE(ECheckBoxState,IsChecked)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	void SetCheckBoxStyle(const FCheckBoxStyle& InCheckBoxStyle);

	/**@欧石楠 样式，设置背景颜色*/
	void SetBorderColor(const FSlateColor& InBorderBackgroundColor);

    /** 设置字体颜色 */
    void SetTextColor(const FLinearColor& TextColor);

	ECheckBoxState IsChecked() const;

	void OnCheckStateChanged(ECheckBoxState InCheck);

	FText Text()const;

private:

	/**@欧石楠 背景*/
	TSharedPtr<SBorder> Border;

    TSharedPtr<SCheckBox> CheckBox;
    TSharedPtr<STextBlock> TB_Text;
	
	FText NormalText;
	FText CheckText;

    int32 ButtonCode;
public:
	/** Are we checked */
	TAttribute<ECheckBoxState> IsCheckboxChecked;

	///** Delegate called when the check box changes state */
	FOnCheckStateChanged CheckStateChanged;
};