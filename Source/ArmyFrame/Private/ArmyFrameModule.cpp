#include "ArmyFrameModule.h"
#include "ModuleManager.h"

IMPLEMENT_MODULE(FArmyFrameModule, ArmyFrame);

FArmyFrameModule* FArmyFrameModule::Singleton = nullptr;

void FArmyFrameModule::StartupModule()
{
    Singleton = this;
}

void FArmyFrameModule::ShutdownModule()
{
    Singleton = nullptr;
}

FArmyFrameModule& FArmyFrameModule::Get()
{
    if (!Singleton)
    {
        check(IsInGameThread());
        FModuleManager::LoadModuleChecked<FArmyFrameModule>("ArmyFrame");
    }
    check(Singleton);

    return *Singleton;
}