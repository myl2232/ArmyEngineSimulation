#pragma once
#include "IFactory.h"
#include <memory>

class Factory :
	public IFactory
{
public:
	Factory();
	virtual ~Factory();

public:
	virtual AttributeEntity* createLayer(const char*layerName, int flags);

	virtual ILineType*		createLineType() ;

	virtual ITextStyle*		 createTextStyle(const TextStyleDesc &desc);

	virtual AttributeEntity* createPoint(const Double4& point);

	virtual AttributeEntity* createLine(const Double4& start, const Double4& end);

	virtual AttributeEntity* createCircle(const Double4&centre, double radius);

	virtual AttributeEntity* createArc(const Double4&centre, double radius, double startAngle, double endAngle);

	virtual AttributeEntity* createPolyline(const Double4*points, int PointsNum);

	virtual AttributeEntity* createPolygon(const Double4*points, int PointsNum, PolygonType isClosed);

	virtual AttributeEntity* createEllipse(const Double4&centre, const Double4&major, const Double4&narmal, double ratio, double startAngle, double endAngle);

	virtual AttributeEntity* createHatch(int loops, bool solid, double scale, double angle, const char*pattern);

	virtual AttributeEntity* createSpline();

	virtual AttributeEntity*createSimpleText(const TextDesc&desc);

	virtual AttributeEntity*createMText(const MTextDesc&desc);

	virtual AttributeEntity*createBlockReference(const char*name, const Double4&insertPoint, const Double4 &scale, double angle, int columns, int rows, double columnSpace, double rowSpace);
 
	virtual AttributeEntity* createDimAligned(const DimensionDesc&desc, const Double4& point1, const Double4& point2);

	virtual AttributeEntity* createDimAngular(const DimensionDesc&desc, const Double4& Line1Start, const Double4& Line1End, const Double4& Line2Start, const Double4& Location);

	virtual  AttributeEntity*createDimDiametric(const DimensionDesc &desc, const Double4&point, double length);

	virtual  AttributeEntity*createDimRadial(const DimensionDesc&desc, const Double4&point, double length) ;

	virtual AttributeEntity* createDimLinear(const DimensionDesc &desc, const Double4&point1, const Double4&point2, double angle, double oblique);

	virtual AttributeEntity* createDimOrdinate(const DimensionDesc&desc, const Double4&point1, const Double4&point2, bool xType);

	virtual AttributeEntity* createDimMLeader(const Double4&BasePoint, const Double4&MidPoint, const Double4&StartPoint, const Double4&FontPoint, const Double4&FontDirection, double FontSize, double arrowSize, const char*context);

	virtual IBlock*	createBlockCanvas(const char*blockName, const Double4&offset);

	virtual IBlock*	createCanvas();

 

	virtual void release();
};

