#pragma once
#include "SArmyWHCModeAttrPanel.h"

class FArmyWHCModeBathroomCabAttr final : public FArmyWHCModeAttrPanel
{
public:

    FArmyWHCModeBathroomCabAttr(TSharedRef<SArmyWHCModeAttrPanelContainer> InContainer);
    virtual void TickPanel() override;
    virtual void Initialize() override;
    virtual bool TryDisplayAttrPanel(class AActor *InActor) override;
    virtual void LoadMoreReplacingData() override;
    virtual TSharedPtr<SWidget> RebuildAttrPanel() override;

private:
    void MakeDimensionUI();

    void MakeDecorationBoardUI();
	//�Ű�����
	void MakeDoorPlankAttributesUI(FShapeInRoom *InShapeInRoom);
	//��������
	void MakeDrawerFaceModel(FShapeInRoom *InShapeInRoom);
	//�Ű���
	void SpawnModeBathRoomSlideDoorText(FDoorGroup* pDoorGroup, int32 groupIndex = 1);
	//������
	void SpawnModeBathRoomDrawerDoorText(FDoorGroup* pDoorGroup, int32 groupIndex = 1);
private:
    TSharedPtr<class FArmyDetailBuilder> BathroomCabDetailPanel;
};