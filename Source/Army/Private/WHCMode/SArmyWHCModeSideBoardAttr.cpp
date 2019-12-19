#include "SArmyWHCModeSideBoardAttr.h"
#include "ArmyShapeTableActor.h"
#include "ArmyStyle.h"

FArmyWHCModeSideBoardAttr::FArmyWHCModeSideBoardAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer)
: FArmyWHCModeAttrPanel(InContainer)
{}

void FArmyWHCModeSideBoardAttr::TickPanel()
{

}

void FArmyWHCModeSideBoardAttr::Initialize()
{
    SAssignNew(SideBoardPanel, SVerticalBox)
    + SVerticalBox::Slot()
    .VAlign(VAlign_Top)
    .HAlign(HAlign_Center)
    .Padding(0.0f, 16.0f, 0.0f, 0.0f)
    [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("见光板功能开发中，敬请期待")))
        .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
        .ColorAndOpacity(FLinearColor::White)
    ];
}

bool FArmyWHCModeSideBoardAttr::TryDisplayAttrPanel(AActor *InActor)
{
    if (InActor->IsA<AXRWhcGapActor>())
    {
        Container.Pin()->SetAttrPanel(SideBoardPanel.ToSharedRef());
        return true;
    }
    else
        return false;
}

void FArmyWHCModeSideBoardAttr::LoadMoreReplacingData()
{

}