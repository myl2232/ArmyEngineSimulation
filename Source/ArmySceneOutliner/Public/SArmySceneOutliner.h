#pragma once

#include "Widgets/SCompoundWidget.h"
#include "SXROutlinerTreeView.h"
#include "Private/XRSceneOutlinerStandaloneTypes.h"
#include "ArmySceneOutlinerPublicTypes.h"
#include "IXRSceneOutliner.h"

class IXRSceneOutlinerColumn;

/** Structure that defines an operation that should be applied to the tree */
struct FArmyPendingTreeOperation
{
    enum EType { Added, Removed, Moved };
    FArmyPendingTreeOperation(EType InType, FArmyTreeItemRef InItem) : Type(InType), Item(InItem) { }

    /** The type of operation that is to be applied */
    EType Type;

    /** The tree item to which this operation relates */
    FArmyTreeItemRef Item;
};

/** Set of actions to apply to new tree items */
namespace EXRNewItemAction
{
    enum Type
    {
        /** Select the item when it is created */
        Select = 1 << 0,
        /** Scroll the item into view when it is created */
        ScrollIntoView = 1 << 1,
        /** Interactively rename the item when it is created (implies the above) */
        Rename = 1 << 2,
    };
}

class SXRSceneOutliner : public IXRSceneOutliner
{
public:
    SLATE_BEGIN_ARGS(SXRSceneOutliner) {}
    SLATE_END_ARGS()
    
    /**
	 * Construct this widget.  Called by the SNew() Slate macro.
	 *
	 * @param	InArgs		Declaration used by the SNew() macro to construct this widget
	 * @param	InitOptions	Programmer-driven initialization options for this widget
	 */
    void Construct(const FArguments& InArgs, const FArmyInitializationOptions& InInitOptions);

    /** Default constructor - initializes data that is shared between all tree items */
    SXRSceneOutliner() : SharedData(MakeShareable(new FArmySharedOutlinerData)) {}

    /** SSceneOutliner destructor */
    ~SXRSceneOutliner();

    /** SWidget interface */
    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

    /** Sends a requests to the Scene Outliner to refresh itself the next chance it gets */
    virtual void Refresh(bool bForce = false) override;

	virtual void CloseExpander(const FName& InName, bool InClose, bool InVisible = false, bool InSelectable = true) override;

    /** Get a const reference to the actual tree hierarchy */
    virtual const STreeView<FArmyTreeItemPtr>& GetTree() const override
    {
        return *OutlinerTreeView;
    }

public:
    /** Instruct the outliner to perform an action on the specified item when it is created */
    void OnItemAdded(const FArmyTreeItemID& ItemID, uint8 ActionMask);

    /** Get the columns to be displayed in this outliner */
    const TMap<FName, TSharedPtr<IXRSceneOutlinerColumn>>& GetColumns() const
    {
        return Columns;
    }

private:
    /** Methods that implement structural modification logic for the tree */

    /** Empty all the tree item containers maintained by this outliner */
    void EmptyTreeItems();

    /** Apply incremental changes to, or a complete repopulation of the tree  */
    void Populate();

    /** Repopulates the entire tree */
    void RepopulateEntireTree();

    /** Attempts to add an item to the tree. Will add any parents if required. */
    bool AddItemToTree(FArmyTreeItemRef InItem);

    /** Add an item to the tree, even if it doesn't match the filter terms. Used to add parent's that would otherwise be filtered out */
    void AddUnfilteredItemToTree(FArmyTreeItemRef Item);

    /** Ensure that the specified item's parent is added to the tree, if applicable */
    FArmyTreeItemPtr EnsureParentForItem(FArmyTreeItemRef Item);

    /** Remove the specified item from the tree */
    void RemoveItemFromTree(FArmyTreeItemRef InItem);

    /** Called when a child has been removed from the specified parent. Will potentially remove the parent from the tree */
    void OnChildRemovedFromParent(IXRTreeItem& Parent);

    /** Called when a child has been moved in the tree hierarchy */
    void OnItemMoved(const FArmyTreeItemRef& Item);

    /** Adds a new item for the specified type and refreshes the tree, provided it matches the filter terms */
    template<typename TreeItemType, typename DataType>
    void ConstructItemFor(const DataType& Data)
    {
        // We test the filters with a temporary so we don't allocate on the heap unnecessarily
        const TreeItemType Temporary(Data);
        //if (Filters->PassesAllFilters(Temporary) && SearchBoxFilter->PassesFilter(Temporary))
        //{
            FArmyTreeItemRef NewItem = MakeShareable(new TreeItemType(Data));
            PendingOperations.Emplace(FArmyPendingTreeOperation::Added, NewItem);
            PendingTreeItemMap.Add(NewItem->GetID(), NewItem);
            Refresh();
        //}
    }

    /** Create a new folder under the specified parent name (NAME_None for root) */
    void CreateFolder(const FName& NewFolderName);

    /** Check that we are reflecting a valid world */
    bool CheckWorld() const { return SharedData->RepresentingWorld != nullptr; }

    /** Checks to see if the actor is valid for displaying in the outliner */
    bool IsActorDisplayable(const AActor* Actor) const;

    /** Tree view event bindings */

    /** Called by STreeView to generate a table row for the specified item */
    TSharedRef<ITableRow> OnGenerateRowForOutlinerTree(FArmyTreeItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable);

    /** Called by STreeView to get child items for the specified parent item */
    void OnGetChildrenForOutlinerTree(FArmyTreeItemPtr InParent, TArray<FArmyTreeItemPtr>& OutChildren);

    /** Called by STreeView when the tree's selection has changed */
    void OnOutlinerTreeSelectionChanged(FArmyTreeItemPtr TreeItem, ESelectInfo::Type SelectInfo);

	void OnOutlinerTreeDoubleClick(FArmyTreeItemPtr InItem);
private:
    /** Level, editor and other global event hooks required to keep the outliner up to date */

    /** Called by USelection::SelectionChangedEvent delegate when the level's selection changes */
    void OnLevelSelectionChanged(UObject* Obj);

    /** Called by the engine when an actor is added to the world. */
    void OnLevelActorsAdded(AActor* InActor);
public:
	/*@郭子阳 直接控制actor在物体列表中的显隐*/
	void ShowOrHideLevelActor(AActor* InActor,bool NewHiden) override
	{
		if (InActor)
		{
			if (NewHiden)
			{
				OnLevelActorsRemoved(InActor);
			}
			else
			{
				OnLevelActorsAdded(InActor);
			}
		}
	}
private:
    /** Called by the engine when an actor is remove from the world. */
    void OnLevelActorsRemoved(AActor* InActor);

    /** Called by the engine when an actor's folder is changed */
    void OnLevelActorFolderChanged(const AActor* InActor, FName OldPath);

	/**	视图类型改变回调函数*/
	void OnViewTypeChanged(int32 InViewType);
private:
    /** Shared data required by the tree and its items */
    TSharedRef<FArmySharedOutlinerData> SharedData;

    /** List of pending operations to be applied to the tree */
    TArray<FArmyPendingTreeOperation> PendingOperations;

    /** Map of actions to apply to new tree items */
    TMap<FArmyTreeItemID, uint8> NewItemActions;

    /** Our tree view */
    TSharedPtr<SXROutlinerTreeView> OutlinerTreeView;

    /** A map of all items we have in the tree */
    FArmyTreeItemMap TreeItemMap;

    /** Pending tree items that are yet to be added the tree */
    FArmyTreeItemMap PendingTreeItemMap;

    /** Root level tree items */
    TArray<FArmyTreeItemPtr> RootTreeItems;

private:
    /** Map of columns that are shown on this outliner. */
    TMap<FName, TSharedPtr<IXRSceneOutlinerColumn>> Columns;

    /** Set up the columns required for this outliner */
    void SetupColumns(SHeaderRow& HeaderRow);

public:
    /** Miscellaneous helper functions */

    /** Scroll the specified item into view */
    void ScrollItemIntoView(FArmyTreeItemPtr Item);

    /** True if the outliner needs to be repopulated at the next appropriate opportunity, usually because our
    actor set has changed in some way. */
    bool bNeedsRefresh;

    /** true if the Scene Outliner should do a full refresh. */
    bool bFullRefresh;

    /** Reentrancy guard */
    bool bIsReentrant;

private:
    /** Handler for recursively expanding/collapsing items */
    void SetItemExpansionRecursive(FArmyTreeItemPtr Model, bool bInExpansionState);
};