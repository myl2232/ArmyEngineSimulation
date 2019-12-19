#include "ArmySlateModuleSlate.h"
#include "SlateOptMacros.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"

IMPLEMENT_MODULE(FArmySlateModule, ArmySlate);

FArmySlateModule* FArmySlateModule::Singleton = NULL;

void FArmySlateModule::StartupModule()
{
	Singleton = this;

    WebImageCache = MakeShareable(new FWebImageCache());
	/*if (!ModalManager.IsValid())
	{
		ModalManager = MakeShareable(new FArmyModalManager());
	}*/
}

void FArmySlateModule::ShutdownModule()
{
	Singleton = nullptr;
}

ARMYSLATE_API FArmySlateModule& FArmySlateModule::Get()
{
	if (Singleton == NULL)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FArmySlateModule>("ArmySlate");
	}
	check(Singleton != NULL);
	return *Singleton;
}
