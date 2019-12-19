// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "XRStaticLightingModule.h"


IMPLEMENT_MODULE(FArmyStaticLightingModule, XRStaticLighting)

FArmyStaticLightingModule* FArmyStaticLightingModule::Singleton = nullptr;

void FArmyStaticLightingModule::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FArmyStaticLightingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



