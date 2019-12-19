#include "SArmyRenderingImage.h"
#include "SCheckBox.h"
#include "STextBlock.h"
#include "SBoxPanel.h"
#include "SButton.h"
#include "SBox.h"
#include "ArmyStyle.h"
#include "ArmyGameInstance.h"
#include "ArmyCameraParamModule.h"
#include "ArmyHttpModule.h"
#include "ArmyEngineTools.h"
#include "ArmyPlayerController.h"
#include "ArmyUser.h"

ECheckBoxState SArmyRenderingImage::IsSelected(int32 InType) const
{
	return RenderType == InType ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SArmyRenderingImage::OnCheckStateChanged(ECheckBoxState InCheck, int32 InType)
{
	if (InCheck == ECheckBoxState::Checked)
	{
		RenderType = InType;
	}
}

void SArmyRenderingImage::Construct(const FArguments & InArgs)
{
	RenderingImages.Reset();
	bRipperIsScreenshot = false;
	RenderType = 1;
	//绑定截图代理
	GVC->ScreenShotFun = ScreenShotDelegate::CreateRaw(this, &SArmyRenderingImage::ProcessScreenShots);
	FArmyCameraParamModule::Get().StartDataDelegate.BindRaw(this, &SArmyRenderingImage::BGetScreenData);
	
	ChildSlot
	[
		SAssignNew(MyWidgetSwitcher, SWidgetSwitcher)

		+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			//设置界面
			WidgetChoose()
		]
	
		+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			//成功界面
			WidgetSuccess()
		]

		+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		[
			//失败界面
			WidgetFail()
		]
	];
}

TSharedRef<SWidget> SArmyRenderingImage::WidgetChoose()
{
	return
		SNew(SBox)
		.WidthOverride(420)
		.HeightOverride(180)
		.Padding(FMargin(20, 40, 20, 20))
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.HAlign(EHorizontalAlignment::HAlign_Left)
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("选择渲染分辨率")))
			]

			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			.HAlign(HAlign_Left)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				[
					SNew(SCheckBox)
					.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
					.IsChecked(this,&SArmyRenderingImage::IsSelected,1)
					.OnCheckStateChanged(this,&SArmyRenderingImage::OnCheckStateChanged,1)
				]
			
				+ SHorizontalBox::Slot()
				.Padding(FMargin(8,0,18,0))
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("1倍")))
				]

				+ SHorizontalBox::Slot()
				[
					SNew(SCheckBox)
					.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
					.IsChecked(this, &SArmyRenderingImage::IsSelected, 2)
					.OnCheckStateChanged(this, &SArmyRenderingImage::OnCheckStateChanged, 2)
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(8, 0, 18, 0))
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("2倍")))
				]

				+ SHorizontalBox::Slot()
				[
					SNew(SCheckBox)
					.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
					.IsChecked(this, &SArmyRenderingImage::IsSelected, 4)
					.OnCheckStateChanged(this, &SArmyRenderingImage::OnCheckStateChanged, 4)
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(8, 0, 18, 0))
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("4倍")))
				]
			]

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "Box")
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FFFF9800"))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(this, &SArmyRenderingImage::OnGenerateClicked)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("生成效果图")))
						]
					]
				]

				+ SHorizontalBox::Slot()
				.Padding(20,0,0,0)
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "Box")
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.OnClicked(this, &SArmyRenderingImage::CancelClicked)
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("关闭")))
						]
					]
				]
			]
		];
}

TSharedRef<SWidget> SArmyRenderingImage::WidgetSuccess()
{
	return
		SNew(SBox)
		.WidthOverride(420)
		.HeightOverride(168)
		.Padding(FMargin(20, 40, 20, 20))
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Top)
				[
					SNew(SImage)
					.Image(FArmyStyle::Get().GetBrush("Icon.alert_success"))
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(16,0,0,0))
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("效果图生成成功！")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FLinearColor::White)
				]
			]
	
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FFFF9800"))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(this, &SArmyRenderingImage::OnOpenDesignSketchFile)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("打开")))
						]
					]
				]
			
				+ SHorizontalBox::Slot()
				.Padding(20, 0, 0, 0)
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ContentPadding(0)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(this, &SArmyRenderingImage::OnOpenDesignSketchDir)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("打开文件夹")))
						]
					]
				]

				+ SHorizontalBox::Slot()
				.Padding(20,0,0,0)
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ContentPadding(0)
						.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(this, &SArmyRenderingImage::CancelClicked)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("关闭")))
						]
					]
				]
			]
		];
}

TSharedRef<SWidget> SArmyRenderingImage::WidgetFail()
{
	return
		SNew(SBox)
		.WidthOverride(420)
		.HeightOverride(168)
		.Padding(FMargin(20, 40, 20, 20))
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(EHorizontalAlignment::HAlign_Left)
				.VAlign(EVerticalAlignment::VAlign_Top)
				[
					SNew(SImage)
					.Image(FArmyStyle::Get().GetBrush("Icon.alert_fail"))
				]

				+ SHorizontalBox::Slot()
				.Padding(FMargin(16,0,0,0))
				.VAlign(EVerticalAlignment::VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("效果图生成失败！")))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FLinearColor::White)
				]
			]
	
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Right)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "Box")
						.ButtonColorAndOpacity(FLinearColor(FColor(0XFFFD9800)))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("重试")))
						]
					]
				]

				+ SHorizontalBox::Slot()
				.Padding(20,0,0,0)
				[
					SNew(SBox)
					.HeightOverride(30)
					.WidthOverride(80)
					[
						SNew(SButton)
						.ButtonStyle(FCoreStyle::Get(), "Box")
						.ButtonColorAndOpacity(FLinearColor(FColor(0XFF353638)))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(this, &SArmyRenderingImage::CancelClicked)
						.Content()
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor::White)
							.Text(FText::FromString(TEXT("关闭")))
						]
					]
				]
			]
		];
}

FReply SArmyRenderingImage::OnGenerateClicked()
{
	//如果不是透视图模式，不能生成效果图
	if (GXRPC->GetXRViewMode() != EXRView_FPS)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请在透视图模式下生成效果图"));
		return FReply::Handled();
	}

	RenderingImages.Reset();
	if (RenderType == 1 || RenderType == 2)
	{
		GGI->Window->ShowThrobber(MAKE_TEXT("效果图生成进度90%"));
		bRipperIsScreenshot = true;
	}
	else
	{
		//启用GameViewportClient
		GVC->bUserPlayerControllerView = true;
		//传递相机位置
		FArmyCameraParamModule::Get().SetCameraLocationAndRotation(GVC->GetViewLocation(), GVC->GetViewRotation());
		FArmyCameraParamModule::Get().StartCamera(true);
	}
		
	DesignSketchFilePath = FResTools::GetSaveScreenshot() + TEXT("DesignSketch_") + FDateTime::Now().ToString() + TEXT(".jpg");

	return FReply::Handled();
}

FReply SArmyRenderingImage::OnOpenDesignSketchFile()
{
	FPlatformMisc::OsExecute(TEXT("open"), *FPaths::ConvertRelativePathToFull(DesignSketchFilePath));
	return FReply::Handled();
}

FReply SArmyRenderingImage::OnOpenDesignSketchDir()
{
	FPlatformProcess::ExploreFolder(*FPaths::ConvertRelativePathToFull(DesignSketchFilePath));
	return FReply::Handled();
}

FReply SArmyRenderingImage::CancelClicked()
{
	GGI->Window->DismissModalDialog();
	return FReply::Handled();
}

void SArmyRenderingImage::ProcessScreenShots(FViewport* InViewport)
{
	if (bRipperIsScreenshot)
	{
		if (bCustomScreenshot)
		{
			MapScreenShot(InViewport);
			bCustomScreenshot = false;
			bRipperIsScreenshot = false;
		}
		else
		{
			//处理数据
			HandleCustomScreenshot();
		}
	}

	if (bRipperIsExportJpg)
	{
		ExportJpg(InViewport);
		bRipperIsExportJpg = false;
	}
}

void SArmyRenderingImage::MapScreenShot(FViewport* InViewport)
{
	TArray<FColor> RawData;

	int32 X = InViewport->GetSizeXY().X;
	int32 Y = InViewport->GetSizeXY().Y;
	FIntRect InRect(0, 0, X, Y);
	GetViewportScreenShot(InViewport, RawData, InRect);

	TArray<FColor> ScaledBitmap;
	int32 ScaledWidth = X;
	int32 ScaledHeight = Y;

	if (RenderType == 1 || RenderType == 2)
	{
		TSharedPtr<IImageWrapper> ImageWrapper;
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		ImageWrapper->SetRaw(RawData.GetData(), ScaledWidth * ScaledHeight * sizeof(FColor), ScaledWidth, ScaledHeight, ERGBFormat::BGRA, 8);
		TArray<uint8> CompressedData = ImageWrapper->GetCompressed(100);
		//保存到本地
		FFileHelper::SaveArrayToFile(CompressedData, *DesignSketchFilePath);
		//上传到后台
		UploadConstructionImage(CompressedData);

		//截图完成后解绑代理
		GVC->ScreenShotFun.Unbind();
		MyWidgetSwitcher->SetActiveWidgetIndex(1);
		GGI->Window->HideThrobber();
	}
	else
	{
		ScreenShotMapArray.Add(RawData);
		int32 Progress = ScreenShotMapArray.Num() * 100 / ((RenderType * 2 - 1) * (RenderType * 2 - 1));
		FText ProgressMessage = FText::FromString(FString::Printf(TEXT("效果图生成进度%d%%"), Progress));
		GGI->Window->ShowThrobber(ProgressMessage);
		//GGI->Window->ShowThrobber(ProgressMessage, FSimpleDelegate::CreateRaw(this, &SArmyRenderingImage::ShutDown));
	}

	FScreenshotRequest::Reset();
	// Reeanble screen messages - if we are NOT capturing a movie
	GAreScreenMessagesEnabled = GScreenMessagesRestoreState;
}

void SArmyRenderingImage::ExportJpg(FViewport* InViewport)
{
	int32 SizeX = ResWidth;
	int32 SizeY = ResHight;

	TArray< TArray<FColor> > FuseRawDataArray;
	FuseRawDataArray.SetNum(RenderType * 2 - 1);
	for (int32 i = 0; i < RenderType * 2 - 1; i++)
	{
		TArray<FColor>& FuseRawData = FuseRawDataArray[i];
		FuseRawData.SetNum(SizeX * SizeY * RenderType);

		for (int32 Index = 0; Index < SizeY; Index++)
		{
			int32 FuseOffset = SizeX / 2;
			FMemory::Memcpy(FuseRawData.GetData() + Index * SizeX * RenderType, (FColor*)(ScreenShotMapArray[i * (RenderType * 2 - 1)].GetData()) + Index * SizeX, SizeX * 2);
			for (int32 PixelIndex = 0; PixelIndex < FuseOffset; PixelIndex++)
			{
				float A = 1 - (float)PixelIndex / (float)FuseOffset;
				float B = (float)PixelIndex / (float)FuseOffset;

				for (int32 MapIndex = 0; MapIndex < RenderType * 2 - 2; MapIndex++)
				{
					int32 TmpIndex = i * (RenderType * 2 - 1) + MapIndex;
					FuseRawData[PixelIndex + Index * SizeX * RenderType + (MapIndex + 1) * FuseOffset].R = A * ScreenShotMapArray[TmpIndex][PixelIndex + FuseOffset + Index * SizeX].R
						+ B * ScreenShotMapArray[TmpIndex + 1][PixelIndex + Index * SizeX].R;
					FuseRawData[PixelIndex + Index * SizeX * RenderType + (MapIndex + 1) * FuseOffset].G = A*ScreenShotMapArray[TmpIndex][PixelIndex + FuseOffset + Index * SizeX].G
						+ B * ScreenShotMapArray[TmpIndex + 1][PixelIndex + Index * SizeX].G;
					FuseRawData[PixelIndex + Index * SizeX * RenderType + (MapIndex + 1) * FuseOffset].B = A*ScreenShotMapArray[TmpIndex][PixelIndex + FuseOffset + Index * SizeX].B
						+ B * ScreenShotMapArray[TmpIndex + 1][PixelIndex + Index * SizeX].B;
					FuseRawData[PixelIndex + Index * SizeX * RenderType + (MapIndex + 1) * FuseOffset].A = A*ScreenShotMapArray[TmpIndex][PixelIndex + FuseOffset + Index * SizeX].A
						+ B * ScreenShotMapArray[TmpIndex + 1][PixelIndex + Index * SizeX].A;
				}
			}
			FMemory::Memcpy(FuseRawData.GetData() + Index * SizeX * RenderType + (RenderType * 2 - 1) * FuseOffset,
				(FColor*)(ScreenShotMapArray[i * (RenderType * 2 - 1) + RenderType * 2 - 2].GetData()) + Index * SizeX + FuseOffset,
				SizeX * 2);
		}
	}

	TArray<FColor> MapRawData;
	MapRawData.SetNum(SizeX*SizeY*RenderType * RenderType);

	int32 FuseOffsetY = SizeY / 2;
	FMemory::Memcpy(MapRawData.GetData(), (FColor*)(FuseRawDataArray[0].GetData()), SizeX * RenderType * FuseOffsetY * 4);
	FMemory::Memcpy(MapRawData.GetData() + (RenderType * 2 - 1) * SizeX * RenderType * FuseOffsetY, (FColor*)(FuseRawDataArray[RenderType * 2 - 2].GetData()) + SizeX * RenderType * FuseOffsetY, SizeX * RenderType * FuseOffsetY * 4);

	for (int32 i = 0; i < RenderType * 2 - 2; i++)
	{
		for (int32 Row = 0; Row < FuseOffsetY; Row++)
		{
			float A = 1 - (float)Row / (float)FuseOffsetY;
			float B = (float)Row / (float)FuseOffsetY;

			for (int32 Column = 0; Column < SizeX * RenderType; Column++)
			{
				int32 Index = SizeX * RenderType * FuseOffsetY + Row * (SizeX * RenderType) + Column;
				MapRawData[Index + i * SizeX * RenderType * FuseOffsetY].R = A * FuseRawDataArray[i][Index].R + B * FuseRawDataArray[i + 1][Row * (SizeX * RenderType) + Column].R;
				MapRawData[Index + i * SizeX * RenderType * FuseOffsetY].G = A * FuseRawDataArray[i][Index].G + B * FuseRawDataArray[i + 1][Row * (SizeX * RenderType) + Column].G;
				MapRawData[Index + i * SizeX * RenderType * FuseOffsetY].B = A * FuseRawDataArray[i][Index].B + B * FuseRawDataArray[i + 1][Row * (SizeX * RenderType) + Column].B;
				MapRawData[Index + i * SizeX * RenderType * FuseOffsetY].A = A * FuseRawDataArray[i][Index].A + B * FuseRawDataArray[i + 1][Row * (SizeX * RenderType) + Column].A;
			}
		}
	}

	int32 ScaledWidth = SizeX * RenderType;
	int32 ScaledHeight = SizeY * RenderType;

	TSharedPtr<IImageWrapper> ImageWrapper;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	ImageWrapper->SetRaw(MapRawData.GetData(), ScaledWidth * ScaledHeight * sizeof(FColor), ScaledWidth, ScaledHeight, ERGBFormat::BGRA, 8);
	TArray<uint8> CompressedData = ImageWrapper->GetCompressed(100);
	FFileHelper::SaveArrayToFile(CompressedData, *DesignSketchFilePath);
	GVC->bUserPlayerControllerView = false;
	//4倍图上传到后台
	UploadConstructionImage(CompressedData);
	ScreenShotMapArray.Empty();
	GVC->ScreenShotFun.Unbind();
	MyWidgetSwitcher->SetActiveWidgetIndex(1);
	GGI->Window->HideThrobber();

	return;
}

void SArmyRenderingImage::BGetScreenData(bool BEnd)
{
	/* 还在继续获取数据，获取数据完成 */
	if (BEnd)
		bRipperIsScreenshot = true;
	else
	{
		bRipperIsExportJpg = true;
		FArmyCameraParamModule::Get().StartDataDelegate.Unbind();
	}
		
}

void SArmyRenderingImage::ShutDown()
{
	FArmyCameraParamModule::Get().StartCamera(false);
	ScreenShotMapArray.Empty(); 
	bCustomScreenshot = false;
	bRipperIsScreenshot = false;
	bRipperIsExportJpg = false; 
	DesignSketchFilePath.Empty();
}

bool SArmyRenderingImage::UploadConstructionImage(const TArray<uint8>& InData)
{
	FString FileName = FString::Printf(TEXT("Screenshot2D_%s.jpg"), *FArmyEngineTools::GetTimeStampString());
	
	//执行请求
    FAliyunOss::Get().PutObject(EOssDir::OD_Picture, FileName, InData, FOnUploadComplete::CreateRaw(this, &SArmyRenderingImage::Callback_RequestUploadImageFile));
	return true;
}

void SArmyRenderingImage::Callback_RequestUploadImageFile(TSharedPtr<FAliyunOssResponse> Response)
{
    if (Response->bWasSuccessful)
    {
        RenderingImages.Add(Response->CdnUrl);
        RequestSaveRenderingImage();
    }
}

void SArmyRenderingImage::HandleCustomScreenshot()
{
	bCustomScreenshot = true;
	
	FString CommandString;
	ResWidth = 1920;
	ResHight = 1080;
	if (RenderType == 1 || RenderType == 4)
	{
		CommandString = FString::Printf(TEXT("HighResShot %dx%d"), ResWidth, ResHight);
		GVC->GetWorld()->GetGameViewport()->ConsoleCommand(*CommandString);
	}
	else if (RenderType == 2)
	{
		CommandString = FString::Printf(TEXT("HighResShot %dx%d"), ResWidth * 2, ResHight * 2);
		GVC->GetWorld()->GetGameViewport()->ConsoleCommand(*CommandString);
	}
}

void SArmyRenderingImage::RequestSaveRenderingImage()
{
	int32 SpaceID = -1;
	int32 RoomId = -1;
	TArray<FObjectWeakPtr> RoomList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (FObjectWeakPtr It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room->IsPointInRoom(GVC->GetViewLocation()))
		{
			SpaceID = Room->GetSpaceId();
			RoomId = Room->GetUniqueIdOfRoom();
		}
	}

	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
	JsonWriter->WriteObjectStart();

	JsonWriter->WriteValue(TEXT("planId"), FArmyUser::Get().GetCurPlanID());
	JsonWriter->WriteValue(TEXT("vrSpaceId"), RoomId); 
	JsonWriter->WriteValue(TEXT("spaceId"), SpaceID);

	JsonWriter->WriteArrayStart(TEXT("thumbnailUrlList"));
	for (auto ImageInfo : RenderingImages)
	{
		JsonWriter->WriteValue(ImageInfo);
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyRenderingImage::Callback_RequestSaveRenderingImage);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/previews", CompleteDelegate, JStr);
    Request->ProcessRequest();
}

void SArmyRenderingImage::Callback_RequestSaveRenderingImage(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        GGI->Window->ShowMessage(MT_Success, TEXT("效果图上传后台成功！"));
    }
    else
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("效果图上传后台失败！"));
    }
}