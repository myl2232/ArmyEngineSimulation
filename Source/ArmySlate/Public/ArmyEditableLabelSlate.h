// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Text/SlateEditableTextLayout.h"

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FOnInPutValueCommited, const FText&,const SWidget*);

class SArmyEditableText : public SEditableText
{
public:
    void Construct(const FArguments& InArgs);

	/**@欧石楠 处理OnKeyChar 实现只能输入数字*/
	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;

	void SetIfOnlyInteger(bool IfOnly);
	void SetIfOnlyFloat(bool IfOnly);

	bool HasTextChangedFromOriginal();

private:
	/** 正则检查是否是数字或者小数点*/
	bool CheckNumberIsVaild(const FString& str);

	bool bOnlyInteger;
	bool bOnlyFloat;
public:
	void Revert()
	{
		EditableTextLayout->RestoreOriginalText();
	}
};
class ARMYSLATE_API SSArmyEditableLabel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSArmyEditableLabel)
	{}
	SLATE_ATTRIBUTE(bool, CheckNumeric)
	/** @欧石楠 仅输入整数 */
	SLATE_ATTRIBUTE(bool,IsIntegerOnly)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	bool IsCharacterNumeric(const TCHAR InChar) const;

	void OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	void SetInputText(const FText& InText);

	void SetFocus();

	void Revert();

	bool HasTextChangedFromOriginal();

	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);

	FString GetText();
public:
	FOnInPutValueCommited OnInputBoxCommitted;
private:
	TSharedPtr<class SArmyEditableText> InputText;

	TAttribute<bool> bCheckNumeric = false;
};
