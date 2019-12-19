#pragma once
#include "ArmyBaseArea.h"
class FArmyHelpRectArea :public FArmyBaseArea
{
public:
	FArmyHelpRectArea();
	~FArmyHelpRectArea();
	void SetVertices(const TArray<FVector>& InVertices);
	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void SetStyle(TSharedPtr<class FArmyBaseEditStyle> InStyle)override;
	virtual void BeginDestroy();
	virtual void SetActorVisible(bool InVisible)override;


};