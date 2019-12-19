#include "ArmyBearingWallOperation.h"
#include "ArmyPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyToolsModule.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmySceneData.h"
#include "ArmyGameInstance.h"
#include "ArmyRoom.h"
#include "ArmyMath.h"

FArmyBearingWallOperation::FArmyBearingWallOperation(EModelType InBelongModel)
	: FArmyOperation(InBelongModel)
{
}

FArmyBearingWallOperation::~FArmyBearingWallOperation()
{
}

void FArmyBearingWallOperation::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (SolidLine.IsValid())
		SolidLine->Draw(PDI, View);
	for (auto Poly : MofidyWallRectList)
	{
		Poly->Draw(PDI, View);
	}
}

void FArmyBearingWallOperation::BeginOperation(XRArgument InArg)
{

	FArmyOperation::BeginOperation(InArg);
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = true;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel = 0;
	//FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Auxiliary;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_LineRange;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Closest;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_VertexCaptrue;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Perpendicular;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Midpoint;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_IntersectionPoint;

	//承重墙可以再户型的任何墙体上绘制，包括外墙
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
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
}

void FArmyBearingWallOperation::EndOperation()
{
	bShouldCreate = false;
	SolidLine = nullptr;
	CachedLines.Empty();
	AllLines.Empty();
	//OutWallLines.Empty();
	GGI->DesignEditor->OpenDefaultCaptureModel();
}

void FArmyBearingWallOperation::Tick()
{
	
}

bool FArmyBearingWallOperation::InputKey(class UArmyEditorViewportClient* InViewPortClient, class FViewport* InViewPort, FKey Key, EInputEvent Event)
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
				//添加线
				TSharedPtr<FArmyLine> Line = MakeShareable(new FArmyLine);
				Line->SetStart(SolidLine->GetStart());
				Line->SetEnd(SolidLine->GetEnd());
				Line->SetBaseColor(FLinearColor::White);
				CachedLines.Push(Line);

				//构建承重墙体多边形
				TSharedPtr<FArmyPolygon> CurrentPolygon = MakeShareable(new FArmyPolygon);
				UMaterial* BearingMaterial = FArmyEngineModule::Get().GetEngineResource()->GetBearingWallMaterial();
				UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BearingMaterial, nullptr);
				MID->AddToRoot();
				MID->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor::White);
				CurrentPolygon->MaterialRenderProxy = MID->GetRenderProxy(false);
	
				FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(Line->GetStart(), CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
				FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(Line->GetEnd(), CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
				FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(Line->GetStart(), CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
				FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(Line->GetEnd(), CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
				FVector ThicknessNormal = (FirstLineStartProject - SecondLineStartProject).GetSafeNormal();
				TArray<FVector> CurrentPoints;
				CurrentPoints.Push(FirstLineStartProject);
				CurrentPoints.Push(FirstLineEndProject);
				CurrentPoints.Push(SecondLineEndProject);
				CurrentPoints.Push(SecondLineStartProject);
				CurrentPolygon->SetVertices(CurrentPoints);

				TArray<TWeakPtr<FArmyObject>> ModifyWallList;
				FArmySceneData::Get()->GetObjects(E_HomeModel, OT_BearingWall, ModifyWallList);
				for (auto It : ModifyWallList)
				{
					TSharedPtr<FArmyBearingWall> ModifyWall = StaticCastSharedPtr<FArmyBearingWall>(It.Pin());
					if (ModifyWall.IsValid())
					{
						if (ModifyWall->IsSelected(SolidLine->GetStart(), nullptr)
							|| ModifyWall->IsSelected(SolidLine->GetEnd(), nullptr))
						{
							//承重墙体允许互相衔接，但不允许重叠
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
								GGI->Window->ShowMessage(MT_Warning, TEXT("承重墙体不允许重叠绘制"));
								bShouldCreate = false;
								SolidLine = nullptr;
								return false;
							}
						}
					}
				}

				TArray<TWeakPtr<FArmyObject>> WallList;
				//FArmySceneData::Get()->GetObjects(E_HomeModel, OT_AddWall, WallList);
				FArmySceneData::Get()->GetObjects(E_HomeModel, OT_ModifyWall, WallList);

				for (auto It : WallList)
				{
					TSharedPtr<FArmyModifyWall> Wall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
					if (Wall.IsValid())
					{
						if (Wall->IsPointInObj(SolidLine->GetStart())
							|| Wall->IsPointInObj(SolidLine->GetEnd())
							||FArmyMath::IsPointInOrOnPolygon2D(Wall->GetWallCaptureInfo().Pos, CurrentPoints))
						{
							GGI->Window->ShowMessage(MT_Warning, TEXT("请在原始墙体上绘制承重墙"));
							bShouldCreate = false;
							SolidLine = nullptr;

							return false;
						}
					}
				}

				TArray<TWeakPtr<FArmyHardware>> AllObjects;
				FArmySceneData::Get()->GetAllHardWare(AllObjects);
				TArray<FVector> ObjVector;
				TSharedPtr<FArmyPolygon> objPolygon = MakeShareable(new FArmyPolygon());
				bool bTypeExist = false;
				FString ObjName;
				for (auto ObjIt : AllObjects)
				{
					if (!ObjIt.IsValid()) continue;
					TSharedPtr<FArmyHardware> Obj = ObjIt.Pin();
					if (Obj.IsValid())
					{
						switch (Obj->GetType())
						{
						case OT_Door:
						case OT_SecurityDoor:
						case OT_SlidingDoor:
						case OT_Pass:
						case OT_Window:
						case OT_RectBayWindow:
						case OT_TrapeBayWindow:
						case OT_FloorWindow:
						case OT_Punch:
							bTypeExist = true;
							break;
						default:
							bTypeExist = false;
							break;
						}
					}
					ObjVector.Empty();
					ObjIt.Pin()->GetVertexes(ObjVector);
					if (ObjVector.Num() > 3 && bTypeExist)
					{
						objPolygon->SetVertices(ObjVector);

						if (objPolygon->IsInside(SolidLine->GetStart())
							|| objPolygon->IsInside(SolidLine->GetEnd())
							|| FArmyMath::IsPolygonInPolygon(ObjVector, CurrentPoints)
							|| FArmyMath::IsPolygonInPolygon(CurrentPoints, ObjVector))
						{
							GGI->Window->ShowMessage(MT_Warning, FString::Printf(TEXT("请在原始墙体上绘制承重墙")/*, *Obj->GetName()*/));
							bShouldCreate = false;
							SolidLine = nullptr;

							return false;
						}
					}
				}

				Exec_AddBearingWall(CurrentPoints);
			}		
			else
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("不符合承重墙体的范围"));
			}
			bShouldCreate = false;
			SolidLine = nullptr;
		}
	}

	return false;
}

void FArmyBearingWallOperation::MouseMove(class UArmyEditorViewportClient* InViewPortClient, FViewport* ViewPort, int32 X, int32 Y)
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

			FArmyBearingWall::FDoubleWallCaptureInfo CaptureInfoStart;
			FArmyBearingWall::FDoubleWallCaptureInfo CaptureInfoEnd;
			FArmyBearingWall::FDoubleWallCaptureInfo CaptureInfoCenter;
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

TSharedPtr<class FArmyBearingWall> FArmyBearingWallOperation::Exec_AddBearingWall(TArray<FVector>& Vertexes)
{
    TSharedPtr<FArmyBearingWall> ResultBearingWall = MakeShareable(new FArmyBearingWall());
	ResultBearingWall->SetOperationLinePos(SolidLine->GetStart(), SolidLine->GetEnd());
	ResultBearingWall->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetBearingWallMaterial());
	ResultBearingWall->SetVertexes(Vertexes);	 
	XRArgument Arg = XRArgument().ArgFName(FName(*FString(TEXT("基础类")))).ArgString(ResultBearingWall->GetName()).ArgUint32(E_HomeModel);
	SCOPE_TRANSACTION(TEXT("添加承重墙"));
	FArmySceneData::Get()->Add(ResultBearingWall, Arg);

	FArmySceneData::Get()->HomeMode_ModifyMultiDelegate.Broadcast();

	return ResultBearingWall;
}

bool FArmyBearingWallOperation::CaptureDoubleLine(const FVector& Pos, FArmyBearingWall::FDoubleWallCaptureInfo& OutInfo)
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
		//计算操作点是否在墙线内
		FVector FirstProjectPos = Pos;
		FVector SecondProjectPos = Pos;
		if (FArmyMath::GetLineSegmentProjectionPos(FirstLine->GetStart(), FirstLine->GetEnd(), FirstProjectPos)
			&& FArmyMath::GetLineSegmentProjectionPos(SecondLine->GetStart(), SecondLine->GetEnd(), SecondProjectPos)
			&& !FArmyMath::IsPointOnLine(Pos, FirstLine->GetStart(), FirstLine->GetEnd())
			&& !FArmyMath::IsPointOnLine(Pos, SecondLine->GetStart(), SecondLine->GetEnd()))
		{
			FVector a = FArmyMath::GetLineDirection(Pos, FirstProjectPos);
			FVector b = FArmyMath::GetLineDirection(Pos, SecondProjectPos);
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
