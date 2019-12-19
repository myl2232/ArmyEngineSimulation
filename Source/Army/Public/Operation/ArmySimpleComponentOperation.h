/**
 * Copyright 2018 ������Ƽ����޹�˾.
 * All Rights Reserved.
 * 
 *
 * @File XRSimpleComponentOperation.h
 * @Description �򵥽ṹ�ؼ�����
 *
 * @Author ŷʯ�
 * @Date 2018��10��12��
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

	/**	����ƶ�*/
	virtual void MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;	

	TSharedPtr<class FArmySimpleComponent> AddSimpleComponent(TSharedPtr<FArmySimpleComponent> ResultComponent);

protected:
	/** �Ƿ����ڲ����� */
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