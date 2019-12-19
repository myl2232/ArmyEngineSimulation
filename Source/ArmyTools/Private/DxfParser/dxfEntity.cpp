

#include "dxfEntity.h"
#include "dxfFile.h"
#include "scene.h"
#include "dxfBlock.h"
#include "codeValue.h"
#include "UnrealMathUtility.h"


#include <algorithm>




// static
std::map<std::string, dxfBasicEntity* > dxfEntity::_registry;
RegisterEntityProxy<dxf3DFace> g_dxf3DFace;
RegisterEntityProxy<dxfCircle> g_dxfCircle;
RegisterEntityProxy<dxfArc> g_dxfArc;
RegisterEntityProxy<dxfPoint> g_dxfPoint;
RegisterEntityProxy<dxfLine> g_dxfLine;
RegisterEntityProxy<dxfVertex> g_dxfVertex;
RegisterEntityProxy<dxfPolyline> g_dxfPolyline;
RegisterEntityProxy<dxfLWPolyline> g_dxfLWPolyline;
RegisterEntityProxy<dxfInsert> g_dxfInsert;
RegisterEntityProxy<dxfText> g_dxfText;
RegisterEntityProxy<dxfSpline> g_dxfSpline;
RegisterEntityProxy<dxfEllipse> g_dxfEllipse;
void
dxfBasicEntity::assign(dxfFile*, codeValue& cv)
{
	switch (cv._groupCode) {
	case 8:
		_layer = cv._string;
		break;
	case 62:
		_color = cv._short;
		break;
	case 420:
		EntityColor = cv._int;
		break;
	}
}

void
dxf3DFace::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	switch (cv._groupCode) {
	case 10:
	case 11:
	case 12:
	case 13:
		_vertices[cv._groupCode - 10].X = d;
		break;
	case 20:
	case 21:
	case 22:
	case 23:
		_vertices[cv._groupCode - 20].Y = d;
		break;
	case 30:
	case 31:
	case 32:
	case 33:
		_vertices[cv._groupCode - 30].Z = d;
		break;

	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void
dxf3DFace::drawScene(scene* sc)
{
	std::vector<FVector> vlist;
	short nfaces = 3;

	// Hate to do that, but hey, that's written in the DXF specs:
	if (_vertices[2] != _vertices[3]) nfaces = 4;

	for (short i = nfaces - 1; i >= 0; i--)
		vlist.push_back(_vertices[i]);

	if (nfaces == 3) {
		// to do make sure we're % 3
		sc->addTriangles(getLayer(), _color, vlist);
	}
	else if (nfaces == 4) {
		// to do make sure we're % 4
		sc->addQuads(getLayer(), _color, vlist);
	}
}

void
dxfVertex::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	// 2005.12.13 pdr: learned today that negative indices mean something and were possible

	int s = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
	if (s < 0) s = -s;
	switch (cv._groupCode) {
	case 10:
		_vertex.X = d;
		break;
	case 20:
		_vertex.Y = d;
		break;
	case 30:
		_vertex.Z = d;
		break;
	case 71:
		_indice1 = s;
		break;
	case 72:
		_indice2 = s;
		break;
	case 73:
		_indice3 = s;
		break;
	case 74:
		_indice4 = s;
		break;

	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void
dxfCircle::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	//unsigned short s = cv._short;
	switch (cv._groupCode) {
	case 10:
		_center.X = d;
		break;
	case 20:
		_center.Y = d;
		break;
	case 30:
		_center.Z = d;
		break;
	case 40:
		_radius = d;
		break;
	case 210:
		_ocs.X = d;
		break;
	case 220:
		_ocs.Y = d;
		break;
	case 230:
		_ocs.Z = d;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void
dxfCircle::drawScene(scene* sc)
{
	Matrix44 m;

	getOCSMatrix(_ocs, m);
	sc->ocs(m);
	std::vector<FVector> vlist;

	double theta = 15.0; // we generate polyline from "spokes" at theta degrees at arc's center
	_useAccuracy = true;
	if (_useAccuracy) {
		// we generate points on a polyline where each point lies on the arc, thus the maximum error occurs at the midpoint of each line segment where it lies furthest inside the arc
		// If we divide the segment in half and connect the bisection point to the arc's center, we have two rightangled triangles with
		// one side=r-maxError, hypotenuse=r, and internal angle at center is half the angle we will step with:

		double maxError = FMath::Min(_maxError, _radius); // Avoid offending acos() in the edge case where allowable deviation is greater than radius.
		double newtheta = FMath::Acos((_radius - maxError) / _radius);
		newtheta = FMath::RadiansToDegrees(newtheta)*2.0;

		// Option to only use the new accuracy code when it would improve on the accuracy of the old method
		if (_improveAccuracyOnly) {
			theta = FMath::Min(newtheta, theta);
		}
		else {
			theta = newtheta;
		}
	}
	theta = FMath::DegreesToRadians(theta);

	// We create an anglestep<=theta so that the line's points are evenly distributed around the circle
	unsigned int numsteps = static_cast<unsigned int>(floor(PI * 2 / theta));
	if (numsteps < 3) numsteps = 3; // Sanity check: minimal representation of a circle is a tri
	double anglestep = PI * 2 / numsteps;

	double angle1 = 0.0;
	FVector a = _center;
	FVector b;
	for (unsigned int r = 0; r <= numsteps; r++) {
		b = a + FVector(_radius * (double)sin(angle1), _radius * (double)cos(angle1), 0);
		angle1 += anglestep;
		vlist.push_back(b);
	}

	sc->addLineStrip(getLayer(), _color, vlist); // Should really add LineLoop implementation and save a vertex
	sc->ocs_clear();
}


void
dxfArc::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	//unsigned short s = cv._short;
	switch (cv._groupCode) {
	case 10:
		_center.X = d;
		break;
	case 20:
		_center.Y = d;
		break;
	case 30:
		_center.Z = d;
		break;
	case 40:
		_radius = d;
		break;
	case 50:
		_startAngle = d;
		break;
	case 51:
		_endAngle = d;
		break;
	case 210:
		_ocs.X = d;
		break;
	case 220:
		_ocs.Y = d;
		break;
	case 230:
		_ocs.Z = d;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void
dxfArc::drawScene(scene* sc)
{
	Matrix44 m;
	getOCSMatrix(_ocs, m);
	sc->ocs(m);
	std::vector<FVector> vlist;
	double end;
	double start;
	if (_startAngle > _endAngle) {
		start = _startAngle;
		end = _endAngle + 360;
	}
	else {
		start = _startAngle;
		end = _endAngle;
	}

	double theta = 15.0; // we generate polyline from "spokes" at theta degrees at arc's center
	_useAccuracy = true;
	if (_useAccuracy) {
		// we generate points on a polyline where each point lies on the arc, thus the maximum error occurs at the midpoint of each line segment where it lies furthest inside the arc
		// If we divide the segment in half and connect the bisection point to the arc's center, we have two rightangled triangles with
		// one side=r-maxError, hypotenuse=r, and internal angle at center is half the angle we will step with:
		double maxError = FMath::Min(_maxError, _radius); // Avoid offending acos() in the edge case where allowable deviation is greater than radius.
		double newtheta = FMath::Acos((_radius - maxError) / _radius);
		newtheta = FMath::RadiansToDegrees(newtheta)*2.0;
		//cout<<"r="<<_radius<<" _me="<<_maxError<<" (_radius-_maxError)="<<(_radius-_maxError)<<" newtheta="<<newtheta<<endl;
		// Option to only use the new accuracy code when it would improve on the accuracy of the old method
		if (_improveAccuracyOnly) {
			theta = FMath::Min(newtheta, theta);
		}
		else {
			theta = newtheta;
		}
	}

	double angle_step = FMath::DegreesToRadians(end - start);
	int numsteps = (int)((end - start) / theta);
	//cout<<"arc theta="<<osg::RadiansToDegrees(theta)<<" end="<<end<<" start="<<start<<" numsteps="<<numsteps<<" e-s/theta="<<((end-start)/theta)<<" end-start="<<(end-start)<<endl;
	if (numsteps * theta < (end - start)) numsteps++;
	numsteps = FMath::Max(numsteps, 2); // Whatever else, minimum representation of an arc is a straightline
	angle_step /= (double)numsteps;
	end = FMath::DegreesToRadians((-_startAngle) + 90.0);
	start = FMath::DegreesToRadians((-_endAngle) + 90.0);
	double angle1 = start;

	FVector a = _center;
	FVector b;

	for (int r = 0; r <= numsteps; r++)
	{
		b = a + FVector(_radius * (double)sin(angle1), _radius * (double)cos(angle1), 0);
		angle1 += angle_step;
		vlist.push_back(b);
	}


	sc->addLineStrip(getLayer(), _color, vlist);
	sc->ocs_clear();
}

void
dxfLine::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	//unsigned short s = cv._short;
	switch (cv._groupCode) {
	case 10:
		_a.X = d;
		break;
	case 20:
		_a.Y = d;
		break;
	case 30:
		_a.Z = d;
		break;
	case 11:
		_b.X = d;
		break;
	case 21:
		_b.Y = d;
		break;
	case 31:
		_b.Z = d;
		break;
	case 210:
		_ocs.X = d;
		break;
	case 220:
		_ocs.Y = d;
		break;
	case 230:
		_ocs.Z = d;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void
dxfLine::drawScene(scene* sc)
{
	Matrix44 m;
	getOCSMatrix(_ocs, m);
	// don't know why this doesn't work
	sc->ocs(m);
	sc->addLine(getLayer(), _color, _b, _a);
}
void
dxfPoint::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	//unsigned short s = cv._short;
	switch (cv._groupCode) {
	case 10:
		_a.X = d;
		break;
	case 20:
		_a.Y = d;
		break;
	case 30:
		_a.Z = d;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void
dxfPoint::drawScene(scene* sc)
{
	Matrix44 m;
	getOCSMatrix(_ocs, m);
	sc->addPoint(getLayer(), _color, _a, EntityColor);
}

void
dxfPolyline::assign(dxfFile* dxf, codeValue& cv)
{
	std::string s = cv._string;
	if (cv._groupCode == 0) {
		if (s == "VERTEX") {
			_currentVertex = new dxfVertex;
			_vertices.push_back(_currentVertex);
		}
	}
	else if (_currentVertex) {
		_currentVertex->assign(dxf, cv);

		if ((_flag & 64 /*i.e. polymesh*/) &&
			(cv._groupCode == 70 /*i.e. vertex flag*/) &&
			(cv._int & 128 /*i.e. vertex is actually a face*/))
			_indices.push_back(_currentVertex); // Add the index only if _currentvertex is actually an index
	}
	else {
		double d = cv._double;
		switch (cv._groupCode) {
		case 10:
			// dummy
			break;
		case 20:
			// dummy
			break;
		case 30:
			_elevation = d; // what is elevation?
			break;
		case 70:
			_flag = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
			break;
		case 71:
			// Meaningful only when _surfacetype == 6, don' trust it for polymeshes.
			// From the docs :
			// "The 71 group specifies the number of vertices in the mesh, and the 72 group
			// specifies the number of faces. Although these counts are correct for all meshes
			// created with the PFACE command, applications are not required to place correct
			// values in these fields.)"
			// Amusing isn't it ?
			_mcount = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
			break;
		case 72:
			// Meaningful only when _surfacetype == 6, don' trust it for polymeshes.
			// From the docs :
			// "The 71 group specifies the number of vertices in the mesh, and the 72 group
			// specifies the number of faces. Although these counts are correct for all meshes
			// created with the PFACE command, applications are not required to place correct
			// values in these fields.)"
			// Amusing isn't it ?
			_ncount = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
			break;
		case 73:
			_mdensity = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
			break;
		case 74:
			_ndensity = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
			break;
		case 75:
			_surfacetype = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
			break;
		case 210:
			_ocs.X = d;
			break;
		case 220:
			_ocs.Y = d;
			break;
		case 230:
			_ocs.Z = d;
			break;
		default:
			dxfBasicEntity::assign(dxf, cv);
			break;
		}
	}
}


void
dxfPolyline::drawScene(scene* sc)
{
	Matrix44 matrx;
	getOCSMatrix(_ocs, matrx);
	sc->ocs(matrx);
	std::vector<FVector> vlist;
	std::vector<FVector> qlist;
	FVector a, b, c, d;
	bool invert_order = false;
	if (_flag & 16) {
		std::vector<FVector> nlist;
		FVector nr;
		bool nset = false;
		//dxfVertex* v = NULL;
		unsigned int ncount;
		unsigned int mcount;
		if (_surfacetype == 6) {
			// I don't have examples of type 5 and 8, but they may be the same as 6
			mcount = _mdensity;
			ncount = _ndensity;
		}
		else {
			mcount = _mcount;
			ncount = _ncount;
		}
		for (unsigned int n = 0; n < ncount - 1; n++) {
			for (unsigned int m = 1; m < mcount; m++) {
				// 0
				a = _vertices[(m - 1)*ncount + n]->getVertex();
				// 1
				b = _vertices[m*ncount + n]->getVertex();
				// 3
				c = _vertices[(m)*ncount + n + 1]->getVertex();
				// 2
				d = _vertices[(m - 1)*ncount + n + 1]->getVertex();
				if (a == b) {
					vlist.push_back(a);
					vlist.push_back(c);
					vlist.push_back(d);
					b = c;
					c = d;
				}
				else if (c == d) {
					vlist.push_back(a);
					vlist.push_back(b);
					vlist.push_back(c);
				}
				else {
					qlist.push_back(a);
					qlist.push_back(b);
					qlist.push_back(c);
					qlist.push_back(d);
				}
				if (!nset) {
					nset = true;
					nr = (b - a) ^ (c - a);
					nr.Normalize();
				}
				nlist.push_back(a);
			}
		}
		if (_flag & 1) {
			for (unsigned int n = 0; n < ncount - 1; n++) {
				// 0
				a = _vertices[(mcount - 1)*ncount + n]->getVertex();
				// 1
				b = _vertices[0 * ncount + n]->getVertex();
				// 3
				c = _vertices[(0)*ncount + n + 1]->getVertex();
				// 2
				d = _vertices[(mcount - 1)*ncount + n + 1]->getVertex();
				if (a == b) {
					vlist.push_back(a);
					vlist.push_back(c);
					vlist.push_back(d);
					b = c;
					c = d;
				}
				else if (c == d) {
					vlist.push_back(a);
					vlist.push_back(b);
					vlist.push_back(c);
				}
				else {
					qlist.push_back(a);
					qlist.push_back(b);
					qlist.push_back(c);
					qlist.push_back(d);
				}
				nlist.push_back(a);
			}
		}
		if (_flag & 32) {
			for (unsigned int m = 1; m < mcount; m++) {
				// 0
				a = _vertices[(m - 1)*ncount + (ncount - 1)]->getVertex();
				// 1
				b = _vertices[m*ncount + (ncount - 1)]->getVertex();
				// 3
				c = _vertices[(m)*ncount]->getVertex();
				// 2
				d = _vertices[(m - 1)*ncount]->getVertex();
				if (a == b) {
					vlist.push_back(a);
					vlist.push_back(c);
					vlist.push_back(d);
					b = c;
					c = d;
				}
				else if (c == d) {
					vlist.push_back(a);
					vlist.push_back(b);
					vlist.push_back(c);
				}
				else {
					qlist.push_back(a);
					qlist.push_back(b);
					qlist.push_back(c);
					qlist.push_back(d);
				}
				nlist.push_back(a);
			}
		}


		if (qlist.size())
			sc->addQuads(getLayer(), _color, qlist, invert_order);
		if (vlist.size())
			sc->addTriangles(getLayer(), _color, vlist, invert_order);

	}
	else if (_flag & 64) {
		unsigned short _facetype = 3;

		for (unsigned int i = 0; i < _indices.size(); i++) {
			dxfVertex* vindice = _indices[i];
			if (!vindice) continue;
			if (vindice->getIndice4()) {
				_facetype = 4;
				d = _vertices[vindice->getIndice4() - 1]->getVertex();
			}
			else {
				_facetype = 3;
			}
			if (vindice->getIndice3()) {
				c = _vertices[vindice->getIndice3() - 1]->getVertex();
			}
			else {
				c = vindice->getVertex(); // Vertex not indexed. Use as is
			}
			if (vindice->getIndice2()) {
				b = _vertices[vindice->getIndice2() - 1]->getVertex();
			}
			else {
				b = vindice->getVertex(); // Vertex not indexed. Use as is
			}
			if (vindice->getIndice1()) {
				a = _vertices[vindice->getIndice1() - 1]->getVertex();
			}
			else {
				a = vindice->getVertex(); // Vertex not indexed. Use as is
			}
			if (_facetype == 4) {
				qlist.push_back(d);
				qlist.push_back(c);
				qlist.push_back(b);
				qlist.push_back(a);
			}
			else {
				vlist.push_back(c);
				vlist.push_back(b);
				vlist.push_back(a);
			}
		}
		if (vlist.size())
			sc->addTriangles(getLayer(), _color, vlist);
		if (qlist.size())
			sc->addQuads(getLayer(), _color, qlist);
	}
	else {
		for (int i = _vertices.size() - 1; i >= 0; i--)
			vlist.push_back(_vertices[i]->getVertex());
		if (_flag & 1) {
			sc->addLineLoop(getLayer(), _color, vlist);
		}
		else {
			sc->addLineStrip(getLayer(), _color, vlist);
		}

	}
	sc->ocs_clear();
}
void dxfEllipse::assign(dxfFile* dxf, codeValue& cv)
{
	switch (cv._groupCode)
	{
	case 40:
		ratio = cv._double;
		break;
	case 41:
		staparam = cv._double;
		break;
	case 42:
		endparam = cv._double;
		break;
	case 11:
		endPoint.X = cv._double;
		break;
	case 21:
		endPoint.Y = cv._double;
		break;
	case 31:
		endPoint.Z = cv._double;
		break;
	case 10:
		CenterPoint.X = cv._double;
		break;
	case 20:
		CenterPoint.Y = cv._double;
		break;
	case 30:
		CenterPoint.Z = cv._double;
	case 210:
		NormalVec.X = cv._double;
		break;
	case 220:
		NormalVec.Y = cv._double;
		break;
	case 230:
		NormalVec.Z = cv._double;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void dxfEllipse::drawScene(scene* sc)
{
	Matrix44 m;
	m.makeIdentity();
	NormalVec = FVector(0, 0, 1);
	getOCSMatrix(NormalVec, m);
	sc->ocs(m);
	float startAngle = FMath::RadiansToDegrees(staparam);
	float endAngle = FMath::RadiansToDegrees(endparam);
	//startAngle = 0.0;
	//endAngle = 360.0;
	int precision = 18;
	std::vector<FVector> vertices;
	float rotateAngle = 180 - FMath::RadiansToDegrees(FMath::Atan2(endPoint.Y, endPoint.X));
	if (rotateAngle >= 90)
	{
		rotateAngle -= 180.0f;
	}
	for (int i = 0; i <= precision; i++)
	{
		double angle = startAngle + i*(endAngle - startAngle) / precision;
		FVector point = PolarCoordinaateRelativeToCenter(angle);
		point = point.RotateAngleAxis(rotateAngle, FVector(0, 0, 1));
		vertices.push_back(point + CenterPoint);
	}
	//ToVertices(vertices, 128);
	sc->addLineStrip(getLayer(), _color, vertices);
	sc->ocs_clear();
}

FVector dxfEllipse::PolarCoordinaateRelativeToCenter(double angle)
{
	double b = FVector::Dist(endPoint, FVector::ZeroVector);
	double a = b* ratio;
	double radians = FMath::DegreesToRadians(angle);
	double  a1 = a * FMath::Sin(radians);
	double b1 = b* FMath::Cos(radians);
	double radius = (a* b) / FMath::Sqrt(b1*b1 + a1* a1);
	return FVector(radius* FMath::Sin(radians), radius* FMath::Cos(radians), 0);
}

void dxfEllipse::ToVertices(std::vector<FVector>& Results, int parts /*= 128*/)
{
	double radMajor, radMinor, cosRot, sinRot, inAngle, curAngle;
	double cosCurr, sinCurr;
	radMajor = FVector::Dist(endPoint, FVector::ZeroVector);
	radMinor = radMajor* ratio;

	inAngle = FMath::Atan2(endPoint.Y, endPoint.X);
	float rotateAngle = FMath::RadiansToDegrees(inAngle);
	cosRot = FMath::Cos(inAngle);
	sinRot = FMath::Sin(inAngle);

	inAngle = PI * 2 / parts;
	curAngle = staparam;
	int i = static_cast<int>(curAngle / inAngle);
	do
	{
		if (curAngle > endparam)
		{
			curAngle = endparam;
			i = parts + 2;
		}
		cosCurr = FMath::Cos(curAngle);
		sinCurr = FMath::Sin(curAngle);
		double x = cosCurr * radMajor;
		double y = sinCurr * radMinor;
		FVector temp = FVector(x, y, 0.0).RotateAngleAxis(rotateAngle, FVector(0, 0, 1));
		/*double x = CenterPoint.X + (cosCurr*cosRot*radMajor) - (sinCurr*sinRot*radMinor);
		double y = CenterPoint.Y + (cosCurr*sinRot*radMajor) + (sinCurr*cosRot*radMinor);*/
		Results.push_back(temp + CenterPoint);
		curAngle = (++i)*inAngle;
	} while (i < parts);

}

void dxfSpline::assign(dxfFile* dxf, codeValue& cv)
{
	double d = cv._double;
	switch (cv._groupCode)
	{
	case 210:
		NormalVec.X = d;
		break;
	case 220:
		NormalVec.Y = d;
		break;
	case 230:
		NormalVec.Z = d;
		break;
	case 12:
		tangStart.X = d;
		break;
	case 22:
		tangStart.Y = d;
		break;
	case 32:
		tangStart.Z = d;
		break;
	case 13:
		tangEnd.X = d;
		break;
	case 23:
		tangEnd.Y = d;
		break;
	case 33:
		tangEnd.Z = d;
		break;
	case 70:
		flags = cv._int;
		break;
	case 71:
		degree = cv._int;
		break;
	case 72:
		nKnots = cv._int;
		break;
	case 73:
		nControl = cv._int;
		break;
	case 74:
		nfit = cv._int;
		break;
	case 42:
		tolknot = cv._double;
		break;
	case 43:
		tolcontrol = cv._double;
		break;
	case 44:
		tolfit = cv._double;
		break;
	case 10:
	{
		currentControlPoint = new FVector();
		currentControlPoint->X = cv._double;
		controllist.push_back(currentControlPoint);
	}
	break;
	case 20:
		if (currentControlPoint != NULL)
			currentControlPoint->Y = cv._double;
		break;
	case 30:
		if (currentControlPoint != NULL)
			currentControlPoint->Z = cv._double;
		break;
	case 11:
	{
		currentFitPoint = new FVector();
		currentFitPoint->X = cv._double;
		filtist.push_back(currentFitPoint);
	}
	break;
	case 21:
		if (currentFitPoint != NULL)
			currentFitPoint->Y = cv._double;
		break;
	case 31:
		if (currentFitPoint != NULL)
			currentFitPoint->Z = cv._double;
		break;
	case 40:
		knotsList.push_back(cv._double);
		break;

	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}

void dxfSpline::drawScene(scene* sc)
{
	Matrix44 m;

	getOCSMatrix(NormalVec, m);
	sc->ocs(m);
	CalculteVertext(0);
	sc->addLineStrip(getLayer(), _color, _vertices);
}


void dxfSpline::CalculteVertext(int precision)
{
	if (controllist.size() >= 4)
	{
		int num = controllist.size();
		FVector first = *controllist[0];
		FVector second = *controllist[1];
		FVector last = *controllist[num - 1];
		FVector lastBefore = *controllist[num - 2];
		FVector firstInsert = first * 2 - second;
		FVector lastInsert = last * 2 - lastBefore;

		std::vector<FVector> TotalControlPoints;
		TotalControlPoints.push_back(firstInsert);
		int index = 0;
		for (auto vertice : controllist)
		{
			TotalControlPoints.push_back(*vertice);
		}
		TotalControlPoints.push_back(lastInsert);
		for (int i = 0; i < TotalControlPoints.size() - 3; i++)
		{
			Bezier(TotalControlPoints[i], TotalControlPoints[i + 1], TotalControlPoints[i + 2], TotalControlPoints[i + 3]);
		}
	}
}

void dxfSpline::Bezier(FVector a, FVector b, FVector c, FVector d)
{
	int n = 30;
	double delta = 1.0 / n;
	for (int i = 0; i < n; i++)
	{
		double t = delta * i;
		double ratio[4];
		ratio[0] = GetRatio(t, -1, 3, -3, 1);
		ratio[1] = GetRatio(t, 3, -6, 0, 4);
		ratio[2] = GetRatio(t, -3, 3, 3, 1);
		ratio[3] = GetRatio(t, 1, 0, 0, 0);
		double x = 0, y = 0;

		x += ratio[0] * a.X + ratio[1] * b.X + ratio[2] * c.X + ratio[3] * d.X;
		y += ratio[0] * a.Y + ratio[1] * b.Y + ratio[2] * c.Y + ratio[3] * d.Y;
		x /= 6.0f;
		y /= 6.0f;
		_vertices.push_back(FVector(x, y, 0));
	}
}

double dxfSpline::GetRatio(double t, double a, double b, double c, double d)
{
	return a * FMath::Pow(t, 3) + b * FMath::Pow(t, 2) + c* t + d;
}

void
dxfLWPolyline::assign(dxfFile* dxf, codeValue& cv)
{
	std::string s = cv._string;

	double d = cv._double;
	TuAngle = 0.0f;
	switch (cv._groupCode) {

	case 10:
		_lastv = new VerticeInfo();
		_vertices.push_back(_lastv);
		_lastv->pos.X = d;
		// x
		break;
	case 20:
		if (_lastv != NULL)
			_lastv->pos.Y = d;


		break;
	case 38:
		_elevation = d; // what is elevation?
		break;
	case 42:
		if (_lastv != NULL)
			_lastv->_convexAngle = d;
		break;
	case 70:
		_flag = cv._int; // 2005.12.13 pdr: group codes [70,78] now signed int.
		break;
	case 90:
		_vcount = cv._short;
		break;
	case 210:
		_ocs.X = d;
		break;
	case 220:
		_ocs.Y = d;
		break;
	case 230:
		_ocs.Z = d;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}


void
dxfLWPolyline::drawScene(scene* sc)
{

	Matrix44 m;
	getOCSMatrix(_ocs, m);
	sc->ocs(m);
	caculateCircle();
	if (_flag & 1)
	{
		sc->addLineLoop(getLayer(), _color, ResultVertices);
	}
	else
	{
		sc->addLineStrip(getLayer(), _color, ResultVertices);
	}
	sc->ocs_clear();
}

void dxfLWPolyline::caculateCircle()
{
	int num = _vertices.size();
	for (int i = 0; i < num; i++)
	{
		VerticeInfo* start = NULL;
		VerticeInfo* end = NULL;
		if (i == num - 1)
		{
			start = _vertices[i];
			end = _vertices[0];
		}
		else
		{
			start = _vertices[i];
			end = _vertices[i + 1];
		}
		if (!(start->pos - end->pos).IsNearlyZero(0.001))
		{

			if (FMath::IsNearlyZero(FMath::Abs(start->_convexAngle), 0.001f) || FMath::Abs(start->_convexAngle) >= 1.0f)
			{
				ResultVertices.push_back(start->pos);

			}
			else
			{

				ArcCircle temp;
				if (start->_convexAngle < 0)
				{
					temp.clockWise = true;
				}
				else {
					temp.clockWise = false;
				}
				temp.startPoint = start->pos;
				temp.endPoint = end->pos;

				double length = FVector::Dist(temp.startPoint, temp.endPoint);
				if (length > 0.2f)
				{
					double theta_arc = FMath::Atan(start->_convexAngle) * 4;
					double theta_degree = FMath::RadiansToDegrees(theta_arc);
					temp.angle = theta_degree;

					if (FMath::Abs(180 - FMath::Abs(theta_degree)) < 0.1)
					{
						temp.centerPoint = (start->pos + end->pos) / 2;
					}
					else
					{
						double dFR = length / (2.0f*FMath::Sin(FMath::Abs(theta_arc) / 2.0f));
						temp.radius = dFR;
						FVector tempCenter1, tempCenter2;
						FVector Horizontal = (temp.endPoint - temp.startPoint).GetSafeNormal();
						FVector Vertical = Horizontal.RotateAngleAxis(90, FVector(0, 0, 1));
						double VerticalHeght = FMath::Sqrt(FMath::Pow(dFR, 2) - FMath::Pow(length / 2, 2));
						tempCenter1 = (temp.startPoint + temp.endPoint) / 2 + VerticalHeght * Vertical;
						tempCenter2 = (temp.startPoint + temp.endPoint) / 2 - VerticalHeght * Vertical;
						if (temp.clockWise)
						{
							temp.centerPoint = tempCenter2;
						}
						else
						{
							temp.centerPoint = tempCenter1;
						}
					}
					int numSide = 18;
					float deltaAngle = -temp.angle / numSide;

					float angle1 = 0.0f;
					for (int i = 0; i < numSide; i++)
					{
						FVector basePoint = temp.centerPoint;
						FVector start = temp.startPoint;
						FVector result = FRotator(0, -angle1, 0).RotateVector(start - basePoint) + basePoint;
						angle1 += deltaAngle;
						ResultVertices.push_back(result);
					}
				}
				else
				{
					ResultVertices.push_back(temp.startPoint);
				}


			}
		}
		else
		{
			ResultVertices.push_back(start->pos);
		}
	}

}

void
dxfInsert::assign(dxfFile* dxf, codeValue& cv)
{
	std::string s = cv._string;
	if (_done || (cv._groupCode == 0 && s != "INSERT")) {
		_done = true;
		return;
	}
	if (cv._groupCode == 2 && !_block) {
		_blockName = s;
		_block = dxf->findBlock(s);
	}
	else {
		double d = cv._double;
		switch (cv._groupCode) {
		case 10:
			_point.X = d;
			break;
		case 20:
			_point.Y = d;
			break;
		case 30:
			_point.Z = d;
			break;
		case 41:
			_scale.X = d;
			break;
		case 42:
			_scale.Y = d;
			break;
		case 43:
			_scale.Z = d;
			break;
		case 50:
			_rotation = d;
			break;
		case 210:
			_ocs.X = d;
			break;
		case 220:
			_ocs.Y = d;
			break;
		case 230:
			_ocs.Z = d;
			break;
		default:
			dxfBasicEntity::assign(dxf, cv);
			break;
		}
	}
}

/// hum. read the doc, then come back here. then try to figure.
void
dxfInsert::drawScene(scene* sc)
{
	// INSERTs can be nested. So pull the current matrix
	// and push it back after we fill our context
	// This is a snapshot in time. I will rewrite all this to be cleaner,
	// but for now, it seems working fine
	// (with the files I have, the results are equal to Voloview,
	// and better than Deep Exploration and Lightwave).

	// sanity check (useful when no block remains after all unsupported entities have been filtered out)
	if (!_block)
		return;

	Matrix44 back = sc->backMatrix();
	Matrix44 m;
	m.makeIdentity();
	sc->pushMatrix(m, true);
	FVector trans = _block->getPosition();
	sc->blockOffset(-trans);
	if (_rotation) {
		if (_scale.X < 0.0f)
		{
			_rotation = -_rotation;
		}
		sc->pushMatrix(Matrix44::rotate(FMath::DegreesToRadians(_rotation), 0, 0, 1));
	}

	sc->pushMatrix(Matrix44::scale(_scale.X, _scale.Y, _scale.Z));
	sc->pushMatrix(Matrix44::translate(_point.X, _point.Y, _point.Z));
	getOCSMatrix(_ocs, m);
	sc->pushMatrix(m);
	sc->pushMatrix(back);

	EntityList& l = _block->getEntityList();
	for (EntityList::iterator itr = l.begin(); itr != l.end(); ++itr) {
		dxfBasicEntity* e = (*itr).getEntity();
		if (e) {
			e->drawScene(sc);
		}
	}

	sc->popMatrix(); // ocs
	sc->popMatrix(); // translate
	sc->popMatrix(); // scale
	if (_rotation) {
		sc->popMatrix(); // rotate
	}
	sc->popMatrix(); // identity
	sc->popMatrix(); // back
	sc->blockOffset(FVector(0, 0, 0));

}

void
dxfText::assign(dxfFile* dxf, codeValue& cv)
{
	switch (cv._groupCode) {
	case 1:
		_string = cv._string;
		break;
	case 10:
		_point1.X = cv._double;
		break;
	case 20:
		_point1.Y = cv._double;
		break;
	case 30:
		_point1.Z = cv._double;
		break;
	case 11:
		_point2.X = cv._double;
		break;
	case 21:
		_point2.Y = cv._double;
		break;
	case 31:
		_point2.Z = cv._double;
		break;
	case 40:
		_height = cv._double;
		break;
	case 41:
		_xscale = cv._double;
		break;
	case 50:
		_rotation = cv._double;
		break;
	case 71:
		_flags = cv._int;
		break;
	case 72:
		_hjustify = cv._int;
		break;
	case 73:
		_vjustify = cv._int;
		break;
	case 210:
		_ocs.X = cv._double;
		break;
	case 220:
		_ocs.Y = cv._double;
		break;
	case 230:
		_ocs.Z = cv._double;
		break;
	default:
		dxfBasicEntity::assign(dxf, cv);
		break;
	}
}


void
dxfText::drawScene(scene* sc)
{
	/* osgText::Text::AlignmentType align;

	 Matrix44 m;
	 getOCSMatrix(_ocs, m);
	 sc->ocs(m);

	 ref_ptr<osgText::Text> _text = new osgText::Text;
	 _text->setText(_string);

	 _text->setCharacterSize( _height, 1.0/_xscale );
	 _text->setFont("D:/arial.ttf");

	 Quat qr( DegreesToRadians(_rotation), Z_AXIS );

	 if ( _flags & 2 ) qr = Quat( PI, Y_AXIS ) * qr;
	 if ( _flags & 4 ) qr = Quat( PI, X_AXIS ) * qr;

	 _text->setAxisAlignment(osgText::Text::USER_DEFINED_ROTATION);
	 _text->setRotation(qr);

	 if ( _hjustify != 0 || _vjustify !=0 ) _point1 = _point2;

	 switch (_vjustify) {
	 case 3:
		 switch (_hjustify) {
		 case 2:
			 align = osgText::Text::RIGHT_TOP;
			 break;
		 case 1:
			 align = osgText::Text::CENTER_TOP;
			 break;
		 default:
			 align = osgText::Text::LEFT_TOP;
		 }
		 break;
	 case 2:
		 switch (_hjustify) {
		 case 2:
			 align = osgText::Text::RIGHT_CENTER;
			 break;
		 case 1:
			 align = osgText::Text::CENTER_CENTER;
			 break;
		 default:
			 align = osgText::Text::LEFT_CENTER;
		 }
		 break;
	 case 1:
		 switch (_hjustify) {
		 case 2:
			 align = osgText::Text::RIGHT_BOTTOM;
			 break;
		 case 1:
			 align = osgText::Text::CENTER_BOTTOM;
			 break;
		 default:
			 align = osgText::Text::LEFT_BOTTOM;
		 }
		 break;
	 default:
		 switch (_hjustify) {
		 case 2:
			 align = osgText::Text::RIGHT_BOTTOM_BASE_LINE;
			 break;
		 case 1:
			 align = osgText::Text::CENTER_BOTTOM_BASE_LINE;
			 break;
		 default:
			 align = osgText::Text::LEFT_BOTTOM_BASE_LINE;
		 }
		 break;
	 }

	 _text->setAlignment(align);

	 sc->addText(getLayer(), _color, _point1, _text.get());
	 sc->ocs_clear();*/
}


// static
void
dxfEntity::registerEntity(dxfBasicEntity* entity)
{
	_registry[entity->name()] = entity;
}

// static
void
dxfEntity::unregisterEntity(dxfBasicEntity* entity)
{
	std::map<std::string, dxfBasicEntity * >::iterator itr = _registry.find(entity->name());
	if (itr != _registry.end()) {
		_registry.erase(itr);
	}
}

void dxfEntity::drawScene(scene* sc)
{
	for (std::vector<dxfBasicEntity*>::iterator itr = _entityList.begin();
		itr != _entityList.end(); ++itr) {
		(*itr)->drawScene(sc);
	}
}

void
dxfEntity::assign(dxfFile* dxf, codeValue& cv)
{
	std::string s = cv._string;
	if (cv._groupCode == 66 && !(_entity && std::string("TABLE") == _entity->name())) {
		// The funny thing here. Group code 66 has been called 'obsoleted'
		// for a POLYLINE. But not for an INSERT. Moreover, a TABLE
		// can have a 66 for... an obscure bottom cell color value.
		// I decided to rely on the presence of the 66 code for
		// the POLYLINE. If you find a better alternative,
		// contact me, or correct this code
		// and post the correction to osg mailing list
		_seqend = true;
	}
	else if (_seqend && cv._groupCode == 0 && s == "SEQEND") {
		_seqend = false;
		//        cout << "... off" << endl;
	}
	else if (_entity) {
		_entity->assign(dxf, cv);
	}
}

