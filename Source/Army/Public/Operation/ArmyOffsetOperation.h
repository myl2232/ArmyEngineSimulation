#pragma once

#include "ArmyOperation.h"
#include "SArmyInputBox.h"

class FArmyOffsetOperation : public FArmyOperation
{
public:
	FArmyOffsetOperation(EModelType InBelongModel);
	virtual ~FArmyOffsetOperation() {}

	//~ Begin IArmyDrawHelper Interface
	virtual void Init() override;
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual void Tick() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	//~ End IArmyDrawHelper Interface

	void Exec_ApplyOffset(const FVector& Offset);

private:
	void OperationLine();
	void OperationArcWall();
	void OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void OnInputBoxKeyDown(const FKeyEvent& InKeyEvent);

	bool IsOperating();

    /** 设置输入框的位置 */
    void SetInputBoxPos(const FVector& InPos);

	void UpdataInputBoxPosition(const FVector& InStartPos, const FVector& InEndPos);

private:
	TSharedPtr<SArmyInputBox> InputBoxWidget;

	TSharedPtr<FArmyLine> SelectedLine;

	//TSharedPtr<FArmyArcWall> SelectedArcWall;
	FVector FirstSelectedArcWallPoint;
	//TSharedPtr<FArmyArcWall> HelpArcWall;
	TSharedPtr<FArmyLine> SolidLine;
	TSharedPtr<FArmyLine> DashLine;

    /** 偏移值 */
    float OffsetValue;

    /** 偏移值是否已设置 */
    bool bOffsetSetted;

	bool bClickedEnter = false;
};