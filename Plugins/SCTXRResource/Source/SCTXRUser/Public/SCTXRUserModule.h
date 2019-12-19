#pragma once

#include "Modules/ModuleInterface.h"

class FSCTXRUserModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    SCTXRUSER_API static FSCTXRUserModule& Get();

    //SCTXRUSER_API TSharedPtr<FSCTXRUserModule> GetUserManager();

private:
    static FSCTXRUserModule* Singleton;

    //TSharedPtr<FSCTXRUserModule> UserManager;
};
