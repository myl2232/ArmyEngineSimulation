#include "FArmyConstructionItemInterface.h"

FArmyConstructionItemInterface::FArmyConstructionItemInterface()	
{	
}

void FArmyConstructionItemInterface::SetConstructionItemCheckedId(TMap<int32/*施工项ID*/, TMap<int32/*属性ID*/, int32/*属性值ID*/>>& CheckedList)
{
	CheckedId = CheckedList;
	//bHasSetted = true;
}

void FArmyConstructionItemInterface::SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{	

	JsonWriter->WriteValue<bool>(TEXT("HasConstructionData"), bHasSetted);

	JsonWriter->WriteArrayStart(TEXT("constructionItem_2"));
	for (auto& It : CheckedId)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("ConstructionID", It.Key);
		JsonWriter->WriteArrayStart(TEXT("PropertyIDArr"));
		for (auto ProIt : CheckedId[It.Key])
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue("PropertyID", ProIt.Key);
			JsonWriter->WriteValue("PropertyValueID", ProIt.Value);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();
}

void FArmyConstructionItemInterface::Deserialization(const TSharedPtr<FJsonObject>& InJsonData)
{
	if (InJsonData.IsValid())
	{
		bHasSetted = InJsonData->GetBoolField(TEXT("HasConstructionData"));

		const TArray<TSharedPtr<FJsonValue> > ConstructionStrArray = InJsonData->GetArrayField("constructionItem_2");
		for (auto& ConIt : ConstructionStrArray)
		{
			TSharedPtr<FJsonObject> ConJObject = ConIt->AsObject();
			const TArray<TSharedPtr<FJsonValue>> TempPropertyArr = ConJObject->GetArrayField("PropertyIDArr");
			TMap<int32, int32> TempProperty;
			for (auto& ProIt : TempPropertyArr)
			{
				TSharedPtr<FJsonObject> ProJObject = ProIt->AsObject();
				int32 TempKey = ProJObject->GetIntegerField("PropertyID");
				int32 TempValue = ProJObject->GetIntegerField("PropertyValueID");
				TempProperty.Add(TempKey, TempValue);
			}
			int32 TempConstructionID = ConJObject->GetIntegerField("ConstructionID");
			CheckedId.Add(TempConstructionID, TempProperty);
		}
	}	
//	bHasSetted = true;
}
