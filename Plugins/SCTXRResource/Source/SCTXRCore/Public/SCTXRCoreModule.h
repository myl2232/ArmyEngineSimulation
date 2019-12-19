#include "Modules/ModuleInterface.h"

class FSCTXRCoreModule : public IModuleInterface
{
    virtual void StartupModule();

    virtual void ShutdownModule();
};