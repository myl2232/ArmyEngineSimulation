#pragma once
#include "ArmyTools/Public/XRClipper.h"
#include "ArmySceneData/Private/Data/HardModeData/Mathematics/earcut.hpp"
#include "CoreMinimal.h"
#include "poly2tri/poly2tri.h"

#define CHECK_ERROR( __condition__ ) { if(!(__condition__)){ return 0;} }
float AreaPoly(TArray<FVector2D> &poly);
//////////////////////////////////////////////////////////////////////////
class FArmyPolygonTool
{

struct TPathNode
{
	bool IsPoly;
	std::vector<p2t::Point*>	mOutLine;
	std::vector<TPathNode*>		mHoles;
	PolyNode*					mNode;
};
public:
	FArmyPolygonTool();
	~FArmyPolygonTool();
	void AddHole() { mHoles.resize(mHoles.size() + 1); }
	void AddHolePoint(float x, float y, int HoleIdx = -1);
	void AddHolePoint(FVector2D Pos, int PolyIdx = -1) { AddHolePoint(Pos.X, Pos.Y, PolyIdx); }

	void AddPoly() { mPolys.resize(mPolys.size() + 1); }
	void AddPolyPoint(float x, float y, int PolyIdx = -1);
	void AddPolyPoint(FVector2D Pos, int PolyIdx = -1) { AddPolyPoint(Pos.X, Pos.Y, PolyIdx); }
	void Clear();

	TArray<uint16>& Execute(TArray<FVector2D>& p2tVers);
	bool Execute();
	bool Clip();
	bool Poly2Tri();
	bool GetVerts(TArray<FVector2D>& p2tVers);
	TArray<uint16>& GetFaces() { return mFaces; }
	bool GetOutLines( TArray< TArray<FVector2D> >& OutLines );

	FVector2D GetPolyMax() { return mMax; }
	FVector2D GetPolyMin() { return mMin; }

	TArray<uint16>& PolyToTri(TArray<FVector2D>& Paths);
protected:
	std::vector< Path > mPolys;
	std::vector< Path > mHoles;

	TArray<TPathNode*> mPathNodes;

	std::vector<p2t::Point>			 m2tVers;
	TArray<uint16>	 mFaces;

	FVector2D	mMin;
	FVector2D	mMax;
protected:
	inline INT32 ValueToClip(float Value) { return INT32((Value + (Value > 0 ? .5 : -.5))); }
	inline float ClipToValue(INT32 Value) { return Value; }
};