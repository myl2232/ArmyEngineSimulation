#include "CircleEntity.h"



CircleEntity::CircleEntity()
{
}


CircleEntity::~CircleEntity()
{
}

void CircleEntity::setCircle(const Double4&centre, double radius)
{
	this->centre =centre;
	this->radius =radius;
}

void	CircleEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	DL_CircleData Circle = DL_CircleData(centre.x, centre.y, centre.z, radius);
 
	dxf->writeCircle(*dw, Circle, attribute);
}
