#include "ArmyHardwareOperation.h"
#include "SArmyInputBox.h"
#include "ArmyBearingWall.h"
#include "ArmyGameInstance.h"
#include "ArmySingleDoor.h"
#include "ArmyWindow.h"
#include "ArmyFloorWindow.h"
#include "ArmyRectBayWindow.h"
#include "ArmyTrapeBayWindow.h"
#include "ArmyCornerBayWindow.h"
#include "ArmyMath.h"
#include "ArmySlidingDoor.h"
#include "ArmyPass.h"
#include "ArmyNewPass.h"
#include "ArmyLine.h"
#include "ArmyRoom.h"
#include "ArmyRect.h"
#include "ArmyPunch.h"
#include "ArmyBearingWall.h"
#include "ArmyGameInstance.h"
#include "ArmyRectSelect.h"
#include "ArmySecurityDoor.h"
#include "ArmyToolsModule.h"
#include "ArmyWallLine.h"
#include "ArmyAirLouver.h"

FArmyHardwareOperation::FArmyHardwareOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
    , OperationPoint(NULL)
    , HardwareType(OT_None)
{
}

void FArmyHardwareOperation::BeginOperation(XRArgument InArg /*= XRArgument()*/)
{
	HardwareType = (EObjectType)InArg._ArgInt32;
	switch (HardwareType)
	{
	case OT_None:
		TempHardware = StaticCastSharedPtr<FArmyHardware>(FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects()[0].Pin());		
		break;

	case OT_Door:
		TempHardware = MakeShareable(new FArmySingleDoor());
		break;

	case OT_SecurityDoor:
		TempHardware = MakeShareable(new FArmySecurityDoor());
		break;

	case OT_SlidingDoor:
		TempHardware = MakeShareable(new FArmySlidingDoor());
		break;

	case OT_Pass:
		TempHardware = MakeShareable(new FArmyPass());
		break;

	case OT_NewPass:
		TempHardware = MakeShareable(new FArmyNewPass());
		break;

	case OT_Window:
		TempHardware = MakeShareable(new FArmyWindow());
		break;

	case OT_RectBayWindow:
		TempHardware = MakeShareable(new FArmyRectBayWindow());
		break;

	case OT_TrapeBayWindow:
		TempHardware = MakeShareable(new FArmyTrapeBayWindow());
		break;

    case OT_CornerBayWindow:
        TempHardware = MakeShareable(new FArmyCornerBayWindow());
        break;

	case OT_FloorWindow:
		TempHardware = MakeShareable(new FArmyFloorWindow());
		break;

	case OT_Punch:
		TempHardware = MakeShareable(new FArmyPunch());
		break;

	case OT_AirLouver:
		TempHardware = MakeShareable(new FArmyAirLouver());
		break;
	default:
		break;
	}

	TempHardware->SetState(OS_Selected);
	//TempHardware->Update();

    // @欧石楠 开始操作前需要先清空捕捉线信息，否则清空画布后仍可以捕捉
    Lines.Empty();

	FArmyToolsModule::Get().GetRectSelectTool()->End();
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_OutRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid())
					Lines.AddUnique(WallLineInfo(Line,Room));
		}
	}	

	if (GetCurrentModel() == E_ModifyModel)
	{
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
		for (auto It : AddWallList)
		{
			TArray<TSharedPtr<FArmyLine>> AddWallLines;
			It.Pin()->GetLines(AddWallLines);
			for (auto Line : AddWallLines)
				if (Line.IsValid())
					Lines.AddUnique(WallLineInfo(Line, It.Pin()));
		}
	}
}

void FArmyHardwareOperation::EndOperation()
{
	TempHardware = NULL;
	SelectedHardware = nullptr;
	OperationPoint = NULL;
	Lines.Empty();
	bShouldAdd = false;

	FArmyOperation::EndOperation();
}

void FArmyHardwareOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint)
{
	if (Object.IsValid())
	{
		SelectedHardware = StaticCastSharedPtr<FArmyHardware>(Object);
		TempHardware = SelectedHardware;
		TempHardware->SetUniqueID(SelectedHardware->GetUniqueID());
		TempHardware->SetPos(SelectedHardware->GetPos());
		TempHardware->SetDirection(SelectedHardware->GetDirection());
		TempHardware->SetHeight(SelectedHardware->GetHeight());
		TempHardware->SetWidth(SelectedHardware->GetWidth());
		TempHardware->SetLength(SelectedHardware->GetLength());
		TempHardware->SetRightOpen(SelectedHardware->IsRightOpen());
		if (Object->GetType() == OT_Door)
		{
			TSharedPtr<FArmySingleDoor> TempSingleDoor = StaticCastSharedPtr<FArmySingleDoor>(TempHardware);
			TSharedPtr<FArmySingleDoor> SelectedSingleDoor = StaticCastSharedPtr<FArmySingleDoor>(SelectedHardware);
			//TempSingleDoor->DoorFrameModel = SelectedSingleDoor->DoorFrameModel;
			//TempSingleDoor->DoorModel = SelectedSingleDoor->DoorModel;
		}
		else if (Object->GetType() == OT_SlidingDoor)
		{
			TSharedPtr<FArmySlidingDoor> TempSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(TempHardware);
			TSharedPtr<FArmySlidingDoor> SelectedSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(SelectedHardware);
            TempSlidingDoor->SlidingDoorActor = SelectedSlidingDoor->SlidingDoorActor;
		}
		else if (Object->GetType() == OT_Pass)
		{
			TSharedPtr<FArmyPass> TempPass = StaticCastSharedPtr<FArmyPass>(TempHardware);
			TSharedPtr<FArmyPass> SelectedPass = StaticCastSharedPtr<FArmyPass>(SelectedHardware);
			TempPass->SetThickness(SelectedPass->GetThickness());
			TempPass->SetOuterWidth(SelectedPass->GetOuterWidth());
		}
		else if (Object->GetType() == OT_RectBayWindow)
		{
			TSharedPtr<FArmyRectBayWindow> tempRectWindow = StaticCastSharedPtr<FArmyRectBayWindow>(TempHardware);
			TSharedPtr<FArmyRectBayWindow> seletedObject = StaticCastSharedPtr<FArmyRectBayWindow>(SelectedHardware);
			tempRectWindow->SetWindowLeftDist(seletedObject->GetWidowLeftDist());
			tempRectWindow->SetWindowRightDist(seletedObject->GetWindowRightDist());
			tempRectWindow->SetWindowHoleDepth(seletedObject->GetWindowHoleDepth());
			tempRectWindow->SetWindowType(seletedObject->GetWindowType());
			//tempRectWindow->WindowBoard = seletedObject->WindowBoard;
			//tempRectWindow->WindowGlass = seletedObject->WindowGlass;
			//tempRectWindow->WindowPillar = seletedObject->WindowPillar;
			tempRectWindow->HoleWindow = seletedObject->HoleWindow;
			tempRectWindow->WindowWall = seletedObject->WindowWall;

		}
		else if (Object->GetType() == OT_TrapeBayWindow)
		{
			TSharedPtr<FArmyTrapeBayWindow> tempRectWindow = StaticCastSharedPtr<FArmyTrapeBayWindow>(TempHardware);
			TSharedPtr<FArmyTrapeBayWindow> seletedObject = StaticCastSharedPtr<FArmyTrapeBayWindow>(SelectedHardware);
			tempRectWindow->SetWindowLeftDist(seletedObject->GetWidowLeftDist());
			tempRectWindow->SetWindowRightDist(seletedObject->GetWindowRightDist());
			tempRectWindow->SetWindowHoleDepth(seletedObject->GetWindowHoleDepth());
			tempRectWindow->SetUpHoleLength(seletedObject->GetUpHoleLength());
			tempRectWindow->SetWindowType(seletedObject->GetWindowType());
			/*		tempRectWindow->WindowBoard = seletedObject->WindowBoard;
					tempRectWindow->WindowGlass = seletedObject->WindowGlass;
					tempRectWindow->WindowPillar = seletedObject->WindowPillar;*/
			tempRectWindow->HoleWindow = seletedObject->HoleWindow;
			tempRectWindow->WindowWall = seletedObject->WindowWall;
		}
        else if (Object->GetType() == OT_CornerBayWindow)
        {
            TSharedPtr<FArmyCornerBayWindow> TempCornerBayWindow = StaticCastSharedPtr<FArmyCornerBayWindow>(TempHardware);
            TSharedPtr<FArmyCornerBayWindow> SelectedCornerBayWindow = StaticCastSharedPtr<FArmyCornerBayWindow>(SelectedHardware);
            TempCornerBayWindow->SetProperty(
                SelectedCornerBayWindow->InnearCenterPos, 
                SelectedCornerBayWindow->LeftDirection, 
                SelectedCornerBayWindow->RightDirection, 
                SelectedCornerBayWindow->LeftWallWidth, 
                SelectedCornerBayWindow->RightWallWidth);
            TempCornerBayWindow->SetLeftWindowLength(SelectedCornerBayWindow->GetLeftWindowLength());
            TempCornerBayWindow->SetRightWindowLength(SelectedCornerBayWindow->GetRightWindowLength());
        }
		else if (Object->GetType() == OT_Window || Object->GetType() == OT_FloorWindow || Object->GetType() == OT_RectBayWindow)
		{
			TSharedPtr<FArmyWindow> tempWindow = StaticCastSharedPtr<FArmyWindow>(TempHardware);
			TSharedPtr<FArmyWindow> selectObject = StaticCastSharedPtr<FArmyWindow>(SelectedHardware);
	
			tempWindow->HoleWindow = selectObject->HoleWindow;
			tempWindow->SetWindowType(selectObject->GetWindowType());
		}
	}
	OperationPoint = InOperationPoint;
}

void FArmyHardwareOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (TempHardware.IsValid())
	{
		TempHardware->Draw(PDI, View);
	}
}

bool FArmyHardwareOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_OutRoom, RoomList);

	if (HardwareType != OT_None) // 添加操作
	{
		if (InViewPort->KeyState(EKeys::LeftMouseButton))
		{
            if (bShouldAdd)
            {
				float Distance = FArmyMath::CalcPointToLineDistance(CaptureWallInfo.FirstLine->GetStart(),
					CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
				if (Distance > G_MaxRoomSpacing || Distance < G_MinRoomSpacing)
				{
					GGI->Window->ShowMessage(MT_Warning, TEXT("请放置在10-1000的墙上"));
					return false;
				}

				//判断是否在拆改墙上
				TArray<TWeakPtr<FArmyObject>> ModifyWallList;
				FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_ModifyWall, ModifyWallList);
				bool isInModify = false;
				for (auto It : ModifyWallList)
				{
					TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
					if (ModifyWall->IsPointInObj(TempHardware->GetStartPos())
						|| ModifyWall->IsPointInObj(TempHardware->GetEndPos())
						|| TempHardware->IsPointInObj(ModifyWall->GetBounds().GetCenter()))
					{
						isInModify = true;

						TArray<TWeakPtr<FArmyObject>> AddWallList;
						FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_AddWall, AddWallList);
						FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_IndependentWall, AddWallList);
						for (auto TempIt : AddWallList)
						{
							TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(TempIt.Pin());
							if (AddWall->IsPointInObj(TempHardware->GetStartPos())
								&& AddWall->IsPointInObj(TempHardware->GetEndPos()))
							{
								isInModify = false;
								break;
							}
						}
					}
					if (isInModify)
					{
						GGI->Window->ShowMessage(MT_Warning, TEXT("拆除墙体上不能放置门窗/门洞"));
						return false;
					}
				}

				//@
				TArray<TWeakPtr<FArmyObject>> BearingWallList;
				FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_BearingWall, BearingWallList);
				for (auto It : BearingWallList)
				{
					TSharedPtr<FArmyBearingWall> Wall = StaticCastSharedPtr<FArmyBearingWall>(It.Pin());
					if (Wall.IsValid())
					{
						if (Wall->IsPointInObj(TempHardware->GetStartPos())
							|| Wall->IsPointInObj(TempHardware->GetEndPos())
							|| TempHardware->IsPointInObj(Wall->GetStartPos())
							|| TempHardware->IsPointInObj(Wall->GetEndPos()))
						{
							GGI->Window->ShowMessage(MT_Warning, TEXT("承重墙体上不能放置门窗/门洞"));
							return false;
						}
					}
				}

                SCOPE_TRANSACTION(TEXT("添加门窗"));
								
				TSharedPtr<FArmyHardware> Hardware = Exec_AddHardware(TempHardware);
				if (!Hardware.IsValid())
					return false;

				for (auto It : RoomList)
				{
					TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
					if (Room.IsValid())
					{
						TArray<TSharedPtr<class FArmyWallLine>> WallLines = Room->GetWallLines();
						for (auto ItLine : WallLines)
						{
							if (ItLine->GetCoreLine() == CaptureWallInfo.FirstLine
								|| ItLine->GetCoreLine() == CaptureWallInfo.SecondLine)
							{
								ItLine->PushAppendObject(Hardware);
							}
						}
					}
				}
            }
		}
		else if (InViewPort->KeyState(EKeys::RightMouseButton) || InViewPort->KeyState(EKeys::Escape))
		{
			TempHardware->DeselectPoints();
			EndOperation();
		}
	}
	else // 拖拽操作
	{
		/*@欧石楠 这里可以统一，具体的逻辑判断都在drag里做的**/
		if (Event == IE_Pressed)
		{			
			DragPrePos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
			GVC->GetMousePosition(MousePreDragPos);
			PrePos = SelectedHardware->GetPos();
			PreStartPos = SelectedHardware->StartPoint->GetPos();
			PreEndPos= SelectedHardware->EndPoint->GetPos();

			if (TempHardware->GetOperationPointType() == 3)
				for (auto It : RoomList)
				{
					TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
					if (Room.IsValid())
					{
						TArray<TSharedPtr<class FArmyWallLine>> WallLines = Room->GetWallLines();
						for (auto ItLine : WallLines)
						{
							if (ItLine->GetCoreLine() == CaptureWallInfo.FirstLine
								|| ItLine->GetCoreLine() == CaptureWallInfo.SecondLine)
							{
								ItLine->RemoveAppendObject(SelectedHardware);
							}
						}
					}
				}
		}			
		else if (Event == IE_Released)
		{			
			TArray<TWeakPtr<FArmyObject>> HardWareList;
			FArmySceneData::Get()->GetHardWareObjects(HardWareList, GetCurrentModel());
			bool isPlaceDoor = false;
			for (auto It : HardWareList)
			{
				TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
				if (Hardware == SelectedHardware)
					continue;

				if ((Hardware->IsPointInObj(TempHardware->GetStartPos())
					|| Hardware->IsPointInObj(TempHardware->GetEndPos())
					|| TempHardware->IsPointInObj(Hardware->GetStartPos())
					|| TempHardware->IsPointInObj(Hardware->GetEndPos())))
				{
					if ((TempHardware->GetType() == OT_Door || TempHardware->GetType() == OT_SlidingDoor)
						&& (Hardware->GetType() == OT_Pass || Hardware->GetType() == OT_DoorHole))
					{
						TSharedPtr<FArmyPass> TempPass = StaticCastSharedPtr<FArmyPass>(Hardware);
						if (TempPass.IsValid() && (!TempPass->ConnectHardwareObj.IsValid() || (TempPass->ConnectHardwareObj.IsValid() && TempPass->ConnectHardwareObj == TempHardware)))
						{
							isPlaceDoor = true;							
						}
						else
						{
							ResumeHardware(TempHardware);
							GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗不可以重合摆放！"));
							EndOperation();
							return false;
						}
					}
					else
					{
                        ResumeHardware(TempHardware);
						GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗不可以重合摆放！"));
						EndOperation();
						return false;
					}
				}
			}

			//@
			TArray<TWeakPtr<FArmyObject>> WallList;
			FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_BearingWall, WallList);
			FArmySceneData::Get()->GetObjects(GetCurrentModel(), OT_ModifyWall, WallList);
			bool bPlaceToErrorPos = false;
			for (auto It : WallList)
			{
				if (It.Pin()->GetType() == OT_BearingWall)
				{
					TSharedPtr<FArmyBearingWall> BearingWall = StaticCastSharedPtr<FArmyBearingWall>(It.Pin());
					if ((BearingWall.IsValid() && (BearingWall->IsPointInObj(TempHardware->GetStartPos())
						|| BearingWall->IsPointInObj(TempHardware->GetEndPos())
						|| TempHardware->IsPointInObj(BearingWall->GetStartPos())
						|| TempHardware->IsPointInObj(BearingWall->GetEndPos()))))
					{
						bPlaceToErrorPos = true;
						GGI->Window->ShowMessage(MT_Warning, TEXT("承重墙体上不能放置门窗/门洞"));
					}					
				}
				else if (It.Pin()->GetType() == OT_ModifyWall)
				{
					TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
					if (((ModifyWall.IsValid() && (ModifyWall->IsPointInObj(TempHardware->GetStartPos())
							|| ModifyWall->IsPointInObj(TempHardware->GetEndPos())
							|| TempHardware->IsPointInObj(ModifyWall->GetStartPos())
							|| TempHardware->IsPointInObj(ModifyWall->GetEndPos())))))
					{
						bPlaceToErrorPos = true;
						GGI->Window->ShowMessage(MT_Warning, TEXT("拆除墙体上不能放置门窗/门洞"));
					}					
				}											
				if (bPlaceToErrorPos)
				{
                    ResumeHardware(TempHardware);
					EndOperation();
					return false;
				}
			}

            EObjectType FirstOwnerType = CaptureWallInfo.FirstOwner->GetType();
            EObjectType SecondOwnerType = CaptureWallInfo.SecondOwner->GetType();
            
			/**@欧石楠 限制防盗门不可以被拖拽到内墙上*/
			if (SelectedHardware->GetType() == OT_SecurityDoor)
			{
                if (FirstOwnerType == OT_InternalRoom && SecondOwnerType == OT_InternalRoom)
                {
                    GGI->Window->ShowMessage(MT_Warning, TEXT("防盗门不能移动到内墙上"));
                    ResumeHardware(TempHardware);
                    EndOperation();
                    return false;
                }
			}			

			/**@欧石楠 限制原始门洞/垭口不可以被拖拽到外墙上*/
			if (SelectedHardware->GetType() == OT_Pass || SelectedHardware->GetType() == OT_NewPass)
			{
                if (FirstOwnerType == OT_OutRoom || SecondOwnerType == OT_OutRoom)
                {
                    GGI->Window->ShowMessage(MT_Warning, TEXT("门洞/垭口不能移动到外墙上"));
                    ResumeHardware(TempHardware);
                    EndOperation();
                    return false;
                }
			}

			/**@欧石楠 限制门不可以被拖拽到包边的门洞上*/
			if (SelectedHardware->GetType() == OT_SlidingDoor || SelectedHardware->GetType() == OT_Door)
			{
                if (FirstOwnerType == OT_OutRoom || SecondOwnerType == OT_OutRoom)
                {
                    FString ErrorMsg = SelectedHardware->GetType() == OT_Door ? TEXT("普通门不能放到外墙上") :
                        TEXT("推拉门不能放到外墙上");
                    GGI->Window->ShowMessage(MT_Warning, ErrorMsg);
                    ResumeHardware(TempHardware);
                    EndOperation();
                    return false;
                }

				TArray<TWeakPtr<FArmyObject>> HoleList;
				FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pass, HoleList);
				for (auto It : HoleList)
				{
					TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
					if (Pass->GetIfGeneratePassModel() && (Pass->IsPointInObj(SelectedHardware->GetStartPos())
						|| Pass->IsPointInObj(SelectedHardware->GetEndPos())
						|| SelectedHardware->IsPointInObj(Pass->GetStartPos())
						|| SelectedHardware->IsPointInObj(Pass->GetEndPos())))
					{
						GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门不能移动到有包边的门洞内"));
                        ResumeHardware(TempHardware);
						EndOperation();
						return false;
					}
					//限制不能被拖拽到已填补的门洞上
					if (Pass->GetIfFillPass() && (Pass->IsPointInObj(SelectedHardware->GetStartPos())
						|| Pass->IsPointInObj(SelectedHardware->GetEndPos())
						|| SelectedHardware->IsPointInObj(Pass->GetStartPos())
						|| SelectedHardware->IsPointInObj(Pass->GetEndPos())))
					{
						GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门不能移动到已填补的门洞内"));
                        ResumeHardware(TempHardware);
						EndOperation();
						return false;
					}
				}
			}			

            SetHardwareLinkedInfo(TempHardware);

			if (TempHardware->GetOperationPointType() == 3)
				for (auto It : RoomList)
				{
					TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
					if (Room.IsValid())
					{
						TArray<TSharedPtr<class FArmyWallLine>> WallLines = Room->GetWallLines();
						for (auto ItLine : WallLines)
						{
							if (ItLine->GetCoreLine() == CaptureWallInfo.FirstLine
								|| ItLine->GetCoreLine() == CaptureWallInfo.SecondLine)
							{
								ItLine->PushAppendObject(TempHardware);
							}
						}
					}
				}

			TempHardware->SetOperationPointType(0);		
			/**@欧石楠 拖拽限制不能超出墙体*/
			if (TempHardware->GetType() == OT_Punch)
			{
				TSharedPtr<FArmyPunch> TempPunch = StaticCastSharedPtr<FArmyPunch>(TempHardware);
				if (TempPunch.IsValid())
				{
					TempPunch->CheckIfOutLimited();
				}
			}

			FVector2D TempPoint;
			GVC->GetMousePosition(TempPoint);
			if (MousePreDragPos != TempPoint)
			{
				SCOPE_TRANSACTION(TEXT("拖拽修改门窗"));
				TempHardware->Modify();

				if (GetCurrentModel() == E_HomeModel)
					FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
				else if (GetCurrentModel() == E_ModifyModel)
					FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
			}
			EndOperation();
		}		
	}

	return false;
}

void FArmyHardwareOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
{
	if (HardwareType == OT_None)
		return;

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

	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	WorldPos.Z = 0;

    // 转角飘窗需要捕捉墙角
    if (TempHardware.IsValid() && TempHardware->GetType() == EObjectType::OT_CornerBayWindow)
    {
        if (CaptureWallCorner(WorldPos, CaptureWallCornerInfo))
        {
            SetWallCornerProperty(CaptureWallCornerInfo.Pos);
            bShouldAdd = true;
        }
        else
        {
            SetWallCornerProperty(WorldPos);
            bShouldAdd = false;
        }
    }
    else
	{
        if (CaptureDoubleLine(WorldPos))
        {
            CaculateHardwareDirectionAndPos(CaptureWallInfo.Pos);
            bShouldAdd = true;
        }
        else
        {
            CaculateHardwareDirectionAndPos(WorldPos);
            bShouldAdd = false;
        }
	}
}

void FArmyHardwareOperation::MouseDrag(UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, FKey Key)
{
	if (HardwareType != OT_None)
		return;

	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);	
	FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	WorldPos.Z = 0;

	/**@欧石楠 如果数值异常则中断操作*/
	if (FMath::Abs(MousePos.X) > 1000000 || FMath::Abs(MousePos.Y) > 1000000)
	{
		EndOperation();
		return;
	}

	TempHardware->bModified = true;

	if (TempHardware->GetOperationPointType() == 3)
	{
        if (TempHardware->GetType() == OT_CornerBayWindow)
        {
            if (CaptureWallCorner(WorldPos, CaptureWallCornerInfo))
            {
                SetWallCornerProperty(CaptureWallCornerInfo.Pos);
                if (TempHardware->LinkFirstLine != CaptureWallCornerInfo.FirstLine)
                {
                    TempHardware->LinkFirstLine = CaptureWallCornerInfo.FirstLine;
                    TempHardware->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallCornerInfo.FirstLine, GetCurrentModel());
					TempHardware->InWallType = (CaptureWallInfo.FirstOwner->GetType() == OT_AddWall || CaptureWallInfo.FirstOwner->GetType() == OT_IndependentWall) ? 1 : 0;
                }
                if (TempHardware->LinkSecondLine != CaptureWallCornerInfo.SecondLine)
                {
                    TempHardware->LinkSecondLine = CaptureWallCornerInfo.SecondLine;
                    TempHardware->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallCornerInfo.SecondLine, GetCurrentModel());
					TempHardware->InWallType = (CaptureWallInfo.SecondOwner->GetType() == OT_AddWall || CaptureWallInfo.SecondOwner->GetType() == OT_IndependentWall) ? 1 : 0;
                }
            }
        }
        else
        {
            if (CaptureDoubleLine(WorldPos))
            {
                CaculateHardwareDirectionAndPos(CaptureWallInfo.Pos);
                if (TempHardware->LinkFirstLine != CaptureWallInfo.FirstLine)
                {
                    TempHardware->LinkFirstLine = CaptureWallInfo.FirstLine;
					
                    TempHardware->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.FirstLine, GetCurrentModel());
					TempHardware->InWallType = (CaptureWallInfo.FirstOwner->GetType() == OT_AddWall || CaptureWallInfo.FirstOwner->GetType() == OT_IndependentWall) ? 1 : 0;
                }
                if (TempHardware->LinkSecondLine != CaptureWallInfo.SecondLine)
                {
                    TempHardware->LinkSecondLine = CaptureWallInfo.SecondLine;
                    TempHardware->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.SecondLine, GetCurrentModel());
					TempHardware->InWallType = (CaptureWallInfo.SecondOwner->GetType() == OT_AddWall || CaptureWallInfo.SecondOwner->GetType() == OT_IndependentWall) ? 1 : 0;
                }
            }
        }	
	}
	/*@欧石楠拖拽缩放点**/
	else if (TempHardware->GetOperationPointType() == 1 || TempHardware->GetOperationPointType() == 2)
	{
		if (TempHardware->ResultPoint == TempHardware->EndPoint)
		{
			FVector TempProjectionPos = FArmyMath::GetProjectionPoint(WorldPos, TempHardware->GetStartPos(), TempHardware->GetEndPos());
			TempHardware->EndPoint->SetPos(TempProjectionPos);
			TempHardware->UpdateEndPoint();
		}
		else
		{
			FVector TempProjectionPos = FArmyMath::GetProjectionPoint(WorldPos, TempHardware->GetStartPos(), TempHardware->GetEndPos());
			TempHardware->StartPoint->SetPos(TempProjectionPos);
			TempHardware->UpdateStartPoint();
		}
	}
	else if (TempHardware->GetOperationPointType() == 4)
	{
		FVector NewDirection = SelectedHardware->GetDirection();
		bool bRightOpen = SelectedHardware->IsRightOpen();

		FVector OperationDirection = FArmyMath::GetLineDirection(SelectedHardware->GetPos(), WorldPos);
		if (FVector::DotProduct(OperationDirection, NewDirection) < 0)
		{
			NewDirection = -NewDirection;
			bRightOpen = !bRightOpen;
		}

		FVector StartDirection = FArmyMath::GetLineDirection(SelectedHardware->GetPos(), SelectedHardware->GetStartPos());
		if (FVector::DotProduct(OperationDirection, StartDirection) > 0)
		{
			bRightOpen = !bRightOpen;
		}

		TempHardware->SetDirection(NewDirection);
        TempHardware->SetRightOpen(bRightOpen);
	}

	if (TempHardware->GetType() == OT_Door)
	{
		TSharedPtr<FArmySingleDoor> TempHWDoor = StaticCastSharedPtr<FArmySingleDoor>(TempHardware);
		TempHWDoor->UpdateDoorHole();
	}
	else if (TempHardware->GetType() == OT_SlidingDoor)
	{
		TSharedPtr<FArmySlidingDoor> TempHWSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(TempHardware);
		TempHWSlidingDoor->UpdateDoorHole();
	}
}

TSharedPtr<FArmyHardware> FArmyHardwareOperation::Exec_AddHardware(TSharedPtr<FArmyHardware> ResultHardware)
{
	//首先判断start和end是否超出外墙边界
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (!FArmyMath::IsPointInOrOnPolygon2D(ResultHardware->GetStartPos(), Room->GetWorldPoints(true))
			|| !FArmyMath::IsPointInOrOnPolygon2D(ResultHardware->GetEndPos(), Room->GetWorldPoints(true)))
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗范围不能超出外墙线"));
			return nullptr;
		}
	}	

	//首先判断是否重合
	TArray<TWeakPtr<FArmyObject>> HardWareList;
	FArmySceneData::Get()->GetHardWareObjects(HardWareList, GetCurrentModel());
	bool isPlaceDoor = false;
	for (auto It : HardWareList)
	{
		/** @欧石楠 空调孔不参与重合判断*/
		if (ResultHardware->GetType() == OT_AirLouver)
		{
			break;
		}

		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		if ((Hardware->IsPointInObj(ResultHardware->GetStartPos())
			|| Hardware->IsPointInObj(ResultHardware->GetEndPos())
			|| ResultHardware->IsPointInObj(Hardware->GetStartPos())
			|| ResultHardware->IsPointInObj(Hardware->GetEndPos())))
		{
			if ((ResultHardware->GetType() == OT_Door || ResultHardware->GetType() == OT_SlidingDoor || ResultHardware->GetType() == OT_SecurityDoor)
				&& (Hardware->GetType() == OT_Pass || Hardware->GetType() == OT_DoorHole))
			{
				isPlaceDoor = true;
				break;
			}
			else
			{
				/**@欧石楠 如果门放到垭口上，提示*/
				if ((ResultHardware->GetType() == OT_Door || ResultHardware->GetType() == OT_SlidingDoor)
					&& (Hardware->GetType() == OT_NewPass))
				{
					GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("垭口上不允许放置门！"));
				}
				else
				{
					GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗不可以重合摆放！"));
				}				
				return nullptr;
			}
		}
	}

	TSharedPtr<FArmyHardware> Result = NULL;

	XRArgument Arg;
	if (ResultHardware->GetType() == OT_Door)
	{
		/*if (!isPlaceDoor)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门必须放到门洞内！"));
			return nullptr;
		}*/

		if (CaptureWallInfo.FirstOwner->GetType() == OT_OutRoom
			|| CaptureWallInfo.SecondOwner->GetType() == OT_OutRoom)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("普通门不能放到外墙上"));
			return nullptr;
		}

		TSharedPtr<FArmySingleDoor> TempSingleDoor = StaticCastSharedPtr<FArmySingleDoor>(ResultHardware);

		TArray<TWeakPtr<FArmyObject>> HoleList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pass, HoleList);
		for (auto It : HoleList)
		{
			TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
			if (Pass->GetIfGeneratePassModel() && (Pass->IsPointInObj(TempSingleDoor->GetStartPos())
				|| Pass->IsPointInObj(TempSingleDoor->GetEndPos())
				|| TempSingleDoor->IsPointInObj(Pass->GetStartPos())
				|| TempSingleDoor->IsPointInObj(Pass->GetEndPos())))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("不能在有包边的门洞内放置门！"));
				return nullptr;
			}

			/**@欧石楠 限制不能在已填补的门洞里放置普通门*/
			if (Pass->GetIfFillPass() && (Pass->IsPointInObj(TempSingleDoor->GetStartPos())
				|| Pass->IsPointInObj(TempSingleDoor->GetEndPos())
				|| TempSingleDoor->IsPointInObj(Pass->GetStartPos())
				|| TempSingleDoor->IsPointInObj(Pass->GetEndPos())))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("不能在已填补的门洞内放置门！"));
				return nullptr;
			}
		}		

		TArray<TWeakPtr<FArmyObject>> DoorList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Door, DoorList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_SlidingDoor, DoorList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_SecurityDoor, DoorList);
		for (auto It : DoorList)
		{
			TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
			if (Hardware->IsPointInObj(TempSingleDoor->GetStartPos())
				|| Hardware->IsPointInObj(TempSingleDoor->GetEndPos())
				|| TempSingleDoor->IsPointInObj(Hardware->GetStartPos())
				|| TempSingleDoor->IsPointInObj(Hardware->GetEndPos()))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗不可以重合摆放！"));
				return nullptr;
			}
		}

		//@ 在新建墙上放置门洞需要填充黑色，否则不填充
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
		TempSingleDoor->RectImagePanel->bIsFilled = false;
		for (auto It : AddWallList)
		{
			TSharedPtr<FArmyAddWall> Wall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
			if (Wall->IsPointInObj(TempSingleDoor->Point->GetPos()))
			{
				TempSingleDoor->RectImagePanel->bIsFilled = true;
				break;
			}
		}
		
		TSharedPtr<FArmySingleDoor> ResultSingleDoor = MakeShareable(new FArmySingleDoor(TempSingleDoor.Get()));
		ResultSingleDoor->UpdateDoorHole();
		Result = ResultSingleDoor;
	}
	else if (ResultHardware->GetType() == OT_SecurityDoor)
	{
		//防盗门单独处理(只能放到外墙)
		if ((CaptureWallInfo.FirstOwner->GetType() == OT_InternalRoom
			&& CaptureWallInfo.SecondOwner->GetType() == OT_InternalRoom)
			|| (CaptureWallInfo.FirstOwner->GetType() == OT_OutRoom
				&& CaptureWallInfo.SecondOwner->GetType() == OT_OutRoom))
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("防盗门必须放置在外墙上"));
			return nullptr;
		}

		TSharedPtr<FArmySecurityDoor> TempSecurityDoor = StaticCastSharedPtr<FArmySecurityDoor>(ResultHardware);		
		TArray<TWeakPtr<FArmyObject>> DoorList;		
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_SecurityDoor, DoorList);
		for (auto It : DoorList)
		{
			TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
			if (Hardware->IsPointInObj(TempSecurityDoor->GetStartPos())
				|| Hardware->IsPointInObj(TempSecurityDoor->GetEndPos())
				|| TempSecurityDoor->IsPointInObj(Hardware->GetStartPos())
				|| TempSecurityDoor->IsPointInObj(Hardware->GetEndPos()))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗不可以重合摆放！"));
				return nullptr;
			}
		}

		TSharedPtr<FArmySecurityDoor> ResultSecurityDoor = MakeShareable(new FArmySecurityDoor(TempSecurityDoor.Get()));
		ResultSecurityDoor->UpdateDoorHole();
		Result = ResultSecurityDoor;
	}
	else if (ResultHardware->GetType() == OT_SlidingDoor)
	{
		/*if (!isPlaceDoor)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门必须放到门洞内！"));
			return nullptr;
		}*/

		if (CaptureWallInfo.FirstOwner->GetType() == OT_OutRoom
			|| CaptureWallInfo.SecondOwner->GetType() == OT_OutRoom)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("普通门不能放到外墙上"));
			return nullptr;
		}

		TSharedPtr<FArmySlidingDoor> TempSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(ResultHardware);

		TArray<TWeakPtr<FArmyObject>> HoleList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pass, HoleList);
		for (auto It : HoleList)
		{
			TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
			if (Pass->GetIfGeneratePassModel() && (Pass->IsPointInObj(TempSlidingDoor->GetStartPos())
				|| Pass->IsPointInObj(TempSlidingDoor->GetEndPos())
				|| TempSlidingDoor->IsPointInObj(Pass->GetStartPos())
				|| TempSlidingDoor->IsPointInObj(Pass->GetEndPos())))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("不能在有包边的门洞内放置门！"));
				return nullptr;
			}
			/**@欧石楠 限制不能在已填补的门洞里放置普通门*/
			if (Pass->GetIfFillPass() && (Pass->IsPointInObj(TempSlidingDoor->GetStartPos())
				|| Pass->IsPointInObj(TempSlidingDoor->GetEndPos())
				|| TempSlidingDoor->IsPointInObj(Pass->GetStartPos())
				|| TempSlidingDoor->IsPointInObj(Pass->GetEndPos())))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("不能在已填补的门洞内放置门！"));
				return nullptr;
			}
		}		

		TArray<TWeakPtr<FArmyObject>> DoorList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Door, DoorList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_SlidingDoor, DoorList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_SecurityDoor, DoorList);
		for (auto It : DoorList)
		{
			TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
			if (Hardware->IsPointInObj(TempSlidingDoor->GetStartPos())
				|| Hardware->IsPointInObj(TempSlidingDoor->GetEndPos())
				|| TempSlidingDoor->IsPointInObj(Hardware->GetStartPos())
				|| TempSlidingDoor->IsPointInObj(Hardware->GetEndPos()))
			{
				GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("门窗不可以重合摆放！"));
				return nullptr;
			}
		}

		//@ 在新建墙上放置推拉门需要填充黑色，否则不填充
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
		TempSlidingDoor->RectImagePanel->bIsFilled = false;
		for (auto It : AddWallList)
		{
			TSharedPtr<FArmyAddWall> Wall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
			if (Wall->IsPointInObj(TempSlidingDoor->Point->GetPos()))
			{
				TempSlidingDoor->RectImagePanel->bIsFilled = true;
				break;
			}
		}

		TSharedPtr<FArmySlidingDoor> ResultSlidingDoor = MakeShareable(new FArmySlidingDoor(TempSlidingDoor.Get()));
		ResultSlidingDoor->UpdateDoorHole();
		Result = ResultSlidingDoor;
	}
	else if (ResultHardware->GetType() == OT_Pass)
	{
		/**@欧石楠 原始门洞只能放在内墙*/
		if (CaptureWallInfo.FirstOwner->GetType() == OT_OutRoom
			|| CaptureWallInfo.SecondOwner->GetType() == OT_OutRoom)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("原始门洞必须放置在内墙上"));
			return nullptr;
		}
		TSharedPtr<FArmyPass> TempPass = StaticCastSharedPtr<FArmyPass>(ResultHardware);
		Result = MakeShareable(new FArmyPass(TempPass.Get()));
	}
	else if (ResultHardware->GetType() == OT_NewPass)
	{
		/**@欧石楠 新开垭口只能放在内墙*/
		if (CaptureWallInfo.FirstOwner->GetType() == OT_OutRoom
			|| CaptureWallInfo.SecondOwner->GetType() == OT_OutRoom)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("新开垭口必须放置在内墙上"));
			return nullptr;
		}
		TSharedPtr<FArmyNewPass> TempPass = StaticCastSharedPtr<FArmyNewPass>(ResultHardware);
		Result = MakeShareable(new FArmyNewPass(TempPass.Get()));
	}
	else if (ResultHardware->GetType() == OT_Window)
	{
		TSharedPtr<FArmyWindow> TempSingleWindow = StaticCastSharedPtr<FArmyWindow>(ResultHardware);
		Result = MakeShareable(new FArmyWindow(TempSingleWindow.Get()));
	}
	else if (ResultHardware->GetType() == OT_FloorWindow)
	{
		TSharedPtr<FArmyFloorWindow> TempSingleWindow = StaticCastSharedPtr<FArmyFloorWindow>(ResultHardware);
		Result = MakeShareable(new FArmyFloorWindow(TempSingleWindow.Get()));
	}
	else if (ResultHardware->GetType() == OT_RectBayWindow)
	{
		if (CaptureWallInfo.FirstOwner->GetType() == OT_InternalRoom
			&& CaptureWallInfo.SecondOwner->GetType() == OT_InternalRoom)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("【飘窗】构件只能放在外墙上"));
			return nullptr;
		}

		TSharedPtr<FArmyRectBayWindow> TempRectBayWindow = StaticCastSharedPtr<FArmyRectBayWindow>(ResultHardware);
		Result = MakeShareable(new FArmyRectBayWindow(TempRectBayWindow.Get()));
	}
	else if (ResultHardware->GetType() == OT_TrapeBayWindow)
	{
		TSharedPtr<FArmyTrapeBayWindow> TempTrapeBayWindow = StaticCastSharedPtr<FArmyTrapeBayWindow>(ResultHardware);
		Result = MakeShareable(new FArmyTrapeBayWindow(TempTrapeBayWindow.Get()));
	}
    else if (ResultHardware->GetType() == OT_CornerBayWindow)
    {
        TSharedPtr<FArmyCornerBayWindow> TempCornerBayWindow = StaticCastSharedPtr<FArmyCornerBayWindow>(ResultHardware);
        Result = MakeShareable(new FArmyCornerBayWindow(TempCornerBayWindow.Get()));
    }
	else if (ResultHardware->GetType() == OT_Punch)
	{
		if (CaptureWallInfo.FirstOwner->GetType() == OT_OutRoom
			|| CaptureWallInfo.SecondOwner->GetType() == OT_OutRoom)
		{
			TSharedPtr<FArmyPunch> TempPunch = StaticCastSharedPtr<FArmyPunch>(ResultHardware);
			Result = MakeShareable(new FArmyPunch(TempPunch.Get()));
		}
		else
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("【开阳台】构件只允许放在外墙上"));
			return nullptr;
		}
	}
	else if (ResultHardware->GetType() == OT_AirLouver)
	{
		TSharedPtr<FArmyAirLouver> TempAirLouver = StaticCastSharedPtr<FArmyAirLouver>(ResultHardware);
		Result = MakeShareable(new FArmyAirLouver(TempAirLouver.Get()));
	}
	
	Result->SetState(OS_Normal);

	EModelType CurrentType;
	if (Result->GetType() == OT_SlidingDoor || Result->GetType() == OT_Door || Result->GetType() == OT_NewPass)
	{
		CurrentType = E_ModifyModel;
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
	}
	else
	{
		CurrentType = E_HomeModel;
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
	SetHardwareLinkedInfo(Result);
	FArmySceneData::Get()->Add(Result, XRArgument(1).ArgUint32(CurrentType).ArgFName(FName(TEXT("门窗"))));
	return Result;
}

void FArmyHardwareOperation::Exec_ModifyHardware(TSharedPtr<FArmyHardware> ModifyingHardware, TSharedPtr<FArmyHardware> ResultHardware)
{
	if (ModifyingHardware->GetType() == OT_Door)
	{
		ModifyHardware<FArmySingleDoor>(ModifyingHardware, ResultHardware);
	}
	else if (ModifyingHardware->GetType() == OT_SlidingDoor)
	{
		ModifyHardware<FArmySlidingDoor>(ModifyingHardware, ResultHardware);
	}
	else if (ModifyingHardware->GetType() == OT_Pass)
	{
		ModifyHardware<FArmyPass>(ModifyingHardware, ResultHardware);
	}
	else if (ModifyingHardware->GetType() == OT_Window)
	{
		ModifyHardware<FArmyWindow>(ModifyingHardware, ResultHardware);
	}
	else if (ModifyingHardware->GetType() == OT_RectBayWindow)
	{
		ModifyHardware<FArmyRectBayWindow>(ModifyingHardware, ResultHardware);
	}
	else if (ModifyingHardware->GetType() == OT_TrapeBayWindow)
	{
		ModifyHardware<FArmyTrapeBayWindow>(ModifyingHardware, ResultHardware);
	}
	else if (ModifyingHardware->GetType() == OT_FloorWindow)
	{
		ModifyHardware<FArmyFloorWindow>(ModifyingHardware, ResultHardware);

	}
}

void FArmyHardwareOperation::ForceCaptureDoubleLine(TSharedPtr<FArmyHardware> TempHW, EModelType InModelType)
{	
	//TempHardware = TempHW;
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(InModelType, OT_OutRoom, RoomList);
	Lines.Reset();
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto ItLine : RoomLines)
				if (ItLine.IsValid())
					Lines.AddUnique(WallLineInfo(ItLine,Room));
		}
	}

	TArray<TWeakPtr<FArmyObject>> AddWallList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_AddWall, AddWallList);
	FArmySceneData::Get()->GetObjects(InModelType, OT_IndependentWall, AddWallList);
	for (auto It : AddWallList)
	{
		TArray<TSharedPtr<FArmyLine>> AddWallLines;
		It.Pin()->GetLines(AddWallLines);
		for (auto Line : AddWallLines)
			if (Line.IsValid())
				Lines.AddUnique(WallLineInfo(Line, It.Pin()));
	}

	FVector WorldPos = TempHW->GetPos();
	WorldPos.Z = 0;
	
	if (CaptureDoubleLine(WorldPos))
	{		
		if (TempHW->LinkFirstLine != CaptureWallInfo.FirstLine)
		{
			TempHW->LinkFirstLine = CaptureWallInfo.FirstLine;
			TempHW->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.FirstLine, InModelType);
			TempHW->InWallType = (CaptureWallInfo.FirstOwner->GetType() == OT_AddWall || CaptureWallInfo.FirstOwner->GetType() == OT_IndependentWall) ? 1 : 0;
		}
		if (TempHW->LinkSecondLine != CaptureWallInfo.SecondLine)
		{
			TempHW->LinkSecondLine = CaptureWallInfo.SecondLine;
			TempHW->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.SecondLine, InModelType);
			TempHW->InWallType = (CaptureWallInfo.SecondOwner->GetType() == OT_AddWall || CaptureWallInfo.SecondOwner->GetType() == OT_IndependentWall) ? 1 : 0;
		}
	}	
}

bool FArmyHardwareOperation::IsOperating()
{
	return OperationPoint.IsValid();
}

void FArmyHardwareOperation::ResumeHardware(TSharedPtr<FArmyHardware> InHardware)
{
    if (InHardware.IsValid())
    {
        InHardware->SetPos(PrePos);
        InHardware->SetLength((PreStartPos - PreEndPos).Size());

        if (CaptureDoubleLine(PrePos))
        {
            CaculateHardwareDirectionAndPos(CaptureWallInfo.Pos);
            if (InHardware->LinkFirstLine != CaptureWallInfo.FirstLine)
            {
                InHardware->LinkFirstLine = CaptureWallInfo.FirstLine;
                InHardware->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.FirstLine, GetCurrentModel());
                InHardware->InWallType = (CaptureWallInfo.FirstOwner->GetType() == OT_AddWall ||
                    CaptureWallInfo.FirstOwner->GetType() == OT_IndependentWall) ? 1 : 0;
            }
            if (InHardware->LinkSecondLine != CaptureWallInfo.SecondLine)
            {
                InHardware->LinkSecondLine = CaptureWallInfo.SecondLine;
                InHardware->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.SecondLine, GetCurrentModel());
                InHardware->InWallType = (CaptureWallInfo.SecondOwner->GetType() == OT_AddWall ||
                    CaptureWallInfo.SecondOwner->GetType() == OT_IndependentWall) ? 1 : 0;
            }
        }

        if (InHardware->GetType() == OT_Door)
        {
            TSharedPtr<FArmySingleDoor> TempHWDoor = StaticCastSharedPtr<FArmySingleDoor>(InHardware);
            TempHWDoor->UpdateDoorHole();
        }
        else if (InHardware->GetType() == OT_SlidingDoor)
        {
            TSharedPtr<FArmySlidingDoor> TempHWSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(InHardware);
            TempHWSlidingDoor->UpdateDoorHole();
        }
    }
}

EModelType FArmyHardwareOperation::GetCurrentModel()
{
    return GGI->DesignEditor->CurrentController->GetDesignModelType();
}

bool FArmyHardwareOperation::CaptureDoubleLine(const FVector& Pos)
{
    if (FArmySceneData::Get()->CaptureDoubleLine(Lines, Pos, CaptureWallInfo))
    {
        //只有拖拽移动时才会更新bIsFilled，读档时不会进入此逻辑
        if (TempHardware.IsValid())
        {
            if (CaptureWallInfo.FirstOwner->GetType() == OT_AddWall ||
                CaptureWallInfo.SecondOwner->GetType() == OT_AddWall ||
                CaptureWallInfo.FirstOwner->GetType() == OT_IndependentWall ||
                CaptureWallInfo.SecondOwner->GetType() == OT_IndependentWall)
            {
                TempHardware->RectImagePanel->bIsFilled = true;
            }
            else
            {
                TempHardware->RectImagePanel->bIsFilled = false;
            }
        }

        return true;
    }

	return false;
}

bool FArmyHardwareOperation::CaptureWallCorner(const FVector& Pos, FWallCornerCaputureInfo& OutInfo)
{
    const float MaxDistance = 40.f;
    FVector FirstSnapPos = Pos;
    TArray< TSharedPtr<FArmyLine> > NearWalls;
    for (auto & LInfo : Lines)
    {
		TWeakPtr<FArmyLine> Object = LInfo.Line;
		if (!Object.IsValid()) continue;
        TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());
        FVector P0 = Wall->GetStart();
        FVector P1 = Wall->GetEnd();
        if (FArmyMath::CalcPointToLineDistance(Pos, P0, P1) <= MaxDistance)
        {
            NearWalls.Push(Wall);
        } 
    }
    int number = NearWalls.Num();
    if (number < 4)
    {
        return false;
    }
    TSharedPtr<FArmyLine> FirstWall = NearWalls[0];
    TSharedPtr<FArmyLine> SecondWall = NULL;
    NearWalls.RemoveAt(0);
    number = NearWalls.Num();
    for (int i = 0; i < number; i++)
    {
        SecondWall = NearWalls[i];
        bool bIsParallel = FArmyMath::AreLinesParallel(FirstWall->GetStart(), FirstWall->GetEnd(), SecondWall->GetStart(), SecondWall->GetEnd());
        if (bIsParallel)
        {
            NearWalls.RemoveAt(i);
            break;
        }
    }
    number = NearWalls.Num();
    if (number < 2 || !SecondWall.IsValid())
    {
        return false;
    }
    TSharedPtr<FArmyLine> ThirdWall = NearWalls[0];
    TSharedPtr<FArmyLine> FourthWall = NULL;
    NearWalls.RemoveAt(0);
    number = NearWalls.Num();
    for (int i = 0; i < number; i++)
    {
        FourthWall = NearWalls[i];
        bool bIsParallel = FArmyMath::AreLinesParallel(ThirdWall->GetStart(), ThirdWall->GetEnd(), FourthWall->GetStart(), FourthWall->GetEnd());
        if (bIsParallel)
        {
            NearWalls.RemoveAt(i);
            break;
        }
    }
    if (!FourthWall.IsValid())
    {
        return false;
    }
    FVector intersectPoint;
    FVector intersectPoint1;
    if (!FindIntersectionPoint(FirstWall, ThirdWall, FourthWall, intersectPoint))
        return false;
    if (!FindIntersectionPoint(SecondWall, ThirdWall, FourthWall, intersectPoint1))
        return false;
    if (intersectPoint == FirstWall->GetStart())
    {
        OutInfo.LeftWallDirection = (-intersectPoint + FirstWall->GetEnd()).GetSafeNormal();
    }
    else
    {
        OutInfo.LeftWallDirection = (-intersectPoint + FirstWall->GetStart()).GetSafeNormal();
    }
    if (intersectPoint1 == ThirdWall->GetStart())
    {
        OutInfo.RightWallDirection = (ThirdWall->GetEnd() - intersectPoint1).GetSafeNormal();
    }
    else if (intersectPoint1 == ThirdWall->GetEnd())
    {
        OutInfo.RightWallDirection = (ThirdWall->GetStart() - intersectPoint1).GetSafeNormal();
    }
    else if (intersectPoint1 == FourthWall->GetStart())
    {
        OutInfo.RightWallDirection = (FourthWall->GetEnd() - intersectPoint1).GetSafeNormal();
    }
    else if (intersectPoint1 == FourthWall->GetEnd())
    {
        OutInfo.RightWallDirection = (FourthWall->GetStart() - intersectPoint1).GetSafeNormal();
    }

    FVector Projection = FirstWall->GetStart();
    FArmyMath::GetLineSegmentProjectionPos(SecondWall->GetStart(), SecondWall->GetEnd(), Projection);
    float dist1 = (FirstWall->GetStart() - Projection).Size();
    Projection = ThirdWall->GetStart();
    FArmyMath::GetLineSegmentProjectionPos(FourthWall->GetStart(), FourthWall->GetEnd(), Projection);
    float dist2 = (ThirdWall->GetStart() - Projection).Size();

    FVector OutToInnearDirection = intersectPoint - intersectPoint1;

    if (FVector::DotProduct(OutInfo.LeftWallDirection, OutToInnearDirection) >= 0 && FVector::DotProduct(OutInfo.RightWallDirection, OutToInnearDirection) >= 0)
    {
        OutInfo.Pos = intersectPoint;
    }
    else
    {
        OutInfo.Pos = intersectPoint1;
    }
    float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(OutInfo.LeftWallDirection, OutInfo.RightWallDirection)));
    FVector tempEndPos = OutInfo.LeftWallDirection.RotateAngleAxis(-angle, FVector(0, 0, 1));
    if (FMath::IsNearlyEqual(FVector::DotProduct(tempEndPos, OutInfo.RightWallDirection), 1.0f, 0.01f))
    {

        OutInfo.LeftWallWidth = dist1;
        OutInfo.RightWallWidth = dist2;
        OutInfo.LeftWallDimPos = FindWallSidePoint(FirstWall, SecondWall, OutInfo.Pos);
        OutInfo.RightWallDimPos = FindWallSidePoint(ThirdWall, FourthWall, OutInfo.Pos);
    }
    else
    {
        FVector temp = OutInfo.LeftWallDirection;
        OutInfo.LeftWallDirection = OutInfo.RightWallDirection;
        OutInfo.RightWallDirection = temp;
        OutInfo.LeftWallWidth = dist2;
        OutInfo.RightWallWidth = dist1;
        OutInfo.LeftWallDimPos = FindWallSidePoint(ThirdWall, FourthWall, OutInfo.Pos);
        OutInfo.RightWallDimPos = FindWallSidePoint(FirstWall, SecondWall, OutInfo.Pos);
    }

    TArray< TSharedPtr<FArmyLine> > WallArray;
    WallArray.Add(FirstWall);
    WallArray.Add(SecondWall);
    WallArray.Add(ThirdWall);
    WallArray.Add(FourthWall);

    for (int32 i = 0; i < WallArray.Num(); ++i)
    {
        TSharedPtr<FArmyRoom> RelatedRoom = GetRelatedRoomByLine(WallArray[i]);
        if (RelatedRoom->GetType() == OT_InternalRoom)
        {
            if (!OutInfo.FirstLine.IsValid())
            {
                OutInfo.FirstLine = WallArray[i];
                continue;
            }
            if (!OutInfo.SecondLine.IsValid())
            {
                OutInfo.SecondLine = WallArray[i];
                break;
            }
        }
    }

    return true;
}

bool FArmyHardwareOperation::FindIntersectionPoint(TSharedPtr<FArmyLine> Wall1, TSharedPtr<FArmyLine> Wall2, TSharedPtr<FArmyLine> Wall3, FVector& Pos)
{
    Pos = Wall1->GetStart();
    if (Pos == Wall2->GetStart() || Pos == Wall2->GetEnd() || Pos == Wall3->GetStart() || Pos == Wall3->GetEnd())
    {
        return true;
    }
    Pos = Wall1->GetEnd();
    if (Pos == Wall2->GetStart() || Pos == Wall2->GetEnd() || Pos == Wall3->GetStart() || Pos == Wall3->GetEnd())
    {
        return true;
    }

    return false;
}

FVector FArmyHardwareOperation::FindWallSidePoint(TSharedPtr<FArmyLine> Wall1, TSharedPtr<FArmyLine> Wall2, FVector& Pos)
{
    FVector Result;
    if (Pos == Wall1->GetStart())
    {
        Result = Wall1->GetEnd();
    }
    if (Pos == Wall1->GetEnd())
    {
        Result = Wall1->GetStart();
    }
    if (Pos == Wall2->GetStart())
    {
        Result = Wall2->GetEnd();
    }
    if (Pos == Wall2->GetEnd())
    {
        Result = Wall2->GetStart();
    }

    return Result;
}

void FArmyHardwareOperation::SetWallCornerProperty(const FVector& Pos)
{
    TSharedPtr<FArmyCornerBayWindow> CornerBayWindow = StaticCastSharedPtr<FArmyCornerBayWindow>(TempHardware);
    if (CornerBayWindow.IsValid())
    {
        CornerBayWindow->SetProperty(
            Pos, 
            CaptureWallCornerInfo.LeftWallDirection, 
            CaptureWallCornerInfo.RightWallDirection, 
            CaptureWallCornerInfo.LeftWallWidth, 
            CaptureWallCornerInfo.RightWallWidth);
    }
}

void FArmyHardwareOperation::CaculateHardwareDirectionAndPos(const FVector& Pos)
{
	FVector direction;	
	if (CaptureWallInfo.FirstLine.IsValid()) 
	{
		direction = CaptureWallInfo.FirstLine->GetStart() - CaptureWallInfo.FirstLine->GetEnd();		
	}		
	else
	{
		direction = FVector(1, 0, 0);
	}		
	direction = direction.GetSafeNormal();
	direction = direction.RotateAngleAxis(90, FVector(0, 0, 1));
	TempHardware->SetPos(Pos);
	TempHardware->SetDirection(direction);
	TempHardware->SetWidth(CaptureWallInfo.Thickness);
}

TSharedPtr<FArmyRoom> FArmyHardwareOperation::GetRelatedRoomByLine(TSharedPtr<FArmyLine> CheckedLine, EModelType InModelType)
{
	TArray<TWeakPtr<FArmyObject>> InternalRoomList;
	TArray<TWeakPtr<FArmyObject>> OutRoomList;
	FArmySceneData::Get()->GetObjects(InModelType, OT_InternalRoom, InternalRoomList);
	FArmySceneData::Get()->GetObjects(InModelType, OT_OutRoom, OutRoomList);

	TSharedPtr<FArmyRoom> Room;
	for (auto It : InternalRoomList)
	{
		Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid() && Line == CheckedLine)
				{					
					return Room;
				}					
		}
	}

	for (auto It : OutRoomList)
	{
		Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid() && Line == CheckedLine)
				{					
					return Room;
				}
		}
	}
	return nullptr;
}

void FArmyHardwareOperation::SetHardwareLinkedInfo(TSharedPtr<FArmyHardware> Hardware)
{
    if (Hardware->GetType() == OT_CornerBayWindow)
    {
        if (Hardware->LinkFirstLine != CaptureWallCornerInfo.FirstLine)
        {
            Hardware->LinkFirstLine = CaptureWallCornerInfo.FirstLine;
            Hardware->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallCornerInfo.FirstLine, GetCurrentModel());
        }
        if (Hardware->LinkSecondLine != CaptureWallCornerInfo.SecondLine)
        {
            Hardware->LinkSecondLine = CaptureWallCornerInfo.SecondLine;
            Hardware->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallCornerInfo.SecondLine, GetCurrentModel());
        }
    }
    else
    {
        if (Hardware->LinkFirstLine != CaptureWallInfo.FirstLine)
        {
            Hardware->LinkFirstLine = CaptureWallInfo.FirstLine;
            Hardware->FirstRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.FirstLine, GetCurrentModel());
			Hardware->InWallType = (CaptureWallInfo.FirstOwner->GetType() == OT_AddWall || CaptureWallInfo.FirstOwner->GetType() == OT_IndependentWall) ? 1 : 0;
			
        }
        if (Hardware->LinkSecondLine != CaptureWallInfo.SecondLine)
        {
            Hardware->LinkSecondLine = CaptureWallInfo.SecondLine;
            Hardware->SecondRelatedRoom = GetRelatedRoomByLine(CaptureWallInfo.SecondLine, GetCurrentModel());
			Hardware->InWallType = (CaptureWallInfo.SecondOwner->GetType() == OT_AddWall || CaptureWallInfo.SecondOwner->GetType() == OT_IndependentWall) ? 1 : 0;
        }
    }
}