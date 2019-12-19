#include "ArmyEditorViewportCommands.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Selection.h"
#include "Modules/ModuleManager.h"
#include "Materials/MaterialInterface.h"
#include "MaterialShared.h"
#include "Engine/Texture2D.h"

#define LOCTEXT_NAMESPACE "EditorViewportCommands"

void FArmyEditorViewportCommands::RegisterCommands()
{
	UI_COMMAND(Perspective, "Perspective", "Switches the viewport to perspective view", EUserInterfaceActionType::Button, FInputChord(/*EKeys::P*/));
	UI_COMMAND(Top, "Top", "Switches the viewport to top view", EUserInterfaceActionType::Button, FInputChord(/*EKeys::T*/));
    UI_COMMAND(Orbit, "Orbit", "Switches the viewport to orbit view", EUserInterfaceActionType::Button, FInputChord(/*EKeys::V*/));
	UI_COMMAND(ToggleRealTime, "Realtime", "Toggles real time rendering in this viewport", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::R));
	UI_COMMAND(ScreenCapture, "Screen Capture", "Take a screenshot of the active viewport.", EUserInterfaceActionType::Button, FInputChord(EKeys::F9) );
	UI_COMMAND(TranslateMode, "Translate Mode", "Select and translate objects", EUserInterfaceActionType::Button, FInputChord(EKeys::One) );
	UI_COMMAND(RotateMode, "Rotate Mode", "Select and rotate objects", EUserInterfaceActionType::Button, FInputChord(EKeys::Two) );
	UI_COMMAND(ScaleMode, "Scale Mode", "Select and scale objects", EUserInterfaceActionType::Button, FInputChord(EKeys::Three) );
	UI_COMMAND(CycleTransformGizmoCoordSystem, "Cycle Transform Coordinate System", "Cycles the transform gizmo coordinate systems between world and local (object) space", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::Tilde));
	UI_COMMAND(CycleTransformGizmos, "Cycle Between Translate, Rotate, and Scale", "Cycles the transform gizmos between translate, rotate, and scale", EUserInterfaceActionType::Button, FInputChord(EKeys::SpaceBar) );
	//UI_COMMAND( FocusViewportToSelection, "Focus Selected", "Moves the camera in front of the selection", EUserInterfaceActionType::Button, FInputChord( EKeys::F ) );
}

#undef LOCTEXT_NAMESPACE