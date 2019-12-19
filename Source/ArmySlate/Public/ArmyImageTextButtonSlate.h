#pragma once

#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"
#include "ArmyTypes.h"
#include "SBorder.h"
#include "STextBlock.h"
#include "DeclarativeSyntaxSupport.h"
#include "SButton.h"

enum EArmyLayoutOrientation
{
    LO_Horizontal,
    LO_Vertical,
};

class ARMYSLATE_API SArmyImageTextButton : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyImageTextButton)
        : _LayoutOrientation(LO_Vertical)
        , _Width(0)
        , _Height(0)
        , _InnerPadding(FMargin(0))
		, _ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF2A2B2E"))
        , _Image(nullptr)
        , _TextColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
        , _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
    {}

    /** 文字和图片的布局方向 */
    SLATE_ARGUMENT(EArmyLayoutOrientation, LayoutOrientation)

    /** 宽度 */
    SLATE_ARGUMENT(float, Width)

    /** 高度 */
    SLATE_ARGUMENT(float, Height)

    /** 内边距 */
    SLATE_ARGUMENT(FMargin, InnerPadding)

    /** 按钮样式 */
    SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)

    /** 图片 */
    SLATE_ARGUMENT(const FSlateBrush*, Image)

    /** 文字 */
    SLATE_ARGUMENT(FText, Text)

    /** 文字颜色 */
    SLATE_ARGUMENT(FLinearColor, TextColor)

    /** 文字样式 */
    SLATE_ARGUMENT(const FTextBlockStyle*, TextStyle)

    /** 按下事件 */
    SLATE_EVENT(FOnClicked, OnClicked)

    /** 高亮事件 */
    SLATE_EVENT(FSimpleDelegate, OnHovered)

    /** 取消高亮事件 */
    SLATE_EVENT(FSimpleDelegate, OnUnhovered)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	void SetButtonStyle(const FButtonStyle& InButtonStyle);

	/**@欧石楠 样式，设置背景颜色*/
	void SetBorderColor(const FSlateColor& InBorderBackgroundColor);

    /** 设置字体颜色 */
    void SetTextColor(const FLinearColor& TextColor);

    /** 设置图片 */
    void SetImage(const FSlateBrush* InImage);

private:

	/**@欧石楠 背景*/
	TSharedPtr<SBorder> Border;

    TSharedPtr<SButton> Button;
    TSharedPtr<class SImage> Image;
    TSharedPtr<STextBlock> TB_Text;

    /** 是否显示图片/文字 */
    bool bDisplayImage = false;
    bool bDisplayText = false;

    int32 ButtonCode;
};