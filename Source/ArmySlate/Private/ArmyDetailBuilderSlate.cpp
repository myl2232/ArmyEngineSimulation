#include "ArmyDetailBuilderSlate.h"
#include "STableRow.h"
#include "ArmyStyle.h"

FArmyDetailNode& FArmyDetailBuilder::AddCategory(FName _CategoryName, FString _DisplayCategoryName, TSharedPtr<SWidget> _NodeWidget/* = nullptr*/, bool bUseBorder/* = false*/)
{
	TSharedRef<FArmyDetailNode> NewNode = MakeShareable(new FArmyDetailNode(_CategoryName, _DisplayCategoryName, _NodeWidget));
	NewNode->bUseSplitter = true;
	/** @欧石楠 标记是否使用醒目的颜色框*/
	NewNode->bUseBackgroundBorder = false;
    NewNode->bIsCategory = true;
	NewNode->bUseBorder = bUseBorder;
	ChildrenNodes.Add(NewNode->NodeName, NewNode);

	return *NewNode;
}

TSharedPtr<FArmyDetailNode> FArmyDetailBuilder::GetCategory(FName _CategoryName)
{
	return EditCategory(_CategoryName);
}

void FArmyDetailBuilder::SortCatergory(TArray<FName> Order)
{
	//@欧石楠
	//排序
	ChildrenNodes.KeySort([&](FName Name1, FName Name2) 
	{
		int32 Index1 = Order.Find(Name1);
		int32 Index2 = Order.Find(Name2);

		if (Index1 != INDEX_NONE &&Index1 != INDEX_NONE)
		{
			return Index1 < Index2;
		}

		if (Index1 != INDEX_NONE)
		{
			return !true;
		}
		if (Index2 != INDEX_NONE)
		{
			return !false;
		}
		return Name1.ToString().Len() < Name2.ToString().Len();
	}
	);
}

void FArmyDetailBuilder::RemoveCategory(FName _CategoryName)
{
	ChildrenNodes.Remove(_CategoryName);
}

TSharedPtr<FArmyDetailNode> FArmyDetailBuilder::EditCategory(FName _CategoryName)
{
	TSharedPtr<FArmyDetailNode>* result = ChildrenNodes.Find(_CategoryName);
	if (result)
		return StaticCastSharedPtr<FArmyDetailNode>((*result));

	return NULL;
}

TSharedPtr<SWidget> FArmyDetailBuilder::BuildDetail()
{
	TreeSource.Reset();

	for (auto& It : ChildrenNodes)
	{
		TreeSource.Add(It.Value);
	}

	ActualLeftSize = 0.4f;


	SAssignNew(TreeView, STreeView< TSharedPtr<FArmyDetailNode>>)
		.TreeItemsSource(&TreeSource)
		.ItemHeight(24)
		.OnGenerateRow(this, &FArmyDetailBuilder::OnGenerateRowForTree)
		.OnGetChildren(this, &FArmyDetailBuilder::GetChildrenForTree)
		.SelectionMode(ESelectionMode::None);

	for (auto& It : TreeSource)
	{
		TreeView->SetItemExpansion(It, true);
	}

	return 
        SNew(SBox)
        .Padding(FMargin(8, 0, 0, 0))
	    [
		    TreeView.ToSharedRef()
	    ];
}

void FArmyDetailBuilder::RefreshTree()
{
	TreeSource.Reset();
	
	for (auto& It : ChildrenNodes)
	{				
		TreeSource.Add(It.Value);		
	}
	TreeView->RequestTreeRefresh();
}

void FArmyDetailBuilder::UpdateListTest()
{
	TreeSource[0]->ChildrenNodes.Remove("1");
	TreeView->RequestTreeRefresh();
}

void FArmyDetailBuilder::UpdateDetail()
{
	TreeSource.Reset();

	for (auto& It : ChildrenNodes)
	{
		TreeSource.Add(It.Value);
	}
}

void FArmyDetailBuilder::SetTreeItemExpansion(bool bOpen)
{
	for (auto& It : TreeSource)
	{
		TreeView->SetItemExpansion(It, bOpen);
	}
}

TSharedRef<ITableRow> FArmyDetailBuilder::OnGenerateRowForTree(TSharedPtr<FArmyDetailNode> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{	
	if (InItem->bUseSplitter)
	{
        TSharedRef<SHorizontalBox> SplitterWidget =
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .FillWidth(86.f / 200.f)
            [
                InItem->GenerateNameWidget().ToSharedRef()
            ] 

            + SHorizontalBox::Slot()
            .FillWidth(114.f / 200.f)
            [
                InItem->GenerateValueWidget().ToSharedRef()
            ];
        SplitterWidget->SetVisibility(InItem->IfVisibility);

		if (InItem->bUseBackgroundBorder)
		{
			return SNew(STableRow<TSharedPtr<FArmyDetailNode>>, OwnerTable)
                .ExpanderStyleSet(&FArmyStyle::Get())
				.Visibility(InItem->IfVisibility)
				[
					SNew(SBorder)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					.BorderBackgroundColor(FSlateColor(FLinearColor(0.01, 0.01, 0.01, 1)))
					.Padding(FMargin(0, 4))
					[
						SplitterWidget
					]
				];
		}
		else
		{
            if (InItem->bIsCategory)
            {
                return SNew(STableRow<TSharedPtr<FArmyDetailNode>>, OwnerTable)
                    .ExpanderStyleSet(&FArmyStyle::Get())
                    .Visibility(InItem->IfVisibility)
                    .Padding(FMargin(8, 4, 0, 4))
                    [
                        SplitterWidget
                    ];
            }
            else
            {
                return SNew(STableRow<TSharedPtr<FArmyDetailNode>>, OwnerTable)
                    .ExpanderStyleSet(&FArmyStyle::Get())
                    .Visibility(InItem->IfVisibility)
                    .Padding(FMargin(8, 4, 0, 4))
                    [
                        SplitterWidget
                    ];
            }
		}
	}
	else
	{
		if (InItem->bUseBackgroundBorder)
		{
			return SNew(STableRow<TSharedPtr<FArmyDetailNode>>, OwnerTable)
                .ExpanderStyleSet(&FArmyStyle::Get())
				[
					SNew(SBorder)
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					.BorderBackgroundColor(FSlateColor(FLinearColor(0.01, 0.01, 0.01, 1)))
                    .Padding(FMargin(0, 4))
					[
						InItem->GenerateWholeRowWidget().ToSharedRef()
					]
				];
		}
		else
		{
			return SNew(STableRow<TSharedPtr<FArmyDetailNode>>, OwnerTable)
                .ExpanderStyleSet(&FArmyStyle::Get())
                .Padding(FMargin(0, 4))
				[
					InItem->GenerateWholeRowWidget().ToSharedRef()
				];
		}
	}
}

void FArmyDetailBuilder::GetChildrenForTree(TSharedPtr< FArmyDetailNode > TreeItem, TArray< TSharedPtr<FArmyDetailNode> >& OutChildren)
{
	for (auto& It : TreeItem->ChildrenNodes)
	{
		OutChildren.Add(It.Value);
	}
}
