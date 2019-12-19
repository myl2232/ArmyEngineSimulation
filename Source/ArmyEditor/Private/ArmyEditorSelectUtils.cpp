#include "ArmyEditorEngine.h"
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
//#include "Editor/GroupActor.h"
#include "Components/ChildActorComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/Selection.h"
#include "ArmyGroupActor.h"
#include "Engine/LevelStreaming.h"

#define LOCTEXT_NAMESPACE "ArmyEditorSelectUtils"

void UArmyEditorEngine::SetActorSelectionFlags(AActor* InActor)
{
	TInlineComponentArray<UActorComponent*> Components;
	InActor->GetComponents(Components);

	//for every component in the actor
	for (int32 ComponentIndex = 0; ComponentIndex < Components.Num(); ComponentIndex++)
	{
		UActorComponent* Component = Components[ComponentIndex];
		if (Component->IsRegistered())
		{
			// If we have a 'child actor' component, want to update its visible selection state
			UChildActorComponent* ChildActorComponent = Cast<UChildActorComponent>(Component);
			if (ChildActorComponent != NULL && ChildActorComponent->GetChildActor() != NULL)
			{
				SetActorSelectionFlags(ChildActorComponent->GetChildActor());
			}

			UPrimitiveComponent* PrimComponent = Cast<UPrimitiveComponent>(Component);
			if (PrimComponent != NULL && PrimComponent->IsRegistered())
			{
				PrimComponent->PushSelectionToProxy();
			}

			UDecalComponent* DecalComponent = Cast<UDecalComponent>(Component);
			if (DecalComponent != NULL)// && DecalComponent->IsRegistered())
			{
				DecalComponent->PushSelectionToProxy();
			}
		}
	}
}

void UArmyEditorEngine::SelectActor(AActor* Actor, bool bInSelected, bool bNotify, bool bSelectEvenIfHidden, bool bForceRefresh)
{
    if (!Actor || !Actor->IsValidLowLevel())
    {
        return;
    }

	//欧石楠 添加过滤功能
	if (Actor && Actor->IsValidLowLevel() && bInSelected && isUsingSelectFilter())
	{
		if (!CanSelectFilter.Execute(Actor))
		{
			return;
		}
	}

	//@欧石楠 临时
    // 标记为不能选中的Actor需跳过选中逻辑 
    if (Actor && !Actor->IsValidLowLevel() && !Actor->bIsSelectable)
    {
        return;
    }

	const bool bWarnIfLevelLocked = true;
	//if (!CanSelectActor(Actor, bInSelected, bSelectEvenIfHidden, bWarnIfLevelLocked))
	//{
	//	return;
	//}

	bool bSelectionHandled = false;

	//TArray<FEdMode*> ActiveModes;
	//GArmyLevelEditorModeTools().GetActiveModes(ActiveModes);
	/*for (int32 ModeIndex = 0; ModeIndex < ActiveModes.Num(); ++ModeIndex)
	{
		bSelectionHandled |= ActiveModes[ModeIndex]->Select(Actor, bInSelected);
	}*/

	// Select the actor and update its internals.
	if (!bSelectionHandled)
	{
		if (bInSelected)
		{
			// If trying to select an Actor spawned by a ChildACtorComponent, instead select Actor that spawned us
			if (UChildActorComponent* ParentComponent = Actor->GetParentComponent())
			{
				Actor = ParentComponent->GetOwner();
			}
		}

		//if (GEditor->bGroupingActive)
		{
			// if this actor is a group, do a group select/deselect
			AArmyGroupActor* SelectedGroupActor = Cast<AArmyGroupActor>(Actor);
			if (SelectedGroupActor)
			{
				SelectGroup(SelectedGroupActor, true, bInSelected, bNotify);
			}
			else
			{
				// Select/Deselect this actor's entire group, starting from the top locked group.
				// If none is found, just use the actor.
				AArmyGroupActor* ActorLockedRootGroup = AArmyGroupActor::GetRootForActor(Actor, true);
				if (ActorLockedRootGroup)
				{
					SelectGroup(ActorLockedRootGroup, false, bInSelected, bNotify);
				}
			}
		}

		// Don't do any work if the actor's selection state is already the selected state.
		const bool bActorSelected = Actor->IsSelected();
		if ((bActorSelected && !bInSelected) || (!bActorSelected && bInSelected))
		{
			if (bInSelected)
			{
				//UE_LOG(LogEditorSelectUtils, Verbose, TEXT("Selected Actor: %s"), *Actor->GetClass()->GetName());
			}
			else
			{
				//UE_LOG(LogEditorSelectUtils, Verbose, TEXT("Deselected Actor: %s"), *Actor->GetClass()->GetName());
			}

			GetSelectedActors()->Select(Actor, bInSelected);
			/*if (!bInSelected)
			{
				if (GetSelectedComponentCount() > 0)
				{
					GetSelectedComponents()->Modify();
				}*/

			//	GetSelectedComponents()->BeginBatchSelectOperation();
			//	for (UActorComponent* Component : Actor->GetComponents())
			//	{
			//		GetSelectedComponents()->Deselect(Component);

			//		// Remove the selection override delegates from the deselected components
			//		auto SceneComponent = Cast<USceneComponent>(Component);
			//		FComponentEditorUtils::BindComponentSelectionOverride(SceneComponent, false);
			//	}
			//	GetSelectedComponents()->EndBatchSelectOperation(false);
			//}
			//else
			//{
			//	// Bind the override delegates for the components in the selected actor
			//	for (UActorComponent* Component : Actor->GetComponents())
			//	{
			//		auto SceneComponent = Cast<USceneComponent>(Component);
			//		FComponentEditorUtils::BindComponentSelectionOverride(SceneComponent, true);
			//	}
			//}

			//A fast path to mark selection rather than reconnecting ALL components for ALL actors that have changed state
			SetActorSelectionFlags(Actor);

			if (bNotify)
			{
				NoteSelectionChange();
			}

			//whenever selection changes, recompute whether the selection contains a locked actor
			//bCheckForLockActors = true;

			//whenever selection changes, recompute whether the selection contains a world info actor
			//bCheckForWorldSettingsActors = true;
		}
		else
		{
			//if (bNotify || bForceRefresh)
			//{
			//	//reset the property windows.  In case something has changed since previous selection
			//	UpdateFloatingPropertyWindows(bForceRefresh);
			//}
		}
	}
}

void UArmyEditorEngine::SelectNone(bool bNoteSelectionChange, bool bDeselectBSPSurfs, bool WarnAboutManyActors)
{	
	DeselectActor(bNoteSelectionChange, bDeselectBSPSurfs, WarnAboutManyActors);
	USelection::SelectNoneEvent.Broadcast();
	/*if (bShowProgress)
	{
		GWarn->EndSlowTask();
	}*/
}

void UArmyEditorEngine::DeselectActor(bool bNoteSelectionChange, bool bDeselectBSPSurfs, bool WarnAboutManyActors)
{
	GArmyEditor->SelectNonePreEvent.ExecuteIfBound();
	//if (GEdSelectionLock)
	//{
	//	return;
	//}

	//bool bShowProgress = false;

	//// If there are a lot of actors to process, pop up a warning "are you sure?" box
	//if (WarnAboutManyActors)
	//{
	//	int32 NumSelectedActors = GEditor->GetSelectedActorCount();
	//	if (NumSelectedActors >= EditorActorSelectionDefs::MaxActorsToSelectBeforeWarning)
	//	{
	//		bShowProgress = true;

	//		const FText ConfirmText = FText::Format(NSLOCTEXT("UnrealEd", "Warning_ManyActorsForDeselect", "There are {0} selected actors. Are you sure you want to deselect them all?"), FText::AsNumber(NumSelectedActors));

	//		FSuppressableWarningDialog::FSetupInfo Info(ConfirmText, NSLOCTEXT("UnrealEd", "Warning_ManyActors", "Warning: Many Actors"), "Warning_ManyActors");
	//		Info.ConfirmText = NSLOCTEXT("ModalDialogs", "ManyActorsForDeselectConfirm", "Continue Deselection");
	//		Info.CancelText = NSLOCTEXT("ModalDialogs", "ManyActorsForDeselectCancel", "Keep Current Selection");

	//		FSuppressableWarningDialog ManyActorsWarning(Info);
	//		if (ManyActorsWarning.ShowModal() == FSuppressableWarningDialog::Cancel)
	//		{
	//			return;
	//		}
	//	}
	//}

	//if (bShowProgress)
	//{
	//	GWarn->BeginSlowTask(LOCTEXT("BeginDeselectingActorsTaskMessage", "Deselecting Actors"), true);
	//}

	// Make a list of selected actors . . .
	TArray<AActor*> ActorsToDeselect;
	for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
	{
		AActor* Actor = static_cast<AActor*>(*It);
		checkSlow(Actor->IsA(AActor::StaticClass()));

		ActorsToDeselect.Add(Actor);
	}

	USelection* SelectedActors = GetSelectedActors();
	SelectedActors->BeginBatchSelectOperation();
	SelectedActors->Modify();

	// . . . and deselect them.
	for (int32 ActorIndex = 0; ActorIndex < ActorsToDeselect.Num(); ++ActorIndex)
	{
		AActor* Actor = ActorsToDeselect[ActorIndex];
		SelectActor(Actor, false, false);
	}

	SelectedActors->EndBatchSelectOperation(bNoteSelectionChange);

	//prevents clicking on background multiple times spamming selection changes
	if (ActorsToDeselect.Num())// || NumDeselectSurfaces)
	{
		if (bNoteSelectionChange)
		{
			NoteSelectionChange();
		}

		//whenever selection changes, recompute whether the selection contains a locked actor
		//bCheckForLockActors = true;

		//whenever selection changes, recompute whether the selection contains a world info actor
		//bCheckForWorldSettingsActors = true;
	}
}

void UArmyEditorEngine::NoteSelectionChange()
{
	// The selection changed, so make sure the pivot (widget) is located in the right place
	UpdatePivotLocationForSelection(true);

	// Clear active editing visualizer on selection change
//	GUnrealEd->ComponentVisManager.ClearActiveComponentVis();

// 	TArray<FEdMode*> ActiveModes;
// 	GLevelEditorModeTools().GetActiveModes(ActiveModes);
// 	for (int32 ModeIndex = 0; ModeIndex < ActiveModes.Num(); ++ModeIndex)
// 	{
// 		ActiveModes[ModeIndex]->ActorSelectionChangeNotify();
// 	}

	const bool bComponentSelectionChanged = GetSelectedComponentCount() > 0;
	USelection* Selection = bComponentSelectionChanged ? GetSelectedComponents() : GetSelectedActors();
	USelection::SelectionChangedEvent.Broadcast(Selection);

	if (!bComponentSelectionChanged)
	{
		//whenever selection changes, recompute whether the selection contains a locked actor
		//bCheckForLockActors = true;

		//whenever selection changes, recompute whether the selection contains a world info actor
		//bCheckForWorldSettingsActors = true;

		//UpdateFloatingPropertyWindows();
	}

	//RedrawLevelEditingViewports();
}

#undef LOCTEXT_NAMESPACE