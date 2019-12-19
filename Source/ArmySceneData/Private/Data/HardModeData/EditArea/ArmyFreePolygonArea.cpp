#include "ArmyFreePolygonArea.h"
#include "ArmyPlayerController.h"
#include "DrawStyle/XRBaseEditStyle.h"
FArmyFreePolygonArea::FArmyFreePolygonArea() :FArmyRegularPolygonArea()
{

	ObjectType = OT_FreePolygonArea;
	bBoundingBox = false;
	InneraArea->MaterialRenderProxy = FArmyEngineModule::Get().GetEngineResource()->GetFloorMaterial()->GetRenderProxy(false);
}

void FArmyFreePolygonArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	SERIALIZEREGISTERCLASS(JsonWriter, FArmyFreePolygonArea)
	FArmyObject::SerializeToJson(JsonWriter);
	FArmyBaseArea::SerializeToJson(JsonWriter);
	FArmyRegularPolygonArea::SerializeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("vertexes"));

	for (FVector& Vertex : InneraArea->Vertices)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
}

void FArmyFreePolygonArea::Draw(class FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	if (GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
	{
		if (FArmyObject::GetDrawModel(MODE_CONSTRUCTION))
		{
			DrawConstructionMode(PDI, View);
		}
		else
		{
			if (BeDrawOutLine || GXRPC->GetXRViewMode() == EXRView_CEILING)
			{
				DrawOutLine(PDI, View, FLinearColor(FColor(0XFF666666)));
			}
		}
	}
}

void FArmyFreePolygonArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	FArmyBaseArea::Deserialization(InJsonData);
	FArmyRegularPolygonArea::Deserialization(InJsonData);
	TArray<FVector> Vertexes;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("vertexes", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		Vertexes.Add(Vertex);
	}
	InneraArea->Vertices = Vertexes;
	OutArea->Vertices = Vertexes;
	TArray<TArray<FVector>> InnHoles;
	MatStyle->SetDrawArea(Vertexes, InnHoles);
	CalculateBackGroundSurface(Vertexes, InnHoles);
	CalculateWallActor(MatStyle);
}

void FArmyFreePolygonArea::SetVertices(const TArray<FVector> InVertices)
{
	InneraArea->Vertices = InVertices;
	OutArea->Vertices = InVertices;
	TArray<TArray<FVector>> Holes;
	TArray<FVector> outList = OutArea->Vertices;
	CalculateBackGroundSurface(outList, Holes);
}

void FArmyFreePolygonArea::ApplyTransform(const FTransform& tempTrans)
{
	TArray<FVector> Vertices = InneraArea->Vertices;
	if (Vertices.Num() != 0)
	{
		for (int32 i = 0; i < Vertices.Num(); i++)
		{
			Vertices[i] = tempTrans.TransformPosition(Vertices[i]);
		}
		SetVertices(Vertices);
	}

	SCOPE_TRANSACTION(TEXT("自由多边形六轴标尺移动"));
	TArray<TArray<FVector>> holes;
	MatStyle->SetDrawArea(InneraArea->Vertices, holes);
	CalculateWallActor(MatStyle);
	RefreshExtrusionPlane();
	PositionChanged.Broadcast();
	this->Modify();
	GXREditor->SelectNone(true, true, false);
}

void FArmyFreePolygonArea::RefreshPolyVertices()
{
	TArray<FVector> tempOutVertices;
	TArray<TArray<FVector>> Holes;
	CalculateOutAndInnerHoles(tempOutVertices, Holes);
	if (SurfaceType == 1)
	{
		if (dropVerts.Num() > 0)
			Holes.Emplace(dropVerts);
	}
	if (MatStyle.IsValid())
	{
		MatStyle->SetDrawArea(tempOutVertices, Holes);
		if (M_ExtrusionHeight != 0.0f)
			MatStyle->SetStylePlaneOffset(M_ExtrusionHeight);
		CalculateWallActor(MatStyle);
	}
	//@郭子阳 刷新子面
	for (auto & SubArea : RoomEditAreas)
	{
		SubArea->RefreshPolyVertices();
	}

    RecalculateArea(LampSlotWidth);
}

void FArmyFreePolygonArea::Move(const FVector & Offset)
{
	TArray<FVector>  OutVertices = GetOutVertices();
	
	for (auto & Vertice : OutVertices)
	{
		Vertice += Offset;
	}
    OffsetExtrusionActors(Offset);
	
	//移动子区域
	for (auto & SubArea : RoomEditAreas)
	{
		SubArea->Move(Offset);
	}
	SetVertices(OutVertices);
	RecaculateRelatedArea();
	Modify();
}

//void FArmyFreePolygonArea::CalculateOutAndInnerHoles(TArray<FVector>& InOutArea, TArray<TArray<FVector>>& InnearHoles)
//{
//	InOutArea = OutArea->Vertices;
//}
