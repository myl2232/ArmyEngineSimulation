#include "ArmyProjectController.h"
#include "ArmyStyle.h"
#include "ArmyUser.h"
#include "SContentItem.h"
#include "ArmyCommonTools.h"
#include "ArmyEngineTools.h"
#include "SArmyNewProject.h"
#include "SArmySystemTitleBar.h"
#include "DownloadFileSet.h"
#include "ArmyDownloadModule.h"
#include "SArmyNewPlan.h"
#include "ArmyGameInstance.h"
#include "ArmyResourceModule.h"
#include "SArmyImageTextButton.h"

#include "SImage.h"
#include "IArmyHttp.h"
#include "ArmyHttpModule.h"

#include "SArmyGreatPlanList.h"
#include "FileManager.h"

const FString FArmyProjectController::MyProject = TEXT("我的项目");
const FString FArmyProjectController::GreatPlan = TEXT("大师方案");

using namespace FContentItemSpace;
using namespace FDownloadSpace;

FArmyProjectController::FArmyProjectController()
    : bClearProjectList(true)
    , PageIndex(-1)
    , PageSize(18)
	, SelectedProjectID(0)
    , TempHomeData(nullptr)
    , TempPlanData(nullptr)
{
}

void FArmyProjectController::Init()
{
    FArmyViewController::Init();

    // 设置系统标题栏样式
    GGI->Window->GetSystemTitleBar()->SetStyle(FArmySystemTitleBarStyle(true, false, true, 48));
    GGI->Window->GetSystemTitleBar()->ShowGotoHomePageButton(false);

    RequestProjectList();
}

TSharedPtr<SWidget> FArmyProjectController::MakeContentWidget()
{
//#if /*SERVER_MODE == DEBUG*/ -1  //@ 大师方案开发正在进行中，暂不上线
//	SAssignNew(ContentSwitcher,SWidgetSwitcher)
//		//我的项目
//		+SWidgetSwitcher::Slot()
//		[
//			SNew(SOverlay)
//			+ SOverlay::Slot()
//			.HAlign(HAlign_Fill)
//			.VAlign(VAlign_Fill)
//			[
//				SAssignNew(ProjectListWidget, SArmyProjectList)
//				.OnProjectListScrollToEnd_Raw(this, &FArmyProjectController::OnProjectListScrollToEnd)
//				.OnNewProjectClicked_Raw(this, &FArmyProjectController::OnNewProjectClicked)
//			]
//			+ SOverlay::Slot()
//			.HAlign(HAlign_Right)
//			.VAlign(VAlign_Fill)
//			[
//				SAssignNew(PlanListWidget, SArmyPlanList)
//				.Visibility(EVisibility::Collapsed)
//				.OnNewPlanClicked_Raw(this, &FArmyProjectController::OnNewPlanClicked)
//			]
//		]
//		//大师方案
//		+SWidgetSwitcher::Slot()
//		[
//			SNew(SOverlay)
//			+ SOverlay::Slot()
//			.HAlign(HAlign_Fill)
//			.VAlign(VAlign_Fill)
//			[
//				SAssignNew(GreatPlanListWidget, SArmyGreatPlanList)
//				/*.OnProjectListScrollToEnd_Raw(this, &FArmyProjectController::OnProjectListScrollToEnd)
//				.OnNewProjectClicked_Raw(this, &FArmyProjectController::OnNewProjectClicked)*/
//			]
//
//			+ SOverlay::Slot()
//			.HAlign(HAlign_Right)
//			.VAlign(VAlign_Fill)
//			[
//				SAssignNew(GreatPlanDetailWidget, SArmyGreatPlanDetail)
//				.Visibility(EVisibility::Collapsed)
//				//点击事件
//			]
//		];
//
//		//默认激活“我的项目”
//		OnMyProjectClicked();
//
//		return ContentSwitcher->GetWidget(0);
//#else  //@ 大师方案开发正在进行中，暂不上线
		return
        SNew(SOverlay)

        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(ProjectListWidget, SArmyProjectList)
            .OnProjectListScrollToEnd_Raw(this, &FArmyProjectController::OnProjectListScrollToEnd)
            .OnNewProjectClicked_Raw(this, &FArmyProjectController::OnNewProjectClicked)
        ]

        + SOverlay::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Fill)
        [
            SAssignNew(PlanListWidget, SArmyPlanList)
            .Visibility(EVisibility::Collapsed)
			.OnNewPlanClicked_Raw(this, &FArmyProjectController::OnNewPlanClicked)
        ];
//#endif
}

void FArmyProjectController::Tick(float DeltaSeconds)
{
    // 记录同步进度
    if (GGI->DesignEditor.IsValid())
    {
        GGI->DesignEditor->UpdateSyncProgress();
    }
}

//#if SERVER_MODE == DEBUG //@ 大师方案开发正在进行中，暂不上线
//TSharedPtr<SWidget> FArmyProjectController::MakeCustomTitleBarWidget()
//{
//	return SNew(SBox)
//		.HeightOverride(40)
//		[
//			SNew(SHorizontalBox)
//			+SHorizontalBox::Slot()
//			.HAlign(HAlign_Right)
//			.Padding(0,0,20,0)
//			[
//				SAssignNew(MyProjectWid, SArmyImageTextButton)
//				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
//				.Text(FText::FromString(MyProject))
//				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_18"))
//				.OnClicked_Raw(this, &FArmyProjectController::OnMyProjectClicked)
//				.OnHovered_Raw(this, &FArmyProjectController::OnMyProjectHovered)
//				.OnUnhovered_Raw(this, &FArmyProjectController::OnMyProjectUnHovered)
//			]
//			+ SHorizontalBox::Slot()
//			.HAlign(HAlign_Left)
//			.Padding(20, 0, 0, 0)
//			[
//				SAssignNew(GreatPlanWid, SArmyImageTextButton)
//				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
//				.Text(FText::FromString(GreatPlan))
//				.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_18"))
//				.OnClicked_Raw(this, &FArmyProjectController::OnGreatPlanClicked)
//				.OnHovered_Raw(this, &FArmyProjectController::OnGreatPlanHovered)
//				.OnUnhovered_Raw(this, &FArmyProjectController::OnGreatPlanUnHovered)
//			]
//		];
//}
//#endif

void FArmyProjectController::RequestProjectList()
{
    if (bClearProjectList)
    {
        PageIndex = 1;
        ProjectListWidget->EmptyProjectList();
        ProjectList.Reset();
        ProjectUIList.Reset();
    }

    FString Url = FString::Printf(TEXT("/api/projects?page=%d&rows=%d"), PageIndex, PageSize);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyProjectController::Callback_RequestProjectList);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyProjectController::Callback_RequestProjectList(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");
        JData->TryGetNumberField("totalPage", TotalPageNum);
        const TArray<TSharedPtr<FJsonValue>> JArray = JData->GetArrayField("list");

        TArray<FContentItemPtr> TempProjectList;

        for (auto& ArrayIt : JArray)
        {
            FContentItemPtr ParsedContentItem = FArmyCommonTools::ParseContentItemFromJson(ArrayIt->AsObject());
            TempProjectList.Add(ParsedContentItem);
            ProjectList.Add(ParsedContentItem);
        }

        FillProjectList(TempProjectList, true);
    }
    else
    {
        if (Response.Data.IsValid())
        {
            GGI->Window->ShowMessage(MT_Warning, Response.Data->GetStringField("message"));
        }
    }
}

void FArmyProjectController::OnProjectListScrollToEnd()
{
    bClearProjectList = false;

    if (++PageIndex <= TotalPageNum)
    {
        RequestProjectList();
    }
}

void FArmyProjectController::FillProjectList(TArray<FContentItemPtr> ItemList, bool ClearProjectList /*= true*/, bool bInSelectLast)
{
    int32 IndexOffset = ProjectUIList.Num();
	for (int32 i = 0; i < ItemList.Num(); i++)
	{
		TArray< TSharedPtr<FContentItemSpace::FResObj> > ResArr = ItemList[i]->GetResObjNoComponent();
		if (ResArr.Num() < 1)
			continue;
		TSharedPtr<FContentItemSpace::FProjectRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FProjectRes>(ResArr[0]);

        TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(IndexOffset + i)
            .bHoverThumbnailOnly(false)
            .bShowDownloadTips(false)
            .ItemWidth(224)
            .ItemHeight(300)
            .ThumbnailPadding(4)
            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_16"))
            .TextColor(FLinearColor::White)
            .ThumbnailURL(ItemList[i]->ThumbnailURL)
            .ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage")))
			.OnClicked_Raw(this, &FArmyProjectController::OnProjectSelected)
            .DisplayText(FText::FromString(ItemList[i]->Name));

        ProjectListWidget->AddProjectWidget(Item.ToSharedRef());
		ProjectUIList.Add(Item);
		if (bInSelectLast && i == ItemList.Num() - 1)
		{
			Item->SetSelected(true);
			OnProjectSelected(IndexOffset + i);
		}
	}
	//if (_ItemList.Num() > 0)
	//{
	//	TargetWidget->ResetScrollNotify();
	//}
}

void FArmyProjectController::RequestPlanList(const int32& ProjectId)
{
    PlanListWidget->Empty();

    FString Url = FString::Printf(TEXT("/api/projects/%d/plans"), ProjectId);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyProjectController::Callback_RequestPlanList);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();

	GGI->Window->ShowThrobber(MAKE_TEXT("方案加载中..."));
}

void FArmyProjectController::Callback_RequestPlanList(FArmyHttpResponse Response)
{
	GGI->Window->HideThrobber();
	PlanDataList.Reset();

    if (Response.bWasSuccessful)
    {
        const TArray< TSharedPtr<FJsonValue> > PlanList = Response.Data->GetArrayField("data");

        TArray<FContentItemPtr> TempPlanList;
        for (auto& PlanIt : PlanList)
        {
            FContentItemPtr ParsedContentItem = FArmyCommonTools::ParseContentItemFromJson(PlanIt->AsObject());
            TempPlanList.Add(ParsedContentItem);
            PlanDataList.Add(ParsedContentItem);
        }
        FillPlanList(TempPlanList);
    }
}

void FArmyProjectController::FillPlanList(TArray<FContentItemPtr> ItemList)
{
	for (int32 i = 0; i < ItemList.Num(); i++)
	{
        TSharedPtr<SWidget> PlanItem =
            SNew(SContentItem)
            /**@欧石楠 改为当前数组索引，便于获取整个content item*/
            .ItemCode(i)
            .bShowDownloadTips(false)
            .ItemWidth(268)
            .ItemHeight(300)
            .ThumbnailURL(ItemList[i]->ThumbnailURL)
            .ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage")))
            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_16"))
            .TextColor(FLinearColor::White)
            .DisplayText(FText::FromString(ItemList[i]->Name))
            .OnDoubleClicked_Raw(this, &FArmyProjectController::OpenPlan);

		PlanListWidget->AddPlanItem(PlanItem.ToSharedRef());
	}
}

void FArmyProjectController::ReqHome(const int32& PlanId)
{
    FString Url = FString::Printf(TEXT("/api/plans/%d/homes"), PlanId);
    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyProjectController::ResHome);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
    Request->ProcessRequest();
}

void FArmyProjectController::ResHome(FArmyHttpResponse Response)
{
    if (Response.bWasSuccessful)
    {
        // 记录当前户型数据
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");
        TempHomeData = FArmyCommonTools::ParseContentItemFromJson(JData);

        /*@梁晓菲 从菜单项打开方案的时候，需要把房间唯一ID的最大值加载进来*/
        JData->TryGetNumberField("maxVrSpaceId", FArmyUser::Get().RoomUniqueId);

        TArray< TSharedPtr<FResObj> > HomeResArr = TempHomeData->GetResObjNoComponent();
        TArray< TSharedPtr<FResObj> > PlanResArr = TempPlanData->GetResObjNoComponent();
        if (HomeResArr.Num() <= 0 || PlanResArr.Num() <= 0)
        {
            return;
        }

        // 下载户型文件和方案文件
        TArray<FDownloadFileInfo> TaskInfos;
        TaskInfos.Add(FDownloadFileInfo(
            TempHomeData->ID, HomeResArr[0]->FilePath, HomeResArr[0]->FileURL, HomeResArr[0]->FileMD5));
        TaskInfos.Add(FDownloadFileInfo(
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
        GGI->DesignEditor->OnSyncFinished = FBoolDelegate::CreateRaw(this, &FArmyProjectController::LoadPlan);
        NewTask->OnDownloadFinished.BindRaw(this, &FArmyProjectController::OnPlanReady, PlanResArr[0]->FilePath);
    }
}

FReply FArmyProjectController::OnNewProjectClicked()
{
    GGI->Window->PresentModalDialog(
        TEXT("新建项目"),
        SAssignNew(NewProjectDialog, SArmyNewProject),
        FSimpleDelegate::CreateRaw(this, &FArmyProjectController::RequestNewProject),false);

    return FReply::Handled();
}

void FArmyProjectController::OnProjectSelected(int32 ProjectId)
{
    for (auto& It : ProjectUIList)
    {
        It->SetSelected(false);
    }
    ProjectUIList[ProjectId]->SetSelected(true);
	SelectedProjectID = ProjectId;
	FArmyUser::Get().CurProjectData = ProjectList[SelectedProjectID];

	TArray< TSharedPtr<FContentItemSpace::FResObj> > ResArr = FArmyUser::Get().CurProjectData->GetResObjNoComponent();
    if (ResArr.Num() < 1)
    {
		return;
    }

    // 显示项目详情
    TSharedPtr<FContentItemSpace::FProjectRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FProjectRes>(ResArr[0]);
	PlanListWidget->SetProjectName(ProjectList[ProjectId]->Name);
    PlanListWidget->SetMealID(TEXT("所选套餐：") + ProRes->SetMealName);
    PlanListWidget->SetContractID(TEXT("项目编码：") + ProRes->Code);
    PlanListWidget->SetOwnerName(TEXT("业主姓名：") + ProRes->CustomerName);
    PlanListWidget->SetOwnerPhoneNumber(TEXT("业主电话：") + ProRes->CustomerPhone);
    PlanListWidget->SetLocation(TEXT("所在地区：") + ProRes->ProvinceName + TEXT(" ") + ProRes->CityName + TEXT(" ") + ProRes->AreaName);
    PlanListWidget->SetVillageName(TEXT("所在小区：") + ProRes->Villages);
    PlanListWidget->SetDetailAddress(TEXT("详细地址：") + ProRes->DetailAddress);
	PlanListWidget->SetVisibility(EVisibility::Visible);

    FArmyUser::Get().SetMealID(ProRes->SetMealId);

    // 请求方案列表
    RequestPlanList(FArmyUser::Get().CurProjectData->ID);
}

void FArmyProjectController::RequestNewProject()
{
	/**@欧石楠检查信息*/
	if (!NewProjectDialog->CheckIsValid())
    {
		return;
	}

    FString JStr;
    TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);
    JsonWriter->WriteObjectStart();
    FArmyProjectInputInfo ProjectInfo = NewProjectDialog->GetProjectInputInfo();
    JsonWriter->WriteValue("name", ProjectInfo.ProjectName);
	JsonWriter->WriteValue("setMealId", ProjectInfo.SetMealID);
	JsonWriter->WriteValue("code", ProjectInfo.ProjectCode);
    JsonWriter->WriteValue("customerName", ProjectInfo.OwnerName);
    JsonWriter->WriteValue("customerPhone", ProjectInfo.OwnerPhoneNumber);
	JsonWriter->WriteValue("provinceId", ProjectInfo.ProvinceID);
	JsonWriter->WriteValue("cityId", ProjectInfo.CityID);
	JsonWriter->WriteValue("areaId", ProjectInfo.DistrictID);
    JsonWriter->WriteValue("communityId", 0);
	JsonWriter->WriteValue("communityName", ProjectInfo.VillageName);
    JsonWriter->WriteValue("addressDetails", ProjectInfo.DetailAddress);
    JsonWriter->WriteObjectEnd();
    JsonWriter->Close();

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyProjectController::Callback_RequestNewProject);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/projects", CompleteDelegate, JStr);
    Request->ProcessRequest();

    // 显示加载提示
    GGI->Window->ShowThrobber(MAKE_TEXT("创建项目..."));
}

void FArmyProjectController::Callback_RequestNewProject(FArmyHttpResponse Response)
{
    GGI->Window->HideThrobber();

    if (Response.bWasSuccessful)
    {
        // 记录当前新建项目的数据
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");
        FContentItemPtr ProjectData = FArmyCommonTools::ParseContentItemFromJson(JData);
        FArmyUser::Get().CurProjectData = ProjectData;

        // 记录当前套餐ID
        TArray<TSharedPtr<FContentItemSpace::FResObj> > ResArr = ProjectData->GetResObjNoComponent();
        if (ResArr.Num() >= 1)
        {
            TSharedPtr<FContentItemSpace::FProjectRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FProjectRes>(ResArr[0]);
            if (ProRes.IsValid())
            {
                FArmyUser::Get().SetMealID(ProRes->SetMealId);
            }
        }

        ProjectList.Add(ProjectData);
        FillProjectList(TArray<FContentItemPtr>{ProjectData}, false, true);

        GGI->Window->DismissModalDialog();
        GGI->Window->ShowMessage(MT_Success, TEXT("创建项目成功"));
    }
    else
    {
        GGI->Window->ShowMessage(MT_Warning, Response.Message);
    }
}

FReply FArmyProjectController::OnNewPlanClicked()
{
	TSharedPtr<SArmyNewPlan> NewPlanWidget;
	SAssignNew(NewPlanWidget, SArmyNewPlan);
	GGI->Window->PresentModalDialog(TEXT("新建方案"), NewPlanWidget->AsShared(), FSimpleDelegate::CreateRaw(NewPlanWidget.Get(), &SArmyNewPlan::OnConfirmClicked), false);
    FSlateApplication::Get().SetKeyboardFocus(NewPlanWidget);

	return FReply::Handled();
}

void FArmyProjectController::OpenPlan(int32 PlanID)
{
    GGI->Window->ShowThrobber(MAKE_TEXT("载入方案中..."));

    // 如果方案文件为空，则无需下载，直接载入
    TempPlanData = PlanDataList[PlanID];
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = TempPlanData->GetResObjNoComponent();

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

void FArmyProjectController::GotoDesignEditor()
{
    // 新建项目成功，跳转到设计模式界面
    GGI->DesignEditor->Init();
    GGI->Window->PresentViewController(GGI->DesignEditor);
}

void FArmyProjectController::OnPlanReady(bool bWasSucceed, FString FilePath)
{
    if (bWasSucceed)
    {
        GGI->DesignEditor->ReqSyncPlan(FilePath);
    }
    else
    {
        GGI->Window->HideThrobber();
        GGI->Window->ShowMessage(MT_Warning, TEXT("MD5码不匹配，请联系管理员!"));
    }
}

void FArmyProjectController::LoadPlan(bool bWasSucceed)
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
        FString HomeFilePath = "";
        if (TempHomeData.IsValid())
        {
			TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = TempHomeData->GetResObjNoComponent();
			if (resArr.Num() > 0 && !resArr[0]->FileURL.Equals(""))
			{
                HomeFilePath = resArr[0]->FilePath;
			}
        }
        GGI->DesignEditor->LoadHome(HomeFilePath);

        // 加载方案数据
        FString PlanFilePath = "";
        if (TempPlanData.IsValid())
        {
			TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = TempPlanData->GetResObjNoComponent();
			if (resArr.Num() > 0 && !resArr[0]->FileURL.Equals(""))
			{
                PlanFilePath = resArr[0]->FilePath;
			}
            else
            {
                GGI->Window->HideThrobber();
            }
        }
        GGI->DesignEditor->LoadPlan(PlanFilePath);

        GotoDesignEditor();
    }
    else
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("方案加载失败"));
    }

    GGI->Window->HideThrobber();
}

FReply FArmyProjectController::OnMyProjectClicked()
{
	GGI->Window->SetWindowContent(ContentSwitcher->GetWidget(0));
	MyProjectWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
	GreatPlanWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"));
	ContentSwitcher->SetActiveWidgetIndex(0);
	return FReply::Unhandled();
}

void FArmyProjectController::OnMyProjectHovered()
{
	if (ContentSwitcher->GetActiveWidgetIndex() != 0)
	{
		MyProjectWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
	}
}

void FArmyProjectController::OnMyProjectUnHovered()
{
	if (ContentSwitcher->GetActiveWidgetIndex() != 0)
	{
		MyProjectWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"));
	}
}

FReply FArmyProjectController::OnGreatPlanClicked()
{
	GGI->Window->SetWindowContent(ContentSwitcher->GetWidget(1));
	ContentSwitcher->SetActiveWidgetIndex(1);
	GreatPlanWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
	MyProjectWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"));

	//请求大师方案数据
	static bool bRequest = false;
	if (!bRequest)
	{
		bRequest = true;
		RequestGreatPlanList();
	}

	return FReply::Unhandled();
}

void FArmyProjectController::OnGreatPlanHovered()
{
	if (ContentSwitcher->GetActiveWidgetIndex() != 1)
	{
		GreatPlanWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FFFF9800"));
	}
}

void FArmyProjectController::OnGreatPlanUnHovered()
{
	if (ContentSwitcher->GetActiveWidgetIndex() != 1)
	{
		GreatPlanWid->SetTextColor(FArmyStyle::Get().GetColor("Color.FF9D9FA5"));
	}
}

void FArmyProjectController::FillGreatPlanList(TArray<FContentItemPtr> ItemList, bool ClearProjectList /*= true*/, bool bInSelectLast /*= false*/)
{
	int32 IndexOffset = GreatPlanUIList.Num();
	for (int32 i = 0; i < ItemList.Num(); i++)
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ItemList[i]->GetResObjNoComponent();
		if (resArr.Num() < 1)
			continue;
		TSharedPtr<FContentItemSpace::FArmyGreatPlanRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FArmyGreatPlanRes>(resArr[0]);

		TSharedPtr<SContentItem> Item = SNew(SContentItem)
			.ItemCode(IndexOffset + i)
			.bHoverThumbnailOnly(false)
			.bShowDownloadTips(false)
			.ItemWidth(224)
			.ItemHeight(300)
			.ThumbnailPadding(4)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_16"))
			.TextStyle2(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FLinearColor::White)
			.TextColor2(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
			.ThumbnailURL(ItemList[i]->ThumbnailURL)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage")))
			.OnClicked_Raw(this, &FArmyProjectController::OnGreatPlanSelected)
			.DisplayText(FText::FromString(ProRes->Name))
			.DisplayText2(FText::FromString(TEXT("现代简约  |  86㎡  |  二居室")));

		GreatPlanListWidget->AddPlanWidget(Item.ToSharedRef());
		GreatPlanUIList.Add(Item);
		/*if (bInSelectLast && i == ItemList.Num() - 1)
		{
			Item->SetSelected(true);
			OnProjectSelected(IndexOffset + i);
		}*/
	}

	//GreatPlanListWidget->SetPlanCount(ItemList.Num());
	GreatPlanListWidget->UpData(ItemList.Num());
}

void FArmyProjectController::RequestGreatPlanList()
{
	//if (bClearProjectList)
	{
		//PageIndex = 0;
		GreatPlanListWidget->EmptyProjectList();
		GreatPlanList.Reset();
		GreatPlanUIList.Reset();
	}

	/*FString Url = FString::Printf(TEXT("/api/queryProject/%d/%d"), PageIndex, PageSize);
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyProjectController::Callback_RequestProjectList);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(Url, CompleteDelegate);
	Request->ProcessRequest();*/

	//后台暂未提供接口，构造测试数据
	FArmyHttpResponse Test;
	CreateTestData(Test);
	Callback_RequestGreatPlanList(Test);
}

void FArmyProjectController::Callback_RequestGreatPlanList(struct FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
        const TSharedPtr<FJsonObject> JData = Response.Data->GetObjectField("data");
        const TArray<TSharedPtr<FJsonValue>> JArray = JData->GetArrayField("list");

		TArray<FContentItemPtr> TempProjectList;

		for (auto& ArrayIt : JArray)
		{
			FContentItemPtr ParsedContentItem = FArmyCommonTools::ParseContentItemFromJson(ArrayIt->AsObject());
			TempProjectList.Add(ParsedContentItem);
			GreatPlanList.Add(ParsedContentItem);
		}

		FillGreatPlanList(TempProjectList, true);
	}
	else
	{
		if (Response.Data.IsValid())
		{
			GGI->Window->ShowMessage(MT_Warning, Response.Data->GetStringField("message"));
		}
	}
}

void FArmyProjectController::OnGreatPlanSelected(int32 PlanId)
{
	for (auto& It : GreatPlanUIList)
	{
		It->SetSelected(false);
	}
	GreatPlanUIList[PlanId]->SetSelected(true);
	SelectedProjectID = PlanId;
	FArmyUser::Get().CurProjectData = GreatPlanList[SelectedProjectID];

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = FArmyUser::Get().CurProjectData->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	// 显示项目详情
	TSharedPtr<FContentItemSpace::FArmyGreatPlanRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FArmyGreatPlanRes>(resArr[0]);
// 	PlanListWidget->SetProjectTitle(ProRes->ContractNo);
// 	FArmyUser::Get().SetMealID(ProRes->GroupID);

	//FString GreatDetailTitleInfo =/* ProRes->Name */TEXT("夏日微风") + /*ProRes->DesignStyle*/	FString::Printf(TEXT("现代简约")) + FString("\n\n")

	FString GreatDetailLeftInfo =
		TEXT("风格：") + /*ProRes->DesignStyle*/	FString::Printf(TEXT("现代简约")) + FString("\n\n") +
		TEXT("居室：") + /*ProRes->HouseType*/  FString::Printf(TEXT("二居室")) + FString("\n\n") +
		TEXT("介绍：") + /*ProRes->Description*/ FString::Printf(TEXT("简约风格的家具，在颜色搭配上也很简单"))+FString("\n\n");

	FString GreatDetailRightInfo =
		TEXT("面积：") + /*ProRes->Area*/FString::Printf(TEXT("86m²")) + FString("\n\n") +
		TEXT("设计师：") + /*ProRes->DesignerName*/FString::Printf(TEXT("周杰伦")) + FString("\n\n");
	//GreatPlanDetailWidget->SetProjectTitle(GreatDetailTitleInfo);
	GreatPlanDetailWidget->SetProjectLeftInfo(GreatDetailLeftInfo);
	GreatPlanDetailWidget->SetProjectRightInfo(GreatDetailRightInfo);
	

	GreatPlanDetailWidget->SetVisibility(EVisibility::Visible);
	GreatPlanDetailWidget->SetGreatPlanPicURL(ProRes->DesignPictureList);
	GreatPlanDetailWidget->ResetCurrentNumInCount();
	//GreatPlanDetailWidget->SetMaxBrushNum(ProRes->DesignPictureList.Num());
	// 请求方案列表
	RequestPlanList(FArmyUser::Get().CurProjectData->ID);
}

void FArmyProjectController::CreateTestData(FArmyHttpResponse & TestResponse)
{
	TestResponse.bWasSuccessful = true;
	// 大师方案测试数据
	FString PlanJStr;
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > PlanJsonWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&PlanJStr);

	int32 PlanNum = 200;
	// 开始写入方案数据
	PlanJsonWriter->WriteObjectStart();
	// 开始写入方案数据
	PlanJsonWriter->WriteObjectStart(TEXT("data"));
	PlanJsonWriter->WriteValue(TEXT("count"), PlanNum);
	PlanJsonWriter->WriteArrayStart(TEXT("list"));
	for (int32 i = 0 ; i < PlanNum; i++)
	{
		PlanJsonWriter->WriteObjectStart();
		PlanJsonWriter->WriteObjectStart(TEXT("data"));
		PlanJsonWriter->WriteValue(TEXT("designID"), i);
		PlanJsonWriter->WriteValue(TEXT("name"), TEXT("夏日微风") + FString::FromInt(i));
		PlanJsonWriter->WriteValue(TEXT("resourceType"), 20);
		PlanJsonWriter->WriteValue(TEXT("designStyle"), 1);
		PlanJsonWriter->WriteValue(TEXT("area"), 56.7);
		PlanJsonWriter->WriteValue(TEXT("areaType"), 2);
		PlanJsonWriter->WriteValue(TEXT("thumbnailUrl"), TEXT("http://mvfurniture.dabanjia.com/Admin_4_1497249835405_0.jpg"));
		PlanJsonWriter->WriteValue(TEXT("panoUrl"), TEXT("https://pano.dabanjia.com/pano/viewer/402/cx6XL2dS"));
		PlanJsonWriter->WriteValue(TEXT("designerId"), 10);
		PlanJsonWriter->WriteValue(TEXT("designerName"), TEXT("张三"));
		PlanJsonWriter->WriteValue(TEXT("description"), TEXT("代码千万行，注释第一行，命名不规范，红包群里见!"));
		
		PlanJsonWriter->WriteArrayStart(TEXT("DesignPictureList"));
		PlanJsonWriter->WriteValue(TEXT("http://mvfurniture.dabanjia.com/Admin_4_1504689240783_0.jpg"));
		PlanJsonWriter->WriteValue(TEXT("http://mvfurniture.dabanjia.com/Admin_4_1498110787186_0.jpg"));
		PlanJsonWriter->WriteValue(TEXT("http://mvfurniture.dabanjia.com/SJHT_358_1530845358304_0.jpg"));
		PlanJsonWriter->WriteValue(TEXT("http://mvfurniture.dabanjia.com/SJHT_358_1526625134588_0.jpg"));
		PlanJsonWriter->WriteValue(TEXT("http://mvfurniture.dabanjia.com/SJHT_358_1522225149133_0.jpg"));
		PlanJsonWriter->WriteArrayEnd();

		PlanJsonWriter->WriteObjectEnd();
		PlanJsonWriter->WriteObjectEnd();
	}
	PlanJsonWriter->WriteArrayEnd();

	PlanJsonWriter->WriteObjectEnd();
	//// 写入文件版本号
	//PlanJsonWriter->WriteValue("Version", *GXRVersion);

	PlanJsonWriter->WriteObjectEnd();
	PlanJsonWriter->Close(); 

	FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(PlanJStr), TestResponse.Data);
}
