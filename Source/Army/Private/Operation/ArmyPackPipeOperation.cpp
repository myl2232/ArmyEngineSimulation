#include "ArmyPackPipeOperation.h"
#include "ArmyAddWall.h"
#include "ArmyMath.h"
#include "ArmyPolygon.h"
#include "ArmyRoom.h"
#include "ArmyEngine/Public/XREngineModule.h"
#include "ArmySceneData.h"
#include "ArmyToolsModule.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmyGameInstance.h"
#include "SArmyAddWallSetting.h"
#include "ArmyRulerLine.h"
#include "ArmyClipper.h"
#include "ArmyWallLine.h"
#include "ArmyIndependentWall.h"

FArmyPackPipeOperation::FArmyPackPipeOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
	, Polygon(MakeShareable(new FArmyPolygon()))
	, SolidLine(MakeShareable(new FArmyLine()))
{
	Polygon->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial()->GetRenderProxy(false);
	Polygon->SetZoomFactor(24.0f);

	SAssignNew(SettingWidget, SArmyAddWallSetting);
	SettingWidget->SetCatchThicknessValue(70);

	RulerLine = MakeShareable(new FArmyRulerLine());
	RulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyPackPipeOperation::OnLineInputBoxCommitted));
}

void FArmyPackPipeOperation::BeginOperation(XRArgument InArg)
{
	CurrentArg = InArg;

	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_VertexCaptrue;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
}

void FArmyPackPipeOperation::EndOperation()
{
	Exec_AddSpace(CachedPoints);
	Polygon->RemoveAllVertexes();
	CachedPoints.Empty();
	CachedDrawPoints.Empty();
	CachedLines.Empty();
	RulerLine->ShowInputBox(false);
	FArmyOperation::EndOperation();
	bOffsetGenerate = false;
	SolidLine->SetStart(FVector::ZeroVector);
	SolidLine->SetEnd(FVector::ZeroVector);
}

void FArmyPackPipeOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{			
	if (CachedPoints.Num() > 0)
	{
		SolidLine->Draw(PDI, View);
		Polygon->Draw(PDI, View);
		for (auto LineIt : CachedLines)
		{
			LineIt->Draw(PDI, View);
		}
		RulerLine->Draw(PDI, View);
	}
}

void FArmyPackPipeOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
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

	if (SolidLine.IsValid() && !(SolidLine->GetStart().Equals(SolidLine->GetEnd())))
	{
		RulerLine->Update(SolidLine->GetStart(), SolidLine->GetEnd());
		RulerLine->SetInputBoxFocus(true);
	}
}

void FArmyPackPipeOperation::GenerateWalls()
{
	if (CachedLines.Num() > 0)
	{				
		Exec_AddSpace(CachedPoints);		
		Polygon->RemoveAllVertexes();
	}
}

void FArmyPackPipeOperation::Tick()
{	
	FArmyOperation::Tick();
	if (CachedPoints.Num() > 0)
	{
		SolidLine->SetStart(CachedPoints.Last());
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		SolidLine->SetEnd(CapturePoint);
	}
}

bool FArmyPackPipeOperation::InputKey(UArmyEditorViewportClient * InViewPortClient, FViewport * InViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton &&Event == EInputEvent::IE_Pressed)
	{
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(CapturePoint));
		Exec_PushPoint(CapturePoint);
	}
	else if ((Key == EKeys::RightMouseButton || Key == EKeys::Escape) && Event == EInputEvent::IE_Pressed)
	{
		if (CachedPoints.Num() <= 2)
		{
			CachedPoints.Empty();
			EndOperation();
			return false;
		}
		else
		{
			//判断首末端点是否都在墙上
			bool isStartOnLine = false;
			bool isEndOnLine = false;
			TArray< TSharedPtr<FArmyLine> > OutLines;

			//@欧石楠 获取全部房间的线
			TArray<TWeakPtr<FArmyObject>> RoomList;
			FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
			for (auto It : RoomList)
			{
				TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
				if (CurrentRoom.IsValid())
				{
					TArray< TSharedPtr<FArmyLine> > TempOutLines;
					CurrentRoom->GetLines(TempOutLines);
					OutLines.Append(TempOutLines);					
				}
			}
			///*@欧石楠 获取全部柱子风道的线*/
			//TArray<TWeakPtr<FArmyObject>> ComponetList;
			//FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pillar, ComponetList);
			//FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AirFlue, ComponetList);
			//for (auto It : ComponetList)
			//{
			//	TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(It.Pin());
			//	if (CurrentComponent.IsValid())
			//	{
			//		TArray< TSharedPtr<FArmyLine> > TempOutLines;
			//		CurrentComponent->GetLines(TempOutLines);
			//		OutLines.Append(TempOutLines);
			//	}
			//}
			///*@欧石楠 获取全部新建墙的线*/
			//TArray<TWeakPtr<FArmyObject>> AddwallList;
			//FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddwallList);
			//for (auto It : AddwallList)
			//{
			//	TSharedPtr<FArmyAddWall> CurrentAddwall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
			//	if (CurrentAddwall.IsValid())
			//	{
			//		TArray< TSharedPtr<FArmyLine> > TempOutLines;
			//		CurrentAddwall->GetLines(TempOutLines);
			//		OutLines.Append(TempOutLines);
			//	}				
			//}

			///*@欧石楠 获取全部包立管的线*/
			//TArray<TWeakPtr<FArmyObject>> PackPipeList;
			//FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_PackPipe, PackPipeList);
			//for (auto It : PackPipeList)
			//{
			//	TSharedPtr<FArmyPackPipe> CurrentPackPipe = StaticCastSharedPtr<FArmyPackPipe>(It.Pin());
			//	if (CurrentPackPipe.IsValid())
			//	{
			//		TArray< TSharedPtr<FArmyLine> > TempOutLines;
			//		CurrentPackPipe->GetLines(TempOutLines);
			//		OutLines.Append(TempOutLines);
			//	}
			//}

			//@欧石楠 对全部线段进行判断，首末端点是否都在线段上
			for (auto ItLine : OutLines)
			{
				if (FArmyMath::IsPointOnLine(CachedPoints[0], ItLine->GetStart(), ItLine->GetEnd()))
				{
					isStartOnLine = true;
				}
				if (FArmyMath::IsPointOnLine(CachedPoints[CachedPoints.Num() - 1], ItLine->GetStart(), ItLine->GetEnd()))
				{
					isEndOnLine = true;
				}

				if (isStartOnLine && isEndOnLine)
					break;
			}

			if (!isStartOnLine || !isEndOnLine)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("包立管必须贴着墙壁"));
				CachedPoints.Empty();
				EndOperation();
				return false;
			}
		}

		GGI->Window->PresentModalDialog(
            TEXT("包立管设置"),
            SettingWidget->AsShared(),
            FSimpleDelegate::CreateRaw(this, &FArmyPackPipeOperation::OnConfirmAddWallClicked),
            FSimpleDelegate(),
			FSimpleDelegate::CreateRaw(this, &FArmyPackPipeOperation::OnCloseAddWallClicked),
            true);

		return true;
	}

	return false;
}

bool FArmyPackPipeOperation::IsOperating()
{
	return false;
}

void FArmyPackPipeOperation::Exec_AddSpace(TArray<FVector>& Vertexes)
{
    if (Vertexes.Num() == 0)
    {
		return;
    }
	float CurrentLength = 0.f;
	for (int i = 0; i < InsideVertices.Num(); i++)
	{
		if (i + 1 == InsideVertices.Num())
			break;
		CurrentLength += (InsideVertices[i] - InsideVertices[i + 1]).Size();
	}
	CurrentLength += (InsideVertices.Num() - 2) * ExtrusionThickness;

	TSharedPtr<FArmyPackPipe> ResultSpace = MakeShareable(new FArmyPackPipe());
	ResultSpace->SetThickness(ExtrusionThickness);
	ResultSpace->SetLength(CurrentLength);
	ResultSpace->bModified = true;
	ResultSpace->SetVertexes(Vertexes);
	ResultSpace->SetInsideVertices(InsideVertices);
	ResultSpace->SetOutsideVertices(OutsideVertices);

	/** @欧石楠 这里计算出起止点之间的线，用于额外绘制*/
	TArray<TWeakPtr<FArmyObject>> RoomList;
	TArray< TSharedPtr<FArmyLine> > OutLines;
	FVector CenterPoint = FBox(CachedDrawPoints).GetCenter();
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (CurrentRoom.IsValid())
		{
			TArray< TSharedPtr<FArmyLine> > TempOutLines;
			TArray<FVector> TempPoints;
			CurrentRoom->GetVertexes(TempPoints);			
			//找到这个包立管数据所在的房间，获取该房间的全部线段
			if (FArmyMath::IsPointInOrOnPolygon2D(CenterPoint, TempPoints))
			{
				CurrentRoom->GetLines(TempOutLines);
				OutLines.Append(TempOutLines);
				break;
			}			
		}
	}
	//找到绘制包立管时两个连接墙体的点分别连接在哪条线上
	TSharedPtr<FArmyLine> FirstLine = nullptr;
	TSharedPtr<FArmyLine> SecondLine = nullptr;
	for (auto ItLine : OutLines)
	{
		if (FArmyMath::IsPointOnLine(CachedDrawPoints[0], ItLine->GetStart(), ItLine->GetEnd()))
		{
			FirstLine = ItLine;			
		}
		if (FArmyMath::IsPointOnLine(CachedDrawPoints[CachedDrawPoints.Num() - 1], ItLine->GetStart(), ItLine->GetEnd()))
		{
			SecondLine = ItLine;			
		}
		if (FirstLine.IsValid() && SecondLine.IsValid())
		{
			break;
		}
	}
	//此时有两种情况，两个点在同一条线或者两个点在两条相交的线
	if (FirstLine.IsValid() && SecondLine.IsValid()) 
	{
		TArray<TSharedPtr<FArmyLine>> ExternalLines;
		if (FirstLine == SecondLine)
		{
			TSharedPtr<FArmyLine> TempLine = MakeShareable(new FArmyLine(CachedDrawPoints[0], CachedDrawPoints[CachedDrawPoints.Num() - 1]));
			ExternalLines.Add(TempLine);
		}
		else if (FirstLine->GetStart().Equals(SecondLine->GetStart()) || FirstLine->GetStart().Equals(SecondLine->GetEnd()))
		{
			TSharedPtr<FArmyLine> TempLine1 = MakeShareable(new FArmyLine(CachedDrawPoints[0], FirstLine->GetStart()));
			TSharedPtr<FArmyLine> TempLine2 = MakeShareable(new FArmyLine(FirstLine->GetStart(), CachedDrawPoints[CachedDrawPoints.Num() - 1]));
			ExternalLines.Add(TempLine1);
			ExternalLines.Add(TempLine2);
		}
		else if (FirstLine->GetEnd().Equals(SecondLine->GetStart()) || FirstLine->GetEnd().Equals(SecondLine->GetEnd()))
		{
			TSharedPtr<FArmyLine> TempLine1 = MakeShareable(new FArmyLine(CachedDrawPoints[0], FirstLine->GetEnd()));
			TSharedPtr<FArmyLine> TempLine2 = MakeShareable(new FArmyLine(FirstLine->GetEnd(), CachedDrawPoints[CachedDrawPoints.Num() - 1]));
			ExternalLines.Add(TempLine1);
			ExternalLines.Add(TempLine2);
		}
		ResultSpace->SetExternalLines(ExternalLines);
	}


	TSharedPtr<FArmyObject> Object = ResultSpace;    

	FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

	XRArgument Arg = XRArgument().ArgFName(FName(*FString(TEXT("基础类")))).ArgString(Object->GetName()).ArgUint32(E_ModifyModel);
	FArmySceneData::Get()->Add(Object, Arg);
}

FVector FArmyPackPipeOperation::Exec_PopPoint()
{
	FVector result = FVector::ZeroVector;
	if (CachedPoints.Num() > 0)
	{
		result = CachedPoints.Pop();
		if (CachedPoints.Num() > 0)
		{
			FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(CachedPoints.Last()));
		}
	}
	if (CachedLines.Num() > 0)
	{
		CachedLines.Pop();
	}

	Polygon->RemoveVertice(result);

	return result;
}



void FArmyPackPipeOperation::Exec_PushPoint(const FVector& Pos)
{
	TArray<TWeakPtr<FArmyObject>> OutRoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_OutRoom, OutRoomList);
	for (auto It : OutRoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (CurrentRoom.IsValid())
		{
			if (!FArmyMath::IsPointInOrOnPolygon2D(Pos, CurrentRoom->GetWorldPoints(true)))
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("包立管不允许绘制在户型外"));
				return;
			}
		}
	}

	if (CachedPoints.Num() > 0)
	{
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, AddWallList);
		for (auto It : AddWallList)
		{
			TSharedPtr<FArmyObject> AddWall = StaticCastSharedPtr<FArmyObject>(It.Pin());
			if (AddWall.IsValid())
			{
				TArray<TSharedPtr<FArmyLine>> AddWallLines;
				AddWall->GetLines(AddWallLines);
				for (auto It : CachedLines)
					AddWallLines.Push(It);
				
				for (auto ItLine : AddWallLines)
				{
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						Pos, CachedPoints.Last(), CurrentIntersect))
					{
						GGI->Window->ShowMessage(MT_Warning, TEXT("包立管不允许交叉绘制"));
						return;
					}
				}
			}
		}
	}

	//再判断新建墙是否在墙体内,并不在拆除墙范围内，则错误提示
	TArray<FVector> CurrentPoints = CachedPoints;
	CurrentPoints.Push(Pos);
	FBox CurrentAddWallBox(CurrentPoints);
	FVector BoxCenter = CurrentAddWallBox.GetCenter();
	bool bInRoom = false;
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (CurrentRoom.IsValid())
		{
			if (FArmyMath::IsPointInOrOnPolygon2D(Pos, CurrentRoom->GetWorldPoints(true))
				&& FArmyMath::IsPointInOrOnPolygon2D(BoxCenter, CurrentRoom->GetWorldPoints(true)))
			{
				bInRoom = true;
				break;
			}
		}
	}
	if (!bInRoom)
	{
		bool bInModify = false;
		TArray<TWeakPtr<FArmyObject>> ModifyWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);
		for (auto It : ModifyWallList)
		{
			TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
			if (ModifyWall.IsValid())
			{
				TArray<FVector> ModifyPoints;
				ModifyWall->GetVertexes(ModifyPoints);
				if (FArmyMath::IsPointInOrOnPolygon2D(Pos, ModifyPoints)
					&& FArmyMath::IsPointInOrOnPolygon2D(BoxCenter, ModifyPoints))
				{
					bInModify = true;
					break;
				}
			}
		}

		if (!bInModify)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("包立管不允许在原始墙体内"));
			return;
		}
	}

	TSharedPtr<FArmyEditPoint> CapturePointPtr = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePointPtr();
	if (CapturePointPtr.IsValid())
	{
		if (CachedPoints.Num() >= 2 
			&& CachedPoints[0].Equals(CapturePointPtr->GetPos()))
		{
			CachedPoints.Empty();
			GGI->Window->ShowMessage(MT_Warning, TEXT("包立管不允许自身封闭"));
			EndOperation();
			return;
		}
		else
		{
			CachedPoints.Add(CapturePointPtr->GetPos());
			CachedDrawPoints.Add(CapturePointPtr->GetPos());
			TSharedPtr<FArmyLine> NewLine = NULL;
			if (CachedPoints.Num() >= 2)
			{
				NewLine = MakeShareable(new FArmyLine(CachedPoints.Last(1), CachedPoints.Last()));
				CachedLines.Push(NewLine);
			}
		}
	}
	else
	{
		CachedPoints.Add(Pos);
		CachedDrawPoints.Add(Pos);
		
		if (CachedPoints.Num() >= 2)
		{
			TSharedPtr<FArmyLine> NewLine = MakeShareable(new FArmyLine(CachedPoints.Last(1), CachedPoints.Last()));
			CachedLines.Push(NewLine);
		}
	}
}

bool FArmyPackPipeOperation::GenerateExtrutionPoints(TArray<FVector>& InPoints)
{
	if (InPoints.Num() < 2)
		return false;

	FVector StartNormal = (InPoints[1] - InPoints[0]).GetSafeNormal() ^ FVector(0, 0, 1);
	StartNormal *= bInternalExtrusion ? 1 : -1;
	FVector EndNormal = (InPoints[InPoints.Num() - 1] - InPoints[InPoints.Num() - 2]).GetSafeNormal() ^ FVector(0, 0, 1);
	EndNormal *= bInternalExtrusion ? 1 : -1;
	FVector StartOffsetPoint = InPoints[0] + StartNormal * ExtrusionThickness;
	FVector EndOffsetPoint;
	if (InPoints.Num() == 2)
	{
		//@ 计算新建墙是否在房间内，此处解决起止点在墙线上的临界条件
		TArray<TWeakPtr<FArmyObject> > WallList;
		FArmySceneData::Get()->GetObjects(EModelType::E_ModifyModel,OT_InternalRoom, WallList);
		EndOffsetPoint = InPoints[1] + EndNormal * ExtrusionThickness;
		FVector TestPoint = (InPoints[0] + InPoints[1]) / 2 + EndNormal;//测试偏移点是否符合
		bool InRoom = false;
		for (auto & W : WallList)
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(W.Pin());
			if (Room.IsValid() && Room->IsPointInRoom(TestPoint))
			{
				InRoom = true;
				break;
			}
		}
		if (!InRoom)
		{
			//再判断点是否在删除墙体区域内
			bool bInModify = false;
			TArray<TWeakPtr<FArmyObject>> ModifyWallList;
			FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);
			for (auto It : ModifyWallList)
			{
				TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
				if (ModifyWall.IsValid())
				{
					TArray<FVector> ModifyPoints;
					ModifyWall->GetVertexes(ModifyPoints);
					if (FArmyMath::IsPointInOrOnPolygon2D(TestPoint, ModifyPoints))
					{
						bInModify = true;
						break;
					}
				}
			}

			if (!bInModify)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("包立管不允许在原始墙体内"));
				return false;
			}
		}

		InPoints.Push(EndOffsetPoint);
		InPoints.Push(StartOffsetPoint);
	}
	else
	{
		EndOffsetPoint = InPoints[InPoints.Num() - 1] + EndNormal * ExtrusionThickness;
		TArray<FVector> ExtursionPoints = FArmyMath::Extrude3D(InPoints, -ExtrusionThickness * (bInternalExtrusion ? 1 : -1));
		ExtursionPoints.RemoveAt(0);
		ExtursionPoints.RemoveAt(ExtursionPoints.Num() - 1);
		FArmyMath::ReversePointList(ExtursionPoints);

		TArray<FVector> OtherVertices;
		OtherVertices.Push(EndOffsetPoint);
		OtherVertices.Insert(ExtursionPoints, 1);
		OtherVertices.Push(StartOffsetPoint);

		if (FArmyMath::IsClockWise(InPoints))
		{
			if (bInternalExtrusion)
			{
				InsideVertices = InPoints;
				OutsideVertices = OtherVertices;
			}
			else
			{
				OutsideVertices = InPoints;
				InsideVertices = OtherVertices;
			}
		}
		else
		{
			if (!bInternalExtrusion)
			{
				InsideVertices = InPoints;
				OutsideVertices = OtherVertices;
			}
			else
			{
				OutsideVertices = InPoints;
				InsideVertices = OtherVertices;
			}
		}

		InPoints.Push(EndOffsetPoint);
		InPoints.Insert(ExtursionPoints, InPoints.Num());
		InPoints.Push(StartOffsetPoint);
	}

	return true;
}

void FArmyPackPipeOperation::OnConfirmAddWallClicked()
{
	bOffsetGenerate = true;
	bInternalExtrusion = SettingWidget->GetIfCheck();
	ExtrusionThickness = FCString::Atof(*SettingWidget->GetThicknessValue().ToString()) / 10.f;
	GenerateExtrutionPoints(CachedPoints);
	GGI->Window->DismissModalDialog();
	EndOperation();
}

void FArmyPackPipeOperation::OnCloseAddWallClicked()
{
	GGI->Window->DismissModalDialog();
	CachedPoints.Empty();
	EndOperation();
}

void FArmyPackPipeOperation::OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		/** 添加一条固定长度的直线 */
		const int32 LineLength = FCString::Atoi(*InText.ToString());
		if (LineLength == 0)
			return;

		SCOPE_TRANSACTION(TEXT("移动线"));

		FVector LineDirection = FArmyMath::GetLineDirection(SolidLine->GetStart(), SolidLine->GetEnd());

		FVector Offset = (LineLength / 10.f) * LineDirection;
		FVector LineEndPos = SolidLine->GetStart() + Offset;

		CachedPoints.Add(LineEndPos);
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(LineEndPos));

		TSharedPtr<FArmyLine> NewLine = NULL;
		if (CachedPoints.Num() >= 2)
		{
			NewLine = MakeShareable(new FArmyLine(CachedPoints.Last(1), CachedPoints.Last()));
			CachedLines.Push(NewLine);
		}
	}
}
