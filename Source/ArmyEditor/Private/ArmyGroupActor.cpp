// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ArmyGroupActor.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEditorEngine.h"

const FLinearColor BOXCOLOR_LOCKEDGROUPS(0.0f, 1.0f, 0.0f);
const FLinearColor BOXCOLOR_UNLOCKEDGROUPS(1.0f, 0.0f, 0.0f);


AArmyGroupActor::AArmyGroupActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bLocked = true;

	USceneComponent* GroupComponent = CreateDefaultSubobject<USceneComponent>(TEXT("GroupComponent"));
	RootComponent = GroupComponent;
}

bool AArmyGroupActor::IsSelected() const
{
	// Group actors can only count as 'selected' if they are locked 
	return (IsLocked() && HasSelectedActors()) || Super::IsSelected();
}


bool ActorHasParentInGroup(const TArray<class AActor*> &GroupActors, const AActor* Actor)
{
	check(Actor);
	// Check that we've not got a parent attachment within the group.
	USceneComponent *Curr = Actor->GetRootComponent();
	for (int32 OtherIndex = 0; OtherIndex < GroupActors.Num(); ++OtherIndex)
	{
		const AActor* OtherActor = GroupActors[OtherIndex];
		if (OtherActor != NULL && OtherActor != Actor)
		{
			USceneComponent *Other = OtherActor->GetRootComponent();
			if (Curr->IsAttachedTo(Other))
			{
				// We do have parent so don't apply the delta - our parent object will apply it instead.
				return true;
			}
		}
	}
	return false;
}


void AArmyGroupActor::GroupApplyDelta(UArmyEditorViewportClient* Viewport, const FVector& InDrag, const FRotator& InRot, const FVector& InScale)
{
	check(Viewport);
	FBox SelectActorsBox(ForceInit);
	TArray<AActor*> SelectActors;
	GArmyEditor->GetSelectedActors(SelectActors);
	SelectActorsBox = GArmyEditor->GetSelectedActorsBox();
	if (!Viewport->bAdsorbed)
		Viewport->FlowAxisBoxOfSelectedActors = SelectActorsBox;
	else
		Viewport->FlowAxisBoxOfSelectedActors = Viewport->FlowAxisBoxOfSelectedActors.MoveTo(Viewport->FlowAxisBoxOfSelectedActors.GetCenter() + InDrag);

	for (int32 ActorIndex = 0; ActorIndex < GroupActors.Num(); ++ActorIndex)
	{
		if (GroupActors[ActorIndex] != NULL)
		{
			// Check that we've not got a parent attachment within the group.
			const bool canApplyDelta = !ActorHasParentInGroup(GroupActors, GroupActors[ActorIndex]);
			if (canApplyDelta)
			{
				if (!Viewport->IsAdsorbDetect(GroupActors[ActorIndex], SelectActors, SelectActorsBox, InDrag))
				{
					Viewport->ApplyDeltaToActor(GroupActors[ActorIndex], InDrag, InRot, InScale);
					Viewport->bAdsorbed = false;
				}
				else
					Viewport->bAdsorbed = true;
					
			}
		}
	}
	for (int32 SubGroupIndex = 0; SubGroupIndex<SubGroups.Num(); ++SubGroupIndex)
	{
		if (SubGroups[SubGroupIndex] != NULL)
		{
			SubGroups[SubGroupIndex]->GroupApplyDelta(Viewport, InDrag, InRot, InScale);
		}
	}
	Viewport->ApplyDeltaToActor(this, InDrag, InRot, InScale);
}

void AArmyGroupActor::GroupApplyDelta(const FVector& InDrag, const FRotator& InRot, const FVector& InScale)
{
	for (int32 ActorIndex = 0; ActorIndex<GroupActors.Num(); ++ActorIndex)
	{
		// Check that we've not got a parent attachment within the group.
		const bool canApplyDelta = !ActorHasParentInGroup(GroupActors, GroupActors[ActorIndex]);
		if (canApplyDelta && GroupActors[ActorIndex] != NULL)
		{
			GArmyEditor->ApplyDeltaToActor(GroupActors[ActorIndex], true, &InDrag, &InRot, &InScale);
		}
	}
	for (int32 SubGroupIndex = 0; SubGroupIndex<SubGroups.Num(); ++SubGroupIndex)
	{
		if (SubGroups[SubGroupIndex] != NULL)
		{
			SubGroups[SubGroupIndex]->GroupApplyDelta(InDrag, InRot, InScale);
		}
	}
	GArmyEditor->ApplyDeltaToActor(this, true, &InDrag, &InRot, &InScale);
}
//void AArmyGroupActor::GroupApplyTransform(const FVector& InDrag, const FRotator& InRot, const FVector& InScale)
//{
//	GArmyEditor->SelectNone(true, true);
//
//	GArmyEditor->SelectActor(this, true, false);
//
//	FVector LocationDelta = InDrag - GetActorLocation();
//	FRotator RotationDelta = InRot - GetActorRotation();
//	FVector ScaleDelta = InScale - GetActorScale3D();
//	GroupApplyDelta(GArmyVC, LocationDelta, RotationDelta, ScaleDelta);
//}

bool AArmyGroupActor::Modify(bool bAlwaysMarkDirty/*=true*/)
{
	bool bSavedToTransactionBuffer = false;
	for (int32 ActorIndex = 0; ActorIndex < GroupActors.Num(); ++ActorIndex)
	{
		if (GroupActors[ActorIndex] != NULL)
		{
			bSavedToTransactionBuffer = GroupActors[ActorIndex]->Modify(bAlwaysMarkDirty) || bSavedToTransactionBuffer;
		}
	}
	for (int32 SubGroupIndex = 0; SubGroupIndex < SubGroups.Num(); ++SubGroupIndex)
	{
		if (SubGroups[SubGroupIndex] != NULL)
		{
			bSavedToTransactionBuffer = SubGroups[SubGroupIndex]->Modify(bAlwaysMarkDirty) || bSavedToTransactionBuffer;
		}
	}
	bSavedToTransactionBuffer = Super::Modify(bAlwaysMarkDirty) || bSavedToTransactionBuffer;
	return  bSavedToTransactionBuffer;
}

void GetBoundingVectorsForGroup(AArmyGroupActor* GroupActor, FViewport* Viewport, FVector& OutVectorMin, FVector& OutVectorMax)
{
	// Draw a bounding box for grouped actors using the vector range we can gather from any child actors (including subgroups)
	OutVectorMin = FVector(BIG_NUMBER);
	OutVectorMax = FVector(-BIG_NUMBER);

	// Grab all actors for this group, including those within subgroups
	TArray<AActor*> ActorsInGroup;
	GroupActor->GetGroupActors(ActorsInGroup, true);

	// Loop through and collect each actor, using their bounding box to create the bounds for this group
	for (int32 ActorIndex = 0; ActorIndex < ActorsInGroup.Num(); ++ActorIndex)
	{
		AActor* Actor = ActorsInGroup[ActorIndex];
		uint64 HiddenClients = Actor->HiddenEditorViews;
		bool bActorHiddenForViewport = false;
		//if (!Actor->IsHiddenEd())
		{
			// 			if (Viewport)
			// 			{
			// 				for (int32 ViewIndex = 0; ViewIndex<GVRSEditor->LevelViewportClients.Num(); ++ViewIndex)
			// 				{
			// 					// If the current viewport is hiding this actor, don't draw brackets around it
			// 					if (Viewport->GetClient() == GVRSEditor->LevelViewportClients[ViewIndex] && HiddenClients & ((uint64)1 << ViewIndex))
			// 					{
			// 						bActorHiddenForViewport = true;
			// 						break;
			// 					}
			// 				}
			// 			}

			if (!bActorHiddenForViewport)
			{
				FBox ActorBox;

				// First check to see if we're dealing with a sprite, otherwise just use the normal bounding box
				// 				UBillboardComponent* SpriteComponent = Actor->FindComponentByClass<UBillboardComponent>();
				// 				if (SpriteComponent != NULL)
				// 				{
				// 					ActorBox = SpriteComponent->Bounds.GetBox();
				// 				}
				// 				else
				{
					ActorBox = Actor->GetComponentsBoundingBox(true);
				}

				// MinVector
				OutVectorMin.X = FMath::Min<float>(ActorBox.Min.X, OutVectorMin.X);
				OutVectorMin.Y = FMath::Min<float>(ActorBox.Min.Y, OutVectorMin.Y);
				OutVectorMin.Z = FMath::Min<float>(ActorBox.Min.Z, OutVectorMin.Z);
				// MaxVector
				OutVectorMax.X = FMath::Max<float>(ActorBox.Max.X, OutVectorMax.X);
				OutVectorMax.Y = FMath::Max<float>(ActorBox.Max.Y, OutVectorMax.Y);
				OutVectorMax.Z = FMath::Max<float>(ActorBox.Max.Z, OutVectorMax.Z);
			}
		}
	}
}

/**
* Draw brackets around all given groups
* @param	PDI			FPrimitiveDrawInterface used to draw lines in active viewports
* @param	Viewport	Current viewport being rendered
* @param	InGroupList	Array of groups to draw brackets for
*/
void PrivateDrawBracketsForGroups(FPrimitiveDrawInterface* PDI, FViewport* Viewport, const TArray<AArmyGroupActor*>& InGroupList)
{
	// Loop through each given group and draw all subgroups and actors
	for (int32 GroupIndex = 0; GroupIndex < InGroupList.Num(); ++GroupIndex)
	{
		AArmyGroupActor* GroupActor = InGroupList[GroupIndex];
		if (GroupActor->GetWorld() == PDI->View->Family->Scene->GetWorld())
		{
			const FLinearColor GROUP_COLOR = GroupActor->IsLocked() ? BOXCOLOR_LOCKEDGROUPS : BOXCOLOR_UNLOCKEDGROUPS;

			FVector MinVector;
			FVector MaxVector;
			GetBoundingVectorsForGroup(GroupActor, Viewport, MinVector, MaxVector);

			// Create a bracket offset to pad the space between brackets and actor(s) and determine the length of our corner axises
			float BracketOffset = FVector::Dist(MinVector, MaxVector) * 0.1f;
			MinVector = MinVector - BracketOffset;
			MaxVector = MaxVector + BracketOffset;

			// Calculate bracket corners based on min/max vectors
			TArray<FVector> BracketCorners;

			// Bottom Corners
			BracketCorners.Add(FVector(MinVector.X, MinVector.Y, MinVector.Z));
			BracketCorners.Add(FVector(MinVector.X, MaxVector.Y, MinVector.Z));
			BracketCorners.Add(FVector(MaxVector.X, MaxVector.Y, MinVector.Z));
			BracketCorners.Add(FVector(MaxVector.X, MinVector.Y, MinVector.Z));

			// Top Corners
			BracketCorners.Add(FVector(MinVector.X, MinVector.Y, MaxVector.Z));
			BracketCorners.Add(FVector(MinVector.X, MaxVector.Y, MaxVector.Z));
			BracketCorners.Add(FVector(MaxVector.X, MaxVector.Y, MaxVector.Z));
			BracketCorners.Add(FVector(MaxVector.X, MinVector.Y, MaxVector.Z));

			for (int32 BracketCornerIndex = 0; BracketCornerIndex < BracketCorners.Num(); ++BracketCornerIndex)
			{
				// Direction corner axis should be pointing based on min/max
				const FVector CORNER = BracketCorners[BracketCornerIndex];
				const int32 DIR_X = CORNER.X == MaxVector.X ? -1 : 1;
				const int32 DIR_Y = CORNER.Y == MaxVector.Y ? -1 : 1;
				const int32 DIR_Z = CORNER.Z == MaxVector.Z ? -1 : 1;

				PDI->DrawLine(CORNER, FVector(CORNER.X + (BracketOffset * DIR_X), CORNER.Y, CORNER.Z), GROUP_COLOR, SDPG_Foreground);
				PDI->DrawLine(CORNER, FVector(CORNER.X, CORNER.Y + (BracketOffset * DIR_Y), CORNER.Z), GROUP_COLOR, SDPG_Foreground);
				PDI->DrawLine(CORNER, FVector(CORNER.X, CORNER.Y, CORNER.Z + (BracketOffset * DIR_Z)), GROUP_COLOR, SDPG_Foreground);
			}

			// Recurse through to any subgroups
			TArray<AArmyGroupActor*> SubGroupsInGroup;
			GroupActor->GetSubGroups(SubGroupsInGroup);
			PrivateDrawBracketsForGroups(PDI, Viewport, SubGroupsInGroup);
		}
	}
}


/**
* Checks to see if the given GroupActor has any parents in the given Array.
* @param	InGroupActor	Group to check lineage
* @param	InGroupArray	Array to search for the given group's parent
* @return	True if a parent was found.
*/
bool GroupHasParentInArray(AArmyGroupActor* InGroupActor, TArray<AArmyGroupActor*>& InGroupArray)
{
	check(InGroupActor);
	AArmyGroupActor* CurrentParentNode = AArmyGroupActor::GetParentForActor(InGroupActor);

	// Use a cursor pointer to continually move up from our starting pointer (InGroupActor) through the hierarchy until
	// we find a valid parent in the given array, or run out of nodes.
	while (CurrentParentNode)
	{
		if (InGroupArray.Contains(CurrentParentNode))
		{
			return true;
		}
		CurrentParentNode = AArmyGroupActor::GetParentForActor(CurrentParentNode);
	}
	return false;
}

void AArmyGroupActor::RemoveSubGroupsFromArray(TArray<AArmyGroupActor*>& GroupArray)
{
	for (int32 GroupIndex = 0; GroupIndex < GroupArray.Num(); ++GroupIndex)
	{
		AArmyGroupActor* GroupToCheck = GroupArray[GroupIndex];
		if (GroupHasParentInArray(GroupToCheck, GroupArray))
		{
			GroupArray.Remove(GroupToCheck);
			--GroupIndex;
		}
	}
}

AArmyGroupActor* AArmyGroupActor::GetRootForActor(AActor* InActor, bool bMustBeLocked/*=false*/, bool bMustBeSelected/*=false*/, bool bMustBeUnlocked/*=false*/, bool bMustBeUnselected/*=false*/)
{
	AArmyGroupActor* RootNode = NULL;
	// If InActor is a group, use that as the beginning iteration node, else try to find the parent
	AArmyGroupActor* InGroupActor = Cast<AArmyGroupActor>(InActor);
	AArmyGroupActor* IteratingNode = InGroupActor == NULL ? AArmyGroupActor::GetParentForActor(InActor) : InGroupActor;
	while (IteratingNode)
	{
		if ((!bMustBeLocked || IteratingNode->IsLocked()) && (!bMustBeSelected || IteratingNode->HasSelectedActors())
			&& (!bMustBeUnlocked || !IteratingNode->IsLocked()) && (!bMustBeUnselected || !IteratingNode->HasSelectedActors()))
		{
			RootNode = IteratingNode;
		}
		IteratingNode = AArmyGroupActor::GetParentForActor(IteratingNode);
	}
	return RootNode;
}

AArmyGroupActor* AArmyGroupActor::GetParentForActor(AActor* InActor)
{
	return (InActor && InActor->IsValidLowLevel()) ? Cast<AArmyGroupActor>(InActor->GroupActor) : nullptr;
}


void AArmyGroupActor::AddSelectedActorsToSelectedGroup()
{

}


void AArmyGroupActor::LockSelectedGroups()
{

}


void AArmyGroupActor::UnlockSelectedGroups()
{

}


ARMYEDITOR_API void AArmyGroupActor::ToggleGroupMode()
{

}


void AArmyGroupActor::SelectGroupsInSelection()
{

}

void AArmyGroupActor::Lock()
{
	bLocked = true;
	for (int32 SubGroupIdx = 0; SubGroupIdx < SubGroups.Num(); ++SubGroupIdx)
	{
		if (SubGroups[SubGroupIdx] != NULL)
		{
			SubGroups[SubGroupIdx]->Lock();
		}
	}
}

void AArmyGroupActor::Add(AActor& InActor)
{
	// See if the incoming actor already belongs to a group
	AArmyGroupActor* InActorParent = AArmyGroupActor::GetParentForActor(&InActor);
	if (InActorParent) // If so, detach it first
	{
		if (InActorParent == this)
		{
			return;
		}
		InActorParent->Modify();
		InActorParent->Remove(InActor);
	}

	Modify();
	AArmyGroupActor* InGroupPtr = Cast<AArmyGroupActor>(&InActor);
	if (InGroupPtr)
	{
		check(InGroupPtr != this);
		SubGroups.AddUnique(InGroupPtr);
	}
	else
	{
		GroupActors.AddUnique(&InActor);
		InActor.Modify();
		InActor.GroupActor = this;
	}
}

void AArmyGroupActor::Remove(AActor& InActor)
{
	AArmyGroupActor* InGroupPtr = Cast<AArmyGroupActor>(&InActor);
	if (InGroupPtr && SubGroups.Contains(InGroupPtr))
	{
		Modify();
		SubGroups.Remove(InGroupPtr);
	}
	else if (GroupActors.Contains(&InActor))
	{
		Modify();
		GroupActors.Remove(&InActor);
		InActor.Modify();
		InActor.GroupActor = NULL;
	}

	PostRemove();
}

void AArmyGroupActor::PostRemove()
{
	// If all children have been removed (or only one subgroup remains), this group is no longer active.
	if (GroupActors.Num() == 0 && SubGroups.Num() <= 1)
	{
		// Remove any potentially remaining subgroups
		SubGroups.Empty();

		// Destroy the actor and remove it from active groups
		AArmyGroupActor* ParentGroup = AArmyGroupActor::GetParentForActor(this);
		if (ParentGroup)
		{
			ParentGroup->Modify();
			ParentGroup->Remove(*this);
		}

		UWorld* MyWorld = GetWorld();
		if (MyWorld)
		{
			// Group is no longer active
			//MyWorld->ActiveGroupActors.Remove(this);

			MyWorld->ModifyLevel(GetLevel());

			// Mark the group actor for removal
			MarkPackageDirty();

			// If not currently garbage collecting (changing maps, saving, etc), remove the group immediately
			// 			if (!IsGarbageCollecting())
			// 			{
			// 				// Refresh all editor browsers after removal
			// 				FScopedRefreshAllBrowsers LevelRefreshAllBrowsers;
			// 
			// 				// Destroy group and clear references.
			// 				GVRSEditor->Layers->DisassociateActorFromLayers(this);
			// 				MyWorld->EditorDestroyActor(this, false);
			// 
			// 				LevelRefreshAllBrowsers.Request();
			// 			}
		}
	}
}

bool AArmyGroupActor::Contains(AActor& InActor) const
{
	AActor* InActorPtr = &InActor;
	AArmyGroupActor* InGroupPtr = Cast<AArmyGroupActor>(InActorPtr);
	if (InGroupPtr)
	{
		return SubGroups.Contains(InGroupPtr);
	}
	return GroupActors.Contains(InActorPtr);
}

bool AArmyGroupActor::HasSelectedActors(bool bDeepSearch/*=true*/) const
{
	for (int32 ActorIndex = 0; ActorIndex < GroupActors.Num(); ++ActorIndex)
	{
		if (GroupActors[ActorIndex] != NULL)
		{
			if (GroupActors[ActorIndex]->IsSelected())
			{
				return true;
			}
		}
	}
	if (bDeepSearch)
	{
		for (int32 GroupIndex = 0; GroupIndex < SubGroups.Num(); ++GroupIndex)
		{
			if (SubGroups[GroupIndex] != NULL)
			{
				if (SubGroups[GroupIndex]->HasSelectedActors(bDeepSearch))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void AArmyGroupActor::ClearAndRemove()
{
	// Actors can potentially be NULL here. Some older maps can serialize invalid Actors 
	// into GroupActors or SubGroups.
	for (int32 ActorIndex = 0; ActorIndex < GroupActors.Num(); ++ActorIndex)
	{
		if (GroupActors[ActorIndex])
		{
			Remove(*GroupActors[ActorIndex]);
		}
		else
		{
			GroupActors.RemoveAt(ActorIndex);
			PostRemove();
		}
		--ActorIndex;
	}
	for (int32 SubGroupIndex = 0; SubGroupIndex < SubGroups.Num(); ++SubGroupIndex)
	{
		if (SubGroups[SubGroupIndex])
		{
			Remove(*SubGroups[SubGroupIndex]);
		}
		else
		{
			SubGroups.RemoveAt(SubGroupIndex);
			PostRemove();
		}
		--SubGroupIndex;
	}
}

void AArmyGroupActor::CenterGroupLocation()
{
	FVector MinVector;
	FVector MaxVector;
	GetBoundingVectorsForGroup(this, NULL, MinVector, MaxVector);

	FVector Center = (MinVector + MaxVector) * 0.5f;
	Center.Z = MinVector.Z;
	SetActorLocation(Center, false);
	//GVRSEditor->NoteSelectionChange();
}

void AArmyGroupActor::GetGroupActors(TArray<AActor*>& OutGroupActors, bool bRecurse/*=false*/) const
{
	if (bRecurse)
	{
		for (int32 i = 0; i < SubGroups.Num(); ++i)
		{
			if (SubGroups[i] != NULL)
			{
				SubGroups[i]->GetGroupActors(OutGroupActors, bRecurse);
			}
		}
	}
	else
	{
		OutGroupActors.Empty();
	}
	for (int32 i = 0; i < GroupActors.Num(); ++i)
	{
		if (GroupActors[i] != NULL)
		{
			OutGroupActors.Add(GroupActors[i]);
		}
	}
}

void AArmyGroupActor::GetSubGroups(TArray<AArmyGroupActor*>& OutSubGroups, bool bRecurse/*=false*/) const
{
	if (bRecurse)
	{
		for (int32 i = 0; i < SubGroups.Num(); ++i)
		{
			if (SubGroups[i] != NULL)
			{
				SubGroups[i]->GetSubGroups(OutSubGroups, bRecurse);
			}
		}
	}
	else
	{
		OutSubGroups.Empty();
	}
	for (int32 i = 0; i < SubGroups.Num(); ++i)
	{
		if (SubGroups[i] != NULL)
		{
			OutSubGroups.Add(SubGroups[i]);
		}
	}
}

void AArmyGroupActor::GetAllChildren(TArray<AActor*>& OutChildren, bool bRecurse/*=false*/) const
{
	GetGroupActors(OutChildren, bRecurse);
	TArray<AArmyGroupActor*> OutSubGroups;
	GetSubGroups(OutSubGroups, bRecurse);
	for (int32 SubGroupIdx = 0; SubGroupIdx < OutSubGroups.Num(); ++SubGroupIdx)
	{
		OutChildren.Add(OutSubGroups[SubGroupIdx]);
	}
}

int32 AArmyGroupActor::GetActorNum() const
{
	return GroupActors.Num();
}

void AArmyGroupActor::PostEditMove(bool bFinished)
{
	//for(int32 ActorIndex=0; ActorIndex<GroupActors.Num(); ++ActorIndex)
	//{
	//	if(GroupActors[ActorIndex]!=NULL)
	//	{
	//		GroupActors[ActorIndex]->PostEditMove(bFinished);
	//	}
	//}
	//for(int32 SubGroupIndex=0; SubGroupIndex<SubGroups.Num(); ++SubGroupIndex)
	//{
	//	if(SubGroups[SubGroupIndex]!=NULL)
	//	{
	//		SubGroups[SubGroupIndex]->PostEditMove(bFinished);
	//	}
	//}
	Super::PostEditMove(bFinished);
}


