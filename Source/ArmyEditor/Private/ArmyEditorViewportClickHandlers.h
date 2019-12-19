#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"

class UArmyEditorViewportClient;
class FLevelEditorViewportClient;
struct FArmyViewportClick;

namespace ArmyClickHandlers
{
	bool ClickActor(UArmyEditorViewportClient* ViewportClient, AActor* Actor, const FArmyViewportClick& Click, bool bAllowSelectionChange);

	void ClickBackdrop(UArmyEditorViewportClient* ViewportClient, const FArmyViewportClick& Click);

	void ClickSurface(UArmyEditorViewportClient* ViewportClient, UModel* Model, int32 iSurf, const FArmyViewportClick& Click);

	/*bool ClickComponent(FLevelEditorViewportClient* ViewportClient, HActor* ActorHitProxy, const FViewportClick& Click);

	void ClickBrushVertex(FLevelEditorViewportClient* ViewportClient, ABrush* InBrush, FVector* InVertex, const FViewportClick& Click);

	void ClickStaticMeshVertex(FLevelEditorViewportClient* ViewportClient, AActor* InActor, FVector& InVertex, const FViewportClick& Click);

	bool ClickGeomPoly(FLevelEditorViewportClient* ViewportClient, HGeomPolyProxy* InHitProxy, const FViewportClick& Click);

	bool ClickGeomEdge(FLevelEditorViewportClient* ViewportClient, HGeomEdgeProxy* InHitProxy, const FViewportClick& Click);

	bool ClickGeomVertex(FLevelEditorViewportClient* ViewportClient, HGeomVertexProxy* InHitProxy, const FViewportClick& Click);


	void ClickLevelSocket(FLevelEditorViewportClient* ViewportClient, HHitProxy* HitProxy, const FViewportClick& Click);*/
};