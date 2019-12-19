#pragma once

#include "CoreMinimal.h"
#include "Style/XRStyle.h"
#include "ArmySceneOutlinerVisitorTypes.h"

struct FArmyTreeItemID;

struct IXRTreeItem : public TSharedFromThis<IXRTreeItem>
{
public:
    /** Get the ID that represents this tree item. Used to reference this item in a map */
    virtual FArmyTreeItemID GetID() const = 0;

    /** Visit this tree item */
    virtual void Visit(const IXRTreeItemVisitor& Visitor) const = 0;
    virtual void Visit(const IXRMutableTreeItemVisitor& Visitor) = 0;

    /** Get some data from this tree item using a 'getter' visitor. */
    template<typename T>
    T Get(const TXRTreeItemGetter<T>& Getter) const
    {
        Visit(Getter);
        return Getter.Result();
    }

public:
    /** Get this item's parent. Can be nullptr. */
    FArmyTreeItemPtr GetParent() const
    {
        return Parent.Pin();
    }

    /** Add a child to this item */
    void AddChild(FArmyTreeItemRef Child)
    {
        Child->Parent = AsShared();
        Children.Add(MoveTemp(Child));
    }

    /** Remove a child from this item */
    void RemoveChild(const FArmyTreeItemRef& Child)
    {
        if (Children.Remove(Child))
        {
            Child->Parent = nullptr;
        }
    }

    /** Get this item's children, if any. Although we store as weak pointers, they are guaranteed to be valid. */
    FORCEINLINE const TArray< TWeakPtr<IXRTreeItem> >& GetChildren() const
    {
        return Children;
    }

    /** Find this item's parent in the specified map. It is valid to return nullptr if this item has no parent */
    virtual FArmyTreeItemPtr FindParent(const FArmyTreeItemMap& ExistingItems) const = 0;

    /** Create this item's parent. It is valid to return nullptr if this item has no parent */
    virtual FArmyTreeItemPtr CreateParent() const = 0;

protected:
    /** Default constructor */
    IXRTreeItem() : SharedData(nullptr), Parent(nullptr) {}
    virtual ~IXRTreeItem() {}

    /** This item's parent, if any. */
    mutable TWeakPtr<IXRTreeItem> Parent;

    /** Array of children contained underneath this item */
    mutable TArray< TWeakPtr<IXRTreeItem> > Children;

public:
    /** Data that is common between all outliner items - owned by the Outliner itself */
    TSharedPtr<FArmySharedOutlinerData> SharedData;

	bool CloseExpander = false;

	bool IsRootItem = false;
};