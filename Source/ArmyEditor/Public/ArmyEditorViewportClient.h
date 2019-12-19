#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "ArmyUnrealWidget.h"
#include "ArmyEditorTypes.h"
#include "ArmyDrawGridHelper.h"
#include "ArmyEditorViewport.h"
#include "ArmyEditorModeManager.h"
#include "ArmyTypes.h"
#include "HitProxies.h"
#include "ArmyRuler.h"
#include "ArmyEditorViewportClient.generated.h"

//class FArmyWidget;
class UArmyEditorViewportClient;
class FArmyEditorModeTools;
class FArmyMouseDeltaTracker;
class FArmyDragTool;

DECLARE_DELEGATE_RetVal(bool, FViewportStateGetter);
DECLARE_MULTICAST_DELEGATE_OneParam(FIntTypeDelegate, int32);

namespace EArmyDragTool
{
	enum Type
	{
		BoxSelect,
		FrustumSelect,
		Measure,
		ViewportChange
	};
}

struct HArmyObjectBaseProxy : public HHitProxy
{
	DECLARE_HIT_PROXY(ARMYEDITOR_API);

	HArmyObjectBaseProxy(EHitProxyPriority InPriority = HPP_Wireframe)
		: HHitProxy(InPriority)
	{
	}

	virtual EMouseCursor::Type GetMouseCursor() override
	{
		return EMouseCursor::Crosshairs;
	}
};
struct FArmyInputEventState
{
public:
	FArmyInputEventState(FViewport* InViewport, FKey InKey, EInputEvent InInputEvent)
		: Viewport(InViewport)
		, Key(InKey)
		, InputEvent(InInputEvent)
	{}

	FViewport* GetViewport() const { return Viewport; }
	EInputEvent GetInputEvent() const { return InputEvent; }
	FKey GetKey() const { return Key; }

	/** return true if the event causing button is a control key */
	bool IsCtrlButtonEvent() const { return (Key == EKeys::LeftControl || Key == EKeys::RightControl); }
	bool IsShiftButtonEvent() const { return (Key == EKeys::LeftShift || Key == EKeys::RightShift); }
	bool IsAltButtonEvent() const { return (Key == EKeys::LeftAlt || Key == EKeys::RightAlt); }

	bool IsLeftMouseButtonPressed() const { return IsButtonPressed(EKeys::LeftMouseButton); }
	bool IsMiddleMouseButtonPressed() const { return IsButtonPressed(EKeys::MiddleMouseButton); }
	bool IsRightMouseButtonPressed() const { return IsButtonPressed(EKeys::RightMouseButton); }

	bool IsMouseButtonEvent() const { return (Key == EKeys::LeftMouseButton || Key == EKeys::MiddleMouseButton || Key == EKeys::RightMouseButton); }
	bool IsButtonPressed(FKey InKey) const { return Viewport->KeyState(InKey); }
	bool IsAnyMouseButtonDown() const { return IsButtonPressed(EKeys::LeftMouseButton) || IsButtonPressed(EKeys::MiddleMouseButton) || IsButtonPressed(EKeys::RightMouseButton); }

	/** return true if alt is pressed right now.  This will be true even if the event was for a different key but an alt key is currently pressed */
	bool IsAltButtonPressed() const { return !(IsAltButtonEvent() && InputEvent == IE_Released) && (IsButtonPressed(EKeys::LeftAlt) || IsButtonPressed(EKeys::RightAlt)); }
	bool IsShiftButtonPressed() const { return !(IsShiftButtonEvent() && InputEvent == IE_Released) && (IsButtonPressed(EKeys::LeftShift) || IsButtonPressed(EKeys::RightShift)); }
	bool IsCtrlButtonPressed() const { return !(IsCtrlButtonEvent() && InputEvent == IE_Released) && (IsButtonPressed(EKeys::LeftControl) || IsButtonPressed(EKeys::RightControl)); }
	bool IsSpaceBarPressed() const { return IsButtonPressed(EKeys::SpaceBar); }

private:
	/** Viewport the event was sent to */
	FViewport* Viewport;
	/** Pressed Key */
	FKey Key;
	/** Key event */
	EInputEvent InputEvent;
};



/**
* Contains information about a mouse cursor position within a viewport, transformed into the correct coordinate
* system for the viewport.
*/
struct FArmyViewportCursorLocation
{
public:
	ARMYEDITOR_API FArmyViewportCursorLocation(const class FSceneView* View, UArmyEditorViewportClient* InViewportClient, int32 X, int32 Y);
	ARMYEDITOR_API virtual ~FArmyViewportCursorLocation();

	const FVector&		GetOrigin()			const { return Origin; }
	const FVector&		GetDirection()		const { return Direction; }
	const FIntPoint&	GetCursorPos()		const { return CursorPos; }
	EArmyLevelViewportType::Type	GetViewportType()	const;
	UArmyEditorViewportClient*	GetViewportClient()	const { return ViewportClient; }

private:
	FVector	Origin;
	FVector	Direction;
	FIntPoint CursorPos;
	UArmyEditorViewportClient* ViewportClient;
};

struct FArmyViewportClick : public FArmyViewportCursorLocation
{
public:
	ARMYEDITOR_API FArmyViewportClick(const class FSceneView* View, class UArmyEditorViewportClient* ViewportClient, FKey InKey, EInputEvent InEvent, int32 X, int32 Y);
	ARMYEDITOR_API ~FArmyViewportClick();

	/** @return The 2D screenspace cursor position of the mouse when it was clicked. */
	const FIntPoint&	GetClickPos()	const { return GetCursorPos(); }
	const FKey&			GetKey()		const { return Key; }
	EInputEvent			GetEvent()		const { return Event; }

	virtual bool	IsControlDown()	const { return ControlDown; }
	virtual bool	IsShiftDown()	const { return ShiftDown; }
	virtual bool	IsAltDown()		const { return AltDown; }

private:
	FKey		Key;
	EInputEvent	Event;
	bool		ControlDown,
		ShiftDown,
		AltDown;
};

struct FArmyDropQuery
{
	FArmyDropQuery()
		: bCanDrop(false)
	{}

	/** True if it's valid to drop the object at the location queried */
	bool bCanDrop;

	/** Optional hint text that may be returned to the user. */
	FText HintText;
};

/**
* Stores the transformation data for the viewport camera
*/
struct ARMYEDITOR_API FArmyViewportCameraTransform
{
public:
	FArmyViewportCameraTransform();

	/** Sets the transform's location */
	void SetLocation(const FVector& Position);

	/** Sets the transform's rotation */
	void SetRotation(const FRotator& Rotation)
	{
		ViewRotation = Rotation;
	}

	/** Sets the location to look at during orbit */
	void SetLookAt(const FVector& InLookAt)
	{
		LookAt = InLookAt;
	}

	/** Set the ortho zoom amount */
	void SetOrthoZoom(float InOrthoZoom)
	{
		OrthoZoom = InOrthoZoom;
	}

	/** Check if transition curve is playing. */
	bool IsPlaying();

	/** @return The transform's location */
	FORCEINLINE const FVector& GetLocation() const { return ViewLocation; }

	/** @return The transform's rotation */
	FORCEINLINE const FRotator& GetRotation() const { return ViewRotation; }

	/** @return The look at point for orbiting */
	FORCEINLINE const FVector& GetLookAt() const { return LookAt; }

	/** @return The ortho zoom amount */
	FORCEINLINE float GetOrthoZoom() const { return OrthoZoom; }

	/**
	* Animates from the current location to the desired location
	*
	* @param InDesiredLocation	The location to transition to
	* @param bInstant			If the desired location should be set instantly rather than transitioned to over time
	*/
	void TransitionToLocation(const FVector& InDesiredLocation, TWeakPtr<SWidget> EditorViewportWidget, bool bInstant);

	/**
	* Updates any current location transitions
	*
	* @return true if there is currently a transition
	*/
	bool UpdateTransition();

	/**
	* Computes a matrix to use for viewport location and rotation when orbiting
	*/
	FMatrix ComputeOrbitMatrix() const;

private:
	/** The time when a transition to the desired location began */
	//double TransitionStartTime;

	/** Curve for animating between locations */
	TSharedPtr<struct FCurveSequence> TransitionCurve;
	/** Current viewport Position. */
	FVector	ViewLocation;
	/** Current Viewport orientation; valid only for perspective projections. */
	FRotator ViewRotation;
	/** Desired viewport location when animating between two locations */
	FVector	DesiredLocation;
	/** When orbiting, the point we are looking at */
	FVector LookAt;
	/** Viewport start location when animating to another location */
	FVector StartLocation;
	/** Ortho zoom amount */
	float OrthoZoom;
};


UCLASS()
class ARMYEDITOR_API UArmyEditorViewportClient : public UGameViewportClient, public FViewElementDrawer
{
	GENERATED_BODY()

public:
	friend class FArmyMouseDeltaTracker;

	UArmyEditorViewportClient();
	virtual ~UArmyEditorViewportClient();

	//实时渲染设置相关
	bool ToggleRealtime();

	void SetRealtime(bool bInRealtime, bool bStoreCurrentValue = false);

	bool IsRealtime() const { return bIsRealtime || RealTimeFrameCount != 0; }

	void RequestRealTimeFrames(uint32 NumRealTimeFrames = 1)
	{
		RealTimeFrameCount = FMath::Max(NumRealTimeFrames, RealTimeFrameCount);
	}

	void RestoreRealtime(const bool bAllowDisable = false);

	// this set ups camera for both orbit and non orbit control
	void SetCameraSetup(const FVector& LocationForOrbiting, const FRotator& InOrbitRotation, const FVector& InOrbitZoom, const FVector& InOrbitLookAt,
		const FVector& InViewLocation, const FRotator &InViewRotation);

	/** Callback for toggling the grid show flag. */
	void SetShowGrid(bool show);

	/** Callback for toggling the realtime preview flag. */
	void SetRealtimePreview();

	FArmyViewportCameraTransform& GetViewTransform()
	{
		return IsPerspective() ? ViewTransformPerspective : ViewTransformOrthographic;
	}

	const FArmyViewportCameraTransform& GetViewTransform() const
	{
		return IsPerspective() ? ViewTransformPerspective : ViewTransformOrthographic;
	}

	void SetViewLocation(const FVector& NewLocation)
	{
		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetLocation(NewLocation);
	}

	void SetViewRotation(const FRotator& NewRotation)
	{
		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
		ViewTransform.SetRotation(NewRotation);
	}

	void SetLookAtLocation(const FVector& LookAt, bool bRecalculateView = false)
	{
		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();

		ViewTransform.SetLookAt(LookAt);

		if (bRecalculateView)
		{
			FMatrix OrbitMatrix = ViewTransform.ComputeOrbitMatrix();
			OrbitMatrix = OrbitMatrix.InverseFast();

			ViewTransform.SetRotation(OrbitMatrix.Rotator());
			ViewTransform.SetLocation(OrbitMatrix.GetOrigin());
		}
	}

	/** Sets ortho zoom amount */
	void SetOrthoZoom(float InOrthoZoom)
	{
		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();

		// A zero ortho zoom is not supported and causes NaN/div0 errors
		check(InOrthoZoom != 0);
		ViewTransform.SetOrthoZoom(InOrthoZoom);
	}
	/** @return the current viewport camera location */
	const FVector& GetViewLocation() const
	{
		const FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetLocation();
	}

	/** @return the current viewport camera rotation */
	const FRotator& GetViewRotation() const
	{
		const FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetRotation();
	}

	/** @return the current look at location */
	const FVector& GetLookAtLocation() const
	{
		const FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetLookAt();
	}

	/** @return the current ortho zoom amount */
	float GetOrthoZoom() const
	{
		const FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
		return ViewTransform.GetOrthoZoom();
	}
	void GetViewportSize(FVector2D& out_ViewportSize) const;

	bool GetMousePosition(FVector2D& MousePosition) const;

	FScale2D GetMouseScale()
	{
		return FScale2D(MouseScale.X, MouseScale.Y);
	}

	/** @return The number of units per pixel displayed in this viewport */
	float GetOrthoUnitsPerPixel(const FViewport* Viewport) const;

	static FString UnrealUnitsToSiUnits(float UnrealUnits);

	void RemoveCameraRoll()
	{
		FRotator Rotation = GetViewRotation();
		Rotation.Roll = 0;
		SetViewRotation(Rotation);
	}

	void SetInitialViewTransform(EArmyLevelViewportType::Type ViewportType, const FVector& ViewLocation, const FRotator& ViewRotation, float InOrthoZoom);

	void TakeHighResScreenShot();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
	virtual void PostInitProperties() override;
	/** FViewElementDrawer interface */
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void Draw(FViewport* InViewport, FCanvas* SceneCanvas) override;

	/** FViewportClient interface */
	virtual void ProcessScreenShots(FViewport* Viewport) override;
	virtual void RedrawRequested(FViewport* Viewport) override;
	virtual void RequestInvalidateHitProxy(FViewport* Viewport) override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = false) override;
	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	virtual bool InputGesture(FViewport* Viewport, EGestureEvent GestureType, const FVector2D& GestureDelta, bool bIsDirectionInvertedFromDevice) override;
	virtual void ReceivedFocus(FViewport* Viewport) override;
	virtual void MouseEnter(FViewport* Viewport, int32 x, int32 y) override;
	virtual void MouseMove(FViewport* Viewport, int32 x, int32 y) override;
	virtual void MouseLeave(FViewport* Viewport) override;
	virtual EMouseCursor::Type GetCursor(FViewport* Viewport, int32 X, int32 Y) override;
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual bool IsOrtho() const override;
	virtual void LostFocus(FViewport* Viewport) override;

	virtual void ProcessClick(class FSceneView& View, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY);
	virtual void TrackingStarted(const struct FArmyInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge);
	virtual void TrackingStopped();
	virtual bool InputWidgetDelta(FViewport* InViewport, EAxisList::Type CurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale);
	virtual void SetWidgetMode(FArmyWidget::EWidgetMode NewMode);
	virtual bool CanSetWidgetMode(FArmyWidget::EWidgetMode NewMode) const;
	virtual bool CanCycleWidgetMode() const { return true; }
	virtual FArmyWidget::EWidgetMode GetWidgetMode() const;
	virtual FVector GetWidgetLocation() const;
	virtual FMatrix GetWidgetCoordSystem() const;
	EArmyCoordSystem GetWidgetCoordSystemSpace() const;
	virtual void SetWidgetCoordSystemSpace(EArmyCoordSystem NewCoordSystem);
	virtual void SetCurrentWidgetAxis(EAxisList::Type InAxis);

	virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;
	virtual void ModifyView(FViewport& InViewport, FSceneViewFamily* ViewFamily, FSceneView& View) override;

	void RenderDragTool(const FSceneView* View, FCanvas* Canvas);
	virtual FSceneView* CalcSceneView(FSceneViewFamily* ViewFamily, const EStereoscopicPass StereoPass = eSSP_FULL);
	virtual FSceneInterface* GetScene() const;
	virtual FLinearColor GetBackgroundColor() const;
	virtual void OverridePostProcessSettings(FSceneView& View);

	virtual void Tick(float DeltaSeconds);
	virtual void UpdateMouseDelta();
	virtual void UpdateGestureDelta();

	virtual UWorld* GetWorld() const override;
	virtual TSharedPtr<FArmyDragTool> MakeDragTool(EArmyDragTool::Type DragToolType);
	virtual bool CanUseDragTool() const;
	virtual bool ShouldOrbitCamera() const;
	bool IsMovingCamera() const;
	virtual void UpdateLinkedOrthoViewports(bool bInvalidate = false) {}
	virtual bool ShouldLockPitch() const;
	virtual void CheckHoveredHitProxy(HHitProxy* HoveredHitProxy);
	//virtual bool UpdateDropPreviewActors(int32 MouseX, int32 MouseY, const TArray<UObject*>& DroppedObjects, bool& out_bDroppedObjectsVisible, UActorFactory* FactoryToUse = NULL) { return false; }
	virtual void DestroyDropPreviewActors() {}
	//virtual FDropQuery CanDropObjectsAtCoordinates(int32 MouseX, int32 MouseY, const FAssetData& AssetInfo) { return FDropQuery(); }
	//virtual bool DropObjectsAtCoordinates(int32 MouseX, int32 MouseY, const TArray<UObject*>& DroppedObjects, TArray<AActor*>& OutNewActors, bool bOnlyDropOnTarget = false, bool bCreateDropPreview = false, bool bSelectActors = true, UActorFactory* FactoryToUse = NULL) { return false; }

	virtual EArmyLevelViewportType::Type GetViewportType() const;
	FVector TranslateDelta(FKey InKey, float InDelta, bool InNudge);
	bool IsPerspective() const;
	void FocusViewportOnBox(const FBox& BoundingBox, bool bInstant = false);
	class FArmyEditorCameraController* GetCameraController(void) { return CameraController; }
	void InputAxisForOrbit(FViewport* Viewport, const FVector& DragDelta, FVector& Drag, FRotator& Rot);
	bool InputTakeScreenshot(FViewport* Viewport, FKey Key, EInputEvent Event);
	void OpenScreenshot(FString SourceFilePath);
	void TakeScreenshot(FViewport* Viewport, bool bInvalidateViewport);
	//截取当前编辑器的主视口的截图（尺寸为视口内的最大正方形）
	void TakeScreenshotInMaxRect(int32 ScaledRectSize, TArray<FColor>& OutUncompressedData, TArray<uint8>& OutCompressedData);

	void ConvertMovementToDragRot(const FVector& InDelta, FVector& InDragDelta, FRotator& InRotDelta) const;
	void ConvertMovementToOrbitDragRot(const FVector& InDelta, FVector& InDragDelta, FRotator& InRotDelta) const;
	void ToggleOrbitCamera(bool bEnableOrbitCamera);
	void SetViewLocationForOrbiting(const FVector& LookAtPoint, float DistanceToCamera = 256.f);
	void MoveViewportCamera(const FVector& InDrag, const FRotator& InRot, bool bDollyCamera = false);

	/*
	* @IsReplacing 是否在替换
	*/
	void PlaceActor(AActor* _NewActor,bool IsReplacing=false);
	void PlaceMaterial(UMaterialInterface* _NewMaterial, FString _ThumbnailURL = "");
	void SetDroppedObjects(TArray<UObject*> _Objects);
	bool IsDroppingObjects();
	//获取第一个正在被拖拽的物体
	UObject* GetFirstDroppingObject();

	void SetDroppingPreviewTexture(bool _Dropping);
	bool GetDroppingPreviewTexture();
	void DropObjectAtCoordinates(int32 MouseX, int32 MouseY, UObject* _DroppedObject, bool SelectObject);
	bool DropObjectOnActor(FSceneView* View, FArmyViewportCursorLocation& Cursor, UObject* DroppedObject, AActor* TargetActor, int32 DroppedUponSlot, bool SelectObject);
	void UpdateDroppedObject();
	void CancelDrop();

	virtual void SetViewportType(EArmyLevelViewportType::Type InViewportType);
	virtual bool RequiresHitProxyStorage() override { return 1; }
	void Invalidate(bool bInvalidateChildViews = true, bool bInvalidateHitProxies = true);
	bool IsAltPressed() const;
	bool IsCtrlPressed() const;
	bool IsShiftPressed() const;
	bool IsCmdPressed() const;
	void MarkMouseMovedSinceClick();
	bool IsUsingAbsoluteTranslation() const;
	bool IsTracking() const { return bIsTracking; }
	EAxisList::Type GetCurrentWidgetAxis() const;
	bool IsVisible() const;
	bool IsActorAutoRotation(AActor* InActor);

	bool  UpdateAutoRotation(AActor* InActor,const TArray<AActor*>& SelectActors,const FBox& ActorInBox,const FVector& InDeltaDrag);

	float GetCameraSpeed() const;
	FArmyEditorModeTools* GetModeTools() const { return ModeTools; }
	//TSharedPtr<SEditorViewport> GetEditorViewportWidget() const { return EditorViewportWidget.Pin(); }

	float GetNearClipPlane() const;
	float GetFarClipPlaneOverride() const;
	void OverrideFarClipPlane(const float InFarPlane);
	void OverrideNearClipPlane(float InNearPlane);

	FVector GetHitProxyObjectLocation(int32 X, int32 Y);
	void ShowWidget(const bool bShow);
	void ProjectActorsIntoWorld(const TArray<AActor*>& Actors);

	void ModifyScale(AActor* InActor, FVector& ScaleDelta, bool bCheckSmallExtent = false) const;
	void ValidateScale(const FVector& CurrentScale, const FVector& BoxExtent, FVector& ScaleDelta, bool bCheckSmallExtent = false) const;
	bool IsAdsorbDetect(AActor* InActor, const TArray<AActor*>& SelectActors, const FBox& ActorInBox, const FVector& InDeltaDrag);
	bool IsAdsorbDetect2D(AActor* InActor,const TArray<AActor*>& SelectActors,const FBox& ActorInBox,FVector MousePoint, FVector& OutLocation);
	void ApplyDeltaToActors(const FVector& InDrag, const FRotator& InRot, const FVector& InScale);
	 virtual void ApplyDeltaToActor(AActor* InActor, const FVector& InDeltaDrag, const FRotator& InDeltaRot, const FVector& InDeltaScale);
	void ApplyDeltaToComponent(USceneComponent* InComponent, const FVector& InDeltaDrag, const FRotator& InDeltaRot, const FVector& InDeltaScale);
	FBox GetActorCollisionBox(AActor* Actor) const;
	//const TSharedPtr<class FUICommandList> GetCommandList() const { return CommandList; }
	void SetWorld(UWorld* NewWorld) { World = NewWorld; }

	float GetUniformScale(FVector Point);
	bool WorldToPixel(const FVector& WorldPoint, FVector2D& OutPixelLocation);
	FVector4 PixelToWorld(float X, float Y, float Z);
	void DeprojectFVector2D(const FVector2D& ScreenPos, FVector& out_WorldOrigin, FVector& out_WorldDirection);

	virtual bool LocalLineTraceSingleByChannel(FVector& OutHit, FVector& OutHitNormal, int32& HitObjectType/*摆放类型（1：地面，2：顶面：3：墙面）*/, const FVector& Start, const FVector& End) { return false; };

	/** 设置环绕模式 */
	void SetOrbitType();

	/** 全屏模式 */
	void SetGameViewportFullScreenMode(bool bInFullScreenMode);

	FVector GetCurrentMouseTraceToWorldPos();

	//@刘克祥添加！
	FVector GetMouseTraceToWorldPos(FVector& InStart, FVector& InEnd);

	FVector GetCurrentMouseTraceToSurfaceNormal();

	FVector GetCurrentMouseTraceToSurfaceNormal(FVector2D& InMousePoint);

	//发射一条射线 获得与墙体相交的点
	bool GetImportPoint(FVector& InStart, FVector& InEnd, FVector& OutLocation);

	virtual float GetWallHeight() { return 280; };

	void SetAutoMultiSelectEnabled(bool InCtrl){ bAutoMultiSelectEnabled = InCtrl;}

	bool AutoMultiSelectEnabled(){return bAutoMultiSelectEnabled;}

	void EnableBSPSelectAble(bool bEnable){ bEnableBSPSelectAble = bEnable;}

	//@ 设置背景网格的可见性
	void SetDrawGridVisibility(bool bVis) { bUsesDrawHelper = bVis; }

	/** 当前是否为环绕模式 */
	bool bIsOrbit;

	bool bEnableBSPSelectAble;

	FVector LastPerspectiveViewLoc;

	FRotator LastPerspectiveViewRot;

	//@欧石楠 是否在拖拽Widget
	bool GetIsDraggingWidget();


protected:
	void InvalidateViewportWidget();
	virtual void PerspectiveCameraMoved() {}
	virtual void RedrawAllViewportsIntoThisScene();
	void ApplyDeltaToRotateWidget(const FRotator& InRot);
	void StartTrackingDueToInput(const struct FArmyInputEventState& InputState);
	void ProcessClickInViewport(const FArmyInputEventState& InputState, FSceneView& View);
	void ProcessDoubleClickInViewport(const struct FArmyInputEventState& InputState, FSceneView& View);
	void OnOrthoZoom(const struct FArmyInputEventState& InputState, float Scale = 1.0f);
	void OnDollyPerspectiveCamera(const struct FArmyInputEventState& InputState);
	void StopTracking();
	void DrawAxes(FViewport* Viewport, FCanvas* Canvas, const FRotator* InRotation = NULL, EAxisList::Type InAxis = EAxisList::XYZ);
	void DrawScaleUnits(FViewport* Viewport, FCanvas* Canvas, const FSceneView& InView);
	//FVector PixelToWorld(float X, float Y, float Z);

	virtual void Callback_PreSelectionNone() {}
	virtual void Callback_SelectionNone() {}
	virtual void Callback_SelectionChanged(UObject* _NewSelectionObject) {}

private:
	bool ShouldPanOrDollyCamera() const;
	void ConditionalCheckHoveredHitProxy();
	void MoveViewportPerspectiveCamera(const FVector& InDrag, const FRotator& InRot, bool bDollyCamera = false);
	void UpdateCameraMovement(float DeltaTime);
	virtual void BeginCameraMovement(bool bHasMovement);
	virtual void EndCameraMovement();

public:
	FViewport* Viewport;
	FViewportStateGetter VisibilityDelegate;
	FArmyViewportCameraTransform		ViewTransformPerspective;
	FArmyViewportCameraTransform		ViewTransformOrthographic;
	EArmyLevelViewportType::Type		ViewportType;
	/** true if the widget's axis is being controlled by an active mouse drag. */
	bool bWidgetAxisControlledByDrag;
	bool bNeedsRedraw;
	bool bNeedsInvalidateHitProxy;
	bool bUsingOrbitCamera;
	/** The viewport's scene view state. */
	FSceneViewStateReference ViewState;
	/** true when we are in a state where we can check the hit proxy for hovering. */
	/** true when a brush is being transformed by its Widget */
	bool					bIsTrackingBrushModification;
	/**
	* Used for drag duplication. Set to true on Alt+LMB so that the selected
	* objects (components or actors) will be duplicated as soon as the widget is displaced.
	*/
	bool					bDuplicateOnNextDrag;
	/**
	* bDuplicateActorsOnNextDrag will not be set again while bDuplicateActorsInProgress is true.
	* The user needs to release ALT and all mouse buttons to clear bDuplicateActorsInProgress.
	*/
	bool					bDuplicateActorsInProgress;
	/**true if only the pivot position has been moved*/
	bool					bOnlyMovedPivot;
	/** Viewport's current horizontal field of view (can be modified by locked cameras etc.) */
	float ViewFOV;
	/** Viewport's stored horizontal field of view (saved in ini files). */
	float FOVAngle;
	float AspectRatio;
	//从建材市场拖拽出来的物体
	TArray<UObject*> DroppedObjects;
	bool bDroppingPreviewTexture;
	bool bEnableClick;
	FVector2D OrthoMousePos;
	FVector OrthoWorldOrigin;
	int32 AddMaterialCount;
	bool bForceHiddenWidget;
	bool bDrawAxes;
	//全局开启多选功能
	bool bAutoMultiSelectEnabled;

	EMouseCursor::Type CurCursor;

	DECLARE_DELEGATE_OneParam(FObjectDelegate, UMaterialInterface*);
	FObjectDelegate CreateNewDroppedMIDEvent;
	FIntTypeDelegate OnViewTypeChanged;

	DECLARE_DELEGATE_OneParam(FActorsDelegate, const TArray<AActor*>&);
	FActorsDelegate ProjectActorsIntoWorldDelegate;

	/** 将Actor放置到场景时的回调 */
	FActorDelegate PlaceActorToWorldDelegate;

	DECLARE_DELEGATE(FObjectDropedDelegate);
	FObjectDropedDelegate CreateNewDroppedEvent;

	DECLARE_DELEGATE_RetVal_OneParam(HAdsorbDetect,FObjectAdsorbDetectDelegate,FVector);
	FObjectAdsorbDetectDelegate AdsorbDetect;


	FBox FlowAxisBoxOfSelectedActors;//一直跟随坐标系的包围盒，尺寸和当前选中物体集的包围盒相同
	bool bAdsorbed;//是否已经吸附
	bool bSnapTranslate;//是否开启吸附
	bool bSnapTranslateTemp;//Alt键临时开启吸附
	float AdsorbDistance;//吸附距离

						 //拖拽前后的鼠标世界坐标
	FVector WorldStartPos;
	FVector WorldEndPos;

	TWeakPtr<class SArmyEditorViewport> EditorViewportWidget;

	/** 是否启用PlayerController的视角计算，否则会使用ArmyCameraController计算视角 */
	bool bUserPlayerControllerView;

	//鼠标滚轮旋转物体功能相关  欧石楠2019.1.16
	bool CanMouseRotate();
	void OpenMouseRotate();
	void CloseMouseRotate();
	
	/** @  ArmyLightmass 测试绘制UV通道 */
	int32 DrawSelectedUVChannel;
	/** @  ArmyLightmass 测试绘制第几个组件的UV */
	int32 DrawSelectedUVComponentIndex;

protected:

	FArmyWidget*	Widget;
	FArmyEditorModeTools*	ModeTools;
	bool bShowWidget;
	FArmyMouseDeltaTracker*		MouseDeltaTracker;

	//编辑器默认的相机管理
	class FArmyEditorCameraController* CameraController;
	class FArmyCameraControllerUserImpulseData* CameraUserImpulseData;

	/* Updated on each mouse drag start */
	uint32 LastMouseX;
	uint32 LastMouseY;

	FVector4 LastWorldPostion;

	/** Represents the last known mouse position. If the mouse stops moving it's not the current but the last position before the current location. */
	uint32 CachedMouseX;
	uint32 CachedMouseY;
	//视口缩放系数 避免鼠标拾取错误
	FVector2D MouseScale;

	// -1, -1 if not set
	FIntPoint CurrentMousePos;
	/**
	* true when within a FMouseDeltaTracker::StartTracking/EndTracking block.
	*/
	bool bIsTracking;
	/**
	* true if the user is dragging by a widget handle.
	*/
	bool bDraggingByHandle;
	float GestureMoveForwardBackwardImpulse;
	uint32 RealTimeFrameCount;
	bool bIsRealtime;
	bool bStoredRealtime;
	bool bShouldCheckHitProxy;
	bool bIsCameraMoving;
	bool bIsCameraMovingOnTick;
	/** If true, the canvas has been been moved using bMoveCanvas Mode*/
	bool bHasMouseMovedSinceClick;
	bool bUseControllingActorViewInfo;
	//TWeakPtr<SEditorViewport> EditorViewportWidget;

	bool bUsesDrawHelper;
	FArmyDrawGridHelper DrawHelper;

	FRotator DefaultOrbitRotation;
	FVector DefaultOrbitLocation;
	FVector DefaultOrbitZoom;
	FVector DefaultOrbitLookAt;


	//是否开启右键旋转功能
	bool isAllowedRotateByMouse = true;
private:
	/** near plane adjustable for each editor view, if < 0 GNearClippingPlane should be used. */
	float NearPlane;
	/** If > 0, overrides the view's far clipping plane with a plane at the specified distance. */
	float FarPlane;
	bool bShouldInvalidateViewportWidget;


};
