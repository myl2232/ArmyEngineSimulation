#pragma once
#include "ArmyObjectMinimal.h"
#include "Modules/ModuleInterface.h"

class FArmyUserModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    ARMYUSER_API static FArmyUserModule& Get();

private:
    static FArmyUserModule* Singleton;
};
