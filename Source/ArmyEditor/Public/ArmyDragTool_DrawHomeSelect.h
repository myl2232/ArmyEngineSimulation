// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "ArmyEditorViewportClient.h"
#include "ArmyEditorDragTools.h"

class FCanvas;
//class UModel;
class UArmyEditorViewportClient;

/**
 * Draws a box in the current viewport and when the mouse button is released,
 * it selects/unselects everything inside of it.
 */
class FArmyDragTool_ActorBoxSelect : public FArmyDragTool
{
public:
	explicit FArmyDragTool_ActorBoxSelect(UArmyEditorViewportClient* InLevelViewportClient )
		: FArmyDragTool(InLevelViewportClient->GetModeTools())
		, LevelViewportClient( InLevelViewportClient )
	{}

	/**
	 * Starts a mouse drag behavior.  The start location is snapped to the editor constraints if bUseSnapping is true.
	 *
	 * @param	InViewportClient	The viewport client in which the drag event occurred.
	 * @param	InStart				Where the mouse was when the drag started.
	 */
	virtual void StartDrag(UArmyEditorViewportClient* InViewportClient, const FVector& InStart, const FVector2D& InStartScreen) override;

	/* Updates the drag tool's end location with the specified delta.  The end location is
	 * snapped to the editor constraints if bUseSnapping is true.
	 *
	 * @param	InDelta		A delta of mouse movement.
	 */
	virtual void AddDelta(const FVector& InDelta) override;

	/**
	* Ends a mouse drag behavior (the user has let go of the mouse button).
	*/
	virtual void EndDrag() override;
	virtual void Render(const FSceneView* View, FCanvas* Canvas) override;

private:
	/** 
	 * Calculates a box to check actors against 
	 * 
	 * @param OutBox	The created box.
	 */
	void CalculateBox( FBox& OutBox );

	/** 
	 * Returns true if the passed in Actor intersects with the provided box 
	 *
	 * @param InActor				The actor to check
	 * @param InBox					The box to check against
	 * @param bUseStrictSelection	true if the actor must be entirely within the frustum
	 */
	bool IntersectsBox( AActor& InActor, const FBox& InBox, bool bUseStrictSelection );

	/** 
	 * Returns true if the provided BSP node intersects with the provided frustum 
	 *
	 * @param InModel				The model containing BSP nodes to check
	 * @param NodeIndex				The index to a BSP node in the model.  This node is used for the bounds check.
	 * @param InFrustum				The frustum to check against.
	 * @param bUseStrictSelection	true if the node must be entirely within the frustum
	 */
	bool IntersectsBox( const UModel& InModel, int32 NodeIndex, const FBox& InBox, bool bUseStrictSelection ) const;

	/** List of BSP models to check for selection */
	TArray<UModel*> ModelsToCheck;

	UArmyEditorViewportClient* LevelViewportClient;
};

