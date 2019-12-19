/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmySaveAsPlane.h
* @Description 另存方案界面
*
* @Author 欧石楠
* @Date 2018年6月25日
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SComboBox.h"
#include "Http.h"
#include "ArmyHttpModule.h"
#include "SArmySavePlan.h"
#include "SArmyComboboxItem.h"

class SArmySaveAsPlan : public SArmySavePlan
{
public:
	SLATE_BEGIN_ARGS(SArmySaveAsPlan) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

private:
	/** 创建选择项目界面 */
	TSharedRef<SWidget> CreateProjectWidget();	

	/** 初始化获取项目等*/
	void Init();

	void OnSlectedProjectChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo);

	/** 向后台数据请求项目列表信息 */
	void ReqGetProjectList();
	void ResGetProjectList(struct FArmyHttpResponse Response);

	/** 请求另存为方案 */
	virtual void ReqSavePlan() override;
	virtual void ResSavePlan(FArmyHttpResponse Response) override;

	TSharedRef<SWidget> OnGenerateComoboWidget(TSharedPtr<FArmyKeyValue> InItem);

private:
	int32 SelectedProjectID;	

	/** 项目数据列表 */
	FArmyComboBoxArray ProjectList;

	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> ProjectComboBox;

	/**@梁晓菲 UI列表*/
	TArray< TSharedPtr<SArmyComboboxItem> > ComboboxUIList;
};