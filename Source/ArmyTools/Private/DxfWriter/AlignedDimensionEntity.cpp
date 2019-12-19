#include "AlignedDimensionEntity.h"



AlignedDimensionEntity::AlignedDimensionEntity()
{
}


AlignedDimensionEntity::~AlignedDimensionEntity()
{
}

void  AlignedDimensionEntity::setAlignedDimensionInfo(const Double4&beg, const Double4&end)
{
	this->startPoint = beg;
	this->endPoint = end;
}

void	AlignedDimensionEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	/* 
	DL_DimAlignedData dimAlignedData(startPoint.x, startPoint.y, 0, endPoint.x, endPoint.y,0);
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
	dxf->writeDimAligned(*dw, this->attribute, dimAlignedData, attribute);
	 */
}

void	AlignedDimensionEntity::asHatch(DL_WriterA*dw, DL_Dxf*dxf)
{

}