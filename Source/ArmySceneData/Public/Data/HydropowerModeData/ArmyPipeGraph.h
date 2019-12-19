#pragma once

#include "ArmyObject.h"
#include "ArmyPipePoint.h"
#include "ArmyPipeline.h"
#include "ArmyCommonTypes.h"


/**
* 水电管线无向图
*/
class ARMYSCENEDATA_API FArmyPipeGraph : public FArmyObject
{
public:
	FArmyPipeGraph();
	virtual ~FArmyPipeGraph ();

	// FObject2D Interface Begin
	virtual void SerializeToJson (TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization (const TSharedPtr<FJsonObject>& InJsonData) override;

	void AddEdge(TSharedPtr<FArmyPipePoint>& InStart,TSharedPtr<FArmyPipePoint>& InEnd);
	void AddEdge(TSharedPtr<FArmyPipeline>& PipeLine);

	void GetObjects(EObjectType InObjectType,TArray<FObjectWeakPtr>& OutObjects);
private:
	//顶点邻接链表
	/*TMap<TSharedPtr<FArmyPipePoint>,TArray<TWeakPtr<FArmyPipePoint>>> PipeAdjacency; */
	TMultiMap<TSharedPtr<FArmyPipePoint>,FGuid>PipeAdjacency;
	TArray<TSharedPtr<FArmyPipeline>> PipeLines;
	TArray<FObjectWeakPtr> Objectes;
	//顶点个数
	int32 verticals;
	// 边的个数
	int32 Edges;

};

REGISTERCLASS(FArmyPipeGraph)