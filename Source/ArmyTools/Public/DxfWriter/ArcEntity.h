#pragma once
#include "CircleEntity.h"
class ArcEntity :
	public CircleEntity
{
public:
	ArcEntity();
	virtual ~ArcEntity();
public:
	OBJECT_DELETE

	void setArc(double start,double end);

	virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void	asHatch(DL_WriterA*dw, DL_Dxf*dxf);

public:
	double  startAngle;
	double  endAngle;
};

