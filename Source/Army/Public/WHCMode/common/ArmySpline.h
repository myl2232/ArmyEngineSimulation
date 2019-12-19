#pragma once
#include "VertexType.h"
#define SINGLE(T)  static T & Instance(){ static T ms_Instance; return ms_Instance; }

struct FArmySplineRes
{
	FString		mName;
	FVector2D	mSize;
	FVector2D	mMin;
	FVector2D	mMax;
	TArray<float>		mVs;
	TArray<FVector2D>	mPoints;
	TArray<uint16>		mFaces;//∫·Ωÿ√Ê
	bool	Init(FString sName);
	bool	Spline(FVector2D* PosPre, FVector2D& PosStart, FVector2D& PosEnd, FVector2D* PosNext, TArray<FPUVVertex>& Vers, TArray<uint16>& OutFaces, int32 nSeg);
	bool	SideFace(TArray<FPUVVertex>& Vers, TArray<uint16>& OutFaces, TArray<FVector2D>& PathVers);
	int32	NumPoint() { return mPoints.Num(); }
protected:
	bool	Spline(FVector2D& PosStart, FVector2D& PosEnd, FVector2D& NorStart, FVector2D& NorEnd, TArray<FPUVVertex>& Vers, TArray<uint16>& OutFaces, int32 nSeg);
};

class FArmySplineResMgr
{
public:
	SINGLE(FArmySplineResMgr);
	FArmySplineRes* GetSplineRes(FString sName);
	FArmySplineRes* CreateSpline(FString sName, TArray<FVector2D>& Paths, bool bIsClose, TArray<FPUVVertex>& Vers, TArray<uint16>& Faces);
protected:
	TMap<FString, TSharedPtr<FArmySplineRes> >	 mSplineRes;
};

#define  gSplineResMgr FArmySplineResMgr::Instance()