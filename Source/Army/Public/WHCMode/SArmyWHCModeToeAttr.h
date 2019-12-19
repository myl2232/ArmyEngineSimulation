#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeToeAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeToeAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;

private:
    TSharedPtr<class FArmyDetailBuilder> ToePanel;
};