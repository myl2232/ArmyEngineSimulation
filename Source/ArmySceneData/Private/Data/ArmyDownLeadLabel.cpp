#include "ArmyDownLeadLabel.h"
#include "SSArmyEditableLabel.h"
#include "CanvasItem.h"
#include "ArmyEditPoint.h"
#include "ArmySettings.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyGameInstance.h"
#include "SArmyDynamicFontSize.h"
#include "SlateApplication.h"
#include "ArmyPrimitive.h"

FArmyDownLeadLabel::FArmyDownLeadLabel()
{
    SetName(TEXT("���߱�ע") + GetUniqueID().ToString());
	ObjectType = OT_DownLeadLabel;
	CurrentLabelType = LT_None;
	EditPoint = MakeShareable(new FArmyEditPoint());
	LeadEditPoint = MakeShareable(new FArmyEditPoint());
	EndEditPoint = MakeShareable(new FArmyEditPoint());
	FillTextItem.Text = FText::FromString(TEXT(""));
}

FArmyDownLeadLabel::~FArmyDownLeadLabel()
{
	if (InputBoxParent.IsValid() && InPutPanel.IsValid())
	{
		InputBoxParent->RemoveSlot(InPutPanel.ToSharedRef());
	}
}
void FArmyDownLeadLabel::Init(TSharedPtr<SOverlay> ParentWidget)
{
	InputBoxParent = ParentWidget;

	SAssignNew(InPutPanel,SVerticalBox)
		.Visibility(EVisibility::Collapsed)
		+SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.AutoHeight()
		[
			SAssignNew(DynamicWidget,SOverlay)
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.AutoHeight()
		.Padding(0, 4)
		[
			SNew(SBox)
			.MinDesiredWidth(40.0f)
			.MinDesiredHeight(20.0f)
			.MaxDesiredWidth(200.0f)
			.MaxDesiredHeight(120.0f)
			[
				SNew(SBorder)
				.BorderBackgroundColor(FLinearColor(FColor(0XFFCCCCCC)))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.Padding(FMargin(1.0f))
				[
					SAssignNew(TextInputBox, SMultiLineEditableTextBox)
					.Style(&FArmyStyle::GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FFFFFFFF"))
					.TextStyle(&FArmyStyle::GetWidgetStyle<FTextBlockStyle>("ArmyText_10_DownLead"))
					.OnTextCommitted(this, &FArmyDownLeadLabel::OnTextCommitted)
				]
			]
		];

	InputBoxParent->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			InPutPanel.ToSharedRef()
		];

	//TextInputBox->OnInputBoxCommitted = FOnInPutValueCommited::CreateRaw();
}
void FArmyDownLeadLabel::SetLabelType(LabelType InType)
{ 
	CurrentLabelType = InType;
}
void FArmyDownLeadLabel::SetObjTag(FObjectPtr InObj, int32 InTag)
{
	BindObject = InObj;
	ObjTag = InTag;
}
void FArmyDownLeadLabel::GetObjTag(FObjectPtr& OutObj, int32& OutTag)
{
	OutObj = BindObject;
	OutTag = ObjTag;
}
void FArmyDownLeadLabel::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);

	JsonWriter->WriteValue("Type", TEXT("OT_DownLeadLabel"));

	if (CurrentLabelType == LT_None)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_None"));
	}
	else if (CurrentLabelType == LT_Size)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_Size"));
	}
	else if (CurrentLabelType == LT_Material)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_Material"));
	}
	else if (CurrentLabelType == LT_Technology)
	{
		JsonWriter->WriteValue("LabelType", TEXT("LT_Technology"));
	}

	JsonWriter->WriteValue("TextColor", TextColor.ToString());
	JsonWriter->WriteValue("TextFontSize", TextFontSize);
	JsonWriter->WriteValue("LabelSize", LabelSize.ToString());
	JsonWriter->WriteValue("LabelContent", LabelContent.ToString());

	if(BindObject.IsValid())JsonWriter->WriteValue("BindObject", BindObject->GetUniqueID().ToString());
	JsonWriter->WriteValue("ObjectTag", ObjTag);

	JsonWriter->WriteValue("Position", WorldPosition.ToString());
	JsonWriter->WriteValue("LeadPoint", LeadEditPoint->GetPos().ToString());
	JsonWriter->WriteValue("EndPoint", EndEditPoint->GetPos().ToString());
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyDownLeadLabel)
}
void FArmyDownLeadLabel::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);

	CurrentLabelType = LabelType(InJsonData->GetIntegerField("LabelType"));
	TextColor.InitFromString(InJsonData->GetStringField("TextColor"));
	LabelSize.InitFromString(InJsonData->GetStringField("LabelSize"));
	InJsonData->TryGetNumberField("TextFontSize", TextFontSize);
	LabelContent = FText::FromString(InJsonData->GetStringField("LabelContent"));

	ObjTag = InJsonData->GetIntegerField("ObjectTag");
	FVector TempPos;
	TempPos.InitFromString(InJsonData->GetStringField("Position"));
	SetWorldPosition(TempPos);

	TempPos.InitFromString(InJsonData->GetStringField("LeadPoint"));
	SetLeadPoint(TempPos);

	TempPos.InitFromString(InJsonData->GetStringField("EndPoint"));
	SetEndPoint(TempPos);

	
	FString ContentText = LabelContent.ToString();
	TArray<FString> TempArr;
	if (!ContentText.IsEmpty())
	{
		ContentText.ParseIntoArray(TempArr, TEXT("\n"), true);
	}
	int32 RowNum = TempArr.Num();
	int32 MaxLen = 0;
	for (auto & S : TempArr)
	{
		S.Len() > MaxLen ? MaxLen = S.Len() : false;
	}
	FVector LineOffset = MainLine->GetEnd() - MainLine->GetStart();
	FVector FontDir(0, -1, 0);
	FRotator Rot(ForceInitToZero);
	FVector LineDir = LineOffset.GetSafeNormal();
	if (FMath::Abs(LineDir.X) < FMath::Abs(LineDir.Y))
	{
			FVector LeadLineOffset = LeadLine->GetEnd() - LeadLine->GetStart();
			FontDir = (LeadLineOffset.GetSafeNormal() * FVector(1, 0, 0));
			FontDir.Normalize();
			Rot = FRotator(FQuat::FindBetweenVectors(FVector(0, 1, 0), FontDir));
	}

	FVector MainDirOffset(ForceInitToZero);
	if (LineDir.X > 0 || LineDir.Y > 0)
	{
		int32 TexLen = TextFontSize * MaxLen - TextFontSize;
		MainDirOffset = LineDir * -TexLen;
	}
	//世界坐标和字体旋转
	TextWorldPosition = MainLine->GetEnd() + MainDirOffset + FontDir * ((TextFontSize + 5)*RowNum); 
	TextDirection = FVector(-FontDir.Y, FontDir.X, FontDir.Z);
	if (FVector::CrossProduct(FontDir, TextDirection).Z < 0)
		TextDirection = -TextDirection;
}
void FArmyDownLeadLabel::OnTextCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	LabelContent = InText;
	
	if (FillTextItem.Text.EqualTo(FText::FromString(TEXT(""))))
	{
		SCOPE_TRANSACTION(TEXT("添加标注文字"));
		this->Create();
	}
	else if (FillTextItem.Text.EqualTo(InText))
	{
	}
	else
	{
		SCOPE_TRANSACTION(TEXT("修改标注文字"));
		this->Modify();
	}
	EndEditText();
}
void FArmyDownLeadLabel::UpdateInputBox(UArmyEditorViewportClient* InViewPortClient)
{
	if (InPutPanel.IsValid())
	{
		FVector2D ViewportSize;
		InViewPortClient->GetViewportSize(ViewportSize);
		InViewPortClient->WorldToPixel(MainLine->GetEnd(), PixPos);
		PixPos -= ViewportSize / 2;

		PixPos.Y -= InPutPanel->GetDesiredSize().Y;
		FSlateRenderTransform Trans(PixPos);
		if (InPutPanel->GetRenderTransform() != Trans)
		{
			InPutPanel->SetRenderTransform(Trans);
		}
	}
}
void FArmyDownLeadLabel::DrawHUD(UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (IsIputText)
		{
			UpdateInputBox(InViewPortClient);
		}
		else
		{
			if (MainLine.IsValid() && MainLine->GetEnd() != MainLine->GetStart())
			{
				FString ContentText = LabelContent.ToString();
				
				TArray<FString> TempArr;
				if (!ContentText.IsEmpty())
				{
					ContentText.ParseIntoArray(TempArr, TEXT("\n"), true);
				}
				int32 RowNum = TempArr.Num();
				int32 MaxLen = 0;
				for (auto & S : TempArr)
				{
					S.Len() > MaxLen ? MaxLen = S.Len() : false;
				}


				FVector LineOffset = MainLine->GetEnd() - MainLine->GetStart();
				
				FVector FontDir(0, -1, 0);
				FRotator Rot(ForceInitToZero);
				FVector LineDir = LineOffset.GetSafeNormal();
				if (FMath::Abs(LineDir.X) < FMath::Abs(LineDir.Y))
				{
					FVector LeadLineOffset = LeadLine->GetEnd() - LeadLine->GetStart();
					FontDir = (LeadLineOffset.GetSafeNormal() * FVector(1,0,0));
					FontDir.Normalize();
					Rot = FRotator(FQuat::FindBetweenVectors(FVector(0, 1, 0), FontDir));
				}

				FVector MainDirOffset(ForceInitToZero);
				if (LineDir.X > 0 || LineDir.Y > 0)
				{
					int32 TexLen = TextFontSize * MaxLen - TextFontSize;
					MainDirOffset = LineDir * -TexLen;
				}
				//世界坐标和字体旋转
				TextWorldPosition = MainLine->GetEnd() + MainDirOffset + FontDir * ((TextFontSize + 5)*RowNum);
				TextDirection = FVector(-FontDir.Y, FontDir.X, FontDir.Z);
				if (FVector::CrossProduct(FontDir, TextDirection).Z < 0)
					TextDirection = -TextDirection;



				FillTextItem.SetTextSize(TextFontSize);
				FillTextItem.SetPosition(MainLine->GetEnd() + MainDirOffset + FontDir * ((TextFontSize + 5)*RowNum));
				FillTextItem.SetRotator(Rot);
				FillTextItem.Text = LabelContent;
				FillTextItem.SetColor(TextColor);
				FillTextItem.DrawHUD(InViewPortClient, View, Canvas);
				
			}
		}
	}
}
void FArmyDownLeadLabel::Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (EditTextState)
		{
			EditTextState = false;
			if (InPutPanel.IsValid())
			{
				InPutPanel->SetVisibility(EVisibility::Visible);
				TextInputBox->SetText(LabelContent);
				FSlateApplication::Get().SetKeyboardFocus(TextInputBox);
				TextInputBox->SelectAllText();
			
				IsIputText = true;
			}
		}
		if (GetState() == OS_Selected)
		{
			//EditPoint->Draw(PDI, View);
			LeadEditPoint->Draw(PDI, View);
			EndEditPoint->Draw(PDI, View);
		}
		//TextWorldPosition = WorldPosition + FVector((LabelContent.ToString().Len() * 10) / 2, -34, 0);
		if (LeadLine.IsValid())
		{
			DrawDisc(PDI, WorldPosition, FVector(1, 0, 0), FVector(0, 1, 0), FColor(255, 153, 153, 153), 2, 16, UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), SDPG_World);
			LeadLine->Draw(PDI, View);
		}
		if (MainLine.IsValid())
		{
			MainLine->Draw(PDI, View);
		}
	}
}
void FArmyDownLeadLabel::SetState(EObjectState InState)
{
	State = InState;
	FLinearColor LinearColor=FLinearColor::White;

	switch (InState)
	{
	case OS_Normal:
		EndEditText();
	case OS_Disable:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		LinearColor = FLinearColor(FColor(0xFF999999));
		break;

	case OS_Hovered:
	case OS_Selected:
		EditPoint->SetState(FArmyEditPoint::OPS_Normal);
		LinearColor = FLinearColor(FColor(0xFFFF9800));
		break;

	default:
		break;
	}
	if (EditPoint.IsValid())
	{
		EditPoint->SetNormalColor(LinearColor);
	}
	if (LeadLine.IsValid())
	{
		LeadLine->SetLineColor(LinearColor);
	}
	if (MainLine.IsValid())
	{
		MainLine->SetLineColor(LinearColor);
	}
	TextColor = LinearColor;

}
bool FArmyDownLeadLabel::IsSelected(const FVector& Pos,UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY) && MainLine.IsValid())
	{
		bool Picked = MainLine->IsSelected(Pos, InViewportClient) || LeadLine->IsSelected(Pos,InViewportClient);

		FVector LineOffset = MainLine->GetEnd() - MainLine->GetStart();

		FVector FontDir(0, -1, 0);
		FRotator Rot(ForceInitToZero);
		FVector LineDir = LineOffset.GetSafeNormal();
		if (FMath::Abs(LineDir.X) < FMath::Abs(LineDir.Y))
		{
			FVector LeadLineOffset = LeadLine->GetEnd() - LeadLine->GetStart();
			FontDir = (LeadLineOffset.GetSafeNormal() * FVector(1, 0, 0));
			FontDir.Normalize();
			Rot = FRotator(FQuat::FindBetweenVectors(FVector(0, 1, 0), FontDir));
		}
		FBox2D TempBox(ForceInitToZero);
		
		TempBox += FVector2D(MainLine->GetEnd() + FontDir * 20 - LineDir * 100);
		TempBox += FVector2D(MainLine->GetEnd() + FontDir * 20 + LineDir * 100);
		TempBox += FVector2D(MainLine->GetEnd() + LineDir * 100);
		TempBox += FVector2D(MainLine->GetEnd() - LineDir * 100);
		bool PickText = TempBox.IsInside(FVector2D(Pos));
		//if (PickText && GetState() == OS_Selected)
		//{
		//	StartEditText();
		//}
		return Picked || PickText;
	}
	return  false;
}

bool FArmyDownLeadLabel::Hover(const FVector& Pos, UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		return MainLine->IsSelected(Pos, InViewportClient) || LeadLine->IsSelected(Pos, InViewportClient);
	}
	return  false;
}
TSharedPtr<FArmyEditPoint> FArmyDownLeadLabel::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			/*			if (EditPoint->IsSelected(Pos, InViewportClient))
							return EditPoint;
						else */
			if (LeadEditPoint->IsSelected(Pos, InViewportClient))
				return LeadEditPoint;
			else if (EndEditPoint->IsSelected(Pos, InViewportClient))
				return EndEditPoint;
		}
	}
	return NULL;
}
TSharedPtr<FArmyEditPoint> FArmyDownLeadLabel::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (EditPoint->IsSelected(Pos, InViewportClient))
				return EditPoint;
			else if (LeadEditPoint->IsSelected(Pos, InViewportClient))
				return LeadEditPoint;
			else if(EndEditPoint->IsSelected(Pos, InViewportClient))
				return EndEditPoint;
		}
	}
	return NULL;
}
void FArmyDownLeadLabel::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	OutLines.Add(LeadLine);
	OutLines.Add(MainLine);
}
void FArmyDownLeadLabel::Refresh()
{
	if (GetState() == OS_Selected)
	{
		SetWorldPosition(EditPoint->GetPos());
		SetLeadPoint(LeadEditPoint->GetPos());
		SetEndPoint(EndEditPoint->GetPos());
	}
}
const FBox FArmyDownLeadLabel::GetBounds()
{
	return FBox(WorldPosition - FVector(LabelSize, 0), WorldPosition + FVector(LabelSize, 0));
}
void FArmyDownLeadLabel::Destroy()
{
	if (InputBoxParent.IsValid() && InPutPanel.IsValid())
	{
		InputBoxParent->RemoveSlot(InPutPanel.ToSharedRef());
	}
}

void FArmyDownLeadLabel::ApplyTransform(const FTransform& Trans)
{
	SetWorldPosition(Trans.TransformPosition(WorldPosition));
}

void FArmyDownLeadLabel::AttachDynamicWidget(TSharedPtr<SWidget> InWidget)
{
	if (DynamicWidget.IsValid())
	{
		DynamicWidget->ClearChildren();
		DynamicWidget->AddSlot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				InWidget.ToSharedRef()
			];
	}
}
void FArmyDownLeadLabel::StartEditText()
{
	EditTextState = true;
}
void FArmyDownLeadLabel::EndEditText()
{
	if (DynamicWidget.IsValid())
	{
		DynamicWidget->ClearChildren();
		if (InPutPanel.IsValid())InPutPanel->SetVisibility(EVisibility::Collapsed);
		IsIputText = false;
	}
	EditTextState = false;
}
void FArmyDownLeadLabel::SetTextSize(int32 InValue)
{
	if (InValue > 0)
	{
		TextFontSize = InValue;
	}
}
int32 FArmyDownLeadLabel::GetTextSize()
{
	return TextFontSize;
}
void FArmyDownLeadLabel::SetLeadPoint(const FVector& InPos)
{
	if (LeadLine.IsValid())
	{
		if (LeadLine->GetEnd() == InPos)
		{
			return;
		}
		LeadLine->SetStart(WorldPosition);
		LeadLine->SetEnd(InPos);
		LeadEditPoint->SetPos(InPos);
		if (MainLine.IsValid())
		{
			FVector LineOffSet = MainLine->GetEnd() - MainLine->GetStart();
			MainLine->SetStart(InPos);
			MainLine->SetEnd(InPos + LineOffSet);
			EndEditPoint->SetPos(MainLine->GetEnd());
		}
		else
		{
			MainLine = MakeShareable(new FArmyLine);
			MainLine->SetStart(InPos);
			MainLine->SetEnd(InPos);
			EndEditPoint->SetPos(InPos);
		}
	}
}
void FArmyDownLeadLabel::SetEndPoint(const FVector& InPos)
{
	if (LeadLine.IsValid() && MainLine.IsValid())
	{
		if (MainLine->GetEnd() == InPos)
		{
			return;
		}
		FVector TempLineOffSet = InPos - MainLine->GetStart();
		if (FMath::Abs(TempLineOffSet.X) > FMath::Abs(TempLineOffSet.Y))
		{
			MainLine->SetEnd(FVector(InPos.X,MainLine->GetStart().Y,0));
		}
		else
		{
			MainLine->SetEnd(FVector(MainLine->GetStart().X, InPos.Y, 0));
		}
		EndEditPoint->SetPos(MainLine->GetEnd());
	}
}
void FArmyDownLeadLabel::SetWorldPosition(const FVector& InPos)
{
	if (WorldPosition == InPos) return;
	WorldPosition = InPos;
	EditPoint->SetPos(WorldPosition);
	if (LeadLine.IsValid())
	{
		FVector LineOffset = LeadLine->GetEnd() - LeadLine->GetStart();
		FVector LineEnd = WorldPosition + LineOffset;
		LeadLine->SetStart(WorldPosition);
		LeadLine->SetEnd(LineEnd);
		LeadEditPoint->SetPos(LineEnd);

		if (MainLine.IsValid())
		{
			LineOffset = MainLine->GetEnd() - MainLine->GetStart();
			LineEnd = LineEnd + LineOffset;
			MainLine->SetStart(LeadLine->GetEnd());
			MainLine->SetEnd(LineEnd);
			EndEditPoint->SetPos(LineEnd);
		}
	}
	else
	{
		LeadLine = MakeShareable(new FArmyLine);
		LeadLine->SetStart(WorldPosition);
		LeadLine->SetEnd(WorldPosition);
		LeadEditPoint->SetPos(WorldPosition);
	}
}