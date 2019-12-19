#include "UIMixin/SArmyWHCModificationRangeUI.h"
#include "SArmyWHCModeAttrPanel.h"

TSharedPtr<SWidget> FArmyWHCModificationRangeUI::MakeWidget()
{
    if (!MRangeUI.IsValid())
    {
        SAssignNew(MRangeUI, SArmyWHCModeModificationRange).bHasSingleRadio(bHasSingleRadio);
    }
    return MRangeUI;
}

void FArmyWHCModificationRangeUI::SetMRange(int32 InRange)
{
    if (MRangeUI.IsValid())
        MRangeUI->Reset((SArmyWHCModeModificationRange::EModificationRange)InRange);
    CachedMRange = InRange;
}

void FArmyWHCModificationRangeUI::SyncMRange()
{
    if (MRangeUI.IsValid())
        CachedMRange = (int32)MRangeUI->GetRange();
}

bool FArmyWHCModificationRangeUI::IsMRangeChanged() const
{
    return MRangeUI.IsValid() ? CachedMRange != MRangeUI->GetRange() : false;
}