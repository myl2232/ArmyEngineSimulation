// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArmyResourceModule.h"
#include "SlateOptMacros.h"

DEFINE_LOG_CATEGORY(ResLog);

FArmyResourceModule* FArmyResourceModule::Singleton = NULL;

void FArmyResourceModule::StartupModule()
{
	Singleton = this;
	if (!ResourceManager)
	{
		ResourceManager = NewObject<UArmyResourceManager>();
		ResourceManager->AddToRoot();
		ResourceManager->Init();
	}
}

void FArmyResourceModule::ShutdownModule()
{
// 	if (ResourceManager)
// 	{
// 		ResourceManager->RemoveFromRoot();
// 	}
}

ARMYRESOURCE_API FArmyResourceModule& FArmyResourceModule::Get()
{
	if (Singleton == NULL)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FArmyResourceModule>("ArmyResource");
	}
	check(Singleton != NULL);
	return *Singleton;
}

IMPLEMENT_MODULE(FArmyResourceModule, ArmyResource);
