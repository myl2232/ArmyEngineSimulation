#include "MTextEntity.h"



MTextEntity::MTextEntity()
{
}


MTextEntity::~MTextEntity()
{
}

void MTextEntity::setTextInfo(const MTextDesc&textDesc)
{
	this->textDesc = textDesc;
}

void	MTextEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	 
	int attachmentPoint = 1;
	switch (textDesc.hAlign)
	{
	case  HAlignLeft:
		attachmentPoint = 1;
		break;
	case  HAlignCenter:
		attachmentPoint = 2;
		break;
	case  HAlignRight:
		attachmentPoint = 3;
		break;
	}

	switch (textDesc.vAlign) 
	{
	case VAlignTop:
		attachmentPoint += 0;
		break;
	case VAlignMiddle:
		attachmentPoint += 3;
		break;
	case VAlignBottom:
	case VAlignBase:
		attachmentPoint += 6;
		break;
	}

	int drawingDirection = 1;
	switch (textDesc.drawDirection)
	{
	case  LeftToRight:
		drawingDirection = 1;
		break;
	case TopToBottom:
		drawingDirection = 3;
		break;
	case ByStyle:
		drawingDirection = 5;
		break;
	}

	int lineSpacingStyle = 2;
	switch (textDesc.lineSpaceStyle) {
	case AtLeast:
		lineSpacingStyle = 1;
		break;
	case Exact:
		lineSpacingStyle = 2;
		break;
	}

	DL_MTextData data(
		textDesc.alignmentPoint.x,
		textDesc.alignmentPoint.y,
		0.0, 
		textDesc.height,
		textDesc.width,
		attachmentPoint,
		drawingDirection,
		lineSpacingStyle,
		textDesc.lineSpacingFactor,
		textDesc.text,
		textDesc.style,
		textDesc.angle
		);
 
	dxf->writeMText(*dw, data, attribute);

}