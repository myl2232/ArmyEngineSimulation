/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File SArmyProjectList.h
 * @Description 分页功能UI
 *
 * @Author 
 * @Date 2019年2月19日
 * @Version 1.0
 */

#pragma once

#include "Widgets/SCompoundWidget.h"

class SArmyPageItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyPageItem) {}
	SLATE_ARGUMENT(FString, TextContentStr)
	SLATE_EVENT(FStringDelegate, OnItemClicked)
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
public:
	//设置或得到当前项是否处于选择状态
	bool GetbSelect() const { return bSelect; }
	void SetbSelect(bool InbSelect) { bSelect = InbSelect; }

	//设置或得到当前项是否处于激活状态
	bool GetbActive() const { return bActive; }
	void SetbActive(bool InbActive) { bActive = InbActive; }

	//设置或获取当前项显示文本内容
	FString GetTextContentStr() const { return TextContentStr; }
	void SetTextContentStr(FString InTextContentStr) { TextContentStr = InTextContentStr; }
private:
	FSlateColor GetTextColor() const;
	FReply OnClick();
	FText GetTextContent()const { return FText::FromString(TextContentStr); }
private:
	TSharedPtr<class STextBlock> TextContent;
	FString TextContentStr;//当前项显示的文本

	FStringDelegate OnItemClicked;

	//是否处于激活状态
	bool bActive = false;

	//被选择
	bool bSelect = false;
};

class SArmyPageQuery : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyPageQuery) {}
	//SLATE_ARGUMENT(int32, EachPageCount)
	//SLATE_ARGUMENT(int32, DataCount)
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);

	//更新所有页码状态
	void UpDatePage(int32 InPlanCount);

	//每页显示多少数据
	const static int32 EachPageShowNum;
	//页码按钮最大数量
	const static int32 MaxPageItemNum;
	//省略号
	const static FString Ellipsis;
private:
	/**
	 * 初始化按钮数量和各分页按钮的状态
	 */
	void Init();

	void GenerateUI();

	void OnItemClicked(const FString & InText);

	//激活指定页
	void ActivePageByContent(const FString & InText);
	void ActivePageByContent(const int32 & InKey);

	//得到当前激活的Key
	int32 GetCurrentActiveKey();
private:
	void AddPageItem(TSharedPtr<class SWidget> WidItem);
	TSharedPtr<class SHorizontalBox> PageHorizontalBox;

	////分页按钮最大数量 ，暂未实现，留待以后扩展
	//int32 MaxBtnNum = 8;

	//每页显示多少数据
	//int32 EachPageCount;

	//数据总量
	int32 DataCount;

	//总页数
	int32 PageCount;

	//当前页码
	FString CurrentPage = TEXT("");

	//所有分页按钮集合
	TMap<int32, TSharedPtr<SArmyPageItem> > MapPageItems;

};