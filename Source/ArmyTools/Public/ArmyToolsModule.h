#pragma once

#include "Modules/ModuleInterface.h"
//#include "ArmyObjectMinimal.h"
#include "ArmyMouseCapture.h"

class FArmyToolsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	ARMYTOOLS_API static FArmyToolsModule& Get();

	ARMYTOOLS_API TSharedPtr<class FArmyMouseCapture> GetMouseCaptureTool();

	ARMYTOOLS_API TSharedPtr<class FArmyRectSelect> GetRectSelectTool();

	//ARMYTOOLS_API static TArray<TSharedPtr<FArmyFurniture>> ParseDXF(const FString& InFilePath);
	// 通过DXF获得图例
	//ARMYTOOLS_API static TSharedPtr<FArmyFurniture> GetFurnitureByDXF(const FString& InFilePath);
private:
    static FArmyToolsModule* Singleton;

	TSharedPtr<class FArmyMouseCapture> MouseCaptureTool;

	TSharedPtr<class FArmyRectSelect> RectSelectTool;
};