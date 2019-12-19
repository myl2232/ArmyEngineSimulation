#include "LineEntity.h"



LineEntity::LineEntity()
{
}


LineEntity::~LineEntity()
{
}

void	LineEntity::setLines(const Double4 &beg, const Double4&end	)
{
	this->startPoint = beg;
	this->endPoint = end;
}

void	LineEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	DL_LineData line = DL_LineData(startPoint.x, startPoint.y, startPoint.z, endPoint.x, endPoint.y, endPoint.z);
 
	dxf->writeLine(*dw, line, attribute);
}

void	LineEntity::asHatch(DL_WriterA*dw, DL_Dxf*dxf)
{
	dxf->writeHatchEdge(
		*dw,
		DL_HatchEdgeData(startPoint.x,
			startPoint.y,
			endPoint.x,
			endPoint.y));
}

 