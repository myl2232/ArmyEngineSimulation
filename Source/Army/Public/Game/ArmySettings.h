// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "GenericPlatform/GenericWindow.h"
#include "Scalability.h"
#include "ArmySettings.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnXRSettingsUINeedsUpdate);

extern XR_API FString GXRSettingsIni;
/**
 * Stores user settings for a game (for example graphics and sound settings), with the ability to save and load to and from a file.
 */
UCLASS(config=XRSettings, configdonotcheckdefaults)
class XR_API UXRSettings : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/** Save the user settings to persistent storage (automatically happens as part of ApplySettings) */
	UFUNCTION(BlueprintCallable, Category=Settings)
	virtual void SaveSettings();

	static UXRSettings* GetXRSettings();

	/** Loads the user .ini settings into GConfig */
	static void LoadConfigIni(bool bForceReload = false);

	UFUNCTION(BlueprintCallable, Category = Settings)
	virtual int32 GetSizeDimensionsFontSize();

	UFUNCTION(BlueprintCallable, Category = Settings)
	virtual void SetSizeDimensionsFontSize(int32 InValue);

	UFUNCTION(BlueprintCallable, Category = Settings)
	virtual int32 GetAnnotationFontSize();

	UFUNCTION(BlueprintCallable, Category = Settings)
	virtual void SetAnnotationFontSize(int32 InValue);

	UFUNCTION(BlueprintCallable, Category = Settings)
	virtual void SetScreenshotResolutionX(int32 InValue);

	UFUNCTION(BlueprintCallable, Category = Settings)
	virtual int32 GetScreenshotResolutionX();
protected:
	UPROPERTY(config)
	int32 SizeDimensions_FontSize;

	UPROPERTY(config)
	int32 Annotation_FontSize;

	UPROPERTY(config)
	int32 XRScreenshotResolutionX;
};
extern UXRSettings* GXRSettings;