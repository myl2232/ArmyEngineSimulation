#pragma once
#include "IEntity.h"
class EllipseEntity :
	public IEntity
{
public:
	EllipseEntity();
	~EllipseEntity();
public:
	void setEllipse(const Double4& centrePoint, const Double4& endPoint, const Double4&normal,float	angleStart, float	angleEnd,float	ratio);

	virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void	asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};
protected:
	DL_EllipseData	 data;
};

