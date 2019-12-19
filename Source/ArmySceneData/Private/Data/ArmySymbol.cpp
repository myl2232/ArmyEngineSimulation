#include "ArmySymbol.h" 
#include "ArmyMath.h"
#include "ArmyEditorViewportClient.h"

#define SYMBOLCONST 15

FArmySymbol::FArmySymbol()
{
	ObjectType = OT_Symbol;
	Init();
}
FArmySymbol::~FArmySymbol() 
{

}
void FArmySymbol::Init()
{
	float R = FMath::Sqrt((SYMBOLCONST * SYMBOLCONST) / 2);
	BaseArrowsVertices.Empty();
	BaseArrowsVertices = {
		FVector(-SYMBOLCONST,SYMBOLCONST, 0),
		FVector(0, 0, 0),
		FVector(0, 0, 0),
		FVector(SYMBOLCONST,SYMBOLCONST, 0),
		FVector(SYMBOLCONST,SYMBOLCONST, 0),
		FVector(R,SYMBOLCONST, 0),
		FVector(-R,SYMBOLCONST, 0),
		FVector(-SYMBOLCONST,SYMBOLCONST, 0),
	};
	Update();
}
void FArmySymbol::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	
	//JsonWriter->WriteValue("TextValue",TextValue);
	JsonWriter->WriteArrayStart("TextValueArray");
	for (auto& TextInfo : TextValueArray)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("TextType", TextInfo.TextType);
		JsonWriter->WriteValue("TextValue", TextInfo.TextValue);
		JsonWriter->WriteValue("FontSize", TextInfo.FontSize);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());

	SERIALIZEREGISTERCLASS(JsonWriter, FArmySymbol)
}
void FArmySymbol::Deserialization(const TSharedPtr<FJsonObject>& InJsonData) 
{
	FArmyObject::Deserialization(InJsonData);

	TextValueArray.Empty();

	FString TextValue;
	if (InJsonData->TryGetStringField("TextValue", TextValue))//用来做兼容以前的代码
	{
		TextValueArray.AddUnique(FTextInfo(ETT_DIRTEXT, TextValue, 15));
	}

	const TArray<TSharedPtr<FJsonValue>>* JsonTextArray = nullptr;
	if (InJsonData->TryGetArrayField("TextValueArray", JsonTextArray))
	{
		for (auto JsonText : *JsonTextArray)
		{
			TSharedPtr<FJsonObject> TextJsonObj = JsonText->AsObject();
			if (TextJsonObj.IsValid())
			{
				 ETextType TextType = ETextType(TextJsonObj->GetIntegerField("TextType"));
				 TextValueArray.AddUnique(FTextInfo(TextType, TextJsonObj->GetStringField("TextValue"), TextJsonObj->GetIntegerField("FontSize")));
			}
		}
	}

	LocalTransform.InitFromString(InJsonData->GetStringField("LocalTransform"));
	Update();
}
void FArmySymbol::SetState(EObjectState InState) 
{
	State = InState;
}
void FArmySymbol::Refresh()
{

}
void FArmySymbol::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) 
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (DrawArrows)
		{
			switch (State)
			{
			case OS_Selected:
			{
				TArray<struct FSimpleElementVertex> TempVertexArray = ArrowsVertexArray;
				for (auto& V : TempVertexArray)
				{
					V.Color = FLinearColor(FColor(0xFFFF9800));
				}
				PDI->DrawLines(TempVertexArray, SDPG_Foreground);
				DrawCircle(PDI, CirclePoint, FVector(1, 0, 0), FVector(0, 1, 0), FLinearColor(FColor(0xFFFF9800)), CircleRadius, 32, SDPG_Foreground);
				if (DrawSpliteLine)
				{
					PDI->DrawLine(CirclePoint - FVector(CircleRadius, 0, 0), CirclePoint + FVector(CircleRadius, 0, 0), FLinearColor(FColor(0xFFFF9800)), SDPG_Foreground);
				}
			}
				break;
			default:
			{
				PDI->DrawLines(ArrowsVertexArray, SDPG_Foreground);
				DrawCircle(PDI, CirclePoint, FVector(1, 0, 0), FVector(0, 1, 0), BorderCircleColor, CircleRadius, 32, SDPG_Foreground);
				if (DrawSpliteLine)
				{
					PDI->DrawLine(CirclePoint - FVector(CircleRadius, 0, 0), CirclePoint + FVector(CircleRadius, 0, 0), BorderCircleColor, SDPG_Foreground);
				}
			}
				break;
			}
		}
	}
}
void FArmySymbol::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) 
{
	if (DrawSpliteLine)
	{
		for (auto& Text : TextValueArray)
		{
			FVector2D TextScale(LocalTransform.GetScale3D());
			FVector TextPos = CirclePoint;
			if (Text.TextType == ETT_DIRTEXT)
			{
				FTCHARToUTF8 Converter(*Text.TextValue);
				int32 CharNum = Converter.Length();

				TextPos.X -= (TextScale.X * Text.FontSize * CharNum / 4);
				TextPos.Y -= (TextScale.Y * Text.FontSize + TextScale.Y * Text.FontSize / 8);
			}
			else if (Text.TextType == ETT_OWNERTEXT)
			{
				FTCHARToUTF8 Converter(*Text.TextValue);
				int32 CharNum = Converter.Length();

				TextPos.X -= (TextScale.X * Text.FontSize * CharNum / 4);
				TextPos.Y += (TextScale.Y * Text.FontSize / 8);
			}
			else
			{
				continue;
			}

			FScaleCanvasText FillTextItem;
			FillTextItem.BaseScale = TextScale;
			FillTextItem.SetTextSize(Text.FontSize);
			FillTextItem.SetPosition(TextPos);
			FillTextItem.Text = FText::FromString(Text.TextValue);
			FillTextItem.DrawHUD(InViewPortClient, View, Canvas);
			switch (State)
			{
			case OS_Selected:
				FillTextItem.SetColor(FLinearColor(FColor(0xFFFF9800)));
				break;
			default:
				break;
			}
		}
	}
	else
	{
		for (auto& Text : TextValueArray)
		{
			FVector2D TextScale(LocalTransform.GetScale3D());
			FVector TextPos = CirclePoint;
			if (Text.TextType == ETT_DIRTEXT)
			{
				FTCHARToUTF8 Converter(*Text.TextValue);
				int32 CharNum = Converter.Length();

				TextPos.X -= (TextScale.X * Text.FontSize * CharNum / 4);
				TextPos.Y -= (TextScale.Y * Text.FontSize / 4 + TextScale.Y * Text.FontSize / 8);

				FScaleCanvasText FillTextItem;
				FillTextItem.BaseScale = FVector2D(LocalTransform.GetScale3D());;
				FillTextItem.SetTextSize(Text.FontSize);
				FillTextItem.SetPosition(TextPos);
				FillTextItem.Text = FText::FromString(Text.TextValue);
				FillTextItem.DrawHUD(InViewPortClient, View, Canvas);

				switch (State)
				{
				case OS_Selected:
					FillTextItem.SetColor(FLinearColor(FColor(0xFFFF9800)));
					break;
				default:
					break;
				}
			}
		}
	}
}
bool FArmySymbol::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	if ((CirclePoint - Pos).Size() <= CircleRadius)
	{
		return true;
	}
	TArray<FVector> VArray;
	for (auto& V : ArrowsVertexArray)
	{
		VArray.Add(V.Position);
	}
	return FArmyMath::IsPointInPolygon2D(Pos,VArray);
}
TSharedPtr<FArmyEditPoint> FArmySymbol::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return NULL;
}
TSharedPtr<FArmyEditPoint> FArmySymbol::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return SelectPoint(Pos,InViewportClient);
}
bool FArmySymbol::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return false; 
}
void FArmySymbol::GetVertexes(TArray<FVector>& OutVertexes) 
{

}
void FArmySymbol::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{

}
void FArmySymbol::ApplyTransform(const FTransform& Trans) 
{
	LocalTransform = Trans;
	Update();
}
const FVector FArmySymbol::GetBasePos()
{
	return LocalTransform.GetLocation();;
}
const FBox FArmySymbol::GetBounds() 
{ 
	return FBox(ForceInitToZero);
}
void FArmySymbol::Update()
{
	ArrowsVertexArray.Empty();
	for (auto & V : BaseArrowsVertices)
	{
		ArrowsVertexArray.Add(FSimpleElementVertex(FVector4(LocalTransform.TransformPosition(V)), FVector2D(0, 0), FLinearColor(1, 1, 1), FHitProxyId()));
	}

	CirclePoint = LocalTransform.TransformPosition(FVector(0, SYMBOLCONST, 0));
	CircleRadius = FMath::Sqrt((SYMBOLCONST * SYMBOLCONST) / 2) * LocalTransform.GetScale3D().Y;
}

// 获得绘制CAD的线段列表
void FArmySymbol::GetDrawCadLines(TArray<TPair<FVector, FVector>>& OutLineList)
{
	int32 CountIndex = ArrowsVertexArray.Num();
	for (int32 i = 0; i < CountIndex; i++)
	{
		FVector StartVector = ArrowsVertexArray[i%CountIndex].Position;
		FVector EndVector = ArrowsVertexArray[(i + 1) % CountIndex].Position;
		OutLineList.Emplace(TPair<FVector, FVector>(StartVector, EndVector));
	}
}
// 获得绘制CAD的圆
TSharedPtr<class FArmyCircle> FArmySymbol::GetDrawCadCircle()
{
	TSharedPtr<FArmyCircle> CurrentCircle = MakeShareable(new FArmyCircle);
	CurrentCircle->Position = CirclePoint;
	CurrentCircle->Radius = CircleRadius;

	return CurrentCircle;
}

// 获得绘制的文字
FScaleCanvasText FArmySymbol::GetCanvasText()
{
	FVector2D TextScale(LocalTransform.GetScale3D());
	FVector TextPos = CirclePoint;
	TextPos.X -= (TextScale.X * 10 / 2);
	TextPos.Y -= (TextScale.Y * 15 / 2);
	TextPos.X += 5;
	FScaleCanvasText FillTextItem;
	FillTextItem.BaseScale = TextScale;
	FillTextItem.SetTextSize(15);
	FillTextItem.SetPosition(TextPos);
	int32 Index = TextValueArray.Find(FTextInfo(ETT_DIRTEXT, "", 15));
	if (Index != INDEX_NONE)
	{
		FillTextItem.Text = FText::FromString(TextValueArray[Index].TextValue);
	}
	return FillTextItem;
}