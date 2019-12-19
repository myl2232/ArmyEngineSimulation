#pragma once

#include "CoreMinimal.h"
#include "SCTModelShape.h"
#include "Json.h"
#include "JsonWriter.h"

/**
 * @brief ��BIM�˵�Json����ת����SCT��¼���ܹ��Խӵ����Shape
 */
class SCTSHAPEMODULE_API FAccessShapeForBim : public FAccessoryShape
{
public:
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
};