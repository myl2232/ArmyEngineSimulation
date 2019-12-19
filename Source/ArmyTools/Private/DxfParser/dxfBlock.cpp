


#include "dxfBlock.h"
#include "dxfFile.h"
#include "codeValue.h"
#include "dxfEntity.h"



 
dxfBlock::~dxfBlock()
{
	delete _currentEntity;
}

void dxfBlock::assign(dxfFile* dxf, codeValue& cv)
{
    std::string s = cv._string;
    if (cv._groupCode == 0) {
        if (_currentEntity && _currentEntity->done()) {
            _currentEntity = new dxfEntity(s);
            _entityList.push_back(*_currentEntity);
        } else if (_currentEntity) {
            _currentEntity->assign(dxf, cv);
        } else {
            _currentEntity = new dxfEntity(s);
            _entityList.push_back(*_currentEntity);
        }
    } else if (_currentEntity) {
        _currentEntity->assign(dxf, cv);
    } else if (cv._groupCode != 0) {
        switch (cv._groupCode) {
            case 2:
                _name = s;
                break;
            case 10:
                _position.X = cv._double;
                break;
            case 20:
                _position.Y = cv._double;
                break;
            case 30:
                _position.Z = cv._double;
                break;
            default:
                // dxf garble
                break;
        }
    }
}

const FVector& dxfBlock::getPosition() const { return _position; }
