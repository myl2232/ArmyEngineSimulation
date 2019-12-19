#pragma once
#include "IEntity.h"
#include "EntityCommonType.h"
class DimAlignedEntity:
	public IEntity
{
public:
	OBJECT_DELETE
	DimAlignedEntity();
	virtual ~DimAlignedEntity();
public:
	void  setDimAlignedInfo(const DimensionDesc& desc,Double4 start,Double4 end);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf) {};

protected:
	DimensionDesc  desc;
	Double4			   startPoint;
	Double4			   endPoint;
};

