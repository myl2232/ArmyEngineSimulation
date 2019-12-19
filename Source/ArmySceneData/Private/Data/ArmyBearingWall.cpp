#include "ArmyBearingWall.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyStyle.h"
#include "ArmyRulerLine.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyMath.h"
#include "ArmyViewPortClient.h"
#include "ArmyGameInstance.h"

FArmyBearingWall::FArmyBearingWall()
	:Polygon(MakeShareable(new FArmyPolygon()))
	, HardWareRulerLine(MakeShareable(new FArmyRulerLine()))
	, FirstLineLeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, FirstLineRightRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondLineLeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondLineRightRulerLine(MakeShareable(new FArmyRulerLine()))
	, Height(FArmySceneData::WallHeight)
	, OutLineColor(FColor::White)
{
	RulerOffsetDis = 30.f;
    SetName(TEXT("承重墙"));
	ObjectType = OT_BearingWall;
	Color = FLinearColor(FColor(0XFFFFFFFF));
	HardWareRulerLine->SetEnableInputBox(false);
	HardWareRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyBearingWall::OnHardWareInputBoxCommitted));
	FirstLineLeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyBearingWall::OnFirstLineLInputBoxCommitted));
	FirstLineRightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyBearingWall::OnFirstLineRInputBoxCommitted));
	SecondLineLeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyBearingWall::OnSecondLineLInputBoxCommitted));
	SecondLineRightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyBearingWall::OnSecondLineRInputBoxCommitted));
}

void FArmyBearingWall::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue(TEXT("operationLineStart"), OperationLineStartPos.ToString());
	JsonWriter->WriteValue(TEXT("operationLineEnd"), OperationLineEndPos.ToString());
	JsonWriter->WriteArrayStart(TEXT("vertexes"));
	TArray<FVector>& Vertexes = Polygon->Vertices;
	for (auto& Vertex : Vertexes)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyBearingWall)
}
void FArmyBearingWall::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetBearingWallMaterial();
	MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);

	FString StartPosStr = InJsonData->GetStringField(TEXT("operationLineStart"));
	FString EndPosStr = InJsonData->GetStringField(TEXT("operationLineEnd"));
	FVector CurrentStart;
	FVector CurrentEnd;
	CurrentStart.InitFromString(StartPosStr);
	CurrentEnd.InitFromString(EndPosStr);
	SetOperationLinePos(CurrentStart, CurrentEnd);

	TArray<FVector> Vertexes;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("vertexes", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		Vertexes.Add(Vertex);
	}
	Polygon->SetVertices(Vertexes);

	FArmyObject::Deserialization(InJsonData);
	if (GetPropertyFlag(FArmyObject::FLAG_MODIFY))
	{
		HardWareRulerLine->SetEnableInputBox(false);
		FirstLineLeftRulerLine->SetEnableInputBox(false);
		FirstLineRightRulerLine->SetEnableInputBox(false);
		SecondLineLeftRulerLine->SetEnableInputBox(false);
		SecondLineRightRulerLine->SetEnableInputBox(false);
	}
}
void FArmyBearingWall::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor(FColor(0XFFFFFFFF));
		break;
	case OS_Selected:
		Color = FLinearColor(FColor(0xFFFFE0B2));
		OutLineColor = FLinearColor(FColor(0xFFFF9800));
		Polygon->SetOutLineColor(OutLineColor);
		break;

	default:
		break;
	}
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);

	if (State != OS_Selected)
	{
		HardWareRulerLine->ShowInputBox(false);
		FirstLineLeftRulerLine->ShowInputBox(false);
		FirstLineRightRulerLine->ShowInputBox(false);
		SecondLineLeftRulerLine->ShowInputBox(false);
		SecondLineRightRulerLine->ShowInputBox(false);
	}
	else
	{
		if (bUpdateDashLine)
			UpdateDashLine();
	}
}

void FArmyBearingWall::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		Polygon->Draw(PDI, View);
		if (State == OS_Selected)
		{
			UpdateDashLine();
			//选中态边框线跟填充颜色不一致，需要额外绘制一遍
			Polygon->DrawOutLine(PDI, View);

			//绘制自身标尺
			HardWareRulerLine->Draw(PDI, View);

			if (FirstRelatedRoom.IsValid())
			{
				FirstLineLeftRulerLine->Draw(PDI, View);
				FirstLineRightRulerLine->Draw(PDI, View);
			}
			else
			{
				FirstLineLeftRulerLine->ShowInputBox(false);
				FirstLineRightRulerLine->ShowInputBox(false);
			}
			if (SecondRelatedRoom.IsValid())
			{
				SecondLineLeftRulerLine->Draw(PDI, View);
				SecondLineRightRulerLine->Draw(PDI, View);
			}
			else
			{
				SecondLineLeftRulerLine->ShowInputBox(false);
				SecondLineRightRulerLine->ShowInputBox(false);
			}
		}
	}
}

bool FArmyBearingWall::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmyBearingWall::GetVertexes(TArray<FVector>& OutVertexes)
{
	OutVertexes.Append(Polygon->Vertices);
}

void FArmyBearingWall::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	Polygon->GetLines(OutLines);
}

const FBox FArmyBearingWall::GetBounds()
{
	return FBox(Polygon->Vertices);
}

void FArmyBearingWall::Delete()
{
	FArmyObject::Delete();

	HardWareRulerLine->ShowInputBox(false);
	FirstLineLeftRulerLine->ShowInputBox(false);
	FirstLineRightRulerLine->ShowInputBox(false);
	SecondLineLeftRulerLine->ShowInputBox(false);
	SecondLineRightRulerLine->ShowInputBox(false);

	//恢复门窗状态
	TArray<TWeakPtr<FArmyObject>> HardWareList;
	FArmySceneData::Get()->GetHardWareObjects(HardWareList);
	for (auto It : HardWareList)
	{
		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		if (IsSelected(Hardware->GetStartPos(), nullptr) || IsSelected(Hardware->GetEndPos(), nullptr)
			|| Hardware->RectImagePanel->IsSelected(OperationLineStartPos, nullptr)
			|| Hardware->RectImagePanel->IsSelected(OperationLineEndPos, nullptr))
			Hardware->bGenerate3D = true;
	}
}

bool FArmyBearingWall::IsPointInObj(const FVector & Pos)
{
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, Polygon->Vertices);
}

void FArmyBearingWall::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyObject::SetPropertyFlag(InType, InUse);
	if (InType == FArmyObject::FLAG_MODIFY && InUse)
	{
		HardWareRulerLine->SetEnableInputBox(false);
		FirstLineLeftRulerLine->SetEnableInputBox(false);
		FirstLineRightRulerLine->SetEnableInputBox(false);
		SecondLineLeftRulerLine->SetEnableInputBox(false);
		SecondLineRightRulerLine->SetEnableInputBox(false);
	}
}

void FArmyBearingWall::SetMaterial(UMaterial* InMaterial)
{
    MI_Line = UMaterialInstanceDynamic::Create(InMaterial, NULL);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
}

void FArmyBearingWall::SetVertexes(TArray<FVector>& Vertexes)
{
	Polygon->SetVertices(Vertexes);
}

TArray<FVector> FArmyBearingWall::GetClipingBox()
{
	TArray<FVector> BoudingBox;
	GetVertexes(BoudingBox);
	FVector lastAndfirst = BoudingBox[BoudingBox.Num() - 1] - BoudingBox[0];
	if (lastAndfirst.IsNearlyZero())
	{
		BoudingBox.RemoveAt(BoudingBox.Num() - 1);
	}
	int number = BoudingBox.Num();
	for (int i = 0; i < number; ++i)
	{
		BoudingBox[i].Z = FloorHeight + Height / 2.f;
	}		
	return BoudingBox;
}

bool FArmyBearingWall::CaptureDoubleLine(const FVector& Pos, FDoubleWallCaptureInfo& OutInfo)
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
	AllLines.Reset();
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		TArray<TSharedPtr<FArmyLine>> RoomLines;
		if (Room.IsValid())
		{
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
				if (Line.IsValid())
					AllLines.Push(Line);
		}
	}

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

void FArmyBearingWall::SetOperationLinePos(const FVector& InStartPos, const FVector& InEndPos)
{
	OperationLineStartPos = InStartPos;
	OperationLineEndPos = InEndPos;
	

	FVector CenterPos = (OperationLineStartPos + OperationLineEndPos) / 2;
	if (CaptureDoubleLine(CenterPos, CaptureWallInfo))
	{
		FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector CurrentDirection = FirstLineStartProject - FirstLineEndProject;
		CurrentDirection = CurrentDirection.GetSafeNormal();
		float TempLength = GetLength();
		FVector TempCenter = (FirstLineStartProject + FirstLineEndProject) / 2.f;
		OperationLineStartPos = TempCenter - CurrentDirection * TempLength / 2.f;
		OperationLineEndPos = TempCenter + CurrentDirection * TempLength / 2.f;

		TArray<TWeakPtr<FArmyObject>> RoomList;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);

		for (auto It : RoomList)
		{
			if (FirstRelatedRoom.IsValid() && SecondRelatedRoom.IsValid())
				break;

			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (Room.IsValid())
			{
				TArray<TSharedPtr<FArmyLine>> RoomLines;
				Room->GetLines(RoomLines);
				for (auto Line : RoomLines)
				{
					if (Line.IsValid() && Line == CaptureWallInfo.FirstLine)
					{
						FirstRelatedRoom = Room;
						break;
					}
					else if (Line.IsValid() && Line == CaptureWallInfo.SecondLine)
					{
						SecondRelatedRoom = Room;
					}
				}
			}
		}

		bUpdateDashLine = true;				
	    UpdateOperationCenterPos(OperationLineStartPos, OperationLineEndPos, CaptureWallInfo);
	}
}

void FArmyBearingWall::UpdateDashLine()
{	
    if (!CaptureWallInfo.FirstLine.IsValid() || !CaptureWallInfo.SecondLine.IsValid())
        return;
    FVector LineDirection = FArmyMath::GetLineDirection(OperationLineStartPos, OperationLineEndPos);
	FVector DashLineDirection = LineDirection.RotateAngleAxis(90, FVector(0, 0, 1));	
	//--------------------控件标尺线更新--------------------
	
	float thickness = CaptureWallInfo.Thickness;
	
	//获取点是否在关联线上
	bool posInFirstLine = FArmyMath::IsPointOnLine(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	bool posInSencondLine = FArmyMath::IsPointOnLine(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
	
	//承重墙如果在外墙上，控件标尺线延外墙线显示，如果不在外墙上，则根据起点顺序进行两侧随机标记
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() == OT_OutRoom)
	{
		if (FirstRelatedRoom->IsPointInRoom(CaptureWallInfo.FirstLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection))
		{
			DashLineDirection *= -1;
		}
		if (posInFirstLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
		}
		else if (posInSencondLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
		}
		HardWareRulerLine->Update(OperationLineStartPosCenter, OperationLineEndPosCenter, DashLineDirection * RulerOffsetDis);
	}
	else if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() == OT_OutRoom)
	{
		if (SecondRelatedRoom->IsPointInRoom(CaptureWallInfo.SecondLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection))
		{
			DashLineDirection *= -1;
		}
		if (posInFirstLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
		}
		else if (posInSencondLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
		}
		HardWareRulerLine->Update(OperationLineStartPosCenter, OperationLineEndPosCenter, DashLineDirection * RulerOffsetDis);
	}
	else
	{
		HardWareRulerLine->Update(OperationLineStartPos, OperationLineEndPos, DashLineDirection * RulerOffsetDis * 2);
		FVector TempCheckPos = (HardWareRulerLine->GetUpDashLineStart() + HardWareRulerLine->GetUpDashLineEnd()) / 2.f;
		if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
		{
			if (!FirstRelatedRoom->IsPointInRoom(TempCheckPos))
			{
				if (posInFirstLine)
				{
					OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
				}
				else
				{
					OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
				}
				HardWareRulerLine->Update(OperationLineStartPosCenter, OperationLineEndPosCenter, -1 * DashLineDirection * RulerOffsetDis);
			}
			else
			{
				if (posInFirstLine)
				{
					OperationLineStartPosCenter = OperationLineStartPos - (DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos - (DashLineDirection * thickness / 2);
				}
				else
				{
					OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
				}
				HardWareRulerLine->Update(OperationLineStartPosCenter, OperationLineEndPosCenter, DashLineDirection * RulerOffsetDis);
			}
		}
		else if (FirstRelatedRoom.IsValid()/* && !FirstRelatedRoom->IsClosed()*/)
		{
			if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
			{
				if (!SecondRelatedRoom->IsPointInRoom(TempCheckPos))
				{
					if (posInSencondLine)
					{
						OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
						OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
					}
					else
					{
						OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
						OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
					}
					HardWareRulerLine->Update(OperationLineStartPosCenter, OperationLineEndPosCenter, -1 * DashLineDirection * RulerOffsetDis);
				}
			}
		}
	}

	//--------------------第一临墙标尺线更新--------------------
	if (CaptureWallInfo.FirstLine.IsValid())
	{
		if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
		{
			if ((!FirstRelatedRoom->IsPointInRoom(CaptureWallInfo.FirstLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && FirstRelatedRoom->GetType() == OT_InternalRoom) ||
				(FirstRelatedRoom->IsPointInRoom(CaptureWallInfo.FirstLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && FirstRelatedRoom->GetType() == OT_OutRoom))
			{
				DashLineDirection *= -1;
			}
		}
		else if (FirstRelatedRoom.IsValid() && !FirstRelatedRoom->IsClosed())
		{
			if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
			{
				if (SecondRelatedRoom->IsPointInRoom(CaptureWallInfo.SecondLine->GetLineEditPoint()->GetPos() + 2 * DashLineDirection))
				{
					DashLineDirection *= -1;
				}
			}
		}
		float TempLength1 = FVector::Distance(CaptureWallInfo.FirstLine->GetStart(), OperationLineStartPos);
		float TempLength2 = FVector::Distance(CaptureWallInfo.FirstLine->GetStart(), OperationLineEndPos);
		if (TempLength1 < TempLength2)
		{
			//前半段
			FirstLineLeftRulerLine->Update(CaptureWallInfo.FirstLine->GetStart(), OperationLineStartPos,
				DashLineDirection * RulerOffsetDis, DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd())));
			//后半段
			FirstLineRightRulerLine->Update(OperationLineEndPos, CaptureWallInfo.FirstLine->GetEnd(),
				DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd())), DashLineDirection * RulerOffsetDis);
		}
		else
		{
			//前半段
			FirstLineLeftRulerLine->Update(CaptureWallInfo.FirstLine->GetStart(), OperationLineEndPos,
				DashLineDirection * RulerOffsetDis, DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd())));
			//后半段
			FVector TempLeftOffset = DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd()));
			FirstLineRightRulerLine->Update(OperationLineStartPos, CaptureWallInfo.FirstLine->GetEnd(),
				TempLeftOffset, DashLineDirection * RulerOffsetDis);
		}
	}

	//--------------------第二临墙标尺线更新--------------------
	if (CaptureWallInfo.SecondLine.IsValid())
	{
		if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
		{
			if ((!SecondRelatedRoom->IsPointInRoom(CaptureWallInfo.SecondLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && SecondRelatedRoom->GetType() == OT_InternalRoom) ||
				(SecondRelatedRoom->IsPointInRoom(CaptureWallInfo.SecondLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && SecondRelatedRoom->GetType() == OT_OutRoom))
			{
				DashLineDirection *= -1;
			}
		}
		else if (SecondRelatedRoom.IsValid() && !SecondRelatedRoom->IsClosed())
		{
			if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
			{
				if (FirstRelatedRoom->IsPointInRoom(CaptureWallInfo.FirstLine->GetLineEditPoint()->GetPos() + 2 * DashLineDirection))
				{
					DashLineDirection *= -1;
				}
			}
		}
		float TempLength1 = FVector::Distance(CaptureWallInfo.SecondLine->GetStart(), OperationLineStartPos);
		float TempLength2 = FVector::Distance(CaptureWallInfo.SecondLine->GetStart(), OperationLineEndPos);
		if (TempLength1 < TempLength2)
		{
			//前半段
			SecondLineLeftRulerLine->Update(CaptureWallInfo.SecondLine->GetStart(), OperationLineStartPos,
				DashLineDirection * RulerOffsetDis, DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())));
			//后半段
			SecondLineRightRulerLine->Update(OperationLineEndPos, CaptureWallInfo.SecondLine->GetEnd(),
				DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())), DashLineDirection * RulerOffsetDis);
		}
		else
		{
			//前半段
			SecondLineLeftRulerLine->Update(CaptureWallInfo.SecondLine->GetStart(), OperationLineEndPos,
				DashLineDirection * RulerOffsetDis, DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())));
			//后半段
			SecondLineRightRulerLine->Update(OperationLineStartPos, CaptureWallInfo.SecondLine->GetEnd(),
				DashLineDirection * (RulerOffsetDis + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())), DashLineDirection * RulerOffsetDis);
		}
	}	
}
void FArmyBearingWall::SetLength(float InValue)
{
	float OldLength = GetLength();
	FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());

	FVector CurrentDirection = FirstLineStartProject - FirstLineEndProject;
	CurrentDirection = CurrentDirection.GetSafeNormal();
	FVector TempCenter = (FirstLineStartProject + FirstLineEndProject) / 2.f;
	OperationLineStartPos = TempCenter - CurrentDirection * InValue / 2.f;
	OperationLineEndPos = TempCenter + CurrentDirection * InValue / 2.f;

	FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
	FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
	TArray<FVector> CurrentPoints;
	CurrentPoints.Push(FirstLineStartProject);
	CurrentPoints.Push(FirstLineEndProject);
	CurrentPoints.Push(SecondLineEndProject);
	CurrentPoints.Push(SecondLineStartProject);
	Polygon->SetVertices(CurrentPoints);
	UpdateDashLine();

}

bool FArmyBearingWall::CheckAddOrModifyLegal()
{

	TArray<TWeakPtr<FArmyObject>> WallList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_AddWall, WallList);
	for (auto It : WallList)
	{
		TSharedPtr<FArmyBearingWall> Wall = StaticCastSharedPtr<FArmyBearingWall>(It.Pin());
		if (Wall.IsValid())
		{
			if (Wall->IsSelected(OperationLineStartPosCenter, nullptr)
				|| Wall->IsSelected(OperationLineEndPosCenter, nullptr)
				|| Polygon->IsSelected(Wall->GetStartPos(), nullptr)
				|| Polygon->IsSelected(Wall->GetEndPos(), nullptr))
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("承重墙体与新建墙相交"));
				return false;
			}
		}
	}

	return true;
}

bool FArmyBearingWall::CalBearingInOutwall()
{
	TArray<TWeakPtr<FArmyObject>> WallList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, WallList);
	float MinDis = FLT_MAX;//记录承重墙起止点到外墙的最小距离
	for (auto It : WallList)
	{
		TSharedPtr<FArmyBearingWall> Wall = StaticCastSharedPtr<FArmyBearingWall>(It.Pin());
		if (Wall.IsValid())
		{
			TArray<TSharedPtr<FArmyLine> > Lines;
			Wall->GetLines(Lines);
			for (auto L : Lines)
			{
				float Temp = FArmyMath::CalcPointToLineDistance(OperationLineStartPosCenter, L->GetStart(), L->GetEnd());
				if (Temp < MinDis)
				{
					MinDis = Temp;
				}
				Temp = FArmyMath::CalcPointToLineDistance(OperationLineEndPosCenter, L->GetStart(), L->GetEnd());
				if (Temp < MinDis)
				{
					MinDis = Temp;
				}
			}
		}
	}

	if (MinDis < FArmySceneData::OutWallThickness)
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void FArmyBearingWall::OnHardWareInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		const float LineLength = FCString::Atof(*InText.ToString());
		SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的宽度"));
		SetLength(LineLength / 10.f);
		Modify();
	}
}

void FArmyBearingWall::OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstLineLeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector CurrentDirection = OperationLineStartPos - OperationLineEndPos;
		CurrentDirection = CurrentDirection.GetSafeNormal();

		float TempLength1 = FVector::Distance(CaptureWallInfo.FirstLine->GetStart(), OperationLineStartPos);
		float TempLength2 = FVector::Distance(CaptureWallInfo.FirstLine->GetStart(), OperationLineEndPos);
		if (TempLength1 < TempLength2)
		{
			if (TempLength < 0)
				OperationLineStartPos -= CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineStartPos += CurrentDirection* TempLength;//
		}
		else
		{
			if (TempLength < 0)
				OperationLineEndPos += CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineEndPos -= CurrentDirection* TempLength;
		}

		FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);

		UpdateDashLine();
	}
}

void FArmyBearingWall::OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstLineRightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector CurrentDirection = OperationLineStartPos - OperationLineEndPos;
		CurrentDirection = CurrentDirection.GetSafeNormal();

		float TempLength1 = FVector::Distance(CaptureWallInfo.FirstLine->GetEnd(), OperationLineStartPos);
		float TempLength2 = FVector::Distance(CaptureWallInfo.FirstLine->GetEnd(), OperationLineEndPos);
		if (TempLength1 > TempLength2)
		{
			if (TempLength < 0)
				OperationLineEndPos += CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineEndPos -= CurrentDirection* TempLength;
		}
		else
		{
			if (TempLength < 0)
				OperationLineStartPos -= CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineStartPos += CurrentDirection* TempLength;
		}

		FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);

		UpdateDashLine();
	}
}

void FArmyBearingWall::OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondLineLeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector CurrentDirection = OperationLineStartPos - OperationLineEndPos;
		CurrentDirection = CurrentDirection.GetSafeNormal();
		
		float TempLength1 = FVector::Distance(CaptureWallInfo.SecondLine->GetStart(), OperationLineStartPos);
		float TempLength2 = FVector::Distance(CaptureWallInfo.SecondLine->GetStart(), OperationLineEndPos);

		if (TempLength1 < TempLength2)
		{
			if (TempLength < 0)
				OperationLineStartPos -= CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineStartPos += CurrentDirection* TempLength;
		}
		else
		{
			if (TempLength < 0)
				OperationLineEndPos += CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineEndPos -= CurrentDirection* TempLength;
		}

		FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);

		UpdateDashLine();
	}
}

void FArmyBearingWall::OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondLineRightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector CurrentDirection = OperationLineStartPos - OperationLineEndPos;
		CurrentDirection = CurrentDirection.GetSafeNormal();
		
		float TempLength1 = FVector::Distance(CaptureWallInfo.SecondLine->GetEnd(), OperationLineStartPos);
		float TempLength2 = FVector::Distance(CaptureWallInfo.SecondLine->GetEnd(), OperationLineEndPos);
		if (TempLength1 > TempLength2)
		{
			if (TempLength < 0)
				OperationLineEndPos += CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineEndPos -= CurrentDirection* TempLength;
		}
		else
		{
			if (TempLength < 0)
				OperationLineStartPos -= CurrentDirection*FMath::Abs(TempLength);
			else
				OperationLineStartPos += CurrentDirection* TempLength;
		}

		FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
		FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);

		UpdateDashLine();
	}
}

bool FArmyBearingWall::UpdateOperationCenterPos(const FVector & OperationStart, const FVector & OperationEnd, const FDoubleWallCaptureInfo & WallInfo)
{
	FVector LineDirection = FArmyMath::GetLineDirection(OperationStart, OperationEnd);
	FVector DashLineDirection = LineDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	
	float thickness = WallInfo.Thickness;

	//获取点是否在关联线上
	bool posInFirstLine = FArmyMath::IsPointOnLine(OperationStart, WallInfo.FirstLine->GetStart(), WallInfo.FirstLine->GetEnd());
	bool posInSencondLine = FArmyMath::IsPointOnLine(OperationEnd, WallInfo.SecondLine->GetStart(), WallInfo.SecondLine->GetEnd());

	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() == OT_OutRoom)
	{
		if (FirstRelatedRoom->IsPointInRoom(CaptureWallInfo.FirstLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection))
		{
			DashLineDirection *= -1;
		}
		if (posInFirstLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
		}
		else if (posInSencondLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
		}
	}
	else if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() == OT_OutRoom)
	{
		if (SecondRelatedRoom->IsPointInRoom(CaptureWallInfo.SecondLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection))
		{
			DashLineDirection *= -1;
		}
		if (posInFirstLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
		}
		else if (posInSencondLine)
		{
			OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
			OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
		}
	}
	else
	{
		FVector TempCheckPos = (HardWareRulerLine->GetUpDashLineStart() + HardWareRulerLine->GetUpDashLineEnd()) / 2.f;
		if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
		{
			if (!FirstRelatedRoom->IsPointInRoom(TempCheckPos))
			{
				if (posInFirstLine)
				{
					OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
				}
				else
				{
					OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
				}
			}
			else
			{
				if (posInFirstLine)
				{
					OperationLineStartPosCenter = OperationLineStartPos - (DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos - (DashLineDirection * thickness / 2);
				}
				else
				{
					OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
					OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
				}
			}
		}
		else if (FirstRelatedRoom.IsValid()/* && !FirstRelatedRoom->IsClosed()*/)
		{
			if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
			{
				if (!SecondRelatedRoom->IsPointInRoom(TempCheckPos))
				{
					if (posInSencondLine)
					{
						OperationLineStartPosCenter = OperationLineStartPos + (DashLineDirection * thickness / 2);
						OperationLineEndPosCenter = OperationLineEndPos + (DashLineDirection * thickness / 2);
					}
					else
					{
						OperationLineStartPosCenter = OperationLineStartPos + (-DashLineDirection * thickness / 2);
						OperationLineEndPosCenter = OperationLineEndPos + (-DashLineDirection * thickness / 2);
					}
				}
			}
		}
	}
	return true;
}
