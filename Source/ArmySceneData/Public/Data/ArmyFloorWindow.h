#pragma once

#include "ArmyWindow.h"

class ARMYSCENEDATA_API FArmyFloorWindow :public FArmyWindow
{
public:
	FArmyFloorWindow();
	FArmyFloorWindow(FArmyFloorWindow* Copy);
	~FArmyFloorWindow();

	virtual void SerializeToJson(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter) override;

protected:
	void GenerateWindowPillars(UWorld* World) override;
};
REGISTERCLASS(FArmyFloorWindow)