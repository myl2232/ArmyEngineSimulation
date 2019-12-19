// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#include "ArmyDragTool_FrustumSelect.h"
#include "StaticMeshResources.h"
#include "HModel.h"
#include "Components/DrawSphereComponent.h"
#include "AI/Navigation/NavLinkRenderingComponent.h"
#include "Engine/Selection.h"
#include "EngineUtils.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "ArmyEditorEngine.h"
#include "ArmyEditorViewportClient.h"
///////////////////////////////////////////////////////////////////////////////
//
// FDragTool_FrustumSelect
//
///////////////////////////////////////////////////////////////////////////////

void FArmyDragTool_ActorFrustumSelect::AddDelta( const FVector& InDelta )
{
	FIntPoint MousePos;
	LevelViewportClient->Viewport->GetMousePos(MousePos);

	EndWk = FVector(MousePos);
	End = EndWk;

	//const bool bUseHoverFeedback = GEditor != NULL && GetDefault<ULevelEditorViewportSettings>()->bEnableViewportHoverFeedback;
}

void FArmyDragTool_ActorFrustumSelect::StartDrag(UArmyEditorViewportClient* InViewportClient, const FVector& InStart, const FVector2D& InStartScreen)
{
	FArmyDragTool::StartDrag(InViewportClient, InStart, InStartScreen);

	//const bool bUseHoverFeedback = GEditor != NULL && GetDefault<ULevelEditorViewportSettings>()->bEnableViewportHoverFeedback;

	// Remove any active hover objects
	//FLevelEditorViewportClient::ClearHoverFromObjects();

	FIntPoint MousePos;
	InViewportClient->Viewport->GetMousePos(MousePos);

	Start = FVector(InStartScreen.X, InStartScreen.Y, 0);
	End = EndWk = Start;
}

void FArmyDragTool_ActorFrustumSelect::EndDrag()
{
	//const bool bGeometryMode = ModeTools->IsModeActive(FBuiltinEditorModes::EM_Geometry);

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(LevelViewportClient->Viewport, LevelViewportClient->GetScene(), LevelViewportClient->EngineShowFlags ));
	FSceneView* SceneView = LevelViewportClient->CalcSceneView(&ViewFamily);

	// Generate a frustum out of the dragged box
	FConvexVolume Frustum;
	CalculateFrustum( SceneView, Frustum, true );

	//FScopedTransaction Transaction( NSLOCTEXT("ActorFrustumSelect", "MarqueeSelectTransation", "Marquee Select" ) );

	bool bShouldSelect = true;
	//GEditor->GetSelectedActors()->Modify();

	if( !bShiftDown )
	{
		// If the user is selecting, but isn't hold down SHIFT, remove all current selections.
		//ModeTools->SelectNone();
	}

	// Does an actor have to be fully contained in the box to be selected
	const bool bStrictDragSelection = false;// GetDefault<ULevelEditorViewportSettings>()->bStrictBoxSelection;
	bool bSelectionChanged = false;

	// Let the editor mode try to handle the selection.
	const bool bEditorModeHandledSelection = false;// ModeTools->FrustumSelect(Frustum, bLeftMouseButtonDown);

	if( !bEditorModeHandledSelection )
	{
		if( !bShiftDown )
		{
			// If the user is selecting, but isn't hold down SHIFT, remove all current selections.
			GArmyEditor->SelectNone( true, true );
		}

		const int32 ViewportSizeX = LevelViewportClient->Viewport->GetSizeXY().X;
		const int32 ViewportSizeY = LevelViewportClient->Viewport->GetSizeXY().Y;

		if( Start.X > End.X )
		{
			Swap( Start.X, End.X );
		}

		if( Start.Y > End.Y )
		{
			Swap( Start.Y, End.Y );
		}

		const bool bTransparentBoxSelection = false;//GetDefault<ULevelEditorViewportSettings>()->bTransparentBoxSelection;
		if (bTransparentBoxSelection)
		{
			// Get a list of frustum-culled actors
			for(FActorIterator It(LevelViewportClient->GetWorld()); It; ++It)
			{
				AActor* Actor = *It;
				if (IntersectsFrustum( *Actor, Frustum, bStrictDragSelection))
				{
					if (Actor->Tags.Num() > 0 && 
						(Actor->Tags[0] == TEXT("MoveableMeshActor") || 
						Actor->Tags[0] == TEXT("EnvironmentAsset") || 
						Actor->Tags[0] == TEXT("BlueprintVRSActor"))
						)
					{
						// Select the actor if it intersected
						GArmyEditor->SelectActor(Actor, bShouldSelect, true);
						bSelectionChanged = true;
					}
				}
			}
		}
		else
		{
			// Extend the endpoint of the rect to get the actual line
			FIntRect BoxRect( FIntPoint( FMath::Max( 0.0f, Start.X ), FMath::Max( 0.0f, Start.Y ) ), FIntPoint( FMath::Min(ViewportSizeX, FMath::TruncToInt(End.X+1)), FMath::Min( ViewportSizeY, FMath::TruncToInt(End.Y+1) ) ) );

			const TArray<FColor>& RawHitProxyData = LevelViewportClient->Viewport->GetRawHitProxyData(BoxRect);

			TSet<AActor*> HitActors;
			TSet<UModel*> HitModels;


			// Lower the resolution with massive box selects
			int32 Step = (BoxRect.Width() > 500 && BoxRect.Height() > 500) ? 4 : 1;


			for (int32 Y = BoxRect.Min.Y; Y < BoxRect.Max.Y; Y = Y < BoxRect.Max.Y - 1 ? FMath::Min(BoxRect.Max.Y-1, Y+Step) : ++Y )
			{
				const FColor* SourceData = &RawHitProxyData[Y * ViewportSizeX];
				for (int32 X = BoxRect.Min.X; X < BoxRect.Max.X; X = X < BoxRect.Max.X-1 ? FMath::Min(BoxRect.Max.X-1, X + Step) : ++X )
				{
					FHitProxyId HitProxyId(SourceData[X]);
					HHitProxy* HitProxy = GetHitProxyById(HitProxyId);

					if (HitProxy)
					{
						if( HitProxy->IsA(HActor::StaticGetType()) )
						{
							AActor* Actor = ((HActor*)HitProxy)->Actor;
							if (Actor)
							{
								HitActors.Add(Actor);
							}
						}
						else if( HitProxy->IsA(HModel::StaticGetType()) )
						{
							HitModels.Add( ((HModel*)HitProxy)->GetModel() );
						}
						else if( HitProxy->IsA(HBSPBrushVert::StaticGetType()) )
						{
							HBSPBrushVert* HitBSPBrushVert = ((HBSPBrushVert*)HitProxy);
							if( HitBSPBrushVert->Brush.IsValid() )
							{
								HitActors.Add( HitBSPBrushVert->Brush.Get() );
							}
						}
					}

				}
			}

// 			if (HitModels.Num() > 0)
// 			{
// 				// Check every model to see if its BSP surfaces should be selected
// 				for (auto It = HitModels.CreateConstIterator(); It; ++It)
// 				{
// 					UModel& Model = **It;
// 					// Check every node in the model
// 					for (int32 NodeIndex = 0; NodeIndex < Model.Nodes.Num(); NodeIndex++)
// 					{
// 						if (IntersectsFrustum(Model, NodeIndex, Frustum, bStrictDragSelection))
// 						{
// 							uint32 SurfaceIndex = Model.Nodes[NodeIndex].iSurf;
// 							FBspSurf& Surf = Model.Surfs[SurfaceIndex];
// 							HitActors.Add( Surf.Actor );
// 						}
// 					}
// 				}
// 			}

			if( HitActors.Num() > 0 )
			{
				for( auto It = HitActors.CreateConstIterator(); It; ++It )
				{
					AActor* Actor = *It;
					if (Actor->Tags.Num() > 0 &&
						(Actor->Tags[0] == TEXT("MoveableMeshActor") ||
							Actor->Tags[0] == TEXT("EnvironmentAsset") ||
							Actor->Tags[0] == TEXT("BlueprintVRSActor"))
						)
					{
						if (bStrictDragSelection && IntersectsFrustum(*Actor, Frustum, bStrictDragSelection))
						{
							GArmyEditor->SelectActor(Actor, bShouldSelect, false);
							bSelectionChanged = true;
						}
						else if (!bStrictDragSelection)
						{
							GArmyEditor->SelectActor(Actor, bShouldSelect, false);
							bSelectionChanged = true;
						}
					}
				}
			}
		}

		if(bSelectionChanged)
		{
			// If any selections were made.  Notify that now.
			GArmyEditor->NoteSelectionChange();
		}
	}

	// Clear any hovered objects that might have been created while dragging
	//FLevelEditorViewportClient::ClearHoverFromObjects();

	FArmyDragTool::EndDrag();
}

void FArmyDragTool_ActorFrustumSelect::Render(const FSceneView* View, FCanvas* Canvas )
{
	FCanvasBoxItem BoxItem( FVector2D(Start.X, Start.Y), FVector2D(End.X-Start.X, End.Y-Start.Y) );
	BoxItem.SetColor( FLinearColor::White );
	Canvas->DrawItem( BoxItem );
}

bool FArmyDragTool_ActorFrustumSelect::IntersectsFrustum( AActor& InActor, const FConvexVolume& InFrustum, bool bUseStrictSelection ) const
{	
	bool bActorHitByBox = false;

	const bool bGeometryMode = false;// ModeTools->IsModeActive(FBuiltinEditorModes::EM_Geometry);

	// Check for special cases (like certain show flags that might hide an actor)
	bool bActorIsHiddenByShowFlags = false;

	// Check to see that volume actors are visible in the viewport
// 	check(LevelViewportClient != nullptr);
// 	if( InActor.IsA(AVolume::StaticClass()) && (!LevelViewportClient->EngineShowFlags.Volumes || !LevelViewportClient->IsVolumeVisibleInViewport(InActor) ) )
// 	{
// 		bActorIsHiddenByShowFlags = true;
// 	}

	// Never drag-select hidden actors or builder brushes. 
	if( !bActorIsHiddenByShowFlags && !InActor.bHidden /*&& !FActorEditorUtils::IsABuilderBrush(&InActor)*/ )
	{
		// Iterate over all actor components, selecting out primitive components
		TInlineComponentArray<UPrimitiveComponent*> Components;
		InActor.GetComponents(Components);

		for (const UPrimitiveComponent* PrimitiveComponent : Components)
		{
			check(PrimitiveComponent != nullptr);
			if ( PrimitiveComponent->IsRegistered() && PrimitiveComponent->IsVisibleInEditor() )
			{
				if (!bGeometryMode)
				{
					bool bIsFullyContained;
					FVector TheOrigin, TheExtent;
					InActor.GetActorBounds(false, TheOrigin, TheExtent);
					if (InFrustum.IntersectBox(TheOrigin, TheExtent, bIsFullyContained))
					{
						if (bIsFullyContained)
						{
							bActorHitByBox = true;
							break;
						}
					}
				}
// 				if (PrimitiveComponent->ComponentIsTouchingSelectionFrustum(InFrustum, LevelViewportClient->EngineShowFlags, bGeometryMode, bUseStrictSelection))
// 				{
// 					bActorHitByBox = true;
// 					break;
// 				}
			}
		}
	}

	return bActorHitByBox;
}

bool FArmyDragTool_ActorFrustumSelect::IntersectsFrustum( const UModel& InModel, int32 NodeIndex, const FConvexVolume& InFrustum, bool bUseStrictSelection ) const
{
// 	FBox NodeBB;
// 	// Get a bounding box of the node being checked
// 	InModel.GetNodeBoundingBox( InModel.Nodes[NodeIndex], NodeBB );
// 
// 	bool bFullyContained = false;
// 
// 	// Does the box intersect the frustum
// 	bool bIntersects = InFrustum.IntersectBox( NodeBB.GetCenter(), NodeBB.GetExtent(), bFullyContained );
// 
// 	return bIntersects && (!bUseStrictSelection || (bUseStrictSelection && bFullyContained));
	return false;
}

void FArmyDragTool_ActorFrustumSelect::CalculateFrustum( FSceneView* View, FConvexVolume& OutFrustum, bool bUseBoxFrustum )
{
	if( bUseBoxFrustum )
	{
		FVector CamPoint = LevelViewportClient->GetViewLocation();
		FVector BoxPoint1, BoxPoint2, BoxPoint3, BoxPoint4;
		FVector WorldDir1, WorldDir2, WorldDir3, WorldDir4;
		// Deproject the four corners of the selection box
		FVector2D Point1(FMath::Min(Start.X, End.X), FMath::Min(Start.Y, End.Y)); // Upper Left Corner
		FVector2D Point2(FMath::Max(Start.X, End.X), FMath::Min(Start.Y, End.Y)); // Upper Right Corner
		FVector2D Point3(FMath::Max(Start.X, End.X), FMath::Max(Start.Y, End.Y)); // Lower Right Corner
		FVector2D Point4(FMath::Min(Start.X, End.X), FMath::Max(Start.Y, End.Y)); // Lower Left Corner
		View->DeprojectFVector2D(Point1, BoxPoint1, WorldDir1);
		View->DeprojectFVector2D(Point2, BoxPoint2, WorldDir2);
		View->DeprojectFVector2D(Point3, BoxPoint3, WorldDir3);
		View->DeprojectFVector2D(Point4, BoxPoint4, WorldDir4);
		// Use the camera position and the selection box to create the bounding planes
		FPlane TopPlane(BoxPoint1, BoxPoint2, CamPoint); // Top Plane
		FPlane RightPlane(BoxPoint2, BoxPoint3, CamPoint); // Right Plane
		FPlane BottomPlane(BoxPoint3, BoxPoint4, CamPoint); // Bottom Plane
		FPlane LeftPlane(BoxPoint4, BoxPoint1, CamPoint); // Left Plane

		// Try to get all six planes to create a frustum
		FPlane NearPlane;
		FPlane FarPlane;
		OutFrustum.Planes.Empty();
		if ( View->ViewMatrices.GetViewProjectionMatrix().GetFrustumNearPlane(NearPlane) )
		{
			OutFrustum.Planes.Add(NearPlane);
		}
		if ( View->ViewMatrices.GetViewProjectionMatrix().GetFrustumFarPlane(FarPlane) )
		{
			OutFrustum.Planes.Add(FarPlane);
		}
		OutFrustum.Planes.Add(TopPlane);
		OutFrustum.Planes.Add(RightPlane);
		OutFrustum.Planes.Add(BottomPlane);
		OutFrustum.Planes.Add(LeftPlane);
		OutFrustum.Init();
	}
	else
	{
		OutFrustum = View->ViewFrustum;
		OutFrustum.Init();
	}
}

// void FArmyDragTool_ActorFrustumSelect::AddHoverEffect( AActor& InActor )
// {
// 	FViewportHoverTarget HoverTarget( &InActor );
// 	FLevelEditorViewportClient::AddHoverEffect( HoverTarget );
// 	FLevelEditorViewportClient::HoveredObjects.Add( HoverTarget );
// }
// 
// void FArmyDragTool_ActorFrustumSelect::RemoveHoverEffect( AActor& InActor  )
// {
// 	FViewportHoverTarget HoverTarget( &InActor );
// 	FSetElementId Id = FLevelEditorViewportClient::HoveredObjects.FindId( HoverTarget );
// 	if( Id.IsValidId() )
// 	{
// 		FLevelEditorViewportClient::RemoveHoverEffect( HoverTarget );
// 		FLevelEditorViewportClient::HoveredObjects.Remove( Id );
// 	}
// }
// 
// void FArmyDragTool_ActorFrustumSelect::AddHoverEffect( UModel& InModel, int32 SurfIndex )
// {
// 	FViewportHoverTarget HoverTarget( &InModel, SurfIndex );
// 	FLevelEditorViewportClient::AddHoverEffect( HoverTarget );
// 	FLevelEditorViewportClient::HoveredObjects.Add( HoverTarget );
// }
// 
// void FArmyDragTool_ActorFrustumSelect::RemoveHoverEffect( UModel& InModel, int32 SurfIndex )
// {
// 	FViewportHoverTarget HoverTarget( &InModel, SurfIndex );
// 	FSetElementId Id = FLevelEditorViewportClient::HoveredObjects.FindId( HoverTarget );
// 	if( Id.IsValidId() )
// 	{
// 		FLevelEditorViewportClient::RemoveHoverEffect( HoverTarget );
// 		FLevelEditorViewportClient::HoveredObjects.Remove( Id );
// 	}
// }
