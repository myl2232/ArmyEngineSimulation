/**
* Copyright 2018 北京伯睿科技有限公司.
* All Rights Reserved.
* 
*
* @File XRSceneOutlinerSelectableGutter.h
* @Description 物体列表锁定
*
* @Author 梁晓菲
* @Date 2018-10-9
* @Version 1.0
*/

#pragma once

#include "CoreMinimal.h"
#include "IXRSceneOutlinerColumn.h"
#include "ArmySceneOutlinerPublicTypes.h"
#include "ArmySceneOutlinerVisitorTypes.h"
#include "IXRTreeItem.h"

/** A 'getter' visitor that gets, and caches the visibility of a tree item */
struct FArmyGetSelectableVisitor : TXRTreeItemGetter<bool>
{
    /** Map of tree item to visibility */
    mutable TMap<const IXRTreeItem*, bool> SelectableInfo;

    /** Get an item's visibility based on its children */
    bool RecurseChildren(const IXRTreeItem& Item) const;

    /** Get an actor's visibility */
    virtual bool Get(const FArmyActorTreeItem& ActorItem) const override;

    /** Get a World's visibility */
    virtual bool Get(const FArmyWorldTreeItem& WorldItem) const override;

    /** Get a folder's visibility */
    virtual bool Get(const FArmyFolderTreeItem& FolderItem) const override;
};
struct FArmySetSelectableVisitor : IXRMutableTreeItemVisitor
{
	/** Whether this item should be visible or not */
	const bool bSelectable;

	FArmySetSelectableVisitor(bool bInSelectable = true) : bSelectable(bInSelectable){}

	virtual void Visit(FArmyActorTreeItem& ActorItem) const override;
	virtual void Visit(FArmyWorldTreeItem& WorldItem) const override;
	virtual void Visit(FArmyFolderTreeItem& FolderItem) const override;
};
/**
 * A gutter for the SceneOutliner which is capable of displaying a variety of Actor details
 */
class FArmySceneOutlinerSelectableGutter : public IXRSceneOutlinerColumn
{
public:
	FArmySceneOutlinerSelectableGutter(IXRSceneOutliner& Outliner);
    virtual ~FArmySceneOutlinerSelectableGutter() {}

    static FName GetID() { return FArmyBuiltInColumnTypes::ActorInfo(); }

    virtual FName GetColumnID() override;

    virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;

    virtual const TSharedRef<SWidget> ConstructRowWidget(FArmyTreeItemRef TreeItem, const STableRow<FArmyTreeItemPtr>& Row) override;

    virtual void Tick(double InCurrentTime, float InDeltaTime) override;

    /** Check whether the specified item is visible */
    FORCEINLINE bool IsItemSelectable(const IXRTreeItem& Item)
    {
        return Item.Get(SelectableCache);
    }

private:
    /** Weak pointer back to the scene outliner - required for setting visibility on current selection. */
    TWeakPtr<IXRSceneOutliner> WeakOutliner;

    /** Visitor used to get (and cache) visibilty for items. Cahced per-frame to avoid expensive recursion. */
    FArmyGetSelectableVisitor SelectableCache;
};