#pragma  once
#include "EntityCommonType.h"
#include "IBlock.h"
class AttributeEntity;

class  IFactory
{
public:
	IFactory();
	virtual ~IFactory();
public:
	static IFactory*		createFactory();

	virtual AttributeEntity* createLayer(const char*layerName,int flags)=0;

	virtual ILineType*		createLineType()=0;

	virtual ITextStyle*		createTextStyle(const TextStyleDesc &desc)=0;

	virtual AttributeEntity* createPoint(const Double4& point) = 0;

	virtual AttributeEntity* createLine(const Double4& start,const Double4& end) = 0;

	virtual AttributeEntity* createCircle(const Double4&centre, double radius)=0;

	virtual AttributeEntity* createArc(const Double4&centre, double radius, double startAngle, double endAngle) = 0;

	virtual AttributeEntity* createPolyline(const Double4*points, int PointsNum)=0;

	virtual AttributeEntity* createPolygon(const Double4*points, int PointsNum, PolygonType isClosed) = 0;

	virtual AttributeEntity* createEllipse(const Double4&centre, const Double4&major, const Double4&narmal, double ratio, double startAngle, double endAngle) = 0;

	virtual AttributeEntity* createSpline() = 0;

	virtual AttributeEntity*createSimpleText(const TextDesc&desc) = 0;

	virtual AttributeEntity*createMText(const MTextDesc&desc) = 0;
 
	virtual AttributeEntity* createHatch(int loops, bool solid, double scale, double angle, const char*pattern) = 0;

	virtual AttributeEntity*createBlockReference(const char*name, const Double4&insertPoint, const Double4 &scale, double angle, int columns, int rows, double columnSpace, double rowSpace)=0;

	virtual AttributeEntity* createDimAligned(const DimensionDesc&desc,const Double4& point1, const Double4& point2)=0;

	virtual AttributeEntity* createDimAngular(const DimensionDesc&desc, const Double4& Line1Start, const Double4& Line1End, const Double4& Line2Start, const Double4& Location) = 0;

	virtual  AttributeEntity*createDimDiametric(const DimensionDesc &desc, const Double4&point, double length)=0;

	virtual  AttributeEntity*createDimRadial(const DimensionDesc&desc, const Double4&point, double length) = 0;

	virtual AttributeEntity* createDimLinear(const DimensionDesc &desc, const Double4&point1, const Double4&point2, double angle, double oblique)=0;

	virtual AttributeEntity* createDimOrdinate(const DimensionDesc&desc, const Double4&point1, const Double4&point2, bool xType) = 0;

	virtual AttributeEntity* createDimMLeader(const Double4&BasePoint, const Double4&MidPoint, const Double4&StartPoint, const Double4&FontPoint,const Double4&FontDirection, double FontSize, double arrowSize, const char*context) = 0;

	virtual IBlock*	createBlockCanvas(const char*blockName,const Double4&offset) = 0;

	virtual IBlock*	createCanvas() = 0;

	virtual void release() = 0;
};

 