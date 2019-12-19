#pragma once

struct FArmyActorFolderProps
{
    FArmyActorFolderProps() : bIsExpanded(true) {}

    /** Serializer */
    FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FArmyActorFolderProps& Folder)
    {
        return Ar << Folder.bIsExpanded;
    }

    bool bIsExpanded;
};

class FArmyEditorActorFolders
{
public:
    TMap<FName, FArmyActorFolderProps> Folders;
};

/** Class responsible for managing an in-memory representation of actor folders in the editor */
struct ARMYEDITOR_API FArmyActorFolders
{
public:
    /** Singleton access - only valid if IsAvailable() */
    static FArmyActorFolders& Get();

    /** Create and update a folder container for the specified world */
    TSharedPtr<FArmyEditorActorFolders> InitializeForWorld(UWorld& InWorld);

    /** Get or create a folder container for the specified world */
    TSharedPtr<FArmyEditorActorFolders> GetOrCreateFoldersForWorld(UWorld& InWorld);

    /** Get a map of folder properties for the specified world (map of folder path -> properties) */
    const TMap<FName, FArmyActorFolderProps>& GetFolderPropertiesForWorld(UWorld& InWorld);

    /** Get a default folder name under the specified parent path */
    FName GetDefaultFolderName(UWorld& InWorld, FName ParentPath = FName());

    /** Get a new default folder name that would apply to the current selection */
    FName GetDefaultFolderNameForSelection(UWorld& InWorld);

    /** Remove any references to folder arrays for dead worlds */
    void Housekeeping();

    /** Add a folder to the folder map for the specified world. Does not trigger any events. */
    bool AddFolderToWorld(UWorld& InWorld, FName Path);

    /** Create a new folder in the specified world, of the specified path */
    void CreateFolder(UWorld& InWorld, FName Path);

    /** Same as CreateFolder, but moves the current actor selection into the new folder as well */
    void CreateFolderContainingSelection(UWorld& InWorld, FName Path);

private:
    /** Singleton instance maintained by the editor */
    static TSharedPtr<FArmyActorFolders> Singleton;

    /** Transient map of folders, keyed on world pointer */
    TMap< TWeakObjectPtr<UWorld>, TSharedPtr<FArmyEditorActorFolders> > TemporaryWorldFolders;
};