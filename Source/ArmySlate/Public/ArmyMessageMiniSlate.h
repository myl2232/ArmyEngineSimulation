#pragma once
#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"

class SArmyMessageMini : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SArmyMessageMini)
		: _TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		, _bError(false)
		, _bRolling(true)
		, _bCancel(true)
		/*, _MessageText(FText::FromString("Message!"))*/
	{}
		SLATE_ARGUMENT(bool, bError)
		SLATE_ARGUMENT(bool, bRolling)
		SLATE_ARGUMENT(bool, bCancel)
		SLATE_ATTRIBUTE(FText, MessageText)
		SLATE_EVENT(FSimpleDelegate, CancelDelegate)
		SLATE_STYLE_ARGUMENT(FTextBlockStyle, TextStyle)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void SetMessage(FText _Message) { MessageText.Set(_Message); }
	void SetErrorVisibility(EVisibility InVisibility) { ErrorVisibility = InVisibility; }
	void SetRollingVisibility(EVisibility InVisibility) { RollingVisibility = InVisibility; }
	void SetCancelVisibility(EVisibility InVisibility) { CancelVisibility = InVisibility; }

private:
	FSimpleDelegate CancelDelegate;

private:
	FText GetMessageText() const { return MessageText.Get(); }
	EVisibility GetErrorVisibility() const { return ErrorVisibility; }
	EVisibility GetRollingVisibility() const { return RollingVisibility; }
	EVisibility GetCancelVisibility() const { return CancelVisibility; }
	FReply OnCancelClicked();

    TSharedPtr<class SImage> Img_Throbble;
    int32 ImgRotDegree;

	TAttribute<FText> MessageText;
	EVisibility ErrorVisibility;
	EVisibility RollingVisibility;
	EVisibility CancelVisibility;

	//注册弹出窗口为不断更新的Widget
	TWeakPtr<FActiveTimerHandle> ActiveTimerHandle;
	EActiveTimerReturnType ActiveTick(double InCurrentTime, float InDeltaTime);
};