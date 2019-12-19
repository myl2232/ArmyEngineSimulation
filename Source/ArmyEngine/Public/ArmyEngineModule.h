#pragma once

#include "Modules/ModuleInterface.h"
#include "ArmyEngineResource.h"

class FArmyEngineModule : public IModuleInterface
{
public:

	// IModuleInterface Interface Begin
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	ARMYENGINE_API static FArmyEngineModule& Get();

    ARMYENGINE_API UArmyEngineResource* GetEngineResource();

private:
	static FArmyEngineModule* Singleton;

    UArmyEngineResource* EngineResource;
};