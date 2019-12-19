#pragma once
#include "SArmyWHCModeAttrPanel.h"
#include "Widgets/Text/STextBlock.h"

class FArmyWHCModeDrawerAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeDrawerAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;

private:
    TSharedPtr<class FArmyDetailBuilder> DrawerDetailPanel;
};