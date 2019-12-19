#include "ArmySceneOutlinerItemLabelColumn.h"
#include "IXRTreeItem.h"
#include "ArmyActorTreeItem.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/Views/STableRow.h"
#include "ArmyFolderTreeItem.h"
#include "ArmyWorldTreeItem.h"
#include "SImage.h"
#include "STextBlock.h"
#include "ArmyEditorEngine.h"

struct FArmyCommonLabelData
{
    TWeakPtr<IXRSceneOutliner> WeakSceneOutliner;
};

FName FArmyItemLabelColumn::GetColumnID()
{
    return GetID();
}

SHeaderRow::FColumn::FArguments FArmyItemLabelColumn::ConstructHeaderRowColumn()
{
    return SHeaderRow::Column(GetColumnID())
        .DefaultLabel(FText::FromString(TEXT("物体列表")))
        .FillWidth(5.0f);
}

struct SXRActorTreeLabel : FArmyCommonLabelData, public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SXRActorTreeLabel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, FArmyActorTreeItem& ActorItem, IXRSceneOutliner& SceneOutliner, const STableRow<FArmyTreeItemPtr>& InRow)
    {
        WeakSceneOutliner = StaticCastSharedRef<IXRSceneOutliner>(SceneOutliner.AsShared());

        TreeItemPtr = StaticCastSharedRef<FArmyActorTreeItem>(ActorItem.AsShared());
        ActorPtr = ActorItem.Actor;

        TSharedPtr<SInlineEditableTextBlock> InlineTextBlock;

        ChildSlot
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(0.0f, 2.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				[
					SAssignNew(InlineTextBlock, SInlineEditableTextBlock)
					.Text(this, &SXRActorTreeLabel::GetDisplayText)
                    .Style(FArmyStyle::Get(), "InlineEditableTextBlockStyle")
				]
			]
		];
    }

private:
	TWeakPtr<FArmyActorTreeItem> TreeItemPtr;
	TWeakObjectPtr<AActor> ActorPtr;

    FText GetDisplayText() const
    {
        const AActor* Actor = ActorPtr.Get();
        return Actor ? FText::FromString(Actor->GetName()) : FText::FromString(TEXT("(Deleted Actor)"));
    }
};

struct SXRWorldTreeLabel : FArmyCommonLabelData, public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SXRWorldTreeLabel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, FArmyWorldTreeItem& WorldItem, IXRSceneOutliner& SceneOutliner, const STableRow<FArmyTreeItemPtr>& InRow)
    {
        TreeItemPtr = StaticCastSharedRef<FArmyWorldTreeItem>(WorldItem.AsShared());
        WeakSceneOutliner = StaticCastSharedRef<IXRSceneOutliner>(SceneOutliner.AsShared());

        ChildSlot
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(0.0f, 2.0f)
			[
				SNew(STextBlock)
				.Text(this, &SXRWorldTreeLabel::GetDisplayText)
                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
			]
		];
    }

private:
    TWeakPtr<FArmyWorldTreeItem> TreeItemPtr;

    FText GetDisplayText() const
    {
        return FText::FromString(TEXT("户型"));
	}
};

struct SXRFolderTreeLabel : FArmyCommonLabelData, public SCompoundWidget
{
    SLATE_BEGIN_ARGS(SXRFolderTreeLabel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, FArmyFolderTreeItem& FolderItem, IXRSceneOutliner& SceneOutliner, const STableRow<FArmyTreeItemPtr>& InRow)
	{
        TreeItemPtr = StaticCastSharedRef<FArmyFolderTreeItem>(FolderItem.AsShared());
        WeakSceneOutliner = StaticCastSharedRef<IXRSceneOutliner>(SceneOutliner.AsShared());

        TSharedPtr<SInlineEditableTextBlock> InlineTextBlock = SNew(SInlineEditableTextBlock)
            .Text(this, &SXRFolderTreeLabel::GetDisplayText)
            .Style(FArmyStyle::Get(), "InlineEditableTextBlockStyle");

        ChildSlot
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.Padding(0.0f, 2.0f)
			[
				InlineTextBlock.ToSharedRef()
			]
		];
    }

private:
    TWeakPtr<FArmyFolderTreeItem> TreeItemPtr;

    FText GetDisplayText() const
    {
        auto Folder = TreeItemPtr.Pin();
        return Folder.IsValid() ? FText::FromName(Folder->LeafName) : FText();
    }
};

const TSharedRef<SWidget> FArmyItemLabelColumn::ConstructRowWidget(FArmyTreeItemRef TreeItem, const STableRow<FArmyTreeItemPtr>& Row)
{
    FArmyColumnWidgetGenerator Generator(*this, Row);
    TreeItem->Visit(Generator);

    return Generator.Widget.ToSharedRef();
}

TSharedRef<SWidget> FArmyItemLabelColumn::GenerateWidget(FArmyActorTreeItem& TreeItem, const STableRow<FArmyTreeItemPtr>& InRow)
{
    IXRSceneOutliner* Outliner = WeakSceneOutliner.Pin().Get();
    check(Outliner);
    return SNew(SXRActorTreeLabel, TreeItem, *Outliner, InRow);
}

TSharedRef<SWidget> FArmyItemLabelColumn::GenerateWidget(FArmyWorldTreeItem& TreeItem, const STableRow<FArmyTreeItemPtr>& InRow)
{
    IXRSceneOutliner* Outliner = WeakSceneOutliner.Pin().Get();
    check(Outliner);
    return SNew(SXRWorldTreeLabel, TreeItem, *Outliner, InRow);

    return SNullWidget::NullWidget;
}

TSharedRef<SWidget> FArmyItemLabelColumn::GenerateWidget(FArmyFolderTreeItem& TreeItem, const STableRow<FArmyTreeItemPtr>& InRow)
{
    IXRSceneOutliner* Outliner = WeakSceneOutliner.Pin().Get();
    check(Outliner);
    return SNew(SXRFolderTreeLabel, TreeItem, *Outliner, InRow);
}
