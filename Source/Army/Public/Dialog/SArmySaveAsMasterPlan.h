

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SComboBox.h"
#include "Http.h"
#include "ArmyHttpModule.h"
#include "SArmySavePlan.h"
#include "SArmyComboboxItem.h"

class SArmySaveAsMasterPlan : public SArmySavePlan
{
public:
public:
	SLATE_BEGIN_ARGS(SArmySaveAsMasterPlan) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

private:
	/** 创建选择项目界面 */
	TSharedRef<SWidget> CreateProjectWidget();

	/**创建输入户型面积界面*/
	TSharedRef<SWidget> CreatePlanAreaWidget();

	/**创建方案介绍界面*/
	TSharedRef<SWidget> CreatePlanInfoWidget();

	/**创建方案名称界面*/
	TSharedRef<SWidget> CreateMasterPlanNameWidget();

	/**创建选择房型界面*/
	TSharedRef<SWidget> CreateMasterHouseTypeWidget();

	/** 初始化获取项目等*/
	void Init();

	void OnSlectedStyleChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo);
	void OnSlectedAreaChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo);

	/** 向后台数据请求项目列表信息 */
	void ReqGetAreaList();
	void ResGetAreaList(struct FArmyHttpResponse Response);
	void ReqGetStyleList();
	void ResGetStyleList(struct FArmyHttpResponse Response);

	/** 请求另存为大师方案 */
	virtual void ReqSavePlan() override;
	virtual void ResSavePlan(FArmyHttpResponse Response) override;

	FText GetCurrentStyle() const { return FText::FromString(MasterPlanStyle); }
	FText GetCurrentArea() const { return FText::FromString(MasterPlanArea); }

	/** 设置详细描述信息*/
	void SetMasterDescText(const FText& InText);

	/* 计算面积所属ID*/
	int32 CalAreaRangeID(double HouseArea);

private:
	int32 SelectedProjectID;

	/** 风格列表 */
	FArmyComboBoxArray StyleList;

	/**	面积列表 */
	FArmyComboBoxArray AreaList;

	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> StyleComboBox;
	TSharedPtr<SComboBox< TSharedPtr<FArmyKeyValue> >> AreaComboBox;

	/** UI列表*/
	TArray< TSharedPtr<SArmyComboboxItem> > ComboboxUIStyleList;
	TArray< TSharedPtr<SArmyComboboxItem> > ComboboxUIAreaList;

	/* 下拉框*/
	TSharedRef<SWidget> OnGenerateComoboStyleWidget(TSharedPtr<FArmyKeyValue> InItem);
	TSharedRef<SWidget> OnGenerateComoboAreaWidget(TSharedPtr<FArmyKeyValue> InItem);

	/* 选择的风格和面积*/
	FString MasterPlanStyle;
	FString MasterPlanArea;

	/** 选择的风格和面积ID*/
	int32 MasterPlanStyleID;
	int32 MasterPlanAreaID;

	/** 智能方案描述*/
	FString IntelPlanDescription;

	/** 面积文本信息*/
	TSharedPtr<class STextBlock> AreaTextInfo;
	////

	TSharedPtr<class SMultiLineEditableTextBox> METB_PlanName;

	//面积范围ID
	int32 MasterPlanAreaRangeID;
};