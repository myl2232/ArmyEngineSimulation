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
#include "SharedPointer.h"

enum class EHardModeOperation: uint8
{
	HO_None,
	HO_DrawRect,
	HO_DrawCircle,
	HO_DrawPolygon,
	HO_DrawFreePolygon,
	HO_Replace,
	HO_MaterialBrush
};

class HardModCommonOperations
{
public:
	static	TSharedPtr<class FArmyMaterialBrushOperation> OP_MaterialBrush;// = MakeShared< FArmyMaterialBrushOperation>(E_HardModel);
};