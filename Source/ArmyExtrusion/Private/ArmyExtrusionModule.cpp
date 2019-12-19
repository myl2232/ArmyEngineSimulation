#include "../Public/XRExtrusionModule.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogExtrusion, Log, All);

IMPLEMENT_MODULE(FXRExtrusionModule, Extrusion);

FXRExtrusionModule* FXRExtrusionModule::Singleton = NULL;

void FXRExtrusionModule::StartupModule()
{
	Singleton = this;
}

void FXRExtrusionModule::ShutdownModule()
{
	Singleton = nullptr;
}

FXRExtrusionModule& FXRExtrusionModule::Get()
{
	if (Singleton == NULL)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FXRExtrusionModule>("XREXTRUSION");
	}
	check(Singleton != NULL);
	return *Singleton;
}
