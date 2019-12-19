#pragma once
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCAboveGroundUI final : public TXRWHCAttrUIComponent<FArmyWHCAboveGroundUI>
{
public:
    FArmyWHCAboveGroundUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCAboveGroundUI>(InContainer) 
    { Type = EWHCAttrUIType::EWAUT_AboveGround; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

private:
    bool CheckIfChangeAllCabHeights(struct FShapeInRoom *InShapeInRoom) const;
};