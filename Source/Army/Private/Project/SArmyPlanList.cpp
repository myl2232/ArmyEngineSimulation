#include "SArmyPlanList.h"
#include "ArmyStyle.h"
#include "SImage.h"

void SArmyPlanList::Construct(const FArguments& InArgs)
{
	OnNewProjectClicked = InArgs._OnNewPlanClicked;

    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(624)
        .HeightOverride(1012)
        [
            SNew(SOverlay)
            
            + SOverlay::Slot()
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SBorder)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF343539"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                .Padding(FMargin(28, 32, 0, 0))
                [
                    SNew(SVerticalBox)

					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SAssignNew(TB_ProjectName, STextBlock)
						.TextStyle(FArmyStyle::Get(), "ArmyText_24_Bold")
					]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Fill)
                    .Padding(0, 24, 40, 0)
                    [
                        SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .FillWidth(0.5f)
                        [
                            SAssignNew(TB_MealID, STextBlock)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(0.5f)
                        [
                            SAssignNew(TB_ContractID, STextBlock)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Fill)
                    .Padding(0, 16, 40, 0)
                    [
                        SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .FillWidth(0.5f)
                        [
                            SAssignNew(TB_OwnerName, STextBlock)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(0.5f)
                        [
                            SAssignNew(TB_OwnerPhoneNumber, STextBlock)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Fill)
                    .Padding(0, 16, 40, 0)
                    [
                        SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .FillWidth(0.5f)
                        [
                            SAssignNew(TB_Location, STextBlock)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]

                        + SHorizontalBox::Slot()
                        .FillWidth(0.5f)
                        [
                            SAssignNew(TB_VillageName, STextBlock)
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Fill)
                    .Padding(0, 16, 40, 0)
                    [
                        SAssignNew(TB_DetailAddress, SMultiLineEditableText)
                        .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        .AllowContextMenu(false)
                        .AutoWrapText(true)
                        .IsReadOnly(true)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(0, 30, 40, 0)
                    [
                        SNew(SImage)
                        .Image(FArmyStyle::Get().GetBrush("Splitter.FF46474D"))
                    ]

                    + SVerticalBox::Slot()
                    .FillHeight(1)
                    [
                        SAssignNew(Container, SScrollWrapBox)
                        .WrapBoxPadding(FMargin(0, 20, 32, 0))
                        .InnerSlotPadding(FVector2D(16, 38))
                    ]
                ]
            ]

            + SOverlay::Slot()
            .Padding(0, 10, 10, 0)
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Top)
            [
                SNew(SButton)
                .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
                .OnClicked(this, &SArmyPlanList::OnClosed)
            ]
        ]
	];
	//@梁晓菲 添加新建方案按钮
	Container->AddItem(MakeNewPlanButton());
}

void SArmyPlanList::Empty()
{
	Container->ClearChildren();
	//@梁晓菲 新建方案按钮
	Container->AddItem(MakeNewPlanButton());
}

void SArmyPlanList::SetProjectName(const FString& InProjectTitle)
{
	TB_ProjectName->SetText(FText::FromString(InProjectTitle));
}

void SArmyPlanList::SetMealID(const FString& InMealID)
{
    TB_MealID->SetText(FText::FromString(InMealID));
}

void SArmyPlanList::SetContractID(const FString& InContractID)
{
    TB_ContractID->SetText(FText::FromString(InContractID));
}

void SArmyPlanList::SetOwnerName(const FString& InOwnerName)
{
    TB_OwnerName->SetText(FText::FromString(InOwnerName));
}

void SArmyPlanList::SetOwnerPhoneNumber(const FString& InOwnerPhoneNumber)
{
    TB_OwnerPhoneNumber->SetText(FText::FromString(InOwnerPhoneNumber));
}

void SArmyPlanList::SetLocation(const FString& InLocation)
{
    TB_Location->SetText(FText::FromString(InLocation));
}

void SArmyPlanList::SetVillageName(const FString& InVillageName)
{
    TB_VillageName->SetText(FText::FromString(InVillageName));
}

void SArmyPlanList::SetDetailAddress(const FString& InDetailAddress)
{
    TB_DetailAddress->SetText(FText::FromString(InDetailAddress));
}

void SArmyPlanList::AddPlanItem(TSharedRef<SWidget> PlanItem)
{
    Container->AddItem(PlanItem);
}

FReply SArmyPlanList::OnClosed()
{
    SetVisibility(EVisibility::Hidden);

    return FReply::Handled();
}

TSharedRef<SWidget> SArmyPlanList::MakeNewPlanButton()
{
    return
        SNew(SButton)
        .ContentPadding(FMargin(0))
        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
        .OnClicked(OnNewProjectClicked)
        .OnHovered(this, &SArmyPlanList::OnNewPlanHovered)
        .OnUnhovered(this, &SArmyPlanList::OnNewPlanUnhovered)
        [
            SNew(SBox)
            .WidthOverride(268)
            .HeightOverride(268)
            [
                SAssignNew(NewPlanBorder, SBorder)
                .Padding(FMargin(2))
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                [
                    SNew(SBorder)
                    .Padding(FMargin(0))
                    .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FF1C1D1E"))
                    .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                    [
                        SNew(SVerticalBox)

                        + SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Center)
						.Padding(FMargin(0, 72, 0, 32))
                        [
							SNew(SBox)
							.WidthOverride(72.f)
							.HeightOverride(72.f)
							[
								SNew(SImage)
								.Image(FArmyStyle::Get().GetBrush("Icon.NewPlan"))
							]
                        ]

                        + SVerticalBox::Slot()
                        .VAlign(VAlign_Top)
                        .HAlign(HAlign_Center)
                        [
                            SNew(STextBlock)
                            .Text(FText::FromString(TEXT("新建方案")))
                            .TextStyle(FArmyStyle::Get(), "ArmyText_14")
                        ]
                    ]
                ]
            ]
        ];

}

void SArmyPlanList::OnNewPlanHovered()
{
	NewPlanBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.FFFD9800"));
}

void SArmyPlanList::OnNewPlanUnhovered()
{
	NewPlanBorder->SetBorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"));
}
