#include "ArmyDrawCoreModule.h"

#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FArmyDrawCoreModule, ArmyDrawCore);

FArmyDrawCoreModule* FArmyDrawCoreModule::Singleton = NULL;

void FArmyDrawCoreModule::StartupModule()
{
    Singleton = this;
}

void FArmyDrawCoreModule::ShutdownModule()
{
}

ARMYDRAWCORE_API FArmyDrawCoreModule& FArmyDrawCoreModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmyDrawCoreModule>("ArmyDrawCore");
    }
    check(Singleton != NULL);

    return *Singleton;
}