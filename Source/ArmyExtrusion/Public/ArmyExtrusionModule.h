#include "CoreMinimal.h"
#include "XRObjectMinimal.h"
#include "Modules/ModuleInterface.h"

class FXRExtrusionModule
	: public IModuleInterface
{
public:

	// IModuleInterface interface

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	XREXTRUSION_API static FXRExtrusionModule& Get();

private:
	static FXRExtrusionModule* Singleton;
};


