#pragma once

#include "ArmyRectBayWindow.h"

class ARMYSCENEDATA_API FArmyTrapeBayWindow : public FArmyRectBayWindow
{
public:
	FArmyTrapeBayWindow();
	FArmyTrapeBayWindow(FArmyTrapeBayWindow* Copy);
	~FArmyTrapeBayWindow();

    //~ Begin FArmyRectBayWindow Interface
	virtual void SerializeToJson(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter) override;
	virtual void Deserialization(const TSharedPtr<FJsonObject>& InJsonData) override;
	virtual void Generate(UWorld* InWorld) override;
	virtual void Update() override;
    //~ End FArmyRectBayWindow Interface

	void SetUpHoleLength(float length);
	float GetUpHoleLength()const { return BayWindowHoleUpLength; }

	virtual void SetOutWindowLength(float InOutLength);
protected:
	/**@欧石楠 更新绘制数据*/
	virtual void UpdateWindowData() override;
	virtual void UpdateArcAngle() override;
	float BayWindowHoleUpLength;
};

REGISTERCLASS(FArmyTrapeBayWindow)
