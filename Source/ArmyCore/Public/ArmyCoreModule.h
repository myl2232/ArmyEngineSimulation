#include "Modules/ModuleInterface.h"

class FArmyCoreModule : public IModuleInterface
{
    virtual void StartupModule();

    virtual void ShutdownModule();
};