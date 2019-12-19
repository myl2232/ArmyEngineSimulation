#include "ArmyToolTipSlate.h"
#include "SBorder.h"
#include "STextBlock.h"
#include "ArmyStyle.h"

void SArmyToolTip::Construct(const FArguments& InArgs)
{
    SToolTip::Construct(SToolTip::FArguments()
        .TextMargin(FMargin(1))
        .BorderImage(FArmyStyle::Get().GetBrush("Border.Gray.FFC8C9CC"))
        .Content()
        [
            SNew(SBorder)
            .Padding(FMargin(8, 6))
            .BorderBackgroundColor(FLinearColor::White)
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
            [
                SNew(STextBlock)
                .Text(InArgs._Text)
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
            ]
        ]);
}