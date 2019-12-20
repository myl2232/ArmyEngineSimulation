/**
 * Copyright 2019 北京伯睿科技有限公司..
 * All Rights Reserved.
 *  
 *
 * @File SArmyMulitCategory.h
 * @Description 多个分类勾选框控件，用于施工项
 *
 * @Author 欧石楠
 * @Date 2019年1月16日
 * @Version 1.0
 */

#pragma once

#include "ArmyDetailBuilderSlate.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmyStringComboBoxSlate.h"


class ARMYSLATE_API SArmyMulitCategory : public TSharedFromThis<SArmyMulitCategory>{

public:
	SArmyMulitCategory();
	virtual ~SArmyMulitCategory();

public:
	/** 使用一份数据初始化多级分类勾选框，且分空间显示，-1默认与空间无关联*/
	void Init(TSharedPtr<FJsonObject> InJsonData, int InSpaceID = -1);

	/** 使用多份数据初始化多级分类勾选框，且分空间显示，-1默认与空间无关联*/
	void Init(TArray<TSharedPtr<FJsonObject>> InJsonData, int InSpaceID = -1);

	/** 清空多级分类勾选框*/
	void Clear();

	/** 设置是否使用框架自动赋值施工项数据*/
	void SetAutoAssignValue(bool bInValue) { bAutoAssignValue = bInValue; }

public:
	/** 获取DetailWidge*/
	TSharedPtr<SWidget> GetDetailWidget();

	/** 获取DetailNode*/
	TArray<TSharedPtr<FArmyDetailNode>> GetDetailNodes();

	/** 传入某对象的施工项接口对象，即可刷新属性面板该显示的数据*/
	void RefreshCheckData(TSharedPtr<FArmyConstructionItemInterface> InObj);	

	/** 获取UI框架中选中的数据*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>>& GetCheckedData();

	/** 刷新结构树*/
	void RefreshItemExpand(TSharedPtr<STreeView<TSharedPtr<FArmyDetailNode>>> TreeView);

	//解析数据,返回默认勾选数据
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>> ParseData(TSharedPtr<FJsonObject> InJsonData);

	//施工项内容变动代理，有任何变动都触发
	FUIStateChange ConstructionStateChangeDelegate;
private:
	void OnCheckBoxStateChanged(ECheckBoxState InNewState);

	void OnListChanged();

	void GenerateUI(int InSpaceID);

protected:

	TSharedPtr<FArmyConstructionItemInterface> TempConstructionItemObj;

	/** 缓存下默认勾选数据*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>> CachedDefaultCheckedData;

	/** 顶层 树状Detial*/
	TSharedPtr<FArmyDetailBuilder> DetailBuilders;

	//三个不同栏的builder
	TSharedPtr<FArmyDetailBuilder> NullSpaceDetailBuilders;
	TSharedPtr<FArmyDetailBuilder> CurrentDetailBuilders;
	TSharedPtr<FArmyDetailBuilder> OtherSpaceDetailBuilders;

	//三个不同栏的widget
	TSharedPtr<SWidget> NullSpaceDetailWidget;
	TSharedPtr<SWidget> CurrentSpaceDetailWidget;
	TSharedPtr<SWidget> OtherSpaceDetailWidget;

	TMap<int32/*施工项ID*/, TArray<int32>/*施工项的可用空间ID列表*/> TM_ConstructionAvaliableSpaceID;

	/**根据施工项ID 存储勾选框*/
	TMap<int32/*施工项ID*/, TSharedPtr<class SCheckBox>> TM_CheckBoxList;

	/**根据施工项下的属性 存储下拉框*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, TSharedPtr<class SArmyStringComboBox>>> TM_ComboBoxList;

	/** 施工品类ID下，存在的施工项ID*/
	TMap<int32/*施工品类ID*/, TArray<int32>/*施工项*/> TM_ConstructionItemData;

	/** 施工项ID对应的施工项名称*/
	TMap<int32/*施工项ID*/, FString/*施工项名称*/> TM_ConstructionItemName;

	/** 施工项ID下，对应的属性ID的属性值ID及属性值数据*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, TMap<int32/*属性值ID*/, FString/*属性值*/>>> TM_ConstructionPropertyData;

	/** 施工项下对应的施工项的属性ID和对应的属性名称*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, FString/*属性名称*/>> TM_Property;

	/** 施工项的属性ID和对应的全部属性值 -- 用于下拉框数据源*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, TArray<TSharedPtr<FString>>>> TM_PropertyValue;

	/**存储下被选中的施工项及下拉选中的数据*/
	TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>> TM_CheckedID;

	/**施工项ID对应的节点数据*/
	TMap<int32, TSharedPtr<FArmyDetailNode>> TM_NodesData;

	/**存储所有的Nodes*/
	TArray<TSharedPtr<FArmyDetailNode>> DetailNodes;

	/**存储所有的顶级Nodes*/
	TArray<TSharedPtr<FArmyDetailNode>> AllTopDetailNodes;

	/** 第一层分类 施工项*/
	TSharedPtr<FArmyDetailBuilder> FirstLevelDetailBuilder;

	/** DetailWidget*/
	TSharedPtr<SWidget> DetailWidget;

	/** 缓存下TreeView用于折叠菜单*/
	TSharedPtr<STreeView<TSharedPtr<FArmyDetailNode>>> CachedTreeView;

	/** 标记是否使用自动赋值施工项数据*/
	bool bAutoAssignValue = true;
};