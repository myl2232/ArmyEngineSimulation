
#ifndef DXF_SCENE
#define DXF_SCENE 1

#include "Matrixf.h"
#include "Primitive2D/ArmyPoint.h"
#include "Primitive2D/ArmyPolyline.h"
#include "Data/ArmyDxfModel.h"
#include "Data/ArmyFurniture.h"
#include "dxfEntity.h"
#include "dxfTable.h"
#include<vector>
#include<map>
class dxfLayerTable;

class bounds {
public:
	bounds() : _min(MAX_FLT, MAX_FLT, MAX_FLT), _max(-MAX_FLT, -MAX_FLT, -MAX_FLT) {}
	inline void expandBy(const FVector & v) {
		if (v.Y <= 100 && v.Y >= 0)
		{

		}
		if (v.X < _min.X) _min.X = v.X;
		if (v.X > _max.X) _max.X = v.X;

		if (v.Y < _min.Y) _min.Y = v.Y;
		if (v.Y > _max.Y) _max.Y = v.Y;

		if (v.Z < _min.Z) _min.Z = v.Z;
		if (v.Z > _max.Z) _max.Z = v.Z;
	}
	inline void makeMinValid() {
		if (_min.X == MAX_FLT) _min.X = 0;
		if (_min.Y == MAX_FLT) _min.Y = 0;
		if (_min.Z == MAX_FLT) _min.Z = 0;
	}
	FVector _min;
	FVector _max;
};

"FArmyPoint static inline CreatePointGeometry(FVector Pos, FLinearColor color)
{
	"FArmyPoint point;
	point.Pos = Pos;
	point.SetBaseColor(color);
	return point;
}
TSharedPtr<"FArmyPolyline> static inline CreatePolyLineGeometry(TArray<FVector>& Vertices, FLinearColor color, "FArmyPolyline::ELineType lintype)
{
	TSharedPtr<"FArmyPolyline> poly = MakeShareable(new "FArmyPolyline());
	poly->SetVertices(Vertices);
	poly->SetColor(color);
	poly->SetLineType(lintype);
	return poly;
}
typedef std::vector<FVector> VList;
typedef std::map<unsigned short, VList> MapVList;
typedef std::vector<VList> VListList;
typedef std::map<unsigned short, VListList> MapVListList;

struct PointInfo
{
	PointInfo(FVector InPosition, int32 color)
	{
		Position = InPosition;
		EntityColor = color;
	}
	FVector Position;
	int32 EntityColor;
};
typedef std::map<unsigned short, std::vector<PointInfo>> PointMapList;
typedef std::map<unsigned short, std::vector<"FArmyLWPolyLine>> LWPolyLines;

class sceneLayer {
public:
	sceneLayer(std::string name) : _name(name) {
		_linestrips.clear();
		_points.clear();
		_lines.clear();
		_triangles.clear();
		_trinorms.clear();
		_quads.clear();
		_quadnorms.clear();
	}
	virtual ~sceneLayer() {}
	void LayerToHomeEditor("FArmyFurniture* root, bounds &b)
	{
		ConvertPoint(root, b);
		ConvertLines(root, b);
		ConvertTriangles(root, b);
		ConvertQuads(root, b);
		ConvertText(root, b);
	}
	void LayerToHome("FArmyDxfLayer* dxfLayer, bounds& b)
	{
		ConvertLwPloyLine(dxfLayer, b);
	}
	LWPolyLines  _lwPolyLines;
	MapVListList    _linestrips;
	PointMapList    _points;
	MapVList        _lines;
	MapVList        _triangles;
	MapVList        _trinorms;
	MapVList        _quads;
	MapVList        _quadnorms;

	struct textInfo
	{
		textInfo(short int color, FVector point/*, osgText::Text *text*/) :
			_color(color), _point(point) /*_text(text)*/ {};
		short int _color;
		FVector _point;
		//TSharedPtr<osgText::Text> _text;
	};

	typedef std::vector<textInfo> TextList;
	TextList _textList;

protected:
	std::string        _name;

	FLinearColor        getColor(unsigned short color);

	void ConvertLwPloyLine("FArmyDxfLayer* dxfLayer, bounds &b)
	{
		for (LWPolyLines::iterator iter = _lwPolyLines.begin(); iter != _lwPolyLines.end(); ++iter)
		{
			for (std::vector<"FArmyLWPolyLine>::iterator iterInner = iter->second.begin();
				iterInner != iter->second.end(); ++iterInner)
			{
				dxfLayer->PloyLines.Push(*iterInner);
			}
		}
	}
	void ConvertPoint("FArmyFurniture* root, bounds &b)
	{

		for (PointMapList::iterator mitr = _points.begin();
			mitr != _points.end(); ++mitr)
		{
			TArray<FVector> coords;

			for (std::vector<PointInfo>::iterator itr = mitr->second.begin();
				itr != mitr->second.end(); ++itr)
			{
				//	FVector v(itr->X - b._min.X, itr->Y - b._min.Y, itr->Z - b._min.Z);
				FVector v = itr->Position - b._min;
				coords.Push(v);
				v *= FVector(0.1f, 0.1f, 0.1f);
				TSharedPtr<"FArmyPoint> point = MakeShareable(new "FArmyPoint());
				point->Size = 10.0f;
				point->bSprite = true;
				point->Pos = v;

				point->PropertyFlag = itr->EntityColor;
				root->AddPoint(point);
			}
		}
	}

	void ConvertLines("FArmyFurniture* root, bounds &b)
	{
		for (MapVListList::iterator mlitr = _linestrips.begin();
			mlitr != _linestrips.end();
			++mlitr)
		{
			for (VListList::iterator itr = mlitr->second.begin();
				itr != mlitr->second.end();
				++itr)
			{
				if (itr->size()) {
					TArray<FVector> coords;
					for (VList::iterator vitr = itr->begin();
						vitr != itr->end(); ++vitr) {
						FVector v(vitr->X - b._min.X, vitr->Y - b._min.Y, vitr->Z - b._min.Z);
						v *= FVector(0.1f, 0.1f, 0.1f);
						coords.Push(v);
					}
					root->AddPoly(CreatePolyLineGeometry(coords, getColor(mlitr->first), "FArmyPolyline::LineStrip));
				}
			}
		}
		for (MapVList::iterator mitr = _lines.begin();
			mitr != _lines.end(); ++mitr) {

			TArray<FVector> coords;
			for (VList::iterator itr = mitr->second.begin();
				itr != mitr->second.end(); ++itr) {
				FVector v(itr->X - b._min.X, itr->Y - b._min.Y, itr->Z - b._min.Z);
				v *= FVector(0.1f, 0.1f, 0.1f);
				coords.Push(v);
			}
			root->AddPoly(CreatePolyLineGeometry(coords, getColor(mitr->first), "FArmyPolyline::Lines));
		}
	}

	void ConvertTriangles("FArmyFurniture* root, bounds &b)
	{
		if (_triangles.size()) {
			for (MapVList::iterator mitr = _triangles.begin();
				mitr != _triangles.end(); ++mitr) {
				TArray<FVector> coords;
				VList::iterator itr;
				for (itr = mitr->second.begin();
					itr != mitr->second.end(); ++itr)
				{
					FVector v(itr->X - b._min.X, itr->Y - b._min.Y, itr->Z - b._min.Z);
					coords.Push(v);
				}
				TArray<FVector> norms;
				VList& normlist = _trinorms[mitr->first];
				for (itr = normlist.begin();
					itr != normlist.end(); ++itr)
				{
					FVector norm(itr->X, itr->Y, itr->Z);
					for (int i = 0; i < 3; ++i)
						norms.Push(norm);
				}
				// to do add triangle
			}
		}
	}
	void ConvertQuads("FArmyFurniture* root, bounds &b)
	{
		if (_quads.size()) {
			for (MapVList::iterator mitr = _quads.begin();
				mitr != _quads.end(); ++mitr) {

				TArray<FVector> coords;
				VList::iterator itr;
				for (itr = mitr->second.begin();
					itr != mitr->second.end(); ++itr) {
					FVector v(itr->X - b._min.X, itr->Y - b._min.Y, itr->Z - b._min.Z);
					coords.Push(v);
				}
				TArray<FVector> norms;
				VList& normlist = _quadnorms[mitr->first];
				for (itr = normlist.begin();
					itr != normlist.end(); ++itr) {
					FVector norm(itr->X, itr->Y, itr->Z);
					for (int i = 0; i < 4; ++i)
						norms.Push(norm);
				}
			}
		}
	}
	void ConvertText("FArmyFurniture* root, bounds &b)
	{
		if (_textList.size()) {
			for (TextList::iterator titr = _textList.begin();
				titr != _textList.end(); ++titr) {
				//titr->_text->setColor(getColor(titr->_color));
				FVector v1 = titr->_point;
				FVector v2(v1.X - b._min.X, v1.Y - b._min.Y, v1.Z - b._min.Z);
				//titr->_text->setPosition(v2);
				// root->addChild(createModel(_name, titr->_text.get()));
			}
		}
	}
};


class scene {
public:
	scene(dxfLayerTable* lt = NULL);
	virtual ~scene() {
		delete _layerTable;
		_layerTable = NULL;

		_layers.clear();
	}
	void setLayerTable(dxfLayerTable* lt);
	void pushMatrix(const Matrix44& m, bool protect = false)
	{
		_mStack.push_back(_m);
		if (protect) // equivalent to setMatrix
			_m = m;
		else
			_m = _m * m;
	}
	void popMatrix()
	{
		_mStack.pop_back();
		if (_mStack.size())
			_m = _mStack.back();
		else
			_m.makeIdentity();
	}
	void ocs(const Matrix44& r)
	{
		_r = r;
	}
	void blockOffset(const FVector& t)
	{
		_t = t;
	}
	void ocs_clear()
	{
		_r.makeIdentity();
	}
	Matrix44& backMatrix() { if (_mStack.size()) return _mStack.back(); else return _m; }
	FVector addVertex(FVector v);
	FVector addNormal(FVector v);
	sceneLayer* findOrCreateSceneLayer(const std::string & l)
	{
		TSharedPtr<sceneLayer> ly = _layers[l];
		if (!ly.IsValid()) {
			ly = MakeShareable(new sceneLayer(l));
			_layers[l] = ly;
		}
		return ly.Get();
	}
	unsigned short correctedColorIndex(const std::string & l, unsigned short color);

	void addPoint(const std::string & l, unsigned short color, FVector & s, int32 entityColor);
	void addLine(const std::string & l, unsigned short color, FVector & s, FVector & e);
	void addLineStrip(const std::string & l, unsigned short color, std::vector<FVector> & vertices);
	void addLineLoop(const std::string & l, unsigned short color, std::vector<FVector> & vertices);
	void addTriangles(const std::string & l, unsigned short color, std::vector<FVector> & vertices, bool inverted = false);
	void addQuads(const std::string & l, unsigned short color, std::vector<FVector> & vertices, bool inverted = false);
	void addText(const std::string & l, unsigned short color, FVector & point/*, osgText::Text *text*/);


	void AddLwPolyLine(const std::string& l, unsigned short color, dxfLWPolyline* polyLine);
	"FArmyDxfModel* SceneToHomeDrawerData()
	{
		"FArmyDxfModel* root = new "FArmyDxfModel();
		_b.makeMinValid();
		FVector v = FVector(_b._min.X, _b._min.Y, _b._min.Z);
		double x = _b._min.X - (double)v.X;
		double y = _b._min.Y - (double)v.Y;
		double z = _b._min.Z - (double)v.Z;
		for (std::map<std::string, TSharedPtr<sceneLayer> >::iterator litr = _layers.begin();
			litr != _layers.end(); ++litr)
		{
			sceneLayer* ly = litr->second.Get();
			if (!ly) continue;
			TSharedPtr<"FArmyFurniture> lg = MakeShareable(new "FArmyFurniture());
			//"FArmyFurniture* lg = new "FArmyFurniture();


			ly->LayerToHomeEditor(lg.Get(), _b);
			lg->InitOperationPoint();
			root->AddLayer(lg);
		}
		return root;
	}
	TArray<TSharedPtr<"FArmyDxfLayer>> SceneToHomeLayer()
	{
		TArray<TSharedPtr<"FArmyDxfLayer>> allLayers;
		_b.makeMinValid();
		for (std::map<std::string, TSharedPtr<sceneLayer>>::iterator litr = _layers.begin(); litr != _layers.end(); ++litr)
		{
			sceneLayer* ly = litr->second.Get();
			if (!ly)
				continue;
			TSharedPtr<"FArmyDxfLayer> layer = MakeShareable(new "FArmyDxfLayer());
			ly->LayerToHome(layer.Get(), _b);
			allLayers.Push(layer);

		}
		return allLayers;
	}
	FBox GetBounds()
	{
		return FBox(_b._min, _b._max);
	}
	std::map<std::string, TSharedPtr<sceneLayer> >&    Getlayers() { return _layers; }
protected:
	Matrix44                _m;
	Matrix44                _r;
	//	FMatrix _m;
	//	FMatrix _r;
	FVector                  _t;
	bounds                      _b;
	std::map<std::string, TSharedPtr<sceneLayer> >        _layers;
	std::vector<Matrix44>   _mStack;
	//std::vector<FMatrix>   _mStack;

	dxfLayerTable*              _layerTable;
};

#endif
