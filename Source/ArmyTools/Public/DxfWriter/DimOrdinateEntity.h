#pragma once
#include "IEntity.h"
#include "EntityCommonType.h"
class DimOrdinateEntity:
	public IEntity
{
public:
	DimOrdinateEntity();
	virtual ~DimOrdinateEntity();
public:
	OBJECT_DELETE
	void setDimOrdinateInfo(const DimensionDesc &desc, const Double4&point1, const Double4&point2, bool xType);

	virtual void asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void asHatch(DL_WriterA*dw, DL_Dxf*dxf){}
protected:
	DimensionDesc desc;
	Double4			point1;
	Double4			point2;
	bool					xtype;
};

