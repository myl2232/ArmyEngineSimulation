#include "BlockEntity.h"



BlockEntity::BlockEntity()
{
}


BlockEntity::~BlockEntity()
{
}

void	BlockEntity::setBlockRefInfo(const char*name, const Double4&insertPoint, const Double4 &scale, double angle, int columns, int rows, double columnSpace, double rowSpace)
{
	this->name = name;
	this->insertPoint = insertPoint;
	this->scale = scale;
	this->angle = angle;
	this->columns = columns;
	this->rows = rows;
	this->columnSpace = columnSpace;
	this->rowSpace = rowSpace;
}

 

void		BlockEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
 
	dxf->writeInsert(
		*dw,
		DL_InsertData(
			name,
			insertPoint.x,
			insertPoint.y,
			insertPoint.z,
			scale.x,
			scale.y,
			scale.z,
			rad2deg(angle),
			columns, rows,
			columnSpace,rowSpace),
		attribute
	);

 }
