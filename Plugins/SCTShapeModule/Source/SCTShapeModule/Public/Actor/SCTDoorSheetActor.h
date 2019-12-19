/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File SCTDoorSheetActor.h
* @Description 组合型Actor
*
* @Author 赵志强
* @Date 2018年5月15日
* @Version 1.0
*/
#pragma once

#include "SCTCompActor.h"
#include "SCTDoorSheetActor.generated.h"

UCLASS()
class SCTSHAPEMODULE_API ASCTDoorSheetActor : public ASCTCompActor
{
	GENERATED_BODY()
public:
	virtual void UpdateActorDimension() override;
};