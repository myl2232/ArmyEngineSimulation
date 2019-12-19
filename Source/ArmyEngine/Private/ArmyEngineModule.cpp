#include "ArmyEngineModule.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FArmyEngineModule, ArmyEngine);

FArmyEngineModule* FArmyEngineModule::Singleton = NULL;

void FArmyEngineModule::StartupModule()
{
	Singleton = this;
}

void FArmyEngineModule::ShutdownModule()
{
	Singleton = nullptr;
}

ARMYENGINE_API FArmyEngineModule& FArmyEngineModule::Get()
{
	if (Singleton == NULL)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FArmyEngineModule>("ArmyEngine");
	}
	check(Singleton != NULL);

	return *Singleton;
}

UArmyEngineResource* FArmyEngineModule::GetEngineResource()
{
    if (!EngineResource)
    {
        EngineResource = NewObject<UArmyEngineResource>();
        EngineResource->AddToRoot();
    }

    return EngineResource;
}