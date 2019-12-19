/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyDynamicFontSize.h
 * @Description 文本字体设置
 *
 * @Author HUAEN
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "Widgets/Input/SComboBox.h"
#include "ArmyObject.h"

class SArmyDynamicFontSize : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyDynamicFontSize) {}

	//SLATE_EVENT(FOnGeneratePrintStart, OnGeneratePrintStart)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

	void SetBoundObj(FObjectWeakPtr InObj);
private:
	TSharedRef<SWidget> GenerateFontSizeComboItem(TSharedPtr<int32> InItem);
	void HandleFontSizeComboChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo);
	FText GetFontSizeComboText() const;
private:
	FObjectWeakPtr BoundObj;
	TArray<TSharedPtr<int32>> TextSizeList;
};