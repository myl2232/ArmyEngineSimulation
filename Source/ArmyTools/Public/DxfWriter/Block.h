#pragma once
#include "IBlock.h"
#include "IEntity.h"
#include <vector>
#include <string>

class Block :
	public IBlock
{
public:
	Block();
	virtual ~Block();
	friend class Canvas;
public:
	OBJECT_DELETE
	virtual bool asFile(const char*fileName) { return false; }

	virtual void addLineType(ILineType*lineType) {};

	virtual void addLayer(AttributeEntity*entity) {};

	virtual void addBlock(IBlock*block) {};
	
	virtual void addTextStyle(ITextStyle*style) {};

	virtual void drawEntity(AttributeEntity*entity);
	
	virtual void	asDXF(DL_WriterA*dw, DL_Dxf*dxf);

	virtual void  asHacth(DL_WriterA*dw, DL_Dxf*dxf);
 
	void		setBlockInfo(const Double4&point,const std::string &name);

	virtual void clear();
protected:
	std::vector<AttributeEntity*>					entitysArray;
	std::string													blockName;
	Double4													offset;
};

