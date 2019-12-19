// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "HitProxies.h"
#include "ArmyObject.h"
#include "ArmyEditorViewportClient.h"
//class FArmyObject;
class AActor;
class FCanvas;
class UArmyEditorViewportClient;
class FPrimitiveDrawInterface;
class FSceneView;
class FViewport;
class SWidget;
struct FArmyViewportClick;

struct HXRObjectVisProxy : public HXRObjectBaseProxy
{
	DECLARE_HIT_PROXY();

	HXRObjectVisProxy (const FArmyObject* InComponent, EHitProxyPriority InPriority = HPP_Wireframe)
	: HXRObjectBaseProxy(InPriority)
		,Component(InComponent->AsShared())
	{
	}

	virtual EMouseCursor::Type GetMouseCursor() override
	{
		return EMouseCursor::Crosshairs;
	}

	TWeakPtr<const FArmyObject> Component;
};


/**  XRObject基类观察者, that draw editor information for a particular component class */
class FArmyObjectVisualizer : public TSharedFromThis<FArmyObjectVisualizer>
{
public:
	FArmyObjectVisualizer () {}
	virtual ~FArmyObjectVisualizer () {}

	/**注册 */
	virtual void OnRegister() {}
	/**绘制*/
	virtual void DrawVisualization(const FArmyObject* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) {}
	/**绘制HUD*/
	virtual void DrawVisualizationHUD(const FArmyObject* Component, const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas) {}
	/** */
	virtual bool VisProxyHandleClick(UArmyEditorViewportClient* InViewportClient, HXRObjectVisProxy* VisProxy, const FArmyViewportClick& Click) { return false; }
	/**选中Actor*/
	virtual bool OnLevelSelectionChanged(UObject* Obj){ return false; };
	/**选中操作点*/
	virtual bool ProxyHandleClick (UArmyEditorViewportClient* InViewportClient,class HHitProxy* ActorHit,const FArmyViewportClick& Click)
	{
		return false;
	}
	/**结束编辑*/
	virtual void EndEditing() {}
	/**获取操作点的位置*/
	virtual bool GetWidgetLocation(const UArmyEditorViewportClient* ViewportClient, FVector& OutLocation) const { return false; }
	/**自定义坐标轴*/
	virtual bool GetCustomInputCoordinateSystem(const UArmyEditorViewportClient* ViewportClient, FMatrix& OutMatrix) const { return false; }
	/**拖拽坐标轴*/
	virtual bool HandleInputDelta(UArmyEditorViewportClient* ViewportClient, FViewport* Viewport, FVector& DeltaTranslate, FRotator& DeltalRotate, FVector& DeltaScale) { return false; }
	/**拖拽坐标轴*/
	virtual bool HandleInputLocation(UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FVector& Location){return false;}
	/** */
	virtual bool HandleInputKey(UArmyEditorViewportClient* ViewportClient,FViewport* Viewport,FKey Key,EInputEvent Event) { return false; }
	/** */
	virtual TSharedPtr<SWidget> GenerateContextMenu() const { return TSharedPtr<SWidget>(); }
	/** */
	virtual bool IsVisualizingArchetype() const { return false; }

	struct FPropertyNameAndIndex
	{
		FPropertyNameAndIndex()
			: Name(NAME_None)
			, Index(INDEX_NONE)
		{}

		explicit FPropertyNameAndIndex(FName InName, int32 InIndex = 0)
			: Name(InName)
			, Index(InIndex)
		{}

		bool IsValid() const { return Name != NAME_None && Index != INDEX_NONE; }

		void Clear()
		{
			Name = NAME_None;
			Index = INDEX_NONE;
		}

		FName Name;
		int32 Index;
	};
};
/**
*缓存观察者
*/
struct FCachedXRObjectVisualizer
{
	TWeakPtr<FArmyObject> Component;
	TSharedPtr<FArmyObjectVisualizer> Visualizer;
	
	FCachedXRObjectVisualizer (FArmyObject* InComponent, TSharedPtr<FArmyObjectVisualizer>& InVisualizer)
		: Component( InComponent->AsShared())
		, Visualizer(InVisualizer)
	{}
};
