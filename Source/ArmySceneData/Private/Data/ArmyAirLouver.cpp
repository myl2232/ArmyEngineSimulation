#include "ArmyAirLouver.h"
#include "ArmyRect.h"

FArmyAirLouver::FArmyAirLouver()
	: FArmyHardware()
	, LeftLengthLine(MakeShareable(new FArmyLine()))
	, RightLengthLine(MakeShareable(new FArmyLine()))
	, LeftWidthLine(MakeShareable(new FArmyLine()))
	, RightWidthLine(MakeShareable(new FArmyLine()))
{
	ObjectType = OT_AirLouver;
	SetPropertyFlag(FLAG_COLLISION, true);
	SetName(TEXT("空调孔"));

	Length = defaultDiameter;
	Width = defaultWidth;
	Height = defaultHeight;	

	LeftLengthLine->bIsDashLine = true;
	RightLengthLine->bIsDashLine = true;
	LeftWidthLine->bIsDashLine = true;
	RightWidthLine->bIsDashLine = true;

	SetState(OS_Normal);

	DirectionPoint->SetState(FArmyPrimitive::OPS_Disable);	 

}

FArmyAirLouver::FArmyAirLouver(FArmyAirLouver * Copy)
{
	ObjectType = Copy->ObjectType;
	SetName(Copy->GetName());
	bBoundingBox = Copy->bBoundingBox;
	State = Copy->GetState();
	PropertyFlag = Copy->PropertyFlag;

	DirectionPoint = MakeShareable(new FArmyEditPoint(Copy->DirectionPoint.Get()));
	StartPoint = MakeShareable(new FArmyEditPoint(Copy->StartPoint.Get()));
	EndPoint = MakeShareable(new FArmyEditPoint(Copy->EndPoint.Get()));
	Point = MakeShareable(new FArmyEditPoint(Copy->Point.Get()));

	LeftLengthLine = MakeShareable(new FArmyLine(Copy->LeftLengthLine.Get()));
	RightLengthLine = MakeShareable(new FArmyLine(Copy->RightLengthLine.Get()));
	LeftWidthLine = MakeShareable(new FArmyLine(Copy->LeftWidthLine.Get()));
	RightWidthLine = MakeShareable(new FArmyLine(Copy->RightWidthLine.Get()));

	RectImagePanel = MakeShareable(new FArmyRect(Copy->RectImagePanel.Get()));

	SetPos(Copy->GetPos());
	Direction = Copy->Direction;
	Length = Copy->Length;
	Width = Copy->Width;
	Height = Copy->Height;	

	Update();
}

FArmyAirLouver::~FArmyAirLouver()
{
}

void FArmyAirLouver::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyHardware::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("pos", GetPos().ToString());
	JsonWriter->WriteValue("direction", GetDirection().ToString());
	JsonWriter->WriteValue("length", GetLength());
	JsonWriter->WriteValue("width", GetWidth());
	JsonWriter->WriteValue("height", GetHeight());
	JsonWriter->WriteValue("heightToFloor", HeightToFloor);
	JsonWriter->WriteValue("bRightOpen", IsRightOpen());

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyAirLouver)
}

void FArmyAirLouver::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyHardware::Deserialization(InJsonData);

	FVector TempPos, TempDirection;
	TempPos.InitFromString(InJsonData->GetStringField("pos"));
	TempDirection.InitFromString(InJsonData->GetStringField("direction"));
	SetPos(TempPos);
	SetDirection(TempDirection);
	SetLength(InJsonData->GetNumberField("length"));
	SetWidth(InJsonData->GetNumberField("Width"));
	SetHeight(InJsonData->GetNumberField("height"));
	SetRightOpen(InJsonData->GetBoolField("bRightOpen"));

	bool bCapture = CaptureDoubleLine(TempPos, E_LayoutModel);
}

void FArmyAirLouver::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		LeftLengthLine->SetBaseColor(FLinearColor(FColor(0xFFFFF700)));
		RightLengthLine->SetBaseColor(FLinearColor(FColor(0xFFFFF700)));
		LeftWidthLine->SetBaseColor(FLinearColor(FColor(0xFFFFF700)));
		RightWidthLine->SetBaseColor(FLinearColor(FColor(0xFFFFF700)));
		DeselectPoints();
		break;

	case OS_Hovered:
	case OS_Selected:
		LeftLengthLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightLengthLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		LeftWidthLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		RightWidthLine->SetBaseColor(FLinearColor(FColor(0xFFFF9800)));
		break;

	case OS_Disable:
		LeftLengthLine->SetBaseColor(FLinearColor::Gray);
		RightLengthLine->SetBaseColor(FLinearColor::Gray);
		LeftWidthLine->SetBaseColor(FLinearColor::Gray);
		RightWidthLine->SetBaseColor(FLinearColor::Gray);
		break;

	default:
		break;
	}
}

void FArmyAirLouver::Draw(FPrimitiveDrawInterface * PDI, const FSceneView * View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		LeftLengthLine->Draw(PDI, View);
		RightLengthLine->Draw(PDI, View);
		LeftWidthLine->Draw(PDI, View);
		RightWidthLine->Draw(PDI, View);		
		if (State == OS_Selected)
		{			
			if (bShowEditPoint)
			{
				StartPoint->Draw(PDI, View);
				EndPoint->Draw(PDI, View);
				Point->Draw(PDI, View);				
			}
		}
	}
}

bool FArmyAirLouver::IsSelected(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		bShowDashLine = true;
		return RectImagePanel->IsSelected(Pos, InViewportClient);
	}
	return false;
}

bool FArmyAirLouver::Hover(const FVector & Pos, UArmyEditorViewportClient * InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return RectImagePanel->IsSelected(Pos, InViewportClient);
	}
	return false;
}

const FBox FArmyAirLouver::GetBounds()
{
	return FBox();
}

void FArmyAirLouver::Destroy()
{
}

void FArmyAirLouver::SetPropertyFlag(PropertyType InType, bool InUse)
{
}

void FArmyAirLouver::Update()
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

	RectImagePanel->Pos = GetStartPos() + Width / 2 * Direction;
	RectImagePanel->Pos.Z = 1.f;
	RectImagePanel->XDirection = HorizontalDirection;
	RectImagePanel->YDirection = -Direction;
	RectImagePanel->Width = Length;
	RectImagePanel->Height = Width;

	LeftWidthLine->SetStart(StartPoint->GetPos() + (Direction * (Width + widthOffset) / 2.f));
	LeftWidthLine->SetEnd(StartPoint->GetPos() - (Direction * (Width + widthOffset) / 2.f));

	RightWidthLine->SetStart(EndPoint->GetPos() + (Direction * (Width + widthOffset) / 2.f));
	RightWidthLine->SetEnd(EndPoint->GetPos() - (Direction * (Width + widthOffset) / 2.f));
	
	LeftLengthLine->SetStart(LeftWidthLine->GetStart());
	LeftLengthLine->SetEnd(RightWidthLine->GetStart());

	RightLengthLine->SetStart(LeftWidthLine->GetEnd());
	RightLengthLine->SetEnd(RightWidthLine->GetEnd());
}
