// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//=============================================================================
// GroupActor: Collects a group of actors, allowing for management and universal transformation.
//=============================================================================

#pragma once
#include "GameFramework/Actor.h"
#include "ArmyGroupActor.generated.h"

class UArmyEditorViewportClient;

UCLASS()
class ARMYEDITOR_API AArmyGroupActor : public AActor
{

public:
	GENERATED_UCLASS_BODY()

		UPROPERTY()
		uint32 bLocked : 1;

	UPROPERTY()
		TArray<class AActor*> GroupActors;

	UPROPERTY()
		TArray<class AArmyGroupActor*> SubGroups;

	int32 AutoDesignPlanID = -1;
	int32 AutoDesignSpaceTypeID = -1;
	int32 AutoDesignActorTypeID = -1;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	virtual bool IsSelected() const;

	/**
	* Apply given deltas to all actors and subgroups for this group.
	* @param	Viewport		The viewport to draw to apply our deltas
	* @param	InDrag			Delta Transition
	* @param	InRot			Delta Rotation
	* @param	InScale			Delta Scale
	*/
	void GroupApplyDelta(UArmyEditorViewportClient* Viewport, const FVector& InDrag, const FRotator& InRot, const FVector& InScale);

	/**
	* Apply given deltas to all actors and subgroups for this group.
	* @param	InDrag			Delta Transition
	* @param	InRot			Delta Rotation
	* @param	InScale			Delta Scale
	*/
	void GroupApplyDelta(const FVector& InDrag, const FRotator& InRot, const FVector& InScale);

	//ARMYEDITOR_API void GroupApplyTransform(const FVector& InDrag, const FRotator& InRot, const FVector& InScale);

	/**
	* Changes the given array to remove any existing subgroups
	* @param	GroupArray	Array to remove subgroups from
	*/
	static void RemoveSubGroupsFromArray(TArray<AArmyGroupActor*>& GroupArray);

	/**
	* Returns the highest found root for the given actor or null if one is not found. Qualifications of root can be specified via optional parameters.
	* @param	InActor			Actor to find a group root for.
	* @param	bMustBeLocked	Flag designating to only return the topmost locked group.
	* @param	bMustBeSelected	Flag designating to only return the topmost selected group.
	* @param	bMustBeUnlocked		Flag designating to only return the topmost unlocked group.
	* @param	bMustBeUnselected	Flag designating to only return the topmost unselected group.
	* @return	The topmost group actor for this actor. Returns null if none exists using the given conditions.
	*/
	static AArmyGroupActor* GetRootForActor(AActor* InActor, bool bMustBeLocked = false, bool bMustBeSelected = false, bool bMustBeUnlocked = false, bool bMustBeUnselected = false);

	/**
	* Returns the direct parent for the actor or null if one is not found.
	* @param	InActor	Actor to find a group parent for.
	* @return	The direct parent for the given actor. Returns null if no group has this actor as a child.
	*/
	static AArmyGroupActor* GetParentForActor(AActor* InActor);

	/**
	* Query to find how many active groups are currently in the editor.
	* @param	bSelected	Flag to only return currently selected groups (defaults to false).
	* @param	bDeepSearch	Flag to do a deep search when checking group selections (defaults to true).
	* @return	Number of active groups currently in the editor.
	*/
	static const int32 NumActiveGroups(bool bSelected = false, bool bDeepSearch = true);

	/**
	* Adds selected ungrouped actors to a selected group. Does nothing if more than one group is selected.
	*/
	static void AddSelectedActorsToSelectedGroup();

	/**
	* Locks the lowest selected groups in the current selection.
	*/
	static void LockSelectedGroups();

	/**
	* Unlocks the highest locked parent groups for actors in the current selection.
	*/
	static void UnlockSelectedGroups();

	/**
	* Toggle group mode
	*/
	static void ToggleGroupMode();

	/**
	* Reselects any valid groups based on current editor selection
	*/
	static void SelectGroupsInSelection();

	/**
	* Lock this group and all subgroups.
	*/
	void Lock();

	/**
	* Unlock this group
	*/
	FORCEINLINE void Unlock()
	{
		bLocked = false;
	};

	/**
	* @return	Group's locked state
	*/
	FORCEINLINE bool IsLocked() const
	{
		return bLocked;
	};

	/**
	* @param	InActor	Actor to add to this group
	*/
	void Add(AActor& InActor);

	/**
	* Removes the given actor from this group. If the group has no actors after this transaction, the group itself is removed.
	* @param	InActor	Actor to remove from this group
	*/
 void Remove(AActor& InActor);

	/**
	* @param InActor	Actor to search for
	* @return True if the group contains the given actor.
	*/
	 bool Contains(AActor& InActor) const;

	/**
	* @param bDeepSearch	Flag to check all subgroups as well. Defaults to true.
	* @return True if the group contains any selected actors.
	*/
	bool HasSelectedActors(bool bDeepSearch = true) const;

	/**
	* Detaches all children (actors and subgroups) from this group and then removes it.
	*/
	void ClearAndRemove();

	/**
	* Sets this group's location to the center point based on current location of its children.
	*/
	 void CenterGroupLocation();

	/**
	* @param	OutGroupActors	Array to fill with all actors for this group.
	* @param	bRecurse		Flag to recurse and gather any actors in this group's subgroups.
	*/
	 void GetGroupActors(TArray<AActor*>& OutGroupActors, bool bRecurse = false) const;

	/**
	* @param	OutSubGroups	Array to fill with all subgroups for this group.
	* @param	bRecurse	Flag to recurse and gather any subgroups in this group's subgroups.
	*/
	void GetSubGroups(TArray<AArmyGroupActor*>& OutSubGroups, bool bRecurse = false) const;

	/**
	* @param	OutChildren	Array to fill with all children for this group.
	* @param	bRecurse	Flag to recurse and gather any children in this group's subgroups.
	*/
	 void GetAllChildren(TArray<AActor*>& OutChildren, bool bRecurse = false) const;

	/**
	* @return The number of actors in the group.
	*/
	int32 GetActorNum() const;

	virtual void PostEditMove(bool bFinished) override;
private:
	/**
	* Helper function for Remove() and ClearAndRemove() - cleans up actors when grouping is no longer needed
	*/
	void PostRemove();
};



