#pragma once

#include "ArmyEditorViewportClient.h"
#include "ArmyObjectVisualizerManager.h"
#include "ArmyViewportClient.generated.h"

DECLARE_DELEGATE_OneParam(ScreenShotDelegate, FViewport*)

/** 绘制模式 */
enum EArmyDrawMode
{
	DM_2D,
	DM_3D,
};

UCLASS()
class UArmyViewportClient : public UArmyEditorViewportClient
{
	GENERATED_BODY()

public:
	UArmyViewportClient();

	//~ Begin UArmyEditorViewportClient Interface
	virtual void Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI) override;
	virtual void Draw(FViewport* InViewport, FCanvas* SceneCanvas) override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed = 1.f, bool bGamepad = false) override;
	virtual bool InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples = 1, bool bGamepad = false) override;
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	bool InputWidgetDelta(FViewport* InViewport, EAxisList::Type InCurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale) override;
	virtual void TrackingStopped() override;
	virtual FArmyWidget::EWidgetMode GetWidgetMode() const override;
	virtual FVector GetWidgetLocation() const override;
	virtual FMatrix GetWidgetCoordSystem() const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) override;
	virtual void ProcessScreenShots(FViewport* Viewport);
	virtual FLinearColor GetBackgroundColor() const override;
	virtual bool LocalLineTraceSingleByChannel(FVector& OutHit, FVector& OutHitNormal, int32& HitObjectType/*摆放类型（1：地面，2：顶面：3：墙面）*/, const FVector& Start, const FVector& End) override;
	virtual float GetWallHeight() override;
	virtual void Callback_PreSelectionNone() override;
	virtual void Callback_SelectionNone() override;
	virtual void Callback_SelectionChanged(UObject* _NewSelectionObject) override;
	//~ End UArmyEditorViewportClient Interface
	void LockViewPortClient(bool locked);
	bool IsLockViewPortClient() { return LocalCamera; };
	/** 设置当前绘制模式 */
	void SetDrawMode(EArmyDrawMode DrawMode);

	FVector2D GetCurrentMousePos();

	/**
	* 魏小坤
	*/
	void ApplyDeltaToActor(AActor* InActor, const FVector& InDeltaDrag, const FRotator& InDeltaRot, const FVector& InDeltaScale) override;

	/**
	* 魏小坤
	*/
	void ZoomCanvas(const struct FArmyInputEventState& InputState);
	/**
	* 特殊坐标系
	* @飞舞轻扬
	*/
	void RegisterXRObjectVisualizer(EObjectType ObjectType, TSharedPtr<class FArmyObjectVisualizer> Visualizer);
	TSharedPtr<class FArmyObjectVisualizer> FindXRObjectVisualizer(EObjectType ObjectType) const;
	void DrawXRObjectVisualizers(const FSceneView* View, FPrimitiveDrawInterface* PDI);
	void DrawXRObjectVisualizersHUD(const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas);
	void UnregisterXRObjectVisualizer(EObjectType ObjectType);
	void ClearVisualizersForSelection();
	void AddVisualizersForSelection(TSharedPtr<class FArmyObject>& Object);
	TArray<EObjectType>& GetRegisteredXRObjectTypes() { return RegisteredXRObjectTypes; }
	FArmyObjectVisualizerManager GetObjectVisualizerManager() { return XRObjectVisManager; }
	TArray<struct FCachedXRObjectVisualizer> GetVisualizersForSelection() { return VisualizersForSelection; }
public:
	ScreenShotDelegate ScreenShotFun;

	bool NotShotBackColor = false;

	TSharedPtr<SOverlay> ViewportOverlayWidget;
private:

	bool LocalCamera;
	FArmyObjectVisualizerManager XRObjectVisManager;

	/**
	* 特殊坐标系
	* @飞舞轻扬
	*/
	TMap< EObjectType, TSharedPtr<class FArmyObjectVisualizer> > XRObjectVisualizerMap;
	TArray<struct FCachedXRObjectVisualizer>VisualizersForSelection;
	TArray<EObjectType> RegisteredXRObjectTypes;

};

extern XR_API UArmyViewportClient* GVC;