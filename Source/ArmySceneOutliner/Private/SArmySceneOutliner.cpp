#include "SXRSceneOutliner.h"
#include "ArmyEditorTypes.h"
#include "ArmyEditorActorFolders.h"
#include "ArmyActorTreeItem.h"
#include "ArmyFolderTreeItem.h"
#include "ArmyWorldTreeItem.h"
#include "EngineUtils.h"
#include "ArmySceneOutlinerModule.h"
#include "IXRSceneOutlinerColumn.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Selection.h"
#include "ArmyActor/XRShapeBoardActor.h"
#include "ArmyLightActor.h"
#include "ArmyExtrusion/Public/XRExtrusionActor.h"
#include "ArmyEditorEngine.h"
#include "ArmySceneOutlinerGutter.h"
#include "Engine/PointLight.h"

struct FArmyItemSelection : IXRMutableTreeItemVisitor
{
    mutable TArray<FArmyActorTreeItem*> Actors;
    mutable TArray<FArmyWorldTreeItem*> Worlds;
    mutable TArray<FArmyFolderTreeItem*> Folders;

    FArmyItemSelection()
    {}

    FArmyItemSelection(SXROutlinerTreeView& Tree)
    {
        for (auto& Item : Tree.GetSelectedItems())
        {
            Item->Visit(*this);
        }
    }

    TArray<TWeakObjectPtr<AActor>> GetWeakActors() const
    {
        TArray<TWeakObjectPtr<AActor>> ActorArray;
        for (const auto* ActorItem : Actors)
        {
            if (ActorItem->Actor.IsValid())
            {
                ActorArray.Add(ActorItem->Actor);
            }
        }
        return ActorArray;
    }

    TArray<AActor*> GetActorPtrs() const
    {
        TArray<AActor*> ActorArray;
        for (const auto* ActorItem : Actors)
        {
            if (AActor* Actor = ActorItem->Actor.Get())
            {
                ActorArray.Add(Actor);
            }
        }
        return ActorArray;
    }

private:
    virtual void Visit(FArmyActorTreeItem& ActorItem) const override
    {
        Actors.Add(&ActorItem);
    }
    virtual void Visit(FArmyWorldTreeItem& WorldItem) const override
    {
        Worlds.Add(&WorldItem);
    }
    virtual void Visit(FArmyFolderTreeItem& FolderItem) const override
    {
        Folders.Add(&FolderItem);
    }
};

void SXRSceneOutliner::Construct(const FArguments& InArgs, const FArmyInitializationOptions& InInitOptions)
{
    SharedData->RepresentingWorld = InInitOptions.RepresentingWorld;
	SharedData->RepresentingClient = InInitOptions.RepresentingClient;

    bFullRefresh = false;
    bNeedsRefresh = true;
    bIsReentrant = false;

    TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

    TSharedRef<SHeaderRow> HeaderRowWidget =
        SNew(SHeaderRow)
        // Only show the list header if the user configured the outliner for that
        //.Visibility(InInitOptions.bShowHeaderRow ? EVisibility::Visible : EVisibility::Collapsed);
        .Visibility(EVisibility::Collapsed);//梁晓菲 去掉之前的“物体列表”提示

    SetupColumns(*HeaderRowWidget);

    ChildSlot
    [
        SNew(SBorder)
        //.BorderImage(this, &SSceneOutliner::OnGetBorderBrush)
        .BorderBackgroundColor(FLinearColor(FColor(0X00000000)))//梁晓菲 右侧物体列表边框颜色
        .Padding(FMargin(0))
        .ShowEffectWhenDisabled(false)
        [
            VerticalBox
        ]
    ];

    VerticalBox->AddSlot()
	.FillHeight(1.0f)
	[
		SNew(SOverlay)

		+ SOverlay::Slot()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SBorder)
            .BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
            .BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
        ]

		+SOverlay::Slot()
        .Padding(0, 8, 0, 0)
		[
			SAssignNew(OutlinerTreeView, SXROutlinerTreeView)

            .ItemHeight(24)

			// multi-select if we're in browsing mode, 
			// single-select if we're in picking mode,
			.SelectionMode(ESelectionMode::Multi)

			// Point the tree to our array of root-level items.  Whenever this changes, we'll call RequestTreeRefresh()
			.TreeItemsSource(&RootTreeItems)

			// Find out when the user selects something in the tree
			.OnSelectionChanged(this, &SXRSceneOutliner::OnOutlinerTreeSelectionChanged)

			//// Called when the user double-clicks with LMB on an item in the list
			.OnMouseButtonDoubleClick( this, &SXRSceneOutliner::OnOutlinerTreeDoubleClick )

			//// Called when an item is scrolled into view
			//.OnItemScrolledIntoView( this, &SSceneOutliner::OnOutlinerTreeItemScrolledIntoView )

			//// Called when an item is expanded or collapsed
			//.OnExpansionChanged(this, &SSceneOutliner::OnItemExpansionChanged)

			// Called to child items for any given parent item
			.OnGetChildren(this, &SXRSceneOutliner::OnGetChildrenForOutlinerTree)

			// Generates the actual widget for a tree item
			.OnGenerateRow(this, &SXRSceneOutliner::OnGenerateRowForOutlinerTree) 

			//// Use the level viewport context menu as the right click menu for tree items
			//.OnContextMenuOpening(this, &SSceneOutliner::OnOpenContextMenu)

			// Header for the tree
			.HeaderRow(HeaderRowWidget)

			// Called when an item is expanded or collapsed with the shift-key pressed down
			.OnSetExpansionRecursive(this, &SXRSceneOutliner::SetItemExpansionRecursive)
		]
	];

    GEngine->OnLevelActorAdded().AddSP(this, &SXRSceneOutliner::OnLevelActorsAdded);
    GEngine->OnLevelActorDeleted().AddSP(this, &SXRSceneOutliner::OnLevelActorsRemoved);
    GEngine->OnLevelActorFolderChanged().AddSP(this, &SXRSceneOutliner::OnLevelActorFolderChanged);
	//GEngine->OnLevelActorShowOrHide().AddSP(this, &SXRSceneOutliner::OnLevelActorShowOrHide);
    // We only synchronize selection when in actor browsing mode
    if (SharedData->Mode == EXRSceneOutlinerMode::ActorBrowsing)
    {
        // Populate and register to find out when the level's selection changes
        OnLevelSelectionChanged(NULL);
        USelection::SelectionChangedEvent.AddRaw(this, &SXRSceneOutliner::OnLevelSelectionChanged);
        USelection::SelectObjectEvent.AddRaw(this, &SXRSceneOutliner::OnLevelSelectionChanged);
    }
	SharedData->RepresentingClient->OnViewTypeChanged.AddRaw(this, &SXRSceneOutliner::OnViewTypeChanged);
    Populate();
}

SXRSceneOutliner::~SXRSceneOutliner()
{
    if (SharedData->Mode == EXRSceneOutlinerMode::ActorBrowsing)
    {
        USelection::SelectionChangedEvent.RemoveAll(this);
        USelection::SelectObjectEvent.RemoveAll(this);
    }
}

void SXRSceneOutliner::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    for (auto& Pair : Columns)
    {
        Pair.Value->Tick(InCurrentTime, InDeltaTime);
    }

    if (bNeedsRefresh && !bIsReentrant)
    {
        Populate();
    }
}

void SXRSceneOutliner::Refresh(bool bForce)
{
	if (bForce)
	{
		Populate();
	}
	else
	{
		bNeedsRefresh = true;
	}
}

void SXRSceneOutliner::OnItemAdded(const FArmyTreeItemID& ItemID, uint8 ActionMask)
{
    NewItemActions.Add(ItemID, ActionMask);
}

void SXRSceneOutliner::EmptyTreeItems()
{
    TreeItemMap.Reset();

    RootTreeItems.Empty();
}

void SXRSceneOutliner::Populate()
{
    // Block events while we clear out the list.  We don't want actors in the level to become deselected
    // while we doing this
    TGuardValue<bool> ReentrantGuard(bIsReentrant, true);

    if (!CheckWorld())
    {
        return;
    }

   // RepopulateEntireTree();

    // Only deal with 500 at a time
    const int32 End = FMath::Min(PendingOperations.Num(), 500);
    for (int32 Index = 0; Index < End; ++Index)
    {
        auto& PendingOp = PendingOperations[Index];
        switch (PendingOp.Type)
        {
        case FArmyPendingTreeOperation::Added:
            //bMadeAnySignificantChanges = AddItemToTree(PendingOp.Item) || bMadeAnySignificantChanges;
            AddItemToTree(PendingOp.Item);
            break;

        case FArmyPendingTreeOperation::Moved:
            //bMadeAnySignificantChanges = true;
            OnItemMoved(PendingOp.Item);
            break;

        case FArmyPendingTreeOperation::Removed:
            //bMadeAnySignificantChanges = true;
            RemoveItemFromTree(PendingOp.Item);
            break;

        default:
            check(false);
            break;
        }
    }

    PendingOperations.RemoveAt(0, End);

	OutlinerTreeView->RequestTreeRefresh();

    if (PendingOperations.Num() == 0)
    {
        // We're fully refreshed now.
        NewItemActions.Empty();
        bNeedsRefresh = false;
    }
}

void SXRSceneOutliner::RepopulateEntireTree()
{
    EmptyTreeItems();

    ConstructItemFor<FArmyWorldTreeItem>(SharedData->RepresentingWorld);

    // Add any folders which might match the current search terms
    for (const auto& Pair : FArmyActorFolders::Get().GetFolderPropertiesForWorld(*SharedData->RepresentingWorld))
    {
        if (!TreeItemMap.Contains(Pair.Key))
        {
            ConstructItemFor<FArmyFolderTreeItem>(Pair.Key);
        }
    }

    // Iterate over every actor in memory. WARNING: This is potentially very expensive!
    for (FActorIterator ActorIt(SharedData->RepresentingWorld); ActorIt; ++ActorIt)
    {
        AActor* Actor = *ActorIt;
        if (Actor && IsActorDisplayable(Actor))
        {
            ConstructItemFor<FArmyActorTreeItem>(Actor);
        }
    }
}

bool SXRSceneOutliner::AddItemToTree(FArmyTreeItemRef InItem)
{
    const auto ItemID = InItem->GetID();

    PendingTreeItemMap.Remove(ItemID);

    // If a tree item already exists that represents the same data, bail
    if (TreeItemMap.Find(ItemID))
    {
        return false;
    }

    AddUnfilteredItemToTree(InItem);

    return true;
}

void SXRSceneOutliner::AddUnfilteredItemToTree(FArmyTreeItemRef Item)
{
    Item->SharedData = SharedData;

    FArmyTreeItemPtr Parent = EnsureParentForItem(Item);

    const FArmyTreeItemID ItemID = Item->GetID();
    if (TreeItemMap.Contains(ItemID))
    {
        // this is a fatal error
        check(false);
    }

    TreeItemMap.Add(ItemID, Item);

    if (Parent.IsValid())
    {
        Parent->AddChild(Item);
    }
    else
    {
        RootTreeItems.Add(Item);
    }

	if (Item->GetID() == FArmyTreeItemID(SharedData->RepresentingWorld))
	{
		SetItemExpansionRecursive(Item, true);
	}
}

FArmyTreeItemPtr SXRSceneOutliner::EnsureParentForItem(FArmyTreeItemRef Item)
{
    FArmyTreeItemPtr Parent = Item->FindParent(TreeItemMap);
    if (Parent.IsValid())
    {
        return Parent;
    }
    else
    {
        auto NewParent = Item->CreateParent();
        if (NewParent.IsValid())
        {
            //NewParent->Flags.bIsFilteredOut = !Filters->TestAndSetInteractiveState(*NewParent) || !SearchBoxFilter->PassesFilter(*NewParent);

            AddUnfilteredItemToTree(NewParent.ToSharedRef());
            return NewParent;
        }
    }

    return nullptr;
}

void SXRSceneOutliner::RemoveItemFromTree(FArmyTreeItemRef InItem)
{
    if (TreeItemMap.Contains(InItem->GetID()))
    {
        auto Parent = InItem->GetParent();

        if (Parent.IsValid())
        {
            Parent->RemoveChild(InItem);
            OnChildRemovedFromParent(*Parent);
        }
        else
        {
            RootTreeItems.Remove(InItem);
        }

        //InItem->Visit(FFunctionalVisitor().Actor([&](const FActorTreeItem& ActorItem) {
        //    if (!ActorItem.Flags.bIsFilteredOut)
        //    {
        //        --FilteredActorCount;
        //    }
        //}));

        TreeItemMap.Remove(InItem->GetID());
    }
}

void SXRSceneOutliner::OnChildRemovedFromParent(IXRTreeItem& Parent)
{
    if (/*Parent.Flags.bIsFilteredOut && */!Parent.GetChildren().Num())
    {
        // The parent no longer has any children that match the current search terms. Remove it.
        RemoveItemFromTree(Parent.AsShared());
    }
}

void SXRSceneOutliner::OnItemMoved(const FArmyTreeItemRef& Item)
{
    // This will potentially remove any non-matching, empty parents as well
    RemoveItemFromTree(Item);
}

void SXRSceneOutliner::CreateFolder(const FName& NewFolderName)
{
    if (!CheckWorld())
    {
        return;
    }

    //const FName NewFolderName = FArmyActorFolders::Get().GetDefaultFolderNameForSelection(*RepresentingWorld);
    FArmyActorFolders::Get().CreateFolderContainingSelection(*SharedData->RepresentingWorld, NewFolderName);

    // At this point the new folder will be in our newly added list, so select it and open a rename when it gets refreshed
    NewItemActions.Add(NewFolderName, EXRNewItemAction::Select | EXRNewItemAction::Rename);
}

bool SXRSceneOutliner::IsActorDisplayable(const AActor* Actor) const
{
    return
        (Actor->IsA(AStaticMeshActor::StaticClass()) ||
			Cast<UBlueprintGeneratedClass>(Actor->GetClass())||
			Actor->IsA(AXRShapeActor::StaticClass()) ||
			Actor->IsA(AXRLightActor::StaticClass()) ||
			Actor->IsA(APointLight::StaticClass()) ||
			Actor->IsA(AArmyExtrusionActor::StaticClass())||
			Actor->IsA(AXRActor::StaticClass())) &&
			(!Actor->Tags.Contains("HideInList")) &&
				!Actor->IsPendingKill()&&
				!Actor->GetName().Contains("NOLIST"); 

    //return	!SharedData->bOnlyShowFolders && 												// Don't show actors if we're only showing folders
    //    Actor->IsEditable() &&															// Only show actors that are allowed to be selected and drawn in editor
    //    Actor->IsListedInSceneOutliner() &&
    //    ((SharedData->bRepresentingPlayWorld || !Actor->HasAnyFlags(RF_Transient)) ||
    //    (SharedData->bShowTransient && Actor->HasAnyFlags(RF_Transient)) ||			// Don't show transient actors in non-play worlds
    //        (Actor->ActorHasTag(SequencerActorTag))) &&
    //    !Actor->IsTemplate() &&															// Should never happen, but we never want CDOs displayed
    //    !FActorEditorUtils::IsABuilderBrush(Actor) &&									// Don't show the builder brush
    //    !Actor->IsA(AWorldSettings::StaticClass()) &&									// Don't show the WorldSettings actor, even though it is technically editable
    //    !Actor->IsPendingKill() &&														// We don't want to show actors that are about to go away
    //    FLevelUtils::IsLevelVisible(Actor->GetLevel());								// Only show Actors whose level is visible

    return true;
}

TSharedRef<ITableRow> SXRSceneOutliner::OnGenerateRowForOutlinerTree(FArmyTreeItemPtr Item, const TSharedRef<STableViewBase>& OwnerTable)
{
    return
        SNew(SXRSceneOutlinerTreeRow, OutlinerTreeView.ToSharedRef(), SharedThis(this))
        .Item(Item);
}

void SXRSceneOutliner::OnGetChildrenForOutlinerTree(FArmyTreeItemPtr InParent, TArray<FArmyTreeItemPtr>& OutChildren)
{
    for (auto& WeakChild : InParent->GetChildren())
    {
        auto Child = WeakChild.Pin();
        // Should never have bogus entries in this list
        check(Child.IsValid());
        OutChildren.Add(Child);
    }
}
void SXRSceneOutliner::OnOutlinerTreeDoubleClick(FArmyTreeItemPtr InItem)
{
}
void SXRSceneOutliner::OnOutlinerTreeSelectionChanged(FArmyTreeItemPtr TreeItem, ESelectInfo::Type SelectInfo)
{
    if (SelectInfo == ESelectInfo::Direct)
    {
        return;
    }

    // We only synchronize selection when in actor browsing mode
    if (SharedData->Mode == EXRSceneOutlinerMode::ActorBrowsing)
    {
        if (!bIsReentrant)
        {
            TGuardValue<bool> ReentrantGuard(bIsReentrant, true);

            // @todo outliner: Can be called from non-interactive selection

            // The tree let us know that selection has changed, but wasn't able to tell us
            // what changed.  So we'll perform a full difference check and update the editor's
            // selected actors to match the control's selection set.

            // Make a list of all the actors that should now be selected in the world.
            FArmyItemSelection Selection(*OutlinerTreeView);
            auto SelectedActors = TSet<AActor*>(Selection.GetActorPtrs());

            bool bChanged = false;
            //bool bAnyInPIE = false;
            for (auto* Actor : SelectedActors)
            {
                if (!GXREditor->GetSelectedActors()->IsSelected(Actor))
                {
                    bChanged = true;
                    break;
                }
            }

            for (FSelectionIterator SelectionIt(*GXREditor->GetSelectedActors()); SelectionIt && !bChanged; ++SelectionIt)
            {
                AActor* Actor = CastChecked< AActor >(*SelectionIt);
                //if (!bAnyInPIE && Actor->GetOutermost()->HasAnyPackageFlags(PKG_PlayInEditor))
                //{
                //    bAnyInPIE = true;
                //}
                if (!SelectedActors.Contains(Actor))
                {
                    // Actor has been deselected
                    bChanged = true;

                    //// If actor was a group actor, remove its members from the ActorsToSelect list
                    //AGroupActor* DeselectedGroupActor = Cast<AGroupActor>(Actor);
                    //if (DeselectedGroupActor)
                    //{
                    //    TArray<AActor*> GroupActors;
                    //    DeselectedGroupActor->GetGroupActors(GroupActors);

                    //    for (auto* GroupActor : GroupActors)
                    //    {
                    //        SelectedActors.Remove(GroupActor);
                    //    }
                    //}
                }
            }

            // If there's a discrepancy, update the selected actors to reflect this list.
            if (bChanged)
            {
                //const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "ClickingOnActors", "Clicking on Actors"), !bAnyInPIE);
                //GEditor->GetSelectedActors()->Modify();

                // Clear the selection.
                GXREditor->SelectNone(false, true, true);

                // We'll batch selection changes instead by using BeginBatchSelectOperation()
                GXREditor->GetSelectedActors()->BeginBatchSelectOperation();

                const bool bShouldSelect = true;
                const bool bNotifyAfterSelect = false;
                const bool bSelectEvenIfHidden = true;	// @todo outliner: Is this actually OK?
                for (auto* Actor : SelectedActors)
                {
					//UE_LOG(LogSceneOutliner, Verbose, TEXT("Clicking on Actor (world outliner): %s (%s)"), *Actor->GetClass()->GetName(), *Actor->GetActorLabel());

					/** @梁晓菲 如果Actor被锁定，那么解锁前不允许被选中*/
					if (Actor->bIsSelectable)
					{
						GXREditor->SelectActor(Actor, bShouldSelect, bNotifyAfterSelect, bSelectEvenIfHidden);
					}
                }

                // Commit selection changes
                GXREditor->GetSelectedActors()->EndBatchSelectOperation();

                // Fire selection changed event
                GXREditor->NoteSelectionChange();
            }

            //bActorSelectionDirty = true;
        }
    }
}

void SXRSceneOutliner::OnLevelSelectionChanged(UObject* Obj)
{
    // We only synchronize selection when in actor browsing mode
    if (SharedData->Mode == EXRSceneOutlinerMode::ActorBrowsing)
    {
        if (!bIsReentrant)
        {
            OutlinerTreeView->ClearSelection();
            //bActorSelectionDirty = true;

			int32 NumSelected = GXREditor->GetSelectedActors()->Num();
			// TODO changes this for multiple selection support
			for (int32 SelectionIndex = 0; SelectionIndex < NumSelected; ++SelectionIndex)
			{
				AActor* Actor = Cast<AActor>(GXREditor->GetSelectedActors()->GetSelectedObject(SelectionIndex));
				if (Actor)
				{
					auto TreeItem = TreeItemMap.FindRef(Actor);
					if (TreeItem.IsValid())
					{
						OutlinerTreeView->Private_SetItemSelection(TreeItem, true);
						OutlinerTreeView->RequestScrollIntoView(TreeItem);
					}
					else
					{
						OnItemAdded(Actor, EXRNewItemAction::ScrollIntoView);
					}
				}
			}
        }
    }
}

void SXRSceneOutliner::OnLevelActorsAdded(AActor* InActor)
{
    if (!bIsReentrant)
    {
        if (InActor && SharedData->RepresentingWorld == InActor->GetWorld() && IsActorDisplayable(InActor))
        {
            if (!TreeItemMap.Find(InActor) && !PendingTreeItemMap.Find(InActor))
            {
                // Update the total actor count that match the filters
                //if (Filters->PassesAllFilters(FActorTreeItem(InActor)))
                //{
                //    ApplicableActors.Emplace(InActor);
                //}

                ConstructItemFor<FArmyActorTreeItem>(InActor);
            }
        }
    }
}



void SXRSceneOutliner::OnLevelActorsRemoved(AActor* InActor)
{
    if (!bIsReentrant)
    {
        if (InActor && SharedData->RepresentingWorld == InActor->GetWorld())
        {
            //ApplicableActors.Remove(InActor);
            auto* ItemPtr = TreeItemMap.Find(InActor);
            if (!ItemPtr)
            {
                ItemPtr = PendingTreeItemMap.Find(InActor);
            }

            if (ItemPtr)
            {
                PendingOperations.Emplace(FArmyPendingTreeOperation::Removed, ItemPtr->ToSharedRef());
                Refresh();
            }
        }
    }
}

void SXRSceneOutliner::OnLevelActorFolderChanged(const AActor* InActor, FName OldPath)
{
    auto* ActorTreeItem = TreeItemMap.Find(InActor);
    if (!InActor || !ActorTreeItem)
    {
        return;
    }

    PendingOperations.Emplace(FArmyPendingTreeOperation::Moved, ActorTreeItem->ToSharedRef());
    Refresh();
}
void SXRSceneOutliner::OnViewTypeChanged(int32 InViewType)
{
	static bool VisibleFlag = true;
	if (InViewType == EXRLevelViewportType::LVT_OrthoXY)
	{
		FArmyTreeItemPtr Item = TreeItemMap.FindRef(FArmyTreeItemID(FName(TEXT("顶面"))));
		if (Item.IsValid())
		{
			const FArmySetVisibilityVisitor Visitor(false);
			Item->Visit(Visitor);
			VisibleFlag = false;
		}
	}
	else if(!VisibleFlag)
	{
		FArmyTreeItemPtr Item = TreeItemMap.FindRef(FArmyTreeItemID(FName(TEXT("顶面"))));
		if (Item.IsValid())
		{
			const FArmySetVisibilityVisitor Visitor(true);
			Item->Visit(Visitor);
		}
		VisibleFlag = true;
	}

}
void SXRSceneOutliner::CloseExpander(const FName& InName, bool InClose, bool InVisible, bool InSelectable)
{
	FArmyTreeItemPtr Item = TreeItemMap.FindRef(FArmyTreeItemID(InName));
	if (Item.IsValid())
	{
		Item->CloseExpander = InClose;
		if (InClose)
		{
			SetItemExpansionRecursive(Item, false);
		}

		const FArmySetVisibilityVisitor Visitor(InVisible, InSelectable,true);
		Item->Visit(Visitor);
	}
}
void SXRSceneOutliner::SetupColumns(SHeaderRow& HeaderRow)
{
    FArmySceneOutlinerModule& SceneOutlinerModule = FModuleManager::LoadModuleChecked<FArmySceneOutlinerModule>("ArmySceneOutliner");

    TSharedPtr<IXRSceneOutlinerColumn> Column;

    if (SharedData->ColumnMap.Num() == 0)
    {
        SharedData->UseDefaultColumns();
    }

    Columns.Empty(SharedData->ColumnMap.Num());
    HeaderRow.ClearColumns();

    // Get a list of sorted columns IDs to create
    TArray<FName> SortedIDs;
    SortedIDs.Reserve(SharedData->ColumnMap.Num());
    SharedData->ColumnMap.GenerateKeyArray(SortedIDs);

    SortedIDs.Sort([&](const FName& A, const FName& B) {
        return SharedData->ColumnMap[A].PriorityIndex < SharedData->ColumnMap[B].PriorityIndex;
    });

    for (const FName& ID : SortedIDs)
    {
        if (SharedData->ColumnMap[ID].Visibility == EXRColumnVisibility::Invisible)
        {
            continue;
        }

        TSharedPtr<IXRSceneOutlinerColumn> Column;

        if (SharedData->ColumnMap[ID].Factory.IsBound())
        {
            Column = SharedData->ColumnMap[ID].Factory.Execute(*this);
        }
        else
        {
            Column = SceneOutlinerModule.FactoryColumn(ID, *this);
        }

        if (ensure(Column.IsValid()))
        {
            check(Column->GetColumnID() == ID);
            Columns.Add(Column->GetColumnID(), Column);

            auto ColumnArgs = Column->ConstructHeaderRowColumn();

            //if (Column->SupportsSorting())
            //{
            //    ColumnArgs
            //        .SortMode(this, &SSceneOutliner::GetColumnSortMode, Column->GetColumnID())
            //        .OnSort(this, &SSceneOutliner::OnColumnSortModeChanged);
            //}

            HeaderRow.AddColumn(ColumnArgs);
        }
    }

    Columns.Shrink();
}

void SXRSceneOutliner::ScrollItemIntoView(FArmyTreeItemPtr Item)
{
    auto Parent = Item->GetParent();
    while (Parent.IsValid())
    {
        OutlinerTreeView->SetItemExpansion(Parent->AsShared(), true);
        Parent = Parent->GetParent();
    }

    OutlinerTreeView->RequestScrollIntoView(Item);
}

void SXRSceneOutliner::SetItemExpansionRecursive(FArmyTreeItemPtr Model, bool bInExpansionState)
{
    if (Model.IsValid())
    {
        OutlinerTreeView->SetItemExpansion(Model, bInExpansionState);
        for (auto& Child : Model->GetChildren())
        {
            if (Child.IsValid())
            {
                SetItemExpansionRecursive(Child.Pin(), bInExpansionState);
            }
        }
    }
}