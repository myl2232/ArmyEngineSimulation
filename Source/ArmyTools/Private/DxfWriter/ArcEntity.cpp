#include "ArcEntity.h"



ArcEntity::ArcEntity()
{
}


ArcEntity::~ArcEntity()
{
}

void ArcEntity::setArc(double start, double end)
{
	this->startAngle = start;
	this->endAngle = end;
}

void	ArcEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	DL_ArcData arc = DL_ArcData(centre.x, centre.y, centre.z, radius, startAngle, endAngle);
 
	dxf->writeArc(*dw, arc, attribute);
}

void	ArcEntity::asHatch(DL_WriterA*dw, DL_Dxf*dxf)
{
	dxf->writeHatchEdge(
		*dw,
		DL_HatchEdgeData(centre.x, centre.y,   radius, startAngle, endAngle,true));
}