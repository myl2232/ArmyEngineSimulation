#include "ArmyHelpPolygonArea.h"
#include "ArmyWallActor.h"
FArmyHelpRectArea::FArmyHelpRectArea() :FArmyBaseArea()
{
	WallActor = NULL;
	ObjectType = OT_HelpRectArea;
}

FArmyHelpRectArea::~FArmyHelpRectArea()
{

}

void FArmyHelpRectArea::SetVertices(const TArray<FVector>& InVertices)
{
	InneraArea->Vertices = InVertices;
	OutArea->Vertices = InVertices;
	TArray<TArray<FVector>> tempInearHoles;
	if (MatStyle.IsValid())
	{
		MatStyle->SetDrawArea(InVertices, tempInearHoles);
		CalculateWallActor(MatStyle);
	}
	CalculateBackGroundSurface(InneraArea->Vertices, tempInearHoles);
}



void FArmyHelpRectArea::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyObject::SerializeToJson(JsonWriter);
	FArmyBaseArea::SerializeToJson(JsonWriter);
	JsonWriter->WriteArrayStart(TEXT("vertexes"));

	for (FVector& Vertex : InneraArea->Vertices)
	{
		JsonWriter->WriteValue(Vertex.ToString());
	}
	JsonWriter->WriteArrayEnd();
}

void FArmyHelpRectArea::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	FArmyObject::Deserialization(InJsonData);
	FArmyBaseArea::Deserialization(InJsonData);
	TArray<FVector> Vertexes;
	TArray<FString> VertexesStrArray;
	InJsonData->TryGetStringArrayField("vertexes", VertexesStrArray);
	for (auto& ArrayIt : VertexesStrArray)
	{
		FVector Vertex;
		Vertex.InitFromString(ArrayIt);
		Vertexes.Add(Vertex);
	}
	TArray<TArray<FVector>> tempHoles;
	InneraArea->Vertices = Vertexes;
	OutArea->Vertices = Vertexes;
	MatStyle->SetDrawArea(Vertexes, tempHoles);
	CalculateBackGroundSurface(Vertexes, tempHoles);
	CalculateWallActor(MatStyle);
}

void FArmyHelpRectArea::SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)
{
	FArmyBaseArea::SetStyle(InStyle);
}

void FArmyHelpRectArea::BeginDestroy()
{
	if (WallActor)
	{
		WallActor->Destroy();
		WallActor = NULL;
	}
}

void FArmyHelpRectArea::SetActorVisible(bool InVisible)
{
	FArmyBaseArea::SetActorVisible(InVisible);
	if (WallActor)
	{
		WallActor->SetActorHiddenInGame(!InVisible);
	}
}


