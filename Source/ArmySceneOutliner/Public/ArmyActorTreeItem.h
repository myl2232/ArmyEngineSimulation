#pragma once

#include "IXRTreeItem.h"

/** A tree item that represents an actor in the world */
struct FArmyActorTreeItem : IXRTreeItem
{
public:
    /** Construct this item from an actor */
    FArmyActorTreeItem(AActor* InActor);

    /** Get the ID that represents this tree item. Used to reference this item in a map */
    virtual FArmyTreeItemID GetID() const override;

    /** Get this item's parent item. It is valid to return nullptr if this item has no parent */
    virtual FArmyTreeItemPtr FindParent(const FArmyTreeItemMap& ExistingItems) const override;

    /** Create this item's parent. It is valid to return nullptr if this item has no parent */
    virtual FArmyTreeItemPtr CreateParent() const override;

    /** Visit this tree item */
    virtual void Visit(const IXRTreeItemVisitor& Visitor) const override;
    virtual void Visit(const IXRMutableTreeItemVisitor& Visitor) override;

public:
    /** The actor this tree item is associated with. */
    mutable TWeakObjectPtr<AActor> Actor;

    /** Constant identifier for this tree item */
    const FObjectKey ID;
};