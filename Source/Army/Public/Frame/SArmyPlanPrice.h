/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyPlanPrice.h
* @Description 方案估价窗口
*
* @Author 郭子阳
* @Date 2019年5月31日
* @Version 1.0
*/


#pragma once
#include "Widgets/SCompoundWidget.h"

struct FArmyPlanPrice;

class SArmyPlanPriceItem : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyPlanPriceItem) {}
	
	/** 标题 */
	SLATE_ATTRIBUTE(FText, TypeName)

    //价格
   // SLATE_ATTRIBUTE(float, Price)

    //项
	//SLATE_ATTRIBUTE(int32, Count)

	SLATE_END_ARGS()

public:
	void Construct(const FArguments & InArgs);

	//void SetTypeName(FText newTypeName);
	void SetCount(int32 newCount);
	void SetPrice(float newPrice);
protected:
	//FText TypeName;
	//项
	int32 Count;
	//价格
	float Price;

	TSharedPtr<STextBlock> PriceText;
	TSharedPtr<STextBlock> CountText;
};

class SArmyPlanPrice : public SCompoundWidget 
{
public:
	SLATE_BEGIN_ARGS(SArmyPlanPrice){}
	SLATE_END_ARGS()

public:

	void Construct(const FArguments & InArgs);

	void SetDataSource(TSharedPtr<FArmyPlanPrice> Data);

	FSimpleDelegate OnShowDetail;

	void Show();
private:
	//设置总价
	void SetTotalPrice(float inPrice);

	//刷新界面
	void Update();

	TSharedPtr<SRichTextBlock> TotalPrice;
	TArray<TSharedPtr<SArmyPlanPriceItem>> Items;
	TSharedPtr<FArmyPlanPrice> DataSource;

	TSharedPtr<SCheckBox> OnlyGoodsConstructionCheckBox;

	void OnOnlyGoodsConstructionChecked(ECheckBoxState NewState);

};

