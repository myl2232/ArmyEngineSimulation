#include "ArmySimpleComponentOperation.h"
#include "ArmyRoom.h"
#include "ArmySimpleComponent.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"
#include "ArmyGameInstance.h"
#include "ArmyMath.h"
#include "ArmyBeam.h"
#include "ArmyPillar.h"
#include "ArmyAirFlue.h"

FArmySimpleComponentOperation::FArmySimpleComponentOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
{
}

void FArmySimpleComponentOperation::BeginOperation(XRArgument InArg)
{
	ObjType = (EObjectType)InArg._ArgInt32;
	switch (ObjType)
	{
		case OT_None:
			TempComponent = StaticCastSharedPtr<FArmySimpleComponent>(FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects()[0].Pin());
			bFirstAdd = false;
			break;
		case OT_Beam:
			TempComponent = MakeShareable(new FArmyBeam());
			break;
		case OT_Pillar:
			TempComponent = MakeShareable(new FArmyPillar());
			break;
		case OT_AirFlue:
			TempComponent = MakeShareable(new FArmyAirFlue());
			break;
		default:
			return;
	}

	TempComponent->SetState(OS_Selected);

	FArmyToolsModule::Get().GetRectSelectTool()->End();
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);	
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());		
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Rooms.Add(Room);
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid())
					Lines.Push(Line);
		}
	}
}

void FArmySimpleComponentOperation::EndOperation()
{
	TempComponent->DeselectPoints();
	TempComponent = nullptr;
	Lines.Empty();
	Rooms.Empty();
	bFirstAdd = true;

	FArmyOperation::EndOperation();
}

void FArmySimpleComponentOperation::SetSelected(FObjectPtr Object, TSharedPtr<FArmyPrimitive> InOperationPoint)
{
}

void FArmySimpleComponentOperation::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (TempComponent.IsValid())
	{
		TempComponent->Draw(PDI, View);
	}
}

bool FArmySimpleComponentOperation::InputKey(UArmyEditorViewportClient * InViewPortClient, FViewport * InViewPort, FKey Key, EInputEvent Event)
{
	if (ObjType != OT_None) // 添加操作
	{
		if (InViewPort->KeyState(EKeys::LeftMouseButton))
		{
			if (bFirstAdd)
			{			
				SCOPE_TRANSACTION(TEXT("添加结构部件"));
				TSharedPtr<FArmySimpleComponent> SimpleComponent = AddSimpleComponent(TempComponent);
				if (!SimpleComponent.IsValid())
					return false;				
			}
		}
		else if (Key == EKeys::RightMouseButton || Key == EKeys::Escape)
		{
			EndOperation();
		}
	}
	else // 拖拽操作
	{
		/*这里可以统一，具体的逻辑判断都在drag里做的**/
		if (Event == IE_Pressed)
		{
			DragPrePos = FVector(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
			GVC->GetMousePosition(MousePreDragPos);

			FVector2D MousePos;
			GVC->GetMousePosition(MousePos);
			FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
			WorldPos.Z = 0;
			TSharedPtr<FArmyRoom> TempRoom = nullptr;
			for (auto It : Rooms)
			{
				if (It.Pin()->IsPointInRoom(WorldPos) && It.Pin()->IsPointInRoom(TempComponent->GetPos()))
				{					
					TempRoom = It.Pin();
					break;
				}				
			}
			TempComponent->SetRelatedRoom(TempRoom);
		}
		else if (Event == IE_Released)
		{			

			if (TempComponent->GetOperationPointType() == ESCS_CENTER)
			{
				TempComponent->SetPos((TempComponent->GetLeftTopPos() + TempComponent->GetRightTopPos() + TempComponent->GetLeftBottomPos() + TempComponent->GetRightBottomPos()) / 4.0f);
			}						
			TempComponent->SetOperationPointType(ESCS_NONE);						

			FVector2D TempPoint;
			GVC->GetMousePosition(TempPoint);
			if (MousePreDragPos != TempPoint)
			{
				SCOPE_TRANSACTION(TEXT("拖拽修改梁"));
				TempComponent->Modify();
			}

			if (TempComponent->GetRelatedRoom().IsValid())
				TempComponent->GetRelatedRoom().Pin()->bPreModified = true;

			EndOperation();	
		}
	}
	return false;
}

void FArmySimpleComponentOperation::MouseMove(UArmyEditorViewportClient * InViewPortClient, FViewport * ViewPort, int32 X, int32 Y)
{
	if (ObjType == OT_None)
		return;	

	static int32 OldX, OldY;
	if (OldX != X || OldY != Y)
	{
		OldX = X;
		OldY = Y;
	}
	else
	{
		return;
	}

	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	WorldPos.Z = 0;

	/**如果移动过快导致控件关联的房间与鼠标所在的房间不一致，则进行标记*/
	bool bShouldReCalcRoom = false;
	if (TempComponent->GetRelatedRoom().IsValid())
	{
		for (auto It : Rooms)
		{
			if (It.Pin()->IsPointInRoom(WorldPos) && It.Pin() != TempComponent->GetRelatedRoom())
			{
				bShouldReCalcRoom = true;
				break;
			}
		}
	}	

	if (bFirstAdd && (!TempComponent->GetRelatedRoom().IsValid() || bShouldReCalcRoom))
	{
		TempComponent->SetPos(WorldPos);
		CaculateDirectionAndPos(WorldPos);
		CaptureLine(WorldPos);
	}
	else
	{
		if (CaptureLine(WorldPos))
		{
			CaculateDirectionAndPos(WorldPos);
		}
	}	
}

void FArmySimpleComponentOperation::MouseDrag(UArmyEditorViewportClient * InViewPortClient, FViewport * ViewPort, FKey Key)
{
	if (ObjType != OT_None)
		return;

	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	FVector WorldPos = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
	WorldPos.Z = 0;

	TempComponent->bModified = true;
	if (TempComponent->GetOperationPointType() == ESCS_CENTER)
	{
		if (CaptureLine(WorldPos))
		{
			CaculateDirectionAndPos(WorldPos);
			TempComponent->AutoAttach();
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		}		
	}
	/*@欧石楠拖拽缩放点**/
	else if (TempComponent->GetOperationPointType() == ESCS_TOP)
	{		
		TempComponent->UpdateTopPoint(WorldPos);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
	else if (TempComponent->GetOperationPointType() == ESCS_BOTTOM)
	{		
		TempComponent->UpdateBottomPoint(WorldPos);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
	else if (TempComponent->GetOperationPointType() == ESCS_LEFT)
	{		
		TempComponent->UpdateLeftPoint(WorldPos);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
	else if (TempComponent->GetOperationPointType() == ESCS_RIGHT)
	{		
		TempComponent->UpdateRightPoint(WorldPos);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
	else if (TempComponent->GetOperationPointType() == ESCS_DIR)
	{
		
	}
}

TSharedPtr<FArmySimpleComponent> FArmySimpleComponentOperation::AddSimpleComponent(TSharedPtr<FArmySimpleComponent> ResultComponent)
{
	if (!ResultComponent->GetRelatedRoom().IsValid())
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("构件必须放置在房间内!"));
		return nullptr;
	}	
	ResultComponent->AutoAttach();
	TSharedPtr<FArmySimpleComponent> Result = NULL;
	if (ResultComponent->GetType() == OT_Beam)
	{
		TSharedPtr<FArmyBeam> TempComponent = StaticCastSharedPtr<FArmyBeam>(ResultComponent);
		Result = MakeShareable(new FArmyBeam(TempComponent.Get()));
	}
	else if (ResultComponent->GetType() == OT_Pillar)
	{		
		TSharedPtr<FArmyPillar> TempComponent = StaticCastSharedPtr<FArmyPillar>(ResultComponent);
		Result = MakeShareable(new FArmyPillar(TempComponent.Get()));
	}
	else if (ResultComponent->GetType() == OT_AirFlue)
	{		
		TSharedPtr<FArmyAirFlue> TempComponent = StaticCastSharedPtr<FArmyAirFlue>(ResultComponent);
		Result = MakeShareable(new FArmyAirFlue(TempComponent.Get()));
	}

	FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	Result->SetState(OS_Normal);	
	Result->SetPos((Result->GetLeftTopPos() + Result->GetRightTopPos() + Result->GetLeftBottomPos() + Result->GetRightBottomPos()) / 4.0f);
	FArmySceneData::Get()->Add(Result, XRArgument(1).ArgUint32(E_HomeModel).ArgFName(FName(TEXT("结构部件"))));
	if (Result->GetRelatedRoom().IsValid())
		Result->GetRelatedRoom().Pin()->bPreModified = true;

	return Result;
}

bool FArmySimpleComponentOperation::IsOperating()
{
	return OperationPoint.IsValid();
}

bool FArmySimpleComponentOperation::CaptureLine(const FVector & Pos)
{
	float MaxDistance = TempComponent->GetWidth() * 1.f / 2.f;
	float MinDistance = MaxDistance;
	FVector TempDir(1, 0, 0);
	bool bShouldStopAuto = false;	
	FVector ProjectionPos = FVector::ZeroVector;
	for (auto Object : Lines)
	{
		if (!Object.IsValid()) continue;
		TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());
		FVector P0 = Wall->GetStart();
		FVector P1 = Wall->GetEnd();

		FVector Projection = Pos;
		if (FArmyMath::GetLineSegmentProjectionPos(P0, P1, Projection))
		{
			float Distance = FVector::Distance(Pos, Projection);
			if (Distance > 0.f && Distance < MinDistance)
			{				
				MinDistance = Distance;
				if (TempComponent->GetRelatedRoom().IsValid() && TempComponent->GetAutoAdaptDir() && TempComponent->GetRelatedRoom().Pin()->CheckHasLine(Wall))
				{
					TempDir = Object.Pin()->GetDirectionNormal();
					TempComponent->SetDirection(TempDir);
					ProjectionPos = Projection;
				}
			}				
		}
	}			
	if (ProjectionPos == FVector::ZeroVector)
	{
		ProjectionPos = Pos;
	}
	else
	{
		ProjectionPos += TempComponent->GetDirection().RotateAngleAxis(90, FVector(0, 0, 1)) * TempComponent->GetWidth() / 2.f;
		if (!TempComponent->CheckAllPointsInRoom(ProjectionPos))
		{
			ProjectionPos -= 2 * TempComponent->GetDirection().RotateAngleAxis(90, FVector(0, 0, 1)) * TempComponent->GetWidth() / 2.f;
		}
	}	
	if (TempComponent->CheckAllPointsInRoom(ProjectionPos))
	{
		TempComponent->SetPos(ProjectionPos);
		return true;
	}	
	return false;
}

void FArmySimpleComponentOperation::CaculateDirectionAndPos(const FVector & Pos)
{
	for (auto It : Rooms)
	{
		if (It.Pin()->IsPointInRoom(Pos) && It.Pin()->IsPointInRoom(TempComponent->GetPos()))
		{
			/*if (!TempComponent->GetRelatedRoom().IsValid())
			{
				TempComponent->HideRuleLineInputBox(false);
			}*/
			TempComponent->SetRelatedRoom(It);					
			if (It.IsValid())
			{			
				Lines.Empty();
				TArray<TSharedPtr<FArmyLine>> RoomLines;
				It.Pin()->GetLines(RoomLines);
				for (auto Line : RoomLines)
					if (Line.IsValid())
						Lines.Push(Line);
			}
			break;
		}
		if (bFirstAdd)
		{
			if (TempComponent->GetRelatedRoom().IsValid())
			{
				TempComponent->HideRuleLineInputBox(true);
			}
			TempComponent->SetRelatedRoom(nullptr);			
		}		
	}	
	if (!TempComponent->GetRelatedRoom().IsValid() || !TempComponent->GetAutoAdapt())
	{
		return;
	}			
	TempComponent->AutoAdapt();
}
