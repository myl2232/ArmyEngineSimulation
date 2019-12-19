// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "ArmyObjectVisualizer.h"

class UArmyEditorViewportClient;
class FViewport;
class SWidget;
struct FArmyViewportClick;

/** 管理xrboject观察者的类 */
class FArmyObjectVisualizerManager
{
public:
	FArmyObjectVisualizerManager();
	virtual ~FArmyObjectVisualizerManager();


	/** 激活XRObject观察者 */
	bool HandleProxyForComponentVis(UArmyEditorViewportClient* InViewportClient, HHitProxy *HitProxy, const FArmyViewportClick &Click);

	/** 清空激活的 */
	void ClearActiveComponentVis();

	/** 点击触发 */
	bool HandleClick(UArmyEditorViewportClient* InViewportClient, HHitProxy *HitProxy, const FArmyViewportClick &Click);

	/** Pass key input to active visualizer */
	bool HandleInputKey(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, FKey Key, EInputEvent Event) const;

	/** Pass delta input to active visualizer */
	bool HandleInputDelta(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale) const;

	/** Get widget location from active visualizer */
	bool GetWidgetLocation(const UArmyEditorViewportClient* InViewportClient, FVector& OutLocation) const;

	bool HandleInputLocation(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport,FVector& TargetLocation);

	/** Get custom widget coordinate system from active visualizer */
	bool GetCustomInputCoordinateSystem(const UArmyEditorViewportClient* InViewportClient, FMatrix& OutMatrix) const;

	/** Generate context menu for the component visualizer */
	TSharedPtr<SWidget> GenerateContextMenuForComponentVis() const;

	/** Returns whether there is currently an active visualizer */
	bool IsActive() const;

	/** Returns whether the component being visualized is an archetype or not */
	bool IsVisualizingArchetype() const;

	void OnLevelSelectionChanged(UObject* Obj);
private:
	FVector GetTargetLocation(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport) const;
private:
	/** Currently 'active' visualizer that we should pass input to etc */
	TWeakPtr<class FArmyObjectVisualizer> EditedVisualizerPtr;

	/** The viewport client for the currently active visualizer */
	UArmyEditorViewportClient* EditedVisualizerViewportClient;
};