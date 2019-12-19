#include "ArmyTextButtonSlate.h"
#include "ArmyStyle.h"

#include "SToolTip.h"

void SArmyTextButton::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBox)
        .WidthOverride(78)
        .HeightOverride(102)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SBox)
                .HeightOverride(78)
                [
                    SNew(SButton)
                    .ButtonStyle(InArgs._ButtonStyle)
                    .OnClicked(InArgs._OnClicked)
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Center)
            .Padding(InArgs._TextPadding)
            [
                SNew(STextBlock)
                .Text(InArgs._Text)
                .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
            ]
        ]
    ];
}