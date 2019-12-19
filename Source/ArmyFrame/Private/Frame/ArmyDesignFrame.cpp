#include "SArmyDesignFrame.h"
#include "ArmyEditorEngine.h"
#include "ArmyStyle.h"

#include "SImage.h"

void SArmyDesignFrame::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SOverlay)

        + SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SHorizontalBox)

            // 左面板
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SBorder)
                .Padding(0)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF191A1D"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                [
                    SAssignNew(LeftPanel, SBox)
                ]
            ]
        
            + SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SNew(SOverlay)

                // 视口
                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Fill)
                [
                    GArmyEditor->GetGameViewportWidget().ToSharedRef()
                ]

                // 设置栏
                + SOverlay::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Bottom)
                .Padding(2, 0, 2, 0)
                [
                    SNew(SBorder)
                    .Padding(0)
                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF191A1D"))
                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                    [
                        SAssignNew(SettingBar, SBox)
                    ]
                ]
            ]

            // 右面版
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Right)
            .AutoWidth()
            [
                SNew(SBorder)
                .Padding(0)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                [
                    SAssignNew(RightPanel, SBox)
                    .WidthOverride(240)
                ]
            ]
        ]

        // 工具栏
        + SOverlay::Slot()
        .HAlign(HAlign_Center)
        .VAlign(VAlign_Top)
        .Padding(0, 4, 0, 0)
        [
            SNew(SBorder)
            .Padding(FMargin(16, 8, 16, 8))
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF2F3033"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            [
                SAssignNew(ToolBar, SBox)
            ]
        ]
    ];
}

void SArmyDesignFrame::HideDesignPanel(bool bHidden)
{
    EVisibility Visibility = bHidden ? EVisibility::Collapsed : EVisibility::Visible;
    LeftPanel->SetVisibility(Visibility);
    RightPanel->SetVisibility(Visibility);
    ToolBar->SetVisibility(Visibility);
    SettingBar->SetVisibility(Visibility);
}