#include "ArmyGlobalActionCallBack.h"
#include "SArmyNewPlan.h"
#include "SArmySaveAsMasterPlan.h"
#include "SArmyOpenPlan.h"
#include "SArmyPanorama.h"
#include "ArmyGameInstance.h"
#include "ArmyHttpModule.h"
#include "SArmyRenderingImage.h"
#include "ArmyLoginWindow.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"
#include "Model/XRHomeData.h"

TSharedPtr<FArmyGlobalActionCallBack> FArmyGlobalActionCallBack::GXRACB = nullptr;

FArmyGlobalActionCallBack::FArmyGlobalActionCallBack()
{
}

FArmyGlobalActionCallBack & FArmyGlobalActionCallBack::Get()
{
	if (!GXRACB.IsValid()) {
		GXRACB = MakeShareable(new FArmyGlobalActionCallBack);
	}
	return *GXRACB;
}

void FArmyGlobalActionCallBack::OnNewPlan()
{
	if (GGI) {
		SAssignNew(NewPlanWidget, SArmyNewPlan);
		GGI->Window->PresentModalDialog(TEXT("新建方案"), NewPlanWidget->AsShared(), FSimpleDelegate::CreateRaw(NewPlanWidget.Get(), &SArmyNewPlan::OnConfirmClicked), false);
		OnGlobalAction.Broadcast(); 
	}
}

void FArmyGlobalActionCallBack::OnOpenPlan()
{
	if (GGI) {
		SAssignNew(OpenPlanWidget, SArmyOpenPlan);
		GGI->Window->PresentModalDialog(TEXT("打开方案"), OpenPlanWidget->AsShared(), FSimpleDelegate::CreateRaw(OpenPlanWidget.Get(), &SArmyOpenPlan::OnConfirmClicked), false);
		OnGlobalAction.Broadcast(); 
	}
}

void FArmyGlobalActionCallBack::OnSavePlan()
{	
	if (GGI != nullptr && GGI->DesignEditor.IsValid())
    {
        GGI->DesignEditor->SavePlan();
		OnGlobalAction.Broadcast(); 
	}
}

void FArmyGlobalActionCallBack::OnSaveAsPlan()
{
    if (GGI != nullptr && GGI->DesignEditor.IsValid())
    {
        GGI->DesignEditor->SaveAsPlan();
		OnGlobalAction.Broadcast(); 
    }
}

void FArmyGlobalActionCallBack::OnCheckSaveAsMasterPlan()
{
	//请求权限数据
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindRaw(this, &FArmyGlobalActionCallBack::OnSaveAsMasterPlan);
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest("/api/perms?code=bim.ai-design.ai-plan", CompleteDelegate);
	Request->ProcessRequest();
}

void FArmyGlobalActionCallBack::OnSaveAsMasterPlan(FArmyHttpResponse Response)
{
	OnGlobalAction.Broadcast(); 
	if (Response.bWasSuccessful)
	{
        const TSharedPtr<FJsonObject> JObjectData = Response.Data->GetObjectField("data");
        bool bHasPower = JObjectData->GetBoolField("hasPerm");
		if (bHasPower)
		{
			if (GGI) {
				SAssignNew(SaveAsMasterPlanWidget, SArmySaveAsMasterPlan);
				GGI->Window->PresentModalDialog(TEXT("大师方案另存为"), SaveAsMasterPlanWidget->AsShared(), FSimpleDelegate::CreateRaw(SaveAsMasterPlanWidget.Get(), &SArmySaveAsMasterPlan::OnConfirmClicked), false);
			}
		}
		else
		{
			//@ 临时处理 提醒狂暂时不支持单个关闭按钮，待支持后此处可删除
			    TSharedPtr<SWidget> ContentWidget =
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					[
						SNew(SBox)
						.MinDesiredWidth(350)
						.MinDesiredHeight(100)
						.Padding(FMargin(20,30,20,20))
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						[
							SNew(SHorizontalBox)

							+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Left)
							.AutoWidth()
							[
								SNew(SBox)
								.HeightOverride(32)
								.WidthOverride(32)
								[
									SNew(SImage)
									.Image(FArmyStyle::Get().GetBrush("Icon.ModalDialog_Warning"))
								]
							]

							+ SHorizontalBox::Slot().Padding(FMargin(16,0,0,0))
							.VAlign(EVerticalAlignment::VAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("您暂无另存为大师方案权限，如需使用请联系管理员开通权限")))
								.TextStyle(FCoreStyle::Get(), "VRSText_12")
								.Justification(ETextJustify::Center)
							]
						]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.HAlign(HAlign_Right)
				[

					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.Padding(0, 0, 15, 15)
					.HAlign(HAlign_Right)
					.AutoWidth()
					[
						 SNew(SBox)
						.WidthOverride(80)
						.HeightOverride(30)
						[
							SNew(SButton)
							.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							.OnClicked(FOnClicked::CreateLambda([this]() {
								GGI->Window->DismissModalDialog();
								return FReply::Handled();
							}))
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("关闭")))
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							]
						]
					]
				];
				
			GGI->Window->PresentModalDialog(TEXT("提醒"),ContentWidget.ToSharedRef());
		}
	}
	else
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("网络错误"));
	}

}

void FArmyGlobalActionCallBack::OnExportList()
{	
	OnGlobalAction.Broadcast(); 
    GGI->DesignEditor->ExportConstructionList();
}
void FArmyGlobalActionCallBack::OnGenerateRenderingImage()
{
	OnGlobalAction.Broadcast(); 
	RenderingImageWidget = NULL;
	if (GGI) {
		SAssignNew(RenderingImageWidget, SArmyRenderingImage);
		GGI->Window->PresentModalDialog(TEXT("渲染效果图"), RenderingImageWidget->AsShared());
	}
}
void FArmyGlobalActionCallBack::OnSinglePanorama()
{
	OnGlobalAction.Broadcast(); 
	PanoramaWidget = NULL;
	if (GGI) {
		SAssignNew(PanoramaWidget, SArmyPanorama)
		.BIsSingle(true);
		GGI->Window->PresentModalDialog(TEXT("渲染单张全景图"), PanoramaWidget->AsShared());
	}
}

void FArmyGlobalActionCallBack::OnMultiplePanorama()
{
	OnGlobalAction.Broadcast(); 
	PanoramaWidget = NULL;
	if (GGI) {
		SAssignNew(PanoramaWidget, SArmyPanorama)
		.BIsSingle(false);
		GGI->Window->PresentModalDialog(TEXT("渲染多房间行走全景图"), PanoramaWidget->AsShared());
	}
}

void FArmyGlobalActionCallBack::OnLogout()
{
			OnGlobalAction.Broadcast(); 
    GGI->Window->PresentModalDialog(TEXT("是否注销当前账号？"), FSimpleDelegate::CreateRaw(this, &FArmyGlobalActionCallBack::Logout));
}

void FArmyGlobalActionCallBack::OnExit()
{
	OnGlobalAction.Broadcast(); 
    GGI->Window->PresentModalDialog(TEXT("是否退出？"), FSimpleDelegate::CreateRaw(this, &FArmyGlobalActionCallBack::Exit));
}

void FArmyGlobalActionCallBack::OnBackGroundManager()
{
	OnGlobalAction.Broadcast();
}

void FArmyGlobalActionCallBack::Logout()
{
    // 清空后台缓存数据
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/logout", FArmyHttpRequestCompleteDelegate());
    Request->ProcessRequest();

    // 清空用户缓存数据
    FArmyUser::Get().Clear();
    if (GConfig)
    {
        GConfig->SetBool(TEXT("UserData"), TEXT("RememberMe"), false, GGameIni);
        GConfig->SetBool(TEXT("UserData"), TEXT("AutoLogin"), false, GGameIni);
        GConfig->SetString(TEXT("UserData"), TEXT("Username"), TEXT(""), GGameIni);
        GConfig->SetString(TEXT("UserData"), TEXT("Password"), TEXT(""), GGameIni);
    }

    // 清空代理
    GVC->OnViewTypeChanged.Clear();
    USelection::SelectNoneEvent.Clear();
    FArmyToolsModule::Get().GetRectSelectTool()->SelectedObjectsDelegate.Unbind();
    OnGlobalAction.Clear();
	// @zengy 清空HomeData多播代理
	FArmySceneData::Get()->ChangedHomeDataDelegate.Clear();

	// zengy noted: 调整了下面两个函数的执行顺序，避免了在清空场景时OverlaySlot已经提前失效的问题
	// 如果影响了其它模式，请在修改前先联系一下zengy
	
    // 清空场景
    GGI->DesignEditor->ClearAll();

	// 清空视口上的所有widget
	GVC->ViewportOverlayWidget->ClearChildren();
    
	GGI->Window->DismissAllViewControllers();
    GGI->DesignEditor->Quit();
    GGI->DesignEditor = nullptr;
	
	GGI->LoginWindow->Init();

	/**@欧石楠 恢复到小窗口登录*/
	TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();
	// 切换到登录界面
	RootWindow->Resize(FVector2D(1280, 768));
	RootWindow->MoveWindowTo(FVector2D(RootWindow->GetFullScreenInfo().GetCenter().X - 1280 / 2, RootWindow->GetFullScreenInfo().GetCenter().Y - 768 / 2));
	RootWindow->SetContent(GGI->LoginWindow.ToSharedRef());
}

void FArmyGlobalActionCallBack::Exit()
{
    // 清空后台缓存数据
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/logout", FArmyHttpRequestCompleteDelegate());
    Request->ProcessRequest();

    // 关闭窗口
    TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();
    if (RootWindow.IsValid())
    {
        RootWindow->RequestDestroyWindow();
    }
}