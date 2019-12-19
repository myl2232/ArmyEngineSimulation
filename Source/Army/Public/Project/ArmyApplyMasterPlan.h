#pragma once
/**
 * Copyright 2019 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRAutoDesignPoint.h
 * @Description 应用大师方案
 *
 * @Author 王志超
 * @Date 2019年3月13日
 * @Version 1.0
 */
#pragma once
#include "CoreMinimal.h"

class XR_API FArmyApplyMasterPlan : public TSharedFromThis<FArmyApplyMasterPlan>
{
public:
	FArmyApplyMasterPlan();
	~FArmyApplyMasterPlan() {}

	//应用大师方案
	bool ApplyMasterPlan();

	//预览大师方案
	void PreviewMasterPlan();
private:
	

public:

	void OnApplyWidgetClick(int32 inN);

	void OnPreviewWidgetClick(int32 inN);



public:
	//应用大师方案界面
	TSharedPtr<class SArmyApplyMasterPlan> ApplyMasterPlanWidget;
	//预览大师方案界面
	TSharedPtr<class SArmyPreviewMasterPlan> PreviewMasterPlanWidget;
};