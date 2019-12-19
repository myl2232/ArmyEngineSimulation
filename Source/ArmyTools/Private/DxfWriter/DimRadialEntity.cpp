#include "DimRadialEntity.h"



DimRadialEntity::DimRadialEntity()
{
}


DimRadialEntity::~DimRadialEntity()
{

}

void  DimRadialEntity::setDimRadialInfo(const DimensionDesc&desc, const Double4&point, double length)
{
	this->desc = desc;
	this->point = point;
	this->length = length;
}

void DimRadialEntity::asDXF(DL_WriterA* dw, DL_Dxf* dxf)
{
	const DL_DimRadialData dimDiametricData(point.x,
		point.y,
		point.z,
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

 

	dxf->writeDimRadial(*dw, dimData, dimDiametricData, attribute);

}