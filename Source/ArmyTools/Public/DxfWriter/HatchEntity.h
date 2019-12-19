#pragma once
#include "IEntity.h"
#include "IBlock.h"
#include <vector>
using namespace std;
class HatchEntity:public  IEntity,public IBlock
{
public:
	HatchEntity();
	virtual ~HatchEntity();

	OBJECT_DELETE

	virtual bool asFile(const char*fileName) { return true; };

	virtual void drawEntity(AttributeEntity*entity) {};

	virtual void addLineType(ILineType*lineType) {}

	virtual void addLayer(AttributeEntity*entity) {}

	virtual void addTextStyle(ITextStyle*style) {};

	virtual void addBlock(IBlock*block) { IblockArray.push_back(block); };

	virtual void asHatch(DL_WriterA*dw, DL_Dxf*dxf) { return; }

	virtual void  asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	void		setHatchInfo(int loops, bool solid,double scale,double angle,const char*pattern);

	virtual void clear();
protected:

	vector<IBlock*>		IblockArray;

	/*! Number of boundary paths (loops). */
	int numLoops;

	/*! Solid fill flag (true=solid, false=pattern). */
	bool solid;

	/*! Pattern scale or spacing */
	double scale;

	/*! Pattern angle */
	double angle;

	/*! Pattern name. */
	string pattern;

	/**!origin */
	Double4 xy;
};

