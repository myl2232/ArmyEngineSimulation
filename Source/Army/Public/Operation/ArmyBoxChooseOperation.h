/**
* Copyright 2018 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File XRBoxChooseOperation.h
* @Description ��ѡ������
*
* @Author ������
* @Date 2019��1��8��
* @Version 1.0
*/

#pragma once

#include "ArmyOperation.h"


class FArmyBoxChooseOperation : public FArmyOperation
{
public:
	FArmyBoxChooseOperation(EModelType InBelongModel) :FArmyOperation(InBelongModel) {};
	~FArmyBoxChooseOperation() {};

	virtual void BeginOperation(XRArgument InArg = XRArgument()) override;
	virtual void EndOperation() override;
	virtual bool InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event) override;
	virtual void MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key) override;
	virtual void SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint) override;

private:
	//���������ק����
	FVector FArmyBoxChooseOperation::CalSelectOperation(const FObjectPtr& ObjectIt, const FVector OutOffset) const;

private:
	TArray<FObjectPtr> SelectedObjList;

private:
	FVector FontFramePos    = FVector::ZeroVector;//ǰһ֡λ��
	FVector CurrentFramePos = FVector::ZeroVector;//��ǰ֡λ��
	bool FlagStartDrag = true;//�ж��Ƿ��ǿ�ʼ��ק
};
