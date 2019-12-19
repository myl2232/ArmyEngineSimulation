/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File SArmyWallSetting.h
* @Description ǽ�����ý���
*
* @Author ŷʯ�
* @Date 2018��6��22��
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SArmyWallSetting : public SCompoundWidget {

public:
	SLATE_BEGIN_ARGS(SArmyWallSetting) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

public:
	float GetWallHeight() const { return WallHeight; }
	
	void OnConfirmClicked();
private:
	void Init();

	void OnWallHeightChanged(float InValue) { WallHeight = InValue; }

	TSharedRef<SWidget> CreateWallHeightWidget();

	TSharedRef<SWidget> CreateWallColorWidget();

private:

	float WallHeight;
	
};