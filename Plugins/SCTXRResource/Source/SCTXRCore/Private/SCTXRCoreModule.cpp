#include "SCTXRCoreModule.h"

#include "Modules/ModuleManager.h"
#include "SCTXRStyle.h"

IMPLEMENT_MODULE(FSCTXRCoreModule, SCTXRCore);

void FSCTXRCoreModule::StartupModule()
{
    FSCTXRStyle::Startup();
}

void FSCTXRCoreModule::ShutdownModule()
{
    FSCTXRStyle::Shutdown();
}