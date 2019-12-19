#pragma once

#include "ArmyCameraParamModule.h"

IMPLEMENT_MODULE(FArmyCameraParamModule, ArmyCameraParam);

bool bRipperIsStartScreenshot = false;
bool bRipperIsScreenshot = false;
bool bRipperIsExportJpg = false;
FVector CameraLocation = FVector::ZeroVector;
FRotator CameraRotation = FRotator::ZeroRotator;

TSharedPtr<ICameraPhotography > FArmyCameraParamModule::CreateCameraPhotography()
{
	TSharedPtr<ICameraPhotography> Photography = nullptr;

	FArmyRipperCameraPhotographyPrivate* PhotographyPrivate = nullptr;
	PhotographyPrivate = new FArmyRipperCameraPhotographyPrivate();
	if (PhotographyPrivate->IsSupported())
	{
		Photography = TSharedPtr<ICameraPhotography>(PhotographyPrivate);
	}
	else
	{
		delete PhotographyPrivate;
	}

	return Photography;
}

ARMYCAMERAPARAM_API void FArmyCameraParamModule::StartCamera(bool BShutDown)
{
	if (BShutDown)
		bRipperIsStartScreenshot = true;
	else
		bRipperIsStartScreenshot = false;
}


ARMYCAMERAPARAM_API void FArmyCameraParamModule::SetCameraLocationAndRotation(FVector Location, FRotator Rotation)
{
	CameraLocation = Location;
	CameraRotation = Rotation;
}

void FArmyCameraParamModule::GetScreenData(bool BEnd)
{
	StartDataDelegate.ExecuteIfBound(BEnd);
}