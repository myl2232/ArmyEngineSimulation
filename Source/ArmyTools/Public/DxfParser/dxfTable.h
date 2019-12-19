

#ifndef DXF_TABLE
#define DXF_TABLE 1

#include <string>
#include <map>




class dxfFile;
class codeValue;

// special case: the layer table

class dxfTable 
{
public:
	dxfTable() {}
	virtual ~dxfTable() {}
	virtual void assign(dxfFile* , codeValue& ) { }
};


class dxfLayer 
{
public:
	dxfLayer(std::string name = "0") : _name(name), _color(7), _frozen(false) {}
	virtual ~dxfLayer() {}
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual const std::string& getName() const { return _name; }
	virtual const unsigned short& getColor() const { return _color; }
	virtual void setName(const std::string& name) { _name = name; }
	const bool& getFrozen() const { return _frozen; }
protected:
	std::string	_name;
	unsigned short _color;
	bool			_frozen;
};

class dxfLayerTable : public dxfTable
{
public:
	dxfLayerTable() { _currentLayer = NULL; }
	virtual ~dxfLayerTable() { delete _currentLayer; _currentLayer = NULL; }
	virtual void assign(dxfFile* dxf, codeValue& cv);

	dxfLayer* findOrCreateLayer(std::string name)
	{
		if (name == "") name = "0"; // nowhere it is said "" is invalid, but...
		dxfLayer* layer = _layers[name];
		if (!layer) {
			layer = new dxfLayer();
			_layers[name] =layer;
		}
		return layer;
	}

protected:
	std::map<std::string, dxfLayer* > _layers;
	dxfLayer* _currentLayer;
};

#endif
