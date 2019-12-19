#include "FArmyAreaEdge.h"
#include "JsonWriter.h"
#include "JsonObject.h"

void FArmyAreaEdge::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	JsonWriter->WriteValue("start", start.ToString());
	JsonWriter->WriteValue("end", end.ToString());
}

void FArmyAreaEdge::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	start.InitFromString(InJsonData->GetStringField("start"));
	end.InitFromString(InJsonData->GetStringField("end"));
}
