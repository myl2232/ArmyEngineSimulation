#include "UIMixin/SArmyWHCSlidingDoorWardrobeBBoardUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "WHCMode/XRWHCCabinetChangeList.h"
#include "ArmyStyle.h"
#include "Actor/XRActorConstant.h"
#include "SCTShape.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

TSharedPtr<SWidget> FArmyWHCSlidingDoorWardrobeBBoardUI::MakeWidget()
{
    return SNew(SCheckBox)
			.OnCheckStateChanged_Lambda(
                [this](ECheckBoxState InNewState) {
                    if (Container->IsWHCItemSelected<AXRShapeFrame>())
                    {
                        AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
                        check(ShapeFrame->ShapeInRoomRef != nullptr);
                        bool bVis = InNewState == ECheckBoxState::Checked;
                        FSCTShapeUtilityTool::SetCabinetSlidingDoorDownPlateBoardValid(ShapeFrame->ShapeInRoomRef->Shape.Get(), bVis);

                        FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange * BBoardVisChange = nullptr;
                        for (const auto & Change : ShapeFrame->ShapeInRoomRef->ChangeList)
                        {
                            if (Change->IsSameType(SHAPE_CHANGE_SWARDROBE_BOTTOM_BOARD_VIS))
                            {
                                BBoardVisChange = static_cast<FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange*>(Change.Get());
                                break;
                            }
                        }
                        if (BBoardVisChange == nullptr)
                        {
                            BBoardVisChange = new FArmyWHCSlidingDoorWardrobeBBoardVisibilityChange(ShapeFrame->ShapeInRoomRef->Shape.Get());
                            ShapeFrame->ShapeInRoomRef->ChangeList.Emplace(MakeShareable(BBoardVisChange));
                        }
                        BBoardVisChange->SetVisibility(bVis);
                        ShapeFrame->ShapeInRoomRef->MakeImmovable();
                        FSCTShapeUtilityTool::SetActorTag(ShapeFrame, XRActorTag::WHCActor);
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
                        bool bVisible = FSCTShapeUtilityTool::GetCabinetSlidingDoorDownPlateBoardIsValid(ShapeFrame->ShapeInRoomRef->Shape.Get());
                        return bVisible ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                    }
                    else
                        return ECheckBoxState::Unchecked;
                }
            );
}