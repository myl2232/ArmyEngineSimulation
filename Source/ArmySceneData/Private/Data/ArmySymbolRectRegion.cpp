#include "ArmySymbolRectRegion.h"

FArmySymbolRectRegion::FArmySymbolRectRegion()
{
	TSharedPtr<FArmyEditPoint> LMainP = MakeShareable(new FArmyEditPoint);
	TSharedPtr<FArmyEditPoint> RMainP = MakeShareable(new FArmyEditPoint);
	TSharedPtr<FArmyEditPoint> LHP = MakeShareable(new FArmyEditPoint);
	TSharedPtr<FArmyEditPoint> RHP = MakeShareable(new FArmyEditPoint);

	LeftLine = MakeShareable(new FArmyLine(LMainP, LHP));
	LeftLine->bIsDashLine = true;
	LeftLine->SetState(FArmyPrimitive::OPS_Selected);
	LeftLine->ShowEditPoint = true;
	RightLine = MakeShareable(new FArmyLine(RHP, RMainP));
	RightLine->bIsDashLine = true;
	RightLine->ShowEditPoint = true;
	RightLine->SetState(FArmyPrimitive::OPS_Selected);
	FaceLine = MakeShareable(new FArmyLine(LHP, RHP));
	FaceLine->bIsDashLine = true;
	FaceLine->ShowEditPoint = true;
	FaceLine->SetState(FArmyPrimitive::OPS_Selected);
	MainLine = MakeShareable(new FArmyLine(LMainP, RMainP));
	MainLine->SetState(FArmyPrimitive::OPS_Selected);
	MainLine->ShowEditPoint = true;
}
FArmySymbolRectRegion::~FArmySymbolRectRegion()
{

}
const FBox FArmySymbolRectRegion::GetBounds()
{
	FBox TempBox(ForceInitToZero);
	TempBox += MainLine->GetStart();
	TempBox += MainLine->GetEnd();
	TempBox += FaceLine->GetStart();
	TempBox += FaceLine->GetEnd();
	return TempBox;
}
void FArmySymbolRectRegion::Init(const FVector& InBasePos,FArmySymbolRectRegion::EDirectionType InType)
{
	DirectionType = InType;
	m_VInBasePos = InBasePos;
	/*switch (InType)
	{
	case FArmySymbolRectRegion::EDT_LEFT:
	{
		FVector MainStart = InBasePos + FVector(0, 1, 0) * DefaultLenth / 2;
		FVector MainEnd = InBasePos - FVector(0, 1, 0) * DefaultLenth / 2;

		FVector FaceStart = InBasePos + FVector(0, 1, 0) * DefaultLenth / 2 + FVector(-1,0,0) * DefaultWidth;
		FVector FaceEnd = InBasePos - FVector(0, 1, 0) * DefaultLenth / 2 + FVector(-1, 0, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
		break;
	case FArmySymbolRectRegion::EDT_RIGHT:
	{
		FVector MainStart = InBasePos - FVector(0, 1, 0) * DefaultLenth / 2;
		FVector MainEnd = InBasePos + FVector(0, 1, 0) * DefaultLenth / 2;

		FVector FaceStart = InBasePos - FVector(0, 1, 0) * DefaultLenth / 2 + FVector(1, 0, 0) * DefaultWidth;
		FVector FaceEnd = InBasePos + FVector(0, 1, 0) * DefaultLenth / 2 + FVector(1, 0, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
		break;
	case FArmySymbolRectRegion::EDT_TOP:
	{
		FVector MainStart = InBasePos - FVector(1, 0, 0) * DefaultLenth / 2;
		FVector MainEnd = InBasePos + FVector(1, 0, 0) * DefaultLenth / 2;

		FVector FaceStart = InBasePos - FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, -1, 0) * DefaultWidth;
		FVector FaceEnd = InBasePos + FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, -1, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
		break;
	case FArmySymbolRectRegion::EDT_BOTTOM:
	{
		FVector MainStart = InBasePos + FVector(1, 0, 0) * DefaultLenth / 2;
		FVector MainEnd = InBasePos - FVector(1, 0, 0) * DefaultLenth / 2;

		FVector FaceStart = InBasePos + FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, 1, 0) * DefaultWidth;
		FVector FaceEnd = InBasePos - FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, 1, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
		break;
	default:
		break;
	}*/
}
void FArmySymbolRectRegion::Update(TSharedPtr<FArmyEditPoint> InEditPoint, const FVector& InOffset)
{
	if (MainLine->GetLineEditPoint() == InEditPoint)
	{
		FVector PreOffset = FaceLine->GetStart() - MainLine->GetStart();
		FVector TempOffset = InOffset.ProjectOnTo(PreOffset);
		if ((TempOffset.GetSafeNormal() - PreOffset.GetSafeNormal()).Size() < 0.1 && TempOffset.Size() >= PreOffset.Size())
		{
			return;
		}
		FVector LastStart = MainLine->GetStart() + TempOffset;
		FVector LastEnd = MainLine->GetEnd() + TempOffset;

		MainLine->SetStart(LastStart);
		MainLine->SetEnd(LastEnd);
	}
	else if (FaceLine->GetLineEditPoint() == InEditPoint)
	{
		FVector PreOffset = FaceLine->GetStart() - MainLine->GetStart();
		FVector TempOffset = InOffset.ProjectOnTo(PreOffset);
		if ((TempOffset.GetSafeNormal() + PreOffset.GetSafeNormal()).Size() < 0.1 && TempOffset.Size() >= PreOffset.Size())
		{
			return;
		}
		FVector LastStart = FaceLine->GetStart() + TempOffset;
		FVector LastEnd = FaceLine->GetEnd() + TempOffset;

		FaceLine->SetStart(LastStart);
		FaceLine->SetEnd(LastEnd);
	}
	else if (LeftLine->GetLineEditPoint() == InEditPoint)
	{
		FVector PreOffset = MainLine->GetEnd() - MainLine->GetStart();
		FVector TempOffset = InOffset.ProjectOnTo(PreOffset);
		if ((TempOffset.GetSafeNormal() - PreOffset.GetSafeNormal()).Size() < 0.1 && TempOffset.Size() >= PreOffset.Size())
		{
			return;
		}
		FVector FaceStart = FaceLine->GetStart() + TempOffset;
		FVector MainStart = MainLine->GetStart() + TempOffset;

		FaceLine->SetStart(FaceStart);
		MainLine->SetStart(MainStart);
	}
	else if (RightLine->GetLineEditPoint() == InEditPoint)
	{
		FVector PreOffset = MainLine->GetEnd() - MainLine->GetStart();
		FVector TempOffset = InOffset.ProjectOnTo(PreOffset);
		if ((TempOffset.GetSafeNormal() + PreOffset.GetSafeNormal()).Size() < 0.1 && TempOffset.Size() >= PreOffset.Size())
		{
			return;
		}
		FVector FaceEnd = FaceLine->GetEnd() + TempOffset;
		FVector MainEnd = MainLine->GetEnd() + TempOffset;

		FaceLine->SetEnd(FaceEnd);
		MainLine->SetEnd(MainEnd);
	}
	else if (MainLine->GetStartPointer() == InEditPoint)
	{
		switch (DirectionType)
		{
		case FArmySymbolRectRegion::EDT_LEFT:
		{
			FVector TempStart = MainLine->GetStart() + InOffset;
			if (TempStart.X <= FaceLine->GetStart().X || TempStart.Y <= MainLine->GetEnd().Y)
			{
				return;
			}
			else
			{
				FVector TempEnd = MainLine->GetEnd();
				TempEnd.X = TempStart.X;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempStart = FaceLine->GetStart();
				FaceTempStart.Y = TempStart.Y;
				FaceLine->SetStart(FaceTempStart);
			}
		}
			break;
		case FArmySymbolRectRegion::EDT_RIGHT:
		{
			FVector TempStart = MainLine->GetStart() + InOffset;
			if (TempStart.X >= FaceLine->GetStart().X || TempStart.Y >= MainLine->GetEnd().Y)
			{
				return;
			}
			else
			{
				FVector TempEnd = MainLine->GetEnd();
				TempEnd.X = TempStart.X;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempStart = FaceLine->GetStart();
				FaceTempStart.Y = TempStart.Y;
				FaceLine->SetStart(FaceTempStart);
			}
		}
			break;
		case FArmySymbolRectRegion::EDT_TOP:
		{
			FVector TempStart = MainLine->GetStart() + InOffset;
			if (TempStart.Y <= FaceLine->GetStart().Y || TempStart.X >= MainLine->GetEnd().X)
			{
				return;
			}
			else
			{
				FVector TempEnd = MainLine->GetEnd();
				TempEnd.Y = TempStart.Y;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempStart = FaceLine->GetStart();
				FaceTempStart.X = TempStart.X;
				FaceLine->SetStart(FaceTempStart);
			}
		}
			break;
		case FArmySymbolRectRegion::EDT_BOTTOM:
		{
			FVector TempStart = MainLine->GetStart() + InOffset;
			if (TempStart.Y >= FaceLine->GetStart().Y || TempStart.X <= MainLine->GetEnd().X)
			{
				return;
			}
			else
			{
				FVector TempEnd = MainLine->GetEnd();
				TempEnd.Y = TempStart.Y;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempStart = FaceLine->GetStart();
				FaceTempStart.X = TempStart.X;
				FaceLine->SetStart(FaceTempStart);
			}
		}
			break;
		default:
			break;
		}
	}
	else if (MainLine->GetEndPointer() == InEditPoint)
	{
		switch (DirectionType)
		{
		case FArmySymbolRectRegion::EDT_LEFT:
		{
			FVector TempEnd = MainLine->GetEnd() + InOffset;
			if (TempEnd.X <= FaceLine->GetEnd().X || TempEnd.Y >= MainLine->GetStart().Y)
			{
				return;
			}
			else
			{
				FVector TempStart = MainLine->GetStart();
				TempStart.X = TempEnd.X;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempEnd = FaceLine->GetEnd();
				FaceTempEnd.X = TempEnd.X;
				FaceLine->SetEnd(FaceTempEnd);
			}
		}
		break;
		case FArmySymbolRectRegion::EDT_RIGHT:
		{
			FVector TempEnd = MainLine->GetEnd() + InOffset;
			if (TempEnd.X >= FaceLine->GetEnd().X || TempEnd.Y <= MainLine->GetStart().Y)
			{
				return;
			}
			else
			{
				FVector TempStart = MainLine->GetStart();
				TempStart.X = TempEnd.X;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempEnd = FaceLine->GetEnd();
				FaceTempEnd.Y = TempEnd.Y;
				FaceLine->SetEnd(FaceTempEnd);
			}
		}
		break;
		case FArmySymbolRectRegion::EDT_TOP:
		{
			FVector TempEnd = MainLine->GetEnd() + InOffset;
			if (TempEnd.Y <= FaceLine->GetEnd().Y || TempEnd.X <= MainLine->GetStart().X)
			{
				return;
			}
			else
			{
				FVector TempStart = MainLine->GetStart();
				TempStart.Y = TempEnd.Y;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempEnd = FaceLine->GetEnd();
				FaceTempEnd.X = TempEnd.X;
				FaceLine->SetEnd(FaceTempEnd);
			}
		}
			break;
		case FArmySymbolRectRegion::EDT_BOTTOM:
		{
			FVector TempEnd = MainLine->GetEnd() + InOffset;
			if (TempEnd.Y >= FaceLine->GetEnd().Y || TempEnd.X >= MainLine->GetStart().X)
			{
				return;
			}
			else
			{
				FVector TempStart = MainLine->GetStart();
				TempStart.Y = TempEnd.Y;
				MainLine->SetStart(TempStart);
				MainLine->SetEnd(TempEnd);

				FVector FaceTempEnd = FaceLine->GetEnd();
				FaceTempEnd.X = TempEnd.X;
				FaceLine->SetEnd(FaceTempEnd);
			}
		}
			break;
		default:
			break;
		}
	}
	else if (FaceLine->GetStartPointer() == InEditPoint)
	{
		switch (DirectionType)
		{
		case FArmySymbolRectRegion::EDT_LEFT:
		{
			FVector TempStart = FaceLine->GetStart() + InOffset;
			if (TempStart.X >= MainLine->GetStart().X || TempStart.Y <= FaceLine->GetEnd().Y)
			{
				return;
			}
			else
			{
				FVector TempEnd = FaceLine->GetEnd();
				TempEnd.X = TempStart.X;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempStart = MainLine->GetStart();
				MainTempStart.Y = TempStart.Y;
				MainLine->SetStart(MainTempStart);
			}
		}
		break;
		case FArmySymbolRectRegion::EDT_RIGHT:
		{
			FVector TempStart = FaceLine->GetStart() + InOffset;
			if (TempStart.X <= MainLine->GetStart().X || TempStart.Y >= FaceLine->GetEnd().Y)
			{
				return;
			}
			else
			{
				FVector TempEnd = FaceLine->GetEnd();
				TempEnd.X = TempStart.X;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempStart = MainLine->GetStart();
				MainTempStart.Y = TempStart.Y;
				MainLine->SetStart(MainTempStart);
			}
		}
		break;
		case FArmySymbolRectRegion::EDT_TOP:
		{
			FVector TempStart = FaceLine->GetStart() + InOffset;
			if (TempStart.Y >= MainLine->GetStart().Y || TempStart.X >= FaceLine->GetEnd().X)
			{
				return;
			}
			else
			{
				FVector TempEnd = FaceLine->GetEnd();
				TempEnd.Y = TempStart.Y;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempStart = MainLine->GetStart();
				MainTempStart.X = TempStart.X;
				MainLine->SetStart(MainTempStart);
			}
		}
			break;
		case FArmySymbolRectRegion::EDT_BOTTOM:
		{
			FVector TempStart = FaceLine->GetStart() + InOffset;
			if (TempStart.Y <= MainLine->GetStart().Y || TempStart.X <= FaceLine->GetEnd().X)
			{
				return;
			}
			else
			{
				FVector TempEnd = FaceLine->GetEnd();
				TempEnd.Y = TempStart.Y;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempStart = MainLine->GetStart();
				MainTempStart.X = TempStart.X;
				MainLine->SetStart(MainTempStart);
			}
		}
			break;
		default:
			break;
		}
	}
	else if (FaceLine->GetEndPointer() == InEditPoint)
	{
		switch (DirectionType)
		{
		case FArmySymbolRectRegion::EDT_LEFT:
		{
			FVector TempEnd = FaceLine->GetEnd() + InOffset;
			if (TempEnd.X >= MainLine->GetEnd().X || TempEnd.Y >= FaceLine->GetStart().Y)
			{
				return;
			}
			else
			{
				FVector TempStart = FaceLine->GetStart();
				TempStart.X = TempEnd.X;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempEnd = MainLine->GetEnd();
				MainTempEnd.Y = TempEnd.Y;
				MainLine->SetEnd(MainTempEnd);
			}
		}
		break;
		case FArmySymbolRectRegion::EDT_RIGHT:
		{
			FVector TempEnd = FaceLine->GetEnd() + InOffset;
			if (TempEnd.X <= MainLine->GetEnd().X || TempEnd.Y <= FaceLine->GetStart().Y)
			{
				return;
			}
			else
			{
				FVector TempStart = FaceLine->GetStart();
				TempStart.X = TempEnd.X;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempEnd = MainLine->GetEnd();
				MainTempEnd.Y = TempEnd.Y;
				MainLine->SetEnd(MainTempEnd);
			}
		}
		break;
		case FArmySymbolRectRegion::EDT_TOP:
		{
			FVector TempEnd = FaceLine->GetEnd() + InOffset;
			if (TempEnd.Y >= MainLine->GetEnd().Y || TempEnd.X <= FaceLine->GetStart().X)
			{
				return;
			}
			else
			{
				FVector TempStart = FaceLine->GetStart();
				TempStart.Y = TempEnd.Y;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempEnd = MainLine->GetEnd();
				MainTempEnd.X = TempEnd.X;
				MainLine->SetEnd(MainTempEnd);
			}
		}
			break;
		case FArmySymbolRectRegion::EDT_BOTTOM:
		{
			FVector TempEnd = FaceLine->GetEnd() + InOffset;
			if (TempEnd.Y <= MainLine->GetEnd().Y || TempEnd.X >= FaceLine->GetStart().X)
			{
				return;
			}
			else
			{
				FVector TempStart = FaceLine->GetStart();
				TempStart.Y = TempEnd.Y;
				FaceLine->SetStart(TempStart);
				FaceLine->SetEnd(TempEnd);

				FVector MainTempEnd = MainLine->GetEnd();
				MainTempEnd.X = TempEnd.X;
				MainLine->SetEnd(MainTempEnd);
			}
		}
			break;
		default:
			break;
		}
	}
}
void FArmySymbolRectRegion::SetSelectRectData(const float InNewLength, const float InNewWidth)
{
	DefaultLenth = InNewLength;
	DefaultWidth = InNewWidth;

	switch (DirectionType)
	{
	case FArmySymbolRectRegion::EDT_LEFT:
	{
		FVector MainStart = m_VInBasePos + FVector(0, 1, 0) * DefaultLenth / 2;
		FVector MainEnd = m_VInBasePos - FVector(0, 1, 0) * DefaultLenth / 2;

		FVector FaceStart = m_VInBasePos + FVector(0, 1, 0) * DefaultLenth / 2 + FVector(-1, 0, 0) * DefaultWidth;
		FVector FaceEnd = m_VInBasePos - FVector(0, 1, 0) * DefaultLenth / 2 + FVector(-1, 0, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
	break;
	case FArmySymbolRectRegion::EDT_RIGHT:
	{
		FVector MainStart = m_VInBasePos - FVector(0, 1, 0) * DefaultLenth / 2;
		FVector MainEnd = m_VInBasePos + FVector(0, 1, 0) * DefaultLenth / 2;

		FVector FaceStart = m_VInBasePos - FVector(0, 1, 0) * DefaultLenth / 2 + FVector(1, 0, 0) * DefaultWidth;
		FVector FaceEnd = m_VInBasePos + FVector(0, 1, 0) * DefaultLenth / 2 + FVector(1, 0, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
	break;
	case FArmySymbolRectRegion::EDT_TOP:
	{
		FVector MainStart = m_VInBasePos - FVector(1, 0, 0) * DefaultLenth / 2;
		FVector MainEnd = m_VInBasePos + FVector(1, 0, 0) * DefaultLenth / 2;

		FVector FaceStart = m_VInBasePos - FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, -1, 0) * DefaultWidth;
		FVector FaceEnd = m_VInBasePos + FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, -1, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
	break;
	case FArmySymbolRectRegion::EDT_BOTTOM:
	{
		FVector MainStart = m_VInBasePos + FVector(1, 0, 0) * DefaultLenth / 2;
		FVector MainEnd = m_VInBasePos - FVector(1, 0, 0) * DefaultLenth / 2;

		FVector FaceStart = m_VInBasePos + FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, 1, 0) * DefaultWidth;
		FVector FaceEnd = m_VInBasePos - FVector(1, 0, 0) * DefaultLenth / 2 + FVector(0, 1, 0) * DefaultWidth;

		MainLine->SetStart(MainStart);
		MainLine->SetEnd(MainEnd);

		FaceLine->SetStart(FaceStart);
		FaceLine->SetEnd(FaceEnd);
	}
	break;
	default:
		break;
	}
}
void FArmySymbolRectRegion::SetBasePos(const FVector InBasePos)
{
	m_VInBasePos = InBasePos;
}
void FArmySymbolRectRegion::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("RectRegionBasePos", m_VInBasePos.ToString());
	JsonWriter->WriteValue("RectRegionWidth", DefaultWidth);
	JsonWriter->WriteValue("RectRegionLength", DefaultLenth);

}
void FArmySymbolRectRegion::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	m_VInBasePos.InitFromString(InJsonData->GetStringField("RectRegionBasePos"));
	DefaultWidth = InJsonData->GetNumberField("RectRegionWidth");
	DefaultLenth = InJsonData->GetNumberField("RectRegionLength");

	SetSelectRectData(InJsonData->GetNumberField("RectRegionLength"), InJsonData->GetNumberField("RectRegionWidth"));
}
void FArmySymbolRectRegion::SetState(EObjectState InState)
{

}
void FArmySymbolRectRegion::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FLAG_VISIBILITY))
	{
		LeftLine->Draw(PDI, View);
		LeftLine->GetLineEditPoint()->Draw(PDI, View);
		RightLine->Draw(PDI, View);
		RightLine->GetLineEditPoint()->Draw(PDI, View);

		FaceLine->Draw(PDI, View);
		FaceLine->GetStartPointer()->Draw(PDI, View);
		FaceLine->GetEndPointer()->Draw(PDI, View);
		FaceLine->GetLineEditPoint()->Draw(PDI, View);
		MainLine->Draw(PDI, View);
		MainLine->GetStartPointer()->Draw(PDI, View);
		MainLine->GetEndPointer()->Draw(PDI, View);
		MainLine->GetLineEditPoint()->Draw(PDI, View);
	}
}
bool FArmySymbolRectRegion::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return false;
}
TSharedPtr<FArmyEditPoint> FArmySymbolRectRegion::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	TSharedPtr<FArmyEditPoint> SelectPoint = MainLine->SelectEditPoint(Pos, InViewportClient);
	if (SelectPoint.IsValid())
	{
		return SelectPoint;
	}
	SelectPoint = FaceLine->SelectEditPoint(Pos, InViewportClient);
	if (SelectPoint.IsValid())
	{
		return SelectPoint;
	}
	SelectPoint = LeftLine->SelectEditPoint(Pos, InViewportClient);
	if (SelectPoint.IsValid())
	{
		return SelectPoint;
	}
	SelectPoint = RightLine->SelectEditPoint(Pos, InViewportClient);
	if (SelectPoint.IsValid())
	{
		return SelectPoint;
	}
	return nullptr;
}