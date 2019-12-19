#include "ArmyHardware.h"
#include "ArmyEditPoint.h"
#include "ArmyResourceModule.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyEngineModule.h"
#include "Math/XRMath.h"
#include "ArmyViewportClient.h"
#include "ArmyRulerLine.h"
#include "ArmyRoom.h"
#include "ArmyGameInstance.h"
#include "ArmyWallLine.h"
#include "Runtime/Engine/Classes/PhysicsEngine/AggregateGeom.h"
#include "Runtime/Engine/Classes/PhysicsEngine/BodySetup.h"

FArmyHardware::FArmyHardware()
	: StartPoint(MakeShareable(new FArmyEditPoint()))
	, EndPoint(MakeShareable(new FArmyEditPoint()))
	, Point(MakeShareable(new FArmyEditPoint()))
	, DirectionPoint(MakeShareable(new FArmyEditPoint()))
	, Length(0)
	, Width(0)
	, Height(0)
	, Direction(FVector(0, 1, 0))
	, HorizontalDirection(ForceInitToZero)
	, bRightOpen(true)
	, HardWareRulerLine(MakeShareable(new FArmyRulerLine()))
	, FirstLineLeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, FirstLineRightRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondLineLeftRulerLine(MakeShareable(new FArmyRulerLine()))
	, SecondLineRightRulerLine(MakeShareable(new FArmyRulerLine()))
	, bShowDashLine(false)
	, RulerOffset(30)
	, HeightToFloor(0.f)
{
	SetPos(FVector(-999999.f, -999999.f, 0.f));

	HardWareRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyHardware::OnHardWareInputBoxCommitted));
	FirstLineLeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyHardware::OnFirstLineLInputBoxCommitted));
	FirstLineRightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyHardware::OnFirstLineRInputBoxCommitted));
	SecondLineLeftRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyHardware::OnSecondLineLInputBoxCommitted));
	SecondLineRightRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyHardware::OnSecondLineRInputBoxCommitted));

	//用于覆盖原始墙线或者新建墙做断墙效果的材质
	RectImagePanel = MakeShareable(new FArmyRect());
	RectImagePanel->SetBaseColor(GVC->GetBackgroundColor());
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1f;
	UMaterial* M_BaseColor = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/XRCommon/Material/M_BaseColor.M_BaseColor"), NULL, LOAD_None, NULL));
	UMaterialInstanceDynamic* MI_BaseColor = UMaterialInstanceDynamic::Create(M_BaseColor, nullptr);
	MI_BaseColor->SetVectorParameterValue("BaseColor", GVC->GetBackgroundColor());
	MI_BaseColor->AddToRoot();
	RectImagePanel->MaterialRenderProxy = MI_BaseColor->GetRenderProxy(false);
	RectImagePanel->bIsFilled = false;
	
	LightMapID = FGuid();
}

void FArmyHardware::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("InWallType", InWallType);
	JsonWriter->WriteValue("bIsFilled", RectImagePanel->bIsFilled);
	JsonWriter->WriteValue("HeightToFloor", HeightToFloor);
	JsonWriter->WriteValue("Length", Length);
	JsonWriter->WriteValue("Width", Width);
	JsonWriter->WriteValue("Height", Height);
}

void FArmyHardware::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	if (GetPropertyFlag(FArmyObject::FLAG_MODIFY))
	{
		HardWareRulerLine->SetEnableInputBox(false);
		FirstLineLeftRulerLine->SetEnableInputBox(false);
		FirstLineRightRulerLine->SetEnableInputBox(false);
		SecondLineLeftRulerLine->SetEnableInputBox(false);
		SecondLineRightRulerLine->SetEnableInputBox(false);

		StartPoint->SetState(FArmyPrimitive::OPS_Disable);
		EndPoint->SetState(FArmyPrimitive::OPS_Disable);
		Point->SetState(FArmyPrimitive::OPS_Disable);
		DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
	}
	InWallType = InJsonData->GetIntegerField("InWallType");
	RectImagePanel->bIsFilled = InJsonData->GetBoolField("bIsFilled");
	HeightToFloor = InJsonData->GetNumberField("HeightToFloor");
	Height = InJsonData->GetNumberField("Height");
	Length = InJsonData->GetNumberField("Length");
	Width = InJsonData->GetNumberField("Width");
	bIsCopy = true;
}

void FArmyHardware::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (State == OS_Selected)
		{
			if (bShowEditPoint)
			{
				StartPoint->Draw(PDI, View);
				EndPoint->Draw(PDI, View);
				Point->Draw(PDI, View);
				DirectionPoint->Draw(PDI, View);
			}

			/**@欧石楠 绘制各种标尺*/
			if (bShowDashLine) {
				UpdateDashLine();

				HardWareRulerLine->Draw(PDI, View);
				/** @欧石楠 如果控件是放在原始墙体*/
				if (InWallType == 0)
				{
					if (FirstRelatedRoom.IsValid() && (!(FirstRelatedRoom->GetType() == OT_OutRoom) || bShowOutWallDashLine))
					{
						FirstLineLeftRulerLine->Draw(PDI, View);
						FirstLineRightRulerLine->Draw(PDI, View);
					}
					else
					{
						FirstLineLeftRulerLine->ShowInputBox(false);
						FirstLineRightRulerLine->ShowInputBox(false);
					}
					if (SecondRelatedRoom.IsValid() && (!(SecondRelatedRoom->GetType() == OT_OutRoom) || bShowOutWallDashLine))
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
				else//控件放在新建墙上
				{
					FirstLineLeftRulerLine->Draw(PDI, View);
					FirstLineRightRulerLine->Draw(PDI, View);					
				}
			}
		}

		if (BrokenWallLeftLine.IsValid())
		{
			BrokenWallLeftLine->Draw(PDI, View);
			BrokenWallRightLine->Draw(PDI, View);
		}
	}
}

TSharedPtr<FArmyEditPoint> FArmyHardware::SelectPoint(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	/*@欧石楠 这里将选中的点存下来**/
	ResultPoint = NULL;

	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		DeselectPoints();
	}

	if (GetState() == OS_Selected)
	{
		if (StartPoint->IsSelected(Pos, InViewportClient) && StartPoint->GetState() != FArmyEditPoint::OPS_Disable)            ResultPoint = StartPoint;
		else if (EndPoint->IsSelected(Pos, InViewportClient) && EndPoint->GetState() != FArmyEditPoint::OPS_Disable)         ResultPoint = EndPoint;
		else if (Point->IsSelected(Pos, InViewportClient) && Point->GetState() != FArmyEditPoint::OPS_Disable)            ResultPoint = Point;
		else if (DirectionPoint->IsSelected(Pos, InViewportClient) && DirectionPoint->GetState() != FArmyEditPoint::OPS_Disable)   ResultPoint = DirectionPoint;
		if (ResultPoint.IsValid())                  ResultPoint->SetState(FArmyEditPoint::OPS_Selected);
	}

	return ResultPoint;
}

TSharedPtr<FArmyEditPoint> FArmyHardware::HoverPoint(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	/*@欧石楠 这里将选中的点存下来**/
	ResultPoint = NULL;

	if (StartPoint->GetState() != FArmyEditPoint::OPS_Selected && StartPoint->GetState() != FArmyEditPoint::OPS_Disable && StartPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = StartPoint;
		OperationPointType = 1;
	}
	else if (EndPoint->GetState() != FArmyEditPoint::OPS_Selected && EndPoint->GetState() != FArmyEditPoint::OPS_Disable && EndPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = EndPoint;
		OperationPointType = 2;
	}
	else if (Point->GetState() != FArmyEditPoint::OPS_Selected && Point->GetState() != FArmyEditPoint::OPS_Disable && Point->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = Point;
		OperationPointType = 3;
	}
	else if (DirectionPoint->GetState() != FArmyEditPoint::OPS_Selected && DirectionPoint->GetState() != FArmyEditPoint::OPS_Disable && DirectionPoint->IsSelected(Pos, InViewportClient))
	{
		ResultPoint = DirectionPoint;
		OperationPointType = 4;
	}

	return ResultPoint;
}

void FArmyHardware::GetVertexes(TArray<FVector>& OutVertexes)
{
	FVector UpLeft = StartPoint->GetPos() + Direction * Width / 2.f;
	FVector UpRight = EndPoint->GetPos() + Direction * Width / 2.f;
	FVector DownRight = UpRight - Direction * Width;
	FVector DownLeft = UpLeft - Direction * Width;

	OutVertexes.Add(UpLeft);
	OutVertexes.Add(UpRight);
	OutVertexes.Add(DownRight);
	OutVertexes.Add(DownLeft);
}

void FArmyHardware::Delete()
{
	FArmyObject::Delete();
	/////////////////////////////解除和门窗关联的墙线/////////////////////////////////////////////
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(CurrentModelType, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(CurrentModelType, OT_OutRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			TArray<TSharedPtr<class FArmyWallLine>> WallLines = Room->GetWallLines();
			TArray<FObjectWeakPtr> CurrentObjects;
			for (auto ItLine : WallLines)
			{
				if (ItLine->GetCoreLine() == LinkFirstLine || ItLine->GetCoreLine() == LinkSecondLine)
				{
					ItLine->RemoveAppendObject(this->AsShared());
					break;//同一个房间只可能有同一个关联的门窗
				}
			}
		}
	}
}

void FArmyHardware::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyObject::SetPropertyFlag(InType, InUse);
	if (InType == FArmyObject::FLAG_MODIFY && InUse)
	{
		HardWareRulerLine->SetEnableInputBox(false);
		FirstLineLeftRulerLine->SetEnableInputBox(false);
		FirstLineRightRulerLine->SetEnableInputBox(false);
		SecondLineLeftRulerLine->SetEnableInputBox(false);
		SecondLineRightRulerLine->SetEnableInputBox(false);

		StartPoint->SetState(FArmyPrimitive::OPS_Disable);
		EndPoint->SetState(FArmyPrimitive::OPS_Disable);
		Point->SetState(FArmyPrimitive::OPS_Disable);
		DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
	}	
}

void FArmyHardware::GetCadLineArray(TArray<TPair<FVector, FVector>>& OutLineList) const
{
	if (RectImagePanel.IsValid())
	{
		//计算Rect各边线段
		const int32& VerticesNum = this->RectImagePanel->GetVertices().Num();
		for (int32 Index = 0; Index < VerticesNum;++Index)
			OutLineList.AddUnique(TPair<FVector, FVector>(RectImagePanel->GetVertices()[Index%VerticesNum], RectImagePanel->GetVertices()[(Index + 1) % VerticesNum]));
	}
}

void FArmyHardware::Update()
{
	if (bRightOpen)
	{
		HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
	}
	else
	{
		HorizontalDirection = Direction.RotateAngleAxis(-90, FVector(0, 0, 1));
	}

	StartPoint->SetPos(GetPos() - HorizontalDirection * Length / 2.f);
	EndPoint->SetPos(GetPos() + HorizontalDirection * Length / 2.f);

	DirectionPoint->SetPos(GetEndPos() + Direction * Length / 2.f);

	//更新断墙线的数据
	if (BrokenWallLeftLine.IsValid())
	{
		FVector CurrentHorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
		FVector CurrentVerticalDirection = CurrentHorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
		BrokenWallLeftLine->SetStart(FVector(GetStartPos().X, GetStartPos().Y, 2) + CurrentVerticalDirection * Width / 2.0f);
		BrokenWallLeftLine->SetEnd(FVector(GetStartPos().X, GetStartPos().Y, 2) - CurrentVerticalDirection * Width / 2.0f);
		BrokenWallRightLine->SetStart(FVector(GetEndPos().X, GetEndPos().Y, 2) + CurrentVerticalDirection * Width / 2.0f);
		BrokenWallRightLine->SetEnd(FVector(GetEndPos().X, GetEndPos().Y, 2) - CurrentVerticalDirection * Width / 2.0f);
	}
}

void FArmyHardware::UpdateStartPoint()
{
	if (bRightOpen)
	{
		HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
	}
	else
	{
		HorizontalDirection = Direction.RotateAngleAxis(-90, FVector(0, 0, 1));
	}

	Length = (StartPoint->GetPos() - EndPoint->GetPos()).Size();
	FVector TempPos = EndPoint->GetPos() - HorizontalDirection * Length / 2.f;
	Point->SetPos(TempPos);
	Update();
}

void FArmyHardware::UpdateEndPoint()
{
	if (bRightOpen)
	{
		HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
	}
	else
	{
		HorizontalDirection = Direction.RotateAngleAxis(-90, FVector(0, 0, 1));
	}

	Length = (StartPoint->GetPos() - EndPoint->GetPos()).Size();
	FVector TempPos = StartPoint->GetPos() + HorizontalDirection * Length / 2.f;
	Point->SetPos(TempPos);
	Update();
}

void FArmyHardware::OnDelete()
{
	HardWareRulerLine->ShowInputBox(false);
	FirstLineLeftRulerLine->ShowInputBox(false);
	FirstLineRightRulerLine->ShowInputBox(false);
	SecondLineLeftRulerLine->ShowInputBox(false);
	SecondLineRightRulerLine->ShowInputBox(false);
}

void FArmyHardware::Modify()
{
	FArmyObject::Modify();
	bModified = true;
}

FVector FArmyHardware::CalcActorDefaultSize(AActor* InActor)
{
	FVector Result = FVector::ZeroVector;

	if (InActor->ActorHasTag("MoveableMeshActor") || InActor->ActorHasTag("BlueprintVRSActor"))
	{
		FVRSObject* VRSObj = FArmyResourceModule::Get().GetResourceManager()->GetObjFromObjID(InActor->GetObjID());
		if (VRSObj->GetSkeletalMeshObject())
		{
			Result = VRSObj->BoundExtent * 2.f;
		}
		else
		{
			AStaticMeshActor* SMA = Cast<AStaticMeshActor>(InActor);
			if (SMA)
			{
				FKAggregateGeom& agg = SMA->GetStaticMeshComponent()->GetBodySetup()->AggGeom;
				FBox AggGeomBox = agg.CalcAABB(FTransform(FVector(0, 0, 0)));
				Result = AggGeomBox.GetSize();
			}
		}
	}

	return Result;
}

bool FArmyHardware::IsPointInObj(const FVector & Pos)
{
	return FArmyMath::IsPointInOrOnPolygon2D(Pos, RectImagePanel->GetVertices());
}

const FVector& FArmyHardware::GetStartPos()
{
	return StartPoint->GetPos();
}

const FVector& FArmyHardware::GetEndPos()
{
	return EndPoint->GetPos();
}

const FVector& FArmyHardware::GetPos()
{
	return Point->GetPos();
}

const FVector& FArmyHardware::GetDirectionPointPos()
{
    return DirectionPoint->GetPos();
}

void FArmyHardware::SetPos(const FVector& InPos)
{
	if (Point->GetPos() != InPos)
	{
		Point->SetPos(InPos);
		Update();
	}
}

void FArmyHardware::SetDirection(const FVector& InDirection)
{
	if (!Direction.Equals(InDirection, KINDA_SMALL_NUMBER))
	{
		Direction = InDirection;
		Update();
	}
}

void FArmyHardware::SetLength(const float& InLength)
{
	float TempValue = InLength;
	if (TempValue < 1)
	{
		TempValue = 1;
	}
	if (Length != TempValue)
	{
		Length = TempValue;
		Update();
	}
}

void FArmyHardware::SetWidth(const float& InWidth)
{
	float TempValue = InWidth;
	if (TempValue < 1)
	{
		TempValue = 1;
	}
	if (Width != TempValue)
	{
		Width = TempValue;
		Update();
	}
}

void FArmyHardware::SetHeight(const float& InHeight)
{
	float TempValue = InHeight;
	if (TempValue < 1)
	{
		TempValue = 1;
	}
	if (Height != TempValue)
	{
		Height = TempValue;
		Update();
	}
}

void FArmyHardware::SetRightOpen(bool InbRightOpen)
{
	if (bRightOpen != InbRightOpen)
	{
		bRightOpen = InbRightOpen;
		Update();
	}
}

const void FArmyHardware::GetAlonePoints(TArray< TSharedPtr<FArmyPoint> >& OutPoints)
{

	FVector TempHorizontalDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
	FVector VerticalDirection = TempHorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	OutPoints.Push(MakeShareable(new FArmyPoint(GetStartPos() + VerticalDirection * Width / 2)));
	OutPoints.Push(MakeShareable(new FArmyPoint(GetStartPos() - VerticalDirection * Width / 2)));
	OutPoints.Push(MakeShareable(new FArmyPoint(GetEndPos() + VerticalDirection * Width / 2)));
	OutPoints.Push(MakeShareable(new FArmyPoint(GetEndPos() - VerticalDirection * Width / 2)));
}

uint8 FArmyHardware::GetOperationPointType() const
{
	return OperationPointType;
}

void FArmyHardware::SetOperationPointType(uint8 Type)
{
	OperationPointType = Type;
}

TSharedPtr<FArmyRoom> FArmyHardware::GetRelatedRoomByLine(TSharedPtr<FArmyLine> CheckedLine, EModelType InModelType)
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

void FArmyHardware::DeselectPoints()
{
	StartPoint->SetState(StartPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	EndPoint->SetState(EndPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	Point->SetState(Point->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);
	DirectionPoint->SetState(DirectionPoint->GetState() != FArmyPrimitive::OPS_Disable ? FArmyPrimitive::OPS_Normal : FArmyPrimitive::OPS_Disable);

	HideRulerLineInputBox(true);
}

void FArmyHardware::HideRulerLineInputBox(bool bHide)
{
	HardWareRulerLine->GetWidget()->RefreshWithCachedLength();
	FirstLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
	FirstLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
	SecondLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
	SecondLineRightRulerLine->GetWidget()->RefreshWithCachedLength();

	HardWareRulerLine->ShowInputBox(!bHide);
	FirstLineLeftRulerLine->ShowInputBox(!bHide);
	FirstLineRightRulerLine->ShowInputBox(!bHide);
	SecondLineLeftRulerLine->ShowInputBox(!bHide);
	SecondLineRightRulerLine->ShowInputBox(!bHide);
}

void FArmyHardware::UpdateDashLine()
{
	FVector TempCenterPos = GetPos();
	TempCenterPos.Z = 0;
	FVector TempStartPos = GetStartPos();
	TempStartPos.Z = 0;
	FVector TempEndPos = GetEndPos();
	TempEndPos.Z = 0;

	/**@欧石楠 更新虚线标注*/
	FVector LineDirection = FArmyMath::GetLineDirection(TempStartPos, TempEndPos);
	FVector DashLineDirection = LineDirection.RotateAngleAxis(90, FVector(0, 0, 1));	

	//--------------------控件标尺线更新--------------------	
	HardWareRulerLine->Update(TempStartPos, TempEndPos, DashLineDirection * RulerOffset);
	FVector TempCheckPos = (HardWareRulerLine->GetUpDashLineStart() + HardWareRulerLine->GetUpDashLineEnd()) / 2.f;
	if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
	{
		if (!FirstRelatedRoom->IsPointInRoom(TempCheckPos))
		{
			HardWareRulerLine->Update(TempStartPos, TempEndPos, -1 * DashLineDirection * RulerOffset);
		}
	}
	else if (FirstRelatedRoom.IsValid() && !FirstRelatedRoom->IsClosed())
	{
		if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
		{
			if (!SecondRelatedRoom->IsPointInRoom(TempCheckPos))
			{
				HardWareRulerLine->Update(TempStartPos, TempEndPos, -1 * DashLineDirection * RulerOffset);
			}
		}
	}

	/** @欧石楠 如果是在新建墙上*/
	if (InWallType == 1)
	{
		if (LinkFirstLine.IsValid() && LinkSecondLine.IsValid())
		{
			//在新建墙上的控件，只显示first标尺线，如果捕捉顺序出现问题，则进行交换
			float TempLength1 = FArmyMath::CalcPointToLineDistance(HardWareRulerLine->GetUpDashLineStart(), LinkFirstLine->GetStart(), LinkFirstLine->GetEnd());
			float TempLength2 = FArmyMath::CalcPointToLineDistance(HardWareRulerLine->GetUpDashLineStart(), LinkSecondLine->GetStart(), LinkSecondLine->GetEnd());
			if (TempLength1 > TempLength2)
			{
				TSharedPtr<FArmyLine> TempLine = LinkFirstLine;
				LinkFirstLine = LinkSecondLine;
				LinkSecondLine = TempLine;
			}
		}				
	}

	//--------------------第一临墙标尺线更新--------------------
	if (LinkFirstLine.IsValid())
	{
		if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
		{
			if ((!FirstRelatedRoom->IsPointInRoom(LinkFirstLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && FirstRelatedRoom->GetType() == OT_InternalRoom) ||
				(FirstRelatedRoom->IsPointInRoom(LinkFirstLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && FirstRelatedRoom->GetType() == OT_OutRoom))
			{
				DashLineDirection *= -1;
			}
		}
		else if (FirstRelatedRoom.IsValid() && !FirstRelatedRoom->IsClosed())
		{
			if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
			{
				if (SecondRelatedRoom->IsPointInRoom(TempCenterPos + (5 + 1.5f * Width) * DashLineDirection))
				{
					DashLineDirection *= -1;
				}
			}
		}
		DashLineUpDir = DashLineDirection;
		GetStartNearestPoint(LinkFirstLine, FirstDestinationStart);
		GetEndNearestPoint(LinkFirstLine, FirstDestinationEnd);
		float TempLength1 = FVector::Distance(FirstDestinationStart, TempStartPos);
		float TempLength2 = FVector::Distance(FirstDestinationStart, TempEndPos);
		if (TempLength1 < TempLength2)
		{
			//前半段
			FirstLineLeftRulerLine->Update(FirstDestinationStart, TempStartPos,
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(TempStartPos, FirstDestinationStart, FirstDestinationEnd)));
			//后半段
			FirstLineRightRulerLine->Update(TempEndPos, FirstDestinationEnd,
				DashLineDirection * (RulerOffset + FArmyMath::Distance(TempEndPos, FirstDestinationStart, FirstDestinationEnd)), DashLineDirection * RulerOffset);
		}
		else
		{
			//前半段
			FirstLineLeftRulerLine->Update(FirstDestinationStart, TempEndPos,
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(TempEndPos, FirstDestinationStart, FirstDestinationEnd)));
			//后半段
			FirstLineRightRulerLine->Update(TempStartPos, FirstDestinationEnd,
				DashLineDirection * (RulerOffset + FArmyMath::Distance(TempStartPos, FirstDestinationStart, FirstDestinationEnd)), DashLineDirection * RulerOffset);
		}
	}

	//--------------------第二临墙标尺线更新--------------------
	if (LinkSecondLine.IsValid())
	{
		if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
		{
			if ((!SecondRelatedRoom->IsPointInRoom(LinkSecondLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && SecondRelatedRoom->GetType() == OT_InternalRoom) ||
				(SecondRelatedRoom->IsPointInRoom(LinkSecondLine->GetLineEditPoint()->GetPos() + 5 * DashLineDirection) && SecondRelatedRoom->GetType() == OT_OutRoom))
			{
				DashLineDirection *= -1;
			}
		}
		else if (SecondRelatedRoom.IsValid() && !SecondRelatedRoom->IsClosed())
		{
			if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
			{
				if (FirstRelatedRoom->IsPointInRoom(TempCenterPos + (5 + 1.5f * Width) * DashLineDirection))
				{
					DashLineDirection *= -1;
				}
			}
		}
		DashLineUpDir = DashLineDirection;
		GetStartNearestPoint(LinkSecondLine, SecondDestinationStart);
		GetEndNearestPoint(LinkSecondLine, SecondDestinationEnd);
		float TempLength1 = FVector::Distance(SecondDestinationStart, TempStartPos);
		float TempLength2 = FVector::Distance(SecondDestinationStart, TempEndPos);
		if (TempLength1 < TempLength2)
		{
			//前半段
			SecondLineLeftRulerLine->Update(SecondDestinationStart, TempStartPos,
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(TempStartPos, SecondDestinationStart, SecondDestinationEnd)));
			//后半段
			SecondLineRightRulerLine->Update(TempEndPos, SecondDestinationEnd,
				DashLineDirection * (RulerOffset + FArmyMath::Distance(TempEndPos, SecondDestinationStart, SecondDestinationEnd)), DashLineDirection * RulerOffset);
		}
		else
		{
			//前半段
			SecondLineLeftRulerLine->Update(SecondDestinationStart, TempEndPos,
				DashLineDirection * RulerOffset, DashLineDirection * (RulerOffset + FArmyMath::Distance(TempEndPos, SecondDestinationStart, SecondDestinationEnd)));
			//后半段
			SecondLineRightRulerLine->Update(TempStartPos, SecondDestinationEnd,
				DashLineDirection * (RulerOffset + FArmyMath::Distance(TempStartPos, SecondDestinationStart, SecondDestinationEnd)), DashLineDirection * RulerOffset);
		}
	}	
}

void FArmyHardware::GetStartNearestPoint(TSharedPtr<FArmyLine> LinkedLine, FVector & DestPos)
{
	TArray<FVector> PosList;
	TArray<TWeakPtr<FArmyHardware>> RelatedObjList;
	TArray<TWeakPtr<FArmyHardware>> AllObjs;
	FArmySceneData::Get()->GetAllHardWare(AllObjs, GGI->DesignEditor->CurrentController->GetDesignModelType());

	for (auto It : AllObjs)
	{
		if (It.Pin().Get() == this)
		{
			continue;
		}
		if (It.Pin()->LinkFirstLine == LinkFirstLine || It.Pin()->LinkSecondLine == LinkSecondLine ||
			It.Pin()->LinkFirstLine == LinkSecondLine || It.Pin()->LinkSecondLine == LinkFirstLine)
		{
			PosList.Add(It.Pin()->GetStartPos());
			PosList.Add(It.Pin()->GetEndPos());
		}
	}

	float NearsetLength;
	FVector NearsetPos;
	bIsLeftToRight = true;
	if (FVector::Distance(LinkedLine->GetStart(), GetStartPos()) < FVector::Distance(LinkedLine->GetStart(), GetEndPos()))
	{
		NearsetLength = FVector::Distance(LinkedLine->GetStart(), GetStartPos());
		NearsetPos = LinkedLine->GetStart();
	}
	else
	{
		bIsLeftToRight = false;
		NearsetLength = FVector::Distance(LinkedLine->GetEnd(), GetStartPos());
		NearsetPos = LinkedLine->GetEnd();
	}

	for (int i = 0; i < PosList.Num(); ++i)
	{
		if (NearsetLength > FVector::Distance(PosList[i], GetStartPos()))
		{
			FVector TempLoc = bIsLeftToRight ? LinkedLine->GetStart() : LinkedLine->GetEnd();
			if (FVector::Distance(TempLoc, GetStartPos()) > FVector::Distance(TempLoc, PosList[i]) || PosList[i].Equals(GetStartPos(), 0.01f))
			{
				NearsetPos = PosList[i];
				NearsetLength = FVector::Distance(PosList[i], GetStartPos());
			}
		}
	}

	DestPos = FArmyMath::GetProjectionPoint(NearsetPos, LinkedLine->GetStart(), LinkedLine->GetEnd());
	DestPos.Z = 0;
}

void FArmyHardware::GetEndNearestPoint(TSharedPtr<FArmyLine> LinkedLine, FVector & DestPos)
{
	TArray<FVector> PosList;
	TArray<TWeakPtr<FArmyHardware>> RelatedObjList;
	TArray<TWeakPtr<FArmyHardware>> AllObjs;
	FArmySceneData::Get()->GetAllHardWare(AllObjs, GGI->DesignEditor->CurrentController->GetDesignModelType());

	for (auto It : AllObjs)
	{
		if (It.Pin().Get() == this)
		{
			continue;
		}
		if (It.Pin()->LinkFirstLine == LinkFirstLine || It.Pin()->LinkSecondLine == LinkSecondLine ||
			It.Pin()->LinkFirstLine == LinkSecondLine || It.Pin()->LinkSecondLine == LinkFirstLine)
		{
			PosList.Add(It.Pin()->GetStartPos());
			PosList.Add(It.Pin()->GetEndPos());
		}
	}

	float NearsetLength;
	FVector NearsetPos;
	bIsLeftToRight = true;
	if (FVector::Distance(LinkedLine->GetEnd(), GetEndPos()) < FVector::Distance(LinkedLine->GetEnd(), GetStartPos()))
	{
		NearsetLength = FVector::Distance(LinkedLine->GetEnd(), GetEndPos());
		NearsetPos = LinkedLine->GetEnd();
	}
	else
	{
		bIsLeftToRight = false;
		NearsetLength = FVector::Distance(LinkedLine->GetStart(), GetEndPos());
		NearsetPos = LinkedLine->GetStart();
	}

	for (int i = 0; i < PosList.Num(); ++i)
	{
		if (NearsetLength > FVector::Distance(PosList[i], GetEndPos()))
		{
			FVector TempLoc = bIsLeftToRight ? LinkedLine->GetEnd() : LinkedLine->GetStart();
			if (FVector::Distance(TempLoc, GetEndPos()) > FVector::Distance(TempLoc, PosList[i]) || PosList[i].Equals(GetEndPos(), 0.01f))
			{
				NearsetPos = PosList[i];
				NearsetLength = FVector::Distance(PosList[i], GetEndPos());
			}
		}
	}

	DestPos = FArmyMath::GetProjectionPoint(NearsetPos, LinkedLine->GetStart(), LinkedLine->GetEnd());
	DestPos.Z = 0;
}

void FArmyHardware::OnHardWareInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float LineLength = FCString::Atof(*InText.ToString());
		/*float MinDistance = FMath::Min(FMath::Min(FirstLineLeftRulerLine->GetInputBoxCachedLength(), FirstLineRightRulerLine->GetInputBoxCachedLength()),
			FMath::Min(SecondLineLeftRulerLine->GetInputBoxCachedLength(), SecondLineRightRulerLine->GetInputBoxCachedLength()));
		if ((LineLength / 10.f - Length) / 2.f > MinDistance)
		{
			LineLength = 20 * MinDistance + Length * 10.f;
			HardWareRulerLine->GetWidget()->RefreshWithCachedLength();
		}*/

		SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的宽度"));
		SetLength(LineLength / 10.f);
		Modify();

		if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
			FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
		else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
			FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
	}
}

void FArmyHardware::OnFirstLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstLineLeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;

		/** @欧石楠 如果是在原始墙体上的控件*/
		if (InWallType == 0)
		{
			if (CachedLength < TempInValue && FMath::Abs(TempLength) > FirstLineRightRulerLine->GetInputBoxCachedLength() && 
				(FirstRelatedRoom->IsClosed() || (bIsLeftToRight ? !FirstDestinationStart.Equals(LinkFirstLine->GetStart(), 0.02f) : !FirstDestinationStart.Equals(LinkFirstLine->GetEnd(), 0.02f))))
			{
				OnFirstLineRInputBoxCommitted(FText::AsNumber(0), ETextCommit::OnEnter);
				FirstLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
				return;
			}

			if (FirstRelatedRoom->IsClosed() || (bIsLeftToRight ? !FirstDestinationStart.Equals(LinkFirstLine->GetStart(), 0.02f) : !FirstDestinationStart.Equals(LinkFirstLine->GetEnd(), 0.02f)))
			{
				SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的位置"));
				SetPos(GetPos() + (TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));
				UpdateDashLine();
				if ((CachedLength < TempInValue && FirstLineLeftRulerLine->GetInputBoxCachedLength() < CachedLength
					|| CachedLength > TempInValue && FirstLineLeftRulerLine->GetInputBoxCachedLength() > CachedLength) || !FMath::IsNearlyEqual(FirstLineLeftRulerLine->GetInputBoxCachedLength(), TempInValue, 0.01f))
				{
					SetPos(GetPos() + (2 * TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
					//SetPos(GetPos() + (TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
				}
				Modify();

				if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
					FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
				else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
					FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

				FirstLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
			}
			else
			{
				if (bIsLeftToRight)
				{
					LinkFirstLine->SetStart(LinkFirstLine->GetStart() + TempLength * (LinkFirstLine->GetEnd() - LinkFirstLine->GetStart()).GetSafeNormal());
				}
				else
				{
					LinkFirstLine->SetEnd(LinkFirstLine->GetEnd() - TempLength * (LinkFirstLine->GetEnd() - LinkFirstLine->GetStart()).GetSafeNormal());
				}
			}
		}
		else//在新建墙体上的控件移动
		{
			if (CachedLength < TempInValue && FMath::Abs(TempLength) > FirstLineRightRulerLine->GetInputBoxCachedLength() &&
				((bIsLeftToRight ? !FirstDestinationStart.Equals(LinkFirstLine->GetStart(), 0.02f) : !FirstDestinationStart.Equals(LinkFirstLine->GetEnd(), 0.02f))))
			{
				OnFirstLineRInputBoxCommitted(FText::AsNumber(0), ETextCommit::OnEnter);
				FirstLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
				return;
			}

			SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的位置"));
			SetPos(GetPos() + (TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));
			UpdateDashLine();
			if ((CachedLength < TempInValue && FirstLineLeftRulerLine->GetInputBoxCachedLength() < CachedLength
				|| CachedLength > TempInValue && FirstLineLeftRulerLine->GetInputBoxCachedLength() > CachedLength) || !FMath::IsNearlyEqual(FirstLineLeftRulerLine->GetInputBoxCachedLength(), TempInValue, 0.01f))
			{
				SetPos(GetPos() + (2 * TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
			}
			Modify();

			if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
				FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

			FirstLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
		}
	}
}

void FArmyHardware::OnFirstLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstLineRightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;

		/** @欧石楠 如果是在原始墙体上的控件*/
		if (InWallType == 0)
		{
			if (CachedLength < TempInValue && FMath::Abs(TempLength) > FirstLineLeftRulerLine->GetInputBoxCachedLength() &&
				(FirstRelatedRoom->IsClosed() || (bIsLeftToRight ? !FirstDestinationEnd.Equals(LinkFirstLine->GetEnd(), 0.02f) : !FirstDestinationEnd.Equals(LinkFirstLine->GetStart(), 0.02f))))
			{
				OnFirstLineLInputBoxCommitted(FText::AsNumber(0), ETextCommit::OnEnter);
				FirstLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
				return;
			}
			if (FirstRelatedRoom->IsClosed() || (bIsLeftToRight ? !FirstDestinationEnd.Equals(LinkFirstLine->GetEnd(), 0.02f) : !FirstDestinationEnd.Equals(LinkFirstLine->GetStart(), 0.02f)))
			{
				SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的位置"));
				SetPos(GetPos() + (TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
				UpdateDashLine();
				if (CachedLength < TempInValue && FirstLineRightRulerLine->GetInputBoxCachedLength() < CachedLength
					|| CachedLength > TempInValue && FirstLineRightRulerLine->GetInputBoxCachedLength() > CachedLength || !FMath::IsNearlyEqual(FirstLineRightRulerLine->GetInputBoxCachedLength(), TempInValue, 0.01f))
				{
					SetPos(GetPos() + (2 * TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));
					//SetPos(GetPos() + (TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));
				}
				Modify();

				if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
					FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
				else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
					FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

				FirstLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
			}
			else
			{
				if (bIsLeftToRight)
				{
					LinkFirstLine->SetEnd(LinkFirstLine->GetEnd() - TempLength * (LinkFirstLine->GetEnd() - LinkFirstLine->GetStart()).GetSafeNormal());
				}
				else
				{
					LinkFirstLine->SetStart(LinkFirstLine->GetStart() + TempLength * (LinkFirstLine->GetEnd() - LinkFirstLine->GetStart()).GetSafeNormal());
				}
			}
		}
		else
		{
			if (CachedLength < TempInValue && FMath::Abs(TempLength) > FirstLineLeftRulerLine->GetInputBoxCachedLength() &&
				((bIsLeftToRight ? !FirstDestinationEnd.Equals(LinkFirstLine->GetEnd(), 0.02f) : !FirstDestinationEnd.Equals(LinkFirstLine->GetStart(), 0.02f))))
			{
				OnFirstLineLInputBoxCommitted(FText::AsNumber(0), ETextCommit::OnEnter);
				FirstLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
				return;
			}
			SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的位置"));
			SetPos(GetPos() + (TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
			UpdateDashLine();
			if (CachedLength < TempInValue && FirstLineRightRulerLine->GetInputBoxCachedLength() < CachedLength
				|| CachedLength > TempInValue && FirstLineRightRulerLine->GetInputBoxCachedLength() > CachedLength || !FMath::IsNearlyEqual(FirstLineRightRulerLine->GetInputBoxCachedLength(), TempInValue, 0.01f))
			{
				SetPos(GetPos() + (2 * TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));				
			}
			Modify();

			if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
				FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

			FirstLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
		}
	}
}

void FArmyHardware::OnSecondLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondLineLeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;

		if (CachedLength < TempInValue && FMath::Abs(TempLength) > SecondLineRightRulerLine->GetInputBoxCachedLength() &&
			(SecondRelatedRoom->IsClosed() || (bIsLeftToRight ? !SecondDestinationStart.Equals(LinkSecondLine->GetStart(), 0.02f) : !SecondDestinationStart.Equals(LinkSecondLine->GetEnd(), 0.02f))))
		{
			OnSecondLineRInputBoxCommitted(FText::AsNumber(0), ETextCommit::OnEnter);
			SecondLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
			return;
		}

		if (SecondRelatedRoom->IsClosed() || (bIsLeftToRight ? !SecondDestinationStart.Equals(LinkSecondLine->GetStart(), 0.02f) : !SecondDestinationStart.Equals(LinkSecondLine->GetEnd(), 0.02f)))
		{
			SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的位置"));
			SetPos(GetPos() + (TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
			UpdateDashLine();
			if (CachedLength < TempInValue && SecondLineLeftRulerLine->GetInputBoxCachedLength() < CachedLength
				|| CachedLength > TempInValue && SecondLineLeftRulerLine->GetInputBoxCachedLength() > CachedLength || !FMath::IsNearlyEqual(SecondLineLeftRulerLine->GetInputBoxCachedLength(), TempInValue, 0.01f))
			{
				SetPos(GetPos() + (2 * TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));
				UpdateDashLine();
			}
			Modify();

			if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
				FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

			SecondLineLeftRulerLine->GetWidget()->RefreshWithCachedLength();
		}
		else
		{
			if (bIsLeftToRight)
			{
				LinkSecondLine->SetStart(LinkSecondLine->GetStart() + TempLength * (LinkSecondLine->GetEnd() - LinkSecondLine->GetStart()).GetSafeNormal());
			}
			else
			{
				LinkSecondLine->SetEnd(LinkSecondLine->GetEnd() - TempLength * (LinkSecondLine->GetEnd() - LinkSecondLine->GetStart()).GetSafeNormal());
			}
		}
	}
}

void FArmyHardware::OnSecondLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondLineRightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;

		if (CachedLength < TempInValue && FMath::Abs(TempLength) > SecondLineLeftRulerLine->GetInputBoxCachedLength() && 
			(SecondRelatedRoom->IsClosed() || (bIsLeftToRight ? !SecondDestinationEnd.Equals(LinkSecondLine->GetEnd(), 0.02f) : !SecondDestinationEnd.Equals(LinkSecondLine->GetStart(), 0.02f))))
		{
			OnSecondLineLInputBoxCommitted(FText::AsNumber(0), ETextCommit::OnEnter);
			SecondLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
			return;
		}

		if (SecondRelatedRoom->IsClosed() || (bIsLeftToRight ? !SecondDestinationEnd.Equals(LinkSecondLine->GetEnd(), 0.02f) : !SecondDestinationEnd.Equals(LinkSecondLine->GetStart(), 0.02f)))
		{
			SCOPE_TRANSACTION(TEXT("通过标尺修改门窗的位置"));
			SetPos(GetPos() + (TempLength * (GetEndPos() - GetStartPos()).GetSafeNormal()));
			UpdateDashLine();
			if (CachedLength < TempInValue && SecondLineRightRulerLine->GetInputBoxCachedLength() < CachedLength
				|| CachedLength > TempInValue && SecondLineRightRulerLine->GetInputBoxCachedLength() > CachedLength || !FMath::IsNearlyEqual(SecondLineRightRulerLine->GetInputBoxCachedLength(), TempInValue, 0.01f))
			{
				SetPos(GetPos() + (2 * TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
				//SetPos(GetPos() + (TempLength * (GetStartPos() - GetEndPos()).GetSafeNormal()));
			}
			Modify();

			if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel)
				FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
			else if (GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel)
				FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

			SecondLineRightRulerLine->GetWidget()->RefreshWithCachedLength();
		}
		else
		{
			if (bIsLeftToRight)
			{
				LinkSecondLine->SetEnd(LinkSecondLine->GetEnd() - TempLength * (LinkSecondLine->GetEnd() - LinkSecondLine->GetStart()).GetSafeNormal());
			}
			else
			{
				LinkSecondLine->SetStart(LinkSecondLine->GetStart() + TempLength * (LinkSecondLine->GetEnd() - LinkSecondLine->GetStart()).GetSafeNormal());
			}
		}
	}
}

void FArmyHardware::GetAttachHoleWallInfo(TArray<FBSPWallInfo>& OutAttachWallInfo)
{
	float outRoomOffset = 0;
	TArray<TSharedPtr<FArmyWallLine>> attWallLines;
	FArmySceneData::Get()->GetHardwareRelateWalllines(this->AsShared(), attWallLines);
	bool bIndependentWall = false;
	FVector IndependentNormal;
	if (attWallLines.Num() == 0)
	{
		//独立墙单独处理
		TArray<FObjectWeakPtr> AddWallList;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_IndependentWall, AddWallList);
		for (auto It : AddWallList)
		{
			TArray< TSharedPtr<class FArmyLine> > PolyLines;
			It.Pin()->GetLines(PolyLines);
			for (auto ItLine : PolyLines)
			{
				if (LinkFirstLine == ItLine || LinkSecondLine == ItLine)
				{
					bIndependentWall = true;
					outRoomOffset = FArmySceneData::Get()->FinishWallThick;
					TSharedPtr<FArmyIndependentWall> IndependentWall = StaticCastSharedPtr<FArmyIndependentWall>(It.Pin());
					IndependentNormal = IndependentWall->GetWallNormal(ItLine);
					break;
				}
			}
			if (bIndependentWall)
				break;
		}
		if (!bIndependentWall)
			return;
	}
	FVector tempStart = FVector(GetStartPos().X, GetStartPos().Y, 0.0f);
	FVector tempEnd = FVector(GetEndPos().X, GetEndPos().Y, 0.0f);
	FVector first =tempStart + Width / 2 * Direction + FVector(0, 0, Height);
	FVector second = tempStart + Width / 2 * Direction;
	FVector third = tempStart - Width / 2 * Direction;
	FVector fourth = tempStart - Width / 2 * Direction + FVector(0, 0, Height);
	TArray<FVector> LeftWallVerts = { first,second,third,fourth };
	TArray<FVector> LeftWallFinishVerts;
	
	if (attWallLines.Num() == 2)
	{
		outRoomOffset = FArmySceneData::Get()->FinishWallThick;
	}
	FVector InnerNormal;
	if (bIndependentWall)
		InnerNormal = IndependentNormal;
	else
		InnerNormal = attWallLines[0]->GetNormal();
	first = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal;
	second = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal;
	third = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal);
	fourth = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal);

	LeftWallFinishVerts = { first,second,third,fourth };
	// 洞口左边面片信息
	FBSPWallInfo LeftOriginalWallInfo, LeftFinishWallInfo;
	LeftOriginalWallInfo.PolyVertices.Push(LeftWallVerts);
	LeftFinishWallInfo.PolyVertices.Push(LeftWallFinishVerts);
	LeftOriginalWallInfo.Normal = LeftFinishWallInfo.Normal = -HorizontalDirection;
	LeftOriginalWallInfo.GenerateFromObjectType = LeftFinishWallInfo.GenerateFromObjectType = ObjectType;
	LeftOriginalWallInfo.UniqueIdForRoomOrHardware = LeftFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	LeftOriginalWallInfo.UniqueId = LeftFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("LeftWall");
	first = tempEnd + Width / 2 * Direction;
	second = tempEnd + Width / 2 * Direction + FVector(0, 0, Height);
	third = tempEnd - Width / 2 * Direction + FVector(0, 0, Height);
	fourth = tempEnd - Width / 2 * Direction;
	TArray<FVector> RightVerts = { first,second,third,fourth };
	TArray<FVector> RightFinishVerts;
	first = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal;
	second = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + FArmySceneData::Get()->FinishWallThick)*InnerNormal;
	third = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal);
	fourth = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + FVector(0, 0, Height - FArmySceneData::Get()->FinishWallThick) + (Width / 2 + outRoomOffset) * (-InnerNormal);
	RightFinishVerts = { first,second,third,fourth };
	// 洞口右边面片信息
	FBSPWallInfo RightOrignalWallInfo, RightFinishWallInfo;
	RightOrignalWallInfo.PolyVertices.Push(RightVerts);
	RightFinishWallInfo.PolyVertices.Push(RightFinishVerts);
	RightOrignalWallInfo.Normal = RightFinishWallInfo.Normal = HorizontalDirection;
	RightOrignalWallInfo.GenerateFromObjectType = RightFinishWallInfo.GenerateFromObjectType = ObjectType;
	RightOrignalWallInfo.UniqueIdForRoomOrHardware = RightFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	RightOrignalWallInfo.UniqueId = RightOrignalWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("RightWall");

	// 地面面片信息
	FBSPWallInfo BottomOrignalWallInfo, BottomFinishWallInfo;
	first = tempStart + Width / 2 * Direction;
	second = tempEnd + Width / 2 * Direction;
	third = tempEnd - Width / 2 * Direction;
	fourth = tempStart - Width / 2 * Direction;
	TArray<FVector> BottomVerts = { first,second,third,fourth };
	TArray<FVector> BottomFinishVerts;
	first = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick + (Width / 2.0f + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick);
	second = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick + (Width / 2.0f + FArmySceneData::Get()->FinishWallThick) * InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick);
	third = tempEnd - HorizontalDirection * FArmySceneData::Get()->FinishWallThick - (Width / 2.0f + FArmySceneData::Get()->FinishWallThick) * InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick);
	fourth = tempStart + HorizontalDirection * FArmySceneData::Get()->FinishWallThick - (Width / 2.0f + FArmySceneData::Get()->FinishWallThick)*InnerNormal + FVector(0, 0, FArmySceneData::Get()->FinishWallThick);
	BottomFinishVerts = { first,second,third,fourth };
	BottomOrignalWallInfo.PolyVertices.Push(BottomVerts);
	BottomFinishWallInfo.PolyVertices.Push(BottomFinishVerts);
	BottomOrignalWallInfo.Normal = BottomFinishWallInfo.Normal = FVector(0, 0, 1);
	BottomOrignalWallInfo.GenerateFromObjectType = BottomFinishWallInfo.GenerateFromObjectType = ObjectType;
	BottomOrignalWallInfo.UniqueIdForRoomOrHardware = BottomFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	BottomOrignalWallInfo.UniqueId = BottomFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("BottomWall");
	// 顶面面片信息
	FBSPWallInfo TopOrignalWallInfo, TopFinishWallInfo;
	TArray<FVector> TopVerts;
	for (const FVector& iter : BottomVerts)
	{
		TopVerts.Push(iter + FVector(0, 0, Height));
	}

	TArray<FVector> TopFinishVerts;
	for (const FVector& iter : BottomFinishVerts)
	{
		TopFinishVerts.Push(iter + FVector(0, 0, Height - 2 * FArmySceneData::Get()->FinishWallThick));
	}
	TopOrignalWallInfo.PolyVertices.Push(TopVerts);
	TopFinishWallInfo.PolyVertices.Push(TopFinishVerts);
	TopOrignalWallInfo.Normal = TopFinishWallInfo.Normal = FVector(0, 0, -1);
	TopOrignalWallInfo.GenerateFromObjectType = TopFinishWallInfo.GenerateFromObjectType = ObjectType;
	TopOrignalWallInfo.UniqueIdForRoomOrHardware = TopFinishWallInfo.UniqueIdForRoomOrHardware = GetUniqueID().ToString();
	TopOrignalWallInfo.UniqueId = TopFinishWallInfo.UniqueId = GetUniqueID().ToString() + TEXT("TopWall");

	OutAttachWallInfo.Push(LeftOriginalWallInfo);
	OutAttachWallInfo.Push(LeftFinishWallInfo);
	OutAttachWallInfo.Push(RightOrignalWallInfo);
	OutAttachWallInfo.Push(RightFinishWallInfo);
	OutAttachWallInfo.Push(BottomOrignalWallInfo);
	OutAttachWallInfo.Push(BottomFinishWallInfo);
	OutAttachWallInfo.Push(TopOrignalWallInfo);
	OutAttachWallInfo.Push(TopFinishWallInfo);
}

bool FArmyHardware::CaptureDoubleLine(const FVector& Pos, EModelType InModelType)
{
    FDoubleWallCaptureInfo CaptureInfo;
	if (FArmySceneData::Get()->CaptureDoubleLine(Pos, InModelType, CaptureInfo))
	{
        CurrentModelType = InModelType;

        LinkFirstLine = CaptureInfo.FirstLine;
        LinkSecondLine = CaptureInfo.SecondLine;

        FirstRelatedRoom = GetRelatedRoomByLine(CaptureInfo.FirstLine, CurrentModelType);
        SecondRelatedRoom = GetRelatedRoomByLine(CaptureInfo.SecondLine, CurrentModelType);

        AutoThickness = CaptureInfo.Thickness;
        AutoPos = CaptureInfo.Pos;

        TArray<TWeakPtr<FArmyObject>> RoomList;
        FArmySceneData::Get()->GetObjects(InModelType, OT_InternalRoom, RoomList);
        FArmySceneData::Get()->GetObjects(InModelType, OT_OutRoom, RoomList);
        for (auto It : RoomList)
        {
            TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
            if (Room.IsValid())
            {
                TArray<TSharedPtr<class FArmyWallLine>> WallLines = Room->GetWallLines();
                for (auto ItLine : WallLines)
                {
                    if (ItLine->GetCoreLine() == LinkFirstLine || ItLine->GetCoreLine() == LinkSecondLine)
                    {
                        ItLine->PushAppendObject(this->AsShared());
                    }
                }
            }
        }

        return true;
	}

	return false;
}

//bool FArmyHardware::CheckPointIsProjectOnLine(FVector & Point)
//{
//	if (bRightOpen)
//	{
//		HorizontalDirection = Direction.RotateAngleAxis(90, FVector(0, 0, 1));
//	}
//	else
//	{
//		HorizontalDirection = Direction.RotateAngleAxis(-90, FVector(0, 0, 1));
//	}
//	FVector TempHWStartPos = Point - HorizontalDirection * Length / 2.f;
//	FVector TempHWEndPos = Point + HorizontalDirection * Length / 2.f;
//	
//	if (LinkFirstLine.IsValid() && LinkSecondLine.IsValid())
//	{
//		FVector2D TempFirstStartPos(LinkFirstLine->GetStart());
//		FVector2D TempFirstEndPos(LinkFirstLine->GetEnd());
//
//		FVector2D TempSecondStartPos(LinkSecondLine->GetStart());
//		FVector2D TempSecondEndPos(LinkSecondLine->GetEnd());
//
//		FVector2D TempHW2DStartPos(TempHWStartPos);
//		FVector2D TempHW2DEndPos(TempHWEndPos);
//
//		//两条线的start点相离最近
//		if (FVector::Distance(LinkFirstLine->GetStart(), LinkSecondLine->GetStart()) < FVector::Distance(LinkFirstLine->GetStart(), LinkSecondLine->GetEnd()))
//		{			
//			//如果第一条线的起点的投影在第二条线上，说明控件移动的最远点应该是第一条线的起点
//			//否则是第二条线的起点
//			if (FArmyMath::IsPointProjectionOnLineSegment2D(TempFirstStartPos, TempSecondStartPos, TempSecondEndPos))
//			{								
//				//如果控件的两端点的投影在线上则说明可移动
//				if (FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DStartPos, TempFirstStartPos, TempFirstEndPos) &&
//					FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DEndPos, TempFirstStartPos, TempFirstEndPos))
//				{
//					return true;
//				}
//			}
//			else
//			{
//				//如果控件的两端点的投影在线上则说明可移动
//				if (FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DStartPos, TempSecondStartPos, TempSecondEndPos) &&
//					FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DEndPos, TempSecondStartPos, TempSecondEndPos))
//				{
//					return true;
//				}
//			}
//		}
//		//否则是第一条start到第二条end最近
//		else
//		{
//			//最远点应该是第一条线的起点
//			//否则是第二条线的终点
//			if (FArmyMath::IsPointProjectionOnLineSegment2D(TempFirstStartPos, TempSecondStartPos, TempSecondEndPos))
//			{
//				//如果控件的两端点的投影在线上则说明可移动
//				if (FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DStartPos, TempFirstStartPos, TempFirstEndPos) &&
//					FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DEndPos, TempFirstStartPos, TempFirstEndPos))
//				{
//					return true;
//				}
//			}
//			else
//			{
//				//如果控件的两端点的投影在线上则说明可移动
//				if (FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DStartPos, TempSecondStartPos, TempSecondEndPos) &&
//					FArmyMath::IsPointProjectionOnLineSegment2D(TempHW2DEndPos, TempSecondStartPos, TempSecondEndPos))
//				{
//					return true;
//				}
//			}
//		}
//	}
//	
//	return false;
//}
