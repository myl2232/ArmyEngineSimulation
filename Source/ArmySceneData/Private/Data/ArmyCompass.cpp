#include "ArmyCompass.h" 
#include "ArmyEditorViewportClient.h"

FArmyCompass::FArmyCompass()
{
	ObjectType = OT_Compass;
	Init();
}
FArmyCompass::~FArmyCompass() 
{

}
void FArmyCompass::Init()
{
	TSharedPtr<FArmyPolygon> DefaultArrows = MakeShareable(new FArmyPolygon);
	TArray<FVector> Vertices = {
		FVector(-33,-93, 0),
		FVector(0, 100, 0),
		FVector(33,-93, 0),
		FVector(0,-50, 0)
	};
	DefaultArrows->SetVertices(Vertices);
	DefaultArrows->MaterialRenderProxy = new FColoredMaterialRenderProxy(UMaterial::GetDefaultMaterial(MD_Surface)->GetRenderProxy(false), FLinearColor(1, 1, 1), FName("SelectionColor"));

	SetArrows(DefaultArrows);
}
void FArmyCompass::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	JsonWriter->WriteValue("Type", TEXT("OT_Compass"));
	JsonWriter->WriteValue("ApplyABSTransform", ApplyABSTransform);
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());

	JsonWriter->WriteValue("BorderCircleRadius", BorderCircleRadius);
	JsonWriter->WriteValue("BorderCircleColor", BorderCircleColor.ToString());

	JsonWriter->WriteArrayStart("ArrowsVertices");
	for (auto & V : BaseArrowsVertices)
	{
		JsonWriter->WriteValue(V.ToString());
	}
	JsonWriter->WriteArrayEnd();
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyCompass)
}
void FArmyCompass::Deserialization(const TSharedPtr<FJsonObject>& InJsonData) 
{
	FArmyObject::Deserialization(InJsonData);

	if (InJsonData->GetStringField("Type") == TEXT("OT_Compass"))
	{
		ObjectType = OT_Compass;
	}

	InJsonData->TryGetBoolField("ApplyABSTransform", ApplyABSTransform);
	LocalTransform.InitFromString(InJsonData->GetStringField("LocalTransform"));

	BorderCircleRadius = InJsonData->GetNumberField("BorderCircleRadius");
	BorderCircleColor.InitFromString(InJsonData->GetStringField("BorderCircleColor"));

	BaseArrowsVertices.Empty();
	const TArray< TSharedPtr<FJsonValue> >& ArrowsVertices = InJsonData->GetArrayField("ArrowsVertices");
	for (auto JsonV : ArrowsVertices)
	{
		FVector V(ForceInitToZero);
		V.InitFromString(JsonV->AsString());
		BaseArrowsVertices.Add(V);
	}
	Update();
}
void FArmyCompass::SetState(EObjectState InState) 
{
	State = InState;
	MoveOperationPoint->SetState(FArmyEditPoint::OPS_Normal);
	ScaleOperationPoint->SetState(FArmyEditPoint::OPS_Normal);
	ScaleOperationPoint->SetState(FArmyEditPoint::OPS_Normal);
}
void FArmyCompass::Refresh()
{

}
void FArmyCompass::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) 
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		float NewRadius = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius, 0, 0)).Size();
		DrawCircle(PDI, LocalTransform.GetTranslation(), FVector(1, 0, 0), FVector(0, 1, 0), BorderCircleColor, NewRadius, 32, SDPG_Foreground);

		Arrows->Draw(PDI, View);

		if (State == OS_Selected)
		{
			MoveOperationPoint->Draw(PDI, View);
			ScaleOperationPoint->Draw(PDI, View);
			RotateOperationPoint->Draw(PDI, View);
		}
	}
}
void FArmyCompass::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) 
{
	float NewOffset = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius + 20, 0, 0)).Size();
	FVector TextPos = GetBasePos() + FVector(NewOffset,0,0);

	FVector2D TextScale(LocalTransform.GetScale3D());
	TextPos.Y -= (TextScale.Y * 100 / 2);
	FScaleCanvasText FillTextItem;
	FillTextItem.BaseScale = TextScale;
	FillTextItem.SetTextSize(100);
	FillTextItem.SetPosition(TextPos);
	FillTextItem.Text = FText::FromString(TEXT("N"));
	FillTextItem.DrawHUD(InViewPortClient, View, Canvas);
}
bool FArmyCompass::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	/* @梁晓菲 根据鼠标位置到图形中心点的距离确定是否选中*/
	float Radius = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius, 0, 0)).Size();
	if ((Pos - GetBasePos()).Size() <= Radius || Arrows->IsSelected(Pos, InViewportClient))
	{
		return true;
	}
	return false; 
}
TSharedPtr<FArmyEditPoint> FArmyCompass::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	TSharedPtr<FArmyEditPoint> ResultPoint = NULL;
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (GetState() == OS_Selected)
		{
			if (MoveOperationPoint->IsSelected(Pos, InViewportClient))        
				ResultPoint = MoveOperationPoint;
			else if (ScaleOperationPoint->IsSelected(Pos, InViewportClient))     
				ResultPoint = ScaleOperationPoint;
			else if (RotateOperationPoint->IsSelected(Pos, InViewportClient))
				ResultPoint = RotateOperationPoint;

			if (ResultPoint.IsValid())                  
				ResultPoint->SetState(FArmyEditPoint::OPS_Selected);
		}
	}
	return ResultPoint;
}
TSharedPtr<FArmyEditPoint> FArmyCompass::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return SelectPoint(Pos,InViewportClient);
}
bool FArmyCompass::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return false; 
}
void FArmyCompass::GetVertexes(TArray<FVector>& OutVertexes) 
{

}
void FArmyCompass::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{

}
void FArmyCompass::ApplyTransform(const FTransform& Trans) 
{
	if (ApplyABSTransform)
	{
		LocalTransform = Trans;
	}
	else
	{
		LocalTransform.Accumulate(Trans);
	}
	Update();
}
const FVector FArmyCompass::GetBasePos()
{
	return LocalTransform.GetLocation();;
}
const FBox FArmyCompass::GetBounds() 
{ 
	FBox TempBox(ForceInitToZero);

	FVector CircleCenter = GetBasePos();

	float NewRadius = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius, 0, 0)).Size();

	TempBox.Min = CircleCenter + FVector(-NewRadius, -NewRadius, 0);
	TempBox.Max = CircleCenter + FVector(NewRadius, NewRadius, 0);

	//if (Arrows.IsValid())
	//{
	//	for (auto& V : Arrows->Vertices)
	//	{
	//		TempBox += LocalTransform.TransformPosition(V);
	//	}
	//}
	return TempBox;
}
void FArmyCompass::SetBorderCircleRadius(float InRadius)
{
	BorderCircleRadius = InRadius;
	BuildOperationPoint();
}
void FArmyCompass::SetBorderCircleColor(FLinearColor InColor)
{
	BorderCircleColor = InColor;
}
void FArmyCompass::SetArrows(TSharedPtr<FArmyPolygon> InArrows)
{
	BaseArrowsVertices = InArrows->Vertices;

	Arrows = InArrows;
	BuildOperationPoint();
}
void FArmyCompass::Update()
{
	TArray<FVector> Vertexes;

	for (auto & V : BaseArrowsVertices)
	{
		Vertexes.Add(LocalTransform.TransformPosition(V));
	}
	Arrows->SetVertices(Vertexes);

	BuildOperationPoint();
}
void FArmyCompass::SetTranFormModel(bool InABSModel)
{
	ApplyABSTransform = InABSModel;
}
void FArmyCompass::BuildOperationPoint()
{
	if (MoveOperationPoint.IsValid())
	{
		MoveOperationPoint->SetPos(GetBasePos());
	}
	else
	{
		MoveOperationPoint = MakeShareable(new FArmyEditPoint);
		MoveOperationPoint->OperationType = XROP_MOVE;
		MoveOperationPoint->SetPos(GetBasePos());
	}

	float NewRadius = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius, 0, 0)).Size();

	if (ScaleOperationPoint.IsValid())
	{
		ScaleOperationPoint->SetPos(MoveOperationPoint->GetPos() + FVector(NewRadius, 0, 0));
	}
	else
	{
		ScaleOperationPoint = MakeShareable(new FArmyEditPoint);
		ScaleOperationPoint->OperationType = XROP_SCALE;
		ScaleOperationPoint->SetPos(MoveOperationPoint->GetPos() + FVector(NewRadius, 0, 0));
	}

	if (RotateOperationPoint.IsValid())
	{
		FVector OnCirclePos = LocalTransform.TransformPosition(FVector(0, -BorderCircleRadius - 50, 0));
		//OnCirclePos += (OnCirclePos - MoveOperationPoint->GetPos()).GetSafeNormal() * 50;
		RotateOperationPoint->SetPos(OnCirclePos);
	}
	else
	{
		RotateOperationPoint = MakeShareable(new FArmyEditPoint);
		RotateOperationPoint->OperationType = XROP_ROTATE;
		FVector OnCirclePos = LocalTransform.TransformPosition(FVector(0, -BorderCircleRadius - 50, 0));
		//OnCirclePos += (OnCirclePos - MoveOperationPoint->GetPos()).GetSafeNormal() * 50;
		RotateOperationPoint->SetPos(OnCirclePos);
	}
}
// 获得圆-CAD
TSharedPtr<FArmyCircle> FArmyCompass::GetCircleInfo()
{
	TSharedPtr<FArmyCircle> Circle = MakeShareable(new FArmyCircle);
	Circle->Position = LocalTransform.GetTranslation();
	Circle->Radius = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius, 0, 0)).Size();
	return Circle;
}

// 获得填充文字-CAD
FScaleCanvasText FArmyCompass::GetCanvasText()
{
	float NewOffset = (LocalTransform.GetScale3D() * FVector(BorderCircleRadius + 20, 0, 0)).Size();
	FVector TextPos = GetBasePos() + FVector(NewOffset, 0, 0);

	FVector2D TextScale(LocalTransform.GetScale3D());
	TextPos.Y -= (TextScale.Y * 100 / 2);
	FScaleCanvasText FillTextItem;
	FillTextItem.BaseScale = TextScale;
	FillTextItem.SetTextSize(100);
	FillTextItem.SetPosition(TextPos);
	FillTextItem.Text = FText::FromString(TEXT("N"));

	return FillTextItem;
}