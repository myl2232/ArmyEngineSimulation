#include "ArmySceneOutlinerSelectableGutter.h"
#include "IXRSceneOutliner.h"
#include "SImage.h"
#include "SSpacer.h"
#include "ArmyStyle.h"
#include "ArmyActorTreeItem.h"
#include "ArmyWorldTreeItem.h"
#include "ArmyFolderTreeItem.h"
#include "Widgets/Views/STreeView.h"
#include "IXRSceneOutliner.h"
#include "ArmyEditorEngine.h"
#include "ArmyActorConstant.h"
/** Widget responsible for managing the visibility for a single actor */
class SXRSelectWidget : public SImage
{
public:
    SLATE_BEGIN_ARGS(SXRSelectWidget) {}
    SLATE_END_ARGS()

    /** Construct this widget */
	void Construct(const FArguments& InArgs, TWeakPtr<FArmySceneOutlinerSelectableGutter> InWeakColumn, TWeakPtr<IXRSceneOutliner> InWeakOutliner, TWeakPtr<IXRTreeItem> InWeakTreeItem)
	{
		WeakColumn = InWeakColumn;
		WeakTreeItem = InWeakTreeItem;
		WeakOutliner = InWeakOutliner;

		SImage::Construct(
			SImage::FArguments()
			.Image(this, &SXRSelectWidget::GetBrush)
		);
	}

private:
    FReply HandleClick()
    {
        auto Outliner = WeakOutliner.Pin();
        auto TreeItem = WeakTreeItem.Pin();
        auto Column = WeakColumn.Pin();

        if (!Outliner.IsValid() || !TreeItem.IsValid() || !Column.IsValid())
        {
            return FReply::Unhandled();
        }

        const auto& Tree = Outliner->GetTree();

        const bool TempbSelectable = !IsSelectable();//点击后的状态，是否可选

        // We operate on all the selected items if the specified item is selected	//摁住Ctrl或者Shift进行多选
        if (Tree.IsItemSelected(TreeItem.ToSharedRef()))
        {
            const FArmySetSelectableVisitor Visitor(TempbSelectable);

            for (auto& SelectedItem : Tree.GetSelectedItems())
            {
                if (IsSelectable(SelectedItem, Column) != TempbSelectable)
                {
                    SelectedItem->Visit(Visitor);
                }
            }
        }
        else
        {
			SetIsSelectable(TempbSelectable);
        }

        return FReply::Handled()/*.DetectDrag(SharedThis(this), EKeys::LeftMouseButton)*/;
    }

    /** Called when the mouse button is pressed down on this widget */
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
    {
        if (MouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        {
            return FReply::Unhandled();
        }

        return HandleClick();
    }

    /** Process a mouse up message */
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
    {
        if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        {
            return FReply::Handled();
        }

        return FReply::Unhandled();
    }

    /** Get the brush for this widget */
    const FSlateBrush* GetBrush() const
    {
        if (IsSelectable())
		{
			return IsHovered() ? FArmyStyle::GetBrush("Level.unlock_hover") :
				FArmyStyle::GetBrush("Level.unlock_normal");
		}
		else
		{
			return IsHovered() ? FArmyStyle::GetBrush("Level.lock_hover") :
				FArmyStyle::GetBrush("Level.lock_normal");
		}
    }

    /** Check if the specified item is visible */
    static bool IsSelectable(const FArmyTreeItemPtr& Item, const TSharedPtr<FArmySceneOutlinerSelectableGutter>& Column)
    {
        return Column.IsValid() && Item.IsValid() ? Column->IsItemSelectable(*Item) : false;
    }

    /** 点击之前是否可选 */
    bool IsSelectable() const
    {
        return IsSelectable(WeakTreeItem.Pin(), WeakColumn.Pin());
    }

    /** Set the actor this widget is responsible for to be hidden or shown */
    void SetIsSelectable(const bool bTempSelectable)
    {
        auto TreeItem = WeakTreeItem.Pin();
        if (TreeItem.IsValid() && IsSelectable() != bTempSelectable)
        {
            FArmySetSelectableVisitor Visitor(bTempSelectable);
            TreeItem->Visit(Visitor);
        }
    }

    /** The tree item we relate to */
    TWeakPtr<IXRTreeItem> WeakTreeItem;

    /** Reference back to the outliner so we can set visibility of a whole selection */
    TWeakPtr<IXRSceneOutliner> WeakOutliner;

    /** Weak pointer back to the column */
    TWeakPtr<FArmySceneOutlinerSelectableGutter> WeakColumn;
};


bool FArmyGetSelectableVisitor::RecurseChildren(const IXRTreeItem& Item) const
{
	if (const bool* Info = SelectableInfo.Find(&Item))
	{
		return *Info;
	}
	else
	{
		bool bIsSelectable = false;
		for (const auto& ChildPtr : Item.GetChildren())
		{
			auto Child = ChildPtr.Pin();
			if (Child.IsValid() && Child->Get(*this))
			{
				bIsSelectable = true;
				break;
			}
		}
		SelectableInfo.Add(&Item, bIsSelectable);

		return bIsSelectable;
	}
}

bool FArmyGetSelectableVisitor::Get(const FArmyActorTreeItem& ActorItem) const
{
	if (const bool* Info = SelectableInfo.Find(&ActorItem))
	{
		return *Info;
	}
	else
	{
		const AActor* Actor = ActorItem.Actor.Get();

		const bool bIsSelectable = (Actor != nullptr) && Actor->bIsSelectable;//判定是否可选
		SelectableInfo.Add(&ActorItem, bIsSelectable);

		return bIsSelectable;
	}
}

bool FArmyGetSelectableVisitor::Get(const FArmyWorldTreeItem& WorldItem) const
{
	return RecurseChildren(WorldItem);
}

bool FArmyGetSelectableVisitor::Get(const FArmyFolderTreeItem& FolderItem) const
{
	return RecurseChildren(FolderItem);
}

void FArmySetSelectableVisitor::Visit(FArmyActorTreeItem& ActorItem) const
{
	AActor* Actor = ActorItem.Actor.Get();
	if (Actor)
	{
		if (!Actor->Tags.Contains(XRActorTag::UnSelectable))
		{
			Actor->bIsSelectable = bSelectable;//设置是否可选
		}
		else
		{
			Actor->bIsSelectable = false;
		}
		/** @梁晓菲 如果Actor被锁定，正处于选中状态，将选中状态取消; 即如果不可选但是当前对该物体进行了选中，那么取消选中*/
		if (Actor->bIsSelectable == false)
		{
			GXREditor->SelectActor(Actor, false, false, true);
		}
	}
}

void FArmySetSelectableVisitor::Visit(FArmyWorldTreeItem& WorldItem) const
{
	for (auto& ChildPtr : WorldItem.GetChildren())
	{
		auto Child = ChildPtr.Pin();
		if (Child.IsValid())
		{
			FArmySetSelectableVisitor Selectable(bSelectable);
			Child->Visit(Selectable);
		}
	}
}

void FArmySetSelectableVisitor::Visit(FArmyFolderTreeItem& FolderItem) const
{
	for (auto& ChildPtr : FolderItem.GetChildren())
	{
		auto Child = ChildPtr.Pin();
		if (Child.IsValid())
		{
			FArmySetSelectableVisitor Selectable(bSelectable);
			Child->Visit(Selectable);
		}
	}
}

FArmySceneOutlinerSelectableGutter::FArmySceneOutlinerSelectableGutter(IXRSceneOutliner& Outliner)
{
	WeakOutliner = StaticCastSharedRef<IXRSceneOutliner>(Outliner.AsShared());
}

FName FArmySceneOutlinerSelectableGutter::GetColumnID()
{
	return GetID();
}

SHeaderRow::FColumn::FArguments FArmySceneOutlinerSelectableGutter::ConstructHeaderRowColumn()
{
	return SHeaderRow::Column(GetColumnID())
		.FixedWidth(16.f)
		[
			SNew(SSpacer)
		];
}

const TSharedRef<SWidget> FArmySceneOutlinerSelectableGutter::ConstructRowWidget(FArmyTreeItemRef TreeItem, const STableRow<FArmyTreeItemPtr>& Row)
{
	//return SNew(SHorizontalBox)
	//	+ SHorizontalBox::Slot()
	//	.AutoWidth()
	//	.Padding(0, 0, 0, 0)
	//	.VAlign(VAlign_Center)
	//	[
	//		SNew(SXRSelectWidget, SharedThis(this), WeakOutliner, TreeItem)
	//	];

    return SNullWidget::NullWidget;
}

void FArmySceneOutlinerSelectableGutter::Tick(double InCurrentTime, float InDeltaTime)
{
	SelectableCache.SelectableInfo.Empty();
}