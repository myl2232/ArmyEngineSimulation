/**
* Copyright 2019 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File SArmyStaticLightingSettings.h
* @Description ����Ч��ͼUI
*
* @Author ������
* @Date 2019��2��28��
* @Version 1.0
*/

#pragma once

#include "AliyunOss.h"
#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ArmyTypes.h"
#include "SWidgetSwitcher.h"
#include "SlateStyle.h"
#include "IHttpRequest.h"
#include "SlateTypes.h"

class SArmyStaticLightingSettings : public SCompoundWidget {

	SLATE_BEGIN_ARGS(SArmyStaticLightingSettings) {}

	SLATE_EVENT(FInt32Delegate, OnGenerateClickedDelegate)
	SLATE_EVENT(FBoolDelegate, OnBuildFinishedDelegate)

	SLATE_END_ARGS()

	void Construct(const FArguments &InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	void InitializeSettings();

	enum EBuildingState
	{
		ENotStart,
		EProcessing,
		ESuccess,
		EFailed,
	}BuildingState;

	FInt32Delegate OnGenerateClickedDelegate;
	FBoolDelegate OnBuildFinishedDelegate;
private:
	//����
	TSharedRef<SWidget> CreateTempletWidget();
	TSharedRef<SWidget> CreateCustomalizeWidget();
	//�ؼ�
	//TSharedPtr<SBorder> QualityBorder0;
	TSharedPtr<SBorder> QualityBorder1;
	TSharedPtr<SBorder> QualityBorder2;

private:
	//��Ⱦ����
	int32 SelectedQualityIndex;
	
private:
	FReply OnQualityClicked(int32 QualityIndex);
	void OnQualityHovered(int32 QualityIndex);
	void OnQualityUnHoverd(int32 QualityIndex);
	void UnHoverAll();
	void CancelProcessingBuild();

	/** ����Ч��ͼ */
	FReply OnGenerateClicked();
	/** �ر� */
	FReply CancelClicked();

	//�Ҿ߲���決
	void OnBuildStaticMeshChanged(ECheckBoxState InNewState);
	ECheckBoxState GetBuildStaticMesh() const { return CachedBuildStaticMesh; }
	ECheckBoxState CachedBuildStaticMesh;
};