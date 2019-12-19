#pragma once

#include "ArmyStyle.h"
#include "Commands.h"

class FArmyWHCModeCommands : public TCommands<FArmyWHCModeCommands>
{
public:
    FArmyWHCModeCommands()
        : TCommands<FArmyWHCModeCommands>(
            TEXT("WHCMode"),
            NSLOCTEXT("Contexts", "WHCMode", "Whole House Customization Mode"),
            NAME_None,
            FArmyStyle::Get().GetStyleSetName()
            )
    {}
    virtual void RegisterCommands() override;

    TSharedPtr<FUICommandInfo> SaveCommand;
    TSharedPtr<FUICommandInfo> DeleteCommand;

    TSharedPtr<FUICommandInfo> HideCommand;
    TSharedPtr<FUICommandInfo> HideCabinetDoorCommand;
    TSharedPtr<FUICommandInfo> HideOnGroundCabCommand;
    TSharedPtr<FUICommandInfo> HideHangingCabCommand;
    TSharedPtr<FUICommandInfo> HideFurnitureCommand;
    TSharedPtr<FUICommandInfo> HideHydropowerCommand;

    TSharedPtr<FUICommandInfo> StyleCommand;

    TSharedPtr<FUICommandInfo> RulerCommand;
    TSharedPtr<FUICommandInfo> CabinetRulerCommand;
    TSharedPtr<FUICommandInfo> EnvRulerCommand;

    TSharedPtr<FUICommandInfo> GeneratingCommand;
    TSharedPtr<FUICommandInfo> PlatformGeneratingCommand;
    TSharedPtr<FUICommandInfo> ToeGeneratingCommand;
    TSharedPtr<FUICommandInfo> TopBlockerGeneratingCommand;
    TSharedPtr<FUICommandInfo> BottomBlockerGeneratingCommand;

    TSharedPtr<FUICommandInfo> ClearSceneCommand;
    TSharedPtr<FUICommandInfo> ClearCabinetCommand;
    TSharedPtr<FUICommandInfo> ClearWardrobeCommand;
    TSharedPtr<FUICommandInfo> ClearOtherCommand;
    TSharedPtr<FUICommandInfo> ClearAllCommand;

    TSharedPtr<FUICommandInfo> TopdownViewCommand;

    TSharedPtr<FUICommandInfo> RegularViewCommand;
};