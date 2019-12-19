#include "Factory.h"
#include "PointEntity.h"
#include "LineEntity.h"
#include "CircleEntity.h"
#include "ArcEntity.h"
#include "Layer.h"
#include "LineType.h"
#include "Canvas.h"
#include "HatchEntity.h"
#include "PolygonEntity.h"
#include "TextEntity.h"
#include "Block.h"
#include "BlockEntity.h"
#include "MTextEntity.h"
#include "DimAlignedEntity.h"
#include "DimAngularEntity.h"
#include "DimDiametricEntity.h"
#include "DimRadialEntity.h"
#include "DimLinearEntity.h"
#include "DimOrdinateEntity.h"
#include "TextStyle.h"
#include "EllipseEntity.h"
#include "PolylineEntity.h"
#include "DimMLeader.h"

Factory::Factory()
{
}


Factory::~Factory()
{
}
 

AttributeEntity* Factory::createLayer(const char*layerName, int flags)
{
	auto layer = new Layer();
	layer->setLayer(layerName, flags);
	return layer;
}

ILineType*Factory::createLineType()
{
	return new LineType();
}

ITextStyle* Factory::createTextStyle(const TextStyleDesc &desc)
{
	auto style = new TextStyle();
	style->setTextStyle(desc);
	return style;
}

AttributeEntity* Factory::createPoint(const Double4& point)
{
	auto pointEntity =	new PointEntity();
	pointEntity->setPoint(point.x, point.y,point.z);
	return pointEntity;
}

AttributeEntity* Factory::createLine(const Double4& start, const Double4& end)
{
	auto lineEntity = new LineEntity();
	lineEntity->setLines(start, end);
	return lineEntity;
}


AttributeEntity* Factory::createCircle(const Double4&centre, double radius)
{
	auto circleEntity = new CircleEntity();
	circleEntity->setCircle(centre, radius);
	return circleEntity;
}

AttributeEntity* Factory::createArc(const Double4&centre, double radius, double startAngle, double endAngle)
{
	auto arcEntity = new ArcEntity();
	arcEntity->setArc(startAngle, endAngle);
	arcEntity->setCircle(centre, radius);
	return arcEntity;
}

AttributeEntity* Factory::createPolyline(const Double4*points, int PointsNum)
{
	auto sEntity = new PolylineEntity();
	sEntity->setPolyline((Double4*)points, PointsNum);
	return sEntity;
}

AttributeEntity* Factory::createPolygon(const Double4*points, int PointsNum, PolygonType isClosed)
{
	auto polygon=  new PolygonEntity();
	polygon->setPolygonInfo(PointsNum, points, isClosed);
	return polygon;
}

AttributeEntity* Factory::createEllipse(const Double4&centre, const Double4&major, const Double4&narmal, double ratio, double startAngle, double endAngle)
{
	auto ellipse = new EllipseEntity();
	ellipse->setEllipse(centre, major, narmal, startAngle, endAngle, ratio);
	return ellipse;
}

AttributeEntity* Factory::createHatch(int loops, bool solid, double scale, double angle, const char*pattern)
{
	auto  entity = new HatchEntity();
	entity->setHatchInfo(loops, solid, scale, angle, pattern);
	return entity;
}

AttributeEntity* Factory::createSpline()
{
	return NULL;
}

AttributeEntity*Factory::createSimpleText(const TextDesc&desc)
{
	auto text = new TextEntity();
	text->setTextInfo(desc);
	return text;
}

AttributeEntity*Factory::createMText(const MTextDesc&desc)
{
	auto text = new MTextEntity();
	text->setTextInfo(desc);
	return text;
}

AttributeEntity*Factory::createBlockReference(const char*name, const Double4&insertPoint, const Double4 &scale, double angle, int columns, int rows, double columnSpace, double rowSpace)
{
	auto blockRef =new BlockEntity();
	blockRef->setBlockRefInfo(name, insertPoint,scale,  angle,columns, rows, columnSpace, rowSpace);
	return blockRef;
}


AttributeEntity* Factory::createDimAligned(const DimensionDesc&desc, const Double4& point1, const Double4& point2)
{
	auto dimAligned = new DimAlignedEntity();
	dimAligned->setDimAlignedInfo(desc, point1, point2);
	return dimAligned;
}

AttributeEntity* Factory::createDimAngular(const DimensionDesc&desc, const Double4& Line1Start, const Double4& Line1End, const Double4& Line2Start, const Double4& Location)
{

	auto dimAngular= new DimAngularEntity();
	dimAngular->setDimAngularInfo(desc,Line1Start, Line1End, Line2Start, Location);
	return dimAngular;
}

AttributeEntity*Factory::createDimDiametric(const DimensionDesc &desc, const Double4&point, double length)
{
	auto dimDiametric = new DimDiametricEntity();
	dimDiametric->setDimDiametricInfo(desc, point, length);
	return dimDiametric;
}

AttributeEntity*Factory::createDimRadial(const DimensionDesc&desc, const Double4&point, double length)
{
	auto dimRadial = new DimRadialEntity();
	dimRadial->setDimRadialInfo(desc, point, length);
	return dimRadial;
}

AttributeEntity * Factory::createDimLinear(const DimensionDesc & desc, const Double4 & point1, const Double4 & point2, double angle, double oblique)
{
	auto dimLinear = new DimLinearEntity();
	dimLinear->setDimLinearInfo(desc, point1, point2, angle, oblique);
	return dimLinear;
}

AttributeEntity * Factory::createDimOrdinate(const DimensionDesc & desc, const Double4 & point1, const Double4 & point2, bool xType)
{
	auto dimOrdinate = new DimOrdinateEntity();
	dimOrdinate->setDimOrdinateInfo(desc, point1, point2, xType);
	return dimOrdinate;
}

AttributeEntity* Factory::createDimMLeader(const Double4&BasePoint, const Double4&MidPoint, const Double4&StartPoint, const Double4&FontPoint, const Double4&FontDirection, double FontSize, double arrowSize, const char*context)
{
	auto dimMLeader = new DimMLeader();
	dimMLeader->setDimMLeaderInfo(BasePoint, MidPoint, StartPoint, FontPoint, FontDirection, FontSize, arrowSize, context);
	return dimMLeader;
}

IBlock*	Factory::createBlockCanvas(const char*blockName, const Double4&offset)
{
	auto block= new Block();
	block->setBlockInfo(offset, blockName);
	return block;
}
	
IBlock*		Factory::createCanvas()
{
	return new Canvas();
}

void Factory::release()
{
	delete this;
}