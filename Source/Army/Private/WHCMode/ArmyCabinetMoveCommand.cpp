#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyWHCabinet.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyShapeTableActor.h"

#include "Game/XRViewportClient.h"

void FRegularMoveCommand::ProcessCommand(FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes)
{
    FArmyWHCRect rect, alignedRect;

	bool bChangeDir = false, bAutoWidth = false;
	int32 nCollision = 0, nAlignedCollision = 0;
	nCollision = ShapeCollision(InPolyRoom, InShapeInRoom, const_cast<FVector&>(InDesiredPosition), rect, bChangeDir, bAutoWidth);
    FShapeInRoom *AlignedActiveShape = CabOperation->GetAlignedActiveShapeByPassive(InShapeInRoom);
	XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
	TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
    if (AlignedActiveShape != nullptr)
    {
        float MyActiveZ = AlignedActiveShape->ShapeFrame->GetActorLocation().Z;
        FVector RelativeLocation = Accessory->Actor->GetRootComponent()->GetRelativeTransform().GetLocation();
        FVector AlignedShapeLocation = InDesiredPosition + FVector(rect.mDir * RelativeLocation.X, 0.0f);
        AlignedShapeLocation.Z = MyActiveZ;

        bool bAlignedChangeDir = false, bAlignedAutoWidth = false;
        nAlignedCollision = ShapeCollision(InPolyRoom, AlignedActiveShape, AlignedShapeLocation, alignedRect, bAlignedChangeDir, bAlignedAutoWidth);
    }

	// 对齐的Active柜子完全无碰撞，Passive柜子完全无碰撞或调整后无碰撞，按照Passive柜子设置位置
	if (nCollision <= 1 && nAlignedCollision == 0)
        InShapeInRoom->SetTranslate(rect);
	// 对齐的Active柜子完全无碰撞或调整后无碰撞，Passive柜子完全无碰撞，按照Active柜子设置位置
	else if (nCollision == 0 && nAlignedCollision == 1)
	{
		AlignedActiveShape->SetTranslate(alignedRect);
		FVector RelativeLocation = Accessory->Actor->GetRootComponent()->GetRelativeTransform().GetLocation();
		float Z = InShapeInRoom->ShapeFrame->GetActorLocation().Z;
		FVector Location = AlignedActiveShape->ShapeFrame->GetActorLocation() - FVector(alignedRect.mDir * RelativeLocation.X, 0.0f);
		Location.Z = Z;
		InShapeInRoom->ShapeFrame->SetActorLocation(Location);
	}

    if (bChangeDir)
    {
        FVector Pos = InShapeInRoom->ShapeFrame->GetActorLocation();
        FVector2D ScreenPos;
        GVC->WorldToPixel(Pos, ScreenPos);
        GVC->Viewport->SetMouse(ScreenPos.X, ScreenPos.Y);
    }
    if (bAutoWidth)
    {
        CabOperation->UpdateHighlight();
    }
}

int32 FRegularMoveCommand::ShapeCollision(FArmyWHCPoly *InPolyRoom, FShapeInRoom *InShape, FVector &ShapePosition, FArmyWHCRect &OutRect, bool &bOutChangeDir, bool &bOutAutoWidth)
{
	constexpr float AdhereThreshold = 20.0f;

	FArmyWHCabinetMgr *RESTRICT CabMgr = FArmyWHCabinetMgr::Get();

	bOutChangeDir = false;
	bOutAutoWidth = false;

	FArmyWHCLineSeg* pSeg = nullptr;
	FArmyWHCRect rect = InShape->GetRect(&ShapePosition); 

	FArmyWHCRect rectOther;
	// 检查摆放点是否和其它柜子有冲突
	bool bCollionShape = CabMgr->Collision(InShape, rect, rectOther, 0.001f);
	// 如果没有冲突则检测是否吸附到了某面墙
	if (!bCollionShape)
		pSeg = CabMgr->CollisionRoom(rect, InShape->RoomAttachedIndex, AdhereThreshold, 0.001f);

	if (pSeg)
		InShape->WallAttachedIndex = pSeg->mIdx;

	bool bOnCorner = false;
	// 检测是否吸附到了墙角
	if (pSeg)
	{
		// 查找前一面墙
		FArmyWHCLineSeg* pSegLeft = CabMgr->CollisionPreRoomSeg(rect, InShape->RoomAttachedIndex, InShape->WallAttachedIndex, 2.0f, 2.0f);
		if (pSegLeft)
		{
			FVector2D Pos = pSegLeft->mEnd + rect.mNor*rect.mSizeHalf.Y;
			float fXElapse = rect.mSizeHalf.X;

			float fDot = rect.mDir | -pSegLeft->mDir;
			if (fDot > 0.001f) // 锐角
			{
				float fAngleDot = -pSegLeft->mDir | rect.mNor;
				fXElapse += FMath::IsNearlyZero(fAngleDot, KINDA_SMALL_NUMBER) ? 0.0f : rect.mSizeHalf.Y * 2 * fDot / fAngleDot;
			}
			Pos += fXElapse*rect.mDir;
			rect.mPos = Pos;
			bOnCorner = true;
		}
		// 查找后一面墙
		FArmyWHCLineSeg* pSegRight = CabMgr->CollisionNextRoomSeg(rect, InShape->RoomAttachedIndex, InShape->WallAttachedIndex, 2.0f, 2.0f);
		if (pSegRight)
		{
			FVector2D Pos = pSegRight->mStart + rect.mNor*rect.mSizeHalf.Y;
			float fXElapse = rect.mSizeHalf.X;

			float fDot = rect.mDir | pSegRight->mDir;
			if (fDot > 0.001f) // 锐角
			{
				float fAngleDot = pSegRight->mDir | rect.mNor;
				fXElapse += FMath::IsNearlyZero(fAngleDot, KINDA_SMALL_NUMBER) ? 0.0f : rect.mSizeHalf.Y * 2 * fDot / fAngleDot;
			}
			Pos += -fXElapse*rect.mDir;
			rect.mPos = Pos;
			bOnCorner = true;
		}
	}

	// 吸附墙
	bool bChangeDir = false;
	if (pSeg && !bOnCorner )
	{
		float fDis, fDot;
		FVector2D PosCross = pSeg->NearPos(FVector2D(ShapePosition.X, ShapePosition.Y), fDis, fDot, 2.0f);
		rect.ChangeDir(pSeg->mDir);
		rect.mPos = PosCross + pSeg->mNor*rect.mSizeHalf.Y;
		if ( InShape->WallAttachedIndex != pSeg->mIdx )
		{
			InShape->WallAttachedIndex = pSeg->mIdx;
			bChangeDir = true;
		}
	}
	bOutChangeDir = bChangeDir;

	// 吸附柜子
	bool bAutoWidth = false;
	bCollionShape = CabMgr->Collision(InShape, rect, rectOther, 0.001f);
	if (bCollionShape)
	{
		if (InShape->WallAttachedIndex == -1)
		{
			FVector2D DirOld = rect.mDir;
			float fDot = (rect.mPos - rectOther.mPos) | rectOther.mDir;
			int32 RightOrLeft = fDot > 0 ? 1 : -1;
			rect.ChangeDir(rectOther.mDir);
			if (RightOrLeft == 1)
			{
				float fSpace = rectOther.mShapeInRoom->GetRightDist(InShape) * 10.0f;
				float fNewWidth = InShape->GetMatchWidth(fSpace);
				if (fNewWidth > 0)
				{
					if (InShape->SetNewWidth(fNewWidth))
					{
						rect.mSizeHalf.X = fNewWidth / 20.0f;
						bAutoWidth = true;
					}
				}
			}
			else
			{
				float fSpace = rectOther.mShapeInRoom->GetLeftDist(InShape) * 10.0f;
				float fNewWidth = InShape->GetMatchWidth(fSpace);
				if (fNewWidth > 0)
				{
					if (InShape->SetNewWidth(fNewWidth))
					{
						rect.mSizeHalf.X = fNewWidth / 20.0f;
						bAutoWidth = true;
					}
				}
			}
			rect.mPos = rectOther.mPos + rectOther.mDir*(rectOther.mSizeHalf.X + rect.mSizeHalf.X)*RightOrLeft + rectOther.mNor*rectOther.mSizeHalf.Y - rect.mNor*rect.mSizeHalf.Y;
			if (!DirOld.Equals(rectOther.mDir, 0.001f) && !gCabinMgr.Collision(InShape, &rect, -0.001f))
			{
				FVector Pos = rect.GetPos(FVector(0, -1, -1)) + rect.GetDir()*10.0f*-RightOrLeft;
				FVector2D ScreenPos;
				GVC->WorldToPixel(Pos, ScreenPos);
				GVC->Viewport->SetMouse(ScreenPos.X, ScreenPos.Y);
			}
		}
		else
		{
			FVector2D Pos = rect.NearRect(rectOther);
			rect.mPos = Pos;
		}
	}
	bOutAutoWidth = bAutoWidth;

	// 吸附柱子
	bool bCollionPillar = CabMgr->CollisionPillar(rect, &rectOther, 0.001f, InPolyRoom);
	if (bCollionPillar && !bCollionShape)
	{
		float fDot = (rect.mPos - rectOther.mPos) | rect.mDir;
		int RightOrLeft = fDot > 0 ? 1 : -1;
		FVector2D Pos;
		if (FMath::Abs(rectOther.mDir | rect.mDir) > 0.99f)
			Pos = rectOther.mPos + rect.mDir*(rectOther.mSizeHalf.X + rect.mSizeHalf.X)*RightOrLeft - rect.mNor*rectOther.mSizeHalf.Y + rect.mNor*rect.mSizeHalf.Y;
		else
			Pos = rectOther.mPos + rect.mDir*(rectOther.mSizeHalf.Y + rect.mSizeHalf.X)*RightOrLeft - rect.mNor*rectOther.mSizeHalf.X + rect.mNor*rect.mSizeHalf.Y;
		if ( FVector2D::Distance(Pos, rect.mPos)<20 )
			rect.mPos = Pos;
	}

	OutRect = rect;

	// 在调整柜子位置后再次检测是否和其它柜子冲突(如果有多个柜子相连，在调整一次后仍旧有可能和其它柜子冲突)
	bool bModifiedCollionShape = CabMgr->Collision(InShape, rect, rectOther, -0.001f);
	// 调整位置后检测是否和房间冲突
	pSeg = CabMgr->CollisionRoom(rect, InShape->RoomAttachedIndex, -0.001f, -0.001f);
	// 调整位置后检测是否在房间内
	bool bOutsideRoom = true;
	const TSharedPtr<FArmyWHCPoly>* pPloy = CabMgr->GetRooms().Find(InShape->RoomAttachedIndex);
	if (pPloy)
		bOutsideRoom = !(*pPloy)->IsIn(rect.mPos);

	if (pSeg || bOutsideRoom)
		return 3;
	else if (bModifiedCollionShape)
		return 2;
	else if (!bModifiedCollionShape && bCollionShape)
		return 1;
	else 
		return 0;
}

void FVentilatorCabOverlappedCommand::ProcessCommand(FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes)
{
	CabOperation->CheckOverlapped(InShapeInRoom, InAllShapes, &InDesiredPosition);
}

void FVentilatorCabAlignedCommand::ProcessCommand(FArmyWHCPoly *InPolyRoom, const FVector &InDesiredPosition, FShapeInRoom *InShapeInRoom, const TArray<TSharedPtr<FShapeInRoom>> &InAllShapes)
{
	FShapeInRoom *AlignedActiveShape = CabOperation->GetAlignedActiveShapeByPassive(InShapeInRoom);
	if (AlignedActiveShape != nullptr)
		return;

	if (FShapeInRoom *PassiveShape = CabOperation->CheckAligned(InShapeInRoom, InAllShapes, 5.0f, &InDesiredPosition))
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = InShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();

		FArmyWHCRect OtherRect = PassiveShape->GetRect();
		FVector RelativeLocation = Accessory->Actor->GetRootComponent()->GetRelativeTransform().GetLocation();
		OtherRect.mPos = OtherRect.mPos + OtherRect.mDir * RelativeLocation.X;
		OtherRect.mPosOrgin = OtherRect.mPosOrgin + OtherRect.mDir * RelativeLocation.X;

		FVector AlignedPos = PassiveShape->ShapeFrame->GetActorLocation();
		AlignedPos += FVector(OtherRect.mDir * RelativeLocation.X, 0.0f);
		AlignedPos.Z = InShapeInRoom->ShapeFrame->GetActorLocation().Z;
		InShapeInRoom->ShapeFrame->SetActorLocation(AlignedPos);
		InShapeInRoom->ShapeFrame->SetActorRotation(PassiveShape->ShapeFrame->GetActorRotation());		
	}
}