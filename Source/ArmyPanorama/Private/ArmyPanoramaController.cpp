#include "ArmyPanoramaController.h"
#include "ArmyPanoramaMgr.h"
#include "ArmySlateModule.h"
#include "ArmyModalManager.h"
#include "ArmyViewportClient.h"
#include "Engine/PostProcessVolume.h"

//TSharedPtr<FArmyPanoramaController> FArmyPanoramaController::MyPanoramaController = nullptr;

FArmyPanoramaController::FArmyPanoramaController()
{
	bScreenshot = false;
	PanoramaMapIndex = 0;
	LastRenderingFrameCount = 0;
	bProcessingScreenshot = false;
	PPV = NULL;
}

FArmyPanoramaController::~FArmyPanoramaController()
{
}
//
//FArmyPanoramaController& FArmyPanoramaController::Get()
//{
//	if (!MyPanoramaController.IsValid())
//	{
//		MyPanoramaController = MakeShareable(new FArmyPanoramaController());
//	}
//	return *MyPanoramaController;
//}

void FArmyPanoramaController::StartPanorama(UArmyViewportClient* MyGVC)
{
	CaptureLocation = MyGVC->GetViewLocation();
	CaptureRotation = MyGVC->GetViewRotation();
    CaptureRotation.Pitch = 0.f;
    CaptureRotation.Roll = 0.f;
	ViewportClient = MyGVC;
	CurWorld = MyGVC->GetWorld();
	bScreenshot = true;
}

void FArmyPanoramaController::Tick(float DeltaTime)
{
	if (bScreenshot)
	{
		//六张全部截取完成
		if (PanoramaMapIndex > 5)
		{
			//ResetAutoExposure();
			if (GetPanoramaDelegate.IsBound())
			{
				GetPanoramaDelegate.Execute(VerticalSixFacesData);
				bScreenshot = false;
				PanoramaMapIndex = 0;
				LastRenderingFrameCount = 0;
				VerticalSixFacesData.Reset();
			}
		}
		else
		{
			if (LastRenderingFrameCount == 0)
			{
				//计算视角
				FRotator MapRotation;

				if (PanoramaMapIndex == 0)
				{
					MapRotation = FRotator(0, -90, 0);
					//SetFastAutoExposure();
				}
                else if (PanoramaMapIndex == 1)
                    MapRotation = FRotator(0, 0, 0);
                else if (PanoramaMapIndex == 2)
                    MapRotation = FRotator(0, 90, 0);
                else if (PanoramaMapIndex == 3)
                    MapRotation = FRotator(0, 180, 0);
                else if (PanoramaMapIndex == 4)
                    MapRotation = FRotator(90, 0, 0);
                else if (PanoramaMapIndex == 5)
                    MapRotation = FRotator(-90, 0, 0);

				//更新视角
				ViewportClient->SetViewLocation(CaptureLocation);
				ViewportClient->SetViewRotation(CaptureRotation + MapRotation);
			}

			LastRenderingFrameCount++;

			//请求截图
			if (LastRenderingFrameCount == CaptureInterval)
			{
				bProcessingScreenshot = true;
				FString CommandString = FString::Printf(TEXT("HighResShot %dx%d"), ResolutionWidth, ResolutionWidth);
				CurWorld->GetGameViewport()->ConsoleCommand(*CommandString);
			}
		}
	}
}

void FArmyPanoramaController::SetFastAutoExposure()
{
	if (!PPV)
	{
		for (TActorIterator<AActor> ActorItr(GWorld); ActorItr; ++ActorItr)
		{
			FString ActorName = (*ActorItr)->GetName();
			if (ActorName == "PPV_StaticLighting")
				PPV = Cast<APostProcessVolume>(*ActorItr);
		}
	}

	if (PPV)
	{
		OldExposureMin = PPV->Settings.AutoExposureMinBrightness;
		OldExposureMax = PPV->Settings.AutoExposureMaxBrightness;
		OldExposureSpeedUp = PPV->Settings.AutoExposureSpeedUp;
		OldExposureSpeedDown = PPV->Settings.AutoExposureSpeedDown;

		PPV->Settings.bOverride_AutoExposureMinBrightness = true;
		PPV->Settings.bOverride_AutoExposureMaxBrightness = true;
		PPV->Settings.bOverride_AutoExposureSpeedUp = true;
		PPV->Settings.bOverride_AutoExposureSpeedDown = true;

		PPV->Settings.AutoExposureMinBrightness = 0.2f;
		PPV->Settings.AutoExposureMaxBrightness = 0.2f;
		PPV->Settings.AutoExposureSpeedUp = 3.f;
		PPV->Settings.AutoExposureSpeedDown = 3.f;
	}
}
 
void FArmyPanoramaController::ResetAutoExposure()
{
	if (PPV)
	{
		PPV->Settings.AutoExposureMinBrightness = OldExposureMin;
		PPV->Settings.AutoExposureMaxBrightness = OldExposureMax;
		PPV->Settings.AutoExposureSpeedUp = OldExposureSpeedUp;
		PPV->Settings.AutoExposureSpeedDown = OldExposureSpeedDown;
	}
}

void FArmyPanoramaController::ProcessScreenShots(FViewport* InViewport)
{
	if (bProcessingScreenshot)
	{
		TArray<FColor> RawData;
		int32 X = InViewport->GetSizeXY().X;
		int32 Y = InViewport->GetSizeXY().Y;
		FIntRect InRect(0, 0, X, Y);
		GetViewportScreenShot(InViewport, RawData, InRect);
		PanoramaMapIndex++;
		TArray<uint32> Data;
		Data.AddDefaulted(X * Y);
		FMemory::Memcpy(Data.GetData(), (uint32*)(RawData.GetData()), RawData.Num() * sizeof(FColor));
		VerticalSixFacesData.Append(Data);
		bProcessingScreenshot = false;
		LastRenderingFrameCount = 0;
	}
}

bool FArmyPanoramaController::IsTickable() const
{
	return true;
}

bool FArmyPanoramaController::IsTickableWhenPaused() const
{
    return true;
}

TStatId FArmyPanoramaController::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FArmyPanoramaController, STATGROUP_Tickables);
}
