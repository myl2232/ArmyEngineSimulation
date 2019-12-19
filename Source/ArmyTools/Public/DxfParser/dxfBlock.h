

#ifndef DXF_BLOCK
#define DXF_BLOCK 1

#include <map>
#include <vector>
#include <string>



class dxfFile;
class codeValue;
class dxfEntity;

typedef std::vector<dxfEntity> EntityList;

class dxfBlock 
{
public:
    dxfBlock() : _currentEntity(NULL) {}
	virtual ~dxfBlock();
    inline const std::string& getName() const { return _name; }
    virtual void assign(dxfFile* dxf, codeValue& cv) ;
    EntityList& getEntityList() { return _entityList; }
    const FVector& getPosition() const;

protected:
    EntityList _entityList;
    dxfEntity* _currentEntity;
    std::string _name;
	FVector _position;
};

#endif
