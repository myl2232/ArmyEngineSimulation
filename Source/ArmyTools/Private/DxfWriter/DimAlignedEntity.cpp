#include "DimAlignedEntity.h"



DimAlignedEntity::DimAlignedEntity()
{
}


DimAlignedEntity::~DimAlignedEntity()
{
}

void  DimAlignedEntity::setDimAlignedInfo(const DimensionDesc& Desc, Double4 start, Double4 end)
{
	this->desc = Desc;
	this->startPoint = start;
	this->endPoint = end;
}

void	DimAlignedEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	DL_DimAlignedData dimAlignedData(startPoint.x, startPoint.y, startPoint.z, endPoint.x, endPoint.y, endPoint.z);
	DL_DimensionData	 dimData(
		desc.definitionPoint.x,
		desc.definitionPoint.y,
		desc.definitionPoint.z,
		desc.midTextPoint.x,
		desc.midTextPoint.y,
		desc.midTextPoint.z,
		desc.type,
		desc.attachmentPoint,
		desc.lineSpacingStyle,
		desc.lineSpacingFactor,
		"",
		"",
		desc.angle,
		desc.linearFactor,
		desc.dimScale
	);
	dimData.arrow1Flipped = desc.arrow1Flipped;
	dimData.arrow2Flipped = desc.arrow2Flipped;

 
	dxf->writeDimAligned(*dw, dimData, dimAlignedData, attribute);
}