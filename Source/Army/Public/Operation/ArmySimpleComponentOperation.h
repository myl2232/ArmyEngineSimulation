/**
 * Copyright 2018 北京伯睿科技有限公司.
 * All Rights Reserved.
 * 
 *
 * @File XRSimpleComponentOperation.h
 * @Description 简单结构控件操作
 *
 * @Author 欧石楠
 * @Date 2018年10月12日
 * @Version 1.0
 */
#pragma once

#include "ArmyOperation.h"

class FArmySimpleComponentOperation : public FArmyOperation
{
public:
	FArmySimpleComponentOperation(EModelType InBelongModel);

	//~ Begin FArmyController Interface
	virtual void InitWidget(TSharedPtr<SOverlay> InParentWidget = NULL) {}
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint) override;
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	//~ End FArmyController Interface

	/**	鼠标移动*/
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;	

	TSharedPtr<class FArmySimpleComponent> AddSimpleComponent(TSharedPtr<FArmySimpleComponent> ResultComponent);

protected:
	/** 是否正在操作点 */
	bool IsOperating();

protected:		
	bool CaptureLine(const FVector& Pos);
	void CaculateDirectionAndPos(const FVector& Pos);

private:	
	TSharedPtr<class FArmySimpleComponent> TempComponent;
	TSharedPtr<FArmyPrimitive> OperationPoint;

	EObjectType ObjType;

	bool bFirstAdd = true;	

	FVector DragPrePos;
	FVector2D MousePreDragPos;

	TArray< TWeakPtr<FArmyLine> > Lines;
	TArray< TWeakPtr<class FArmyRoom> > Rooms;
};