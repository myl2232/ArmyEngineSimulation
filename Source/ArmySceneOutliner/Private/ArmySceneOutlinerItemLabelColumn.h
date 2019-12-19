#pragma once

#include "IXRSceneOutliner.h"
#include "IXRSceneOutlinerColumn.h"
#include "ArmySceneOutlinerPublicTypes.h"
#include "ArmySceneOutlinerVisitorTypes.h"

/** A column for the SceneOutliner that displays the item label */
class FArmyItemLabelColumn : public IXRSceneOutlinerColumn
{
public:
    FArmyItemLabelColumn(IXRSceneOutliner& SceneOutliner)
        : WeakSceneOutliner(StaticCastSharedRef<IXRSceneOutliner>(SceneOutliner.AsShared()))
    {}

    virtual ~FArmyItemLabelColumn() {}

    static FName GetID() { return FArmyBuiltInColumnTypes::Label(); }

    virtual FName GetColumnID() override;

    virtual SHeaderRow::FColumn::FArguments ConstructHeaderRowColumn() override;

    virtual const TSharedRef<SWidget> ConstructRowWidget(FArmyTreeItemRef TreeItem, const STableRow<FArmyTreeItemPtr>& Row) override;

private:
    TWeakPtr<IXRSceneOutliner> WeakSceneOutliner;

private:
    TSharedRef<SWidget> GenerateWidget(FArmyActorTreeItem& TreeItem, const STableRow<FArmyTreeItemPtr>& InRow);
    TSharedRef<SWidget> GenerateWidget(FArmyWorldTreeItem& TreeItem, const STableRow<FArmyTreeItemPtr>& InRow);
    TSharedRef<SWidget> GenerateWidget(FArmyFolderTreeItem& TreeItem, const STableRow<FArmyTreeItemPtr>& InRow);

    struct FArmyColumnWidgetGenerator : FArmyColumnGenerator
    {
        FArmyItemLabelColumn& Column;
        const STableRow<FArmyTreeItemPtr>& Row;

        FArmyColumnWidgetGenerator(FArmyItemLabelColumn& InColumn, const STableRow<FArmyTreeItemPtr>& InRow)
            : Column(InColumn)
            , Row(InRow)
        {}

        virtual TSharedRef<SWidget> GenerateWidget(FArmyFolderTreeItem& Item) const override { return Column.GenerateWidget(Item, Row); }
        virtual TSharedRef<SWidget> GenerateWidget(FArmyWorldTreeItem& Item) const override { return Column.GenerateWidget(Item, Row); }
        virtual TSharedRef<SWidget> GenerateWidget(FArmyActorTreeItem& Item) const override { return Column.GenerateWidget(Item, Row); }
    };
};