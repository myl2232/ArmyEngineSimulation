// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class SCTBaseToolModule : public IModuleInterface
{
	virtual void StartupModule();

	virtual void ShutdownModule();
};