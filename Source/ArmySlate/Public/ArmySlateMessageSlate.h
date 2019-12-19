#pragma once
#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"

class SArmySlateMessage : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SArmySlateMessage)
		: _AlignCenter(false)
		, _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
	{}
		SLATE_ARGUMENT(bool, AlignCenter)
		SLATE_ARGUMENT(bool, Rolling)
		SLATE_ARGUMENT(bool, Confirm)
		SLATE_ARGUMENT(bool, Cancel)
		SLATE_ATTRIBUTE(FText, MessageText)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void SetMessage(FText _Message) { MessageText.Set(_Message); }

	FSimpleDelegate ConfirmDelegate;
	FSimpleDelegate CancelDelegate;

private:
	FText GetMessageText() const { return MessageText.Get(); }
	EVisibility GetRollingVisibility() const { return RollingVisibility; }
	EVisibility GetConfirmVisibility() const { return ConfirmVisibility; }
	EVisibility GetCancelVisibility() const { return CancelVisibility; }
	FReply OnConfirmClicked();
	FReply OnCancelClicked();

    TSharedPtr<class SImage> Img_Throbble;
    int32 ImgRotDegree;

	TAttribute<FText> MessageText;
	EVisibility RollingVisibility;
	EVisibility ConfirmVisibility;
	EVisibility CancelVisibility;

	//注册弹出窗口为不断更新的Widget
	TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;
	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
};