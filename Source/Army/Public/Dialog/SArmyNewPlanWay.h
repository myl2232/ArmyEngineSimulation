/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyNewPlanWay.h
* @Description 选择新建方案的方式界面
*
* @Author 欧石楠
* @Date 2018年7月2日
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyStyle.h"

class SArmyNewPlanWay : public SCompoundWidget {

public:
	SLATE_BEGIN_ARGS(SArmyNewPlanWay) {}
	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);

private:	

	void OnNewClicked();

	void OnImportClicked();
};