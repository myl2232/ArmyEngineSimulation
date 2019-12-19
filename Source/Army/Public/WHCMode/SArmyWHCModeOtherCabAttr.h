#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeOtherCabAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeOtherCabAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    virtual TSharedPtr<SWidget> RebuildAttrPanel() override;

private:
    void MakeDimensionUI();
    void MakeDecorationBoardUI(struct FShapeInRoom *InShapeInRoom);
    void MakePlatformUI(struct FShapeInRoom *InShapeInRoom);

private:
    TSharedPtr<class FArmyDetailBuilder> OtherCabDetailPanel;
};