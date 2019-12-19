#include "UIMixin/SArmyWHCDoorVisibilityUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "../common/XRShapeUtilities.h"
#include "ArmyStyle.h"
#include "SCTCabinetShape.h"
#include "SCTDoorGroup.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

TSharedPtr<SWidget> FArmyWHCDoorVisibilityUI::MakeWidget()
{
    return SNew(SCheckBox)
			.OnCheckStateChanged_Lambda(
                [this](ECheckBoxState InNewState) {
                    if (Container->IsWHCItemSelected<AXRShapeFrame>())
                    {
                        AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                        check(ShapeFrame->ShapeInRoomRef != nullptr);
                        bool bVis = InNewState == ECheckBoxState::Checked;
                        TArray<FDoorGroup*> DoorGroups = GetDoorGroups(ShapeFrame->ShapeInRoomRef);
                        for (auto &DoorGroup : DoorGroups)
                            DoorGroup->HiddenDoorGroupActors(bVis);
                    }
                }
            )
			.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
			.IsChecked_Lambda(
                [this]() -> ECheckBoxState {
                    if (Container->IsWHCItemSelected<AXRShapeFrame>())
                    {
                        AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                        check(ShapeFrame->ShapeInRoomRef != nullptr);
                        TArray<FDoorGroup*> DoorGroups = GetDoorGroups(ShapeFrame->ShapeInRoomRef);
                        if (DoorGroups.Num() > 0)
                        {
                            return DoorGroups[0]->GetDoorGoupIsHIdden() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;  
                        }
                        else
                            return ECheckBoxState::Unchecked;    
                    }
                    else
                        return ECheckBoxState::Unchecked;
                }
            );
}

TArray<FDoorGroup*> FArmyWHCDoorVisibilityUI::GetDoorGroups(FShapeInRoom *InShapeInRoom) const
{
    FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());

    TArray<FDoorGroup*> DoorGroups;
    XRShapeUtilities::GetCabinetDoors(CabShape, DoorGroups);
    return MoveTemp(DoorGroups);
}


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

TSharedPtr<SWidget> FArmyWHCabDrawerDoorVisibilityUI::MakeWidget()
{
	return SNew(SCheckBox)
		.OnCheckStateChanged_Lambda(
			[this](ECheckBoxState InNewState) {
		if (Container->IsWHCItemSelected<AXRShapeFrame>())
		{
			AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
			check(ShapeFrame->ShapeInRoomRef != nullptr);
			bool bVis = InNewState == ECheckBoxState::Checked;
			TArray<FDoorGroup*> DoorGroups = GetDrawerDoorGroups(ShapeFrame->ShapeInRoomRef);
			if (SelIndex > 0 && DoorGroups.Num() >= SelIndex)
			{
				FDoorGroup* pDrawerDoorGroup = DoorGroups[SelIndex - 1];
				if (pDrawerDoorGroup) {
					//FDrawerDoorShape* CurDrawerDoor = StaticCast<FDrawerDoorShape*>(pDrawerDoorGroup);
					pDrawerDoorGroup->HiddenDoorGroupActors(bVis);
				}
			}
		}
	}
		)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
		.IsChecked_Lambda(
			[this]() -> ECheckBoxState {
		if (Container->IsWHCItemSelected<AXRShapeFrame>())
		{
			AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
			check(ShapeFrame->ShapeInRoomRef != nullptr);
			TArray<FDoorGroup*> DoorGroups = GetDrawerDoorGroups(ShapeFrame->ShapeInRoomRef);
			if (DoorGroups.Num() > 0)
			{
				FDoorGroup* pDrawerDoorGroup = DoorGroups[SelIndex - 1];
				//FDrawerDoorShape* CurDrawerDoor = StaticCast<FDrawerDoorShape*>(pDrawerDoorGroup);
				return pDrawerDoorGroup->GetDoorGoupIsHIdden() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			}
			else
				return ECheckBoxState::Unchecked;
		}
		else
			return ECheckBoxState::Unchecked;
	}
	);
}

TArray<FDoorGroup*> FArmyWHCabDrawerDoorVisibilityUI::GetDrawerDoorGroups(FShapeInRoom *InShapeInRoom) const
{
	FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());

	TArray<FDoorGroup*> DoorGroups;
	XRShapeUtilities::GetCabinetDrawerDoors(CabShape, DoorGroups);
	return MoveTemp(DoorGroups);
}