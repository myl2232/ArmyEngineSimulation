#include "PointEntity.h"
#include "dl_dxf.h"


PointEntity::PointEntity()
{
}


PointEntity::~PointEntity()
{
}

void	PointEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	dxf->writePoint(*dw, DL_PointData(point.x, point.y, point.z), attribute);
}

void  PointEntity::setPoint(double px, double py, double pz)
{
	point.x = px;
	point.y = py;
	point.z = pz;
	point.w = 0.0;
}
