#include "ArmyWHCRect.h"
#include "ArmyMath.h"
#include "ArmyMathFunc.h"
bool FArmyWHCRect::Init(FVector Pos, FVector2D Dir, FVector Size, FVector2D Anchor)
{
	mDir = Dir;
	mNor = FVector2D(-mDir.Y, mDir.X);
	mSizeHalf = FVector2D(Size.X, Size.Y) * 0.5f;
	mMinZ = Pos.Z;
	mMaxZ = Pos.Z + Size.Z;
	mPos = FVector2D(Pos.X, Pos.Y) - mSizeHalf.X*Anchor.X*mDir - mSizeHalf.Y*Anchor.Y*mNor;

	mPosOrgin = mPos;
	return true;
}
bool FArmyWHCRect::Extern(float RightExt /* = 0.0f */, float LeftExt /* = 0.0f */, float FrontExt /* = 0.0f */, float BackExt /* = 0.0f */, float fTopExt /* = 0.0f */, float fBottomExt /* = 0.0f */)
{
	mPos += RightExt*0.5f*mNor;
	mSizeHalf.Y += RightExt*0.5f;

	mPos -= LeftExt*0.5f*mNor;
	mSizeHalf.Y += LeftExt*0.5f;

	mPos += FrontExt*0.5f*mDir;
	mSizeHalf.X += FrontExt*0.5f;

	mPos -= BackExt*0.5f*mDir;
	mSizeHalf.X += BackExt*0.5f;

	mMaxZ += fTopExt;
	mMinZ -= fBottomExt;
	return true;
}
FVector FArmyWHCRect::GetPos(FVector Anchor, float RightExt /* = 0.0f */, float LeftExt /* = 0.0f */, float FrontExt /* = 0.0f */, float BackExt /* = 0.0f */, float fTopExt /* = 0.0f */, float fBottomExt /* = 0.0f */)
{
	FVector2D Pos2d = mPos;
	Pos2d -= RightExt*0.5f*mNor;
	Pos2d += LeftExt*0.5f*mNor;
	Pos2d -= FrontExt*0.5f*mDir;
	Pos2d += BackExt*0.5f*mDir;
	Pos2d += (mSizeHalf.X*Anchor.X*mDir + mSizeHalf.Y*Anchor.Y*mNor);
	return FVector(Pos2d.X, Pos2d.Y, (mMinZ+mMaxZ)*0.5f+fBottomExt+(mMaxZ-mMinZ)*0.5f*Anchor.Z );
}
FVector FArmyWHCRect::GetPos(eVertXType eXType, eVertYType eYType, eVertZType eZType, float RightExt /* = 0.0f */, float LeftExt /* = 0.0f */, float FrontExt /* = 0.0f */, float BackExt /* = 0.0f */, float fTopExt /* = 0.0f */, float fBottomExt /* = 0.0f */)
{
	return GetPos(FVector(eXType, eYType, eZType), RightExt, LeftExt, FrontExt, BackExt, fTopExt, fBottomExt);
}

FVector2D FArmyWHCRect::GetPos2d(eVertXType eXType, eVertYType eYType, float RightExt /* = 0.0f */, float LeftExt /* = 0.0f */, float FrontExt /* = 0.0f */, float BackExt /* = 0.0f */, float fTopExt /* = 0.0f */, float fBottomExt /* = 0.0f */)
{
	FVector pos= GetPos(FVector(eXType, eYType, 0.0f), RightExt, LeftExt, FrontExt, BackExt, fTopExt, fBottomExt);
	return FVector2D(pos.X, pos.Y);
}

float FArmyWHCRect::GetProjectionRadius(FVector2D& Axis, float fOffset)
{
	return FMath::Abs(mDir | Axis)*(mSizeHalf.X+fOffset) + FMath::Abs(mNor | Axis)*(mSizeHalf.Y+fOffset);
}
bool FArmyWHCRect::Collision(FArmyWHCRect* pRect, float fOffset)
{
	/*
		分离轴方法判断两个矩形是否相�?
	*/

	// Z方向比较
	if (((pRect->mMaxZ+fOffset) < (mMinZ-fOffset) ) || ((pRect->mMinZ-fOffset) > (mMaxZ+fOffset)))
		return false;

	// 两个中心点距�?
	FVector2D VecDist = mPos - pRect->mPos;
	float fVecDot;

	// 中心点距离在Self.mDir方向上的投影
	fVecDot = FMath::Abs(VecDist | mDir);
	// 中心点距离大于两个Rect的X方向半长度在Self.mDir方向上投影之�?
	if (fVecDot >= 0.0001f && ((mSizeHalf.X+fOffset) + pRect->GetProjectionRadius(mDir, fOffset)) <= fVecDot)
		return false;

	// 中心点距离在Self.mNor方向上的投影
	fVecDot = FMath::Abs(VecDist | mNor);
	// 中心点距离大于两个Rect的Y方向半长度在Self.mNor方向上投影之�?
	if (fVecDot >= 0.0001f && ((mSizeHalf.Y+fOffset) + pRect->GetProjectionRadius(mNor, fOffset)) <= fVecDot)
		return false;

	// 中心点距离在Other.mDir方向上的投影
	fVecDot = FMath::Abs(VecDist | pRect->mDir);
	// 中心点距离大于两个Rect的X方向半长度在Other.mDir方向上投影之�?
	if (fVecDot >= 0.0001f && ((pRect->mSizeHalf.X+fOffset) + GetProjectionRadius(pRect->mDir, fOffset)) <= fVecDot)
		return false;

	// 中心点距离在Other.mNor方向上的投影
	fVecDot = FMath::Abs(VecDist | pRect->mNor);
	// 中心点距离大于两个Rect的Y方向半长度在Other.mNor方向上投影之�?
	if (fVecDot >= 0.0001f && ((pRect->mSizeHalf.Y+fOffset) + GetProjectionRadius(pRect->mNor,fOffset)) <= fVecDot)
		return false;

	return true;
}
bool FArmyWHCRect::Collision(FArmyWHCLineSeg& Seg, float fOffsetX, float fOffsetY)
{
	/*
		分离轴方法判断线段是否和矩形有重�?
	*/

	// 线段中心点和矩形中心点的距离
	FVector2D VecDist = Seg.mPos - mPos;
	float fVecDot;
	// @zengy 去掉偏移值限�?
	// float fOffsetY = fOffset; // (fOffset<10.0f && fOffset>0.0f)?-0.0001f:fOffset;
	// VecDist在矩形mDir方向上的投影，即矩形X方向上的投影距离
	fVecDot = FMath::Abs(VecDist | mDir);
	// VecDist > 矩形X方向半长度和线段半长度在mDir方向投影距离之和，矩形和线段无重�?
	if (fVecDot >= 0.0001f && (((mSizeHalf.X + fOffsetX) + FMath::Abs(Seg.mLenHalf*Seg.mDir | mDir) ) <= fVecDot))
		return false;

	// VecDist在矩形mNor方向上的投影，即矩形Y方向上的投影距离
	fVecDot = FMath::Abs(VecDist | mNor);
	// VecDist > 矩形Y方向半长度和线段半长度在mNor方向投影距离之和，矩形和线段无重�?
	if (fVecDot >= 0.0001f && (((mSizeHalf.Y + fOffsetY) + FMath::Abs(Seg.mLenHalf*Seg.mDir | mNor) ) <= fVecDot))
		return false;

	// VecDist在线段mDir方向上的投影
	fVecDot = FMath::Abs(VecDist | Seg.mDir);
	// VecDist > 线段半长度和ProjectRadius(在mDir方向上的project)之和，矩形和线段无重�?
	if (fVecDot >= 0.0001f && (Seg.mLenHalf + GetProjectionRadius(Seg.mDir, fOffsetX)) <= fVecDot)
		return false;

	// VecDist在线段mNor方向上的投影
	fVecDot = FMath::Abs(VecDist | Seg.mNor);
	// VecDist > 线段半长度和ProjectRadius(在mNor方向上的project)之和，矩形和线段无重�?
	if (fVecDot >= 0.0001f && (GetProjectionRadius(Seg.mNor, fOffsetY) <= fVecDot))
		return false;

	return true;
}
bool FArmyWHCRect::ChangeDir(FVector2D& Dir)
{
	mDir = Dir;
	mNor.X = -mDir.Y;
	mNor.Y = mDir.X;
	return true;
}
FRotator FArmyWHCRect::GetRotation()
{
	float fAngle = atan2f(mDir.Y,mDir.X) * 180.0f /PI;
	return FRotator(0, fAngle, 0);
}
FVector2D FArmyWHCRect::NearRect(FArmyWHCRect& Rect)
{
	// // 找到两个柜子Rect的最小距�?
	// float fMin = (Rect.GetPos2d(eXRight, eYFront) - mPos) | mDir;
	// float fDist1 = (Rect.GetPos2d(eXRight, eYBack)  - mPos) | mDir;
	// if (FMath::Abs(fDist1) < FMath::Abs(fMin))
	// 	fMin = fDist1;
	// fDist1 = (Rect.GetPos2d(eXLeft, eYFront) - mPos) | mDir;
	// if (FMath::Abs(fDist1) < FMath::Abs(fMin))
	// 	fMin = fDist1;
	// fDist1 = (Rect.GetPos2d(eXLeft, eYBack) - mPos) | mDir;
	// if (FMath::Abs(fDist1) < FMath::Abs(fMin))
	// 	fMin = fDist1;
	// // 如果两个柜子Rect的最小距离当前柜子的距离
	// if (FMath::Abs(fMin) < Rect.mSizeHalf.X)
	// {
	// 	if (fMin >= 0)
	// 		return mPos + mDir*(fMin - mSizeHalf.X);
	// 	else
	// 		return mPos + mDir*(mSizeHalf.X+fMin);	
	// }

	float fDist = (mPos - Rect.mPos) | mDir;
	if (FMath::Abs(fDist) > mSizeHalf.X + Rect.mSizeHalf.X)
		return mPos;
	else if (fDist < 0.0f)
		return Rect.mPos - Rect.mNor * mSizeHalf.Y - mDir * (mSizeHalf.X + Rect.mSizeHalf.X) + mNor * mSizeHalf.Y;
	else
		return Rect.mPos - Rect.mNor * mSizeHalf.Y + mDir * (mSizeHalf.X + Rect.mSizeHalf.X) + mNor * mSizeHalf.Y;
}
//////////////////////////////////////////////////////////////////////////
bool FArmyWHCLineSeg::Init(FVector2D& PosStart, FVector2D& PosEnd)
{
	mStart = PosStart;
	mEnd = PosEnd;

	mDir = (mEnd - mStart);
	mLenHalf = mDir.Size()*0.5f;
	mDir.Normalize();
	mPos = (mStart + mEnd) / 2.0f;
	mNor.X = -mDir.Y;
	mNor.Y = mDir.X;
	return true;
}
bool FArmyWHCLineSeg::Init(FVector PosStart, FVector PosEnd)
{
	mStart.X = PosStart.X;
	mStart.Y = PosStart.Y;
	mEnd.X = PosEnd.X;
	mEnd.Y = PosEnd.Y;
	return Init(mStart, mEnd);
}
FVector2D FArmyWHCLineSeg::NearPos(FVector2D Point, float& fDistance, float& fDot, float fAdhereThreshold)
{
	FVector2D AP = Point - mStart;
	FVector2D AB = mEnd - mStart;
	fDot = (AP | AB) / (AB | AB);

	float AdhereRatio = fAdhereThreshold / AB.Size();

	FVector2D AC = AB * fDot;
	FVector2D point_C = AC + mStart;

	if (fDot >= 1.0f - AdhereRatio)
	{
		FVector2D BP = Point - mEnd;
		point_C = mEnd;
		fDistance = BP.Size();
	}
	else if (fDot <= 0.0f + AdhereRatio)
	{
		point_C = mStart;
		fDistance = AP.Size();
	}
	else
	{
		FVector2D PC = Point - point_C;
		fDistance = PC.Size();
	}
	return point_C;
}

bool FArmyWHCLineSeg::RayCollisonWall(FVector& PosStart, FVector& Dir, float MinZ, float MaxZ, FVector &OutPositionInRoom)
{
	FPlane RoomWallPlane(FVector(mPos.X, mPos.Y, MinZ), FVector(mNor.X, mNor.Y,0) );
	FVector Intersection = FMath::LinePlaneIntersection(PosStart, PosStart + Dir * 10000.0f, RoomWallPlane);

	float ProjectDis = FVector::DotProduct(Intersection - PosStart, Dir);
	if (XRWHCMode::FloatGreater(ProjectDis, 0.0f) && XRWHCMode::FloatGreater(Intersection.Z, MinZ) && XRWHCMode::FloatLess(Intersection.Z, MaxZ) )
	{
		if ( Intersection.X+0.001f>=FMath::Min(mStart.X,mEnd.X) && Intersection.X<=FMath::Max(mStart.X,mEnd.X) + 0.001f && Intersection.Y + 0.001f >= FMath::Min(mStart.Y, mEnd.Y) && Intersection.Y <= FMath::Max(mStart.Y, mEnd.Y) + 0.001f)
		{
			OutPositionInRoom = Intersection;
			return true;
		}
		return false;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
bool FArmyWHCPoly::Init(TArray<FVector>& Points)
{
	for ( int i=0; i<Points.Num(); ++i )
	{
		FArmyWHCLineSeg Seg;
		Seg.mIdx = i;
		Seg.Init(Points[i], Points[(i+1)%Points.Num()]);
		mLines.Add(Seg);
	}
	return true;
}
FArmyWHCLineSeg* FArmyWHCPoly::Collision(FArmyWHCRect& Rect, float fOffsetX, float fOffsetY)
{
	for (int i = 0; i < mLines.Num(); ++i)
	{
		if (Rect.Collision(mLines[i], fOffsetX, fOffsetY))
			return &mLines[i];
	}
	return nullptr;
}

FArmyWHCRect* FArmyWHCPoly::CollisonChild(FArmyWHCRect& Rect, float Offset /* = 0.0f */)
{
	for (auto& p : mChildRects )
	{
		auto& RectOther = p.Value;
		if (Rect.Collision(&RectOther, Offset))
		{
			return &RectOther;
		}
	}
	return nullptr;
}
bool FArmyWHCPoly::IsIn(const FVector2D& TestPoint )
{
	const int NumPoints = mLines.Num();
	float AngleSum = 0.0f;
	for (int PointIndex = 0; PointIndex < NumPoints; ++PointIndex)
	{
		const FVector2D& VecAB = FVector2D(mLines[PointIndex].mStart - TestPoint);
		const FVector2D& VecAC = FVector2D(mLines[PointIndex].mEnd - TestPoint);
		const float Angle = FMath::Sign(FVector2D::CrossProduct(VecAB, VecAC)) * FMath::Acos(FMath::Clamp(FVector2D::DotProduct(VecAB, VecAC) / (VecAB.Size() * VecAC.Size()), -1.0f, 1.0f));
		AngleSum += Angle;
	}
	return (FMath::Abs(AngleSum) > 0.009f);
}
FVector2D FArmyWHCPoly::GetNearSeg(FVector2D Pos, int32& Idx, float& fDistance)
{
	fDistance = FLT_MAX;
	FVector2D RetPos = Pos;
	float fDis, fDot;
	for ( int32 i=0;i< mLines.Num(); ++i)
	{
		FVector2D PosCross = mLines[i].NearPos(Pos, fDis, fDot);
		if ( fDis < fDistance )
		{
			fDistance = fDis;
			Idx = i;
			RetPos = PosCross;
		}
	}
	return RetPos;
}
bool FArmyWHCPoly::IsIn(FArmyWHCRect& Rect)
{
	FVector2D Pos = Rect.GetPos2d(FArmyWHCRect::eXRight, FArmyWHCRect::eYFront);
	if (IsIn(Pos))
		return true;

	Pos = Rect.GetPos2d(FArmyWHCRect::eXLeft, FArmyWHCRect::eYFront);
	if (IsIn(Pos))
		return true;

	Pos = Rect.GetPos2d(FArmyWHCRect::eXRight, FArmyWHCRect::eYBack);
	if (IsIn(Pos))
		return true;

	Pos = Rect.GetPos2d(FArmyWHCRect::eXLeft, FArmyWHCRect::eYBack);
	if (IsIn(Pos))
		return true;

	return false;
}
bool FArmyWHCPoly::RayCollisonFloor(FVector& WorldPoint, FVector& WorldDir, float InAboveGround, FVector &OutPositionInRoom)
{
	if (mLines.Num() < 3 )
		return false;
	FPlane RoomGroundPlane( FVector(mLines[0].mPos.X, mLines[0].mPos.Y, InAboveGround), FVector::UpVector);
	FVector Intersection = FMath::LinePlaneIntersection(WorldPoint, WorldPoint + WorldDir * 10000.0f, RoomGroundPlane);

	float ProjectDis = FVector::DotProduct(Intersection - WorldPoint, WorldDir);
	FVector2D Pos2d(Intersection.X, Intersection.Y);
	if (XRWHCMode::FloatGreater(ProjectDis, 0.0f) && IsIn(Pos2d) )
	{
		OutPositionInRoom = Intersection;
		return true;
	}
	return false;
}
FArmyWHCLineSeg* FArmyWHCPoly::RayCollisonWall(FVector& PosStart, FVector& Dir, float MinZ, float MaxZ, FVector &OutPositionInRoom)
{
	for (auto& p:mLines )
	{
		if ( p.RayCollisonWall(PosStart,Dir,MinZ,MaxZ,OutPositionInRoom) )
			return &p;
	}
	return nullptr;
}