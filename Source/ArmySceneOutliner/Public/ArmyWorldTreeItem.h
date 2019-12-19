#pragma once

#include "IXRTreeItem.h"

/** A tree item that represents an entire world */
struct FArmyWorldTreeItem : IXRTreeItem
{
public:
    /** Construct this item from a world */
    FArmyWorldTreeItem(UWorld* InWorld);

    /** Get the ID that represents this tree item. Used to reference this item in a map */
    virtual FArmyTreeItemID GetID() const override;

    /** Get this item's parent item. Always returns nullptr. */
    virtual FArmyTreeItemPtr FindParent(const FArmyTreeItemMap& ExistingItems) const override;

    /** Create this item's parent. Always returns nullptr. */
    virtual FArmyTreeItemPtr CreateParent() const override;

    /** Visit this tree item */
    virtual void Visit(const IXRTreeItemVisitor& Visitor) const override;
    virtual void Visit(const IXRMutableTreeItemVisitor& Visitor) override;

public:
    /** The world this tree item is associated with. */
    mutable TWeakObjectPtr<UWorld> World;

    /** Constant identifier for this tree item */
    const FObjectKey ID;
};