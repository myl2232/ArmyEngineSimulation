#include "SArmySavePlan.h"
#include "ArmyGameInstance.h"
#include "ArmyViewportClient.h"
#include "ArmyEngineTools.h"
#include "ArmyStyle.h"
#include "SArmyEditableNumberBox.h"
#include "ArmyDesignEditor.h"
#include "ArmyCommonTools.h"
#include "SEditableTextBox.h"
#include "SImage.h"
#include "Json.h"
#include "IArmyHttp.h"
#include "ArmyHttpModule.h"
#include "ArmyClipper.h"

void SArmySavePlan::Construct(const FArguments & InArgs)
{
	ChildSlot
	[
	    SNew(SHorizontalBox)

	    + SHorizontalBox::Slot()
	    .Padding(20, 20, 20, 0)
	    .VAlign(EVerticalAlignment::VAlign_Top)
	    [
		    SNew(SBox)
		    .WidthOverride(136)
	        .HeightOverride(136)
	        [
		        SAssignNew(Img_Thumbnail, SImage)
	        ]
		]

	    + SHorizontalBox::Slot()
		.Padding(20, 20, 20, 0)
		.AutoWidth()
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
		    .Padding(0, 0, 20, 0)
		    .AutoHeight()
		    [
			    //方案名称
			    CreatePlanWidget()
		    ]

	        // 错误信息
	        + SVerticalBox::Slot()
		    .Padding(84, 5, 0, 0)
		    .AutoHeight()
		    [
			    SAssignNew(TB_ErrorMessage, STextBlock)
			    .Text(FText::FromString(TEXT("方案名称不能为空")))
		        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		        .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF5A5A"))
		        .Visibility(EVisibility::Collapsed)
		    ]

	        +SVerticalBox::Slot()
		    .Padding(0, 16, 20, 20)
		    .VAlign(VAlign_Bottom)
		    .AutoHeight()
		    [
			    //选择房型
			    CreateHouseTypeWidget()
		    ]
		]
    ];

	Init();
}

bool SArmySavePlan::CheckValid()
{
	if (ETB_PlanName->GetText().ToString().TrimStart() == "") {
		TB_ErrorMessage->SetVisibility(EVisibility::Visible);
		return false;
	}
	TB_ErrorMessage->SetVisibility(EVisibility::Collapsed);
	return true;
}

void SArmySavePlan::OnConfirmClicked()
{
	if (CheckValid()) {
		PreReqSavePlan();
	}
}

TSharedRef<SWidget> SArmySavePlan::CreatePlanWidget()
{
	return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(64)
		    [
			    SNew(STextBlock)
			    .Text(FText::FromString(TEXT("方案名称")))
		        .TextStyle(FCoreStyle::Get(), "VRSText_12")
		        .Justification(ETextJustify::Right)
		    ]
		]

	    + SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SBox)
			.WidthOverride(368)
		    .HeightOverride(32)
		    [
			    SAssignNew(ETB_PlanName, SEditableTextBox)
			    .Padding(FMargin(8, 0, 0, 0))
		        .ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		        .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		        .HintText(FText::FromString(TEXT("请输入方案名称")))
                .AllowContextMenu(false)
		    ]
		];
}

TSharedRef<SWidget> SArmySavePlan::CreateHouseTypeWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 20, 0)
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(64)
		    [
			    SNew(STextBlock)
			    .Text(FText::FromString(TEXT("房型")))
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                .Justification(ETextJustify::Right)
            ]
		]

	    // 几室
	    + SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		    .Padding(0, 0, 5, 0)
		    .AutoWidth()
		    [
			    SNew(SBox)
			    .WidthOverride(40)
		        .HeightOverride(32)
		        [
			        SAssignNew(ETB_Bedroom, SArmyEditableNumberBox)
			        .Padding(FMargin(8, 0, 0, 0))
		            .ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		            .IsIntegerOnly(true)
		            .Text(FText::FromString("0"))
		        ]
		    ]

	        + SHorizontalBox::Slot()
		    .AutoWidth()
		    .VAlign(VAlign_Center)
		    [
			    SNew(STextBlock)
			    .Text(FText::FromString(TEXT("室")))
		        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		    ]
		]

	    // 几厅
	    + SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		    .Padding(0, 0, 5, 0)
		    .AutoWidth()
		    [
			    SNew(SBox)
			    .WidthOverride(40)
		        .HeightOverride(32)
		        [
			        SAssignNew(ETB_LivingRoom, SArmyEditableNumberBox)
			        .Padding(FMargin(8, 0, 0, 0))
		            .ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		            .IsIntegerOnly(true)
		            .Text(FText::FromString("0"))
		        ]
		    ]

	        + SHorizontalBox::Slot()
		    .AutoWidth()
		    .VAlign(VAlign_Center)
		    [
			    SNew(STextBlock)
			    .Text(FText::FromString(TEXT("厅")))
		        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		    ]
		]

	    // 几厨
	    + SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		    .Padding(0, 0, 5, 0)
		    .AutoWidth()
		    [
			    SNew(SBox)
			    .WidthOverride(40)
		        .HeightOverride(32)
		        [
			        SAssignNew(ETB_Kitchen, SArmyEditableNumberBox)
			        .Padding(FMargin(8, 0, 0, 0))
		            .ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		            .IsIntegerOnly(true)
		            .Text(FText::FromString("0"))
		        ]
		    ]

	        + SHorizontalBox::Slot()
		    .AutoWidth()
		    .VAlign(VAlign_Center)
		    [
			    SNew(STextBlock)
			    .Text(FText::FromString(TEXT("厨")))
		        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		    ]
		]

	    // 几卫
	    + SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		    .Padding(0, 0, 5, 0)
		    .AutoWidth()
		    [
			    SNew(SBox)
			    .WidthOverride(40)
		        .HeightOverride(32)
		        [
			        SAssignNew(ETB_Bathroom, SArmyEditableNumberBox)
			        .Padding(FMargin(8, 0, 0, 0))
		            .ForegroundColor(FLinearColor(FColor(0XFFFFFFFF)))
		            .Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.FF212224"))
		            .IsIntegerOnly(true)
		            .Text(FText::FromString("0"))
		        ]
		    ]

	        + SHorizontalBox::Slot()
		    .AutoWidth()
		    .VAlign(VAlign_Center)
		    [
			    SNew(STextBlock)
			    .Text(FText::FromString(TEXT("卫")))
		        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		    ]
		];
}

void SArmySavePlan::Init()
{
	ETB_PlanName->SetText(FText::FromString(FArmyUser::Get().CurPlanData->Name));
	if (FArmyUser::Get().CurPlanData.IsValid())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = FArmyUser::Get().CurPlanData->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		TSharedPtr<FArmyPlanRes> PlanRes = StaticCastSharedPtr<FArmyPlanRes>(resArr[0]);
		if (PlanRes.IsValid()) {
			HouseName = FArmyUser::Get().CurPlanData->Name;

			ETB_Bedroom->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlanRes->BedroomNum)));
			ETB_LivingRoom->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlanRes->LivingRoomNum)));
			ETB_Kitchen->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlanRes->KitchenNum)));
			ETB_Bathroom->SetText(FText::FromString(FString::Printf(TEXT("%d"), PlanRes->BathroomNum)));
		}
	}
}

void SArmySavePlan::Reset()
{
    HomeData.Reset();
    PlanData.Reset();

    bUploadHomeThumbnailReady = bUploadPlanThumbnailReady =
        bUploadHomeFileReady = bUploadPlanFileReady = bUploadFacsimileReady = false;

    HomeThumbnailUrl = PlanThumbnailUrl = HomeFileUrl = PlanFileUrl = FacsimileUrl = ViewImageUrl = TEXT("");
}

void SArmySavePlan::TakeHomeScreenshot()
{
    TArray<FColor> UncompressedData;
    int32 ScaledImageSize = 720;
    GVC->TakeScreenshotInMaxRect(ScaledImageSize, UncompressedData, HomeThumbnailData);
}

void SArmySavePlan::TakePlanScreentshot()
{
    GenerateFilenames();

	TArray<FColor> UncompressedData;
	int32 ScaledImageSize = 720;
	GVC->TakeScreenshotInMaxRect(ScaledImageSize, UncompressedData, PlanThumbnailData);

	// 缩放为720尺寸
	TArray<uint8> OriginalData;
	int32 InRawSize = ScaledImageSize * ScaledImageSize * sizeof(FColor);
	OriginalData.AddUninitialized(InRawSize);
	FMemory::Memcpy(OriginalData.GetData(), UncompressedData.GetData(), InRawSize);

	// 设置到UI中
	ThumbnailBrush = FSlateDynamicImageBrush::CreateWithImageData(FName(*PlanThumbnailFilename), FVector2D(ScaledImageSize, ScaledImageSize), OriginalData);
	Img_Thumbnail->SetImage(ThumbnailBrush.Get());
}

void SArmySavePlan::GenerateFilenames()
{
    FString TimeStampString = FArmyEngineTools::GetTimeStampString();
    HomeThumbnailFilename = FString::Printf(TEXT("HomeThumbnail_%s.jpg"), *TimeStampString);
    PlanThumbnailFilename = FString::Printf(TEXT("PlanThumbnail_%s.jpg"), *TimeStampString);
    HomeFilename = FString::Printf(TEXT("Home_%s.json"), *TimeStampString);
    PlanFilename = FString::Printf(TEXT("Plan_%s.json"), *TimeStampString);
}

void SArmySavePlan::PreReqSavePlan(bool bTakeHomeScreenshot/* = false*/)
{
	Reset();

	// @欧石楠 如果户型缩略图没有截取，有可能是在户型模式下保存且没有切换过模式，需要自动截取一下
    if (bTakeHomeScreenshot)
    {
        TakeHomeScreenshot();
    }

    TakePlanScreentshot();

	// 获取户型和方案数据流
	GGI->DesignEditor->CollectPlanData(HomeData, PlanData);

	// 2D模式下直接保存后（没有切换到立面模式），会手动生成对应的面片，所以要隐藏
	if (//@郭子阳 显隐立面actor与有没有外墙无关   FArmySceneData::Get()->bHasOutWall && 
		(GGI->DesignEditor->CurrentController->GetDesignModelType() == E_HomeModel
		|| GGI->DesignEditor->CurrentController->GetDesignModelType() == E_ModifyModel
		|| GGI->DesignEditor->CurrentController->GetDesignModelType() == E_LayoutModel))
		FArmySceneData::Get()->SetHardwareModeActorVisible(false);

	HomeFileMD5 = FArmyEngineTools::CalcMD5FromFileData(HomeData);
	PlanFileMD5 = FArmyEngineTools::CalcMD5FromFileData(PlanData);

	// 绑定回调
    FOnUploadComplete OnUploadHomeThumbnail;
    OnUploadHomeThumbnail.BindSP(this, &SArmySavePlan::ResUploadHomeThumbnail);
    FOnUploadComplete OnUploadPlanThumbnail;
    OnUploadPlanThumbnail.BindSP(this, &SArmySavePlan::ResUploadPlanThumbnail);
	FOnUploadComplete OnUploadHomeFile;
	OnUploadHomeFile.BindSP(this, &SArmySavePlan::ResUploadHomeFile);
	FOnUploadComplete OnUploadPlanFile;
	OnUploadPlanFile.BindSP(this, &SArmySavePlan::ResUploadPlanFile);

	// 执行请求
    FAliyunOss::Get().PutObject(EOssDir::OD_Picture, HomeThumbnailFilename, HomeThumbnailData, OnUploadHomeThumbnail);
	FAliyunOss::Get().PutObject(EOssDir::OD_Picture, PlanThumbnailFilename, PlanThumbnailData, OnUploadPlanThumbnail);
	FAliyunOss::Get().PutObject(EOssDir::OD_Resource, HomeFilename, HomeData, OnUploadHomeFile);
	FAliyunOss::Get().PutObject(EOssDir::OD_Resource, PlanFilename, PlanData, OnUploadPlanFile);

    TWeakPtr<FArmyReferenceImage> Facsimile = FArmySceneData::Get()->GetFacsimile();
    if (Facsimile.IsValid())
    {
        FOnUploadComplete OnUploadFacsimile;
        OnUploadFacsimile.BindSP(this, &SArmySavePlan::ResUploadFacsimile);
        FAliyunOss::Get().PutObject(
            EOssDir::OD_Picture, Facsimile.Pin()->GetFilename(), Facsimile.Pin()->GetData(), OnUploadFacsimile);
    }
    else
    {
        bUploadFacsimileReady = true;
    }

	GGI->Window->ShowThrobber(MAKE_TEXT("保存方案中..."));
}

void SArmySavePlan::ReqSavePlan()
{
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);

	// 开始写入方案
	JsonWriter->WriteObjectStart();

	JsonWriter->WriteValue("id", FArmyUser::Get().GetCurPlanID());
	JsonWriter->WriteValue("projectId", FArmyUser::Get().GetCurProjectID());
	JsonWriter->WriteValue("fileVersion", GResourceVersion);
	JsonWriter->WriteValue("name", FArmyUser::Get().CurPlanData->Name);
	JsonWriter->WriteValue("fileUrl", *PlanFileUrl);
	JsonWriter->WriteValue("fileMd5", *PlanFileMD5);
	JsonWriter->WriteValue("thumbnail", *PlanThumbnailUrl);

	// 开始写入户型
	JsonWriter->WriteObjectStart("home");

	JsonWriter->WriteValue("fileVersion", GResourceVersion);
	JsonWriter->WriteValue("fileUrl", *HomeFileUrl);
	JsonWriter->WriteValue("fileMd5", *HomeFileMD5);
    JsonWriter->WriteValue("thumbnail", *HomeThumbnailUrl);
    JsonWriter->WriteValue("background", *FacsimileUrl);
	JsonWriter->WriteValue("bedroom", FCString::Atoi(*ETB_Bedroom->GetText().ToString()));
	JsonWriter->WriteValue("livingRoom", FCString::Atoi(*ETB_LivingRoom->GetText().ToString()));
	JsonWriter->WriteValue("kitchen", FCString::Atoi(*ETB_Kitchen->GetText().ToString()));
	JsonWriter->WriteValue("bathroom", FCString::Atoi(*ETB_Bathroom->GetText().ToString()));

	/** @欧石楠 计算套内面积，规则是原始户型下全部房间的面积之和*/
	TArray<TWeakPtr<FArmyObject>> RoomList;

    float TotalInnerArea = 0.f;
	TArray<FVector> TempRoomPoints = CalcInnerArea();
    if (TempRoomPoints.Num() > 0)
    {
        TArray<FVector> PointList = FArmyMath::Extrude3D(TempRoomPoints, FArmySceneData::OutWallThickness / 10.f);
        TotalInnerArea = FArmyMath::CalcPolyArea(PointList) * 0.0001f;
    }

	JsonWriter->WriteValue("innerArea", TotalInnerArea);
	

	// 结束写入户型
	JsonWriter->WriteObjectEnd();

	/*@梁晓菲 写入房间在方案内的唯一标志*/
	JsonWriter->WriteArrayStart(TEXT("vrSpaceList"));

	RoomList.Empty();
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (FObjectWeakPtr It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("vrSpaceId", Room->GetUniqueIdOfRoom());
		JsonWriter->WriteValue("spaceId", Room->GetSpaceId());
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//写入方案户型信息
	JsonWriter->WriteArrayStart("spaceList");
	RoomList.Empty();
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		float SpaceArea, SpacePerimeter;
		Room->GetRoomAreaAndPerimeter(SpaceArea, SpacePerimeter);
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("spaceName"), Room->GetSpaceName());
		JsonWriter->WriteValue(TEXT("spaceArea"), SpaceArea);
		JsonWriter->WriteValue(TEXT("spacePerimeter"), SpacePerimeter);
		JsonWriter->WriteValue(TEXT("spaceStoreyHeight"), FArmySceneData::Get()->WallHeight / 100.f);
		JsonWriter->WriteValue(TEXT("spaceType"), 0);
		JsonWriter->WriteObjectEnd();
	}
	RoomList.Empty();
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		float SpaceArea, SpacePerimeter;
		Room->GetRoomAreaAndPerimeter(SpaceArea, SpacePerimeter);
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("spaceName"), Room->GetSpaceName());
		JsonWriter->WriteValue(TEXT("spaceArea"), SpaceArea);
		JsonWriter->WriteValue(TEXT("spacePerimeter"), SpacePerimeter);
		JsonWriter->WriteValue(TEXT("spaceStoreyHeight"), FArmySceneData::Get()->WallHeight / 100.f);
		JsonWriter->WriteValue(TEXT("spaceType"), 1);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("doorOpeningList");
	TArray<TWeakPtr<FArmyObject>> DoorList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, DoorList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, DoorList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Pass, DoorList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_DoorHole, DoorList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, DoorList);
	for (auto It : DoorList)
	{
		TSharedPtr<FArmyHardware> Door = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("doorOpeningHeight"), Door->GetHeight() * 10);
		JsonWriter->WriteValue(TEXT("doorOpeningWidth"), Door->GetLength() * 10);
		JsonWriter->WriteValue(TEXT("wallThickness"), Door->GetWidth() * 10);
		if (Door->FirstRelatedRoom.IsValid())
		{
			JsonWriter->WriteValue(TEXT("adjacentSpaceOneId"), Door->FirstRelatedRoom->GetSpaceId());
			JsonWriter->WriteValue(TEXT("adjacentSpaceNameOne"), Door->FirstRelatedRoom->GetSpaceName());
		}
		if (Door->SecondRelatedRoom.IsValid())
		{
			JsonWriter->WriteValue(TEXT("adjacentSpaceTwoId"), Door->SecondRelatedRoom->GetSpaceId());
			JsonWriter->WriteValue(TEXT("adjacentSpaceNameTwo"), Door->SecondRelatedRoom->GetSpaceName());
		}
		if (Door->GetType() == OT_Pass)
		{
			TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(It.Pin());
			if (Pass->GetIfGeneratePassModel())
				JsonWriter->WriteValue(TEXT("hemming"), 1);
			else
				JsonWriter->WriteValue(TEXT("hemming"), 0);

			JsonWriter->WriteValue(TEXT("hasDoor"), 0);
		}
		else
		{
			JsonWriter->WriteValue(TEXT("hemming"), 0);
			JsonWriter->WriteValue(TEXT("hasDoor"), 1);
		}
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("windowList");
	TArray<TWeakPtr<FArmyObject>> WindowList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, WindowList);
	for (auto It : WindowList)
	{
		TSharedPtr<FArmyHardware> Window = StaticCastSharedPtr<FArmyHardware>(It.Pin());
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("windowHeight"), Window->GetHeight() * 10);
		JsonWriter->WriteValue(TEXT("windowWidth"), Window->GetLength() * 10);
		JsonWriter->WriteValue(TEXT("windowDepth"), Window->GetWidth() * 10);
		JsonWriter->WriteValue(TEXT("groundHeight"), Window->GetHeightToFloor() * 10);
		JsonWriter->WriteValue(TEXT("wallThickness"), Window->GetWidth() * 10);
		if (Window->FirstRelatedRoom.IsValid())
		{
			JsonWriter->WriteValue(TEXT("adjacentSpaceOneId"), Window->FirstRelatedRoom->GetSpaceId());
			JsonWriter->WriteValue(TEXT("adjacentSpaceNameOne"), Window->FirstRelatedRoom->GetSpaceName());
		}
		if (Window->SecondRelatedRoom.IsValid())
		{
			JsonWriter->WriteValue(TEXT("adjacentSpaceTwoId"), Window->SecondRelatedRoom->GetSpaceId());
			JsonWriter->WriteValue(TEXT("adjacentSpaceNameTwo"), Window->SecondRelatedRoom->GetSpaceName());
		}
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 结束写入方案
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	//如果之前请求残留，取消
	if (SubmitSaveRequest.IsValid())
	{
		SubmitSaveRequest->OnProcessRequestComplete().Unbind();
		SubmitSaveRequest->CancelRequest();
		SubmitSaveRequest = nullptr;
	}

	FString Url = TEXT("/api/plans");
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmySavePlan::ResSavePlan);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPutRequest(Url, CompleteDelegate, JStr);
    Request->ProcessRequest();
}

void SArmySavePlan::ResSavePlan(FArmyHttpResponse Response)
{
	GGI->Window->HideThrobber();

    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject>* JData = nullptr;
        if (Response.Data->TryGetObjectField("data", JData))
        {
            const TSharedPtr<FJsonObject>* HomeObj = nullptr;
            if ((*JData)->TryGetObjectField("home", HomeObj))
            {
                FArmyUser::Get().CurHomeData = FArmyCommonTools::ParseContentItemFromJson(*HomeObj);

                // 先删除原来匹配该ID的文件
                FArmyCommonTools::DeleteFiles(FArmyUser::Get().GetCurHomeID(), EResourceType::Home);

                // 拿到新的文件ID，直接把数据流文件转存到本地，就不需要下载了
                FString HomeFilePath = FString::Printf(TEXT("%s/%d_%s.json"), *FResTools::GetDrawHomeDir(), FArmyUser::Get().GetCurHomeID(), *HomeFileMD5);
                FFileHelper::SaveArrayToFile(HomeData, *HomeFilePath);
            }

            FArmyUser::Get().CurPlanData = FArmyCommonTools::ParseContentItemFromJson(*JData);

            // 先删除原来匹配该ID的文件
            FArmyCommonTools::DeleteFiles(FArmyUser::Get().GetCurPlanID(), EResourceType::Plan);

            // 拿到新的文件ID，直接把数据流文件转存到本地，就不需要下载了
            FString PlanFilePath = FString::Printf(TEXT("%s/%d_%s.json"), *FResTools::GetLevelPlanDir(), FArmyUser::Get().GetCurPlanID(), *PlanFileMD5);
            FFileHelper::SaveArrayToFile(PlanData, *PlanFilePath);
        }

        GGI->Window->DismissModalDialog();
        GGI->Window->ShowMessage(MT_Success, TEXT("方案保存成功"));
    }
    else
    {
        if (Response.Message.IsEmpty())
        {
            GGI->Window->ShowMessage(MT_Warning, TEXT("方案保存失败"));
        }
        else
        {
            GGI->Window->ShowMessage(MT_Warning, Response.Message);
        }
    }

    Reset();
}

void SArmySavePlan::ResUploadHomeThumbnail(TSharedPtr<FAliyunOssResponse> Response)
{
    if (Response->bWasSuccessful)
    {
        bUploadHomeThumbnailReady = true;
        HomeThumbnailUrl = Response->CdnUrl;
    }
    else
    {
        bUploadHomeThumbnailReady = false;
        GGI->Window->HideThrobber();
        GGI->Window->ShowMessage(MT_Warning, TEXT("保存失败，请检查网络"));
    }

    CheckToSave();
}

void SArmySavePlan::ResUploadPlanThumbnail(TSharedPtr<FAliyunOssResponse> Response)
{
	if (Response->bWasSuccessful)
	{
		bUploadPlanThumbnailReady = true;
		PlanThumbnailUrl = Response->CdnUrl;
	}
    else
    {
        bUploadPlanThumbnailReady = false;
        GGI->Window->HideThrobber();
        GGI->Window->ShowMessage(MT_Warning, TEXT("保存失败，请检查网络"));
    }

    CheckToSave();
}

void SArmySavePlan::ResUploadHomeFile(TSharedPtr<FAliyunOssResponse> Response)
{
	if (Response->bWasSuccessful)
	{
		bUploadHomeFileReady = true;
		HomeFileUrl = Response->CdnUrl;
	}
    else
    {
        bUploadHomeFileReady = false;
        GGI->Window->HideThrobber();
        GGI->Window->ShowMessage(MT_Warning, TEXT("保存失败，请检查网络"));
    }

    CheckToSave();
}

void SArmySavePlan::ResUploadPlanFile(TSharedPtr<FAliyunOssResponse> Response)
{
	if (Response->bWasSuccessful)
	{
		bUploadPlanFileReady = true;
		PlanFileUrl = Response->CdnUrl;
	}
    else
    {
        bUploadPlanFileReady = false;
        GGI->Window->HideThrobber();
        GGI->Window->ShowMessage(MT_Warning, TEXT("保存失败，请检查网络"));
    }

    CheckToSave();
}

void SArmySavePlan::ResUploadFacsimile(TSharedPtr<FAliyunOssResponse> Response)
{
    if (Response->bWasSuccessful)
    {
        bUploadFacsimileReady = true;
        FacsimileUrl = Response->CdnUrl;
    }
    else
    {
        bUploadFacsimileReady = false;
        GGI->Window->HideThrobber();
        GGI->Window->ShowMessage(MT_Warning, TEXT("保存失败，请检查网络"));
    }

    CheckToSave();
}

void SArmySavePlan::CheckToSave()
{
	if (bUploadHomeThumbnailReady &&
        bUploadPlanThumbnailReady &&
        bUploadHomeFileReady &&
        bUploadPlanFileReady &&
        bUploadFacsimileReady)
	{
		ReqSavePlan();
	}
}

TArray<FVector> SArmySavePlan::CalcInnerArea()
{
    TArray<FVector> UseOutLinePoints;

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);

    if (RoomList.Num() <= 0)
    {
        return UseOutLinePoints;
    }

	//根据布尔运算找出&的多边形区域，再挤出
	Clipper c;
	TArray<FVector> OutLinePoints;
	Paths PrePointsPath;	

	int WallThickness = FArmySceneData::OutWallThickness / 10;
	TSharedPtr<FArmyRoom> Room0 = StaticCastSharedPtr<FArmyRoom>(RoomList.Pop().Pin());
	TArray<FVector> PointListPre = FArmyMath::Extrude3D(Room0->GetWorldPoints(true), -WallThickness);

	int32 Index = RoomList.Num() - 1;
	while (RoomList.Num() > 0 && Index >= 0)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[Index].Pin());
		TArray<FVector> PointListCurrent = FArmyMath::Extrude3D(Room->GetWorldPoints(true), -WallThickness);

		if (FArmyMath::ArePolysIntersection(PointListPre, PointListCurrent, true))
		{
			Paths ClipperPath;
			Path RoomPath;
			for (int i = 0; i < PointListPre.Num(); i++)
			{
				RoomPath.push_back(IntPoint(PointListPre[i].X * 1000, PointListPre[i].Y * 1000));
			}
			ClipperPath.push_back(RoomPath);
			c.AddPaths(ClipperPath, ptSubject, true);

			ClipperPath.clear();
			RoomPath.clear();
			for (int i = 0; i < PointListCurrent.Num(); i++)
			{
				RoomPath.push_back(IntPoint(PointListCurrent[i].X * 1000, PointListCurrent[i].Y * 1000));
			}
			ClipperPath.push_back(RoomPath);
			c.AddPaths(ClipperPath, ptClip, true);

			Paths ResultPoints;
			if (c.Execute(ctUnion, ResultPoints, pftEvenOdd, pftEvenOdd))
			{
				OutLinePoints.Empty();
				Path ResultPath = ResultPoints[0];
				for (auto& It : ResultPath)
				{
					OutLinePoints.Push(FVector(It.X / 1000.f, It.Y / 1000.f, 0));
				}

				PointListPre = OutLinePoints;
				c.Clear();
			}

			RoomList.RemoveAt(Index);
			Index = RoomList.Num() - 1;
		}
		else
		{
			--Index;
		}
	}

	if (RoomList.Num() == 0)
	{
		OutLinePoints = PointListPre;
	}

	int32 IndexTag = 0;
	//重新排序，将小于50mm的点放到第一个
	for (int i = 0; i < OutLinePoints.Num(); i++)
	{
		float LineLength = (OutLinePoints[i] - OutLinePoints[i == OutLinePoints.Num() - 1 ? 0 : i + 1]).Size();
		if (LineLength < 5)
		{
			IndexTag = i;
			break;
		}
		else
			UseOutLinePoints.Push(OutLinePoints[i]);
	}
	//将小于50mm的外墙线去掉
	if (IndexTag > 0)
	{
		int32 NextIndex = 0;
		for (int i = IndexTag; i < OutLinePoints.Num(); i++)
		{
			UseOutLinePoints.Insert(OutLinePoints[i], NextIndex++);
		}

		if (UseOutLinePoints.Num() > 3)
		{
			FVector PreLine = UseOutLinePoints[0] - UseOutLinePoints[UseOutLinePoints.Num() - 1];
			FVector NextLine = UseOutLinePoints[2] - UseOutLinePoints[3];
			FVector2D IntersectePoint2D;
			if (FArmyMath::Line2DIntersection(FVector2D(UseOutLinePoints[0]), FVector2D(UseOutLinePoints[UseOutLinePoints.Num() - 1]),
				FVector2D(UseOutLinePoints[2]), FVector2D(UseOutLinePoints[3]), IntersectePoint2D))
			{
				FVector IntersectePoint(IntersectePoint2D, UseOutLinePoints[0].Z);
				UseOutLinePoints.RemoveAt(0, 3);
				UseOutLinePoints.Insert(IntersectePoint, 0);
			}
		}
	}
	else
	{
		UseOutLinePoints = OutLinePoints;
	}		

	return UseOutLinePoints;
}
