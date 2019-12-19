#include "ArmyRoom.h"
#include "ArmyEngineModule.h"
#include "ArmyStyle.h"
#include "ArmyConstructionQuantity.h"
#include "ArmyPolygon.h"
#include "EngineUtils.h"
#include "ArmyWindow.h"
#include "ArmyPass.h"
#include "ArmySingleDoor.h"
#include "ArmySlidingDoor.h"
#include "ArmyRulerLine.h"
#include "ArmyMath.h"
#include "ArmyResourceModule.h"
#include "ArmyGameInstance.h"
#include "ArmyTrapeBayWindow.h"
#include "ArmyUser.h"
#include "ArmyWallLine.h"
#include "ArmyActorConstant.h"
#include "ArmyDesignEditor.h"
#include "ArmyTextLabel.h"
#include "ArmyActor.h"
const int32 SPACENAMESIZE = 18;

FArmyRoom::FArmyRoom()
{
	bBoundingBox = false;
	ObjectType = OT_InternalRoom;
	SetPropertyFlag(FLAG_PICKPRIMITIVE, true);
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("空间")/* + GetUniqueID().ToString()*/);

	SpaceName = TEXT("未命名");
	SpaceId = 1;

	SelectedRoomSpace = MakeShareable(new FArmyPolygon);
	SelectedRoomSpace->FillColor = FLinearColor(FColor(0xCCFFFFFF));
	SelectedRoomSpace->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetRoomSpaceMaterial()->GetRenderProxy(false);

	RulerLine = MakeShareable(new FArmyRulerLine());
	RulerLine->SetEnableInputBox(false);

	FArmyUser::Get().RoomUniqueId++;
	UniqueIdOfRoom = FArmyUser::Get().RoomUniqueId;
}

FArmyRoom::FArmyRoom(FArmyRoom* Copy)
{

}

bool FArmyRoom::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	/*if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return Polygon->IsSelected(Pos, InViewportClient);
	}*/
	return false;
}
TSharedPtr<FArmyEditPoint> FArmyRoom::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (auto It : Points)
	{
		if (It->IsSelected(Pos, InViewportClient))
		{
			return It;
		}
	}
	return NULL;
}

TSharedPtr<FArmyLine> FArmyRoom::HoverLine(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	for (auto It : Lines)
	{
		if (It->GetCoreLine()->IsSelected(Pos, InViewportClient))
		{
			return It->GetCoreLine();
		}
	}
	return NULL;
}

void FArmyRoom::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_TOPVIEW))
		{
			for (auto It : Lines)
			{
				TSharedPtr<FArmyLine> TopViewLine = MakeShareable(new FArmyLine(It->GetCoreLine().Get()));
				FVector LineStartPos = TopViewLine->GetStart();
				FVector LineEndPos = TopViewLine->GetEnd();
				LineStartPos.Z = LineEndPos.Z = FArmySceneData::WallHeight + 21.f;
				TopViewLine->SetStart(LineStartPos);
				TopViewLine->SetEnd(LineEndPos);
				TopViewLine->SetLineColor(FLinearColor::Black);
				TopViewLine->Draw(PDI, View);
			}
		}
		else
		{
			for (auto line : Lines)
			{
				line->Draw(PDI, View);
			}

			if (GetState() != OS_Selected)
			{
				for (auto It : Points)
				{
					if (It->GetReferenceNum() != 2)
					{
						It->ShowEditPoint = true;
						It->SetBaseColor(FLinearColor::Red);
						It->Draw(PDI, View);
						//删除不必要的线段
						if (FArmySceneData::Get()->bHasOutWall)
						{
							TArray<TSharedPtr<class FArmyLine>> DeleteLines = It->GetReferenceLines();
							TArray<TSharedPtr<class FArmyWallLine>> DeleteWallLines;
							for (auto ItLine : DeleteLines)
							{
								if (ItLine->GetStartPointer()->GetReferenceNum() < 2
									|| ItLine->GetEndPointer()->GetReferenceNum() < 2)
								{
									for (auto ItWallLine : Lines)
									{
										if (ItWallLine->GetCoreLine() == ItLine)
										{
											DeleteWallLines.Add(ItWallLine);
										}
									}
								}
							}
							for (auto ItLine : DeleteWallLines)
							{
								Lines.Remove(ItLine);
							}
							Points.Remove(It);
							break;
						}
					}
					else
					{
						It->ShowEditPoint = false;
						It->SetBaseColor(FLinearColor::White);
					}
				}
			}


			if (GetState() == OS_Selected)
			{
				/*for (auto It : Points)
				{
					It->Draw(PDI, View);
				}*/

				if (IsClosed())
				{
					SelectedRoomSpace->SetVertices(GetWorldPoints(true));
					SelectedRoomSpace->Draw(PDI, View);

					/** @欧石楠 绘制全部墙线的标尺*/
					int index = 0;
					for (auto It : Lines)
					{
						float TempAngle = GetType() == OT_InternalRoom ? 90 : -90;
						FVector LineDirection = FArmyMath::GetLineDirection(It->GetCoreLine()->GetStart(), It->GetCoreLine()->GetEnd());
						FVector DashLineDirection = LineDirection.RotateAngleAxis(TempAngle, FVector(0, 0, 1));
						AllRulerLines[index]->Update(It->GetCoreLine()->GetStart(), It->GetCoreLine()->GetEnd(), DashLineDirection * 30.f);
						if (!IsPointInRoom((It->GetCoreLine()->GetStart() + It->GetCoreLine()->GetEnd()) / 2 + DashLineDirection * 30.f))
						{
							/** @欧石楠 如果标尺不在空间内则转向*/
							AllRulerLines[index]->Update(It->GetCoreLine()->GetStart(), It->GetCoreLine()->GetEnd(), -DashLineDirection * 30.f);
						}

						AllRulerLines[index++]->Draw(PDI, View);
					}					
				}
				RulerLine->ShowInputBox(false);

				for (auto It : LineInputBoxList)
				{
					It.Key->Update(It.Value.LineStart, It.Value.LineEnd);
					It.Key->Draw(PDI, View);
				}
			}
			else
			{
				bool TempShouldBreak = false;
				bool TempHasShowLine = false;
				for (auto line : Lines)
				{
					if (line->GetCoreLine()->GetState() == FArmyPrimitive::EOPState::OPS_Selected
						&& ((line->GetCoreLine()->GetLineEditPoint()->GetState() != FArmyPrimitive::OPS_Hovered)
							&& (line->GetCoreLine()->GetStartPointer()->GetState() != FArmyPrimitive::OPS_Hovered)
							&& (line->GetCoreLine()->GetEndPointer()->GetState() != FArmyPrimitive::OPS_Hovered)))
					{
						TArray<FObjectWeakPtr> TempObjects;
						FArmySceneData::Get()->GetHardWareObjects(TempObjects);
						for (auto ItObject : TempObjects)
						{
							TSharedPtr<FArmyHardware> HardWareObject = StaticCastSharedPtr<FArmyHardware>(ItObject.Pin());
							if (HardWareObject->LinkFirstLine == line->GetCoreLine() || HardWareObject->LinkSecondLine == line->GetCoreLine())
							{
								TempShouldBreak = true;
								break;
							}
						}
						if (!TempShouldBreak)
						{
							float TempAngle = GetType() == OT_InternalRoom ? 90 : -90;
							FVector LineDirection = FArmyMath::GetLineDirection(line->GetCoreLine()->GetStart(), line->GetCoreLine()->GetEnd());
							FVector DashLineDirection = LineDirection.RotateAngleAxis(TempAngle, FVector(0, 0, 1));
							RulerLine->Update(line->GetCoreLine()->GetStart(), line->GetCoreLine()->GetEnd(), DashLineDirection * 30.f);
							RulerLine->Draw(PDI, View);
							TempHasShowLine = true;
							break;
						}
					}
				}
				if (!TempHasShowLine)
				{
					RulerLine->ShowInputBox(false);
				}
			}
		}
	}
}
void FArmyRoom::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue(TEXT("Name"), SpaceName);
	JsonWriter->WriteValue("SpaceId", SpaceId);
	JsonWriter->WriteValue("UniqueIdOfRoom", UniqueIdOfRoom);
	/**@欧石楠 序列化type*/
	FString TempType = GetType() == OT_OutRoom ? "OT_OutRoom" : "OT_InternalRoom";
	JsonWriter->WriteValue(TEXT("ObjType"), TempType);
	JsonWriter->WriteValue(TEXT("IsClosed"), IsClosed());

	JsonWriter->WriteValue(TEXT("OriginGroundHeight"), OriginGroundHeight);
	JsonWriter->WriteValue(TEXT("FinishGroundHeight"), FinishGroundHeight);

	JsonWriter->WriteArrayStart(TEXT("points"));
	for (auto& Point : GetWorldPoints(true))
	{
		JsonWriter->WriteValue(Point.ToString());
	}
	JsonWriter->WriteArrayEnd();

	//序列化WallLine信息
	JsonWriter->WriteArrayStart("wallLines");
	for (auto Line : Lines)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("lineID"), Line->GetUniqueID().ToString());
		JsonWriter->WriteValue(TEXT("lineStart"), Line->GetCoreLine()->GetStart().ToString());
		JsonWriter->WriteValue(TEXT("lineEnd"), Line->GetCoreLine()->GetEnd().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyRoom)
}
void FArmyRoom::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	//清空原有数据
	Points.Empty();
	Lines.Empty();

	FArmyObject::Deserialization(InJsonData);

	SpaceName = InJsonData->GetStringField(TEXT("Name"));
	SpaceId = InJsonData->GetIntegerField("SpaceId");
	UniqueIdOfRoom = InJsonData->GetIntegerField("UniqueIdOfRoom");
	/**@欧石楠 反序列化type*/
	FString TempType = InJsonData->GetStringField(TEXT("ObjType"));	
	OriginGroundHeight = InJsonData->GetNumberField("OriginGroundHeight");
	FinishGroundHeight = InJsonData->GetNumberField("FinishGroundHeight");

	bool IsClose = InJsonData->GetBoolField(TEXT("IsClosed"));
	SetType((TempType == "OT_OutRoom") ? OT_OutRoom : OT_InternalRoom);
	TArray<FVector> WorldPoints;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("points", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Point;
		Point.InitFromString(ArrayIt);
		WorldPoints.Add(Point);
	}

	int32 LineNum = 0;
	if (IsClose || WorldPoints.Num() == 1)
		LineNum = WorldPoints.Num();
	else
		LineNum = WorldPoints.Num() - 1;//为了重做的时候不让最后的点和首个点连线


	for (int i = 0; i < WorldPoints.Num(); i++)
	{
		TSharedPtr<FArmyEditPoint> StartPoint = NULL;
		if (i == 0)
		{
			StartPoint = MakeShareable(new FArmyEditPoint(WorldPoints[i]));
			Points.AddUnique(StartPoint);
		}
		else
			StartPoint = Points[Points.Num() - 1];

		TSharedPtr<FArmyEditPoint> EndPoint = NULL;
		if (i == WorldPoints.Num() - 1)
		{
			if (IsClose)
				EndPoint = Points[0];
			else
				break;
		}
		else
		{
			EndPoint = MakeShareable(new FArmyEditPoint(WorldPoints[i + 1]));
			Points.AddUnique(EndPoint);
		}

		/*TSharedPtr<FArmyWallLine> Line = MakeShareable(new FArmyWallLine(StartPoint, EndPoint));
		Line->SetColor(FLinearColor::Gray);
		Line->SetWidth(WALLLINEWIDTH);
		StartPoint->AddReferenceLine(Line->GetCoreLine());
		EndPoint->AddReferenceLine(Line->GetCoreLine());
		Lines.AddUnique(Line);*/
	}

	TArray<TSharedPtr<FJsonValue> > LinesJson = InJsonData->GetArrayField("wallLines");
	for (auto Line : LinesJson)
	{
		const TSharedPtr<FJsonObject> LJsonObj = Line->AsObject();
		FString LineId = LJsonObj->GetStringField(TEXT("lineID"));
		FString LineStart = LJsonObj->GetStringField(TEXT("lineStart"));
		FString LineEnd = LJsonObj->GetStringField(TEXT("lineEnd"));
		TSharedPtr<FArmyWallLine> NewLine = MakeShareable(new FArmyWallLine);
		NewLine->SetColor(FLinearColor::Gray);
		NewLine->SetWidth(WALLLINEWIDTH);
		NewLine->SetRelateRoom(StaticCastSharedRef<FArmyRoom>(this->AsShared()));

		FGuid LineGUID;
		FGuid::Parse(LineId, LineGUID);
		NewLine->SetUniqueID(LineGUID);
		FVector CurrentStartPoint;
		CurrentStartPoint.InitFromString(LineStart);
		FVector CurrentEndPoint;
		CurrentEndPoint.InitFromString(LineEnd);
		for (auto Point : Points)
		{
			if (CurrentStartPoint.Equals(Point->GetPos()))
			{
				NewLine->GetCoreLine()->SetStartPointer(Point);
				Point->AddReferenceLine(NewLine->GetCoreLine());
			}
			else if (CurrentEndPoint.Equals(Point->GetPos()))
			{
				NewLine->GetCoreLine()->SetEndPointer(Point);
				Point->AddReferenceLine(NewLine->GetCoreLine());
			}	
		}
		Lines.AddUnique(NewLine);
	}
}
void FArmyRoom::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		Color = FLinearColor::Gray;
		break;

	case OS_Hovered:
		Color = FLinearColor::Blue;
		break;

	case OS_Selected:
		Color = FLinearColor::Blue;
		break;

	case OS_Disable:
		Color = FLinearColor::Gray;
		break;

	default:
		break;
	}

	if (InState == OS_Selected && IsClosed() && !GetPropertyFlag(FLAG_MODIFY) && !GetPropertyFlag(FLAG_LAYOUT))
	{
		TArray<TWeakPtr<FArmyObject>> RoomList;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
		for (auto It : RoomList)
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			TArray<TSharedPtr<FArmyLine>> RoomLines;
			if (Room.IsValid() && Room->IsClosed() && Room != this->AsShared())
			{
				Room->GetLines(RoomLines);
				for (auto Line : RoomLines)
					if (Line.IsValid())
						CatchLines.Add(Line, Room);
			}
		}

		UpdateSpacing();
	}
	else
	{
		CatchLines.Empty();
		for (auto It : LineInputBoxList)
		{
			It.Key->ShowInputBox(false);
		}
		LineInputBoxList.Empty();
	}

	/** @欧石楠 选中空间时检查全部线的标尺对象是否就位*/
	if (InState == OS_Selected && IsClosed())
	{
		int checkNum = Lines.Num() - AllRulerLines.Num();
		if (checkNum > 0)
		{
			for (int i = 0; i < checkNum; ++i)
			{
				AllRulerLines.Add(MakeShareable(new FArmyRulerLine));
			}
		}	
		for (auto It : AllRulerLines)
		{
			It->SetEnableInputBox(false);
		}
	}
	else
	{
		for (auto It : AllRulerLines)
		{
			It->ShowInputBox(false);
		}
	}

}
bool FArmyRoom::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (!IsClosed())
		return false;

	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return FArmyMath::IsPointInOrOnPolygon2D(Pos, GetWorldPoints(true));
	}
	return false;
}

void FArmyRoom::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	for (auto It : Lines)
	{
		OutLines.Push(It->GetCoreLine());
	}
}
const FBox FArmyRoom::GetBounds()
{
	FBox TempBox(ForceInitToZero);
	for (auto L : Lines)
	{
		TempBox += L->GetCoreLine()->GetStart();
		TempBox += L->GetCoreLine()->GetEnd();
	}
	return TempBox;
}

const FBox FArmyRoom::GetBounds3D()
{
	FBox Box(EForceInit::ForceInit);
	TArray<FVector> WorldPoints = GetWorldPoints(true);
	for (int32 i = 0; i < WorldPoints.Num(); i++)
	{
		FVector Pos = WorldPoints[i];
		FVector Poz(Pos.X, Pos.Y, Pos.Z + FArmySceneData::WallHeight);
		Box += Pos;
		Box += Poz;
	}
	return Box;
}

void FArmyRoom::RemoveLine(TSharedPtr<FArmyLine> InLine)
{	
    InLine->GetStartPointer()->MinusReferenceLine(InLine);
    InLine->GetEndPointer()->MinusReferenceLine(InLine);
    InLine->GetStartPointer()->SetState(FArmyPrimitive::OPS_Normal);
    InLine->GetEndPointer()->SetState(FArmyPrimitive::OPS_Normal);

    if (InLine->GetStartPointer()->GetReferenceNum() == 0)
    {
        RemovePoint(InLine->GetStartPointer());
    }
    if (InLine->GetEndPointer()->GetReferenceNum() == 0)
    {
        RemovePoint(InLine->GetEndPointer());
    }

	for (auto It : Lines)
	{
		if (It->GetCoreLine() == InLine)
		{
			Lines.Remove(It);
			break;
		}
	}

	//@ 空间不闭合，不绘制空间名称
	if (SpaceNameLabel.IsValid())
	{
		SpaceNameLabel->SetbDrawLabel(IsClosed());
	}

	/** @欧石楠 删除线时移除一个标尺线对象*/
	if (AllRulerLines.Num() > 0)
	{
		AllRulerLines.RemoveAt(AllRulerLines.Num() - 1);
	}	
	
	/** @欧石楠 删除完线段之后如果断成两段不连续的线则变成两个空间*/
	if (Points.Num() > 3)
	{
		int count = 0;
		FVector SecondStartPos;
		FVector SecondEndPos;
		TSharedPtr<FArmyWallLine> BeginLine = nullptr;
		//查找是否存在四个引用线计数为1的点
		for (auto It : Lines)
		{
			if (It->GetCoreLine()->GetStartPointer()->GetReferenceNum() == 1)
			{
				++count;
				if (!BeginLine.IsValid())
				{
					BeginLine = It;
				}
				if (count > 2)
				{
					SecondStartPos = It->GetCoreLine()->GetStartPointer()->GetPos();
				}
			}
			if (It->GetCoreLine()->GetEndPointer()->GetReferenceNum() == 1)
			{
				++count;
				if (!BeginLine.IsValid())
				{
					BeginLine = It;
				}
				if (count > 2)
				{
					SecondEndPos = It->GetCoreLine()->GetEndPointer()->GetPos();
				}
			}			
		}
		//存在四个引用线计数为1的点则意味着可以划分为两个空间
		if (count >= 4)
		{
			int StartIndex = 0;
			int EndIndex = 0;
			//根据前面记录的第二段线的起点和终点去查找对应的点的数据
			for (auto It : Points)
			{
				if (It->GetPos().Equals(SecondStartPos, 0.1f))
				{
					Points.Find(It, StartIndex);
				}
				else if (It->GetPos().Equals(SecondEndPos, 0.1f))
				{
					Points.Find(It, EndIndex);
				}
			}
			//根据数据把这个空间的点都删掉，然后用这些数据重新生成一个空间
			if (StartIndex != EndIndex)
			{
				if (StartIndex > EndIndex)
				{
					int TempIndex = StartIndex;
					StartIndex = EndIndex;
					EndIndex = TempIndex;															
				}
				for (int i = StartIndex; i <= EndIndex; ++i)
				{
					CachedOtherRoomPonits.Add(Points[i]->GetPos());		
					for (auto It : Lines)
					{
						if (It->GetCoreLine()->GetStartPointer() == Points[i] ||
							It->GetCoreLine()->GetEndPointer() == Points[i])
						{
							Lines.Remove(It);
							break;
						}
					}
				}			
				for (int i = EndIndex; i >= StartIndex; --i)
				{										
					RemovePoint(Points[i]);
				}
			}
		}
		else//如果没有重新生成房间，则对剩下的线段和点进行排序
		{
			if (BeginLine.IsValid())
			{
				TArray< TSharedPtr<FArmyWallLine> > TempLines;
				if (BeginLine->GetCoreLine()->GetStartPointer()->GetReferenceNum() != 1)
				{
					TSharedPtr<FArmyEditPoint> TempPoint = BeginLine->GetCoreLine()->GetStartPointer();
					BeginLine->GetCoreLine()->SetStartPointer(BeginLine->GetCoreLine()->GetEndPointer());
					BeginLine->GetCoreLine()->SetEndPointer(TempPoint);					
				}				
				TempLines.Add(BeginLine);
				SortLines(BeginLine, TempLines);

				Lines = TempLines;
				Points.Empty();
				for (auto It : Lines)
				{
					Points.AddUnique(It->GetCoreLine()->GetStartPointer());
					Points.AddUnique(It->GetCoreLine()->GetEndPointer());
				}
			}			
		}
	}	
}

void FArmyRoom::Delete()
{
	FArmyObject::Delete();

	{
		DeleteRelatedHardWareObj();
		DeleteSpaceNameObj();

	}
	 
}

void FArmyRoom::Modify()
{
	FArmyObject::Modify();
	bModified = true;
	bPreModified = true;
}

void FArmyRoom::GetHardwareListRelevance(TArray< TSharedPtr<FArmyHardware> >& OutObjects) const
{
	for (auto ItLine : Lines)
	{
		TArray<FObjectWeakPtr> CurrentObjects;
		ItLine->GetAppendObjects(CurrentObjects);
		for (auto ItObject : CurrentObjects)
		{
			TSharedPtr<FArmyHardware> HWObject = StaticCastSharedPtr<FArmyHardware>(ItObject.Pin());
			if (HWObject.IsValid())
			{
				OutObjects.AddUnique(HWObject);
			}
		}
	}
}

void FArmyRoom::OnDelete()
{
	RulerLine->ShowInputBox(false);

	for (auto It : LineInputBoxList)
	{
		It.Key->ShowInputBox(false);
	}
	LineInputBoxList.Empty();

	/** @欧石楠 隐藏全部标尺*/
	for (auto It : AllRulerLines)
	{
		It->ShowInputBox(false);
	}
}

void FArmyRoom::OnSpaceIDChanged()
{
	TSharedPtr<FArmyRoom> Me = StaticCastSharedRef<FArmyRoom>(this->AsShared());
	SpaceIDChanged.ExecuteIfBound(Me);
}

void FArmyRoom::RemovePoint(TSharedPtr<FArmyEditPoint> InPoint)
{
	Points.Remove(InPoint);
}

void FArmyRoom::AddLine(TSharedPtr<class FArmyWallLine> InLine)
{
	InLine->SetWidth(WALLLINEWIDTH);
	Lines.Push(InLine);
	Points.AddUnique(InLine->GetCoreLine()->GetStartPointer());
	Points.AddUnique(InLine->GetCoreLine()->GetEndPointer());
	
	AllRulerLines.Add(MakeShareable(new FArmyRulerLine));

	/** @欧石楠 进行排序*/
	if (Lines.Num() > 1 && !IsClosed())
	{
		//如果第一条线的起点已经不是起点，说明这次添加的线不是顺序画线
		if (Lines[0]->GetCoreLine()->GetStartPointer()->GetReferenceNum() != 1)
		{
			TArray< TSharedPtr<FArmyWallLine> > TempLines;
			if (InLine->GetCoreLine()->GetStartPointer()->GetReferenceNum() != 1)
			{
				TSharedPtr<FArmyEditPoint> TempPoint = InLine->GetCoreLine()->GetStartPointer();
				InLine->GetCoreLine()->SetStartPointer(InLine->GetCoreLine()->GetEndPointer());
				InLine->GetCoreLine()->SetEndPointer(TempPoint);
				TempLines.Add(InLine);
				SortLines(InLine, TempLines);
				
				Lines = TempLines;
				Points.Empty();
				for (auto It : Lines)
				{
					Points.AddUnique(It->GetCoreLine()->GetStartPointer());
					Points.AddUnique(It->GetCoreLine()->GetEndPointer());
				}
			}
		}
	}
}

bool FArmyRoom::IsHasLine(TSharedPtr<class FArmyWallLine> InLine)
{
	for (auto ItLine : Lines)
	{
		if ((ItLine->GetCoreLine()->GetStartPointer() == InLine->GetCoreLine()->GetStartPointer()
			&& ItLine->GetCoreLine()->GetEndPointer() == InLine->GetCoreLine()->GetEndPointer())
			|| (ItLine->GetCoreLine()->GetStartPointer() == InLine->GetCoreLine()->GetEndPointer()
				&& ItLine->GetCoreLine()->GetEndPointer() == InLine->GetCoreLine()->GetStartPointer()))
		{
			return true;
		}
	}

	return false;
}

TSharedPtr<class FArmyEditPoint> FArmyRoom::GetEditPoint(const FVector& InPoint)
{
	for (auto ItPoint : Points)
	{
		if (ItPoint->GetPos().Equals(InPoint, 0.1f))
			return ItPoint;
	}
	return nullptr;
}

const TArray<TSharedPtr<class FArmyEditPoint>>& FArmyRoom::GetPoints()
{
	return Points;
}

TArray<FVector> FArmyRoom::GetWorldPoints(bool bSort /*= false*/)
{
	TArray<FVector> WorldPoints;
	if (!IsClosed() || !bSort)//如果没闭合或者不排序，直接返回Points
	{
		for (auto& ItPoint : Points)
			WorldPoints.Push(ItPoint->GetPos());
	}
	else
	{
		GenerateSortPoints(WorldPoints);
		if (!FArmyMath::IsClockWise(WorldPoints))
			FArmyMath::ReversePointList(WorldPoints);
	}

	return WorldPoints;
}

bool FArmyRoom::IsClosed()
{
	for (auto It : Points)
	{
		if (It->GetReferenceNum() != 2)
			return false;
	}

	return true;
}

void FArmyRoom::Clear()
{
	Lines.Empty();
	Points.Empty();
}

void FArmyRoom::GetRoomAreaAndPerimeter(float& OutArea, float& OutPerimeter)
{
	TArray<FVector> TempPoints;
	GenerateSortPoints(TempPoints);
	OutArea = FArmyMath::CalcPolyArea(TempPoints) * 0.0001f;

	// 计算周长
	/**@欧石楠 清空周长*/
	OutPerimeter = 0;
	for (auto& It : Lines)
	{
		OutPerimeter += It->GetCoreLine()->Size();
	}
	OutPerimeter /= 100.f;
}


bool FArmyRoom::IsPointInRoom(const FVector& Point)
{
	FVector TempPoint = Point;
	TempPoint.Z = 0;
	bool bNearFloor = FMath::IsNearlyZero(Point.Z, 0.0001f) || Point.Z >= 0.f;
	bool bNearTop = FMath::IsNearlyZero((Point.Z - FArmySceneData::WallHeight), 0.0001f) || Point.Z <= FArmySceneData::WallHeight;
	return FArmyMath::IsPointInOrOnPolygon2D(TempPoint, GetWorldPoints(true)) && (bNearFloor && bNearTop);
}

void FArmyRoom::CollectItemsInRoom(TArray< FArmyGoods>& ArtificialData)
{
	for (TActorIterator<AActor> ActorIt(GVC->GetWorld()); ActorIt; ++ActorIt)
	{
		if (ActorIt->Tags.Contains("MoveableMeshActor") || ActorIt->Tags.Contains("BlueprintVRSActor"))
		{
			if (IsPointInRoom(ActorIt->GetActorLocation()) && !ActorIt->Tags.Contains("HydropowerPipeActor") && !ActorIt->Tags.Contains("HydropowerActor") &&
				!ActorIt->ActorHasTag(XRActorTag::Door) && !ActorIt->ActorHasTag(XRActorTag::OriginalPoint))
			{

				auto XRActor = Cast<AXRActor>((*ActorIt)->GetOwner());

				if (!XRActor)
				{
					continue;
				}
				auto FurnitureObj = StaticCastSharedPtr<FArmyFurniture>(XRActor->GetRelevanceObject().Pin());

				if (!FurnitureObj.IsValid())
				{
					continue;
				}

				int32 ItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(*ActorIt);
				
				FArmyGoods goods;
				goods.GoodsId = ItemID;
				goods.Type = 1;
			
				// 施工项
				TSharedPtr<FArmyConstructionItemInterface> tempConstructoinData = XRConstructionManager::Get()->GetSavedCheckedData(FurnitureObj->GetUniqueID(), FurnitureObj->GetConstructionParameter());
				if (tempConstructoinData.IsValid())
					for (auto It : tempConstructoinData->CheckedId)
					{

						int32 Quotaindex = goods.QuotaData.Add(FArmyQuota());
						FArmyQuota &Quota = goods.QuotaData[Quotaindex];
						Quota.QuotaId = It.Key;
						for (auto PropertyIt : It.Value)
						{
							Quota.QuotaProperties.Add(FArmyPropertyValue(PropertyIt.Key, PropertyIt.Value));
						}

					}

				int32 GoodsIndex;
				if (!ArtificialData.Find(goods, GoodsIndex))
				{
				GoodsIndex = ArtificialData.Add(goods);
				}
				FArmyGoods& Goods = ArtificialData[GoodsIndex];
				//合并施工项数量
				for (int32 i = 0; i < ArtificialData[GoodsIndex].QuotaData.Num(); i++)
				{
					ArtificialData[GoodsIndex].QuotaData[i].Dosage += 1;
				}
				Goods.Param.D +=1;
			
			}
		}
	}
	

}




bool FArmyRoom::CheckHasLine(TSharedPtr<FArmyLine> InLine)
{
	for (auto It : Lines)
	{
		if (InLine == It->GetCoreLine())
		{
			return true;
		}
	}
	return false;
}

void FArmyRoom::SetSpaceName(FString InStr)
{
	if (InStr == SpaceName)
		return;

	SpaceName = InStr;
	if (SpaceNameLabel.IsValid())
	{
		SpaceNameLabel->SetLabelContent(FText::FromString(SpaceName));
	}

	/**@欧石楠 修改房间名称不会引起户型修改*/
	/*if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	else */
	/*if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
		FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();*/
	/*else */
	//拆改后改房间名，不可以引起数据拷贝，注释掉
	/*if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_LayoutModel)
		FArmySceneData::LayOutMode_ModifyMultiDelegate.Broadcast();*/
}

 void FArmyRoom::SetSpaceId(const int32 InSpaceId)
 {
	 if(SpaceId != InSpaceId)
	 { 
		SpaceId = InSpaceId;
	
		OnSpaceIDChanged();
	
	 }
 }


void FArmyRoom::OnLineInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction, TSharedPtr<SWidget> InThisWidget)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		/** 添加一条固定长度的直线 */
		float LineLength = FCString::Atof(*InText.ToString());
		if (LineLength <= 0.f)
		{
			GGI->Window->ShowMessage(MT_Warning, TEXT("空间间距范围必须大于0"));
			return;
		}

		LineLength /= 10.f;
		TSharedPtr<SArmyInputBox> ThisInputBox = StaticCastSharedPtr<SArmyInputBox>(InThisWidget);

		for (auto It : LineInputBoxList)
		{
			if (It.Key->GetWidget() == ThisInputBox)
			{
				SCOPE_TRANSACTION(TEXT("移动空间"));
				Modify();

				float Offset = LineLength - It.Value.PreDistance;
                OffsetRoom(It.Value.MoveDirection, Offset);

				return;
			}
		}
	}
}

void FArmyRoom::UpdateSpacing()
{
	for (auto It : LineInputBoxList)
	{
		It.Key->ShowInputBox(false);
	}
	LineInputBoxList.Empty();

    TArray<FVector> Vertexes;
    GetVertexes(Vertexes);

    for (int32 i = 0; i < Vertexes.Num(); ++i)
    {
        FVector CurrentStart = Vertexes[i];
        FVector CurrentEnd = (i + 1 == Vertexes.Num()) ? Vertexes[0] : Vertexes[i + 1];
        for (auto It2 : CatchLines)
        {
            TSharedPtr<FArmyLine> OtherLine = It2.Key;
            FVector OtherStart = OtherLine->GetStart();
            FVector OtherEnd = OtherLine->GetEnd();
            FVector OtherCenter = (OtherStart + OtherEnd) / 2;
            FVector CenterProjectionPoint = FArmyMath::GetProjectionPoint(OtherCenter, CurrentStart, CurrentEnd);
            FVector OtherToCurrentDirect = (CurrentEnd - CurrentStart).RotateAngleAxis(90.f, FVector(0.f, 0.f, 1.f));
            OtherToCurrentDirect.Normalize();

            float Distance = FVector::Distance(OtherCenter, CenterProjectionPoint);
            //判断间距线是否和区域相交
            if (IsParallel(CurrentStart, CurrentEnd, OtherStart, OtherEnd))
            {
                FSpacingEditData SED;
                SED.PreDistance = Distance;
                SED.MoveDirection = OtherToCurrentDirect;
                TSharedPtr<FArmyRulerLine> CurrentRulerLine = MakeShareable(new FArmyRulerLine());
                CurrentRulerLine->SetOnTextForThisCommittedDelegate(FOnTextForThisCommitted::CreateRaw(this, &FArmyRoom::OnLineInputBoxCommitted));

                FVector ProjectOtherStart = OtherStart;
                FVector ProjectOtherEnd = OtherEnd;
                FVector ProjectCurrentStart = CurrentStart;
                FVector ProjectCurrentEnd = CurrentEnd;

                FVector RulerLineStart, RulerLineEnd;
                //共四种情况
                //      -----------
                //-------------------------  CurrentLine
                if (FArmyMath::GetLineSegmentProjectionPos(CurrentStart, CurrentEnd, ProjectOtherStart)
                    && FArmyMath::GetLineSegmentProjectionPos(CurrentStart, CurrentEnd, ProjectOtherEnd))
                {
                    SED.StartPoint = OtherCenter;
                    RulerLineStart = OtherCenter;
                    RulerLineEnd = CenterProjectionPoint;
                }
                //------------
                //       ---------- CurrentLine 
                else if (FArmyMath::GetLineSegmentProjectionPos(CurrentStart, CurrentEnd, ProjectOtherStart))
                {
                    SED.StartPoint = OtherStart;

                    FVector TestCurrentPoint = CurrentStart;
                    if (FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, TestCurrentPoint))
                    {
                        RulerLineStart = FArmyMath::GetLineCenter(TestCurrentPoint, OtherStart);
                        RulerLineEnd = FArmyMath::GetLineCenter(CurrentStart, ProjectOtherStart);
                    }
                    else
                    {
                        TestCurrentPoint = CurrentEnd;
                        bool TestP = FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, TestCurrentPoint);

                        RulerLineStart = FArmyMath::GetLineCenter(TestCurrentPoint, OtherStart);
                        RulerLineEnd = FArmyMath::GetLineCenter(CurrentEnd, ProjectOtherStart);
                    }
                }
                //     ----------
                //---------- CurrentLine
                else if (FArmyMath::GetLineSegmentProjectionPos(CurrentStart, CurrentEnd, ProjectOtherEnd))
                {
                    SED.StartPoint = OtherEnd;
                    FVector TestCurrentPoint = CurrentStart;
                    if (FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, TestCurrentPoint))
                    {
                        RulerLineStart = FArmyMath::GetLineCenter(OtherEnd, TestCurrentPoint);
                        RulerLineEnd = FArmyMath::GetLineCenter(ProjectOtherEnd, CurrentStart);
                    }
                    else
                    {
                        TestCurrentPoint = CurrentEnd;
                        bool TestP = FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, TestCurrentPoint);

                        RulerLineStart = FArmyMath::GetLineCenter(OtherEnd, TestCurrentPoint);
                        RulerLineEnd = FArmyMath::GetLineCenter(ProjectOtherEnd, CurrentEnd);
                    }
                }
                //--------------------------------
                //         ------------- CurrentLine
                else if (FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, ProjectCurrentStart)
                    && FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, ProjectCurrentEnd))
                {
                    FVector CurrentLineCenter = (CurrentStart + CurrentEnd) / 2;
                    FVector CurrentLineCenterProject = CurrentLineCenter;
                    FArmyMath::GetLineSegmentProjectionPos(OtherStart, OtherEnd, CurrentLineCenterProject);
                    SED.StartPoint = CurrentLineCenterProject;
                    RulerLineStart = CurrentLineCenterProject;
                    RulerLineEnd = CurrentLineCenter;
                }
                else
                    continue;

                bool bIntersect = false;
                TArray<TSharedPtr<FArmyLine>> OtherRoomLines;
                It2.Value->GetLines(OtherRoomLines);
                for (auto ItLine : OtherRoomLines)
                {
                    if (FArmyMath::CalculateLine2DIntersection(ItLine->GetStart(), ItLine->GetEnd(),
                        RulerLineStart, RulerLineEnd) ||
                        //不存在otherroom中的某条线的两个点都在RulerLine上
                        (FArmyMath::IsPointOnLine(ItLine->GetStart(), RulerLineStart, RulerLineEnd)
                            && FArmyMath::IsPointOnLine(ItLine->GetEnd(), RulerLineStart, RulerLineEnd)))
                    {
                        bIntersect = true;
                        break;
                    }
                }
                for (auto ItLine : Lines)
                {
                    if (FArmyMath::CalculateLine2DIntersection(ItLine->GetCoreLine()->GetStart(), ItLine->GetCoreLine()->GetEnd(),
                        RulerLineStart, RulerLineEnd) ||
                        (FArmyMath::IsPointOnLine(ItLine->GetCoreLine()->GetStart(), RulerLineStart, RulerLineEnd)
                            && FArmyMath::IsPointOnLine(ItLine->GetCoreLine()->GetEnd(), RulerLineStart, RulerLineEnd)))
                    {
                        bIntersect = true;
                        break;
                    }
                }

                if (!bIntersect)
                {
                    SED.LineStart = RulerLineStart;
                    SED.LineEnd = RulerLineEnd;
                    LineInputBoxList.Add(CurrentRulerLine, SED);
                }

            }
        }
    }
}

void FArmyRoom::CombineCollinearLines()
{
	for (int i = 0; i < Points.Num(); i++)
	{
		TSharedPtr<FArmyEditPoint> CurrentPoint = Points[i];
		TArray<TSharedPtr<class FArmyLine>> CurrentLines = CurrentPoint->GetReferenceLines();
		if (CurrentLines.Num() != 2)
			continue;
		if (FArmyMath::AreLinesParallel(CurrentLines[0]->GetStart(), CurrentLines[0]->GetEnd(),
			CurrentLines[1]->GetStart(), CurrentLines[1]->GetEnd()))
		{
			CurrentLines[0]->GetStartPointer()->MinusReferenceLine(CurrentLines[0]);
			CurrentLines[0]->GetEndPointer()->MinusReferenceLine(CurrentLines[0]);
			CurrentLines[1]->GetStartPointer()->MinusReferenceLine(CurrentLines[1]);
			CurrentLines[1]->GetEndPointer()->MinusReferenceLine(CurrentLines[1]);
			TArray<TWeakPtr<FArmyWallLine>> DeleteLines;
			for (auto ItLine : Lines)
			{
				if (ItLine->GetCoreLine() == CurrentLines[0]
					|| ItLine->GetCoreLine() == CurrentLines[1])
					DeleteLines.Push(ItLine);
			}
			for (auto ItDeleteLine : DeleteLines)
				Lines.Remove(ItDeleteLine.Pin());

			Points.Remove(CurrentPoint);

			TSharedPtr<FArmyWallLine> NewLine = MakeShareable(new FArmyWallLine);
			if (CurrentPoint == CurrentLines[0]->GetStartPointer())
				NewLine->GetCoreLine()->SetStartPointer(CurrentLines[0]->GetEndPointer());
			else if (CurrentPoint == CurrentLines[0]->GetEndPointer())
				NewLine->GetCoreLine()->SetStartPointer(CurrentLines[0]->GetStartPointer());
			if (CurrentPoint == CurrentLines[1]->GetStartPointer())
				NewLine->GetCoreLine()->SetEndPointer(CurrentLines[1]->GetEndPointer());
			else if (CurrentPoint == CurrentLines[1]->GetEndPointer())
				NewLine->GetCoreLine()->SetEndPointer(CurrentLines[1]->GetStartPointer());
			NewLine->GetCoreLine()->GetStartPointer()->AddReferenceLine(NewLine->GetCoreLine());
			NewLine->GetCoreLine()->GetEndPointer()->AddReferenceLine(NewLine->GetCoreLine());
			AddLine(NewLine);
			NewLine->SetRelateRoom(StaticCastSharedRef<FArmyRoom>(this->AsShared()));

			i = -1;
		}
	}
}

TArray<TSharedPtr<class FArmyWallLine>> FArmyRoom::GetWallLines() const
{
	return Lines;
}

FVector FArmyRoom::GetWallLineNormal(TSharedPtr<class FArmyWallLine> InWallLine)
{
	for (auto It : Lines)
	{
		if (It == InWallLine)
		{
			FVector CurrentNormal = InWallLine->GetDirection() ^ FVector(0, 0, 1);
			FVector OffsetPoint = (InWallLine->GetCoreLine()->GetStart() + InWallLine->GetCoreLine()->GetEnd()) / 2
				+ CurrentNormal;
			if (IsPointInRoom(OffsetPoint))
				return CurrentNormal;
			else
				return -CurrentNormal;
		}
	}

	return FVector::ZeroVector;
}

void FArmyRoom::GetObjectsRelevance(TArray<TWeakPtr<FArmyObject>>& InObjects)
{
	TArray<FVector> RoomPoints = GetWorldPoints(true);

	TArray<TWeakPtr<FArmyObject>> ObjectList;
	// @曾援 梁的放置不受限于房间，所以在房间中去掉对梁的查找
	// FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Beam, ObjectList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Pillar, ObjectList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AirFlue, ObjectList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_PackPipe, ObjectList);
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, ObjectList);
	for (TWeakPtr<FArmyObject> IterCom : ObjectList)
	{
		if (IterCom.Pin()->GetType() == OT_PackPipe)
		{
			TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(IterCom.Pin());
			TArray<FVector> Vertexes;
			PackPipe->GetVertexes(Vertexes);
			//@郭子阳
			//包立管已经和墙融为一体，只要包立管有一点和墙重合就视为相关
			for (auto& Vertice : Vertexes)
			{
				if (FArmyMath::IsPointInOrOnPolygon2D(Vertice, RoomPoints))
				{
					InObjects.AddUnique(IterCom);
					continue;
				}
			}
		}
		else if (IterCom.Pin()->GetType() == OT_IndependentWall)
		{
			TSharedPtr<FArmyIndependentWall> IndependentWall = StaticCastSharedPtr<FArmyIndependentWall>(IterCom.Pin());
			if (FArmyMath::IsPointInOrOnPolygon2D(IndependentWall->GetBasePos(), RoomPoints))
			{
				InObjects.AddUnique(IterCom);
				continue;
			}
		}
		else
		{
			TSharedPtr<FArmySimpleComponent> tempComponent = StaticCastSharedPtr<FArmySimpleComponent>(IterCom.Pin());
			const FVector& centerPos = FVector(tempComponent->GetPos().X, tempComponent->GetPos().Y, 0.0);
			if (FArmyMath::IsPointInOrOnPolygon2D(centerPos, RoomPoints))
			{
				InObjects.AddUnique(IterCom);
				continue;
			}

			TArray<FVector> Vertexes;
			tempComponent->GetVertexes(Vertexes);
			//@郭子阳
			//柱子和风道和墙融为一体，只要/柱子和风道有一点和墙重合就视为相关
			for (auto& Vertice : Vertexes)
			{
				if (FArmyMath::IsPointInOrOnPolygon2D(Vertice, RoomPoints))
				{
					InObjects.AddUnique(IterCom);
					continue;
				}
			}
		}
		
	}
}

void FArmyRoom::GetObjectsRelevanceByObjectVertices(TArray<TWeakPtr<FArmyObject>>& InObjects)
{
	TArray<FVector> RoomPoints = GetWorldPoints(true);

	TArray<TWeakPtr<FArmyObject>> ObjectList;
	FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_Beam, ObjectList);
	for (const auto &Object : ObjectList)
	{
		auto ObjectPtr = Object.Pin();
		switch (ObjectPtr->GetType())
		{
			case OT_Beam:
			{
				auto TempComp = StaticCastSharedPtr<FArmySimpleComponent>(ObjectPtr);
				TArray<FVector> Vertices;
				TempComp->GetVertexes(Vertices);
				if (FArmyMath::ArePolysOverlap(Vertices, RoomPoints))
					InObjects.AddUnique(Object);
				break;
			}
		}
	}
}

bool FArmyRoom::IsParallel(TSharedPtr<FArmyLine> Line1, TSharedPtr<FArmyLine> Line2)
{
    return IsParallel(Line1->GetStart(), Line1->GetEnd(), Line2->GetStart(), Line2->GetEnd());
}

bool FArmyRoom::IsParallel(const FVector& S0, const FVector& E0, const FVector& S1, const FVector& E1)
{
    FVector Direction1 = FArmyMath::GetLineDirection(S0, E0);
    FVector Direction2 = FArmyMath::GetLineDirection(S1, E1);
    if (FMath::IsNearlyEqual(FVector::DotProduct(Direction1, Direction2), 1.f, KINDA_SMALL_NUMBER)
        || FMath::IsNearlyEqual(FVector::DotProduct(Direction1, Direction2), -1.f, KINDA_SMALL_NUMBER))
    {
        return true;
    }

    return false;
}

bool FArmyRoom::OffsetRoom(FVector InDirect, float InDistance)
{
	TArray<FVector> CurrentPoints;
	for (auto& It : GetWorldPoints(true))
	{
		CurrentPoints.Push(It + InDirect * InDistance);
	}

	TArray< TWeakPtr<FArmyObject> > RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid() && Room->IsClosed() && Room != this->AsShared())
		{
            if (FArmyMath::ArePolysOverlap(CurrentPoints, Room->GetWorldPoints(true)))
            {
                GGI->Window->ShowMessage(MT_Warning, TEXT("此操作使空间重叠，移动失败"));
				return false;
            }
		}
	}

    // 判断当前房间偏移后会不会超出外墙
    TArray< TWeakPtr<FArmyObject> > OutRoomList;
    FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, OutRoomList);
    if (OutRoomList.Num() == 1)
    {
        TSharedPtr<FArmyRoom> OutRoom = StaticCastSharedPtr<FArmyRoom>(OutRoomList[0].Pin());
        TArray<FVector> OutRoomVertexes;
        OutRoom->GetVertexes(OutRoomVertexes);

        for (auto It : CurrentPoints)
        {
            if (!FArmyMath::IsPointInPolygon2D(It, OutRoomVertexes))
            {
                GGI->Window->ShowMessage(MT_Warning, TEXT("内墙位置不能超出外墙范围"));
                return false;
            }
        }
    }

	for (auto It : Points)
	{
		It->SetPos(It->GetPos() + InDirect * InDistance);
	}
    UpdateSpacing();

    // 设置完房间位置后，重新更新一下相关门窗的捕捉
    FArmySceneData::Get()->ForceUpdateHardwaresCapturing(SharedThis(this));

	FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();

	return true;
}

bool FArmyRoom::DeleteRelatedObj()
{
	DeleteRelatedInRoomObj();
	DeleteRelatedBearingObj();
	return true;
}

bool FArmyRoom::UpdateSpaceNamePos()
{
	if (GetType() == OT_InternalRoom && IsClosed())
	{
        // @欧石楠 如果更新时发现空间名没生成，则先生成
        if (!SpaceNameLabel.IsValid())
        {
            GenerateSpaceNameLabel();
        }

        // @欧石楠 空间合并需要显示房间名
        SpaceNameLabel->SetbDrawLabel(true);

		FBox RoomBox = GetBounds();
		//先设置标注位置在空间包围盒的中心
		FVector RoomCenter = RoomBox.GetCenter();
		float MinDis = FLT_MAX;
		FVector FirstProjPos = FVector::ZeroVector;
		FVector SecondProjPos = FVector::ZeroVector;
		if (!IsPointInRoom(RoomCenter))
		{
			CalPointToRoomLineMinDis(RoomCenter, FirstProjPos, SecondProjPos);
			SpaceNameLabel->SetWorldPosition((FirstProjPos + SecondProjPos) / 2);

		}
		else
		{
			SpaceNameLabel->SetWorldPosition(RoomCenter);
		}
	}
	return true;
}

bool FArmyRoom::GenerateSpaceNameLabel(EModelType InModelType/* = E_HomeModel*/)
{
    if (GetType() == OT_InternalRoom)
    {
        SpaceNameLabel = MakeShareable(new FArmyTextLabel);
        SpaceNameLabel->SetLabelType(FArmyTextLabel::LT_SpaceName);
        SpaceNameLabel->SetLabelContent(FText::FromString(GetSpaceName()));
        SpaceNameLabel->SetTextSize(SPACENAMESIZE);
        SpaceNameLabel->SetObjTag(SharedThis(this), -1);
        SpaceNameLabel->SetbUseEditPos(false);
        UpdateSpaceNamePos();

        FArmySceneData::Get()->Add(SpaceNameLabel, XRArgument(1).ArgUint32(InModelType));
    }

    return true;
}

bool FArmyRoom::DeleteRelatedHardWareObj()
{
	for (auto ItLine : Lines)
	{
		TArray<FObjectWeakPtr> HardWareObjects;
		FArmySceneData::Get()->GetHardWareObjects(HardWareObjects);
		for (auto ItObject : HardWareObjects)
		{
			TSharedPtr<FArmyHardware> HWObject = StaticCastSharedPtr<FArmyHardware>(ItObject.Pin());
			if (HWObject->LinkFirstLine == ItLine->GetCoreLine() || HWObject->LinkSecondLine == ItLine->GetCoreLine())
			{
				//@欧石楠 撤销重做记录
				FArmySceneData::Get()->Delete(HWObject);
			}
		}
	}
	return true;
}

bool FArmyRoom::DeleteRelatedInRoomObj()
{
	//删除外墙时不删除梁、柱子、风道、点位
	if (GetType() == OT_InternalRoom)
	{
		TArray<FObjectWeakPtr> ObjArr;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_Pillar, ObjArr);
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_Beam, ObjArr);
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_AirFlue, ObjArr);
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_ComponentBase, ObjArr);
		for (auto ObjIt = ObjArr.CreateIterator(); ObjIt; ++ObjIt)
		{
			TWeakPtr<FArmyObject> Obj = *ObjIt;
			if (Obj.IsValid() && Obj.Pin()->GetType() == OT_ComponentBase)
			{
				TSharedPtr<FArmyFurniture> Fur = StaticCastSharedPtr<FArmyFurniture>(Obj.Pin());
				if (Fur.IsValid() && IsPointInRoom(Fur->GetBaseEditPoint()->GetPos()))
				{
					FArmySceneData::Get()->Delete(Fur);
				}
			}
			else
			{
				TSharedPtr<FArmySimpleComponent> SimpleCom = StaticCastSharedPtr<FArmySimpleComponent>(Obj.Pin());
				if (SimpleCom.IsValid() && IsPointInRoom(SimpleCom->GetPos()))
				{
					FArmySceneData::Get()->Delete(SimpleCom);
				}
			}
		}
	}
	return true;
}

bool FArmyRoom::DeleteRelatedBearingObj()
{
	for (auto ItLine : Lines)
	{
		TArray<FObjectWeakPtr> ObjArr;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_BearingWall, ObjArr);
		for (auto ObjIt : ObjArr)
		{
			if (ObjIt.IsValid())
			{
				TSharedPtr<FArmyBearingWall> Bearing = StaticCastSharedPtr<FArmyBearingWall>(ObjIt.Pin());
				if (Bearing->GetWallCaptureInfo().FirstLine == ItLine->GetCoreLine() || Bearing->GetWallCaptureInfo().SecondLine == ItLine->GetCoreLine())
				{
					FArmySceneData::Get()->Delete(Bearing);
				}
			}
		}
	}
	return true;
}

bool FArmyRoom::DeleteSpaceNameObj(bool bModify)
{
	if (SpaceNameLabel.IsValid())
	{
		FArmySceneData::Get()->Delete(SpaceNameLabel);
		if (!bModify)
		{
			UpdateDeleteSpaceName(GetSpaceName());
		}
	}
	return true;
}

bool FArmyRoom::ModifySpaceName(FString InNewSpaceName)
{
	FString OldName = GetSpaceName();
	EModelType ModeType = GGI->DesignEditor->CurrentController->GetDesignModelType();
	int32 MaxIndex = FArmySceneData::Get()->GetMaxOrderBySpaceName(ModeType, InNewSpaceName);
	InNewSpaceName = InNewSpaceName.Right(1).IsNumeric() ? InNewSpaceName.Left(InNewSpaceName.Len() - 1) : InNewSpaceName;
	FString NewName = MaxIndex == 0 ? InNewSpaceName : InNewSpaceName + FString::FromInt(MaxIndex + 1);
	SetSpaceName(NewName);
	UpdateSpaceName(OldName, NewName);
	return true;
}

bool FArmyRoom::UpdateSpaceName(FString ModifyBefore,FString ModifyAfter)
{
	EModelType ModeType = GGI->DesignEditor->CurrentController->GetDesignModelType();
	TArray<FObjectWeakPtr> RoomBeforeArr = FArmySceneData::Get()->GetObjectBySpaceName(ModeType, ModifyBefore);
	TArray<FObjectWeakPtr> RoomAfterArr = FArmySceneData::Get()->GetObjectBySpaceName(ModeType, ModifyAfter);
	
	//将旧的数据的所有编号减1
	FArmySceneData::Get()->CalReduceRoomOrder(RoomBeforeArr);

	return true;
}

bool FArmyRoom::UpdateDeleteSpaceName(FString InSpaceName)
{
	EModelType ModeType = GGI->DesignEditor->CurrentController->GetDesignModelType();
	TArray<FObjectWeakPtr> RoomBeforeArr = FArmySceneData::Get()->GetObjectBySpaceName(ModeType, InSpaceName);

	//将旧的数据的所有编号减1
	FArmySceneData::Get()->CalReduceRoomOrder(RoomBeforeArr);
	return true;
}

/**
* 找到公共点的墙体
*/
void FArmyRoom::GetSamePointWalls(const FVector SamePoint, TArray<TSharedPtr<class FArmyWallLine>>& OutLines)
{
	for (auto It : Lines)
	{
		if ((It->GetCoreLine()->GetStart() - SamePoint).IsNearlyZero()|| (It->GetCoreLine()->GetEnd() - SamePoint).IsNearlyZero())
		{
			OutLines.Push(It);
		}
	}
}

void FArmyRoom::SetWallLinesColor(const FLinearColor InColor)
{
    for (auto It : Lines)
    {
        It->SetColor(InColor);
    }
}

void FArmyRoom::GetRelatedWallLines(TSharedPtr<FArmyWallLine> SourceWallLine, TArray< TSharedPtr<FArmyWallLine> >& OutWallLines)
{
    OutWallLines.Empty();

    for (int32 i = 0; i < Lines.Num() || OutWallLines.Num() < 2; ++i)
    {
        if (SourceWallLine->GetCoreLine()->GetStart().Equals(Lines[i]->GetCoreLine()->GetStart(), 0.01f) ||
            SourceWallLine->GetCoreLine()->GetStart().Equals(Lines[i]->GetCoreLine()->GetEnd(), 0.01f) ||
            SourceWallLine->GetCoreLine()->GetEnd().Equals(Lines[i]->GetCoreLine()->GetStart(), 0.01f) ||
            SourceWallLine->GetCoreLine()->GetEnd().Equals(Lines[i]->GetCoreLine()->GetEnd(), 0.01f))
        {
            OutWallLines.Add(Lines[i]);
        }
    }
}

bool FArmyRoom::CalPointToRoomLineMinDis(FVector Pos, FVector & _FirstProjPos, FVector & _SecondProjPos)
{
	TSharedPtr<FArmyLine> FirstMinDisLine = nullptr;
	TSharedPtr<FArmyLine> SecondMinDisLine = nullptr;
	float FirstMinDis = FLT_MAX;
	float SecondMinDis = FLT_MAX;

	//计算第一最近距离线
	for (auto L : Lines)
	{
		TSharedPtr<FArmyWallLine> WallLine = StaticCastSharedPtr<FArmyWallLine>(L);
		if ( WallLine.IsValid() && WallLine->GetCoreLine().IsValid())
		{
			TSharedPtr<FArmyLine> TempLine = WallLine->GetCoreLine();
			if (FArmyMath::IsPointProjectionOnLineSegment2D(FVector2D(Pos),FVector2D(TempLine->GetStart()), FVector2D(TempLine->GetEnd())))
			{
				float TempPos = FArmyMath::CalcPointToLineDistance(Pos, TempLine->GetStart(), TempLine->GetEnd());
				if (TempPos < FirstMinDis)
				{
					FirstMinDis = TempPos;
					FirstMinDisLine = TempLine;
				}
			}
		}
	}

	FVector FirstProjPos = Pos;
	if (FirstMinDisLine.IsValid() && FArmyMath::GetLineSegmentProjectionPos(FirstMinDisLine->GetStart(), FirstMinDisLine->GetEnd(), FirstProjPos))
	{
		_FirstProjPos = FirstProjPos;
	}

	FBox RoomBox = GetBounds();
	float MaxLength = FVector::Dist2D(RoomBox.Max, RoomBox.Min);
	FVector TestLine = Pos + (FirstProjPos - Pos).GetSafeNormal() * MaxLength;
	//计算第二最近距离线
	for (auto L : Lines)
	{
		TSharedPtr<FArmyWallLine> WallLine = StaticCastSharedPtr<FArmyWallLine>(L);
		if (WallLine.IsValid()  && WallLine->GetCoreLine().IsValid() && WallLine->GetCoreLine() != FirstMinDisLine)
		{
			TSharedPtr<FArmyLine> TempLine = WallLine->GetCoreLine();
			if (FArmyMath::CalculateLine2DIntersection(Pos, TestLine, TempLine->GetStart(), TempLine->GetEnd()))
			{
				float TempPos = FArmyMath::CalcPointToLineDistance(Pos, TempLine->GetStart(), TempLine->GetEnd());
				if (TempPos < SecondMinDis)
				{
					SecondMinDis = TempPos;
					SecondMinDisLine = TempLine;
				}
			}
		}
	}

	FVector SecondProjPos = Pos;
	if (SecondMinDisLine.IsValid() && FArmyMath::GetLineSegmentProjectionPos(SecondMinDisLine->GetStart(), SecondMinDisLine->GetEnd(), SecondProjPos))
	{
		_SecondProjPos= SecondProjPos;
	}
	return true;
}

void FArmyRoom::HiddenRulerLine()
{
	if (RulerLine.IsValid())
	{
		RulerLine->ShowInputBox(false);
	}
}

const TSharedPtr<class FArmyRulerLine>& FArmyRoom::GetRulerLine()
{
	return RulerLine;
}

void FArmyRoom::SortLines(TSharedPtr<class FArmyWallLine> InLine, TArray<TSharedPtr<class FArmyWallLine>>& OutLines)
{
	for (int i = 0; i < Lines.Num(); ++i)
	{
		if (OutLines.Contains(Lines[i]))
		{
			continue;
		}
		if (Lines[i]->GetCoreLine()->GetStartPointer()->GetPos().Equals(InLine->GetCoreLine()->GetEndPointer()->GetPos()))
		{
			OutLines.Add(Lines[i]);
			SortLines(Lines[i], OutLines);
			break;
		}
		else if (Lines[i]->GetCoreLine()->GetEndPointer()->GetPos().Equals(InLine->GetCoreLine()->GetEndPointer()->GetPos()))
		{
			TSharedPtr<FArmyEditPoint> TempPoint = Lines[i]->GetCoreLine()->GetStartPointer();
			Lines[i]->GetCoreLine()->SetStartPointer(Lines[i]->GetCoreLine()->GetEndPointer());
			Lines[i]->GetCoreLine()->SetEndPointer(TempPoint);

			OutLines.Add(Lines[i]);
			SortLines(Lines[i], OutLines);
			break;
		}
	}
}

void FArmyRoom::GenerateSortPoints(TArray<FVector>& InPoints)
{
	if (Lines.Num() == 0)
		return;

	bool IsClose = IsClosed();
	if (!IsClose)
	{
		for (int i = 0; i < Lines.Num(); i++)
		{
			if (Lines[i]->GetCoreLine()->GetStartPointer()->GetReferenceNum() != 2
				|| Lines[i]->GetCoreLine()->GetEndPointer()->GetReferenceNum() != 2)
			{
				Lines.Swap(i, 0);
				break;
			}
		}
	}

	TArray<TWeakPtr<FArmyWallLine>> CurrentLines;
	TWeakPtr<FArmyWallLine> Line = Lines[0];
	CurrentLines.AddUnique(Line);
	FVector CalPoint;
	if (!IsClose)
	{
		if (Line.Pin()->GetCoreLine()->GetStartPointer()->GetReferenceNum() != 2)
		{
			InPoints.Push(Line.Pin()->GetCoreLine()->GetStart());
			CalPoint = Line.Pin()->GetCoreLine()->GetEnd();
		}
		else
		{
			InPoints.Push(Line.Pin()->GetCoreLine()->GetEnd());
			CalPoint = Line.Pin()->GetCoreLine()->GetStart();
		}

	}
	else
	{
		CalPoint = Line.Pin()->GetCoreLine()->GetEnd();
	}
	InPoints.Push(CalPoint);

	int breakCount = 0;
	while (CurrentLines.Num() != Lines.Num())
	{
		++breakCount;
		for (int j = 0; j < Lines.Num(); j++)
		{
			if (!CurrentLines.Contains(Lines[j]))
			{
				TWeakPtr<FArmyWallLine> NextLine = Lines[j];
				if (NextLine.Pin()->GetCoreLine()->GetStart().Equals(CalPoint))
				{
					CalPoint = NextLine.Pin()->GetCoreLine()->GetEnd();
					InPoints.Push(CalPoint);
					CurrentLines.AddUnique(NextLine);
					break;
				}
				else if (NextLine.Pin()->GetCoreLine()->GetEnd().Equals(CalPoint))
				{
					CalPoint = NextLine.Pin()->GetCoreLine()->GetStart();
					InPoints.Push(CalPoint);
					CurrentLines.AddUnique(NextLine);
					break;
				}
				else if (NextLine.Pin()->GetCoreLine()->GetStart().Equals(NextLine.Pin()->GetCoreLine()->GetEnd()))//TODO:进入此条件属于Bug，还未复现
				{
					InPoints.Push(CalPoint);
					CurrentLines.AddUnique(NextLine);
					break;
				}
			}
		}
		//用于强行中断错误的死循环
		if (breakCount > 2 * Lines.Num())
		{
			break;
		}
	}
}
 void FArmyRoom::GetVertexes(TArray<FVector>& OutVertexes) 
 {
	 OutVertexes =GetWorldPoints(true);
 }