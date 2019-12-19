#pragma once
#include "IEntity.h"

class DimLinearEntity:
public IEntity
{
public:
	OBJECT_DELETE
	DimLinearEntity();
	virtual ~DimLinearEntity();
public:
	void setDimLinearInfo(const DimensionDesc &desc,const Double4&point1,const Double4&point2,double angle,double oblique);

	virtual void asDXF(DL_WriterA* dw, DL_Dxf* dxf) ;

	virtual void asHatch(DL_WriterA* dw, DL_Dxf* dxf){};
protected:
	DimensionDesc desc;

	Double4			 point1;
	Double4			 point2;
	double				 angle;
	double				 oblique;
};

