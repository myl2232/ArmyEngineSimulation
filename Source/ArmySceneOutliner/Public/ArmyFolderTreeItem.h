#pragma once

#include "IXRTreeItem.h"

/** A tree item that represents a folder in the world */
struct FArmyFolderTreeItem : IXRTreeItem
{
public:
    /** Constructor that takes a path to this folder (including leaf-name) */
    FArmyFolderTreeItem(FName InPath);

    /** Get the ID that represents this tree item. Used to reference this item in a map */
    virtual FArmyTreeItemID GetID() const override;

    /** Get this item's parent item. It is valid to return nullptr if this item has no parent */
    virtual FArmyTreeItemPtr FindParent(const FArmyTreeItemMap& ExistingItems) const override;

    /** Create this item's parent. It is valid to return nullptr if this item has no parent */
    FArmyTreeItemPtr CreateParent() const override;

    /** Visit this tree item */
    virtual void Visit(const IXRTreeItemVisitor& Visitor) const override;
    virtual void Visit(const IXRMutableTreeItemVisitor& Visitor) override;

public:
    /** The path of this folder. / separated. */
    FName Path;

    /** The leaf name of this folder */
    FName LeafName;
};