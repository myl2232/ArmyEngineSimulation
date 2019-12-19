#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeTopBlockerAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeTopBlockerAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    
private:
    TSharedPtr<class FArmyDetailBuilder> TopBlockerPanel;
};