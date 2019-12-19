#include "ArmyWorldTreeItem.h"
#include "ArmySceneOutlinerStandaloneTypes.h"

FArmyWorldTreeItem::FArmyWorldTreeItem(UWorld* InWorld)
    : World(InWorld)
    , ID(InWorld)
{
	CloseExpander = true;
	IsRootItem = true;
}

FArmyTreeItemID FArmyWorldTreeItem::GetID() const
{
    return ID;
}

FArmyTreeItemPtr FArmyWorldTreeItem::FindParent(const FArmyTreeItemMap& ExistingItems) const
{
    return nullptr;
}

FArmyTreeItemPtr FArmyWorldTreeItem::CreateParent() const
{
    return nullptr;
}

void FArmyWorldTreeItem::Visit(const IXRTreeItemVisitor& Visitor) const
{
    Visitor.Visit(*this);
}

void FArmyWorldTreeItem::Visit(const IXRMutableTreeItemVisitor& Visitor)
{
    Visitor.Visit(*this);
}