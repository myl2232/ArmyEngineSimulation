#include "ArmyTransaction.h"
#include "JsonWriter.h"
#include "ArmyObject.h"
#include "ArmySceneData.h"

FArmyTransaction::FArmyTransaction(const FString& InDescription)
    : Description(InDescription)
{
}

void FArmyTransaction::Undo()
{
    // 撤销时要按照堆栈倒序撤销
    for (int32 i = Records.Num() - 1; i >= 0; i--)
    {
		TSharedPtr<FArmyObjectRecord> Record=Records[i];
        if (Record->TransType == TT_Create)
        {
            TSharedPtr<FJsonObject> JsonObject;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Record->Record), JsonObject))
            {
                UpdatePrimaryObjectPtr(Record, JsonObject);

				if (Record->PrimaryObject.Pin().IsValid())
				{
					Record->PrimaryObject.Pin()->PostDelete();
					FArmySceneData::Get()->Delete(Record->PrimaryObject.Pin(), true);
				}
            }
        }
        else if (Record->TransType == TT_Delete)
        { 
            TSharedPtr<FJsonObject> JsonObject;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Record->Record), JsonObject))
            {
                const FString ClassName = JsonObject->GetStringField("RegisterClass");
                TSharedPtr<FArmyObject> XRObject = ClassFactory::GetInstance(ClassName);
                XRObject->Deserialization(JsonObject);

				if (Record->PrimaryObject.Pin().IsValid())
				{
					FArmySceneData::Get()->Add(XRObject, Record->Args, nullptr, true);
					Record->PrimaryObject = XRObject;
				}
            }
        }
        else if (Record->TransType == TT_Modify)
        {
            TSharedPtr<FJsonObject> JsonObject;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Record->RecordBeforeModify), JsonObject))
            {
                UpdatePrimaryObjectPtr(Record, JsonObject);

				if (Record->PrimaryObject.Pin().IsValid())
				{
					Record->PrimaryObject.Pin()->Deserialization(JsonObject);
					Record->PrimaryObject.Pin()->Record = Record->RecordBeforeModify;

					Record->PrimaryObject.Pin()->PostModify(true);
				}
            }
        }
    }
}

void FArmyTransaction::Redo()
{
    for (auto& It : Records)
    {
        if (It->TransType == TT_Create)
        {
            TSharedPtr<FJsonObject> JsonObject;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(It->Record), JsonObject))
            {
                const FString ClassName = JsonObject->GetStringField("RegisterClass");
                TSharedPtr<FArmyObject> XRObject = ClassFactory::GetInstance(ClassName);
                XRObject->Deserialization(JsonObject);
				XRObject->RecordBeforeModify = It->Record;
                FArmySceneData::Get()->Add(XRObject, It->Args, nullptr, true);
                It->PrimaryObject = XRObject;
            }
        }
        else if (It->TransType == TT_Delete)
        {
            TSharedPtr<FJsonObject> JsonObject;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(It->Record), JsonObject))
            {
                UpdatePrimaryObjectPtr(It, JsonObject);

				if (It->PrimaryObject.Pin().IsValid())
				{ 
					It->PrimaryObject.Pin()->PostDelete();
					FArmySceneData::Get()->Delete(It->PrimaryObject.Pin(), true);
				}
            }
        }
        else if (It->TransType == TT_Modify)
        {
            TSharedPtr<FJsonObject> JsonObject;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(It->Record), JsonObject))
            {
                UpdatePrimaryObjectPtr(It, JsonObject);

				if (It->PrimaryObject.Pin().IsValid())
				{
					It->PrimaryObject.Pin()->Deserialization(JsonObject);
					It->PrimaryObject.Pin()->Record = It->Record;

					It->PrimaryObject.Pin()->PostModify(true);
				}
            }
        }
    }
}

void FArmyTransaction::SaveXRObject(ETransType TransType, TWeakPtr<FArmyObject> PrimaryObject, FString& RecordBeforeModify, FString& Record)
{
    if (RecordBeforeModify.Len() > 0 && Record.Len() > 0)
    {
        RecordBeforeModify = Record;
    }

    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
        TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Record);

    // 将XRObject序列化为json字符串
    JsonWriter->WriteObjectStart();
    PrimaryObject.Pin()->SerializeToJson(JsonWriter);
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    if (RecordBeforeModify.Len() == 0)
    {
        RecordBeforeModify = Record;
    }
    // 记录对象
    Records.Add(MakeShareable(new FArmyObjectRecord(TransType, PrimaryObject, RecordBeforeModify, Record, PrimaryObject.Pin()->Args)));
}

void FArmyTransaction::UpdatePrimaryObjectPtr(TSharedPtr<FArmyObjectRecord> Record, TSharedPtr<FJsonObject> JsonObject)
{
    // 更新指针失败可能有以下几个原因:
    // 1. XRObject删除但没有释放，导致Record->PrimaryObject不为空
    // 2. GUID没有被正确序列化/反序列化
    // 3. 没有从FArmySceneData里找到该GUID对应的XRObject
    if (!Record->PrimaryObject.IsValid())
    {
        FGuid ObjID;
        FGuid::Parse(JsonObject->GetStringField("objectId"), ObjID);
        Record->PrimaryObject = FArmySceneData::Get()->GetObjectByGuid((EModelType)Record->Args._ArgUint32, ObjID).Pin();
    }
}
