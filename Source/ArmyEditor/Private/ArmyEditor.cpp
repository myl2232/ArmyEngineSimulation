#include "ArmyEditor.h"
#include "ArmyEditorModeManager.h"

class FArmyEditorModeTools& GArmyLevelEditorModeTools()
{
	static FArmyEditorModeTools* EditorModeToolsSingleton = new FArmyEditorModeTools;
	return *EditorModeToolsSingleton;
}