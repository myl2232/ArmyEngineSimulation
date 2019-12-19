#pragma once
#include "ArmyOperation.h"

class FArmyCircleAreaOperation :public FArmyOperation
{
public:
	FArmyCircleAreaOperation(EModelType InBelongModel);
	~FArmyCircleAreaOperation() {}
	//~ Begin FArmyController Interface
	virtual void Init()override {}
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget)override;
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)override;
	virtual void StartTrackingDueToInput(const FArmyInputEventState& InPutState);
	virtual void StopTracking();
	virtual void BeginOperation(XRArgument InArg = XRArgument());
	virtual void EndOperation() override;
	virtual void Tick() override;
	virtual void UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove) override;


	//~ End FArmyController Interface
	//设置输入框是否可见
	virtual void SetInputVisibility(bool visibility);
	//更新输入框位置
	void UpdateInputBoxPosition();


	virtual void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea) override;
	//@设置欲绘制面的父面，可能是绘制区域
	virtual void SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea);
	//@郭子阳
	//根据鼠标当前的位置设置父面
	void GetCurrentParentSurfaceByMouse();
protected:
	void WarnningInfo();
	void OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	bool IsOperating();
	void AddCircleArea();
	int32 CurrentState = 0; // 0 
	FVector LastMousePos;
	FVector CurrentMousePos;
	TSharedPtr<class FArmyLine> DashLine;
	TSharedPtr<class SArmyInputBox> InputBoxWidget;
	TSharedPtr<class FArmyCircleArea> TempCircleArea;
	TSharedPtr<class FArmyCircleArea> SelectedCircleArea;
	//当前附着的原始面
	TSharedPtr<class FArmyRoomSpaceArea> CurrentAttachArea;
	//当前选择的父面
	TSharedPtr<class FArmyBaseArea> CurrentParentArea;
};