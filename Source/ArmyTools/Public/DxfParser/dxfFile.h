

#ifndef DXF_FILE
#define DXF_FILE 1



#include <iostream>
#include <string>

#include "dxfSectionBase.h"
#include "dxfReader.h"
#include "dxfSection.h"
#include "scene.h"
#include "codeValue.h"

class ARMYTOOLS_API dxfFile {
public:
    dxfFile(std::string fileName) :
                _fileName(fileName),
                _isNewSection(false)
    {
		_reader = NULL;
		_current = NULL;
		_header = NULL;
		_tables = NULL;
		_blocks = NULL;
		_entities = NULL;
		_unknown = NULL;
		_scene = NULL;
	}
    bool            parseFile();
    "FArmyDxfModel*        DxfToHomeEditorData();
    dxfBlock*        findBlock(std::string name);
    VariableList    getVariable(std::string var);

	bool GetLines(TArray<FVector2D>& Points,FVector2D& vMin,FVector2D& vMax);
protected:

    short assign(codeValue& cv);
    std::string                 _fileName;
    bool                        _isNewSection;
    dxfReader*    _reader;
    dxfSection*    _current;
    dxfHeader*     _header;
    dxfTables*    _tables;
    dxfBlocks*    _blocks;
    dxfEntities*   _entities;
    dxfSection*    _unknown;
    scene*         _scene;
};


#endif
