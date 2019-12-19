#pragma once 

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Widgets/SWidget.h"
#include "Framework/Commands/Commands.h"
#include "Engine/TextureStreamingTypes.h"

class FUICommandList;

/**
 * Class containing commands for editor viewport actions common to all viewports
 */
class ARMYEDITOR_API FArmyEditorViewportCommands : public TCommands<FArmyEditorViewportCommands>
{
public:
	FArmyEditorViewportCommands() 
		: TCommands<FArmyEditorViewportCommands>
		(
			TEXT("ArmyEditorViewport"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "EditorViewportCommands", "Common Viewport Commands"), // Localized context name for displaying
			NAME_None,
			FName("EditorStyle") // Icon Style Set
		)
	{
	}
	
	TSharedPtr<FUICommandInfo> Perspective;
    TSharedPtr<FUICommandInfo> Top;
    TSharedPtr<FUICommandInfo> Orbit;
    TSharedPtr<FUICommandInfo> Left;
    TSharedPtr<FUICommandInfo> Right;
    TSharedPtr<FUICommandInfo> Front;
    TSharedPtr<FUICommandInfo> Back;
    TSharedPtr<FUICommandInfo> ToggleRealTime;
    TSharedPtr<FUICommandInfo> ScreenCapture;
    TSharedPtr<FUICommandInfo> TranslateMode;
    TSharedPtr<FUICommandInfo> RotateMode;
    TSharedPtr<FUICommandInfo> ScaleMode;
    TSharedPtr<FUICommandInfo> CycleTransformGizmos;
    TSharedPtr<FUICommandInfo> CycleTransformGizmoCoordSystem;
    TSharedPtr<FUICommandInfo> FocusViewportToSelection;

public:
	/** Registers our commands with the binding system */
	virtual void RegisterCommands() override;
};
