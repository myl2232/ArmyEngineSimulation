/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SArmyStaticLightingSettings.h
* @Description 生成效果图UI
*
* @Author 马云龙
* @Date 2019年2月28日
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
	//界面
	TSharedRef<SWidget> CreateTempletWidget();
	TSharedRef<SWidget> CreateCustomalizeWidget();
	//控件
	//TSharedPtr<SBorder> QualityBorder0;
	TSharedPtr<SBorder> QualityBorder1;
	TSharedPtr<SBorder> QualityBorder2;

private:
	//渲染质量
	int32 SelectedQualityIndex;
	
private:
	FReply OnQualityClicked(int32 QualityIndex);
	void OnQualityHovered(int32 QualityIndex);
	void OnQualityUnHoverd(int32 QualityIndex);
	void UnHoverAll();
	void CancelProcessingBuild();

	/** 生成效果图 */
	FReply OnGenerateClicked();
	/** 关闭 */
	FReply CancelClicked();

	//家具参与烘焙
	void OnBuildStaticMeshChanged(ECheckBoxState InNewState);
	ECheckBoxState GetBuildStaticMesh() const { return CachedBuildStaticMesh; }
	ECheckBoxState CachedBuildStaticMesh;
};