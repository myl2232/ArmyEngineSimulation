#include "ArmyTextLabel.h"
#include "ArmyEditableLabelSlate.h"
#include "CanvasItem.h"
#include "ArmyEditPoint.h"
#include "ArmyEditorViewportClient.h"
#include "SMultiLineEditableTextBox.h"

class FCanvasTileTextItem : public FCanvasTextItem
{
public:
	FCanvasTileTextItem(const FVector2D& InPosition, const FText& InText, const UFont* InFont, const FLinearColor& InColor)
		:FCanvasTextItem(InPosition, InText, InFont, InColor)
	{
	}
	FVector2D GetSize() {
		return TileItem.Size;
	};
	virtual void Draw(FCanvas* InCanvas) override
	{
		if (bDontCorrectStereoscopic || !GEngine->IsStereoscopic3D())
		{
			if ((bCentreX || bCentreY))
			{
				FVector2D DrawPos(Position.X, Position.Y);

				const FVector2D MeasuredTextSize = GetTextSize();

				// Calculate the offset if we are centering
				if (bCentreX || bCentreY)
				{
					// Note we drop the fraction after the length divide or we can end up with coords on 1/2 pixel boundaries
					if (bCentreX)
					{
						DrawPos.X -= (int)(MeasuredTextSize.X / 2);
					}
					if (bCentreY)
					{
						DrawPos.Y -= (int)(MeasuredTextSize.Y / 2);
					}
				}

				const FVector2D StereoOutlineBoxSize(2.0f, 2.0f);
				TileItem.SetColor(FLinearColor::White);
				//TileItem.MaterialRenderProxy = GEngine->RemoveSurfaceMaterial->GetRenderProxy(false);
				TileItem.Position = DrawPos - StereoOutlineBoxSize;
				const FVector2D CorrectionSize = MeasuredTextSize + StereoOutlineBoxSize + StereoOutlineBoxSize;
				TileItem.Size = CorrectionSize;
				TileItem.bFreezeTime = true;
				TileItem.Draw(InCanvas);
			}
		}

		FCanvasTextItem::Draw(InCanvas);
	}
};

FArmyTextLabel::FArmyTextLabel()
{
	SetName(TEXT("文本标签") + GetUniqueID().ToString());
	ObjectType = OT_TextLabel;
	CurrentLabelType = LT_None;
	EditPoint = MakeShareable(new FArmyEditPoint());
	EditPoint->OperationType = XROP_MOVE;
	TextColor = FLinearColor::White;
}

FArmyTextLabel::~FArmyTextLabel()
{
}
void FArmyTextLabel::Init(TSharedPtr<SOverlay> ParentWidget)
{
	SAssignNew(TextInputBox, SMultiLineEditableTextBox)
		.OnTextCommitted_Raw(this, &FArmyTextLabel::OnTextCommitted)
		.Visibility(EVisibility::Collapsed);

	ParentWidget->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.MinDesiredWidth(32)
		[
			TextInputBox.ToSharedRef()
		]
		];

	//TextInputBox->OnTextCommitted = FOnInPutValueCommited::CreateRaw(this, &FArmyTextLabel::OnTextCommitted);
}
void FArmyTextLabel::SetTextSize(int32 InSize)
{
	TextSize = InSize;
}
int32 FArmyTextLabel::GetTextSize()
{
	return TextSize;
}
void FArmyTextLabel::SetLabelType(LabelType InType)
{
	CurrentLabelType = InType;
	DrawLines.Empty();
	RebuildDrawLines();
}
void FArmyTextLabel::SetObjTag(FObjectPtr InObj, int32 InTag)
{
	BindObject = InObj;
	ObjTag = InTag;
	BindObjectGuid = BindObject->GetUniqueID();
}
void FArmyTextLabel::GetObjTag(FObjectPtr& OutObj, int32& OutTag)
{
	OutObj = BindObject;
	OutTag = ObjTag;
}
void FArmyTextLabel::OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	LabelContent = InText;
	if (TextInputBox.IsValid())TextInputBox->SetVisibility(EVisibility::Collapsed);
	IsIputText = false;
	RebuildDrawLines();
}

void FArmyTextLabel::UpdateInputBox(UArmyEditorViewportClient* InViewPortClient)
{
	if (TextInputBox.IsValid())
	{
		FVector2D PixPos;
		FVector2D ViewportSize;
		InViewPortClient->GetViewportSize(ViewportSize);
		TSharedPtr<FArmyLine> Line2 = DrawLines.FindRef(2);
		InViewPortClient->WorldToPixel(Line2->GetEnd(), PixPos);
		PixPos -= ViewportSize / 2;
		PixPos += FVector2D(15, -12);
		FSlateRenderTransform Trans(PixPos);
		if (TextInputBox->GetRenderTransform() != Trans)
		{
			TextInputBox->SetRenderTransform(Trans);
		}
	}
}

void FArmyTextLabel::RebuildDrawLines()
{
	switch (CurrentLabelType)
	{
	case LT_None:
	case LT_SpaceName:
	case LT_SpaceArea:
	case LT_SpacePerimeter:
	case LT_Switch:
	case LT_Socket:
	case LT_WaterPoint:
	case LT_Beam:
		TextWorldPosition = WorldPosition;
		break;
	case LT_SpaceHeight:
	case LT_CustomHeight:
	case LT_BaseGroundHeight:
	case LT_PostGroundHeight:
	{
		TSharedPtr<FArmyLine> Line1 = DrawLines.FindRef(1);
		TSharedPtr<FArmyLine> Line2 = DrawLines.FindRef(2);
		TSharedPtr<FArmyLine> Line3 = DrawLines.FindRef(3);
		if (!Line1.IsValid())
		{
			Line1 = MakeShareable(new FArmyLine);
			DrawLines.Add(1, Line1);
		}
		if (!Line2.IsValid())
		{
			Line2 = MakeShareable(new FArmyLine);
			DrawLines.Add(2, Line2);
		}
		if (!Line3.IsValid())
		{
			Line3 = MakeShareable(new FArmyLine);
			DrawLines.Add(3, Line3);
		}
		Line1->SetStart(WorldPosition);
		Line1->SetEnd(WorldPosition + FVector(-8, -8.8, 0));
		Line2->SetStart(WorldPosition);
		Line2->SetEnd(WorldPosition + FVector(8, -8.8, 0));
		Line3->SetStart(WorldPosition + FVector(-8, -8.8, 0));

		FString ContentStr = LabelContent.ToString();

		TArray<FString> TempArr;
		if (!ContentStr.IsEmpty())
		{
			ContentStr.ParseIntoArrayLines(TempArr);
		}
		int32 RowNum = TempArr.Num();
		int32 MaxLen = 0;
		for (auto & S : TempArr)
		{
			FTCHARToUTF8 Converter(*S);
			int32 CharNum = Converter.Length();
			if (CharNum > MaxLen) MaxLen = CharNum;
		}

		Line3->SetEnd(WorldPosition + FVector(18 + (TextSize * MaxLen / 4), -8.8, 0));
		TextWorldPosition = WorldPosition + FVector(5 + (TextSize * MaxLen / 8), -(8.8 + (TextSize + TextSize /4 ) * RowNum), 0);
	}
	break;
	default:
		TextWorldPosition = WorldPosition;
		break;
	}
}
void FArmyTextLabel::DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY) && bDrawLabel)
	{
		if (!IsIputText)
		{
			FScaleCanvasText FillTextItem;
			FillTextItem.SetTextSize(TextSize);
			FillTextItem.SetPosition(TextWorldPosition);
			FillTextItem.Text = LabelContent;
			FillTextItem.bCentreX = true;
			FillTextItem.bCentreY = true;
			FillTextItem.SetColor(TextColor);
			FillTextItem.DrawHUD(InViewPortClient, View, Canvas);

			LabelSize = FVector2D(FillTextItem.GetScaleTextSize() * LabelContent.ToString().Len(), FillTextItem.GetScaleTextSize());


			//switch (CurrentLabelType)
			//{
			//case FArmyTextLabel::LT_None:
			//	//{
			//	//	FCanvasTileTextItem TextIt(PixPos, LabelContent, GEngine->GetLargeFont(), TextColor);
			//	//	TextIt.Scale = FVector2D(1 / Scale, 1 / Scale);
			//	//	TextIt.bCentreX = true;
			//	//	//TextIt.bCentreY = true;
			//	//	TextIt.Draw(Canvas);
			//	//	LabelSize = TextIt.GetSize();
			//	//}
			//	//	break;
			//case FArmyTextLabel::LT_SpaceName:
			//case FArmyTextLabel::LT_SpaceArea:
			//case FArmyTextLabel::LT_SpacePerimeter:
			//case FArmyTextLabel::LT_SpaceHeight:
			//case FArmyTextLabel::LT_Switch:
			//case FArmyTextLabel::LT_Socket:
			//case FArmyTextLabel::LT_WaterPoint:
			//case FArmyTextLabel::LT_CustomHeight:
			//case FArmyTextLabel::LT_Beam:
			//{
			//	FScaleCanvasText FillTextItem;
			//	FillTextItem.SetTextSize(TextSize);
			//	FillTextItem.SetPosition(TextWorldPosition);
			//	FillTextItem.Text = LabelContent;
			//	FillTextItem.bCentreX = true;
			//	FillTextItem.bCentreY = true;
			//	FillTextItem.SetColor(TextColor);
			//	FillTextItem.DrawHUD(InViewPortClient, View, Canvas);

			//	LabelSize = FVector2D(FillTextItem.GetScaleTextSize() * LabelContent.ToString().Len(), FillTextItem.GetScaleTextSize());
			//}
			//break;
			//default:
			//	break;
			//}
		}
		else
		{
			UpdateInputBox(InViewPortClient);
		}
	}
}
void FArmyTextLabel::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (EditTextState)
		{
			EditTextState = false;
			if (TextInputBox.IsValid())
			{
				TextInputBox->SetVisibility(EVisibility::Visible);
				TextInputBox->SetText(LabelContent);
				FSlateApplication::Get().SetKeyboardFocus(TextInputBox);
				TextInputBox->SelectAllText();
				IsIputText = true;
			}
		}

		if (GetState() == OS_Selected && bUseEditPos)
		{
			EditPoint->Draw(PDI, View);
		}
		for (auto L : DrawLines)
		{
			L.Value->Draw(PDI, View);
		}
	}
}
void FArmyTextLabel::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("Type", TEXT("OT_TextLabel"));

	if (CurrentLabelType == LT_None)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_None"));
	}
	else if (CurrentLabelType == LT_SpaceHeight)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_SpaceHeight"));
	}
	else if (CurrentLabelType == LT_SpaceName)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_SpaceName"));
	}
	else if (CurrentLabelType == LT_Beam)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_Beam"));
	}
	else if (CurrentLabelType == LT_Switch)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_Switch"));
	}
	else if (CurrentLabelType == LT_WaterPoint)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_WaterPoint"));
	}
	else if (CurrentLabelType == LT_Socket)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_Socket"));
	}
	else if (CurrentLabelType == LT_SpaceArea)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_SpaceArea"));
	}
	else if (CurrentLabelType == LT_SpacePerimeter)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_SpacePerimeter"));
	}
	else if (CurrentLabelType == LT_CustomHeight)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_CustomHeight"));
	}

	JsonWriter->WriteValue("TextColor", TextColor.ToString());
	JsonWriter->WriteValue("LabelSize", LabelSize.ToString());
	JsonWriter->WriteValue("LabelContent", LabelContent.ToString());
	JsonWriter->WriteValue("TextSize", TextSize);

	if (BindObject.IsValid())
	{
		JsonWriter->WriteValue("BindObject", BindObject->GetUniqueID().ToString());
	}
	JsonWriter->WriteValue("ObjectTag", ObjTag);

	JsonWriter->WriteValue("LabelPos", WorldPosition.ToString());
	JsonWriter->WriteValue("bUseEditPos", bUseEditPos);
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyTextLabel)
}
void FArmyTextLabel::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	FString TempLabelType = InJsonData->GetStringField("LabelType");
	if (TempLabelType == "LT_None")
	{
		CurrentLabelType = LT_None;
	}
	else if (TempLabelType == "LT_SpaceHeight")
	{
		CurrentLabelType = LT_SpaceHeight;
	}
	else if (TempLabelType == "LT_SpaceName")
	{
		CurrentLabelType = LT_SpaceName;
	}
	else if (TempLabelType == "LT_Beam")
	{
		CurrentLabelType = LT_Beam;
	}
	else if (TempLabelType == "LT_Switch")
	{
		CurrentLabelType = LT_Switch;
	}
	else if (TempLabelType == "LT_Socket")
	{
		CurrentLabelType = LT_Socket;
	}
	else if (TempLabelType == "LT_WaterPoint")
	{
		CurrentLabelType = LT_WaterPoint;
	}
	else if (TempLabelType == "LT_SpaceArea")
	{
		CurrentLabelType = LT_SpaceArea;
	}
	else if (TempLabelType == "LT_SpacePerimeter")
	{
		CurrentLabelType = LT_SpacePerimeter;
	}
	else if (TempLabelType == "LT_CustomHeight")
	{
		CurrentLabelType = LT_CustomHeight;
	}

	TextColor.InitFromString(InJsonData->GetStringField("TextColor"));
	LabelSize.InitFromString(InJsonData->GetStringField("LabelSize"));
	LabelContent = FText::FromString(InJsonData->GetStringField("LabelContent"));

	FString ObjID = InJsonData->GetStringField("BindObject"); 
	TextSize = InJsonData->GetIntegerField("TextSize");
	FGuid::Parse(ObjID, BindObjectGuid);

	ObjTag = InJsonData->GetIntegerField("ObjectTag");
	bUseEditPos = InJsonData->GetBoolField("bUseEditPos");
	WorldPosition.InitFromString(InJsonData->GetStringField("LabelPos"));
	SetWorldPosition(WorldPosition);
}
void FArmyTextLabel::SetState(EObjectState InState)
{
	State = InState;

	switch (InState)
	{
	case OS_Normal:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		TextColor = FLinearColor::White;
		break;

	case OS_Hovered:
	case OS_Selected:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		TextColor = FLinearColor(FColor(0xFFFF9800));
		break;

	case OS_Disable:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		TextColor = FLinearColor::White;
		break;

	default:
		break;
	}
	for (auto & L : DrawLines)
	{
		L.Value->SetLineColor(TextColor);
	}
}
bool FArmyTextLabel::IsSelected(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		FVector2D PosPix, WorldPix ,LeftWorldPix, RightWorldPix;
		InViewportClient->WorldToPixel(Pos, PosPix);
		//InViewportClient->WorldToPixel(WorldPosition, WorldPix);
		InViewportClient->WorldToPixel(WorldPosition - FVector(LabelSize / 2, 0), LeftWorldPix);
		InViewportClient->WorldToPixel(WorldPosition + FVector(LabelSize / 2, 0), RightWorldPix);

		FBox2D Box(ForceInitToZero);
		for (auto & L : DrawLines)
		{
			FVector2D P1, P2;
			InViewportClient->WorldToPixel(L.Value->GetStart(), P1);
			InViewportClient->WorldToPixel(L.Value->GetEnd(), P2);

			Box += (P1);
			Box += (P2);
		}
		Box += FBox2D(LeftWorldPix, RightWorldPix);
		bool bInSelect = Box.IsInside(PosPix);
		/*if (bInSelect)
		{
			GVC->CurCursor = EMouseCursor::CardinalCross;
		}
		else
		{
			GVC->CurCursor = EMouseCursor::Default;
		}*/
		return bInSelect;
	}
	return  false;
}

bool FArmyTextLabel::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return IsSelected(Pos, InViewportClient);
	}
	return  false;
}
TSharedPtr<FArmyEditPoint> FArmyTextLabel::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (bUseEditPos && EditPoint->IsSelected(Pos, InViewportClient))
			{
				return EditPoint;
			}
			else if(IsSelected(Pos, InViewportClient))
			{
				return EditPoint;
			}
		}
	}
	return NULL;
}
TSharedPtr<FArmyEditPoint> FArmyTextLabel::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	return SelectPoint(Pos, InViewportClient);
}
void FArmyTextLabel::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{

}
void FArmyTextLabel::Refresh()
{
	if (GetState() == OS_Selected)
	{
		WorldPosition = EditPoint->GetPos();
		RebuildDrawLines();
	}
}
const FBox FArmyTextLabel::GetBounds()
{
	FVector2D Scaler(0.25f, 0.5f);
	return FBox(WorldPosition - FVector(LabelSize*Scaler, 0), WorldPosition + FVector(LabelSize*Scaler, 0));
}
void FArmyTextLabel::ApplyTransform(const FTransform& Trans)
{
	SetWorldPosition(Trans.TransformPosition(WorldPosition));
}
void FArmyTextLabel::StartEditText()
{
	if (TextInputBox.IsValid())
	{
		EditTextState = true;
		TextInputBox->SetVisibility(EVisibility::Visible);
		IsIputText = true;
	}
}
void FArmyTextLabel::SetWorldPosition(const FVector& InPos)
{
	WorldPosition = InPos;
	EditPoint->SetPos(WorldPosition);
	RebuildDrawLines();
}