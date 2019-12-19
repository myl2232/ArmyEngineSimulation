#include "TextEntity.h"



TextEntity::TextEntity()
{
}


TextEntity::~TextEntity()
{
}

void		TextEntity::setTextInfo(const TextDesc &desc)
{
	this->textDesc = desc;
}

void		TextEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{
	DL_TextData data(textDesc.insertPoint.x,
		textDesc.insertPoint.y,
		textDesc.insertPoint.z,
		textDesc.alignPoint.x,
		textDesc.alignPoint.y,
		textDesc.alignPoint.z,
		textDesc.height,
		textDesc.xScaleFactor,
		textDesc.textGenerationFlags,
		textDesc.hJustification,
		textDesc.vJustification,
		textDesc.text,
		textDesc.style,
		textDesc.angle
	);
	dxf->writeText(*dw, data, attribute);
}

 