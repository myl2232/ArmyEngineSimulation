#pragma once

#include "ArmyOperation.h"
#include "SArmyInputBox.h"
#include "ArmyRect.h"

class FArmyBaseRectOperation :public FArmyOperation
{
public:
	FArmyBaseRectOperation(EModelType InBelongModel);

	virtual ~FArmyBaseRectOperation(){}
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument());
	virtual void EndOperation() override;
	virtual void Tick() override;

	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;

protected:
	void UpateInputBoxPosition();
	void UpdateBoxWidget(TSharedPtr<SArmyInputBox> InputBox, FVector start, FVector end);
	void OnInputBoxKeyDown(const FKeyEvent& InKeyEvent);
	void OnWidthInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void OnHeightInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<SArmyInputBox> WidthInputBoxWidget;
	TSharedPtr<SArmyInputBox> HeightInputBoxWidget;
	TSharedPtr<class FArmyRect> RectArea;
	uint32 CurrentState = -1;
};