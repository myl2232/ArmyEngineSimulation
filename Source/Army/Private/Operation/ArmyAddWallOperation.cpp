#include "ArmyAddWallOperation.h"
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

FArmyAddWallOperation::FArmyAddWallOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
	, Polygon(MakeShareable(new FArmyPolygon()))
	, SolidLine(MakeShareable(new FArmyLine()))
{
	Polygon->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial()->GetRenderProxy(false);
	Polygon->SetZoomFactor(24.0f);

	SAssignNew(SettingWidget, SArmyAddWallSetting).bNeedHeight(true);

	RulerLine = MakeShareable(new FArmyRulerLine());
	RulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyAddWallOperation::OnLineInputBoxCommitted));
}

void FArmyAddWallOperation::BeginOperation(XRArgument InArg)
{
	CurrentArg = InArg;

	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_VertexCaptrue;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
}

void FArmyAddWallOperation::EndOperation()
{
	if (bCreateWall)
	{
		Exec_AddSpace(CachedPoints);
	}
	Polygon->RemoveAllVertexes();
	CachedPoints.Empty();
	CachedLines.Empty();
	CachedOriginPoints.Empty();
	RulerLine->ShowInputBox(false);
	RulerLine->Update(FVector::ZeroVector, FVector::ZeroVector);
	FArmyOperation::EndOperation();
	bOffsetGenerate = false;
	SolidLine->SetStart(FVector::ZeroVector);
	SolidLine->SetEnd(FVector::ZeroVector);
}

void FArmyAddWallOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{			
	if (CachedPoints.Num() > 0)
	{
		SolidLine->Draw(PDI, View);
		Polygon->Draw(PDI, View);
		for (auto LineIt : CachedLines)
		{
			LineIt->Draw(PDI, View);
		}
		if (SolidLine.IsValid() && !(SolidLine->GetStart().Equals(SolidLine->GetEnd())) && RulerLine->GetInputBoxCachedLength() > 1)
		{
			RulerLine->Draw(PDI, View);
		}		
	}
}

void FArmyAddWallOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
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

void FArmyAddWallOperation::GenerateWalls()
{
	if (CachedLines.Num() > 0)
	{				
		Exec_AddSpace(CachedPoints);		
		Polygon->RemoveAllVertexes();
	}
}

void FArmyAddWallOperation::Tick()
{	
	FArmyOperation::Tick();
	if (CachedPoints.Num() > 0)
	{
		SolidLine->SetStart(CachedPoints.Last());
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		SolidLine->SetEnd(CapturePoint);
	}
}

bool FArmyAddWallOperation::InputKey(UArmyEditorViewportClient * InViewPortClient, FViewport * InViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton &&Event == EInputEvent::IE_Pressed)
	{
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(CapturePoint));
		Exec_PushPoint(CapturePoint);
	}
	else if ((Key == EKeys::RightMouseButton || Key == EKeys::Escape) && Event == EInputEvent::IE_Pressed)
	{
		if (CachedPoints.Num() < 2)
		{
			CachedPoints.Empty();
			EndOperation();
			return false;
		}
		GGI->Window->PresentModalDialog(
            TEXT("新建墙设置"),
            SettingWidget->AsShared(),
            FSimpleDelegate::CreateRaw(this, &FArmyAddWallOperation::OnConfirmAddWallClicked),
            FSimpleDelegate(),
			FSimpleDelegate::CreateRaw(this, &FArmyAddWallOperation::OnCloseAddWallClicked),
            true);

		return true;
	}

	return false;
}

bool FArmyAddWallOperation::IsOperating()
{
	return false;
}



void FArmyAddWallOperation::Exec_AddSpace(TArray<FVector>& Vertexes)
{
    if (Vertexes.Num() == 0)
    {
		return;
    }

	TArray<TWeakPtr<FArmyObject>> ModifyWallList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);
	//首先判断是否是独立墙
	FVector BoxCenter = FBox(Vertexes).GetCenter();
	bool isIndepedentWall = true;
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (CurrentRoom.IsValid() && FArmyMath::IsPointInOrOnPolygon2D(BoxCenter, CurrentRoom->GetWorldPoints(true)))
		{
			TArray< TSharedPtr<FArmyLine> > OutLines;
			CurrentRoom->GetLines(OutLines);
			for (auto ItLine : OutLines)
			{
				for (auto& ItPoint : Vertexes)
				{
					if (FArmyMath::IsPointOnLine(ItPoint, ItLine->GetStart(), ItLine->GetEnd()))
					{
						isIndepedentWall = false;
						break;
					}
				}
				if (!isIndepedentWall)
					break;
			}
		}
		if (!isIndepedentWall)
			break;
	}
	//在判断是否在拆改墙范围内，若在则认为是新建墙
	if (isIndepedentWall)
	{
		for (auto It : ModifyWallList)
		{
			TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
			TArray<FVector> ModifyVertices = ModifyWall->Polygon->Vertices;
			if (FArmyMath::IsPointInPolygon2D(BoxCenter, ModifyVertices))
			{
				isIndepedentWall = false;
				break;
			}
		}
		//@欧石楠 判断一下是否接着新建墙、柱子、风道和包立管绘制，如果是则认为是新建墙
		/*TArray< TSharedPtr<FArmyLine> > OutLines;
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		for (auto It : AddWallList)
		{
			TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
			TArray< TSharedPtr<FArmyLine> > TempOutLines;
			AddWall->GetLines(TempOutLines);
			OutLines.Append(TempOutLines);
		}
		TArray<TWeakPtr<FArmyObject>> ComponentList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pillar, ComponentList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AirFlue, ComponentList);
		for (auto It : ComponentList)
		{
			TSharedPtr<FArmySimpleComponent> CurrentComponent = StaticCastSharedPtr<FArmySimpleComponent>(It.Pin());
			if (CurrentComponent->IsAttachedToRoom())
			{
				TArray< TSharedPtr<FArmyLine> > TempOutLines;
				CurrentComponent->GetLines(TempOutLines);
				OutLines.Append(TempOutLines);
			}
		}
		TArray<TWeakPtr<FArmyObject>> PackPipeList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_PackPipe, PackPipeList);
		for (auto It : PackPipeList)
		{
			TSharedPtr<FArmyPackPipe> CurrentComponent = StaticCastSharedPtr<FArmyPackPipe>(It.Pin());
			TArray< TSharedPtr<FArmyLine> > TempOutLines;
			CurrentComponent->GetLines(TempOutLines);
			OutLines.Append(TempOutLines);
		}
		for (auto ItLine : OutLines)
		{
			for (auto& ItPoint : Vertexes)
			{
				if (FArmyMath::IsPointOnLine(ItPoint, ItLine->GetStart(), ItLine->GetEnd()))
				{
					isIndepedentWall = false;
					break;
				}
			}
			if (!isIndepedentWall)
				break;
		}*/
	}

	TSharedPtr<FArmyObject> Object = nullptr;
	if (isIndepedentWall)
	{
		TSharedPtr<FArmyIndependentWall> ResultSpace = MakeShareable(new FArmyIndependentWall());
		ResultSpace->bModified = true;
		ResultSpace->SetInternalExtrusion(bInternalExtrusion);
		ResultSpace->SetVertexes(Vertexes);
		ResultSpace->SetCachedPoints(CachedOriginPoints);
		ResultSpace->Polygon->SetZoomFactor(24.0f);
		ResultSpace->SetWidth(SettingWidget->GetCachedThicknessValue() / 10.f);
		/** @欧石楠 设置新建墙体高度，如果低于全局墙高则默认为半墙*/
		ResultSpace->SetHeight(SettingWidget->GetCachedHeightValue() / 10.f);
		if (SettingWidget->GetCachedHeightValue() / 10.f < FArmySceneData::WallHeight)
		{
			ResultSpace->SetIsHalfWall(true);
		}

		Object = ResultSpace;
	}
	else
	{
		TSharedPtr<FArmyAddWall> ResultAddWall = MakeShareable(new FArmyAddWall());
        ResultAddWall->bModified = true;
        ResultAddWall->bOffsetGenerate = bOffsetGenerate;
		ResultAddWall->SetInternalExtrusion(bInternalExtrusion);
        ResultAddWall->SetVertexes(Vertexes);
		ResultAddWall->SetCachedPoints(CachedOriginPoints);
        ResultAddWall->Polygon->SetZoomFactor(24.0f);
        ResultAddWall->SetWidth(SettingWidget->GetCachedThicknessValue() / 10.f);
		/** @欧石楠 设置新建墙体高度，如果低于全局墙高则默认为半墙*/
		ResultAddWall->SetHeight(SettingWidget->GetCachedHeightValue() / 10.f);
		if (SettingWidget->GetCachedHeightValue() / 10.f < FArmySceneData::WallHeight)
		{
			ResultAddWall->SetIsHalfWall(true);			
		}
		Object = ResultAddWall;
	}
    
	//判断所有的点是否和墙体相交
	TArray<TSharedPtr<FArmyLine>> Lines;
	Object->GetLines(Lines);
	for (auto ItAddLine : Lines)
	{
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
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
						ItAddLine->GetEnd(), ItAddLine->GetStart(), CurrentIntersect))
					{
						bool OnModifyLine = false;
						//判断交点是否在拆改墙线上
						for (auto It : ModifyWallList)
						{
							TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
							if (ModifyWall.IsValid())
							{
								TArray< TSharedPtr<FArmyLine> > ModifyLines;
								ModifyWall->GetLines(ModifyLines);
								for (auto ItLine : ModifyLines)
								{
									if (FArmyMath::IsPointOnLine(CurrentIntersect, ItLine->GetStart(), ItLine->GetEnd()))
									{
										OnModifyLine = true;
										break;
									}
								}
								if (OnModifyLine)
									break;
							}
						}
						if (!OnModifyLine)
						{
							GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许交叉绘制"));
							return;
						}
					}
				}
			}
		}
	}

	FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

	XRArgument Arg = XRArgument().ArgFName(FName(*FString(TEXT("基础类")))).ArgString(Object->GetName()).ArgUint32(E_ModifyModel);
	FArmySceneData::Get()->Add(Object, Arg);
}

FVector FArmyAddWallOperation::Exec_PopPoint()
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



void FArmyAddWallOperation::Exec_PushPoint(const FVector& Pos)
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
				GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许绘制在户型外"));
				return;
			}
		}
	}

	TArray<TWeakPtr<FArmyObject>> ModifyWallList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);

	bool OnModifyLine = false;
	if (CachedPoints.Num() > 0)
	{
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
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
						//判断交点是否在拆改墙线上
						for (auto It : ModifyWallList)
						{
							TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
							if (ModifyWall.IsValid())
							{
								TArray< TSharedPtr<FArmyLine> > ModifyLines;
								ModifyWall->GetLines(ModifyLines);
								for (auto ItLine : ModifyLines)
								{
									if (FArmyMath::IsPointOnLine(CurrentIntersect, ItLine->GetStart(), ItLine->GetEnd()))
									{
										OnModifyLine = true;
										break;
									}
								}
								if (OnModifyLine)
									break;
							}
						}
						if (!OnModifyLine)
						{
							GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许交叉绘制"));
							return;
						}
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

		if (!bInModify && !OnModifyLine)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许在原始墙体内"));
			return;
		}
	}

	TSharedPtr<FArmyEditPoint> CapturePointPtr = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePointPtr();
	if (CapturePointPtr.IsValid())
	{
		if (CachedPoints.Num() >= 2 
			&& CachedPoints[0].Equals(CapturePointPtr->GetPos()))
		{
			//EndOperation();
			return;
		}
		else
		{
			CachedPoints.Add(CapturePointPtr->GetPos());
			CachedOriginPoints.Add(CapturePointPtr->GetPos());
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
		CachedOriginPoints.Add(Pos);

		if (CachedPoints.Num() >= 2)
		{
			TSharedPtr<FArmyLine> NewLine = MakeShareable(new FArmyLine(CachedPoints.Last(1), CachedPoints.Last()));
			CachedLines.Push(NewLine);
		}
	}

	if (CachedPoints.Num() == 2)
	{
		GGI->Window->ShowMessage(MT_Normal, TEXT("可单击右键完成新建墙体绘制"));
	}
}

bool FArmyAddWallOperation::GenerateExtrutionPoints(TArray<FVector>& InPoints)
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
				GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许在原始墙体内"));
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
		InPoints.Push(EndOffsetPoint);
		InPoints.Insert(ExtursionPoints, InPoints.Num());
		InPoints.Push(StartOffsetPoint);
	}

	return true;
}

void FArmyAddWallOperation::OnConfirmAddWallClicked()
{
	bOffsetGenerate = true;
	bInternalExtrusion = SettingWidget->GetIfCheck();
	ExtrusionThickness = FCString::Atof(*SettingWidget->GetThicknessValue().ToString()) / 10.f;
	bCreateWall = GenerateExtrutionPoints(CachedPoints);
	GGI->Window->DismissModalDialog();
	EndOperation();
}

void FArmyAddWallOperation::OnCloseAddWallClicked()
{
	GGI->Window->DismissModalDialog();
	CachedPoints.Empty();
	EndOperation();
}

void FArmyAddWallOperation::OnLineInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
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
