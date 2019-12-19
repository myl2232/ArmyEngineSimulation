#include "ArmyPackPipe.h"
#include "ArmyPolygon.h"
#include "ArmyEngineModule.h"
#include "ArmyStyle.h"
#include "ArmyRulerLine.h"

#include "ArmySceneData.h"
#include "ArmyAxisRuler.h"
#include "ArmyUser.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"

#include "ArmyPrimitive.h"
#include "ArmyClipper.h"

FArmyPackPipe::FArmyPackPipe() 
	: Polygon(MakeShareable(new FArmyPolygon()))
	, Height(FArmySceneData::WallHeight)	
	, PlaneViewPolygon(MakeShareable(new FArmyPolygon()))
	, FillPolygon(MakeShareable(new FArmyPolygon()))
{
	UMaterial* LineMaterial = FArmyEngineModule::Get().GetEngineResource()->GetAddWallMaterial();
	Color = FLinearColor::Green;
	MI_Line = UMaterialInstanceDynamic::Create(LineMaterial, NULL /*GXRVC->GetWorld()->GetCurrentLevel()*/);
	MI_Line->AddToRoot();
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	//Polygon->MaterialRenderProxy = MI_Line->GetRenderProxy(false);
	ObjectType = OT_PackPipe;
	SetPropertyFlag(FLAG_COLLISION, true);
    SetName(TEXT("包立管"));

	SelectedRulerLine = MakeShareable(new FArmyRulerLine());
	SelectedRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyPackPipe::OnLineInputBoxCommittedCommon));

	MI_PlaneView = FArmyEngineModule::Get().GetEngineResource()->GetUnStageBaseColorMaterial();
	MI_PlaneView->AddToRoot();
	MI_PlaneView->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor::White);
	PlaneViewPolygon->MaterialRenderProxy = MI_PlaneView->GetRenderProxy(false);	

	FillPolygon->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial()->GetRenderProxy(false);

	ConstructionItemData = MakeShareable(new FArmyConstructionItemInterface);

	//AxisRuler = MakeShareable(new FArmyAxisRuler());
	//AxisRuler->Init(GVC->ViewportOverlayWidget);

	////@ 添加墙输入框数值越界后输入框内容重置操作
	//AxisRuler->AxisOperationDelegate.BindRaw(this, &FArmyPackPipe::PreCalTransformIllegalCommon);
}

bool FArmyPackPipe::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}
void FArmyPackPipe::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_ADDWALLPOST))
		{
			for (auto line : PolyLines)
			{
				FArmyLine PostLine(line.Get());
				PostLine.DepthPriority = SDPG_Foreground;
				PostLine.SetBaseColor(FLinearColor::White);
				PostLine.Draw(PDI, View);
			}
			for (auto ItLine : ExternalLines)
			{
				ItLine->DepthPriority = SDPG_Foreground;
				ItLine->SetBaseColor(FLinearColor::White);
				ItLine->Draw(PDI, View);
			}
		}
		else
		{
			/**@欧石楠  如果在平面视图下，则进行绘制*/
			if (GetDrawModel(MODE_TOPVIEW) || GetDrawModel(MODE_CEILING))
			{
				//PlaneViewPolygon->SetPolygonOffset(0.2);
				PlaneViewPolygon->Draw(PDI, View);
				FillPolygon->Draw(PDI, View);

				for (auto line : PolyLines)
				{		
					line->bIsDashLine = true;
					line->SetLineColor(FLinearColor::Black);
					line->Draw(PDI, View);
				}

				return;
			}

			if (State == OS_Selected)
			{
				TArray<TWeakPtr<FArmyObject>> objects;
				FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, objects);
				//AxisRuler->Update(GVC, objects);

			}

			bool bSelectedLine = false;
			for (auto line : PolyLines)
			{
				if (line->GetState() == FArmyPrimitive::EOPState::OPS_Selected)
				{
					SelectedLine = line;
					UpdateSeletedRulerLine();
					SelectedRulerLine->Draw(PDI, View);
					bSelectedLine = true;
				}
				
				if (FArmyObject::GetDrawModel(MODE_MODIFYADD))//新增墙模式
				{
					line->DepthPriority = SDPG_Foreground;
					if (FArmyObject::GetDrawModel(MODE_NORAMAL))
					{
						line->DepthPriority = SDPG_World;
					}
				}
				line->bIsDashLine = false;
				if (FArmySceneData::Get()->bIsDisplayDismantle && State != OS_Selected && line->PriState != FArmyPrimitive::OPS_Selected)
					line->SetLineColor(FLinearColor::Green);
				if (!FArmySceneData::Get()->bIsDisplayDismantle)
					line->SetLineColor(FLinearColor::White);
				line->Draw(PDI, View);
			}
			for (auto ItLine : ExternalLines)
			{
				ItLine->DepthPriority = SDPG_Foreground;
				ItLine->SetBaseColor(FLinearColor::White);
				ItLine->Draw(PDI, View);
			}
			if (!FArmySceneData::Get()->bIsDisplayDismantle || !bSelectedLine)
				SelectedRulerLine->ShowInputBox(false);

			Polygon->SetPolygonOffset(0.2);
			Polygon->Draw(PDI, View);
			//AxisRuler->Draw(PDI, View);
		}
	}
}
void FArmyPackPipe::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("vertexes"));
	TArray<FVector>& Vertexes = Polygon->Vertices;
	for (auto& Vertex : Vertexes)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue("Height", GetHeight());
	JsonWriter->WriteValue("Thickness", GetThickness());
	JsonWriter->WriteValue("Length", GetLength());

	JsonWriter->WriteArrayStart(TEXT("insideVertices"));
	for (auto& Vertex : InsideVertices)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteArrayStart(TEXT("outsideVertices"));
	for (auto& Vertex : OutsideVertices)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart(TEXT("ExternalLines"));
	for (auto& It : ExternalLines)
	{
		JsonWriter->WriteValue(It->GetStart().ToString());
		JsonWriter->WriteValue(It->GetEnd().ToString());
	}
	JsonWriter->WriteArrayEnd();

	/**@欧石楠 存储施工项*/
	ConstructionItemData->SerializeToJson(JsonWriter);

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyPackPipe)
}
void FArmyPackPipe::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	TArray<FVector> Vertexes;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("vertexes", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		Vertexes.Add(Vertex);
	}
	SetVertexes(Vertexes, true);
	SetHeight(InJsonData->GetNumberField(TEXT("Height")));
	SetThickness(InJsonData->GetNumberField(TEXT("Thickness")));
	SetLength(InJsonData->GetNumberField(TEXT("Length")));

	VertexesStrArray.Empty();
	InJsonData->TryGetStringArrayField("insideVertices", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		InsideVertices.Add(Vertex);
	}
	VertexesStrArray.Empty();
	InJsonData->TryGetStringArrayField("outsideVertices", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		OutsideVertices.Add(Vertex);
	}

	//读取额外绘制线的点
	VertexesStrArray.Empty();
	TArray<FVector> PointsArray;
	InJsonData->TryGetStringArrayField("ExternalLines", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		PointsArray.Add(Vertex);
	}
	TSharedPtr<FArmyLine> TempLine = MakeShareable(new FArmyLine);
	for (int i = 0; i < PointsArray.Num(); ++i)
	{		
		if (i & 1)
		{
			TempLine->SetEnd(PointsArray[i]);
			ExternalLines.Add(TempLine);
			TempLine = MakeShareable(new FArmyLine);
		}
		else
		{
			TempLine->SetStart(PointsArray[i]);
		}		
	}


	/**@欧石楠 读取施工项*/
	ConstructionItemData->Deserialization(InJsonData);

}
void FArmyPackPipe::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor::Green;
		/*HideAllRulerLines(true);*/
		break;
	case OS_Selected:
		Color = FLinearColor(FColor(0xFFFF9800));		
		break;
	default:
		break;
	}
	MI_Line->SetVectorParameterValue(TEXT("MainColor"), Color);
	for (auto line : PolyLines)
	{
		line->SetBaseColor(Color);
		line->SetNormalColor(Color);
	}
	/*if (State == OS_Selected)
		AxisRuler->SetBoundObj(this->AsShared());
	else
		AxisRuler->SetBoundObj(nullptr);*/
}
bool FArmyPackPipe::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}
	return false;
}

void FArmyPackPipe::GetVertexes(TArray<FVector>& OutVertexes)
{
	OutVertexes.Append(Polygon->Vertices);
}
void FArmyPackPipe::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	OutLines = PolyLines;
}
const FBox FArmyPackPipe::GetBounds()
{
	return FBox(Polygon->Vertices);
}

TSharedPtr<FArmyLine> FArmyPackPipe::HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (auto It : PolyLines)
	{
		//排除掉小于250mm的线段
		if (It->IsSelected(Pos, InViewportClient) /*&& It->Size() > 25*/)
		{
			return It;
		}
	}
	return NULL;
}

void FArmyPackPipe::ApplyTransform(const FTransform& Trans)
{
	TArray<FVector> Points = Polygon->Vertices;
	for (auto& It : Points)
	{
		It = Trans.TransformPosition(It);
	}

	//@ 计算变换是否越界
	if (!CalInternalWallIntersection(Points))
	{
		SetVertexes(Points);
	}
	else
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("输入的数值超出范围"));
	}
}

void FArmyPackPipe::Delete()
{
	FArmyObject::Delete();
	SelectedRulerLine->ShowInputBox(false);
}

bool FArmyPackPipe::IsPointInObj(const FVector & Pos)
{
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, Polygon->Vertices);
}

void FArmyPackPipe::SetVertexes(TArray<FVector>& Vertexes, bool bDeserialization)
{
	if (Vertexes.Num() == 0)
		return;

	PolyLines.Empty();
	TSharedPtr<FArmyEditPoint> PrePoint = MakeShareable(new FArmyEditPoint(Vertexes[0]));
	TSharedPtr<FArmyEditPoint> FirstPoint = PrePoint;
	for (int i = 1; i < Vertexes.Num(); i++)
	{
		TSharedPtr<FArmyEditPoint> CurrentPoint = MakeShareable(new FArmyEditPoint(Vertexes[i]));
		TSharedPtr<FArmyLine> CurrentLine = MakeShareable(new FArmyLine);
		CurrentLine->SetStartPointer(PrePoint);
		CurrentLine->SetEndPointer(CurrentPoint);
		CurrentLine->SetLineWidth(WALLLINEWIDTH);
		//CurrentLine->DepthPriority = SDPG_Foreground;
		PrePoint = CurrentPoint;
		PolyLines.Push(CurrentLine);
	}
	TSharedPtr<FArmyLine> EndLine = MakeShareable(new FArmyLine);
	EndLine->SetStartPointer(PrePoint);
	EndLine->SetEndPointer(FirstPoint);
	EndLine->SetLineWidth(WALLLINEWIDTH);
	PolyLines.Push(EndLine);

	//@欧石楠 把所有的线的基础颜色设为规定颜色
	for (auto line : PolyLines)
	{
		line->SetBaseColor(Color);
		line->SetNormalColor(Color);
		line->SetHoverColor(FLinearColor(FColor(0xFFF55200)));
	}

	UpdatePolyVertices(bDeserialization);
}

TSharedPtr<FArmyLine> FArmyPackPipe::GetParallelLine() const
{
	if (!SelectedLine.IsValid())
		return nullptr;

	FVector SelectedLineCenter = FArmyMath::GetLineCenter(SelectedLine->GetStart(), SelectedLine->GetEnd());
	
	//@ 计算标尺线反方向与选择线最近的相交线
	float BoxMaxLength = 100000;
	TSharedPtr<FArmyLine> OutLine = nullptr;
	if (SelectedLine.IsValid())
	{
		float Distance = FLT_MAX;
		for (auto It : PolyLines)
		{
			if (It != SelectedLine 
				/*&& FArmyMath::AreLinesParallel(It->GetStart(), It->GetEnd(), SelectedLine->GetStart(), SelectedLine->GetEnd())*/
				&& FArmyMath::CalculateLine2DIntersection(It->GetStart(), It->GetEnd(), SelectedLineCenter,SelectedLineCenter + (-SelectedRulerDir) * BoxMaxLength))
			{
				float CurrentDis = FArmyMath::CalcPointToLineDistance(SelectedLineCenter, It->GetStart(), It->GetEnd());
				if (CurrentDis < Distance)
				{
					Distance = CurrentDis;
					OutLine = It;
				}	
			}
		}
	}

	return OutLine;
}

void FArmyPackPipe::UpdatePolyVertices(bool bDeserialization)
{
	TArray<FVector> Points;
	for (int i = 0; i < PolyLines.Num(); i++)
	{
		Points.Push(PolyLines[i]->GetStart());
	}
	Polygon->SetVertices(Points);
	PlaneViewPolygon->SetVertices(Points);
	if (!bDeserialization)
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid() && FArmyMath::IsPointInOrOnPolygon2D(FBox(Points).GetCenter(), Room->GetWorldPoints(true)))
		{
			if (!bDeserialization)
				Room->bPreModified = true;

			Clipper Cli;
			TArray<FVector> RoomPoints = Room->GetWorldPoints(true);
			Paths ClipperPath;
			Path RoomPath;
			for (int i = 0; i < RoomPoints.Num(); i++)
				RoomPath.push_back(IntPoint(RoomPoints[i].X * 1000, RoomPoints[i].Y * 1000));
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptSubject, true);

			ClipperPath.clear();
			RoomPath.clear();
			for (int i = 0; i < Points.Num(); i++)
				RoomPath.push_back(IntPoint(Points[i].X * 1000, Points[i].Y * 1000));
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptClip, true);

			TArray<FVector> InternalPoints;
			Paths ResultPoints;
			if (Cli.Execute(ctDifference, ResultPoints, pftEvenOdd, pftEvenOdd))
			{
				if (ResultPoints.size() != 2)
					return;

				TArray<TArray<FVector>> CurrentPointList;
				for (int i = 0; i < ResultPoints.size(); i++)
				{
					TArray<FVector> CurrentPoints;
					for (int j = 0; j < ResultPoints[i].size(); j++)
					{
						FVector CurrentPoint(ResultPoints[i][j].X / 1000.f, ResultPoints[i][j].Y / 1000.f, 0);
						CurrentPoints.Push(CurrentPoint);
					}
					CurrentPointList.Push(CurrentPoints);
				}
				//找出面积较小的多边形
				if (FArmyMath::CalcPolyArea(CurrentPointList[0]) < FArmyMath::CalcPolyArea(CurrentPointList[1]))
					InternalPoints = CurrentPointList[0];
				else
					InternalPoints = CurrentPointList[1];

				FillPolygon->SetVertices(InternalPoints);
			}

			ResultPoints.clear();
			Cli.Clear();
			Cli.AddPaths(ClipperPath, ptSubject, true);
			ClipperPath.clear();
			RoomPath.clear();
			for (int i = 0; i < InternalPoints.Num(); i++)
				RoomPath.push_back(IntPoint(InternalPoints[i].X * 1000, InternalPoints[i].Y * 1000));
			ClipperPath.push_back(RoomPath);
			Cli.AddPaths(ClipperPath, ptClip, true);
			if (Cli.Execute(ctUnion, ResultPoints, pftEvenOdd, pftEvenOdd))
			{
				if (ResultPoints.size() != 1)
					return;
				for (int j = 0; j < ResultPoints[0].size(); j++)
				{
					FVector CurrentPoint(ResultPoints[0][j].X / 1000.f, ResultPoints[0][j].Y / 1000.f, 0);
					FillOutSideVertices.Push(CurrentPoint);
				}
			}

			break;
		}
	}
}

void FArmyPackPipe::UpdateSeletedRulerLine()
{
	bIsStartOrEndLine = false;
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);

	FVector SelectedLineCenter = FArmyMath::GetLineCenter(SelectedLine->GetStart(), SelectedLine->GetEnd());
	FVector SelectedLineDir = FArmyMath::GetLineDirection(SelectedLine->GetStart(), SelectedLine->GetEnd());
	SelectedRulerDir = SelectedLineDir.RotateAngleAxis(90, FVector(0, 0, 1));

	if (FArmyMath::IsPointInOrOnPolygon2D(SelectedLineCenter + SelectedRulerDir * 5, Polygon->Vertices))
	{
		SelectedRulerDir *= -1;
	}

	//@ 计算选择线是否为末端线
	float VecNum = Polygon->Vertices.Num();
	for (int32 i = 0; i < VecNum; ++i)
	{
		FVector StartLineCenter = FArmyMath::GetLineCenter(Polygon->Vertices[0], Polygon->Vertices[VecNum - 1]);
		FVector EndLineCenter = FArmyMath::GetLineCenter(Polygon->Vertices[VecNum / 2 - 1], Polygon->Vertices[VecNum / 2]);
		if (FVector::Distance(SelectedLineCenter, StartLineCenter) < 1 || FVector::Distance(SelectedLineCenter, EndLineCenter) < 1)
		{
			bIsStartOrEndLine = true;
			break;
		}
	}
	
	TSharedPtr<FArmyLine> NeighborLine;
	if (bIsStartOrEndLine)
	{
		//@ 获取选择线的邻接线
		for (auto L : PolyLines)
		{
			if (L.IsValid() && L != SelectedLine && (L->GetStart() == SelectedLine->GetStart() || L->GetEnd() == SelectedLine->GetStart()))
			{
				NeighborLine = L;
				break;
			}
		}

		//计算标尺线的方向，此计算考虑到末端线段可能与两侧线不垂直，标尺线需要跟两侧线的方向一致
		FVector NeighborLineCenter = FArmyMath::GetLineCenter(NeighborLine->GetStart(), NeighborLine->GetEnd());
		if (NeighborLine->GetStart() == SelectedLine->GetStart() || NeighborLine->GetEnd() == SelectedLine->GetStart())
		{
			SelectedRulerDir = (SelectedLine->GetStart() - NeighborLineCenter).GetSafeNormal();
		}
		else if (NeighborLine->GetStart() == SelectedLine->GetEnd() || NeighborLine->GetEnd() == SelectedLine->GetEnd())
		{
			SelectedRulerDir = (SelectedLine->GetEnd() - NeighborLineCenter).GetSafeNormal();
		}
	}

	float MinRoomLineDis = MAX_flt;
	TSharedPtr<FArmyLine> MinDisRoomLine;
	FVector SelectedLineInRoomInter = FVector::ZeroVector;
	float MaxDis = 100000;
	//遍历查找所有房间中距离选择线最近的内墙线
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid() && Room->IsClosed())
		{
			float dis = MAX_flt;
			TArray<TSharedPtr<FArmyLine>> RoomLines;
			Room->GetLines(RoomLines);
			for (auto Line : RoomLines)
			{
				if (Line.IsValid())
				{
					FVector posInter;
					FVector LDir = (Line->GetStart() - Line->GetEnd()).GetSafeNormal();
					//在标尺线的方向上与墙线求交，避免标尺线穿过新建墙自身标注
					if (FArmyMath::CalculateLinesIntersection(SelectedLineCenter, SelectedLineCenter + SelectedRulerDir * MaxDis, Line->GetStart()/*  + LDir * FArmySceneData::OutWallThickness*/, Line->GetEnd() /*+ (-LDir * FArmySceneData::OutWallThickness)*/, posInter))
					{
						dis = FVector::Distance(SelectedLineCenter, posInter);
						if (dis < MinRoomLineDis)
						{
							SelectedLineInRoomInter = posInter;
							MinRoomLineDis = dis;
							MinDisRoomLine = Line;
						}
					}
				}
			}
		}
	}
	if (SelectedRulerLine.IsValid() && SelectedLineInRoomInter != FVector::ZeroVector)
	{
		SelectedRulerLine->Update(SelectedLineInRoomInter,SelectedLineCenter);
	}
}

bool FArmyPackPipe::CalInternalWallIntersection(TArray<FVector> Points)
{
	if (Points.Num() < 4)
		return false;
	//计算墙线是否与内墙线相交
	//FBox WallBox(Points);
	//bool bRoomInter = true;//是否与内墙线相交，true表示相交，否则不相交
	//TArray<TWeakPtr<FArmyObject>> RoomList;
	//FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	//for (auto It : RoomList)
	//{
	//	TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
	//	TArray<FVector> RoomVec;
	//	if (Room.IsValid() && Room->IsClosed())
	//	{
	//		RoomVec = Room->GetWorldPoints(true);
	//	}
	//	else
	//	{
	//		continue;
	//	}

	//	//计算中心点、包围盒最小最大点是否在同一个房间内
	//	if (FArmyMath::IsPointInOrOnPolygon2D(WallBox.GetCenter(), RoomVec)
	//		&& FArmyMath::IsPointInOrOnPolygon2D(WallBox.Min, RoomVec)
	//		&& FArmyMath::IsPointInOrOnPolygon2D(WallBox.Max, RoomVec))
	//	{
	//		bRoomInter = false;
	//		break;
	//	}
	//}

	TArray<TWeakPtr<FArmyObject>> OutRoomList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_OutRoom, OutRoomList);
	for (auto It : OutRoomList)
	{
		TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (CurrentRoom.IsValid())
		{
			for (auto ItPoint : Points)
			{
				if (!FArmyMath::IsPointInOrOnPolygon2D(ItPoint, CurrentRoom->GetWorldPoints(true)))
				{
					//GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许绘制在户型外"));
					return true;
				}
			}
		}
	}

	TArray<TWeakPtr<FArmyObject>> AddWallList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_InternalRoom, AddWallList);
	for (auto It : AddWallList)
	{
		TSharedPtr<FArmyObject> AddWall = StaticCastSharedPtr<FArmyObject>(It.Pin());
		if (AddWall.IsValid())
		{
			TArray<TSharedPtr<FArmyLine>> AddWallLines;
			AddWall->GetLines(AddWallLines);

			for (auto ItLine : AddWallLines)
			{
				for (int32 i = 0; i < Points.Num(); i++)
				{
					FVector PrePoint = Points[i];
					FVector CurrentPoint = Points[i == Points.Num() - 1 ? 0 : i + 1];
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						PrePoint, CurrentPoint, CurrentIntersect))
					{
						//GGI->Window->ShowMessage(MT_Warning, TEXT("新建墙体不允许和内墙线交叉"));
						return true;
					}
				}
				
			}
		}
	}

	//计算转换是否与其他新建墙相交
	TArray<TWeakPtr<FArmyObject>> AddWall;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_AddWall, AddWall);
	TArray<FVector> WallPoints;
	for (auto W : AddWall)
	{
		TSharedPtr<FArmyPackPipe> wall = StaticCastSharedPtr<FArmyPackPipe>(W.Pin());
		WallPoints.Empty();
		wall->GetVertexes(WallPoints);
		if ((wall.IsValid() && wall != this->AsShared())
			&&(FArmyMath::IsPolygonInPolygon(WallPoints, Points)
				|| FArmyMath::IsPolygonInPolygon(Points,WallPoints)
				|| FArmyMath::ArePolysIntersection(Points,WallPoints)))
		{
			return true;
		}
	}

	//@ 计算是否自相交
	TArray<TSharedPtr<FArmyLine>> OldLines;
	Polygon->GetLines(OldLines);

	TArray<TSharedPtr<FArmyLine>> NewLines = PolyLines;
	if (OldLines.Num() == PolyLines.Num())
	{
		FVector OldNormal = FVector::ZeroVector;
		FVector NewNoraml = FVector::ZeroVector;
		for (int i = 0; i < OldLines.Num(); ++i)
		{
			OldNormal = FArmyMath::GetLineDirection(OldLines[i]->GetStart(), OldLines[i]->GetEnd()).GetSafeNormal();
			NewNoraml = FArmyMath::GetLineDirection(PolyLines[i]->GetStart(), PolyLines[i]->GetEnd()).GetSafeNormal();
			
			//修改后线方向逆向改变视为不合理操作
			float Result = FVector::DotProduct(OldNormal, NewNoraml);
			if (Result < 0)
			{
				return true;//表示相交
			}
		}

		FVector pos;
		for (auto L1 : PolyLines)
		{
			for (auto L2 : PolyLines)
			{
				//修改后线段长度为0也视为操作无效
				if (FMath::Abs(FVector::Dist2D(L1->GetStart(), L1->GetEnd())) < 0.001
					|| FMath::Abs(FVector::Dist2D(L2->GetStart(), L2->GetEnd())) < 0.001)
				{
					return true;
				}

				//存在自相交视为无效操作
				if (FArmyMath::CalculateLinesCross(L1->GetStart(), L1->GetEnd(), L2->GetStart(), L2->GetEnd(), pos))//存在相交，退出循环
				{
					return true;
				}
			}
		}
	}
	else
	{
		return true;
	}

	return false;
}

void FArmyPackPipe::OnLineInputBoxCommitted(float InLength)
{
	float CachedLength = SelectedRulerLine->GetInputBoxCachedLength();
	FVector RulerDir = SelectedRulerDir;
	float Distance = FMath::Abs(CachedLength - InLength);
	if (CachedLength < InLength)
		RulerDir *= -1;

	//@  末端线仅延伸线的长度，不修改其他线的位置
	TSharedPtr<FArmyLine> OtherLine;
	FVector PreSelectedStartPoint;
	FVector PreSelectedEndPoint;
	FVector PreOtherStartPoint;
	FVector PreOtherEndPoint;

	FVector OtherOffsetStart;
	FVector OtherOffsetEnd;
	if (!bIsStartOrEndLine)
	{
		OtherLine = GetParallelLine();
		if (OtherLine.IsValid())
		{
			PreOtherStartPoint = OtherLine->GetStart();
			PreOtherEndPoint = OtherLine->GetEnd();
			OtherOffsetStart = OtherLine->GetStart() + RulerDir * Distance;
			OtherOffsetEnd = OtherLine->GetEnd() + RulerDir * Distance;
			OtherLine->SetStart(OtherOffsetStart);
			OtherLine->SetEnd(OtherOffsetEnd);
		}
	}

	PreSelectedStartPoint = SelectedLine->GetStart();
	PreSelectedEndPoint = SelectedLine->GetEnd();
	FVector SelectedOffsetStart = SelectedLine->GetStart() + RulerDir * Distance;
	FVector SelectedOffsetEnd = SelectedLine->GetEnd() + RulerDir * Distance;
	SelectedLine->SetStart(SelectedOffsetStart);
	SelectedLine->SetEnd(SelectedOffsetEnd);

	//@ 如果尺寸修改后墙线出现自相交，则此次操作无效，恢复操作前状态TArray<FVector> Points;
	TArray<FVector> Points;
	for (int i = 0; i < PolyLines.Num(); i++)
	{
		Points.Push(PolyLines[i]->GetStart());
	}

	if (CalInternalWallIntersection(Points))
	{
		FVector SelectedOffsetStart = SelectedLine->GetStart() - RulerDir * Distance;
		FVector SelectedOffsetEnd = SelectedLine->GetEnd() - RulerDir * Distance;
		SelectedLine->SetStart(SelectedOffsetStart);
		SelectedLine->SetEnd(SelectedOffsetEnd);

		//@ 末端线仅延伸线的长度，不修改其他线的位置
		if (!bIsStartOrEndLine)
		{
			if (OtherLine.IsValid())
			{
				FVector OtherOffsetStart = OtherLine->GetStart() - RulerDir * Distance;
				FVector OtherOffsetEnd = OtherLine->GetEnd() - RulerDir * Distance;
				OtherLine->SetStart(OtherOffsetStart);
				OtherLine->SetEnd(OtherOffsetEnd);
			}
		}

		SelectedRulerLine->GetWidget()->SetInputText(FText::FromString(FString::FromInt(FMath::CeilToInt(CachedLength) * 10) + TEXT(" mm")));

		GGI->Window->ShowMessage(MT_Warning, TEXT("超出新建墙体尺寸范围"));

	}
	else
	{
		//UpdateSeletedRulerLine();
		UpdatePolyVertices();
		for (auto& It : OutsideVertices)
		{
			if (It.Equals(PreOtherStartPoint))
				It = OtherOffsetStart;
			else if (It.Equals(PreOtherEndPoint))
				It = OtherOffsetEnd;
			else if (It.Equals(PreSelectedStartPoint))
				It = SelectedOffsetStart;
			else if (It.Equals(PreSelectedEndPoint))
				It = SelectedOffsetEnd;
		}

		for (auto& It : InsideVertices)
		{
			if (It.Equals(PreOtherStartPoint))
				It = OtherOffsetStart;
			else if (It.Equals(PreOtherEndPoint))
				It = OtherOffsetEnd;
			else if (It.Equals(PreSelectedStartPoint))
				It = SelectedOffsetStart;
			else if (It.Equals(PreSelectedEndPoint))
				It = SelectedOffsetEnd;
		}
	}

	GGI->Window->DismissModalDialog();
	bModified = true;
}

void FArmyPackPipe::OnLineInputBoxCommittedCancel()
{
	SelectedRulerLine->GetWidget()->SetInputText(
		FText::FromString(FString::FromInt(FMath::CeilToInt(SelectedRulerLine->GetInputBoxCachedLength()) * 10) + TEXT(" mm")));
	GGI->Window->DismissModalDialog();
}

void FArmyPackPipe::OnLineInputBoxCommittedCommon(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		bool HasHardware = false;
		TArray<TWeakPtr<FArmyObject>> HardWareList;
		FArmySceneData::Get()->GetHardWareObjects(HardWareList);
		for (auto It : HardWareList)
		{
			TSharedPtr<FArmyHardware> Hardware = StaticCastSharedPtr<FArmyHardware>(It.Pin());
			if (Hardware.IsValid())
			{
				for (auto It : PolyLines)
				{
					if (Hardware->LinkFirstLine == It || Hardware->LinkSecondLine == It)
					{
						HasHardware = true;
						break;
					}
				}
				if (HasHardware)
					break;
			}
		}

		const float LineLength = FCString::Atof(*InText.ToString()) / 10.f;
		if (HasHardware)
		{
			GGI->Window->PresentModalDialog(TEXT("移动墙体后，相关联的门窗数据将删除，是否继续？"), /*SettingWidget->AsShared(),*/
				FSimpleDelegate::CreateRaw(this, &FArmyPackPipe::OnLineInputBoxCommitted, LineLength),
				FSimpleDelegate::CreateRaw(this, &FArmyPackPipe::OnLineInputBoxCommittedCancel), false);
		}
		else
			OnLineInputBoxCommitted(LineLength);		
	}
}

//void FArmyPackPipe::GenerateAllRulerLines()
//{
//	if (AllRulerLines.Num() != PolyLines.Num())
//	{
//		AllRulerLines.Reset();
//		for (auto It : PolyLines)
//		{
//			TSharedPtr<FArmyRulerLine> TempRulerLine = MakeShareable(new FArmyRulerLine);
//			TempRulerLine->SetEnableInputBox(false);
//			TempRulerLine->SetUpdateShowInDraw(true);
//			AllRulerLines.Add(TempRulerLine);
//		}
//	}
//
//	UpdateAllRulerLines();
//}

//void FArmyPackPipe::UpdateAllRulerLines()
//{
//	if (AllRulerLines.Num() == PolyLines.Num())
//	{
//		for (int i = 0; i < AllRulerLines.Num(); ++i)
//		{
//			AllRulerLines[i]->Update(PolyLines[i]->GetStart(), PolyLines[i]->GetEnd(), PolyLines[i]->GetDirectionNormal().RotateAngleAxis(90, FVector(0, 0, 1)) * 10.f);
//		}
//	}	
//}

//void FArmyPackPipe::DrawAllRulerLines(FPrimitiveDrawInterface * PDI, const FSceneView * View)
//{
//	UpdateAllRulerLines();
//	for (auto It : AllRulerLines)
//	{
//		It->Draw(PDI, View);
//	}
//}

//void FArmyPackPipe::HideAllRulerLines(bool bHide)
//{
//	for (auto It : AllRulerLines)
//	{
//		It->ShowInputBox(!bHide);
//	}
//}

void FArmyPackPipe::PreCalTransformIllegal(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag)
{
	bool result = true;
	if (SelectedObj->GetType() == OT_PackPipe)
	{
		TSharedPtr<FArmyPackPipe> wall = StaticCastSharedPtr<FArmyPackPipe>(SelectedObj);
		if (wall.IsValid())
		{
			TArray<FVector> Points;
			TArray<FVector> TempPoints;
			wall->GetVertexes(Points);
			for (auto It : Points)
			{
				TempPoints.Add(trans.TransformPosition(It));
			}

			result = CalInternalWallIntersection(TempPoints) ? true : false;
			if (result)
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("输入的数值超出范围"));
			}
			else
			{
				wall->SetVertexes(TempPoints);
				bModified = true;
			}
		}
	}
}

bool FArmyPackPipe::PreCalTransformIllegalCommon(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag)
{
	PreCalTransformIllegal(SelectedObj, trans, Flag);

	return true;
}

void FArmyPackPipe::GetAreaAndPerimeter(float & OutArea, float & OutPerimeter)
{
	/** @欧石楠 取外墙面积，此计算方式不对*/
	/*TArray<FVector> TempPoints = Polygon->Vertices;
	OutArea = FArmyMath::CalcPolyArea(TempPoints) * 0.0001f;*/

	OutPerimeter = 0;
	for (auto It : PolyLines)
	{
		OutPerimeter += It->Size();
	}	
	OutPerimeter /= 100.f;

	/** @欧石楠 计算外墙面积*/
	int TempCout = PolyLines.Num() > 6 ? 2 : 0;
	OutArea = (OutPerimeter * 100 + TempCout * Thickness) / 2 * Height * 0.0001f;
}

void FArmyPackPipe::SetInsideVertices(TArray<FVector> InVertexes)
{
	InsideVertices = InVertexes;
}

void FArmyPackPipe::SetOutsideVertices(TArray<FVector> InVertexes)
{
	OutsideVertices = InVertexes;
}

void FArmyPackPipe::GetInsideVertices(TArray<FVector>& Vertexes)
{
	Vertexes = InsideVertices;
}

void FArmyPackPipe::GetOutsideVertices(TArray<FVector>& Vertexes)
{
	Vertexes = OutsideVertices;
}

void FArmyPackPipe::GetFillOutsideVertices(TArray<FVector>& Vertexes)
{
	Vertexes = FillOutSideVertices;
}

// 获得立面投影box
TArray<struct FLinesInfo> FArmyPackPipe::GetFacadeBox(TSharedPtr<class FArmyLine> InLine)
{
	TArray<FLinesInfo> FacadeBox;
	if (!InLine.IsValid())
	{
		return FacadeBox;
	}
	TArray<FVector> Intersections;
	TArray<FVector> TmpVertexes;
	// 获得墙线与包围盒的交点
	if (FArmyMath::CalLinePolygonIntersection(InLine->GetStart(), InLine->GetEnd(), OutsideVertices, Intersections))
	{
		if (Intersections.Num() != 2)
		{
			return FacadeBox;
		}
		else
		{
			TmpVertexes = Intersections;
		}
	}
	else
	{
		if (FArmyMath::CalLinePolygonCoincidence(InLine->GetStart(), InLine->GetEnd(), OutsideVertices, Intersections))
		{
			if (Intersections.Num() != 2)
			{
				return FacadeBox;
			}
			else
			{
				TmpVertexes = Intersections;
			}
		}
	}

	if (TmpVertexes.Num() == 2)
	{
		FLinesInfo pack_pipe;
		pack_pipe.Color = FLinearColor(0.0f, 1.0f, 1.0f);
		pack_pipe.DrawType = 0;
		pack_pipe.LineType = 0;

		FVector BaseZ = FVector(0, 0, 1);
		FVector FirstBottom = TmpVertexes[0];
		FVector SecondBottom = TmpVertexes[1];
		FVector FirstTop = FirstBottom + BaseZ*GetHeight();
		FVector SecondTop = SecondBottom + BaseZ*GetHeight();

		pack_pipe.Vertices.Emplace(FirstBottom);
		pack_pipe.Vertices.Emplace(FirstTop);
		pack_pipe.Vertices.Emplace(SecondTop);
		pack_pipe.Vertices.Emplace(SecondBottom);
		FacadeBox.Emplace(pack_pipe);
	}

	return FacadeBox;
}
void FArmyPackPipe::SetExternalLines(TArray<TSharedPtr<FArmyLine>>& InLines)
{
    ExternalLines = InLines;
}