#pragma once
#include  "DxfWriter/IEntity.h"
class AlignedDimensionEntity :
	public IEntity
{
public:
 
	AlignedDimensionEntity();
	virtual ~AlignedDimensionEntity();
public:
	OBJECT_DELETE

	void  setAlignedDimensionInfo(const Double4&beg, const Double4&end);

	virtual	void		asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void		asHatch(DL_WriterA*dw, DL_Dxf*dxf);

protected:

	DimensionDesc desc;
	Double4	startPoint;
	Double4  endPoint;
};

