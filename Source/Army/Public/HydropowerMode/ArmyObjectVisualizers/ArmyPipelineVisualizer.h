// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ArmyObjectVisualizer.h"

class AActor;
class FCanvas;
class UArmyEditorViewportClient;
class FPrimitiveDrawInterface;
class FSceneView;
class FViewport;
class SWidget;
struct FArmyViewportClick;


/** Proxy for a spline key */
struct HPipelineKeyProxy : public HXRObjectVisProxy
{
	DECLARE_HIT_PROXY ();

	HPipelineKeyProxy (const FArmyObject* InComponent,int32 InKeyIndex)
		: HXRObjectVisProxy (InComponent)
		,KeyIndex (InKeyIndex)
	{
	}

	int32 KeyIndex;
};
//管线观察者
class FArmyPipelineVisualizer : public FArmyObjectVisualizer
{
public:
	FArmyPipelineVisualizer () ;
	virtual ~FArmyPipelineVisualizer ();

	/** */
	virtual void OnRegister() override;
	/** Draw visualization for the supplied component */
	virtual void DrawVisualization(const FArmyObject* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	/** */
	virtual bool VisProxyHandleClick(UArmyEditorViewportClient* InViewportClient, HXRObjectVisProxy* VisProxy, const FArmyViewportClick& Click) override;
	virtual bool ProxyHandleClick (UArmyEditorViewportClient* InViewportClient,class HHitProxy* ActorHit,const FArmyViewportClick& Click) override;
	virtual bool OnLevelSelectionChanged(UObject* Obj) override;
	/** */
	virtual void EndEditing() override;
	/** */
	virtual bool GetWidgetLocation(const UArmyEditorViewportClient* ViewportClient, FVector& OutLocation) const override;
	/** */
	virtual bool GetCustomInputCoordinateSystem(const UArmyEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const { return false; }
	/** */
	virtual bool HandleInputDelta(UArmyEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltalRotate, FVector& DeltaScale);
	/** */
	virtual bool HandleInputLocation(UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& Location) override;
	/** */
	virtual bool HandleInputKey(UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FKey Key,EInputEvent Event) { return false; }
	/** */
	virtual TSharedPtr<SWidget> GenerateContextMenu() const { return TSharedPtr<SWidget>(); }
	/** */
	virtual bool IsVisualizingArchetype() const { return false; }

protected:
	TWeakPtr<const FArmyObject> OwningObject;

	/** Index of the last key we selected */
	int32 KeyIndexSelected;
};
