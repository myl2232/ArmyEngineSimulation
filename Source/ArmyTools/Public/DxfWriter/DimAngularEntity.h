#pragma once
#include "IEntity.h"
#include "EntityCommonType.h"
class DimAngularEntity :
	public IEntity
{
public:
	OBJECT_DELETE
	DimAngularEntity();
	virtual ~DimAngularEntity();
public:
	void	setDimAngularInfo(const DimensionDesc &desc, const Double4	&V1, const Double4	&V2, const Double4	&V3, const Double4	&V4);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};
protected:
	DimensionDesc desc;
	Double4			  definitionPoint1;
	Double4			  definitionPoint2;
	Double4			  definitionPoint3;
	Double4			  definitionPoint4;
};

