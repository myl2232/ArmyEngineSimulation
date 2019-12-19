#pragma once
#include "ArmyOperation.h"

class FArmyRegularPolygonAreaOperation :public FArmyOperation
{
public:
	FArmyRegularPolygonAreaOperation(EModelType InBelongModel);
	~FArmyRegularPolygonAreaOperation() {}

	//~ Begin FArmyController Interface
	virtual void Init()override;
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget);
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
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
	virtual void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea) override;

	//@设置欲绘制面的父面，可能是绘制区域
	virtual void SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea);
	//@郭子阳
	//根据鼠标当前的位置设置父面
	void GetCurrentParentSurfaceByMouse();
protected:
	//当前附着的原始面
	TSharedPtr<class FArmyRoomSpaceArea> CurrentAttachArea;
	//当前选择的父面
	TSharedPtr<class FArmyBaseArea> CurrentParentArea;

	void WarnningInfo();
	void OnInputBoxKeyDown(const FKeyEvent& InKeyEvent);
	void OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void OnInputBoxCommitted1(const FText& InText, const ETextCommit::Type InTextAction);
	void UpdateInputboxPosition();
	TSharedPtr<class SHorizontalBox> InPutBox;
	TSharedPtr<class SArmyInputBox> InputBoxWidget;
	TSharedPtr<class SArmyInputBox> InputBoxWidget1;
	bool IsOperating();
	void AddRegularPolygonArea();
	void ModifyArea();
	bool UseOrthoMode = false;
	TSharedPtr<FArmyEditPoint> OperationPoint;
	//TSharedPtr<class FArmyLine> OperationLine;
	int32 CurrentState = 0; // 0 
	FVector LastMousePos;
	FVector CurrentMousePos;
	TSharedPtr<class FArmyRegularPolygonArea> TempRegularArea;
	TSharedPtr<class FArmyRegularPolygonArea> SelectedRegularPolyArea;
	TSharedPtr<class FArmyLine> DashLine;
};