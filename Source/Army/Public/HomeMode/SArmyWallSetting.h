/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyWallSetting.h
* @Description 墙体设置界面
*
* @Author 欧石楠
* @Date 2018年6月22日
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