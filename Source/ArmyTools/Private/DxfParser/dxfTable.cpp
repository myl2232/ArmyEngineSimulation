
#include "dxfTable.h"
#include "dxfFile.h"
#include "codeValue.h"


void dxfLayer::assign(dxfFile*, codeValue& cv)
{
    switch (cv._groupCode) {
        case 2:
            _name = cv._string;
            break;
        case 62:
            _color = cv._short;
            if ((short)_color < 0) _frozen = true;
            break;
        case 70:
            _frozen = (bool)(cv._short & 1);
            break;
    }
}

void dxfLayerTable::assign(dxfFile* dxf, codeValue& cv)
{
    std::string s = cv._string;
    if (cv._groupCode == 0 ) {
        if (_currentLayer) {
            _layers[_currentLayer->getName()] = _currentLayer;
        }
        if (s == "LAYER") {
            _currentLayer = new dxfLayer;
        } // otherwise it's the close call from ENDTAB
    } else if (_currentLayer) {
        _currentLayer->assign(dxf, cv);
    }
}
