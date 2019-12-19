#pragma once
#include "IEntity.h"
class LineEntity :
	public  IEntity 
{
public:
	LineEntity();
	virtual ~LineEntity();
public:
	OBJECT_DELETE

	void	setLines(const Double4 &beg, const Double4&end);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf);

 
 
public:
	Double4 startPoint;
	Double4 endPoint;

};

