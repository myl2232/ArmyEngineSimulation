#pragma once
#include "ArmyAddWall.h"
class ARMYSCENEDATA_API FArmyIndependentWall :public FArmyAddWall
{
public:

	FArmyIndependentWall();
	~FArmyIndependentWall() {};

	virtual void Draw(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

	void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;



	FVector GetWallNormal(TSharedPtr<FArmyLine> InLine);

private:
};

REGISTERCLASS(FArmyIndependentWall)