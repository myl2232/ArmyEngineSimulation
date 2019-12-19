#include "Layer.h"



Layer::Layer()
{
}


Layer::~Layer()
{
}


void Layer::setLayer(const char* layerName, int flags)
{
	data.name = layerName;
	data.flags = flags&(~LayerOff);
	data.off = (flags&LayerOff)!=0;
}

