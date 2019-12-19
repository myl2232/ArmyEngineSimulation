#include "ArmyConstructionFrame.h" 
#include "ArmyEditorViewportClient.h"

FArmyConstructionFrame::FArmyConstructionFrame(bool IsEditModel):EditState(IsEditModel)
{
	ObjectType = OT_ConstructionFrame;
	FrameType = E_Frame_Default;
}
FArmyConstructionFrame::~FArmyConstructionFrame() 
{

}
void FArmyConstructionFrame::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("Type", TEXT("OT_ConstructionFrame"));

	// 图框类型
	JsonWriter->WriteValue("FrameType", (int32)FrameType);

	JsonWriter->WriteValue("ApplyABSTransform", ApplyABSTransform);
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());

	JsonWriter->WriteArrayStart("FrameLines");
	for (auto L : FrameBaseLines)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("StartPoint", L->GetStart().ToString());
		JsonWriter->WriteValue("EndPoint", L->GetEnd().ToString());
		JsonWriter->WriteValue("Color", L->GetBaseColor().ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("FillAreaMap");
	for (auto A : FillAreaMap)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("AreaType", A.Key);
		JsonWriter->WriteValue("AreaContent", A.Value.TextContent);
		JsonWriter->WriteValue("AreaFontSize", int32(A.Value.FontSize));
		JsonWriter->WriteValue("AreaPos", A.Value.RectArea->Pos.ToString());
		JsonWriter->WriteValue("AreaWidth", A.Value.RectArea->Width);
		JsonWriter->WriteValue("AreaHeight", A.Value.RectArea->Height);
		JsonWriter->WriteValue("AreaColor", A.Value.RectArea->GetBaseColor().ToString());

		if (A.Value.DXFData.IsValid())
		{
			JsonWriter->WriteObjectStart("DXFData");
			A.Value.DXFData->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyConstructionFrame)
}
void FArmyConstructionFrame::Deserialization(const TSharedPtr<FJsonObject>& InJsonData) 
{
	bool PreState = EditState;
	EditState = true;
	FArmyObject::Deserialization(InJsonData);

	if (InJsonData->GetStringField("Type") == TEXT("OT_ConstructionFrame"))
	{
		ObjectType = OT_ConstructionFrame;
	}

	// 图框类型
	int32 TmpFrameType = 0;
	InJsonData->TryGetNumberField("FrameType", TmpFrameType);
	FrameType = (EConstructionFrameType)TmpFrameType;

	InJsonData->TryGetBoolField("ApplyABSTransform", ApplyABSTransform);
	LocalTransform.InitFromString(InJsonData->GetStringField("LocalTransform"));

	const TArray< TSharedPtr<FJsonValue> >& FrameLines = InJsonData->GetArrayField("FrameLines");
	for (auto JsonV : FrameLines)
	{
		const TSharedPtr<FJsonObject>& JsonObj = JsonV->AsObject();
		if (JsonObj.IsValid())
		{
			TSharedPtr<FArmyLine> L = MakeShareable(new FArmyLine);
			FVector StartPoint,EndPoint; 
			StartPoint.InitFromString(JsonObj->GetStringField("StartPoint"));
			EndPoint.InitFromString(JsonObj->GetStringField("EndPoint"));
			L->SetStart(StartPoint);
			L->SetEnd(EndPoint);
			FLinearColor BaseColor;
			BaseColor.InitFromString(JsonObj->GetStringField("Color"));
			L->SetBaseColor(BaseColor);
			FrameBaseLines.Add(L);
		}
	}

	const TArray< TSharedPtr<FJsonValue> >& JsonAreaMap = InJsonData->GetArrayField("FillAreaMap");
	for (auto JsonV : JsonAreaMap)
	{
		const TSharedPtr<FJsonObject>& JsonObj = JsonV->AsObject();
		if (JsonObj.IsValid())
		{
			TSharedPtr<FArmyRect> R = MakeShareable(new FArmyRect);
			
			int32 AreaType = JsonObj->GetIntegerField("AreaType");
			FString AreaContent = JsonObj->GetStringField("AreaContent");
			int32 AreaFontSize = JsonObj->GetIntegerField("AreaFontSize");

			R->Pos.InitFromString(JsonObj->GetStringField("AreaPos"));
			FLinearColor BaseColor;
			BaseColor.InitFromString(JsonObj->GetStringField("AreaColor"));
			R->SetBaseColor(BaseColor);
			R->Width = JsonObj->GetNumberField("AreaWidth");
			R->Height = JsonObj->GetNumberField("AreaHeight");
			AddFillArea(AreaType, R, AreaFontSize);
			FillAreaText(AreaType, AreaContent);

			const TSharedPtr<FJsonObject> * DXFJsonData = NULL;
			if (JsonObj->TryGetObjectField("DXFData", DXFJsonData))
			{
				TSharedPtr<FArmyFurniture> DxfObj = MakeShareable(new FArmyFurniture);
				DxfObj->Deserialization(*DXFJsonData);
				FillAreaDxf(AreaType, DxfObj);
			}
		}
	}
	BuildOperationPoint();
	EditState = PreState;
}
void FArmyConstructionFrame::SetState(EObjectState InState) 
{
	State = InState;
	MoveOperationPoint->SetState(FArmyEditPoint::OPS_Normal);
	ScaleOperationPointLeftTop->SetState(FArmyEditPoint::OPS_Normal);
	ScaleOperationPointLeftBottom->SetState(FArmyEditPoint::OPS_Normal);
	ScaleOperationPointRightTop->SetState(FArmyEditPoint::OPS_Normal);
	ScaleOperationPointRightBottom->SetState(FArmyEditPoint::OPS_Normal);

	FLinearColor LinearColor = FLinearColor::White;

	int32 ColorState = 0;
	switch (InState)
	{
	case OS_Normal:
	case OS_Disable:
		LinearColor = FLinearColor(FColor(0xFF999999));
		ColorState = 0;
		break;

	case OS_Hovered:
	case OS_Selected:
		LinearColor = FLinearColor(FColor(0xFFFF9800));
		ColorState = 1;
		break;

	default:
		break;
	}

	for (auto L : FrameBaseLines)
	{
		L->SetLineColor(LinearColor);
	}
	for (auto& A : FillAreaMap)
	{
		if (A.Value.DXFData.IsValid())
		{
			A.Value.DXFData->SetState(State);
		}
		A.Value.CurrentTextColor = ColorState == 1 ? LinearColor : A.Value.BaseTextColor;
	}
}
void FArmyConstructionFrame::Refresh()
{

}
void FArmyConstructionFrame::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) 
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (EditState)
		{
			for (auto L : FrameBaseLines)
			{
				L->Draw(PDI, View);
			}
			for (auto& A : FillAreaMap)
			{
				A.Value.RectArea->Draw(PDI, View);
			}
		}
		else
		{
			for (auto& A : FillAreaMap)
			{
				if (A.Value.DXFData.IsValid())
				{
					A.Value.DXFData->Draw(PDI, View);
				}
			}
			for (auto L : FrameBaseLines)
			{
				PDI->DrawLine(LocalTransform.TransformPosition(L->GetStart()), LocalTransform.TransformPosition(L->GetEnd()), L->GetBaseColor(), SDPG_Foreground, L->Thickness, 0.0f, true);
			}
		}
		if (State == OS_Selected)
		{
			MoveOperationPoint->Draw(PDI, View);
			ScaleOperationPointLeftTop->Draw(PDI, View);
			ScaleOperationPointLeftBottom->Draw(PDI, View);
			ScaleOperationPointRightTop->Draw(PDI, View);
			ScaleOperationPointRightBottom->Draw(PDI, View);
		}
	}
}
void FArmyConstructionFrame::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) 
{
	//if (!EditState)
	//{
		for (auto& A : FillAreaMap)
		{
			FString ContentText = A.Value.TextContent;

			TArray<FString> TempArr;
			if (!ContentText.IsEmpty())
			{
				ContentText.ParseIntoArray(TempArr, TEXT("\n"), true); 
			}
			for (int i = 0; i < TempArr.Num(); i++)
			{
				int32 CharNum = TempArr[i].Len();
				int32 CulCharNum = FMath::FloorToInt(A.Value.RectArea->Width * 0.5 / A.Value.FontSize) * 2;
				int32 RowNum = FMath::CeilToInt(float(CharNum) / CulCharNum);

				for (int32 Index = 1; Index < RowNum; ++Index)
				{
					int32 PreIndex = FMath::FloorToInt((Index - 1) * CulCharNum);
					int32 LastIndex = -1;
					TempArr[i].Mid(PreIndex, CulCharNum).FindLastChar(' ', LastIndex);
					if (LastIndex >= 0)
					{
						TempArr[i][LastIndex + PreIndex] = '\n';
					}
					else
					{
						TempArr[i].InsertAt(FMath::FloorToInt(Index * CulCharNum), TEXT("\n"));
					}
				}
			}
			FString LastContentText = "";
			for (int i = 0; i < TempArr.Num(); i++)
			{
				if(i < (TempArr.Num() - 1))
					LastContentText.Append(TempArr[i] + TEXT("\n"));
				else
				{
					LastContentText.Append(TempArr[i]);
				}
			}
			FBox RectBox(A.Value.RectArea->GetVertices());
			FVector TextPos(RectBox.Min);
			TextPos = LocalTransform.TransformPosition(TextPos);
			FScaleCanvasText FillTextItem;
			FillTextItem.BaseScale = FVector2D(LocalTransform.GetScale3D());
			FillTextItem.SetTextSize(A.Value.FontSize);
			FillTextItem.SetRotator(LocalTransform.Rotator());
			FillTextItem.SetPosition(TextPos);
			FillTextItem.Text = FText::FromString(LastContentText);
			FillTextItem.SetColor(A.Value.CurrentTextColor);
			if (A.Key != FrameFillType::FRAME_AREA_COMPANYNAME)
			{
				FillTextItem.DrawHUD(InViewPortClient, View, Canvas);
			}
		}
	//}
}
bool FArmyConstructionFrame::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	if (EditState)
	{
		for (auto L : FrameBaseLines)
		{
			if (L->IsSelected(Pos, InViewportClient))
			{
				return true;
			}
		}
	}
	else
	{
		for (auto L : FrameBaseLines)
		{
			if (L->IsSelected(LocalTransform.InverseTransformPosition(Pos), InViewportClient))
			{
				return true;
			}
		}
	}

	return false; 
}
TSharedPtr<FArmyEditPoint> FArmyConstructionFrame::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	TSharedPtr<FArmyEditPoint> ResultPoint = NULL;
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (MoveOperationPoint->IsSelected(Pos, InViewportClient))        
				ResultPoint = MoveOperationPoint;
			else if (ScaleOperationPointLeftTop->IsSelected(Pos, InViewportClient))
				ResultPoint = ScaleOperationPointLeftTop;
			else if (ScaleOperationPointLeftBottom->IsSelected(Pos, InViewportClient))
				ResultPoint = ScaleOperationPointLeftBottom;
			else if (ScaleOperationPointRightTop->IsSelected(Pos, InViewportClient))
				ResultPoint = ScaleOperationPointRightTop;
			else if (ScaleOperationPointRightBottom->IsSelected(Pos, InViewportClient))     
				ResultPoint = ScaleOperationPointRightBottom;

			if (ResultPoint.IsValid())                  
				ResultPoint->SetState(FArmyEditPoint::OPS_Selected);
		}
	}
	return ResultPoint;
}
TSharedPtr<FArmyEditPoint> FArmyConstructionFrame::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return SelectPoint(Pos,InViewportClient);
}
bool FArmyConstructionFrame::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return false; 
}
void FArmyConstructionFrame::GetVertexes(TArray<FVector>& OutVertexes) 
{

}
void FArmyConstructionFrame::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	if (EditState)
	{
		OutLines.Append(FrameBaseLines);
	}
}
void FArmyConstructionFrame::RemoveLine(TSharedPtr<FArmyLine> InLine)
{
	if (EditState)
	{
		FrameBaseLines.Remove(InLine);
		BuildOperationPoint();
	}
}
void FArmyConstructionFrame::ApplyTransform(const FTransform& Trans) 
{
	if (ApplyABSTransform)
	{
		LocalTransform = Trans;
	}
	else
	{
		LocalTransform.Accumulate(Trans);
	}
	BuildOperationPoint();
}
const FVector FArmyConstructionFrame::GetBasePos()
{
	return LocalTransform.GetLocation();
}
const FBox FArmyConstructionFrame::GetBounds() 
{ 
	FBox TempBox(ForceInitToZero);
	for (auto L : FrameBaseLines)
	{
		TempBox += LocalTransform.TransformPosition(L->GetStart());
		TempBox += LocalTransform.TransformPosition(L->GetEnd());
	}
	return TempBox;
}
void FArmyConstructionFrame::Update()
{
	for (auto It : FillAreaMap)
	{
		FillAreaInfo* Info = FillAreaMap.Find(It.Key);
		if (Info)
		{
			if (Info->DXFData.IsValid())
			{
				FBox RectBox(Info->RectArea->GetVertices());
				Info->DXFData->SetTransform(FTransform(RectBox.Min) * LocalTransform);
			}
		}
	}
}
void FArmyConstructionFrame::AddFrameLine(TSharedPtr<FArmyLine> InLine)
{
	if (EditState)
	{
		FrameBaseLines.Add(InLine);
	}
}

void FArmyConstructionFrame::DeleteFrameLine(TSharedPtr<FArmyLine> InLine)
{
	if (EditState)
	{
		FrameBaseLines.Remove(InLine);
	}
}
void FArmyConstructionFrame::AddFillArea(int32 InType,TSharedPtr<FArmyRect> InRectArea, uint32 InFontSize)
{
	if (EditState)
	{
		FillAreaMap.Add(InType, FillAreaInfo(InRectArea,InFontSize));
	}
}
void FArmyConstructionFrame::FillAreaText(int32 InType, FString InContent)
{
	FillAreaInfo* Info = FillAreaMap.Find(InType);
	if (Info)
	{
		Info->TextContent = InContent;
	}
}

void FArmyConstructionFrame::FillAreaDxf(int32 InType, const TSharedPtr<FArmyFurniture> InDXFData)
{
	FillAreaInfo* Info = FillAreaMap.Find(InType);
	if (Info)
	{
		FBox RectBox(Info->RectArea->GetVertices());
		
		Info->DXFData = StaticCastSharedPtr<FArmyFurniture>(InDXFData->CopySelf());
		Info->DXFData->SetTransform(FTransform(RectBox.Min) * LocalTransform);
	}

	//using namespace std;
	//FString FilePath = FPaths::ProjectContentDir() + "Assets/" + "loveSpaceLogo.dxf";
	//std::string FilePaths(TCHAR_TO_UTF8(*FilePath));
	//dxfFile DxfFile(FilePaths);
	//TSharedPtr<FArmyDxfModel> DxfModel = NULL;
	//if (DxfFile.parseFile())
	//{
	//	DxfModel = MakeShareable(DxfFile.DxfToHomeEditorData());
	//	TArray<TSharedPtr<FArmyFurniture>> TempObjs = DxfModel->GetAllLayers();
	//	if (TempObjs.Num() > 0)
	//	{
	//		TSharedPtr<FArmyFurniture> SelectedComponent = TempObjs[0];
	//		FillAreaDxfMap.Add(InType, StaticCastSharedPtr<FArmyFurniture>(SelectedComponent->CopySelf()));

	//		FillAreaInfo* Info = FillAreaMap.Find(InType);
	//		if (Info)
	//		{
	//			FBox RectBox(Info->RectArea->GetVertices());
	//			FillAreaDxfMap.FindRef(InType)->SetTransform(FTransform(RectBox.Min) * LocalTransform);
	//		}
	//	}
	//}
}

void FArmyConstructionFrame::DeleteFillArea(int32 InType)
{
	if (EditState)
	{
		FillAreaMap.Remove(InType);
	}
}
FBox FArmyConstructionFrame::GetAreaBox(int32 InType) const
{
	const FillAreaInfo* Info = FillAreaMap.Find(InType);
	if (Info)
	{
		return FBox(Info->RectArea->GetVertices());
	}
	return FBox(ForceInitToZero);
}
void FArmyConstructionFrame::SetTranFormModel(bool InABSModel)
{
	ApplyABSTransform = InABSModel;
}
void FArmyConstructionFrame::BuildOperationPoint()
{
	FBox TempBox = GetBounds();
	if (TempBox.IsValid)
	{
		if (MoveOperationPoint.IsValid())
		{
			MoveOperationPoint->SetPos(TempBox.GetCenter());
		}
		else
		{
			MoveOperationPoint = MakeShareable(new FArmyEditPoint);
			MoveOperationPoint->OperationType = XROP_MOVE;
			MoveOperationPoint->SetPos(TempBox.GetCenter());
		}

		if (ScaleOperationPointLeftTop.IsValid())
		{
			ScaleOperationPointLeftTop->SetPos(TempBox.Min);
		}
		else
		{
			ScaleOperationPointLeftTop = MakeShareable(new FArmyEditPoint);
			ScaleOperationPointLeftTop->OperationType = XROP_SCALE;
			ScaleOperationPointLeftTop->SetPos(TempBox.Min);
		}
		if (ScaleOperationPointLeftBottom.IsValid())
		{
			ScaleOperationPointLeftBottom->SetPos(FVector(TempBox.Min.X, TempBox.Max.Y, TempBox.Max.Z));
		}
		else
		{
			ScaleOperationPointLeftBottom = MakeShareable(new FArmyEditPoint);
			ScaleOperationPointLeftBottom->OperationType = XROP_SCALE;
			ScaleOperationPointLeftBottom->SetPos(FVector(TempBox.Min.X, TempBox.Max.Y, TempBox.Max.Z));
		}
		if (ScaleOperationPointRightTop.IsValid())
		{
			ScaleOperationPointRightTop->SetPos(FVector(TempBox.Max.X, TempBox.Min.Y, TempBox.Min.Z));
		}
		else
		{
			ScaleOperationPointRightTop = MakeShareable(new FArmyEditPoint);
			ScaleOperationPointRightTop->OperationType = XROP_SCALE;
			ScaleOperationPointRightTop->SetPos(FVector(TempBox.Max.X, TempBox.Min.Y, TempBox.Min.Z));
		}
		if (ScaleOperationPointRightBottom.IsValid())
		{
			ScaleOperationPointRightBottom->SetPos(TempBox.Max);
		}
		else
		{
			ScaleOperationPointRightBottom = MakeShareable(new FArmyEditPoint);
			ScaleOperationPointRightBottom->OperationType = XROP_SCALE;
			ScaleOperationPointRightBottom->SetPos(TempBox.Max);
		}
		Update();//梁晓菲 更新dxf缩放以及位置
	}
}

// 获得绘制cad的相关数据
// 获得绘制cad的相关数据
void FArmyConstructionFrame::GetFrameBaseLines(TArray<TSharedPtr<FArmyLine>> &OutLines)
{
	OutLines = FrameBaseLines;
}

void FArmyConstructionFrame::GetFillAreasMap(TMap<int32, FillAreaInfo> &OutFillAreasMap)
{
	OutFillAreasMap = FillAreaMap;
}

FTransform FArmyConstructionFrame::GetLocalTransform()
{
	return LocalTransform;
}