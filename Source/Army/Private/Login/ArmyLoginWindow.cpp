#include "ArmyLoginWindow.h"
#include "ArmyStyle.h"
#include "SOverlay.h"
#include "SBoxPanel.h"
#include "SArmyLogin.h"
#include "ArmyHttpModule.h"
#include "ArmyProjectController.h"
#include "ArmyGameInstance.h"
#include "ArmyUser.h"
#include "SArmyThrobber.h"
#include "SArmyCarouselPanel.h"
#include "SArmyModalDialog.h"
#include "ArmyGlobalActionCallBack.h"

void SArmyLoginWindow::Construct(const FArguments & InArgs)
{
	// 绑定回调
	FArmyUser::Get().OnLoginSuccess.BindRaw(this, &SArmyLoginWindow::OnLoginSuccess);
	FArmyUser::Get().OnLoginFailed.BindRaw(this, &SArmyLoginWindow::OnLoginFailed);	

	SAssignNew(LoginCarouselPanel, SArmyCarouselPanel);
	LoginCarouselPanel->AddCarouselItem(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Launcher.LoginView1")));
	LoginCarouselPanel->AddCarouselItem(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Launcher.LoginView2")));		

	ChildSlot
	[
		SAssignNew(Overlay, SOverlay)

		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(1280)
			.HeightOverride(768)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBox)
					.WidthOverride(812)
					[
						LoginCarouselPanel->AsShared()
					]
				]
				+SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SBox)					
					[
						SAssignNew(LoginWidget, SArmyLogin)
						.OnLogin_Raw(this, &SArmyLoginWindow::RequestLogin)
					]
				]
			]
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(0, 23.5, 23.5, 0)
		[
			SNew(SBox)			
			[
				SNew(SButton)
				.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
				.OnClicked(this, &SArmyLoginWindow::OnCloseButtonClicked)
			]
		]
		// 加载提示
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SAssignNew(Throbber, SArmyThrobber)
			.Visibility(EVisibility::Hidden)
		]
	];	
	Init();

	LoginCarouselPanel->SwitchToNext();
}

void SArmyLoginWindow::Init()
{
	// 记住密码
	RememberMe();

	// 自动登录
	AutoLogin();
}

void SArmyLoginWindow::ShowThrobber(const TAttribute<FText>& Message, bool bCanceable, TSharedPtr<IHttpRequest> CanceableRequest)
{
	Throbber->SetVisibility(EVisibility::Visible);
	Throbber->SetMessage(Message);
	Throbber->SetCanceable(bCanceable, CanceableRequest);
}

void SArmyLoginWindow::HideThrobber()
{
	if (Throbber->GetVisibility() == EVisibility::Visible)
	{
		Throbber->SetVisibility(EVisibility::Hidden);
	}
}

void SArmyLoginWindow::RequestLogin(FString Username, FString Password)
{
	LoginWidget->TB_ErrorMessage->SetVisibility(EVisibility::Collapsed);

	// 如果输入密码与记住的密码不一致，则重新计算md5加密密码
	if (!FArmyUser::Get().GetPassword().Equals(Password))
	{
        Password = FMD5::HashAnsiString(*Password);
	}

	FArmyUser::Get().RequestLogin(Username, Password);
	ShowThrobber(MAKE_TEXT("登录中..."), false);
}

void SArmyLoginWindow::OnLoginSuccess()
{
	HideThrobber();

	// 是否选择记住密码或自动登录
	FArmyUser::Get().bRememberMe = (LoginWidget->CB_RememberMe->GetCheckedState() == ECheckBoxState::Checked);

	// 保存选项到配置文件中
	if (GConfig)
	{
		FString Section = TEXT("UserData");

		FString TempU = FArmyUser::Get().GetUsername();
		FString TempP = FArmyUser::Get().GetPassword();

		GConfig->SetBool(*Section, TEXT("RememberMe"), FArmyUser::Get().bRememberMe, GGameIni);
		GConfig->SetBool(*Section, TEXT("AutoLogin"), FArmyUser::Get().bAutoLogin, GGameIni);
		GConfig->SetString(*Section, TEXT("Username"), *(FArmyUser::Get().GetUsername()), GGameIni);
		if (FArmyUser::Get().bRememberMe)
		{
			GConfig->SetString(*Section, TEXT("Password"), *(FArmyUser::Get().GetPassword()), GGameIni);
		}		

		GConfig->Flush(false, GGameIni);
	}

	TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();
	RootWindow->SetContent(GGI->Window.ToSharedRef());	

	// 跳转到项目列表界面
	TSharedPtr<FArmyProjectController> ProjectController = NewVC<FArmyProjectController>();
	GGI->Window->PresentViewController(ProjectController);

	/** @欧石楠 显示右侧账号管理相关信息*/
	GGI->Window->GetSystemTitleBar()->SetAccountName(FArmyUser::Get().GetUserRealName());
    GGI->Window->GetSystemTitleBar()->SetCompanyName(FArmyUser::Get().GetCompanyName());
    GGI->Window->GetSystemTitleBar()->SetAppIcon(FArmyUser::Get().GetAppIconUrl());

	// 创建DesignEditor
	GGI->DesignEditor = MakeShareable(new FArmyDesignEditor());
	GGI->DesignEditor->PreInit();

	/**@欧石楠 最大化*/
	RootWindow->Maximize();
}

void SArmyLoginWindow::OnLoginFailed(const int32 ErrorCode, const FString & ErrorMsg)
{
	HideThrobber();

    if (ErrorCode == XRHttp::ALREADY_LOGGED_IN)
    {
        const FString Message = TEXT("检测到您的账号已经在其他设备登录，是否登录？\n继续登录将会强制对方下线");
        FSimpleDelegate OnConfirm, OnClose;
        OnConfirm.BindLambda([this]() {
            FArmyUser::Get().ReqRelogin();
            ShowThrobber(MAKE_TEXT("重新登录中..."), false);
        });
        OnClose.BindLambda([this]() {
            Overlay->RemoveSlot(ModalDialog.ToSharedRef());
        });
        PresentModalDialog(Message, OnConfirm, OnClose, TEXT("继续登录"), TEXT("取消"), true, TEXT("异地登录提醒"));
    }
    else
    {
        // @欧石楠 如果用户已登录的情况下重新登陆失败，需要回到登录界面
        if (FArmyUser::Get().IsLoggedIn())
        {
            FArmyGlobalActionCallBack::Get().Logout();
        }
        else
        {
            LoginWidget->TB_ErrorMessage->SetText(FText::FromString(ErrorMsg));
            LoginWidget->TB_ErrorMessage->SetVisibility(EVisibility::Visible);
        }
    }
}

void SArmyLoginWindow::RememberMe()
{
	if (FArmyUser::Get().bRememberMe)
	{
		FString Username = FArmyUser::Get().GetUsername();
		FString Password = FArmyUser::Get().GetPassword();
		LoginWidget->ETB_Username->SetText(FText::FromString(*Username));
		LoginWidget->ETB_Pwd->SetText(FText::FromString(*Password));
		LoginWidget->CB_RememberMe->SetIsChecked(ECheckBoxState::Checked);
	}
	else
	{
		FString Username = FArmyUser::Get().GetUsername();
		LoginWidget->ETB_Username->SetText(FText::FromString(*Username));
		LoginWidget->ETB_Pwd->SetText(FText::FromString(""));
		LoginWidget->CB_RememberMe->SetIsChecked(ECheckBoxState::Unchecked);
	}
}

void SArmyLoginWindow::AutoLogin()
{
	if (FArmyUser::Get().bAutoLogin)
	{
		LoginWidget->CB_AutoLogin->SetIsChecked(ECheckBoxState::Checked);

		FString Username = FArmyUser::Get().GetUsername();
		FString Password = FArmyUser::Get().GetPassword();
		RequestLogin(Username, Password);
	}
	else
	{
		LoginWidget->CB_AutoLogin->SetIsChecked(ECheckBoxState::Unchecked);
	}
}

FReply SArmyLoginWindow::OnCloseButtonClicked()
{
	TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();

	if (RootWindow.IsValid())
	{
		RootWindow->RequestDestroyWindow();
	}

	return FReply::Handled();
}

void SArmyLoginWindow::PresentModalDialog(
    const FString& InMessage, 
    FSimpleDelegate OnConfirm, 
    FSimpleDelegate OnClose, 
    const FString& ConfirmString, 
    const FString& CancelString, 
    bool bCloseImmediately /*= true*/, 
    const FString& InTitle /*= TEXT("提醒")*/)
{
    TSharedPtr<SWidget> ContentWidget =
		SNew(SBox)
		.MinDesiredWidth(350)
		.MinDesiredHeight(100)
		.Padding(FMargin(20, 30, 20, 20))
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
			+ SHorizontalBox::Slot().Padding(FMargin(16, 0, 0, 0))
			.VAlign(EVerticalAlignment::VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(InMessage))
				.TextStyle(FArmyStyle::Get(), "ArmyText_12")
				.Justification(ETextJustify::Center)
			]
		];

	SAssignNew(ModalDialog, SArmyModalDialog)
	.Title(InTitle)
	.ContentWidget(ContentWidget)
	.OnConfirm(OnConfirm)
	.OnImmediatelyClose(OnClose)
	.bCloseImmediately(bCloseImmediately)
	.ConfirmText(ConfirmString)
	.CancelText(CancelString);

    Overlay->AddSlot()
        [
            ModalDialog.ToSharedRef()
        ];
}
