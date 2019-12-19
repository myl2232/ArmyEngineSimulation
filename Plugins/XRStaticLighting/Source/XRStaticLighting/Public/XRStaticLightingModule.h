// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"


/**
 * The public interface to this module
 */
class FArmyStaticLightingModule : public IModuleInterface
{

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	XRSTATICLIGHTING_API static inline FArmyStaticLightingModule& Get()
	{
		if (Singleton == NULL)
		{
			check(IsInGameThread());
			FModuleManager::LoadModuleChecked<FArmyStaticLightingModule>("ArmyStaticLighting");
		}
		check(Singleton != NULL);

		return *Singleton;
	}

private:
	static FArmyStaticLightingModule* Singleton;
};
