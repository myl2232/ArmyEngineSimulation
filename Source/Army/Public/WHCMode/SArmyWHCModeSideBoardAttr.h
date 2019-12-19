#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeSideBoardAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeSideBoardAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;

private:
    TSharedPtr<SVerticalBox> SideBoardPanel;
};