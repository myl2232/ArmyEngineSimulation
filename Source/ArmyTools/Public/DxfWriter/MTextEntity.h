#pragma once
#include "IEntity.h"
class MTextEntity :
	public IEntity
{
public:
	MTextEntity();
	~MTextEntity();
public:
	 OBJECT_DELETE
	 

	 void setTextInfo(const MTextDesc&textDesc);

	 virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	 virtual void	asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};
protected:

	MTextDesc textDesc;
};

