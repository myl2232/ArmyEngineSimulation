/**
* Copyright 2019 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRWHTextRenderActor.h
* @Description 3D 字体
*
* @Author 欧石楠
* @Date 2019年7月3日
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

	/** 当actor 开始play时，覆盖的本地事件. */
	virtual void BeginPlay() override;

public:

	AXRWHTextRenderActor(const FObjectInitializer& ObjectInitializer);

	/**
		*当一个actor完成对世界的生成后调用(UWorld::SpawnActor)。
		*对于具有根组件的参与者，位置和旋转已经设置好。
		*发生在调用任何构造脚本之后
	*/
	virtual void PostActorCreated() override;

	/** 允许actor 在 C++ 端初始化自己 */
	virtual void PostInitializeComponents() override;

	/**
		Actor在每一帧上调用此函数。重写此函数以实现要在每个帧中执行的自定义逻辑。
		*注意，Tick在默认情况下是禁用的，您需要检查PrimaryActorTick。bCanEverTick被设置为true以启用它。
		*
		* @param DeltaSeconds 最后一帧时间内经过时间膨胀修改 这个游戏时间值
	*/
	virtual void Tick(float DeltaSeconds) override;

	void SetText(const FString& TextString);

	void SetColor(FColor textColor);

	void SetTextRelativeLocation(FVector& relativeLoc);
protected:
	
	float ShowTextMaxTime = 10.0f;

	float Timer = 0.0f;
};
