#include "UEGTEngine.h"
#include "Core.h"
#include "ModuleManager.h"




void FUEGTEngineModule::StartupModule()
{

	FString BaseDir = FPaths::Combine(*FPaths::ProjectPluginsDir(), TEXT("SCTTess"));
#if UE_BUILD_SHIPPING || UE_BUILD_DEVELOPMENT
	FString LibPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/GTEngineForUE4/bin/Release/"));
#else
	FString LibPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/GTEngineForUE4/bin/Debug/"));
#endif
	FPlatformProcess::PushDllDirectory(*LibPath);
#if UE_BUILD_SHIPPING || UE_BUILD_DEVELOPMENT
	FString DllPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/GTEngineForUE4/bin/Release/GTEngineForUE4.dll"));
#else
	FString DllPath = FPaths::Combine(*BaseDir, TEXT("ThirdParty/GTEngineForUE4/bin/Debug/GTEngineForUE4.dll"));
#endif
	LibraryHandle = FPlatformProcess::GetDllHandle(*DllPath);
	FPlatformProcess::PopDllDirectory(*LibPath);
}

void FUEGTEngineModule::ShutdownModule()
{
	FPlatformProcess::FreeDllHandle(LibraryHandle);
	LibraryHandle = nullptr;
}

IMPLEMENT_MODULE(FUEGTEngineModule, UEGTEngine)
