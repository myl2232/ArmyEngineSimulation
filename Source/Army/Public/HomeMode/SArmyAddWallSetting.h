/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File SArmyAddWallSetting.h
* @Description ��������ǽʱ��ѡ��
*
* @Author ŷʯ�
* @Date 2018��09��26��
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "SCheckBox.h"

class SArmyAddWallSetting : public SCompoundWidget {

public:
	SLATE_BEGIN_ARGS(SArmyAddWallSetting) 
	: _bNeedHeight(false)
	{}
	SLATE_ARGUMENT(bool, bNeedHeight)
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

	FText GetThicknessValue() const { return FText::FromString(FString::Printf(TEXT("%d"), CachedThicknessValue)); }

	FText GetHeightValue() const { return FText::FromString(FString::Printf(TEXT("%d"), CachedHeightValue)); }

	void SetCatchThicknessValue(int InValue) { CachedThicknessValue = InValue; }
	int GetCachedThicknessValue() { return CachedThicknessValue; }

	void SetCatchHeightValue(int InValue) { CachedHeightValue = InValue; }
	int GetCachedHeightValue() { return CachedHeightValue; }

	bool GetIfCheck() { return CheckBox->GetCheckedState() == ECheckBoxState::Checked; }

public:
	void OnConfirmClicked();

private:
	void OnThicknessValueChanged(const FText& InText, ETextCommit::Type CommitType);

	void OnHeightValueChanged(const FText& InText, ETextCommit::Type CommitType);

private:
	bool bNeedHeight = false;

	int CachedThicknessValue = 120;
	int CachedHeightValue = 2800;
	const int MinValue = 10;
	const int MaxValue = 450;

	TSharedPtr<class SArmyEditableNumberBox> ThicknessBox;
	TSharedPtr<class SArmyEditableNumberBox> HeightBox;
	TSharedPtr<class SCheckBox> CheckBox;
	TSharedPtr<class SBox> HeightContainBox;
};