#pragma once
#include "Block.h"
#include "dl_dxf.h"
#include <unordered_map>
#include <memory>
#include "IEntity.h"
#include "LineType.h"
#include "Layer.h"
 
 
class AttributeEntity;
using namespace  std;
class Canvas :
	public Block
{
public:
	Canvas();
	virtual ~Canvas();
public:
	OBJECT_DELETE
	virtual bool asFile(const char*fileName);
 
	virtual void addLineType(ILineType*lineType);
 
	virtual void addLayer(AttributeEntity*entity);
 
	virtual void addBlock(IBlock*block);

	virtual void addTextStyle(ITextStyle*style);

	virtual void clear();
protected:
	void SectionHeader();

	void SectionClasses();

	void TableSectionStart();

	void LineTypeTable();

	void LayerTypeTable();

	void TextStyleTable();

	void OtherTable();

	void	 TableSectionEnd();

	void BlockSection();

	void EntitySection();

	void End();

protected:
	std::string  utf82gbk(const std::string& utf8);

	DL_Dxf*			dxf;

	DL_WriterA*		dw;
	 
	vector<ITextStyle*>														textStyleArray;

	vector<AttributeEntity*>												layerArray;

	std::vector<IBlock*>														blockArray;
};

