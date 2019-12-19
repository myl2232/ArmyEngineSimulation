#pragma once
#include "dl_dxf.h"
#include "IEntity.h"
#include <string>
using namespace  std;

class Layer :
	  public IEntity
{
public:
	OBJECT_DELETE
	Layer();
	virtual ~Layer();
public:
	friend class Canvas;

	virtual void setLayer(const char* layerName, int flags);

	virtual void asDXF(DL_WriterA*dw, DL_Dxf*dxf) {}

	virtual void asHatch(DL_WriterA*dw, DL_Dxf*dxf) {}
private:
	DL_LayerData data;
};

