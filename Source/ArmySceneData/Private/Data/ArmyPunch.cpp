#include "ArmyPunch.h"
#include "ArmySceneData.h"
#include "ArmyRulerLine.h"
#include "ArmyRoom.h"
#include "ArmyMath.h"
#include "ArmyGameInstance.h"

#define DefaultPunchLength 120
#define DefaultPunchHeight FArmySceneData::WallHeight + 20

FArmyPunch::FArmyPunch()
	: FArmyHardware()
	//, HeightToFloor(0)
	, PunchDepth(0)
	, DepthRulerLine(MakeShareable(new FArmyRulerLine()))
	, DepthPoint(MakeShareable(new FArmyEditPoint()))
{
	SetName(TEXT("开阳台"));
	Length = DefaultPunchLength;
	Height = DefaultPunchHeight;
	ObjectType = OT_Punch;

	//RectImagePanel = MakeShareable(new FArmyRect());
	RectImagePanel->SetBaseColor(FLinearColor::White);
	RectImagePanel->LineThickness = WALLLINEWIDTH + 0.1F;

	PunchDepthRect = MakeShareable(new FArmyRect());
	PunchDepthRect->SetBaseColor(FLinearColor::White);
	PunchDepthRect->LineThickness = WALLLINEWIDTH + 0.1F;

	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);
	DepthPoint->SetState(FArmyPrimitive::OPS_Disable);
	HardWareRulerLine->SetEnableInputBox(false);
	//DepthRulerLine->SetInputBoxPosUseLRLine(true);
	DepthRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyPunch::OnDepthInputBoxCommitted));

	SetState(OS_Normal);
}

FArmyPunch::FArmyPunch(FArmyPunch * Copy)
{
	RectImagePanel = MakeShareable(new FArmyRect());
	RectImagePanel->SetBaseColor(FLinearColor::White);
	RectImagePanel->LineThickness = Copy->RectImagePanel->LineThickness;

	PunchDepthRect = MakeShareable(new FArmyRect());
	PunchDepthRect->SetBaseColor(FLinearColor::White);
	PunchDepthRect->LineThickness = Copy->PunchDepthRect->LineThickness;

	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));
	DepthPoint = MakeShareable(new FArmyEditPoint(Copy->DepthPoint.Get()));
	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));

	SetName(Copy->GetName());
	ObjectType = Copy->ObjectType;
	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;
	Direction = Copy->Direction;
	HorizontalDirection = Copy->HorizontalDirection;
	DashLineUpDir = Copy->DashLineUpDir;
	State = Copy->State;
	PropertyFlag = Copy->PropertyFlag;
	bRightOpen = Copy->bRightOpen;

	BrokenWallLeftLine = MakeShareable(new FArmyLine());
	BrokenWallRightLine = MakeShareable(new FArmyLine());

	DepthRulerLine = MakeShareable(new FArmyRulerLine());
	//DepthRulerLine->SetInputBoxPosUseLRLine(true);
	DepthRulerLine->SetOnTextCommittedDelegate(FOnTextCommitted::CreateRaw(this, &FArmyPunch::OnDepthInputBoxCommitted));

	HeightToFloor = Copy->HeightToFloor;
	if (Copy->PunchDepth == 0) {
		PunchDepth = 50;
	}
	else
	{
		PunchDepth = Copy->PunchDepth;
	}


	HardWareRulerLine->SetEnableInputBox(false);

	SetState(OS_Selected);

	Update();
}

FArmyPunch::~FArmyPunch()
{
}

void FArmyPunch::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("depthPos", DepthPoint->GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	//JsonWriter->WriteValue("OffsetGroundHeight", GetOffGoundDist());
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());
	JsonWriter->WriteValue("DashLineUpDir", DashLineUpDir.ToString());

	JsonWriter->WriteValue("PunchDepth", GetPunchDepth());

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyPunch)
}

void FArmyPunch::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);
	if (GetPropertyFlag(FArmyObject::FLAG_MODIFY))
	{
		DepthRulerLine->SetEnableInputBox(false);
	}

	FVector Pos, Direction, TempDepthPos;
	Pos.InitFromString(InJsonData->GetStringField("pos"));
	Direction.InitFromString(InJsonData->GetStringField("direction"));
	DashLineUpDir.InitFromString(InJsonData->GetStringField("DashLineUpDir"));
	TempDepthPos.InitFromString(InJsonData->GetStringField("depthPos"));
	DepthPoint->SetPos(TempDepthPos);

	SetPos(Pos);
	SetDirection(Direction);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));

	//SetHeightToFloor(InJsonData->GetNumberField("OffsetGroundHeight"));

	SetPunchDepth(InJsonData->GetNumberField("PunchDepth"));

	SetState(OS_Normal);

	bool bCapture = CaptureDoubleLine(Pos, E_LayoutModel);
}

void FArmyPunch::SetState(EObjectState InState)
{
	State = InState;
	switch (InState)
	{
	case OS_Normal:
		DrawColor = FLinearColor::White;
		DeselectPoints();
		break;
	case OS_Hovered:
	case OS_Selected:
		DrawColor = FLinearColor(FColor(0xFFFF9800));
		break;
	case OS_Disable:
		DrawColor = FLinearColor::Gray;
		break;
	default:
		break;
	}
	RectImagePanel->SetBaseColor(DrawColor);
	PunchDepthRect->SetBaseColor(DrawColor);
	if (BrokenWallLeftLine.IsValid())
	{
		BrokenWallLeftLine->SetBaseColor(DrawColor);
		BrokenWallRightLine->SetBaseColor(DrawColor);
	}
}

void FArmyPunch::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
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

				FVector LineDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
				FVector DashLineDirection = LineDirection.RotateAngleAxis(90, FVector(0, 0, 1));
				//--------------------控件标尺线更新--------------------	
				HardWareRulerLine->Update(GetStartPos(), GetEndPos(), -1 * DashLineDirection * RulerOffset);
				FVector TempCheckPos = (HardWareRulerLine->GetUpDashLineStart() + HardWareRulerLine->GetUpDashLineEnd()) / 2.f;
				if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
				{
					if (FirstRelatedRoom->IsPointInRoom(TempCheckPos))
					{
						HardWareRulerLine->Update(GetStartPos(), GetEndPos(), DashLineDirection * RulerOffset);
					}
				}
				DashLineUpDir = HardWareRulerLine->GetDashLineUpDir();
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

				//深度标尺线更新				
				DepthRulerLine->Update(GetPos() + Width / 2 * -DashLineUpDir, GetPos() + (Width / 2 + PunchDepth) * -DashLineUpDir);
				DepthRulerLine->Draw(PDI, View);
			}
		}
		if (BrokenWallLeftLine.IsValid())
		{
			BrokenWallLeftLine->Draw(PDI, View);
			BrokenWallRightLine->Draw(PDI, View);
		}

		RectImagePanel->Draw(PDI, View);
		if (PunchDepth != 0)
		{
			PunchDepthRect->Draw(PDI, View);
		}

		if (DashLineUpDir.IsNearlyZero())
		{
			FVector LineDirection = FArmyMath::GetLineDirection(GetStartPos(), GetEndPos());
			FVector DashLineDirection = LineDirection.RotateAngleAxis(90, FVector(0, 0, 1));
			//--------------------控件标尺线更新--------------------	
			HardWareRulerLine->Update(GetStartPos(), GetEndPos(), DashLineDirection * RulerOffset);
			FVector TempCheckPos = (HardWareRulerLine->GetUpDashLineStart() + HardWareRulerLine->GetUpDashLineEnd()) / 2.f;
			if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->IsClosed())
			{
				if (FirstRelatedRoom->IsPointInRoom(TempCheckPos))
				{
					HardWareRulerLine->Update(GetStartPos(), GetEndPos(), -1 * DashLineDirection * RulerOffset);
				}
			}
			if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->IsClosed())
			{
				if (SecondRelatedRoom->IsPointInRoom(TempCheckPos))
				{
					HardWareRulerLine->Update(GetStartPos(), GetEndPos(), -1 * DashLineDirection * RulerOffset);
				}
			}
			DashLineUpDir = HardWareRulerLine->GetDashLineUpDir();
			HardWareRulerLine->ShowInputBox(false);
			Update();
			CheckIfOutLimited();
		}
	}
}

bool FArmyPunch::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		bShowDashLine = true;
		if (RectImagePanel->IsSelected(Pos, InViewportClient) || PunchDepthRect->IsSelected(Pos, InViewportClient))
		{
			return true;
		}
		return false;
	}
	return false;
}

bool FArmyPunch::Hover(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return false;
	}
	return false;
}

const FBox FArmyPunch::GetBounds()
{
	return FBox(GetClipingBox());
}

void FArmyPunch::SetPropertyFlag(PropertyType InType, bool InUse)
{
	FArmyHardware::SetPropertyFlag(InType, InUse);
	if (GetPropertyFlag(FArmyObject::FLAG_MODIFY))
	{
		DepthRulerLine->SetEnableInputBox(false);
	}
}

void FArmyPunch::Update()
{
	FArmyHardware::Update();

	if ((Height + HeightToFloor > FArmySceneData::WallHeight) && (Height + HeightToFloor < FArmySceneData::WallHeight + 20))
	{
		Height = FArmySceneData::WallHeight + 20 - HeightToFloor;
	}

	StartPoint->Pos.Z = 2;
	EndPoint->Pos.Z = 2;

	RectImagePanel->Pos = GetStartPos() + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 2.f;//为了让门挡住墙体线
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;

	DashLineUpDir = HardWareRulerLine->GetDashLineUpDir();
	PunchDepthRect->Pos = GetStartPos() + Width / 2 * -DashLineUpDir + (PunchDepth) * -DashLineUpDir.GetSafeNormal();
	PunchDepthRect->XDirection = HorizontalDirection;
	PunchDepthRect->YDirection = Direction;
	PunchDepthRect->Width = Length;
	PunchDepthRect->Height = PunchDepth;
	PunchDepthRect->InCenterPos = GetPos() + (PunchDepth + Width) / 2 * -DashLineUpDir;

	if (!DashLineUpDir.IsNearlyZero())
	{
		DepthPoint->SetPos(GetPos() + (Width / 2.f + PunchDepth) * -DashLineUpDir.GetSafeNormal());
	}	
}

void FArmyPunch::Generate(UWorld* InWorld)
{
	//TArray<FVector> Vertices = GetFirstClipingBox();
	//float Height = GetHeight();	
	//UMaterialInstance* WallMaterialInstance = FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial();
	//BSPManager->CreateBsp(Vertices, Height, Brush_Subtract, WallMaterialInstance,
	//	FString::Printf(TEXT("FirstPunch_%d"), BrushIndex), false, BT_OTHER);

	////检查整个构件是不是完全投影在内墙线以内
	//FVector TempProjectionStart, TempProjectionEnd;
	//StartPoint->Pos.Z = 0;
	//EndPoint->Pos.Z = 0;
	//if (FirstRelatedRoom.IsValid() && FirstRelatedRoom->GetType() == OT_OutRoom)
	//{
	//	TempProjectionStart = FArmyMath::GetProjectionPoint(GetStartPos(), LinkSecondLine->GetStart(), LinkSecondLine->GetEnd());
	//	TempProjectionEnd = FArmyMath::GetProjectionPoint(GetEndPos(), LinkSecondLine->GetStart(), LinkSecondLine->GetEnd());
	//	if (FArmyMath::IsPointOnLineSegment2D(FVector2D(TempProjectionStart), FVector2D(LinkSecondLine->GetStart()), FVector2D(LinkSecondLine->GetEnd()), 0.05f) &&
	//		FArmyMath::IsPointOnLineSegment2D(FVector2D(TempProjectionEnd), FVector2D(LinkSecondLine->GetStart()), FVector2D(LinkSecondLine->GetEnd()), 0.05f))
	//	{
	//		bInsideInteralWall = true;
	//	}
	//	else
	//	{
	//		bInsideInteralWall = false;
	//	}
	//}
	//else if (SecondRelatedRoom.IsValid() && SecondRelatedRoom->GetType() == OT_OutRoom)
	//{

	//	TempProjectionStart = FArmyMath::GetProjectionPoint(GetStartPos(), LinkFirstLine->GetStart(), LinkFirstLine->GetEnd());
	//	TempProjectionEnd = FArmyMath::GetProjectionPoint(GetEndPos(), LinkFirstLine->GetStart(), LinkFirstLine->GetEnd());
	//	if (FArmyMath::IsPointOnLineSegment2D(FVector2D(TempProjectionStart), FVector2D(LinkFirstLine->GetStart()), FVector2D(LinkFirstLine->GetEnd()), 0.05f) &&
	//		FArmyMath::IsPointOnLineSegment2D(FVector2D(TempProjectionEnd), FVector2D(LinkFirstLine->GetStart()), FVector2D(LinkFirstLine->GetEnd()), 0.05f))
	//	{
	//		bInsideInteralWall = true;
	//	}
	//	else
	//	{
	//		bInsideInteralWall = false;
	//	}
	//}
	////没有洞深时，不构建第二个BSP 或者洞身处于内墙线之间时
	//if (FMath::IsNearlyZero(PunchDepth, KINDA_SMALL_NUMBER) || (bInsideInteralWall && (Height + HeightToFloor) <= FArmySceneData::WallHeight))
	//	return;

	//Vertices = GetSecondClipingBox();
	//BSPManager->CreateBsp(Vertices, Height, Brush_Subtract, WallMaterialInstance,
	//	FString::Printf(TEXT("SecondPunch_%d"), BrushIndex), false, BT_OTHER);
}

TArray<FVector> FArmyPunch::GetBoundingBox()
{
	TArray<FVector> results;
	return results;
}

TArray<FVector> FArmyPunch::GetFirstClipingBox()
{
	StartPoint->Pos.Z = 0;
	EndPoint->Pos.Z = 0;
	Point->Pos.Z = 0;

	TArray<FVector> BoudingBox;
	FVector HorizontalDirection = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));

	FVector TempPos = GetPos();

	FVector LeftTop = TempPos - Length / 2 * HorizontalDirection + (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector LeftBottom = TempPos - Length / 2 * HorizontalDirection - (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector RightBottom = TempPos + Length / 2 * HorizontalDirection - (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector RightTop = TempPos + Length / 2 * HorizontalDirection + (Width + SubTracteThicknessError) / 2 * VerticalDirection;
	FVector offset = FVector(0, 0, HeightToFloor + Height / 2.0f);
	BoudingBox.Push((LeftBottom + offset));
	BoudingBox.Push((LeftTop + offset));
	BoudingBox.Push((RightTop + offset));
	BoudingBox.Push((RightBottom + offset));
	return BoudingBox;
}

TArray<FVector> FArmyPunch::GetSecondClipingBox()
{
	StartPoint->Pos.Z = 0;
	EndPoint->Pos.Z = 0;
	Point->Pos.Z = 0;

	TArray<FVector> BoudingBox;
	FVector HorizontalDirection = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));

	FVector TempPos = GetPos() + (Width) / 2 * -DashLineUpDir;

	FVector LeftTop = TempPos - Length / 2 * HorizontalDirection;
	FVector RightTop = TempPos + Length / 2 * HorizontalDirection;
	FVector LeftBottom = TempPos - Length / 2 * HorizontalDirection + (PunchDepth + SubTracteThicknessError) / 1 * VerticalDirection;
	FVector RightBottom = TempPos + Length / 2 * HorizontalDirection + (PunchDepth + SubTracteThicknessError) / 1 * VerticalDirection;

	/*FVector LeftTop = TempPos - Length / 2 * HorizontalDirection + (PunchDepth + SubTracteThicknessError) / 1 * VerticalDirection;
	FVector LeftBottom = TempPos - Length / 2 * HorizontalDirection - (PunchDepth + SubTracteThicknessError) / 1 * VerticalDirection;
	FVector RightBottom = TempPos + Length / 2 * HorizontalDirection - (PunchDepth + SubTracteThicknessError) / 1 * VerticalDirection;
	FVector RightTop = TempPos + Length / 2 * HorizontalDirection + (PunchDepth + SubTracteThicknessError) / 1 * VerticalDirection;*/
	FVector offset = FVector(0, 0, HeightToFloor + Height / 2.0f);
	BoudingBox.Push((LeftBottom + offset));
	BoudingBox.Push((LeftTop + offset));
	BoudingBox.Push((RightTop + offset));
	BoudingBox.Push((RightBottom + offset));
	return BoudingBox;
}

TArray<FVector> FArmyPunch::GetClipingBox()
{
	StartPoint->Pos.Z = 0;
	EndPoint->Pos.Z = 0;
	Point->Pos.Z = 0;

	TArray<FVector> BoudingBox;
	FVector HorizontalDirection = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));
	
	FVector TempPos = GetPos() + (PunchDepth) / 2 * (DepthPoint->GetPos() - GetPos()).GetSafeNormal();

	FVector LeftTop = TempPos - Length / 2 * HorizontalDirection + (Width + PunchDepth /*+ SubTracteThicknessError*/) / 2 * VerticalDirection.GetSafeNormal();
	FVector LeftBottom = TempPos - Length / 2 * HorizontalDirection - (Width + PunchDepth /*+ SubTracteThicknessError*/) / 2 * VerticalDirection.GetSafeNormal();
	FVector RightBottom = TempPos + Length / 2 * HorizontalDirection - (Width + PunchDepth /*+ SubTracteThicknessError*/) / 2 * VerticalDirection.GetSafeNormal();
	FVector RightTop = TempPos + Length / 2 * HorizontalDirection + (Width + PunchDepth /*+ SubTracteThicknessError*/) / 2 * VerticalDirection.GetSafeNormal();
	FVector offset = FVector(0, 0, HeightToFloor + Height / 2.0f);
	BoudingBox.Push((LeftBottom + offset));
	BoudingBox.Push((LeftTop + offset));
	BoudingBox.Push((RightTop + offset));
	BoudingBox.Push((RightBottom + offset));
	return BoudingBox;
}

FVector2D FArmyPunch::GetXDir()
{
	FVector tempX = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	return FVector2D(tempX.X, tempX.Y);
}

FVector2D FArmyPunch::GetYDir()
{
	FVector HorizontalDirection = FArmyMath::GetLineDirection(StartPoint->GetPos(), EndPoint->GetPos());
	FVector VerticalDirection = HorizontalDirection.RotateAngleAxis(90, FVector(0, 0, 1));

	return FVector2D(VerticalDirection.X, VerticalDirection.Y);
}

void FArmyPunch::SetPunchDepth(float InDepth)
{
	if (FirstRelatedRoom.IsValid() && SecondRelatedRoom.IsValid() && FirstRelatedRoom->GetType() != OT_OutRoom && SecondRelatedRoom->GetType() != OT_OutRoom)
	{
		return;
	}
	PunchDepth = InDepth;
	Update();
}

void FArmyPunch::SetHeightToFloor(float InDist)
{
	HeightToFloor = InDist;
	if ((HeightToFloor + Height > FArmySceneData::WallHeight) && (HeightToFloor + Height < FArmySceneData::WallHeight + 20))
	{
		Height = FArmySceneData::WallHeight + 20 - HeightToFloor;
	}
}

void FArmyPunch::CheckIfOutLimited()
{
	if (FirstRelatedRoom.IsValid() && SecondRelatedRoom.IsValid() && LinkFirstLine.IsValid() && LinkSecondLine.IsValid() && DepthRulerLine.IsValid())
	{
		FVector2D TempOutStart, TempOutEnd, TempInternalStart, TempInternalEnd;
		if (FirstRelatedRoom->GetType() == OT_OutRoom)
		{
			TempOutStart = FVector2D(LinkFirstLine->GetStart());
			TempOutEnd = FVector2D(LinkFirstLine->GetEnd());
			TempInternalStart = FVector2D(LinkSecondLine->GetStart());
			TempInternalEnd = FVector2D(LinkSecondLine->GetEnd());
		}
		else
		{
			TempOutStart = FVector2D(LinkSecondLine->GetStart());
			TempOutEnd = FVector2D(LinkSecondLine->GetEnd());
			TempInternalStart = FVector2D(LinkFirstLine->GetStart());
			TempInternalEnd = FVector2D(LinkFirstLine->GetEnd());
		}

		FVector2D Temp2DStartPos = FVector2D(GetStartPos());
		FVector2D Temp2DEndPos = FVector2D(GetEndPos());
		FVector2D TempProjectionPos = FVector2D(FArmyMath::GetProjectionPoint(FVector(Temp2DStartPos, 0), FVector(TempInternalStart, 0), FVector(TempInternalEnd, 0)));
		if (!FArmyMath::IsPointOnLineSegment2D(TempProjectionPos, TempInternalStart, TempInternalEnd, 0.05f))
		{
			TempProjectionPos = FVector2D(FArmyMath::GetProjectionPoint(FVector(Temp2DStartPos, 0), FVector(TempOutStart, 0), FVector(TempOutEnd, 0)));
			float TempDistance1 = FVector2D::Distance(TempProjectionPos, TempOutStart);
			float TempDistance2 = FVector2D::Distance(TempProjectionPos, TempOutEnd);
			float TempDistance = TempDistance1 < TempDistance2 ? TempDistance1 : TempDistance2;
			if (!FMath::IsNearlyZero(TempDistance, 0.001f))
			{
				int flag = 1;
				if ((FirstRelatedRoom->GetType() == OT_OutRoom && !FirstRelatedRoom->IsPointInRoom(FVector(TempProjectionPos, 0))) ||
					(SecondRelatedRoom->GetType() == OT_OutRoom && !SecondRelatedRoom->IsPointInRoom(FVector(TempProjectionPos, 0))))
				{
					flag = -1;
				}
				FVector TempDist = TempDistance * (GetEndPos() - GetStartPos()).GetSafeNormal();
				StartPoint->SetPos(StartPoint->GetPos() - flag * TempDist);
				UpdateStartPoint();
			}
		}
		TempProjectionPos = FVector2D(FArmyMath::GetProjectionPoint(FVector(Temp2DEndPos, 0), FVector(TempInternalStart, 0), FVector(TempInternalEnd, 0)));
		if (!FArmyMath::IsPointOnLineSegment2D(TempProjectionPos, TempInternalStart, TempInternalEnd, 0.05f))
		{
			TempProjectionPos = FVector2D(FArmyMath::GetProjectionPoint(FVector(Temp2DEndPos, 0), FVector(TempOutStart, 0), FVector(TempOutEnd, 0)));
			float TempDistance1 = FVector2D::Distance(TempProjectionPos, TempOutStart);
			float TempDistance2 = FVector2D::Distance(TempProjectionPos, TempOutEnd);
			float TempDistance = TempDistance1 < TempDistance2 ? TempDistance1 : TempDistance2;
			if (!FMath::IsNearlyZero(TempDistance, 0.001f))
			{
				int flag = 1;
				if ((FirstRelatedRoom->GetType() == OT_OutRoom && !FirstRelatedRoom->IsPointInRoom(FVector(TempProjectionPos, 0))) ||
					(SecondRelatedRoom->GetType() == OT_OutRoom && !SecondRelatedRoom->IsPointInRoom(FVector(TempProjectionPos, 0))))
				{
					flag = -1;
				}
				FVector TempDist = TempDistance2 * (GetStartPos() - GetEndPos()).GetSafeNormal();
				EndPoint->SetPos(EndPoint->GetPos() - flag * TempDist);
				UpdateStartPoint();
			}
		}
	}
}

// 获得绘制CAD的线段列表
void FArmyPunch::GetDrawCadLines(TArray<TPair<FVector, FVector>>& OutLineList)
{
	float ToleranceFloat = 0.01;
	TArray<FVector> FirstVectors = GetFirstClipingBox();
	int32 FirstCount = FirstVectors.Num();
	for (int32 i = 0; i < FirstCount; i++)
	{
		OutLineList.Emplace(TPair<FVector, FVector>(FirstVectors[i%FirstCount], FirstVectors[(i + 1)% FirstCount]));
	}

	TArray<FVector> SecondVectors = GetSecondClipingBox();
	int32 SecondCount = SecondVectors.Num();
	for (int32 i = 0; i < SecondVectors.Num(); i++)
	{
		FVector StartVector = SecondVectors[i%SecondCount];
		FVector EndVector = SecondVectors[(i + 1)% SecondCount];

		bool InsertFlag = true;
		for (auto It : OutLineList)
		{
			if ((StartVector.Equals(It.Key, ToleranceFloat) && EndVector.Equals(It.Value, ToleranceFloat)) ||
				(StartVector.Equals(It.Value, ToleranceFloat) && EndVector.Equals(It.Key, ToleranceFloat)))
			{
				InsertFlag = false;
				break;
			}
		}
		if (InsertFlag)
		{
			OutLineList.Emplace(TPair<FVector, FVector>(StartVector, EndVector));
		}
	}
}

void FArmyPunch::HideRulerLineInputBox(bool bHide)
{
	FArmyHardware::HideRulerLineInputBox(bHide);
	DepthRulerLine->ShowInputBox(!bHide);
}

void FArmyPunch::OnFirstLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstLineLeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector TempProjectPoint = FArmyMath::GetProjectionPoint(FirstDestinationStart, GetStartPos(), GetEndPos());
		FVector TempDist = TempLength * (TempProjectPoint - GetStartPos()).GetSafeNormal();
		TempDist.Z = 0;
		StartPoint->SetPos(StartPoint->GetPos() + TempDist);
		UpdateStartPoint();

		if (TempInValue != 0)
		{
			CheckIfOutLimited();
		}
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

void FArmyPunch::OnFirstLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = FirstLineRightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector TempProjectPoint = FArmyMath::GetProjectionPoint(FirstDestinationEnd, GetStartPos(), GetEndPos());
		FVector TempDist = TempLength * (TempProjectPoint - GetEndPos()).GetSafeNormal();
		TempDist.Z = 0;
		EndPoint->SetPos(EndPoint->GetPos() + TempDist);
		UpdateEndPoint();

		if (TempInValue != 0)
		{
			CheckIfOutLimited();
		}
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

void FArmyPunch::OnSecondLineLInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondLineLeftRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector TempProjectPoint = FArmyMath::GetProjectionPoint(SecondDestinationStart, GetStartPos(), GetEndPos());
		FVector TempDist = TempLength * (TempProjectPoint - GetStartPos()).GetSafeNormal();
		TempDist.Z = 0;
		StartPoint->SetPos(StartPoint->GetPos() + TempDist);
		UpdateStartPoint();

		if (TempInValue != 0)
		{
			CheckIfOutLimited();
		}
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

void FArmyPunch::OnSecondLineRInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float CachedLength = SecondLineRightRulerLine->GetInputBoxCachedLength();
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		float TempLength = CachedLength - TempInValue;
		FVector TempProjectPoint = FArmyMath::GetProjectionPoint(SecondDestinationEnd, GetStartPos(), GetEndPos());
		FVector TempDist = TempLength * (TempProjectPoint - GetEndPos()).GetSafeNormal();
		TempDist.Z = 0;
		EndPoint->SetPos(EndPoint->GetPos() + TempDist);
		UpdateEndPoint();

		if (TempInValue != 0)
		{
			CheckIfOutLimited();
		}
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

void FArmyPunch::OnDepthInputBoxCommitted(const FText & InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float TempInValue = FCString::Atof(*InText.ToString()) / 10.f;
		SetPunchDepth(TempInValue);
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}
