#pragma once
#include "SArmyWHCModeCabinetAttr.h"

class FArmyWHCModeVentilatorCabinetAttr final : public FArmyWHCModeCabinetAttr
{
public:
    FArmyWHCModeVentilatorCabinetAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer)
    : FArmyWHCModeCabinetAttr(InContainer) {}

    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;

private:
    void RebuildVentilatorAttrPanel(struct FShapeInRoom *InShapeInRoom, int32 InFlag);

private:
    void Callback_VisModeChanged(const FString &InNewStr);
    void Callback_VentilatorCabUpValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void Callback_VentilatorCabFrontValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
	void Callback_VentilatorCabRightValueCommitted(const FText& InText, const ETextCommit::Type InTextAction);
    void VisMode_BothVentilatorAndCab(SArmyWHCModeAttrPanelContainer *InContainer, struct FShapeInRoom *InShapeInRoom);
    void VisMode_BothVentilatorAndCabInternal(SArmyWHCModeAttrPanelContainer *InContainer, struct FShapeInRoom *InShapeInRoom);
    void VisMode_OnlyVentilatorCab(SArmyWHCModeAttrPanelContainer *InContainer, struct FShapeInRoom *InShapeInRoom);
    void VisMode_OnlyVentilator(SArmyWHCModeAttrPanelContainer *InContainer, struct FShapeInRoom *InShapeInRoom);

private:
    TSharedPtr<SArmyWHCModeNormalComboBox> VisModeCombo;
};