
#pragma once

#include "ModuleManager.h"


class FUEGTEngineModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	void*	LibraryHandle;
};
