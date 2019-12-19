
#include "ArmyHydropowerSwitchConnect.h"
#include "SWidgetSwitcher.h"
#include "STreeView.h"
#include "IArmyTreeItem.h"
#include "ArmyObject.h"
#include "ArmyFolderTreeItem.h"
#include "SArmyImageTextButton.h"
#include "SArmyTextCheckBox.h"
#include "ArmyViewportClient.h"


FArmyHydropowerSwitchConnect::FArmyHydropowerSwitchConnect():CurrentItem(nullptr)
{
}

void SArmySwitchOutlinerTreeRow::Construct(const FArguments& InArgs,const TSharedRef<SArmySwitchConnectTreeView>& OutlinerTreeView)
{
	Item = InArgs._Item;
	//郭子阳 2018.1.14
	//添加单击响应事件
	OnClicked = InArgs._OnClicked;

	Item->RowUI = this;

	STableRow<SArmySpaceTreeItemPtr>::Construct(
		STableRow<SArmySpaceTreeItemPtr>::FArguments()
        .Style(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.GrayWithFocused"))
		
		.Content()
		[
            SNew(SBox)
            .HeightOverride(20)
            [
                SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 0, 0)
				[
					SNew(SExpanderArrow, SharedThis(this)).IndentAmount(12)
				]
				+SHorizontalBox::Slot()
				.Padding(6,0,0,0)
				.VAlign(VAlign_Center)
				.FillWidth(1.0f)
                [
                    SNew(STextBlock)
                    .Text(FText::FromName(Item->SpaceName))
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
                ]
            ]
		],OutlinerTreeView);
}

FReply SArmySwitchOutlinerTreeRow::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FReply Reply = STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (Reply.IsEventHandled() && OnClicked.IsBound())
	{
		OnClicked.Execute();
	}
	return Reply;
};

TSharedRef<SWidget> SArmySwitchOutlinerTreeRow::GenerateWidgetForColumn(const FName & ColumnName)
{
	// Create the widget for this item
	TSharedRef<SWidget> NewItemWidget = SNullWidget::NullWidget;
	return NewItemWidget;
}

void SArmySwitchItemOutlinerTreeRow::Construct(const FArguments& InArgs,const TSharedRef<SArmySwitchConnectTreeView>& OutlinerTreeView)
{
	Item = InArgs._Item;
	Item->RowUI = this;
	OnClicked = InArgs._OnClicked;

	STableRow<SArmySpaceTreeItemPtr>::Construct(
		STableRow<SArmySpaceTreeItemPtr>::FArguments()
        .Style(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Normal"))
		.Content()
		[
            SNew(SBox)
            .HeightOverride(20)
            [
                SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 0, 0)
				[
					SNew(SExpanderArrow, SharedThis(this)).IndentAmount(12)
				]
                + SHorizontalBox::Slot()
                .Padding(10, 0, 0, 0)
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(FText::FromName(Item->SpaceName))
                    .TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
                    .ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))
                ]
				+ SHorizontalBox::Slot()
                .Padding(0,0,10,0)
                .VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
                [
					SNew(SArmyTextCheckBox)
					.CheckBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("CheckBox.AddBox"))
					.NormalText(FText::FromString(TEXT("添加")))
					.CheckText(FText::FromString(TEXT("确认")))
					.OnCheckStateChanged(InArgs._OnCheckStateChanged)
					.IsChecked(InArgs._IsChecked)
                ]
            ]
		],OutlinerTreeView);
}

TSharedRef<SWidget> SArmySwitchItemOutlinerTreeRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	return SNullWidget::NullWidget;
}

FReply SArmySwitchItemOutlinerTreeRow::OnMouseButtonDown(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	FReply Reply = STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (Reply.IsEventHandled() && OnClicked.IsBound())
	{
		OnClicked.Execute();
	}
	return Reply;
}

void SArmyLightItemOutlinerTreeRow::Construct(const FArguments& InArgs,const TSharedRef<SArmySwitchConnectTreeView>& OutlinerTreeView)
{
	Item = InArgs._Item;
	OnItemClicked = InArgs._OnItemClicked;
	Item->RowUI = this;



	STableRow<SArmySpaceTreeItemPtr>::Construct(
		STableRow<SArmySpaceTreeItemPtr>::FArguments()
        .Style(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.GrayWithFocused"))
		.Content()
		[
            SNew(SBox)
            .HeightOverride(20)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .Padding(10, 0, 20, 0)
                .AutoWidth()
				.MaxWidth(130)
                .VAlign(VAlign_Center)
                [
					SNew(STextBlock)
					.Text(FText::FromName(Item->SpaceName))
					.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
					.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF9D9FA5"))	
                ]
				+ SHorizontalBox::Slot()
                .Padding(0,0,10,0)
                .VAlign(VAlign_Center)
				.HAlign(HAlign_Right)
                [
					SNew(SBox)
					.WidthOverride(20)
					.HeightOverride(20)
					[
						SNew(SButton)
						.ButtonStyle(FArmyStyle::Get(), "Button.Del")
						.ButtonColorAndOpacity(FLinearColor(FColor(0XFFFFFFFF)))
						.HAlign(EHorizontalAlignment::HAlign_Center)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.OnClicked(InArgs._OnClicked)
					]
                ]
            ]
		],OutlinerTreeView);

}


FReply SArmyLightItemOutlinerTreeRow::OnMouseButtonDown(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	FReply Reply = STableRow::OnMouseButtonDown(MyGeometry, MouseEvent);
	if (Reply.IsEventHandled()&& OnItemClicked.IsBound())
	{
		OnItemClicked.Execute();
	}
	return Reply;
}

TSharedRef<SWidget> SArmyLightItemOutlinerTreeRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> FArmyHydropowerSwitchConnect::CreateDetailWidget()
{
	TSharedRef<SHeaderRow> HeaderRowWidget = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed);

	//SetupColumns(*HeaderRowWidget);

	return 
		SNew(SOverlay)
		+SOverlay::Slot()
		[
			SAssignNew(OutlinerTreeView,SArmySwitchConnectTreeView)
			.SelectionMode(ESelectionMode::Multi)
			.TreeItemsSource(&RootTreeItems)
			.OnGetChildren(this,&FArmyHydropowerSwitchConnect::OnGetChildrenForOutlinerTree)
		// Generates the actual widget for a tree item
		.OnGenerateRow(this,&FArmyHydropowerSwitchConnect::OnGenerateRowForOutlinerTree)
		// Header for the tree
		.HeaderRow(HeaderRowWidget)
		];
}

void FArmyHydropowerSwitchConnect::ClearRootTree()
{
	RootTreeItems.Empty();
	Refresh();
}

void FArmyHydropowerSwitchConnect::AddItemToTree(SArmySpaceTreeItemPtr Item)
{
	RootTreeItems.AddUnique(Item);
}

void FArmyHydropowerSwitchConnect::Refresh()
{
	CurrentItem = nullptr;
	OutlinerTreeView->RequestTreeRefresh();
}

TSharedRef<ITableRow> FArmyHydropowerSwitchConnect::OnGenerateRowForOutlinerTree(SArmySpaceTreeItemPtr Item,const TSharedRef<STableViewBase>& OwnerTable)
{
	switch (Item->itemType)
	{
	case SArmySpaceTreeItem::IT_SPACE:
	case SArmySpaceTreeItem::IT_SWITCH:
		return SNew(SArmySwitchOutlinerTreeRow,OutlinerTreeView.ToSharedRef())
			.OnClicked(this, &FArmyHydropowerSwitchConnect::OnSwitchClicked,Item)
			.Item(Item);
	case SArmySpaceTreeItem::IT_LIAN:
		return SNew(SArmySwitchItemOutlinerTreeRow,OutlinerTreeView.ToSharedRef())
			.Item(Item)
			.OnClicked(this, &FArmyHydropowerSwitchConnect::OnSwitchClicked, Item)
			.OnCheckStateChanged(this,&FArmyHydropowerSwitchConnect::OnCheckStateChanged,Item)
			.IsChecked(this,&FArmyHydropowerSwitchConnect::IsChecked,Item);
	case SArmySpaceTreeItem::IT_LIGHT:
		return SNew(SArmyLightItemOutlinerTreeRow, OutlinerTreeView.ToSharedRef())
			.Item(Item)
			.OnItemClicked(this, &FArmyHydropowerSwitchConnect::OnSwitchClicked, Item)
			.OnClicked(this,&FArmyHydropowerSwitchConnect::DeleteItem,Item);
	default:
		return SNew(SArmySwitchOutlinerTreeRow,OutlinerTreeView.ToSharedRef())
			.Item(Item);
	}
		
}

void FArmyHydropowerSwitchConnect::OnGetChildrenForOutlinerTree(SArmySpaceTreeItemPtr InParent,TArray<SArmySpaceTreeItemPtr>& OutChildren)
{
	for (auto& WeakChild:InParent->GetChildren())
	{
		auto Child = WeakChild;
		// Should never have bogus entries in this list
		check(Child.IsValid());
		OutChildren.Add(Child);
	}
}

void FArmyHydropowerSwitchConnect::OnCheckStateChanged(ECheckBoxState InCheck,SArmySpaceTreeItemPtr InCurrentItem)
{
	if (InCheck == ECheckBoxState::Checked&&!CurrentItem.IsValid())
	{
		CurrentItem = InCurrentItem;
		GVC->SetAutoMultiSelectEnabled(true);
	}
	else if(InCheck != ECheckBoxState::Checked&&CurrentItem.IsValid())
	{
		CurrentItem = nullptr;
		GVC->SetAutoMultiSelectEnabled(false);
	}
	CheckStateChanged.ExecuteIfBound(InCheck,InCurrentItem);
}

ECheckBoxState FArmyHydropowerSwitchConnect::IsChecked(SArmySpaceTreeItemPtr InCurrentItem)const
{
	if (CurrentItem == InCurrentItem)
		return ECheckBoxState::Checked;
	return ECheckBoxState::Unchecked;
}

FReply FArmyHydropowerSwitchConnect::DeleteItem(SArmySpaceTreeItemPtr Item)
{
	if (Item.IsValid())
	{
		OnDelete.ExecuteIfBound(Item);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

//在UI中单击开关响应时间
//添加单击响应事件

 FReply FArmyHydropowerSwitchConnect::OnSwitchClicked(SArmySpaceTreeItemPtr Item) {
	 if (OnItemClicked.IsBound())
	 {
		 OnItemClicked.Execute(Item);
		 return FReply::Handled();
	 }

		return FReply::Unhandled();
}

 void FArmyHydropowerSwitchConnect::HighLightCorrespondingUI(AActor * actor)
 {
	 SArmySpaceTreeItemPtr CorrespondingItemPtr;
	 if (FindCorrespondingItemPtr(RootTreeItems[0], actor, CorrespondingItemPtr))
	 {
		 if (CorrespondingItemPtr->itemType == SArmySpaceTreeItem::ItemType::IT_SWITCH)
		 {
			 auto parent = CorrespondingItemPtr->Parent.Pin();
			 OutlinerTreeView->ClearSelection();
			 OutlinerTreeView->SetItemExpansion(parent, true);
			 OutlinerTreeView->Private_SetItemSelection(CorrespondingItemPtr, true, true);
		 }

		 if (CorrespondingItemPtr->itemType == SArmySpaceTreeItem::ItemType::IT_LIGHT)
		 {
			 auto parent = CorrespondingItemPtr->Parent.Pin();
			 OutlinerTreeView->ClearSelection();
			 OutlinerTreeView->SetItemExpansion(parent->Parent.Pin()->Parent.Pin(), true);
			 OutlinerTreeView->SetItemExpansion(parent->Parent.Pin(), true);
			 OutlinerTreeView->SetItemExpansion(parent, true);
			 OutlinerTreeView->Private_SetItemSelection(CorrespondingItemPtr, true, true);
		 }
	}
 }

 bool FArmyHydropowerSwitchConnect::FindCorrespondingItemPtr(SArmySpaceTreeItemPtr& InItem, AActor * Inactor, SArmySpaceTreeItemPtr& out)
 {
	 if (InItem->Actor == Inactor)
	 {
		 out = InItem;
		 return true;
	 }
	 else
	 {
		for(auto Child: InItem->Children)
		{
			if (FindCorrespondingItemPtr(Child, Inactor, out))
			{
				return true;
			}
		}
	 }
	return false;
 }




