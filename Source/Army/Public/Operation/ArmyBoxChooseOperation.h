/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRBoxChooseOperation.h
* @Description 框选操作器
*
* @Author 刘俊良
* @Date 2019年1月8日
* @Version 1.0
*/

#pragma once

#include "ArmyOperation.h"


class FArmyBoxChooseOperation : public FArmyOperation
{
public:
	FArmyBoxChooseOperation(EModelType InBelongModel) :FArmyOperation(InBelongModel) {};
	~FArmyBoxChooseOperation() {};

	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint) override;

private:
	//计算鼠标拖拽规则
	FVector FArmyBoxChooseOperation::CalSelectOperation(const FObjectPtr& ObjectIt, const FVector OutOffset) const;

private:
	TArray<FObjectPtr> SelectedObjList;

private:
	FVector FontFramePos    = FVector::ZeroVector;//前一帧位置
	FVector CurrentFramePos = FVector::ZeroVector;//当前帧位置
	bool FlagStartDrag = true;//判断是否是开始拖拽
};
