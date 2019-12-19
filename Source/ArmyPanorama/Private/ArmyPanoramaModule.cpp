#include "ArmyPanoramaModule.h"
#include "Object.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FArmyPanoramaModule, XRPanorama);

FArmyPanoramaModule* FArmyPanoramaModule::Singleton = NULL;

void FArmyPanoramaModule::StartupModule()
{
}

void FArmyPanoramaModule::ShutdownModule()
{
	Singleton = nullptr;
}

XRPANORAMA_API FArmyPanoramaModule& FArmyPanoramaModule::Get()
{
	if (Singleton == NULL)
	{
		check(IsInGameThread());
		Singleton = &FModuleManager::LoadModuleChecked<FArmyPanoramaModule>("ArmyPanorama");
	}
	check(Singleton != NULL);
	return *Singleton;
}

XRPANORAMA_API TSharedPtr<FArmyPanoramaController> FArmyPanoramaModule::GetPanoramaController()
{
	if (!PanoramaController.IsValid())
		PanoramaController = MakeShareable(new FArmyPanoramaController());

	return PanoramaController;
}
