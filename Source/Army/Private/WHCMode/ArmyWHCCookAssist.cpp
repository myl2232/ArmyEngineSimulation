#include "ArmyWHCCookAssist.h"
#include "ArmyWHCabinet.h"
#include "ArmyShapeFrameActor.h"
#include "common/XRMathFunc.h"
#include "common/XRPolygonAssist.h"
#include "SCTCabinetShape.h"
#include "SCTShapeActor.h"
#include "Math/UnrealMathUtility.h"
#include "ArmyRoom.h"
#include "ArmyWHCTableObject.h"
#include "ArmyMath.h"
#include "ArmyPillar.h"
#include "ArmySceneData.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

static int gBoxId = 0;
ETinyBoxType	mCurBoxType;//地柜或高柜当前计算的空间
FVector2D RoundVector(FVector2D Pos)
{
	// return FVector2D( int(Pos.X + (Pos.X > 0 ? .5 : -.5)), int(Pos.Y + (Pos.Y > 0 ? .5 : -.5)));
	return FVector2D(FMath::RoundToInt(Pos.X), FMath::RoundToInt(Pos.Y));
}
//////////////////////////////////////////////////////////////////////////
void FArmyWHEdgeLine::SortLine()
{
	for ( int i=0; i<mChilds.Num(); ++i )
	{
		TSharedPtr<FArmyWHTinyLine>& pLine = mChilds[i];
		pLine->mDist2OwnerStart = FVector2D::Distance(pLine->mStart, mStart);
		float fDist2OwnerEnd = FVector2D::Distance(pLine->mEnd, mStart);
		if (fDist2OwnerEnd < pLine->mDist2OwnerStart)
		{
			FVector2D v = pLine->mStart;
			pLine->mStart = pLine->mEnd;
			pLine->mEnd = v;
			pLine->mDist2OwnerStart = fDist2OwnerEnd;
		}
	}
	mChilds.Sort([&](const TSharedPtr<FArmyWHTinyLine>& A, const TSharedPtr<FArmyWHTinyLine>& B)
	{
		return A->mDist2OwnerStart < B->mDist2OwnerStart;
	});
}
int FArmyWHEdgeLine::GetLine(int iOpp, TArray<FArmyWHSeg>& Lines)
{
	if (iOpp == eEdgeNull)
	{
		if (mType == eEdgeNull)
		{
			FArmyWHSeg seg(mStart, mEnd);
			seg.mNormal = mNormal;
			seg.mEdgeId = mBoxEdgeIdx;
			Lines.Add(seg);
			return 1;
		}
		if (mType == eEdgeLinkPart)
		{
			int NumLine = 0;
			FVector2D s = mStart;
			FVector2D Dir = (mEnd - mStart);
			for (int i = 0; i < mChilds.Num(); ++i)
			{
				if (FVector2D::DistSquared(mChilds[i]->mStart, s)> Epsilon+1.0f)
				{
					if (((mChilds[i]->mStart - s) | Dir)>=0.0f)
					{
						FArmyWHSeg seg(s, mChilds[i]->mStart);
						seg.mNormal = mNormal;
						seg.mEdgeId = mBoxEdgeIdx;
						++NumLine;
						Lines.Add(seg);
					}
				}
				if (((mChilds[i]->mEnd - mStart) | Dir) >= ((s - mStart) | Dir)+0.1 )
					s = mChilds[i]->mEnd;
			}
			if (FVector2D::DistSquared(mEnd, s) > Epsilon+1.01f)
			{
				FArmyWHSeg seg(s, mEnd);
				seg.mNormal = mNormal;
				seg.mEdgeId = mBoxEdgeIdx;
				++NumLine;
				Lines.Add(seg);
			}
			return NumLine;
		}
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////
int FArmyWHTinyBox::GetLine(int iOpp, TArray<FArmyWHSeg>& Lines)
{
	int NumLine = 0;
	if (iOpp == eEdgeNull)
	{
		if (mType == eBoxLinkSlot || mType == eBoxFillSlotHigh)
		{
			FArmyWHSeg seg(mVerPos[0], mVerPos[3]);
			seg.mNormal = mDir;
			seg.mEdgeId = 3;
			Lines.Add(seg);
			return 1;
		}
		if (mType == eBoxFloor || mType == eBoxHang || mType == eBoxHigh || mType == eBoxOnFloor)
		{
			for (int i = 0; i < 4; ++i)
			{
				NumLine += mEdge[i].GetLine(iOpp, Lines);
			}
			return NumLine;
		}
	}
	return 0;
}
bool FArmyWHTinyBox::IsCoincident(const FArmyWHTinyBox &OtherBox) const
{
#define SAME_POINT(Point1, Point2) \
	((FMath::RoundToInt(Point1.X) == FMath::RoundToInt(Point2.X)) && (FMath::RoundToInt(Point1.Y) == FMath::RoundToInt(Point2.Y)))

	int32 FirstMatching = -1;
	for (int32 i = 0; i < 4; ++i)
	{
		if (SAME_POINT(mVerPos[0], OtherBox.mVerPos[i]))
		{
			FirstMatching = i;
			break;
		}
	}

	if (FirstMatching == -1)
		return false;
	else
	{
		return SAME_POINT(mVerPos[0], OtherBox.mVerPos[FirstMatching]) && 
			SAME_POINT(mVerPos[1], OtherBox.mVerPos[(FirstMatching + 1) % 4]) &&
			SAME_POINT(mVerPos[2], OtherBox.mVerPos[(FirstMatching + 2) % 4]) &&
			SAME_POINT(mVerPos[3], OtherBox.mVerPos[(FirstMatching + 3) % 4]);
	}
}
FArmyWHTinyBox::FArmyWHTinyBox()
{
	mId = ++gBoxId;
	for (int i = 0; i < 4; ++i)
	{
		mWallId[i] = -1;
		mEdge[i].mType = eEdgeNull;
	}
	mHeight.X = -FLT_MAX;
	mHeight.Y = FLT_MAX;
	mPillarInConerFill = false;
}
void FArmyWHTinyBox::GenVerPos()
{
	FVector2D pos0 = mSizeHalf.X*mDir;
	FVector2D pos1 = mSizeHalf.Y* FVector2D(-mDir.Y,mDir.X);
	mVerPos[0] = mPos + pos0+pos1;
	mVerPos[1] = mPos - pos0+pos1;
	mVerPos[2] = mPos - pos0-pos1;
	mVerPos[3] = mPos + pos0-pos1;
}
/*
	计算二维点到二维线段的投影点
	point_A: 线段起点
	point_B: 线段终点
	point_P: 任意二维点
	fDot: 投影点到起点的距离，(-inf, 0) -- A (0) -- (0, 1) -- B (1) -- (0, inf)
	point_C: 投影点，当投影点落在线段内部时，返回投影点坐标，在起点左侧返回起点，在终点右侧返回终点
	ret: 返回任意点到投影点的距离
*/
float Point2LineSeg(const FVector2D& point_A, const FVector2D& point_B, const FVector2D& point_P, float &fDot, FVector2D &point_C)
{
	FVector2D AP = point_P - point_A;
	FVector2D AB = point_B - point_A;
	fDot = (AP|AB)/(AB|AB);

	if (fDot > 1)
	{
		FVector2D BP = point_P - point_B;
		point_C = point_B;
		return BP.Size();
	}
	else if (fDot < 0)
	{
		point_C = point_A;
		return AP.Size();
	}
	else
	{
		point_C = AB * fDot + point_A;
		FVector2D PC = point_P - point_C;
		return PC.Size();
	}
};

float Point2Line(FVector2D& Pos, FVector2D& PosStart, FVector2D& PosEnd)
{
	float fDot;
	FVector2D Point_C;
	return Point2LineSeg(PosStart, PosEnd, Pos, fDot, Point_C);
}

bool FArmyWHTinyBox::Pos2Dist(FVector2D& Pos, FArmyWHEdgeRelVeter& RelData, bool b)
{
	RelData.mDistance = FLT_MAX;
	for (int i = 0; i < 4; ++i)
	{
		float fDot;
		FVector2D CrossPos;
		float fDist = Point2LineSeg(mVerPos[i], mVerPos[(i+1)%4], Pos, fDot,CrossPos);
		if (fDist<RelData.mDistance && (fDot >= -0.005f) && (fDot <= 1.005f) )
		{
			RelData.mDistance = fDist;
			int Id = i+100;
			if (fabs(fDot) < Epsilon+0.005f)
				Id = i;
			else if (fabs(fDot - 1.0f) < Epsilon+0.005f)
				Id = (i + 1) % 4;
			if (b)
				RelData.mId1 = Id;
			else
				RelData.mId0 = Id;
			RelData.mVerPos = Pos;
			RelData.mCrossPos = CrossPos;
		}
	}
	// zengy added 0.05厘米容差
	return (RelData.mDistance <= (MinFlapLen + 0.05f));
}
float FArmyWHTinyBox::Pos2Dist(FVector2D& Pos)
{
	float MinDis = FLT_MAX;
	float fDot;
	FVector2D CrossPos;
	for (int i = 0; i < 4; ++i)
	{
		float fDistance = Point2LineSeg(mVerPos[i], mVerPos[(i + 1) % 4], Pos, fDot, CrossPos);
		if (fDistance < MinDis )
			MinDis = fDistance;
	}
	return MinDis;
}
int32 FArmyWHTinyBox::Collision(TSharedPtr<FArmyWHTinyBox>& pBox, FArmyWHEdgeRelVeter* pDetailData, bool b)
{
	int32 nNum=0;
	for ( int i=0; i<4; ++i )
	{
		if (pBox->Pos2Dist(mVerPos[i], *pDetailData,b))
		{
			if (b)
				pDetailData->mId0 = i;
			else
				pDetailData->mId1 = i;
			++pDetailData;
			++nNum;
		}
	}
	return nNum;
}
TSharedPtr<FArmyWHNeighbor> FArmyWHTinyBox::GenRelation(TSharedPtr<FArmyWHTinyBox>& pBox1)
{
	float PosDistance = (mPos - pBox1->mPos).Size();
	float fMaxDistance = (FMath::Max(mSizeHalf.X, mSizeHalf.Y) + FMath::Max(pBox1->mSizeHalf.X, pBox1->mSizeHalf.Y))/cos(PI/4)+ MinFlapLen;
	if (PosDistance > fMaxDistance)
		return nullptr;

	FArmyWHEdgeRelVeter TotalRel[8];
	int32 NumCollison = Collision(pBox1, TotalRel,true);
	
	TSharedPtr<FArmyWHTinyBox> pBoxThis = this->AsShared();
	NumCollison += pBox1->Collision(pBoxThis, &TotalRel[NumCollison],false);
	if (NumCollison < 2)
		return nullptr;

	int32 Id0MinDist = 0;
	int32 Id1SecDist = 1;
	for ( int idx=1; idx<NumCollison;++idx)
	{
		if (!(TotalRel[Id0MinDist].mId0 == TotalRel[idx].mId0 && TotalRel[Id0MinDist].mId1 == TotalRel[idx].mId1))//重复
		{
			Id1SecDist = idx;
			break;
		}
	}
	if (TotalRel[0].mDistance > TotalRel[Id1SecDist].mDistance)
		Swap(Id0MinDist, Id1SecDist);
	for ( int i= Id1SecDist+1;i<NumCollison; ++i )
	{
		if (TotalRel[Id0MinDist].mId0 == TotalRel[i].mId0 && TotalRel[Id0MinDist].mId1 == TotalRel[i].mId1)
			continue;

		if (TotalRel[Id1SecDist].mId0 == TotalRel[i].mId0 && TotalRel[Id1SecDist].mId1 == TotalRel[i].mId1)
			continue;

		if (TotalRel[i].mDistance < TotalRel[Id0MinDist].mDistance)
		{
			Id1SecDist = Id0MinDist;
			Id0MinDist = i;
			continue;
		}
		else if (TotalRel[i].mDistance < TotalRel[Id1SecDist].mDistance)
			Id1SecDist = i;
	}

	if (TotalRel[Id0MinDist].mId0 == TotalRel[Id1SecDist].mId0 && TotalRel[Id0MinDist].mId1 == TotalRel[Id1SecDist].mId1)//重复
		return nullptr;

	TSharedPtr<FArmyWHNeighbor> pNeighbor = MakeShareable(new FArmyWHNeighbor());
	pNeighbor->mBox0 = pBoxThis;
	pNeighbor->mBox1 = pBox1;
	pNeighbor->mDetail[0] = TotalRel[Id0MinDist];
	pNeighbor->mDetail[1] = TotalRel[Id1SecDist];
	return pNeighbor;
}
bool FArmyWHTinyBox::IsOnWallCorner(int& WallId0,int& WallId1)
{
	WallId0 = -1;
	WallId1 = -1;
	for ( int i=0;i<4;++i)
	{
		if ( (mWallId[i]!=-1) && (mWallId[i] != WallId0))
		{
			if ( WallId0==-1)
			{
				WallId0 = mWallId[i];
			}
			else
			{
				WallId1 = mWallId[i];
				return true;
			}
		}
	}
	return false;
}
float FArmyWHTinyBox::GetVerWallLen(int WallId)
{
	for( int i=0; i<4;++i)
	{
		if ( ((mWallId[i]==WallId)&& (mWallId[(i + 1) % 4]==-1))  || ((mWallId[i] == -1) && (mWallId[(i + 1) % 4] == WallId)) )
		{
			return (mVerPos[i] - mVerPos[(i + 1) % 4]).Size();
		}
	}
	return -1;
}
bool FArmyWHTinyBox::IsOnWall(int WallId)
{
	for (int i = 0; i < 4; ++i)
	{
		if (mWallId[i] == WallId)
			return true;
	}
	return false;
}
bool FArmyWHTinyBox::Init(FVector2D Pos, FVector2D HalfSize, FVector2D Dir, ETinyBoxType eType)
{
	mPos = Pos;
	mSizeHalf = HalfSize;
	mDir = Dir; mDir.Normalize();
	mType = eType;
	GenVerPos();

	mEdge[3].mNormal = mDir;
	mEdge[0].mNormal = FVector2D(-mDir.Y, mDir.X);
	mEdge[1].mNormal = -mDir;
	mEdge[2].mNormal = -mEdge[0].mNormal;

	mEdge[0].mBoxEdgeIdx = 0;
	mEdge[1].mBoxEdgeIdx = 1;
	mEdge[2].mBoxEdgeIdx = 2;
	mEdge[3].mBoxEdgeIdx = 3;

	mEdge[0].mOwner = this->AsShared();
	mEdge[1].mOwner = this->AsShared();
	mEdge[2].mOwner = this->AsShared();
	mEdge[3].mOwner = this->AsShared();
	return true;
}
bool FArmyWHTinyBox::GetTrimLine()
{
	switch (mType)
	{
	case eBoxNull:
		break;
	case eBoxHang:
	case eBoxFloor:
	case eBoxHigh:
	case eBoxOnFloor:
	{
		for ( int i=0; i<4; ++i)
		{
			GetTrimLineIdx( i);
			mEdge[i].SortLine();
		}
	}
		break;
	case eBoxFillSlot:
		break;
	case eBoxWall:
		break;
	case eBoxPillar:
		break;
	case eBoxLinkSlot:
	case eBoxFillSlotHigh:
	{
		GetTrimLineLinkSlot();
	}
		break;
	default:
		break;
	}
	return true;
}
bool FArmyWHTinyBox::GetTrimLineLinkSlot()
{
	return true;
}
void EdgeId2Corner(int EdgeId, int& CornerId0, int& CornerId1)
{
	CornerId0 = EdgeId;
	CornerId1 = (EdgeId + 1) % 4;
}
bool FArmyWHTinyBox::GetTrimLineIdx(int EdgeId)
{
	if (mEdge[EdgeId].mType == eEdgeFull)
		return true;
	int CornerId0, CornerId1;
	EdgeId2Corner(EdgeId, CornerId0, CornerId1);
	mEdge[EdgeId].mStart = mVerPos[CornerId0];
	mEdge[EdgeId].mEnd   = mVerPos[CornerId1];

	if ( mWallId[CornerId0]!=-1 && mWallId[CornerId1]!=-1 )//��ǽ
	{
		mEdge[EdgeId].mType = eEdgeOnWall;
		return true;
	}
	for (auto & It : mNeighbors)
	{
		TSharedPtr<FArmyWHNeighbor> pNeighbor  = It.Value.Pin();
		FVector2D CrossPos0, CrossPos1;
		INT32	Idx0, Idx1;
		if( pNeighbor->mBox0.Get() == this )
		{
			pNeighbor->GetBox0(CrossPos0, Idx0, CrossPos1, Idx1);
			if ( (Idx0 == CornerId0 && Idx1 == CornerId1) || (Idx0 == CornerId1 && Idx1 == CornerId0) )
			{
				mEdge[EdgeId].mType = eEdgeFull;
				return true;
			}
			if ( (Idx0 == 100 + EdgeId) || (Idx1 == 100 + EdgeId) )
			{
				if (pNeighbor->mBox1->mType == eBoxPillar || pNeighbor->mBox1->mType == eBoxWall)
				{
					if (mCurBoxType == eBoxFloor)
						continue;

					if (mCurBoxType == eBoxHang || mCurBoxType == eBoxOnFloor)
					{
						if (pNeighbor->mDetail[0].mDistance > 40)
							continue;
					}
				}
				mEdge[EdgeId].AddLine(pNeighbor->mBox1, CrossPos0, CrossPos1);
				mEdge[EdgeId].mType = eEdgeLinkPart;
			}
		}
		if (pNeighbor->mBox1.Get() == this)
		{
			pNeighbor->GetBox1(CrossPos0, Idx0, CrossPos1, Idx1);
			if ((Idx0 == CornerId0 && Idx1 == CornerId1) || (Idx0 == CornerId1 && Idx1 == CornerId0))
			{
				mEdge[EdgeId].mType = eEdgeFull;
				return true;
			}
			if ((Idx0 == 100 + EdgeId) || (Idx1 == 100 + EdgeId))
			{
				if ((pNeighbor->mBox0->mType == eBoxPillar || pNeighbor->mBox0->mType == eBoxWall))
				{
					if (mCurBoxType == eBoxFloor)
					{
						continue;
					}
					if (mCurBoxType == eBoxHang || mCurBoxType == eBoxOnFloor)
					{
						if (pNeighbor->mDetail[0].mDistance > 40)
							continue;
					}
				}
				mEdge[EdgeId].AddLine(pNeighbor->mBox0, CrossPos0, CrossPos1);
				mEdge[EdgeId].mType = eEdgeLinkPart;
			}
		}
	}
	return true;
}

TSharedPtr<FArmyWHTinyBox> FArmyWHTinyBox::GetNeighborBox()
{
	if (mFromNeighbor->mBox0->mType == eBoxFloor || mFromNeighbor->mBox0->mType == eBoxHigh || mFromNeighbor->mBox0->mType == eBoxHang || mFromNeighbor->mBox0->mType == eBoxOnFloor)
	{
		return mFromNeighbor->mBox0;
	}
	if (mFromNeighbor->mBox1->mType == eBoxFloor || mFromNeighbor->mBox1->mType == eBoxHigh || mFromNeighbor->mBox1->mType == eBoxHang || mFromNeighbor->mBox1->mType == eBoxOnFloor)
	{
		return mFromNeighbor->mBox1;
	}
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
bool FArmyWHEdgeLine::AddLine(TSharedPtr<FArmyWHTinyBox>& pBox, FVector2D Start, FVector2D End)
{
	TSharedPtr<FArmyWHTinyLine> pLine = MakeShareable( new FArmyWHTinyLine(pBox, Start, End) );
	pLine->mOwner = mOwner;
	pLine->mBox = pBox;
	pLine->mBoxEdgeIdx = mBoxEdgeIdx;
	pLine->mNormal = mNormal;
	mChilds.Add(pLine);
	return true;
}
//////////////////////////////////////////////////////////////////////////
TSharedPtr<FArmyWHTinyBox> FArmyWHNeighbor::CreatLinkSlot()
{
	FVector2D VecHor;
	FVector2D HalfSize; HalfSize.Y = mDetail[0].mDistance/2;
	FVector2D Pos = (mDetail[0].mVerPos+ mDetail[0].mCrossPos+mDetail[1].mVerPos+ mDetail[1].mCrossPos) / 4;

	mLinkBit = 0;
	FVector2D Box0Cross0, Box0Cross1;
	INT32	Box0Idx0, Box0Idx1;
	GetBox0(Box0Cross0, Box0Idx0, Box0Cross1, Box0Idx1);

	FVector2D Box1Cross0, Box1Cross1;
	INT32	Box1Idx0, Box1Idx1;
	GetBox1(Box1Cross0, Box1Idx0, Box1Cross1, Box1Idx1);

	if ( mBox0->mType == eBoxFloor || mBox0->mType== eBoxHang || mBox0->mType == eBoxHigh || mBox0->mType == eBoxOnFloor)
	{
		if ((Box0Idx0 == 0 && Box0Idx1 == 1)|| (Box0Idx0 == 1 && Box0Idx1 == 0) )//整个左侧板全相邻 
		{
			if ( Box0Idx0 == 0 )
				VecHor = Box0Cross0 - Box0Cross1;
			else
				VecHor = Box0Cross1 - Box0Cross0;
			mLinkBit =1;
		}
		if ( (Box0Idx0 == 0 && Box0Idx1 == 100)|| (Box0Idx0 == 100 && Box0Idx1 == 0) )//左侧板 从左上开始相邻
		{
			if (Box0Idx0 == 0)
				VecHor = Box0Cross0 - Box0Cross1;
			else
				VecHor = Box0Cross1 - Box0Cross0;
			mLinkBit =2;
		}
		if ((Box0Idx0 == 3 && Box0Idx1 == 2)|| (Box0Idx0 == 2 && Box0Idx1 == 3))//整个右侧板全相邻 
		{
			if (Box0Idx0 == 3)
				VecHor = Box0Cross0 - Box0Cross1;
			else
				VecHor = Box0Cross1 - Box0Cross0;
			mLinkBit =3;
		}
		if ( (Box0Idx0 == 3 && Box0Idx1 == 102)|| (Box0Idx0 == 102 && Box0Idx1 == 3) )//右侧板 从右上开始相邻
		{
			if (Box0Idx0 == 3)
				VecHor = Box0Cross0 - Box0Cross1;
			else
				VecHor = Box0Cross1 - Box0Cross0;
			mLinkBit =4;
		}
	}
	if ( mBox1->mType == eBoxFloor || mBox1->mType == eBoxHang || mBox1->mType == eBoxHigh || mBox1->mType == eBoxOnFloor)
	{
		if (mLinkBit != 3 && mLinkBit != 4)//另一个的右侧板没有调整板  
		{
			if (((Box1Idx0 == 0 && Box1Idx1 == 1) || (Box1Idx0 == 1 && Box1Idx1 == 0)))//整个左侧板全相邻  
			{
				if (Box1Idx0 == 0)
					VecHor = Box1Cross0 - Box1Cross1;
				else
					VecHor = Box1Cross1 - Box1Cross0;
				mLinkBit +=100;
			}
			if ((Box1Idx0 == 0 && Box1Idx1 == 100) || (Box1Idx0 == 100 && Box1Idx1 == 0))//左侧板 从左上开始相邻
			{
				if (Box1Idx0 == 0)
					VecHor = Box1Cross0 - Box1Cross1;
				else
					VecHor = Box1Cross1 - Box1Cross0;
				mLinkBit += 200;
			}
		}
		if (mLinkBit != 1 && mLinkBit != 2)
		{
			if ((Box1Idx0 == 3 && Box1Idx1 == 2) || (Box1Idx0 == 2 && Box1Idx1 == 3))//整个右侧板全相邻
			{
				if (Box1Idx0 == 3)
					VecHor = Box1Cross0 - Box1Cross1;
				else
					VecHor = Box1Cross1 - Box1Cross0;
				mLinkBit += 300;
			}
			if ((Box1Idx0 == 3 && Box1Idx1 == 102) || (Box1Idx0 == 102 && Box1Idx1 == 3))//右侧板 从右上开始相邻
			{
				if (Box1Idx0 == 3)
					VecHor = Box1Cross0 - Box1Cross1;
				else
					VecHor = Box1Cross1 - Box1Cross0;
				mLinkBit += 400;
			}
		}
	}
	ETinyBoxType eType = eBoxLinkSlot;
	if ( (mBox0->mType == eBoxHigh && mBox1->mType == eBoxHigh) || 
			(mBox0->mType == eBoxHigh && mBox1->mType != eBoxFloor && mBox1->mType != eBoxHang && mBox1->mType != eBoxOnFloor) || 
			(mBox1->mType == eBoxHigh && mBox0->mType != eBoxFloor && mBox0->mType != eBoxHang && mBox0->mType != eBoxOnFloor) )
	{
		eType = eBoxFillSlotHigh;
	}
	HalfSize.X = VecHor.Size() / 2;
	if (mLinkBit && HalfSize.X >0.01f &&  HalfSize.Y > 0.25f)
	{
		TSharedPtr<FArmyWHTinyBox> pBox = MakeShareable(new FArmyWHTinyBox());
		pBox->mNumAdjustBorad = mLinkBit;
		pBox->mFromNeighbor = this;
		pBox->Init(Pos, HalfSize, VecHor, eType);
		return pBox;
	}

	return TSharedPtr<FArmyWHTinyBox>();
}
bool FArmyWHNeighbor::GetBox0(FVector2D& CrossPos0, int&Idx0, FVector2D& CrossPos1, int&Idx1)
{
	if ( mDetail[0].mId0 <4 && mDetail[0].mId0 >=0 )
	{
		CrossPos0 = mBox0->mVerPos[mDetail[0].mId0];
		Idx0 = mDetail[0].mId0;
	}
	if (mDetail[0].mId0 < 104 && mDetail[0].mId0 >= 100)
	{
		CrossPos0 = mDetail[0].mCrossPos;
		Idx0 = mDetail[0].mId0;
	}
	//////////////////////////////////////////////////////////////////////////
	if (mDetail[1].mId0 < 4 && mDetail[1].mId0 >= 0)
	{
		CrossPos1 = mBox0->mVerPos[mDetail[1].mId0];
		Idx1 = mDetail[1].mId0;
	}
	if (mDetail[1].mId0 < 104 && mDetail[1].mId0 >= 100)
	{
		CrossPos1 = mDetail[1].mCrossPos;
		Idx1 = mDetail[1].mId0;
	}
	return true;
}
bool FArmyWHNeighbor::GetBox1(FVector2D& CrossPos0, int&Idx0, FVector2D& CrossPos1, int&Idx1)
{
	if (mDetail[0].mId1 < 4 && mDetail[0].mId1 >= 0)
	{
		CrossPos0 = mBox1->mVerPos[mDetail[0].mId1];
		Idx0 = mDetail[0].mId1;
	}
	if (mDetail[0].mId1 < 104 && mDetail[0].mId1 >= 100)
	{
		CrossPos0 = mDetail[0].mCrossPos;
		Idx0 = mDetail[0].mId1;
	}
	//////////////////////////////////////////////////////////////////////////
	if (mDetail[1].mId1 < 4 && mDetail[1].mId1 >= 0)
	{
		CrossPos1 = mBox1->mVerPos[mDetail[1].mId1];
		Idx1 = mDetail[1].mId1;
	}
	if (mDetail[1].mId1 < 104 && mDetail[1].mId1 >= 100)
	{
		CrossPos1 = mDetail[1].mCrossPos;
		Idx1 = mDetail[1].mId1;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::AddTinyBox(FVector2D Pos, FVector2D HalfSize, FVector2D Dir, ETinyBoxType eType)
{
	TSharedPtr<FArmyWHTinyBox> pBox = MakeShareable(new FArmyWHTinyBox());
	pBox->Init(Pos, HalfSize, Dir, eType);
	if ( eType == eBoxPillar )
	{
		mPillars.Add(pBox->mId, pBox);
	}
	else
	{
		mBoxs.Add(pBox->mId, pBox);
	}
	GenWallBoxRel(pBox);
	return pBox;
}
bool FArmyWHCCookAssist::GenNeighbors()
{
	for (auto& It : mBoxs)
	{
		TSharedPtr<FArmyWHTinyBox>& pBox0 = It.Value;
		if (pBox0->mPillarInConerFill && (mCurBoxType == eBoxHang || mCurBoxType == eBoxOnFloor))
			continue;
		for (auto & It2 : mBoxs)
		{
			TSharedPtr<FArmyWHTinyBox>& pBox1 = It2.Value;
			if (pBox0.Get() == pBox1.Get() )
				continue;
			if ( (pBox0->mType == eBoxFloor && (pBox1->mType == eBoxHang || pBox1->mType == eBoxOnFloor)) || (pBox1->mType == eBoxFloor && (pBox0->mType == eBoxHang || pBox0->mType == eBoxOnFloor)) )
				continue;
			if ( pBox1->mPillarInConerFill && (mCurBoxType == eBoxHang || mCurBoxType == eBoxOnFloor) )
				continue;
			INT32 Id = (pBox0->mId > pBox1->mId) ? (pBox0->mId * 1000 + pBox1->mId) : (pBox1->mId * 1000 + pBox0->mId);
			TSharedPtr<FArmyWHNeighbor>* pNeighbor = mNeighbors.Find(Id);
			if (pNeighbor)
			{
				pBox0->mNeighbors.Add(Id, *pNeighbor);
				continue;
			}
			TSharedPtr<FArmyWHNeighbor> pNeighborNew = pBox0->GenRelation(pBox1);
			if (pNeighborNew.IsValid() )
			{
				pNeighborNew->mId = Id;
				pBox0->mNeighbors.Add(Id, pNeighborNew);
				mNeighbors.Add(Id, pNeighborNew);
			}
		}
	}
	for (auto& p :mNeighbors)
	{
		TSharedPtr<FArmyWHNeighbor>& pNeighbor = p.Value;
		if (pNeighbor->mDetail[0].mDistance >= 0.01f)
		{
			pNeighbor->mType = eNeighborGap;
		}
		else
		{
			pNeighbor->mType = eNeighborEdge;
		}
	}
	GenBoxFillVisble();
	return true;
}
bool FArmyWHCCookAssist::GenBoxFillVisble()
{
	for (auto& p : mNeighbors)
	{
		TSharedPtr<FArmyWHNeighbor>& pNeighbor = p.Value;
		if (pNeighbor->mType == eNeighborGap)
		{
			TSharedPtr<FArmyWHTinyBox> pBox = pNeighbor->CreatLinkSlot();
			if (pBox.IsValid())
			{
				// zengy add: 2019-3-20 在生成Gap的Box之后检查Box是否和某个墙角占位Box重合，如果重合这个Gap是不需要生成调整板的
				bool bCoincident = false;
				TMap<INT32, TSharedPtr<FArmyWHTinyBox> >::TConstIterator cIter(mFillSlotCorners);
				for (; cIter; ++cIter)
				{
					if (pBox->IsCoincident(*(cIter.Value().Get())))
					{
						bCoincident = true;
						break;
					}
				}
				if (!bCoincident)
					mBoxs.Add(pBox->mId, pBox);
			}
		}
	}
	return true;
}
int WallLinkCornerId(int wall0, int wall1)
{
	if ( FMath::Abs(wall1-wall0)>1 )
		return FMath::Min(wall0, wall1);
	return FMath::Max(wall0, wall1);
}
bool FArmyWHCCookAssist::GenBoxFillPillars()
{
	for (auto& p :mPillars)
	{
		if (AdjustPillar(p.Value).IsValid())
		{
			mBoxs.Add(p.Value->mId, p.Value);
		}
	}
	for (int i=0; i<mNumWall; ++i )
	{
		AddCornerSlot(i,nullptr);
	}
	for (int i = 0; i < mNumWall; ++i)
	{
		FVector2D Pos0 = WallPoint[i];
		FVector2D Pos1 = WallPoint[(i+1)%mNumWall];
		FVector2D Dir = (Pos0 - Pos1);
		float fWidth = Dir.Size() / 2.0f;
		Dir.Normalize();
		FVector2D mNor = FVector2D(-Dir.Y, Dir.X);

		float fHeight = 1.0f;
		FVector2D Pos = mNor*fHeight + (Pos0+Pos1)*0.5f;
		AddTinyBox(Pos, FVector2D(fHeight,fWidth), mNor, eBoxWall);
	}
	return false;
}
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::AdjustPillar(TSharedPtr<FArmyWHTinyBox>& pBox)
{
	int wall0, wall1;
	if (pBox->IsOnWallCorner(wall0, wall1))
	{
		int CornerId = WallLinkCornerId(wall0, wall1);
		return AddCornerSlot(CornerId,pBox.Get());
	}
	return nullptr;
}
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::AddCornerSlot(int CornerId, FArmyWHTinyBox* pPillar)
{
	TSharedPtr<FArmyWHTinyBox>* ppBoxSlot = mFillSlotCorners.Find(CornerId);
	if (ppBoxSlot)
		return *ppBoxSlot;

	int wall0 = CornerId==0?mNumWall-1:CornerId-1;
	int wall1 = CornerId;
	FVector2D HorDir = GetWallDir(CornerId, wall0);
	FVector2D VerDir = GetWallDir(CornerId, wall1);
	FVector2D CornerPos = WallPoint[CornerId];

	TSharedPtr<FArmyWHTinyBox> pBoxSlot;
	TSharedPtr<FArmyWHTinyBox> pNearBoxHor = FindNearBox(CornerPos, wall0);
	TSharedPtr<FArmyWHTinyBox> pNearBoxVer = FindNearBox(CornerPos, wall1);
	if (!pNearBoxHor.IsValid() && !pNearBoxVer.IsValid() )
		return pBoxSlot;
	if (pNearBoxHor.IsValid() && pNearBoxVer.IsValid() && pNearBoxHor->mType==eBoxPillar && pNearBoxVer->mType==eBoxPillar )
		return pBoxSlot;

	float HorBoxLenHalf = FLT_MAX;
	float VerBoxLenHalf = FLT_MAX;
	float Hor2Corner = FLT_MAX;
	float Ver2Corner = FLT_MAX;
	if (pNearBoxHor.IsValid())
	{
		HorBoxLenHalf = pNearBoxHor->GetVerWallLen(wall0) / 2;
		Hor2Corner    = pNearBoxHor->Pos2Dist(CornerPos);
	}
	if (pNearBoxVer.IsValid())
	{
		VerBoxLenHalf = pNearBoxVer->GetVerWallLen(wall1) / 2;
		Ver2Corner    = pNearBoxVer->Pos2Dist(CornerPos);
	}
	if ( Hor2Corner <= VerBoxLenHalf * 2 + MinFlapLen && Ver2Corner<HorBoxLenHalf * 2 + MinFlapLen)
	{
		if ( pNearBoxVer != pNearBoxHor )
		{
			float HorHalf = Hor2Corner / 2;
			float VerHalf = Ver2Corner / 2;
			if (HorHalf >= VerBoxLenHalf)
				HorHalf = VerBoxLenHalf;
			if(VerHalf >= HorBoxLenHalf)
				VerHalf = HorBoxLenHalf;
			FVector2D CentryPos = CornerPos + HorDir*HorHalf + VerDir*VerHalf;
			if (!(pNearBoxHor->mType == eBoxHigh && pNearBoxVer->mType == eBoxHigh&& mCurBoxType==eBoxFloor) )
			{
				pBoxSlot = AddTinyBox(CentryPos, FVector2D(HorHalf, VerHalf), HorDir, eBoxFillSlot);
				mFillSlotCorners.Add(CornerId, pBoxSlot);
				if (pPillar)
					pPillar->mPillarInConerFill = true;
				return pBoxSlot;
			}
		}
		else
		{
			return pBoxSlot; //�սǿ�ǽ�ع�
		}
	}
	if (pPillar)
	{
		pBoxSlot = AddTinyBox(pPillar->mPos, pPillar->mSizeHalf, pPillar->mDir, eBoxWall);
		pPillar->mPillarInConerFill = true;
		mFillSlotCorners.Add(CornerId, pBoxSlot);
	}
	return pBoxSlot;
}
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::FindNearBox(FVector2D Pos,int WallId)
{
	float fMinBox0 = FLT_MAX;
	TSharedPtr<FArmyWHTinyBox> pBoxRet;
	for (auto& p : mBoxs)
	{
		TSharedPtr<FArmyWHTinyBox>& pBox = p.Value;
		if ( pBox->IsOnWall(WallId) )
		{
			float fDist = pBox->Pos2Dist(Pos);
			if (fDist < fMinBox0)
			{
				fMinBox0 = fDist;
				pBoxRet = pBox;
			}
		}
	}
	return pBoxRet;
}
bool FArmyWHCCookAssist::GenWallBoxRel(TSharedPtr<FArmyWHTinyBox> pBox)
{
	int NumCorner = WallPoint.Num();
	for(int i=0; i<NumCorner;++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			// 当Box的某条边距离某面墙的距离小于2cm时，认为Box和这面墙产生关联
			if (Point2Line(pBox->mVerPos[j], WallPoint[i], WallPoint[XRWHCMode::RoundIndex(i, 1, NumCorner)]) < 2.0f )
			{
				pBox->mWallId[j] = i;
			}
		}
	}
	return true;
}
FVector2D FArmyWHCCookAssist::GetWallDir(int StarCorner, int EndCorner)
{
	if (StarCorner == EndCorner)
		EndCorner = (EndCorner + 1) % mNumWall;
	FVector2D Dir = WallPoint[EndCorner] - WallPoint[StarCorner];
	Dir.Normalize();
	return Dir;
}
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::AddSinkCabinet(FShapeInRoom *PlacedShape)
{
	if (PlacedShape->CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PlacedShape->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid() && Accessory->Actor != nullptr)
		{
			FVector		ShapeLocation = Accessory->Actor->GetActorLocation();
			FRotator	ShapeRotation = Accessory->Actor->GetActorRotation();

			UStaticMeshComponent* pComponent = Accessory->Actor->GetStaticMeshComponent();
			FVector HalfSize3d = pComponent->GetStaticMesh()->GetBoundingBox().GetExtent();

			float fAngle = ShapeRotation.Yaw*PI / 180.0f + PI / 2;
			FVector2D HalfSize(HalfSize3d.Y * 10 - 5, HalfSize3d.X*10.0f - 5);
			FVector2D Dir(FMath::Cos(fAngle), FMath::Sin(fAngle));
			FVector2D Pos = FVector2D(ShapeLocation.X * 10, ShapeLocation.Y * 10);
			Pos = RoundVector(Pos) /*+ (Dir*HalfSize.X)*/;

			TSharedPtr<FArmyWHTinyBox> pBox = MakeShareable(new FArmyWHTinyBox());
			pBox->Init(Pos, HalfSize, Dir, eBoxSinkCabinet);
			mSinkCabinets.Add(pBox->mId, pBox);
		}
	}
	return nullptr;
}
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::AddBox(const TSharedPtr<FShapeInRoom>& PlacedShape)
{
	return AddBox(PlacedShape.Get());
}
TSharedPtr<FArmyWHTinyBox> FArmyWHCCookAssist::AddBox(FShapeInRoom *PlacedShape)
{
	ETinyBoxType BoxType = (ETinyBoxType)PlacedShape->Shape->GetShapeCategory();

	if ( (mCurBoxType == eBoxFloor && (BoxType == eBoxHang || BoxType == eBoxOnFloor)) || ((mCurBoxType == eBoxHang || mCurBoxType == eBoxOnFloor) && BoxType == eBoxFloor) )
		return nullptr;

	FVector		ShapeLocation = PlacedShape->ShapeFrame->GetActorLocation();
	FRotator	ShapeRotation = PlacedShape->ShapeFrame->GetActorRotation();
	float		ShapeWidth = PlacedShape->Shape->GetShapeWidth()/2;
	float		ShapeDepth = PlacedShape->Shape->GetShapeDepth()/2;
	float		ShapeHeight = PlacedShape->Shape->GetShapeHeight();
	if (ShapeLocation.Z + ShapeHeight / 10 < mTopCabinet*0.1f - 0.1f)
		return nullptr;
	AddSinkCabinet(PlacedShape);

	FVector2D Pos = FVector2D(ShapeLocation.X*10, ShapeLocation.Y*10);
	float fAngle = ShapeRotation.Yaw*PI / 180.0f + PI / 2;
	FVector2D Dir(FMath::Cos(fAngle), FMath::Sin(fAngle));

	float SunBorardWidthRight = PlacedShape->GetSunBoardWidth(0)*10.f*0.5f;
	if (SunBorardWidthRight > 0)
	{
		ShapeWidth += SunBorardWidthRight;
		Pos += SunBorardWidthRight*FVector2D(-Dir.Y, Dir.X);
	}

	float SunBorardWidthLeft = PlacedShape->GetSunBoardWidth(1)*10.f*0.5f;
	if (SunBorardWidthLeft>0 )
	{
		ShapeWidth += SunBorardWidthLeft;
		Pos -= SunBorardWidthLeft*FVector2D(-Dir.Y, Dir.X);
	}

	FVector2D HalfSize(ShapeDepth, ShapeWidth);
	Pos = RoundVector(Pos)+(Dir*HalfSize.X);
	TSharedPtr<FArmyWHTinyBox> pBox = AddTinyBox(Pos, HalfSize, Dir, BoxType);
	pBox->mHeight.X = ShapeLocation.Z;
	pBox->mHeight.Y = ShapeLocation.Z+ ShapeHeight/10.0f;
	pBox->mShape = PlacedShape;
	if ( !((mCurBoxType == eBoxFloor) && (BoxType == eBoxHigh || BoxType == eBoxOther || BoxType == eBoxBathroomFloor || BoxType == eBoxBathroomHang || BoxType == eBoxBathroomStorage)) && 
		(ShapeLocation.Z + ShapeHeight / 10 > mTopCabinet * 0.1f + 0.1f) )
		pBox->mType = eBoxPillar;
	return pBox;
}
bool FArmyWHCCookAssist::GenRoomPoint(FArmyWHCPoly* pRoom)
{
	TArray<FArmyWHCLineSeg>& LineSeg = pRoom->mLines;
	for (auto& it : LineSeg)
	{
		WallPoint.Emplace(RoundVector(FVector2D(it.mStart.X*10.0f, it.mStart.Y*10.0f)));
	}
	mNumWall = WallPoint.Num();
	for (auto& it : pRoom->mChildRects)
	{
		FArmyWHCRect& rect = it.Value;
		AddTinyBox(RoundVector(rect.mPos*10.0f), rect.mSizeHalf*10.0f, rect.mDir, eBoxPillar);
	}
	return true;
}

FArmyWHCCookAssist::~FArmyWHCCookAssist()
{
	mBoxs.Empty();
	mNeighbors.Empty();
	mPillars.Empty();
	WallPoint.Empty();
	mFillSlotCorners.Empty();
}

bool FArmyWHCCookAssist::PrepareGenTable(bool bGenGap, FArmyWHCPoly* pPolyRoom, const TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes, int32 iTop, bool bIgnoreGenGlobalTableFlag)
{
	if (!bGenGap)
		return true;
	mTopCabinet = iTop;

	mCurBoxType = eBoxFloor;

	GenRoomPoint(pPolyRoom);

	for (auto &PlacedShape : PlacedShapes)
	{
		if (PlacedShape->HasValidShape())
		{
			bool bValue = bIgnoreGenGlobalTableFlag ? true : PlacedShape->CanGenGlobalTableBoardAndToe();
			if (PlacedShape->RoomAttachedIndex == pPolyRoom->mGuid && bValue)
				AddBox(PlacedShape);
		}
	}
	if (mBoxs.Num() == 0)
		return true;
	GenBoxFillPillars();
	GenNeighbors();
	GenBoxLines();

	return true;
}

bool FArmyWHCCookAssist::PrepareGenTable(bool bGenGap, FArmyWHCPoly* pPolyRoom, const TArray<FShapeInRoom*> &PlacedShapes, int32 iTop, bool bIgnoreGenGlobalTableFlag)
{
	if (!bGenGap)
		return true;
	mTopCabinet = iTop;

	mCurBoxType = eBoxFloor;

	GenRoomPoint(pPolyRoom);

	for (auto &PlacedShape : PlacedShapes)
	{
		if (PlacedShape->HasValidShape())
		{
			bool bValue = bIgnoreGenGlobalTableFlag ? true : PlacedShape->CanGenGlobalTableBoardAndToe();
			if (PlacedShape->RoomAttachedIndex == pPolyRoom->mGuid && bValue)
				AddBox(PlacedShape);	
		}
	}
	if (mBoxs.Num() == 0)
		return true;
	GenBoxFillPillars();
	GenNeighbors();
	GenBoxLines();

	return true;
}

bool FArmyWHCCookAssist::PrepareGenHangGapBoard(bool bGenGap, FArmyWHCPoly* pPolyRoom, const TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes, int32 iTop)
{
	if (!bGenGap)
		return true;
	mTopCabinet = iTop;

	GenRoomPoint(pPolyRoom);

	mCurBoxType = eBoxHang;
	for (const auto &PlacedShape : PlacedShapes)
	{
		if (PlacedShape->HasValidShape())
		{
			if (PlacedShape->RoomAttachedIndex == pPolyRoom->mGuid)
				AddBox(PlacedShape);	
		}
	}
	if (mBoxs.Num() == 0)
		return true;
	GenBoxFillPillars();
	GenNeighbors();
	GenBoxLines();

	return true;
}

void FArmyWHCCookAssist::GenTableMesh(float InFrontElapse, float InFrontWidth, float InTableHeight, const FGenTable &GenTableCallback, const FGenTrim &GenTrimCallback, const FCollectTrimOutline &CollectTrimOutlineCallback)
{
	TArray<TArray<FVector2D>> HoleLines;
	GetHole(HoleLines);

	FArmyPolygonTool tool;
	for (auto& hole:HoleLines )
	{
		tool.AddPoly();
		for (auto& p:hole )
			tool.AddPolyPoint(p);
	}

	for (auto& It : mBoxs)
	{
		TSharedPtr<FArmyWHTinyBox> pBox = It.Value;	
		if (pBox->mType == eBoxPillar || 
			pBox->mType == eBoxWall ||
			pBox->mType == eBoxHigh ||
			pBox->mType == eBoxHang ||
			pBox->mType == eBoxOnFloor ||
			pBox->mType == eBoxFillSlotHigh)
			continue;
		tool.AddPoly();
		for (int i = 0; i < 4; ++i)
			tool.AddPolyPoint(pBox->mVerPos[i]);
	}

	for (auto& It : mPillars)
	{
		TSharedPtr<FArmyWHTinyBox> pBox = It.Value;
		if (pBox->mType == eBoxPillar)
		{
			tool.AddHole();
			for (int i = 0; i < 4; ++i)
				tool.AddHolePoint(pBox->mVerPos[i]);
		}
	}

	for (auto& It : mSinkCabinets)
	{
		tool.AddHole();
		TSharedPtr<FArmyWHTinyBox> pBox = It.Value;
		for (int i = 0; i < 4; ++i)
			tool.AddHolePoint(pBox->mVerPos[i]);
	}

	TArray<FVector2D> p2tVers;
	TArray<uint16>& Faces = tool.Execute(p2tVers);

	FArmyWHPath ph;
	GetLinePath(ph, 0, InFrontElapse, InFrontElapse);

	FVector2D vMinVer = tool.GetPolyMin() / 10.0f;
	int NumVer = p2tVers.Num();

	TArray<FPUVVertex> Vers;
	for (int i = 0; i < NumVer; ++i)
	{
		FVector2D posOrgin(p2tVers[i].X, p2tVers[i].Y);
		FVector2D Pos = ph.GetPos(posOrgin) / 10.0f;
		FVector2D UvPos = (Pos - vMinVer) / 51.2f;
		FPUVVertex v;
		v.Position = FVector(Pos.X, Pos.Y, mTopCabinet*0.1f + InTableHeight);
		v.Texcoord0 = UvPos;
		Vers.Add(v);
	}
	TArray<uint16> FaceBack = Faces;
	int32 NumFace = Faces.Num();
	int VerFront = Vers.Num();
	for (int i = 0; i < VerFront; ++i)
	{
		FPUVVertex v(Vers[i].Position, Vers[i].Texcoord0);
		v.Position.Z = mTopCabinet*0.1f;
		Vers.Add(v);
	}
	for (int i = 0; i < NumFace; ++i)
	{
		uint16 IdxBack = VerFront + Faces[NumFace - 1 - i];
		Faces.Add(IdxBack);
	}
	// gFArmyWHCabinMgr->RefreshTableMesh(Vers, Faces);
	GenTableCallback.ExecuteIfBound(Vers, Faces);
	//////////////////////////////////////////////////////////////////////////
	TArray< TArray<FVector2D> > OutLines;
	tool.GetOutLines(OutLines);

	TArray<FArmyWHCTableObject::FWHCTableBatchData> TableBatchDataArray;
	for (auto& p :OutLines)
	{
		// TArray<FVector2D> VerFronts;
		// TArray<FVector2D> VerBacks;
		int32 Index = TableBatchDataArray.Add(FArmyWHCTableObject::FWHCTableBatchData());
		FArmyWHCTableObject::FWHCTableBatchData& TabelBatchData = TableBatchDataArray[Index];
		bool bClosed = SliptVer(p, InFrontWidth, TabelBatchData.VerFronts, TabelBatchData.VerBacks);
		TabelBatchData.bClosed = bClosed;
		GenTableTrim(TabelBatchData.VerFronts, bClosed, ph, 1, mTopCabinet*0.1f, GenTrimCallback);
		GenTableTrim(TabelBatchData.VerBacks, false, ph, 2, mTopCabinet*0.1f + InTableHeight, GenTrimCallback);
	}
	CollectTrimOutlineCallback.ExecuteIfBound(TableBatchDataArray);
}

void FArmyWHCCookAssist::GenGapMesh(const FGenGap &GenGapCallback, const FGenBoardGap &GenBoardGapCallback)
{
	check(GenGapCallback.IsBound());

	for (const auto& It : mBoxs)
	{
		TSharedPtr<FArmyWHTinyBox> pBox = It.Value;
		if ((pBox->mType == eBoxLinkSlot || pBox->mType == eBoxFillSlotHigh) && pBox->mSizeHalf.Y > 4.0f )
		{
			TSharedPtr<FArmyWHTinyBox> pParent = pBox->GetNeighborBox();
			FVector2D Height0 = pBox->mFromNeighbor->mBox0->mHeight;
			FVector2D Height1 = pBox->mFromNeighbor->mBox1->mHeight;
			float fTop = Height1.Y < Height0.Y ? Height1.Y: Height0.Y;
			float fBottom = Height1.X > Height0.X ? Height1.X : Height0.X;

			FVector2D s = pBox->mVerPos[0]/10;
			FVector2D e = pBox->mVerPos[3]/10;
			
			if ( mCurBoxType == eBoxFloor )
			{
				UMaterialInstanceDynamic *pMtl = Cast<UMaterialInstanceDynamic>(pParent->mShape->GetDoorMtl());
				if (pMtl == nullptr)
					pMtl = Cast<UMaterialInstanceDynamic>(pParent->mShape->GetInsertionBoardMtl());
				GenGapCallback.Execute(0, s, e, fBottom, fTop, pMtl);
			}
			else
			{
				float ScaleY = GenGapCallback.Execute(1, s, e, fBottom, fTop, Cast<UMaterialInstanceDynamic>(pParent->mShape->GetDoorMtl())) * 1.8f;

				float fHeightLateral = FWHCModeGlobalData::GlobalParams.AdjustmentLateralBlockerDepth;
				if (FMath::Abs(ScaleY + 1.0f) < 0.0001f)
					fHeightLateral = 1.8f;

				s = s - pBox->mDir*ScaleY;
				e = e - pBox->mDir*ScaleY;

				FPUVVertex v0, v1, v2, v3, b0, b1, b2, b3;

				v0.Position.X = v3.Position.X = s.X;
				v0.Position.Y = v3.Position.Y = s.Y;
				v1.Position.X = v2.Position.X = e.X;
				v1.Position.Y = v2.Position.Y = e.Y;
				v0.Position.Z = v1.Position.Z = fBottom;
				v2.Position.Z = v3.Position.Z = fTop;

				v0.Texcoord0.Y = v1.Texcoord0.Y = 0.0f;
				v2.Texcoord0.Y = v3.Texcoord0.Y = 0.0f;
				v0.Texcoord0.X = v3.Texcoord0.X = 0.0f;
				v1.Texcoord0.X = v2.Texcoord0.X = pBox->mSizeHalf.Y / pParent->mSizeHalf.Y;

				FVector2D sb = pBox->mVerPos[1] / 10.0f;
				FVector2D eb = pBox->mVerPos[2] / 10.0f;

				b0.Position.X = b3.Position.X = sb.X;
				b0.Position.Y = b3.Position.Y = sb.Y;
				b1.Position.X = b2.Position.X = eb.X;
				b1.Position.Y = b2.Position.Y = eb.Y;
				b0.Position.Z = b1.Position.Z = fBottom;
				b2.Position.Z = b3.Position.Z = fTop;

				b0.Texcoord0.X = b3.Texcoord0.X = 0.0f;
				b1.Texcoord0.X = b2.Texcoord0.X = pBox->mSizeHalf.Y / pParent->mSizeHalf.Y;
				b0.Texcoord0.Y = b1.Texcoord0.Y = b2.Texcoord0.Y = b3.Texcoord0.Y = pBox->mSizeHalf.X / pParent->mSizeHalf.X;
				
				FPUVVertex vb0, vb1, vb2, vb3, bb0, bb1, bb2, bb3;
				vb0 = v0; vb1 = v1; vb2 = v2; vb3 = v3;
				bb0 = b0; bb1 = b1; bb2 = b2; bb3 = b3;
				vb0.Position.Z = vb1.Position.Z = fBottom+ fHeightLateral;
				vb2.Position.Z = vb3.Position.Z = fTop- fHeightLateral;
				bb0.Position.Z = bb1.Position.Z = fBottom+ fHeightLateral;
				bb2.Position.Z = bb3.Position.Z = fTop- fHeightLateral;

				TArray<FPUVVertex> Vers = 
				{
					v0,v1,v2,v3,b0,b1,b2,b3,
					vb0,vb1,vb2,vb3,bb0,bb1,bb2,bb3,
				};
				TArray<uint16> Faces =
				{
					3,2,6,3,6,7,
					0,5,1,0,4,5,
					3+8,6+8,2+8,3+8,7+8,6+8,
					0+8,1+8,5+8,0+8,5+8,4+8,
				};
				// gFArmyWHCabinMgr->GenGapActor(Vers, Faces, pParent->mShape->GetBottomMtl() );
				GenBoardGapCallback.ExecuteIfBound(Vers, Faces, Cast<UMaterialInstanceDynamic>(pParent->mShape->GetBottomMtl()));
			}
		}
	}
}

void FArmyWHCCookAssist::GenFloorTrim(const FGenTrim &GenTrimCallback)
{
	FArmyWHPath phFloor;
	GetLinePath(phFloor, 0, -TrimFrontElapse - 10, -TrimElapse - 10);
	for (auto& it: phFloor.mPaths)
	{
		if (it->mVers.Num() > 1) // zengy 2019-6-11 防止路径中只有一个点导致生成顶点缓冲时出错
		{
			TArray<FVector2D> Vers;
			bool bCloss = it->GetVers(Vers);
			GenTableTrim(Vers, bCloss, phFloor, 0, 0, GenTrimCallback);
		}
	}
}

void FArmyWHCCookAssist::GenHangTrim(const FGenTrim &GenTrimCallback)
{
	FArmyWHPath phFloor;
	GetLinePath(phFloor, 0, -70 , -70);
	for (auto& it : phFloor.mPaths)
	{
		TArray<FVector2D> Vers;
		bool bCloss = it->GetVers(Vers);
		if (Vers.Num() == 2 && ((Vers[0] - Vers[1]).Size() < 110))
			continue;
		GenTableTrim(Vers, bCloss, phFloor, 3, mTopCabinet*0.1f, GenTrimCallback);
	}
}

bool FArmyWHCCookAssist::GenBoxLines()
{
	for (auto& p:mBoxs )
	{
		p.Value->GetTrimLine();
	}
	return true;
}
bool FArmyWHCCookAssist::IsNearFloorBox(TArray<FVector2D>& OutLine)
{
	// 提高比较精度
	auto Vector2DEqual = [](const FVector2D &Left, const FVector2D &Right) -> bool {
		FVector2D Leftx10 = Left * 10.0f;
		FVector2D Rightx10 = Right * 10.0f;
		return FMath::RoundToInt(Leftx10.X) == FMath::RoundToInt(Rightx10.X) && 
			FMath::RoundToInt(Leftx10.Y) == FMath::RoundToInt(Rightx10.Y);
	};

	for (auto& V:OutLine)
	{
		for (auto& It : mBoxs)
		{
			TSharedPtr<FArmyWHTinyBox> pBox = It.Value;
			if (pBox->mType == eBoxFloor)
			{
				for (int i = 0; i < 4; ++i)
				{
					if (Vector2DEqual(pBox->mVerPos[i], V)) 
						return true;
				}
			}
		}
	}
	return false;
}
bool FArmyWHCCookAssist::GetHole(TArray<TArray<FVector2D>>& HoleLines)
{
	HoleLines.Empty();
	TArray<TArray<FVector2D>> OutLines;

	FArmyPolygonTool tool;

	tool.AddPoly();
	for (auto& p : WallPoint)
		tool.AddPolyPoint(p);

	for (auto& It : mBoxs)
	{
		TSharedPtr<FArmyWHTinyBox> pBox = It.Value;
		//if (pBox->mType == eBoxPillar)
		//	continue;
		if (pBox->mType == eBoxWall || 
			pBox->mType == eBoxHang ||
			pBox->mType == eBoxOnFloor ||
			pBox->mType == eBoxFillSlotHigh)
			continue;
		//if (pBox->mType == eBoxHigh)
		//	continue;
		tool.AddHole();
		for (int i = 0; i < 4; ++i)
			tool.AddHolePoint(pBox->mVerPos[i]);
	}
	tool.Execute();
	tool.GetOutLines(OutLines);

	for (auto& OutLine: OutLines )
	{
		// zengy noted: AreaPoly(OutLine) < 800 * 800 这个判定逻辑不是十分严谨，只有面积小于800*800才认为是墙角的"hole"
		if ( AreaPoly(OutLine) < 800 * 800 && IsNearFloorBox(OutLine) )
			HoleLines.Add(OutLine);
	}
	return true;
}

bool FArmyWHCCookAssist::GetLinePath(FArmyWHPath& ph, int Opp, float FrontElapse, float SideElapse)
{
	for (auto& it:mBoxs )
	{
		it.Value->GetLine(Opp, ph.mLines);
	}
	for (int i = 0; i < ph.mLines.Num(); ++i)
	{
		ph.AddVert(ph.mLines[i]);
	}
	ph.AdjustVer(FrontElapse, SideElapse);
	ph.GenGroup();
	return true;
}

void FArmyWHCCookAssist::GenTableTrim(TArray<FVector2D>& Ver2ds, bool bIsClose, FArmyWHPath& ph, INT32 TrimType, float PlaceHigh, const FGenTrim &GenTrimCallback)
{
	if (Ver2ds.Num() <= 1)
		return;
	TArray<FVector2D> vs;
	for (auto& v : Ver2ds)
	{
		FVector2D v2 = ph.GetPos(v) / 10;
		vs.Add(v2);
	}
	// return gFArmyWHCabinMgr->GenTrimActor(TrimType, vs, bIsClose, PlaceHigh);
	GenTrimCallback.ExecuteIfBound(TrimType, vs, bIsClose, PlaceHigh);
}
////////////////////////////////////////////////////////////////////////////
FString Pos2Id(FVector2D Pos)
{
	int x = int(Pos.X + (Pos.X > 0 ? .5 : -.5));
	int y = int(Pos.Y + (Pos.Y > 0 ? .5 : -.5));
	return FString::Printf(TEXT("%d_%d"), x, y);
}
bool FArmyWHPath::AddVert(FArmyWHSeg& seg)
{
	TSharedPtr<FArmyWHVert> pVer0 = GetVert(seg.mStart);
	TSharedPtr<FArmyWHVert> pVer1 = GetVert(seg.mEnd);
	if (pVer0.IsValid() )
	{
		pVer0->mSeg[1] = &seg;
		seg.mVert[0] = pVer0;
	}
	else
	{
		TSharedPtr<FArmyWHVert> pVer0 = MakeShareable(new FArmyWHVert() );
		pVer0->mId = Pos2Id(seg.mStart);
		pVer0->mSeg[0] = &seg;
		pVer0->mPos = pVer0->mOriginPos = seg.mStart;
		seg.mVert[0] = pVer0;
		mVers.Add(pVer0->mId, pVer0);
	}
	if ( pVer1.IsValid() )
	{
		pVer1->mSeg[1] = &seg;
		seg.mVert[1] = pVer1;
	}
	else
	{
		TSharedPtr<FArmyWHVert> pVer1 = MakeShareable(new FArmyWHVert());
		pVer1->mId = Pos2Id(seg.mEnd);
		pVer1->mSeg[0] = &seg;
		pVer1->mPos = pVer1->mOriginPos = seg.mEnd;
		seg.mVert[1] = pVer1;
		mVers.Add(pVer1->mId, pVer1);
	}
	return true;
}
TSharedPtr<FArmyWHVert> FArmyWHPath::GetVert(FVector2D& Pos)
{
	TSharedPtr<FArmyWHVert>* p= mVers.Find( Pos2Id(Pos) );
	if ( p )
	{
		return *p;
	}
	else
	{
		TSharedPtr<FArmyWHVert> v;
		return v;
	}
}
bool FArmyWHPath::AdjustVer(float FrontElapse, float SideElapse)
{
	for ( auto& p : mVers )
	{
		TSharedPtr<FArmyWHVert> pVer = p.Value;
		FArmyWHSeg* pLine0 = pVer->mSeg[0];
		FArmyWHSeg* pLine1 = pVer->mSeg[1];
		if ( pLine0 )
		{
			if (pLine0->mEdgeId == 3)
				pVer->mPos = pVer->mPos + FrontElapse*pLine0->mNormal;
			else
				pVer->mPos = pVer->mPos + SideElapse*pLine0->mNormal;
		}
		if (pLine1  && (pLine0->mNormal-pLine1->mNormal).SizeSquared()>Epsilon )
		{
			if (pLine1->mEdgeId == 3)
				pVer->mPos = pVer->mPos + FrontElapse*pLine1->mNormal;
			else
				pVer->mPos = pVer->mPos + SideElapse*pLine1->mNormal;
		}
	}
	return true;
}
FVector2D FArmyWHPath::GetPos(FVector2D& OrginPos)
{
	FVector2D Pos = OrginPos;
	TSharedPtr<FArmyWHVert> pVer = GetVert(OrginPos);
	if (pVer.IsValid())
		Pos = pVer->mPos;
	return Pos;
}
bool FArmyWHPath::GenGroup()
{
	for (auto& p : mVers)
		p.Value->mbCllocet = false;

	for (TSharedPtr<FArmyWHVert> p = GetNotCollect(); p.IsValid(); p = GetNotCollect())
	{
		TSharedPtr<FArmyWHLineGroup> pGroup = MakeShareable( new FArmyWHLineGroup() );
		pGroup->GenList(p);
		mPaths.Add(pGroup);
	}
	return true;
}
TSharedPtr<FArmyWHVert> FArmyWHPath::GetNotCollect()
{
	for (auto& p:mVers)
	{
		if ( p.Value->mbCllocet == false )
		{
			p.Value->mbCllocet = true;
			return p.Value;
		}
	}
	return TSharedPtr<FArmyWHVert>();
}
//
bool FArmyWHLineGroup::GenList(TSharedPtr<FArmyWHVert> p)
{
	p->mbCllocet = true;
	mVers.AddHead(p);

	TSharedPtr<FArmyWHVert> pVerLeft = p;
	for(FArmyWHSeg* pLineLeft = p->mSeg[0]; pLineLeft; pLineLeft = pVerLeft->GetLinkSeg(pLineLeft))
	{
		pVerLeft = pLineLeft->GetOppVert(pVerLeft);
		if (pVerLeft == p )
		{
			mbClose = true;
			return true;
		}
		if (!pVerLeft->mbCllocet)
		{
			pVerLeft->mbCllocet = true;
			mVers.AddHead(pVerLeft);
		}
		else
		{
			break;
		}
	}

	TSharedPtr<FArmyWHVert> pVerRight = p;
	for (FArmyWHSeg* pLineRight = p->mSeg[1]; pLineRight;pLineRight = pVerRight->GetLinkSeg(pLineRight))
	{
		pVerRight = pLineRight->GetOppVert(pVerRight);
		if (pVerRight == p)
		{
			mbClose = true;
			return true;
		}
		if (!pVerRight->mbCllocet)
		{
			pVerRight->mbCllocet = true;
			mVers.AddTail(pVerRight);
		}
		else
		{
			break;
		}
	}
	return true;
}
bool FArmyWHCCookAssist::IsOnWall(FVector2D Pos)
{
	for (int i = 0; i < mNumWall; ++i)
	{
		if (Point2Line(Pos, WallPoint[i], WallPoint[(i + 1) % mNumWall])<2.0f)
		{
			return true;
		}
	}
	for ( auto& p:mBoxs )
	{
		if ( p.Value->mType == eBoxHigh || p.Value->mType == eBoxPillar)
		{
			if (p.Value->Pos2Dist(Pos) <= 2)
				return true;
		}
	}
	for (auto& p : mPillars)
	{
		if (p.Value->Pos2Dist(Pos) <= 2)
			return true;
	}
	return false;
}
bool FArmyWHLineGroup::GetVers(TArray<FVector2D>& Vers)
{
	TArray<FVector2D> TempVers;
	for (auto& p:mVers )
	{
		FVector2D Pos(p->mPos);
		TempVers.Add(Pos);
	}
	Vers.Add(TempVers[0]);
	int preIdx = 0;
	for ( int i=1; i<TempVers.Num() -1; ++i )
	{
		float fDot;
		FVector2D Point_C;
		float fDistance= Point2LineSeg(TempVers[preIdx], TempVers[i + 1], TempVers[i], fDot, Point_C);
		if ((fDistance > 0.9f) || fDot < 0 || fDot>1)
		{
			Vers.Add(TempVers[i]);
			preIdx = i;
		}
	}
	Vers.Add(TempVers[TempVers.Num() - 1]);
	return mbClose;
}
bool FArmyWHCCookAssist::SliptVer(TArray<FVector2D>& pOutLine, float InFrontWidth, TArray<FVector2D>& VerFronts, TArray<FVector2D>& VerBacks)
{
	int32 IdxStartFront = -1;
	int32 IdxEndFront = -1;
	int32 NumOutLine = pOutLine.Num();
	for ( int32 i=0; i<NumOutLine; ++i )
	{
		FVector2D VPre = pOutLine[(i - 1 + NumOutLine) % NumOutLine];
		FVector2D V0 = pOutLine[i];
		FVector2D VNext = pOutLine[(i + 1) % NumOutLine];
		VPre = 0.99f*VPre + 0.01f*V0;
		VNext = 0.99f*VNext + 0.01f*V0;
		bool IsVPreOnWall = IsOnWall(VPre);
		bool IsV0OnWall = IsOnWall(V0);
		bool IsVNextOnWall = IsOnWall(VNext);
		if (!IsVNextOnWall && (IsV0OnWall && IsVPreOnWall))
		{
			IdxStartFront = i;
		}
		// if (!IsVPreOnWall && (IsV0OnWall&&IsVNextOnWall))
		// {
		// 	IdxEndFront = i;
		// }
	}
	for ( int32 i = NumOutLine - 1; i >= 0; --i)
	{
		FVector2D VPre = pOutLine[(i + 1) % NumOutLine];
		FVector2D V0 = pOutLine[i];
		FVector2D VNext = pOutLine[(i - 1 + NumOutLine) % NumOutLine];
		VPre = 0.99f*VPre + 0.01f*V0;
		VNext = 0.99f*VNext + 0.01f*V0;
		bool IsVPreOnWall = IsOnWall(VPre);
		bool IsV0OnWall = IsOnWall(V0);
		bool IsVNextOnWall = IsOnWall(VNext);
		if (!IsVNextOnWall && (IsV0OnWall && IsVPreOnWall))
			IdxEndFront = i;
	}
	if (IdxStartFront == -1 || IdxEndFront == -1)
	{
		for (int i = 0; i<NumOutLine; ++i)
		{
			FVector2D v = pOutLine[i];
			VerFronts.Add(v);
		}
		FArmyMath::ReverPointList(VerFronts);
		return true;
	}
	int32 Idx = IdxStartFront;
	while ( true )
	{
		FVector2D v = pOutLine[Idx];
		VerFronts.Add(v);
		++Idx;
		if (Idx == NumOutLine)
			Idx = 0;
		if (Idx == IdxEndFront)
		{
			FVector2D v = pOutLine[Idx];
			VerFronts.Add(v);
			FArmyMath::ReverPointList(VerFronts);
			break;
		}
	}
	Idx = IdxEndFront;
	while (true)
	{
		FVector2D v = pOutLine[Idx];
		VerBacks.Add(v);
		++Idx;
		if (Idx == NumOutLine)
			Idx = 0;
		if (Idx == IdxStartFront)
		{
			float fExtern = InFrontWidth * 10.0f;
			FVector2D v = pOutLine[Idx];
			VerBacks.Add(v);

			FVector2D vDir = (VerBacks[0] - VerBacks[1]);
			vDir.Normalize();
			VerBacks[0] += vDir * fExtern;

			vDir = (VerBacks[VerBacks.Num() - 1] - VerBacks[VerBacks.Num() - 2]);
			vDir.Normalize();
			VerBacks[VerBacks.Num() - 1] += vDir * fExtern;
			return false;
		}
	}
	return false;
}