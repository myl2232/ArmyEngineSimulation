#pragma once
#include "IEntity.h"
class BlockEntity :
	public IEntity
{
public:
	BlockEntity();
	virtual ~BlockEntity();
	friend class Canvas;
	OBJECT_DELETE
	void		setBlockRefInfo(const char*name, const Double4&insertPoint, const Double4 &scale, double angle, int columns, int rows, double columnSpace, double rowSpace);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf){}

protected:
	std::string name;
	Double4  insertPoint;
	Double4	scale;
	double		angle;
	int			columns;
	int			rows;
	double		columnSpace;
	double		rowSpace;
};

