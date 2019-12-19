#include "LayerEntity.h"



LayerEntity::LayerEntity()
{
}


LayerEntity::~LayerEntity()
{
}

void LayerEntity::setLayerInfo(const char*layerName, int flag, bool off)
{
	this->layerData.name = layerName;
	this->layerData.flags = flag;
	this->layerData.off = off;
}

void LayerEntity::asDXF(DL_WriterA*dw, DL_Dxf*dxf)
{



}