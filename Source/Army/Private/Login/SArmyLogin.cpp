#include "SArmyLogin.h"
#include "Style/XRStyle.h"
#include "SArmyTextButton.h"
#include "SArmyEditableTextBox.h"
#include "SImage.h"
#include "ArmyUser.h"
#include "ConfigCacheIni.h"
#include "SArmyTextBlock.h"
#include "ArmyHttpModule.h"

void SArmyLogin::Construct(const FArguments& InArgs)
{
    OnLogin = InArgs._OnLogin;

    ChildSlot
    [
        SNew(SOverlay)

        + SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
        [
            SNew(SBox)			
			.WidthOverride(468)
			.HeightOverride(768)
            [
                SNew(SOverlay)

                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                    SNew(SImage)
                    .Image(FArmyStyle::Get().GetBrush("Launcher.LoginBackground1"))
                ]

                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .Padding(0, 140, 0, 0)
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    [
                        SNew(SImage)
                        .Image(FArmyStyle::Get().GetBrush("Launcher.Logo"))
                    ]

                    + SVerticalBox::Slot()
                    .Padding(0, 80, 0, 0)
                    .HAlign(HAlign_Center)
                    .AutoHeight()
                    [
                        SAssignNew(ETB_Username, SArmyEditableTextBox)
                        .IconImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.Username")))
                        .HintText(FText::FromString(TEXT("请输入手机号/账号")))
                    ]

                    + SVerticalBox::Slot()
                    .Padding(0, 20, 0, 0)
                    .HAlign(HAlign_Center)
                    .AutoHeight()
                    [
						SAssignNew(ETB_Pwd, SArmyEditableTextBox)
                        .IconImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.Password")))
                        .HintText(FText::FromString(TEXT("请输入密码")))
                        .IsPassword(true)
						.OnTextCommitted(this, &SArmyLogin::OnPwdCommitted)
                    ]

                    + SVerticalBox::Slot()
                    .Padding(70, 5, 70, 0)
                    .AutoHeight()
                    [
                        SAssignNew(TB_ErrorMessage, STextBlock)
                        .Text(FText::FromString(TEXT("账号名或密码错误")))
                        .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                        .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF5A5A"))
                        .Visibility(EVisibility::Collapsed)
                    ]

                    + SVerticalBox::Slot()
                    .Padding(70, 18, 70, 20)
                    .AutoHeight()
                    [
                        CreateOptionBox()
                    ]

                    + SVerticalBox::Slot()
                    .Padding(0, 40, 0, 0)
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    [
                        SNew(SBox)
                        .WidthOverride(328)
                        .HeightOverride(44)
                        [
                            SNew(SButton)
                            .HAlign(HAlign_Center)
                            .VAlign(VAlign_Center)
                            .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
                            .OnClicked(this, &SArmyLogin::OnLoginButtonClicked)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(TEXT("登录")))
                                .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                            ]
                        ]
                    ]

                    //+ SVerticalBox::Slot()
                    //.Padding(0, 64, 0, 0)
                    //.HAlign(HAlign_Center)
                    //.VAlign(VAlign_Top)
                    //[
                    //    SNew(SArmyTextBlock)
                    //    .bShowUnderline(true)
                    //    .Text(FText::FromString(TEXT("免费申请试用账号")))
                    //    .OnClicked(this, &SArmyLogin::OnApply)
                    //]

                    + SVerticalBox::Slot()
                    .Padding(70, 64, 70, 20)
                    .FillHeight(1)
                    .VAlign(VAlign_Bottom)
                    [
                        CreateVersionTab()
                    ]
                ]                
            ]
        ]
    ];
}

FReply SArmyLogin::OnLoginButtonClicked()
{
    FString UserName = ETB_Username->GetText().ToString();
    FString Password = ETB_Pwd->GetText().ToString();
    OnLogin.ExecuteIfBound(UserName, Password);

    return FReply::Handled();
}

void SArmyLogin::OnApply()
{
    FString URL = "https://www.dabanjia.com/html/dbj_bim.html#apply";
    FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

void SArmyLogin::OnForgetPwd()
{
	FString URL = FArmyHttpModule::Get().GetConsoleAddress() + "/passwordReset.html";
	FPlatformProcess::LaunchURL(*URL, NULL, NULL);
}

TSharedRef<SWidget> SArmyLogin::CreateOptionBox()
{
    return
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SAssignNew(CB_RememberMe, SCheckBox)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
        ]

        + SHorizontalBox::Slot()
        .Padding(10, 0)
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("记住密码")))
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
        ]

        + SHorizontalBox::Slot()
        .FillWidth(1)
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Center)
        [
            SAssignNew(CB_AutoLogin, SCheckBox)
			.IsChecked(this,&SArmyLogin::IsAutoChecked)
			.OnCheckStateChanged(this, &SArmyLogin::OnCheckStateChanged)
            .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(10, 0, 0, 0)
		.HAlign(HAlign_Right)
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text(FText::FromString(TEXT("自动登录")))
            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
        ]
		
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Center)
		[
			SNew(SArmyTextBlock)
			.bShowUnderline(false)
			.Text(FText::FromString(TEXT("忘记密码")))
			.OnClicked(this, &SArmyLogin::OnForgetPwd)
		];
}

TSharedRef<SWidget> SArmyLogin::CreateVersionTab()
{
    return
        SNew(SBox)
        .WidthOverride(328)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .FillWidth(1)
            .VAlign(VAlign_Center)
            [
                SNew(SImage)
                .Image(FArmyStyle::Get().GetBrush("Splitter.Thin"))
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .Padding(20, 0)
            [
                SNew(STextBlock)
                .Text(FText::FromString(FString::Printf(TEXT("版本号：%s"), *FArmyUser::Get().GetApplicationVersion())))
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
            ]

            + SHorizontalBox::Slot()
            .FillWidth(1)
            .VAlign(VAlign_Center)
            [
                SNew(SImage)
                .Image(FArmyStyle::Get().GetBrush("Splitter.Thin"))
            ]
        ];
}

void SArmyLogin::OnPwdCommitted(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		OnLoginButtonClicked();
	}
}

//void SArmyLogin::SetAutoLoginCheckState(ECheckBoxState State)
//{
//	CB_AutoLogin->SetIsChecked(State);
//}
void SArmyLogin::OnCheckStateChanged(ECheckBoxState InNewState)
{
	FArmyUser::Get().bAutoLogin = InNewState == ECheckBoxState::Checked ? true : false;
}
ECheckBoxState SArmyLogin::IsAutoChecked() const
{
	return FArmyUser::Get().bAutoLogin ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
void SArmyLogin::EmptyPwdText()
{
	ETB_Pwd->SetText(FText::FromString(TEXT("")));
}
