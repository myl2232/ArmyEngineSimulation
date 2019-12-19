#include "PolylineEntity.h"
#include <string.h>


PolylineEntity::PolylineEntity()
{
}


PolylineEntity::~PolylineEntity()
{
}
void PolylineEntity::setPolyline(Double4*vertex, int size)
{
	vertexes.resize(size);
	memcpy(&vertexes[0], vertex, sizeof(Double4)*size);
	data.number = size;
	data.m = 0;
	data.n = 0;
	data.flags = 128;
}
void	PolylineEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	
	dxf->writePolyline(
		*dw,
		DL_PolylineData(vertexes.size(),
			0, 0,
			128),
		attribute
	);
	for (auto &data: vertexes) {
		dxf->writeVertex(*dw, DL_VertexData(data));
	}
	dxf->writePolylineEnd(*dw);
}