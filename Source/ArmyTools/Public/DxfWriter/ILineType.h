#pragma once
#include "EntityCommonType.h"
class  ILineType
{
public:
	ILineType();
	virtual	~ILineType();
public:
	/** 默认线性类型 TODO:*/
	virtual void  setLineType(const char* lineName, int flag=0)=0;

	/** 自定义线性类型 TODO: */
	virtual void  setCustomLineType(const char*lineName, const char*lineDesc, int dashNum, double*patternNum, int patternLength, int flag = 0)=0;

	/** 资源释放 */
	virtual void release() = 0;
};

