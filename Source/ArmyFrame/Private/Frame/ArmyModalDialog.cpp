#include "SArmyModalDialog.h"
#include "ArmyStyle.h"
#include "ArmyModalManager.h"

void SArmyModalDialog::Construct(const FArguments& InArgs)
{
    OnConfirm = InArgs._OnConfirm;
    OnCancel = InArgs._OnCancel;
	OnImmediatelyClose = InArgs._OnImmediatelyClose;

    bCloseImmediately = InArgs._bCloseImmediately;
	ConfirmText = InArgs._ConfirmText;
	CancelText = InArgs._CancelText;

    ChildSlot
    [
        SNew(SBorder)
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Center)
        .Padding(0)
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.99000000"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        [
            SNew(SBorder)
            .Padding(0)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF28292B"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            [
                SAssignNew(VB_Dialog, SVerticalBox)

                // 标题栏
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SBox)
                    .HeightOverride(48)
                    [
                        SNew(SBorder)
                        .Padding(0)
                        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
                        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                        [
                            SNew(SHorizontalBox)

                            // 标题
                            + SHorizontalBox::Slot()
                            .Padding(16, 0, 0, 0)
                            .AutoWidth()
                            .VAlign(VAlign_Center)
                            [
                                SNew(STextBlock)
                                .Text(FText::FromString(InArgs._Title))
                                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
                            ]

                            // 关闭按钮
                            + SHorizontalBox::Slot()
                            .Padding(0, 0, 5, 0)
                            .FillWidth(1)
                            .HAlign(HAlign_Right)
                            .VAlign(VAlign_Center)
                            [
                                SNew(SBox)
                                .WidthOverride(30)
                                .HeightOverride(30)
                                [
                                    SNew(SButton)
                                    .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
                                    .OnClicked_Raw(this, &SArmyModalDialog::OnCloseButtonClicked)
                                ]
                            ]
                        ]
                    ]
                ]

                // 内容
                + SVerticalBox::Slot()
                .Padding(0)
				.AutoHeight()
                [
                    InArgs._ContentWidget.ToSharedRef()
                ]
            ]
        ]
    ];

    // 如果没有传入回调函数，则不显示确定、取消按钮
    if (InArgs._OnConfirm.IsBound())
    {
        TSharedRef<SWidget> ConfirmCancelWidget = MakeConfirmCancelButtons();

        // 如果没有传入标题，则默认不是自定义弹框，确定、取消按钮居中
        if (InArgs._Title.Equals(""))
        {
            VB_Dialog->AddSlot()
                .FillHeight(1)
                .Padding(0, 0, 0, 20)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    ConfirmCancelWidget
                ];
        }
        else
        {
            VB_Dialog->AddSlot()
                .Padding(0, 20, 20, 20)
                .FillHeight(1)
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Bottom)
                [
                    ConfirmCancelWidget
                ];
        }
    }
}

FReply SArmyModalDialog::OnConfirmButtonClicked()
{
    OnConfirm.ExecuteIfBound();

    if (bCloseImmediately)
    {
		OnImmediatelyClose.ExecuteIfBound();
    }

    return FReply::Handled();
}

FReply SArmyModalDialog::OnCloseButtonClicked()
{
	OnImmediatelyClose.ExecuteIfBound();
    return FReply::Handled();
}

FReply SArmyModalDialog::OnCancelButtonClicked()
{
    OnCancel.ExecuteIfBound();
    return OnCloseButtonClicked();
}

TSharedRef<SWidget> SArmyModalDialog::MakeConfirmCancelButtons()
{
    return
        SNew(SHorizontalBox)

        + SHorizontalBox::Slot()
        .Padding(0, 0, 20, 0)
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
                .OnClicked(this, &SArmyModalDialog::OnConfirmButtonClicked)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(ConfirmText))
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                ]
            ]
        ]
            
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SBox)
            .WidthOverride(80)
            .HeightOverride(30)
            [
                SNew(SButton)
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
                .OnClicked_Raw(this, &SArmyModalDialog::OnCancelButtonClicked)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(CancelText))
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                ]
            ]
        ];
}
