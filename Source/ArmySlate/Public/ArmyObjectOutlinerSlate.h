// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "ArmyObjectTreeItemSlate.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

//#define LOCTEXT_NAMESPACE "SArmyObjectOutliner"
DECLARE_DELEGATE_TwoParams(OutlinerTreeEvent, const SArmyObjectTreeItemPtr,bool);
DECLARE_DELEGATE_TwoParams(OutlinerTreeRowModify, const SArmyObjectTreeItemPtr, const FText&);
DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SWidget>, OutlinerPopMenu, int32);

class SArmyObjectOutliner;

enum OutlinerType
{
	OUTLINER_OBJLIST,
	OUTLINER_LAYERLIST
};

class SArmyObjectTreeView : public STreeView<SArmyObjectTreeItemPtr>
{
public:
	void SelectItem(SArmyObjectTreeItemPtr Item,bool InEdit = false)
	{
		Private_ClearSelection();
		SetItemSelection(Item, true);
	}
};
/** Widget that represents a row in the outliner's tree control.  Generates widgets for each column on demand. */
class SArmyObjectOutlinerTreeRow
	: public STableRow< SArmyObjectTreeItemPtr >
{

public:

	SLATE_BEGIN_ARGS(SArmyObjectOutlinerTreeRow) {}

	/** The list item for this row */
	SLATE_ARGUMENT(SArmyObjectTreeItemPtr, Item)

	SLATE_ARGUMENT(SArmyObjectOutliner*, Outliner)

		SLATE_END_ARGS()


	/** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<SArmyObjectTreeView> OutlinerTreeView);
	void SetCheckState(ECheckBoxState NewCheckedState);
	void OnCheckedChange(ECheckBoxState NewCheckedState);

private:
	TSharedPtr<SCheckBox> CheckBox;
	/** The item associated with this row of data */
	SArmyObjectTreeItemPtr Item;

	SArmyObjectOutliner* ObjOutliner;
};

class SArmyLayerOutlinerTreeRow
	: public STableRow< SArmyObjectTreeItemPtr >
{

public:
	SLATE_BEGIN_ARGS(SArmyLayerOutlinerTreeRow) {}

	/** The list item for this row */
	SLATE_ARGUMENT(SArmyObjectTreeItemPtr, Item)

		SLATE_ARGUMENT(SArmyObjectOutliner*, Outliner)

		SLATE_END_ARGS()


		/** Construct function for this widget */
		void Construct(const FArguments& InArgs, const TSharedRef<SArmyObjectTreeView> OutlinerTreeView);

		FText GetDisplayText() const;

		void OnLabelCommitted(const FText& InLabel, ETextCommit::Type InCommitInfo);

		//virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
private:
	/** The item associated with this row of data */
	SArmyObjectTreeItemPtr Item;

	SArmyObjectOutliner* ObjOutliner;
};

class ARMYSLATE_API SArmyObjectOutliner : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SArmyObjectOutliner) {}

	SLATE_END_ARGS()


	void Construct(const FArguments& InArgs);

	/** Default constructor - initializes data that is shared between all tree items */
	SArmyObjectOutliner() {}

	/** SArmyObjectOutliner destructor */
	~SArmyObjectOutliner();

	/** Sends a requests to the Scene Outliner to refresh itself the next chance it gets */
	void Refresh();

	/** Get a const reference to the actual tree hierarchy */
	const STreeView<SArmyObjectTreeItemPtr>& GetTree() const
	{
		return *OutlinerTreeView;
	}

	/** Called by STreeView to generate a table row for the specified item */
	TSharedRef< ITableRow > OnGenerateRowForOutlinerTree(SArmyObjectTreeItemPtr InItem, const TSharedRef< STableViewBase >& OwnerTable);
	/**	ÓÒ¼ü²Ëµ¥*/
	TSharedPtr<SWidget> OnOpenContextMenu();
	void OnSelectionChanged(SArmyObjectTreeItemPtr SelectedItem, ESelectInfo::Type SelectInfo);
	/** Called by STreeView to get child items for the specified parent item */
	void OnGetChildrenForOutlinerTree(SArmyObjectTreeItemPtr InParent, TArray< SArmyObjectTreeItemPtr >& OutChildren);

	/** Empty all the tree item containers maintained by this outliner */
	void EmptyTreeItems();

	/** Attempts to add an item to the tree. Will add any parents if required. */
	bool AddItemToTree(SArmyObjectTreeItemPtr InItem);

	/** Remove the specified item from the tree */
	void RemoveItemFromTree(const FGuid& InItemID);

	void SelectItem(SArmyObjectTreeItemPtr InItem);

	void SelectItem(FName InItemName);

	OutlinerTreeEvent TreeEventTrigger;

	OutlinerTreeRowModify TreeRowModify;

	OutlinerPopMenu PopMenu;
	/** Root level tree items */
	TArray<SArmyObjectTreeItemPtr> RootTreeItems;

	TSharedPtr<SArmyObjectTreeView> OutlinerTreeView;

	OutlinerType TreeType = OUTLINER_OBJLIST;
};