#pragma once
#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/GCObject.h"
#include "ArmyUnrealWidget.h"
#include "Engine/Selection.h"
#include "ConvexVolume.h"

class UArmyEditorViewportClient;

class ARMYEDITOR_API FArmyEditorModeTools : public FGCObject
{
public:
	FArmyEditorModeTools();
	virtual ~FArmyEditorModeTools();

public:
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	void SetPivotLocation(const FVector& Location, const bool bIncGridBase);

	/** Mouse tracking interface.  Passes tracking messages to all active modes */
	bool StartTracking(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport);
	bool EndTracking(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport);
	bool IsTracking() const { return bIsTracking; }

	bool MouseEnter(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, int32 X, int32 Y);

	bool MouseLeave(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport);

	/** Notifies all active modes that the mouse has moved */
	bool MouseMove(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, int32 X, int32 Y);

	/** Notifies all active modes that a viewport has received focus */
	bool ReceivedFocus(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport);

	/** Notifies all active modes that a viewport has lost focus */
	bool LostFocus(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport);

	/** true if any active mode uses a transform widget */
	bool UsesTransformWidget() const;

	/** true if any active mode uses the passed in transform widget */
	bool UsesTransformWidget(FArmyWidget::EWidgetMode CheckMode) const;

	bool CapturedMouseMove(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY);

	bool AllowsViewportDragTool() const;

	/** Notifies all active modes to empty their selections */
	void SelectNone();

	/** Notifies all active modes of box selection attempts */
	bool BoxSelect(FBox& InBox, bool InSelect);

	/** Notifies all active modes of frustum selection attempts */
	bool FrustumSelect(const FConvexVolume& InFrustum, bool InSelect);

	void DrawHUD(UArmyEditorViewportClient* InViewportClient, FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);

	FVector GetWidgetLocation() const;
	/**
	* Changes the current widget mode.
	*/
	void SetWidgetMode(FArmyWidget::EWidgetMode InWidgetMode);
	FArmyWidget::EWidgetMode GetWidgetMode() const;
	void CycleWidgetMode();
	/** Notifies all active modes of any change in mouse movement */
	bool InputDelta(UArmyEditorViewportClient* InViewportClient, FViewport* InViewport, FVector& InDrag, FRotator& InRot, FVector& InScale);
	/** True if we should allow widget move */
	bool AllowWidgetMove() const;

	EArmyCoordSystem GetCoordSystem(bool bGetRawValue = false);
	/** Sets the current CoordSystem */
	void SetCoordSystem(EArmyCoordSystem NewCoordSystem);

	/**
	* Returns the set of selected actors.
	*/
	virtual USelection* GetSelectedActors() const;

	/**
	* @return the set of selected non-actor objects.
	*/
	virtual USelection* GetSelectedObjects() const;

	/**
	* Returns the set of selected components.
	*/
	virtual USelection* GetSelectedComponents() const;

	FMatrix GetCustomDrawingCoordinateSystem();
	FMatrix GetCustomInputCoordinateSystem();

	bool PivotShown;
	bool Snapping;
	bool SnappedActor;

	FVector CachedLocation;
	FVector PivotLocation;
	FVector SnappedLocation;
	FVector GridBase;
	/** The angle for the translate rotate widget */
	float TranslateRotateXAxisAngle;

	/** The angles for the 2d translate rotate widget */
	float TranslateRotate2DAngle;

	/** Draws in the top level corner of all UArmyEditorViewportClient windows (can be used to relay info to the user). */
	FString InfoString;
	/** The mode that the editor viewport widget is in. */
	FArmyWidget::EWidgetMode WidgetMode;
protected:

	void OnEditorSelectionChanged(UObject* NewSelection);
	void OnEditorSelectNone();

private:

	/** The coordinate system the widget is operating within. */
	EArmyCoordSystem CoordSystem;
	bool bIsTracking;
};

