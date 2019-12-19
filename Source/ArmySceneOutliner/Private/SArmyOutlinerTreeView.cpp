#include "SXROutlinerTreeView.h"
#include "SXRSceneOutliner.h"
#include "IXRSceneOutlinerColumn.h"

void SXROutlinerTreeView::Construct(const FArguments& InArgs)
{
    STreeView::Construct(InArgs);
}

void SXRSceneOutlinerTreeRow::Construct(const FArguments& InArgs, const TSharedRef<SXROutlinerTreeView>& OutlinerTreeView, TSharedRef<SXRSceneOutliner> SceneOutliner)
{
    Item = InArgs._Item->AsShared();
    SceneOutlinerWeak = SceneOutliner;

    auto Args = FSuperRowType::FArguments();
    Args._Style = &FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Gray");

    SMultiColumnTableRow<FArmyTreeItemPtr>::Construct(Args, OutlinerTreeView);
}

EVisibility SXRSceneOutlinerTreeRow::GetArrowVisibility() const
{
	return (Item.IsValid() && Item.Pin()->CloseExpander) ? EVisibility::Collapsed : EVisibility::Visible;
}

TSharedRef<SWidget> SXRSceneOutlinerTreeRow::GenerateWidgetForColumn(const FName& ColumnName)
{
    auto ItemPtr = Item.Pin();
    if (!ItemPtr.IsValid())
    {
        return SNullWidget::NullWidget;
    }
	if (ItemPtr->IsRootItem)
	{
		return SNullWidget::NullWidget;
	}
    // Create the widget for this item
    TSharedRef<SWidget> NewItemWidget = SNullWidget::NullWidget;

    auto Outliner = SceneOutlinerWeak.Pin();
    check(Outliner.IsValid());

    auto Column = Outliner->GetColumns().FindRef(ColumnName);
    if (Column.IsValid())
    {
        NewItemWidget = Column->ConstructRowWidget(ItemPtr.ToSharedRef(), *this);
    }

    if(ColumnName == FArmyBuiltInColumnTypes::Label())
	{
		// The first column gets the tree expansion arrow for this row
		return
            SNew(SBox)
            .HeightOverride(24)
            [
                SNew(SHorizontalBox)

			    + SHorizontalBox::Slot()
			    .AutoWidth()
			    [
				    SNew(SExpanderArrow, SharedThis(this))
                    .StyleSet(&FArmyStyle::Get())
                    .IndentAmount(4)
                    .Visibility(this,&SXRSceneOutlinerTreeRow::GetArrowVisibility)
			    ]

			    + SHorizontalBox::Slot()
			    .Padding(4, 0, 0, 0)
			    .FillWidth(1.0f)
			    [
				    NewItemWidget
			    ]
            ];
	}
	else
	{
		// Other columns just get widget content -- no expansion arrow needed
		return 
            SNew(SBox)
            .HeightOverride(24)
            [
                NewItemWidget
            ];
	}
}
