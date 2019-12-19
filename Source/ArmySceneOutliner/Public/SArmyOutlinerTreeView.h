#pragma once

#include "Widgets/SCompoundWidget.h"
#include "STreeView.h"
#include "IXRTreeItem.h"
#include "ArmySceneOutlinerFwd.h"

class SXRSceneOutliner;

class SXROutlinerTreeView : public STreeView<FArmyTreeItemPtr>
{
public:
    void Construct(const FArguments& InArgs);
};

/** Widget that represents a row in the outliner's tree control.  Generates widgets for each column on demand. */
class SXRSceneOutlinerTreeRow : public SMultiColumnTableRow<FArmyTreeItemPtr>
{
public:
    SLATE_BEGIN_ARGS(SXRSceneOutlinerTreeRow) {}

        /** The list item for this row */
        SLATE_ARGUMENT(FArmyTreeItemPtr, Item)

    SLATE_END_ARGS()

    /** Construct function for this widget */
	void Construct(const FArguments& InArgs, const TSharedRef<SXROutlinerTreeView>& OutlinerTreeView, TSharedRef<SXRSceneOutliner> SceneOutliner);

    /** Overridden from SMultiColumnTableRow.  Generates a widget for this column of the tree row. */
    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	EVisibility GetArrowVisibility() const;

    /** Weak reference to the outliner widget that owns our list */
    TWeakPtr<SXRSceneOutliner> SceneOutlinerWeak;

    /** The item associated with this row of data */
    TWeakPtr<IXRTreeItem> Item;
};