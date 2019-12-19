// Fill out your copyright notice in the Description page of Project Settings.

#include "AutoDesignModule.h"
#include "Modules/ModuleManager.h"
#include "Core.h"
 

IMPLEMENT_MODULE(AutoDesignModule, AutoDesignModule);
void AutoDesignModule::StartupModule()
{


	FString BaseDir = FPaths::Combine(*FPaths::ProjectPluginsDir(), TEXT("AutoDesignModule"));
#if UE_BUILD_SHIPPING || UE_BUILD_DEVELOPMENT
	FString LibPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/AutoDesign/bin/Release/"));
#else
	FString LibPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/AutoDesign/bin/Debug/"));
#endif
	FPlatformProcess::PushDllDirectory(*LibPath);
#if UE_BUILD_SHIPPING || UE_BUILD_DEVELOPMENT
	FString DllPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/AutoDesign/bin/Release/AIDesign.dll"));
#else
	FString DllPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/AutoDesign/bin/Debug/AIDesign.dll"));
#endif
	LibraryHandle = FPlatformProcess::GetDllHandle(*DllPath);
	FPlatformProcess::PopDllDirectory(*LibPath);
}

void AutoDesignModule::ShutdownModule()
{
	FPlatformProcess::FreeDllHandle(LibraryHandle);
	LibraryHandle = nullptr;
}
