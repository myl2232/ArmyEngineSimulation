#pragma once

#include "Engine/World.h"
#include "ArmyEditorViewportClient.h"
#include "ArmySceneOutlinerFwd.h"

enum class EXRSceneOutlinerMode : uint8
{
    /** Allows all actors to be browsed and selected; syncs selection with the editor; drag and drop attachment, stc. */
    ActorBrowsing,

    /** Sets the outliner to operate as an actor 'picker'. */
    ActorPicker,
};

/** Container for built in column types. Function-static so they are available without linking */
struct FArmyBuiltInColumnTypes
{
    /** The gutter column */
    static const FName& Gutter()
    {
        static FName Gutter("Gutter");
        return Gutter;
    }

    /** The item label column */
    static const FName& Label()
    {
        static FName Label("ItemLabel");
        return Label;
    }

    /** Generic actor info column */
    static FName& ActorInfo()
    {
        static FName ActorInfo("ActorInfo");
        return ActorInfo;
    }
};

/** Visibility enum for scene outliner columns */
enum class EXRColumnVisibility : uint8
{
    /** This column defaults to being visible on the scene outliner */
    Visible,

    /** This column defaults to being invisible, yet still available on the scene outliner */
    Invisible,
};

/** Column information for the scene outliner */
struct FArmyColumnInfo
{
    FArmyColumnInfo(EXRColumnVisibility InVisibility, int32 InPriorityIndex, const FArmyCreateSceneOutlinerColumn& InFactory = FArmyCreateSceneOutlinerColumn())
        : Visibility(InVisibility)
        , PriorityIndex(InPriorityIndex)
        , Factory(InFactory)
    {}

    FArmyColumnInfo() {}

    FArmyColumnInfo(const FArmyColumnInfo& InColumnInfo)
        : Visibility(InColumnInfo.Visibility)
        , PriorityIndex(InColumnInfo.PriorityIndex)
        , Factory(InColumnInfo.Factory)
    {}

    EXRColumnVisibility Visibility;
    uint8 PriorityIndex;

    FArmyCreateSceneOutlinerColumn Factory;
};

/** Default column information for the scene outliner */
struct FArmyDefaultColumnInfo : public FArmyColumnInfo
{
    FArmyDefaultColumnInfo(const FArmyColumnInfo& InColumnInfo, TOptional<EXRSceneOutlinerMode> InValidMode = TOptional<EXRSceneOutlinerMode>())
        : ColumnInfo(InColumnInfo)
        , ValidMode(InValidMode)
    {}

    FArmyColumnInfo ColumnInfo;

    // The valid mode for this column. If not set, this column will be valid for all.
    TOptional<EXRSceneOutlinerMode> ValidMode;
};

struct FArmySharedDataBase
{
    /** Mode to operate in */
    EXRSceneOutlinerMode Mode;

    /** Map of column types available to the scene outliner, along with default ordering */
    TMap<FName, FArmyColumnInfo> ColumnMap;

public:
    /** Constructor */
    FArmySharedDataBase()
        : Mode(EXRSceneOutlinerMode::ActorBrowsing)
    {}

    /** Set up a default array of columns for this outliner */
    void UseDefaultColumns();
};

/**
 * Settings for the Scene Outliner set by the programmer before spawning an instance of the widget.  This
 * is used to modify the outliner's behavior in various ways, such as filtering in or out specific classes
 * of actors.
 */
struct FArmyInitializationOptions : FArmySharedDataBase
{
    /** The world that we are representing */
    UWorld* RepresentingWorld;
	UArmyEditorViewportClient* RepresentingClient;

    FArmyInitializationOptions()
		: RepresentingWorld(nullptr),
		RepresentingClient(nullptr)
    {}
};

/** Outliner data that is shared between a scene outliner and its items */
struct FArmySharedOutlinerData : FArmySharedDataBase, TSharedFromThis<FArmySharedOutlinerData>
{
    /** The world that we are representing */
    UWorld* RepresentingWorld;
	UArmyEditorViewportClient* RepresentingClient;

    FArmySharedOutlinerData()
        : RepresentingWorld(nullptr),
		RepresentingClient(nullptr)
    {}
};