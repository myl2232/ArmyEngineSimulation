#pragma once

#include "Modules/ModuleInterface.h"

class FArmyDrawCoreModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    ARMYDRAWCORE_API static FArmyDrawCoreModule& Get();

private:
    static FArmyDrawCoreModule* Singleton;
};