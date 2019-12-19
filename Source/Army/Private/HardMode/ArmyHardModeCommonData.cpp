#pragma once
/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRHardModeCommonData.h
* @Description 立面模式共有的类和变量
*
* @Author 郭子阳
* @Date 2019年4月24日
* @Version 1.0
*/

#include "ArmyHardModeCommonData.h"
#include "ArmyMaterialBrushOperation.h"

TSharedPtr<FArmyMaterialBrushOperation> HardModCommonOperations::OP_MaterialBrush = MakeShared< FArmyMaterialBrushOperation>(E_HardModel);
