#pragma once

#include "ArmyObject.h"
#include "ArmyTypes.h"
#include "ArmyViewportClient.h"

class FArmyOperation
{
public:
	FArmyOperation(EModelType InBelongModel);
	virtual ~FArmyOperation() {}

	/** 初始化 */
	virtual void Init() {}

	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget = NULL) {}

	/** 将选中的物体填充进来作为编辑数据 */
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint) {}

	/** 将选中的图形 */
	virtual void SetSelectedPrimitive(TSharedPtr<FArmyPrimitive> InPrimitive, TSharedPtr<FArmyEditPoint> InOperationPoint) {}

	virtual FObjectPtr GetOperationObject() { return NULL; }

	virtual TSharedPtr<FArmyEditPoint> GetOperationPoint() { return NULL; }

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) {}

	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas) {}

	/** 重置 */
	virtual void BeginOperation(XRArgument InArg = XRArgument()) {}
	virtual void EndOperation() { EndOperationDelegate.ExecuteIfBound(); }

	virtual bool CanExit() { return true; }

	/** 更新状态 */
	virtual void Tick() {}
	virtual void TickMouseDelta(FVector mouseDelat) {}
	/** 输入事件 */
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) { return false; }

	/** 鼠标单击事件，所有的鼠标左键/右键/中键单击事件都在该方法中实现 */
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) {}

	virtual void ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event) {}

	virtual void  StartTrackingDueToInput(const FArmyInputEventState& InPutState) {}

	virtual void StopTracking() {}

	/** 移动物体 */
	virtual void ApplyTransform(FObjectPtr Object, const FTransform& Trans) { Object->ApplyTransform(Trans); }

	virtual TArray< TSharedPtr<class FArmyLine> >& GetLines() { return DoNotUse; }

	virtual void MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) {}

	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) {}

	//设置输入框是否可见
	virtual void SetInputVisibility(bool visibility) {}
	//更新图形大小输入框信息（包括位置、可见性）
	virtual void UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove) {}
	virtual void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea) {}
	/**
	 * 根据输入的点和输入的面，将点所在面的相对XY坐标转化为世界坐标
	 * @param InputPosition - FVector - 输入的点
	 * @param InputaseArea - TSharedPtr<FArmyBaseArea> - 点所在的平面
	 * @return FVector - 转化后的世界坐标
	 */
	FVector GetWorldFromLocal(FVector InputPosition, TSharedPtr<class FArmyBaseArea> InputaseArea);

	static FVector GetIntersectPointWithPlane(const FVector2D& InMousePos, TSharedPtr<class FArmyBaseArea> CurrentAttachedArea);
	//设置操作方式，true为修改操作，否则不修改
	void SetModifyOperating(bool operating) { ModifyOpera = operating; }
	bool GetModifyOperating() { return ModifyOpera; }
	void SetMoveOperating(bool MoveOperating) { bMoveOperation = MoveOperating; }
	bool GetMoveOperating() { return bMoveOperation; }
	void SethitTime(int32 _hitTime) { hitTime = _hitTime; }
	int32 GethitTime() { return hitTime; }

protected:
    /** 标志当前鼠标是否移动位置 */
    bool ShouldTick();

public:
	FSimpleDelegate EndOperationDelegate;

protected:
	EModelType BelongModel;//所属模式

private:
	TArray< TSharedPtr<FArmyLine> > DoNotUse;
	//是否为修改大小的操作，true为修改，圆形修改半径、矩形修改长宽
	bool ModifyOpera;
	bool bMoveOperation;

protected:
	TSharedPtr<class FArmyLine> OperationLine;
	//
	bool ModifyOperationState;
	int32 ModifyOperationTime;
	int32 hitTime;

    FVector2D ViewportSize;

    FVector2D LastViewportMousePos;

    FVector2D ViewportMousePos;
};