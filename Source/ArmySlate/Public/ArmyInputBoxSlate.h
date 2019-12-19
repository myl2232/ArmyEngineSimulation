#pragma once

#include "SlateDelegates.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

#include "ArmyTypes.h"
#include "SlateDelegates.h"

DECLARE_DELEGATE_ThreeParams(FOnTextForThisCommitted, const FText&, ETextCommit::Type, TSharedPtr<class SWidget>);

class ARMYSLATE_API SArmyInputBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyInputBox) {}

	SLATE_EVENT(FOnTextCommitted, OnInputBoxCommitted)

	SLATE_EVENT(FOnTextForThisCommitted, OnInputBoxForThisCommitted)

	SLATE_EVENT(FInputKeyDelegate, OnKeyDown)

	SLATE_EVENT(FPointerEventHandler, OnMouseButtonUp)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** 是否显示 */
	void Show(bool bShow);

	/** 是否聚焦 */
	void SetFocus(bool bFocus);

	void SetPos(const FVector2D& Pos);

	void SetTransform(const FTransform2D& trans);

	void SetInputText(const FText& InText);

	/**
	 * 设置起点和终点，输入框的位置会居中，并且更新线段长度
	 * @return 是否显示输入框，如果线段长度为0则不显示
	 */
	bool SetStartAndEnd(const FVector& Start, const FVector& End,bool bIgnoreZ = true);

	/**@欧石楠 获取cached length*/
	float GetCachedLength() { return CachedLength; }

	/**@欧石楠 用cachedlength 刷新 inputbox的text*/
	void RefreshWithCachedLength();

	/**@刘克祥*/

	void Revert();

protected:
	/** 只允许输入数字 */
	bool IsCharacterNumeric(const TCHAR InChar) const;


	void OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	void OnTextCommittedForThis(const FText& InText, const ETextCommit::Type InTextAction, TSharedPtr<SWidget> InThisWidget);

public:
	FOnTextCommitted OnInputBoxCommitted;

	FOnTextForThisCommitted OnInputBoxForThisCommitted;

	FInputKeyDelegate OnKeyDownCallback;

protected:
	TSharedPtr<class SArmyEditableText> InputText;

	TSharedPtr<SWidget> LastFocusedWidget;

	float CachedLength;
};