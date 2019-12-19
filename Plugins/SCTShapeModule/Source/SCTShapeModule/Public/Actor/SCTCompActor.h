/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTInsideShape.h
* @Description 组合型Actor
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "SCTShapeActor.h"
#include "SCTCompActor.generated.h"

UCLASS()
class SCTSHAPEMODULE_API ASCTCompActor : public ASCTShapeActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASCTCompActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

public:
	virtual void UpdateActorDimension() override;
	virtual void UpdateActorPosition() override;
	virtual void UpdateActorRotation() override;

};