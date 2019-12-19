#include "ArmyTransactionModule.h"
#include "Modules/ModuleManager.h"

FArmyTransactionModule* FArmyTransactionModule::Singleton = NULL;

void FArmyTransactionModule::StartupModule()
{
    Singleton = this;
    Undo = NULL;
}

void FArmyTransactionModule::ShutdownModule()
{
    Singleton = NULL;
}

FArmyTransactionModule& FArmyTransactionModule::Get()
{
    if (Singleton == NULL)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmyTransactionModule>("ArmyTransaction");
    }
    check(Singleton != NULL);

    return *Singleton;
}

IMPLEMENT_MODULE(FArmyTransactionModule, ArmyTransaction);