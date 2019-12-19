#pragma once
#include "ArmyOperation.h"
#include "Data/XRFurniture.h"
//#include "ArmyComponentTransform.h"
class  FArmyHydropowerComponentOperation :
	public FArmyOperation
{
public:
	FArmyHydropowerComponentOperation(EModelType InBelongModel);
	~FArmyHydropowerComponentOperation();

	/** 绘制 */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);

	/** 重置 */
	virtual void Reset();

	/** 更新状态 */
	virtual void Tick();

	/** 输入事件 */
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event);

	virtual void BeginOperation(XRArgument InArg = XRArgument());

	virtual void EndOperation();

	void SetCurrentItem (TSharedPtr<FContentItemSpace::FContentItem> item);

	void SetCurrentActor(class AXRFurnitureActor* InCurActor);

private:
	XRArgument	OperationArg;
	bool IsModifying = false;
	bool IsAdded = false;
	TSharedPtr<FArmyFurniture> SelectedComponent = NULL;
	bool IsSelectCaptruePoint = false;
	FVector MousePosition;

	FVector WallNormal = FVector::UpVector;
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem;
	class AXRFurnitureActor* CurrentActor;
};