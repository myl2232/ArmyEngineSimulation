// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ArmySettings.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/IConsoleManager.h"
#include "GenericPlatform/GenericApplication.h"
#include "Misc/App.h"
#include "EngineGlobals.h"

FString  GXRSettingsIni = TEXT("");
UXRSettings* GXRSettings = NULL;
UXRSettings::UXRSettings(const FObjectInitializer& ObjectInitializer)
:	Super(ObjectInitializer), SizeDimensions_FontSize(12), Annotation_FontSize(12), XRScreenshotResolutionX(3840)
{
	LoadConfigIni();
	GXRSettings = this;
}
UXRSettings* UXRSettings::GetXRSettings()
{
	if (GXRSettings == NULL)
	{
		UXRSettings::LoadConfigIni();
		GXRSettings = NewObject<UXRSettings>();
	}
	return GXRSettings;
}
void UXRSettings::SaveSettings()
{
	SaveConfig(CPF_Config, *GXRSettingsIni);
}

void UXRSettings::LoadConfigIni( bool bForceReload/*=false*/ )
{
	FConfigCacheIni::LoadGlobalIniFile(GXRSettingsIni, TEXT("ArmySettings"), NULL, bForceReload);
}
int32 UXRSettings::GetSizeDimensionsFontSize()
{
	return SizeDimensions_FontSize;
}
void UXRSettings::SetSizeDimensionsFontSize(int32 InValue)
{
	SizeDimensions_FontSize = InValue;
}
int32 UXRSettings::GetAnnotationFontSize()
{
	return Annotation_FontSize;
}
void UXRSettings::SetAnnotationFontSize(int32 InValue)
{
	Annotation_FontSize = InValue;
}
void UXRSettings::SetScreenshotResolutionX(int32 InValue)
{
	XRScreenshotResolutionX = InValue;
}
int32 UXRSettings::GetScreenshotResolutionX()
{
	return XRScreenshotResolutionX;
}