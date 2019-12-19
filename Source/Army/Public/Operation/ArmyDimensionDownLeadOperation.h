/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File XRDimensionDownLeadOperation.h
* @Description ��ߵ����߲�����
*
* @Author ������
* @Date 2018��10��26��
* @Version 1.0
*/
#pragma once
#include "ArmyOperation.h"

DECLARE_DELEGATE_OneParam(GetObjectArrayDelegate, TArray<FObjectWeakPtr>&)

class FArmyDimensionDownLeadOperation : public FArmyOperation
{
public:
	FArmyDimensionDownLeadOperation(EModelType InBelongModel);
	virtual ~FArmyDimensionDownLeadOperation() {}

	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive) override;
	virtual FObjectPtr GetOperationObject() override;
	virtual TSharedPtr<FArmyEditPoint> GetOperationPoint() override;
	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	/** ���� */
	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View);
	virtual void DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, const FSceneView* View, FCanvas* Canvas);
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseMove(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y) override;

public:
	GetObjectArrayDelegate OnGetObjectArrayDelegate;
private:
	XRArgument CurrentArg;
	//TSharedPtr<SOverlay> ViewPortOverlay;

	TSharedPtr<class FArmyRectSelect> RectSelect;

	TSharedPtr<class FArmyDimensions> CurrentDimension;

	int32 CurrentOperationIndex = 0;
	/** ������ */
	TSharedPtr<FArmyEditPoint> OperationPoint;
};