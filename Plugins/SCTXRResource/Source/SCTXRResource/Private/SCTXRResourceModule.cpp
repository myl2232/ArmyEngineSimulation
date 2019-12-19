// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SCTXRResourceModule.h"
#include "SlateOptMacros.h"
#include "SCTResManager.h"

DEFINE_LOG_CATEGORY(ResLog);


void FSCTXRResourceModule::StartupModule()
{
	FSCTXRResourceManagerInstatnce::GetIns().StartUp();
}

void FSCTXRResourceModule::ShutdownModule()
{
	FSCTXRResourceManagerInstatnce::GetIns().ShutDown();
}

IMPLEMENT_MODULE(FSCTXRResourceModule, SCTXRResource);
