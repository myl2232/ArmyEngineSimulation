#pragma once
#include "IEntity.h"
#include <vector>
class PolygonEntity :
	public IEntity
{
public:
	PolygonEntity();
	~PolygonEntity();
public:
	OBJECT_DELETE

	void setPolygonInfo(int number,const Double4*array, int flag);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf);

protected:
	 
	unsigned int number;

 
	int flags;

	std::vector<Double4>	vertex;
};

