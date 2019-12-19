#include "ArmyActorTreeItem.h"
#include "ArmySceneOutlinerStandaloneTypes.h"
#include "ArmyFolderTreeItem.h"
#include "ArmyWorldTreeItem.h"

FArmyActorTreeItem::FArmyActorTreeItem(AActor* InActor)
    : Actor(InActor)
    , ID(InActor)
{
}

FArmyTreeItemID FArmyActorTreeItem::GetID() const
{
    return ID;
}

FArmyTreeItemPtr FArmyActorTreeItem::FindParent(const FArmyTreeItemMap& ExistingItems) const
{
    AActor* ActorPtr = Actor.Get();
    if (!ActorPtr)
    {
        return nullptr;
    }

    // Parents should have already been added to the tree
    AActor* ParentActor = ActorPtr->GetAttachParentActor();
    if (ParentActor)
    {
        return ExistingItems.FindRef(ParentActor);
    }
    else
    {
        //const bool bShouldShowFolders = SharedData->Mode == EXRSceneOutlinerMode::ActorBrowsing || SharedData->bOnlyShowFolders;

        const FName ActorFolder = ActorPtr->GetFolderPath();
        if (/*bShouldShowFolders && */!ActorFolder.IsNone())
        {
            return ExistingItems.FindRef(ActorFolder);
        }
    }

    if (UWorld* World = ActorPtr->GetWorld())
    {
        return ExistingItems.FindRef(World);
    }

    return nullptr;
}

FArmyTreeItemPtr FArmyActorTreeItem::CreateParent() const
{
    AActor* ActorPtr = Actor.Get();
    if (!ActorPtr)
    {
        return nullptr;
    }

    AActor* ParentActor = ActorPtr->GetAttachParentActor();
    if (ParentActor && ensureMsgf(ParentActor != ActorPtr, TEXT("Encountered an Actor attached to itself (%s)"), *ParentActor->GetName()))
    {
        return MakeShareable(new FArmyActorTreeItem(ParentActor));
    }
    else if (!ParentActor)
    {
        //const bool bShouldShowFolders = SharedData->Mode == ESceneOutlinerMode::ActorBrowsing || SharedData->bOnlyShowFolders;

        const FName ActorFolder = ActorPtr->GetFolderPath();
        if (/*bShouldShowFolders && */!ActorFolder.IsNone())
        {
            return MakeShareable(new FArmyFolderTreeItem(ActorFolder));
        }

        if (UWorld* World = ActorPtr->GetWorld())
        {
            return MakeShareable(new FArmyWorldTreeItem(World));
        }
    }

    return nullptr;
}

void FArmyActorTreeItem::Visit(const IXRTreeItemVisitor& Visitor) const
{
    Visitor.Visit(*this);
}

void FArmyActorTreeItem::Visit(const IXRMutableTreeItemVisitor& Visitor)
{
    Visitor.Visit(*this);
}
