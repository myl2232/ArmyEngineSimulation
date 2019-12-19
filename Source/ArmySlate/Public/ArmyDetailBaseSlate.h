/**
* Copyright 2019 北京北京伯睿科技有限公司.
* All Rights Reserved.
*  
*
* @File ArmyDetailBase.h
* @Description 属性面板内容基础框架
*
* @Author 欧石楠
* @Date 2018-06-13
* @Version 1.0
*/
#pragma once

#include "ArmyDetailBuilderSlate.h"

class FArmyObject;

class ARMYSLATE_API FArmyDetailBase : public TSharedFromThis<FArmyDetailBase> {

public:
	FArmyDetailBase();
	virtual ~FArmyDetailBase();

public:
	/** 创建属性widget*/
	virtual TSharedRef<SWidget> CreateDetailWidget();

	/** 显示选中的ArmyObject属性*/
	virtual void ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject);

	/** 显示选中的ArmyObject属性*/
	virtual void ShowSelectedDetial(TSharedPtr<FArmyObject> InSelectedObject, bool bHiddenSelected);

	/** 显示选中的UObject属性*/
	virtual void ShowSelectedDetial(UObject* InSelectedObject);

	/** 使用缓存的obj刷新属性面板*/
	virtual void RefreshSelectedDetial();
public:
	/** 获取DetailWidge*/
	TSharedRef<SWidget> GetDetailWidget();

	/** 设置DetailWidget显隐*/
	void HiddenDetailWidget(EVisibility State = EVisibility::Collapsed);

protected:
	/**计算选中的物体的尺寸*/
	FVector CalcActorSize(AActor* InActor);

	/**计算选中的物体的默认尺寸*/
	FVector CalcActorDefaultSize(AActor* InActor);

	// @欧石楠 为控件增加ToolTip，当控件不足以完全显示文字内容时，通过ToolTip来显示
	void AddToolTipForWidget(TSharedRef<SWidget> InWidget, const TAttribute<FText> &InToolTipText);
	
protected:
	/** 选中的UObject*/
	UObject* SelectedUObject = NULL;

	/** 选中的ArmyObject*/
	TSharedPtr<FArmyObject> SelectedArmyObject;

	/** 树状Detial*/
	TSharedPtr<FArmyDetailBuilder> DetailBuilder;

	/** DetailWidget*/
	TSharedPtr<SWidget> DetailWidget;
};