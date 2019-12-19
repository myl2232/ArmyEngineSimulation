#pragma once
#include "IEntity.h"

class DimRadialEntity:
	public IEntity
{
public:
	DimRadialEntity();
	virtual ~DimRadialEntity();
public:
	OBJECT_DELETE
	void  setDimRadialInfo(const DimensionDesc&desc, const Double4&point, double length);

	virtual void asDXF(DL_WriterA* dw, DL_Dxf* dxf) ;

	virtual void asHatch(DL_WriterA* dw, DL_Dxf* dxf) {}
protected:
	DimensionDesc desc;
	Double4			point;
	double				length;
};

