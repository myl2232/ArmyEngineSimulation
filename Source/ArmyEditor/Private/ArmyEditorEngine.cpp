#include "ArmyEditorEngine.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Selection.h"
#include "ArmyEditorModeManager.h"
#include "ArmyGroupActor.h"
#include "ArmyEditor.h"
#include "Engine/StaticMeshActor.h"
#include "ArmySpotLightActor.h"
#include "ArmyPointLightActor.h"  
#include "ArmyEditorViewportClient.h"
#include "Slate/SGameLayerManager.h"
#include "SArmyEditorViewport.h"
#include "SDockTab.h"
#include "ArmyThumbnailManager.h"
#include "ArmyResourceModule.h"

// 模型离地的距离
static const FVector GModelZOffset = FVector(0, 0, 1);

UArmyEditorEngine* GArmyEditor = nullptr;

//////////////////////////////////////////////////////////////////////////
// FCachedActorLabels

// FCachedActorLabels::FCachedActorLabels()
// {
// 
// }
// 
// FCachedActorLabels::FCachedActorLabels(UWorld* World, const TSet<AActor*>& IgnoredActors)
// {
// 	Populate(World, IgnoredActors);
// }
// 
// void FCachedActorLabels::Populate(UWorld* World, const TSet<AActor*>& IgnoredActors)
// {
// 	ActorLabels.Empty();
// 
// 	for (FActorIterator It(World); It; ++It)
// 	{
// 		if (!IgnoredActors.Contains(*It))
// 		{
// 			ActorLabels.Add(It->GetActorLabel());
// 		}
// 	}
// 	ActorLabels.Shrink();
// }

static inline USelection*& PrivateGetSelectedActors()
{
	static USelection* SSelectedActors = NULL;
	return SSelectedActors;
};

static inline USelection*& PrivateGetSelectedComponents()
{
	static USelection* SSelectedComponents = NULL;
	return SSelectedComponents;
}

static inline USelection*& PrivateGetSelectedObjects()
{
	static USelection* SSelectedObjects = NULL;
	return SSelectedObjects;
};

static void OnObjectSelected(UObject* Object)
{
	// Whenever an actor is unselected we must remove its components from the components selection
	if (!Object->IsSelected())
	{
		TArray<UActorComponent*> ComponentsToDeselect;
		for (FSelectionIterator It(*PrivateGetSelectedComponents()); It; ++It)
		{
			UActorComponent* Component = CastChecked<UActorComponent>(*It);
			if (Component->GetOwner() == Object)
			{
				ComponentsToDeselect.Add(Component);
			}
		}
		if (ComponentsToDeselect.Num() > 0)
		{
			PrivateGetSelectedComponents()->Modify();
			PrivateGetSelectedComponents()->BeginBatchSelectOperation();
			for (UActorComponent* Component : ComponentsToDeselect)
			{
				PrivateGetSelectedComponents()->Deselect(Component);
			}
			PrivateGetSelectedComponents()->EndBatchSelectOperation();
		}
	}
}

void UArmyEditorEngine::Init(IEngineLoop* InEngineLoop)
{
	/*TSharedPtr<SWindow> WindowToResize = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (WindowToResize.IsValid())
	WindowToResize->Maximize();
	FVector2D DrawSize = WindowToResize->GetWindowGeometryInScreen().GetDrawSize();
	FSystemResolution::RequestResolutionChange(DrawSize.X, DrawSize.Y, WindowToResize->GetWindowMode());*/
	GArmyEditor = this;
	InitEditor(InEngineLoop);

	GEngine->SetSubduedSelectionOutlineColor(FLinearColor(0.58f, 0.42f, 0.26f, 1.f));
}

void UArmyEditorEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds, bIdleMode);

	UArmyEditorViewportClient* EditorViewportClient = Cast<UArmyEditorViewportClient>(GameViewport);

	if (EditorViewportClient != nullptr &&
		EditorViewportClient->Viewport->GetSizeXY().X > 0 &&
		EditorViewportClient->IsVisible())
	{
		EditorViewportClient->Tick(DeltaSeconds);
		EditorViewportClient->Viewport->Draw();

		if (EditorViewportClient->bNeedsInvalidateHitProxy)
		{
			EditorViewportClient->Viewport->InvalidateHitProxy();
			EditorViewportClient->bNeedsInvalidateHitProxy = false;
		}
	}
}

void UArmyEditorEngine::RedrawViewports(bool bShouldPresent /*= true*/)
{
	//if (GCurrentLevelEditingViewportClient->IsVisible())
	//{
	//	Super::RedrawViewports(bShouldPresent);
 //		if (GCurrentLevelEditingViewportClient->bNeedsInvalidateHitProxy)
 //		{
 //			GCurrentLevelEditingViewportClient->Viewport->InvalidateHitProxy();
 //			GCurrentLevelEditingViewportClient->bNeedsInvalidateHitProxy = false;
 //		}
	//}
}

static void PrivateInitSelectedSets()
{
	PrivateGetSelectedActors() = NewObject<USelection>(GetTransientPackage(), TEXT("SelectedActors"), RF_Transactional);
	PrivateGetSelectedActors()->AddToRoot();
	PrivateGetSelectedActors()->Initialize(&GSelectedActorAnnotation);

	PrivateGetSelectedActors()->SelectObjectEvent.AddStatic(&OnObjectSelected);

	PrivateGetSelectedComponents() = NewObject<USelection>(GetTransientPackage(), TEXT("SelectedComponents"), RF_Transactional);
	PrivateGetSelectedComponents()->AddToRoot();
	PrivateGetSelectedComponents()->Initialize(&GSelectedComponentAnnotation);

	PrivateGetSelectedObjects() = NewObject<USelection>(GetTransientPackage(), TEXT("SelectedObjects"), RF_Transactional);
	PrivateGetSelectedObjects()->AddToRoot();
	PrivateGetSelectedObjects()->Initialize(&GSelectedObjectAnnotation);
}

void UArmyEditorEngine::InitEditor(IEngineLoop* InEngineLoop)
{
	PrivateInitSelectedSets();

	UGameEngine::Init(InEngineLoop);
}

void UArmyEditorEngine::CreateGameViewportWidget(UGameViewportClient* GameViewportClient)
{
	bool bRenderDirectlyToWindow = false;// !StartupMovieCaptureHandle.IsValid() && GIsDumpingMovie == 0;
	const bool bStereoAllowed = true;// bRenderDirectlyToWindow;
	TSharedRef<SOverlay> ViewportOverlayWidgetRef = SNew(SOverlay);

	TSharedRef<SGameLayerManager> GameLayerManagerRef = SNew(SGameLayerManager)
		.SceneViewport_UObject(this, &UArmyEditorEngine::GetGameSceneViewport, GameViewportClient)
		[
			ViewportOverlayWidgetRef
		];

	//@  欧石楠 创建Viewport的时候设置为不要直接渲染到窗口
	bRenderDirectlyToWindow = false;

	UArmyEditorViewportClient* ArmyEditorViewportClient = Cast<UArmyEditorViewportClient>(GameViewportClient);

	GameViewportWidget =
		SNew(SArmyEditorViewport)
		.ViewportClient(ArmyEditorViewportClient)
		.RenderDirectlyToWindow(bRenderDirectlyToWindow)
		.EnableGammaCorrection(false)
		.EnableStereoRendering(bStereoAllowed);

	GameViewportWidget->SetContent(GameLayerManagerRef);
	GameViewportClient->SetViewportOverlayWidget(GameViewportWindow.Pin(), ViewportOverlayWidgetRef);
	GameViewportClient->SetGameLayerManager(GameLayerManagerRef);
}

void UArmyEditorEngine::SelectGroup(class AArmyGroupActor* InGroupActor, bool bForceSelection /*= false*/, bool bInSelected /*= true*/, bool bNotify /*= true*/)
{
	USelection* SelectedActors = GetSelectedActors();
	//SelectedActors->BeginBatchSelectOperation();
	SelectedActors->Modify();

	static bool bIteratingGroups = false;

	if (!bIteratingGroups)
	{
		bIteratingGroups = true;
		// Select all actors within the group (if locked or forced)
		if (bForceSelection || InGroupActor->IsLocked())
		{
			TArray<AActor*> GroupActors;
			InGroupActor->GetGroupActors(GroupActors);
			for (int32 ActorIndex = 0; ActorIndex < GroupActors.Num(); ++ActorIndex)
			{
				SelectActor(GroupActors[ActorIndex], bInSelected, false);
			}
			bForceSelection = true;

			// Recursively select any subgroups
			TArray<AArmyGroupActor*> SubGroups;
			InGroupActor->GetSubGroups(SubGroups);
			for (int32 GroupIndex = 0; GroupIndex < SubGroups.Num(); ++GroupIndex)
			{
				SelectGroup(SubGroups[GroupIndex], bForceSelection, bInSelected, false);
			}
		}

		//SelectedActors->EndBatchSelectOperation(bNotify);
		if (bNotify)
		{
			NoteSelectionChange();
		}

		//whenever selection changes, recompute whether the selection contains a locked actor
		//bCheckForLockActors = true;

		//whenever selection changes, recompute whether the selection contains a world info actor
		//bCheckForWorldSettingsActors = true;

		bIteratingGroups = false;
	}
}

void UArmyEditorEngine::SetPivot(FVector NewPivot, bool bSnapPivotToGrid, bool bIgnoreAxis, bool bAssignPivot /*= false*/)
{
	FArmyEditorModeTools& EditorModeTools = GArmyLevelEditorModeTools();

	if (!bIgnoreAxis)
	{
		// Don't stomp on orthonormal axis.
		// TODO: this breaks if there is genuinely a need to set the pivot to a coordinate containing a zero component
		if (NewPivot.X == 0) NewPivot.X = EditorModeTools.PivotLocation.X;
		if (NewPivot.Y == 0) NewPivot.Y = EditorModeTools.PivotLocation.Y;
		if (NewPivot.Z == 0) NewPivot.Z = EditorModeTools.PivotLocation.Z;
	}

	// Set the pivot.
	EditorModeTools.SetPivotLocation(NewPivot, false);

	// 	if (bSnapPivotToGrid)
	// 	{
	// 		FRotator DummyRotator(0, 0, 0);
	// 		FSnappingUtils::SnapToBSPVertex(EditorModeTools.SnappedLocation, EditorModeTools.GridBase, DummyRotator);
	// 		EditorModeTools.PivotLocation = EditorModeTools.SnappedLocation;
	// 	}
	// 
	// 	// Check all actors.
	// 	int32 Count = 0, SnapCount = 0;
	// 
	// 	//default to using the x axis for the translate rotate widget
	// 	EditorModeTools.TranslateRotateXAxisAngle = 0.0f;
	// 	EditorModeTools.TranslateRotate2DAngle = 0.0f;
	// 	FVector TranslateRotateWidgetWorldXAxis;
	// 
	// 	FVector Widget2DWorldXAxis;
	// 
	// 	AActor* LastSelectedActor = NULL;
	// 	for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
	// 	{
	// 		AActor* Actor = static_cast<AActor*>(*It);
	// 		checkSlow(Actor->IsA(AActor::StaticClass()));
	// 
	// 		if (Count == 0)
	// 		{
	// 			TranslateRotateWidgetWorldXAxis = Actor->ActorToWorld().TransformVector(FVector(1.0f, 0.0f, 0.0f));
	// 			//get the xy plane project of this vector
	// 			TranslateRotateWidgetWorldXAxis.Z = 0.0f;
	// 			if (!TranslateRotateWidgetWorldXAxis.Normalize())
	// 			{
	// 				TranslateRotateWidgetWorldXAxis = FVector(1.0f, 0.0f, 0.0f);
	// 			}
	// 
	// 			Widget2DWorldXAxis = Actor->ActorToWorld().TransformVector(FVector(1, 0, 0));
	// 			Widget2DWorldXAxis.Y = 0;
	// 			if (!Widget2DWorldXAxis.Normalize())
	// 			{
	// 				Widget2DWorldXAxis = FVector(1, 0, 0);
	// 			}
	// 		}
	// 
	// 		LastSelectedActor = Actor;
	// 		++Count;
	// 		++SnapCount;
	// 	}
	// 
	// 	if (bAssignPivot && LastSelectedActor && GEditor->bGroupingActive)
	// 	{
	// 		// set group pivot for the root-most group
	// 		AGroupActor* ActorGroupRoot = AGroupActor::GetRootForActor(LastSelectedActor, true, true);
	// 		if (ActorGroupRoot)
	// 		{
	// 			ActorGroupRoot->SetActorLocation(EditorModeTools.PivotLocation, false);
	// 		}
	// 	}
	// 
	// 	//if there are multiple actors selected, just use the x-axis for the "translate/rotate" or 2D widgets
	// 	if (Count == 1)
	// 	{
	// 		EditorModeTools.TranslateRotateXAxisAngle = TranslateRotateWidgetWorldXAxis.Rotation().Yaw;
	// 		EditorModeTools.TranslateRotate2DAngle = FMath::RadiansToDegrees(FMath::Atan2(Widget2DWorldXAxis.Z, Widget2DWorldXAxis.X));
	// 	}
	// 
	// 	// Update showing.
	// 	EditorModeTools.PivotShown = SnapCount > 0 || Count > 1;
}

class USelection* UArmyEditorEngine::GetSelectedActors() const
{
	return PrivateGetSelectedActors();
}

void UArmyEditorEngine::GetSelectedActors(TArray<AActor*>& OutActors)
{
	OutActors.Reset();
	for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
	{
		if (AActor* Actor = Cast<AActor>(*It))
		{
			OutActors.Add(Actor);
		}
	}
}

class USelection* UArmyEditorEngine::GetSelectedComponents() const
{
	return PrivateGetSelectedComponents();
}

class USelection* UArmyEditorEngine::GetSelectedObjects() const
{
	return PrivateGetSelectedObjects();
}

class FSelectionIterator UArmyEditorEngine::GetSelectedActorIterator() const
{
	return FSelectionIterator(*GetSelectedActors());
}

class FSelectionIterator UArmyEditorEngine::GetSelectedComponentIterator() const
{
	return FSelectionIterator(*GetSelectedComponents());

}

int32 UArmyEditorEngine::GetSelectedComponentCount() const
{
	int32 NumSelectedComponents = 0;
	for (FSelectionIterator It(GetSelectedComponentIterator()); It; ++It)
	{
		++NumSelectedComponents;
	}

	return NumSelectedComponents;
}

int32 UArmyEditorEngine::GetSelectedActorCount() const
{
	int32 NumSelectedActors = 0;
	for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
	{
		++NumSelectedActors;
	}

	return NumSelectedActors;
}

bool UArmyEditorEngine::CanAllSelectedBeMoved()
{
	bool result = true;
	//当前不支持任何Component的移动
	if (GetSelectedComponentCount() > 0)
	{
		result = false;
	}

	int32 NumSelectedActor = 0;
	for (FSelectionIterator It(*GetSelectedActors()); It; ++It)
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(*It);
		AActor* Actor = Cast<AActor>(*It);
		if (SMA)
        {
            //只有StaticMeshActor，并且Tag为可移动的家具，才会被人为是可以选择并能通过坐标系移动的物体
            if (SMA->Tags.Num() > 0 && SMA->Tags[0] == TEXT("MoveableMeshActor"))
            {
            }
            else
            {
                result = false;
            }

			//停用的不可移动
			if (Actor->Tags.Num() > 0 && Actor->Tags.Contains(TEXT("NoWidget")))
			{
				result = false;
			}

        }
        else if (Actor)
        {
            // 3d模式下选中门不显示坐标轴
            if (Actor->ActorHasTag("Door") || Actor->ActorHasTag("SecurityDoor"))
            {
                result = false;
            }

            //只有Actor，并且Tag为可移动的蓝图交互物体，才会被人为是可以选择并能通过坐标系移动的物体
            if (Actor->Tags.Num() > 0 && (Actor->Tags[0] == TEXT("BlueprintVRSActor") || Actor->Tags[0] == TEXT("EnvironmentAsset")) || Actor->Tags.Contains(TEXT("HydropowerActor")) || Actor->Tags.Contains(TEXT("HydropowerPipeActor")))
            {
            }
            else
            {
                result = false;
            }


        }
		NumSelectedActor++;
	}
	//如果没有选中任何Actor，则一定为False
	if (NumSelectedActor == 0)
		result = false;



	return result;
}

bool UArmyEditorEngine::AllSelectedWithDetail()
{
	bool result = true;
	//当前不支持任何Component的移动
	if (GetSelectedComponentCount() > 0)
	{
		result = false;
	}

	int32 NumSelectedActor = 0;
	for (FSelectionIterator It(*GetSelectedActors()); It; ++It)
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(*It);
		AActor* Actor = Cast<AActor>(*It);
		if (SMA)
		{
			//只有StaticMeshActor，并且Tag为可移动的家具，才会被人为是可以选择并能通过坐标系移动的物体
			if (SMA->Tags.Num() > 0 && SMA->Tags[0] == TEXT("MoveableMeshActor"))
			{
			}
			else
			{
				result = false;
			}
		}
		else if (Actor)
		{
			//只有Actor，并且Tag为可移动的蓝图交互物体，才会被人为是可以选择并能通过坐标系移动的物体
			if (Actor->Tags.Num() > 0 &&
				(Actor->Tags[0] == TEXT("BlueprintVRSActor") || Actor->Tags[0].ToString().StartsWith(TEXT("EnvironmentAsset")))
				)
			{
			}
			else
			{
				result = false;
			}
		}
		NumSelectedActor++;
	}
	//如果没有选中任何Actor，则一定为False
	if (NumSelectedActor == 0)
		result = false;

	return result;
}

AActor* UArmyEditorEngine::GetSingleSelectedActor()
{
	int32 NumSelectedActor = 0;
	AActor* SingleActor = NULL;
	for (FSelectionIterator It(*GetSelectedActors()); It; ++It)
	{
		AActor* Actor = Cast<AActor>(*It);
		if (Actor)
		{
			SingleActor = Actor;
		}
		NumSelectedActor++;
	}
	//如果没有选中任何Actor，则一定为False
	if (NumSelectedActor != 1)
		SingleActor = NULL;

	return SingleActor;
}

bool UArmyEditorEngine::IsMaterialSelected()
{
	TArray<AActor*> Actors;
	if (SelectedMaterial)
	{
		return true;
	}
	else
	{
		for (FSelectionIterator It(*GetSelectedActors()); It; ++It)
		{
			AActor* Actor = Cast<AActor>(*It);
			if (Actor)
			{
				Actors.Add(Actor);
			}
			else
			{
				return false;
			}
		}
		if (Actors.Num() == 1)
		{
			AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Actors[0]);
			if (SMA)
			{
				//可以动的家具直接返回false
				if (SMA->Tags.Num() > 0 && SMA->Tags[0] == TEXT("MoveableMeshActor"))
				{
					return false;
				}
				//静态墙体，拾取Material0，返回true
				FString PathName = SMA->GetStaticMeshComponent()->GetStaticMesh()->GetPathName();
				if (PathName.EndsWith(TEXT("_F")) ||
					PathName.EndsWith(TEXT("_W")) ||
					PathName.EndsWith(TEXT("_T")))
				{
					SelectedMaterial = SMA->GetStaticMeshComponent()->GetMaterial(0);
					SelectedMaterialOwnerSMC = SMA->GetStaticMeshComponent();
					return true;
				}
			}
		}
	}

	return false;
}

FBox UArmyEditorEngine::GetSelectedActorsBox()
{
	FBox SelectActorsBox(ForceInit);
	if (GetSelectedActorsBoundingBoxDelegate.IsBound())
	{
		SelectActorsBox = GetSelectedActorsBoundingBoxDelegate.Execute();
	}
	return SelectActorsBox;
}

void UArmyEditorEngine::ApplyDeltaToActor(AActor* InActor, bool bDelta, const FVector* InTrans, const FRotator* InRot, const FVector* InScale, bool bAltDown /*= false*/, bool bShiftDown /*= false*/, bool bControlDown /*= false*/) const
{
	bool bTranslationOnly = true;

	///////////////////
	// Rotation

	// Unfortunately this can't be moved into ABrush::EditorApplyRotation, as that would
	// create a dependence in Engine on Editor.
	if (InRot)
	{
		const FRotator& InDeltaRot = *InRot;
		const bool bRotatingActor = !bDelta || !InDeltaRot.IsZero();
		if (bRotatingActor)
		{
			bTranslationOnly = false;

			if (bDelta)
			{
				if (InActor->GetRootComponent() != NULL)
				{
					const FRotator OriginalRotation = InActor->GetRootComponent()->GetComponentRotation();

					GArmyEditor->EditorApplyRotation(InActor, InDeltaRot, bAltDown, bShiftDown, bControlDown);

					// Check to see if we should transform the rigid body
// 					UPrimitiveComponent* RootPrimitiveComponent = Cast< UPrimitiveComponent >(InActor->GetRootComponent());
// 					if (bIsSimulatingInEditor && GIsPlayInEditorWorld && RootPrimitiveComponent != NULL)
// 					{
// 						FRotator ActorRotWind, ActorRotRem;
// 						OriginalRotation.GetWindingAndRemainder(ActorRotWind, ActorRotRem);
// 
// 						const FQuat ActorQ = ActorRotRem.Quaternion();
// 						const FQuat DeltaQ = InDeltaRot.Quaternion();
// 						const FQuat ResultQ = DeltaQ * ActorQ;
// 
// 						const FRotator NewActorRotRem = FRotator(ResultQ);
// 						FRotator DeltaRot = NewActorRotRem - ActorRotRem;
// 						DeltaRot.Normalize();
// 
// 						// @todo SIE: Not taking into account possible offset between root component and actor
// 						RootPrimitiveComponent->SetWorldRotation(OriginalRotation + DeltaRot);
// 					}
				}

				FVector NewActorLocation = InActor->GetActorLocation();
				NewActorLocation -= GArmyLevelEditorModeTools().PivotLocation;
				NewActorLocation = FRotationMatrix(InDeltaRot).TransformPosition(NewActorLocation);
				NewActorLocation += GArmyLevelEditorModeTools().PivotLocation;
				NewActorLocation -= InActor->GetActorLocation();
				GArmyEditor->EditorApplyTranslation(InActor, NewActorLocation, bAltDown, bShiftDown, bControlDown);
			}
			else
			{
				InActor->SetActorRotation(InDeltaRot);
			}
		}
	}
	if (InTrans)
	{
		if (bDelta)
		{
			if (InActor->GetRootComponent() != NULL)
			{
				const FVector OriginalLocation = InActor->GetRootComponent()->GetComponentLocation();

				GArmyEditor->EditorApplyTranslation(InActor, *InTrans, bAltDown, bShiftDown, bControlDown);


				// Check to see if we should transform the rigid body
// 				UPrimitiveComponent* RootPrimitiveComponent = Cast< UPrimitiveComponent >(InActor->GetRootComponent());
// 				if (bIsSimulatingInEditor && GIsPlayInEditorWorld && RootPrimitiveComponent != NULL)
// 				{
// 					// @todo SIE: Not taking into account possible offset between root component and actor
// 					RootPrimitiveComponent->SetWorldLocation(OriginalLocation + *InTrans);
// 				}
			}
		}
		else
		{
			InActor->SetActorLocation(*InTrans, false);
		}
	}

	///////////////////
	// Scaling
	if (InScale)
	{
		const FVector& InDeltaScale = *InScale;
		const bool bScalingActor = !bDelta || !InDeltaScale.IsNearlyZero(0.000001f);
		if (bScalingActor)
		{
			bTranslationOnly = false;

			FVector ModifiedScale = InDeltaScale;

			// Note: With the new additive scaling method, this is handled in FLevelEditorViewportClient::ModifyScale
			if (false/*GEditor->UsePercentageBasedScaling()*/)
			{
				// Get actor box extents
				const FBox BoundingBox = InActor->GetComponentsBoundingBox(true);
				const FVector BoundsExtents = BoundingBox.GetExtent();

				// Make sure scale on actors is clamped to a minimum and maximum size.
				const float MinThreshold = 1.0f;

				for (int32 Idx = 0; Idx < 3; Idx++)
				{
					if ((FMath::Pow(BoundsExtents[Idx], 2)) > BIG_NUMBER)
					{
						ModifiedScale[Idx] = 0.0f;
					}
					else if (SMALL_NUMBER < BoundsExtents[Idx])
					{
						const bool bBelowAllowableScaleThreshold = ((InDeltaScale[Idx] + 1.0f) * BoundsExtents[Idx]) < MinThreshold;

						if (bBelowAllowableScaleThreshold)
						{
							ModifiedScale[Idx] = (MinThreshold / BoundsExtents[Idx]) - 1.0f;
						}
					}
				}
			}

			if (bDelta)
			{
				// Flag actors to use old-style scaling or not
				// @todo: Remove this hack once we have decided on the scaling method to use.
				//AActor::bUsePercentageBasedScaling = GEditor->UsePercentageBasedScaling();

				GArmyEditor->EditorApplyScale(
					InActor,
					ModifiedScale,
					&GArmyLevelEditorModeTools().PivotLocation,
					bAltDown,
					bShiftDown,
					bControlDown
				);

			}
			else if (InActor->GetRootComponent() != NULL)
			{
				InActor->GetRootComponent()->SetRelativeScale3D(InDeltaScale);
			}
		}
	}

	InActor->MarkPackageDirty();
	InActor->InvalidateLightingCacheDetailed(bTranslationOnly);
	InActor->PostEditMove(false);

	//回调Actor被修改。用来刷新UI
	GArmyEditor->SelectionEditedEvent.ExecuteIfBound(InActor);
}

void UArmyEditorEngine::EditorApplyTranslation(AActor* InActor, const FVector& DeltaTranslation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	InActor->EditorApplyTranslation(DeltaTranslation, bAltDown, bShiftDown, bCtrlDown);
	//if (InActor->GetRootComponent() != NULL)
	//{
	//	InActor->GetRootComponent()->SetWorldLocation(InActor->GetRootComponent()->GetComponentLocation() + DeltaTranslation);
	//}
}

void UArmyEditorEngine::EditorApplyRotation(AActor* InActor, const FRotator& DeltaRotation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	USceneComponent* RootComponent = InActor->GetRootComponent();
	if (RootComponent != NULL)
	{
		const FRotator Rot = RootComponent->GetAttachParent() != NULL ? InActor->GetActorRotation() : RootComponent->RelativeRotation;

		FRotator ActorRotWind, ActorRotRem;
		Rot.GetWindingAndRemainder(ActorRotWind, ActorRotRem);

		const FQuat ActorQ = ActorRotRem.Quaternion();
		const FQuat DeltaQ = DeltaRotation.Quaternion();
		const FQuat ResultQ = DeltaQ * ActorQ;
		const FRotator NewActorRotRem = FRotator(ResultQ);
		FRotator DeltaRot = NewActorRotRem - ActorRotRem;
		DeltaRot.Normalize();

		if (RootComponent->GetAttachParent() != NULL)
		{
			RootComponent->SetWorldRotation(Rot + DeltaRot);
		}
		else
		{
			// No attachment.  Directly set relative rotation (to support winding)
			RootComponent->SetRelativeRotation(Rot + DeltaRot);
		}
	}
}

void UArmyEditorEngine::EditorApplyScale(AActor* InActor, const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	USceneComponent* RootComponent = InActor->GetRootComponent();
	if (RootComponent != NULL)
	{
		const FVector CurrentScale = RootComponent->RelativeScale3D;

		// @todo: Remove this hack once we have decided on the scaling method to use.
		FVector ScaleToApply;

		//if (AActor::bUsePercentageBasedScaling)
		//{
		//	ScaleToApply = CurrentScale * (FVector(1.0f) + DeltaScale);
		//}
		//else
		{
			ScaleToApply = CurrentScale + DeltaScale;
		}

		RootComponent->SetRelativeScale3D(ScaleToApply);

		if (PivotLocation)
		{
			const FVector CurrentScaleSafe(CurrentScale.X ? CurrentScale.X : 1.0f,
				CurrentScale.Y ? CurrentScale.Y : 1.0f,
				CurrentScale.Z ? CurrentScale.Z : 1.0f);

			FVector Loc = InActor->GetActorLocation();
			Loc -= *PivotLocation;
			Loc *= (ScaleToApply / CurrentScaleSafe);
			Loc += *PivotLocation;
			RootComponent->SetWorldLocation(Loc);
		}
	}
}

void UArmyEditorEngine::EditorApplyMirror(AActor* InActor, const FVector& MirrorScale, const FVector& PivotLocation)
{
	const FRotationMatrix TempRot(InActor->GetActorRotation());
	const FVector New0(TempRot.GetScaledAxis(EAxis::X) * MirrorScale);
	const FVector New1(TempRot.GetScaledAxis(EAxis::Y) * MirrorScale);
	const FVector New2(TempRot.GetScaledAxis(EAxis::Z) * MirrorScale);
	// Revert the handedness of the rotation, but make up for it in the scaling.
	// Arbitrarily choose the X axis to remain fixed.
	const FMatrix NewRot(-New0, New1, New2, FVector::ZeroVector);

	if (InActor->GetRootComponent() != NULL)
	{
		InActor->GetRootComponent()->SetRelativeRotation(NewRot.Rotator());
		FVector Loc = InActor->GetActorLocation();
		Loc -= PivotLocation;
		Loc *= MirrorScale;
		Loc += PivotLocation;
		InActor->GetRootComponent()->SetRelativeLocation(Loc);

		FVector Scale3D = InActor->GetRootComponent()->RelativeScale3D;
		Scale3D.X = -Scale3D.X;

		if (!InActor->IsA(AArmyPointLightActor::StaticClass()) && !InActor->IsA(AArmySpotLightActor::StaticClass()))
			InActor->GetRootComponent()->SetRelativeScale3D(Scale3D);

		//更改Scale之后要MarkRenderStateDirty，否则会反面
		InActor->GetRootComponent()->MarkRenderStateDirty();
		TArray<USceneComponent*> COMs;
		InActor->GetRootComponent()->GetChildrenComponents(true, COMs);
		for (auto& It : COMs)
		{
			UActorComponent* ACom = Cast<UActorComponent>(It);
			if (ACom)
			{
				ACom->MarkRenderStateDirty();
			}
		}
	}
}

void UArmyEditorEngine::MirrorActors(const FVector& MirrorScale)
{
	for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
	{
		AActor* Actor = static_cast<AActor*>(*It);
		checkSlow(Actor->IsA(AActor::StaticClass()));

		const FVector PivotLocation = GArmyLevelEditorModeTools().PivotLocation;
		Actor->Modify();
		EditorApplyMirror(Actor, MirrorScale, PivotLocation);
	}
}

void UArmyEditorEngine::UpdatePivotLocationForSelection(bool bOnChange /*= false*/)
{
	// Pick a new common pivot, or not.
	AActor* SingleActor = nullptr;
	USceneComponent* SingleComponent = nullptr;

	 	if (GetSelectedComponentCount() > 0)
	 	{
	 		for (FSelectedEditableComponentIterator It(*GetSelectedComponents()); It; ++It)
	 		{
	 			UActorComponent* Component = CastChecked<UActorComponent>(*It);
	 			AActor* ComponentOwner = Component->GetOwner();
	 
	 			if (ComponentOwner != nullptr)
	 			{
	 				auto SelectedActors = GetSelectedActors();
	 				const bool bIsOwnerSelected = SelectedActors->IsSelected(ComponentOwner);
	 				check(bIsOwnerSelected);
	 
	 				if (ComponentOwner->GetWorld() == GWorld)
	 				{
	 					SingleActor = ComponentOwner;
	 					if (Component->IsA<USceneComponent>())
	 					{
	 						SingleComponent = CastChecked<USceneComponent>(Component);
	 					}
	 
	 					//const bool IsTemplate = ComponentOwner->IsTemplate();
	 					//const bool LevelLocked = !FLevelUtils::IsLevelLocked(ComponentOwner->GetLevel());
	 					//check(IsTemplate || LevelLocked);
	 				}
	 			}
	 		}
	 	}
	 	else
	{
		for (FSelectionIterator It(GetSelectedActorIterator()); It; ++It)
		{
			AActor* Actor = static_cast<AActor*>(*It);
			checkSlow(Actor->IsA(AActor::StaticClass()));

			if (Actor->GetWorld() == GWorld)
			{
				//const bool IsTemplate = Actor->IsTemplate();
				//const bool LevelLocked = !FLevelUtils::IsLevelLocked(Actor->GetLevel());
				//check(IsTemplate || LevelLocked);

				SingleActor = Actor;
			}
		}
	}

	if (SingleComponent != NULL)
	{
		SetPivot(SingleComponent->GetComponentLocation(), false, true);
	}
	else if (SingleActor != NULL)
	{
		// For geometry mode use current pivot location as it's set to selected face, not actor
		FArmyEditorModeTools& Tools = GArmyLevelEditorModeTools();
		/*if (Tools.IsModeActive(FBuiltinEditorModes::EM_Geometry) == false || bOnChange == true)*/
		{
			// Set pivot point to the actor's location, accounting for any set pivot offset
			FVector PivotPoint = SingleActor->GetTransform().TransformPosition(FVector(0, 0, 0) /*SingleActor->GetPivotOffset()*/);

			// If grouping is active, see if this actor is part of a locked group and use that pivot instead
			//if (GVRSEditor->bGroupingActive)
			{
				AArmyGroupActor* ActorGroupRoot = AArmyGroupActor::GetRootForActor(SingleActor, true, true);
				if (ActorGroupRoot)
				{
					PivotPoint = ActorGroupRoot->GetActorLocation();
				}
			}
			SetPivot(PivotPoint, false, true);
		}
	}
	else
	{
		//ResetPivot();
	}

	//SetPivotMovedIndependently(false);
	//GArmyVC->Invalidate();
}

float UArmyEditorEngine::GetGridSize()
{
	return 1.f;
}

class UArmyThumbnailManager* UArmyEditorEngine::GetThumbnailManager()
{
	return &(UArmyThumbnailManager::Get());
}

void UArmyEditorEngine::ResetSelectedActor()
{
	AActor* SingleActor = GetSingleSelectedActor();
	if (SingleActor)
	{
		FArmyResourceModule::Get().GetResourceManager()->ClearActorOverridenMaterials(SingleActor);
		FArmyResourceModule::Get().GetResourceManager()->ResetActorTransform(SingleActor);
	}
}

void UArmyEditorEngine::MirrorXSelectedActor()
{
	if (GetSelectedActorCount() > 0 && CanAllSelectedBeMoved())
	{
		MirrorActors(FVector(-1.f, 1.f, 1.f));
	}
}

void UArmyEditorEngine::MirrorYSelectedActor()
{
	if (GetSelectedActorCount() > 0 && CanAllSelectedBeMoved())
	{
		MirrorActors(FVector(1.f, -1.f, 1.f));
	}
}

void UArmyEditorEngine::SnapSelectedActorToGround()
{
	//  TArray<AActor*> SelectedActors;
	//  GetSelectedActors(SelectedActors);
	//  if (SelectedActors.Num() > 0)
	//  {
	//      FVector PivotLoc = GCurrentLevelEditingViewportClient->GetWidgetLocation();
		  //APlayerController* PC = GCurrentLevelEditingViewportClient->GetWorld()->GetFirstPlayerController();
	//      static FName TraceTag = FName(TEXT("Pick"));
	//      FCollisionQueryParams TraceParams(TraceTag, true, PC);
	//      TraceParams.AddIgnoredActor(PC->GetPawn());
	//      for (auto& It : SelectedActors)
	//      {
	//          TraceParams.AddIgnoredActor(It);
	//      }
	//      TraceParams.bTraceAsyncScene = true;
	//      FVector TraceStart, TraceEnd, TargetLocation;
	//      FHitResult Hit(ForceInit);
	//      TraceStart = PivotLoc + FVector(0, 0, 10);
	//      TraceEnd = TraceStart - FVector(0, 0, 1) * 10000.f;
	//      //-----------------------------------------------------------Trace
	//      GCurrentLevelEditingViewportClient->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
	//      if (Hit.bBlockingHit)
	//      {
	//          TargetLocation = Hit.ImpactPoint + GModelZOffset;

	//          FVector Delta = TargetLocation - PivotLoc;
	//          GCurrentLevelEditingViewportClient->ApplyDeltaToActors(Delta, FRotator(0, 0, 0), FVector(0, 0, 0));
	//          GArmyLevelEditorModeTools().SetPivotLocation(GCurrentLevelEditingViewportClient->GetWidgetLocation() + Delta, false);
	//      }
	//  }
}