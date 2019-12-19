#pragma once
#include "IEntity.h"
class PointEntity
	:public IEntity
{
public:
	PointEntity();
	~PointEntity();
public:
	OBJECT_DELETE
	virtual void  setPoint(double px, double py, double pz = 0.0);
 
	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};

 
private:
	Double4	point;
};

