#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCSlidingDoorWardrobeBBoardUI final : public TXRWHCAttrUIComponent<FArmyWHCSlidingDoorWardrobeBBoardUI>
{
public:
    FArmyWHCSlidingDoorWardrobeBBoardUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCSlidingDoorWardrobeBBoardUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_SlidingDoorWardrobeBBoard; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
};