/**
* Copyright 2019 北京北京伯睿科技有限公司.
* All Rights Reserved.
*  
*
* @File SArmyDetailComponent.h
* @Description 常用属性slate控件
*
* @Author 欧石楠
* @Date 2018-06-15
* @Version 1.0
*/
#pragma once
#include "SCompoundWidget.h"
#include "NumericTypeInterface.h"
#include "SSpinBox.h"
#include "SComboBox.h"
#include "SScrollBox.h"

DECLARE_DELEGATE_OneParam(FOnDelegateColor, const FLinearColor&);

/** Button with text*/
class ARMYSLATE_API SArmyDetailButton : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SArmyDetailButton)
		: _Text(FText::FromString(TEXT("按钮")))
		, _ButtonStyle(&FCoreStyle::Get().GetWidgetStyle< FButtonStyle >("Button"))		
	{}
	SLATE_ARGUMENT(FText, Text)
	SLATE_STYLE_ARGUMENT(FButtonStyle, ButtonStyle)
		/*SLATE_ATTRIBUTE(FSlateColor, ButtonColorAndOpacity)
		SLATE_ATTRIBUTE(FSlateColor, TextColorAndOpacity)*/
	SLATE_EVENT(FOnClicked, OnClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

};

/** Laber - Button can call collor picker*/
class SArmyDetailColorButton : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SArmyDetailColorButton)
		: _Text(FText::FromString(TEXT("颜色")))
	{}

	SLATE_ATTRIBUTE(FLinearColor, Color)
	SLATE_ARGUMENT(FText, Text)
	SLATE_EVENT(FOnDelegateColor, OnColorChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	//用于动态修改颜色按钮的颜色
	void SetBorderColor(const FLinearColor& color);

private:
	FReply OnColorClicked();
	void OnColorChanged(const FLinearColor _Color);
	FSlateColor GetBorderColor() const { return FSlateColor(BorderColor.Get()); }

private:
	//用于动态修改颜色按钮的颜色	
	TAttribute<FLinearColor> BorderColor;
	FOnDelegateColor OnColorChangedDelegate;
};

/** EditableTextBox*/
class SArmyDetailEditableTextBox : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SArmyDetailEditableTextBox)
		: _Text(FText::FromString(TEXT("属性")))
		, _EditableText(FText::FromString(TEXT("可编辑文字")))
	{}
	SLATE_ARGUMENT(FText, Text)
	SLATE_ATTRIBUTE(FText, EditableText)
	SLATE_EVENT(FOnTextChanged, OnTextChanged)
	SLATE_EVENT(FOnTextCommitted, OnTextCommitted)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

};

//左边文字 右边SpinBox
class SArmyDetailInputValue : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SArmyDetailInputValue)
		: _Delta(0)
		, _MinValue(TNumericLimits<float>::Lowest())
		, _MaxValue(TNumericLimits<float>::Max())
		, _MinSliderValue(0)
		, _MaxSliderValue(100)
		, _Text(FText::FromString(TEXT("属性")))
		, _FlagColor(FLinearColor::White)
	{}
	SLATE_ARGUMENT(FText, Text)

	SLATE_ARGUMENT(FSlateColor, FlagColor)
	SLATE_ATTRIBUTE(TOptional<float>, Value)
	SLATE_ATTRIBUTE(float, Delta)
	SLATE_ATTRIBUTE(TOptional<float>, MinValue)
	SLATE_ATTRIBUTE(TOptional<float>, MaxValue)
	SLATE_ATTRIBUTE(TOptional<float>, MinSliderValue)
	SLATE_ATTRIBUTE(TOptional<float>, MaxSliderValue)
	SLATE_ARGUMENT(bool, AllowSpin)
	SLATE_EVENT(FOnFloatValueChanged, OnValueChanged)
	SLATE_EVENT(FOnFloatValueCommitted, OnValueCommitted)
	SLATE_ARGUMENT(TSharedPtr< INumericTypeInterface<float> >, TypeInterface)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TAttribute< TOptional<float> > ValueAttribute;
	TSharedPtr< SSpinBox<float> > MySpinBox;

	float OnGetValueForSpinBox() const
	{
		const auto& Value = ValueAttribute.Get();
		if (Value.IsSet() == true)
			return Value.GetValue();
		return 0;
	}
};

class SArmyDetailCheckBox : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SArmyDetailCheckBox)
		: _IsChecked(ECheckBoxState::Checked)
		, _Text(FText::FromString(TEXT("属性")))
	{}
	SLATE_ARGUMENT(FText, Text)
	SLATE_ATTRIBUTE(ECheckBoxState, IsChecked)
	SLATE_EVENT(FOnCheckStateChanged, OnCheckStateChanged)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TSharedPtr<SCheckBox> MyCheckBox;
};


class ARMYSLATE_API SArmyDetailWidget : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SArmyDetailWidget) {}
	SLATE_ARGUMENT(FText, TitleText)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void AddDetailItem(TSharedRef<SWidget> _ItemWidget);

	TSharedPtr<class SScrollBox> Container;
	TSharedPtr<class SVerticalBox> Container1;
};