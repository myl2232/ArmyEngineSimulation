#include "DimAngularEntity.h"



DimAngularEntity::DimAngularEntity()
{
}

DimAngularEntity::~DimAngularEntity()
{
}

void	DimAngularEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{

	DL_DimAngularData dimAngularData(definitionPoint1.x, definitionPoint1.y, definitionPoint1.z,
		definitionPoint2.x, definitionPoint2.y, definitionPoint2.z,
		definitionPoint3.x, definitionPoint3.y, definitionPoint3.z,
		definitionPoint4.x, definitionPoint4.y, definitionPoint4.z);
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

	DL_Attributes attributes;
 
	dxf->writeDimAngular(*dw, dimData, dimAngularData, attribute);
}

void	DimAngularEntity::setDimAngularInfo(const DimensionDesc &desc, const Double4	&V1, const Double4	&V2, const Double4	&V3, const Double4	&V4)
{
	this->desc = desc;
	this->definitionPoint1 = V1;
	this->definitionPoint2 = V2;
	this->definitionPoint3 = V3;
	this->definitionPoint4 = V4;
}