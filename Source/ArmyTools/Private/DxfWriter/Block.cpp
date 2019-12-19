#include "Block.h"



Block::Block()
{
}


Block::~Block()
{
}

void	Block::setBlockInfo(const Double4&point, const std::string &name)
{
	offset = point;
	blockName = name;
}

void  Block::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	dxf->writeBlock(*dw, DL_BlockData(blockName, 0, offset.x, offset.y	, offset.z));
	for (auto entity : entitysArray)
	{
		IEntity* temp =dynamic_cast<IEntity*>(entity);
		temp->asDXF(dw, dxf);
	}
	dxf->writeEndBlock(*dw, blockName);
}
 
void Block::asHacth(DL_WriterA*dw, DL_Dxf*dxf)
{
	DL_HatchLoopData lData((int)entitysArray.size());
	dxf->writeHatchLoop1(*dw, lData);
	for (auto entity : entitysArray)
	{
		IEntity* temp = dynamic_cast<IEntity*>(entity);
		temp->asHatch(dw, dxf);
	}
	dxf->writeHatchLoop2(*dw, lData);
}


void Block::drawEntity(AttributeEntity*entity)
{
	if (!entity)
		return;
	entitysArray.push_back(entity);
}

void Block::clear()
{
	for (auto entity : entitysArray)
		entity->release();
	entitysArray.clear();
}