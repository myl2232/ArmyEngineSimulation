#include "DimMLeader.h"



DimMLeader::DimMLeader()
{
}


DimMLeader::~DimMLeader()
{
}

void DimMLeader::setDimMLeaderInfo(const Double4&BasePoint, const Double4&MidPoint, const Double4&StartPoint, const Double4&FontPoint, const Double4&FontDirection, double FontSize, double arrowSize ,const char*context)
{
	data.arrowSize = arrowSize;
	data.baseX = BasePoint.x;
	data.baseY = BasePoint.y;
	data.context = context;
	data.fontHeight = FontSize;
	data.fontX = FontPoint.x;
	data.fontY= FontPoint.y;
	data.midX = MidPoint.x;
	data.midY = MidPoint.y;
	data.startX = StartPoint.x;
	data.startY = StartPoint.y;
	data.fontdx = FontDirection.x;
	data.fontdy = FontDirection.y;
}

void	DimMLeader::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	dxf->writeMLeader(*dw,data,attribute);
}