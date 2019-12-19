#include "HatchEntity.h"
#include "Block.h"


HatchEntity::HatchEntity()
{
}


HatchEntity::~HatchEntity()
{
}


void  HatchEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	if (!IblockArray.size())
		return;
 
	DL_HatchData data(numLoops,solid,scale,rad2deg(angle),pattern);
	data.originX = xy.x;
	data.originY = xy.y;
	
	dxf->writeHatch1(*dw, data, attribute);
	for (auto block : IblockArray)
	{
		Block *Eblock = dynamic_cast<Block*>(block);
		Eblock->asHacth(dw,dxf);
	}
	dxf->writeHatch2(*dw, data, attribute);
}


void	 HatchEntity::setHatchInfo(int loops, bool solid, double scale, double angle, const char*pattern)
{
	this->numLoops = loops;
	this->solid = solid;
	this->scale = scale;
	this->angle = angle;
	this->pattern = pattern;
	xy = Double4(0, 0);
}

void HatchEntity::clear()
{
	for (auto block : IblockArray)
		block->release();
	IblockArray.clear();
}
