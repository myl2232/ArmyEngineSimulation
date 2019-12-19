
#include "SCTAccessShapeForBim.h"

void FAccessShapeForBim::ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject)
{

	// ͨ��pakMd5�ֶ����ж���ԭʼ��BIM��Json���󣬻����Ѿ����������Json����
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
	//�˽ӿ���ʱΪ����BIMǰ��ʹ��
	FAccessoryShape::SaveToJson(JsonWriter);
}
