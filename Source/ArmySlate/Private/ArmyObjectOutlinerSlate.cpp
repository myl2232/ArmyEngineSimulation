#include "ArmyObjectOutlinerSlate.h"
#include "ArmyStyle.h"
#include "SInlineEditableTextBlock.h"
#include "MultiBoxBuilder.h"

void SArmyObjectOutlinerTreeRow::Construct(const FArguments& InArgs, const TSharedRef<SArmyObjectTreeView> OutlinerTreeView)
{
	Item = InArgs._Item;
	Item->RowUI = this;
	ObjOutliner = InArgs._Outliner;

	STableRow<SArmyObjectTreeItemPtr>::Construct(
		STableRow<SArmyObjectTreeItemPtr>::FArguments()
        .Style(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Gray"))
		.Content()
		[
            SNew(SBox)
            .HeightOverride(20)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .Padding(10, 0, 0, 0)
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromName(Item->ModuleName))
                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
                    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
                ]

                + SHorizontalBox::Slot()
                .Padding(0, 0, 10, 0)
                .FillWidth(1)
                .HAlign(HAlign_Right)
                [
                    SAssignNew(CheckBox, SCheckBox)
                    .Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"))
                    .OnCheckStateChanged(this, &SArmyObjectOutlinerTreeRow::OnCheckedChange)
                    .IsChecked(Item->Visibility ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
                ]
            ]
		]

	, OutlinerTreeView);
}
void SArmyObjectOutlinerTreeRow::SetCheckState(ECheckBoxState NewCheckedState)
{
	if (CheckBox.IsValid() && CheckBox->GetCheckedState() != NewCheckedState)
	{
		CheckBox->ToggleCheckedState();
	}
}
void SArmyObjectOutlinerTreeRow::OnCheckedChange(ECheckBoxState NewCheckedState)
{
	const TArray<TSharedPtr<SArmyObjectTreeItem>>& Children = Item->GetChildren();

	if (Children.Num() > 0 && IsItemExpanded())
	{
		for (auto ChildIt : Children)
		{
			if (ChildIt->RowUI)
			{
				if (SArmyObjectOutlinerTreeRow* ChildUI = (SArmyObjectOutlinerTreeRow*)(ChildIt->RowUI))
				{
					ChildUI->SetCheckState(NewCheckedState);
				}
			}
		}
	}
	bool V = NewCheckedState == ECheckBoxState::Checked;
	Item->SetVisible(V);
	if(ObjOutliner)ObjOutliner->TreeEventTrigger.ExecuteIfBound(Item,V);
}
void SelectItem(SArmyObjectTreeItemPtr InItem)
{

}
//////////////////////////////////////////////////////////////////////////
void SArmyLayerOutlinerTreeRow::Construct(const FArguments& InArgs, const TSharedRef<SArmyObjectTreeView> OutlinerTreeView)
{
	Item = InArgs._Item;
	Item->RowUI = this;
	ObjOutliner = InArgs._Outliner;

	TSharedPtr<SInlineEditableTextBlock> DisplayLabel;
	STableRow<SArmyObjectTreeItemPtr>::Construct(
		STableRow<SArmyObjectTreeItemPtr>::FArguments()
		.Style(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Layers"))
		.Content()
		[
			SNew(SBox)
			.HeightOverride(32)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(10,0,0,0))
				.HAlign(HAlign_Left)
				[
					//SNew(STextBlock)
					//.Text(FText::FromName(Item->ModuleName))
					SAssignNew(DisplayLabel,SInlineEditableTextBlock)
					.IsReadOnly(true)
					.Font(FSlateFontInfo("PingFangSC-Regular", 12))
					.Style(FArmyStyle::Get(),"InlineEditableTextBlockStyle")
					.Text(this, &SArmyLayerOutlinerTreeRow::GetDisplayText)
					.ColorAndOpacity(FLinearColor::White)
					.OnTextCommitted(this, &SArmyLayerOutlinerTreeRow::OnLabelCommitted)
				
			//.OnVerifyTextChanged(this, &SArmyLayerOutlinerTreeRow::OnVerifyItemLabelChanged)
					.IsSelected(FIsSelected::CreateSP(this, &STableRow<SArmyObjectTreeItemPtr>::IsSelectedExclusively))
				]
			]
		]

	, OutlinerTreeView);

	if (Item->EditState)
	{
		DisplayLabel->EnterEditingMode();
	}
}
FText SArmyLayerOutlinerTreeRow::GetDisplayText() const
{
	return FText::FromName(Item->ModuleName);
}
void SArmyLayerOutlinerTreeRow::OnLabelCommitted(const FText& InLabel, ETextCommit::Type InCommitInfo)
{
	Item->EditState = false;
	if (ObjOutliner)ObjOutliner->TreeRowModify.ExecuteIfBound(Item, InLabel);
}
//FReply SArmyLayerOutlinerTreeRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
//{
//	//if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
//	//{
//	//	if (ObjOutliner)ObjOutliner->TreeEventTrigger.ExecuteIfBound(Item, true);
//	//}
//	return STableRow< SArmyObjectTreeItemPtr>::OnMouseButtonDown(MyGeometry,MouseEvent);
//}
//////////////////////////////////////////////////////////////////////////
void SArmyObjectOutliner::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SAssignNew(OutlinerTreeView, SArmyObjectTreeView)
			.SelectionMode(ESelectionMode::Single)
			// Point the tree to our array of root-level items.  Whenever this changes, we'll call RequestTreeRefresh()
			.TreeItemsSource(&RootTreeItems)
			// Called to child items for any given parent item
			.OnGetChildren(this, &SArmyObjectOutliner::OnGetChildrenForOutlinerTree)
			// Generates the actual widget for a tree item
			.OnGenerateRow(this, &SArmyObjectOutliner::OnGenerateRowForOutlinerTree)
			.OnSelectionChanged(this, &SArmyObjectOutliner::OnSelectionChanged)
			// Use the level viewport context menu as the right click menu for tree items
			//.OnContextMenuOpening(this, &SArmyObjectOutliner::OnOpenContextMenu)
		];
}
TSharedPtr<SWidget> SArmyObjectOutliner::OnOpenContextMenu()
{
	return PopMenu.Execute(1);
}
void SArmyObjectOutliner::OnSelectionChanged(SArmyObjectTreeItemPtr SelectedItem, ESelectInfo::Type SelectInfo)
{
	TreeEventTrigger.ExecuteIfBound(SelectedItem, true);
}
SArmyObjectOutliner::~SArmyObjectOutliner()
{

}
void SArmyObjectOutliner::Refresh()
{
	OutlinerTreeView->RequestTreeRefresh();
}
void SArmyObjectOutliner::EmptyTreeItems()
{
	//FilteredActorCount = 0;
	//ApplicableActors.Empty();

	//PendingOperations.Empty();
	//PendingTreeItemMap.Empty();

	RootTreeItems.Empty();
	Refresh();
}

void SArmyObjectOutliner::RemoveItemFromTree(const FGuid& InItemID)
{
	//RootTreeItems.Remove()
	SArmyObjectTreeItemPtr RemoveItem;
	for (auto Item : RootTreeItems)
	{
		if (Item->ItemID == InItemID)
		{
			RemoveItem = Item;
			break;
		}
	}
	if (RemoveItem.IsValid())
	{
		RootTreeItems.Remove(RemoveItem);
		Refresh();
	}
	//SArmyObjectTreeItemPtr ItemPtr = TreeItemMap.FindRef(InItemName);
	//if (ItemPtr.IsValid())
	//{
	//	auto Parent = ItemPtr->GetParent();

	//	if (Parent.IsValid())
	//	{
	//		Parent->RemoveChild(ItemPtr);
	//		//OnChildRemovedFromParent(*Parent);
	//	}
	//	else
	//	{
	//		RootTreeItems.Remove(ItemPtr);
	//	}

	//	TreeItemMap.Remove(InItemName);
	//	Refresh();
	//}
}
void SArmyObjectOutliner::SelectItem(SArmyObjectTreeItemPtr InItem)
{
	OutlinerTreeView->SelectItem(InItem);
}

void SArmyObjectOutliner::SelectItem(FName InItemName)
{
	for (auto& It : RootTreeItems)
	{
		if (It->ModuleName == InItemName)
		{
			SelectItem(It);
		}
	}
}

bool SArmyObjectOutliner::AddItemToTree(SArmyObjectTreeItemPtr Item)
{
	SArmyObjectTreeItemPtr Parent = Item->GetParent();
	if (Parent.IsValid())
	{
		Parent->AddChild(Item);
	}
	else
	{
		RootTreeItems.Add(Item);
	}
	OutlinerTreeView->RequestTreeRefresh();
	return true;
}

TSharedRef< ITableRow > SArmyObjectOutliner::OnGenerateRowForOutlinerTree(SArmyObjectTreeItemPtr InItem, const TSharedRef< STableViewBase >& OwnerTable)
{
	switch (TreeType)
	{
	case OUTLINER_OBJLIST:
		return SNew(SArmyObjectOutlinerTreeRow, OutlinerTreeView.ToSharedRef()).Item(InItem).Outliner(this);
		break;
	case OUTLINER_LAYERLIST:
		return SNew(SArmyLayerOutlinerTreeRow, OutlinerTreeView.ToSharedRef()).Item(InItem).Outliner(this);
		break;
	default:
		return SNew(SArmyObjectOutlinerTreeRow, OutlinerTreeView.ToSharedRef()).Item(InItem).Outliner(this);
		break;
	}
}

void SArmyObjectOutliner::OnGetChildrenForOutlinerTree(SArmyObjectTreeItemPtr InParent, TArray< SArmyObjectTreeItemPtr >& OutChildren)
{
	for (auto& WeakChild : InParent->GetChildren())
	{
		auto Child = WeakChild;
		// Should never have bogus entries in this list
		check(Child.IsValid());
		OutChildren.Add(Child);
	}

	// If the item needs it's children sorting, do that now
	if (OutChildren.Num())
	{
		// Empty out the children and repopulate them in the correct order
		InParent->Children.Empty();
		for (auto& Child : OutChildren)
		{
			InParent->Children.Emplace(Child);
		}
	}
}
#undef LOCTEXT_NAMESPACE
