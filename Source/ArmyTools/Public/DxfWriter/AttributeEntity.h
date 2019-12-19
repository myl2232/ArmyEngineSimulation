#pragma once
#include "EntityCommonType.h"
class  AttributeEntity
{
public:
	AttributeEntity();
	virtual ~AttributeEntity();
public:
	/** 填充实体属性，包含图层信息，线信息，颜色等 ->To:AttributeDesc*/
	virtual void	setEntityAttribute(const AttributeDesc&Desc) = 0;
	/** 释放实体 */
	virtual void release() = 0;
};

