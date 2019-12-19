#include "ArmyModifyWall.h"
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
#include "ArmyHttpModule.h"

FArmyModifyWall::FArmyModifyWall()
	:Polygon(MakeShareable(new FArmyPolygon()))
	, FirstLineLeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, FirstLineRightRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondLineLeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondLineRightRulerLine(MakeShareable(new FArmyRulerLine()))
	, Height(FArmySceneData::WallHeight)
	, HardWareRulerLine(MakeShareable(new FArmyRulerLine()))

{
	HardWareRulerLine->SetEnableInputBox(false);
	RulerOffsetDis = 30.f;
    SetName(TEXT("拆改墙")/* + GetUniqueID().ToString()*/);
	ObjectType = OT_ModifyWall;
	//UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetLineMaterial();
	Color = FLinearColor(FColor(0xFFE305E5));
	//MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL /*GXRVC->GetWorld()->GetCurrentLevel()*/);
	//MI_Line->AddToRoot();
	//Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);

	FirstLineLeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyModifyWall::OnFirstLineLInputBoxCommitted));
	FirstLineRightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyModifyWall::OnFirstLineRInputBoxCommitted));
	SecondLineLeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyModifyWall::OnSecondLineLInputBoxCommitted));
	SecondLineRightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyModifyWall::OnSecondLineRInputBoxCommitted));

	/**@欧石楠 请求材质商品信息*/
	ReqMaterialSaleID();

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);
}

void FArmyModifyWall::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("CurrentWallType", int32(CurrentWallType));
	JsonWriter->WriteValue(TEXT("operationLineStart"), OperationLineStartPos.ToString());
	JsonWriter->WriteValue(TEXT("operationLineEnd"), OperationLineEndPos.ToString());
	JsonWriter->WriteArrayStart(TEXT("vertexes"));
	TArray<FVector>& Vertexes = Polygon->Vertices;
	for (auto& Vertex : Vertexes)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue("MatType", GetMatType().ToString());
	JsonWriter->WriteValue("Height", GetHeight());
	JsonWriter->WriteValue("FloorHeight", GetFloorHeight());

	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyModifyWall)
}
void FArmyModifyWall::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	if (InJsonData->TryGetNumberField("CurrentWallType", CurrentWallType))
	{
		if (CurrentWallType == 0)
		{
			UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetModifyWallMaterial();
			MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
			MI_Line->AddToRoot();
			MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
			Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
		}
		else if (CurrentWallType == 1)
		{
			UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetMainWallMaterial();
			MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL);
			MI_Line->AddToRoot();
			Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
		}
	}
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

	SetMatType(FText::FromString(InJsonData->GetStringField(TEXT("MatType"))));
	SetHeight(InJsonData->GetNumberField(TEXT("Height")));
	SetFloorHeight(InJsonData->GetNumberField(TEXT("FloorHeight")));	

	/**@欧石楠 读取施工项*/
	ConstructionItemData->Deserialization(InJsonData);

	UpdateBorderLine();
}
void FArmyModifyWall::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor(FColor(0xFFE305E5));
		break;
	case OS_Selected:
		Color = FLinearColor(FColor(0xFFFF9800));
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

void FArmyModifyWall::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_DELETEWALLPOST))
		{
			for (auto L : BorderLines)
			{
				L->Draw(PDI, View);
			}
		}
		else
		{
			TArray < TSharedPtr<FArmyLine>> lines;
			Polygon->GetLines(lines);
			for (auto line : lines)
			{
				//@欧石楠 临时处理让拆除墙显示在最上面盖住门洞的空洞
				line->SetBaseColor(Color);
				line->SetLineWidth(1.5f);
				FVector TempV = line->GetStart();
				TempV.Z = 3.f;
				line->SetStart(TempV);
				TempV = line->GetEnd();
				TempV.Z = 3.f;
				line->SetEnd(TempV);
				line->SetLineWidth(WALLLINEWIDTH + 0.1F);
				line->Draw(PDI, View);
			}
			Polygon->Draw(PDI, View);
		}

		if (State == OS_Selected)
		{
			UpdateDashLine();
			//绘制自身标尺
			HardWareRulerLine->Draw(PDI, View);

			if (FirstRelatedRoom.IsValid() && !(FirstRelatedRoom->GetType() == OT_OutRoom))
			{
				FirstLineLeftRulerLine->Draw(PDI, View);
				FirstLineRightRulerLine->Draw(PDI, View);
			}
			else
			{
				FirstLineLeftRulerLine->ShowInputBox(false);
				FirstLineRightRulerLine->ShowInputBox(false);
			}
			if (SecondRelatedRoom.IsValid() && !(SecondRelatedRoom->GetType() == OT_OutRoom))
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

bool FArmyModifyWall::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}

bool FArmyModifyWall::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmyModifyWall::GetVertexes(TArray<FVector>& OutVertexes)
{
	OutVertexes.Append(Polygon->Vertices);
}

void FArmyModifyWall::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	Polygon->GetLines(OutLines);
}

const FBox FArmyModifyWall::GetBounds()
{
	return FBox(Polygon->Vertices);
}

void FArmyModifyWall::Delete()
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

void FArmyModifyWall::SetMaterial(UMaterial* InMaterial)
{
    MI_Line = UMaterialInstanceDynamic::Create(InMaterial, NULL /*GXRVC->GetWorld()->GetCurrentLevel()*/);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
}

void FArmyModifyWall::SetVertexes(TArray<FVector>& Vertexes)
{
	Polygon->SetVertices(Vertexes);
	UpdateBorderLine();
	UpdateCombineHardwares();
}
void FArmyModifyWall::AddBorderLine(TSharedPtr<FArmyLine> InLine)
{
	BorderLines.AddUnique(InLine);
}
void FArmyModifyWall::Generate(UWorld* InWorld)
{
}

bool FArmyModifyWall::IsPointInObj(const FVector & Pos)
{
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, Polygon->Vertices);
}

TArray<FVector> FArmyModifyWall::GetClipingBox()
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

bool FArmyModifyWall::CaptureDoubleLine(const FVector& Pos, FDoubleWallCaptureInfo& OutInfo)
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

void FArmyModifyWall::SetOperationLinePos(const FVector& InStartPos, const FVector& InEndPos)
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
	}
		
}

void FArmyModifyWall::UpdateDashLine()
{	
	FVector LineDirection = FArmyMath::GetLineDirection(OperationLineStartPos, OperationLineEndPos);
	FVector DashLineDirection = LineDirection.RotateAngleAxis(90, FVector(0, 0, 1));	
	//--------------------控件标尺线更新--------------------

	float thickness = CaptureWallInfo.Thickness;

	//获取点是否在关联线上
	bool posInFirstLine = FArmyMath::IsPointOnLine(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	bool posInSencondLine = FArmyMath::IsPointOnLine(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());

	//@  绘制自身标尺
	HardWareRulerLine->Update(OperationLineStartPos, OperationLineEndPos, DashLineDirection * 5);
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
				HardWareRulerLine->Update(OperationLineStartPos + (-DashLineDirection * thickness / 2), OperationLineEndPos + (-DashLineDirection * thickness / 2), -1 * DashLineDirection * RulerOffsetDis);
			}
		}
	}

	//--------------------第一临墙标尺线更新--------------------
	if (CaptureWallInfo.FirstLine.IsValid())
	{
		if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
		{
			if (!FirstRelatedRoom->IsPointInRoom(CaptureWallInfo.FirstLine->GetLineEditPoint()->GetPos() + 2 * DashLineDirection))
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
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd())));
			//后半段
			FirstLineRightRulerLine->Update(OperationLineEndPos, CaptureWallInfo.FirstLine->GetEnd(),
				DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd())), DashLineDirection * RulerOffset);
		}
		else
		{
			//前半段
			FirstLineLeftRulerLine->Update(CaptureWallInfo.FirstLine->GetStart(), OperationLineEndPos,
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd())));
			//后半段
			FVector TempLeftOffset = DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd()));
			FirstLineRightRulerLine->Update(OperationLineStartPos, CaptureWallInfo.FirstLine->GetEnd(),
				TempLeftOffset, DashLineDirection * RulerOffset);
		}
	}

	//--------------------第二临墙标尺线更新--------------------
	if (CaptureWallInfo.SecondLine.IsValid())
	{
		if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
		{
			if (!SecondRelatedRoom->IsPointInRoom(CaptureWallInfo.SecondLine->GetLineEditPoint()->GetPos() + 2 * DashLineDirection))
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
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())));
			//后半段
			SecondLineRightRulerLine->Update(OperationLineEndPos, CaptureWallInfo.SecondLine->GetEnd(),
				DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())), DashLineDirection * RulerOffset);
		}
		else
		{
			//前半段
			SecondLineLeftRulerLine->Update(CaptureWallInfo.SecondLine->GetStart(), OperationLineEndPos,
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())));
			//后半段
			SecondLineRightRulerLine->Update(OperationLineStartPos, CaptureWallInfo.SecondLine->GetEnd(),
				DashLineDirection * (RulerOffset + FArmyMath::Distance(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd())), DashLineDirection * RulerOffset);
		}
	}	
}
void FArmyModifyWall::SetMatType(FText InStr)
{
	{ MatType = InStr; }
	/**@欧石楠 因为后台使用前台写死数据 暂时硬处理*/
	if (MatType.EqualTo(FText::FromString(TEXT("红砖"))))
	{
		SelectedMaterialID = 1;
	}
	else if (MatType.EqualTo(FText::FromString(TEXT("轻体砖"))))
	{
		SelectedMaterialID = 2;
	}
	else if (MatType.EqualTo(FText::FromString(TEXT("轻体砖保温填充材质"))))
	{
		SelectedMaterialID = 3;
	}
	else if (MatType.EqualTo(FText::FromString(TEXT("保温板+石膏板保温"))))
	{
		SelectedMaterialID = 4;
	}
	else if (MatType.EqualTo(FText::FromString(TEXT("轻钢龙骨隔墙"))))
	{
		SelectedMaterialID = 5;
	}
	else if (MatType.EqualTo(FText::FromString(TEXT("大芯板"))))
	{
		SelectedMaterialID = 6;
	}

	/**@欧石楠 请求材质商品信息*/
	ReqMaterialSaleID();
}
void FArmyModifyWall::SetLength(float InValue)
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
	UpdateBorderLine();
	UpdateCombineHardwares();
}
void FArmyModifyWall::SetHeight(float InValue)
{
	Height = InValue;
}
void FArmyModifyWall::SetFloorHeight(float InValue)
{
	FloorHeight = InValue;
}
void FArmyModifyWall::ReqMaterialSaleID()
{
    FString Url = FString::Printf(TEXT("/api/brick/%d"), SelectedMaterialID);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyModifyWall::ResMaterialSaleID);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}
void FArmyModifyWall::ResMaterialSaleID(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        int32 Id = JObject->GetIntegerField("id");
        FString Name = JObject->GetStringField("name");

        MaterialSaleID = Id;
    }
}

void FArmyModifyWall::GetAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
    TArray < TSharedPtr<FArmyLine> > Lines;
    Polygon->GetLines(Lines);

    // 计算周长
    for (auto It : Lines)
    {
        OutPerimeter += It->Size();
    }

    // 计算墙中线长度，周长减去两侧宽度，得到内外墙总长度，除以二得到墙中线长度
    float InnerWallLength = (OutPerimeter - CaptureWallInfo.Thickness * 2.f) / 2.f;
    OutArea = InnerWallLength * Height / 10000.f;
    OutPerimeter /= 100.f;
}

void FArmyModifyWall::ResetCalPolygonVertices()
{
	FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
	FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());

	TArray<FVector> PrePoints = Polygon->Vertices;
	TArray<FVector> CurrentPoints;
	CurrentPoints.Push(FirstLineStartProject);
	CurrentPoints.Push(FirstLineEndProject);
	CurrentPoints.Push(SecondLineEndProject);
	CurrentPoints.Push(SecondLineStartProject);
	Polygon->SetVertices(CurrentPoints);

	UpdateDashLine();
	UpdateBorderLine();

	UpdateCombineHardwares();
}

void FArmyModifyWall::UpdateBorderLine()
{
	BorderLines.Empty();

    if (!CaptureWallInfo.FirstLine.IsValid() || !CaptureWallInfo.SecondLine.IsValid())
        return;

	FVector FirstLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FVector FirstLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.FirstLine->GetStart(), CaptureWallInfo.FirstLine->GetEnd());
	FVector SecondLineStartProject = FArmyMath::GetProjectionPoint(OperationLineStartPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());
	FVector SecondLineEndProject = FArmyMath::GetProjectionPoint(OperationLineEndPos, CaptureWallInfo.SecondLine->GetStart(), CaptureWallInfo.SecondLine->GetEnd());

	TSharedPtr<FArmyLine> BorderLine1 = MakeShareable(new FArmyLine(FirstLineStartProject, SecondLineStartProject));
	TSharedPtr<FArmyLine> BorderLine2 = MakeShareable(new FArmyLine(FirstLineEndProject, SecondLineEndProject));
	TSharedPtr<FArmyLine> BorderLine3 = MakeShareable(new FArmyLine(FirstLineStartProject, FirstLineEndProject));
	TSharedPtr<FArmyLine> BorderLine4 = MakeShareable(new FArmyLine(SecondLineStartProject, SecondLineEndProject));
	BorderLine1->SetBaseColor(CaptureWallInfo.FirstLine->GetBaseColor());
	BorderLine1->SetLineWidth(WALLLINEWIDTH);
	BorderLine2->SetBaseColor(CaptureWallInfo.FirstLine->GetBaseColor());
	BorderLine2->SetLineWidth(WALLLINEWIDTH);
	BorderLine3->SetBaseColor(GVC->GetBackgroundColor());
	BorderLine3->SetLineWidth(WALLLINEWIDTH + 0.1);
	BorderLine4->SetBaseColor(GVC->GetBackgroundColor());
	BorderLine4->SetLineWidth(WALLLINEWIDTH + 0.1);

	AddBorderLine(BorderLine1);
	AddBorderLine(BorderLine2);
	AddBorderLine(BorderLine3);
	AddBorderLine(BorderLine4);
}

void FArmyModifyWall::UpdateCombineHardwares(bool InbGenerate)
{
	TArray<TWeakPtr<FArmyObject>> HardWareList;
	FArmySceneData::Get()->GetHardWareObjects(HardWareList);
	for (auto It : HardWareList)
	{
		TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		if (IsSelected(Hardware->GetStartPos(), nullptr) || IsSelected(Hardware->GetEndPos(), nullptr)
			|| Hardware->RectImagePanel->IsSelected(OperationLineStartPos, nullptr)
			|| Hardware->RectImagePanel->IsSelected(OperationLineEndPos, nullptr))
			Hardware->bGenerate3D = false;
		else
			Hardware->bGenerate3D = true;
	}
}

bool FArmyModifyWall::IsSuccessful()
{
	TArray<TWeakPtr<FArmyObject>> ModifyWallList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_ModifyWall, ModifyWallList);

	bool OnModifyLine = false;
	TArray<TWeakPtr<FArmyObject>> AddWallList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, RoomList);
	TArray<TSharedPtr<FArmyLine>> RoomLines;
	for (auto ItRoom : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(ItRoom.Pin());
		Room->GetLines(RoomLines);
	}
	for (auto It : AddWallList)
	{
		TSharedPtr<FArmyObject> AddWall = StaticCastSharedPtr<FArmyObject>(It.Pin());
		if (AddWall.IsValid())
		{
			TArray<TSharedPtr<FArmyLine>> AddWallLines;
			AddWall->GetLines(AddWallLines);
			for (auto ItLine : AddWallLines)
			{
				for (auto RoomLine : RoomLines)
				{
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						RoomLine->GetStart(), RoomLine->GetEnd(), CurrentIntersect))
					{
						//判断交点是否在拆改墙线上
						for (auto TempIt : ModifyWallList)
						{
							TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(TempIt.Pin());
							if (ModifyWall.IsValid())
							{
								TArray< TSharedPtr<FArmyLine> > ModifyLines;
								ModifyWall->GetLines(ModifyLines);
								for (auto TempItLine : ModifyLines)
								{
									if (FArmyMath::IsPointOnLine(CurrentIntersect, TempItLine->GetStart(), TempItLine->GetEnd()))
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
							GGI->Window->ShowMessage(MT_Warning, TEXT("此编辑会引起新建墙和原始墙交叉"));
							return false;
						}
					}
				}		
			}
		}
	}

	//再判断新建墙是否在墙体内,并不在拆除墙范围内，则错误提示
	for (auto AddWallIt : AddWallList)
	{
		TArray<FVector> CurrentPoints;
		FBox CurrentAddWallBox = AddWallIt.Pin()->GetBounds();
		FVector MinPoint = CurrentAddWallBox.Min;
		FVector MaxPoint = CurrentAddWallBox.Max;
		bool bInRoom = false;
		TArray<TWeakPtr<FArmyObject>> TempRoomList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, TempRoomList);
		for (auto It : TempRoomList)
		{
			TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (CurrentRoom.IsValid())
			{
				if (FArmyMath::IsPointInOrOnPolygon2D(MinPoint, CurrentRoom->GetWorldPoints(true))
					&& FArmyMath::IsPointInOrOnPolygon2D(MaxPoint, CurrentRoom->GetWorldPoints(true)))
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
					if (FArmyMath::IsPointInOrOnPolygon2D(MinPoint, ModifyPoints)
						&& FArmyMath::IsPointInOrOnPolygon2D(MaxPoint, ModifyPoints))
					{
						bInModify = true;
						break;
					}
				}
			}

			if (!bInModify/* && !OnModifyLine*/)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("此编辑会引起新建墙和原始墙重合"));
				return false;
			}
		}
	}
	
	return true;
}

void FArmyModifyWall::OnFirstLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		FVector PreOperationLineStartPos = OperationLineStartPos;
		FVector PreOperationLineEndPos = OperationLineEndPos;
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
		
		TArray<FVector> PrePoints = Polygon->Vertices;
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);
		if (!IsSuccessful())
		{
			Polygon->SetVertices(PrePoints);
			FirstLineLeftRulerLine->GetWidget()->SetInputText(FText::FromString(FString::FromInt(FMath::CeilToInt(CachedLength) * 10) + TEXT(" mm")));
			OperationLineStartPos = PreOperationLineStartPos;
			OperationLineEndPos = PreOperationLineEndPos;
			return;
		}

		UpdateDashLine();
		UpdateBorderLine();
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

		UpdateCombineHardwares();
	}
}

void FArmyModifyWall::OnFirstLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		FVector PreOperationLineStartPos = OperationLineStartPos;
		FVector PreOperationLineEndPos = OperationLineEndPos;
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

		TArray<FVector> PrePoints = Polygon->Vertices;
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);
		if (!IsSuccessful())
		{
			Polygon->SetVertices(PrePoints);
			FirstLineRightRulerLine->GetWidget()->SetInputText(FText::FromString(FString::FromInt(FMath::CeilToInt(CachedLength) * 10) + TEXT(" mm")));
			OperationLineStartPos = PreOperationLineStartPos;
			OperationLineEndPos = PreOperationLineEndPos;
			return;
		}

		UpdateDashLine();
		UpdateBorderLine();
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

		UpdateCombineHardwares();
	}
}

void FArmyModifyWall::OnSecondLineLInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		FVector PreOperationLineStartPos = OperationLineStartPos;
		FVector PreOperationLineEndPos = OperationLineEndPos;
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
		
		TArray<FVector> PrePoints = Polygon->Vertices;
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);
		if (!IsSuccessful())
		{
			Polygon->SetVertices(PrePoints);
			SecondLineLeftRulerLine->GetWidget()->SetInputText(FText::FromString(FString::FromInt(FMath::CeilToInt(CachedLength) * 10) + TEXT(" mm")));
			OperationLineStartPos = PreOperationLineStartPos;
			OperationLineEndPos = PreOperationLineEndPos;
			return;
		}

		UpdateDashLine();
		UpdateBorderLine();
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

		UpdateCombineHardwares();
	}
}

void FArmyModifyWall::OnSecondLineRInputBoxCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		FVector PreOperationLineStartPos = OperationLineStartPos;
		FVector PreOperationLineEndPos = OperationLineEndPos;
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
		TArray<FVector> PrePoints = Polygon->Vertices;
		
		TArray<FVector> CurrentPoints;
		CurrentPoints.Push(FirstLineStartProject);
		CurrentPoints.Push(FirstLineEndProject);
		CurrentPoints.Push(SecondLineEndProject);
		CurrentPoints.Push(SecondLineStartProject);
		Polygon->SetVertices(CurrentPoints);
		if (!IsSuccessful())
		{
			Polygon->SetVertices(PrePoints);
			SecondLineRightRulerLine->GetWidget()->SetInputText(FText::FromString(FString::FromInt(FMath::CeilToInt(CachedLength) * 10) + TEXT(" mm")));
			OperationLineStartPos = PreOperationLineStartPos;
			OperationLineEndPos = PreOperationLineEndPos;
			return;
		}

		UpdateDashLine();
		UpdateBorderLine();
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

		UpdateCombineHardwares();
	}
}
