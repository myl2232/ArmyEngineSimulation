#include "DimOrdinateEntity.h"



DimOrdinateEntity::DimOrdinateEntity()
{
}


DimOrdinateEntity::~DimOrdinateEntity()
{
}

void DimOrdinateEntity::setDimOrdinateInfo(const DimensionDesc &desc, const Double4&point1, const Double4&point2, bool xType)
{
	this->desc = desc;
	this->point1 = point1;
	this->point2 = point2;
	this->xtype = xType;

}

void DimOrdinateEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	const DL_DimOrdinateData dimDiametricData(point1.x,
		point1.y,point1.z,point2.x,point2.y,point2.z,xtype
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

 

	dxf->writeDimOrdinate(*dw, dimData, dimDiametricData, attribute);

}