#include "DimLinearEntity.h"



DimLinearEntity::DimLinearEntity()
{
}


DimLinearEntity::~DimLinearEntity()
{

}

void DimLinearEntity::asDXF(DL_WriterA* dw, DL_Dxf* dxf)
{

	const DL_DimLinearData dimDiametricData(
		point1.x,point1.y,point1.z,
		point2.x,point2.y,point2.z,angle,oblique
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

 

	dxf->writeDimLinear(*dw, dimData, dimDiametricData, attribute);

}

void DimLinearEntity::setDimLinearInfo(const DimensionDesc &desc, const Double4&point1, const Double4&point2, double angle, double oblique)
{
	this->desc = desc;
	this->point1 = point1;
	this->point2 = point2;
	this->angle = angle;
	this->oblique = oblique;
}
