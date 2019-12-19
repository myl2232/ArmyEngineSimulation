#include "SArmyOpenPlan.h"
#include "SBoxPanel.h"
#include "SEditableTextBox.h"
#include "ArmyStyle.h"
#include "SBox.h"
#include "ArmyUser.h"
#include "ArmyHttpModule.h"
#include "Json.h"
#include "SImage.h"
#include "SButton.h"
#include "SOverlay.h"
#include "SScrollWrapBox.h"
#include "ArmyCommonTools.h"
#include "ArmyEngineTools.h"
#include "SContentItem.h"
#include "ArmyGameInstance.h"
#include "ArmyDownloadModule.h"
#include "ArmyDesignEditor.h"
#include "ArmyResourceModule.h"
#include "FileManager.h"

void SArmyOpenPlan::Construct(const FArguments & InArgs)
{
    TempHomeData = nullptr;
    TempPlanData = nullptr;

	ChildSlot
	[
		SNew(SVerticalBox)

		+SVerticalBox::Slot()
		.Padding(20, 20, 20, 16)
		.AutoHeight()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		[			
			//下拉选择项目
			CreateProjectComboWidget()
		]

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0)
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Splitter.FF343538"))
		]

		+ SVerticalBox::Slot()
		.Padding(20, 20, 0, 20)
		[
			//方案列表面板
			CreatePlanListWidget()
		]
	];
	Init();
}

void SArmyOpenPlan::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    GGI->DesignEditor->UpdateSyncProgress();
}

void SArmyOpenPlan::OnConfirmClicked()
{
    OpenPlan(SelectedPlanIndex);
}

void SArmyOpenPlan::OnGreatPlanConfirmClicked()
{
	GreatPlanOpenPlan(SelectedPlanIndex);
}

void SArmyOpenPlan::Init()
{
	ReqGetProjectList();
}

void SArmyOpenPlan::ReqGetProjectList()
{	
	PlanUIList.Reset();	
	SelectedProjectID = 0;
	SelectedPlanIndex = 0;

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyOpenPlan::ResGetProjectList);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/no-paging/projects", CompleteDelegate);
    Request->ProcessRequest();
}

void SArmyOpenPlan::ResGetProjectList(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TArray<TSharedPtr<FJsonValue>> JArray = Response.Data->GetArrayField("data");
        ProjectList.Reset();
        for (auto& ArrayIt : JArray)
        {
            TSharedPtr<FJsonObject> JObject = ArrayIt->AsObject();
            int32 Id = JObject->GetIntegerField("id");
            FString Name = JObject->GetStringField("name");
            ProjectList.Add(MakeShareable(new FArmyKeyValue(Id, Name)));
        }
    }

	if (ProjectList.Array.Num() > 0)
    {
		SelectedProjectName = ProjectList.Array[0]->Value;
		SelectedProjectID = ProjectList.Array[0]->Key;
		TSharedPtr<FArmyKeyValue> TempKV = ProjectList.FindByKey(FArmyUser::Get().GetCurProjectID());
		if (TempKV.IsValid())
        {
			SelectedProjectName = TempKV->Value;
			SelectedProjectID = TempKV->Key;			
		}
		ReqGetPlanList(SelectedProjectID);
	}
}

void SArmyOpenPlan::ReqGetPlanList(const int32 & ProjectId)
{
	PlanListWidget->ClearChildren();
	PlanUIList.Reset();

    FString Url = FString::Printf(TEXT("/api/projects/%d/plans"), ProjectId);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyOpenPlan::ResGetPlanList);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void SArmyOpenPlan::ResGetPlanList(FArmyHttpResponse Response)
{
	PlanIdList.Empty();

    if (Response.bWasSuccessful)
    {
        PlanDataList.Reset();

        const TArray< TSharedPtr<FJsonValue> > PlanList = Response.Data->GetArrayField("data");

        TArray<FContentItemPtr> TempPlanList;
        for (auto& PlanIt : PlanList)
        {
            //获取方案ID列表
            TSharedPtr<FJsonObject> DataObject = PlanIt->AsObject();
            PlanIdList.Add(DataObject->GetIntegerField("id"));

            FContentItemPtr ParsedContentItem = FArmyCommonTools::ParseContentItemFromJson(PlanIt->AsObject());
            TempPlanList.Add(ParsedContentItem);
            PlanDataList.Add(ParsedContentItem);
        }
        FillPlanList(TempPlanList);
    }
}

void SArmyOpenPlan::FillPlanList(TArray<FContentItemPtr> ItemList)
{
	for (int32 i = 0; i < ItemList.Num(); i++)
	{
        TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(i)
            .bHoverThumbnailOnly(true)
            .bShowDownloadTips(false)
            .ItemWidth(200)
            .ItemHeight(240)
            .ThumbnailPadding(0)
            .ThumbnailURL(ItemList[i]->ThumbnailURL)
            .ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage")))
            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
            .TextColor(FLinearColor::White)
            .DisplayText(FText::FromString(ItemList[i]->Name))
            .OnClicked(this, &SArmyOpenPlan::OnPlanSelected);

		PlanListWidget->AddItem(Item);
		PlanUIList.Add(Item);
	}

	if (PlanUIList.Num() > 0)
	{
		// @梁晓菲 如果当前Combobox选中的是当前项目，那么默认选择的方案是当前方案
		if (SelectedProjectID == FArmyUser::Get().GetCurProjectID())
		{
			for (int i = 0; i < PlanIdList.Num(); i++)
			{
				if (PlanIdList[i] == FArmyUser::Get().GetCurPlanID())
				{
					PlanUIList[i]->SetSelected(true);
				}
			}
		}
		// 如果不是当前项目，默认选择第一个方案
		else
		{
			PlanUIList[0]->SetSelected(true);
		}
	}
}

void SArmyOpenPlan::OnPlanSelected(int32 PlanListIndex)
{
	for (auto& It : PlanUIList)
	{
		It->SetSelected(false);
	}
	PlanUIList[PlanListIndex]->SetSelected(true);

	SelectedPlanIndex = PlanListIndex;
}

void SArmyOpenPlan::ReqGetSearchProjectList()
{
}

void SArmyOpenPlan::ResGetSearchProjectList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
}

FReply SArmyOpenPlan::OnSearchClicked()
{
	if (ETB_SearchTextBox->GetText().ToString().TrimStart() != "") {
		ReqGetSearchProjectList();
	}
	return FReply::Handled();
}

void SArmyOpenPlan::OnSearchTextCommited(const FText & _NewText, ETextCommit::Type _Committype)
{
	OnSearchClicked();
}

void SArmyOpenPlan::ReqHome(const int32& PlanId)
{
    FString Url = FString::Printf(TEXT("/api/plans/%d/homes"), PlanId);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyOpenPlan::ResHome);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void SArmyOpenPlan::ResHome(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        // 记录当前户型数据
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");
        const TSharedPtr<FJsonObject> Resource = JData->GetObjectField("resource");

        /*@梁晓菲 从提示框打开方案的时候，需要把房间唯一ID的最大值加载进来*/
        FArmyUser::Get().RoomUniqueId = Resource->GetIntegerField("maxVrSpaceId");

        TempHomeData = FArmyCommonTools::ParseContentItemFromJson(JData);

        TArray< TSharedPtr<FContentItemSpace::FResObj> > HomeResArr = TempHomeData->GetResObjNoComponent();
        TArray< TSharedPtr<FContentItemSpace::FResObj> > PlanResArr = TempPlanData->GetResObjNoComponent();
        if (HomeResArr.Num() <= 0 || PlanResArr.Num() <= 0)
        {
            return;
        }

        // 下载户型文件和方案文件
        TArray<FDownloadSpace::FDownloadFileInfo> TaskInfos;
        TaskInfos.Add(FDownloadSpace::FDownloadFileInfo(
            TempHomeData->ID, HomeResArr[0]->FilePath, HomeResArr[0]->FileURL, HomeResArr[0]->FileMD5));
        TaskInfos.Add(FDownloadSpace::FDownloadFileInfo(
            TempPlanData->ID, PlanResArr[0]->FilePath, PlanResArr[0]->FileURL, PlanResArr[0]->FileMD5));

        // @欧石楠 下载底图
        TSharedPtr<FContentItemSpace::FArmyHomeRes> HomeRes = StaticCastSharedPtr<FContentItemSpace::FArmyHomeRes>(HomeResArr[0]);
        if (HomeRes.IsValid() && !HomeRes->FacsimileUrl.IsEmpty())
        {
            int32 SlashPos = HomeRes->FacsimileUrl.Find(TEXT("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
            FString FacsimileFilename = HomeRes->FacsimileUrl.Right(HomeRes->FacsimileUrl.Len() - 1 - SlashPos);
            FString FacsimileFilePath = FArmyCommonTools::GetPathFromFileName(EResourceType::Facsimile, FacsimileFilename);
            // @欧石楠 下载之前先删除之前的底图，因为同一方案底图只能有一份
            IFileManager::Get().Delete(*FacsimileFilePath, true, true);
            TaskInfos.Add(FDownloadSpace::FDownloadFileInfo(-1, FacsimileFilePath, HomeRes->FacsimileUrl, "", true));
        }

        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(TaskInfos);
        GGI->DesignEditor->OnSyncFinished = FBoolDelegate::CreateSP(this, &SArmyOpenPlan::LoadPlan);
        NewTask->OnDownloadFinished.BindSP(this, &SArmyOpenPlan::OnPlanReady, PlanResArr[0]->FilePath);
    }
}

void SArmyOpenPlan::OpenPlan(int32 PlanListIndex)
{
	if (PlanDataList.Num() == 0)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("方案为空"));
		return;
	}
    // 如果打开的方案和当前方案ID一致，则什么都不做
    TempPlanData = PlanDataList[PlanListIndex];
    if (TempPlanData->ID == FArmyUser::Get().GetCurPlanID())
    {
        GGI->Window->DismissModalDialog();
        return;
    }

	SelectedPlanIndex = PlanListIndex;

    GGI->Window->ShowThrobber(MAKE_TEXT("载入方案中..."));

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = TempPlanData->GetResObjNoComponent();
    // 如果方案文件为空，则无需下载，直接载入
    if (resArr.Num() > 0 && resArr[0]->FileURL.Equals(""))
    {
        LoadPlan(true);
    }
    else
    {
        // 请求户型数据
        ReqHome(TempPlanData->ID);
    }
}

void SArmyOpenPlan::GreatPlanOpenPlan(int32 PlanListIndex)
{
	if (PlanDataList.Num() == 0)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("方案为空"));
		return;
	}
	// 如果打开的方案和当前方案ID一致，则什么都不做
	TempPlanData = PlanDataList[PlanListIndex];
	if (TempPlanData->ID == FArmyUser::Get().GetCurPlanID())
	{
		GGI->Window->DismissModalDialog();
		return;
	}

	SelectedPlanIndex = PlanListIndex;

	GGI->Window->ShowThrobber(MAKE_TEXT("载入方案中..."));

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = TempPlanData->GetResObjNoComponent();
	// 如果方案文件为空，则无需下载，直接载入
	if (resArr.Num() > 0 && resArr[0]->FileURL.Equals(""))
	{
		LoadPlan(true);
	}
	else
	{
		// 请求户型数据
		ReqHome(TempPlanData->ID);
	}
}

void SArmyOpenPlan::OnPlanReady(bool bWasSucceed, FString FilePath)
{
    if (bWasSucceed)
    {
        GGI->DesignEditor->ReqSyncPlan(FilePath);
    }
}

void SArmyOpenPlan::LoadPlan(bool bWasSucceed)
{
    if (bWasSucceed)
    {
        // 解决打开不同项目墙顶地材质丢失的bug
        FArmySceneData::Get()->LoadDataFromJason = true;

		// 加载默认资源
        GGI->DesignEditor->LoadCommonResource();
        // 数据下载完成，将解析的数据存储起来
        FArmyResourceModule::Get().GetResourceManager()->AppendSynList(GGI->DesignEditor->SynContentItems);

        FArmyUser::Get().CurHomeData = TempHomeData;
        FArmyUser::Get().CurPlanData = TempPlanData;
        // 加载户型数据
		if (TempHomeData.IsValid())
		{
			TArray<TSharedPtr<FContentItemSpace::FResObj> >homeResArr = TempHomeData->GetResObjNoComponent();
			if (homeResArr.Num() > 0 && !homeResArr[0]->FileURL.Equals(""))
			{
				GGI->DesignEditor->LoadHome(homeResArr[0]->FilePath);
			}
			else
			{
				GGI->DesignEditor->LoadHome("");
			}
		}
		else
		{
			GGI->DesignEditor->LoadHome("");
		}

        // 加载方案数据
		if (TempPlanData.IsValid())
		{
			TArray<TSharedPtr<FContentItemSpace::FResObj> >PlanResArr = TempPlanData->GetResObjNoComponent();
			if (PlanResArr.Num() > 0 && !PlanResArr[0]->FileURL.Equals(""))
			{
				GGI->DesignEditor->LoadPlan(PlanResArr[0]->FilePath);
			}
			else
			{
				GGI->DesignEditor->LoadPlan("");
			}
		}

        GGI->Window->DismissModalDialog();

		// @梁晓菲 根据项目ID获取项目详情，设置当前方案FArmyUser::Get().CurProjectData
		ReqProject();
    }
    else
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("方案加载失败"));
    }

    GGI->Window->HideThrobber();
}

void SArmyOpenPlan::ReqProject()
{
	//FString JStr;
	//TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);		
	//
	//JsonWriter->WriteObjectStart();
	//JsonWriter->WriteValue("projectId", SelectedProjectID);
	//JsonWriter->WriteObjectEnd();
	//JsonWriter->Close();

    FString Url = FString::Printf(TEXT("/api/projects/%d"), SelectedProjectID);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &SArmyOpenPlan::ResProject);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate/*, JStr*/);
    Request->ProcessRequest();
}

void SArmyOpenPlan::ResProject(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");

        FContentItemPtr ParsedContentItem = FArmyCommonTools::ParseContentItemFromJson(JData);
        FArmyUser::Get().CurProjectData = ParsedContentItem;

        // 记录当前套餐ID
        TArray<TSharedPtr<FContentItemSpace::FResObj> > ResArr = ParsedContentItem->GetResObjNoComponent();
        if (ResArr.Num() >= 1)
        {
            TSharedPtr<FContentItemSpace::FProjectRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FProjectRes>(ResArr[0]);
            if (ProRes.IsValid())
            {
                FArmyUser::Get().SetMealID(ProRes->SetMealId);
            }
        }
    }
    else
    {
        if (Response.Message.IsEmpty())
        {
            GGI->Window->ShowMessage(MT_Warning, TEXT("方案打开失败"));
        }
        else
        {
            GGI->Window->ShowMessage(MT_Warning, Response.Message);
        }
    }
}

TSharedRef<SWidget> SArmyOpenPlan::CreateProjectComboWidget()
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
				.Text(FText::FromString(TEXT("选择项目")))
				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
				.Justification(ETextJustify::Center)
			]
		]
		+ SHorizontalBox::Slot()		
		.AutoWidth()
		.HAlign(EHorizontalAlignment::HAlign_Left)
		[
			SAssignNew(ProjectComboBox, SComboBox< TSharedPtr<FArmyKeyValue> >)
			.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.None"))
			.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.PropertyCombobox"))
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("ComboBox.Button.FF212224"))
			.OptionsSource(&ProjectList.Array)
			.MaxListHeight(200.f)
			.OnGenerateWidget(this, &SArmyOpenPlan::OnGenerateComoboWidget)
			.OnSelectionChanged(this, &SArmyOpenPlan::OnSlectedProjectChanged)
			.Content()
			[
				SNew(SBox)
				.WidthOverride(360.f)
				.HeightOverride(32.f)
				.VAlign(VAlign_Center)
				[
					SNew(SHorizontalBox)

					+SHorizontalBox::Slot()
					[
						SNew(STextBlock)
						.TextStyle(FArmyStyle::Get(), "ArmyText_12")
						.Text(this, &SArmyOpenPlan::GetSelectedProject)
					]
	
					+SHorizontalBox::Slot()
					.HAlign(EHorizontalAlignment::HAlign_Right)
					.Padding(FMargin(0,0,0,0))
					.AutoWidth()
					[
						SNew(SImage)
						.Image(FArmyStyle::Get().GetBrush("Icon.DownArrow_Gray"))
					]
				]
			]
		];
}

TSharedRef<SWidget> SArmyOpenPlan::CreateSearchWidget()
{
	return 
		SNew(SBox)
		.WidthOverride(224)
		.HeightOverride(35)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(ETB_SearchTextBox, SEditableTextBox)
				.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox.Gray"))
				.HintText(FText::FromString(TEXT("输入搜索内容")))
				.OnTextCommitted(this, &SArmyOpenPlan::OnSearchTextCommited)
			]
			+ SOverlay::Slot()
			.Padding(0, 0, 5, 0)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(35)
				.HeightOverride(35)
				[
					SNew(SButton)
					.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Search"))
					.OnClicked(this, &SArmyOpenPlan::OnSearchClicked)
				]
			]
		];
}

TSharedRef<SWidget> SArmyOpenPlan::CreatePlanListWidget()
{
	return 
		SNew(SBox)
		.WidthOverride(876)
		.HeightOverride(408)
		[
			SAssignNew(PlanListWidget, SScrollWrapBox)
			.InnerSlotPadding(FVector2D(16, 17))
		];
}

void SArmyOpenPlan::OnSlectedProjectChanged(TSharedPtr<FArmyKeyValue> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (NewSelection.IsValid()) {
		SelectedProjectName = NewSelection->Value;
		SelectedProjectID = NewSelection->Key;		
		ReqGetPlanList(SelectedProjectID);
	}
}

TSharedRef<SWidget> SArmyOpenPlan::OnGenerateComoboWidget(TSharedPtr<FArmyKeyValue> InItem)
{
	return
		SNew(SBox)
		.WidthOverride(80.f)
		.HeightOverride(32.f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(10, 0, 0, 0))
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InItem->Value))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
			]
		];
}