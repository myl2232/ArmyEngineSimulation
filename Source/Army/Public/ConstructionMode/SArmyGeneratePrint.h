/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyConstructionMode.h
 * @Description 施工图绘制
 *
 * @Author HUAEN
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SScrollBox.h"
#include "SOverlay.h"
#include "Widgets/Layout/SGridPanel.h"

DECLARE_DELEGATE_OneParam(FOnGeneratePrintStart, const TArray<FName>&);

class SArmyGeneratePrint : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyGeneratePrint) {}

	SLATE_EVENT(FOnGeneratePrintStart, OnGeneratePrintStart)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
	void InitContent(const TArray<FName>& InLayerList);
private:
	FReply OnCommand(int32 InV);
	FSlateColor GetOkButtonColor() const;
	void OnCheckStateChanged(ECheckBoxState InCheck, FName InName);
	ECheckBoxState IsItemChecked(FName InName) const;
private:
	FOnGeneratePrintStart OnGeneratePrintStart;
	TSharedPtr<SGridPanel> LayerListBox;
	TArray<FName> SelectedLayerList;

	TArray<FName> AllLayerList;
};