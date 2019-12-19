#pragma once
#include "IEntity.h"
#include "dl_entities.h"
class DimMLeader:
	public	IEntity
{
public:
	DimMLeader();
	~DimMLeader();
public:
	OBJECT_DELETE
	void setDimMLeaderInfo(const Double4&BasePoint,const Double4&MidPoint,const Double4&StartPoint, const Double4&FontPoint,const Double4&FontDirection,double FontSize,double arrowSize,const char*context);

	virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void	asHatch(DL_WriterA*dw, DL_Dxf*dxf) {}

private:
	DL_DimMLeaderData data;
};

