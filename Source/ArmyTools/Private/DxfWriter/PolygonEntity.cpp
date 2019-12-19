#include "PolygonEntity.h"



PolygonEntity::PolygonEntity()
{
	number = 0;
}


PolygonEntity::~PolygonEntity()
{
}

void  PolygonEntity::setPolygonInfo(int number, const Double4*array, int flag)
{
	if (!number)
		return;
	this->number = number;
	this->vertex.resize(number);
	this->flags = flag;
	memcpy(&vertex[0], array, sizeof(Double4)*number);
}

void	 PolygonEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	if (!number)
		return;
	 

	dxf->writePolyline(
		*dw,
		DL_PolylineData(number,
			0, 0, flags),
		attribute
	);
	for (size_t i = 0; i < number; i++) {
		Double4 v = vertex[i];
		dxf->writeVertex(*dw, DL_VertexData(v.x, v.y, v.z	, v.w));
	}
	dxf->writePolylineEnd(*dw);
}

void	 PolygonEntity::asHatch(DL_WriterA*dw, DL_Dxf*dxf)
{
	
}