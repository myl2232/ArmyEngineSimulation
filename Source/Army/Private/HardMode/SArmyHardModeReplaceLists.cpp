#include "SArmyHardModeReplaceLists.h"

void SArmyDoorReplaceList::Construct(const FArguments& InArgs)
{
    SArmyReplaceList::Construct(InArgs);
}

void SArmyDoorReplaceList::SetFilterSize(const float InLength, const float InHeight)
{
    Length = InLength;
    Height = InHeight;
}

TSharedRef<SWidget> SArmyDoorReplaceList::CreateFilterItems()
{
    return
        SNew(SBox)
        .HeightOverride(40)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(16, 0, 0, 0)
            [
                SNew(SCheckBox)
                .Style(FArmyStyle::Get(), "SingleCheckBox")
                .OnCheckStateChanged_Lambda([this](ECheckBoxState CheckState) {
                    bFitSize = (CheckState == ECheckBoxState::Checked);
                    ClearList();
                    ReqNextPage();
                })
            ]
                
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            .Padding(8, 0, 0, 0)
            [
                SNew(STextBlock)
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFC8C9CC"))
                .Text(MAKE_TEXT("仅显示尺寸匹配的素材"))
            ]
        ];
}

void SArmyDoorReplaceList::RebuildFullUrl()
{
    SArmyReplaceList::RebuildFullUrl();

    if (bFitSize)
    {
        FullUrl = FString::Printf(TEXT("%s&length=%f&height=%f"), *FullUrl, Length, Height);
    }
}