#pragma once
#include "dl_creationadapter.h"
#include "IFactory.h"
#include <vector>
#include "CoreMinimal.h"
class ComponentReader :
	public DL_CreationAdapter
{
public:
	ComponentReader();
	virtual ~ComponentReader();
	virtual void clearWithoutRelease();
	virtual bool readBlockFile(const char*);
	virtual void addLine(const DL_LineData&);
	virtual void addArc(const DL_ArcData&);
	virtual void addCircle(const DL_CircleData&);
	virtual void addEllipse(const DL_EllipseData&);
	virtual void addPolyline(const DL_PolylineData&);
	virtual void addVertex(const DL_VertexData&);
	virtual void addPoint(const DL_PointData&);
	virtual void addBlock(const DL_BlockData&) { onReadBlock = true; };
	virtual void endBlock() { onReadBlock = false; }
	virtual void processVertex();
	TArray<AttributeEntity*>	getEntity();
	Double4		getBenchmarkPoint();
private:
	std::vector<DL_LineData>		lines;
	std::vector<DL_ArcData>		arcs;
	std::vector<DL_CircleData>	circlles;
	std::vector<DL_EllipseData>	ellipses;
	std::vector<std::vector<DL_VertexData>>polylines;
	std::vector<DL_PointData>	points;



	bool																			onReadBlock = false;
	DL_PointData 															BenchmarkPoint;
	int																			vertexNum;
	std::vector<DL_VertexData>									vertexes;
	std::vector<AttributeEntity*>									entityArray;
	IFactory*																	factory;
};

