#include "ArmyGameInstance.h"
#include "ArmyCoreModule.h"
#include "ArmyEditorModule.h"
#include "ArmyUser.h"
#include "ArmyViewportClient.h"
#include "ArmyFrameCommands.h"
#include "GameFramework/GameUserSettings.h"
#include "ArmyPlayerController.h"
#include "ArmyLoginWindow.h"
#include "ArmyFrameModule.h"
#include "ArmyHttpModule.h"
#include "ArmyProjectController.h"
#include "ArmyWorldManager.h"

UXRGameInstance* GGI = nullptr;

UXRGameInstance::UXRGameInstance()
    : Window(nullptr)
    , DesignEditor(nullptr)
{
    GGI = this;
}

void UXRGameInstance::Init()
{
	UGameViewportClient::OnViewportCreated().AddUObject(this, &UXRGameInstance::InitLocalData);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UXRGameInstance::OnPostLoadMap);
}

void UXRGameInstance::InitLocalData()
{
	GVC->ViewportOverlayWidget = SNew(SOverlay);
	GVC->AddViewportWidgetContent(GVC->ViewportOverlayWidget.ToSharedRef());
}

void UXRGameInstance::ResetTransactions()
{
    for (auto& It : Window->ControllerStack)
    {
        It->TransMgr->Reset();
    }
}

void UXRGameInstance::OnApplicationClosed()
{
    FSimpleDelegate OnApplicationCloseConfirmed;
    OnApplicationCloseConfirmed.BindUObject(this, &UXRGameInstance::OnApplicationCloseConfirmed);
    Window->PresentModalDialog(TEXT("请检查方案是否保存，确认退出？"), OnApplicationCloseConfirmed);
}

void UXRGameInstance::OnApplicationCloseConfirmed()
{
    // 清空后台缓存数据
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/logout", FArmyHttpRequestCompleteDelegate());
    Request->ProcessRequest();

    // 关闭程序窗口
    if (GEngine->GameViewport->GetWindow().IsValid())
    {
        GEngine->GameViewport->GetWindow()->RequestDestroyWindow();
    }
}

void UXRGameInstance::OnGotoHomePage()
{
    FSimpleDelegate OnGotoHomePageConfirmed;
    OnGotoHomePageConfirmed.BindUObject(this, &UXRGameInstance::OnGotoHomePageConfirmed);
    Window->PresentModalDialog(TEXT("请检查方案是否保存，确认返回？"), OnGotoHomePageConfirmed);
}

void UXRGameInstance::OnGotoHomePageConfirmed()
{
	// @zengy 通知退出当前的ViewController
	Window->NotifyQuitCurrentViewController();
    TSharedPtr<FArmyViewController> ProjectController = NewVC<FArmyProjectController>();
    Window->PresentViewController(ProjectController);
	
	// @欧石楠 返回主页时重置DesignEditor初始化状态
    // GGI->DesignEditor->Quit();
}

void UXRGameInstance::OnShowPlanPrice()
{
	if (DesignEditor.IsValid())
	{
		DesignEditor->OnShowPlanPrice();
	}
}

void UXRGameInstance::InitUICommand()
{
	FArmyFrameCommands::Register();

	TArray<TSharedPtr<FArmyMenuInfo>> MenuInfoArray;
	{
		TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
		MenuInfoPtr->M_Label = FText::FromString(TEXT("文件(F)"));
		MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open the file menu"));
		MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
		FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
		RefSection.HeadingText = FText::FromString(TEXT(""));
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuNew);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuOpen);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuSave);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuSaveAs);
#ifdef MASTER_PLAN //大师方案另存为
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuMasterSaveAs);
#endif
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuExportList);
		MenuInfoArray.Push(MenuInfoPtr);
	}
	{
		TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
		MenuInfoPtr->M_Label = FText::FromString(TEXT("编辑(E)"));
		MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open the edit menu"));
		MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
		{
			FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
			RefSection.HeadingText = FText::FromString(TEXT(""));
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuRotate);
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuScale);
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuMirrorX);
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuMirrorY);
		}

		{
			MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
			FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
			RefSection.HeadingText = FText::FromString(TEXT(""));
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuCopy);
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuPaste);
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuDelete);
		}
		{
			MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
			FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
			RefSection.HeadingText = FText::FromString(TEXT(""));
			//RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuUndo);
			//RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuRedo);
		}
		{
			MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
			FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
			RefSection.HeadingText = FText::FromString(TEXT(""));
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuGroup);
			RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuUnGroup);
			//RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuSaveGroup);
		}
		MenuInfoArray.Push(MenuInfoPtr);
	}

	{
		TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
		MenuInfoPtr->M_Label = FText::FromString(TEXT("视图(V)"));
		MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open view menu"));
		MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
		FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
		RefSection.HeadingText = FText::FromString(TEXT(""));
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuOverallView);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuTopView);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuCeilingView);
		//RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuSurroundView);//@梁晓菲 去掉环绕图
		MenuInfoArray.Push(MenuInfoPtr);
	}

	{
		TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
		MenuInfoPtr->M_Label = FText::FromString(TEXT("渲染(R)"));
		MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open render menu"));
		MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
		FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
		RefSection.HeadingText = FText::FromString(TEXT(""));
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuDesignSketch);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuSinglePanorama);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuMultiplePanoram);
		MenuInfoArray.Push(MenuInfoPtr);
	}

	{
		TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
		MenuInfoPtr->M_Label = FText::FromString(TEXT("画质(Q)"));
		MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open picture quality menu"));
		MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
		FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
		RefSection.HeadingText = FText::FromString(TEXT(""));
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuHighQuality);
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuNormalQuality);
		MenuInfoArray.Push(MenuInfoPtr);
	}

	{
		TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
		MenuInfoPtr->M_Label = FText::FromString(TEXT("VR(VR)"));
		MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open VR menu"));
		MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
		FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
		RefSection.HeadingText = FText::FromString(TEXT(""));
		RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuVRMode);
		MenuInfoArray.Push(MenuInfoPtr);
	}

	//{
	//	TSharedPtr<FArmyMenuInfo> MenuInfoPtr = MakeShareable(new FArmyMenuInfo());
	//	MenuInfoPtr->M_Label = FText::FromString(TEXT("帮助(H)"));
	//	MenuInfoPtr->M_Tip = FText::FromString(TEXT("Open the help menu"));
	//	MenuInfoPtr->SectionInfos.Push(FArmyMenuInfo::FArmyMenuSectionInfo());
	//	FArmyMenuInfo::FArmyMenuSectionInfo& RefSection = MenuInfoPtr->SectionInfos.Top();
	//	RefSection.HeadingText = FText::FromString(TEXT(""));
	//	RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuOnlineService);
	//	RefSection.ActionList.Push(FArmyFrameCommands::Get().CommandMenuVideoTutorial);
	//	MenuInfoArray.Push(MenuInfoPtr);
	//}

	TArray< TSharedPtr< FString > > AccountComboBoxSource
	{
		MakeShareable(new FString(TEXT("后台管理"))),
		MakeShareable(new FString(TEXT("注销"))),
		MakeShareable(new FString(TEXT("退出")))
	};
	
	SAssignNew(Window, SArmyWindow)
		.MenuConstrctionInfos(MenuInfoArray)
		.CommandList(FArmyFrameCommands::Get().GlobalCommandsList)
		.ExtendComboList(AccountComboBoxSource)
		.OnComboItemChanged(FOnComboBoxSelectionChanged::CreateStatic(FArmyFrameCommands::OnAccoutChanged));
    GGI->WindowOverlay = Window->WindowOverlay;
	
	SAssignNew(LoginWindow, SArmyLoginWindow);
}
void UXRGameInstance::StartGameInstance()
{
	Super::StartGameInstance();

    // 设置字体分辨率
    SlateApplicationUtils::SetGlobalFontDPI(72, 72);

    // 加载模块
    FModuleManager::LoadModuleChecked<FArmyCoreModule>(TEXT("ArmyCore"));
    FModuleManager::LoadModuleChecked<FArmyHttpModule>(TEXT("ArmyHttp"));
    FModuleManager::LoadModuleChecked<FArmyEditorModule>(TEXT("ArmyEditor"));
	FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("AutoDesignModule"));

	EModuleLoadResult FailureReason;
	FModuleManager::Get().LoadModuleWithFailureReason(FName("ArmyCameraParam"), FailureReason);

    // 绑定事件
    FArmyFrameModule::Get().OnApplicationClosed.BindUObject(this, &UXRGameInstance::OnApplicationClosed);
    FArmyFrameModule::Get().OnGotoHomePage.BindUObject(this, &UXRGameInstance::OnGotoHomePage);
	FArmyFrameModule::Get().OnShowPricePage.BindUObject(this, &UXRGameInstance::OnShowPlanPrice);
	// 加载用户数据
	FArmyUser::Get().Init();

    // 创建自己的窗口,初始化界面为登录界面
	InitUICommand();
    TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();
	RootWindow->SetContent(LoginWindow.ToSharedRef());    

	GetGameViewportClient()->EngineShowFlags.CompositeEditorPrimitives = true;
}

void UXRGameInstance::OnPostLoadMap(UWorld* InWorld)
{
	FArmyWorldManager::Get().Init(InWorld);
}
