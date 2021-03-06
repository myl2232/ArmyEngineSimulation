/**
* Copyright 2018 北京科技公司
* All Rights Reserved.
* 
*
* @File IEntity
* @Description  
*
* @Author 欧石楠
* @Date 2018.1.22
* @Version 1.0
*/
#pragma once
#include <string>
#include "ILineType.h"
#include "AttributeEntity.h"
#include "dl_dxf.h"
#include "EntityCommonType.h"
#define	OBJECT_DELETE		virtual void release(){delete this;} 		
class DL_WriterA;
class DL_Dxf;

class DL_Attributes;
class IEntity:public AttributeEntity
{
public:
	 
	IEntity();
	virtual ~IEntity();
public:
	OBJECT_DELETE
	/**设置实体 */
	virtual void	setEntityAttribute(const AttributeDesc &Desc);

	virtual	void	asDXF(DL_WriterA*dw, DL_Dxf*dxf)=0;

	virtual void	asHatch(DL_WriterA*dw, DL_Dxf*dxf)=0;

	double	 rad2deg(double a);

	int	colorToNumber(int R, int G, int B,ColorType type);

	int	colorToNumber24(int R, int G, int B, ColorType type);

	int    widthToNumber(LineWeight weight);
protected:
	DL_Attributes attribute;
};

