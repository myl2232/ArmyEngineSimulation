#include "ArmyPipeGraph.h"
#include "ArmyEditPoint.h"
#include "CoreMinimal.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyStyle.h"
#include "ArmyMath.h"
#include "ArmyPipePointActor.h"
#include "SceneManagement.h"
#include "ArmyViewportClient.h"
#include "ArmyConstructionQuantity.h"
#include "ArmySceneData.h"
#include "ArmyPipePoint.h"


FArmyPipeGraph::FArmyPipeGraph()
{

}

FArmyPipeGraph::~FArmyPipeGraph()
{

}

void FArmyPipeGraph::SerializeToJson(TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
    SERIALIZEREGISTERCLASS(JsonWriter, FArmyPipeGraph)
}

void FArmyPipeGraph::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{

}

void FArmyPipeGraph::AddEdge(TSharedPtr<FArmyPipePoint>& InStart,TSharedPtr<FArmyPipePoint>& InEnd)
{
	PipeAdjacency.Add(InStart,InEnd->GetUniqueID());
	PipeAdjacency.Add(InEnd,InStart->GetUniqueID());
	verticals = PipeAdjacency.Num();
	Objectes.AddUnique(InStart);
	Objectes.AddUnique(InEnd);
}

void FArmyPipeGraph::AddEdge(TSharedPtr<FArmyPipeline>& PipeLine)
{
	AddEdge(PipeLine->StartPoint,PipeLine->EndPoint);
	PipeLines.AddUnique(PipeLine);
	Objectes.AddUnique(PipeLine);
	Edges = PipeLines.Num();
}

void FArmyPipeGraph::GetObjects(EObjectType InObjectType,TArray<FObjectWeakPtr>& OutObjects)
{
	for(auto ObjIt:Objectes)
	{
		if(ObjIt.Pin()->GetType()==InObjectType)
		{
			OutObjects.Add(ObjIt);
		}
	}
}
