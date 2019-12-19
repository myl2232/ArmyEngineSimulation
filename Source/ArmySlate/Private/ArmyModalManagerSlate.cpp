// Fill out your copyright notice in the Description page of Project Settings.

#include "ArmyModalManagerSlate.h"
#include "SlateOptMacros.h"
#include "ArmySlateMessageSlate.h"
#include "ArmyMessageMiniSlate.h"
#include "SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<FArmyModalManager> FArmyModalManager::ArmyModalMgr = nullptr;

FArmyModalManager::FArmyModalManager()
{
	ErrorMessageDuaration = 2.5f;
	ErrorMessageElapsedTime = 0.f;
	bCountErrorMessageTime = false;
}

bool FArmyModalManager::IsTickable() const
{
	return true;

}

TStatId FArmyModalManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FArmyModalManager, STATGROUP_Tickables);
}

void FArmyModalManager::Tick(float InDeltaTime)
{
	if (bCountErrorMessageTime && ErrorMessageElapsedTime < ErrorMessageDuaration)
	{
		ErrorMessageElapsedTime += InDeltaTime;
		if (ErrorMessageElapsedTime >= ErrorMessageDuaration)
		{
			SetCustomWindowState(ECWS_Normal);
			bCountErrorMessageTime = false;
		}
	}
}

FArmyModalManager& FArmyModalManager::Get()
{
	if (!ArmyModalMgr.IsValid())
	{
		ArmyModalMgr = MakeShareable(new FArmyModalManager());
	}
	return *ArmyModalMgr;
}

void FArmyModalManager::ShowWindow(ECommonWindowType _WindowType, FText _WindowTitle)
{
	bCustomWindow = false;
	if (MyWindow.IsValid())
	{
		MyWindow->SetOnWindowClosed(FOnWindowClosed());
		MyWindow->RequestDestroyWindow();
		MyWindow = NULL;
	}

	ConfirmDelegate.Unbind();
	CancelDelegate.Unbind();
	OverlayCancelDelegate.Unbind();
	MessageText = FText::GetEmpty();

	//弹出框
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();

	SAssignNew(MyWindow, SWindow)
		.Title(_WindowTitle)
		.SizingRule(ESizingRule::Autosized)
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.HasCloseButton(false)
		.SupportsMinimize(false)
		.SupportsMaximize(false);

	TSharedPtr<SArmySlateMessage> MessageWidget = SNew(SArmySlateMessage)
		.AlignCenter(false)
		.Rolling(_WindowType == ECommonWindowType::E_RollingMessageCancel || _WindowType == ECommonWindowType::E_RollingMessage)
		.Confirm(_WindowType == ECommonWindowType::E_MessageConfirm || _WindowType == ECommonWindowType::E_MessageConfirmCancel)
		.Cancel(_WindowType == ECommonWindowType::E_MessageConfirmCancel || _WindowType == ECommonWindowType::E_RollingMessageCancel)
		.MessageText_Raw(this, &FArmyModalManager::GetMessageText);

    MakeWindowWidget(_WindowTitle, MessageWidget, false);

	MessageWidget->ConfirmDelegate.BindRaw(this, &FArmyModalManager::OnConfirmClicked);
	MessageWidget->CancelDelegate.BindRaw(this, &FArmyModalManager::OnCancelClicked);

	//FSlateApplication::Get().AddWindowAsNativeChild(MyWindow.ToSharedRef(), ParentWindow.ToSharedRef(), true);
	FSlateApplication::Get().AddModalWindow(MyWindow.ToSharedRef(), ParentWindow, true);
	MyWindow->ShowWindow();
}

void FArmyModalManager::ShowCustomWindow(FText _WindowTitle, TSharedPtr<SWidget> _CustomWidget, bool InbModal)
{
	bCustomWindow = true;
	if (MyWindow.IsValid())
	{
		MyWindow->SetOnWindowClosed(FOnWindowClosed());
		MyWindow->RequestDestroyWindow();
		MyWindow = NULL;
	}

	ConfirmDelegate.Unbind();
	CancelDelegate.Unbind();
	OverlayCancelDelegate.Unbind();
	MessageText = FText::GetEmpty();

	//弹出框
	TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().GetActiveTopLevelWindow();

    MakeWindowWidget(_WindowTitle, _CustomWidget);

	if (InbModal)
	{
		FSlateApplication::Get().AddModalWindow(MyWindow.ToSharedRef(), ParentWindow, true);
	}
	else
	{
		FSlateApplication::Get().AddWindowAsNativeChild(MyWindow.ToSharedRef(), ParentWindow.ToSharedRef(), true);
	}
	MyWindow->ShowWindow();
}

void FArmyModalManager::SetCustomWindowState(ECustomWindowState InState)
{
	if (bCustomWindow && MessageMini.IsValid())
	{
		if (InState == ECWS_Normal)
		{
			MessageMini.Pin()->SetVisibility(EVisibility::Collapsed);
		}
		else if (InState == ECWS_MessageAutoDispear)
		{
			MessageMini.Pin()->SetVisibility(EVisibility::Visible);
			MessageMini.Pin()->SetErrorVisibility(EVisibility::Visible);
			MessageMini.Pin()->SetRollingVisibility(EVisibility::Collapsed);
			MessageMini.Pin()->SetCancelVisibility(EVisibility::Collapsed);
			bCountErrorMessageTime = true;
			ErrorMessageElapsedTime = 0.f;
		}
		else if (InState == ECWS_RollingMessageCancel)
		{
			MessageMini.Pin()->SetVisibility(EVisibility::Visible);
			MessageMini.Pin()->SetErrorVisibility(EVisibility::Collapsed);
			MessageMini.Pin()->SetRollingVisibility(EVisibility::Visible);
			MessageMini.Pin()->SetCancelVisibility(EVisibility::Visible);
		}
	}
}

void FArmyModalManager::HideWindow()
{
	if (MyWindow.IsValid())
	{
		MyWindow->RequestDestroyWindow();
		MyWindow = NULL;
	}
}

void FArmyModalManager::SetMessage(FText _Message)
{
	MessageText = _Message;
}

void FArmyModalManager::MakeWindowWidget(const FText& InWindowTitle, TSharedPtr<class SWidget> InMessageWidget, bool bCreateTitle/* = true*/)
{
    SAssignNew(MyWindow, SWindow)
        .CreateTitleBar(false)
		.SizingRule(ESizingRule::Autosized)
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.HasCloseButton(true)
		.SupportsMinimize(false)
		.SupportsMaximize(false);

    // 创建自定义的弹出框样式
    TSharedPtr<SVerticalBox> MessageBoxWidget = NULL;
    SAssignNew(MessageBoxWidget, SVerticalBox);

    if (bCreateTitle)
    {
        MessageBoxWidget->AddSlot()
        .AutoHeight()
        [
            SNew(SBorder)
            .Padding(FMargin(0))
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF191A1D"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            [
                SNew(SBox)
                .HeightOverride(48)
                [
                    SNew(SHorizontalBox)

                    + SHorizontalBox::Slot()
                    .Padding(20, 0, 0, 0)
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(InWindowTitle)
                        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                    ]
                ]
            ]
        ];

        MessageBoxWidget->AddSlot()
        .AutoHeight()
        [
            SNew(SImage)
            .Image(FArmyStyle::Get().GetBrush("Splitter.Thin1"))
        ];
    }

    MessageBoxWidget->AddSlot()
    .FillHeight(1)
    [
        SNew(SBorder)
        .Padding(FMargin(0))
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF191A1D"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        .Padding(FMargin(0))
        [
			SNew(SOverlay)
			+ SOverlay::Slot()
            [
				InMessageWidget.ToSharedRef()
			]
				
			+ SOverlay::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SAssignNew(MessageMini, SArmyMessageMini)
				.MessageText_Raw(this, &FArmyModalManager::GetMessageText)
				.CancelDelegate_Raw(this, &FArmyModalManager::OnOverlayCloseClicked)
				.Visibility(EVisibility::Collapsed)
			]
        ]
    ];

	MyWindow->SetContent(MessageBoxWidget.ToSharedRef());
}

void FArmyModalManager::OnConfirmClicked()
{
	ConfirmDelegate.ExecuteIfBound();
	HideWindow();
}

void FArmyModalManager::OnCancelClicked()
{
	CancelDelegate.ExecuteIfBound();
	HideWindow();
}

void FArmyModalManager::OnOverlayCloseClicked()
{
	OverlayCancelDelegate.ExecuteIfBound();
	if (MessageMini.IsValid())
	{
		MessageMini.Pin()->SetVisibility(EVisibility::Collapsed);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION