#include "EllipseEntity.h"



EllipseEntity::EllipseEntity()
{
}


EllipseEntity::~EllipseEntity()
{
}

void EllipseEntity::setEllipse(const Double4& centrePoint, const Double4& endPoint, const Double4&normal, float	angleStart, float	angleEnd, float	ratio)
{
	data.angle1 = angleStart;
	data.angle2 = angleEnd;
	data.cx = centrePoint.x;
	data.cy = centrePoint.y;
	data.cz = centrePoint.z;
	data.mx = endPoint.x;
	data.my = endPoint.y;
	data.mz = endPoint.z;
	data.ratio = ratio;
	data.nx = normal.x;
	data.ny = normal.y;
	data.nz = normal.z;
}
void	EllipseEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	dxf->writeEllipse(*dw, data, attribute);
}