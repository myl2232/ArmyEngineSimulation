#include "ArmySpline.h"
#include "ArmyMath.h"
#include "dxfFile.h"
#include "ArmyPolygonAssist.h"
#include "ArmyMathFunc.h"
bool FArmySplineRes::Init(FString sName)
{
	std::string FilePaths(TCHAR_TO_UTF8(*sName));
	dxfFile DxfFile(FilePaths);
	CHECK_ERROR(DxfFile.parseFile());
	CHECK_ERROR(DxfFile.GetLines(mPoints, mMin, mMax));
	//if (FArmyMath::IsClockWise(mPoints))
	FArmyMath::ReverPointList(mPoints);

	mMin *= 0.1f;
	mMax *= 0.1f;

	float RMaxY = mMin.Y;
	for (auto P : mPoints)
	{
		if (P.X == mMin.X && P.Y > RMaxY)
		{
			RMaxY = P.Y;
		}
	}
	if (RMaxY > mMin.Y)
	{
		mMax.Y = RMaxY;
	}

	mSize = mMax - mMin;

	float totalV = 0;
	mVs.Add(totalV);
	for (int i = 1; i < mPoints.Num(); ++i)
	{
		totalV += (mPoints[i] - mPoints[i-1]).Size()/51.20f;
		mVs.Add(totalV);
	}
	for (int i = 0; i < mPoints.Num(); ++i)
	{
		mPoints[i] -= mMin;
	}
	if ( mPoints.Num() >=3 )
	{
		FArmyPolygonTool tool;
		mFaces = tool.PolyToTri(mPoints);
	}
	return true;
}
FVector2D NorLineSeg(FVector2D& PosStart, FVector2D& PosEnd)
{
	FVector2D Dir(PosEnd - PosStart);
	Dir.Normalize();
	return FVector2D(-Dir.Y,Dir.X);
}
bool FArmySplineRes::Spline(FVector2D* PosPre, FVector2D& PosStart, FVector2D& PosEnd, FVector2D* PosNext, TArray<FPUVVertex>& Vers, TArray<uint16>& OutFaces, int32 nSeg)
{
	FVector2D NorSeg = NorLineSeg(PosStart,PosEnd);
	FVector2D DirStart;
	if (PosPre)
	{
		DirStart = NorLineSeg(*PosPre, PosStart)+NorSeg;
		DirStart.Normalize();
	}
	else
		DirStart = NorSeg;

	FVector2D DirEnd;
	if (PosNext)
	{
		DirEnd = NorLineSeg(PosEnd,*PosNext) + NorSeg;
		DirEnd.Normalize();
	}
	else
		DirEnd = NorSeg;

	return Spline(PosStart,PosEnd,DirStart,DirEnd,Vers,OutFaces,nSeg);
}
bool FArmySplineRes::Spline(FVector2D& PosStart, FVector2D& PosEnd, FVector2D& DirStart, FVector2D& DirEnd, TArray<FPUVVertex>& Vers, TArray<uint16>& OutFaces, int32 nSeg)
{
	float ScaleU = 1.0f / 51.2f;

	FVector2D Dir = PosEnd-PosStart;
	float LenSeg = Dir.Size();
	Dir /= LenSeg;

	float RecProNorStart = FMath::Abs(1.0f/(DirStart | FVector2D(-Dir.Y,Dir.X) ));
	float NorProDirStart = (DirStart | Dir)*RecProNorStart;

	float RecProNorEnd = FMath::Abs(1.0f / (DirEnd | FVector2D(-Dir.Y,Dir.X)));
	float NorProDirEnd = (DirEnd | Dir)*RecProNorEnd;

	FVector2D DirStartFactor = DirStart*RecProNorStart;
	FVector2D DirEndFactor = DirEnd*RecProNorEnd;

	uint16 nStartVer = (mPoints.Num()-1)*4*nSeg;
	uint16 nFaceStart = nStartVer*3/2;
	for ( int i=0; i<mPoints.Num()-1;++i )
	{
		int16 ns = nStartVer + i * 4;
		// 左下
		Vers[ns+0].Position.X = PosStart.X + DirStartFactor.X*mPoints[i].X;
		Vers[ns + 0].Position.Y = PosStart.Y + DirStartFactor.Y*mPoints[i].X;
		Vers[ns + 0].Position.Z = mPoints[i].Y;
		Vers[ns + 0].Texcoord0.X = mPoints[i].X*NorProDirStart*ScaleU;
		Vers[ns + 0].Texcoord0.Y = mVs[i];
		//左上
		Vers[ns + 1].Position.X = PosStart.X + DirStartFactor.X*mPoints[i+1].X;
		Vers[ns + 1].Position.Y = PosStart.Y + DirStartFactor.Y*mPoints[i+1].X;
		Vers[ns + 1].Position.Z = mPoints[i+1].Y;
		Vers[ns + 1].Texcoord0.X = mPoints[i+1].X*NorProDirStart*ScaleU;
		Vers[ns + 1].Texcoord0.Y = mVs[i+1];

		//右上
		Vers[ns + 2].Position.X = PosEnd.X + DirEndFactor.X*mPoints[i + 1].X;
		Vers[ns + 2].Position.Y = PosEnd.Y + DirEndFactor.Y*mPoints[i + 1].X;
		Vers[ns + 2].Position.Z = mPoints[i + 1].Y;
		Vers[ns + 2].Texcoord0.X = (LenSeg + mPoints[i + 1].X*NorProDirEnd)*ScaleU;
		Vers[ns + 2].Texcoord0.Y = mVs[i + 1];

		// 右下
		Vers[ns + 3].Position.X = PosEnd.X + DirEndFactor.X*mPoints[i].X;
		Vers[ns + 3].Position.Y = PosEnd.Y + DirEndFactor.Y*mPoints[i].X;
		Vers[ns + 3].Position.Z = mPoints[i].Y;
		Vers[ns + 3].Texcoord0.X = (LenSeg+mPoints[i].X*NorProDirEnd)*ScaleU;
		Vers[ns + 3].Texcoord0.Y = mVs[i];
		//
		int16 nf = nFaceStart + i * 6;
		OutFaces[nf + 0] = ns + 0;
		OutFaces[nf + 1] = ns + 1;
		OutFaces[nf + 2] = ns + 2;

		OutFaces[nf + 3] = ns + 0;
		OutFaces[nf + 4] = ns + 2;
		OutFaces[nf + 5] = ns + 3;
	}
	return true;
}
bool FArmySplineRes::SideFace(TArray<FPUVVertex>& Vers, TArray<uint16>& OutFaces, TArray<FVector2D>& PathVers)
{
	float ScaleU = 1.0f / 51.2f;

	//前
	FVector2D PosStart = PathVers[0];
	FVector2D DirStart = PathVers[1] - PathVers[0];
	DirStart.Normalize();
	FVector2D DirStartFactor(-DirStart.Y, DirStart.X);

	uint16 VerIdxStart = (PathVers.Num() - 1)*(mPoints.Num() - 1) * 4;
	for (int i = 0; i < mPoints.Num() - 1; ++i)
	{
		uint16 ns = VerIdxStart + i;
		Vers[ns].Position.X = PosStart.X + DirStartFactor.X*mPoints[i].X;
		Vers[ns].Position.Y = PosStart.Y + DirStartFactor.Y*mPoints[i].X;
		Vers[ns].Position.Z = mPoints[i].Y;
		Vers[ns].Texcoord0.X = mPoints[i].X*ScaleU;
		Vers[ns].Texcoord0.Y = mPoints[i].Y*ScaleU;
	}
	uint16 FaceIdxStart = VerIdxStart * 3 / 2;
	for (int i = 0; i < mFaces.Num(); ++i)
		OutFaces[FaceIdxStart + i] = mFaces[mFaces.Num()-i-1]+VerIdxStart;

	//后
	FVector2D PosEnd = PathVers[PathVers.Num() - 1];
	FVector2D DirEnd = PathVers[PathVers.Num()-1] - PathVers[PathVers.Num() - 2];
	DirEnd.Normalize();
	FVector2D DirEndFactor(-DirEnd.Y, DirEnd.X);

	uint16 LeftVertStart = VerIdxStart + mPoints.Num() - 1;
	for (int i = 0; i < mPoints.Num() - 1; ++i)
	{
		uint16 ns = LeftVertStart + i;
		Vers[ns].Position.X = PosEnd.X + DirEndFactor.X*mPoints[i].X;
		Vers[ns].Position.Y = PosEnd.Y + DirEndFactor.Y*mPoints[i].X;
		Vers[ns].Position.Z = mPoints[i].Y;
		Vers[ns].Texcoord0.X = mPoints[i].X*ScaleU;
		Vers[ns].Texcoord0.Y = mPoints[i].Y*ScaleU;
	}
	uint16 LeftFaceStart = FaceIdxStart + mFaces.Num();
	for (int i = 0; i < mFaces.Num(); ++i)
		OutFaces[LeftFaceStart + i] = mFaces[i] + LeftVertStart;
	return true;
}
//////////////////////////////////////////////////////////////////////////
FArmySplineRes* FArmySplineResMgr::GetSplineRes(FString sName)
{
	TSharedPtr<FArmySplineRes>* pRes = mSplineRes.Find(sName);
	if (pRes == nullptr)
	{
		TSharedPtr<FArmySplineRes> ResData = MakeShareable(new FArmySplineRes());
		if (!ResData->Init(sName))
		{
			return nullptr;
		}
		mSplineRes.Add(sName, ResData);
		pRes = &ResData;
	}
	return (*pRes).Get();
}
FArmySplineRes* FArmySplineResMgr::CreateSpline(FString sName, TArray<FVector2D>& PathVers, bool bIsClose, TArray<FPUVVertex>& Vers, TArray<uint16>& Faces)
{
	FArmySplineRes* ResData = GetSplineRes(sName);
	CHECK_ERROR(ResData);
	Vers.Empty();
	Faces.Empty();
	int32 NumPathVer = PathVers.Num();
	CHECK_ERROR(NumPathVer >= 2);
	if (bIsClose)
		CHECK_ERROR(NumPathVer >= 3);
	//////////////////////////////////////////////////////////////////////////
	int32 NumPoint = ResData->NumPoint();
	if ( !bIsClose )
	{
		uint16 NumVer = (NumPathVer -1)*(NumPoint - 1) * 4 + (NumPoint-1)*2;
		Vers.AddUninitialized(NumVer);
		uint16 NumFace = (NumPathVer - 1)*(NumPoint - 1) * 6 +ResData->mFaces.Num() * 2;
		Faces.AddUninitialized(NumFace);
	}
	else
	{
		uint16 NumVer = NumPathVer*(NumPoint - 1) * 4;
		Vers.AddUninitialized(NumVer);
		uint16 NumFace = NumVer / 2 * 3;
		Faces.AddUninitialized(NumFace);
	}
	if ( bIsClose )
	{
		for ( int i=0; i<NumPathVer;++i )
		{
			int PreIdx = XRWHCMode::RoundIndex(i, -1, NumPathVer);
			int EndIdx = XRWHCMode::RoundIndex(i, 1, NumPathVer);
			int NextIdx = XRWHCMode::RoundIndex(i, 2, NumPathVer);
			ResData->Spline(&PathVers[PreIdx], PathVers[i], PathVers[EndIdx], &PathVers[NextIdx], Vers, Faces, i);
		}
	}
	else
	{
		for (int i = 0; i < NumPathVer - 1; ++i)
		{
			FVector2D* PreVer = i ? &PathVers[i - 1] : nullptr;
			FVector2D* NextVer = (i != NumPathVer - 2) ? &PathVers[i + 2] : nullptr;
			ResData->Spline(PreVer, PathVers[i], PathVers[i + 1], NextVer, Vers, Faces, i);
		}
		ResData->SideFace(Vers, Faces, PathVers);
	}
	return ResData;
}