#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "ArmyModalManagerSlate.h"

class FArmySlateModule
	: public IModuleInterface
{
public:

	// IModuleInterface Interface Begin
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	ARMYSLATE_API static FArmySlateModule& Get();

	/*inline TSharedPtr<FArmyModalManager> GetModalManager()
	{
		return ModalManager;
	}*/
	// IModuleInterface Interface End
	TSharedPtr<class FWebImageCache> WebImageCache;

private:
	static FArmySlateModule* Singleton;
	//TSharedPtr<FArmyModalManager> ModalManager = NULL;
};
