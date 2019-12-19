#pragma once
#include "IEntity.h"
#include "EntityCommonType.h"
class DimDiametricEntity:
	public IEntity
{
public:
	OBJECT_DELETE
	DimDiametricEntity();
	virtual ~DimDiametricEntity();
public:
	void	setDimDiametricInfo(const DimensionDesc&desc,const Double4&point,double length);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};
protected:
	DimensionDesc	desc;
	Double4				point;
	double					length;
};

