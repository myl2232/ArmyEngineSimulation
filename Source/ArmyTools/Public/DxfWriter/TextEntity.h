#pragma once
#include "IEntity.h"
 
class TextEntity :
	public IEntity
{
public:
	TextEntity();
	~TextEntity();
public:
	OBJECT_DELETE

	 void		setTextInfo(const TextDesc &desc);

	 void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	 void		asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};
 
public:
	TextDesc textDesc;
};

