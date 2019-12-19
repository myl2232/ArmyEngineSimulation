#include "IEntity.h"



IEntity::IEntity()
{
}


IEntity::~IEntity()
{
}

/**设置实体 */
void	IEntity::setEntityAttribute(const AttributeDesc &Desc)
{
	attribute.setLineScale(Desc.lineScale);
	if (Desc.layerName.size())
		attribute.setLayer(Desc.layerName);
	else
		attribute.setLayer("0");
	if (Desc.lineType.size())
		attribute.setLineType(Desc.lineType);
	else
		attribute.setLineType("CONTINUOUS");
	if (Desc.blockProperty){
		attribute.setColor(Desc.color);
		attribute.setColor24(Desc.color24);
	}
	else
	{
		attribute.setColor(colorToNumber(Desc.red, Desc.green, Desc.blue, Desc.colorType));
		attribute.setColor24(colorToNumber24(Desc.red, Desc.green, Desc.blue, Desc.colorType));
	}
	attribute.setWidth(widthToNumber(Desc.weight));
}

double		IEntity::rad2deg(double a)
{
	return (a / (2.0 * M_PI) * 360.0);
}
 
int	IEntity::colorToNumber(int R,int G,int B, ColorType type)
{
	if (type == ColorType::ByBlock)
		return 0;
	else if (type == ColorType::ByLayer)
		return 256;
	else if (R == 0 && G == 0 && B == 0)
		return 7;
	else {

		int num = 0;
		int diff = 255 * 3;  

		for (int i = 1; i <= 255; i++) {
			int d = abs(R - (int)(dxfColors[i][0] * 255))
				+ abs(G - (int)(dxfColors[i][1] * 255))
				+ abs(B - (int)(dxfColors[i][2] * 255));

			if (d < diff) {
				diff = d;
				num = i;
				if (d == 0) {
					break;
				}
			}
		}
		return num;
	}
}

int	IEntity::colorToNumber24(int R, int G, int B, ColorType type)
{
	if (ColorType::ByBlock || ColorType::ByLayer)
		return -1;
	else 
		return   (R << 16) | (G << 8) | (B << 0);
}

int    IEntity::widthToNumber(LineWeight weight)
{
	switch (weight) {
	case LineWeight::WeightByLayer:
		return -1;
		break;
	case LineWeight::WeightByBlock:
		return -2;
		break;
	case LineWeight::WeightByLwDefault:
		return -3;
		break;
	default:
		break;
	}
	return (int)weight;
}