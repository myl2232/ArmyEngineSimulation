#include "CoreMinimal.h"
#include "ArmyEditorEngine.h"
#include "UnrealExporter.h"
#include "ArmyGroupActor.h"
#include "Engine/Selection.h"
#include "Exporters/Exporter.h"
#include "HAL/PlatformApplicationMisc.h"
#include "ArmyLevelFactory.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyTypes.h"
#include "ArmySceneData.h"
/*-----------------------------------------------------------------------------
Actor adding/deleting functions.
-----------------------------------------------------------------------------*/

class FSelectedActorExportObjectInnerContext : public FExportObjectInnerContext
{
public:
    FSelectedActorExportObjectInnerContext()
        //call the empty version of the base class
        : FExportObjectInnerContext(false)
    {
        // For each object . . .
        for (UObject* InnerObj : TObjectRange<UObject>(RF_ClassDefaultObject, true, EInternalObjectFlags::PendingKill))
        {
            UObject* OuterObj = InnerObj->GetOuter();

            //assume this is not part of a selected actor
            bool bIsChildOfSelectedActor = false;

            UObject* TestParent = OuterObj;
            while (TestParent)
            {
                AActor* TestParentAsActor = Cast<AActor>(TestParent);
                if (TestParentAsActor && TestParentAsActor->IsSelected())
                {
                    bIsChildOfSelectedActor = true;
                    break;
                }
                TestParent = TestParent->GetOuter();
            }

            if (bIsChildOfSelectedActor)
            {
                InnerList* Inners = ObjectToInnerMap.Find(OuterObj);
                if (Inners)
                {
                    // Add object to existing inner list.
                    Inners->Add(InnerObj);
                }
                else
                {
                    // Create a new inner list for the outer object.
                    InnerList& InnersForOuterObject = ObjectToInnerMap.Add(OuterObj, InnerList());
                    InnersForOuterObject.Add(InnerObj);
                }
            }
        }
    }
};

void UArmyEditorEngine::edactCopySelected(UWorld* InWorld, FString* DestinationData /*= NULL*/)
{
    // 	if (GetSelectedComponentCount() > 0)
    // 	{
    // 		// Copy components
    // 		TArray<UActorComponent*> SelectedComponents;
    // 		for (FSelectedEditableComponentIterator It(GetSelectedEditableComponentIterator()); It; ++It)
    // 		{
    // 			SelectedComponents.Add(CastChecked<UActorComponent>(*It));
    // 		}
    // 
    // 		FComponentEditorUtils::CopyComponents(SelectedComponents);
    // 	}
    // 	else
    {
        // Copy Actors
        // Before copying, deselect:
        //		- Actors belonging to prefabs unless all actors in the prefab are selected.
        //		- Builder brushes.
        //      - World Settings.

        TArray<AActor*> ActorsToDeselect;

        // 		bool bSomeSelectedActorsNotInCurrentLevel = false;
        // 		for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
        // 		{
        // 			AActor* Actor = static_cast<AActor*>(*It);
        // 			checkSlow(Actor->IsA(AActor::StaticClass()));
        // 
        // 			// Deselect any selected builder brushes.
        // 			ABrush* Brush = Cast< ABrush >(Actor);
        // 			const bool bActorIsBuilderBrush = (Brush && FActorEditorUtils::IsABuilderBrush(Brush));
        // 			if (bActorIsBuilderBrush)
        // 			{
        // 				ActorsToDeselect.Add(Actor);
        // 			}
        // 
        // 			// Deselect world settings
        // 			if (Actor->IsA(AWorldSettings::StaticClass()))
        // 			{
        // 				ActorsToDeselect.Add(Actor);
        // 			}
        // 
        // 			// If any selected actors are not in the current level, warn the user that some actors will not be copied.
        // 			if (!bSomeSelectedActorsNotInCurrentLevel && !Actor->GetLevel()->IsCurrentLevel())
        // 			{
        // 				bSomeSelectedActorsNotInCurrentLevel = true;
        // 				FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("UnrealEd", "CopySelectedActorsInNonCurrentLevel", "Some selected actors are not in the current level and will not be copied."));
        // 			}
        // 		}

                //const FScopedBusyCursor BusyCursor;
        for (int32 ActorIndex = 0; ActorIndex < ActorsToDeselect.Num(); ++ActorIndex)
        {
            AActor* Actor = ActorsToDeselect[ActorIndex];
            GetSelectedActors()->Deselect(Actor);
        }

        // Export the actors.
        FStringOutputDevice Ar;
        const FSelectedActorExportObjectInnerContext Context;
        UExporter::ExportToOutputDevice(&Context, InWorld, NULL, Ar, TEXT("copy"), 0, PPF_DeepCompareInstances | PPF_ExportsNotFullyQualified);
        FPlatformApplicationMisc::ClipboardCopy(*Ar);
        if (DestinationData)
        {
            *DestinationData = MoveTemp(Ar);
        }
    }
}

/**
* Creates offsets for locations based on the editor grid size and active viewport.
*/
static FVector CreateLocationOffset(bool bDuplicate, bool bOffsetLocations)
{
    const float Offset = static_cast<float>(bOffsetLocations ? 5.f : 0);
    FVector LocationOffset(Offset, Offset, 0.f);
    if (bDuplicate /*&& GCurrentLevelEditingViewportClient*/)
    {
        //switch (GCurrentLevelEditingViewportClient->ViewportType)
        //{
        //case EArmyLevelViewportType::LVT_OrthoXZ:
        //	LocationOffset = FVector(Offset, 0.f, Offset);
        //	break;
        //case EArmyLevelViewportType::LVT_OrthoYZ:
        //	LocationOffset = FVector(0.f, Offset, Offset);
        //	break;
        //default:
        LocationOffset = FVector(Offset, 0.f, 0.f);
        //	break;
        //}
    }
    return LocationOffset;
}

void UArmyEditorEngine::edactPasteSelected(UWorld* InWorld, bool bDuplicate, bool bOffsetLocations, bool bWarnIfHidden, FString* SourceData /*= NULL*/)
{
    UArmyEditorViewportClient* VC = Cast<UArmyEditorViewportClient>(InWorld->GetGameViewport());
    TArray<UObject*> Objects;
    //check and warn if the user is trying to paste to a hidden level. This will return if he wishes to abort the process
// 	if (bWarnIfHidden && WarnIfDestinationLevelIsHidden(InWorld) == true)
// 	{
// 		return;
// 	}

// 	if (GetSelectedComponentCount() > 0)
// 	{
// 		auto SelectedActor = CastChecked<AActor>(*GetSelectedActorIterator());
// 
// 		TArray<UActorComponent*> PastedComponents;
// 		FComponentEditorUtils::PasteComponents(PastedComponents, SelectedActor, SelectedActor->GetRootComponent());
// 
// 		if (PastedComponents.Num() > 0)
// 		{
// 			// Make sure all the SCS trees have a chance to update
// 			FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
// 			LevelEditor.BroadcastComponentsEdited();
// 
// 			// Select the new clones
// 			USelection* ComponentSelection = GetSelectedComponents();
// 			ComponentSelection->Modify(false);
// 			ComponentSelection->BeginBatchSelectOperation();
// 			ComponentSelection->DeselectAll();
// 
// 			for (auto PastedComp : PastedComponents)
// 			{
// 				GEditor->SelectComponent(PastedComp, true, false);
// 			}
// 
// 			ComponentSelection->EndBatchSelectOperation(true);
// 		}
// 	}
// 	else
    {
        //const FScopedBusyCursor BusyCursor;

        // Create a location offset.
        const FVector LocationOffset = CreateLocationOffset(bDuplicate, bOffsetLocations);

        //FCachedActorLabels ActorLabels(InWorld);

        // Transact the current selection set.
        USelection* SelectedActors = GetSelectedActors();
        SelectedActors->Modify();

        // Get pasted text.
        FString PasteString;
        if (SourceData)
        {
            PasteString = *SourceData;
        }
        else
        {
            FPlatformApplicationMisc::ClipboardPaste(PasteString);
        }
        const TCHAR* Paste = *PasteString;

        // Import the actors.
        UArmyLevelFactory* Factory = NewObject<UArmyLevelFactory>();
        Factory->FactoryCreateText(ULevel::StaticClass(), InWorld->GetCurrentLevel(), InWorld->GetCurrentLevel()->GetFName(), RF_Transactional, NULL, bDuplicate ? TEXT("move") : TEXT("paste"), Paste, Paste + FCString::Strlen(Paste), GWarn);

        // Fire ULevel::LevelDirtiedEvent when falling out of scope.
        FScopedLevelDirtied			LevelDirtyCallback;
		TArray<AActor*> SeletecdActors;
		GArmyEditor->GetSelectedActors(SeletecdActors);
        // Update the actors' locations and update the global list of visible layers.
        for (int32 i = 0; i<SeletecdActors.Num(); i++)
        {
            AActor* Actor = SeletecdActors[i];
            checkSlow(Actor->IsA(AActor::StaticClass()));
            Objects.Add(Actor);
            // We only want to offset the location if this actor is the root of a selected attachment hierarchy
            // Offsetting children of an attachment hierarchy would cause them to drift away from the node they're attached to
            // as the offset would effectively get applied twice
            const AActor* const ParentActor = Actor->GetAttachParentActor();
            const FVector& ActorLocationOffset = (ParentActor && ParentActor->IsSelected()) ? FVector::ZeroVector : LocationOffset;

            // Offset the actor's location.
            FVector Loc = Actor->GetActorLocation();

			if (OnActorDuplicated.IsBound())
			{
				
				AActor* Owner = Actor->GetOwner();
				if (Owner && Owner->IsValidLowLevel())
				{
					Owner->Children.AddUnique(Actor);
				}
				OnActorDuplicated.Broadcast(Actor);
			}
            //Actor->TeleportTo(Actor->GetActorLocation() + ActorLocationOffset, Actor->GetActorRotation(), false, true);
            //OnLevelActorAdded.Broadcast(Actor);

            // Re-label duplicated actors so that labels become unique
            //FActorLabelUtilities::SetActorLabelUnique(Actor, Actor->GetActorLabel(), &ActorLabels);
            //ActorLabels.Add(Actor->GetActorLabel());

            //GEditor->Layers->InitializeNewActorLayers(Actor);

            // Ensure any layers this actor belongs to are visible
            //GEditor->Layers->SetLayersVisibility(Actor->Layers, true);

            //Actor->CheckDefaultSubobjects();
            //Actor->InvalidateLightingCache();
            // Call PostEditMove to update components, etc.
            //Actor->PostEditMove(true);
            //Actor->PostDuplicate(false);
            //Actor->CheckDefaultSubobjects();

            // Request saves/refreshes.
            //Actor->MarkPackageDirty();
            //LevelDirtyCallback.Request();
        }
        // Note the selection change.  This will also redraw level viewports and update the pivot.
        NoteSelectionChange();
		
    }

    // 按住Shift等轴复制时不需要走此逻辑
    if (VC && !VC->IsShiftPressed())
    {
        VC->SetDroppedObjects(Objects);
        VC->Invalidate();
    }
}

void UArmyEditorEngine::edactDuplicateSelected(ULevel* InLevel, bool bOffsetLocations)
{
	
	{
		FString ScratchData;

		GArmyEditor->edactCopySelected(InLevel->OwningWorld,&ScratchData);
		GArmyEditor->edactPasteSelected(InLevel->OwningWorld,true,bOffsetLocations,true,&ScratchData);
	}



    //  	auto NumSelectedComponents = GetSelectedComponentCount();
    //  	if (NumSelectedComponents > 0)
    //  	{
    //  		TArray<UActorComponent*> NewComponentClones;
    //  		NewComponentClones.Reserve(NumSelectedComponents);
    //  
    //  		// Duplicate selected components if they are an Instance component
    //  		for (FSelectedEditableComponentIterator It(GetSelectedEditableComponentIterator()); It; ++It)
    //  		{
    //  			auto Component = CastChecked<UActorComponent>(*It);
    //  			if (Component->CreationMethod == EComponentCreationMethod::Instance)
    //  			{
    //  				UActorComponent* Clone = FComponentEditorUtils::DuplicateComponent(Component);
    //  				if (Clone)
    //  				{
    //  					NewComponentClones.Add(Clone);
    //  				}
    //  			}
    //  		}
    //  
    //  		if (NewComponentClones.Num() > 0)
    //  		{
    //  			// Make sure all the SCS trees have a chance to update
    //  			FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    //  			LevelEditor.BroadcastComponentsEdited();
    //  
    //  			// Select the new clones
    //  			USelection* ComponentSelection = GetSelectedComponents();
    //  			ComponentSelection->Modify(false);
    //  			ComponentSelection->BeginBatchSelectOperation();
    //  			ComponentSelection->DeselectAll();
    //  
    //  			for (auto Clone : NewComponentClones)
    //  			{
    //  				GEditor->SelectComponent(Clone, true, false);
    //  			}
    //  
    //  			ComponentSelection->EndBatchSelectOperation(true);
    //  		}
    //  	}
    //  	else



    //  	{
    //  		using namespace DuplicateSelectedActors;
    //  
    //  		//const FScopedBusyCursor BusyCursor;
    //  		GetSelectedActors()->Modify();
    //  
    //  		// Create per-level job lists.
    //  		typedef TMap<ULevel*, FDuplicateJob*>	DuplicateJobMap;
    //  		DuplicateJobMap							DuplicateJobs;
    //  
    //  		// Build set of selected actors before duplication
    //  		TArray<AActor*> PreDuplicateSelection;
    //  
    //  		// Add selected actors to the per-level job lists.
    //  		bool bHaveActorLocation = false;
    //  		FVector AnyActorLocation = FVector::ZeroVector;
    //  		for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
    //  		{
    //  			AActor* Actor = static_cast<AActor*>(*It);
    //  			checkSlow(Actor->IsA(AActor::StaticClass()));
    //  
    //  			if (!bHaveActorLocation)
    //  			{
    //  				bHaveActorLocation = true;
    //  				AnyActorLocation = Actor->GetActorLocation();
    //  			}
    //  
    //  			PreDuplicateSelection.Add(Actor);
    //  
    //  			ULevel* OldLevel = Actor->GetLevel();
    //  			FDuplicateJob** Job = DuplicateJobs.Find(OldLevel);
    //  			if (Job)
    //  			{
    //  				(*Job)->Actors.Add(Actor);
    //  			}
    //  			else
    //  			{
    //  				// Allocate a new job for the level.
    //  				FDuplicateJob* NewJob = new FDuplicateJob;
    //  				NewJob->SrcLevel = OldLevel;
    //  				NewJob->Actors.Add(Actor);
    //  				DuplicateJobs.Add(OldLevel, NewJob);
    //  			}
    //  		}
    //  
    //  		UWorld* World = InLevel->OwningWorld;
    //  		ULevel* DesiredLevel = InLevel;
    //  
    //  		USelection* SelectedActors = GetSelectedActors();
    //  		SelectedActors->BeginBatchSelectOperation();
    //  		SelectedActors->Modify();
    //  
    //  		// For each level, select the actors in that level and copy-paste into the destination level.
    //  		TArray<AActor*>	NewActors;
    //  		for (DuplicateJobMap::TIterator It(DuplicateJobs); It; ++It)
    //  		{
    //  			FDuplicateJob* Job = It.Value();
    //  			check(Job);
    //  			Job->DuplicateActorsToLevel(NewActors, InLevel, bOffsetLocations);
    //  		}
    //  
    //  		// Select any newly created actors and prefabs.
    //  		SelectNone(false, true);
    //  		for (int32 ActorIndex = 0; ActorIndex < NewActors.Num(); ++ActorIndex)
    //  		{
    //  			AActor* Actor = NewActors[ActorIndex];
    //  			SelectActor(Actor, true, false);
    //  		}
    //  		SelectedActors->EndBatchSelectOperation();
    //  		NoteSelectionChange();
    //  
    //  		// Finally, cleanup.
    //  		for (DuplicateJobMap::TIterator It(DuplicateJobs); It; ++It)
    //  		{
    //  			FDuplicateJob* Job = It.Value();
    //  			delete Job;
    //  		}
    //  
    //  		// Build set of selected actors after duplication
    // //  		TArray<AActor*> PostDuplicateSelection;
    // //  		for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
    // //  		{
    // //  			AActor* Actor = static_cast<AActor*>(*It);
    // //  			checkSlow(Actor->IsA(AActor::StaticClass()));
    // //  
    // //  			// We generate new seeds when we duplicate
    // //  			Actor->SeedAllRandomStreams();
    // //  
    // //  			PostDuplicateSelection.Add(Actor);
    // //  		}
    // //  
    // //  		TArray<FEdMode*> ActiveModes;
    // //  		GLevelEditorModeTools().GetActiveModes(ActiveModes);
    // //  
    // //  		for (int32 ModeIndex = 0; ModeIndex < ActiveModes.Num(); ++ModeIndex)
    // //  		{
    // //  			// Tell the tools about the duplication
    // //  			ActiveModes[ModeIndex]->ActorsDuplicatedNotify(PreDuplicateSelection, PostDuplicateSelection, bOffsetLocations);
    // //  		}
    //  
}

bool UArmyEditorEngine::edactDeleteSelected(UWorld* InWorld, bool bVerifyDeletionCanHappen /*= true*/)
{
    // 	if (bVerifyDeletionCanHappen)
    // 	{
    // 		// Provide the option to abort the delete
    // 		if (ShouldAbortActorDeletion())
    // 		{
    // 			return false;
    // 		}
    // 	}
    //
    //	const double StartSeconds = FPlatformTime::Seconds();
    //
    // 	if (GetSelectedComponentCount() > 0)
    // 	{
    // 		TArray<UActorComponent*> SelectedEditableComponents;
    // 		for (FSelectedEditableComponentIterator It(GetSelectedEditableComponentIterator()); It; ++It)
    // 		{
    // 			SelectedEditableComponents.Add(CastChecked<UActorComponent>(*It));
    // 		}
    // 
    // 		if (SelectedEditableComponents.Num() > 0)
    // 		{
    // 			// Modify the actor that owns the selected components
    // 			check(GetSelectedActorCount() == 1);
    // 			(*GetSelectedActorIterator())->Modify();
    // 
    // 			// Delete the selected components
    // 			UActorComponent* ComponentToSelect = nullptr;
    // 			int32 NumDeletedComponents = FComponentEditorUtils::DeleteComponents(SelectedEditableComponents, ComponentToSelect);
    // 
    // 			if (NumDeletedComponents > 0)
    // 			{
    // 				// Make sure all the SCS trees have a chance to rebuild
    // 				FLevelEditorModule& LevelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    // 				LevelEditor.BroadcastComponentsEdited();
    // 
    // 				// Update the editor component selection if possible
    // 				if (ComponentToSelect)
    // 				{
    // 					USelection* ComponentSelection = GetSelectedComponents();
    // 					ComponentSelection->Modify(false);
    // 					ComponentSelection->BeginBatchSelectOperation();
    // 					ComponentSelection->DeselectAll();
    // 
    // 					GEditor->SelectComponent(ComponentToSelect, true, false);
    // 
    // 					// Make sure the selection changed event fires so the SCS trees can update their selection
    // 					ComponentSelection->MarkBatchDirty();
    // 					ComponentSelection->EndBatchSelectOperation(true);
    // 				}
    // 
    // 				UE_LOG(LogEditorActor, Log, TEXT("Deleted %d Components (%3.3f secs)"), NumDeletedComponents, FPlatformTime::Seconds() - StartSeconds);
    // 				return true;
    // 			}
    // 		}
    // 
    // 		return false;
    // 	}

    GetSelectedActors()->Modify();

    // Fire ULevel::LevelDirtiedEvent when falling out of scope.
    FScopedLevelDirtied			LevelDirtyCallback;

    // Get a list of all the deletable actors in the selection
    TArray<AActor*> ActorsToDelete;
    CanDeleteSelectedActors(InWorld, false, true, &ActorsToDelete);

    // Maintain a list of levels that have already been Modify()'d so that each level
    // is modify'd only once.
    //TArray<ULevel*> LevelsAlreadyModified;
    // A list of levels that will need their Bsp updated after the deletion is complete
    //TArray<ULevel*> LevelsToRebuild;

    //bool	bBrushWasDeleted = false;
    //bool	bRequestedDeleteAllByLevel = false;
    //bool	bRequestedDeleteAllByActor = false;
    //EAppMsgType::Type MessageType = ActorsToDelete.Num() > 1 ? EAppMsgType::YesNoYesAllNoAll : EAppMsgType::YesNo;
    //int32		DeleteCount = 0;

    USelection* SelectedActors = GetSelectedActors();

    for (int32 ActorIndex = 0; ActorIndex < ActorsToDelete.Num(); ++ActorIndex)
    {
        AActor* Actor = ActorsToDelete[ActorIndex];

        //If actor is referenced by script, ask user if they really want to delete
        //ULevelScriptBlueprint* LSB = Actor->GetLevel()->GetLevelScriptBlueprint(true);
        //TArray<AActor*> ReferencingActors;
        //TArray<UClass*> ClassTypesToIgnore;
        //ClassTypesToIgnore.Add(ALevelScriptActor::StaticClass());
        //FBlueprintEditorUtils::FindActorsThatReferenceActor(Actor, ClassTypesToIgnore, ReferencingActors);

        //bool bReferencedByLevelScript = (NULL != LSB && FBlueprintEditorUtils::FindNumReferencesToActorFromLevelScript(LSB, Actor) > 0);
        //bool bReferencedByActor = false;
        //for (AActor* ReferencingActor : ReferencingActors)
        //{
        //	if (ReferencingActor->ParentComponentActor.Get() != Actor)
        //	{
        //		bReferencedByActor = true;
        //		break;
        //	}
        //}

// 		if (bReferencedByLevelScript || bReferencedByActor)
// 		{
// 			if ((bReferencedByLevelScript && !bRequestedDeleteAllByLevel) ||
// 				(bReferencedByActor && !bRequestedDeleteAllByActor))
// 			{
// 				FText ConfirmDelete;
// 				if (bReferencedByLevelScript && bReferencedByActor)
// 				{
// 					ConfirmDelete = FText::Format(LOCTEXT("ConfirmDeleteActorReferenceByScriptAndActor",
// 						"Actor {0} is referenced by the level blueprint and another Actor, do you really want to delete it?"),
// 						FText::FromString(Actor->GetName()));
// 				}
// 				else if (bReferencedByLevelScript)
// 				{
// 					ConfirmDelete = FText::Format(LOCTEXT("ConfirmDeleteActorReferencedByScript",
// 						"Actor {0} is referenced by the level blueprint, do you really want to delete it?"),
// 						FText::FromString(Actor->GetName()));
// 				}
// 				else
// 				{
// 					ConfirmDelete = FText::Format(LOCTEXT("ConfirmDeleteActorReferencedByActor",
// 						"Actor {0} is referenced by another Actor, do you really want to delete it?"),
// 						FText::FromString(Actor->GetName()));
// 				}
// 
// 				int32 Result = FMessageDialog::Open(MessageType, ConfirmDelete);
// 				if (Result == EAppReturnType::YesAll)
// 				{
// 					bRequestedDeleteAllByLevel = bReferencedByLevelScript;
// 					bRequestedDeleteAllByActor = bReferencedByActor;
// 				}
// 				else if (Result == EAppReturnType::NoAll)
// 				{
// 					break;
// 				}
// 				else if (Result == EAppReturnType::No || Result == EAppReturnType::Cancel)
// 				{
// 					continue;
// 				}
// 			}
// 
// 			if (bReferencedByLevelScript)
// 			{
// 				FBlueprintEditorUtils::ModifyActorReferencedGraphNodes(LSB, Actor);
// 			}
// 			if (bReferencedByActor)
// 			{
// 				for (int32 ReferencingActorIndex = 0; ReferencingActorIndex < ReferencingActors.Num(); ReferencingActorIndex++)
// 				{
// 					ReferencingActors[ReferencingActorIndex]->Modify();
// 				}
// 			}
// 		}

// 		ABrush* Brush = Cast< ABrush >(Actor);
// 		if (Brush && !FActorEditorUtils::IsABuilderBrush(Brush)) // Track whether or not a brush actor was deleted.
// 		{
// 			bBrushWasDeleted = true;
// 			ULevel* BrushLevel = Actor->GetLevel();
// 			if (BrushLevel)
// 			{
// 				LevelsToRebuild.AddUnique(BrushLevel);
// 			}
// 		}
        // If the actor about to be deleted is in a group, be sure to remove it from the group
        AArmyGroupActor* ActorParentGroup = AArmyGroupActor::GetParentForActor(Actor);
        if (ActorParentGroup)
        {
            ActorParentGroup->Remove(*Actor);
        }

        // Mark the actor's level as dirty.
        //Actor->MarkPackageDirty();
        //LevelDirtyCallback.Request();

        // Deselect the Actor.
        SelectedActors->Deselect(Actor);

        // Modify the level.  Each level is modified only once.
        // @todo DB: Shouldn't this be calling UWorld::ModifyLevel?
        //ULevel* Level = Actor->GetLevel();
        //if (LevelsAlreadyModified.Find(Level) == INDEX_NONE)
        //{
        //	LevelsAlreadyModified.Add(Level);
        //	Level->Modify();
    //	}

        //UE_LOG(LogEditorActor, Log, TEXT("Deleted Actor: %s"), *Actor->GetClass()->GetName());

        // Destroy actor and clear references.
        //GEditor->Layers->DisassociateActorFromLayers(Actor);
        //bool WasDestroyed = Actor->GetWorld()->EditorDestroyActor(Actor, false);
        InWorld->DestroyActor(Actor, false, false);
		/** @欧石楠 删除该Actor关联的施工项*/
		FArmySceneData::Get()->DeleteActorConstructionItemData(Actor->GetUniqueID());
        OnLevelActorRemoved.ExecuteIfBound(Actor);
        //checkf(WasDestroyed, TEXT("Failed to destroy Actor %s (%s)"), *Actor->GetClass()->GetName(), *Actor->GetActorLabel());

        //DeleteCount++;
    }

    // Remove all references to destroyed actors once at the end, instead of once for each Actor destroyed..
    //CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    GArmyEditor->SelectNone(true, true);
    NoteSelectionChange();
    // If any brush actors were deleted, update the Bsp in the appropriate levels
// 	if (bBrushWasDeleted)
// 	{
// 		FlushRenderingCommands();
// 
// 		for (int32 LevelIdx = 0; LevelIdx < LevelsToRebuild.Num(); ++LevelIdx)
// 		{
// 			GEditor->RebuildLevel(*(LevelsToRebuild[LevelIdx]));
// 		}
// 
// 		RedrawLevelEditingViewports();
// 		ULevel::LevelDirtiedEvent.Broadcast();
// 	}

    //UE_LOG(LogEditorActor, Log, TEXT("Deleted %d Actors (%3.3f secs)"), DeleteCount, FPlatformTime::Seconds() - StartSeconds);

    return true;
}

bool UArmyEditorEngine::CanDeleteSelectedActors(const UWorld* InWorld, const bool bStopAtFirst, const bool bLogUndeletable, TArray<AActor*>* OutDeletableActors /*= NULL*/) const
{
    bool bContainsDeletable = false;
    for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
    {
        AActor* Actor = static_cast<AActor*>(*It);
        checkSlow(Actor->IsA(AActor::StaticClass()));

        //if (Actor->HasAllFlags(RF_Transactional))
        /**@欧石楠 有tag才让删除*/
        if (Actor->Tags.Num() > 0 && !Actor->Tags.Contains(TEXT("AreaHighLight")) 
			&& !Actor->Tags.Contains(TEXT("RectAreaHightActor"))
			&&!Actor->Tags.Contains(TEXT("CanNotDelete")))
        {
            OutDeletableActors->Add(Actor);
            bContainsDeletable = true;
        }
    }
    return bContainsDeletable;
}

