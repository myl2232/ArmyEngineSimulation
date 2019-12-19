#pragma once
#include "IEntity.h"
class PolylineEntity
	:public IEntity
{
 
public:
	PolylineEntity();
	virtual ~PolylineEntity();
	OBJECT_DELETE
public:
	 
	void setPolyline(Double4*vertexes,int size);

	virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void	asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};

public:
	DL_PolylineData								data;
	std::vector<DL_VertexData>			vertexes;
};

