#pragma once

#include "CameraPhotographyModule.h"
#include "Camera/CameraPhotography.h"
#include "Camera/CameraTypes.h"

class FArmyRipperCameraPhotographyPrivate : public ICameraPhotography
{
public:
	FArmyRipperCameraPhotographyPrivate();
	virtual ~FArmyRipperCameraPhotographyPrivate() override;
	virtual bool UpdateCamera(FMinimalViewInfo& InOutPOV, APlayerCameraManager* PCMgr) override;
	virtual bool IsSupported() override;
	virtual const TCHAR* const GetProviderName() override { return TEXT("NVIDIA MyAnsel"); };

private:

	bool bAnselSessionActive;
	bool bAnselSessionNewlyActive;
	bool bAnselSessionWantDeactivate;
	bool bAnselCaptureActive;
	bool bAnselCaptureNewlyActive;
	bool bAnselCaptureNewlyFinished;

	bool bForceDisallow;
	bool bIsOrthoProjection;

	bool bWasMovableCameraBeforeSession;
	bool bWasPausedBeforeSession;
	bool bWasShowingHUDBeforeSession;
	bool bWereSubtitlesEnabledBeforeSession;
	bool bWasFadingEnabledBeforeSession;

	bool bAutoPostprocess;
	bool bAutoPause;
	
	int32 ScreenShortCount;
	FMinimalViewInfo POV;

	//4倍
	int32 RenderType = 4;
};