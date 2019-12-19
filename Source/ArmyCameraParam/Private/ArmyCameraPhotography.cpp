#pragma once

#include "ArmyCameraPhotography.h"
#include "ArmyCameraParamModule.h"
#include "Kismet/GameplayStatics.h"
#include "RenderingThread.h"
#include "SXRRenderingImage.h"
#include "ArmyViewportClient.h"

extern bool bRipperIsStartScreenshot;
extern bool bRipperIsExportJpg;
extern FVector CameraLocation;
extern FRotator CameraRotation;

FArmyRipperCameraPhotographyPrivate::FArmyRipperCameraPhotographyPrivate()
	: ICameraPhotography()
	, bAnselSessionActive(false)
	, bAnselSessionNewlyActive(false)
	, bAnselSessionWantDeactivate(false)
	, bAnselCaptureActive(false)
	, bAnselCaptureNewlyActive(false)
	, bAnselCaptureNewlyFinished(false)
	, bForceDisallow(false)
	, bIsOrthoProjection(false)
{
	ScreenShortCount = 0;
	//TmpPOV = InOutPOV;
	POV.FOV = 28.0724869f;
	POV.OffCenterProjectionOffset.X = -3;
	POV.OffCenterProjectionOffset.Y = 3;
}

FArmyRipperCameraPhotographyPrivate::~FArmyRipperCameraPhotographyPrivate()
{

}

bool FArmyRipperCameraPhotographyPrivate::IsSupported()
{
	return true;// bAnselDLLLoaded && ansel::isAnselAvailable();
}

bool FArmyRipperCameraPhotographyPrivate::UpdateCamera(FMinimalViewInfo& InOutPOV, APlayerCameraManager* PCMgr)
{
	//return false;
	check(PCMgr != nullptr);
	bool bGameCameraCutThisFrame = false;

	if (bRipperIsStartScreenshot)
	{
		//调用了 bUserPlayerControllerView = true，所以运行的是GameViewportClient，但是之前一直运行的是XREditorViewportClient，所以需要先赋值
		InOutPOV.Location = CameraLocation;
		InOutPOV.Rotation = CameraRotation;

		bGameCameraCutThisFrame = true;
		APlayerController* PCOwner = PCMgr->GetOwningPlayerController();
		ScreenShortCount++;

		if (ScreenShortCount == 1)
		{
			POV = InOutPOV;
			POV.FOV = 28.0724869f;

			PCMgr->OnPhotographyMultiPartCaptureStart();

			PCOwner->SetPause(true);
			UGameplayStatics::SetSubtitlesEnabled(false);
			PCMgr->bEnableFading = false;
		}

		InOutPOV = POV;
		FlushRenderingCommands();

		int32 CountMap = (2 * RenderType - 1)*(2 * RenderType - 1);
		int32 Index = ScreenShortCount / 10;
		if (Index > CountMap - 1)
		{
			FArmyCameraParamModule::Get().GetScreenData(false);
			ScreenShortCount = 0;
			PCMgr->OnPhotographyMultiPartCaptureEnd();
			bRipperIsStartScreenshot = false;
			PCOwner->SetPause(false);
		}
		else
		{
			int32 X = Index % (2 * RenderType - 1) - (RenderType - 1);
			int32 Y = (RenderType - 1) - Index / (2 * RenderType - 1);

			POV.OffCenterProjectionOffset.X = X;
			POV.OffCenterProjectionOffset.Y = Y;

			if (ScreenShortCount % 10 == 9)
			{
				FArmyCameraParamModule::Get().GetScreenData(true);

				//GAnselFilePath = FPaths::GameSavedDir() + FString::Printf(TEXT("%d.jpg"), Index);
			}
		}

		{
			InOutPOV.PostProcessSettings.bOverride_BloomDirtMaskIntensity = 1;
			InOutPOV.PostProcessSettings.BloomDirtMaskIntensity = 0.f;
			InOutPOV.PostProcessSettings.bOverride_LensFlareIntensity = 1;
			InOutPOV.PostProcessSettings.LensFlareIntensity = 0.f;
			InOutPOV.PostProcessSettings.bOverride_VignetteIntensity = 1;
			InOutPOV.PostProcessSettings.VignetteIntensity = 0.f;
			InOutPOV.PostProcessSettings.bOverride_SceneFringeIntensity = 1;
			InOutPOV.PostProcessSettings.SceneFringeIntensity = 0.f;

			// motion blur doesn't make sense with a teleporting camera
			InOutPOV.PostProcessSettings.bOverride_MotionBlurAmount = 1;
			InOutPOV.PostProcessSettings.MotionBlurAmount = 0.f;

			// DoF can look poor/wrong at high-res, depending on settings.
			InOutPOV.PostProcessSettings.bOverride_DepthOfFieldScale = 1;
			InOutPOV.PostProcessSettings.DepthOfFieldScale = 0.f; // BokehDOF
			InOutPOV.PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = 1;
			InOutPOV.PostProcessSettings.DepthOfFieldNearBlurSize = 0.f; // GaussianDOF
			InOutPOV.PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = 1;
			InOutPOV.PostProcessSettings.DepthOfFieldFarBlurSize = 0.f; // GaussianDOF
			InOutPOV.PostProcessSettings.bOverride_DepthOfFieldDepthBlurRadius = 1;
			InOutPOV.PostProcessSettings.DepthOfFieldDepthBlurRadius = 0.f; // CircleDOF
			InOutPOV.PostProcessSettings.bOverride_DepthOfFieldVignetteSize = 1;
			InOutPOV.PostProcessSettings.DepthOfFieldVignetteSize = 200.f; // Scene.h says 200.0 means 'no effect'

																		   // freeze auto-exposure adaptation
			InOutPOV.PostProcessSettings.bOverride_AutoExposureSpeedDown = 1;
			InOutPOV.PostProcessSettings.AutoExposureSpeedDown = 0.f;
			InOutPOV.PostProcessSettings.bOverride_AutoExposureSpeedUp = 1;
			InOutPOV.PostProcessSettings.AutoExposureSpeedUp = 0.f;


			InOutPOV.PostProcessSettings.bOverride_ScreenSpaceReflectionIntensity = 1;
			InOutPOV.PostProcessSettings.ScreenSpaceReflectionIntensity = 0.f;
		}
	}

	return bGameCameraCutThisFrame;
}