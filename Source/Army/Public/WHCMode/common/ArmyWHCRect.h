#pragma once
#include "CoreMinimal.h"

struct FArmyWHCLineSeg;

struct FArmyWHCRect
{
	enum eVertXType
	{
		eXLeft = -1,
		eXCentry = 0,
		eXRight = 1,
	};
	enum eVertYType
	{
		eYBack = -1,
		eYCentry = 0,
		eYFront = 1,
	};
	enum eVertZType
	{
		eZBottom = -1,
		eZCentry = 0,
		eZTop = 1,
	};
	FVector2D mPos;		 	// 矩形中心点
	FVector2D mSizeHalf; 	// 矩形半长度
	FVector2D mDir;		 	// 矩形X方向(World Space)
	FVector2D mNor;		 	// 矩形Y方向(World Space)
	float	 mMinZ;		 	// 矩形最小Z值
	float	 mMaxZ;			// 矩形最大Z值
	FVector2D mPosOrgin;	// 矩形未经过Extern的原始中心点
	bool Init(FVector Pos, FVector2D Dir, FVector Size, FVector2D Anchor = FVector2D(0, 0) );
	FVector GetPos(FVector Anchor = FVector(0,0,0), float RightExt = 0.0f, float LeftExt = 0.0f, float FrontExt = 0.0f, float BackExt = 0.0f, float fTopExt = 0.0f, float fBottomExt = 0.0f);
	FVector GetPos(eVertXType eXType, eVertYType eYType, eVertZType eZType=eZCentry, float RightExt = 0.0f, float LeftExt = 0.0f, float FrontExt = 0.0f, float BackExt = 0.0f, float fTopExt = 0.0f, float fBottomExt = 0.0f);
	FVector2D GetPos2d(eVertXType eXType, eVertYType eYType, float RightExt = 0.0f, float LeftExt = 0.0f, float FrontExt = 0.0f, float BackExt = 0.0f, float fTopExt = 0.0f, float fBottomExt = 0.0f);
	bool Extern(float RightExt = 0.0f, float LeftExt = 0.0f, float FrontExt = 0.0f, float BackExt = 0.0f, float fTopExt = 0.0f, float fBottomExt = 0.0f);
	bool Collision(FArmyWHCRect* pOtherRect, float fOffset = 0.0f);
	bool Collision(FArmyWHCLineSeg& Seg, float fOffsetX = 0.0f, float fOffsetY = 0.0f);
	bool ChangeDir(FVector2D& Dir);
	FRotator GetRotation();
	float SizezHalf() { return (mMaxZ - mMinZ)*0.5f; }
	FVector SizeHalf(float Offset = 0.0f) { return FVector(mSizeHalf.X, mSizeHalf.Y, SizezHalf()) + Offset; }
	FVector GetOffset() { return FVector(mPosOrgin.X - mPos.X, mPosOrgin.Y - mPos.Y, 0); }
	FVector GetDir() { return FVector(mDir.X, mDir.Y, 0); }
	FVector GetNor() { return FVector(mNor.X, mNor.Y, 0); }
	FVector2D NearRect(FArmyWHCRect& Rect);
	//
	float GetProjectionRadius(FVector2D& Axis, float fOffset = 0.0f);
	struct FShapeInRoom* mShapeInRoom;
};

struct FArmyWHCLineSeg
{
	int32	mIdx;
	FVector2D mStart;
	FVector2D mEnd;
	FVector2D mDir;
	FVector2D mNor;
	FVector2D mPos;
	float mLenHalf;//

	bool Init(FVector2D& PosStart, FVector2D& PosEnd);
	bool Init(FVector PosStart, FVector PosEnd);
	FVector2D NearPos(FVector2D Point, float& fDistance, float& fDot, float fAdhereThreshold = 0.0f);
	bool RayCollisonWall(FVector& PosStart, FVector& Dir, float MinZ,float MaxZ, FVector &OutPositionInRoom);
};

struct FArmyWHCPoly
{
	FGuid mGuid;
	TArray<FArmyWHCLineSeg> mLines;
	bool Init(TArray<FVector>& Points);
	FArmyWHCLineSeg* Collision(FArmyWHCRect& Rect, float OffsetX = 0.0f, float OffsetY = 0.0f);
	FArmyWHCRect* CollisonChild(FArmyWHCRect& Rect, float Offset = 0.0f);
	bool IsIn(const FVector2D& TestPoint);
	bool IsIn(FArmyWHCRect& Rect);
	FArmyWHCLineSeg& GetSeg(int32 Idx) { return mLines[Idx]; }
	FArmyWHCLineSeg& GetNextSeg(int32 Idx) { return mLines[(Idx + 1) % mLines.Num()]; }
	FArmyWHCLineSeg& GetPreSeg(int32 Idx) { return Idx==0?mLines[mLines.Num()-1] : mLines[Idx - 1]; }
	FVector2D GetNearSeg(FVector2D Pos,int32& Idx, float& fDistance);
	//
    bool AddRect(const FGuid& id, FArmyWHCRect& Rect) { mChildRects.Add(id, Rect); return true; }
	TMap<FGuid, FArmyWHCRect> mChildRects; // 烟道、柱子等障碍物
	//
	bool RayCollisonFloor(FVector& PosStart, FVector& Dir, float InAboveGround, FVector &OutPositionInRoom);
	FArmyWHCLineSeg* RayCollisonWall(FVector& PosStart, FVector& Dir, float MinZ, float MaxZ, FVector &OutPositionInRoom);
};