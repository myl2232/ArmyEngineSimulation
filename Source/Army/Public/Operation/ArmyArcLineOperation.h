#pragma once

#include "ArmyOperation.h"
#include "ArmyArcLine.h"
#include "SArmyInputBox.h"
#include "ArmyEditPoint.h"

/**
 * 线段绘制操作类
 */
class FArmyArcLineOperation : public FArmyOperation
{
public:
	FArmyArcLineOperation(EModelType InBelongModel);

	//~ Begin FArmyOperation Interface
	virtual void SetSelectedPrimitive(TSharedPtr<FArmyPrimitive> InPrimitive,TSharedPtr<FArmyEditPoint> InOperationPoint);

	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient,class FViewport* InViewPort,FKey Key,EInputEvent Event) override;

	/**	鼠标移动*/
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient,FViewport* ViewPort,int32 X,int32 Y) override;

	/** 状态开始 */
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;

	/** 状态结束 */
	virtual void EndOperation() override;

	virtual void Draw(FPrimitiveDrawInterface* PDI,const FSceneView* View) override;

protected:
	XRArgument CacheArg;

	TSharedPtr<FArmyArcLine> CurrentArcLine;

	TSharedPtr<FArmyEditPoint> CurrentOperationPoint;

private:
	/* @梁晓菲 保存捕捉状态*/
	uint32 MouseCaptureStatus;
};