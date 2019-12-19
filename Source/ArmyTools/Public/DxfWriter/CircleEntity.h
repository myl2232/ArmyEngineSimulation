#pragma once
#include "IEntity.h"
class CircleEntity :
	public IEntity
{
public:
	CircleEntity();
	virtual ~CircleEntity();
public:
	OBJECT_DELETE

	void setCircle(const Double4&centre, double radius);

	virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};

public:
	Double4 centre;
	double	   radius;
};

