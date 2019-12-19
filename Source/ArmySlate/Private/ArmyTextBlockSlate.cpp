#include "ArmyTextBlockSlate.h"
#include "SImage.h"
#include "STextBlock.h"

void SArmyTextBlock::Construct(const FArguments& InArgs)
{
    OnClicked = InArgs._OnClicked;

	NormalColor = InArgs._ColorAndOpacity;
	UnderlineNormalImage = InArgs._UnderlineNormalImage;

    TSharedPtr<SVerticalBox> Container = nullptr;

    ChildSlot
    [
        SAssignNew(Container, SVerticalBox)
        
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew(TextBlock, STextBlock)
            .TextStyle(InArgs._TextStyle)
            .Text(InArgs._Text)
            .ColorAndOpacity(InArgs._ColorAndOpacity)
        ]
    ];

    if (InArgs._bShowUnderline)
    {
        Container->AddSlot()
            .AutoHeight()
            [
                SAssignNew(UnderlineImage, SImage)
                .Image(InArgs._UnderlineNormalImage)
                .Visibility(EVisibility::Collapsed)
            ];
    }
}

FReply SArmyTextBlock::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    OnClicked.ExecuteIfBound();

    return FReply::Handled();
}

void SArmyTextBlock::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    TextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFD9800"));
    if (UnderlineImage.IsValid())
    {
        UnderlineImage->SetImage(FArmyStyle::Get().GetBrush("Splitter.Orange.FFFD9800"));
        UnderlineImage->SetVisibility(EVisibility::HitTestInvisible);
    }
}

void SArmyTextBlock::OnMouseLeave(const FPointerEvent& MouseEvent)
{
    TextBlock->SetColorAndOpacity(NormalColor);
    if (UnderlineImage.IsValid())
    {
        UnderlineImage->SetImage(UnderlineNormalImage);
        UnderlineImage->SetVisibility(EVisibility::Collapsed);
    }
}