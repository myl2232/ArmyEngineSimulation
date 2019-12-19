#pragma once

#include "ArmySceneOutlinerFwd.h"

/** A const tree item visitor. Derive to implement type-specific behaviour for tree items. */
struct IXRTreeItemVisitor
{
    virtual ~IXRTreeItemVisitor() {}
    virtual void Visit(const FArmyActorTreeItem& Actor) const {}
    virtual void Visit(const FArmyWorldTreeItem& World) const {}
    virtual void Visit(const FArmyFolderTreeItem& Folder) const {}
};

/** A non-const tree item visitor. Derive to implement type-specific behaviour for tree items. */
struct IXRMutableTreeItemVisitor
{
    virtual ~IXRMutableTreeItemVisitor() {}
    virtual void Visit(FArmyActorTreeItem& Actor) const {}
    virtual void Visit(FArmyWorldTreeItem& World) const {}
    virtual void Visit(FArmyFolderTreeItem& Folder) const {}
};

/** A visitor specialized for getting/extracting a value from a tree item. */
template<typename TDataType>
struct TXRTreeItemGetter : IXRTreeItemVisitor
{
    mutable TDataType Data;

    /** Override to extract the data from specific tree item types */
    virtual TDataType Get(const FArmyActorTreeItem& ActorItem) const { return TDataType(); }
    virtual TDataType Get(const FArmyWorldTreeItem& WorldItem) const { return TDataType(); }
    virtual TDataType Get(const FArmyFolderTreeItem& FolderItem) const { return TDataType(); }

    /** Return the result returned from Get() */
    FORCEINLINE const TDataType& Result() const { return Data; }

private:
    virtual void Visit(const FArmyActorTreeItem& ActorItem) const override { Data = Get(ActorItem); }
    virtual void Visit(const FArmyWorldTreeItem& WorldItem) const override { Data = Get(WorldItem); }
    virtual void Visit(const FArmyFolderTreeItem& FolderItem) const override { Data = Get(FolderItem); }
};

/** A visitor class used to generate column cells for specific tree item types */
struct FArmyColumnGenerator : IXRMutableTreeItemVisitor
{
    mutable TSharedPtr<SWidget> Widget;

    virtual TSharedRef<SWidget> GenerateWidget(FArmyActorTreeItem& Item) const { return SNullWidget::NullWidget; }
    virtual TSharedRef<SWidget> GenerateWidget(FArmyWorldTreeItem& Item) const { return SNullWidget::NullWidget; }
    virtual TSharedRef<SWidget> GenerateWidget(FArmyFolderTreeItem& Item) const { return SNullWidget::NullWidget; }

private:
    virtual void Visit(FArmyActorTreeItem& Item) const override { Widget = GenerateWidget(Item); }
    virtual void Visit(FArmyWorldTreeItem& Item) const override { Widget = GenerateWidget(Item); }
    virtual void Visit(FArmyFolderTreeItem& Item) const override { Widget = GenerateWidget(Item); }
};