/**
 * Copyright 2019 ������Ƽ����޹�˾..
 * All Rights Reserved.
 *  
 *
 * @File SArmyStringComboBox.h
 * @Description �ı�����������ѭ�����ɴ��������ؼ�ʱ
 *
 * @Author ŷʯ�
 * @Date 2019��2��21��
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

	/** ��ʾ������Դ */
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

	/**����ѡ�����ݵ��ı���*/
	TSharedPtr<STextBlock> ValueTextBlock;

	TArray<TSharedPtr<FString>> OptionsSource;
};