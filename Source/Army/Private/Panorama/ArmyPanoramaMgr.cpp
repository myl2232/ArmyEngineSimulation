// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyPanoramaMgr.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Runtime/Engine/Public/ImageUtils.h"
#include "ArmyUploadPanoramaItem.h"
#include "ArmyEngineTools.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyUserModule.h"
#include "ArmyUser.h"
#include "ArmyViewportClient.h"
#include "ArmyPanoramaModule.h"
#include "ArmyResourceModule.h"
#include "ArmyRoom.h"
#include "ArmySceneData.h"
#include "ArmyGameInstance.h"
#include "ArmySingleDoor.h"
#include "ArmyPass.h"
#include "ArmySlidingDoor.h"
#include "ArmyPlayerController.h"
#include "ArmyEditorEngine.h"
#include "ArmyActorVisitorFactory.h"
#include "IArmyHttp.h"
#include "ArmyHttpModule.h"

TSharedPtr<FArmyPanoramaMgr> FArmyPanoramaMgr::XRPanoramaMgr = nullptr;

FArmyPanoramaMgr& FArmyPanoramaMgr::Get()
{
	if (!XRPanoramaMgr.IsValid())
	{
		XRPanoramaMgr = MakeShareable(new FArmyPanoramaMgr());
	}
	return *XRPanoramaMgr;
}

FArmyPanoramaMgr::FArmyPanoramaMgr()
{
	bBusy = false;
	bAddToList = false;
	bMultiple = false;
}

void FArmyPanoramaMgr::StartUploadSingle()
{
	bMultiple = false;

	GGI->Window->ShowThrobber(MAKE_TEXT("全景图生成进度80.00%"), FSimpleDelegate::CreateRaw(this, &FArmyPanoramaMgr::CancelRequestIfRequest));

	dir = FString::Printf(TEXT("pano/%d_%s"), FArmyUser::Get().GetCurPlanID(), *FArmyEngineTools::GetTimeStampString());
	// 上传全景图
	GVC->GetWorld()->GetTimerManager().ClearTimer(TH_CheckUploadingList);
	GVC->GetWorld()->GetTimerManager().SetTimer(TH_CheckUploadingList, FTimerDelegate::CreateRaw(this, &FArmyPanoramaMgr::TickUploadItems), 0.3f, true);
}

void FArmyPanoramaMgr::FillnavMapPicData()
{
	//转到平面图下截取导航图
	GXRPC->SetXRViewMode(EXRView_TOP);
	//隐藏家具
    TSharedPtr<IArmyActorVisitor> FurnitureAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_Model);
    FurnitureAV->Show(false);
	//隐藏比例尺和坐标轴
	GVC->bDrawAxes = false;

	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, RoomList);
	TSharedPtr<FArmyRoom> Room;
	for (FObjectWeakPtr It : RoomList)
	{
		Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
	}
	FBox box(Room->GetBounds());
	float OrthZoom = FMath::Max(box.GetSize().X, box.GetSize().Y);

	FVector2D tempViewPortSize;
	GVC->GetViewportSize(tempViewPortSize);
	GVC->SetViewLocation(Room.Get()->GetBasePos());

	//获取外墙的屏幕坐标
	float X1 = box.Min.X;
	float X2 = box.Max.X;
	float Y1 = box.Min.Y;
	float Y2 = box.Max.Y;
	OutRoom2D = FVector2D(FMath::Abs(X1 - X2), FMath::Abs(Y1 - Y2));

	if (OutRoom2D.X > OutRoom2D.Y)
	{
		OrthZoom = OrthZoom* FMath::Max(tempViewPortSize.X, tempViewPortSize.Y) / 1920;
	}
	else
	{
		OrthZoom = OrthZoom* FMath::Max(tempViewPortSize.X, tempViewPortSize.Y) / 2160;
	}

	if (OrthZoom != 0.0f)
	{
		GVC->SetOrthoZoom(OrthZoom);
	}

	//绑定截图代理
	GVC->ScreenShotFun = ScreenShotDelegate::CreateRaw(this, &FArmyPanoramaMgr::OnScreenShot);

	//截图
	FString CommandString = FString::Printf(TEXT("HighResShot %dx%d"), (int32)OutRoom2D.X, (int32)OutRoom2D.Y);
	GVC->GetWorld()->GetGameViewport()->ConsoleCommand(*CommandString);
}

void FArmyPanoramaMgr::OnScreenShot(FViewport * InViewport)
{
	TArray<FColor> RawData;
	int32 X = InViewport->GetSizeXY().X;
	int32 Y = InViewport->GetSizeXY().Y;
	FIntRect InRect(0, 0, X, Y);
	GetViewportScreenShot(InViewport, RawData, InRect);

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	ImageWrapper->SetRaw(RawData.GetData(), (int32)OutRoom2D.X * (int32)OutRoom2D.Y * sizeof(FColor), (int32)OutRoom2D.X, (int32)OutRoom2D.Y, ERGBFormat::BGRA, 8);

	//截图完成后解绑代理
	GVC->ScreenShotFun.Unbind();

	//显示家具
    TSharedPtr<IArmyActorVisitor> FurnitureAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_Model);
    FurnitureAV->Show(true);

	//显示比例尺和坐标轴
	GVC->bDrawAxes = true;

	//回到透视图
	GXRPC->SetXRViewMode(EXRView_FPS);

	//将导航图数据填充到上传列表中上传到oss服务器，上传列表的第一个就是导航图
	TSharedPtr<FArmyPanoramaUploadItem> UploadItem = MakeShareable(new FArmyPanoramaUploadItem(ImageWrapper->GetCompressed(), FString("navMap.jpg")));
	UploadingList.Add(UploadItem);

	//上传导航图和全景图到oss服务器
	GVC->GetWorld()->GetTimerManager().ClearTimer(TH_CheckUploadingList);
	GVC->GetWorld()->GetTimerManager().SetTimer(TH_CheckUploadingList, FTimerDelegate::CreateRaw(this, &FArmyPanoramaMgr::TickUploadItems), 0.3f, true);
}

bool FArmyPanoramaMgr::IsTickable() const
{
	return true;

}

bool FArmyPanoramaMgr::IsTickableWhenPaused() const
{
	return true;

}

TStatId FArmyPanoramaMgr::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FArmyPanoramaMgr, STATGROUP_Tickables);

}

void FArmyPanoramaMgr::Tick(float DeltaTime)
{
}

void FArmyPanoramaMgr::SetActorList(TArray<FActorInfo>& InActorArray)
{
	ActorArray = InActorArray;
}

void FArmyPanoramaMgr::GenerateSingleAndUpload(UArmyViewportClient* MyGVC, ConfigOfPanorama ConfigOfPanorama)
{
	ClearAll();

	MyConfigOfPanorama = ConfigOfPanorama;
	ViewportClient = MyGVC;
	CurWorld = MyGVC->GetWorld();
	CaptureLocation = MyGVC->GetViewLocation();
	CaptureRotation = MyGVC->GetViewRotation();
    CaptureRotation.Pitch = 0.f;
    CaptureRotation.Roll = 0.f;
	FileNamePreFix = "Pano_" + FArmyEngineTools::GetTimeStampString() + ".jpg";
	SavePath = "D:/" + FileNamePreFix;

	//是否在房间内
	if (GetCurrentRoom().IsValid())
	{
		SingleRoomName = GetCurrentRoom()->GetSpaceName();
	}
	else
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("当前不在房间内"));
		return;
	}

	//取消选中
	GXREditor->SelectNone(true, true, false);

	//提示
	GGI->Window->ShowThrobber(MAKE_TEXT("全景图生成进度0.00%"), FSimpleDelegate::CreateRaw(this, &FArmyPanoramaMgr::ClearAll));//清空全部，不只是停止发送请求

	for (TActorIterator<AActor> ActorItr(CurWorld); ActorItr; ++ActorItr)
	{
		APostProcessVolume* PPV = Cast<APostProcessVolume>(*ActorItr);
		if (PPV)
		{
			PPV->Settings.bOverride_VignetteIntensity = true;
			PPV->Settings.VignetteIntensity = 0.f;
		}
	}

	WorkType = E_GenerateSingleAndUpload;
	WorkState = E_Rendering;
	GVC->GetWorld()->GetGameViewport()->ConsoleCommand("showflag.ScreenSpaceReflections 0");

	//获取截图数据
	FArmyPanoramaModule::Get().GetPanoramaController()->GetPanoramaDelegate.BindRaw(this, &FArmyPanoramaMgr::GetPanorama);
	FArmyPanoramaModule::Get().GetPanoramaController()->StartPanorama(MyGVC);
}

void FArmyPanoramaMgr::GenerateMultipleAndUpload(ConfigOfPanorama ConfigOfPanorama)
{	
	ClearAll();

	MyConfigOfPanorama = ConfigOfPanorama;

	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	if (RoomList.Num() == 0)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请先绘制墙线"));
		return;
	}

	//取消选中
	GXREditor->SelectNone(true, true, false);

	TotalScreenShotIamge = RoomList.Num();
	for (FObjectWeakPtr It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		RoomCenterList.Add(Room->GetBounds3D().GetCenter());
	}

	WorkType = E_GenerateMulipleAndUpload;
	WorkState = E_Rendering;
	bMultiple = true;
	GVC->GetWorld()->GetGameViewport()->ConsoleCommand("showflag.ScreenSpaceReflections 0");

	GGI->Window->ShowThrobber(MAKE_TEXT("全景图生成进度0.00%"), FSimpleDelegate::CreateRaw(this, &FArmyPanoramaMgr::ClearAll));//清空全部，不只是停止发送请求

	//截图
	GVC->GetWorld()->GetTimerManager().ClearTimer(TH_CheckIsScreenshot);
	GVC->GetWorld()->GetTimerManager().SetTimer(TH_CheckIsScreenshot, FTimerDelegate::CreateRaw(this, &FArmyPanoramaMgr::TickScreenshot), 0.3f, true);
}

void FArmyPanoramaMgr::TickScreenshot()
{
	if (!bBusy)
	{
		if (CurrentScreenShotImage < TotalScreenShotIamge)
		{
			bBusy = true;
			GVC->SetViewLocation(RoomCenterList[CurrentScreenShotImage]);
			GVC->SetViewRotation(FRotator(0, -90, 0));
			//获取截图数据
			FArmyPanoramaModule::Get().GetPanoramaController()->GetPanoramaDelegate.BindRaw(this, &FArmyPanoramaMgr::GetPanorama);
			FArmyPanoramaModule::Get().GetPanoramaController()->StartPanorama(GVC);

			//生成进度提示
			GGI->Window->ShowThrobber(MAKE_TEXT("全景图生成进度5.00%"), FSimpleDelegate::CreateRaw(this, &FArmyPanoramaMgr::CancelRequestIfRequest));
			Progress = (float)CurrentScreenShotImage / (float)TotalScreenShotIamge * 100;
			TAttribute<FText> ProgressTextAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FArmyPanoramaMgr::GetProgressText));
			GGI->Window->ShowThrobber(ProgressTextAttr, FSimpleDelegate::CreateRaw(this, &FArmyPanoramaMgr::ClearAll));//清空全部，不只是停止发送请求

			CurrentScreenShotImage++;
		}
	}
	else
		return;
}

void FArmyPanoramaMgr::OnePanoramaFinished()
{
	//本地单张截取完成后，直接保存到本地，置为空闲状态
	if (WorkType == EWorkType::E_GenerateSingleLocal)
	{
		WorkState = EWorkState::E_NoneState;

		TSharedPtr<IImageWrapper> ImageWrapper;
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		ImageWrapper->SetRaw(VerticalSixFacesData.GetData(), ResolutionWidth * ResolutionWidth * sizeof(FColor) * 6, ResolutionWidth, ResolutionWidth * 6, ERGBFormat::BGRA, 8);
	}
	else if (WorkType == EWorkType::E_GenerateSingleAndUpload)
	{
		WorldPanoData.HotPointSingle = CreateHotPoint(CaptureLocation);

		WorldPanoData.HotPointSingle.PanoImageData.ImageState = FPanoData::Generated;

        // 压缩全景图截图
        TSharedPtr<IImageWrapper> ImageWrapper;
        IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
        ImageWrapper->SetRaw(VerticalSixFacesData.GetData(), ResolutionWidth * ResolutionWidth * sizeof(FColor) * 6, ResolutionWidth, ResolutionWidth * 6, ERGBFormat::BGRA, 8);
		ImageData = ImageWrapper->GetCompressed(PanoramaCompressQuality);
 		//FString DesignSketchFilePath = "D:/" + FArmyEngineTools::GetTimeStampString() + ".jpg";
 		//FFileHelper::SaveArrayToFile(ImageData, *DesignSketchFilePath);

		GVC->GetWorld()->GetGameViewport()->ConsoleCommand("showflag.ScreenSpaceReflections 1");
		WorkState = EWorkState::E_Uploading;
		StartUploadSingle();
	}
	else if (WorkType == EWorkType::E_GenerateMulipleAndUpload)
	{
		WorkState = EWorkState::E_NoneState;

		FileNamePreFix = "MuliplePano_" + FArmyEngineTools::GetTimeStampString() + ".jpg";
		FileName.Add(FileNamePreFix);
		SavePath = "D:/" + FileNamePreFix;

		TSharedPtr<IImageWrapper> ImageWrapper;
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		ImageWrapper->SetRaw(VerticalSixFacesData.GetData(), ResolutionWidth * ResolutionWidth * sizeof(FColor) * 6, ResolutionWidth, ResolutionWidth * 6, ERGBFormat::BGRA, 8);
		Array_ImageData.Add(ImageWrapper->GetCompressed(PanoramaCompressQuality));
		VerticalSixFacesData.Reset();
		if (CurrentScreenShotImage == TotalScreenShotIamge)
		{
			GVC->GetWorld()->GetGameViewport()->ConsoleCommand("showflag.ScreenSpaceReflections 1");
			WorkState = EWorkState::E_Uploading;
			CurrentScreenShotImage = 0;
			GVC->GetWorld()->GetTimerManager().ClearTimer(TH_CheckIsScreenshot);//关闭计时器
			dir = FString::Printf(TEXT("pano/%d_%s"), FArmyUser::Get().GetCurPlanID(), *FArmyEngineTools::GetTimeStampString());
			//全屋全景图，还需要获取导航图的图片
			FillnavMapPicData();
			GGI->Window->ShowThrobber(MAKE_TEXT("全景图生成进度99.99%"), FSimpleDelegate::CreateRaw(this, &FArmyPanoramaMgr::CancelRequestIfRequest));
		}
	}
}

void FArmyPanoramaMgr::TickUploadItems()
{
	if (WorkState != EWorkState::E_Uploading)
	{
		return;
	}

	if (WorkType == EWorkType::E_GenerateSingleAndUpload)
	{
		if (WorldPanoData.HotPointSingle.PanoImageData.ImageState == FPanoData::Generated)
		{
			WorldPanoData.HotPointSingle.PanoImageData.ImageState = FPanoData::Uploading;
			TSharedPtr<FArmyPanoramaUploadItem> UploadItem = MakeShareable(new FArmyPanoramaUploadItem(ImageData, FileNamePreFix));
			UploadingList.Add(UploadItem); 
		}
	}
	else if (WorkType == EWorkType::E_GenerateMulipleAndUpload)
	{
		if (!bAddToList)
		{
			bAddToList = true;
			for (int i = 0; i < TotalScreenShotIamge; i++)
			{
				TSharedPtr<FArmyPanoramaUploadItem> UploadItem = MakeShareable(new FArmyPanoramaUploadItem(Array_ImageData[i], FileName[i]));
				UploadingList.Add(UploadItem);
			}
		}
	}

	if (UploadingList.Num() != 0)
	{	
		//检查上传列表，开始上传
		for (auto& It : UploadingList)
		{
			if (It->UploadState == EUpDownloadState::NotStarted)
			{
				if (UploadingCount < UploadPanoramaMax)
				{
					It->StartUpload(dir);
					UploadingCount++;
				}
			}
			else if (It->UploadState == EUpDownloadState::Failed)
			{
				GVC->GetWorld()->GetTimerManager().ClearTimer(TH_CheckUploadingList);
				ClearAll();
				PanoramaFailedDelegate.ExecuteIfBound(0, TEXT("生成失败")); 
				GGI->Window->ShowMessage(MT_Warning, TEXT("全景图上传失败"));
			}
			else if (It->UploadState == EUpDownloadState::Succeeded)
			{
				UploadingCount--;
				UploadSuccessCount++;
			}
		}
	}

	//上传完成
	if (UploadSuccessCount >= UploadingList.Num() && UploadSuccessCount > 0)
	{
		WorkState = EWorkState::E_NoneState;
		GVC->GetWorld()->GetTimerManager().ClearTimer(TH_CheckUploadingList);
		RequestSubmitPanorama();
	}
}

void FArmyPanoramaMgr::ClearAll()
{
	WorkType = EWorkType::E_NoneType;
	WorkState = EWorkState::E_NoneState;
	ThumbnailPanoIndex = 0;
	UploadingCount = 0;
	UploadSuccessCount = 0;
	Progress = 0.f;
	VerticalSixFacesData.Reset();
	WorldPanoData.ClearData();
	UploadingList.Reset();
	ActorArray.Reset();
	ImageData.Reset();
	dir = "";
	MyConfigOfPanorama = { "105","-1", "" };
	SingleRoomName = "UnNaming";
	SubmitRequest = nullptr;
	RoomCenterList.Reset();
	bBusy = false;
	TotalScreenShotIamge = 0;
	CurrentScreenShotImage = 0;
	Array_ImageData.Reset();
	bAddToList = false;
	bMultiple = false;
	ScreenshotList.Reset();
	Screenshot.SpotList.Reset();
}

void FArmyPanoramaMgr::CancelRequestIfRequest()
{
	if (SubmitRequest.IsValid())
	{
		SubmitRequest->OnProcessRequestComplete().Unbind();
		SubmitRequest->CancelRequest();
		SubmitRequest = nullptr;
	}
}

void FArmyPanoramaMgr::RequestSubmitPanorama()
{
	WorkType = EWorkType::E_NoneType;
	
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);

	if (!bMultiple)
	{
		for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->Tags.Num() > 0 && (ActorItr->Tags[0] == "MoveableMeshActor" || ActorItr->Tags[0] == "EnvironmentAsset"))
			{
				if (IsPointInRoom(ActorItr->GetActorLocation()))
				{
					FVector2D SpotLocation = GetVR360SpotLocation(CaptureLocation, CaptureRotation, ActorItr->GetActorLocation());
					FPanoSpot MyPanoSpot;
					MyPanoSpot.AtH = SpotLocation.X;
					MyPanoSpot.AtV = SpotLocation.Y;
					MyPanoSpot.LinkFileName = FString::FromInt(0);
					MyPanoSpot.SpotType = 2;
					MyPanoSpot.FurId = FString::FromInt(FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(*ActorItr));
					Screenshot.SpotList.Add(MyPanoSpot);
				}
			}
		}

		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue("introduction", MyConfigOfPanorama.Introduction);
		JsonWriter->WriteValue("dir", dir);
		JsonWriter->WriteValue("simple", 1);
		JsonWriter->WriteValue("planId", FArmyUser::Get().GetCurPlanID());
		JsonWriter->WriteValue("validTime", MyConfigOfPanorama.EValidTime);
		JsonWriter->WriteArrayStart(TEXT("scenes"));

		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue("roomId", GetCurrentRoom()->GetUniqueIdOfRoom());
		JsonWriter->WriteValue("pic", FileNamePreFix);
		JsonWriter->WriteValue("title", SingleRoomName);

		JsonWriter->WriteArrayStart(TEXT("hotspots"));
		for (auto It : Screenshot.SpotList)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteValue("ath", It.AtH);
			JsonWriter->WriteValue("atv", It.AtV);
			JsonWriter->WriteValue("type", It.SpotType);
			JsonWriter->WriteValue("linked", It.FurId);

			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}
	else
	{
		//将商品热点填充到房间里
		SetSpotPointToRoom();
		//将导航热点填充到房间里
		SetDoorAndPassToRoom();

		JsonWriter->WriteObjectStart();

		JsonWriter->WriteValue("introduction", MyConfigOfPanorama.Introduction);
		JsonWriter->WriteValue("dir", dir);
		JsonWriter->WriteValue("simple", 0);
		JsonWriter->WriteValue("planId", FArmyUser::Get().GetCurPlanID());
		JsonWriter->WriteValue("validTime", MyConfigOfPanorama.EValidTime);
		JsonWriter->WriteValue("navMapPic", FString("navMap.jpg"));
		JsonWriter->WriteValue("navMapWidth", int32(OutRoom2D.X));
		JsonWriter->WriteValue("navMapHeight", int32(OutRoom2D.Y));
		JsonWriter->WriteArrayStart(TEXT("scenes"));
		for (auto It : ScreenshotList)
		{
			JsonWriter->WriteObjectStart();

			JsonWriter->WriteValue("roomId", It.RoomID);
			JsonWriter->WriteValue("pic", It.FileName);
			JsonWriter->WriteValue("title", It.Title);
			JsonWriter->WriteValue("navMapRatioX", It.navMapRatioX);
			JsonWriter->WriteValue("navMapRatioY", It.navMapRatioY);

			JsonWriter->WriteArrayStart(TEXT("hotspots"));
			for (auto It1 : It.SpotList)
			{
				JsonWriter->WriteObjectStart();

				JsonWriter->WriteValue("ath", It1.AtH);
				JsonWriter->WriteValue("atv", It1.AtV);
				JsonWriter->WriteValue("type", It1.SpotType);
				JsonWriter->WriteValue("linked", It1.FurId);

				JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayEnd();

			JsonWriter->WriteObjectEnd();
		}

		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteObjectEnd();
		JsonWriter->Close();
	}

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyPanoramaMgr::Callback_RequestSubmitPanorama);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/pano/save", CompleteDelegate, JStr);
    Request->ProcessRequest();
}

void FArmyPanoramaMgr::Callback_RequestSubmitPanorama(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JObject = Response.Data->GetObjectField("data");
        if (JObject.IsValid())
        {
            FString shortUrl = JObject->GetStringField("shortUrl");
            FString PanoURL = JObject->GetStringField("panoUrl");
            FString qrCodeUrl = JObject->GetStringField("codeUrl");
            int32 panoId = JObject->GetIntegerField("panoId");

            PanoramaCompletedDelegate.ExecuteIfBound(shortUrl, qrCodeUrl, panoId);
        }
    }
    else
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("全景图生成失败"));
        PanoramaFailedDelegate.ExecuteIfBound(0, TEXT("生成失败"));
    }

	GGI->Window->HideThrobber();
}

void FArmyPanoramaMgr::CalcLevelInfo()
{
	for (auto& It : WorldPanoData.HotPointList)
	{
		int32 iBuilding, iLevel, iRoom, iRoomBox;
		bool result = GetCurVRIndex(It.Value.Location, iBuilding, iLevel, iRoom, iRoomBox);
		if (result)
		{
			//为HotPoint赋值Label
			It.Value.Label = WorldPanoData.BuildingList[iBuilding].LevelList[iLevel].RoomList[iRoom].RoomBoxList[iRoomBox].BoxName;
			//为每个HotPoint找到它的OwnerRoom
			It.Value.OwnerRoom = &WorldPanoData.BuildingList[iBuilding].LevelList[iLevel].RoomList[iRoom];
			//为每个Room收集MyHotPointList
			WorldPanoData.BuildingList[iBuilding].LevelList[iLevel].RoomList[iRoom].MyHotPointList.Add(&It.Value);
		}
		else
		{
			WorldPanoData.bValid = false;
		}
	}

	//匹配每个Door到对应的Room
	for (auto& It : WorldPanoData.DoorList)
	{
		FHomeInfo_RoomBox* OutRoomBox = GetRoomBoxInfo(It.Value.RoomBoxID0);
		if (OutRoomBox)
		{
			//通过RoomBox找到Room，然后找到该Room里离此Door最近的HotPoint
			FHomeInfo_HotPoint* ClosestHotPoint = OutRoomBox->OwnerRoom->GetClosestHotPoint(It.Value.Location);
			if (ClosestHotPoint)
			{
				It.Value.HotPoint0 = ClosestHotPoint;
			}
			//把此Door添加到所在的Room里
			OutRoomBox->OwnerRoom->MyDoorList.Add(&It.Value);
		}
		OutRoomBox = GetRoomBoxInfo(It.Value.RoomBoxID1);
		if (OutRoomBox)
		{
			//通过RoomBox找到Room，然后找到该Room里离此Door最近的HotPoint
			FHomeInfo_HotPoint* ClosestHotPoint = OutRoomBox->OwnerRoom->GetClosestHotPoint(It.Value.Location);
			if (ClosestHotPoint)
			{
				It.Value.HotPoint1 = ClosestHotPoint;
			}
			//把此Door添加到所在的Room里
			OutRoomBox->OwnerRoom->MyDoorList.Add(&It.Value);
		}
	}

	//匹配每个Stair到对应的Room
	for (auto& It : WorldPanoData.StairList)
		//for (int32 iStair = 0; iStair < CurHomeVRInfo.StairList.Num(); iStair++)
	{
		FHomeInfo_RoomBox* OutRoomBox = GetRoomBoxInfo(It.Value.RoomBoxID0);
		if (OutRoomBox)
		{
			//通过RoomBox找到Room，然后找到该Room里离此Stair最近的HotPoint
			FHomeInfo_HotPoint* ClosestHotPoint = OutRoomBox->OwnerRoom->GetClosestHotPoint(It.Value.Location);
			if (ClosestHotPoint)
			{
				It.Value.HotPoint0 = ClosestHotPoint;
			}
			//把此Stair添加到所在的Room里
			OutRoomBox->OwnerRoom->MyStairList.Add(&It.Value);
		}
		OutRoomBox = GetRoomBoxInfo(It.Value.RoomBoxID1);
		if (OutRoomBox)
		{
			//通过RoomBox找到Room，然后找到该Room里离此Stair最近的HotPoint
			FHomeInfo_HotPoint* ClosestHotPoint = OutRoomBox->OwnerRoom->GetClosestHotPoint(It.Value.Location);
			if (ClosestHotPoint)
			{
				It.Value.HotPoint1 = ClosestHotPoint;
			}
			//把此Stair添加到所在的Room里
			OutRoomBox->OwnerRoom->MyStairList.Add(&It.Value);
		}
	}
}

bool FArmyPanoramaMgr::IsInBox(FVector _Loc, FVector _Min, FVector _Max)
{
	if (_Loc.X <= _Max.X && _Loc.X >= _Min.X &&
		_Loc.Y <= _Max.Y && _Loc.Y >= _Min.Y &&
		_Loc.Z <= _Max.Z && _Loc.Z >= _Min.Z)
	{
		return true;
	}

	return false;
}

bool FArmyPanoramaMgr::GetCurVRIndex(FVector _Loc, int32& _BuildingIndex, int32& _LevelIndex, int32& _RoomIndex, int32& _RoomBoxIndex)
{
	for (int32 iBuidling = 0; iBuidling < WorldPanoData.BuildingList.Num(); iBuidling++)
	{
		for (int32 iLevel = 0; iLevel < WorldPanoData.BuildingList[iBuidling].LevelList.Num(); iLevel++)
		{
			for (int32 iRoom = 0; iRoom < WorldPanoData.BuildingList[iBuidling].LevelList[iLevel].RoomList.Num(); iRoom++)
			{
				for (int32 iRoomBox = 0; iRoomBox < WorldPanoData.BuildingList[iBuidling].LevelList[iLevel].RoomList[iRoom].RoomBoxList.Num(); iRoomBox++)
				{
					bool result = IsInBox(_Loc,
						WorldPanoData.BuildingList[iBuidling].LevelList[iLevel].RoomList[iRoom].RoomBoxList[iRoomBox].Min,
						WorldPanoData.BuildingList[iBuidling].LevelList[iLevel].RoomList[iRoom].RoomBoxList[iRoomBox].Max
					);

					if (result)
					{
						_BuildingIndex = iBuidling;
						_LevelIndex = iLevel;
						_RoomIndex = iRoom;
						_RoomBoxIndex = iRoomBox;
						return true;
					}
				}
			}
		}
	}
	return false;
}

FHomeInfo_Room* FArmyPanoramaMgr::GetRoomInfo(int32 _RoomBoxID)
{
	for (auto& BuildIt : WorldPanoData.BuildingList)
	{
		for (auto& LevelIt : BuildIt.LevelList)
		{
			for (auto& RoomIt : LevelIt.RoomList)
			{
				for (auto& RoomBoxIt : RoomIt.RoomBoxList)
				{
					if (RoomBoxIt.ID == _RoomBoxID)
					{
						return &RoomIt;
					}
				}
			}
		}
	}

	return NULL;
}

FHomeInfo_Room* FArmyPanoramaMgr::GetRoomInfo(FVector _Loc)
{
	for (auto& BuildIt : WorldPanoData.BuildingList)
	{
		for (auto& LevelIt : BuildIt.LevelList)
		{
			for (auto& RoomIt : LevelIt.RoomList)
			{
				for (auto& RoomBoxIt : RoomIt.RoomBoxList)
				{
					bool result = IsInBox(_Loc, RoomBoxIt.Min, RoomBoxIt.Max);
					if (result)
						return &RoomIt;
				}
			}
		}
	}
	return NULL;
}

FHomeInfo_RoomBox* FArmyPanoramaMgr::GetRoomBoxInfoByLocation(FVector _Loc)
{
	for (auto& BuildIt : WorldPanoData.BuildingList)
	{
		for (auto& LevelIt : BuildIt.LevelList)
		{
			for (auto& RoomIt : LevelIt.RoomList)
			{
				for (auto& RoomBoxIt : RoomIt.RoomBoxList)
				{
					bool result = IsInBox(_Loc, RoomBoxIt.Min, RoomBoxIt.Max);
					if (result)
						return &RoomBoxIt;
				}
			}
		}
	}

	return NULL;
}

FHomeInfo_RoomBox* FArmyPanoramaMgr::GetRoomBoxInfo(int32 _RoomBoxID)
{
	for (auto& BuildIt : WorldPanoData.BuildingList)
	{
		for (auto& LevelIt : BuildIt.LevelList)
		{
			for (auto& RoomIt : LevelIt.RoomList)
			{
				for (auto& RoomBoxIt : RoomIt.RoomBoxList)
				{
					if (RoomBoxIt.ID == _RoomBoxID)
					{
						return &RoomBoxIt;
					}
				}
			}
		}
	}

	return NULL;
}

FHomeInfo_HotPoint FArmyPanoramaMgr::CreateHotPoint(FVector _Loc)
{
	FHomeInfo_RoomBox* RoomBox = GetRoomBoxInfoByLocation(_Loc);
	if (RoomBox)
	{
		return FHomeInfo_HotPoint(RoomBox->ID, _Loc, RoomBox->BoxName);
	}

	return FHomeInfo_HotPoint(0, _Loc, TEXT("未命名"));
}

FVector2D FArmyPanoramaMgr::GetVR360SpotLocation(FVector _CenterLoc, FRotator _CenterRot, FVector _SpotLoc)
{
	FVector FrontDir = _CenterRot.Vector();
	FVector SpotDir = _SpotLoc - _CenterLoc;

	FVector FrontDir2D_XY = FrontDir;
	FrontDir2D_XY.Z = 0.f;

	FVector SpotDir2D_XY = SpotDir;
	SpotDir2D_XY.Z = 0.f;

	FVector ZRot = FQuat::FindBetweenVectors(FrontDir2D_XY, SpotDir2D_XY).Euler();
	FVector YRot = FQuat::FindBetweenVectors(FVector(SpotDir2D_XY.Size(), 0, SpotDir.Z), FVector(1, 0, 0)).Euler();

	FVector2D SpotLocation(ZRot.Z, YRot.Y);

	return SpotLocation;
}

void FArmyPanoramaMgr::FillVRHomeInfo(FWorldPanoData& InCurHomeVRInfo)
{
	WorldPanoData = InCurHomeVRInfo;
	CalcLevelInfo();
}

int32 FArmyPanoramaMgr::IsInRoomId(FVector Point)
{
	return -1;
}

TSharedPtr<FArmyRoom> FArmyPanoramaMgr::GetCurrentRoom()
{
	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (FObjectWeakPtr It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room->IsPointInRoom(CaptureLocation))
		{
			return Room;
		}
	}
	return nullptr;
}

void FArmyPanoramaMgr::GetPanorama(const TArray<uint32>& Panorama)
{
	if (bMultiple)
	{
		bBusy = false;
	}
	VerticalSixFacesData = Panorama;
	OnePanoramaFinished();
}

bool FArmyPanoramaMgr::IsPointInRoom(FVector ActorLoc)
{
	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (FObjectWeakPtr It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room->IsPointInRoom(CaptureLocation))
		{
			return Room->IsPointInRoom(ActorLoc);
		}
	}
	return false;
}

void FArmyPanoramaMgr::SetSpotPointToRoom()
{
	FBox Box;
	FVector2D OutRoomSize;
	TArray<FObjectWeakPtr> OutRoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, OutRoomList);
	for (int i = 0; i < OutRoomList.Num(); i++)
	{
		TSharedPtr<FArmyRoom> OutRoom = StaticCastSharedPtr<FArmyRoom>(OutRoomList[i].Pin());
		Box = OutRoom->GetBounds();
		OutRoomSize = FVector2D(Box.GetSize().X, Box.GetSize().Y);
	}

	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (int i = 0; i < RoomList.Num(); i++)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[i].Pin());
				
		float RoomX = FMath::Abs(Box.Min.X - Room->GetBasePos().X) / OutRoomSize.X;
		float RoomY = FMath::Abs(Box.Min.Y - Room->GetBasePos().Y) / OutRoomSize.Y;

		FString navMapRatioX = FString::SanitizeFloat(RoomX);
		FString navMapRatioY = FString::SanitizeFloat(RoomY);

		FPanoScreenshot MyPanoScreenshot;
		MyPanoScreenshot.RoomID = Room->GetUniqueIdOfRoom();
		MyPanoScreenshot.Title = Room->GetSpaceName();
		MyPanoScreenshot.FileName = FileName[i];
		MyPanoScreenshot.navMapRatioX = navMapRatioX;
		MyPanoScreenshot.navMapRatioY = navMapRatioY;
		ScreenshotList.Add(MyPanoScreenshot);
	}

	for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->Tags.Num() > 0 && (ActorItr->Tags[0] == "MoveableMeshActor" || ActorItr->Tags[0] == "EnvironmentAsset"))
		{
            TArray<FObjectWeakPtr> RoomList;
            FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
            for (int i = 0; i < RoomList.Num(); i++)
			{
                TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[i].Pin());
				if (Room->IsPointInRoom(ActorItr->GetActorLocation()))
				{
					FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0,-90,0), ActorItr->GetActorLocation());
					FPanoSpot MyPanoSpot;
					MyPanoSpot.AtH = SpotLocation.X;
					MyPanoSpot.AtV = SpotLocation.Y;
					MyPanoSpot.LinkFileName = FString::FromInt(0);
					MyPanoSpot.SpotType = 2;
					MyPanoSpot.FurId = FString::FromInt(FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(*ActorItr));

					ScreenshotList[i].SpotList.Add(MyPanoSpot);
				}
			}
		}
	}
}

void FArmyPanoramaMgr::SetDoorAndPassToRoom()
{
	//标准门
	TArray<TWeakPtr<FArmyObject>> Doors;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, Doors);
	for (int32 i = 0; i < Doors.Num(); i++)
	{
		TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(Doors[i].Pin());

		//不与外墙连接的每个门肯定要有两个对应的热点
		FPanoSpot MyPanoSpotFirst, MyPanoSpotSecond;
		int32 FirstRelatedNum = -1, SecondRelatedNum = -1;
		MyPanoSpotFirst.LinkFileName = FString::FromInt(0);
		MyPanoSpotFirst.SpotType = 1;
		MyPanoSpotSecond = MyPanoSpotFirst;

		TArray<FObjectWeakPtr> RoomList;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
		for (int j = 0; j < RoomList.Num(); j++)
		{
			if (Door->FirstRelatedRoom.IsValid() && Door->FirstRelatedRoom->GetUniqueID() == RoomList[j].Pin()->GetUniqueID())
			{
				FirstRelatedNum = j;
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[j].Pin());
				FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0, -90, 0), Door->GetBasePos());
				MyPanoSpotFirst.AtH = SpotLocation.X;
				MyPanoSpotFirst.AtV = SpotLocation.Y;
			}

			if (Door->SecondRelatedRoom.IsValid() && Door->SecondRelatedRoom->GetUniqueID() == RoomList[j].Pin()->GetUniqueID())
			{
				SecondRelatedNum = j;
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[j].Pin());
				FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0, -90, 0), Door->GetBasePos());
				MyPanoSpotSecond.AtH = SpotLocation.X;
				MyPanoSpotSecond.AtV = SpotLocation.Y;
			}
		}
		if (SecondRelatedNum != -1 && FirstRelatedNum != -1)
		{
			MyPanoSpotFirst.FurId = FileName[SecondRelatedNum];
			MyPanoSpotSecond.FurId = FileName[FirstRelatedNum];
			ScreenshotList[FirstRelatedNum].SpotList.Add(MyPanoSpotFirst);
			ScreenshotList[SecondRelatedNum].SpotList.Add(MyPanoSpotSecond);
		}
	}

	//垭口
	TArray<TWeakPtr<FArmyObject>> Passes;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, Passes);
	for (int i = 0; i < Passes.Num(); i++)
	{
		TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(Passes[i].Pin());

		FPanoSpot MyPanoSpotFirst, MyPanoSpotSecond;
		int32 FirstRelatedNum = -1, SecondRelatedNum = -1;
		MyPanoSpotFirst.LinkFileName = FString::FromInt(0);
		MyPanoSpotFirst.SpotType = 1;
		MyPanoSpotSecond = MyPanoSpotFirst;

		TArray<FObjectWeakPtr> RoomList;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
		for (int j = 0; j < RoomList.Num(); j++)
		{
			if (Pass->FirstRelatedRoom->GetUniqueID() == RoomList[j].Pin()->GetUniqueID())
			{
				FirstRelatedNum = j;
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[j].Pin());
				FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0, -90, 0), Pass->GetBasePos());
				MyPanoSpotFirst.AtH = SpotLocation.X;
				MyPanoSpotFirst.AtV = SpotLocation.Y;
			}

			if (Pass->SecondRelatedRoom->GetUniqueID() == RoomList[j].Pin()->GetUniqueID())
			{
				SecondRelatedNum = j;
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[j].Pin());
				FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0, -90, 0), Pass->GetBasePos());
				MyPanoSpotSecond.AtH = SpotLocation.X;
				MyPanoSpotSecond.AtV = SpotLocation.Y;
			}
		}
		//不与外墙相连
		if (SecondRelatedNum != -1 && FirstRelatedNum != -1)
		{
			MyPanoSpotFirst.FurId = FileName[SecondRelatedNum];
			MyPanoSpotSecond.FurId = FileName[FirstRelatedNum];
			ScreenshotList[FirstRelatedNum].SpotList.Add(MyPanoSpotFirst);
			ScreenshotList[SecondRelatedNum].SpotList.Add(MyPanoSpotSecond);
		}
	}

	//推拉门
	TArray<TWeakPtr<FArmyObject>> SlidingDoorList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, SlidingDoorList);
	for (int i = 0; i < SlidingDoorList.Num(); i++)
	{
		TSharedPtr<FArmySlidingDoor> SlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(SlidingDoorList[i].Pin());

		FPanoSpot MyPanoSpotFirst, MyPanoSpotSecond;
		int32 FirstRelatedNum = -1, SecondRelatedNum = -1;
		MyPanoSpotFirst.LinkFileName = FString::FromInt(0);
		MyPanoSpotFirst.SpotType = 1;
		MyPanoSpotSecond = MyPanoSpotFirst;

		TArray<FObjectWeakPtr> RoomList;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
		for (int j = 0; j < RoomList.Num(); j++)
		{
			if (SlidingDoor->FirstRelatedRoom->GetUniqueID() == RoomList[j].Pin()->GetUniqueID())
			{
				FirstRelatedNum = j;
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[j].Pin());
				FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0, -90, 0), SlidingDoor->GetBasePos());
				MyPanoSpotFirst.AtH = SpotLocation.X;
				MyPanoSpotFirst.AtV = SpotLocation.Y;
			}

			if (SlidingDoor->SecondRelatedRoom->GetUniqueID() == RoomList[j].Pin()->GetUniqueID())
			{
				SecondRelatedNum = j;
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[j].Pin());
				FVector2D SpotLocation = GetVR360SpotLocation(Room->GetBounds3D().GetCenter(), FRotator(0, -90, 0), SlidingDoor->GetBasePos());
				MyPanoSpotSecond.AtH = SpotLocation.X;
				MyPanoSpotSecond.AtV = SpotLocation.Y;
			}
		}
		if (SecondRelatedNum != -1 && FirstRelatedNum != -1)
		{
			MyPanoSpotFirst.FurId = FileName[SecondRelatedNum];
			MyPanoSpotSecond.FurId = FileName[FirstRelatedNum];
			ScreenshotList[FirstRelatedNum].SpotList.Add(MyPanoSpotFirst);
			ScreenshotList[SecondRelatedNum].SpotList.Add(MyPanoSpotSecond);
		}
	}
}