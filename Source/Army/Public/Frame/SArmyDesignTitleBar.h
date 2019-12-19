/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyDesignTitleBar.h
 * @Description 设计界面标题栏控件
 *
 * @Author 欧石楠
 * @Date 2018年6月7日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "MultiBoxBuilder.h"
#include "SComboBox.h"
#include "SArmyComboboxItem.h"

class FArmyDesignEditor;

class SArmyDesignTitleBar : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SArmyDesignTitleBar)
        : _OwnerEditorPtr(nullptr)
    {}

    SLATE_ARGUMENT(TWeakPtr<FArmyDesignEditor>, OwnerEditorPtr)

    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);

    ~SArmyDesignTitleBar();

    /** 初始化设计模式列表 */
    void InitDesignModeList();

    void OnDesignModeChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo);
private:
    TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FString> InItem);
    FText GetSelectedDesignMode() const;
    const FSlateBrush* GetSelectedDesignModeBrush() const;

    TSharedPtr< SComboBox< TSharedPtr<FString> > > MakeDesignModeComboBox();

private:
    FString SelectedDesignMode;
    TArray< TSharedPtr<FString> > ModeNameArray;
    TSharedPtr<SBox> Box_DesignModeList;
    TSharedPtr< SComboBox< TSharedPtr<FString> > > CB_DesignModeList;

    TWeakPtr<FArmyDesignEditor> OwnerEditorPtr;

	/**@梁晓菲 UI列表*/
	TArray< TSharedPtr<SArmyComboboxItem> > ComboboxUIList;
};