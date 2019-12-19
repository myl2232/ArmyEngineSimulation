/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyNewPlan.h
* @Description 新建方案界面
*
* @Author 欧石楠
* @Date 2018年6月21日
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SComboBox.h"
#include "Http.h"

class SArmyNewPlan : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SArmyNewPlan) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

	bool CheckValid();

	void OnConfirmClicked();

private:
	/**创建方案界面*/
	TSharedRef<SWidget> CreatePlanWidget();
	TSharedRef<SWidget> CreateHouseTypeWidget();
    TSharedRef<SWidget> CreateAreaWidget();
	
	/**提交新建方案*/
	void ReqNewPlan();
	void ResNewPlan(struct FArmyHttpResponse Response);

private:
	TSharedPtr<class SArmyEditableNumberBox> ETB_Bedroom;
	TSharedPtr<class SArmyEditableNumberBox> ETB_LivingRoom;
	TSharedPtr<class SArmyEditableNumberBox> ETB_Kitchen;
	TSharedPtr<class SArmyEditableNumberBox> ETB_Bathroom;
	TSharedPtr<class SArmyEditableTextBox> ETB_Name;
    TSharedPtr<SArmyEditableNumberBox> ETB_Area;
	TSharedPtr< SComboBox< TSharedPtr<FArmyKeyValue> > > ProjectComboBox;
	/** 错误信息文本 */
	TSharedPtr<class STextBlock> TB_ErrorMessage;
};