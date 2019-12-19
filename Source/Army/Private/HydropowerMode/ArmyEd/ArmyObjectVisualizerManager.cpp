// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ArmyObjectVisualizerManager.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "ArmyEditorViewportClient.h"
#include "EngineUtils.h"
#include "ArmyActor.h"
#include "ArmyHydropowerModeController.h"
#include "ArmyGameInstance.h"
#include "Classes/Engine/Selection.h"
#include "ArmyEditorEngine.h"
#include "ArmyGroupActor.h"
#include "ArmyPipeline.h"
#include "ArmyPipePoint.h"

FArmyObjectVisualizerManager::FArmyObjectVisualizerManager()
	: EditedVisualizerViewportClient(nullptr)
{
	USelection::SelectionChangedEvent.AddRaw(this,&FArmyObjectVisualizerManager::OnLevelSelectionChanged);
}

FArmyObjectVisualizerManager::~FArmyObjectVisualizerManager()
{
	USelection::SelectionChangedEvent.RemoveAll(this);
}

/** Handle a click on the specified editor viewport client */
bool FArmyObjectVisualizerManager::HandleClick(UArmyEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FArmyViewportClick& Click)
{
	bool bHandled = HandleProxyForComponentVis(InViewportClient, HitProxy, Click);
	if (bHandled && Click.GetKey() == EKeys::RightMouseButton)
	{
		TSharedPtr<SWidget> MenuWidget = GenerateContextMenuForComponentVis();
		if (MenuWidget.IsValid())
		{
			TSharedPtr<SArmyEditorViewport> ViewportWidget = InViewportClient->EditorViewportWidget.Pin();
			if(ViewportWidget.IsValid ())
			{
				FSlateApplication::Get ().PushMenu (
					ViewportWidget.ToSharedRef (),
					FWidgetPath (),
					MenuWidget.ToSharedRef (),
					FSlateApplication::Get ().GetCursorPos (),
					FPopupTransitionEffect (FPopupTransitionEffect::ContextMenu));

				return true;
			}
		}
	}

	return false;
}

bool FArmyObjectVisualizerManager::HandleProxyForComponentVis(UArmyEditorViewportClient* InViewportClient, HHitProxy* HitProxy, const FArmyViewportClick& Click)
{ 

		if (EditedVisualizerPtr.IsValid())
		{
			HXRObjectVisProxy* VisProxy = nullptr;
			if (HitProxy && HitProxy->IsA(HXRObjectVisProxy::StaticGetType()))
			{
				VisProxy=(HXRObjectVisProxy*)HitProxy;
			}
			return EditedVisualizerPtr.Pin()->VisProxyHandleClick(InViewportClient, VisProxy, Click);
		}

	return false;
}

void FArmyObjectVisualizerManager::ClearActiveComponentVis()
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();

	if (EditedVisualizer.IsValid())
	{
		EditedVisualizer->EndEditing();
	}

	EditedVisualizerPtr.Reset();
	EditedVisualizerViewportClient = nullptr;

}

bool FArmyObjectVisualizerManager::HandleInputKey(UArmyEditorViewportClient* ViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) const
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();

	if (EditedVisualizer.IsValid())
	{
		if(EditedVisualizer->HandleInputKey(ViewportClient, Viewport, Key, Event))
		{
			return true;
		}
	}

	return false;
}

bool FArmyObjectVisualizerManager::HandleInputDelta(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) const
{
	TArray<FCachedXRObjectVisualizer> SelctionS = GVC->GetVisualizersForSelection();
	if (SelctionS.Num() > 1&&InViewportClient->GetCurrentWidgetAxis()!=EAxisList::None)
	{
		TArray<AXRGroupActor*> ActorGroups;

		// Apply the deltas to any selected actors.
		for(FSelectionIterator SelectedActorIt(GXREditor->GetSelectedActorIterator()); SelectedActorIt; ++SelectedActorIt)
		{
			AActor* Actor=static_cast<AActor*>(*SelectedActorIt);
			checkSlow(Actor->IsA(AActor::StaticClass()));

			{
				if(GXREditor->GetSelectedComponentCount()<=0)
				{
					AXRGroupActor* ParentGroup=AXRGroupActor::GetRootForActor(Actor,true,true);
					if(ParentGroup /*&& UActorGroupingUtils::IsGroupingActive()*/)
					{
						ActorGroups.AddUnique(ParentGroup);
					}
				}
			}
		}
		
		TArray<TSharedPtr<FArmyPipePoint>> Pointes;
		for(auto& Visulizer:SelctionS)
		{
			FObjectWeakPtr Component=Visulizer.Component;
			if(Component.Pin()->IsPipeLine())
			{
				TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(Component.Pin());
				Pointes.AddUnique(Line->StartPoint);
				Pointes.AddUnique(Line->EndPoint);
				if (InDrag.Size() > 0)
				{
					Line->IsChanged = true;
				}
			}
		}

		if (Pointes.Num())
		{
			for (auto& Point : Pointes)
				Point->Location += InDrag;
			return true;
		}
		AXRGroupActor::RemoveSubGroupsFromArray(ActorGroups);
		for(int32 ActorGroupsIndex=0; ActorGroupsIndex<ActorGroups.Num(); ++ActorGroupsIndex)
		{
			ActorGroups[ActorGroupsIndex]->PostEditMove(true);
		}
		return false;
	}
	
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();

	if (EditedVisualizer.IsValid() && EditedVisualizerViewportClient == InViewportClient && InViewportClient->GetCurrentWidgetAxis() != EAxisList::None)
	{
		 const EAxisList::Type CurrentAxis = InViewportClient->GetCurrentWidgetAxis();
		const bool bSingleAxisDrag=CurrentAxis==EAxisList::X||CurrentAxis==EAxisList::Y||CurrentAxis==EAxisList::Z;

		if (bSingleAxisDrag)
		{
			return EditedVisualizer->HandleInputDelta(InViewportClient, InViewport, InDrag, InRot, InScale);
		}
		if(!bSingleAxisDrag)
		{
			FVector TargetLocation = GetTargetLocation(InViewportClient,InViewport);
			return EditedVisualizer->HandleInputLocation(InViewportClient,InViewport,TargetLocation);
		}
	}

	return false;
}

bool FArmyObjectVisualizerManager::GetWidgetLocation(const UArmyEditorViewportClient* ViewportClient, FVector& OutLocation) const
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();

	if (EditedVisualizer.IsValid())
	{
		return EditedVisualizer->GetWidgetLocation(ViewportClient, OutLocation);
	}

	return false;
}

bool FArmyObjectVisualizerManager::HandleInputLocation(UArmyEditorViewportClient* InViewportClient,FViewport* InViewport,FVector& TargetLocation)
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer=EditedVisualizerPtr.Pin();

	if(EditedVisualizer.IsValid()&&EditedVisualizerViewportClient==InViewportClient && InViewportClient->GetCurrentWidgetAxis()!=EAxisList::None)
	{
		return EditedVisualizer->HandleInputLocation(InViewportClient,InViewport,TargetLocation);
	}
	return false;
}

bool FArmyObjectVisualizerManager::GetCustomInputCoordinateSystem(const UArmyEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();

	if (EditedVisualizer.IsValid())
	{
		return EditedVisualizer->GetCustomInputCoordinateSystem(ViewportClient, OutMatrix);
	}

	return false;
}

TSharedPtr<SWidget> FArmyObjectVisualizerManager::GenerateContextMenuForComponentVis() const
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();

	if (EditedVisualizer.IsValid())
	{
		return EditedVisualizer->GenerateContextMenu();
	}

	return TSharedPtr<SWidget>();
}

bool FArmyObjectVisualizerManager::IsActive() const
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();
	return EditedVisualizer.IsValid();
}

bool FArmyObjectVisualizerManager::IsVisualizingArchetype() const
{
	TSharedPtr<FArmyObjectVisualizer> EditedVisualizer = EditedVisualizerPtr.Pin();
	return EditedVisualizer.IsValid() && EditedVisualizer->IsVisualizingArchetype();
}

void FArmyObjectVisualizerManager::OnLevelSelectionChanged(UObject* Obj)
{
	//AXRActor * Actor=Cast<AXRActor>(Obj);
	USelection* Selection = GXREditor->GetSelectedActors(); 
	if ( Selection && Selection->IsValidLowLevel() )
	{
		AXRActor* Actor=GXREditor->GetSelectedActors()->GetBottom<AXRActor>();
		if(Actor && Actor->IsValidLowLevel())
		{
			const FArmyObject* ClickedComponent=Actor->GetRelevanceObject().Pin().Get();
			if(ClickedComponent!=NULL)
			{

				TSharedPtr<FArmyObjectVisualizer> Visualizer=GVC->FindXRObjectVisualizer (ClickedComponent->GetType ());//FArmyHydropowerEditorModule::Get(). GetEditorManager() ->FindXRObjectVisualizer(ClickedComponent->GetType());

				if(Visualizer.IsValid ())
				{
					bool bIsActive=Visualizer->OnLevelSelectionChanged(Actor);
					if(bIsActive)
					{
						EditedVisualizerPtr=Visualizer;
						EditedVisualizerViewportClient=GVC;
						return;
					}
				}
			}
		}
	}

	ClearActiveComponentVis();
	
}

FVector FArmyObjectVisualizerManager::GetTargetLocation(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport) const
{
	FCollisionQueryParams TraceParams(FName(TEXT("Pick")),true);
	FVector TraceStart,TraceEnd;
	TraceParams.bTraceComplex=false;
	TraceParams.AddIgnoredActor(InViewportClient->GetWorld()->GetFirstPlayerController()->GetPawn());
	AXRGroupActor* GroupActor=NULL;
	TArray<AActor*> SelectedActors;
	for(FSelectionIterator It(GXREditor->GetSelectedActorIterator()); It; ++It)
	{
		if(AActor* Actor=Cast<AActor>(*It))
		{
			SelectedActors.Add(Actor);
		}
	}
	for(auto& It:SelectedActors)
	{
		AXRGroupActor* TempGroupActor=AXRGroupActor::GetRootForActor(It,true,true);
		if(TempGroupActor)
		{
			GroupActor=TempGroupActor;
		}
		TraceParams.AddIgnoredActor(It);
	}

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		InViewport,
		InViewportClient->GetScene(),
		InViewportClient->EngineShowFlags)
		.SetRealtimeUpdate(true));
	FSceneView* View=InViewportClient->CalcSceneView(&ViewFamily);
	FVector2D MousePos;
	if(InViewportClient->GetMousePosition(MousePos))
	{
		FVector WorldOrigin,WorldDirection,WorldOriginOrtho;
		View->DeprojectFVector2D(MousePos,WorldOrigin,WorldDirection);
		if(InViewportClient->GetViewportType()==EXRLevelViewportType::LVT_Perspective)
		{
			TraceStart=WorldOrigin;
			TraceEnd=TraceStart+WorldDirection*10000.f;
		}

		FHitResult OutHit(ForceInit);
		InViewportClient->GetWorld()->LineTraceSingleByChannel(OutHit,TraceStart,TraceEnd,ECC_Visibility,TraceParams);

		FVector TargetLocation,TempLoc;
		float Offset=0.f;
		//计算TargetLocation
		if(OutHit.bBlockingHit)
		{
			{
				TargetLocation=OutHit.ImpactPoint;
				//如果是法线朝上，则Z值太高1个单位
				float DotValue=FVector::DotProduct(OutHit.ImpactNormal,FVector(0,0,1));
				if(DotValue>0.9f)
				{
					TargetLocation.Z+=1.f;
				}
			}
		}
		else
		{
			TargetLocation=TraceStart+WorldDirection * 300.f;
			if(TargetLocation.Z<0)
				TargetLocation.Z=0.f;
		}
		return TargetLocation;
	}
	return FVector::ZeroVector;
}
