#include "ComponentReader.h"
#include "dl_dxf.h"
#include <memory>
#include "AttributeEntity.h"
ComponentReader::ComponentReader()
{
	factory = IFactory::createFactory();
}
ComponentReader::~ComponentReader()
{
	factory->release();
}

void ComponentReader::clearWithoutRelease()
{
	entityArray.clear();
}

bool ComponentReader::readBlockFile(const char*name)
{
	std::shared_ptr<DL_Dxf> reader = std::make_shared<DL_Dxf>();
	if (!reader->in(name, this))
		return false;
	processVertex();
	return true;
}
void ComponentReader::processVertex()
{
	FVector bench = FVector(BenchmarkPoint.x, BenchmarkPoint.y, BenchmarkPoint.z);
	for (auto i : lines)
	{
		FVector start = FVector(i.x1, i.y1, i.z1);
		FVector end = FVector(i.x2, i.y2, i.z2);
	 
		start = start - bench;
		end = end - bench;
		start.Y = -start.Y;
		end.Y = -end.Y;
		auto entity = factory->createLine(Double4(start.X, start.Y, start.Z), Double4(end.X, end.Y , end.Z));
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.colorType = Fixed;
		Desc.lineType = const_cast<char*>(attributes.getLineType().c_str());
		Desc.weight = (LineWeight)attributes.getWidth();
		Desc.color = attributes.getColor();
		Desc.color24 = attributes.getColor24();
		Desc.blockProperty = true;
		entity->setEntityAttribute(Desc);
		entityArray.push_back(entity);
	}

	for (auto i : points)
	{
		FVector start = FVector(i.x, i.y, i.z);
		start= start - bench;
		start.Y = -start.Y;
		auto entity = factory->createPoint(Double4(start.X, start.Y, start.Z));
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.colorType = Fixed;
		Desc.lineType = const_cast<char*>(attributes.getLineType().c_str());
		Desc.weight = (LineWeight)attributes.getWidth();
		Desc.color = attributes.getColor();
		Desc.color24 = attributes.getColor24();
		Desc.blockProperty = true;
		entity->setEntityAttribute(Desc);
		entityArray.push_back(entity);
	}
	for (auto i : arcs)
	{
		FVector start = FVector(i.cx, i.cy, i.cz);
		start = start - bench;
		start.Y = -start.Y;
		auto entity = factory->createArc(Double4(start.X, start.Y, start.Z), i.radius / 10, i.angle1, i.angle2);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.colorType = Fixed;
		Desc.lineType = const_cast<char*>(attributes.getLineType().c_str());
		Desc.weight = (LineWeight)attributes.getWidth();
		Desc.color = attributes.getColor();
		Desc.color24 = attributes.getColor24();
		Desc.blockProperty = true;
		entity->setEntityAttribute(Desc);
		entityArray.push_back(entity);
	}

	for (auto i : circlles)
	{
		FVector start = FVector(i.cx, i.cy, i.cz);
		start = start - bench;
		start.Y = -start.Y;
		auto entity = factory->createCircle(Double4(start.X, start.Y, start.Z), i.radius / 10);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.colorType = Fixed;
		Desc.lineType = const_cast<char*>(attributes.getLineType().c_str());
		Desc.weight = (LineWeight)attributes.getWidth();
		Desc.color = attributes.getColor();
		Desc.color24 = attributes.getColor24();
		Desc.blockProperty = true;
		entity->setEntityAttribute(Desc);
		entityArray.push_back(entity);

	}
	for (auto i : ellipses)
	{
		FVector start = FVector(i.cx, i.cy, i.cz);
		FVector end = FVector(i.mx, i.my, i.mz);
		 
		start = start - bench;
		end = end;
		start.Y = -start.Y;
		end.Y = -end.Y;

		FVector normal = FVector(i.nx, i.ny, i.nz);
		auto entity = factory->createEllipse(Double4(start.X, start.Y, start.Z), Double4(end.X, end.Y, end.Z),Double4(normal.X, normal.Y, normal.Z), i.ratio, i.angle1, i.angle2);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.colorType = Fixed;
		Desc.lineType = const_cast<char*>(attributes.getLineType().c_str());
		Desc.weight = (LineWeight)attributes.getWidth();
		Desc.color = attributes.getColor();
		Desc.color24 = attributes.getColor24();
		Desc.blockProperty = true;
		entity->setEntityAttribute(Desc);
		entityArray.push_back(entity);
	}
	for (auto i : polylines)
	{
		std::vector<Double4>	points;
		for (auto point : i)
		{
			FVector start = FVector(point.x, point.y, point.z);
			start = start - bench;
			start.Y = -start.Y;
			points.push_back(Double4(start.X, start.Y, start.Z));
		}
		 

		auto entity = factory->createPolygon(&points[0], (int)points.size(), PolygonType::P_OPEN);
		AttributeDesc Desc;
		FMemory::Memzero(Desc);
		Desc.colorType = Fixed;
		Desc.lineType = const_cast<char*>(attributes.getLineType().c_str());
		Desc.weight = (LineWeight)attributes.getWidth();
		Desc.color = attributes.getColor();
		Desc.color24 = attributes.getColor24();
		Desc.blockProperty = true;
		entity->setEntityAttribute(Desc);
		entityArray.push_back(entity);
	}
}


void ComponentReader::addLine(const DL_LineData&data)
{
	if (attributes.getLayer() != "0" || onReadBlock)
		return;
	lines.push_back(data);
}

void ComponentReader::addPoint(const DL_PointData&data)
{
	if (attributes.getLayer() != "0" || onReadBlock)
		return;
	int color = attributes.getColor24();
	int red, green, blue = 0;

	red = (color & 0x00ff0000) >> 16;
	green = (color & 0x0000ff00) >> 8,
		blue = (color & 0x000000ff) >> 0;
	if (red &1 )
	{
		BenchmarkPoint = data;
		return;
	}
	points.push_back(data);
}

void ComponentReader::addArc(const DL_ArcData&data)
{
	if (attributes.getLayer() != "0" || onReadBlock)
		return;
	arcs.push_back(data);
}

void ComponentReader::addCircle(const DL_CircleData&data)
{
	if (attributes.getLayer() != "0" || onReadBlock)
		return;
	circlles.push_back(data);
 }
void ComponentReader::addEllipse(const DL_EllipseData&data)
{
	if (attributes.getLayer() != "0" || onReadBlock)
		return;
	ellipses.push_back(data);
}
void ComponentReader::addPolyline(const DL_PolylineData&data)
{
	vertexNum = data.number;
 }
void ComponentReader::addVertex(const DL_VertexData&data)
{
	if (attributes.getLayer() != "0" || onReadBlock)
		return;
	vertexes.push_back(data);
	if (--vertexNum == 0)
	{ 
		polylines.push_back(std::move(vertexes));
	}
}
 
TArray<AttributeEntity*>	ComponentReader::getEntity()
{
	TArray<AttributeEntity*> Out;
	for (auto entity : entityArray)
		Out.Add(entity);
	return Out;
}
 
Double4		ComponentReader::getBenchmarkPoint()
{
	return Double4(BenchmarkPoint.x, BenchmarkPoint.y, BenchmarkPoint.z);
}