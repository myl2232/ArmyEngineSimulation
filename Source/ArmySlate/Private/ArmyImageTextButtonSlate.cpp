#include "ArmyImageTextButtonSlate.h"
#include "SImage.h"

void SArmyImageTextButton::Construct(const FArguments& InArgs)
{
    bDisplayImage = (InArgs._Image != nullptr);
    bDisplayText = !InArgs._Text.IsEmpty();

    TSharedPtr<SBox> Container = nullptr;

    ChildSlot
    [
        SAssignNew(Container, SBox)
        .Padding(InArgs._InnerPadding)
        [
			SAssignNew(Button, SButton)
			.ContentPadding(FMargin(0))
			.ButtonStyle(InArgs._ButtonStyle)
			.OnClicked(InArgs._OnClicked)
			.OnHovered(InArgs._OnHovered)
			.OnUnhovered(InArgs._OnUnhovered)
			[
				SAssignNew(Border, SBorder)
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.BorderBackgroundColor(FSlateColor(FLinearColor(0, 0, 0, 0)))
			]
        ]
    ];

    if (InArgs._LayoutOrientation == LO_Vertical) // 垂直布局
    {
        TSharedPtr<SVerticalBox> VerticalBox = SNew(SVerticalBox);

        if (bDisplayImage)
        {
            VerticalBox->AddSlot()
                .FillHeight(1)
                .HAlign(HAlign_Center)
                .VAlign(EVerticalAlignment::VAlign_Bottom)
                [
                    SAssignNew(Image, SImage)
                    .Image(InArgs._Image)
                ];
        }

        if (bDisplayText)
        {
            VerticalBox->AddSlot()
                .FillHeight(1)
                .HAlign(HAlign_Center)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(TB_Text, STextBlock)
                    .ColorAndOpacity(InArgs._TextColor)
                    .Text(InArgs._Text)
                    .TextStyle(InArgs._TextStyle)
                ];
        }

        Border->SetContent(VerticalBox->AsShared());
    }
    else // 水平布局
    {
        TSharedPtr<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

        if (bDisplayImage)
        {
            HorizontalBox->AddSlot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SAssignNew(Image, SImage)
                    .Image(InArgs._Image)
                ];
        }

        if (bDisplayText)
        {
            HorizontalBox->AddSlot()
                .FillWidth(1)
                .Padding(6, 0, 0, 0)
                .VAlign(VAlign_Center)
                [
                    SAssignNew(TB_Text, STextBlock)
                    .ColorAndOpacity(InArgs._TextColor)
                    .Text(InArgs._Text)
                    .TextStyle(InArgs._TextStyle)
                ];
        }

        Border->SetContent(HorizontalBox->AsShared());
    }

    if (InArgs._Width != 0 && InArgs._Height != 0)
    {
        Container->SetWidthOverride(InArgs._Width);
        Container->SetHeightOverride(InArgs._Height);
    }
}

void SArmyImageTextButton::SetButtonStyle(const FButtonStyle& InButtonStyle)
{
	Button->SetButtonStyle(&InButtonStyle);
}

void SArmyImageTextButton::SetBorderColor(const FSlateColor& InBorderBackgroundColor)
{
	Border->SetBorderBackgroundColor(InBorderBackgroundColor);
}

void SArmyImageTextButton::SetTextColor(const FLinearColor& TextColor)
{
    if (bDisplayText)
    {
        TB_Text->SetColorAndOpacity(TextColor);
    }
}

void SArmyImageTextButton::SetImage(const FSlateBrush* InImage)
{
    if (bDisplayImage)
    {
        Image->SetImage(InImage);
    }
}