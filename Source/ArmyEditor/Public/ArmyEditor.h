#pragma once

ARMYEDITOR_API class FArmyEditorModeTools& GArmyLevelEditorModeTools();
#define CAMERA_ZOOM_DAMPEN			200.f

/*-----------------------------------------------------------------------------
Parameter parsing functions.
-----------------------------------------------------------------------------*/

bool GetFVECTOR(const TCHAR* Stream, const TCHAR* Match, FVector& Value);
bool GetFVECTOR(const TCHAR* Stream, FVector& Value);
const TCHAR* GetFVECTORSpaceDelimited(const TCHAR* Stream, FVector& Value);
bool GetFROTATOR(const TCHAR* Stream, const TCHAR* Match, FRotator& Rotation, int32 ScaleFactor);
bool GetFROTATOR(const TCHAR* Stream, FRotator& Rotation, int ScaleFactor);
const TCHAR* GetFROTATORSpaceDelimited(const TCHAR* Stream, FRotator& Rotation, int32 ScaleFactor);
bool GetBEGIN(const TCHAR** Stream, const TCHAR* Match);
bool GetEND(const TCHAR** Stream, const TCHAR* Match);
bool GetREMOVE(const TCHAR** Stream, const TCHAR* Match);
bool GetSUBSTRING(const TCHAR*	Stream, const TCHAR* Match, TCHAR* Value, int32 MaxLen);
TCHAR* SetFVECTOR(TCHAR* Dest, const FVector* Value);


/**
* Import the entire default properties block for the class specified
*
* @param	Class		the class to import defaults for
* @param	Text		buffer containing the text to be imported
* @param	Warn		output device for log messages
* @param	Depth		current nested subobject depth
* @param	LineNumber	the starting line number for the defaultproperties block (used for log messages)
*
* @return	NULL if the default values couldn't be imported
*/

/**
* Parameters for ImportObjectProperties
*/
struct FArmyImportObjectParams
{
	/** the location to import the property values to */
	uint8*				DestData;

	/** pointer to a buffer containing the values that should be parsed and imported */
	const TCHAR*		SourceText;

	/** the struct for the data we're importing */
	UStruct*			ObjectStruct;

	/** the original object that ImportObjectProperties was called for.
	if SubobjectOuter is a subobject, corresponds to the first object in SubobjectOuter's Outer chain that is not a subobject itself.
	if SubobjectOuter is not a subobject, should normally be the same value as SubobjectOuter */
	UObject*			SubobjectRoot;

	/** the object corresponding to DestData; this is the object that will used as the outer when creating subobjects from definitions contained in SourceText */
	UObject*			SubobjectOuter;

	/** output device to use for log messages */
	FFeedbackContext*	Warn;

	/** current nesting level */
	int32					Depth;

	/** used when importing defaults during script compilation for tracking which line we're currently for the purposes of printing compile errors */
	int32					LineNumber;

	/** contains the mappings of instanced objects and components to their templates; used when recursively calling ImportObjectProperties; generally
	not necessary to specify a value when calling this function from other code */
	FObjectInstancingGraph* InInstanceGraph;

	/** provides a mapping from an actor name to a new instance to which it should be remapped */
	const TMap<FName, AActor*>* ActorRemapper;

	/** True if we should call PreEditChange/PostEditChange on the object as it's imported.  Pass false here
	if you're going to do that on your own. */
	bool				bShouldCallEditChange;


	/** Constructor */
	FArmyImportObjectParams()
		: DestData(NULL),
		SourceText(NULL),
		ObjectStruct(NULL),
		SubobjectRoot(NULL),
		SubobjectOuter(NULL),
		Warn(NULL),
		Depth(0),
		LineNumber(INDEX_NONE),
		InInstanceGraph(NULL),
		ActorRemapper(NULL),
		bShouldCallEditChange(true)
	{
	}
};


/**
* Parse and import text as property values for the object specified.
*
* @param	InParams	Parameters for object import; see declaration of FImportObjectParams.
*
* @return	NULL if the default values couldn't be imported
*/

const TCHAR* ImportObjectProperties(FArmyImportObjectParams& InParams);
/**
* Parse and import text as property values for the object specified.
*
* @param	DestData			the location to import the property values to
* @param	SourceText			pointer to a buffer containing the values that should be parsed and imported
* @param	ObjectStruct		the struct for the data we're importing
* @param	SubobjectRoot		the original object that ImportObjectProperties was called for.
*								if SubobjectOuter is a subobject, corresponds to the first object in SubobjectOuter's Outer chain that is not a subobject itself.
*								if SubobjectOuter is not a subobject, should normally be the same value as SubobjectOuter
* @param	SubobjectOuter		the object corresponding to DestData; this is the object that will used as the outer when creating subobjects from definitions contained in SourceText
* @param	Warn				output device to use for log messages
* @param	Depth				current nesting level
* @param	LineNumber			used when importing defaults during script compilation for tracking which line the defaultproperties block begins on
* @param	InstanceGraph		contains the mappings of instanced objects and components to their templates; used when recursively calling ImportObjectProperties; generally
*								not necessary to specify a value when calling this function from other code
* @param	ActorRemapper		used when duplicating actors to remap references from a source actor to the duplicated actor
*
* @return	NULL if the default values couldn't be imported
*/
const TCHAR* ImportObjectProperties(
	uint8*				DestData,
	const TCHAR*		SourceText,
	UStruct*			ObjectStruct,
	UObject*			SubobjectRoot,
	UObject*			SubobjectOuter,
	FFeedbackContext*	Warn,
	int32					Depth,
	int32					LineNumber = INDEX_NONE,
	FObjectInstancingGraph* InstanceGraph = NULL,
	const TMap<FName, AActor*>* ActorRemapper = NULL
);