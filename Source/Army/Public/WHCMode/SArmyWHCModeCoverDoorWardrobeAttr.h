#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeCoverDoorWardrobeAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeCoverDoorWardrobeAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    virtual TSharedPtr<SWidget> RebuildAttrPanel() override;

private:
    void MakeDimensionUI();
    void MakeDecorationBoardUI();

private:
    TSharedPtr<class FArmyDetailBuilder> CoverDoorWardrobeDetailPanel;
};