#include "ArmyEditorActorFolders.h"
#include "ArmyEditorEngine.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"

TSharedPtr<FArmyActorFolders> FArmyActorFolders::Singleton = nullptr;

FArmyActorFolders& FArmyActorFolders::Get()
{
    if (!Singleton.IsValid())
    {
        Singleton = MakeShareable(new FArmyActorFolders());
    }
    return *Singleton;
}

TSharedPtr<FArmyEditorActorFolders> FArmyActorFolders::InitializeForWorld(UWorld& InWorld)
{
    // Clean up any stale worlds
    Housekeeping();

    TSharedPtr<FArmyEditorActorFolders> Folders = MakeShareable(new FArmyEditorActorFolders());
    TemporaryWorldFolders.Add(&InWorld, Folders);

    // Ensure the list is entirely up to date with the world before we write our serialized properties into it.
    for (FActorIterator ActorIt(&InWorld); ActorIt; ++ActorIt)
    {
        AddFolderToWorld(InWorld, ActorIt->GetFolderPath());
    }

    return Folders;
}

TSharedPtr<FArmyEditorActorFolders> FArmyActorFolders::GetOrCreateFoldersForWorld(UWorld& InWorld)
{
    if (TSharedPtr<FArmyEditorActorFolders>* Folders = TemporaryWorldFolders.Find(&InWorld))
    {
        return *Folders;
    }

    return InitializeForWorld(InWorld);
}

const TMap<FName, FArmyActorFolderProps>& FArmyActorFolders::GetFolderPropertiesForWorld(UWorld& InWorld)
{
    return GetOrCreateFoldersForWorld(InWorld)->Folders;
}

FName FArmyActorFolders::GetDefaultFolderName(UWorld& InWorld, FName ParentPath /*= FName()*/)
{
    // This is potentially very slow but necessary to find a unique name
    const auto& ExistingFolders = GetFolderPropertiesForWorld(InWorld);

    // Create a valid base name for this folder
    uint32 Suffix = 1;
    FText LeafName = FText::FromString(FString::Printf(TEXT("NewFolder%d"), Suffix++));

    FString ParentFolderPath = ParentPath.IsNone() ? TEXT("") : ParentPath.ToString();
    if (!ParentFolderPath.IsEmpty())
    {
        ParentFolderPath += "/";
    }

    FName FolderName(*(ParentFolderPath + LeafName.ToString()));
    while (ExistingFolders.Contains(FolderName))
    {
        LeafName = FText::FromString(FString::Printf(TEXT("NewFolder%d"), Suffix++));
        FolderName = FName(*(ParentFolderPath + LeafName.ToString()));
        if (Suffix == 0)
        {
            // We've wrapped around a 32bit unsigned int - something must be seriously wrong!
            return FName();
        }
    }

    return FolderName;
}

FName FArmyActorFolders::GetDefaultFolderNameForSelection(UWorld& InWorld)
{
    // Find a common parent folder, or put it at the root
    FName CommonParentFolder;
    for (FSelectionIterator SelectionIt(*GArmyEditor->GetSelectedActors()); SelectionIt; ++SelectionIt)
    {
        AActor* Actor = CastChecked<AActor>(*SelectionIt);
        if (CommonParentFolder.IsNone())
        {
            CommonParentFolder = Actor->GetFolderPath();
        }
        else if (Actor->GetFolderPath() != CommonParentFolder)
        {
            CommonParentFolder = NAME_None;
            break;
        }
    }

    return GetDefaultFolderName(InWorld, CommonParentFolder);
}

void FArmyActorFolders::Housekeeping()
{
    for (auto It = TemporaryWorldFolders.CreateIterator(); It; ++It)
    {
        if (!It.Key().Get())
        {
            It.RemoveCurrent();
        }
    }
}

bool FArmyActorFolders::AddFolderToWorld(UWorld& InWorld, FName Path)
{
    if (!Path.IsNone())
    {
        TSharedPtr<FArmyEditorActorFolders> Folders = GetOrCreateFoldersForWorld(InWorld);

        if (!Folders->Folders.Contains(Path))
        {
            // Add the parent as well
            const FName ParentPath(*FPaths::GetPath(Path.ToString()));
            if (!ParentPath.IsNone())
            {
                AddFolderToWorld(InWorld, ParentPath);
            }

            Folders->Folders.Add(Path);

            return true;
        }
    }

    return false;
}

void FArmyActorFolders::CreateFolder(UWorld& InWorld, FName Path)
{
    AddFolderToWorld(InWorld, Path);

    //if (AddFolderToWorld(InWorld, Path))
    //{
    //    OnFolderCreate.Broadcast(InWorld, Path);
    //}
    //else
    //{
    //    Transaction.Cancel();
    //}
}

void FArmyActorFolders::CreateFolderContainingSelection(UWorld& InWorld, FName Path)
{
    CreateFolder(InWorld, Path);
    //SetSelectedFolderPath(Path);
}
