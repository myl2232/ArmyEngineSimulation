/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyHardModeContentBrowser.h
 * @Description 水电模式自动布线错误弹框
 *
 * @Author 朱同宽
 * @Date 2018年6月25日
 * @Version 1.0
 */

#pragma once

#include "ArmyDetailBase.h"
#include "STextBlockImageSpinbox.h"

//智能布线结果信息
struct FAutoDesignResultInfo : TSharedFromThis<FAutoDesignResultInfo>
{
	bool bSuccessed; //是否成功
	FString LoopName;//回路名字
	FString ResultInfo;//结果信息
	FAutoDesignResultInfo(bool InSuccessed,FString InLoopName,FString InResultInfo) :
		bSuccessed(InSuccessed),
		LoopName(InLoopName),
		ResultInfo(InResultInfo)
	{

	}
};

//智能布线提示错误界面
class SArmyHydropowerAutoDesignError : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS (SArmyHydropowerAutoDesignError)
	{
	}

	SLATE_ATTRIBUTE(FText,Title)
	SLATE_ATTRIBUTE(FName,TitleIcon)
	SLATE_ATTRIBUTE(FText,Content)
	SLATE_ATTRIBUTE(FName,ContentIcon)

	SLATE_END_ARGS ()
public:

	void Construct (const FArguments& InArgs);
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent) override;

};
//智能布线结果界面
class SArmyHydropowerAutoDesignFinish : public SCompoundWidget
{
public:
	typedef SListView< TSharedPtr<FAutoDesignResultInfo> > ErrorList;
	SLATE_BEGIN_ARGS (SArmyHydropowerAutoDesignFinish)
	{
	}
	SLATE_ARGUMENT(float,MaxListHeight)
		SLATE_ARGUMENT(const TArray< TSharedPtr<FAutoDesignResultInfo> >*,OptionsSource)
		SLATE_EVENT(FOnClicked,OnClicked)
		SLATE_END_ARGS()
public:

	void Construct(const FArguments& InArgs);
	TSharedRef<ITableRow> GenerateMenuItemRow(TSharedPtr<FAutoDesignResultInfo> InItem,const TSharedRef<STableViewBase>& OwnerTable);
private:
	TSharedPtr<ErrorList> ListView;
};
