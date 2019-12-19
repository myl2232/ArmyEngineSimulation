#include "ArmyModifyWallOperation.h"
#include "ArmyPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyToolsModule.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmySceneData.h"
#include "ArmyGameInstance.h"
#include "ArmyRoom.h"
#include "ArmyBearingWall.h"

FArmyModifyWallOperation::FArmyModifyWallOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
{
}


FArmyModifyWallOperation::~FArmyModifyWallOperation()
{
}

void FArmyModifyWallOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (SolidLine.IsValid())
		SolidLine->Draw(PDI, View);
	/*for (auto It : CachedLines)
	{
		It->Draw(PDI, View);
	}*/
	for (auto Poly : MofidyWallRectList)
	{
		Poly->Draw(PDI, View);
	}
}

void FArmyModifyWallOperation::BeginOperation(XRArgument InArg)
{
	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = 0;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Auxiliary;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_LineRange;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Closest;

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			TArray<TSharedPtr<FArmyEditPoint>> Points =  Room->GetPoints();
			for (auto TempIt : Points)
				FArmyToolsModule::Get().GetMouseCaptureTool()->AddAuxiliaryPoint(TempIt);
			
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid())
					AllLines.Push(Line);
		}
	}
	RoomList.Empty();
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_OutRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			TArray<TSharedPtr<FArmyEditPoint>> Points = Room->GetPoints();
			for (auto TempIt : Points)
				FArmyToolsModule::Get().GetMouseCaptureTool()->AddAuxiliaryPoint(TempIt);

			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid())
					OutWallLines.Push(Line);
		}
	}
}

void FArmyModifyWallOperation::EndOperation()
{
	bShouldCreate = false;
	SolidLine = nullptr;
	CachedLines.Empty();
	AllLines.Empty();
	OutWallLines.Empty();
	GGI->DesignEditor->OpenDefaultCaptureModel();
}

void FArmyModifyWallOperation::Tick()
{
	
}

bool FArmyModifyWallOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
{
	if (Key == EKeys::LeftMouseButton &&Event == EInputEvent::IE_Pressed)
	{						
		FVector2D MousePos;
		GVC->GetMousePosition(MousePos);
		FVector MousePos3D = GVC->PixelToWorld(MousePos.X, MousePos.Y, 0);
		MousePos3D.Z = 0;

		TSharedPtr<FArmyEditPoint> tt = FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePointPtr();
		FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);
		FArmyToolsModule::Get().GetMouseCaptureTool()->SetCurrentPoint(FVector2D(CapturePoint));

		if (!SolidLine.IsValid())
		{
			SolidLine = MakeShareable(new FArmyLine());
			SolidLine->bIsDashLine = true;
			SolidLine->SetStart(CapturePoint);
			SolidLine->SetEnd(CapturePoint);
		}
		else
		{
			if (bShouldCreate)
			{
				TArray<TWeakPtr<FArmyObject>> WallList;
				FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, WallList);
				for (auto It : WallList)
				{
					TSharedPtr<FArmyAddWall> AddWall = StaticCastSharedPtr<FArmyAddWall>(It.Pin());
					if (AddWall.IsValid())
					{
						if (AddWall->IsSelected(SolidLine->GetStart(), nullptr)
							|| AddWall->IsSelected(SolidLine->GetEnd(), nullptr))
						{
							GGI->Window->ShowMessage(MT_Warning, TEXT("请在原始内墙上进行绘制"));
							bShouldCreate = false;
							SolidLine = nullptr;

							return false;
						}
					}
				}


				//添加线
				TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine);
				Line->SetStart(SolidLine->GetStart());
				Line->SetEnd(SolidLine->GetEnd());
				Line->SetBaseColor(FLinearColor::Red);
				CachedLines.Push(Line);
				
				//SolidLine->SetStart(CapturePoint);

				//构建拆除的墙体多边形
				TSharedPtr<FArmyPolygon> CurrentPolygon = MakeShareable(new FArmyPolygon);
				UMaterial* ModifyMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
				UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(ModifyMaterial, nullptr);
				MID->AddToRoot();
				MID->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor::White);
				CurrentPolygon->MaterialRenderProxy = MID->GetRenderProxy(false);
	
				FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(Line->GetStart(), CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
				FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(Line->GetEnd(), CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
				FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(Line->GetStart(), CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
				FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(Line->GetEnd(), CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
				TArray<FVector> CurrentPoints;
				CurrentPoints.Push(FirstLineStartProject);
				CurrentPoints.Push(FirstLineEndProject);
				CurrentPoints.Push(SecondLineEndProject);
				CurrentPoints.Push(SecondLineStartProject);
				CurrentPolygon->SetVertices(CurrentPoints);


				WallList.Empty();
				FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_BearingWall, WallList);
				TArray<FVector> VecPoints;
				TSharedPtr<FArmyPolygon> objPolygon = MakeShareable(new FArmyPolygon());
				for (auto It : WallList)
				{
					TSharedPtr<FArmyBearingWall> Wall = StaticCastSharedPtr<FArmyBearingWall>(It.Pin());
					VecPoints.Empty();
					Wall->GetVertexes(VecPoints);
					if (Wall.IsValid())
					{
						objPolygon->SetVertices(VecPoints);
						if (FArmyMath::ArePolysIntersection(VecPoints, CurrentPoints,true))
						{
							GGI->Window->ShowMessage(MT_Warning, TEXT("承重墙不允许拆除"));
							bShouldCreate = false;
							SolidLine = nullptr;

							return false;
						}
					}
				}

				//@ //拆除墙体允许互相衔接，但不允许重叠和相交
				TArray<TWeakPtr<FArmyObject>> ModifyWallList;
				FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);
				for (auto It : ModifyWallList)
				{
					TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
					if (ModifyWall.IsValid())
					{
						if (ModifyWall->IsSelected(SolidLine->GetStart(), nullptr)
							|| ModifyWall->IsSelected(SolidLine->GetEnd(), nullptr))
						{
							FVector ModifyWallCenter = ModifyWall->GetBasePos();
							FBox modifyBox(CurrentPoints);
							FVector LineCenter = modifyBox.GetCenter();
							float CenterDis = FVector::Distance(ModifyWallCenter, LineCenter);
							TArray<TSharedPtr <FArmyLine> > Lines;
							TArray<TSharedPtr <FArmyLine> > NewLines;
							ModifyWall->GetLines(Lines);
							CurrentPolygon->GetLines(NewLines);
							bool isIntersection = false;
							FVector ModifyInterPos = FVector::ZeroVector;
							FVector NewModifyInterPos = FVector::ZeroVector;
							for (auto L : Lines)
							{
								isIntersection = FArmyMath::CalculateLinesIntersection(LineCenter, ModifyWallCenter, L->GetStart(), L->GetEnd(), ModifyInterPos) ? true : false;
								if (isIntersection)
									break;
								else
									continue;
							}

							for (auto L : NewLines)
							{
								isIntersection = FArmyMath::CalculateLinesIntersection(LineCenter, ModifyWallCenter, L->GetStart(), L->GetEnd(), NewModifyInterPos) ? true : false;
								if (isIntersection)
									break;
								else
									continue;
							}

							float dis1 = FVector::Dist2D(ModifyWallCenter, ModifyInterPos);
							float dis2 = FVector::Dist2D(LineCenter, NewModifyInterPos);
							if ((dis1 + dis2) > CenterDis)
							{
								GGI->Window->ShowMessage(MT_Warning, TEXT("拆除墙体不允许重叠绘制"));
								bShouldCreate = false;
								SolidLine = nullptr;
								return false;
							}
						}
					}
				}

				Exec_AddModifyWall(CurrentPoints);
			}		
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("请在原始内墙上进行绘制"));
			}
			bShouldCreate = false;
			SolidLine = nullptr;
		}
	}

	return false;
}

void FArmyModifyWallOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
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

	FVector CapturePoint(FArmyToolsModule::Get().GetMouseCaptureTool()->GetCapturePoint(), 0);

	if (SolidLine.IsValid())
	{
		if (SolidLine->GetEnd() != CapturePoint)
		{
			SolidLine->SetEnd(CapturePoint);

			//首先判断SolidLine是否在外墙区域内
			FVector SolidLineCenter = FArmyMath::GetLineCenter(SolidLine->GetStart(), SolidLine->GetEnd());
			for (auto It : OutWallLines)
			{
				FVector CurrentPoint = SolidLineCenter;
				if (FArmyMath::GetLineSegmentProjectionPos(It.Pin()->GetStart(), It.Pin()->GetEnd(), CurrentPoint))
				{
					if (FArmyMath::Distance(SolidLine->GetStart(), It.Pin()->GetStart(), It.Pin()->GetEnd()) + 0.01 < FArmySceneData::OutWallThickness / 10.f
						|| FArmyMath::Distance(SolidLine->GetEnd(), It.Pin()->GetStart(), It.Pin()->GetEnd()) + 0.01 < FArmySceneData::OutWallThickness / 10.f)
					{
						bShouldCreate = false;
						SolidLine->SetBaseColor(FLinearColor::Red);
						return;
					}
				}
			}

			FArmyModifyWall::FDoubleWallCaptureInfo CaptureInfoStart;
			FArmyModifyWall::FDoubleWallCaptureInfo CaptureInfoEnd;
			FArmyModifyWall::FDoubleWallCaptureInfo CaptureInfoCenter;
			FVector CurrentCenter = (SolidLine->GetStart() + SolidLine->GetEnd()) / 2;
			if (CaptureDoubleLine(SolidLine->GetStart(), CaptureInfoStart)
				&& CaptureDoubleLine(SolidLine->GetEnd(), CaptureInfoEnd)
				&& CaptureDoubleLine(CurrentCenter, CaptureInfoCenter))
			{
				CaptureWallInfo = CaptureInfoCenter;
			}
			else
			{
				bShouldCreate = false;
				SolidLine->SetBaseColor(FLinearColor::Red);
				return;
			}

			for (auto Object : AllLines)
			{
				TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());
				if (FArmyMath::CalculateLine2DIntersection(SolidLine->GetStart(), SolidLine->GetEnd(),
					Wall->GetStart(), Wall->GetEnd()))
				{
					bShouldCreate = false;
					SolidLine->SetBaseColor(FLinearColor::Red);
					return;
				}	
			}

			bShouldCreate = true;
			SolidLine->SetBaseColor(FLinearColor::Green);
		}
	}
}

TSharedPtr<class FArmyModifyWall> FArmyModifyWallOperation::Exec_AddModifyWall(TArray<FVector>& Vertexes)
{
    TSharedPtr<FArmyModifyWall> ResultModifyWall = MakeShareable(new FArmyModifyWall());
	ResultModifyWall->SetOperationLinePos(SolidLine->GetStart(), SolidLine->GetEnd());
	ResultModifyWall->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial());
    ResultModifyWall->SetVertexes(Vertexes);
    ResultModifyWall->SetWidth(CaptureWallInfo.Thickness);
	XRArgument Arg = XRArgument().ArgFName(FName(*FString(TEXT("基础类")))).ArgString(ResultModifyWall->GetName()).ArgUint32(E_ModifyModel);
	SCOPE_TRANSACTION(TEXT("添加拆改墙"));
	FArmySceneData::Get()->Add(ResultModifyWall, Arg);
	FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
	return ResultModifyWall;
}

bool FArmyModifyWallOperation::CaptureDoubleLine(const FVector& Pos, FArmyModifyWall::FDoubleWallCaptureInfo& OutInfo)
{
	TSharedPtr<FArmyLine> FirstLine;
	TSharedPtr<FArmyLine> SecondLine;

	// 捕捉墙体的最大距离
	const float MaxDistance = FArmySceneData::MaxWallCaptureThickness;
	float MinDistance = MaxDistance;

	// 捕捉第一面墙
	FVector FirstSnapPos = Pos;
	for (auto Object : AllLines)
	{
		TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());
		FVector P0 = Wall->GetStart();
		FVector P1 = Wall->GetEnd();

		FVector Projection = Pos;
		if (FArmyMath::GetLineSegmentProjectionPos(P0, P1, Projection))
		{
			float Distance = FVector::Distance(Pos, Projection);
			if (Distance > 0.f && Distance < MaxDistance && Distance < MinDistance)
			{
				FirstSnapPos = Projection;
				FirstLine = Wall;
				MinDistance = Distance;
			}
		}
	}

	// 捕捉第二面墙
	FVector SecondSnapPos = Pos;
	if (FirstLine.IsValid())
	{
		float TempMinDistance = MaxDistance;
		for (auto Object : AllLines)
		{
			TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());

			bool IsSameline = (FirstLine->GetStart() == Wall->GetStart() && FirstLine->GetEnd() == Wall->GetEnd()) ? true : false;
			bool bIsParallel = FArmyMath::AreLinesParallel(FirstLine->GetStart(), FirstLine->GetEnd(), Wall->GetStart(), Wall->GetEnd());
			FVector CurrentPoint;
			float LinesDistance = FArmyMath::DistanceLineToLine(FirstLine->GetStart(), FirstLine->GetEnd(), Wall->GetStart(), Wall->GetEnd(), CurrentPoint);
			if (bIsParallel && !IsSameline && LinesDistance > 5.f)//防止两条平行线，并且在一条水平线上
			{
				FVector Projection = Pos;
				FVector Projection2 = Pos;
				if (FArmyMath::GetLineSegmentProjectionPos(Wall->GetStart(), Wall->GetEnd(), Projection))
				{
					float Distance = FVector::Distance(SecondSnapPos, Projection);
					if (Distance >= 0.0f && Distance <= MaxDistance && Distance < TempMinDistance)
					{
						SecondSnapPos = Projection;
						SecondLine = Wall;
						TempMinDistance = Distance;
					}
				}
			}
		}
	}

	if (FirstLine.IsValid() && !SecondLine.IsValid())
	{
		SecondSnapPos = Pos;
		float TempMinDistance = MaxDistance;
		for (auto Object : AllLines)
		{
			TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());

			bool IsSameline = (FirstLine->GetStart() == Wall->GetStart() && FirstLine->GetEnd() == Wall->GetEnd()) ? true : false;
			bool bIsParallel = FArmyMath::AreLinesParallel(FirstLine->GetStart(), FirstLine->GetEnd(), Wall->GetStart(), Wall->GetEnd());
			FVector CurrentPoint;
			float LinesDistance = FArmyMath::DistanceLineToLine(FirstLine->GetStart(), FirstLine->GetEnd(), Wall->GetStart(), Wall->GetEnd(), CurrentPoint);
			if (bIsParallel && !IsSameline && LinesDistance > 5.f)
			{
				FVector Projection = FArmyMath::GetProjectionPoint(Pos, Wall->GetStart(), Wall->GetEnd());
				float Distance = FVector::Distance(SecondSnapPos, Projection);
				if (Distance >= 0.0f && Distance <= MaxDistance && Distance < TempMinDistance)
				{
					SecondSnapPos = Projection;
					SecondLine = Wall;
					TempMinDistance = Distance;
				}
			}
		}
	}
	else if (!FirstLine.IsValid() && SecondLine.IsValid())
	{
		SecondSnapPos = Pos;
		float TempMinDistance = MaxDistance;
		for (auto Object : AllLines)
		{
			TSharedPtr<FArmyLine> Wall = StaticCastSharedPtr<FArmyLine>(Object.Pin());

			bool IsSameline = (SecondLine->GetStart() == Wall->GetStart() && SecondLine->GetEnd() == Wall->GetEnd()) ? true : false;
			bool bIsParallel = FArmyMath::AreLinesParallel(SecondLine->GetStart(), SecondLine->GetEnd(), Wall->GetStart(), Wall->GetEnd());
			FVector CurrentPoint;
			float LinesDistance = FArmyMath::DistanceLineToLine(FirstLine->GetStart(), FirstLine->GetEnd(), Wall->GetStart(), Wall->GetEnd(), CurrentPoint);
			if (bIsParallel && !IsSameline && LinesDistance > 5.f)
			{
				FVector Projection = FArmyMath::GetProjectionPoint(Pos, Wall->GetStart(), Wall->GetEnd());
				float Distance = FVector::Distance(SecondSnapPos, Projection);
				if (Distance >= 0.0f && Distance <= MaxDistance && Distance < TempMinDistance)
				{
					SecondSnapPos = Projection;
					FirstLine = Wall;
					TempMinDistance = Distance;
				}
			}
		}
	}

	if (FirstLine.IsValid() && SecondLine.IsValid())
	{

		//@ 计算操作点是否在墙线内
		FVector FirstProjectPos = Pos;
		FVector SecondProjectPos = Pos;
		if (FArmyMath::GetLineSegmentProjectionPos(FirstLine->GetStart(), FirstLine->GetEnd(), FirstProjectPos)
			&& FArmyMath::GetLineSegmentProjectionPos(SecondLine->GetStart(), SecondLine->GetEnd(), SecondProjectPos))
		{
			float PosAngle = FVector::DotProduct(FArmyMath::GetLineDirection(Pos, FirstProjectPos), FArmyMath::GetLineDirection(Pos, SecondProjectPos));

			//大于0说明捕捉点在墙外
			if (PosAngle > 0)
				return false;
		}

		OutInfo.FirstLine = FirstLine;
		OutInfo.SecondLine = SecondLine;

		// 保存两面墙中长度较短的那一面，用作计算门到两边墙面的距离
		const float FirstWallLength = FVector::Distance(FirstLine->GetStart(), FirstLine->GetEnd());
		const float SecondWallLength = FVector::Distance(SecondLine->GetStart(), SecondLine->GetEnd());
		OutInfo.MinDistanceWall = FirstWallLength <= SecondWallLength ? FirstLine : SecondLine;

		OutInfo.Pos = (FirstSnapPos + SecondSnapPos) / 2.0f;
		OutInfo.Thickness = FVector::Distance(FirstSnapPos, SecondSnapPos);

		if (FVector::Distance(Pos, FirstSnapPos) > FVector::Distance(Pos, SecondSnapPos))
		{
			OutInfo.Direction = FArmyMath::GetLineDirection(FirstSnapPos, SecondSnapPos);
		}
		else
		{
			OutInfo.Direction = FArmyMath::GetLineDirection(SecondSnapPos, FirstSnapPos);
		}

		return true;
	}

	return false;
}
