#include "ArmyUserModule.h"

IMPLEMENT_MODULE(FArmyUserModule, ArmyUser);

FArmyUserModule* FArmyUserModule::Singleton = NULL;

void FArmyUserModule::StartupModule()
{
    Singleton = this;
}

void FArmyUserModule::ShutdownModule()
{
}

ARMY_USER_API FArmyUserModule& FArmyUserModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmyUserModule>("ArmyUser");
    }
    check(Singleton != NULL);

    return *Singleton;
}