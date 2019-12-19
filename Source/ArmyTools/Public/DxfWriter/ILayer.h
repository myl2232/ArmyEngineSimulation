#pragma once
#include "EntityCommonType.h"
 
class ILayer
{
public:
	ILayer();
	virtual ~ILayer();
public:
	virtual void setLayer(const char* layerName, int flags)=0;
	
	virtual void release() = 0;
};

