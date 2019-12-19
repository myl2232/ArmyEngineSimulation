#pragma once
/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRBearingWallOperation.h
 * @Description 文本操作逻辑
 *
 * @Author 
 * @Date 2018年12月21日
 * @Version 1.0
 */
#include "ArmyOperation.h"

class FArmyTextLabelOperation :public FArmyOperation
{
public:
	FArmyTextLabelOperation(EModelType InBelongModel);
	~FArmyTextLabelOperation();

	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint)override;
private:
	TSharedPtr<class FArmyTextLabel> SelectTextLabel;
};