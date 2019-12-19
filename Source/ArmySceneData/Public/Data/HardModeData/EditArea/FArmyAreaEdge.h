/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @Module FArmyAreaEdge
* @Description 用以表示某个区域的单条边
*
* @Author 梁晓菲
* @Date 2019年2月21日
* @Version 1.2
*/
#pragma once

#include "Vector.h"
#include "JsonWriter.h"
#include "JsonObject.h"

class FArmyAreaEdge 
{
public:
	FArmyAreaEdge(FVector EdgeStart, FVector EdgeEnd)
	{
		start = EdgeStart;
		end = EdgeEnd;
	}
	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter);

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData);

	FVector GetStartPos()const { return start; }

	FVector GetEndPos()const { return end; }

private:
	friend bool operator==(const FArmyAreaEdge& edge1, const FArmyAreaEdge& edge2)
	{
		return (edge1.start == edge2.start && edge1.end == edge2.end)
			|| (edge1.start == edge2.end && edge1.end == edge2.start);

	}
	friend bool operator!=(const FArmyAreaEdge& edge1, const FArmyAreaEdge& edge2)
	{
		return !(edge1 == edge2);
	}
	friend uint32 GetTypeHash(const FArmyAreaEdge& edge)
	{
		FCrc::Init();
		return FCrc::MemCrc32(&edge, sizeof(FArmyAreaEdge));
	}

private:
	FVector start;
	FVector end;
};