#pragma  once
#include "ArmyOperation.h"
class FArmyFreePolygonAreaOperation :public FArmyOperation
{
public:
	FArmyFreePolygonAreaOperation(EModelType InBelongModel);
	~FArmyFreePolygonAreaOperation() {}

	virtual void Init()override {}
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget)override;
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;



	virtual void ProcessClickInViewPortClient(FViewport* InViewPort, FKey Key, EInputEvent Event)override;
	void 	UpdateInputBoxInfo(XRArgument InArg, FObjectPtr Object, TSharedPtr<FArmyEditPoint> InOperationPoint, bool bModifyOperation, bool bVisibility, bool bMove);
	virtual void BeginOperation(XRArgument InArg = XRArgument());
	virtual void EndOperation() override;
	virtual void Tick() override;//@梁晓菲  拖拽修改位置
	virtual void SetCurrentEditSurface(TSharedPtr<class FArmyRoomSpaceArea> RoomSpaceArea) override;
	//@郭子阳
	//根据鼠标当前的位置设置父面
	void GetCurrentParentSurfaceByMouse();
	//@设置欲绘制面的父面，可能是绘制区域
	virtual void SetCurrentParentSurface(TSharedPtr<class FArmyBaseArea> BaseArea);
	virtual TArray< TSharedPtr<class FArmyLine> >& GetLines()override;
	virtual void StartTrackingDueToInput(const FArmyInputEventState& InPutState);//@梁晓菲 开始拖拽
	virtual void StopTracking();//@梁晓菲  结束拖拽
private:
	TArray<TSharedPtr<FArmyLine>> CaputerLines;
	void AddPolyArea();
	TArray<FVector> PolygonVertices;
	uint32 CurrentState = 0;
	FVector LastMousePos;
	FVector CurrentMousePos;
	TArray<FVector> TempPolygonVertices;
	TSharedPtr<class FArmyFreePolygonArea> SelectedArea;

	//当前附着的原始面
	TSharedPtr<class FArmyRoomSpaceArea> CurrentAttachArea;
	//当前选择的父面
	TSharedPtr<class FArmyBaseArea> CurrentParentArea;

	


	/* @梁晓菲 输入框*/
	TSharedPtr<class SArmyInputBox> InputBoxWidget;
	// @梁晓菲 设置输入框是否可见
	void SetInputVisibility(bool visibility);
	// @梁晓菲 输入确定
	void OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	// @梁晓菲 是否画鼠标点到上一个点的临时线
	bool BDrawTempLine;
	bool bShouldTick;
	bool IsOperating();
};