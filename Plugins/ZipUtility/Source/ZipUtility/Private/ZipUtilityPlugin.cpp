// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ZipUtilityPlugin.h"
#include "ZipUtilityPrivatePCH.h"
#include "ZipFileFunctionLibrary.h"
#define LOCTEXT_NAMESPACE "FZipUtilityModule"

void FZipUtilityModule::PostLoadCallback()
{	
}

void FZipUtilityModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FZipFileFunctionLibrary::Startup();
}

void FZipUtilityModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FZipFileFunctionLibrary::ShutDown();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FZipUtilityModule, ZipUtility)