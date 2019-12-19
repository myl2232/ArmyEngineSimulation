#pragma once

#include "CoreMinimal.h"
#include "SCTModelShape.h"
#include "Json.h"
#include "JsonWriter.h"

/**
 * @brief 将BIM端的Json数据转换成SCT型录端能够对接的五金Shape
 */
class SCTSHAPEMODULE_API FAccessShapeForBim : public FAccessoryShape
{
public:
	virtual void ParseFromJson(const TSharedPtr<FJsonObject>& InJsonObject) override;
	virtual void SaveToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
};