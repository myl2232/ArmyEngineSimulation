#pragma once
#include "ArmyRoomOperation.h"
#include "Style/XRStyle.h"
#include "Math/XRMath.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmyToolsModule.h"
#include "ArmySceneData.h"
#include "ArmyDesignEditor.h"
#include "ArmyRulerLine.h"
#include "ArmyGameInstance.h"
#include "ArmyWallLine.h"

FArmyRoomOperation::FArmyRoomOperation(EModelType InBelongModel)
    : FArmyOperation(InBelongModel)
{
	RulerLine = MakeShareable(new FArmyRulerLine());
	RulerLine->SetInputBoxPosUseLRLine(true);
	RulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyRoomOperation::OnLineInputBoxCommitted));
}
void FArmyRoomOperation::InitWidget(TSharedPtr<SOverlay> InParentWidget)
{
	InParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SAssignNew(InputBoxWidget, SArmyInputBox)
			.OnInputBoxCommitted(FOnTextCommitted::CreateRaw(this, &FArmyRoomOperation::OnInputBoxCommitted))
		];
}
void FArmyRoomOperation::SetSelectedPrimitive(TSharedPtr<FArmyPrimitive> InPrimitive, TSharedPtr<FArmyEditPoint> InOperationPoint)
{
	/*CurrentEditLine = StaticCastSharedPtr<FArmyLine>(InPrimitive);
	CurrentOperationPoint = InOperationPoint;
	CurrentOperationPointIndex = CurrentEditLine.Pin()->GetEditPointIndex(InOperationPoint);*/
}

void FArmyRoomOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (SolidLine.IsValid() && !SolidLine->GetStart().Equals(SolidLine->GetEnd()))
	{
		SolidLine->Draw(PDI, View);
		UpLeftDashLine->Draw(PDI, View);
		UpRightDashLine->Draw(PDI, View);
		UpDashLine->Draw(PDI, View);
	}
	if (CurrentOperationLine.IsValid() && CurrentOperationPoint.IsValid()) {
		RulerLine->Draw(PDI, View);
		CurrentEditRoom.Pin()->GetRulerLine()->SetInputBoxFocus(true);
	}
}

void FArmyRoomOperation::Update(UArmyEditorViewportClient* InViewPortClient)
{
	FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);

	if (SolidLine.IsValid() && (CurrentArg._ArgInt32 == 1 || CurrentArg._ArgInt32 == 3))
	{
		if (SolidLine->GetEnd() != CapturePoint)
		{
			SolidLine->SetEnd(CapturePoint);
		}

		if (CurrentArg._ArgInt32 == 3 && CurrentOperationPoint.IsValid())
		{
			InputBoxWidget->SetFocus(true);

			CurrentOperationPoint->SetPos(CapturePoint);
		}
	}
	else if (CurrentArg._ArgInt32 == 2 && bShouldDragPrimitive)
	{
		if (!CurrentOperationLine.IsValid() && FArmyToolsModule::Get().GetRectSelectTool()->GetCurrentOperationInfo().CurrentOperationLine.IsValid())
		{
			CurrentOperationLine = FArmyToolsModule::Get().GetRectSelectTool()->GetCurrentOperationInfo().CurrentOperationLine;
			RulerLine->SetInputBoxFocus(true);
		}
		/**@欧石楠 选中了线并选中了线上的操作点才能拖拽线段*/
		if (CurrentOperationLine.IsValid() && CurrentOperationPoint.IsValid())
		{
			FVector TempProjectionPoint = FArmyMath::GetProjectionPoint(CapturePoint, CurrentOperationLine->GetStart(), CurrentOperationLine->GetEnd());
			float TempDistance = FVector::Distance(CapturePoint, TempProjectionPoint);
			FVector TempDirection = (CapturePoint - TempProjectionPoint).GetSafeNormal();
			RulerLine->Update(CurrentOperationLine->GetStart(), CurrentOperationLine->GetEnd(), TempDistance * TempDirection);
			RulerLine->SetInputBoxFocus(true);

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}
	}

	if (!SolidLine.IsValid())
		return;

	FVector LineDirection = FArmyMath::GetLineDirection(SolidLine->GetStart(), SolidLine->GetEnd());
	FVector DashLineDirection = LineDirection.RotateAngleAxis(-90, FVector(0, 0, 1));

	// 更新虚线框的显示
	UpLeftDashLine->SetStart(SolidLine->GetStart());
	UpLeftDashLine->SetEnd(UpLeftDashLine->GetStart() + DashLineDirection * 26 * InViewPortClient->GetUniformScale(SolidLine->GetStart()));
	UpRightDashLine->SetStart(SolidLine->GetEnd());
	UpRightDashLine->SetEnd(UpRightDashLine->GetStart() + DashLineDirection * 26 * InViewPortClient->GetUniformScale(SolidLine->GetEnd()));
	UpDashLine->SetStart(UpLeftDashLine->GetEnd());
	UpDashLine->SetEnd(UpRightDashLine->GetEnd());


	if (!SolidLine->GetStart().Equals(SolidLine->GetEnd(), 0.01f))
	{
		InputBoxWidget->SetStartAndEnd(SolidLine->GetStart(), SolidLine->GetEnd());
		InputBoxWidget->Show(true);

		// 设置输入框位置
		FVector2D TempViewportSize;
		InViewPortClient->GetViewportSize(TempViewportSize);
		FVector LineCenter = (UpDashLine->GetStart() + UpDashLine->GetEnd()) / 2;
		FVector2D InputBoxPos;
		InViewPortClient->WorldToPixel(LineCenter, InputBoxPos);
		InputBoxPos -= TempViewportSize / 2;
		InputBoxWidget->SetPos(InputBoxPos);
		InputBoxWidget->SetFocus(true);

		bShouldClick = true;
	}
	else
		InputBoxWidget->SetFocus(false);
}

void FArmyRoomOperation::InitControlLines()
{
	SolidLine = MakeShareable(new FArmyLine);
    SolidLine->SetBaseColor(FArmySceneData::Get()->GetWallLinesColor());

	UpLeftDashLine = MakeShareable(new FArmyLine);
	UpRightDashLine = MakeShareable(new FArmyLine);
	UpDashLine = MakeShareable(new FArmyLine);

	ExtendUpLeftDashLine = MakeShareable(new FArmyLine);
	ExtendUpRightDashLine = MakeShareable(new FArmyLine);
	ExtendUpDashLine = MakeShareable(new FArmyLine);

	UpLeftDashLine->bIsDashLine = true;
	UpLeftDashLine->SetBaseColor(FLinearColor::Gray);
	UpRightDashLine->bIsDashLine = true;
	UpRightDashLine->SetBaseColor(FLinearColor::Gray);
	UpDashLine->bIsDashLine = true;
	UpDashLine->SetBaseColor(FLinearColor::Gray);
	ExtendUpDashLine->SetBaseColor(FLinearColor::Gray);
	ExtendUpDashLine->bIsDashLine = true;
	ExtendUpLeftDashLine->SetBaseColor(FLinearColor::Gray);
	ExtendUpLeftDashLine->bIsDashLine = true;
	ExtendUpRightDashLine->SetBaseColor(FLinearColor::Gray);
	ExtendUpRightDashLine->bIsDashLine = true;
}
void FArmyRoomOperation::AddLinePoint(const FVector& InPoint, TSharedPtr<FArmyEditPoint> CapturePoint)
{
	TArray<TWeakPtr<FArmyObject>> CurrentRoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, CurrentRoomList);
	for (auto It : CurrentRoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			TArray<TSharedPtr<FArmyLine>> RoomLines;
			Room->GetLines(RoomLines);

			for (auto ItLine : RoomLines)
			{
				FVector CurrentIntersect;
				if (FArmyMath::IsPointOnLineExcludeEndPoint(InPoint, ItLine->GetStart(), ItLine->GetEnd()))
				{
					GGI->Window->ShowMessage(MT_Warning, TEXT("不允许交叉绘制"));
					return;
				}
			}
		}
	}


	//是不是接着上个房间画
	bool IsAContinuousPoint = SolidLine.IsValid();

	if (!SolidLine.IsValid())
	{
		InitControlLines();
		SolidLine->SetStart(InPoint);
		SolidLine->SetEnd(InPoint);

		if (CapturePoint.IsValid())
			PrePoint = CapturePoint;
		else
			PrePoint = MakeShareable(new FArmyEditPoint(InPoint));

		FArmyToolsModule::Get().GetMouseCaptureTool()->AddAuxiliaryPoint(PrePoint);

		if (CapturePoint.IsValid() && !CurrentEditRoom.IsValid())//接着一个未闭合的区域绘制的
		{
			CurrentEditRoom = FArmySceneData::Get()->GetRoomFromPoint(CapturePoint);
		}
		//InputBoxWidget->Show(true);
	}
	else
	{
		for (auto It : CurrentRoomList)
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (Room.IsValid() && PrePoint.IsValid())
			{
				TArray<TSharedPtr<FArmyLine>> RoomLines;
				Room->GetLines(RoomLines);

				for (auto ItLine : RoomLines)
				{
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						PrePoint->GetPos(), InPoint, CurrentIntersect))
					{
						GGI->Window->ShowMessage(MT_Warning, TEXT("不允许交叉绘制"));
						return;
					}
				}
			}
		}

		if (CapturePoint.IsValid())
			CurrentPoint = CapturePoint;
		else
		{
			//判断新添加的点是否可以和空间的点合并
			bool bFind = false;
			if (CurrentEditRoom.IsValid())
			{
				for (auto ItPoint : CurrentEditRoom.Pin()->GetPoints())
				{
					if (InPoint.Equals(ItPoint->GetPos(), 0.1f)
						&& ItPoint->GetReferenceNum() < 2)
					{
						CurrentPoint = ItPoint;
						bFind = true;
						break;
					}
				}
			}
			
			if (!bFind)
				CurrentPoint = MakeShareable(new FArmyEditPoint(InPoint));
		}

		//检测线段的终点捕捉到的点是否和线段起点属于同一个Room
		if (CapturePoint.IsValid() &&
			(FArmySceneData::Get()->GetRoomFromPoint(PrePoint) != FArmySceneData::Get()->GetRoomFromPoint(CurrentPoint)))
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("不能连接两个未闭合区域！"));
			return;
		}
		if (!PrePoint.IsValid())
		{
			PrePoint = MakeShareable(new FArmyEditPoint(InPoint));
		}		
		if (PrePoint->GetPos().Equals(CurrentPoint->GetPos()))
			return;

		TSharedPtr<FArmyWallLine> Line = MakeShareable(new FArmyWallLine);
		Line->GetCoreLine()->SetStartPointer(PrePoint);
		Line->GetCoreLine()->SetEndPointer(CurrentPoint);
		//添加线对点的引用计数
		PrePoint->AddReferenceLine(Line->GetCoreLine());
		CurrentPoint->AddReferenceLine(Line->GetCoreLine());

		//为了区分创建Room和修改Room
		if (!CurrentEditRoom.IsValid())//第一条线
		{
			TSharedPtr<FArmyRoom> Room = MakeShareable(new FArmyRoom());
			Room->AddLine(Line);

			SCOPE_TRANSACTION(TEXT("添加空间"));
			FArmySceneData::Get()->Add(Room, XRArgument(1).ArgUint32(E_HomeModel));

			CurrentEditRoom = Room;
		}
		else
		{
			SCOPE_TRANSACTION(TEXT("添加线"));
			CurrentEditRoom.Pin()->AddLine(Line);			
			CurrentEditRoom.Pin()->Modify();
		}

		Line->SetRelateRoom(CurrentEditRoom.Pin());
		
		FArmyToolsModule::Get().GetMouseCaptureTool()->AddAuxiliaryPoint(CurrentPoint);

		PrePoint = CurrentPoint;
		
		SolidLine->SetEnd(InPoint);
		SolidLine->SetStart(SolidLine->GetEnd());

        // @欧石楠 每画一笔都需要合并平行且相连的线段
        CurrentEditRoom.Pin()->CombineCollinearLines();

		//空间闭合后要对点进行排序
		if (CurrentEditRoom.Pin()->IsClosed())
		{
			//@ 删除空间线再连接后需更新空间位置
            CurrentEditRoom.Pin()->UpdateSpaceNamePos();

			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			InputBoxWidget->Show(false);

			CurrentEditRoom = nullptr;
			SolidLine = nullptr;
			CurrentOperationPoint = nullptr;
			CurrentOperationLine = nullptr;

			if (CurrentRoomList.Num() == 1)
				GGI->Window->ShowMessage(MT_Normal, TEXT("可通过【偏移】命令绘制另一个房间"));
			else if (CurrentRoomList.Num() == 2)
				GGI->Window->ShowMessage(MT_Normal, TEXT("可通过【生成外墙】命令一键生成外墙"));
			
			return;
		}
	}
	FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(InPoint), IsAContinuousPoint);
}
bool FArmyRoomOperation::InputKey(UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (InViewPortClient->IsOrtho())
	{
		if (Event == IE_Pressed)
		{
			if (Key == EKeys::LeftMouseButton)
			{
				/**@欧石楠 拖拽线体生效*/
				if (CurrentArg._ArgInt32 == 2 && CurrentOperationPoint.IsValid() && CurrentOperationLine.IsValid())
				{
                    if (CanMoveWallLine(RulerLine->GetUpDashLineStart(), RulerLine->GetUpDashLineEnd()))
                    {
                        SCOPE_TRANSACTION(TEXT("移动线"));
                        CurrentOperationLine->SetStart(RulerLine->GetUpDashLineStart());
                        CurrentOperationLine->SetEnd(RulerLine->GetUpDashLineEnd());
                        CurrentEditRoom.Pin()->UpdateSpaceNamePos();
                        CurrentEditRoom.Pin()->Modify();

                        // 设置完房间位置后，重新更新一下相关门窗的捕捉
                        FArmySceneData::Get()->ForceUpdateHardwaresCapturing(CurrentEditRoom.Pin());
                        EndOperation();
                        return false;
                    }
				}

				if (CurrentArg._ArgInt32 == 2 && CurrentOperationPoint.IsValid())
				{
					EndOperation();
					return false;
				}

                FVector ThePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
				TSharedPtr<FArmyEditPoint> CapturePoint = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePointPtr();
                if (CapturePoint.IsValid() && CapturePoint->GetReferenceNum() == 2)
                {
                    return false;
                }
				
                if (CurrentArg._ArgInt32 == 1)
                {
					AddLinePoint(ThePoint, CapturePoint);
                }
				else if (CurrentArg._ArgInt32 == 3)
				{
                    MoveEditPoint(ThePoint, false);
					return false;
				}
			}
			else if (Key == EKeys::RightMouseButton || Key == EKeys::Escape)
			{
				EndOperation();
				return false;
			}
			bShouldClick = false;
		}
	}

	return false;
}
void FArmyRoomOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	static int32 OldX = X, OldY = Y;
	if (OldX == X && OldY == Y)
	{
		return;
	}
	else
	{
		OldX = X;
		OldY = Y;
	}

	if (CurrentArg._ArgInt32 == 2)
		bShouldDragPrimitive = true;
	
	Update(InViewPortClient);
}
void FArmyRoomOperation::BeginOperation(XRArgument InArg)
{
	if (InArg._ArgInt32 == 1)//表示绘制户型操作
	{
		TArray<TWeakPtr<FArmyObject>> RoomList;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
		if (RoomList.Num() == 1)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("请点击【删除外墙】命令继续绘制"));
			EndOperation();
			return;
		}
	}

	CurrentArg = InArg;
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
}
void FArmyRoomOperation::EndOperation()
{
	RulerLine->ShowInputBox(false);
	InputBoxWidget->Show(false);
	SolidLine = NULL;
	CurrentOperationPoint = NULL;
	CurrentOperationLine = nullptr;

	CurrentEditRoom = NULL;
	bShouldClick = false;
	bShouldDragPrimitive = false;

	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel &= ~FArmyMouseCapture::Model_Ortho;
	FArmyToolsModule::Get().GetMouseCaptureTool()->ClearAuxiliaryPoints();
	FArmyOperation::EndOperation();
}

void FArmyRoomOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPrimitive)
{
	CurrentEditRoom = StaticCastSharedPtr<FArmyRoom>(Object);
	
	FArmyEditPoint* TempCurrentPoint = dynamic_cast<FArmyEditPoint*>(InOperationPrimitive.Get());
	CurrentOperationPoint = StaticCastSharedPtr<FArmyEditPoint>(InOperationPrimitive);
	if (TempCurrentPoint == NULL)
	{
		CurrentOperationLine = StaticCastSharedPtr<FArmyLine>(InOperationPrimitive);
		CurrentOperationLine->SetState(FArmyPrimitive::OPS_Selected);
		DragPrePoint = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
	}
	else
	{
		if (!CurrentOperationPoint.IsValid())
		{
			EndOperation();
			return;
		}
			
		TSharedPtr<FArmyLine> CurrentSelectedLine;
		TArray<FArmyRectSelect::FPrimitiveInfo> Lines = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedPrimitives();
		if (Lines.Num() > 0)
		{
			CurrentSelectedLine = StaticCastSharedPtr<FArmyLine>(Lines[Lines.Num() - 1].Primitive);//TODO:临时处理，取最后一个
			if (CurrentSelectedLine.IsValid())
			{
                // @欧石楠 如果房间闭合，并且已经生成外墙的情况下，不允许选中墙线两端的端点进行操作
                if (CurrentOperationPoint.IsValid() && CurrentOperationPoint != CurrentSelectedLine->GetLineEditPoint() &&
                    CurrentEditRoom.Pin()->IsClosed() && FArmySceneData::Get()->bHasOutWall)
                {
                    EndOperation();
                    return;
                }

                if (CurrentOperationPoint == CurrentSelectedLine->GetLineEditPoint())
                {
					CurrentArg._ArgInt32 = 2;
                }
				else
				{
					CurrentArg._ArgInt32 = 3;

					FVector SolidStart, SolidEnd;
					if (CurrentOperationPoint == CurrentSelectedLine->GetStartPointer())
					{
						SolidStart = CurrentSelectedLine->GetEnd();
						SolidEnd = CurrentSelectedLine->GetStart();
					}
					else if (CurrentOperationPoint == CurrentSelectedLine->GetEndPointer())
					{
						SolidStart = CurrentSelectedLine->GetStart();
						SolidEnd = CurrentSelectedLine->GetEnd();
					}
					else//Hover到了未选中线的点，后期要修改此逻辑
					{
						EndOperation();
						return;
					}

					InitControlLines();
					SolidLine->SetStart(SolidStart);
					SolidLine->SetEnd(SolidEnd);
					InputBoxWidget->Show(true);
					InputBoxWidget->SetFocus(true);
					FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
				}	
			}
		}
	}
	/*@欧石楠 辅助对齐**/
	TArray<TSharedPtr<FArmyLine>> RoomLines;
	Object->GetLines(RoomLines);

	TSharedPtr<FArmyEditPoint> PreP = NULL, PosP = NULL;
	for (auto L : RoomLines)
	{
		if (L->GetStartPointer() == CurrentOperationPoint)
		{
			if (PreP.IsValid())
			{
				PosP = L->GetEndPointer();
				break;
			}
			else
			{
				PreP = L->GetEndPointer();
			}
		}
		else if (L->GetEndPointer() == CurrentOperationPoint)
		{
			if (PreP.IsValid())
			{
				PosP = L->GetStartPointer();
				break;
			}
			else
			{
				PreP = L->GetStartPointer();
			}
		}
	}

	if (PreP.IsValid() && PosP.IsValid())
	{
		FVector2D P1(PreP->GetPos().X, PosP->GetPos().Y);
		FVector2D P2(PosP->GetPos().X, PreP->GetPos().Y);

		if ((P1 - FVector2D(CurrentOperationPoint->GetPos())).Size() < (P2 - FVector2D(CurrentOperationPoint->GetPos())).Size())
		{
			FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(P1);
		}
		else
		{
			FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(P2);
		}
	}
	else if (PreP.IsValid())
	{
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(PreP->GetPos()));
	}
	else if (PosP.IsValid())
	{
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(PosP->GetPos()));
	}
}

FObjectPtr FArmyRoomOperation::GetOperationObject()
{
	//绘制状态不返回
	return CurrentEditRoom.Pin();
// 	if (CurrentArg._ArgInt32 == 2)
// 	{
// 		return CurrentEditRoom.Pin();
// 	}
// 	return NULL;
}
TSharedPtr<FArmyEditPoint> FArmyRoomOperation::GetOperationPoint()
{
	return CurrentOperationPoint;
}

TSharedPtr<FArmyEditPoint> FArmyRoomOperation::GetPrePoint() const
{
	return PrePoint;
}

void FArmyRoomOperation::OnInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		/** 添加一条固定长度的直线 */
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		if (LineLength == 0)
			return;

		SCOPE_TRANSACTION(TEXT("添加线"));

		FVector LineDirection = FArmyMath::GetLineDirection(SolidLine->GetStart(), SolidLine->GetEnd());

		FVector Offset = (LineLength / 10.f) * LineDirection;
		FVector LineEndPos = SolidLine->GetStart() + Offset;

		if (CurrentArg._ArgInt32 == 1)
			AddLinePoint(LineEndPos);

		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(LineEndPos));
		FVector2D InputBoxPos;
		FVector2D TempViewportSize;
		GVC->GetViewportSize(TempViewportSize);
		GVC->WorldToPixel(LineEndPos, InputBoxPos);
		InputBoxPos -= TempViewportSize / 2;
		InputBoxWidget->SetPos(InputBoxPos);

        // @欧石楠 通过输入框移动端点时，如果端点和其它端点重合，也需要合并
		if (CurrentArg._ArgInt32 == 3)
		{
			if (CurrentOperationPoint.IsValid())
			{
                MoveEditPoint(LineEndPos, true);
			}
		}
	}
}

void FArmyRoomOperation::OnLineInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		if (CurrentArg._ArgInt32 == 2 && CurrentOperationPoint.IsValid() && CurrentOperationLine.IsValid())
		{
            FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
            FVector TempProjectionPoint = FArmyMath::GetProjectionPoint(CapturePoint, CurrentOperationLine->GetStart(), CurrentOperationLine->GetEnd());
            FVector TempDirection = (CapturePoint - TempProjectionPoint).GetSafeNormal();
            FVector NewStart = CurrentOperationLine->GetStart() + TempDirection * LineLength / 10.f;
            FVector NewEnd = CurrentOperationLine->GetEnd() + TempDirection * LineLength / 10.f;

            if (!CanMoveWallLine(NewStart, NewEnd))
            {
                return;
            }

			SCOPE_TRANSACTION(TEXT("移动线"));
			CurrentOperationLine->SetStart(NewStart);
			CurrentOperationLine->SetEnd(NewEnd);
			CurrentEditRoom.Pin()->UpdateSpaceNamePos();
			CurrentEditRoom.Pin()->Modify();

            // 设置完房间位置后，重新更新一下相关门窗的捕捉
            FArmySceneData::Get()->ForceUpdateHardwaresCapturing(CurrentEditRoom.Pin());
			EndOperation();			
		}
	}
}

bool FArmyRoomOperation::CanMoveWallLine(const FVector& InNewStart, const FVector& InNewEnd)
{
    if (CurrentEditRoom.Pin()->GetType() == OT_InternalRoom)
    {
        return CanMoveInnerWallLine(InNewStart, InNewEnd);
    }
    else if (CurrentEditRoom.Pin()->GetType() == OT_OutRoom)
    {
        return CanMoveOuterWallLine(InNewStart, InNewEnd);
    }

    return false;
}

bool FArmyRoomOperation::CanMoveInnerWallLine(const FVector& InNewStart, const FVector& InNewEnd)
{
    // @欧石楠 记录移动前的线段端点位置
    FVector OldStart = CurrentOperationLine->GetStart();
    FVector OldEnd = CurrentOperationLine->GetEnd();

    // @欧石楠 移动线段到新的位置
    CurrentOperationLine->SetStart(InNewStart);
    CurrentOperationLine->SetEnd(InNewEnd);

    // @欧石楠 判断移动后的房间是否和其它房间重叠
    TArray< TWeakPtr<FArmyObject> > Rooms;
    FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, Rooms);
    for (auto It : Rooms)
    {
        TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
        if (Room.IsValid() && Room->IsClosed() && Room != CurrentEditRoom)
        {
            if (FArmyMath::ArePolysOverlap(CurrentEditRoom.Pin()->GetWorldPoints(true), Room->GetWorldPoints(true)))
            {
                // @欧石楠 将线段移回原位
                CurrentOperationLine->SetStart(OldStart);
                CurrentOperationLine->SetEnd(OldEnd);

                GGI->Window->ShowMessage(MT_Warning, TEXT("此操作使空间重叠，移动失败"));
                return false;
            }
        }
    }

    // @欧石楠 判断移动后的房间是否在外墙以内
    TArray<FObjectWeakPtr> OutRooms;
    FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, OutRooms);
    if (OutRooms.Num() == 1)
    {
        TSharedPtr<FArmyRoom> OutRoom = StaticCastSharedPtr<FArmyRoom>(OutRooms[0].Pin());
        if (OutRoom.IsValid())
        {
            TArray<FVector> OutRoomVertexes;
            OutRoom->GetVertexes(OutRoomVertexes);

            if (!FArmyMath::IsPointInPolygon2D(InNewStart, OutRoomVertexes) ||
                !FArmyMath::IsPointInPolygon2D(InNewEnd, OutRoomVertexes))
            {
                // @欧石楠 将线段移回原位
                CurrentOperationLine->SetStart(OldStart);
                CurrentOperationLine->SetEnd(OldEnd);

                GGI->Window->ShowMessage(MT_Warning, TEXT("内墙位置不能超出外墙范围"));
                return false;
            }
        }
    }

    // @欧石楠 将线段移回原位
    CurrentOperationLine->SetStart(OldStart);
    CurrentOperationLine->SetEnd(OldEnd);

    return true;
}

bool FArmyRoomOperation::CanMoveOuterWallLine(const FVector& InNewStart, const FVector& InNewEnd)
{
    // @欧石楠 记录移动前的线段端点位置
    FVector OldStart = CurrentOperationLine->GetStart();
    FVector OldEnd = CurrentOperationLine->GetEnd();

    // @欧石楠 移动线段到新的位置
    CurrentOperationLine->SetStart(InNewStart);
    CurrentOperationLine->SetEnd(InNewEnd);

    TArray<FVector> OutRoomVertexes;
    CurrentEditRoom.Pin()->GetVertexes(OutRoomVertexes);

    TArray<FObjectWeakPtr> Rooms;
    FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, Rooms);
    for (auto It : Rooms)
    {
        TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
        if (Room.IsValid() && Room->IsClosed() && Room != CurrentEditRoom)
        {
            TArray<FVector> RoomVertexes;
            Room->GetVertexes(RoomVertexes);

            // @欧石楠 判断移动后的外墙是否在内墙以内
            if (!FArmyMath::IsPolygonInPolygon(RoomVertexes, OutRoomVertexes))
            {
                // @欧石楠 将线段移回原位
                CurrentOperationLine->SetStart(OldStart);
                CurrentOperationLine->SetEnd(OldEnd);

                GGI->Window->ShowMessage(MT_Warning, TEXT("外墙位置不能超过内墙范围"));
                return false;
            }
        }
    }

    // @欧石楠 将线段移回原位
    CurrentOperationLine->SetStart(OldStart);
    CurrentOperationLine->SetEnd(OldEnd);

    return true;
}

void FArmyRoomOperation::MoveEditPoint(const FVector& InNewPos, bool bUserInput)
{
    SCOPE_TRANSACTION(TEXT("移动点"));

    TSharedPtr<FArmyEditPoint> CapturedPoint = nullptr;

	/** @欧石楠 两端不连接的线结成一个房间*/
	bool bHasConnect2OtherRoom = false;
	TArray<TWeakPtr<FArmyObject>> AllRooms;
	TArray<TSharedPtr<FArmyRoom>> AllUnCompleteRoom;//找到全部未闭合的空间
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, AllRooms);
	for (auto It : AllRooms)
	{
		//本次操作的未完成房间直接略过
		if (It.Pin() == CurrentEditRoom.Pin())
		{
			continue;
		}
		TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (!TempRoom->IsClosed())
		{
			bool bCanMerge = false;
			for (auto It : TempRoom->GetPoints())
			{
				if (It->GetPos().Equals(InNewPos, 0.1f))
				{
					bCanMerge = true;
					break;
				}
			}
			if(bCanMerge)
			{
				TArray<TSharedPtr<FArmyEditPoint>> TempPoints1 = TempRoom->GetPoints();
				TArray<TSharedPtr<FArmyEditPoint>> TempPoints2 = CurrentEditRoom.Pin()->GetPoints();
				FArmySceneData::Get()->Delete(TempRoom);
				FArmySceneData::Get()->Delete(CurrentEditRoom.Pin());
				if (TempPoints1[0]->GetPos().Equals(InNewPos, 0.1f))
				{					
					FArmyMath::ReverseArray(TempPoints1);
				}
				TempPoints1.RemoveAt(TempPoints1.Num() - 1);
				if (!TempPoints2[0]->GetPos().Equals(InNewPos, 0.1f))
				{
					FArmyMath::ReverseArray(TempPoints2);
				}
				TempPoints1.Append(TempPoints2);
				PrePoint = nullptr;
				CurrentEditRoom = nullptr;							
				for (auto It : TempPoints1)
				{					
					AddLinePoint(It->GetPos());
				}								
				bHasConnect2OtherRoom = true;
				break;
			}
		}
	}
	//@欧石楠 如果前面与不同未完成房间合并了则不走下面逻辑
	if (!bHasConnect2OtherRoom)
	{
		// @欧石楠 找到当前移动操作点到新位置是否会连接其它操作点，且该点必须是独立端点才能连接
		for (auto It : CurrentEditRoom.Pin()->GetPoints())
		{
			if ((It != CurrentOperationPoint) && It->GetPos().Equals(InNewPos, 0.1f) && It->GetReferenceNum() < 2)
			{
				CapturedPoint = It;
				break;
			}
		}

		if (CapturedPoint.IsValid()) // @欧石楠 连接两个重叠的操作点
		{
			TArray< TSharedPtr<FArmyLine> > CapturedLineArr = CurrentOperationPoint->GetReferenceLines();
			TArray< TSharedPtr<FArmyLine> > CapturedLineArr1 = CapturedPoint->GetReferenceLines();

			CapturedPoint->SetPos(InNewPos);
			CapturedPoint->AddReferenceLine(CapturedLineArr[0]);

			if (CapturedLineArr[0]->GetStart().Equals(CurrentOperationPoint->GetPos(), 0.1f))
			{
				CurrentEditRoom.Pin()->RemovePoint(CapturedLineArr[0]->GetStartPointer());
				CapturedLineArr[0]->SetStartPointer(CapturedPoint);
			}
			else if (CapturedLineArr[0]->GetEnd().Equals(CurrentOperationPoint->GetPos(), 0.1f))
			{
				CurrentEditRoom.Pin()->RemovePoint(CapturedLineArr[0]->GetEndPointer());
				CapturedLineArr[0]->SetEndPointer(CapturedPoint);
			}			
		}
		else // @欧石楠 没有连接操作点
		{
			// @欧石楠 如果是用户输入，需要手动更新位置
			if (bUserInput)
			{
				CurrentOperationPoint->SetPos(InNewPos);
			}
		}
		CurrentEditRoom.Pin()->CombineCollinearLines();
	}    	
    CurrentEditRoom.Pin()->UpdateSpaceNamePos();
    CurrentEditRoom.Pin()->Modify();

    EndOperation();
}
