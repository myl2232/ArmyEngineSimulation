#include "ArmySceneOutlinerGutter.h"
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
class SXRVisibilityWidget : public SImage
{
public:
    SLATE_BEGIN_ARGS(SXRVisibilityWidget) {}
    SLATE_END_ARGS()

    /** Construct this widget */
	void Construct(const FArguments& InArgs, TWeakPtr<FArmySceneOutlinerGutter> InWeakColumn, TWeakPtr<IXRSceneOutliner> InWeakOutliner, TWeakPtr<IXRTreeItem> InWeakTreeItem)
	{
		WeakTreeItem = InWeakTreeItem;
		WeakOutliner = InWeakOutliner;
		WeakColumn = InWeakColumn;

		SImage::Construct(
			SImage::FArguments()
			.Image(this, &SXRVisibilityWidget::GetBrush)
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

        const bool bVisible = !IsVisible();

        // We operate on all the selected items if the specified item is selected
        if (Tree.IsItemSelected(TreeItem.ToSharedRef()))
        {
            const FArmySetVisibilityVisitor Visitor(bVisible);

            for (auto& SelectedItem : Tree.GetSelectedItems())
            {
                if (IsVisible(SelectedItem, Column) != bVisible)
                {
                    SelectedItem->Visit(Visitor);
                }
            }

            //GEditor->RedrawAllViewports();
        }
        else
        {
            SetIsVisible(bVisible);
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
        if (IsVisible())
        {
            return IsHovered() ? FArmyStyle::GetBrush("Level.VisibleHighlightIcon16x") :
                FArmyStyle::GetBrush("Level.VisibleIcon16x");
        }
        else
        {
            return IsHovered() ? FArmyStyle::GetBrush("Level.NotVisibleHighlightIcon16x") :
                FArmyStyle::GetBrush("Level.NotVisibleIcon16x");
        }
    }

    /** Check if the specified item is visible */
    static bool IsVisible(const FArmyTreeItemPtr& Item, const TSharedPtr<FArmySceneOutlinerGutter>& Column)
    {
        return Column.IsValid() && Item.IsValid() ? Column->IsItemVisible(*Item) : false;
    }

    /** Check if our wrapped tree item is visible */
    bool IsVisible() const
    {
        return IsVisible(WeakTreeItem.Pin(), WeakColumn.Pin());
    }

    /** Set the actor this widget is responsible for to be hidden or shown */
    void SetIsVisible(const bool bVisible)
    {
        auto TreeItem = WeakTreeItem.Pin();
        if (TreeItem.IsValid() && IsVisible() != bVisible)
        {
            FArmySetVisibilityVisitor Visitor(bVisible);
            TreeItem->Visit(Visitor);

            //GEditor->RedrawAllViewports();
        }
    }

    /** The tree item we relate to */
    TWeakPtr<IXRTreeItem> WeakTreeItem;

    /** Reference back to the outliner so we can set visibility of a whole selection */
    TWeakPtr<IXRSceneOutliner> WeakOutliner;

    /** Weak pointer back to the column */
    TWeakPtr<FArmySceneOutlinerGutter> WeakColumn;
};

FArmySceneOutlinerGutter::FArmySceneOutlinerGutter(IXRSceneOutliner& Outliner)
{
    WeakOutliner = StaticCastSharedRef<IXRSceneOutliner>(Outliner.AsShared());
}

FName FArmySceneOutlinerGutter::GetColumnID()
{
    return GetID();
}

const TSharedRef<SWidget> FArmySceneOutlinerGutter::ConstructRowWidget(FArmyTreeItemRef TreeItem, const STableRow<FArmyTreeItemPtr>& Row)
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
		.Padding(16, 0, 0, 0)
        .VAlign(VAlign_Center)
        [
            SNew(SXRVisibilityWidget, SharedThis(this), WeakOutliner, TreeItem)
        ];
}

void FArmySceneOutlinerGutter::Tick(double InCurrentTime, float InDeltaTime)
{
    VisibilityCache.VisibilityInfo.Empty();
}

SHeaderRow::FColumn::FArguments FArmySceneOutlinerGutter::ConstructHeaderRowColumn()
{
    return SHeaderRow::Column(GetColumnID())
        .FixedWidth(32.f)
        [
            SNew(SSpacer)
        ];
}

bool FArmyGetVisibilityVisitor::RecurseChildren(const IXRTreeItem& Item) const
{
    if (const bool* Info = VisibilityInfo.Find(&Item))
    {
        return *Info;
    }
    else
    {
        bool bIsVisible = false;
        for (const auto& ChildPtr : Item.GetChildren())
        {
            auto Child = ChildPtr.Pin();
            if (Child.IsValid() && Child->Get(*this))
            {
                bIsVisible = true;
                break;
            }
        }
        VisibilityInfo.Add(&Item, bIsVisible);

        return bIsVisible;
    }
}

bool FArmyGetVisibilityVisitor::Get(const FArmyActorTreeItem& ActorItem) const
{
    if (const bool* Info = VisibilityInfo.Find(&ActorItem))
    {
        return *Info;
    }
    else
    {
        const AActor* Actor = ActorItem.Actor.Get();

        const bool bIsVisible = (Actor != nullptr) && !Actor->bHidden;
        VisibilityInfo.Add(&ActorItem, bIsVisible);

        return bIsVisible;
    }
}

bool FArmyGetVisibilityVisitor::Get(const FArmyWorldTreeItem& WorldItem) const
{
    return RecurseChildren(WorldItem);
}

bool FArmyGetVisibilityVisitor::Get(const FArmyFolderTreeItem& FolderItem) const
{
    return RecurseChildren(FolderItem);
}

void FArmySetVisibilityVisitor::Visit(FArmyActorTreeItem& ActorItem) const
{
	AActor* Actor = ActorItem.Actor.Get();
	if (Actor)
	{
		Actor->SetActorHiddenInGame(!bSetVisibility);
		if (bUsePropertySelect)
		{
			if (!Actor->Tags.Contains(XRActorTag::UnSelectable))
			{
				Actor->bIsSelectable = bSelectable;//设置是否可选
			}
			else
			{
				Actor->bIsSelectable = false;
			}
		}

        // 当选中的物体被隐藏时，需要取消物体的选中
        if (!bSetVisibility)
        {
            for (FSelectionIterator It(GXREditor->GetSelectedActorIterator()); It; ++It)
            {
                AActor* SelectedActor = Cast<AActor>(*It);
                if (SelectedActor == Actor)
                {
                    GXREditor->SelectNone(true, true);
                }
            }
        }
	}
}

void FArmySetVisibilityVisitor::Visit(FArmyWorldTreeItem& WorldItem) const
{
	for (auto& ChildPtr : WorldItem.GetChildren())
	{
		auto Child = ChildPtr.Pin();
		if (Child.IsValid())
		{
			FArmySetVisibilityVisitor Visibility(bSetVisibility,bSelectable,bUsePropertySelect);
			Child->Visit(Visibility);
		}
	}
}

void FArmySetVisibilityVisitor::Visit(FArmyFolderTreeItem& FolderItem) const
{
	for (auto& ChildPtr : FolderItem.GetChildren())
	{
		auto Child = ChildPtr.Pin();
		if (Child.IsValid())
		{
			FArmySetVisibilityVisitor Visibility(bSetVisibility, bSelectable, bUsePropertySelect);
			Child->Visit(Visibility);
		}
	}
}