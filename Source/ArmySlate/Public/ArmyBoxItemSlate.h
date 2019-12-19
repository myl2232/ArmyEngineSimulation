#pragma once

#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "ArmyStyle.h"

/**
 * UI标准单元
 */
class ARMYSLATE_API  SBoxItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBoxItem)
        : _ItemWidth(168)
        , _ItemHeight(168)
        , _BackgroundColor(FArmyStyle::Get().GetColor("Color.FF383A3D"))
		, _ContentImageHeight(64)
		, _ContentImageItemWidth(64)
		, _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
	{}

    /** 用来唯一标识该单元的一个序号，通常为数组索引 */
	SLATE_ARGUMENT(int32, ItemCode)

    /** 决定item的样式，只hover图片区域还是hover整个item */
    SLATE_ARGUMENT(bool, bHoverThumbnailOnly)

	/** item宽度，决定了图片尺寸 */
	SLATE_ARGUMENT(float, ItemWidth)


	/** 内嵌图片的高度 */
	SLATE_ARGUMENT(float, ContentImageHeight)

	/** 背景色 */
	SLATE_ARGUMENT(FSlateColor, BackgroundColor)

	/** 内嵌图片的宽度 */
	SLATE_ARGUMENT(float, ContentImageItemWidth)

    /** Item高度 */
    SLATE_ARGUMENT(float, ItemHeight)

	/** 文字样式 */
	SLATE_ARGUMENT(const FTextBlockStyle*, TextStyle)
 
    /** 内嵌图片内容 */
	SLATE_ARGUMENT(TSharedPtr<class SImage>, ContentImage)

	/** 文字信息*/
	SLATE_ARGUMENT(FText, DesInfoText)

	SLATE_EVENT(FSimpleDelegate, OnBoxClickedDelegate)

	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);

	FReply OnBoxClicked();
private:
	/** 内嵌图片Box*/
	TSharedPtr<class SBox> Box_ContentImageContainer;

	/** 内嵌背景板*/
	TSharedPtr<class SBorder> Border_ContentSBorder;

	/** 内嵌框勾选代理*/
	FSimpleDelegate OnBoxClickedDelegate;

	/** 已选择标识图片*/
	TSharedPtr<SImage> IsCheckedImage;

	
	void OnBoxHovered();
	void OnBoxUnhovered();


	/** 内嵌图片*/
	TSharedPtr<SImage> ContentImage;

public:

	bool IsChoosed;

};