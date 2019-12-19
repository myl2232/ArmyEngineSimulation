#include "ArmyFolderTreeItem.h"
#include "ArmySceneOutlinerStandaloneTypes.h"
#include "ArmySceneOutlinerPublicTypes.h"
#include "ArmyWorldTreeItem.h"

FArmyFolderTreeItem::FArmyFolderTreeItem(FName InPath)
    : Path(InPath)
{
    FString PathString = InPath.ToString();
    int32 LeafIndex = 0;
    if (PathString.FindLastChar('/', LeafIndex))
    {
        LeafName = FName(*PathString.RightChop(LeafIndex + 1));
    }
    else
    {
        LeafName = InPath;
    }
}

FArmyTreeItemID FArmyFolderTreeItem::GetID() const
{
    return FArmyTreeItemID(Path);
}

FArmyTreeItemPtr FArmyFolderTreeItem::FindParent(const FArmyTreeItemMap& ExistingItems) const
{
    const FName ParentPath = GetParentPath(Path);
    if (!ParentPath.IsNone())
    {
        return ExistingItems.FindRef(ParentPath);
    }

    if (SharedData->RepresentingWorld)
    {
        return ExistingItems.FindRef(SharedData->RepresentingWorld);
    }

    return nullptr;
}

FArmyTreeItemPtr FArmyFolderTreeItem::CreateParent() const
{
    const FName ParentPath = GetParentPath(Path);
    if (!ParentPath.IsNone())
    {
        return MakeShareable(new FArmyFolderTreeItem(ParentPath));
    }

    if (SharedData->RepresentingWorld)
    {
        return MakeShareable(new FArmyWorldTreeItem(SharedData->RepresentingWorld));
    }

    return nullptr;
}

void FArmyFolderTreeItem::Visit(const IXRTreeItemVisitor& Visitor) const
{
    Visitor.Visit(*this);
}

void FArmyFolderTreeItem::Visit(const IXRMutableTreeItemVisitor& Visitor)
{
    Visitor.Visit(*this);
}
