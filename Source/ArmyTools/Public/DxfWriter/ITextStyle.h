#pragma once
#include "EntityCommonType.h"
/**
*@ 字体风格类                                                              
*/
class  ITextStyle
{
public:
	ITextStyle();
	virtual ~ITextStyle();
public:
	/** 
	*@设置字体风格
	*/
	virtual void setTextStyle(const TextStyleDesc& Desc) = 0;

	/**
	*@清理内存
	*/
	virtual void release() = 0;

};

