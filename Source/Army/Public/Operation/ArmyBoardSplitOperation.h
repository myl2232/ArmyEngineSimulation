#pragma once

#include "ArmyOperation.h"

class FArmyBoardSplitline;

class FArmyBoardSplitOperation :public FArmyOperation
{
public:
    FArmyBoardSplitOperation(EModelType InBelongModel):FArmyOperation(InBelongModel){}
	virtual ~FArmyBoardSplitOperation() {}

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument())override;
	virtual void EndOperation() override;
    virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;

    TSharedPtr<FArmyBoardSplitline> Exec_AddBoardSplitLine(const FVector& Point);
private:
	TSharedPtr<FArmyBoardSplitline> CurrentBoardSplitline;
};