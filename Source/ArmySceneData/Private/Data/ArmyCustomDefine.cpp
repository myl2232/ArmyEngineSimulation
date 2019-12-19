#include "ArmyCustomDefine.h" 
#include "ArmyEditorViewportClient.h"

FArmyCustomDefine::FArmyCustomDefine()
{
	ObjectType = OT_UserDefine;
}

FArmyCustomDefine::FArmyCustomDefine(const TArray<struct FSimpleElementVertex>& InVertexArray, const FTransform& Trans, int32 InBelongClass)
{
	ObjectType = OT_UserDefine;
	BaseVertexArray = InVertexArray;
	BelongClass = InBelongClass;
	LocalTransform = Trans;

	Update();
}

FArmyCustomDefine::~FArmyCustomDefine() 
{

}
void FArmyCustomDefine::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	
	JsonWriter->WriteValue("BelongClass", BelongClass);
	JsonWriter->WriteValue("LocalTransform", LocalTransform.ToString());
	JsonWriter->WriteArrayStart("BaseVertexArray");
	for (auto& V : BaseVertexArray)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("Position", V.Position.ToString());
		JsonWriter->WriteValue("TextureCoordinate", V.TextureCoordinate.ToString());
		JsonWriter->WriteValue("Color", V.Color.ToString());
		JsonWriter->WriteValue("HitProxyIdColor", V.HitProxyIdColor.ToString());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	SERIALIZEREGISTERCLASS(JsonWriter, FArmyCustomDefine)
}
void FArmyCustomDefine::Deserialization(const TSharedPtr<FJsonObject>& InJsonData) 
{
	FArmyObject::Deserialization(InJsonData);
	InJsonData->TryGetNumberField("BelongClass", BelongClass);
	LocalTransform.InitFromString(InJsonData->GetStringField("LocalTransform"));

	BaseVertexArray.Empty();
	const TArray< TSharedPtr<FJsonValue> >* OutArray = NULL;
	if (InJsonData->TryGetArrayField("BaseVertexArray", OutArray))
	{
		for (auto JsonV : *OutArray)
		{
			const TSharedPtr<FJsonObject> VObj = JsonV->AsObject();
			if (VObj.IsValid())
			{
				int32 Index = BaseVertexArray.Add(FSimpleElementVertex());
				FSimpleElementVertex& V = BaseVertexArray[Index];
				V.Position.InitFromString(VObj->GetStringField("Position"));
				V.TextureCoordinate.InitFromString(VObj->GetStringField("TextureCoordinate"));
				V.Color.InitFromString(VObj->GetStringField("Color"));
				V.HitProxyIdColor.InitFromString(VObj->GetStringField("HitProxyIdColor"));
			}
		}
	}
	Update();
}
void FArmyCustomDefine::SetState(EObjectState InState) 
{
	State = InState;
}
void FArmyCustomDefine::Refresh()
{

}
void FArmyCustomDefine::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View) 
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		PDI->DrawLines(DrawVertexArray, SDPG_Foreground);
	}
}
void FArmyCustomDefine::DrawHUD(class UArmyEditorViewportClient* InViewPortClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) 
{

}
bool FArmyCustomDefine::IsSelected(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return false; 
}
TSharedPtr<FArmyEditPoint> FArmyCustomDefine::SelectPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return NULL;
}
TSharedPtr<FArmyEditPoint> FArmyCustomDefine::HoverPoint(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return SelectPoint(Pos,InViewportClient);
}
bool FArmyCustomDefine::Hover(const FVector& Pos, class UArmyEditorViewportClient* InViewportClient) 
{ 
	return false; 
}
void FArmyCustomDefine::GetVertexes(TArray<FVector>& OutVertexes) 
{

}
void FArmyCustomDefine::GetLines(TArray< TSharedPtr<FArmyLine> >& OutLines, bool IsAbs)
{
	if (GetPropertyFlag(FArmyObject::PropertyType::FLAG_VISIBILITY))
	{
		OutLines.Append(LineArray);
	}
}
void FArmyCustomDefine::ApplyTransform(const FTransform& Trans) 
{
	LocalTransform = Trans;
	Update();
}
void FArmyCustomDefine::SetBaseVertexArray(const TArray<struct FSimpleElementVertex>& InVertexArray)
{
	BaseVertexArray = InVertexArray;
	Update();
}
const FVector FArmyCustomDefine::GetBasePos()
{
	return LocalTransform.GetLocation();;
}
const FBox FArmyCustomDefine::GetBounds() 
{ 
	return Bounds;
}
void FArmyCustomDefine::Update()
{
	DrawVertexArray = BaseVertexArray;
	for (auto & V : DrawVertexArray)
	{
		V.Position = LocalTransform.TransformPosition(V.Position);
	}
	LineArray.Empty();
	for (int32 i = 0; i < DrawVertexArray.Num() - 1; i += 2)
	{
		LineArray.Add(MakeShareable(new FArmyLine(DrawVertexArray[i].Position, DrawVertexArray[i + 1].Position)));
	}
	
	//更新Bounds
	TArray<FVector> Vertices;
	for (auto& It : DrawVertexArray)
	{
		Vertices.Add(It.Position);
	}
	Bounds = FBox(Vertices);
}

// 获得绘制CAD的线段列表
void FArmyCustomDefine::GetDrawCadLines(TArray<TSharedPtr<FArmyLine>>& OutLineList)
{
	for (auto It : LineArray)
	{
		OutLineList.Add(It);
	}
}