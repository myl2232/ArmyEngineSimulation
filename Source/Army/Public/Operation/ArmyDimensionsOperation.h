#pragma once

#include "ArmyOperation.h"

class FArmyDimensions;

class FArmyDimensionsOperation :public FArmyOperation
{
public:
    FArmyDimensionsOperation(EModelType InBelongModel):FArmyOperation(InBelongModel),CurrentOperation(-1){}
	virtual ~FArmyDimensionsOperation() {}

	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument())override;
	virtual void EndOperation() override;
	virtual void Tick() override;
    virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;

    TSharedPtr<FArmyDimensions> Exec_AddDimensions(const FVector& Point);
	TSharedPtr<FArmyDimensions> Exec_AddSeriesDimensions(TSharedPtr<FArmyDimensions> PreDimensions);
    //void Exec_DeleteDimensions(const FVector& Point);

private:
	int32 CurrentOperation;
	EObjectType CurrentType = OT_Dimensions;
	TSharedPtr<FArmyDimensions> CurrentDimensions;
};