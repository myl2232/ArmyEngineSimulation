#include "SArmyWindow.h"
#include "ArmyStyle.h"
#include "SArmySystemTitleBar.h"
#include "SArmyModalDialog.h"
#include "SArmyThrobber.h"
#include "SWidgetSwitcher.h"

void SArmyWindow::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
		SAssignNew(WindowSwitcher, SWidgetSwitcher)
		+ SWidgetSwitcher::Slot()
		[
			SNew(SOverlay)

			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SVerticalBox)

				// 标题栏
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					.Padding(0)
					.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF2F3033"))
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					[
						SNew(SVerticalBox)

						// 系统标题栏
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(SystemTitleBar, SArmySystemTitleBar)
							.MenuConstrctionInfos(InArgs._MenuConstrctionInfos)
							.CommandsList(InArgs._CommandList)
							.ExtendComboList(InArgs._ExtendComboList)
							.OnComboItemChanged(InArgs._OnComboItemChanged)
						]

						// 自定义标题栏
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(CustomTitleBarContainer, SBox)
						]
					]
				]
        
				+ SVerticalBox::Slot()
				.FillHeight(1.f)
				[
					SNew(SOverlay)

					// 窗口内容
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(WindowContainer, SBox)
					]

					// 模态视图内容
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SAssignNew(ModalViewOverlay, SOverlay)
					]

					// 交互信息提示
					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Bottom)
					.Padding(0, 0, 0, 200)
					[
						SAssignNew(InteractiveMessageWidget, SBorder)
						.BorderBackgroundColor(FSlateColor(FLinearColor(0.1, 0.1, 0.1, 0.6)))
						.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						.Padding(FMargin(25,5,25,5))
						.Visibility(EVisibility::Hidden)
						[
							SNew(STextBlock)
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FSlateColor(FLinearColor(1,1,1,1)))
							.Text(FText::FromString(TEXT("按F键交互")))
						]
					]
				]
			]

			// 窗口层叠
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(WindowOverlay, SOverlay)
			]

			// 加载提示
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(Throbber, SArmyThrobber)
				.Visibility(EVisibility::Hidden)
			]

			// 消息提示 /*@刘克祥 提示消息显示在最顶层*/
			+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Top)
			.Padding(0, 140, 0, 0)
			[
				SAssignNew(MessageWidget, SArmyMessage)
			]
		]
		//VR模式 全屏显示
		+ SWidgetSwitcher::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				GEngine->GetGameViewportWidget().ToSharedRef()
			]
			//@刘克祥 按回车键退出提示  4.18之后版本 VR模式下无法叠加SWidget到SViewport上 具体原因待研究
			/*+ SOverlay::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			.Padding(0, 0, 0, 200)
			[
				SAssignNew(InteractiveMessageWidget, SBorder)
				.BorderBackgroundColor(FSlateColor(FLinearColor(0.1, 0.1, 0.1, 0.6)))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				.Padding(FMargin(25,5,25,5))
				.Visibility(EVisibility::Hidden)
				[
					SNew(STextBlock)
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FSlateColor(FLinearColor(1,1,1,1)))
					.Text(FText::FromString(TEXT("按\"回车\"键退出")))
				]
			]*/
		]
    ];
}

void SArmyWindow::PresentViewController(TSharedPtr<FArmyViewController> ViewController)
{
    if (ControllerStack.Num() > 0)
    {
        ControllerStack.Pop();
    }
    ControllerStack.Push(ViewController);

    SetTitleBar(ViewController->TitleBarWidget);
    SetWindowContent(ViewController->ContentWidget);
}

void SArmyWindow::PresentModalViewController(TSharedPtr<FArmyViewController> ViewController)
{
    ControllerStack.Push(ViewController);

    SetTitleBar(ViewController->TitleBarWidget);
    ModalViewOverlay->AddSlot()
        [
            ViewController->ContentWidget.ToSharedRef()
        ];
}

void SArmyWindow::DismissModalViewController()
{
    if (ModalViewOverlay->GetNumWidgets() > 0)
    {
        ModalViewOverlay->RemoveSlot();
        ControllerStack.Pop();

        if (ControllerStack.Num() > 0)
        {
            SetTitleBar(ControllerStack.Last()->TitleBarWidget);
            SetWindowContent(ControllerStack.Last()->ContentWidget);
        }
    }
}

void SArmyWindow::DismissAllModalViewControllers()
{
    while (ModalViewOverlay->GetNumWidgets() > 0)
    {
        DismissModalViewController();
    }
}

void SArmyWindow::DismissAllViewControllers()
{
	DismissAllModalViewControllers();
	ControllerStack.Reset();
}

void SArmyWindow::PresentModalDialog(TSharedRef<SWidget> InContentWidget)
{
	WindowOverlay->AddSlot()
		[
			InContentWidget
		];
}
void SArmyWindow::PresentModalDialog(const FString& InTitle, TSharedRef<SWidget> InContentWidget, FSimpleDelegate OnConfirm /*= FSimpleDelegate() */, bool bCloseImmediately /*= true*/)
{
    TSharedPtr<SArmyModalDialog> ModalDialog =
        SNew(SArmyModalDialog)
        .Title(InTitle)
        .ContentWidget(InContentWidget)
        .OnConfirm(OnConfirm)
		.OnImmediatelyClose(this,&SArmyWindow::DismissModalDialog)
        .bCloseImmediately(bCloseImmediately);

	PresentModalDialog(ModalDialog.ToSharedRef());
}

void SArmyWindow::PresentModalDialog(const FString& InMessage, FSimpleDelegate OnConfirm, bool bCloseImmediately /*= true*/, const FString& InTitle)
{
    TSharedPtr<SWidget> ContentWidget =
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
				.Text(FText::FromString(InMessage))
				.TextStyle(FCoreStyle::Get(), "VRSText_12")
				.Justification(ETextJustify::Center)
			]
        ];

    PresentModalDialog(InTitle, ContentWidget.ToSharedRef(), OnConfirm, bCloseImmediately);
}

void SArmyWindow::PresentModalDialog(
    const FString& InTitle,
    TSharedRef<SWidget> InContentWidget,
    FSimpleDelegate OnConfirm,
    FSimpleDelegate OnCancel,
    FSimpleDelegate OnClose,
    bool bCloseImmediately,
    const FString& ConfirmString/* = TEXT("确认")*/,
    const FString& CancelString/* = TEXT("取消")*/)
{
	TSharedPtr<SArmyModalDialog> ModalDialog =
		SNew(SArmyModalDialog)
		.Title(InTitle)
		.ContentWidget(InContentWidget)
		.OnConfirm(OnConfirm)
        .OnCancel(OnCancel)
		.OnImmediatelyClose(OnClose)
		.bCloseImmediately(bCloseImmediately)
        .ConfirmText(ConfirmString)
        .CancelText(CancelString);

	PresentModalDialog(ModalDialog.ToSharedRef());
}

void SArmyWindow::PresentModalDialog(
    const FString & InMessage,
    FSimpleDelegate OnConfirm,
    FSimpleDelegate OnCancel,
    FSimpleDelegate OnClose,
    const FString & ConfirmString,
    const FString & CancelString,
    bool bCloseImmediately,
    const FString & InTitle)
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

	TSharedPtr<SArmyModalDialog> ModalDialog =
		SNew(SArmyModalDialog)
		.Title(InTitle)
		.ContentWidget(ContentWidget)
		.OnConfirm(OnConfirm)
        .OnCancel(OnCancel)
		.OnImmediatelyClose(OnClose)
		.bCloseImmediately(bCloseImmediately)
		.ConfirmText(ConfirmString)
		.CancelText(CancelString);

	PresentModalDialog(ModalDialog.ToSharedRef());
}

void SArmyWindow::PresentModalDialog(const FString& InMessage, FSimpleDelegate OnConfirm, FSimpleDelegate OnClose, bool bCloseImmediately /*= true*/, const FString& InTitle /*= TEXT("提醒")*/)
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

	PresentModalDialog(InTitle, ContentWidget.ToSharedRef(), OnConfirm, FSimpleDelegate(), OnClose, bCloseImmediately);
}

void SArmyWindow::DismissModalDialog()
{
    if (WindowOverlay->GetNumWidgets() > 0)
    {
        WindowOverlay->RemoveSlot();
    }
    HideThrobber();
}

void SArmyWindow::ShowThrobber(const TAttribute<FText>& Message, bool bCanceable /*= false*/, TSharedPtr<IHttpRequest> CanceableRequest /*= nullptr*/)
{
    Throbber->SetMessage(Message);
    Throbber->SetCanceable(bCanceable, CanceableRequest);
	Throbber->SetVisibility(EVisibility::Visible);

}

void SArmyWindow::ShowThrobber(const TAttribute<FText>& Message, FSimpleDelegate OnCancel)
{
    Throbber->SetMessage(Message);
    Throbber->SetOnCancel(OnCancel);
	Throbber->SetVisibility(EVisibility::Visible);
}

void SArmyWindow::UpdateThrobberMessage(const TAttribute<FText>& Message)
{
	Throbber->SetMessage(Message);
}

void SArmyWindow::AlwaysShowThrobber(const TAttribute<FText>& Message, bool bCanceable /*= false*/, TSharedPtr<IHttpRequest> CanceableRequest /*= nullptr*/)
{
    Throbber->SetVisibility(EVisibility::Visible);
    Throbber->SetMessage(Message);
    Throbber->SetCanceable(bCanceable, CanceableRequest);

	AlwaysShow = true;
}

void SArmyWindow::AlwaysShowThrobber(const TAttribute<FText>& Message, FSimpleDelegate OnCancel)
{
    Throbber->SetVisibility(EVisibility::Visible);
    Throbber->SetMessage(Message);
    Throbber->SetOnCancel(OnCancel);

	AlwaysShow = true;
}

void SArmyWindow::HideThrobber()
{
    if (Throbber->GetVisibility() == EVisibility::Visible && !AlwaysShow)
    {
        Throbber->SetVisibility(EVisibility::Hidden);
    }
}

void SArmyWindow::AlwaysHideThrobber()
{
	if (Throbber->GetVisibility() == EVisibility::Visible)
	{
		Throbber->SetVisibility(EVisibility::Hidden);
		AlwaysShow = false;
	}
}

void SArmyWindow::ShowMessage(EArmyMessageType MessageType, const FString& Message)
{
    MessageWidget->ShowMessage(MessageType, Message);
}

void SArmyWindow::ShowMessageWithoutTimeout(EArmyMessageType MessageType, const FString& Message)
{
	MessageWidget->ShowMessageWithoutTimeout(MessageType, Message);
}

void SArmyWindow::HideMessage()
{
	MessageWidget->HideMessage();
}

void SArmyWindow::ShowInteractiveMessage(bool bInShow)
{
	InteractiveMessageWidget->SetVisibility(bInShow ? EVisibility::HitTestInvisible : EVisibility::Hidden);
}

void SArmyWindow::SetTitleBar(TSharedPtr<SWidget> InTitleBar)
{
    if (InTitleBar.IsValid())
    {
        CustomTitleBarContainer->SetVisibility(EVisibility::Visible);
		CustomTitleBarWidget = InTitleBar;
        CustomTitleBarContainer->SetContent(InTitleBar.ToSharedRef());
    }
    else
    {
        CustomTitleBarContainer->SetVisibility(EVisibility::Collapsed);
    }
}

void SArmyWindow::SetWindowContent(TSharedPtr<SWidget> Widget)
{
    WindowContainer->SetContent(Widget.ToSharedRef());
}

TSharedPtr<FArmyViewController> SArmyWindow::GetActiveController()
{
    if (ControllerStack.Num() > 0)
    {
        return ControllerStack.Top();
    }

    return nullptr;
}
FReply SArmyWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (FSlateApplication::Get().IsNormalExecution())
	{
		if (GetActiveController()->GetCommandList().IsValid() &&
			GetActiveController()->GetCommandList()->ProcessCommandBindings(InKeyEvent))
		{
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

void SArmyWindow::SetViewportFullscreen(bool bFullscreen)
{
	WindowSwitcher->SetActiveWidgetIndex(bFullscreen ? 1 : 0);

	FSlateApplication::Get().ResetToDefaultInputSettings();
	FSlateApplication::Get().SetAllUserFocus(GEngine->GetGameViewportWidget(), EFocusCause::SetDirectly);
}

void SArmyWindow::NotifyQuitCurrentViewController()
{
	TSharedPtr<FArmyViewController> CurrentController = GetActiveController();
	if (CurrentController.IsValid())
		CurrentController->Quit();
}