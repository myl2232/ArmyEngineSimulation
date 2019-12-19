#pragma once


#include "ArmyOperation.h"

enum class EDragType
{
	NoDrag, //无拖拽
	DragArea, //拖拽整个面
	DragEdge, //拖拽边界

};


class FArmyRectAreaOperation :public FArmyOperation
{
public:
	FArmyRectAreaOperation(EModelType InBelongModel);
	~FArmyRectAreaOperation() {}

	//~ Begin FArmyController Interface
	virtual void Init()override;
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget);
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)override;
	virtual void StartTrackingDueToInput(const FArmyInputEventState& InPutState);
	virtual void StopTracking();
	virtual void BeginOperation(XRArgument InArg = XRArgument());
	virtual void EndOperation() override;
	virtual void Tick() override;

	//~ End FArmyController Interface
	virtual void UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove) override;

	//设置输入框是否可见
	virtual void SetInputVisibility(bool visibility);

	//@设置当前正在编辑的墙面
	virtual void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea) override;

	//@设置欲绘制面的父面，可能是绘制区域
	virtual void SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea) ;

	virtual void SelectRectAreaBoundrary(const FVector2D& InMousePos, TSharedPtr<FArmyObject> rect);

	class AXRShapeActor* HelpHightLightArea = NULL;
protected:
	void DrawHelpRulerAxis(FPrimitiveDrawInterface* PDI, const FSceneView* View, FLinearColor color ,float thikness);
	void UpdateHelpRulerAxisPos();
	void AddRectArea();
	void ModifyRectArea();

	void RefreshBoundaryDriveEvent();
	void DisableHightLightArea();

	//@郭子阳
	//根据鼠标当前的位置设置父面
	void GetCurrentParentSurfaceByMouse();

private:
	void OnRectDrawFinished();

	void WarnningInfo();
	void UpdateInputBoxPosition();
	void UpdateBoxWidget(TSharedPtr<class SArmyInputBox> InputBox, FVector start, FVector end, bool IsHorizontal);
	void OnInputBoxKeyDown(const FKeyEvent& InKeyEvent);
	void OnUpWidthInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void OnLeftHeightInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);

	TSharedPtr<class SArmyInputBox> UpWidthInputBoxWidget;
	TSharedPtr<class SArmyInputBox> LeftHeightInputBoxWidget;
	TSharedPtr<class SArmyInputBox> DownWidthInputBoxWidget;
	TSharedPtr<class SArmyInputBox> RightHeightInputBoxWidget;

	FVector LastMousePos;
	FVector CurrentMousePos;
	bool DrawBoundary = false;
	FVector HelpDrawLineStart = FVector::ZeroVector;
	FVector HelpDrawLineEnd = FVector::ZeroVector;
	struct HelpDrawRulerAxis
	{
		HelpDrawRulerAxis()
		{
			StartPos = EndPos = FVector::ZeroVector;
		}
		FVector StartPos;
		FVector EndPos;
	};
private:

	//当前选择的墙面
	TSharedPtr<class FArmyRoomSpaceArea> CurrentAttachArea;
	
	//@郭子阳
	//绘制区域的父面
	TSharedPtr<class FArmyBaseArea> CurrentParentArea;

	//@郭子阳 
	//拖拽类型
	EDragType DragType= EDragType::NoDrag;

	bool IsOperating();
	TSharedPtr<class FArmyEditPoint> OperationPoint;
	TSharedPtr<class FArmyRectArea> SelectedRectArea;
	
	TSharedPtr<class FArmyRectArea> TempRectArea;

	TSharedPtr<class FArmyRect> RectArea;
	int32 CurrentState = 0;

	HelpDrawRulerAxis UpRulerAxis;
	HelpDrawRulerAxis LeftRulerAxis;
	HelpDrawRulerAxis RightRulerAxis;
	HelpDrawRulerAxis DownRulerAxis;

	FVector PlaneXDir = FVector(1, 0, 0);
	FVector PlaneYDir = FVector(0, 1, 0);
	FVector PlaneNormal = FVector(0, 0, 1);
	FVector PlaneOrginPos = FVector(0, 0, 0);

	//@郭子阳 是否是当前操作
	bool IsActive = false;

};

