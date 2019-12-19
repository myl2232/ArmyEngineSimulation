/**
* Copyright 2019 ������Ƽ����޹�˾.
* All Rights Reserved.
* 
*
* @File XRWHTextRenderActor.h
* @Description 3D ����
*
* @Author ŷʯ�
* @Date 2019��7��3��
* @Version 1.0
*/
#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "ArmyWHTextRenderActor.generated.h"

/**
 * 
 */
UCLASS()
class  AXRWHTextRenderActor : public ATextRenderActor
{
	GENERATED_BODY()
	
	
protected:

	/** ��actor ��ʼplayʱ�����ǵı����¼�. */
	virtual void BeginPlay() override;

public:

	AXRWHTextRenderActor(const FObjectInitializer& ObjectInitializer);

	/**
		*��һ��actor��ɶ���������ɺ����(UWorld::SpawnActor)��
		*���ھ��и�����Ĳ����ߣ�λ�ú���ת�Ѿ����úá�
		*�����ڵ����κι���ű�֮��
	*/
	virtual void PostActorCreated() override;

	/** ����actor �� C++ �˳�ʼ���Լ� */
	virtual void PostInitializeComponents() override;

	/**
		Actor��ÿһ֡�ϵ��ô˺�������д�˺�����ʵ��Ҫ��ÿ��֡��ִ�е��Զ����߼���
		*ע�⣬Tick��Ĭ��������ǽ��õģ�����Ҫ���PrimaryActorTick��bCanEverTick������Ϊtrue����������
		*
		* @param DeltaSeconds ���һ֡ʱ���ھ���ʱ�������޸� �����Ϸʱ��ֵ
	*/
	virtual void Tick(float DeltaSeconds) override;

	void SetText(const FString& TextString);

	void SetColor(FColor textColor);

	void SetTextRelativeLocation(FVector& relativeLoc);
protected:
	
	float ShowTextMaxTime = 10.0f;

	float Timer = 0.0f;
};
