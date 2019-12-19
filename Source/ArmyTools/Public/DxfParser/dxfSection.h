#ifndef DXF_SECTION
#define DXF_SECTION 1

#include "dxfSectionBase.h"
#include "dxfTable.h"
#include "codeValue.h"
#include "dxfEntity.h"

#include <map>
#include <vector>
#include <iostream>
#include <string>

class dxfFile;

class dxfSection : public dxfSectionBase
{
public:
	dxfSection() {}
	virtual ~dxfSection() {}
};

class dxfHeader : public dxfSection
{
public:
	dxfHeader() : _inVariable(false) {}
	virtual ~dxfHeader() {}
	virtual void assign(dxfFile* dxf, codeValue& cv)override;
	VariableList& getVariable(std::string inVar) { return _variables[inVar]; }
protected:
	std::map<std::string, VariableList> _variables;
	bool _inVariable;
	std::string _currentVariable;
};


class dxfTables : public dxfSection
{
public:
	dxfTables() : _inLayerTable(false) { _layerTable = NULL; _currentTable = NULL; }
	virtual ~dxfTables() { 
		delete _layerTable; 
		delete _currentTable;
		std::vector<dxfTable*>::iterator iter = _others.begin();
		for(iter;iter!=_others.end();++iter)
		{
			delete *iter;
		}
		_others.clear();
	}
	virtual void assign(dxfFile* dxf, codeValue& cv);
	dxfLayerTable*    getOrCreateLayerTable()
	{
		if (!_layerTable)
			_layerTable = new dxfLayerTable;
		return _layerTable;
	}

protected:
	bool _inLayerTable;
	dxfLayerTable*              _layerTable;
	std::vector<dxfTable* >    _others;
	dxfTable*               _currentTable;

};
class scene;
class dxfEntities : public dxfSection
{
public:
	dxfEntities() : _currentEntity(NULL) {}
	virtual ~dxfEntities() { delete _currentEntity; }
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual void drawScene(scene* sc);

protected:
	dxfEntity*              _currentEntity;
	EntityList              _entityList;

};

class dxfBlocks : public dxfSection
{
public:
	dxfBlocks() : _currentBlock(NULL) {}
	virtual ~dxfBlocks() {}
	virtual void assign(dxfFile* dxf, codeValue& cv);
	dxfBlock* findBlock(std::string s);
	std::vector<dxfBlock* >& GetBlock() { return _blockList; }
protected:
	dxfBlock*    _currentBlock;
	std::map<std::string, dxfBlock*> _blockNameList;
	std::vector<dxfBlock* > _blockList;
};

#endif
