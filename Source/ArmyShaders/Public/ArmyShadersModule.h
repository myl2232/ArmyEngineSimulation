#pragma once

#include "Modules/ModuleInterface.h"

class FArmyShadersModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};