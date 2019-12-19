/* Importation de fichiers DXF dans OpenSceneGraph (see www.openscenegraph.org)
Copyright (C) 2004 by Paul de Repentigny <pdr@grapharchitecture.com>
*/

#include "scene.h"
#include "dxfTable.h"
#include "aci.h"


using namespace std;

FVector preMultd(const Matrix44& m, const FVector& v)
{
	double d = 1.0f / (m(3, 0)*v.X + m(3, 1)*v.Y + m(3, 2)*v.Z + m(3, 3));
	return FVector((m(0, 0)*v.X + m(1, 0)*v.Y + m(2, 0)*v.Z + m(3, 0))*d,
		(m(0, 1)*v.X + m(1, 1)*v.Y + m(2, 1)*v.Z + m(3, 1))*d,
		(m(0, 2)*v.X + m(1, 2)*v.Y + m(2, 2)*v.Z + m(3, 2))*d);


}

FVector postMultd(const Matrix44& m, const FVector& v)
{
	double d = 1.0f / (m(3, 0)*v.X + m(3, 1)*v.Y + m(3, 2)*v.Z + m(3, 3));
	return FVector((m(0, 0)*v.X + m(0, 1)*v.Y + m(0, 2)*v.Z + m(0, 3))*d,
		(m(1, 0)*v.X + m(1, 1)*v.Y + m(1, 2)*v.Z + m(1, 3))*d,
		(m(2, 0)*v.X + m(2, 1)*v.Y + m(2, 2)*v.Z + m(2, 3))*d);

}

FLinearColor sceneLayer::getColor(unsigned short color)
{
	// you're supposed to have a correct color in hand
	unsigned short r = color * 3;
	unsigned short g = color * 3 + 1;
	unsigned short b = color * 3 + 2;
	FLinearColor c(aci::table[r], aci::table[g], aci::table[b], 1.0f);
	return c;
}


scene::scene(dxfLayerTable* lt) : _layerTable(lt)
{
	_t = FVector::ZeroVector;

	_m.makeIdentity();
	_r.makeIdentity();
}

void
scene::setLayerTable(dxfLayerTable* lt)
{
	_layerTable = lt;

}

FVector scene::addVertex(FVector v)
{
	v += _t;
	v = preMultd(_r, v);

	Matrix44 m = Matrix44::translate(v.X, v.Y, v.Z);
	m = m * _m;
	FVector a = preMultd(m, FVector(0, 0, 0));
	_b.expandBy(a);
	return a;
}

FVector scene::addNormal(FVector v)
{

	return v;
}
void
scene::addPoint(const std::string & l, unsigned short color, FVector & s, int32 entityColor)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen()) return;
	sceneLayer* ly = findOrCreateSceneLayer(l);
	FVector a(addVertex(s));
	PointInfo info(a, entityColor);
	ly->_points[correctedColorIndex(l, color)].push_back(info);
}

void
scene::addLine(const std::string & l, unsigned short color, FVector & s, FVector & e)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen()) return;
	sceneLayer* ly = findOrCreateSceneLayer(l);
	FVector a(addVertex(s)), b(addVertex(e));
	ly->_lines[correctedColorIndex(l, color)].push_back(a);
	ly->_lines[correctedColorIndex(l, color)].push_back(b);
}
void scene::addLineStrip(const std::string & l, unsigned short color, std::vector<FVector> & vertices)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen()) return;
	sceneLayer* ly = findOrCreateSceneLayer(l);
	std::vector<FVector> converted;
	for (std::vector<FVector>::iterator itr = vertices.begin();
		itr != vertices.end(); ++itr) {
		converted.push_back(addVertex(*itr));
	}
	ly->_linestrips[correctedColorIndex(l, color)].push_back(converted);
}
void scene::AddLwPolyLine(const std::string& l, unsigned short color, dxfLWPolyline* polyLine)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen())
		return;
	sceneLayer* ly = findOrCreateSceneLayer(l);


}
void scene::addLineLoop(const std::string & l, unsigned short color, std::vector<FVector> & vertices)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen()) return;
	sceneLayer* ly = findOrCreateSceneLayer(l);
	std::vector<FVector> converted;
	for (std::vector<FVector>::iterator itr = vertices.begin();
		itr != vertices.end(); ++itr) {
		converted.push_back(addVertex(*itr));
	}
	converted.push_back(addVertex(vertices.front()));
	ly->_linestrips[correctedColorIndex(l, color)].push_back(converted);
}


void scene::addTriangles(const std::string & l, unsigned short color, std::vector<FVector> & vertices, bool inverted)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen()) return;
	sceneLayer* ly = findOrCreateSceneLayer(l);
	for (VList::iterator itr = vertices.begin();
		itr != vertices.end(); ) {
		VList::iterator a;
		VList::iterator b;
		VList::iterator c;
		if (inverted) {
			c = itr++;
			b = itr++;
			a = itr++;
		}
		else {
			a = itr++;
			b = itr++;
			c = itr++;
		}
		if (a != vertices.end() &&
			b != vertices.end() &&
			c != vertices.end()) {
			FVector n = ((*b - *a) ^ (*c - *a));
			n.Normalize();
			ly->_trinorms[correctedColorIndex(l, color)].push_back(n);
			ly->_triangles[correctedColorIndex(l, color)].push_back(addVertex(*a));
			ly->_triangles[correctedColorIndex(l, color)].push_back(addVertex(*b));
			ly->_triangles[correctedColorIndex(l, color)].push_back(addVertex(*c));
		}
	}
}
void scene::addQuads(const std::string & l, unsigned short color, std::vector<FVector> & vertices, bool inverted)
{
	dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	if (layer->getFrozen()) return;

	sceneLayer* ly = findOrCreateSceneLayer(l);
	for (VList::iterator itr = vertices.begin();
		itr != vertices.end(); ) {
		VList::iterator a = vertices.end();
		VList::iterator b = vertices.end();
		VList::iterator c = vertices.end();
		VList::iterator d = vertices.end();
		if (inverted) {
			d = itr++;
			if (itr != vertices.end())
				c = itr++;
			if (itr != vertices.end())
				b = itr++;
			if (itr != vertices.end())
				a = itr++;
		}
		else {
			a = itr++;
			if (itr != vertices.end())
				b = itr++;
			if (itr != vertices.end())
				c = itr++;
			if (itr != vertices.end())
				d = itr++;
		}
		if (a != vertices.end() &&
			b != vertices.end() &&
			c != vertices.end() &&
			d != vertices.end()) {
			FVector n = ((*b - *a) ^ (*c - *a));
			n.Normalize();
			short cindex = correctedColorIndex(l, color);
			ly->_quadnorms[cindex].push_back(n);
			VList& vl = ly->_quads[cindex];
			vl.push_back(addVertex(*a));
			vl.push_back(addVertex(*b));
			vl.push_back(addVertex(*c));
			vl.push_back(addVertex(*d));
		}
	}
}


void scene::addText(const std::string & l, unsigned short color, FVector & point/*, osgText::Text *text*/)
{
	//dxfLayer* layer = _layerTable->findOrCreateLayer(l);
	//if (layer->getFrozen()) return;
	//sceneLayer* ly = findOrCreateSceneLayer(l);

	//// Apply the scene settings to the text size and rotation

	//FVector pscene(addVertex(point));
	//FVector xvec = addVertex( point + (text->getRotation() * X_AXIS) ) - pscene;
	//FVector yvec = addVertex( point + (text->getRotation() * Y_AXIS) ) - pscene;
	//text->setCharacterSize( text->getCharacterHeight()*yvec.length(), text->getCharacterAspectRatio()*yvec.length()/xvec.length() );

	//Matrix qm = _r * _m;
	//FVector t, s;
	//Quat ro, so;
	//qm.decompose( t, ro, s, so );
	//text->setRotation( text->getRotation() * ro );

	//sceneLayer::textInfo ti( correctedColorIndex(l,color), pscene, text );
	//ly->_textList.push_back(ti);
}




unsigned short
scene::correctedColorIndex(const std::string & l, unsigned short color)
{
	if (color >= aci::MIN && color <= aci::MAX)
	{
		return color;
	}
	else if (!color || color == aci::BYLAYER)
	{
		dxfLayer* layer = _layerTable->findOrCreateLayer(l);
		unsigned short lcolor = layer->getColor();
		if (lcolor >= aci::MIN && lcolor <= aci::MAX)
		{
			return lcolor;
		}
	}
	return aci::WHITE;
}
