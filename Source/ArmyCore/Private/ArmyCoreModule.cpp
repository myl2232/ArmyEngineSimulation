#include "ArmyCoreModule.h"

#include "Modules/ModuleManager.h"
#include "ArmyStyle.h"

IMPLEMENT_MODULE(FArmyCoreModule, ArmyCore);

void FArmyCoreModule::StartupModule()
{
    FArmyStyle::Startup();
}

void FArmyCoreModule::ShutdownModule()
{
    FArmyStyle::Shutdown();
}