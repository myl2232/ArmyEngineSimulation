/**
* Copyright 2019 北京北京伯睿科技有限公司.
* All Rights Reserved.
*  
*
* @File XRSettingBarBuilder.h
* @Description 设置栏封装
*
* @Author 欧石楠
* @Date 2018-06-11
* @Version 1.0
*/
#pragma once
#include "CoreMinimal.h"
#include "SWidget.h"
#include "SCheckBox.h"
#include "ArmyStyle.h"

class ARMYSLATE_API FArmySettingBarBuilder {

public:
	FArmySettingBarBuilder();
	~FArmySettingBarBuilder();

public:
	TSharedRef<SWidget> CreateSettingBar();

	void AddItem(int32 CommandIndex, const FText &LocText, const FOnCheckStateChanged &OnStateChanged, ECheckBoxState State = ECheckBoxState::Unchecked);

	void AddItem(const TSharedRef<SWidget> &Widget);

	TSharedRef<SWidget> GetWidgetByIndex(int32 Index);

private:

	TMap<int32, TSharedPtr<SWidget>> CommandWidgetList;

	int32 WidgetCount;

};