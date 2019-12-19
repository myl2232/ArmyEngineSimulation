
#include "SCTAccessShapeForBim.h"

void FAccessShapeForBim::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{

	// 通过pakMd5字段来判断是原始的BIM端Json对象，还是已经被保存过的Json对象
	if (InJsonObject->HasField(TEXT("fileMd5")) == false)
	{
		FAccessoryShape::ParseFromJson(InJsonObject);
		SetShapeType(ST_Accessory);
		SetShapeCategory(InJsonObject->GetIntegerField(TEXT("type")));
		if (InJsonObject->HasField(TEXT("fileUrl")))
		{
			SetFileUrl(InJsonObject->GetStringField(TEXT("fileUrl")));
		}
		else if (InJsonObject->HasField(TEXT("pakUrl")))
		{
			SetFileUrl(InJsonObject->GetStringField(TEXT("pakUrl")));
		}
		SetFileMd5(InJsonObject->GetStringField(TEXT("pakMd5")));
		SetoptimizeParam(InJsonObject->GetStringField(TEXT("optimizeParam")));
	}
	else
	{
		FAccessoryShape::ParseFromJson(InJsonObject);
	}
	
}

void FAccessShapeForBim::SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	//此接口暂时为了让BIM前段使用
	FAccessoryShape::SaveToJson(JsonWriter);
}
