
#ifndef DXF_ENTITY
#define DXF_ENTITY 1

#include <vector>
#include <string>
#include <iostream>
#include "Matrixf.h"

#include "dxfBlock.h"


class scene;
class codeValue;
class dxfFile;

static inline void
getOCSMatrix(const FVector& ocs, Matrix44& m)
{
	static const double one_64th = 1.0 / 64.0;
	m.makeIdentity();
	if (ocs == FVector(0, 0, 1)) return;
	FVector ax(1, 0, 0), ay(0, 1, 0), az(0, 0, 1);
	FVector ocsaxis(ocs);
	ocsaxis.Normalize();
	if (fabs(ocsaxis.X) < one_64th && fabs(ocsaxis.Y) < one_64th) {
		ax = ay ^ ocsaxis;
	}
	else {
		ax = az ^ ocsaxis;
	}
	ax.Normalize();
	ay = ocsaxis ^ ax;
	ay.Normalize();
	m = Matrix44(ax.X, ax.Y, ax.Z, 0,
		ay.X, ay.Y, ay.Z, 0,
		ocsaxis.X, ocsaxis.Y, ocsaxis.Z, 0,
		0, 0, 0, 1);

}

class dxfBasicEntity
{
public:
	dxfBasicEntity() : _color(0), _useAccuracy(false), _maxError(0.01), _improveAccuracyOnly(false) {}
	virtual ~dxfBasicEntity() {}
	virtual dxfBasicEntity* create() = 0;
	virtual const char* name() = 0;
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual void drawScene(scene*) {}
	const std::string getLayer() const { return _layer; }

	void setAccuracy(bool useAccuracy, double maxError, bool improveAccuracyOnly) {
		_useAccuracy = useAccuracy;
		_maxError = maxError;
		_improveAccuracyOnly = improveAccuracyOnly;
	}


protected:
	std::string    _layer;
	unsigned short    _color;
	int EntityColor;
	bool _useAccuracy;          // true to specify a maximum deviation for curve rendering
	double _maxError;         // the error in model units, if _useAccuracy==true
	bool _improveAccuracyOnly;// if true only use _maxError where it would increase the quality of curves compared to the previous algorithm

};


class dxfCircle : public dxfBasicEntity
{
public:
	dxfCircle() : _radius(0), _ocs(0, 0, 1) {}
	virtual ~dxfCircle() {}
	virtual dxfBasicEntity* create() { // we create a copy which uses our accuracy settings
		dxfBasicEntity* circle = new dxfCircle;
		circle->setAccuracy(_useAccuracy, _maxError, _improveAccuracyOnly);
		return circle;
	}
	virtual const char* name() { return "CIRCLE"; }
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual void drawScene(scene* sc);
protected:
	FVector    _center;
	double    _radius;
	FVector    _ocs;
};

class dxfArc : public dxfBasicEntity
{
public:
	dxfArc() : _radius(0), _startAngle(0), _endAngle(360), _ocs(0, 0, 1) {}
	virtual ~dxfArc() {}
	virtual dxfBasicEntity* create() { // we create a copy which uses our accuracy settings
		dxfBasicEntity* arc = new dxfArc;
		arc->setAccuracy(_useAccuracy, _maxError, _improveAccuracyOnly);
		//std::cout<<"dxfArc::create with _useAccuracy="<<_useAccuracy<<" maxError="<<_maxError<<" improveAccuracyOnly="<<_improveAccuracyOnly<<std::endl;
		return arc;
	}
	virtual const char* name() { return "ARC"; }
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual void drawScene(scene* sc);
protected:
	FVector    _center;
	double    _radius;
	double    _startAngle;
	double    _endAngle;
	FVector   _ocs;
};

class dxfPoint : public dxfBasicEntity
{
public:
	dxfPoint() : _ocs(0, 0, 1) {}
	virtual ~dxfPoint() {}
	virtual dxfBasicEntity* create() { return new dxfPoint; }
	virtual const char* name() { return "POINT"; }
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual void drawScene(scene* sc);
protected:
	FVector    _a;
	//FVector    _b;
	FVector   _ocs;
};

class dxfLine : public dxfBasicEntity
{
public:
	dxfLine() : _ocs(0, 0, 1) {}
	virtual ~dxfLine() {}
	virtual dxfBasicEntity* create() { return new dxfLine; }
	virtual const char* name() { return "LINE"; }
	virtual void assign(dxfFile* dxf, codeValue& cv)override;
	virtual void drawScene(scene* sc);
protected:
	FVector    _a;
	FVector    _b;
	FVector   _ocs;
};

class dxf3DFace : public dxfBasicEntity
{
public:
	dxf3DFace()
	{
		_vertices[0] = FVector(0, 0, 0);
		_vertices[1] = FVector(0, 0, 0);
		_vertices[2] = FVector(0, 0, 0);
		_vertices[3] = FVector(0, 0, 0);
	}
	virtual ~dxf3DFace() {}
	virtual dxfBasicEntity* create() { return new dxf3DFace; }
	virtual const char* name() { return "3DFACE"; }
	virtual void assign(dxfFile* dxf, codeValue& cv)override;
	virtual void drawScene(scene* sc);
protected:
	FVector _vertices[4];
};

class dxfVertex : public dxfBasicEntity
{
public:
	dxfVertex() : _vertex(FVector(0, 0, 0)), _indice1(0), _indice2(0), _indice3(0), _indice4(0) {}
	virtual ~dxfVertex() {}
	virtual dxfBasicEntity* create() { return new dxfVertex; }
	virtual const char* name() { return "VERTEX"; }
	virtual void assign(dxfFile* dxf, codeValue& cv);
	void getVertex(double &x, double &y, double &z) { x = _vertex.X; y = _vertex.Y; z = _vertex.Z; }
	const FVector& getVertex() const { return _vertex; }
	unsigned int getIndice1() const { return _indice1; }
	unsigned int getIndice2() const { return _indice2; }
	unsigned int getIndice3() const { return _indice3; }
	unsigned int getIndice4() const { return _indice4; }

protected:
	FVector    _vertex;
	unsigned int _indice1, _indice2, _indice3, _indice4;
};

class dxfPolyline : public dxfBasicEntity
{
public:
	dxfPolyline() : _currentVertex(NULL),
		_elevation(0.0),
		_flag(0),
		_mcount(0),
		_ncount(0),
		_nstart(0),
		_nend(0),
		_ocs(FVector(0, 0, 1)),
		_mdensity(0),
		_ndensity(0),
		_surfacetype(0)
	{}
	virtual ~dxfPolyline() {}
	virtual dxfBasicEntity*        create() { return new dxfPolyline; }
	virtual const char*            name() { return "POLYLINE"; }
	virtual void                assign(dxfFile* dxf, codeValue& cv);
	virtual int                    vertexCount() { return _vertices.size(); }
	virtual void                drawScene(scene* sc);

protected:
	dxfVertex*                    _currentVertex;
	std::vector<dxfVertex* >        _vertices;
	std::vector<dxfVertex* >        _indices;
	double                        _elevation;
	unsigned short                _flag;
	unsigned int                  _mcount;
	unsigned int                  _ncount;
	unsigned short                _nstart; // 71
	unsigned short                _nend; //72
	FVector                   _ocs; //210 220 230
	unsigned short                _mdensity; // 73
	unsigned short                _ndensity; // 74
	unsigned short                _surfacetype; //75

};

class dxfEllipse :public dxfBasicEntity
{
public:
	dxfEllipse() :
		NormalVec(FVector(0, 0, 1))
		, CenterPoint(FVector::ZeroVector)
		, endPoint(FVector::ZeroVector)
		, staparam(0)
		, endparam(2 * PI)
	{

	}
	virtual dxfBasicEntity*        create() { return new dxfEllipse; }
	virtual const char*            name() { return "ELLIPSE"; }
	virtual void                assign(dxfFile* dxf, codeValue& cv);
	virtual void                drawScene(scene* sc);

protected:
	FVector  PolarCoordinaateRelativeToCenter(double angle);
	void ToVertices(std::vector<FVector>& Results, int parts = 128);
	double ratio; // ration code 40
	double staparam; // start parameter ,code 41 for full ellipse
	double endparam; // end paramter ,code 42 2*PI for full ellipse
	int isccw; // is couter clockwise or nont 
	FVector CenterPoint;
	FVector endPoint;
	FVector NormalVec;

};
class dxfSpline :public dxfBasicEntity
{
public:
	dxfSpline()
		:currentControlPoint(NULL),
		currentFitPoint(NULL),
		NormalVec(FVector(0, 0, 1))
	{

	}
	~dxfSpline() {
		while (!controllist.empty())
		{
			controllist.pop_back();
		}
		while (!filtist.empty())
		{
			filtist.pop_back();
		}
	}

	virtual dxfBasicEntity*        create() { return new dxfSpline; }
	virtual const char*            name() { return "SPLINE"; }
	virtual void                assign(dxfFile* dxf, codeValue& cv);
	virtual void                drawScene(scene* sc);
	void CalculteVertext(int precision);
	void Bezier(FVector a, FVector b, FVector c, FVector d);
	double GetRatio(double t, double a, double b, double c, double d);
protected:
	FVector NormalVec;
	FVector tangStart;
	FVector tangEnd;
	int flags; // spline flag ,code 70
	int degree; // degree of thss spline
	int32 nKnots; // number of knots points
	int32 nControl; // number of control point;
	int32 nfit; // number of fit points;
	double tolknot; // knot tolerance 
	double tolfit;  // fit point tolerance 
	double tolcontrol;
	std::vector<double> knotsList;
	std::vector<FVector*> controllist;
	std::vector<FVector*> filtist;
	std::vector<FVector> _vertices;
	FVector* currentControlPoint;
	FVector* currentFitPoint;
	FVector                   _ocs; //210 220 230
};
class dxfLWPolyline : public dxfBasicEntity
{
public:
	dxfLWPolyline() :
		_elevation(0.0),
		_flag(0),
		_vcount(0),
		_ocs(FVector(0, 0, 1)),
		_lastv(NULL),
		TuAngle(0.0)

	{}
	virtual ~dxfLWPolyline() {
		while (!_vertices.empty())
		{
			_vertices.pop_back();
		}
	}
	virtual dxfBasicEntity*        create() { return new dxfLWPolyline; }
	virtual const char*            name() { return "LWPOLYLINE"; }
	virtual void                assign(dxfFile* dxf, codeValue& cv);
	virtual int                    vertexCount() { return _vertices.size(); }
	virtual void                drawScene(scene* sc);

public:
	struct ArcCircle
	{
		FVector startPoint;
		FVector endPoint;
		FVector centerPoint;
		float angle;
		bool clockWise;
		float radius;
	};
	struct VerticeInfo
	{
		VerticeInfo()
		{
			pos = FVector::ZeroVector;
			_convexAngle = 0.0f;
		}
		VerticeInfo(FVector InPos, float InAngle)
		{
			pos = InPos;
			_convexAngle = InAngle;
		}
		FVector pos;
		float  _convexAngle;
	};
protected:
	void caculateCircle();
	double                        _elevation;
	unsigned short                _flag;
	unsigned short                _vcount; // 90

	FVector                   _ocs; //210 220 230
	VerticeInfo*                 _lastv;
	std::vector< VerticeInfo* >     _vertices;
	std::vector<FVector> ResultVertices;
	float TuAngle;

};

class dxfInsert : public dxfBasicEntity
{
public:
	dxfInsert() : _block(NULL),
		_done(false),
		_rotation(0),
		_scale(1, 1, 1),
		_point(FVector(0, 0, 0)),
		_ocs(FVector(0, 0, 1)) {}
	virtual ~dxfInsert() { delete _block; }
	virtual dxfBasicEntity* create() { return new dxfInsert; }
	virtual const char* name() { return "INSERT"; }
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual void drawScene(scene* sc);

protected:
	std::string _blockName;
	dxfBlock* _block;
	bool _done; // since we are on a SEQEND, we must
				// make sure not getting values addressed to other
				// entities (dxf garble things) in the sequence
	double            _rotation;
	FVector       _scale;
	FVector       _point;
	FVector      _ocs;
};

class dxfText : public dxfBasicEntity
{
public:
	dxfText() :
		_string(""),
		_point1(0, 0, 0),
		_point2(0, 0, 0),
		_ocs(0, 0, 1),
		_height(1),
		_xscale(1),
		_rotation(0),
		_flags(0),
		_hjustify(0),
		_vjustify(0) {}

	virtual ~dxfText() {}
	virtual dxfBasicEntity*        create() { return new dxfText; }
	virtual const char*            name() { return "TEXT"; }
	virtual void                   assign(dxfFile* dxf, codeValue& cv);
	virtual void                   drawScene(scene* sc);

protected:
	std::string       _string;    // 1
	FVector        _point1;    // 10,20,30
	FVector       _point2;    // 11,21,31
	FVector       _ocs;       // 210,220,230
	double            _height;    // 40
	double            _xscale;    // 41
	double            _rotation;  // 50
	int               _flags;     // 71
	int               _hjustify;  // 72
	int               _vjustify;  // 73
};

class dxfEntity
{
public:
	dxfEntity(std::string s) : _entity(NULL), _seqend(false)
	{
		_entity = findByName(s);
		if (_entity) {
			_entityList.push_back(_entity);
			//    std::cout << "entity " << s << std::endl;
		}
	}
	virtual ~dxfEntity() {}
	virtual void assign(dxfFile* dxf, codeValue& cv);
	virtual bool done() { return !_seqend; }
	static void registerEntity(dxfBasicEntity*);
	static void unregisterEntity(dxfBasicEntity*);
	static dxfBasicEntity* findByName(std::string s)
	{
		dxfBasicEntity* be = _registry[s];
		if (be)
			return be->create();
		else {
			std::cout << " no " << s << std::endl;
			return NULL;
		}
	}
	virtual void drawScene(scene* sc);
	dxfBasicEntity* getEntity() { return _entity; }

	// Returns the exemplar from the registry - all other entities of this type are created by this one via entity->create
	static dxfBasicEntity* getRegistryEntity(std::string s) {
		return _registry[s];
	}

protected:
	std::vector<dxfBasicEntity* > _entityList;
	static std::map<std::string, dxfBasicEntity* > _registry;
	dxfBasicEntity* _entity;
	bool    _seqend; // bypass 0 codes. needs a 0 seqend to close.



};

/** Proxy class for automatic registration of dxf entities reader/writers.*/
template<class T>
class RegisterEntityProxy
{
public:
	RegisterEntityProxy()
	{
		_rw = new T;
		dxfEntity::registerEntity(_rw);
	}

	~RegisterEntityProxy()
	{
		dxfEntity::unregisterEntity(_rw);
	}

	T* get() { return _rw; }

protected:
	T* _rw;
};

#endif
