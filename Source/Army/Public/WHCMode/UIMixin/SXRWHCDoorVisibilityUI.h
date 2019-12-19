#pragma once
#include "CoreMinimal.h"
#include "../XRWHCAttrPanelMixin.h"

class FArmyWHCDoorVisibilityUI final : public TXRWHCAttrUIComponent<FArmyWHCDoorVisibilityUI>
{
public:
    FArmyWHCDoorVisibilityUI(class SArmyWHCModeAttrPanelContainer *InContainer)
    : TXRWHCAttrUIComponent<FArmyWHCDoorVisibilityUI>(InContainer)
    { Type = EWHCAttrUIType::EWAUT_DoorVisibility; }
    virtual TSharedPtr<SWidget> MakeWidget() override;

private:
    TArray<class FDoorGroup*> GetDoorGroups(struct FShapeInRoom *InShapeInRoom) const;
};

//////////////////////////////////////////////////////////////////////////
// drawer face door
//////////////////////////////////////////////////////////////////////////

class FArmyWHCabDrawerDoorVisibilityUI final : public TXRWHCAttrUIComponent<FArmyWHCabDrawerDoorVisibilityUI>
{
public:
	FArmyWHCabDrawerDoorVisibilityUI(class SArmyWHCModeAttrPanelContainer *InContainer)
		: TXRWHCAttrUIComponent<FArmyWHCabDrawerDoorVisibilityUI>(InContainer)
	{
		Type = EWHCAttrUIType::EWAUT_CabDrawerDoorVisibility;
	}
	virtual TSharedPtr<SWidget> MakeWidget() override;
	void GrabLocation(int32 index){
		SelIndex = index;
	}
private:
	TArray<class FDoorGroup*> GetDrawerDoorGroups(struct FShapeInRoom *InShapeInRoom) const;
	int32 SelIndex =0;
};