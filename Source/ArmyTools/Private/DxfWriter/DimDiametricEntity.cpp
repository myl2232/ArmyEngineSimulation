#include "DimDiametricEntity.h"



DimDiametricEntity::DimDiametricEntity()
{
}


DimDiametricEntity::~DimDiametricEntity()
{
}

void	DimDiametricEntity::setDimDiametricInfo(const DimensionDesc&desc, const Double4&point, double length)
{
	this->desc = desc;
	this->point = point;
	this->length = length;
}

void	DimDiametricEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	const DL_DimDiametricData dimDiametricData(point.x,
		point.y	,
		point.z	,
		length
	);
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
	
	dxf->writeDimDiametric(*dw, dimData, dimDiametricData, attribute);
}
