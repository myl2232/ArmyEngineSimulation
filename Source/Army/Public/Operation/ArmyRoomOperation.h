#pragma once

#include "ArmyOperation.h"
#include "ArmyLine.h"
#include "ArmyRoom.h"
#include "SArmyInputBox.h"
#include "ArmyEditPoint.h"

/**
 * 线段绘制操作类
 */
class FArmyRoomOperation : public FArmyOperation
{
public:
	FArmyRoomOperation(EModelType InBelongModel);

	//~ Begin FArmyOperation Interface
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget);

	virtual void SetSelectedPrimitive(TSharedPtr<FArmyPrimitive> InPrimitive, TSharedPtr<FArmyEditPoint> InOperationPoint);

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;

	/**	鼠标移动*/
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;

	/** 更新状态 */
	virtual void Tick() override {};

	/** 状态开始 */
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;

	/** 状态结束 */
	virtual void EndOperation() override;

	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive) override;

	virtual FObjectPtr GetOperationObject() override;

	virtual TSharedPtr<FArmyEditPoint> GetOperationPoint() override;

	TSharedPtr<FArmyEditPoint> GetPrePoint() const;

	void AddLinePoint(const FVector& InPoint, TSharedPtr<FArmyEditPoint> CapturePoint = nullptr);
private:
	void Update(UArmyEditorViewportClient* InViewPortClient);

	void InitControlLines();

	void OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	/**@欧石楠 输入数据驱动直线位置*/
	void OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

    /** 判断是否能够移动线段，判断条件为：当前房间不能与外墙相交 */
    bool CanMoveWallLine(const FVector& InNewStart, const FVector& InNewEnd);
    bool CanMoveInnerWallLine(const FVector& InNewStart, const FVector& InNewEnd);
    bool CanMoveOuterWallLine(const FVector& InNewStart, const FVector& InNewEnd);

    /**
     * @欧石楠 移动线段的操作点
     * @param InNewPos - const FVector & - 新的位置
     * @param bUserInput - bool - 是否是用户手动输入的位置
     */
    void MoveEditPoint(const FVector& InNewPos, bool bUserInput);

protected:

	TSharedPtr<SArmyInputBox> InputBoxWidget;
	
	TSharedPtr<FArmyLine> SolidLine;
	TSharedPtr<FArmyLine> UpLeftDashLine;
	TSharedPtr<FArmyLine> UpRightDashLine;
	TSharedPtr<FArmyLine> UpDashLine;
	
	TSharedPtr<FArmyLine> ExtendUpLeftDashLine;
	TSharedPtr<FArmyLine> ExtendUpRightDashLine;
	TSharedPtr<FArmyLine> ExtendUpDashLine;

    /** 实时操作的虚线 */
	TSharedPtr<FArmyEditPoint> CurrentOperationPoint;
	TSharedPtr<FArmyLine> CurrentOperationLine;

	XRArgument CurrentArg;

	/**@欧石楠拖拽线段时的标尺线*/
	TSharedPtr<class FArmyRulerLine> RulerLine;

	TWeakPtr<FArmyRoom> CurrentEditRoom;

	TSharedPtr<FArmyEditPoint> PrePoint;
	TSharedPtr<FArmyEditPoint> CurrentPoint;

	bool bShouldClick = false;
	bool bShouldDragPrimitive = false;
	FVector DragPrePoint;
};