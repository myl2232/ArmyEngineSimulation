/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File SArmyNewPlanWay.h
* @Description ѡ���½������ķ�ʽ����
*
* @Author ŷʯ�
* @Date 2018��7��2��
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