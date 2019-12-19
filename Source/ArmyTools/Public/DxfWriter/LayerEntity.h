#pragma once
#include "IEntity.h"

class LayerEntity:
	public IEntity
{
public:
	OBJECT_DELETE
	LayerEntity();
	~LayerEntity();
public:
	void  setLayerInfo(const char*layerName, int flag, bool off);

	virtual void asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void asHatch(DL_WriterA*dw, DL_Dxf*dxf) {}

protected:
	DL_LayerData layerData;
};

