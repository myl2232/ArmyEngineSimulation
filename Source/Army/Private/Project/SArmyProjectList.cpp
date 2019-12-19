#include "SArmyProjectList.h"
#include "ArmyStyle.h"
#include "ArmyDesignEditor.h"
#include "ArmyGameInstance.h"
#include "SArmyPlanList.h"

#include "SImage.h"

void SArmyProjectList::Construct(const FArguments& InArgs)
{
    OnNewProjectClicked = InArgs._OnNewProjectClicked;

    ChildSlot
    [
        SNew(SBorder)
        .Padding(FMargin(0, 32))
        .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF262729"))
        .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        [
            SAssignNew(Container, SScrollWrapBox)
            .Delegate_ScrollToEnd(InArgs._OnProjectListScrollToEnd)
            .WrapBoxPadding(FMargin(64, 0))
            .InnerSlotPadding(FVector2D(34, 34))
        ]
    ];

    // 添加新建项目按钮
    Container->AddItem(MakeNewProjectButton());
}

void SArmyProjectList::AddProjectWidget(TSharedRef<SWidget> Widget)
{
    Container->AddItem(Widget);
}

void SArmyProjectList::EmptyProjectList()
{
    Container->ClearChildren();
    Container->AddItem(MakeNewProjectButton());
}

TSharedRef<SWidget> SArmyProjectList::MakeNewProjectButton()
{
    return
        SNew(SButton)
        .ContentPadding(FMargin(0))
        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
        .OnClicked(OnNewProjectClicked)
        .OnHovered(this, &SArmyProjectList::OnNewProjectHovered)
        .OnUnhovered(this, &SArmyProjectList::OnNewProjectUnhovered)
        [
            SNew(SBox)
            .WidthOverride(224)
            .HeightOverride(300)
            [
                SAssignNew(NewProjectBorder, SBorder)
                .Padding(FMargin(2))
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                [
                    SNew(SBorder)
                    .Padding(FMargin(7))
                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                    [
                        SNew(SVerticalBox)

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew(SImage)
                            .Image(FArmyStyle::Get().GetBrush("Icon.NewProject"))
                        ]

                        + SVerticalBox::Slot()
                        .FillHeight(1)
                        .VAlign(VAlign_Center)
                        .HAlign(HAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("新建项目")))
                            .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
                        ]
                    ]
                ]
            ]
        ];
}

void SArmyProjectList::OnNewProjectHovered()
{
    NewProjectBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
}

void SArmyProjectList::OnNewProjectUnhovered()
{
    NewProjectBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
}