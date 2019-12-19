#pragma once
#include "ArmyWHCModeData.h"
#include "ArmyWHCRect.h"
#include "Actor/VertexType.h"
#include "ArmyWHCTableObject.h"

#define MinFlapLen (FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.Y*10)
#define Epsilon 1.0E-6
#define TrimElapse 30
#define TrimFrontElapse 50
//
enum ETinyBoxType
{
	eBoxNull=0,
	eBoxOther = ECabinetType::EType_Other,
	eBoxCoverDoorWardrobe = ECabinetType::EType_CoverDoorWardrobe,
	eBoxSlidingDoorWardrobe = ECabinetType::EType_SlidingDoorWardrobe,
	eBoxTop = ECabinetType::EType_TopCab,
	eBoxHang = ECabinetType::EType_HangCab,   // 吊柜
	eBoxFloor = ECabinetType::EType_OnGroundCab,// 地柜
	eBoxHigh = ECabinetType::EType_TallCab,   // 高柜
	eBoxOnFloor = ECabinetType::EType_OnCabCab, // 台上柜
	eBoxBathroomFloor = ECabinetType::EType_BathroomOnGroundCab,
	eBoxBathroomHang = ECabinetType::EType_BathroomHangCab,
	eBoxBathroomStorage = ECabinetType::EType_BathroomStorageCab,
	eBoxFillSlot,   // 占位柜子(辅助生成)，墙角填充
	eBoxFillSlotHigh,// 高柜之间的调整板
	eBoxWall,		// 墙辅助生成的
	eBoxPillar,		// 柱子
	eBoxLinkSlot,	// 柜子间的空隙补充
	eBoxSinkCabinet,// 水槽柜
};
struct FArmyWHTinyBox;
struct FArmyWHEdgeRelVeter
{
	INT32	mId0;//-1 [0-3]���� [100+��Id]��
	INT32	mId1;
	//
	FVector2D mVerPos;
	FVector2D mCrossPos;
	float	 mDistance;
};

struct FArmyWHVert;
struct FArmyWHSeg //�߶�
{
	FVector2D		mStart;
	FVector2D		mEnd;
	FVector2D		mNormal;
	int				mEdgeId;
	TSharedPtr<FArmyWHVert>	mVert[2];
	FArmyWHSeg() { }
	FArmyWHSeg(FVector2D& vstart, FVector2D& vend)
	{
		mStart = vstart;
		mEnd = vend;
	}
	TSharedPtr<FArmyWHVert> GetOppVert(TSharedPtr<FArmyWHVert>& p)
	{
		if (mVert[0] == p)
			return mVert[1];
		if (mVert[1] == p)
			return mVert[0];
		return TSharedPtr<FArmyWHVert>();
	}
	bool AnTiclock()
	{
		return (mNormal ^ (mEnd - mStart))>0;
	}
};

struct FArmyWHVert
{
	FString		  mId; //��ԭʼ��x y���ɵ�Id
	FVector2D	  mOriginPos;
	FVector2D	  mPos;
	FArmyWHSeg* mSeg[2];
	bool		mbCllocet;
	FArmyWHVert()
	{
		mbCllocet = false;
		mSeg[0] = mSeg[1] = nullptr;
	}
	FArmyWHSeg* GetLinkSeg(FArmyWHSeg* pNow)
	{
		if (mSeg[0] == pNow)
			return mSeg[1];
		if (mSeg[1] == pNow)
			return mSeg[0];
		return nullptr;
	}
};
struct FArmyWHLineGroup
{
	FArmyWHLineGroup() { mbClose = false; }
	bool	mbClose;
	TDoubleLinkedList< TSharedPtr<FArmyWHVert> > mVers;
	bool GenList(TSharedPtr<FArmyWHVert> p);
	bool GetVers(TArray<FVector2D>& Vers);
};

struct FArmyWHPath
{
	TMap<FString, TSharedPtr<FArmyWHVert> > mVers;
	TArray<FArmyWHSeg> mLines;

	bool AddVert(FArmyWHSeg& seg);
	TSharedPtr<FArmyWHVert> GetVert(FVector2D& Pos);
	bool AdjustVer(float FrontElapse, float SideElapse);
	FVector2D GetPos(FVector2D& OrginPos);
	//
	TArray< TSharedPtr<FArmyWHLineGroup> > mPaths;
	bool GenGroup();
	TSharedPtr<FArmyWHVert> GetNotCollect();
};

enum EEdgeType
{
	eEdgeNull,//������
	eEdgeOnWall,//��ǽ
	eEdgeFull,//ȫ��������
	eEdgeLinkPart,//������
};
struct FArmyWHTinyLine
{
	EEdgeType	mType;
	FVector2D		mStart;
	FVector2D		mEnd;
	float			mDist2OwnerStart;
	FVector2D		mNormal;
	int			mBoxEdgeIdx;
	TSharedPtr<FArmyWHTinyBox>	mOwner;//���ڵ�Box
	TSharedPtr<FArmyWHTinyBox>	mBox;  //���ڵ�box
	FArmyWHTinyLine() {}
	FArmyWHTinyLine(TSharedPtr<FArmyWHTinyBox>& pBox, FVector2D Start, FVector2D End)
	{
		mBox = pBox;
		mStart = Start;
		mEnd = End;
	}
};

struct FArmyWHEdgeLine : public FArmyWHTinyLine
{
	TArray< TSharedPtr<FArmyWHTinyLine> > mChilds;
	void SortLine();
	int GetLine(int iOpp, TArray<FArmyWHSeg>& Lines);
	bool AddLine(TSharedPtr<FArmyWHTinyBox>& pBox, FVector2D Start, FVector2D End);
};

struct FArmyWHNeighbor;
struct FShapeInRoom;
struct FArmyWHTinyBox : public TSharedFromThis<FArmyWHTinyBox>
{
	ETinyBoxType mType;
	INT32   mId;
	FVector2D mPos;
	FVector2D mSizeHalf;
	FVector2D mDir;
	FVector2D mHeight;
	FArmyWHTinyBox();
	TMap<INT32, TWeakPtr<FArmyWHNeighbor> > mNeighbors;
	FShapeInRoom *mShape;
	FVector2D mVerPos[4];
	int		mWallId[4];
	FArmyWHEdgeLine	mEdge[4];
	bool mPillarInConerFill;
	//
	int	mNumAdjustBorad;//����������
	FArmyWHNeighbor* mFromNeighbor;//����this�����������
	TSharedPtr<FArmyWHTinyBox> GetNeighborBox();

	TSharedPtr<FArmyWHNeighbor> GenRelation(TSharedPtr<FArmyWHTinyBox>& pBox1);
	int32 Collision(TSharedPtr<FArmyWHTinyBox>& pBox, FArmyWHEdgeRelVeter* pDetailData,bool b); // true this��Ϊbox0 
	bool Pos2Dist(FVector2D& Pos,FArmyWHEdgeRelVeter& RelData,bool b);
	float Pos2Dist(FVector2D& Pos);
	//
	void GenVerPos();

	bool Init(FVector2D Pos, FVector2D HalfSize, FVector2D Dir, ETinyBoxType eType);
	bool IsOnWallCorner(int& WallId0,int& WallId1);
	float GetVerWallLen(int WallId);//
	bool IsOnWall(int WallId);

	bool GetTrimLine( );
	bool GetTrimLineIdx(int Idx);
	bool GetTrimLineLinkSlot();

	int GetLine(int iOpp, TArray<FArmyWHSeg>& Lines);

	// zengy added 2019-3-19
	bool IsCoincident(const FArmyWHTinyBox &OtherBox) const;
};
enum ENeighborType
{
	eNeighborNull,
	eNeighborEdge,  //������
	eNeighborGap,   //�����ڵ��з�϶
	eNeighborVertex,//��������
};
struct FArmyWHNeighbor
{
	ENeighborType mType;
	TSharedPtr<FArmyWHTinyBox>	mBox0;
	TSharedPtr<FArmyWHTinyBox>	mBox1;
	INT32		mId;
	FArmyWHEdgeRelVeter mDetail[2];
	INT32		mLinkBit;
	TSharedPtr<FArmyWHTinyBox> CreatLinkSlot();
	bool GetBox0(FVector2D& CrossPos0, int&Idx0, FVector2D& CrossPos1, int&Idx1);
	bool GetBox1(FVector2D& CrossPos0, int&Idx0, FVector2D& CrossPos1, int&Idx1);
};

class FArmyWHCCookAssist
{
public:
	// 生成台面的代理
	DECLARE_DELEGATE_TwoParams(FGenTable, 
		const TArray<FPUVVertex>&, // 顶点缓冲
		const TArray<uint16>& // 索引缓冲 
	)
	// 生成带造型调整板的代理
	DECLARE_DELEGATE_RetVal_SixParams(float, FGenGap, 
		int32, // 调整板类型，0 - 地柜调整板， 1 - 吊柜调整板
		const FVector2D&, // 起点
		const FVector2D&, // 终点
		float, // 底部高度
		float, // 顶部高度
		class UMaterialInstanceDynamic* // 使用的材质
	)
	// 生成普通调整板的代理
	DECLARE_DELEGATE_ThreeParams(FGenBoardGap,
		const TArray<FPUVVertex>&, // 顶点缓冲
		const TArray<uint16>&, // 索引缓冲
		class UMaterialInstanceDynamic* // 使用的材质
	)
	// 生成线条的代理
	DECLARE_DELEGATE_FourParams(FGenTrim,
		int32, // 线条类型
		const TArray<FVector2D>&, // 线条轮廓线
		bool, // 是否闭合
		float // 线条高度，以cm为单位
	)
	// 收集台面前沿后挡轮廓线的代理
	DECLARE_DELEGATE_OneParam(FCollectTrimOutline,
		const TArray<FArmyWHCTableObject::FWHCTableBatchData>&
	)

	~FArmyWHCCookAssist();
	bool PrepareGenTable(bool bGenGap, FArmyWHCPoly* pPoly, const TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes, int32 iTop = 0, bool bIgnoreGenGlobalTableFlag = false);
	bool PrepareGenTable(bool bGenGap, FArmyWHCPoly* pPoly, const TArray<FShapeInRoom*> &PlacedShapes, int32 iTop = 0, bool bIgnoreGenGlobalTableFlag = false);
	bool PrepareGenHangGapBoard(bool bGenGap, FArmyWHCPoly* pPoly, const TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes, int32 iTop = 0);
	bool IsOnWall(FVector2D Pos);

	void GenTableMesh(float InFrontElapse, float InFrontWidth, float InTableHeight, const FGenTable &GenTableCallback, const FGenTrim &GenTrimCallback, const FCollectTrimOutline &CollectTrimOutlineCallback);
	void GenGapMesh(const FGenGap &GenGapCallback, const FGenBoardGap &GenBoardGapCallback);
	void GenFloorTrim(const FGenTrim &GenTrimCallback);
	void GenHangTrim(const FGenTrim &GenTrimCallback);

private:
	TMap<INT32, TSharedPtr<FArmyWHNeighbor> >	  mNeighbors;
	TMap<INT32, TSharedPtr<FArmyWHTinyBox> >    mBoxs;
	TMap<INT32, TSharedPtr<FArmyWHTinyBox> >	  mPillars;
	TMap<INT32, TSharedPtr<FArmyWHTinyBox> >	  mSinkCabinets;
	TMap<INT32, TSharedPtr<FArmyWHTinyBox> > mFillSlotCorners;
	TArray<FVector2D> WallPoint;
	INT32 mNumWall;
private:
	TSharedPtr<FArmyWHTinyBox> AddBox(const TSharedPtr<FShapeInRoom>& PlacedShape);
	TSharedPtr<FArmyWHTinyBox> AddBox(FShapeInRoom *PlacedShape);
	TSharedPtr<FArmyWHTinyBox> AddTinyBox(FVector2D Pos, FVector2D HalfSize, FVector2D Dir, ETinyBoxType eType);
	TSharedPtr<FArmyWHTinyBox> AddSinkCabinet(FShapeInRoom *PlacedShape);

	bool GenNeighbors();
	bool GenBoxFillVisble();
	bool GenBoxFillPillars();

	bool GenWallBoxRel(TSharedPtr<FArmyWHTinyBox> pBox);
	TSharedPtr<FArmyWHTinyBox> FindNearBox(FVector2D Pos, int WallId);
	FVector2D GetWallDir(int StarCorner,int EndCorner);

	TSharedPtr<FArmyWHTinyBox> AdjustPillar(TSharedPtr<FArmyWHTinyBox>& pBox);
	TSharedPtr<FArmyWHTinyBox> AddCornerSlot(int CornerId, FArmyWHTinyBox* pPillar);
	//
	bool GenBoxLines();
	bool GenRoomPoint(FArmyWHCPoly* pRoom );
	//
	
	
	bool GetLinePath(FArmyWHPath& ph,int Opp, float FrontElapse, float SideElapse);
	void GenTableTrim(TArray<FVector2D>& Ver2ds, bool bIsClose, FArmyWHPath& ph, INT32 TrimType, float PlaceHigh, const FGenTrim &GenTrimCallback);
	bool SliptVer(TArray<FVector2D>& pOutLine, float InFrontWidth, TArray<FVector2D>& VerFronts, TArray<FVector2D>& VerBacks);
	bool GetHole(TArray< TArray<FVector2D> >& HoleLines);//������ǽ��ɵĴ�����С��
	//
	
	bool IsNearFloorBox(TArray<FVector2D>& OutLine);
private:
	int32 mTopCabinet;
};