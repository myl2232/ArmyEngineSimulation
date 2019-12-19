#pragma once
#include "SArmyWHCModeAttrPanel.h"
#include "Widgets/Input/SCheckBox.h"

class FArmyWHCModeDoorAttr final : public FArmyWHCModeAttrPanel
{
public:
    FArmyWHCModeDoorAttr(TSharedRef<class SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    virtual TSharedPtr<SWidget> RebuildAttrPanel() override;

private:
    void Callback_DoorOpenDirValueChanged(const FString &InNewValue);

private:
    TSharedPtr<class FArmyDetailBuilder> DoorDetailPanel;
    TSharedPtr<SArmyWHCModeNormalComboBox> DoorOpenDirComboBox;
};