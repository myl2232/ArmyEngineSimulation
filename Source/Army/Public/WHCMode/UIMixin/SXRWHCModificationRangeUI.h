#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCModificationRangeUI final : public TXRWHCAttrUIComponent<FArmyWHCModificationRangeUI>
{
public:
    FArmyWHCModificationRangeUI(class SArmyWHCModeAttrPanelContainer *InContainer, bool bHasSingleRadio_)
    : TXRWHCAttrUIComponent<FArmyWHCModificationRangeUI>(InContainer)
    , bHasSingleRadio(bHasSingleRadio_)
    { Type = EWHCAttrUIType::EWAUT_ModificationRange; }
    virtual TSharedPtr<SWidget> MakeWidget() override;
    void SetMRange(int32 InRange);
    void SyncMRange();
    int32 GetMRange() const { return CachedMRange; }
    bool IsMRangeChanged() const;

private:
    TSharedPtr<class SArmyWHCModeModificationRange> MRangeUI;
    int32 CachedMRange;
    bool bHasSingleRadio;
};