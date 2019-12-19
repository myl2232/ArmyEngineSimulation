#pragma once
#include "ArmyOperation.h"

class FArmyFacsimileOperation : public FArmyOperation
{
public:
	FArmyFacsimileOperation(EModelType InBelongModel);
	virtual ~FArmyFacsimileOperation() {}

	//~ Begin IArmyDrawHelper Interface
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas) override;
	virtual void BeginOperation(XRArgument InArg /* = XRArgument() */) override;
	virtual void EndOperation() override;
	virtual void Tick() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;

	void ShowInputScaleWindow();

    /** 设置比例尺 */
    void SetFacsimileScale(int32 InScale);

private:
	int32 CurrentState;

	FVector2D FacsimileStartPos;
	FVector2D FacsimileEndPos;

    TSharedPtr<class SArmyEditableNumberBox> ENB_ScaleValue;
};