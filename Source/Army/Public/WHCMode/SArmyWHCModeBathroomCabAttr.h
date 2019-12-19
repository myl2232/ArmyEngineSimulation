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
	//门板属性
	void MakeDoorPlankAttributesUI(FShapeInRoom *InShapeInRoom);
	//抽面造型
	void MakeDrawerFaceModel(FShapeInRoom *InShapeInRoom);
	//门板编号
	void SpawnModeBathRoomSlideDoorText(FDoorGroup* pDoorGroup, int32 groupIndex = 1);
	//抽面编号
	void SpawnModeBathRoomDrawerDoorText(FDoorGroup* pDoorGroup, int32 groupIndex = 1);
private:
    TSharedPtr<class FArmyDetailBuilder> BathroomCabDetailPanel;
};