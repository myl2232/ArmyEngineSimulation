/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File SArmyGenerateOutWall.h
* @Description ������ǽʱ����ʾ
*
* @Author ŷʯ�
* @Date 2018��09��04��
* @Version 1.0
*/
#pragma once


#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SArmyGenerateOutWall : public SCompoundWidget {

public:
	SLATE_BEGIN_ARGS(SArmyGenerateOutWall) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);
	
	FText GetValue() const { return FText::FromString(FString::Printf(TEXT("%d"), CachedValue)); }

	int GetCachedValue() { return CachedValue; }

public:
	void OnConfirmClicked();

private:
	void OnValueChanged(const FText& InText, ETextCommit::Type CommitType);		
private:
	int CachedValue = 240;
	const int MinValue = 10;
	const int MaxValue = 1000;

	TSharedPtr<class SArmyEditableNumberBox> EditableNumberBox;
};