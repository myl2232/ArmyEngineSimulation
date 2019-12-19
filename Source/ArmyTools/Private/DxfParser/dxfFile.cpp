
#include "dxfFile.h"
#include "dxfReader.h"
#include "dxfBlock.h"
#include "dxfEntity.h"
#include "dxfDataTypes.h"
#include "scene.h"
#include "codeValue.h"




bool
dxfFile::parseFile()
{
    if (_fileName == "") return false;
    _reader = new dxfReader;

    if (_reader->openFile(_fileName)) {
        codeValue cv;
        while(_reader->nextGroupCode(cv)) {
            short result = assign(cv);
			if (result < 0)
			{
				_reader->closeFile();
				return false;
			}
            else if (result == 0) {
				_reader->closeFile();
                return true;
            }
        }
        // we did not reach 0 EOF
		_reader->closeFile();
        return false;
    } else {
        return false;
    }
}
bool dxfFile::GetLines(TArray<FVector2D>& Points,FVector2D& vMin,FVector2D& vMax)
{
	if (!_entities) return NULL;
	if (!_tables) 
		_tables = new dxfTables;
	dxfLayerTable* layerTable = _tables->getOrCreateLayerTable();
	_scene = new scene(layerTable);
	_entities->drawScene(_scene);
	FBox b = _scene->GetBounds();
	vMin.X = b.Min.X;
	vMin.Y = b.Min.Y;
	vMax.X = b.Max.X;
	vMax.Y = b.Max.Y;
	for (auto&it:_scene->Getlayers() )
	{
		TSharedPtr<sceneLayer>& p = it.second;
		if ( p.IsValid() )
		{
			auto& Vlist = p->_linestrips;
			for (MapVListList::iterator mlitr = Vlist.begin();mlitr != Vlist.end();++mlitr)
			{
				for (VListList::iterator itr = mlitr->second.begin();itr != mlitr->second.end();++itr)
				{
					if (itr->size())
					{
						for (VList::iterator vitr = itr->begin();vitr != itr->end(); ++vitr)
						{
							FVector2D v(vitr->X*0.1f, vitr->Y*0.1f);
							Points.Add(v);
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}
"FArmyDxfModel*
dxfFile::DxfToHomeEditorData()
{
    if (!_entities) return NULL;
    if (!_tables) { // a dxfTable is needed to create undefined layers
        _tables = new dxfTables;
    }
    dxfLayerTable* layerTable = _tables->getOrCreateLayerTable();
    // to do: should be more general and pass a pointer to ourselves
    // which in turn should be able to feed any information
    // the scene might need
    _scene = new scene(layerTable);
    _entities->drawScene(_scene);
	"FArmyDxfModel* g = _scene->SceneToHomeDrawerData();
	
	g->boundingBox = _scene->GetBounds();
	
    return g;

}
dxfBlock*
dxfFile::findBlock(std::string name)
{
    if (_blocks)
        return _blocks->findBlock(name);
    return NULL;
}

/// not used. if you want to know what a header variable
/// contains, call this. pass the complete variable name
/// including "$", for example: "$EXTMAX"
VariableList
dxfFile::getVariable(std::string var)
{
    return _header->getVariable(var);
}

/// parse the dxf sections
short
dxfFile::assign(codeValue& cv)
{
    std::string s = cv._string;
    if (cv._groupCode == 0 && s == std::string("ENDSEC")) {
        _isNewSection = false;
        _current = _unknown;
    } else if (cv._groupCode == 0 && s == std::string("SECTION")) {
        _isNewSection = true;
    } else if (cv._groupCode == 0 && s == std::string("EOF")) {
        return 0;
    } else if (cv._groupCode == 999) {    // skip comments
    } else if (cv._groupCode == 2 && _isNewSection) {
        _isNewSection = false;
//        std::cout << "Reading section " << s << std::endl;
        if (s =="HEADER") {
            _header = new dxfHeader;
            _current = _header;
        } else if (s =="TABLES") {
            _tables = new dxfTables;
            _current = _tables;
        } else if (s =="BLOCKS") {
            _blocks = new dxfBlocks;
            _current = _blocks;
        } else if (s =="ENTITIES") {
            _entities = new dxfEntities;
            _current = _entities;
        } else {
            _current = _unknown;
        }
    } else if (_isNewSection) {
        // problem. a 0/SECTION should be followed by a 2/SECTION_NAME
        std::cout << "No groupcode for changing section " << cv._groupCode << " value: " << s << std::endl;
        return -1;
    } else if (_current) {
        _current->assign(this, cv);
    }
    return 1;
}
