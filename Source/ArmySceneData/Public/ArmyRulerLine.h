/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRRulerLine.h
* @Description 标尺线封装
*
* @Author 欧石楠
* @Date 2018年8月9日
* @Version 1.0
*/

#pragma once
#include "ArmyObject.h"
#include "SArmyInputBox.h"

class ARMYSCENEDATA_API FArmyRulerLine {
public:
	FArmyRulerLine();
	~FArmyRulerLine();

	/**更新标尺线，输入标尺线的起点终点和方向offset*/
	void Update(FVector Begin, FVector End, FVector LeftOffset = FVector::ZeroVector, FVector RightOffset = FVector::ZeroVector,bool bIgnoreZ = true);

	void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	void ShowInputBox(bool bShow = true);

	void SetOnTextCommittedDelegate(FOnTextCommitted InDelegate) {
		InputBoxWidget->OnInputBoxCommitted = InDelegate;
	}

	void SetOnTextForThisCommittedDelegate(FOnTextForThisCommitted InDelegate) {
		InputBoxWidget->OnInputBoxForThisCommitted = InDelegate;
	}

	float GetInputBoxCachedLength() { return InputBoxWidget->GetCachedLength(); }	

	void SetInputBoxPosUseLRLine(bool bUse) { bUseLRLine = bUse; }

	void SetInputBoxFocus(bool bFocus) { InputBoxWidget->SetFocus(bFocus); }

	void SetEnableInputBox(bool bEnable) { if(InputBoxWidget->IsEnabled() != bEnable) InputBoxWidget->SetEnabled(bEnable); }

	FVector GetUpDashLineStart() { return UpDashLine->GetStart(); }

	FVector GetUpDashLineEnd() { return UpDashLine->GetEnd(); }

	FVector GetUpDashLineNormal() 
	{
		return (UpDashLine->GetStart() - UpDashLine->GetEnd()).GetSafeNormal();
	}

	FVector GetDashLineUpDir() { return DashLineUpDir; }

	TSharedPtr<SArmyInputBox> GetWidget() { return InputBoxWidget; }

	bool GetUpdateShowInDraw() { return bUpdateShowInDraw; }
	void SetUpdateShowInDraw(bool InValue) { bUpdateShowInDraw = InValue; }

private:
	TSharedPtr<SArmyInputBox> InputBoxWidget;

	bool bUseLRLine = false;

	bool bUpdateShowInDraw = true;

	FVector DashLineUpDir = FVector::ZeroVector;

	TSharedPtr<FArmyLine> LeftDashLine;
	TSharedPtr<FArmyLine> RightDashLine;
	TSharedPtr<FArmyLine> UpDashLine;

	FOnTextCommitted Delegate_OnTextCommitted;
};