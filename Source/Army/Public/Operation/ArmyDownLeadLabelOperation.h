#pragma once
#include "ArmyOperation.h"

DECLARE_DELEGATE_OneParam(GetObjectArrayDelegate, TArray<FObjectWeakPtr>&)

class FArmyDownLeadLabelOperation : public FArmyOperation
{
public:
	FArmyDownLeadLabelOperation(EModelType InBelongModel);
	virtual ~FArmyDownLeadLabelOperation() {}

	//~ Begin IArmyDrawHelper Interface
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget = NULL);
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive) override;
	virtual FObjectPtr GetOperationObject() override;
	virtual TSharedPtr<FArmyEditPoint> GetOperationPoint() override;
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas);
	virtual void Tick() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	//~ End IArmyDrawHelper Interface
public:
	GetObjectArrayDelegate OnGetObjectArrayDelegate;
private:
	XRArgument CurrentArg;
	bool BeginStartEdit = false;

	TSharedPtr<SOverlay> ViewPortOverlay;

	TSharedPtr<class FArmyRectSelect> RectSelect;

	TSharedPtr<class FArmyDownLeadLabel> CurrentDownLeadLabel;

	int32 CurrentOperationIndex = 0;
	/** 操作点 */
	TSharedPtr<FArmyEditPoint> OperationPoint;
};