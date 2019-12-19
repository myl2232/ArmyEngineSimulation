/**
 * Copyright 2019 北京伯睿科技有限公司..
 * All Rights Reserved.
 *  
 *
 * @File SArmyStringComboBox.h
 * @Description 文本下拉，用于循环生成大量下拉控件时
 *
 * @Author 欧石楠
 * @Date 2019年2月21日
 * @Version 1.0
 */
#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SComboBox.h"

DECLARE_DELEGATE(FUIStateChange);

class ARMYSLATE_API SArmyStringComboBox : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SArmyStringComboBox)
		: _Width(114)
		, _Height(24)
		, _DefaultItem("")
	{}

	/** 显示项数据源 */
	SLATE_ARGUMENT(TArray<TSharedPtr<FString>>, OptionsSource)

	SLATE_ARGUMENT(int32, Width)

	SLATE_ARGUMENT(int32, Height)	

	SLATE_EVENT(FUIStateChange, OnSelected)

	SLATE_ARGUMENT(FString, DefaultItem)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

public:
	FString GetSelectedItem();

	FText GetSelectedComboText() const;

	void SetSelectedItem(FString InItem);

private:
	TSharedRef<SWidget> GenerateComboItem(TSharedPtr<FString> InItem);

	void OnSelectionChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);

	FUIStateChange SelectionChangeEvent;
private:
	FString SelectedString;

	TSharedPtr< SComboBox< TSharedPtr<FString> > > ComboBoxWidget;

	/**存着选中内容的文本块*/
	TSharedPtr<STextBlock> ValueTextBlock;

	TArray<TSharedPtr<FString>> OptionsSource;
};