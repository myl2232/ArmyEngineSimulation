#include "ArmyPolygonAssist.h"

FArmyPolygonTool::FArmyPolygonTool()
{
}
FArmyPolygonTool::~FArmyPolygonTool()
{
	Clear();
}
void FArmyPolygonTool::Clear()
{
	for (auto p : mPathNodes)
		delete(p);
	mPathNodes.Empty();
}
void FArmyPolygonTool::AddHolePoint(float x, float y, int HoleIdx /* = -1 */)
{
	if (mHoles.empty())
		AddHole();
	IntPoint P(ValueToClip(x), ValueToClip(y));
	if (HoleIdx == -1)
		mHoles.rbegin()->push_back(P);
	else
		mHoles[HoleIdx].push_back(P);
}
void FArmyPolygonTool::AddPolyPoint(float x, float y, int PolyIdx /* = -1 */)
{
	if (mPolys.empty())
		AddPoly();
	IntPoint P(ValueToClip(x), ValueToClip(y));
	if (PolyIdx == -1)
		mPolys.rbegin()->push_back(P);
	else
		mPolys[PolyIdx].push_back(P);
}

bool FArmyPolygonTool::Execute()
{
	m2tVers.clear();
	Clip();
	return true;
}
TArray<uint16>& FArmyPolygonTool::Execute(TArray<FVector2D>& p2tVers)
{
	Execute();
	Poly2Tri();
	GetVerts(p2tVers);
	return mFaces;
}

bool FArmyPolygonTool::GetVerts(TArray<FVector2D>& p2tVers)
{
	p2tVers.Empty();
	for (auto& p : m2tVers)
	{
		FVector2D v(p.x, p.y);
		p2tVers.Add(v);
	}
	return true;
}

bool FArmyPolygonTool::GetOutLines(TArray< TArray<FVector2D> >& OutLines )
{
	OutLines.Empty();
	for (auto& pNode : mPathNodes)
	{
		if (pNode->IsPoly)
		{
			TArray<FVector2D> OutLine;
			for (auto& pv : pNode->mOutLine )
			{
				FVector2D v(pv->x, pv->y);
				OutLine.Add(v);
			}
			OutLines.Add(OutLine);
		}
	}
	return true;
}
bool FArmyPolygonTool::Clip()
{
	// zengy modified 2019-2-28
	// 修改台面生成逻辑

	// 将所有柜子先做一次并集，合并后的轮廓就是台面的外轮廓
	Clipper cUnion;
	PolyTree RetPolysUnion;
	cUnion.AddPaths(mPolys, ptSubject, true);
	cUnion.Execute(ctUnion, RetPolysUnion, pftNonZero, pftNonZero);
	if (RetPolysUnion.ChildCount() == 0)
		return false;

	Clipper c;
	PolyTree RetPolys;
	std::vector<Path> Paths;
	// 将所有顶层ChildNode的轮廓放到一个数组中
	for (const auto &ChildNode : RetPolysUnion.Childs)
		Paths.emplace_back(ChildNode->Contour);
	// 用外轮廓减去内部的所有洞的轮廓
	c.AddPaths(Paths, ptSubject, true);
	c.AddPaths(mHoles, ptClip, true);
	c.Execute(ctDifference, RetPolys, pftNonZero, pftNonZero);
	IntRect rect = c.GetBounds();
	mMin.X = ClipToValue(rect.left);
	mMin.Y = ClipToValue(rect.bottom);
	mMax.X = ClipToValue(rect.right);
	mMax.Y = ClipToValue(rect.top);

	INT32 NumVer = 0;
	TMap<PolyNode*, TPathNode*> TempNodes;
	for (PolyNode* p = RetPolys.GetFirst(); p != NULL; p = p->GetNext())
	{
		NumVer += p->Contour.size();
		TPathNode* pNode = new TPathNode;
		pNode->mNode = p;
		pNode->IsPoly = !p->IsHole();
		mPathNodes.Add(pNode);
		TempNodes.Add(p, pNode);
	}
	m2tVers.resize(NumVer);

	int PointIdx = 0;
	for ( auto& pNode:mPathNodes )
	{
		Path& ph = pNode->mNode->Contour;
		for (Path::iterator p = ph.begin(); p != ph.end(); ++p)
		{
			p2t::Point& pt = m2tVers[PointIdx];
			pt.id = PointIdx;
			++PointIdx;
			pt.x = ClipToValue(p->X);
			pt.y = ClipToValue(p->Y);
			pNode->mOutLine.push_back(&pt);
		}
		for (auto& p : pNode->mNode->Childs )
		{ 
			auto pChild = *TempNodes.Find(p);
			pNode->mHoles.push_back(pChild);
		}
	}
	return true;
}
TArray<uint16>& FArmyPolygonTool::PolyToTri(TArray<FVector2D>& Paths)
{
	m2tVers.resize(Paths.Num());
	std::vector<p2t::Point*> OutLine(Paths.Num()-1);
	mFaces.Empty();
	for ( int32 i=0; i<Paths.Num()-1; ++i )
	{
		m2tVers[i].x = Paths[i].X;
		m2tVers[i].y = Paths[i].Y;
		m2tVers[i].id = i;
		OutLine[i] = &m2tVers[i];
	}
	p2t::CDT cdt(OutLine);
	cdt.Triangulate();
	for (auto& p : cdt.GetTriangles())
	{
		mFaces.Add(p->GetPoint(1)->id);
		mFaces.Add(p->GetPoint(0)->id);
		mFaces.Add(p->GetPoint(2)->id);
	}
	return mFaces;
}
bool FArmyPolygonTool::Poly2Tri()
{
	mFaces.Empty();
	std::vector<p2t::Point*>::size_type num = 0;
	for (auto& pNode : mPathNodes)
	{
		if ( pNode->IsPoly )
		{
			num += pNode->mOutLine.size();
			p2t::CDT cdt(pNode->mOutLine);
			for (auto& p : pNode->mHoles )
			{
				cdt.AddHole(p->mOutLine);
				num += p->mOutLine.size();
			}

			cdt.Triangulate();
			for (auto& p : cdt.GetTriangles())
			{
				uint16 id1 = p->GetPoint(1)->id;
				uint16 id2 = p->GetPoint(0)->id;
				uint16 id3 = p->GetPoint(2)->id;
				// 当索引值小于总顶点数量时，才会创建新的三角面
				// 防止在TypedPrimitive中生成切空间三向量时出现数组越界的情况
				if (id1 < num && id2 < num && id3 < num) 
				{
					mFaces.Add(id1);
					mFaces.Add(id2);
					mFaces.Add(id3);
				}
			}
		}
	}
	return true;
}

float AreaPoly(TArray<FVector2D> &poly)
{
	int size = (int)poly.Num();
	if (size < 3) return 0;

	double a = 0;
	for (int i = 0, j = size - 1; i < size; ++i)
	{
		a += ((double)poly[j].X + poly[i].X) * ((double)poly[j].Y - poly[i].Y);
		j = i;
	}
	return -a * 0.5;
}
