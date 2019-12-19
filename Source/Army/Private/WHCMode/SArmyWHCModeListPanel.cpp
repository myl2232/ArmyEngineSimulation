#include "SArmyWHCModeListPanel.h"
#include "ArmyStyle.h"
#include "SScrollWrapBox.h"
#include "SContentItem.h"

void SArmyWHCModeListPanel::Construct(const FArguments &InArgs)
{
    ChildSlot
        [
            SNew(SBox)
            .WidthOverride(280.0f)
            [
                SNew(SBorder)
                .Padding(0)
                .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
                .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
                [
                    SNew(SVerticalBox)

                    // 标题栏
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SBox)
                        .HeightOverride(32.0f)
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
                                .FillWidth(1.0f)
                                .VAlign(VAlign_Center)
                                [
                                    SAssignNew(PanelTitle, STextBlock)
                                    // .Text(FText::FromString(TEXT("柜子类型")))
                                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                                ]

                                // 关闭按钮
                                + SHorizontalBox::Slot()
                                .Padding(0, 0, 5, 0)
                                .AutoWidth()
                                .HAlign(HAlign_Center)
                                .VAlign(VAlign_Center)
                                [
                                    SNew(SBox)
                                    .WidthOverride(30)
                                    .HeightOverride(30)
                                    [
                                        SNew(SButton)
                                        .ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.CloseApplication"))
                                        .OnClicked(InArgs._OnPanelClosed)
                                    ]
                                ]
                            ]
                        ]
                    ]

                    // 控制界面
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Expose(ListControllerSlot)

                    // 内容
                    + SVerticalBox::Slot()
                    .Padding(16.0f, 16.0f, 0.0f, 16.0f)
                    .FillHeight(1.0f)
                    [
                        SAssignNew(Container, SScrollWrapBox)
                        .InnerSlotPadding(FVector2D(16.0f, 16.0f))
                    ]
                ]
            ]
        ];

    LastSelected = nullptr;
}

void SArmyWHCModeListPanel::SetCurrentState(const FString &InNewState)
{
    CurrentState = InNewState;
    PanelTitle->SetText(FText::FromString(InNewState));
}

void SArmyWHCModeListPanel::SetListController(TSharedRef<SWidget> InController)
{
    ListControllerSlot->operator[] (InController);
}

void SArmyWHCModeListPanel::ClearListController()
{
    ListControllerSlot->DetachWidget();
}

void SArmyWHCModeListPanel::AddItem(const TSharedPtr<SContentItem> &InNewItem)
{
    Container->AddItem(InNewItem);
    ContentItemList.Emplace(InNewItem);
}

const TArray<TSharedPtr<SContentItem>>& SArmyWHCModeListPanel::GetItems() const
{
    return ContentItemList;
}

void SArmyWHCModeListPanel::ClearItems()
{
    LastSelected = nullptr;
    Container->ClearChildren();
    ContentItemList.Empty();
}

void SArmyWHCModeListPanel::SetSelectedItem(int32 InIndex)
{
    if (InIndex < GetItems().Num() && InIndex >= 0)
    {
        if (LastSelected != nullptr)
            LastSelected->SetSelected(false);
        const TSharedPtr<SContentItem> & Item = GetItems()[InIndex];
        LastSelected = Item.Get();
        LastSelected->SetSelected(true);   
    }
}

SContentItem* SArmyWHCModeListPanel::GetSelectedItem() const
{
    return LastSelected;
}

int32 SArmyWHCModeListPanel::GetSelectedItemIndex() const
{
    return GetItems().IndexOfByPredicate([this](const TSharedPtr<SContentItem> & Item) -> bool { return LastSelected == Item.Get(); } );
}

FReply SArmyWHCModeListPanel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    SCompoundWidget::OnMouseButtonDown(MyGeometry, MouseEvent);
    return FReply::Handled();
}

FReply SArmyWHCModeListPanel::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    SCompoundWidget::OnMouseButtonUp(MyGeometry, MouseEvent);
    return FReply::Handled();
}

FReply SArmyWHCModeListPanel::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    SCompoundWidget::OnMouseWheel(MyGeometry, MouseEvent);
    return FReply::Handled();
}

FReply SArmyWHCModeListPanel::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    SCompoundWidget::OnMouseButtonDoubleClick(MyGeometry, MouseEvent);
    return FReply::Handled();
}