#include "ArmyEditorViewportClient.h"
#include "ArmyUnrealWidget.h"
#include "ArmyMouseDeltaTracker.h"
#include "ArmyEditorViewportClickHandlers.h"
#include "HModel.h"
#include "ArmyActorPositioning.h"
#include "Camera/CameraComponent.h"
#include "ArmyEditorEngine.h"
#include "ArmyEditorModeManager.h"
#include "ArmyEditor.h"
#include "ArmySpotLightActor.h"
#include "ArmyPointLightActor.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyLevelAssetBoardActor.h"
#include "ArmyGroupActor.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "ArmyDragTool_FrustumSelect.h"
#include "ArmyDragTool_BoxSelect.h"
#include "PhysicsEngine/BodySetup.h"
#include "ArmyCameraController.h"
#include "ArmyEditorViewportCommands.h"
#include "Engine/Console.h"
#include "EngineModule.h"
#include "Engine/Canvas.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Components/SphereReflectionCaptureComponent.h"
#include "Components/LineBatchComponent.h"
#include "ArmySlateModule.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "SImage.h"
#include "ArmyMath.h"
#include "ArmySceneData/Public/Data/HardModeData/DrawStyle/ArmyBaseEditStyle.h"
#include "ArmyResourceModule.h"
#include "ArmyActorConstant.h"
//@  ArmyLightmass
#include "ArmyWallActor.h"
#include "ArmyWindowActor.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyHelpPolygonArea.h"

#define LOCTEXT_NAMESPACE "ArmyEditorViewportClient"

static const float MIN_ACTOR_BOUNDS_EXTENT = 1.0f;

namespace ArmyOrbitConstants
{
	const float OrbitPanSpeed = 1.0f;
	const float IntialLookAtDistance = 1024.f;
}

namespace ArmyFocusConstants
{
	const float TransitionTime = 0.25f;
}

namespace
{
	static const float GridSize = 20480.0f;
	static const int32 CellSize = 160;
	static const float AutoViewportOrbitCameraTranslate = 256.0f;
	static const float LightRotSpeed = 0.22f;
}

//#define MIN_ORTHOZOOM				250.0					/* Limit of 2D viewport zoom in */
//#define MAX_ORTHOZOOM				MAX_FLT					/* Limit of 2D viewport zoom out */
#define MIN_ORTHOZOOM				10.0					/* Limit of 2D viewport zoom in */
#define MAX_ORTHOZOOM				1800000.0f					/* Limit of 2D viewport zoom out */

IMPLEMENT_HIT_PROXY(HArmyObjectBaseProxy, HHitProxy);

float ComputeOrthoZoomFactor(const float ViewportWidth)
{
	float Ret = 1.0f;

	//if (CVarAlignedOrthoZoom.GetValueOnGameThread())
	//{
	// We want to have all ortho view ports scale the same way to have the axis aligned with each other.
	// So we take out the usual scaling of a view based on it's width.
	// That means when a view port is resized in x or y it shows more content, not the same content larger (for x) or has no effect (for y).
	// 500 is to get good results with existing view port settings.
	Ret = ViewportWidth / 500.0f;
	//}

	return Ret;
}

FArmyViewportCursorLocation::FArmyViewportCursorLocation(const FSceneView* View, UArmyEditorViewportClient* InViewportClient, int32 X, int32 Y)
	: Origin(ForceInit), Direction(ForceInit), CursorPos(X, Y)
{
	FVector4 ScreenPos = View->PixelToScreen(X, Y, 0);

	const FMatrix InvViewMatrix = View->ViewMatrices.GetInvViewMatrix();
	const FMatrix InvProjMatrix = View->ViewMatrices.GetInvProjectionMatrix();

	const float ScreenX = ScreenPos.X;
	const float ScreenY = ScreenPos.Y;

	ViewportClient = InViewportClient;

	if (ViewportClient->IsPerspective())
	{
		Origin = View->ViewMatrices.GetViewOrigin();
		Direction = InvViewMatrix.TransformVector(FVector(InvProjMatrix.TransformFVector4(FVector4(ScreenX * GNearClippingPlane, ScreenY * GNearClippingPlane, 0.0f, GNearClippingPlane)))).GetSafeNormal();
	}
	else
	{
		Origin = InvViewMatrix.TransformFVector4(InvProjMatrix.TransformFVector4(FVector4(ScreenX, ScreenY, 0.5f, 1.0f)));
		Direction = InvViewMatrix.TransformVector(FVector(0, 0, 1)).GetSafeNormal();
	}
}

FArmyViewportCursorLocation::~FArmyViewportCursorLocation()
{
}

FArmyViewportClick::FArmyViewportClick(const FSceneView* View, UArmyEditorViewportClient* ViewportClient, FKey InKey, EInputEvent InEvent, int32 X, int32 Y)
	: FArmyViewportCursorLocation(View, ViewportClient, X, Y)
	, Key(InKey), Event(InEvent)
{
	ControlDown = ViewportClient->IsCtrlPressed();
	ShiftDown = ViewportClient->IsShiftPressed();
	AltDown = ViewportClient->IsAltPressed();
}

FArmyViewportClick::~FArmyViewportClick()
{

}

FArmyViewportCameraTransform::FArmyViewportCameraTransform()
	: TransitionCurve(new FCurveSequence(0.0f, ArmyFocusConstants::TransitionTime, ECurveEaseFunction::CubicOut))
	, ViewLocation(FVector::ZeroVector)
	, ViewRotation(FRotator::ZeroRotator)
	, DesiredLocation(FVector::ZeroVector)
	, LookAt(FVector::ZeroVector)
	, StartLocation(FVector::ZeroVector)
	, OrthoZoom(DEFAULT_ORTHOZOOM)
{}

void FArmyViewportCameraTransform::SetLocation(const FVector& Position)
{
	ViewLocation = Position;
	DesiredLocation = ViewLocation;
}

void FArmyViewportCameraTransform::TransitionToLocation(const FVector& InDesiredLocation, TWeakPtr<SWidget> EditorViewportWidget, bool bInstant)
{
	// 	if (bInstant || !EditorViewportWidget.IsValid())
	// 	{
	// 		SetLocation(InDesiredLocation);
	// 		TransitionCurve->JumpToEnd();
	// 	}
	// 	else
	// 	{
	// 		DesiredLocation = InDesiredLocation;
	// 		StartLocation = ViewLocation;
	// 
	// 		TransitionCurve->Play(EditorViewportWidget.Pin().ToSharedRef());
	// 	}
}

bool FArmyViewportCameraTransform::UpdateTransition()
{
	bool bIsAnimating = false;
	if (TransitionCurve->IsPlaying() || ViewLocation != DesiredLocation)
	{
		float LerpWeight = TransitionCurve->GetLerp();

		if (LerpWeight == 1.0f)
		{
			// Failsafe for the value not being exact on lerps
			ViewLocation = DesiredLocation;
		}
		else
		{
			ViewLocation = FMath::Lerp(StartLocation, DesiredLocation, LerpWeight);
		}


		bIsAnimating = true;
	}

	return bIsAnimating;
}

FMatrix FArmyViewportCameraTransform::ComputeOrbitMatrix() const
{
	FTransform Transform =
		FTransform(-LookAt) *
		FTransform(FRotator(0, ViewRotation.Yaw, 0)) *
		FTransform(FRotator(0, 0, ViewRotation.Pitch)) *
		FTransform(FVector(0, (ViewLocation - LookAt).Size(), 0));

	return Transform.ToMatrixNoScale() * FInverseRotationMatrix(FRotator(0, 90.f, 0));
}

bool FArmyViewportCameraTransform::IsPlaying()
{
	return TransitionCurve->IsPlaying();
}

float UArmyEditorViewportClient::GetCameraSpeed() const
{
    return 0.09375f;
}

//UArmyEditorViewportClient* GArmyVC = NULL;

UArmyEditorViewportClient::UArmyEditorViewportClient()
	: Viewport(NULL)
	, ViewportType(EArmyLevelViewportType::LVT_Perspective)
	, ViewFOV(90.f)
	, FOVAngle(90.f)
	, AspectRatio(1.777777f)
	, bWidgetAxisControlledByDrag(false)
	, bNeedsRedraw(true)
	, bNeedsInvalidateHitProxy(false)
	, bUsingOrbitCamera(false)
	, Widget(new FArmyWidget)
	, MouseDeltaTracker(new FArmyMouseDeltaTracker)
	, CameraController(new FArmyEditorCameraController())
	, CameraUserImpulseData(new FArmyCameraControllerUserImpulseData())
	, bHasMouseMovedSinceClick(false)
	, LastMouseX(0)
	, LastMouseY(0)
	, CachedMouseX(0)
	, CachedMouseY(0)
	, CurrentMousePos(-1, -1)
	, bIsTracking(false)
	, bDraggingByHandle(false)
	, RealTimeFrameCount(0)
	, bDuplicateOnNextDrag(false)
	, GestureMoveForwardBackwardImpulse(0.0f)
	, bIsRealtime(false)
	, bStoredRealtime(false)
	, bShouldCheckHitProxy(false)
	, bUsesDrawHelper(true)
	, bIsCameraMoving(false)
	, bIsCameraMovingOnTick(false)
	//, EditorViewportWidget(InEditorViewportWidget)
	, NearPlane(-1.0f)
	, FarPlane(0.0f)
	, bShouldInvalidateViewportWidget(false)

	, bDuplicateActorsInProgress(false)
	, bIsTrackingBrushModification(false)
	, bOnlyMovedPivot(false)
	, bUseControllingActorViewInfo(false)
	, ViewState()
	//, DroppedObject(NULL)
	, bDroppingPreviewTexture(false)
	, bEnableClick(true)
	, AddMaterialCount(0)
	, bForceHiddenWidget(false)
	, bDrawAxes(true)
	, bAutoMultiSelectEnabled(false)
	, bAdsorbed(false)
	, bSnapTranslate(false)
	, bSnapTranslateTemp(false)
	, AdsorbDistance(15.f)
	, MouseScale(FVector2D(1, 1))
	, bIsOrbit(false)
	, bUserPlayerControllerView(false)
	, bEnableBSPSelectAble(true)
{
	//GArmyVC = this;

	if (this == GetClass()->GetDefaultObject())
		return;

	//if (GArmyEditor)
	//    GArmyEditor->EditorViewportClients.Add(this);

	USelection::SelectNoneEvent.AddUObject(this, &UArmyEditorViewportClient::Callback_SelectionNone);
	USelection::SelectObjectEvent.AddUObject(this, &UArmyEditorViewportClient::Callback_SelectionChanged);
	GArmyEditor->SelectNonePreEvent.BindUObject(this, &UArmyEditorViewportClient::Callback_PreSelectionNone);

	ViewState.Allocate();
	ModeTools = &GArmyLevelEditorModeTools();
	ModeTools->SetWidgetMode(FArmyWidget::WM_Translate);
	Widget->SetUsesEditorModeTools(ModeTools);

	bUsesDrawHelper = true;
	DrawHelper.bDrawPivot = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.bDrawGrid = true;
	DrawHelper.GridColorAxis = FColor(160, 160, 160);
	DrawHelper.GridColorMajor = FColor(144, 144, 144);
	DrawHelper.GridColorMinor = FColor(128, 128, 128);
	DrawHelper.PerspectiveGridSize = GridSize;
	DrawHelper.NumCells = DrawHelper.PerspectiveGridSize / (CellSize * 2);

	ViewTransformOrthographic.SetLocation(FVector::ZeroVector);
	ViewTransformOrthographic.SetRotation(FRotator::ZeroRotator);
	ViewTransformOrthographic.SetOrthoZoom(800);
	CurCursor = EMouseCursor::Default;

	//@  ArmyLightmass
	DrawSelectedUVChannel = 1;
	DrawSelectedUVComponentIndex = 0;
}

UArmyEditorViewportClient::~UArmyEditorViewportClient()
{
	delete Widget;
	delete MouseDeltaTracker;

	delete CameraController;
	CameraController = NULL;

	delete CameraUserImpulseData;
	CameraUserImpulseData = NULL;
}

void UArmyEditorViewportClient::PostInitProperties()
{
	Super::PostInitProperties();

	//默认打开物体选择边框效果
	EngineShowFlags.SetSelection(true);
	EngineShowFlags.SetSelectionOutline(true);
	EngineShowFlags.SetVertexColors(true);
	//程序最小化不会执行渲染
	//ConsoleCommand("t.IdleWhenNotForeground 1");

	FVector InitLocation = FVector(0, 0, 200);
	FRotator InitRotation = FRotator(-15, 0, 0);
	SetInitialViewTransform(EArmyLevelViewportType::LVT_Perspective, InitLocation, InitRotation, 1);
}

bool UArmyEditorViewportClient::ToggleRealtime()
{
	SetRealtime(!bIsRealtime);
	return bIsRealtime;
}

void UArmyEditorViewportClient::SetRealtime(bool bInRealtime, bool bStoreCurrentValue)
{
	if (bStoreCurrentValue)
	{
		//Cache the Realtime and ShowStats flags
		bStoredRealtime = bIsRealtime;
		//bStoredShowStats = bShowStats;
	}

	bIsRealtime = bInRealtime;

	if (!bIsRealtime)
	{
		//SetShowStats(false);
	}
	else
	{
		bShouldInvalidateViewportWidget = true;
	}
}

void UArmyEditorViewportClient::RestoreRealtime(const bool bAllowDisable)
{
	if (bAllowDisable)
	{
		bIsRealtime = bStoredRealtime;
		//bShowStats = bStoredShowStats;
	}
	else
	{
		bIsRealtime |= bStoredRealtime;
		//bShowStats |= bStoredShowStats;
	}

	if (bIsRealtime)
	{
		bShouldInvalidateViewportWidget = true;
	}
}

bool UArmyEditorViewportClient::GetIsDraggingWidget()
{
	return Widget->IsDragging();
}

void UArmyEditorViewportClient::InvalidateViewportWidget()
{
	//if (EditorViewportWidget.IsValid())
	{
		// Invalidate the viewport widget to register its active timer
		//EditorViewportWidget.Pin()->Invalidate();
	}
	bShouldInvalidateViewportWidget = false;
}

void UArmyEditorViewportClient::RedrawRequested(FViewport* InViewport)
{
	bNeedsRedraw = true;
}

void UArmyEditorViewportClient::RequestInvalidateHitProxy(FViewport* InViewport)
{
	bNeedsInvalidateHitProxy = true;
}
void UArmyEditorViewportClient::GetViewportSize(FVector2D& out_ViewportSize) const
{
	if (Viewport != NULL)
	{
		out_ViewportSize.X = Viewport->GetSizeXY().X;
		out_ViewportSize.Y = Viewport->GetSizeXY().Y;
	}
}
bool UArmyEditorViewportClient::GetMousePosition(FVector2D& MousePosition) const
{
	bool bGotMousePosition = false;

	if (Viewport && FSlateApplication::Get().IsMouseAttached())
	{
		FIntPoint MousePos;
		Viewport->GetMousePos(MousePos);
		if (MousePos.X >= 0 && MousePos.Y >= 0)
		{
			MousePosition = FVector2D(MousePos);
			bGotMousePosition = true;
		}
	}

	return bGotMousePosition;
}
float UArmyEditorViewportClient::GetOrthoUnitsPerPixel(const FViewport* InViewport) const
{
	const float SizeX = InViewport->GetSizeXY().X;
	// 15.0f was coming from the CAMERA_ZOOM_DIV marco, seems it was chosen arbitrarily
	return GetOrthoZoom() / SizeX;//(GetOrthoZoom() / (SizeX * 1.f)) * ComputeOrthoZoomFactor(SizeX);
}

FString UArmyEditorViewportClient::UnrealUnitsToSiUnits(float UnrealUnits)
{
	// Put it in mm to start off with
	UnrealUnits *= 10.f;

	const int32 OrderOfMagnitude = UnrealUnits > 0 ? FMath::TruncToInt(FMath::LogX(10.0f, UnrealUnits)) : 0;

	// Get an exponent applied to anything >= 1,000,000,000mm (1000km)
	const int32 Exponent = (OrderOfMagnitude - 6) / 3;
	const FString ExponentString = Exponent > 0 ? FString::Printf(TEXT("e+%d"), Exponent * 3) : TEXT("");

	float ScaledNumber = UnrealUnits;

	// Factor the order of magnitude into thousands and clamp it to km
	const int32 OrderOfThousands = OrderOfMagnitude / 3;
	if (OrderOfThousands != 0)
	{
		// Scale units to m or km (with the order of magnitude in 1000s)
		ScaledNumber /= FMath::Pow(1000.f, OrderOfThousands);
	}

	// Round to 2 S.F.
	const TCHAR* Approximation = TEXT("");
	{
		const int32 ScaledOrder = OrderOfMagnitude % (FMath::Max(OrderOfThousands, 1) * 3);
		const float RoundingDivisor = FMath::Pow(10.f, ScaledOrder) / 10.f;
		const int32 Rounded = FMath::TruncToInt(ScaledNumber / RoundingDivisor) * RoundingDivisor;
		if (ScaledNumber - Rounded > KINDA_SMALL_NUMBER)
		{
			ScaledNumber = Rounded;
			Approximation = TEXT("~");
		}
	}

	if (OrderOfMagnitude <= 2)
	{
		// Always show cm not mm
		ScaledNumber /= 10;
	}

	static const TCHAR* UnitText[] = { TEXT("cm"), TEXT("m"), TEXT("km") };
	if (FMath::Fmod(ScaledNumber, 1.f) > KINDA_SMALL_NUMBER)
	{
		return FString::Printf(TEXT("%s%.1f%s%s"), Approximation, ScaledNumber, *ExponentString, UnitText[FMath::Min(OrderOfThousands, 2)]);
	}
	else
	{
		return FString::Printf(TEXT("%s%d%s%s"), Approximation, FMath::TruncToInt(ScaledNumber), *ExponentString, UnitText[FMath::Min(OrderOfThousands, 2)]);
	}
}

void UArmyEditorViewportClient::SetViewLocationForOrbiting(const FVector& LookAtPoint, float DistanceToCamera)
{
	FMatrix Matrix = FTranslationMatrix(-GetViewLocation());
	Matrix = Matrix * FInverseRotationMatrix(GetViewRotation());
	FMatrix CamRotMat = Matrix.InverseFast();
	FVector CamDir = FVector(CamRotMat.M[0][0], CamRotMat.M[0][1], CamRotMat.M[0][2]);
	SetViewLocation(LookAtPoint - DistanceToCamera * CamDir);
	SetLookAtLocation(LookAtPoint);
}

void UArmyEditorViewportClient::SetInitialViewTransform(EArmyLevelViewportType::Type InViewportType, const FVector& ViewLocation, const FRotator& ViewRotation, float InOrthoZoom)
{
	check(InViewportType < EArmyLevelViewportType::LVT_MAX);

	FArmyViewportCameraTransform& ViewTransform = (InViewportType == EArmyLevelViewportType::LVT_Perspective) ? ViewTransformPerspective : ViewTransformOrthographic;

	ViewTransform.SetLocation(ViewLocation);
	ViewTransform.SetRotation(ViewRotation);

	// Make a look at location in front of the camera
	const FQuat CameraOrientation = FQuat::MakeFromEuler(ViewRotation.Euler());
	FVector Direction = CameraOrientation.RotateVector(FVector(1, 0, 0));

	ViewTransform.SetLookAt(ViewLocation + Direction * ArmyOrbitConstants::IntialLookAtDistance);
	ViewTransform.SetOrthoZoom(InOrthoZoom);
}

void UArmyEditorViewportClient::ToggleOrbitCamera(bool bEnableOrbitCamera)
{
	if (bUsingOrbitCamera != bEnableOrbitCamera)
	{
		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();

		bUsingOrbitCamera = bEnableOrbitCamera;

		// Convert orbit view to regular view
		FMatrix OrbitMatrix = ViewTransform.ComputeOrbitMatrix();
		OrbitMatrix = OrbitMatrix.InverseFast();

		if (!bUsingOrbitCamera)
		{
			// Ensure that the view location and rotation is up to date to ensure smooth transition in and out of orbit mode
			ViewTransform.SetRotation(OrbitMatrix.Rotator());
		}
		else
		{
			FRotator ViewRotation = ViewTransform.GetRotation();

			bool bUpsideDown = (ViewRotation.Pitch < -90.0f || ViewRotation.Pitch > 90.0f || !FMath::IsNearlyZero(ViewRotation.Roll, KINDA_SMALL_NUMBER));

			// if the camera is upside down compute the rotation differently to preserve pitch
			// otherwise the view will pop to right side up when transferring to orbit controls
			if (bUpsideDown)
			{
				FMatrix OrbitViewMatrix = FTranslationMatrix(-ViewTransform.GetLocation());
				OrbitViewMatrix *= FInverseRotationMatrix(ViewRotation);
				OrbitViewMatrix *= FRotationMatrix(FRotator(0, 90.f, 0));

				FMatrix RotMat = FTranslationMatrix(-ViewTransform.GetLookAt()) * OrbitViewMatrix;
				FMatrix RotMatInv = RotMat.InverseFast();
				FRotator RollVec = RotMatInv.Rotator();
				FMatrix YawMat = RotMatInv * FInverseRotationMatrix(FRotator(0, 0, -RollVec.Roll));
				FMatrix YawMatInv = YawMat.InverseFast();
				FRotator YawVec = YawMat.Rotator();
				FRotator rotYawInv = YawMatInv.Rotator();
				ViewTransform.SetRotation(FRotator(-RollVec.Roll, YawVec.Yaw, 0));
			}
			else
			{
				ViewTransform.SetRotation(OrbitMatrix.Rotator());
			}
		}

		ViewTransform.SetLocation(OrbitMatrix.GetOrigin());
	}
}

void UArmyEditorViewportClient::FocusViewportOnBox(const FBox& BoundingBox, bool bInstant /* = false */)
{
	const FVector Position = BoundingBox.GetCenter();
	float Radius = FMath::Max(BoundingBox.GetExtent().Size(), 10.f);

	float AspectToUse = AspectRatio;
	FIntPoint ViewportSize = Viewport->GetSizeXY();
	if (!bUseControllingActorViewInfo && ViewportSize.X > 0 && ViewportSize.Y > 0)
	{
		AspectToUse = Viewport->GetDesiredAspectRatio();
	}

	const bool bEnable = false;
	ToggleOrbitCamera(bEnable);

	{
		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();

		if (!IsOrtho())
		{
			/**
			* We need to make sure we are fitting the sphere into the viewport completely, so if the height of the viewport is less
			* than the width of the viewport, we scale the radius by the aspect ratio in order to compensate for the fact that we have
			* less visible vertically than horizontally.
			*/
			if (AspectToUse > 1.0f)
			{
				Radius *= AspectToUse;
			}

			/**
			* Now that we have a adjusted radius, we are taking half of the viewport's FOV,
			* converting it to radians, and then figuring out the camera's distance from the center
			* of the bounding sphere using some simple trig.  Once we have the distance, we back up
			* along the camera's forward vector from the center of the sphere, and set our new view location.
			*/

			const float HalfFOVRadians = FMath::DegreesToRadians(ViewFOV / 2.0f);
			const float DistanceFromSphere = Radius / FMath::Tan(HalfFOVRadians);
			FVector CameraOffsetVector = ViewTransform.GetRotation().Vector() * -DistanceFromSphere;

			ViewTransform.SetLookAt(Position);
			//ViewTransform.TransitionToLocation(Position + CameraOffsetVector, EditorViewportWidget, bInstant);

		}
		else
		{
			// For ortho viewports just set the camera position to the center of the bounding volume.
			//SetViewLocation( Position );
			//ViewTransform.TransitionToLocation(Position, EditorViewportWidget, bInstant);

			if (!(Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl)))
			{
				/**
				* We also need to zoom out till the entire volume is in view.  The following block of code first finds the minimum dimension
				* size of the viewport.  It then calculates backwards from what the view size should be (The radius of the bounding volume),
				* to find the new OrthoZoom value for the viewport. The 15.0f is a fudge factor.
				*/
				float NewOrthoZoom;
				uint32 MinAxisSize = (AspectToUse > 1.0f) ? Viewport->GetSizeXY().Y : Viewport->GetSizeXY().X;
				float Zoom = Radius / (MinAxisSize / 2.0f);

				NewOrthoZoom = Zoom * (Viewport->GetSizeXY().X*15.0f);
				NewOrthoZoom = FMath::Clamp<float>(NewOrthoZoom, MIN_ORTHOZOOM, MAX_ORTHOZOOM);
				ViewTransform.SetOrthoZoom(NewOrthoZoom);
			}
		}
	}

	// Tell the viewport to redraw itself.
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////
FSceneView* UArmyEditorViewportClient::CalcSceneView(FSceneViewFamily* ViewFamily, const EStereoscopicPass StereoPass)
{
	const bool bStereoRendering = StereoPass != eSSP_FULL;

	FSceneViewInitOptions ViewInitOptions;

	// Takes care of HighDPI based screen percentage in editor viewport when not in VR editor.
	if (!bStereoRendering)
	{
		// Disables any screen percentage derived for game such as r.ScreenPercentage or FPostProcessSettings::ScreenPercentage.
		//ViewInitOptions.bDisableGameScreenPercentage = true;

		// Forces screen percentage showflag on so that we always upscale on HighDPI configuration.
		ViewFamily->EngineShowFlags.ScreenPercentage = true;
	}

	FArmyViewportCameraTransform& ViewTransform = GetViewTransform();
	const EArmyLevelViewportType::Type EffectiveViewportType = GetViewportType();

	ViewInitOptions.ViewOrigin = ViewTransform.GetLocation();
	const FRotator& ViewRotation = ViewTransform.GetRotation();

	const FIntPoint ViewportSizeXY = Viewport->GetSizeXY();

	FIntRect ViewRect = FIntRect(0, 0, ViewportSizeXY.X, ViewportSizeXY.Y);
	ViewInitOptions.SetViewRectangle(ViewRect);

	const bool bConstrainAspectRatio = bUseControllingActorViewInfo;// && ControllingActorViewInfo.bConstrainAspectRatio;
	const EAspectRatioAxisConstraint AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MaintainXFOV;//GetDefault<ULevelEditorViewportSettings>()->AspectRatioAxisConstraint;

	if (bUseControllingActorViewInfo)
	{
		ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewRotation) * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));

		//FMinimalViewInfo::CalculateProjectionMatrixGivenView(ControllingActorViewInfo, AspectRatioAxisConstraint, Viewport, /*inout*/ ViewInitOptions);
	}
	else
	{
		//
		if (EffectiveViewportType == EArmyLevelViewportType::LVT_Perspective)
		{
			if (bUsingOrbitCamera)
			{
				ViewInitOptions.ViewRotationMatrix = FTranslationMatrix(ViewInitOptions.ViewOrigin) * ViewTransform.ComputeOrbitMatrix();
			}
			else
			{
				ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewRotation);
			}

			ViewInitOptions.ViewRotationMatrix = ViewInitOptions.ViewRotationMatrix * FMatrix(
				FPlane(0, 0, 1, 0),
				FPlane(1, 0, 0, 0),
				FPlane(0, 1, 0, 0),
				FPlane(0, 0, 0, 1));

			float MinZ = GetNearClipPlane();
			float MaxZ = MinZ;
			// Avoid zero ViewFOV's which cause divide by zero's in projection matrix
			float MatrixFOV = FMath::Max(0.001f, ViewFOV) * (float)PI / 360.0f;

			if (bConstrainAspectRatio)
			{
				if ((int32)ERHIZBuffer::IsInverted != 0)
				{
					ViewInitOptions.ProjectionMatrix = FReversedZPerspectiveMatrix(
						MatrixFOV,
						MatrixFOV,
						1.0f,
						AspectRatio,
						MinZ,
						MaxZ
					);
				}
				else
				{
					ViewInitOptions.ProjectionMatrix = FPerspectiveMatrix(
						MatrixFOV,
						MatrixFOV,
						1.0f,
						AspectRatio,
						MinZ,
						MaxZ
					);
				}
			}
			else
			{
				float XAxisMultiplier;
				float YAxisMultiplier;

				if (((ViewportSizeXY.X > ViewportSizeXY.Y) && (AspectRatioAxisConstraint == AspectRatio_MajorAxisFOV)) || (AspectRatioAxisConstraint == AspectRatio_MaintainXFOV))
				{
					//if the viewport is wider than it is tall
					XAxisMultiplier = 1.0f;
					YAxisMultiplier = ViewportSizeXY.X / (float)ViewportSizeXY.Y;
				}
				else
				{
					//if the viewport is taller than it is wide
					XAxisMultiplier = ViewportSizeXY.Y / (float)ViewportSizeXY.X;
					YAxisMultiplier = 1.0f;
				}

				if ((int32)ERHIZBuffer::IsInverted != 0)
				{
					ViewInitOptions.ProjectionMatrix = FReversedZPerspectiveMatrix(
						MatrixFOV,
						MatrixFOV,
						XAxisMultiplier,
						YAxisMultiplier,
						MinZ,
						MaxZ
					);
				}
				else
				{
					ViewInitOptions.ProjectionMatrix = FPerspectiveMatrix(
						MatrixFOV,
						MatrixFOV,
						XAxisMultiplier,
						YAxisMultiplier,
						MinZ,
						MaxZ
					);
				}
			}
		}
		else
		{
			static_assert((int32)ERHIZBuffer::IsInverted != 0, "Check all the Rotation Matrix transformations!");
			float ZScale = 0.5f / HALF_WORLD_MAX;
			float ZOffset = HALF_WORLD_MAX;
			//The divisor for the matrix needs to match the translation code.
			const float Zoom = GetOrthoUnitsPerPixel(Viewport);

			float OrthoWidth = Zoom * ViewportSizeXY.X / 2.0f;
			float OrthoHeight = Zoom * ViewportSizeXY.Y / 2.0f;

			if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoXY || EffectiveViewportType == EArmyLevelViewportType::LVT_OnlyCanvas)
			{
				ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewRotation);

				ViewInitOptions.ViewRotationMatrix = /*ViewInitOptions.ViewRotationMatrix * */FMatrix(
					FPlane(1, 0, 0, 0),
					FPlane(0, -1, 0, 0),
					FPlane(0, 0, -1, 0),
					FPlane(0, 0, -ViewInitOptions.ViewOrigin.Z, 1));
			}
			else if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoXZ)
			{
				ViewInitOptions.ViewRotationMatrix = FMatrix(
					FPlane(1, 0, 0, 0),
					FPlane(0, 0, -1, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, -ViewInitOptions.ViewOrigin.Y, 1));
			}
			else if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoYZ)
			{
				ViewInitOptions.ViewRotationMatrix = FMatrix(
					FPlane(0, 0, 1, 0),
					FPlane(1, 0, 0, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, ViewInitOptions.ViewOrigin.X, 1));
			}
			else if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoNegativeXY)
			{
				ViewInitOptions.ViewRotationMatrix = FMatrix(
					FPlane(-1, 0, 0, 0),
					FPlane(0, -1, 0, 0),
					FPlane(0, 0, 1, 0),
					FPlane(0, 0, -ViewInitOptions.ViewOrigin.Z, 1));
			}
			else if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoNegativeXZ)
			{
				ViewInitOptions.ViewRotationMatrix = FMatrix(
					FPlane(-1, 0, 0, 0),
					FPlane(0, 0, 1, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, -ViewInitOptions.ViewOrigin.Y, 1));
			}
			else if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoNegativeYZ)
			{
				ViewInitOptions.ViewRotationMatrix = FMatrix(
					FPlane(0, 0, -1, 0),
					FPlane(-1, 0, 0, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, ViewInitOptions.ViewOrigin.X, 1));
			}
			else if (EffectiveViewportType == EArmyLevelViewportType::LVT_OrthoFreelook)
			{
				ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewRotation);
				ViewInitOptions.ViewRotationMatrix = ViewInitOptions.ViewRotationMatrix*FMatrix(
					FPlane(0, 0, 1, 0),
					FPlane(1, 0, 0, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, 0, 1));
			}
			else
			{
				// Unknown viewport type
				check(false);
			}

			ViewInitOptions.ProjectionMatrix = FReversedZOrthoMatrix(
				OrthoWidth,
				OrthoHeight,
				ZScale,
				ZOffset
			);
		}

		if (bConstrainAspectRatio)
		{
			ViewInitOptions.SetConstrainedViewRectangle(Viewport->CalculateViewExtents(AspectRatio, ViewRect));
		}
	}

	ViewInitOptions.ViewFamily = ViewFamily;
	ViewInitOptions.SceneViewStateInterface = ViewState.GetReference();
	ViewInitOptions.StereoPass = StereoPass;

	ViewInitOptions.ViewElementDrawer = this;

	ViewInitOptions.BackgroundColor = GetBackgroundColor();

	//ViewInitOptions.EditorViewBitflag = (uint64)1 << 0/*ViewIndex*/, // send the bit for this view - each actor will check it's visibility bits against this

	// for ortho views to steal perspective view origin
	//ViewInitOptions.OverrideLODViewOrigin = FVector::ZeroVector;
	//ViewInitOptions.bUseFauxOrthoViewPos = true;

	if (bUseControllingActorViewInfo)
	{
		ViewInitOptions.bUseFieldOfViewForLOD = false;// ControllingActorViewInfo.bUseFieldOfViewForLOD;
	}

	ViewInitOptions.OverrideFarClippingPlaneDistance = FarPlane;
	ViewInitOptions.CursorPos = CurrentMousePos;

	FSceneView* View = new FSceneView(ViewInitOptions);

	View->ViewLocation = ViewTransform.GetLocation();
	View->ViewRotation = ViewRotation;// FRotator(0, 0, 0);

	View->SubduedSelectionOutlineColor = GEngine->GetSubduedSelectionOutlineColor();

	ViewFamily->Views.Add(View);

	View->StartFinalPostprocessSettings(View->ViewLocation);

	OverridePostProcessSettings(*View);

	//ViewInitOptions.EditorViewScreenPercentage = GetEditorScreenPercentage();

	View->EndFinalPostprocessSettings(ViewInitOptions);

	MouseScale.X = (float)View->ViewRect.Max.X / (float)View->UnscaledViewRect.Max.X;
	MouseScale.Y = (float)View->ViewRect.Max.Y / (float)View->UnscaledViewRect.Max.Y;

	return View;
}

void UArmyEditorViewportClient::ReceivedFocus(FViewport* InViewport)
{
	// Viewport has changed got to reset the cursor as it could of been left in any state
	//UpdateRequiredCursorVisibility();
	//ApplyRequiredCursorVisibility(true);

	// Force a cursor update to make sure its returned to default as it could of been left in any state and wont update itself till an action is taken
	//SetRequiredCursorOverride(false, EMouseCursor::Default);
	FSlateApplication::Get().QueryCursor();

	ModeTools->ReceivedFocus(this, Viewport);
}

void UArmyEditorViewportClient::LostFocus(FViewport* InViewport)
{
	StopTracking();
	ModeTools->LostFocus(this, Viewport);
}

void UArmyEditorViewportClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	ConditionalCheckHoveredHitProxy();
	UpdateCameraMovement(DeltaSeconds);
	UpdateMouseDelta();
	UpdateDroppedObject();
}

EMouseCursor::Type UArmyEditorViewportClient::GetCursor(FViewport* InViewport, int32 X, int32 Y)
{
	//EMouseCursor::Type MouseCursor = EMouseCursor::Default;

	if (InViewport->IsCursorVisible() && !bWidgetAxisControlledByDrag)
	{
		// allow editor modes to override cursor
		/*EMouseCursor::Type EditorModeCursor = EMouseCursor::Default;
		if (ModeTools->GetCursor(EditorModeCursor))
		{
		MouseCursor = EditorModeCursor;
		}
		else
		{*/

		static const auto ScreenPercentageCVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.ScreenPercentage"));
		float Value = ScreenPercentageCVar->GetValueOnGameThread();
		float MouseX = X * Value / 100.f;
		float MouseY = Y * Value / 100.f;

		HHitProxy* HitProxy = InViewport->GetHitProxy(MouseX, MouseY);

		// Change the mouse cursor if the user is hovering over something they can interact with.
		if (HitProxy && !bUsingOrbitCamera)
		{
			//MouseCursor = HitProxy->GetMouseCursor();
			bShouldCheckHitProxy = true;
		}
		else
		{
			// Turn off widget highlight if there currently is one
			if (Widget->GetCurrentAxis() != EAxisList::None)
			{
				SetCurrentWidgetAxis(EAxisList::None);
				Invalidate(false, false);
			}
		}
		//}
	}

	CachedMouseX = X;
	CachedMouseY = Y;

	return CurCursor;
	//return MouseCursor;
}

bool UArmyEditorViewportClient::IsOrtho() const
{
	return !IsPerspective();
}

bool UArmyEditorViewportClient::IsPerspective() const
{
	return (GetViewportType() == EArmyLevelViewportType::LVT_Perspective);
}

EArmyLevelViewportType::Type UArmyEditorViewportClient::GetViewportType() const
{
	return ViewportType;
}

void UArmyEditorViewportClient::SetViewportType(EArmyLevelViewportType::Type InViewportType)
{
	// 从环绕图切换到透视图时还原摄像机原来的位置
	if (bIsOrbit && InViewportType == EArmyLevelViewportType::LVT_Perspective)
	{
		bIsOrbit = false;
		SetViewLocation(LastPerspectiveViewLoc);
		SetViewRotation(LastPerspectiveViewRot);
	}

	ViewportType = InViewportType;
	OnViewTypeChanged.Broadcast(ViewportType);
	Invalidate();
}

//void UArmyEditorViewportClient::UpdateCameraMovement(float DeltaTime)
//{
//	return;
//	// 获取摄像机位置旋转
//	FVector NewViewLocation = FVector::ZeroVector;
//	FRotator NewViewRotation = FRotator::ZeroRotator;
//	if (GetViewportType() == EArmyLevelViewportType::LVT_Perspective)
//	{
//		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
//		{
//			(*Iterator)->GetPlayerViewPoint(NewViewLocation, NewViewRotation);
//		}
//	}
//	else if (GetViewportType() == EArmyLevelViewportType::LVT_OrthoXY)
//	{
//		TArray<USceneComponent*> COMs;
//		GetWorld()->GetFirstPlayerController()->GetPawn()->GetRootComponent()->GetChildrenComponents(true, COMs);
//		for (auto& It : COMs)
//		{
//			UCameraComponent* CameraCOM = Cast<UCameraComponent>(It);
//			if (CameraCOM)
//			{
//				NewViewLocation = CameraCOM->GetComponentLocation();
//				NewViewRotation = FRotator(0, 90, 0);
//				break;
//			}
//		}
//	}
//
//	if (!NewViewLocation.Equals(GetViewLocation(), SMALL_NUMBER) ||
//		NewViewRotation != GetViewRotation())// ||
//											 //!FMath::IsNearlyEqual(NewViewFOV, ViewFOV, float(SMALL_NUMBER)))
//	{
//		//Invalidate(true, true);
//	}
//
//	SetViewLocation(NewViewLocation);
//	SetViewRotation(NewViewRotation);
//}

void UArmyEditorViewportClient::UpdateCameraMovement(float DeltaTime)
{
	if (ViewportConsole && ViewportConsole->ConsoleActive())
	{
		return;
	}
	// We only want to move perspective cameras around like this
	if (Viewport != NULL && IsPerspective() && !ShouldOrbitCamera())
	{
		ToggleOrbitCamera(false);

		// Certain keys are only available while the flight camera input mode is active
		const bool bUsingFlightInput = true;

		// 是否按下的辅助键
		const bool bUnmodifiedPress = !IsAltPressed() && !IsShiftPressed() && !IsCtrlPressed() && !IsCmdPressed();
		const bool bRemapArrowKeys = bUnmodifiedPress;
		//没有拖拽操作，才会移动
		const bool bRemapWASDKeys = bUnmodifiedPress && !MouseDeltaTracker->UsingDragTool();

		// Apply impulse from magnify gesture and reset impulses if we're using WASD keys
		CameraUserImpulseData->MoveForwardBackwardImpulse = GestureMoveForwardBackwardImpulse;
		CameraUserImpulseData->MoveRightLeftImpulse = 0.0f;
		CameraUserImpulseData->MoveUpDownImpulse = 0.0f;
		CameraUserImpulseData->ZoomOutInImpulse = 0.0f;
		CameraUserImpulseData->RotateYawImpulse = 0.0f;
		CameraUserImpulseData->RotatePitchImpulse = 0.0f;
		CameraUserImpulseData->RotateRollImpulse = 0.0f;

		GestureMoveForwardBackwardImpulse = 0.0f;

		bool MoveKeysPressing = false;

		// Forward/back
		if ((bRemapWASDKeys && Viewport->KeyState(EKeys::W)) || (bRemapArrowKeys && Viewport->KeyState(EKeys::Up)))
		{
			CameraUserImpulseData->MoveForwardBackwardImpulse += 1.f;
			MoveKeysPressing = true;
		}
		if ((bRemapWASDKeys && Viewport->KeyState(EKeys::S)) || (bRemapArrowKeys && Viewport->KeyState(EKeys::Down)))
		{
			CameraUserImpulseData->MoveForwardBackwardImpulse -= 1.f;
			MoveKeysPressing = true;
		}

		// Right/left
		if ((bRemapWASDKeys && Viewport->KeyState(EKeys::D)) || (bRemapArrowKeys && Viewport->KeyState(EKeys::Right)))
		{
			CameraUserImpulseData->MoveRightLeftImpulse += 1.f;
			MoveKeysPressing = true;
		}
		if ((bRemapWASDKeys && Viewport->KeyState(EKeys::A)) || (bRemapArrowKeys && Viewport->KeyState(EKeys::Left)))
		{
			CameraUserImpulseData->MoveRightLeftImpulse -= 1.f;
			MoveKeysPressing = true;
		}

		/*	FArmyOutputLogModule& OutputLogModule = FModuleManager::LoadModuleChecked<FArmyOutputLogModule>(TEXT("ArmyOutputLog"));
		OutputLogModule.SetDebugCategoryContent(2,
		FString::Printf(
		TEXT("DeltaTime:%.4f\rDPressed:%d\rAPressed:%d\rMoveRightLeftImpulse:%.4f"),
		DeltaTime, DPressed, APressed, CameraUserImpulseData->MoveRightLeftImpulse)
		);*/

		// Up/down
		if (bRemapWASDKeys && Viewport->KeyState(EKeys::E))
			CameraUserImpulseData->MoveUpDownImpulse += 1.f;
		if (bRemapWASDKeys && Viewport->KeyState(EKeys::Q))
			CameraUserImpulseData->MoveUpDownImpulse -= 1.f;

		// Zoom FOV out/in
		if (bRemapWASDKeys && Viewport->KeyState(EKeys::Z))
			CameraUserImpulseData->ZoomOutInImpulse += 1.0f;
		if (bRemapWASDKeys && Viewport->KeyState(EKeys::C))
			CameraUserImpulseData->ZoomOutInImpulse -= 1.0f;

		if (!CameraController->IsRotating())
		{
			CameraController->GetConfig().bForceRotationalPhysics = false;
		}

		FVector NewViewLocation = GetViewLocation();
		FRotator NewViewRotation = GetViewRotation();
		FVector NewViewEuler = GetViewRotation().Euler();
		float NewViewFOV = ViewFOV;

		const float CameraSpeed = GetCameraSpeed();
		const float FinalCameraSpeedScale = /*FlightCameraSpeedScale **/ CameraSpeed;

		const bool bAllowRecoilIfNoImpulse = false;

		// Update the camera's position, rotation and FOV
		float EditorMovementDeltaUpperBound = 1.0f;	// Never "teleport" the camera further than a reasonable amount after a large quantum

													// Check whether the camera is being moved by the mouse or keyboard
		bool bHasMovement = bIsTracking;

		if ((*CameraUserImpulseData).RotateYawVelocityModifier != 0.0f ||
			(*CameraUserImpulseData).RotatePitchVelocityModifier != 0.0f ||
			(*CameraUserImpulseData).RotateRollVelocityModifier != 0.0f ||
			(*CameraUserImpulseData).MoveForwardBackwardImpulse != 0.0f ||
			(*CameraUserImpulseData).MoveRightLeftImpulse != 0.0f ||
			(*CameraUserImpulseData).MoveUpDownImpulse != 0.0f ||
			(*CameraUserImpulseData).ZoomOutInImpulse != 0.0f ||
			(*CameraUserImpulseData).RotateYawImpulse != 0.0f ||
			(*CameraUserImpulseData).RotatePitchImpulse != 0.0f ||
			(*CameraUserImpulseData).RotateRollImpulse != 0.0f
			)
		{
			bHasMovement = true;
		}

		BeginCameraMovement(bHasMovement);

		CameraController->UpdateSimulation(
			*CameraUserImpulseData,
			FMath::Min(DeltaTime, EditorMovementDeltaUpperBound),
			bAllowRecoilIfNoImpulse,
			FinalCameraSpeedScale,
			NewViewLocation,
			NewViewEuler,
			NewViewFOV);

		// We'll zero out rotation velocity modifier after updating the simulation since these actions
		// are always momentary -- that is, when the user mouse looks some number of pixels,
		// we increment the impulse value right there
		{
			CameraUserImpulseData->RotateYawVelocityModifier = 0.0f;
			CameraUserImpulseData->RotatePitchVelocityModifier = 0.0f;
			CameraUserImpulseData->RotateRollVelocityModifier = 0.0f;
		}


		// Check for rotation difference within a small tolerance, ignoring winding
		if (!GetViewRotation().GetDenormalized().Equals(FRotator::MakeFromEuler(NewViewEuler).GetDenormalized(), SMALL_NUMBER))
		{
			NewViewRotation = FRotator::MakeFromEuler(NewViewEuler);
		}

		// See if translation/rotation have changed
		const bool bTransformDifferent = !NewViewLocation.Equals(GetViewLocation(), SMALL_NUMBER) || NewViewRotation != GetViewRotation();
		// See if FOV has changed
		const bool bFOVDifferent = !FMath::IsNearlyEqual(NewViewFOV, ViewFOV, float(SMALL_NUMBER));

		// If something has changed, tell the actor
		if (bTransformDifferent || bFOVDifferent)
		{
			// Something has changed!
			const bool bInvalidateChildViews = true;

			// When flying the camera around the hit proxies dont need to be invalidated since we are flying around and not clicking on anything
            if (!MoveKeysPressing)
            {
                Invalidate(bInvalidateChildViews, false);
            }

			// Update the FOV
			ViewFOV = NewViewFOV;

			// Actually move/rotate the camera
			if (bTransformDifferent)
			{
				MoveViewportPerspectiveCamera(
					NewViewLocation - GetViewLocation(),
					NewViewRotation - GetViewRotation());
			}

			//// Invalidate the viewport widget
			//if (EditorViewportWidget.IsValid())
			//{
			//	EditorViewportWidget.Pin()->Invalidate();
			//}
		}
	}
}

void UArmyEditorViewportClient::BeginCameraMovement(bool bHasMovement)
{
	// If there's new movement broadcast it
	if (bHasMovement)
	{
		if (!bIsCameraMoving)
		{
			// 			AActor* ActorLock = GetActiveActorLock().Get();
			// 			if (!bIsCameraMovingOnTick && ActorLock)
			// 			{
			// 				GEditor->BroadcastBeginCameraMovement(*ActorLock);
			// 			}
			bIsCameraMoving = true;
		}
	}
	else
	{
		bIsCameraMoving = false;
	}
}

void UArmyEditorViewportClient::EndCameraMovement()
{
	//// If there was movement and it has now stopped, broadcast it
	//if (bIsCameraMovingOnTick && !bIsCameraMoving)
	//{
	//	if (AActor* ActorLock = GetActiveActorLock().Get())
	//	{
	//		GEditor->BroadcastEndCameraMovement(*ActorLock);
	//	}
	//}
}

//鼠标滚轮旋转物体功能

 bool UArmyEditorViewportClient::CanMouseRotate() { return isAllowedRotateByMouse; }

 void UArmyEditorViewportClient::OpenMouseRotate() { isAllowedRotateByMouse = true; }

 void UArmyEditorViewportClient::CloseMouseRotate() { isAllowedRotateByMouse = false; }

void UArmyEditorViewportClient::UpdateMouseDelta()
{
	//如果正在建材市场拖拽物体，不做任何事情
	if (DroppedObjects.Num() > 0)
	{
		//return;
	}

	if (MouseDeltaTracker->UsingDragTool())
	{
		return;
	}

	if (bIsTracking && MouseDeltaTracker->GetTrackingWidgetMode() != GetWidgetMode())
	{
		StopTracking();
		return;
	}

	FVector DragDelta = MouseDeltaTracker->GetDelta();

	GArmyEditor->MouseMovement += DragDelta.GetAbs();

	if (Viewport)
	{
		if (!DragDelta.IsNearlyZero())
		{
			const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
			const bool MiddleMouseButtonDown = Viewport->KeyState(EKeys::MiddleMouseButton);
			const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
			const bool bIsUsingTrackpad = FSlateApplication::Get().IsUsingTrackpad();
			const bool bIsNonOrbitMiddleMouse = MiddleMouseButtonDown && !IsAltPressed();

			// Convert the movement delta into drag/rotation deltas
			FVector Drag;
			FRotator Rot;
			FVector Scale;
			EAxisList::Type CurrentAxis = Widget->GetCurrentAxis();
			if (IsOrtho() && (LeftMouseButtonDown || bIsUsingTrackpad) && RightMouseButtonDown)
			{
				bWidgetAxisControlledByDrag = false;
				Widget->SetCurrentAxis(EAxisList::None);
				MouseDeltaTracker->ConvertMovementDeltaToDragRot(this, DragDelta, Drag, Rot, Scale);
				Widget->SetCurrentAxis(CurrentAxis);
				CurrentAxis = EAxisList::None;
			}
			else
			{
				//if Absolute Translation, and not just moving the camera around
				if (IsUsingAbsoluteTranslation())
				{
					// Compute a view.
					FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
						Viewport,
						GetScene(),
						EngineShowFlags)
						.SetRealtimeUpdate(true));
					FSceneView* View = CalcSceneView(&ViewFamily);

					MouseDeltaTracker->AbsoluteTranslationConvertMouseToDragRot(View, this, Drag, Rot, Scale);
				}
				else
				{
					//非透视图，按下鼠标中键，拖拽画布
					if (IsOrtho() && MiddleMouseButtonDown)
					{
						WorldEndPos = PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0);
						FVector WorldDrageDelta = WorldStartPos - WorldEndPos;

						MouseDeltaTracker->ConvertMovementDeltaToDragRot(this, WorldDrageDelta, Drag, Rot, Scale);
					}
					else
					{
						MouseDeltaTracker->ConvertMovementDeltaToDragRot(this, DragDelta, Drag, Rot, Scale);
					}
				}
			}

			const bool bInputHandledByGizmos = InputWidgetDelta(Viewport, CurrentAxis, Drag, Rot, Scale);

			if (!Rot.IsZero())
			{
				Widget->UpdateDeltaRotation();
			}

			if (!bInputHandledByGizmos)
			{
				if (ShouldOrbitCamera())
				{
					bool bHasMovement = !DragDelta.IsNearlyZero();

					BeginCameraMovement(bHasMovement);

					FVector TempDrag;
					FRotator TempRot;
					InputAxisForOrbit(Viewport, DragDelta, TempDrag, TempRot);
				}
				else
				{
					// Disable orbit camera
					const bool bEnable = false;
					ToggleOrbitCamera(bEnable);

					if (ShouldPanOrDollyCamera())
					{
						bool bHasMovement = !Drag.IsNearlyZero() || !Rot.IsNearlyZero();

						BeginCameraMovement(bHasMovement);

						if (!IsOrtho())
						{
							const float CameraSpeed = GetCameraSpeed();
							Drag *= CameraSpeed;
						}
						MoveViewportCamera(Drag, Rot);
					}
				}
			}
			// Clean up
			MouseDeltaTracker->ReduceBy(DragDelta);

			Invalidate(false, false);
		}
	}

	//LastWorldPostion = View->PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0);
	if (Viewport)
		WorldStartPos = PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0.0f);

}

static bool IsOrbitRotationMode(FViewport* Viewport)
{
	bool	LeftMouseButton = Viewport->KeyState(EKeys::LeftMouseButton),
		MiddleMouseButton = Viewport->KeyState(EKeys::MiddleMouseButton),
		RightMouseButton = Viewport->KeyState(EKeys::RightMouseButton);
	return LeftMouseButton && !MiddleMouseButton && !RightMouseButton;
}

static bool IsOrbitPanMode(FViewport* Viewport)
{
	bool	LeftMouseButton = Viewport->KeyState(EKeys::LeftMouseButton),
		MiddleMouseButton = Viewport->KeyState(EKeys::MiddleMouseButton),
		RightMouseButton = Viewport->KeyState(EKeys::RightMouseButton);

	bool bAltPressed = Viewport->KeyState(EKeys::LeftAlt) || Viewport->KeyState(EKeys::RightAlt);

	return  (MiddleMouseButton && !LeftMouseButton && !RightMouseButton) || (!bAltPressed && MiddleMouseButton);
}

static bool IsOrbitZoomMode(FViewport* Viewport)
{
	bool	LeftMouseButton = Viewport->KeyState(EKeys::LeftMouseButton),
		MiddleMouseButton = Viewport->KeyState(EKeys::MiddleMouseButton),
		RightMouseButton = Viewport->KeyState(EKeys::RightMouseButton);

	return RightMouseButton || (LeftMouseButton && MiddleMouseButton);
}

void UArmyEditorViewportClient::InputAxisForOrbit(FViewport* InViewport, const FVector& DragDelta, FVector& Drag, FRotator& Rot)
{
	if (InViewport->KeyState(EKeys::RightMouseButton) && bIsOrbit)
	{
		ToggleOrbitCamera(true);

		FRotator TempRot = GetViewRotation();

		SetViewRotation(FRotator(0, 90, 0));
		FVector NewLoc;
		FRotator NewRot;
		ConvertMovementToDragRot(MouseDeltaTracker->GetDelta(), NewLoc, NewRot);
		SetViewRotation(TempRot);

		NewLoc.X = MouseDeltaTracker->GetDelta().X;

		FArmyViewportCameraTransform& ViewTransform = GetViewTransform();

		SetViewRotation(GetViewRotation() + FRotator(NewRot.Pitch, -NewRot.Yaw, NewRot.Roll));
		SetViewLocation(ViewTransform.ComputeOrbitMatrix().Inverse().GetOrigin());
	}
}

void UArmyEditorViewportClient::MarkMouseMovedSinceClick()
{
	if (!bHasMouseMovedSinceClick)
	{
		bHasMouseMovedSinceClick = true;
		//if we care about the cursor
		if (Viewport->IsCursorVisible() && Viewport->HasMouseCapture())
		{
			//force a refresh
			Viewport->UpdateMouseCursor(true);
		}
	}
}

bool UArmyEditorViewportClient::IsUsingAbsoluteTranslation() const
{
	//bool bIsHotKeyAxisLocked = Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);
	//bool bCameraLockedToWidget = Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
	// Screen-space movement must always use absolute translation
	//bool bScreenSpaceTransformation = Widget && (Widget->GetCurrentAxis() == EAxisList::Screen);
	//bool bAbsoluteMovementEnabled = GetDefault<ULevelEditorViewportSettings>()->bUseAbsoluteTranslation || bScreenSpaceTransformation;
	bool bCurrentWidgetSupportsAbsoluteMovement = FArmyWidget::AllowsAbsoluteTranslationMovement(GetWidgetMode());// || bScreenSpaceTransformation;
	bool bWidgetActivelyTrackingAbsoluteMovement = Widget && (Widget->GetCurrentAxis() != EAxisList::None);

	const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
	const bool MiddleMouseButtonDown = Viewport->KeyState(EKeys::MiddleMouseButton);
	const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);

	const bool bAnyMouseButtonsDown = (LeftMouseButtonDown || MiddleMouseButtonDown || RightMouseButtonDown);

	return (bCurrentWidgetSupportsAbsoluteMovement && bWidgetActivelyTrackingAbsoluteMovement && !IsOrtho() && bAnyMouseButtonsDown);
}

bool UArmyEditorViewportClient::InputKey(FViewport* InViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed /*= 1.f*/, bool bGamepad /*= false*/)
{
	bool bHandled = false;

	if (IgnoreInput())
	{
		return ViewportConsole ? ViewportConsole->InputKey(ControllerId, Key, Event, AmountDepressed, bGamepad) : false;
	}


	// route to subsystems that care
	bHandled = (ViewportConsole ? ViewportConsole->InputKey(ControllerId, Key, Event, AmountDepressed, bGamepad) : false);

	if (!bHandled)
	{
		ULocalPlayer* const TargetPlayer = GEngine->GetLocalPlayerFromControllerId(this, ControllerId);
		if (TargetPlayer && TargetPlayer->PlayerController)
		{
			TargetPlayer->PlayerController->InputKey(Key, Event, AmountDepressed, bGamepad);
		}
	}

	//bHandled = Super::InputKey(Viewport, ControllerId, Key, Event, AmountDepressed, bGamepad);

	if (bHandled)
		return bHandled;

	if (!bEnableClick)
		return false;


	FArmyInputEventState InputState(InViewport, Key, Event);
	bSnapTranslateTemp = InputState.IsAltButtonPressed();
	//SetLastKeyViewport();

	// Compute a view.
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		InViewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(true));
	FSceneView* View = CalcSceneView(&ViewFamily);

	int32 HitX = InViewport->GetMouseX();
	int32 HitY = InViewport->GetMouseY();

	const float XScale = (float)View->ViewRect.Max.X / (float)View->UnscaledViewRect.Max.X;
	const float YScale = (float)View->ViewRect.Max.Y / (float)View->UnscaledViewRect.Max.Y;
	HitX = HitX * XScale;
	HitY = HitY * YScale;

	// Compute the click location.
	if (InputState.IsAnyMouseButtonDown())
	{
		const FArmyViewportCursorLocation Cursor(View, this, HitX, HitY);
		const FArmyActorPositionTraceResult TraceResult = FArmyActorPositioning::TraceWorldForPositionWithDefault(Cursor, *View);
		GArmyEditor->UnsnappedClickLocation = TraceResult.Location;
		GArmyEditor->ClickLocation = TraceResult.Location;
		GArmyEditor->ClickPlane = FPlane(TraceResult.Location, TraceResult.SurfaceNormal);

		// Snap the new location if snapping is enabled
		//FSnappingUtils::SnapPointToGrid(GEditor->ClickLocation, FVector::ZeroVector);
	}

	//从建材市场拖拽物体点击鼠标左键时，放下模型，或者把贴壁纸。直接略过此次点击。
	if (DroppedObjects.Num() > 0 && Event == IE_Released)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (DroppedObjects.Num() == 1)
			{
				AActor* TempActor = Cast<AActor>(DroppedObjects[0]);
				if (TempActor)
				{
					GArmyEditor->UpdatePivotLocationForSelection();
					Invalidate(true, true);
					TArray<UObject*> Objects;
					SetDroppedObjects(Objects);
					PlaceActorToWorldDelegate.ExecuteIfBound(TempActor);
					CreateNewDroppedEvent.ExecuteIfBound();
				}
				else
				{
					DropObjectAtCoordinates(HitX, HitY, /*Viewport->GetMouseX(), Viewport->GetMouseY(),*/ DroppedObjects[0], true);
					Invalidate(true, true);
				}
			}
			else if (DroppedObjects.Num() > 1)
			{
				GArmyEditor->UpdatePivotLocationForSelection();
				Invalidate(true, true);
				TArray<UObject*> Objects;
				SetDroppedObjects(Objects);
			}
		}
		else if (Key == EKeys::RightMouseButton)
		{
			CancelDrop();
		}
		return true;
	}

	FString InputStr = FString::Printf(TEXT("LeftMousePressed:%d"),
		Viewport->KeyState(EKeys::LeftMouseButton)
	);

	//FArmyOutputLogModule& OutputLogModule = FModuleManager::LoadModuleChecked<FArmyOutputLogModule>(TEXT("ArmyOutputLog"));
	//OutputLogModule.SetDebugCategoryContent(2, InputStr);

	// Start tracking if any mouse button is down and it was a tracking event (MouseButton/Ctrl/Shift/Alt):
	if (InputState.IsAnyMouseButtonDown()
		&& (Event == IE_Pressed || Event == IE_Released)
		&& (InputState.IsMouseButtonEvent() || InputState.IsCtrlButtonEvent() || InputState.IsAltButtonEvent() || InputState.IsShiftButtonEvent()))
	{

		StartTrackingDueToInput(InputState);
		return true;
	}

	//取消拖拽的物体
	if (/*IsPerspective() && */Key == EKeys::Escape)
	{
		CancelDrop();
		return true;
	}

	// If we are tracking and no mouse button is down and this input event released the mouse button stop tracking and process any clicks if necessary
	if (bIsTracking && !InputState.IsAnyMouseButtonDown() && InputState.IsMouseButtonEvent())
	{
		// Handle possible mouse click viewport
		ProcessClickInViewport(InputState, *View);

		// Stop tracking if no mouse button is down
		StopTracking();

		bHandled |= true;
	}

	if ((Key == EKeys::MouseScrollUp || Key == EKeys::MouseScrollDown || Key == EKeys::Add || Key == EKeys::Subtract) && (Event == IE_Pressed || Event == IE_Repeat))
	{
		//选中物体时滑动鼠标中键可以旋转物体
		if (GArmyEditor->GetSelectedActorCount() > 0  && CanMouseRotate())
		{
			TArray<AActor*> SelectedActors;
			bool bCanRotation = true;
			for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
			{
				if (AActor* Actor = Cast<AActor>(*It))
				{
					SelectedActors.Add(Actor);
					if (Actor->Tags.Contains(TEXT("HydropowerActor")) || Actor->Tags.Contains(TEXT("HydropowerPipeActor"))
						|| Actor->Tags.Contains(TEXT("AreaHighLight")) || Actor->Tags.Contains(TEXT("Immovable")))//@欧石楠 门窗墙放样线条在立面模式下不可以旋转
						bCanRotation = false;
				}
			}
			if (bCanRotation)
				ApplyDeltaToActors(FVector(0, 0, 0), FRotator(0, Key == EKeys::MouseScrollUp ? 5 : -5, 0), FVector(0, 0, 0));
			bHandled |= true;
		}
		//如果是顶视图则缩放画布
		else if (IsOrtho())
		{
			OnOrthoZoom(InputState);
			bHandled |= true;
		}
		// 3D场景下缩放
		else if (IsPerspective())
		{
			OnDollyPerspectiveCamera(InputState);
		}
	}


	// Clear Duplicate Actors mode when ALT and all mouse buttons are released
	if (!InputState.IsAltButtonPressed() && !InputState.IsAnyMouseButtonDown())
	{
		bDuplicateActorsInProgress = false;
	}
	return bHandled;
}

void UArmyEditorViewportClient::StopTracking()
{
	if (bIsTracking)
	{
		//cache the axis of any widget we might be moving
		EAxisList::Type DraggingAxis = EAxisList::None;
		if (Widget != NULL)
		{
			DraggingAxis = Widget->GetCurrentAxis();
		}

		MouseDeltaTracker->EndTracking(this);

		Widget->SetCurrentAxis(EAxisList::None);

		// Force an immediate redraw of the viewport and hit proxy.
		// The results are required straight away, so it is not sufficient to defer the redraw until the next tick.
		if (Viewport)
		{
			Viewport->InvalidateHitProxy();
			Viewport->Draw();

			// If there are child viewports, force a redraw on those too
			/*FSceneViewStateInterface* ParentView = ViewState.GetReference();
			if (ParentView->IsViewParent())
			{
			for (UArmyEditorViewportClient* ViewportClient : GEditor->AllViewportClients)
			{
			if (ViewportClient != nullptr)
			{
			FSceneViewStateInterface* ViewportParentView = ViewportClient->ViewState.GetReference();

			if (ViewportParentView != nullptr &&
			ViewportParentView->HasViewParent() &&
			ViewportParentView->GetViewParent() == ParentView &&
			!ViewportParentView->IsViewParent())
			{
			ViewportClient->Viewport->InvalidateHitProxy();
			ViewportClient->Viewport->Draw();
			}
			}
			}
			}*/
		}

		//SetRequiredCursorOverride(false);

		bWidgetAxisControlledByDrag = false;

		// Update the hovered hit proxy here.  If the user didnt move the mouse
		// they still need to be able to pick up the gizmo without moving the mouse again
		HHitProxy* HitProxy = Viewport->GetHitProxy(CachedMouseX, CachedMouseY);
		CheckHoveredHitProxy(HitProxy);

		bIsTracking = false;
	}

	bHasMouseMovedSinceClick = false;
}

void UArmyEditorViewportClient::DrawAxes(FViewport* Viewport, FCanvas* Canvas, const FRotator* InRotation /*= NULL*/, EAxisList::Type InAxis /*= EAxisList::XYZ*/)
{
	FMatrix ViewTM = FMatrix::Identity;
	ViewTM = FRotationMatrix(GetViewRotation());

	if (InRotation)
	{
		ViewTM = FRotationMatrix(*InRotation);
	}
	//获取绘制户型视口的长和高度，把坐标轴绘制在固定屏幕空间上
	const int32 SizeX = Viewport->GetSizeXY().X;
	const int32 SizeY = Viewport->GetSizeXY().Y;

	const FIntPoint AxisOrigin(30, SizeY - 90);
	const float AxisSize = 25.0f;

	UFont* Font = GEngine->GetSmallFont();

	int32 XL, YL;
	StringSize(Font, XL, YL, TEXT("Z"));

	FVector AxisVec;
	FIntPoint AxisEnd;
	FCanvasLineItem LineItem;
	FCanvasTextItem TextItem(FVector2D::ZeroVector, FText::GetEmpty(), Font, FLinearColor::White);
	if ((InAxis & EAxisList::X) == EAxisList::X)
	{
		AxisVec = AxisSize * ViewTM.InverseTransformVector(FVector(1, 0, 0));
		AxisEnd = AxisOrigin + FIntPoint(AxisVec.Y, -AxisVec.Z);
		LineItem.SetColor(FLinearColor::Red);
		TextItem.SetColor(FLinearColor::Red);
		LineItem.Draw(Canvas, AxisOrigin, AxisEnd);
		TextItem.Text = LOCTEXT("XAxis", "X");
		TextItem.Draw(Canvas, FVector2D(AxisEnd.X + 2, AxisEnd.Y - 0.5*YL));
	}

	if ((InAxis & EAxisList::Y) == EAxisList::Y)
	{
		AxisVec = AxisSize * ViewTM.InverseTransformVector(FVector(0, 1, 0));
		AxisEnd = AxisOrigin + FIntPoint(AxisVec.Y, -AxisVec.Z);
		LineItem.SetColor(FLinearColor::Green);
		TextItem.SetColor(FLinearColor::Green);
		LineItem.Draw(Canvas, AxisOrigin, AxisEnd);
		TextItem.Text = LOCTEXT("YAxis", "Y");
		TextItem.Draw(Canvas, FVector2D(AxisEnd.X + 2, AxisEnd.Y - 0.5*YL));

	}

	if ((InAxis & EAxisList::Z) == EAxisList::Z)
	{
		AxisVec = AxisSize * ViewTM.InverseTransformVector(FVector(0, 0, 1));
		AxisEnd = AxisOrigin + FIntPoint(AxisVec.Y, -AxisVec.Z);
		LineItem.SetColor(FLinearColor::Blue);
		TextItem.SetColor(FLinearColor::Blue);
		LineItem.Draw(Canvas, AxisOrigin, AxisEnd);
		TextItem.Text = LOCTEXT("ZAxis", "Z");
		TextItem.Draw(Canvas, FVector2D(AxisEnd.X + 2, AxisEnd.Y - 0.5*YL));
	}
}

void UArmyEditorViewportClient::DrawScaleUnits(FViewport* InViewport, FCanvas* Canvas, const FSceneView& InView)
{
	const float UnitsPerPixel = GetOrthoUnitsPerPixel(InViewport);

	// Find the closest power of ten to our target width
	static const int32 ApproxTargetMarkerWidthPx = 100;
	const float SegmentWidthUnits = UnitsPerPixel > 0 ? FMath::Pow(10.f, FMath::RoundToFloat(FMath::LogX(10.f, UnitsPerPixel * ApproxTargetMarkerWidthPx))) : 0.f;

	const FString DisplayText = UnrealUnitsToSiUnits(SegmentWidthUnits);

	UFont* Font = GEngine->GetTinyFont();
	int32 TextWidth, TextHeight;
	StringSize(Font, TextWidth, TextHeight, *DisplayText);

	// Origin is the bottom left of the scale
	const FIntPoint StartPoint(80, InViewport->GetSizeXY().Y - 60);
	const FIntPoint EndPoint = StartPoint + (UnitsPerPixel != 0 ? FIntPoint(SegmentWidthUnits / UnitsPerPixel, 0) : FIntPoint(0, 0));

	// Sort out the color for the text and widget
	FLinearColor HSVBackground = InView.BackgroundColor.LinearRGBToHSV().CopyWithNewOpacity(1.f);
	const int32 Sign = (0.5f - HSVBackground.B) / FMath::Abs(HSVBackground.B - 0.5f);
	HSVBackground.B = HSVBackground.B + Sign*0.4f;
	const FLinearColor SegmentColor = HSVBackground.HSVToLinearRGB();

	const FIntPoint VerticalTickOffset(0, -3);

	// Draw the scale
	FCanvasLineItem LineItem;
	LineItem.SetColor(SegmentColor);
	LineItem.Draw(Canvas, StartPoint, StartPoint + VerticalTickOffset);
	LineItem.Draw(Canvas, StartPoint, EndPoint);
	LineItem.Draw(Canvas, EndPoint, EndPoint + VerticalTickOffset);

	// Draw the text
	FCanvasTextItem TextItem(EndPoint + FIntPoint(-(TextWidth + 3), -TextHeight), FText::FromString(DisplayText), Font, SegmentColor);
	TextItem.Draw(Canvas);
}
float UArmyEditorViewportClient::GetUniformScale(FVector Point)
{
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(false));
	FSceneView* View = CalcSceneView(&ViewFamily);
	float UniformScale = View->WorldToScreen(Point).W * (4.0f / View->ViewRect.Width() / View->ViewMatrices.GetProjectionMatrix().M[0][0]);

	return UniformScale;

}
bool UArmyEditorViewportClient::WorldToPixel(const FVector& WorldPoint, FVector2D& OutPixelLocation)
{
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(false));
	FSceneView* View = CalcSceneView(&ViewFamily);
	return View->WorldToPixel(WorldPoint, OutPixelLocation);
}
FVector4 UArmyEditorViewportClient::PixelToWorld(float X, float Y, float Z)
{
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(false));
	FSceneView* View = CalcSceneView(&ViewFamily);
	return View->PixelToWorld(X, Y, Z);
}
void UArmyEditorViewportClient::DeprojectFVector2D(const FVector2D& ScreenPos, FVector& out_WorldOrigin, FVector& out_WorldDirection)
{
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(false));
	FSceneView* View = CalcSceneView(&ViewFamily);
	return View->DeprojectFVector2D(ScreenPos, out_WorldOrigin, out_WorldDirection);
}

void UArmyEditorViewportClient::SetOrbitType()
{
	bIsOrbit = true;

	// 如果当前为FPS视角，则记录当前视角旋转，以便之后再次切换回FPS使用
	if (ViewportType == EArmyLevelViewportType::LVT_Perspective)
	{
		LastPerspectiveViewLoc = GetViewLocation();
		LastPerspectiveViewRot = GetViewRotation();
	}
	else
	{
		SetViewportType(EArmyLevelViewportType::LVT_Perspective);
	}

	SetViewLocationForOrbiting(FVector::ZeroVector, 1000.f);
}

void UArmyEditorViewportClient::SetGameViewportFullScreenMode(bool bInFullScreenMode)
{
	//if (bInFullScreenMode)
	//{
	//    FArmyMainFrameModule& MF = FModuleManager::LoadModuleChecked<FArmyMainFrameModule>(TEXT("ArmyMainFrame"));
	//    MF.MainSwitcher->SetActiveWidgetIndex(1);
	//    MF.FullScreenModeContainer->SetContent(GEngine->GetGameViewportWidget().ToSharedRef());

	//    FSlateApplication::Get().SetAllUserFocus(GEngine->GetGameViewportWidget(), EFocusCause::Mouse);
	//    FSlateApplication::Get().ResetToDefaultPointerInputSettings();
	//}
	//else
	//{
	//    FArmyMainFrameModule& MF = FModuleManager::LoadModuleChecked<FArmyMainFrameModule>(TEXT("ArmyMainFrame"));
	//    MF.MainSwitcher->SetActiveWidgetIndex(0);
	//    MF.FullScreenModeContainer->SetContent(SNullWidget::NullWidget);
	//    FSlateApplication::Get().ResetToDefaultInputSettings();
	//    FSlateApplication::Get().SetAllUserFocus(GEngine->GetGameViewportWidget(), EFocusCause::SetDirectly);
	//}
}

FVector UArmyEditorViewportClient::GetCurrentMouseTraceToWorldPos()
{
	static FName TraceTag = FName(TEXT("PickPos"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->bHidden || (ActorItr->Tags.Num() > 0 && (ActorItr->Tags[0] == "MoveableMeshActor" 
			|| ActorItr->Tags[0] == "BlueprintVRSActor" 
			|| ActorItr->Tags.Contains(TEXT("HydropowerPipeActor")) //@欧石楠 管线不能被击中
			)))
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}
	}

	TraceParams.bTraceAsyncScene = true;

	FVector2D MousePos(Viewport->GetMouseX(), Viewport->GetMouseY());
	FVector TraceStart, TraceEnd, TraceDirection;

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene(), EngineShowFlags));
	FSceneView* SceneView = CalcSceneView(&ViewFamily);
	SceneView->DeprojectFVector2D(MousePos, TraceStart, TraceDirection);
	TraceEnd = TraceStart + TraceDirection * 10000.f;

	FHitResult Hit(ForceInit);
	//-----------------------------------------------------------Trace---------------------------------------------
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
	/**@欧石楠获取的世界点稍微往后退一点*/
	return Hit.ImpactPoint;//- TraceDirection * 0.0001f;
}

FVector UArmyEditorViewportClient::GetMouseTraceToWorldPos(FVector& InStart, FVector& InEnd)
{

	FVector Start = 0.01*(InEnd - InStart).GetSafeNormal() + InStart;
	static FName TraceTag = FName(TEXT("PickPos"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->bHidden || (ActorItr->Tags.Num() > 0 && (ActorItr->Tags[0] == "MoveableMeshActor" || ActorItr->Tags[0] == "BlueprintVRSActor")))
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}
	}
	TraceParams.bTraceAsyncScene = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, Start, InEnd, ECC_Visibility, TraceParams);
	return Hit.ImpactPoint;
}

FVector UArmyEditorViewportClient::GetCurrentMouseTraceToSurfaceNormal()
{
	FVector2D MousePos(Viewport->GetMouseX(), Viewport->GetMouseY());
	return GetCurrentMouseTraceToSurfaceNormal(MousePos);
}

FVector UArmyEditorViewportClient::GetCurrentMouseTraceToSurfaceNormal(FVector2D& InMousePoint)
{
	static FName TraceTag = FName(TEXT("PickPos"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	for (auto Object : DroppedObjects)
	{
		AActor* DropActor = Cast<AActor>(Object);
		TraceParams.AddIgnoredActor(DropActor);
	}
	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* DropActor = Cast<AActor>(*ActorItr);
		TraceParams.AddIgnoredActor(DropActor);
	}
	TraceParams.bTraceAsyncScene = true;


	FVector TraceStart, TraceEnd, TraceDirection;

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene(), EngineShowFlags));
	FSceneView* SceneView = CalcSceneView(&ViewFamily);
	SceneView->DeprojectFVector2D(InMousePoint, TraceStart, TraceDirection);
	TraceEnd = TraceStart + TraceDirection * 10000.f;

	FHitResult Hit(ForceInit);
	//-----------------------------------------------------------Trace---------------------------------------------
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		FVector HitPos = Hit.ImpactPoint;
		if (FMath::IsNearlyEqual(HitPos.Z, 0, KINDA_SMALL_NUMBER))
			return FVector(0, 0, 1);
		else if (FMath::IsNearlyEqual(HitPos.Z, GetWallHeight(), KINDA_SMALL_NUMBER))
			return FVector(0, 0, -1);
		return Hit.ImpactNormal;

	}

	return FVector::ZeroVector;
}

bool UArmyEditorViewportClient::GetImportPoint(FVector& InStart, FVector& InEnd, FVector& OutLocation)
{
	static FName TraceTag = FName(TEXT("PickPos"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	for (auto Object : DroppedObjects)
	{
		AActor* DropActor = Cast<AActor>(Object);
		TraceParams.AddIgnoredActor(DropActor);
	}
	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* DropActor = Cast<AActor>(*ActorItr);
		TraceParams.AddIgnoredActor(DropActor);
	}
	TraceParams.bTraceAsyncScene = true;


	FVector TraceStart = InStart;
	FVector TraceEnd = InEnd;

	FHitResult Hit(ForceInit);
	//-----------------------------------------------------------Trace---------------------------------------------
	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
	{
		OutLocation = Hit.ImpactPoint;
		return true;
	}
	return false;
}

void UArmyEditorViewportClient::StartTrackingDueToInput(const struct FArmyInputEventState& InputState)
{
	// Check to see if the current event is a modifier key and that key was already in the
	// same state.
	EInputEvent Event = InputState.GetInputEvent();
	FViewport* InputStateViewport = InputState.GetViewport();
	FKey Key = InputState.GetKey();

	bool bIsRedundantModifierEvent =
		(InputState.IsAltButtonEvent() && ((Event != IE_Released) == IsAltPressed())) ||
		(InputState.IsCtrlButtonEvent() && ((Event != IE_Released) == IsCtrlPressed())) ||
		(InputState.IsShiftButtonEvent() && ((Event != IE_Released) == IsShiftPressed()));

	if (MouseDeltaTracker->UsingDragTool() && InputState.IsLeftMouseButtonPressed() && Event != IE_Released)
	{
		bIsRedundantModifierEvent = true;
	}

	const int32	HitX = InputStateViewport->GetMouseX();
	const int32	HitY = InputStateViewport->GetMouseY();


	//First mouse down, note where they clicked
	LastMouseX = HitX;
	LastMouseY = HitY;

	WorldStartPos = PixelToWorld(HitX, HitY, 0);
	WorldEndPos = PixelToWorld(HitX, HitY, 0);

	// Only start (or restart) tracking mode if the current event wasn't a modifier key that
	// was already pressed or released.
	if (!bIsRedundantModifierEvent)
	{
		const bool bWasTracking = bIsTracking;

		// Stop current tracking
		if (bIsTracking)
		{
			MouseDeltaTracker->EndTracking(this);
			bIsTracking = false;
		}

		bDraggingByHandle = (Widget && Widget->GetCurrentAxis() != EAxisList::None);

		if (Event == IE_Pressed)
		{
			// Tracking initialization:
			GArmyEditor->MouseMovement = FVector::ZeroVector;
		}

		// Start new tracking. Potentially reset the widget so that StartTracking can pick a new axis.
		if (Widget && (!bDraggingByHandle || InputState.IsCtrlButtonPressed()))
		{
			bWidgetAxisControlledByDrag = false;
			Widget->SetCurrentAxis(EAxisList::None);
		}
		const bool bNudge = false;
		MouseDeltaTracker->StartTracking(this, HitX, HitY, InputState, bNudge, !bWasTracking);
		bIsTracking = true;

		//if we are using a widget to drag by axis ensure the cursor is correct
		if (bDraggingByHandle == true)
		{
			//reset the flag to say we used a drag modifier	if we are using the widget handle
			if (bWidgetAxisControlledByDrag == false)
			{
				//MouseDeltaTracker->ResetUsedDragModifier();
			}

			//SetRequiredCursorOverride(true, EMouseCursor::CardinalCross);
		}

		//only reset the initial point when the mouse is actually clicked
		if (InputState.IsAnyMouseButtonDown() && Widget)
		{
			Widget->ResetInitialTranslationOffset();
		}

		//Don't update the cursor visibility if we don't have focus or mouse capture 
		if (InputStateViewport->HasFocus() || InputStateViewport->HasMouseCapture())
		{
			//Need to call this one more time as the axis variable for the widget has just been updated
			//UpdateRequiredCursorVisibility();
		}
	}
	//ApplyRequiredCursorVisibility(true);
}

void UArmyEditorViewportClient::ProcessClickInViewport(const FArmyInputEventState& InputState, FSceneView& View)
{
	// Ignore actor manipulation if we're using a tool
	if (!MouseDeltaTracker->UsingDragTool())
	{
		EInputEvent Event = InputState.GetInputEvent();
		FViewport* InputStateViewport = InputState.GetViewport();
		FKey Key = InputState.GetKey();

		int32	HitX = InputStateViewport->GetMouseX();
		int32	HitY = InputStateViewport->GetMouseY();

		const float XScale = (float)View.ViewRect.Max.X / (float)View.UnscaledViewRect.Max.X;
		const float YScale = (float)View.ViewRect.Max.Y / (float)View.UnscaledViewRect.Max.Y;
		HitX = HitX * XScale;
		HitY = HitY * YScale;
		// Calc the raw delta from the mouse to detect if there was any movement
		FVector RawMouseDelta = MouseDeltaTracker->GetRawDelta();

		// Note: We are using raw mouse movement to double check distance moved in low performance situations.  In low performance situations its possible 
		// that we would get a mouse down and a mouse up before the next tick where GEditor->MouseMovment has not been updated.  
		// In that situation, legitimate drags are incorrectly considered clicks
		bool bNoMouseMovment = RawMouseDelta.SizeSquared() < MOUSE_CLICK_DRAG_DELTA && GArmyEditor->MouseMovement.SizeSquared() < MOUSE_CLICK_DRAG_DELTA;

		// If the mouse haven't moved too far, treat the button release as a click.
		if (bNoMouseMovment)// && !MouseDeltaTracker->WasExternalMovement())
		{

			Viewport->InvalidateHitProxy();
			HHitProxy* HitProxy = InputStateViewport->GetHitProxy(HitX, HitY);
			// When clicking, the cursor should always appear at the location of the click and not move out from undere the user
			InputStateViewport->SetPreCaptureMousePosFromSlateCursor();
			ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
		}
	}
}

bool UArmyEditorViewportClient::IsAltPressed() const
{
	return Viewport->KeyState(EKeys::LeftAlt) || Viewport->KeyState(EKeys::RightAlt);
}

bool UArmyEditorViewportClient::IsCtrlPressed() const
{
	return Viewport->KeyState(EKeys::LeftControl) || Viewport->KeyState(EKeys::RightControl);
}

bool UArmyEditorViewportClient::IsShiftPressed() const
{
	return Viewport->KeyState(EKeys::LeftShift) || Viewport->KeyState(EKeys::RightShift);
}

bool UArmyEditorViewportClient::IsCmdPressed() const
{
	return Viewport->KeyState(EKeys::LeftCommand) || Viewport->KeyState(EKeys::RightCommand);
}

void UArmyEditorViewportClient::ProcessDoubleClickInViewport(const struct FArmyInputEventState& InputState, FSceneView& View)
{
	// Stop current tracking
	if (bIsTracking)
	{
		MouseDeltaTracker->EndTracking(this);
		bIsTracking = false;
	}

	FViewport* InputStateViewport = InputState.GetViewport();
	EInputEvent Event = InputState.GetInputEvent();
	FKey Key = InputState.GetKey();

	const int32	HitX = InputStateViewport->GetMouseX();
	const int32	HitY = InputStateViewport->GetMouseY();

	MouseDeltaTracker->StartTracking(this, HitX, HitY, InputState);
	bIsTracking = true;
	//GEditor->MouseMovement = FVector::ZeroVector;
	HHitProxy*	HitProxy = InputStateViewport->GetHitProxy(HitX, HitY);
	ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
	MouseDeltaTracker->EndTracking(this);
	bIsTracking = false;

	// This needs to be set to false to allow the axes to update
	bWidgetAxisControlledByDrag = false;
	MouseDeltaTracker->ResetUsedDragModifier();
	//RequiredCursorVisibiltyAndAppearance.bHardwareCursorVisible = true;
	//RequiredCursorVisibiltyAndAppearance.bSoftwareCursorVisible = false;
	//ApplyRequiredCursorVisibility();
}

void UArmyEditorViewportClient::OnOrthoZoom(const struct FArmyInputEventState& InputState, float Scale)
{
	FViewport* InputStateViewport = InputState.GetViewport();
	FKey Key = InputState.GetKey();

	// Scrolling the mousewheel up/down zooms the orthogonal viewport in/out.
	int32 Delta = 25 * Scale;
	if (Key == EKeys::MouseScrollUp || Key == EKeys::Add)
	{
		Delta *= -1;
	}

	//Extract current state
	int32 ViewportWidth = InputStateViewport->GetSizeXY().X;
	int32 ViewportHeight = InputStateViewport->GetSizeXY().Y;

	FVector OldOffsetFromCenter;

	//const bool bCenterZoomAroundCursor = GetDefault<ULevelEditorViewportSettings>()->bCenterZoomAroundCursor && (Key == EKeys::MouseScrollDown || Key == EKeys::MouseScrollUp);

	//if (bCenterZoomAroundCursor)
	{
		//Y is actually backwards, but since we're move the camera opposite the cursor to center, we negate both
		//therefore the x is negated
		//X Is backwards, negate it
		//default to viewport mouse position
		int32 CenterX = InputStateViewport->GetMouseX();
		int32 CenterY = InputStateViewport->GetMouseY();
		//if (ShouldUseMoveCanvasMovement())
		{
			//use virtual mouse while dragging (normal mouse is clamped when invisible)
			//CenterX = LastMouseX;
			//CenterY = LastMouseY;
		}
		int32 DeltaFromCenterX = -(CenterX - (ViewportWidth >> 1));
		int32 DeltaFromCenterY = (CenterY - (ViewportHeight >> 1));
		switch (GetViewportType())
		{
		case EArmyLevelViewportType::LVT_OnlyCanvas:
		case EArmyLevelViewportType::LVT_OrthoXY:
			OldOffsetFromCenter.Set(DeltaFromCenterX, -DeltaFromCenterY, 0.0f);
			break;
		case EArmyLevelViewportType::LVT_OrthoXZ:
			OldOffsetFromCenter.Set(DeltaFromCenterX, 0.0f, DeltaFromCenterY);
			break;
		case EArmyLevelViewportType::LVT_OrthoYZ:
			OldOffsetFromCenter.Set(0.0f, DeltaFromCenterX, DeltaFromCenterY);
			break;
		case EArmyLevelViewportType::LVT_OrthoNegativeXY:
			OldOffsetFromCenter.Set(-DeltaFromCenterX, -DeltaFromCenterY, 0.0f);
			break;
		case EArmyLevelViewportType::LVT_OrthoNegativeXZ:
			OldOffsetFromCenter.Set(-DeltaFromCenterX, 0.0f, DeltaFromCenterY);
			break;
		case EArmyLevelViewportType::LVT_OrthoNegativeYZ:
			OldOffsetFromCenter.Set(0.0f, -DeltaFromCenterX, DeltaFromCenterY);
			break;
		case EArmyLevelViewportType::LVT_OrthoFreelook:
		{

			//FVector rightDirection = GetViewRotation().RotateVector(FVector(1, 0, 0)).GetSafeNormal();
			//OldOffsetFromCenter = DeltaFromCenterX * rightDirection * 100 + DeltaFromCenterY * rightDirection * 100;
			OldOffsetFromCenter = FVector::ZeroVector;
			break;
		}
		case EArmyLevelViewportType::LVT_Perspective:
			break;
		}
	}

	//save off old zoom
	const float OldUnitsPerPixel = GetOrthoUnitsPerPixel(Viewport);

	//update zoom based on input
	SetOrthoZoom(GetOrthoZoom() + (GetOrthoZoom() / CAMERA_ZOOM_DAMPEN) * Delta);
	SetOrthoZoom(FMath::Clamp<float>(GetOrthoZoom(), MIN_ORTHOZOOM, MAX_ORTHOZOOM));

	//if (bCenterZoomAroundCursor)
	{
		//This is the equivalent to moving the viewport to center about the cursor, zooming, and moving it back a proportional amount towards the cursor
		FVector FinalDelta = (GetOrthoUnitsPerPixel(Viewport) - OldUnitsPerPixel)*OldOffsetFromCenter;

		//now move the view location proportionally
		SetViewLocation(GetViewLocation() + FinalDelta);
	}

	const bool bInvalidateViews = true;

	// Update linked ortho viewport movement based on updated zoom and view location, 
	UpdateLinkedOrthoViewports(bInvalidateViews);

	const bool bInvalidateHitProxies = true;

	Invalidate(bInvalidateViews, bInvalidateHitProxies);

	//mark "externally moved" so context menu doesn't come up
	MouseDeltaTracker->SetExternalMovement();
}

void UArmyEditorViewportClient::OnDollyPerspectiveCamera(const struct FArmyInputEventState& InputState)
{
	FKey Key = InputState.GetKey();

	// Scrolling the mousewheel up/down moves the perspective viewport forwards/backwards.
	FVector Drag(0, 0, 0);

	const FRotator& ViewRotation = GetViewRotation();
	Drag.X = FMath::Cos(ViewRotation.Yaw * PI / 180.f) * FMath::Cos(ViewRotation.Pitch * PI / 180.f);
	Drag.Y = FMath::Sin(ViewRotation.Yaw * PI / 180.f) * FMath::Cos(ViewRotation.Pitch * PI / 180.f);
	Drag.Z = FMath::Sin(ViewRotation.Pitch * PI / 180.f);

	if (Key == EKeys::MouseScrollDown)
	{
		Drag = -Drag;
	}

	float CameraSpeed = 4.f;
	Drag *= CameraSpeed * 4.f;

	const bool bDollyCamera = true;
	MoveViewportCamera(Drag, FRotator::ZeroRotator, bDollyCamera);
}

void UArmyEditorViewportClient::ProcessClick(class FSceneView& View, class HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	const FArmyViewportClick Click(&View, this, Key, Event, HitX, HitY);

	bool bRightMouseButtonPressed = Key == EKeys::RightMouseButton;

	if (HitProxy == NULL)
	{
		ArmyClickHandlers::ClickBackdrop(this, Click);
		if (bRightMouseButtonPressed && DroppedObjects.Num() > 0)
		{
			CancelDrop();
		}
	}
	else if (HitProxy->Priority >= 0 && HitProxy->OrthoPriority >= 0)
	{
		if (HitProxy->IsA(HArmyWidgetAxis::StaticGetType()))
		{
			if (bRightMouseButtonPressed)
			{
			}
			// The user clicked on an axis translation/rotation hit proxy.  However, we want
			// to find out what's underneath the axis widget.  To do this, we'll need to render
			// the viewport's hit proxies again, this time *without* the axis widgets!

			// OK, we need to be a bit evil right here.  Basically we want to hijack the ShowFlags
			// for the scene so we can re-render the hit proxies without any axis widgets.  We'll
			// store the original ShowFlags and modify them appropriately
			const bool bOldModeWidgets1 = EngineShowFlags.ModeWidgets;
			const bool bOldModeWidgets2 = View.Family->EngineShowFlags.ModeWidgets;

			EngineShowFlags.SetModeWidgets(false);
			FSceneViewFamily* SceneViewFamily = const_cast<FSceneViewFamily*>(View.Family);
			SceneViewFamily->EngineShowFlags.SetModeWidgets(false);
			bool bWasWidgetDragging = Widget->IsDragging();
			Widget->SetDragging(false);

			// Invalidate the hit proxy map so it will be rendered out again when GetHitProxy
			// is called
			Viewport->InvalidateHitProxy();

			// This will actually re-render the viewport's hit proxies!
			HHitProxy* HitProxyWithoutAxisWidgets = Viewport->GetHitProxy(HitX, HitY);
			if (HitProxyWithoutAxisWidgets != NULL && !HitProxyWithoutAxisWidgets->IsA(HArmyWidgetAxis::StaticGetType()))
			{
				// Try this again, but without the widget this time!
				ProcessClick(View, HitProxyWithoutAxisWidgets, Key, Event, HitX, HitY);
			}

			// Undo the evil
			EngineShowFlags.SetModeWidgets(bOldModeWidgets1);
			SceneViewFamily->EngineShowFlags.SetModeWidgets(bOldModeWidgets2);

			Widget->SetDragging(bWasWidgetDragging);

			// Invalidate the hit proxy map again so that it'll be refreshed with the original
			// scene contents if we need it again later.
			Viewport->InvalidateHitProxy();
		}
		else if (HitProxy->IsA(HActor::StaticGetType()))
		{
			if (bRightMouseButtonPressed && bDroppingPreviewTexture)
			{
				CancelDrop();
				return;
			}

			auto ActorHitProxy = (HActor*)HitProxy;
			const bool bSelectComponent = false;

			if (bSelectComponent)
			{
				//ClickHandlers::ClickComponent(this, ActorHitProxy, Click);
			}
			else
			{
				FString ActorTag;
				bool bIgnore = false;
				FString ActorName = ActorHitProxy->Actor->GetName();
				if (ActorName.StartsWith(TEXT("SM_Surrounding")) || ActorName.StartsWith(TEXT("Cylinder001")))
				{
					bIgnore = true;
				}

				if (!bIgnore)
				{
					ArmyClickHandlers::ClickActor(this, ActorHitProxy->Actor, Click, true);
					//更新跟随坐标轴的包围盒
					if (Click.GetKey() == EKeys::LeftMouseButton)
					{
						FBox SelectActorsBox(ForceInit);
						SelectActorsBox = GArmyEditor->GetSelectedActorsBox();
						FlowAxisBoxOfSelectedActors = SelectActorsBox;
					}
				}

			}
		}
		else if (HitProxy && HitProxy->IsA(HArmyObjectBaseProxy::StaticGetType()))
		{
		}
		else if (HitProxy&&HitProxy->IsA(HArmyBaseAreaProxy::StaticGetType()))
		{

		}
		else
		{
			// 没有选中要选的东西时，取消选中状态
			if (bRightMouseButtonPressed)
			{
				CancelDrop();
				return;
			}
			//@  （朱同宽）刘克祥， 进入多选模式不允许取消之前选中的
			if (!AutoMultiSelectEnabled())
			{
				GArmyEditor->SelectNone(true, true);
			}
		}
	}

}

bool UArmyEditorViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type InCurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale)
{
	bool bHandled = false;

	// 	if (ModeTools->InputDelta(this, Viewport, Drag, Rot, Scale))
	// 	{
	// 		if (ModeTools->AllowWidgetMove())
	// 		{
	// 			ModeTools->PivotLocation += Drag;
	// 			ModeTools->SnappedLocation += Drag;
	// 		}
	// 
	// 		// Update visuals of the rotate widget 
	// 		ApplyDeltaToRotateWidget(Rot);
	// 		return true;
	// 	}
	// 	else
	// 	{
	// 		return false;
	// 	}
	//@TODO: MODETOOLS: Much of this needs to get pushed to Super, but not all of it can be...
	if (InCurrentAxis != EAxisList::None)
	{
		// Skip actors transformation routine in case if any of the selected actors locked
		// but still pretend that we have handled the input
		if (true)//!GVRSEditor->HasLockedActors())
		{
			ModeTools->PivotLocation += Drag;
			ModeTools->SnappedLocation += Drag;

			const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
			const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
			const bool MiddleMouseButtonDown = Viewport->KeyState(EKeys::MiddleMouseButton);

			// If duplicate dragging . . .
			if (IsShiftPressed() && (LeftMouseButtonDown || RightMouseButtonDown))
			{
				// The widget has been offset, so check if we should duplicate the selection.
				if (bDuplicateOnNextDrag)
				{
					// Only duplicate if we're translating or rotating.
					if (!Drag.IsNearlyZero() || !Rot.IsZero())
					{
						// Widget hasn't been dragged since ALT+LMB went down.
						bDuplicateOnNextDrag = false;
						//ABrush::SetSuppressBSPRegeneration(true);
						GArmyEditor->edactDuplicateSelected(GetWorld()->GetCurrentLevel(), false);
						//ABrush::SetSuppressBSPRegeneration(false);
					}
				}
			}

			// We do not want actors updated if we are holding down the middle mouse button.
			if (!MiddleMouseButtonDown)
			{
				//bool bSnapped = FSnappingUtils::SnapActorsToNearestActor(Drag, this);
				//bSnapped = bSnapped || FSnappingUtils::SnapDraggedActorsToNearestVertex(Drag, this);

				// If we are only changing position, project the actors onto the world
				const bool bOnlyTranslation = !Drag.IsZero() && Rot.IsZero() && Scale.IsZero();

				const EAxisList::Type CurrentAxis = GetCurrentWidgetAxis();
				const bool bSingleAxisDrag = CurrentAxis == EAxisList::X || CurrentAxis == EAxisList::Y || CurrentAxis == EAxisList::Z;
				if (/*!bSnapped &&*/ !bSingleAxisDrag && /*GetDefault<ULevelEditorViewportSettings>()->SnapToSurface.bEnabled &&*/ bOnlyTranslation)
				{
					TArray<AActor*> SelectedActors;
					for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
					{
						if (AActor* Actor = Cast<AActor>(*It))
						{
							SelectedActors.Add(Actor);
						}
					}
					ProjectActorsIntoWorld(SelectedActors);
					Drag = FVector::ZeroVector;
				}
				else
				{
					ApplyDeltaToActors(Drag, Rot, Scale);
				}

				ApplyDeltaToRotateWidget(Rot);
			}
			else
			{
				//FSnappingUtils::SnapDragLocationToNearestVertex(ModeTools->PivotLocation, Drag, this);
				//GUnrealEd->SetPivotMovedIndependently(true);
				//bOnlyMovedPivot = true;
			}

			if (IsShiftPressed())
			{
				//FVector CameraDelta(Drag);
				//MoveViewportCamera(CameraDelta, FRotator::ZeroRotator);
			}

			//TArray<FEdMode*> ActiveModes;
			//ModeTools->GetActiveModes(ActiveModes);

			//for (int32 ModeIndex = 0; ModeIndex < ActiveModes.Num(); ++ModeIndex)
			//{
			//	ActiveModes[ModeIndex]->UpdateInternalData();
			//}
		}

		bHandled = true;
	}

	return bHandled;
}

void UArmyEditorViewportClient::SetWidgetMode(FArmyWidget::EWidgetMode NewMode)
{
	ModeTools->SetWidgetMode(NewMode);
	Viewport->InvalidateHitProxy();
	bShouldCheckHitProxy = true;
}

bool UArmyEditorViewportClient::CanSetWidgetMode(FArmyWidget::EWidgetMode NewMode) const
{
	return ModeTools->UsesTransformWidget(NewMode) == true;
}

FArmyWidget::EWidgetMode UArmyEditorViewportClient::GetWidgetMode() const
{
	return ModeTools->GetWidgetMode();
}

FVector UArmyEditorViewportClient::GetWidgetLocation() const
{
	return ModeTools->GetWidgetLocation();
}

FMatrix UArmyEditorViewportClient::GetWidgetCoordSystem() const
{
	return ModeTools->GetCustomInputCoordinateSystem();
}

void UArmyEditorViewportClient::SetWidgetCoordSystemSpace(EArmyCoordSystem NewCoordSystem)
{
	//ModeTools->SetCoordSystem(NewCoordSystem);
	//RedrawAllViewportsIntoThisScene();
}

EArmyCoordSystem UArmyEditorViewportClient::GetWidgetCoordSystemSpace() const
{
	return ModeTools->GetCoordSystem();
}

void UArmyEditorViewportClient::ApplyDeltaToRotateWidget(const FRotator& InRot)
{

}

void UArmyEditorViewportClient::RedrawAllViewportsIntoThisScene()
{
	Invalidate();
}

FSceneInterface* UArmyEditorViewportClient::GetScene() const
{
	UWorld* TempWorld = GetWorld();
	if (TempWorld)
	{
		return TempWorld->Scene;
	}

	return NULL;
}

UWorld* UArmyEditorViewportClient::GetWorld() const
{
	return UGameViewportClient::GetWorld();
	// 	UWorld* OutWorldPtr = NULL;
	// 	// If we have a valid scene get its world
	// 	if (PreviewScene)
	// 	{
	// 		OutWorldPtr = PreviewScene->GetWorld();
	// 	}
	// 	if (OutWorldPtr == NULL)
	// 	{
	// 		OutWorldPtr = GWorld;
	// 	}
	// 	return OutWorldPtr;
}

void UArmyEditorViewportClient::DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas)
{
	RenderDragTool(&View, &Canvas);
	//ModeTools->DrawHUD(this, &InViewport, &View, &Canvas);

	/*FVector ViewOrigin = View.ViewMatrices.GetViewOrigin();
	FString Str = FString::Printf(TEXT("ViewOrigin：%.f, %.f, %.f"), ViewOrigin.X, ViewOrigin.Y, ViewOrigin.Z);
	Canvas.DrawShadowedString(0, 875, *Str, GEngine->GetSmallFont(), FLinearColor::Red, 1.f);*/
}

void UArmyEditorViewportClient::ModifyView(FViewport& InViewport, FSceneViewFamily* ViewFamily, FSceneView& View)
{
	if (bUserPlayerControllerView)
	{
		return;
	}
	FSceneViewFamily NewViewFamily = *ViewFamily;
	NewViewFamily.Views.Empty();
	FSceneView* NewView = CalcSceneView(&NewViewFamily);
	View.ViewMatrices = NewView->ViewMatrices;
	View.ViewLocation = NewView->ViewLocation;
	View.ViewRotation = NewView->ViewRotation;
	View.ViewFrustum = NewView->ViewFrustum;
	View.ShadowViewMatrices = NewView->ShadowViewMatrices;
}

static UCanvas* GetCanvasByName(FName CanvasName)
{
	// Cache to avoid FString/FName conversions/compares
	static TMap<FName, UCanvas*> CanvasMap;
	UCanvas** FoundCanvas = CanvasMap.Find(CanvasName);
	if (!FoundCanvas)
	{
		UCanvas* CanvasObject = FindObject<UCanvas>(GetTransientPackage(), *CanvasName.ToString());
		if (!CanvasObject)
		{
			CanvasObject = NewObject<UCanvas>(GetTransientPackage(), CanvasName);
			CanvasObject->AddToRoot();
		}

		CanvasMap.Add(CanvasName, CanvasObject);
		return CanvasObject;
	}

	return *FoundCanvas;
}

void UArmyEditorViewportClient::Draw(FViewport* InViewport, FCanvas* SceneCanvas)
{
	if (bUserPlayerControllerView)
	{
		Super::Draw(InViewport, SceneCanvas);
		return;
	}

	FViewport* ViewportBackup = Viewport;
	Viewport = InViewport ? InViewport : Viewport;
	FSceneView* NewView = NULL;

	//创建命令框画布
	FIntPoint DebugCanvasSize = InViewport->GetSizeXY();
	UCanvas* DebugCanvasObject = GetCanvasByName("DebugCanvasObject");
	DebugCanvasObject->Init(DebugCanvasSize.X, DebugCanvasSize.Y, NULL, InViewport->GetDebugCanvas());

	//创建ViewFamily
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		//Viewport,
		SceneCanvas->GetRenderTarget(),
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(true));

	//计算View
	NewView = CalcSceneView(&ViewFamily);
	//透视图
	if (IsPerspective())
	{
		ESplitScreenType::Type SplitScreenConfig = GetCurrentSplitscreenConfiguration();
		ViewFamily.ViewMode = VMI_Lit;
		EngineShowFlags.PostProcessing = true;
		EngineShowFlagOverride(ESFIM_Game, ViewFamily.ViewMode, ViewFamily.EngineShowFlags, NAME_None, SplitScreenConfig != ESplitScreenType::None);
		EngineShowFlagOrthographicOverride(IsPerspective(), ViewFamily.EngineShowFlags);
	}
	//其他正交视图
	else
	{
		ViewFamily.EngineShowFlags.DisableAdvancedFeatures();
		ESplitScreenType::Type SplitScreenConfig = GetCurrentSplitscreenConfiguration();
		ViewFamily.ViewMode = VMI_Unlit;
		EngineShowFlagOverride(ESFIM_Game, ViewFamily.ViewMode, ViewFamily.EngineShowFlags, NAME_None, SplitScreenConfig != ESplitScreenType::None);
		EngineShowFlagOrthographicOverride(IsPerspective(), ViewFamily.EngineShowFlags);
		ViewFamily.EngineShowFlags.SetAntiAliasing(true);
		ViewFamily.EngineShowFlags.SetWireframe(false);
		ViewFamily.EngineShowFlags.SetBloom(false);
		ViewFamily.EngineShowFlags.SetVisualizeHDR(false);
		ViewFamily.EngineShowFlags.PostProcessing = false;

	}

	//渲染调试命令
	PostRender(DebugCanvasObject);

	//渲染命令框
	if (ViewportConsole)
		ViewportConsole->PostRender_Console(DebugCanvasObject);

	//渲染调试命令
	DrawStatsHUD(GetWorld(), InViewport, InViewport->GetDebugCanvas(), DebugCanvasObject, DebugProperties, FVector::ZeroVector, FRotator::ZeroRotator);

	//执行渲染命令
	GetRendererModule().BeginRenderingViewFamily(SceneCanvas, &ViewFamily);

	if (NewView)
		RenderDragTool(NewView, SceneCanvas);

	//渲染坐标轴文字
	if (Widget)
		Widget->DrawHUD(SceneCanvas);

	if (!IsPerspective() && bDrawAxes)
	{
		const FRotator XYRot(-90.0f, -90.0f, 0.0f);
		DrawAxes(Viewport, SceneCanvas, &XYRot, EAxisList::XY);
		DrawScaleUnits(Viewport, SceneCanvas, *NewView);
	}

	//Viewport = ViewportBackup;
}

void UArmyEditorViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (!bForceHiddenWidget)
		Widget->Render(View, PDI, this);

	if (bUsesDrawHelper)
		DrawHelper.Draw(View, PDI);

	// Remove temporary debug lines.
	// Possibly a hack. Lines may get added without the scene being rendered etc.
	if (World->LineBatcher != NULL && (World->LineBatcher->BatchedLines.Num() || World->LineBatcher->BatchedPoints.Num() || World->LineBatcher->BatchedMeshes.Num()))
	{
		World->LineBatcher->Flush();
	}

	if (World->ForegroundLineBatcher != NULL && (World->ForegroundLineBatcher->BatchedLines.Num() || World->ForegroundLineBatcher->BatchedPoints.Num() || World->ForegroundLineBatcher->BatchedMeshes.Num()))
	{
		World->ForegroundLineBatcher->Flush();
	}
}

void UArmyEditorViewportClient::RenderDragTool(const FSceneView* View, FCanvas* Canvas)
{
	MouseDeltaTracker->RenderDragTool(View, Canvas);
}

FLinearColor UArmyEditorViewportClient::GetBackgroundColor() const
{
	return FLinearColor(FColor(0XFF191A1D));
}

void UArmyEditorViewportClient::SetCameraSetup(
	const FVector& LocationForOrbiting,
	const FRotator& InOrbitRotation,
	const FVector& InOrbitZoom,
	const FVector& InOrbitLookAt,
	const FVector& InViewLocation,
	const FRotator &InViewRotation)
{
	if (bUsingOrbitCamera)
	{
		SetViewRotation(InOrbitRotation);
		SetViewLocation(InViewLocation + InOrbitZoom);
		SetLookAtLocation(InOrbitLookAt);
	}
	else
	{
		SetViewLocation(InViewLocation);
		SetViewRotation(InViewRotation);
	}


	// Save settings for toggling between orbit and unlocked camera
	DefaultOrbitLocation = InViewLocation;
	DefaultOrbitRotation = InOrbitRotation;
	DefaultOrbitZoom = InOrbitZoom;
	DefaultOrbitLookAt = InOrbitLookAt;
}

FVector UArmyEditorViewportClient::TranslateDelta(FKey InKey, float InDelta, bool InNudge)
{
	const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
	const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
	const bool bIsUsingTrackpad = FSlateApplication::Get().IsUsingTrackpad();

	FVector vec(0.0f, 0.0f, 0.0f);

	float X = InKey == EKeys::MouseX ? InDelta : 0.f;
	float Y = InKey == EKeys::MouseY ? InDelta : 0.f;

	switch (GetViewportType())
	{
	case EArmyLevelViewportType::LVT_OnlyCanvas:
	case EArmyLevelViewportType::LVT_OrthoXY:
	{
		LastMouseX -= X;
		LastMouseY += Y;

		//if ((X != 0.0f) || (Y != 0.0f))
		//{
		//	MarkMouseMovedSinceClick();
		//}

		////only invert x,y if we're moving the camera
		//if (ShouldUseMoveCanvasMovement())
		//{
		//	if (Widget->GetCurrentAxis() == EAxisList::None)
		//	{
		//		X = -X;
		//		Y = -Y;
		//	}
		//}

		//update the position
		Viewport->SetSoftwareCursorPosition(FVector2D(LastMouseX, LastMouseY));
		//UE_LOG(LogEditorViewport, Log, *FString::Printf( TEXT("can:%d %d") , LastMouseX , LastMouseY ));
		//change to grab hand
		//SetRequiredCursorOverride(true, EMouseCursor::CardinalCross);
		//update and apply cursor visibility
		//UpdateAndApplyCursorVisibility();

		FArmyWidget::EWidgetMode WidgetMode = GetWidgetMode();
		bool bIgnoreOrthoScaling = (WidgetMode == FArmyWidget::WM_Scale) && (Widget->GetCurrentAxis() != EAxisList::None);

		if (InNudge || bIgnoreOrthoScaling)
		{
			vec = FVector(X, Y, 0.f);
		}
		else
		{
			const float UnitsPerPixel = GetOrthoUnitsPerPixel(Viewport);
			vec = FVector(X * UnitsPerPixel, Y * UnitsPerPixel, 0.f);
			vec *= 1.5f;
			if (Widget->GetCurrentAxis() == EAxisList::None)
			{
				switch (GetViewportType())
				{
				case EArmyLevelViewportType::LVT_OrthoXY:
					vec.Y *= -1.0f;
					break;
				case EArmyLevelViewportType::LVT_OrthoXZ:
					vec = FVector(X * UnitsPerPixel, 0.f, Y * UnitsPerPixel);
					break;
				case EArmyLevelViewportType::LVT_OrthoYZ:
					vec = FVector(0.f, X * UnitsPerPixel, Y * UnitsPerPixel);
					break;
				case EArmyLevelViewportType::LVT_OrthoNegativeXY:
					vec = FVector(-X * UnitsPerPixel, -Y * UnitsPerPixel, 0.0f);
					break;
				case EArmyLevelViewportType::LVT_OrthoNegativeXZ:
					vec = FVector(-X * UnitsPerPixel, 0.f, Y * UnitsPerPixel);
					break;
				case EArmyLevelViewportType::LVT_OrthoNegativeYZ:
					vec = FVector(0.f, -X * UnitsPerPixel, Y * UnitsPerPixel);
					break;
				case EArmyLevelViewportType::LVT_OrthoFreelook:
				case EArmyLevelViewportType::LVT_Perspective:
					break;
				}
			}
		}
	}
	break;
	case EArmyLevelViewportType::LVT_Perspective:
		// Update the software cursor position
		Viewport->SetSoftwareCursorPosition(FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY()));
		vec = FVector(X, Y, 0.f);
		break;
	default:
		check(0);		// Unknown viewport type
		break;
	}

	return vec;
}

bool UArmyEditorViewportClient::InputAxis(FViewport* InViewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	Super::InputAxis(InViewport, ControllerId, Key, Delta, DeltaTime, NumSamples, bGamepad);

	//if (bDisableInput)
	//{
	//	return true;
	//}

	//// Let the current mode have a look at the input before reacting to it.
	//if (ModeTools->InputAxis(this, Viewport, ControllerId, Key, Delta, DeltaTime))
	//{
	//	return true;
	//}

	const bool bMouseButtonDown = InViewport->KeyState(EKeys::LeftMouseButton) || InViewport->KeyState(EKeys::MiddleMouseButton) || InViewport->KeyState(EKeys::RightMouseButton);
	//const bool bLightMoveDown = InViewport->KeyState(EKeys::L);

	// Look at which axis is being dragged and by how much
	const float DragX = (Key == EKeys::MouseX) ? Delta : 0.f;
	const float DragY = (Key == EKeys::MouseY) ? Delta : 0.f;

	// 	if (bLightMoveDown && bMouseButtonDown && PreviewScene)
	// 	{
	// 		// Adjust the preview light direction
	// 		FRotator LightDir = PreviewScene->GetLightDirection();
	// 
	// 		LightDir.Yaw += -DragX * LightRotSpeed;
	// 		LightDir.Pitch += -DragY * LightRotSpeed;
	// 
	// 		PreviewScene->SetLightDirection(LightDir);
	// 
	// 		// Remember that we adjusted it for the visualization
	// 		MovingPreviewLightTimer = PreviewLightConstants::MovingPreviewLightTimerDuration;
	// 		MovingPreviewLightSavedScreenPos = FVector2D(LastMouseX, LastMouseY);
	// 
	// 		Invalidate();
	// 	}
	// 	else
	// 	{
	// 		/**Save off axis commands for future camera work*/
	// 		FCachedJoystickState* JoystickState = GetJoystickState(ControllerId);
	// 		if (JoystickState)
	// 		{
	// 			JoystickState->AxisDeltaValues.Add(Key, Delta);
	// 		}

	if (bIsTracking)
	{
		// Accumulate and snap the mouse movement since the last mouse button click.
		MouseDeltaTracker->AddDelta(this, Key, Delta, 0);
	}
	//}

	/*FIntPoint MousePos;
	if (Viewport)
	{
	Viewport->GetMousePos(MousePos);
	FString InputStr = FString::Printf(TEXT("CurMouse:%d,%d\rStart:%.2f,%.2f\rEnd:%.2f,%.2f\rLastMouse:%d,%d\rDelta:%.2f,%.2f"),
	MousePos.X, MousePos.Y,
	MouseDeltaTracker->GetDragStartPos().X, MouseDeltaTracker->GetDragStartPos().Y,
	MouseDeltaTracker->GetDragEndPos().X, MouseDeltaTracker->GetDragEndPos().Y,
	LastMouseX, LastMouseY,
	MouseDeltaTracker->GetDelta().X, MouseDeltaTracker->GetDelta().Y
	);

	FArmyOutputLogModule& OutputLogModule = FModuleManager::LoadModuleChecked<FArmyOutputLogModule>(TEXT("ArmyOutputLog"));
	OutputLogModule.SetDebugCategoryContent(2, InputStr);
	}*/


	// If we are using a drag tool, paint the viewport so we can see it update.
	if (MouseDeltaTracker->UsingDragTool())
	{
		Invalidate(false, false);
	}

	return true;
}

bool UArmyEditorViewportClient::InputGesture(FViewport* InViewport, EGestureEvent GestureType, const FVector2D& GestureDelta, bool bIsDirectionInvertedFromDevice)
{
	return false;
}

void UArmyEditorViewportClient::UpdateGestureDelta()
{

}

void UArmyEditorViewportClient::ConvertMovementToDragRot(const FVector& InDelta, FVector& InDragDelta, FRotator& InRotDelta) const
{
	const FRotator& ViewRotation = GetViewRotation();

	const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
	const bool MiddleMouseButtonDown = Viewport->KeyState(EKeys::MiddleMouseButton);
	const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
	const bool bIsUsingTrackpad = FSlateApplication::Get().IsUsingTrackpad();

	InDragDelta = FVector::ZeroVector;
	InRotDelta = FRotator::ZeroRotator;

	switch (GetViewportType())
	{
	case EArmyLevelViewportType::LVT_OnlyCanvas:
	case EArmyLevelViewportType::LVT_OrthoXY:
	case EArmyLevelViewportType::LVT_OrthoXZ:
	case EArmyLevelViewportType::LVT_OrthoYZ:
	case EArmyLevelViewportType::LVT_OrthoNegativeXY:
	case EArmyLevelViewportType::LVT_OrthoNegativeXZ:
	case EArmyLevelViewportType::LVT_OrthoNegativeYZ:
	{
		if ((LeftMouseButtonDown || bIsUsingTrackpad) && RightMouseButtonDown)
		{
			// Both mouse buttons change the ortho viewport zoom.
			InDragDelta = FVector(0, 0, InDelta.Z);
		}
		else if (RightMouseButtonDown)
		{
			// @todo: set RMB to move opposite to the direction of drag, in other words "grab and pull".
			//InDragDelta = InDelta;
		}
		else if (LeftMouseButtonDown)
		{
			// LMB moves in the direction of the drag.
			InDragDelta = InDelta;
		}
		else if (MiddleMouseButtonDown)
		{
			// LMB moves in the direction of the drag.
			InDragDelta = InDelta;
		}
	}
	break;
	case EArmyLevelViewportType::LVT_Perspective:

	{
		//const ULevelEditorViewportSettings* ViewportSettings = GetDefault<ULevelEditorViewportSettings>();
		float MouseSensitivty = 0.6;
		if (LeftMouseButtonDown && !RightMouseButtonDown)
		{
			// Move forward and yaw

			InDragDelta.X = InDelta.Y * FMath::Cos(ViewRotation.Yaw * PI / 180.f);
			InDragDelta.Y = InDelta.Y * FMath::Sin(ViewRotation.Yaw * PI / 180.f);

			InRotDelta.Yaw = InDelta.X * MouseSensitivty;
		}
		else if (MiddleMouseButtonDown || bIsUsingTrackpad || ((LeftMouseButtonDown || bIsUsingTrackpad) && RightMouseButtonDown))
		{
			// Pan left/right/up/down
			const bool bInvert = !bIsUsingTrackpad && MiddleMouseButtonDown /*&& GetDefault<ULevelEditorViewportSettings>()->bInvertMiddleMousePan*/;


			float Direction = bInvert ? 1 : -1;
			InDragDelta.X = InDelta.X * Direction * FMath::Sin(ViewRotation.Yaw * PI / 180.f);
			InDragDelta.Y = InDelta.X * -Direction * FMath::Cos(ViewRotation.Yaw * PI / 180.f);
			InDragDelta.Z = -Direction * InDelta.Y;
		}
		else if (RightMouseButtonDown && !LeftMouseButtonDown)
		{
			// Change viewing angle

			// inverting orbit axis is handled elsewhere
			const bool bInvertY = !ShouldOrbitCamera() && false;/*&& GetDefault<ULevelEditorViewportSettings>()->bInvertMouseLookYAxis*/;
			float Direction = bInvertY ? -1 : 1;

			InRotDelta.Yaw = InDelta.X * /*ViewportSettings->*/MouseSensitivty;
			InRotDelta.Pitch = InDelta.Y * /*ViewportSettings->*/MouseSensitivty * Direction;
		}
	}

	break;
	case EArmyLevelViewportType::LVT_OrthoFreelook:
	{
		InDragDelta = InDelta;
	}
	break;
	default:
		check(0);	// unknown viewport type
		break;
	}
}

void UArmyEditorViewportClient::ConvertMovementToOrbitDragRot(const FVector& InDelta,
	FVector& InDragDelta,
	FRotator& InRotDelta) const
{

}

TSharedPtr<FArmyDragTool> UArmyEditorViewportClient::MakeDragTool(EArmyDragTool::Type DragToolType)
{
	// Let the drag tool handle the transaction
	//TrackingTransaction.Cancel();

	TSharedPtr<FArmyDragTool> DragTool;
	switch (DragToolType)
	{
	case EArmyDragTool::BoxSelect:
		DragTool = MakeShareable(new FArmyDragTool_ActorBoxSelect(this));
		break;
	case EArmyDragTool::FrustumSelect:
		DragTool = MakeShareable(new FArmyDragTool_ActorFrustumSelect(this));
		break;
	case EArmyDragTool::Measure:
		//DragTool = MakeShareable(new FVRSDragTool_Measure(this));
		break;
	case EArmyDragTool::ViewportChange:
		//DragTool = MakeShareable(new FVRSDragTool_ViewportChange(this));
		break;
	};

	return DragTool;
}

bool UArmyEditorViewportClient::CanUseDragTool() const
{
	return /*!ShouldOrbitCamera() &&*/ (GetCurrentWidgetAxis() == EAxisList::None) && ((ModeTools == nullptr) || ModeTools->AllowsViewportDragTool());
}

bool UArmyEditorViewportClient::ShouldOrbitCamera() const
{
	return bIsOrbit;
}

bool UArmyEditorViewportClient::IsMovingCamera() const
{
	return bUsingOrbitCamera;
}

EAxisList::Type UArmyEditorViewportClient::GetCurrentWidgetAxis() const
{
	return Widget->GetCurrentAxis();
}

bool UArmyEditorViewportClient::IsVisible() const
{
	bool bIsVisible = false;

	if (VisibilityDelegate.IsBound())
		bIsVisible = VisibilityDelegate.Execute();

	return bIsVisible;
}

bool UArmyEditorViewportClient::IsActorAutoRotation(AActor* InActor)
{
	bool AutoRotation = false;
	UArmyResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(InActor->GetSynID());
	if (ActorItemInfo.IsValid())
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ActorItemInfo->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return false;
		TSharedPtr<FContentItemSpace::FModelRes> model = StaticCastSharedPtr<FContentItemSpace::FModelRes>(resArr[0]);
		//only the object on wall need auto rotate with the wall normal
		AutoRotation = !model->placePosition.bCeiling && !model->placePosition.bFloor &&
			model->placePosition.bWall && !model->placePosition.bMesa;
	}
	return AutoRotation;
}

bool UArmyEditorViewportClient::UpdateAutoRotation(AActor* InActor, const TArray<AActor*>& SelectActors, const FBox& ActorInBox, const FVector& InDeltaDrag)
{
	FVector Center, Extent;
	float DefaultRayRangeToBoxBorderInterval = 10.f;//默认的固定值30mm
	float ThresholdRayRangeToBoxBorderInterval = 5.f;//比例阈值50mm
	float ExtentScale = 0.1f;
	ActorInBox.GetCenterAndExtents(Center, Extent);
	float SubX = Extent.X >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.X;
	float SubY = Extent.Y >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.Y;
	float SubZ = Extent.Z >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.Z;

	Extent.X -= SubX;
	Extent.Y -= SubY;
	Extent.Z -= SubZ;

	// Extent = FVector(FMath::Abs(Extent.X), FMath::Abs(Extent.Y), FMath::Abs(Extent.Z));
	FVector PreActorLocation = InActor->GetActorLocation();
	FVector ActorToCenterVector = Center - InActor->GetActorLocation();//Actor到包围盒中心点的一个矢量

	FHitResult OutHit(ForceInit);
	bool bHit = false;

	static FName TraceTag = FName(TEXT("AdjustTrace"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.bTraceAsyncScene = true;
	AController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
		TraceParams.AddIgnoredActor(PlayerController->GetPawn());
	for (int i = 0; i < SelectActors.Num(); i++)
	{
		TraceParams.AddIgnoredActor(SelectActors[i]);
	}

	FVector TraceStart = Center;
	FVector TraceEnd;
	if (FMath::IsNearlyZero(InDeltaDrag.Size()))
	{
		return false;
	}

	{
		FVector TestDelta(ForceInitToZero);
		InDeltaDrag.X > 0.001 ? TestDelta.X = 1 : (InDeltaDrag.X < -0.001 ? TestDelta.X = -1 : 0);
		InDeltaDrag.Y > 0.001 ? TestDelta.Y = 1 : (InDeltaDrag.Y < -0.001 ? TestDelta.Y = -1 : 0);
		InDeltaDrag.Z > 0.001 ? TestDelta.Z = 1 : (InDeltaDrag.Z < -0.001 ? TestDelta.Z = -1 : 0);

		TraceEnd = TraceStart - TestDelta * DefaultRayRangeToBoxBorderInterval;
		bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
			FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
			FCollisionShape::MakeBox(Extent), TraceParams);
		if (bHit)
		{
			TraceEnd = TraceStart + TestDelta * DefaultRayRangeToBoxBorderInterval * 2;
			bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
				FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
				FCollisionShape::MakeBox(Extent), TraceParams);
			if (!bHit && (InDeltaDrag * TestDelta).Size() > (TestDelta * DefaultRayRangeToBoxBorderInterval * 2).Size())
			{
				return false;
			}
			else
			{
				FVector MoveDir = (TraceEnd - TraceStart).GetSafeNormal();
				FVector LastPos = PreActorLocation + MoveDir * (OutHit.Distance - (TestDelta * FVector(SubX, SubY, SubZ)).Size());

				FVector Normal = OutHit.ImpactNormal;
				float TempAngle = FQuat::FindBetweenNormals(MoveDir, OutHit.Normal.GetSafeNormal()).GetAngle();
				if (TempAngle > (PI / 2) && IsActorAutoRotation(InActor))
				{
					InActor->SetActorRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), Normal));
					return true;
				}
			}
		}
		else
		{
			TraceEnd = TraceStart + TestDelta * DefaultRayRangeToBoxBorderInterval * 2;
			bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
				FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
				FCollisionShape::MakeBox(Extent), TraceParams);
			if (bHit)
			{
				FVector MoveDir = (TraceEnd - TraceStart).GetSafeNormal();
				FVector LastPos = PreActorLocation + MoveDir * (OutHit.Distance - (TestDelta * FVector(SubX, SubY, SubZ)).Size());

				FVector Normal = OutHit.ImpactNormal;
				float TempAngle = FQuat::FindBetweenNormals(MoveDir, OutHit.Normal.GetSafeNormal()).GetAngle();
				if (TempAngle > (PI / 2) && IsActorAutoRotation(InActor))
				{
					InActor->SetActorRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), Normal));
					return true;
				}
				return false;
			}

		}
	}
	return false;
}

void UArmyEditorViewportClient::SetCurrentWidgetAxis(EAxisList::Type InAxis)
{
	Widget->SetCurrentAxis(InAxis);
}

float UArmyEditorViewportClient::GetNearClipPlane() const
{
	return (NearPlane < 0.0f) ? GNearClippingPlane : NearPlane;
}

float UArmyEditorViewportClient::GetFarClipPlaneOverride() const
{
	return FarPlane;
}

void UArmyEditorViewportClient::OverrideFarClipPlane(const float InFarPlane)
{
	FarPlane = InFarPlane;
}

void UArmyEditorViewportClient::OverrideNearClipPlane(float InNearPlane)
{
	NearPlane = InNearPlane;
}

FVector UArmyEditorViewportClient::GetHitProxyObjectLocation(int32 X, int32 Y)
{
	// #todo: for now we are just getting the actor and using its location for 
	// depth. in the future we will just sample the depth buffer
	HHitProxy* const HitProxy = Viewport->GetHitProxy(X, Y);
	if (HitProxy && HitProxy->IsA(HActor::StaticGetType()))
	{
		HActor* const ActorHit = static_cast<HActor*>(HitProxy);

		// dist to component will be more reliable than dist to actor
		if (ActorHit->PrimComponent != nullptr)
		{
			return ActorHit->PrimComponent->GetComponentLocation();
		}

		if (ActorHit->Actor != nullptr)
		{
			return ActorHit->Actor->GetActorLocation();
		}
	}

	return FVector::ZeroVector;
}

void UArmyEditorViewportClient::ShowWidget(const bool bShow)
{
	bShowWidget = bShow;
}

void UArmyEditorViewportClient::MoveViewportCamera(const FVector& InDrag, const FRotator& InRot, bool bDollyCamera)
{
	switch (GetViewportType())
	{
	case EArmyLevelViewportType::LVT_OnlyCanvas:
	case EArmyLevelViewportType::LVT_OrthoXY:
	case EArmyLevelViewportType::LVT_OrthoXZ:
	case EArmyLevelViewportType::LVT_OrthoYZ:
	case EArmyLevelViewportType::LVT_OrthoNegativeXY:
	case EArmyLevelViewportType::LVT_OrthoNegativeXZ:
	case EArmyLevelViewportType::LVT_OrthoNegativeYZ:
	{
		const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
		const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
		const bool bIsUsingTrackpad = FSlateApplication::Get().IsUsingTrackpad();

		if ((LeftMouseButtonDown || bIsUsingTrackpad) && RightMouseButtonDown)
		{
			SetOrthoZoom(GetOrthoZoom() + (GetOrthoZoom() / CAMERA_ZOOM_DAMPEN) * InDrag.Z);
			SetOrthoZoom(FMath::Clamp<float>(GetOrthoZoom(), MIN_ORTHOZOOM, MAX_ORTHOZOOM));
		}
		else
		{
			SetViewLocation(GetViewLocation() + InDrag);
		}

		// Update any linked orthographic viewports.
		UpdateLinkedOrthoViewports();
	}
	break;

	case EArmyLevelViewportType::LVT_OrthoFreelook:
	{
		SetViewLocation(GetViewLocation() + InDrag);
	}
	//@TODO: CAMERA: Not sure how to handle this
	break;

	case EArmyLevelViewportType::LVT_Perspective:
	{
		MoveViewportPerspectiveCamera(InDrag, InRot, bDollyCamera);
	}
	break;
	}
}

void UArmyEditorViewportClient::PlaceActor(AActor* _NewActor, bool IsReplacing)
{
	if (IsReplacing)
	{
		GArmyEditor->SelectNone(false, false, false);
		GArmyEditor->SelectActor(_NewActor, true, true);
		return;
	}
	FVector2D Size;
	GetViewportSize(Size);
	FVector ActorLocation = _NewActor->GetActorLocation();
	if (ActorLocation.IsNearlyZero())
		Viewport->SetMouse(Size.X / 2, Size.Y * 2 / 3);
	else
	{
		FVector2D PixPos;
		WorldToPixel(ActorLocation, PixPos);
		Viewport->SetMouse(PixPos.X, PixPos.Y);
	}

	GArmyEditor->SelectNone(false, false, false);
	GArmyEditor->SelectActor(_NewActor, true, true);
	SetWidgetMode(FArmyWidget::WM_Translate);
	TArray<UObject*> Objects;
	Objects.Add(_NewActor);
	SetDroppedObjects(Objects);
	SetDroppingPreviewTexture(false);
	CurCursor = EMouseCursor::Default;
	Invalidate(true, true);
}

void UArmyEditorViewportClient::PlaceMaterial(UMaterialInterface* _NewMaterial, FString _ThumbnailURL)
{
	GArmyEditor->SelectNone(false, false, false);
	TArray<UObject*> Objects;
	Objects.Add(_NewMaterial);
	SetDroppedObjects(Objects);
	AddMaterialCount = 0;
	SetDroppingPreviewTexture(true);
	CurCursor = EMouseCursor::Custom;

	SetCustomCursorWidget(
		SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SImage)
			.Image(FArmySlateModule::Get().WebImageCache->Download(_ThumbnailURL).Get().Attr())
		.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.6)))
		]
	/*+ SOverlay::Slot()
	.VAlign(VAlign_Center)
	.HAlign(HAlign_Center)
	[
	SNew(SImage)
	.Image(&GRes->Crosshair)
	]*/
	);
}

bool UArmyEditorViewportClient::ShouldLockPitch() const
{
	return CameraController->GetConfig().bLockedPitch;
}

void UArmyEditorViewportClient::CheckHoveredHitProxy(HHitProxy* HoveredHitProxy)
{
	const EAxisList::Type SaveAxis = Widget->GetCurrentAxis();
	EAxisList::Type NewAxis = EAxisList::None;

	const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton) ? true : false;
	const bool MiddleMouseButtonDown = Viewport->KeyState(EKeys::MiddleMouseButton) ? true : false;
	const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton) ? true : false;
	const bool bMouseButtonDown = (LeftMouseButtonDown || MiddleMouseButtonDown || RightMouseButtonDown);

	// Change the mouse cursor if the user is hovering over something they can interact with.
	if (HoveredHitProxy)
	{
		if (HoveredHitProxy->IsA(HArmyWidgetAxis::StaticGetType()) && !bUsingOrbitCamera && !bMouseButtonDown)
		{
			// In the case of the widget mode being overridden we can have a hit proxy
			// from the previous mode with an inappropriate axis for rotation.
			EAxisList::Type ProxyAxis = ((HArmyWidgetAxis*)HoveredHitProxy)->Axis;
			if (!IsOrtho() || GetWidgetMode() != FArmyWidget::WM_Rotate
				|| ProxyAxis == EAxisList::X || ProxyAxis == EAxisList::Y || ProxyAxis == EAxisList::Z)
			{
				NewAxis = ProxyAxis;
			}
			else
			{
				switch (GetViewportType())
				{
				case EArmyLevelViewportType::LVT_OnlyCanvas:
				case EArmyLevelViewportType::LVT_OrthoXY:
				case EArmyLevelViewportType::LVT_OrthoNegativeXY:
					NewAxis = EAxisList::Z;
					break;
				case EArmyLevelViewportType::LVT_OrthoXZ:
				case EArmyLevelViewportType::LVT_OrthoNegativeXZ:
					NewAxis = EAxisList::Y;
					break;
				case EArmyLevelViewportType::LVT_OrthoYZ:
				case EArmyLevelViewportType::LVT_OrthoNegativeYZ:
					NewAxis = EAxisList::X;
					break;
				default:
					break;
				}
			}
		}


		// If the current axis on the widget changed, repaint the viewport.
		if (NewAxis != SaveAxis)
		{
			SetCurrentWidgetAxis(NewAxis);

			//Invalidate(false, false);
		}
	}
}

bool UArmyEditorViewportClient::ShouldPanOrDollyCamera() const
{
	const bool bIsCtrlDown = IsCtrlPressed();

	const bool bLeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
	const bool bRightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
	const bool bIsMarqueeSelect = IsOrtho() && bLeftMouseButtonDown;

	const bool bOrthoRotateObjectMode = IsOrtho() && IsCtrlPressed() && bRightMouseButtonDown && !bLeftMouseButtonDown;
	// Pan the camera if not marquee selecting or the left and right mouse buttons are down
	return !bOrthoRotateObjectMode && !bIsCtrlDown && (!bIsMarqueeSelect || (bLeftMouseButtonDown && bRightMouseButtonDown));
}

void UArmyEditorViewportClient::ConditionalCheckHoveredHitProxy()
{
	//bShouldCheckHitProxy = true;

	// If it has been decided that there is more important things to do than check hit proxies, then don't check them.
	if (!bShouldCheckHitProxy || bWidgetAxisControlledByDrag == true)
	{
		return;
	}
	//float Scale = EngineShowFlags.ScreenPercentage / 100.f;
	//const float XScale = (float)Viewport->.ViewRect.Max.X / (float)View.UnscaledViewRect.Max.X;
	//const float YScale = (float)View.ViewRect.Max.Y / (float)View.UnscaledViewRect.Max.Y;
	//HitX = HitX * XScale;
	//HitY = HitY * YScale;

	static const auto ScreenPercentageCVar = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.ScreenPercentage"));
	float Value = ScreenPercentageCVar->GetValueOnGameThread();
	float MouseX = CachedMouseX * Value / 100.f * MouseScale.X;
	float MouseY = CachedMouseY * Value / 100.f * MouseScale.Y;
	HHitProxy* HitProxy = Viewport->GetHitProxy(MouseX, MouseY);

	CheckHoveredHitProxy(HitProxy);

	// We need to set this to false here as if mouse is moved off viewport fast, it will keep doing CheckHoveredOverHitProxy for this viewport when it should not.
	bShouldCheckHitProxy = false;
}

void UArmyEditorViewportClient::MoveViewportPerspectiveCamera(const FVector& InDrag, const FRotator& InRot, bool bDollyCamera)
{
	check(IsPerspective());

	FVector ViewLocation = GetViewLocation();
	FRotator ViewRotation = GetViewRotation();

	if (ShouldLockPitch())
	{
		// Update camera Rotation
		ViewRotation += FRotator(InRot.Pitch, InRot.Yaw, InRot.Roll);

		// normalize to -180 to 180
		ViewRotation.Pitch = FRotator::NormalizeAxis(ViewRotation.Pitch);
		// Make sure its withing  +/- 90 degrees.
		ViewRotation.Pitch = FMath::Clamp(ViewRotation.Pitch, -90.f, 90.f);
	}
	else
	{
		//when not constraining the pitch (matinee feature) we need to rotate differently to avoid a gimbal lock
		const FRotator PitchRot(InRot.Pitch, 0, 0);
		const FRotator LateralRot(0, InRot.Yaw, InRot.Roll);

		//update lateral rotation
		ViewRotation += LateralRot;

		//update pitch separately using quaternions
		const FQuat ViewQuat = ViewRotation.Quaternion();
		const FQuat PitchQuat = PitchRot.Quaternion();
		const FQuat ResultQuat = ViewQuat * PitchQuat;

		//get our correctly rotated ViewRotation
		ViewRotation = ResultQuat.Rotator();
	}

	// Update camera Location
	ViewLocation += InDrag;

	if (!bDollyCamera)
	{
		const float DistanceToCurrentLookAt = FVector::Dist(GetViewLocation(), GetLookAtLocation());

		const FQuat CameraOrientation = FQuat::MakeFromEuler(ViewRotation.Euler());
		FVector Direction = CameraOrientation.RotateVector(FVector(1, 0, 0));

		SetLookAtLocation(ViewLocation + Direction * DistanceToCurrentLookAt);
	}

	SetViewLocation(ViewLocation);
	SetViewRotation(ViewRotation);

	if (bUsingOrbitCamera)
	{
		FVector LookAtPoint = GetLookAtLocation();
		const float DistanceToLookAt = FVector::Dist(ViewLocation, LookAtPoint);

		SetViewLocationForOrbiting(LookAtPoint, DistanceToLookAt);
	}

	PerspectiveCameraMoved();
}

void UArmyEditorViewportClient::SetShowGrid(bool isShow)
{
	bUsesDrawHelper = isShow;
}

void UArmyEditorViewportClient::SetRealtimePreview()
{
	SetRealtime(!IsRealtime());
	Invalidate();
}

void UArmyEditorViewportClient::Invalidate(bool bInvalidateChildViews /*= true*/, bool bInvalidateHitProxies /*= true*/)
{
	if (Viewport)
	{
		if (bInvalidateHitProxies)
		{
			// Invalidate hit proxies and display pixels.
			Viewport->Invalidate();
		}
		else
		{
			// Invalidate only display pixels.
			Viewport->InvalidateDisplay();
		}

		// If this viewport is a view parent . . .
		//if (bInvalidateChildViews &&
		//	ViewState.GetReference()->IsViewParent())
		//{
		//GEditor->InvalidateChildViewports(ViewState.GetReference(), bInvalidateHitProxies);
		//}
	}
}

void UArmyEditorViewportClient::MouseEnter(FViewport* InViewport, int32 x, int32 y)
{
	ModeTools->MouseEnter(this, Viewport, x, y);

	MouseMove(InViewport, x, y);

	//PixelInspectorRealtimeManagement(this, true);
}

void UArmyEditorViewportClient::MouseMove(FViewport* InViewport, int32 x, int32 y)
{
	check(IsInGameThread());

	CurrentMousePos = FIntPoint(x, y);

	// Let the current editor mode know about the mouse movement.
	ModeTools->MouseMove(this, Viewport, x, y);
}

void UArmyEditorViewportClient::MouseLeave(FViewport* InViewport)
{
	check(IsInGameThread());

	ModeTools->MouseLeave(this, Viewport);

	CurrentMousePos = FIntPoint(-1, -1);

	FCommonViewportClient::MouseLeave(InViewport);

	//PixelInspectorRealtimeManagement(this, false);
}

void UArmyEditorViewportClient::CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY)
{
	//UpdateRequiredCursorVisibility();
	//ApplyRequiredCursorVisibility();

	// Let the current editor mode know about the mouse movement.
	if (ModeTools->CapturedMouseMove(this, InViewport, InMouseX, InMouseY))
	{
		return;
	}
}

void UArmyEditorViewportClient::OpenScreenshot(FString SourceFilePath)
{
	FPlatformProcess::ExploreFolder(*(FPaths::GetPath(SourceFilePath)));
}

void UArmyEditorViewportClient::TakeScreenshot(FViewport* InViewport, bool bInValidatViewport)
{

}

void UArmyEditorViewportClient::TakeScreenshotInMaxRect(int32 ScaledRectSize, TArray<FColor>& OutUncompressedData, TArray<uint8>& OutCompressedData)
{
	int32 X = Viewport->GetSizeXY().X;
	int32 Y = Viewport->GetSizeXY().Y;
	FIntRect InRect;
	if (X >= Y)
	{
		InRect = FIntRect((X - Y) / 2, 0, (X + Y) / 2, Y);
	}
	else
	{
		InRect = FIntRect(0, (Y - X) / 2, X, (X + Y) / 2);
	}

	TArray<FColor> RawData;

	GetViewportScreenShot(Viewport, RawData, InRect);

	int32 ScaledWidth = ScaledRectSize;
	int32 ScaledHeight = ScaledRectSize;
	if (X >= Y)
	{
		FImageUtils::ImageResize(Y, Y, RawData, ScaledWidth, ScaledHeight, OutUncompressedData, true);
	}
	else
	{
		FImageUtils::ImageResize(X, X, RawData, ScaledWidth, ScaledHeight, OutUncompressedData, true);
	}

	TSharedPtr<IImageWrapper> ImageWrapper;
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	ImageWrapper->SetRaw(OutUncompressedData.GetData(), ScaledWidth * ScaledHeight * sizeof(FColor), ScaledWidth, ScaledHeight, ERGBFormat::BGRA, 8);
	OutCompressedData = ImageWrapper->GetCompressed(50);
}

bool UArmyEditorViewportClient::InputTakeScreenshot(FViewport* InViewport, FKey Key, EInputEvent Event)
{
	const bool F9Down = InViewport->KeyState(EKeys::F9);

	// Whether or not we accept the key press
	bool bHandled = false;

	if (F9Down)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (Event == IE_Pressed)
			{
				// We need to invalidate the viewport in order to generate the correct pixel buffer for picking.
				Invalidate(false, true);
			}
			else if (Event == IE_Released)
			{
				TakeScreenshot(InViewport, false);
			}
			bHandled = true;
		}
	}

	return bHandled;
}

void UArmyEditorViewportClient::TakeHighResScreenShot()
{
	if (Viewport)
	{
		Viewport->TakeHighResScreenShot();
	}
}

void UArmyEditorViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice /*= true*/)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);

	ConsoleCommand("t.IdleWhenNotForeground 1");
	ConsoleCommand("r.SceneRenderTargetResizeMethod 2");
	ConsoleCommand("r.DefaultFeature.MotionBlur 0");
	//CommandList = MakeShareable(new FUICommandList);
	//FArmyEditorViewportCommands::Register();
	//BindCommands();
}

void UArmyEditorViewportClient::ProcessScreenShots(FViewport* InViewport)
{
}

void UArmyEditorViewportClient::SetDroppedObjects(TArray<UObject*> _Objects)
{
	DroppedObjects = _Objects;
}

bool UArmyEditorViewportClient::IsDroppingObjects()
{
	return DroppedObjects.Num() != 0;
}

UObject * UArmyEditorViewportClient::GetFirstDroppingObject()
{
	if (IsDroppingObjects())
	{
		return DroppedObjects[0];
	}
	return nullptr;
}

void UArmyEditorViewportClient::SetDroppingPreviewTexture(bool _Dropping)
{
	bDroppingPreviewTexture = _Dropping;
}

bool UArmyEditorViewportClient::GetDroppingPreviewTexture()
{
	return bDroppingPreviewTexture;
}

static bool AttemptApplyObjToComponent(UObject* ObjToUse, USceneComponent* ComponentToApplyTo, int32 TargetMaterialSlot = -1, bool bTest = false)
{
	bool bResult = false;

	if (ComponentToApplyTo)
	{
		UMeshComponent* MeshComponent = Cast<UMeshComponent>(ComponentToApplyTo);
		if (MeshComponent)
		{
			// Dropping a material?
			UMaterialInterface* DroppedObjAsMaterial = Cast<UMaterialInterface>(ObjToUse);
			if (DroppedObjAsMaterial)
			{
				if (TargetMaterialSlot == -1)
					TargetMaterialSlot = 0;

				//检查是不是阴影材质
				if (MeshComponent->OverrideMaterials.IsValidIndex(TargetMaterialSlot))
				{
					UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MeshComponent->OverrideMaterials[TargetMaterialSlot]);
					if (MID)
					{
						FString TempName = MID->Parent->GetName();
						if (TempName.Contains(TEXT("Shadow"), ESearchCase::IgnoreCase, ESearchDir::FromEnd))
						{
							bResult = false;
							return bResult;
						}
					}
				}

				MeshComponent->SetMaterial(TargetMaterialSlot, DroppedObjAsMaterial);
				MeshComponent->MarkRenderStateDirty();
				bResult = true;
			}
		}
	}

	return bResult;
}

static bool AttemptApplyObjToActor(UObject* ObjToUse, AActor* ActorToApplyTo, int32 TargetMaterialSlot = -1, bool bTest = false)
{
	bool bResult = false;

	if (ActorToApplyTo)
	{
		bResult = false;

		TInlineComponentArray<USceneComponent*> SceneComponents;
		ActorToApplyTo->GetComponents(SceneComponents);
		for (USceneComponent* SceneComp : SceneComponents)
		{
			bResult |= AttemptApplyObjToComponent(ObjToUse, SceneComp, TargetMaterialSlot, bTest);
		}

		// Notification hook for dropping asset onto actor
		if (!bTest)
		{
			//FEditorDelegates::OnApplyObjectToActor.Broadcast(ObjToUse, ActorToApplyTo);
		}
	}

	return bResult;
}

void UArmyEditorViewportClient::DropObjectAtCoordinates(int32 MouseX, int32 MouseY, UObject* DroppedObject, bool SelectObject)
{
	if (DroppedObject)
	{
		// Compute a view.
		FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
			Viewport,
			GetScene(),
			EngineShowFlags)
			.SetRealtimeUpdate(true));
		FSceneView* View = CalcSceneView(&ViewFamily);

		FArmyViewportCursorLocation Cursor(View, this, MouseX, MouseY);
		HHitProxy* HitProxy = Viewport->GetHitProxy(Cursor.GetCursorPos().X, Cursor.GetCursorPos().Y);

		if (HitProxy == nullptr)
		{
		}
		else if (HitProxy->IsA(HActor::StaticGetType()))
		{
			AActor* TargetActor = NULL;
			int32 TargetMaterialSlot = -1;

			HActor* TargetProxy = static_cast<HActor*>(HitProxy);
			TargetActor = TargetProxy->Actor;
			TargetMaterialSlot = TargetProxy->MaterialIndex;

			DropObjectOnActor(View, Cursor, DroppedObject, TargetActor, TargetMaterialSlot, SelectObject);
		}
	}
}

bool UArmyEditorViewportClient::DropObjectOnActor(FSceneView* View, FArmyViewportCursorLocation& Cursor, UObject* DroppedObject, AActor* TargetActor, int32 DroppedUponSlot, bool SelectObject)
{
	if (DroppedObject && TargetActor)
	{
		//绘制户型的外景模型叫做SM_Surrounding 外景物体不能附材质
		FString ActorName = TargetActor->GetName();
		if (ActorName.StartsWith(TEXT("SM_Surrounding")) || ActorName.StartsWith(TEXT("SkyActor")))
		{
			return false;
		}

		//如果是静态墙体，则强制赋值到Material0上
		bool bMovable = false;
		if (TargetActor->Tags.Num() > 0 && TargetActor->Tags[0] == TEXT("MoveableMeshActor"))
			bMovable = true;

		//if (!bMovable)
		//	 DroppedUponSlot = 0;

		if (DroppedUponSlot == -1)
			DroppedUponSlot = 0;

		UMaterialInterface* MI = Cast<UMaterialInterface>(DroppedObject);
		if (MI)
		{
			//如果连续添加壁纸，则需要复制一份内存
			if (AddMaterialCount > 0)
			{
				CreateNewDroppedMIDEvent.ExecuteIfBound(MI);
				DroppedObject = DroppedObjects[0];
				MI = Cast<UMaterialInterface>(DroppedObject);
			}
			//如果是硬件造型，则需要把材质开启UV投射，变成万象材质
			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MI);
			if (MID)
			{
				if (bMovable)
					MID->SetScalarParameterValue(TEXT("UseProjectiveUV"), 1);
				else
					MID->SetScalarParameterValue(TEXT("UseProjectiveUV"), 0);

				const bool bAppliedToActor = AttemptApplyObjToActor(MID, TargetActor, DroppedUponSlot, false);

				if (bAppliedToActor)
				{
					AddMaterialCount++;
					GArmyEditor->SelectNone(true, true);
					GArmyEditor->SelectActor(TargetActor, true, true);
					return true;
				}
			}
			//GVRSEditor->OnAddActorMaterial.ExecuteIfBound(TargetActor, SMA->GetStaticMeshComponent()->GetMaterial(DroppedUponSlot), DroppedUponSlot);
			//SMA->GetStaticMeshComponent()->SetMaterial(DroppedUponSlot, MI);
		}
	}
	return false;
	//if (DroppedObject && TargetActor)
	//{
	// AStaticMeshActor* SMA = Cast<AStaticMeshActor>(TargetActor);
	// if (SMA)
	// {
	//	 //如果是静态墙体，则强制赋值到Material0上
	//	 bool bMovable = false;
	//	 if (SMA->Tags.Num() > 0 && SMA->Tags[0] == TEXT("MoveableMeshActor"))
	//		 bMovable = true;
	//	 if (!bMovable)
	//		 DroppedUponSlot = 0;

	//	 FString str = SMA->GetStaticMeshComponent()->GetStaticMesh()->GetPathName();
	//	 if (str.EndsWith(TEXT("_W"), ESearchCase::IgnoreCase) || str.EndsWith(TEXT("_F"), ESearchCase::IgnoreCase) || str.EndsWith(TEXT("_T"), ESearchCase::IgnoreCase))
	//	 {
	//		 if (DroppedUponSlot == -1)
	//		 {
	//			 DroppedUponSlot = 0;
	//		 }

	//		 UMaterialInterface* MI = Cast<UMaterialInterface>(DroppedObject);
	//		 if (MI)
	//		 {
	//			 //如果连续添加壁纸，则需要复制一份内存
	//			 if (AddMaterialCount > 0)
	//			 {
	//				 CreateNewDroppedMIDEvent.ExecuteIfBound(MI);
	//				 DroppedObject = DroppedObjects[0];
	//				 MI = Cast<UMaterialInterface>(DroppedObject);
	//			 }
	//			 //如果是硬件造型，则需要把材质开启UV投射，变成万象材质
	//			 UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MI);
	//			 if (MID)
	//			 {
	//				 if (bMovable)
	//					 MID->SetScalarParameterValue(TEXT("UseProjectiveUV"), 1);
	//				 else
	//					 MID->SetScalarParameterValue(TEXT("UseProjectiveUV"), 0);
	//			 }
	//			 GVRSEditor->OnAddActorMaterial.ExecuteIfBound(SMA, SMA->GetStaticMeshComponent()->GetMaterial(DroppedUponSlot), DroppedUponSlot);
	//			 SMA->GetStaticMeshComponent()->SetMaterial(DroppedUponSlot, MI);

	//			 AddMaterialCount++;
	//		 }
	//		 GVRSEditor->SelectNone(true, true);
	//		 GVRSEditor->SelectActor(TargetActor, true, true);
	//		 return true;
	//	 }
	// }
	//}
	//return false;
}

void UArmyEditorViewportClient::UpdateDroppedObject()
{
	if (DroppedObjects.Num() == 1)
	{
		AActor* DroppedActor = Cast<AActor>(DroppedObjects[0]);
		if (DroppedActor)
		{
			TArray<AActor*> Actors;
			Actors.Add(DroppedActor);
			ProjectActorsIntoWorld(Actors);
		}
	}
	else if (DroppedObjects.Num() > 1)
	{
		TArray<AActor*> Actors;
		for (auto& It : DroppedObjects)
		{
			AActor* DroppedActor = Cast<AActor>(It);
			if (DroppedActor)
			{
				Actors.Add(DroppedActor);
			}
		}
		ProjectActorsIntoWorld(Actors);
	}
}

void UArmyEditorViewportClient::CancelDrop()
{
	//取消选中之前，把上次选中的物体设置为Static
	TArray<AActor*> SelectedActors;
	GArmyEditor->GetSelectedActors(SelectedActors);

	/*if (SelectedActors.IsValidIndex(0))
	{
		if (SelectedActors[0] && SelectedActors[0]->Tags.Num() > 0 && SelectedActors[0]->Tags[0] == "MoveableMeshActor")
		{
			SelectedActors[0]->GetRootComponent()->SetMobility(EComponentMobility::Static);
		}
	}*/

	for (auto& It : DroppedObjects)
	{
		AActor* Actor = Cast<AActor>(It);
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	TArray<UObject*> Objects;
	SetDroppedObjects(Objects);
	SetDroppingPreviewTexture(false);
	
	GArmyEditor->SelectNone(true, true);
	CurCursor = EMouseCursor::Default;
	bAdsorbed = false;
	//GVRSEditor->RightSelectedEvent.ExecuteIfBound(NULL);
}

void UArmyEditorViewportClient::OverridePostProcessSettings(FSceneView& View)
{
	//const UCameraComponent* CameraComponent = GetCameraComponentForView();
	//if (CameraComponent)
	//{
	//	View.OverridePostProcessSettings(CameraComponent->PostProcessSettings, CameraComponent->PostProcessBlendWeight);
	//}
}

void UArmyEditorViewportClient::ProjectActorsIntoWorld(const TArray<AActor*>& Actors)
{
	int32 PlaceFlag = 0;
	bool AutoRotation = false;//是否自动旋转，例如挂画

	FCollisionQueryParams TraceParams(FName(TEXT("Pick")), true);
	FVector TraceStart, TraceEnd;
	TraceParams.bTraceComplex = false;
	TraceParams.AddIgnoredActor(GetWorld()->GetFirstPlayerController()->GetPawn());
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if ((ActorItr->bHidden && ActorItr->GetFolderPath() != FArmyActorPath::GetRoofPath()) || ActorItr->Tags.Num() > 0 && (ActorItr->Tags[0] == "MoveableMeshActor" || ActorItr->Tags[0] == "BlueprintVRSActor"))
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}
	}
	AArmyGroupActor* GroupActor = NULL;

	FBox SelectActorsBox(ForceInitToZero);
	for (auto& It : Actors)
	{
		AArmyGroupActor* TempGroupActor = AArmyGroupActor::GetRootForActor(It, true, true);
		if (TempGroupActor)
		{
			GroupActor = TempGroupActor;
		}
		TraceParams.AddIgnoredActor(It);

		FVector Org, Exten;
		It->GetActorBounds(true, Org, Exten);

		SelectActorsBox += FBox(Org - Exten, Org + Exten);
	}

	// Compute a view.
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(true));
	FSceneView* View = CalcSceneView(&ViewFamily);
	FVector2D MousePos;
	if (GetMousePosition(MousePos))
	{

		FVector WorldOrigin, WorldDirection, WorldOriginOrtho;
		View->DeprojectFVector2D(MousePos, WorldOrigin, WorldDirection);

		OrthoMousePos = MousePos;
		OrthoWorldOrigin = WorldOrigin;

		if (GetViewportType() == EArmyLevelViewportType::LVT_Perspective)
		{
			TraceStart = WorldOrigin;
			TraceEnd = TraceStart + WorldDirection*10000.f;
		}
		else if (GetViewportType() == EArmyLevelViewportType::LVT_OrthoXY)
		{
			TraceStart = WorldOrigin;
			TraceStart.Z = 20000.f;
			TraceEnd = WorldOrigin;
			TraceEnd.Z = -2000.f;
		}

		FHitResult OutHit(ForceInit);
		GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

		bool bBlockingHit = OutHit.bBlockingHit;
		FVector ImpactPoint = OutHit.ImpactPoint;
		FVector ImpactNormal = OutHit.ImpactNormal;

		int32 HitObjType = 0;
		if (OutHit.Actor.IsValid())
		{
			if (OutHit.Actor->GetFolderPath() == FArmyActorPath::GetFloorPath())
			{
				HitObjType = 1;
			}
			else if (OutHit.Actor->GetFolderPath() == FArmyActorPath::GetRoofPath())
			{
				HitObjType = 2;
			}
			else if (OutHit.Actor->GetFolderPath() == FArmyActorPath::GetWallPath())
			{
				HitObjType = 3;
			}
		}

		/**	add local hit,for example DBJ BIM FArmyObject element hit*/
		//////////////////////////////////////////////////////////////////////////
		//FVector LocalHitPoint, LocalHitNormal;
		//int32 HitObjType = 0;
		//if (LocalLineTraceSingleByChannel(LocalHitPoint, LocalHitNormal, HitObjType, TraceStart, TraceEnd))
		//{
		//	if ((LocalHitPoint - TraceStart).Size() < OutHit.Distance)
		//	{
		//		bBlockingHit = true;
		//		ImpactPoint = LocalHitPoint;
		//		ImpactNormal = LocalHitNormal;
		//	}
		//}

		if (Actors.Num() == 1)
		{
			UArmyResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
			TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(Actors[0]->GetSynID());
			if (ActorItemInfo.IsValid())
			{
				TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ActorItemInfo->GetResObjNoComponent();
				if (resArr.Num() < 1)
					return;
				TSharedPtr<FContentItemSpace::FModelRes> model = StaticCastSharedPtr<FContentItemSpace::FModelRes>(resArr[0]);

				bool OnlyCeiling = model->placePosition.bCeiling && !model->placePosition.bFloor &&
					!model->placePosition.bWall && !model->placePosition.bMesa;

				//only the object on wall need auto rotate with the wall normal
				AutoRotation = !model->placePosition.bCeiling && !model->placePosition.bFloor &&
					model->placePosition.bWall && !model->placePosition.bMesa;

				if (OnlyCeiling && HitObjType != 2)//only ceiling and the hit point not is ceiling point
				{
					TraceStart = bBlockingHit ? ImpactPoint + ImpactNormal : FVector(0, 0, 1);
					TraceEnd = TraceStart + FVector(0, 0, 2000);

					FHitResult NewOutHit(ForceInit);
					if (GetWorld()->LineTraceSingleByChannel(NewOutHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams))
					{
						bBlockingHit = NewOutHit.bBlockingHit;
						ImpactPoint = NewOutHit.ImpactPoint;
						ImpactNormal = NewOutHit.ImpactNormal;
					}


					//if (LocalLineTraceSingleByChannel(LocalHitPoint, LocalHitNormal, HitObjType, TraceStart, TraceEnd))
					//{
					//	bBlockingHit = true;
					//	ImpactPoint = LocalHitPoint;
					//	ImpactNormal = LocalHitNormal;
					//}
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////

		FVector TargetLocation, TempLoc;
		float Offset = 0.f;
		//计算Offset
		if (Actors.Num() == 1)
		{
			AArmySpotLightActor* TempLightActor = Cast<AArmySpotLightActor>(Actors[0]);
			AArmyPointLightActor* TempLightActor2 = Cast<AArmyPointLightActor>(Actors[0]);
			AArmyReflectionCaptureActor* TempRCActor = Cast<AArmyReflectionCaptureActor>(Actors[0]);
			AArmyLevelAssetBoardActor* TempLAActor = Cast<AArmyLevelAssetBoardActor>(Actors[0]);
			AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Actors[0]);
			if (TempLightActor)
				Offset = 10.f;
			else if (TempLightActor2)
				Offset = 40.f;
			else if (TempRCActor)
				Offset = 40.f;
			else if (TempLAActor)
				Offset = 40.f;
			//else// if (SMA)
			//	Offset = 1.f;
		}
		//计算TargetLocation
		if (bBlockingHit)
		{
			if (Actors.Num() == 1)
				TargetLocation = ImpactPoint + ImpactNormal * Offset;
			else
			{
				TargetLocation = ImpactPoint;
				//如果是法线朝上，则Z值太高1个单位
				float DotValue = FVector::DotProduct(ImpactNormal, FVector(0, 0, 1));
				if (DotValue > 0.9f)
				{
					TargetLocation.Z += 1.f;
				}
			}
		}
		else
		{
			TargetLocation = TraceStart + WorldDirection * 300.f;
			if (TargetLocation.Z < 0)
				TargetLocation.Z = 0.f;
		}

		//设置Location
		if (Actors.Num() == 1)
		{
			if (GetViewportType() == EArmyLevelViewportType::LVT_OrthoXY)
			{
				//UpdateAutoRotation(Actors[0],Actors,SelectActorsBox,TargetLocation-Actors[0]->GetActorLocation());
				bool bTempAds = false;// IsAdsorbDetect2D(Actors[0],Actors,SelectActorsBox,TargetLocation,TargetLocation);
				if (AdsorbDetect.IsBound())
				{
					HAdsorbDetect adsInfo = AdsorbDetect.Execute(TargetLocation);
					bTempAds = adsInfo.bAdsorbDetect;
					if (bTempAds)
					{
						TargetLocation = adsInfo.AdsPoint;
						ImpactNormal = adsInfo.WallNormal;
					}
				}
				if (AutoRotation && !((ImpactNormal.GetSafeNormal() - FVector(0, 0, 1)).Size() < 0.001 || (ImpactNormal.GetSafeNormal() + FVector(0, 0, 1)).Size() < 0.001))
				{
					Actors[0]->SetActorRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), ImpactNormal));
				}
				Actors[0]->SetActorLocation(TargetLocation);
			}
			else
			{
				if (!IsAdsorbDetect(Actors[0], Actors, SelectActorsBox, TargetLocation - Actors[0]->GetActorLocation()))
				{
					Actors[0]->SetActorLocation(TargetLocation);
					if (AutoRotation && !((ImpactNormal.GetSafeNormal() - FVector(0, 0, 1)).Size() < 0.001 || (ImpactNormal.GetSafeNormal() + FVector(0, 0, 1)).Size() < 0.001))
					{
						Actors[0]->SetActorRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), ImpactNormal));
					}
				}
				else
				{
					if (GetViewportType() != EArmyLevelViewportType::LVT_OrthoXY)
						TargetLocation = Actors[0]->GetActorLocation();
				}
			}

		}
		else
		{
			//如果是法线朝上，则Z值太高1个单位
			float DotValue = FVector::DotProduct(ImpactNormal, FVector(0, 0, 1));
			for (auto& It : Actors)
			{
				FVector ActorOffset = It->GetActorLocation() - ModeTools->PivotLocation;
				FVector NewActorLocation = ImpactPoint + ActorOffset;
				if (DotValue > 0.9f)
				{
					NewActorLocation.Z += 1.f;
				}
				It->SetActorLocation(NewActorLocation);
			}
		}
		ProjectActorsIntoWorldDelegate.ExecuteIfBound(Actors);
		//设置GourpActor位置
		if (GroupActor)
		{
			GroupActor->SetActorLocation(TargetLocation);
		}
		ModeTools->SetPivotLocation(TargetLocation, false);

		//if (OutHit.bBlockingHit)
		//{
		//	float Offset = 0.f;
		//	FVector TargetLocation, TempLoc;
		//	//灯类物体要偏移40个单位
		//	if (Actors.Num() == 1)
		//	{
		//		AArmySpotLightActor* TempLightActor = Cast<AArmySpotLightActor>(Actors[0]);
		//		AArmyPointLightActor* TempLightActor2 = Cast<AArmyPointLightActor>(Actors[0]);
		//		AArmyReflectionCaptureActor* TempRCActor = Cast<AArmyReflectionCaptureActor>(Actors[0]);
		//		AArmyLevelAssetBoardActor* TempLAActor = Cast<AArmyLevelAssetBoardActor>(Actors[0]);
		//		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Actors[0]);
		//		if (TempLightActor)
		//		{
		//			Offset = 10.f;
		//		}
		//		else if (TempLightActor2)
		//		{
		//			Offset = 40.f;
		//		}
		//		else if (TempRCActor)
		//		{
		//			Offset = 40.f;
		//		}
		//		else if (TempLAActor)
		//		{
		//			Offset = 40.f;
		//		}
		//		else// if (SMA)
		//		{
		//			Offset = 1.f;
		//		}
		//		TargetLocation = OutHit.ImpactPoint + OutHit.ImpactNormal * Offset;
		//		Actors[0]->SetActorLocation(TargetLocation);
		//	}
		//	else
		//	{
		//		TargetLocation = OutHit.ImpactPoint;
		//		//如果是法线朝上，则Z值太高1个单位
		//		float DotValue = FVector::DotProduct(OutHit.ImpactNormal, FVector(0, 0, 1));
		//		if (DotValue > 0.9f)
		//		{
		//			TargetLocation.Z += 1.f;
		//		}

		//		for (auto& It : Actors)
		//		{
		//			FVector ActorOffset = It->GetActorLocation() - ModeTools->PivotLocation;
		//			FVector NewActorLocation = OutHit.ImpactPoint + ActorOffset;
		//			if (DotValue > 0.9f)
		//			{
		//				NewActorLocation.Z += 1.f;
		//			}
		//			It->SetActorLocation(NewActorLocation);
		//		}
		//	}
		//	if (GroupActor)
		//	{
		//		GroupActor->SetActorLocation(TargetLocation);
		//	}
		//	ModeTools->SetPivotLocation(TargetLocation, false);
		//}
		//else
		//{
		//	//如果
		//	if (GetViewportType() == EArmyLevelViewportType::LVT_OrthoXY)
		//	{
		//		WorldOriginOrtho.Z = 0.f;
		//		for (auto& It : Actors)
		//		{
		//			FVector ActorOffset = It->GetActorLocation() - ModeTools->PivotLocation;
		//			FVector NewActorLocation = WorldOriginOrtho + ActorOffset;
		//			It->SetActorLocation(NewActorLocation);
		//		}
		//		ModeTools->SetPivotLocation(WorldOriginOrtho, false);
		//	}
		//}
		//回调Actor被修改。用来刷新UI
		if (Actors.Num() > 0)
		{
			GArmyEditor->SelectionEditedEvent.ExecuteIfBound(Actors[0]);
		}
	}
}

void UArmyEditorViewportClient::ModifyScale(AActor* InActor, FVector& ScaleDelta, bool bCheckSmallExtent /*= false*/) const
{
	if (InActor->GetRootComponent())
	{
		const FVector CurrentScale = InActor->GetRootComponent()->RelativeScale3D;

		const FBox LocalBox = InActor->GetComponentsBoundingBox(true);
		const FVector ScaledExtents = LocalBox.GetExtent() * CurrentScale;
		ValidateScale(CurrentScale, ScaledExtents, ScaleDelta, bCheckSmallExtent);

		if (ScaleDelta.IsNearlyZero())
		{
			ScaleDelta = FVector::ZeroVector;
		}
	}
}

/** Helper function for ModifyScale - Convert the active Dragging Axis to per-axis flags */
static void CheckActiveAxes(EAxisList::Type DraggingAxis, bool bActiveAxes[3])
{
	bActiveAxes[0] = bActiveAxes[1] = bActiveAxes[2] = false;
	switch (DraggingAxis)
	{
	default:
	case EAxisList::None:
		break;
	case EAxisList::X:
		bActiveAxes[0] = true;
		break;
	case EAxisList::Y:
		bActiveAxes[1] = true;
		break;
	case EAxisList::Z:
		bActiveAxes[2] = true;
		break;
	case EAxisList::XYZ:
	case EAxisList::All:
	case EAxisList::Screen:
		bActiveAxes[0] = bActiveAxes[1] = bActiveAxes[2] = true;
		break;
	case EAxisList::XY:
		bActiveAxes[0] = bActiveAxes[1] = true;
		break;
	case EAxisList::XZ:
		bActiveAxes[0] = bActiveAxes[2] = true;
		break;
	case EAxisList::YZ:
		bActiveAxes[1] = bActiveAxes[2] = true;
		break;
	}
}

/** Helper function for ModifyScale - Check scale criteria to see if this is allowed, returns modified absolute scale*/
static float CheckScaleValue(float ScaleDeltaToCheck, float CurrentScaleFactor, float CurrentExtent, bool bCheckSmallExtent, bool bSnap)
{
	float AbsoluteScaleValue = ScaleDeltaToCheck + CurrentScaleFactor;
	if (bSnap)
	{
		AbsoluteScaleValue = FMath::GridSnap(AbsoluteScaleValue, GSNAPSCALE/*GEditor->GetScaleGridSize()*/);
	}
	// In some situations CurrentExtent can be 0 (eg: when scaling a plane in Z), this causes a divide by 0 that we need to avoid.
	if (CurrentExtent < KINDA_SMALL_NUMBER) {
		return AbsoluteScaleValue;
	}
	float UnscaledExtent = CurrentExtent / CurrentScaleFactor;
	float ScaledExtent = UnscaledExtent * AbsoluteScaleValue;

	if ((FMath::Square(ScaledExtent)) > BIG_NUMBER)	// cant get too big...
	{
		return CurrentScaleFactor;
	}
	else if (bCheckSmallExtent &&
		(FMath::Abs(ScaledExtent) < MIN_ACTOR_BOUNDS_EXTENT * 0.5f ||		// ...or too small (apply sign in this case)...
		(CurrentScaleFactor < 0.0f) != (AbsoluteScaleValue < 0.0f)))	// ...also cant cross the zero boundary
	{
		return ((MIN_ACTOR_BOUNDS_EXTENT * 0.5) / UnscaledExtent) * (CurrentScaleFactor < 0.0f ? -1.0f : 1.0f);
	}

	return AbsoluteScaleValue;
}

/**
* Helper function for ValidateScale().
* If the setting is enabled, this function will appropriately re-scale the scale delta so that
* proportions are preserved when snapping.
* @param	CurrentScale	The object's current scale
* @param	bActiveAxes		The axes that are active when scaling interactively.
* @param	InOutScaleDelta	The scale delta we are potentially transforming.
* @return true if the axes should be snapped individually, according to the snap setting (i.e. this function had no effect)
*/
static bool OptionallyPreserveNonUniformScale(const FVector& InCurrentScale, const bool bActiveAxes[3], FVector& InOutScaleDelta)
{
	// 	const ULevelEditorViewportSettings* ViewportSettings = GetDefault<ULevelEditorViewportSettings>();
	// 
	// 	if (ViewportSettings->SnapScaleEnabled && ViewportSettings->PreserveNonUniformScale)
	// 	{
	// 		// when using 'auto-precision', we take the max component & snap its scale, then proportionally scale the other components
	// 		float MaxComponentSum = -1.0f;
	// 		int32 MaxAxisIndex = -1;
	// 		for (int Axis = 0; Axis < 3; ++Axis)
	// 		{
	// 			if (bActiveAxes[Axis])
	// 			{
	// 				const float AbsScale = FMath::Abs(InOutScaleDelta[Axis] + InCurrentScale[Axis]);
	// 				if (AbsScale > MaxComponentSum)
	// 				{
	// 					MaxAxisIndex = Axis;
	// 					MaxComponentSum = AbsScale;
	// 				}
	// 			}
	// 		}
	// 
	// 		check(MaxAxisIndex != -1);
	// 
	// 		float AbsoluteScaleValue = FMath::GridSnap(InCurrentScale[MaxAxisIndex] + InOutScaleDelta[MaxAxisIndex], GEditor->GetScaleGridSize());
	// 		float ScaleRatioMax = InCurrentScale[MaxAxisIndex] == 0.0f ? 1.0f : AbsoluteScaleValue / InCurrentScale[MaxAxisIndex];
	// 		for (int Axis = 0; Axis < 3; ++Axis)
	// 		{
	// 			if (bActiveAxes[Axis])
	// 			{
	// 				if (Axis == MaxAxisIndex)
	// 				{
	// 					InOutScaleDelta[Axis] = AbsoluteScaleValue - InCurrentScale[Axis];
	// 				}
	// 				else
	// 				{
	// 					InOutScaleDelta[Axis] = (InCurrentScale[Axis] * ScaleRatioMax) - InCurrentScale[Axis];
	// 				}
	// 			}
	// 		}
	// 
	// 		return false;
	// 	}
	// 
	// 	return ViewportSettings->SnapScaleEnabled;
	return true;
}

void UArmyEditorViewportClient::ValidateScale(const FVector& InCurrentScale, const FVector& InBoxExtent, FVector& InOutScaleDelta, bool bInCheckSmallExtent /*= false*/) const
{
	// get the axes that are active in this operation
	bool bActiveAxes[3];
	CheckActiveAxes(Widget != NULL ? Widget->GetCurrentAxis() : EAxisList::None, bActiveAxes);

	bool bSnapAxes = OptionallyPreserveNonUniformScale(InCurrentScale, bActiveAxes, InOutScaleDelta);

	// check each axis
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		if (bActiveAxes[Axis])
		{
			float ModifiedScaleAbsolute = CheckScaleValue(InOutScaleDelta[Axis], InCurrentScale[Axis], InBoxExtent[Axis], bInCheckSmallExtent, bSnapAxes);
			InOutScaleDelta[Axis] = ModifiedScaleAbsolute - InCurrentScale[Axis];
		}
		else
		{
			InOutScaleDelta[Axis] = 0.0f;
		}
	}
}

void UArmyEditorViewportClient::TrackingStarted(const struct FArmyInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge)
{
	// Begin transacting.  Give the current editor mode an opportunity to do the transacting.
	const bool bTrackingHandledExternally = ModeTools->StartTracking(this, Viewport);

	//TrackingTransaction.End();

	// Re-initialize new tracking only if a new button was pressed, otherwise we continue the previous one.
	if (InInputState.GetInputEvent() == IE_Pressed)
	{
		EInputEvent Event = InInputState.GetInputEvent();
		FKey Key = InInputState.GetKey();

		if (InInputState.IsShiftButtonPressed() && bDraggingByHandle)
		{
			if (Event == IE_Pressed && (Key == EKeys::LeftMouseButton || Key == EKeys::RightMouseButton) && !bDuplicateActorsInProgress)
			{
				// Set the flag so that the actors actors will be duplicated as soon as the widget is displaced.
				bDuplicateOnNextDrag = true;
				bDuplicateActorsInProgress = true;
			}
		}
		else
		{
			bDuplicateOnNextDrag = false;
		}
	}

	bOnlyMovedPivot = false;

	const bool bIsDraggingComponents = GArmyEditor->GetSelectedComponentCount() > 0;
	//PreDragActorTransforms.Empty();
	if (bIsDraggingComponents)
	{
		if (bIsDraggingWidget)
		{
			//			Widget->SetSnapEnabled(true);
		}
	}
	else
	{
		for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It && !bIsTrackingBrushModification; ++It)
		{
			AActor* Actor = static_cast<AActor*>(*It);
			checkSlow(Actor->IsA(AActor::StaticClass()));

			if (bIsDraggingWidget)
			{
				// Notify that this actor is beginning to move
				//				GVRSEditor->BroadcastBeginObjectMovement(*Actor);
			}

			//			Widget->SetSnapEnabled(true);

			// See if any brushes are about to be transformed via their Widget
			TArray<AActor*> AttachedActors;
			Actor->GetAttachedActors(AttachedActors);
			const bool bExactClass = true;
			// First, check for selected brush actors, check the actors attached actors for brush actors as well.  If a parent actor moves, the bsp needs to be rebuilt
			ABrush* Brush = Cast< ABrush >(Actor);
			//if (Brush && (!Brush->IsVolumeBrush() && !FActorEditorUtils::IsABuilderBrush(Actor)))
			//{
			//	bIsTrackingBrushModification = true;
			//}
			//else // Next, check for selected groups actors that contain brushes
			//{
			//AGroupActor* GroupActor = Cast<AGroupActor>(Actor);
			//if (GroupActor)
			//{
			//	TArray<AActor*> GroupMembers;
			//	GroupActor->GetAllChildren(GroupMembers, true);
			//	for (int32 GroupMemberIdx = 0; GroupMemberIdx < GroupMembers.Num(); ++GroupMemberIdx)
			//	{
			//		Brush = Cast< ABrush >(GroupMembers[GroupMemberIdx]);
			//		if (Brush && (!Brush->IsVolumeBrush() && false))//!FActorEditorUtils::IsABuilderBrush(Actor)))
			//		{
			//			bIsTrackingBrushModification = true;
			//		}
			//	}
			//}
			//}
		}
	}

	// Start a transformation transaction if required
	//if (!bTrackingHandledExternally)
	{
		if (bIsDraggingWidget)
		{
			TArray<AActor*> TheActors;
			GArmyEditor->GetSelectedActors(TheActors);
			if (TheActors.Num() > 0)
			{
				GArmyEditor->OnActorsOperation.ExecuteIfBound(TheActors, 0);
			}

			//TrackingTransaction.TransCount++;

			FText ObjectTypeBeingTracked;// = bIsDraggingComponents ? LOCTEXT("TransactionFocus_Components", "Components") : LOCTEXT("TransactionFocus_Actors", "Actors");
			FText TrackingDescription;

			switch (GetWidgetMode())
			{
			case FArmyWidget::WM_Translate:
				TrackingDescription = FText::Format(LOCTEXT("MoveTransaction", "Move {0}"), ObjectTypeBeingTracked);
				break;
			case FArmyWidget::WM_Rotate:
				TrackingDescription = FText::Format(LOCTEXT("RotateTransaction", "Rotate {0}"), ObjectTypeBeingTracked);
				break;
			case FArmyWidget::WM_Scale:
				TrackingDescription = FText::Format(LOCTEXT("ScaleTransaction", "Scale {0}"), ObjectTypeBeingTracked);
				break;
			case FArmyWidget::WM_TranslateRotateZ:
				TrackingDescription = FText::Format(LOCTEXT("TranslateRotateZTransaction", "Translate/RotateZ {0}"), ObjectTypeBeingTracked);
				break;
			case FArmyWidget::WM_2D:
				TrackingDescription = FText::Format(LOCTEXT("TranslateRotate2D", "Translate/Rotate2D {0}"), ObjectTypeBeingTracked);
				break;
			default:
				if (bNudge)
				{
					TrackingDescription = FText::Format(LOCTEXT("NudgeTransaction", "Nudge {0}"), ObjectTypeBeingTracked);
				}
			}

			if (!TrackingDescription.IsEmpty())
			{
				if (bNudge)
				{
					//TrackingTransaction.Begin(TrackingDescription);
				}
				else
				{
					// If this hasn't begun due to a nudge, start it as a pending transaction so that it only really begins when the mouse is moved
					//TrackingTransaction.BeginPending(TrackingDescription);
				}
			}

			//if (TrackingTransaction.IsActive() || TrackingTransaction.IsPending())
			//{
			// Suspend actor/component modification during each delta step to avoid recording unnecessary overhead into the transaction buffer
			//GEditor->DisableDeltaModification(true);
			//}
		}
	}
}

void UArmyEditorViewportClient::TrackingStopped()
{
	const bool AltDown = IsAltPressed();
	const bool ShiftDown = IsShiftPressed();
	const bool ControlDown = IsCtrlPressed();
	const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
	const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);
	const bool MiddleMouseButtonDown = Viewport->KeyState(EKeys::MiddleMouseButton);

	// Only disable the duplicate on next drag flag if we actually dragged the mouse.
	bDuplicateOnNextDrag = false;

	// here we check to see if anything of worth actually changed when ending our MouseMovement
	// If the TransCount > 0 (we changed something of value) so we need to call PostEditMove() on stuff
	// if we didn't change anything then don't call PostEditMove()
	bool bDidAnythingActuallyChange = false;

	// Stop transacting.  Give the current editor mode an opportunity to do the transacting.
	const bool bTransactingHandledByEditorMode = ModeTools->EndTracking(this, Viewport);
	/*if (!bTransactingHandledByEditorMode)
	{
	if (TrackingTransaction.TransCount > 0)
	{
	bDidAnythingActuallyChange = true;
	TrackingTransaction.TransCount--;
	}
	}*/

	// Finish tracking a brush transform and update the Bsp
	if (bIsTrackingBrushModification)
	{
		//bDidAnythingActuallyChange = HaveSelectedObjectsBeenChanged() && !bOnlyMovedPivot;

		bIsTrackingBrushModification = false;
		if (bDidAnythingActuallyChange && bWidgetAxisControlledByDrag)
		{
			//GEditor->RebuildAlteredBSP();
		}
	}

	// Notify the selected actors that they have been moved.
	// Don't do this if AddDelta was never called.
	TArray<AArmyGroupActor*> ActorGroups;
	if (/*bDidAnythingActuallyChange && */MouseDeltaTracker->HasReceivedDelta())
	{
		for (FSelectionIterator It(GArmyEditor->GetSelectedActorIterator()); It; ++It)
		{
			AActor* Actor = static_cast<AActor*>(*It);
			checkSlow(Actor->IsA(AActor::StaticClass()));

			// Verify that the actor is in the same world as the viewport before moving it.
			//if (GEditor->PlayWorld)
			//{
			//	if (bIsSimulateInEditorViewport)
			//	{
			//		// If the Actor's outer (level) outer (world) is not the PlayWorld then it cannot be moved in this viewport.
			//		if (!(GEditor->PlayWorld == Actor->GetOuter()->GetOuter()))
			//		{
			//			continue;
			//		}
			//	}
			//	else if (!(GEditor->EditorWorld == Actor->GetOuter()->GetOuter()))
			//	{
			//		continue;
			//	}
			//}
			AArmyGroupActor* ParentGroup = AArmyGroupActor::GetRootForActor(Actor, true, true);
			if (ParentGroup /*&& GVRSEditor->bGroupingActive*/)
			{
				ActorGroups.AddUnique(ParentGroup);
			}
			else
			{

				Actor->PostEditMove(true);
				GArmyEditor->BroadcastEndObjectMovement(*Actor);
			}
		}
		AArmyGroupActor::RemoveSubGroupsFromArray(ActorGroups);
		for (int32 ActorGroupsIndex = 0; ActorGroupsIndex < ActorGroups.Num(); ++ActorGroupsIndex)
		{
			ActorGroups[ActorGroupsIndex]->PostEditMove(true);
		}
		//	if (!GUnrealEd->IsPivotMovedIndependently())
		//	{
		//		GUnrealEd->UpdatePivotLocationForSelection();
		//	}
	}

	//// End the transaction here if one was started in StartTransaction()
	//if (TrackingTransaction.IsActive() || TrackingTransaction.IsPending())
	//{
	//	if (!HaveSelectedObjectsBeenChanged())
	//	{
	//		TrackingTransaction.Cancel();
	//	}
	//	else
	//	{
	//		TrackingTransaction.End();
	//	}

	//	// Restore actor/component delta modification
	//	GEditor->DisableDeltaModification(false);
	//}

	//TArray<FEdMode*> ActiveModes;
	//ModeTools->GetActiveModes(ActiveModes);
	//for (int32 ModeIndex = 0; ModeIndex < ActiveModes.Num(); ++ModeIndex)
	//{
	//	// Also notify the current editing modes if they are interested.
	//	ActiveModes[ModeIndex]->ActorMoveNotify();
	//}

	//if (bDidAnythingActuallyChange)
	//{
	//	FScopedLevelDirtied LevelDirtyCallback;
	//	LevelDirtyCallback.Request();

	//	RedrawAllViewportsIntoThisScene();
	//}

	//PreDragActorTransforms.Empty();


}



bool UArmyEditorViewportClient::IsAdsorbDetect(AActor* InActor, const TArray<AActor*>& SelectActors, const FBox& ActorInBox, const FVector& InDeltaDrag)
{
	if (!bSnapTranslate && !bSnapTranslateTemp)
		return false;

	FVector Center, Extent;
	float DefaultRayRangeToBoxBorderInterval = 10.f;//默认的固定值30mm
	float ThresholdRayRangeToBoxBorderInterval = 5.f;//比例阈值50mm
	float ExtentScale = 0.1f;
	ActorInBox.GetCenterAndExtents(Center, Extent);
	float SubX = Extent.X >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.X;
	float SubY = Extent.Y >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.Y;
	float SubZ = Extent.Z >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.Z;

	Extent.X -= SubX;
	Extent.Y -= SubY;
	Extent.Z -= SubZ;

	// Extent = FVector(FMath::Abs(Extent.X), FMath::Abs(Extent.Y), FMath::Abs(Extent.Z));
	FVector PreActorLocation = InActor->GetActorLocation();
	FVector ActorToCenterVector = Center - InActor->GetActorLocation();//Actor到包围盒中心点的一个矢量

	FHitResult OutHit(ForceInit);
	bool bHit = false;

	static FName TraceTag = FName(TEXT("AdjustTrace"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.bTraceAsyncScene = true;
	AController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
		TraceParams.AddIgnoredActor(PlayerController->GetPawn());
	for (int i = 0; i < SelectActors.Num(); i++)
	{
		TraceParams.AddIgnoredActor(SelectActors[i]);
	}

	FVector TraceStart = Center;
	FVector TraceEnd;
	if (FMath::IsNearlyZero(InDeltaDrag.Size()))
	{
		return false;
	}

	if (GetViewportType() == EArmyLevelViewportType::LVT_OrthoXY)
	{
		//@刘克祥，添加2D模式下吸附墙体操作
		//UpdateAutoRotation(Actors[0],Actors,SelectActorsBox,TargetLocation-Actors[0]->GetActorLocation());
		bool bTempAds = false;// IsAdsorbDetect2D(Actors[0],Actors,SelectActorsBox,TargetLocation,TargetLocation);
		FVector TargetLocation = PreActorLocation;
		FVector ImpactNormal = FVector::ZeroVector;
		if (AdsorbDetect.IsBound())
		{
			HAdsorbDetect adsInfo = AdsorbDetect.Execute(PreActorLocation);
			bTempAds = adsInfo.bAdsorbDetect;
			if (bTempAds)
			{
				TargetLocation = adsInfo.AdsPoint;
				ImpactNormal = adsInfo.WallNormal;
			}
		}
		if (IsActorAutoRotation(InActor) && !((ImpactNormal.GetSafeNormal() - FVector(0, 0, 1)).Size() < 0.001 || (ImpactNormal.GetSafeNormal() + FVector(0, 0, 1)).Size() < 0.001))
		{
			InActor->SetActorRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), ImpactNormal));
		}
		InActor->SetActorLocation(TargetLocation);
		ModeTools->SetPivotLocation(TargetLocation, false);

	}
	else
	{
		FVector TestDelta(ForceInitToZero);
		InDeltaDrag.X > 0.001 ? TestDelta.X = 1 : (InDeltaDrag.X < -0.001 ? TestDelta.X = -1 : 0);
		InDeltaDrag.Y > 0.001 ? TestDelta.Y = 1 : (InDeltaDrag.Y < -0.001 ? TestDelta.Y = -1 : 0);
		InDeltaDrag.Z > 0.001 ? TestDelta.Z = 1 : (InDeltaDrag.Z < -0.001 ? TestDelta.Z = -1 : 0);

		TraceEnd = TraceStart - TestDelta * DefaultRayRangeToBoxBorderInterval;
		bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
			FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
			FCollisionShape::MakeBox(Extent), TraceParams);
		if (bHit)
		{
			TraceEnd = TraceStart + TestDelta * DefaultRayRangeToBoxBorderInterval * 2;
			bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
				FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
				FCollisionShape::MakeBox(Extent), TraceParams);
			if (!bHit && (MouseDeltaTracker->GetRawDelta() * TestDelta).Size() > (TestDelta * DefaultRayRangeToBoxBorderInterval * 2).Size())
			{
				return false;
			}
			else
			{
				ModeTools->SetPivotLocation(PreActorLocation, false);
				return true;
			}
		}
		else
		{
			TraceEnd = TraceStart + TestDelta * DefaultRayRangeToBoxBorderInterval * 2;
			bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
				FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
				FCollisionShape::MakeBox(Extent), TraceParams);

			if (bHit)
			{
				FVector MoveDir = (TraceEnd - TraceStart).GetSafeNormal();
				FVector LastPos = PreActorLocation + MoveDir * (OutHit.Distance - (TestDelta * FVector(SubX, SubY, SubZ)).Size());

				FVector Normal = OutHit.ImpactNormal;
				float TempAngle = FQuat::FindBetweenNormals(MoveDir, OutHit.Normal.GetSafeNormal()).GetAngle();
				if (TempAngle > (PI / 2))
				{
					InActor->SetActorLocation(LastPos);
					ModeTools->SetPivotLocation(LastPos, false);
					return true;
				}
				return false;
			}
		}
	}

	//if (Widget->GetCurrentAxis() == EAxisList::X)
	//{
	//    //首先计算射线范围边缘到碰撞盒边缘的间隙比例
	//    if (Extent.Y > Extent.Z)
	//    {
	//        if (Extent.Y * ExtentScale < ThresholdRayRangeToBoxBorderInterval)
	//            DefaultRayRangeToBoxBorderInterval = Extent.Y * ExtentScale;
	//    }
	//    else
	//    {
	//        if (Extent.Z * ExtentScale < ThresholdRayRangeToBoxBorderInterval)
	//            DefaultRayRangeToBoxBorderInterval = Extent.Z * ExtentScale;
	//    }

	//    if (FMath::IsNearlyZero(InDeltaDrag.X))
	//    {
	//        return false;
	//    }
	//    else if (InDeltaDrag.X > 0.f)
	//    {
	//        //首先判断该物体有没有沿着X轴负方向吸附
	//        TraceEnd = TraceStart + FVector(-1.f, 0.f, 0.f) * (Extent.X + AdsorbDistance);
	//        bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//            FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//            FCollisionShape::MakeBox(FVector(0.f, Extent.Y - DefaultRayRangeToBoxBorderInterval, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);

	//        if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//            && FMath::IsNearlyEqual(FVector::DotProduct(FVector(-1, 0, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//        {
	//            FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(-1, 0, 0)*Extent.X;
	//            FVector HitPoint(OutHit.Location.X, CurrentAxisToBoxBorder.Y, CurrentAxisToBoxBorder.Z);
	//            if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//            {
	//                InActor->SetActorLocation(FVector(CurrentAxisToBoxBorder.X + Extent.X - ActorToCenterVector.X, InActor->GetActorLocation().Y, InActor->GetActorLocation().Z));
	//                return false;
	//            }

	//            return true;
	//        }
	//        else
	//        {
	//            TraceEnd = TraceStart + FVector(1.f, 0.f, 0.f) * (Extent.X + AdsorbDistance);
	//            bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//                FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//                FCollisionShape::MakeBox(FVector(0.f, Extent.Y - DefaultRayRangeToBoxBorderInterval, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);

	//            //有碰撞，碰撞到的Actor不在被选中的Actor集合中，同时法线平行且相反
	//            if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//                && FMath::IsNearlyEqual(FVector::DotProduct(FVector(1, 0, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//            {
	//                InActor->SetActorLocation(FVector(OutHit.Location.X - Extent.X - ActorToCenterVector.X, InActor->GetActorLocation().Y, InActor->GetActorLocation().Z));

	//                FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(1, 0, 0)*Extent.X;
	//                FVector HitPoint(OutHit.Location.X, CurrentAxisToBoxBorder.Y, CurrentAxisToBoxBorder.Z);
	//                if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//                {
	//                    InActor->SetActorLocation(FVector(CurrentAxisToBoxBorder.X - Extent.X - ActorToCenterVector.X, InActor->GetActorLocation().Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(1, 0, 0)*Extent.X*/);
	//                    return false;
	//                }

	//                return true;
	//            }
	//        }
	//    }
	//    else if (InDeltaDrag.X < 0.f)
	//    {
	//        //首先判断该物体有没有沿着X轴正方向吸附
	//        TraceEnd = TraceStart + FVector(1.f, 0.f, 0.f) * (Extent.X + AdsorbDistance);
	//        bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//            FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//            FCollisionShape::MakeBox(FVector(0.f, Extent.Y - DefaultRayRangeToBoxBorderInterval, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);
	//        if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//            && FMath::IsNearlyEqual(FVector::DotProduct(FVector(1, 0, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//        {
	//            FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(1, 0, 0)*Extent.X;
	//            FVector HitPoint(OutHit.Location.X, CurrentAxisToBoxBorder.Y, CurrentAxisToBoxBorder.Z);
	//            if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//            {
	//                InActor->SetActorLocation(FVector(CurrentAxisToBoxBorder.X - Extent.X - ActorToCenterVector.X, InActor->GetActorLocation().Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(1, 0, 0)*Extent.X*/);
	//                return false;
	//            }

	//            return true;
	//        }
	//        else
	//        {
	//            TraceEnd = TraceStart + FVector(-1.f, 0.f, 0.f) * (Extent.X + AdsorbDistance);
	//            bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//                FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//                FCollisionShape::MakeBox(FVector(0.f, Extent.Y - DefaultRayRangeToBoxBorderInterval, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);

	//            if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//                && FMath::IsNearlyEqual(FVector::DotProduct(FVector(-1, 0, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//            {
	//                InActor->SetActorLocation(FVector(OutHit.Location.X + Extent.X - ActorToCenterVector.X, InActor->GetActorLocation().Y, InActor->GetActorLocation().Z));

	//                FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(-1, 0, 0)*Extent.X;
	//                FVector HitPoint(OutHit.Location.X, CurrentAxisToBoxBorder.Y, CurrentAxisToBoxBorder.Z);
	//                if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//                {
	//                    InActor->SetActorLocation(FVector(CurrentAxisToBoxBorder.X + Extent.X - ActorToCenterVector.X, InActor->GetActorLocation().Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(-1, 0, 0)*Extent.X*/);
	//                    return false;
	//                }

	//                return true;
	//            }
	//        }
	//    }
	//}
	//else if (Widget->GetCurrentAxis() == EAxisList::Y)
	//{
	//    //首先计算射线范围边缘到碰撞盒边缘的间隙比例
	//    if (Extent.X > Extent.Z)
	//    {
	//        if (Extent.X * ExtentScale < ThresholdRayRangeToBoxBorderInterval)
	//            DefaultRayRangeToBoxBorderInterval = Extent.X * ExtentScale;
	//    }
	//    else
	//    {
	//        if (Extent.Z * ExtentScale < ThresholdRayRangeToBoxBorderInterval)
	//            DefaultRayRangeToBoxBorderInterval = Extent.Z * ExtentScale;
	//    }

	//    if (FMath::IsNearlyZero(InDeltaDrag.Y))
	//    {
	//        return false;
	//    }
	//    else if (InDeltaDrag.Y > 0.f)
	//    {
	//        //首先判断该物体有没有沿着Y轴负方向吸附
	//        TraceEnd = TraceStart + FVector(0.f, -1.f, 0.f) * (Extent.Y + AdsorbDistance);
	//        bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//            FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//            FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, 0.f, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);

	//        if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//            && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0, -1.f, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//        {
	//            FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0, -1.f, 0) * Extent.Y;
	//            FVector HitPoint(CurrentAxisToBoxBorder.X, OutHit.Location.Y, CurrentAxisToBoxBorder.Z);
	//            if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, CurrentAxisToBoxBorder.Y + Extent.Y - ActorToCenterVector.Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(0, -1.f, 0)*Extent.Y*/);
	//                return false;
	//            }

	//            return true;
	//        }
	//        else
	//        {
	//            TraceEnd = TraceStart + FVector(0.f, 1.f, 0.f) * (Extent.Y + AdsorbDistance);
	//            bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//                FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//                FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, 0.f, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);

	//            //有碰撞，同时法线平行且相反
	//            if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//                && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0, 1, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, OutHit.Location.Y - Extent.Y - ActorToCenterVector.Y, InActor->GetActorLocation().Z));

	//                FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0, 1, 0)*Extent.Y;
	//                FVector HitPoint(CurrentAxisToBoxBorder.X, OutHit.Location.Y, CurrentAxisToBoxBorder.Z);
	//                if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//                {
	//                    InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, CurrentAxisToBoxBorder.Y - Extent.Y - ActorToCenterVector.Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(0, 1, 0)*Extent.Y*/);
	//                    return false;
	//                }

	//                return true;
	//            }
	//        }
	//    }
	//    else if (InDeltaDrag.Y < 0.f)
	//    {
	//        //首先判断该物体有没有沿着Y轴正方向吸附
	//        TraceEnd = TraceStart + FVector(0.f, 1.f, 0.f) * (Extent.Y + AdsorbDistance);
	//        bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//            FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//            FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, 0.f, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);
	//        if (bHit /*&& (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//            && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0, 1, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//        {
	//            FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0, 1, 0)*Extent.Y;
	//            FVector HitPoint(CurrentAxisToBoxBorder.X, OutHit.Location.Y, CurrentAxisToBoxBorder.Z);
	//            if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, CurrentAxisToBoxBorder.Y - Extent.Y - ActorToCenterVector.Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(0, 1, 0) * Extent.Y*/);
	//                return false;
	//            }

	//            return true;
	//        }
	//        else
	//        {
	//            TraceEnd = TraceStart + FVector(0.f, -1.f, 0.f) * (Extent.Y + AdsorbDistance);
	//            bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//                FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//                FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, 0.f, Extent.Z - DefaultRayRangeToBoxBorderInterval)), TraceParams);

	//            if (bHit /*&& (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//                && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0.f, -1.f, 0.f), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, OutHit.Location.Y + Extent.Y - ActorToCenterVector.Y, InActor->GetActorLocation().Z));

	//                FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0.f, -1.f, 0.f)*Extent.Y;
	//                FVector HitPoint(CurrentAxisToBoxBorder.X, OutHit.Location.Y, CurrentAxisToBoxBorder.Z);
	//                if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//                {
	//                    InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, CurrentAxisToBoxBorder.Y + Extent.Y - ActorToCenterVector.Y, InActor->GetActorLocation().Z)/*CurrentWidgetLocation - FVector(0.f, -1.f, 0.f)*Extent.Y*/);
	//                    return false;
	//                }

	//                return true;
	//            }
	//        }
	//    }
	//}
	//else if (Widget->GetCurrentAxis() == EAxisList::Z)
	//{
	//    //首先计算射线范围边缘到碰撞盒边缘的间隙比例
	//    if (Extent.X > Extent.Y)
	//    {
	//        if (Extent.X * ExtentScale < ThresholdRayRangeToBoxBorderInterval)
	//            DefaultRayRangeToBoxBorderInterval = Extent.X * ExtentScale;
	//    }
	//    else
	//    {
	//        if (Extent.Y * ExtentScale < ThresholdRayRangeToBoxBorderInterval)
	//            DefaultRayRangeToBoxBorderInterval = Extent.Y * ExtentScale;
	//    }

	//    if (FMath::IsNearlyZero(InDeltaDrag.Z))
	//    {
	//        return false;
	//    }
	//    else if (InDeltaDrag.Z > 0.f)
	//    {
	//        //首先判断该物体有没有沿着Y轴负方向吸附
	//        TraceEnd = TraceStart + FVector(0.f, 0.f, -1.f) * (Extent.Z + AdsorbDistance);
	//        bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//            FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//            FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, Extent.Y - DefaultRayRangeToBoxBorderInterval, 0.f)), TraceParams);

	//        if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//            && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0, -1.f, 0), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//        {
	//            FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0, 0, -1.f) * Extent.Z;
	//            FVector HitPoint(CurrentAxisToBoxBorder.X, CurrentAxisToBoxBorder.Y, OutHit.Location.Z);
	//            if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, InActor->GetActorLocation().Y, CurrentAxisToBoxBorder.Z + Extent.Z - ActorToCenterVector.Z));
	//                return false;
	//            }

	//            return true;
	//        }
	//        else
	//        {
	//            TraceEnd = TraceStart + FVector(0.f, 0.f, 1.f) * (Extent.Z + AdsorbDistance);
	//            bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//                FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//                FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, Extent.Y - DefaultRayRangeToBoxBorderInterval, 0.f)), TraceParams);

	//            //有碰撞，同时法线平行且相反
	//            if (bHit/* && (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//                && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0, 0, 1), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, InActor->GetActorLocation().Y, OutHit.Location.Z - Extent.Z - ActorToCenterVector.Z));

	//                FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0, 0, 1)*Extent.Z;
	//                FVector HitPoint(CurrentAxisToBoxBorder.X, CurrentAxisToBoxBorder.Y, OutHit.Location.Z);
	//                if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//                {
	//                    InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, InActor->GetActorLocation().Y, CurrentAxisToBoxBorder.Z - Extent.Z - ActorToCenterVector.Z)/*CurrentWidgetLocation - FVector(0, 1, 0)*Extent.Y*/);
	//                    return false;
	//                }

	//                return true;
	//            }
	//        }
	//    }
	//    else if (InDeltaDrag.Z < 0.f)
	//    {
	//        //首先判断该物体有没有沿着Y轴正方向吸附
	//        TraceEnd = TraceStart + FVector(0.f, 0.f, 1.f) * (Extent.Z + AdsorbDistance);
	//        bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//            FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//            FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, Extent.Y - DefaultRayRangeToBoxBorderInterval, 0.f)), TraceParams);
	//        if (bHit /*&& (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//            && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0, 0, 1), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//        {
	//            FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0, 0, 1)*Extent.Z;
	//            FVector HitPoint(CurrentAxisToBoxBorder.X, CurrentAxisToBoxBorder.Y, OutHit.Location.Z);
	//            if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, InActor->GetActorLocation().Y, CurrentAxisToBoxBorder.Z - Extent.Z - ActorToCenterVector.Z)/*CurrentWidgetLocation - FVector(0, 1, 0) * Extent.Y*/);
	//                return false;
	//            }

	//            return true;
	//        }
	//        else
	//        {
	//            TraceEnd = TraceStart + FVector(0.f, 0.f, -1.f) * (Extent.Z + AdsorbDistance);
	//            bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
	//                FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
	//                FCollisionShape::MakeBox(FVector(Extent.X - DefaultRayRangeToBoxBorderInterval, Extent.Y - DefaultRayRangeToBoxBorderInterval, 0.f)), TraceParams);

	//            if (bHit /*&& (SelectActors.Find(OutHit.Actor.Get()) == INDEX_NONE)*/
	//                && FMath::IsNearlyEqual(FVector::DotProduct(FVector(0.f, 0.f, -1.f), OutHit.Normal), -1.f, KINDA_SMALL_NUMBER))
	//            {
	//                InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, InActor->GetActorLocation().Y, OutHit.Location.Z + Extent.Z - ActorToCenterVector.Z));

	//                FVector CurrentAxisToBoxBorder = FlowAxisBoxOfSelectedActors.GetCenter() + FVector(0.f, 0.f, -1.f)*Extent.Z;
	//                FVector HitPoint(CurrentAxisToBoxBorder.X, CurrentAxisToBoxBorder.Y, OutHit.Location.Z);
	//                if ((CurrentAxisToBoxBorder - HitPoint).Size() > AdsorbDistance)
	//                {
	//                    InActor->SetActorLocation(FVector(InActor->GetActorLocation().X, InActor->GetActorLocation().Y, CurrentAxisToBoxBorder.Z + Extent.Z - ActorToCenterVector.Z));
	//                    return false;
	//                }

	//                return true;
	//            }
	//        }
	//    }
	//}

	return false;
}

bool UArmyEditorViewportClient::IsAdsorbDetect2D(AActor* InActor, const TArray<AActor*>& SelectActors, const FBox& ActorInBox, FVector MousePoint, FVector& OutLocation)
{

	/*if (!bSnapTranslate&&!bSnapTranslateTemp)
		return false;*/
	FVector Center, Extent;
	float DefaultRayRangeToBoxBorderInterval = 15.f;//默认的固定值30mm
	float ThresholdRayRangeToBoxBorderInterval = 5.f;//比例阈值50mm
	float ExtentScale = 0.1f;
	ActorInBox.GetCenterAndExtents(Center, Extent);
	float SubX = Extent.X >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.X;
	float SubY = Extent.Y >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.Y;
	float SubZ = Extent.Z >= DefaultRayRangeToBoxBorderInterval ? DefaultRayRangeToBoxBorderInterval : Extent.Z;

	Extent.X -= SubX;
	Extent.Y -= SubY;
	Extent.Z -= SubZ;

	// Extent = FVector(FMath::Abs(Extent.X), FMath::Abs(Extent.Y), FMath::Abs(Extent.Z));
	FVector PreActorLocation = InActor->GetActorLocation();
	FVector ActorToCenterVector = Center - InActor->GetActorLocation();//Actor到包围盒中心点的一个矢量

	FHitResult OutHit(ForceInit);
	bool bHit = false;

	static FName TraceTag = FName(TEXT("AdjustTrace"));
	FCollisionQueryParams TraceParams(TraceTag, false);
	TraceParams.bTraceAsyncScene = true;
	AController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
		TraceParams.AddIgnoredActor(PlayerController->GetPawn());
	for (int i = 0; i < SelectActors.Num(); i++)
	{
		TraceParams.AddIgnoredActor(SelectActors[i]);
	}
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->Tags.Num() > 0 && (ActorItr->Tags[0] == "MoveableMeshActor" || ActorItr->Tags[0] == "BlueprintVRSActor"))
		{
			TraceParams.AddIgnoredActor(*ActorItr);
		}
	}

	FVector TraceStart = MousePoint;
	FVector TraceEnd;
	bool bTempAdsorded = bAdsorbed;
	FVector TempPoint = MousePoint;
	{
		FVector Directiones[4] = { FVector::ForwardVector,-FVector::ForwardVector,FVector::RightVector,-FVector::RightVector };
		for (int32 i = 0; i < sizeof(Directiones) / sizeof(FVector); i++)
		{
			FVector TestDelta = Directiones[i];
			TraceEnd = TraceStart + TestDelta * DefaultRayRangeToBoxBorderInterval;
			bHit = GetWorld()->SweepSingleByChannel(OutHit, TraceStart, TraceEnd,
				FRotator(0, 0, 0).Quaternion(), ECC_Visibility,
				FCollisionShape::MakeBox(Extent), TraceParams);
			FHitResult OutHit1(ForceInit);
			bool hit2 = GetWorld()->LineTraceSingleByChannel(OutHit1, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
			if (hit2)
			{
				FVector MoveDir = (TraceEnd - TraceStart).GetSafeNormal();
				FVector LastPos = OutHit1.ImpactPoint;

				FVector Normal = OutHit1.ImpactNormal;
				InActor->SetActorRotation(FQuat::FindBetweenVectors(FVector(0, 1, 0), Normal));
				{
					TempPoint = LastPos;
					ModeTools->SetPivotLocation(LastPos, false);
					bTempAdsorded = true;
					break;
				}
				bTempAdsorded = false;
			}
			else
				bTempAdsorded = false;
		}
	}

	OutLocation = TempPoint;
	bAdsorbed = bTempAdsorded;
	return bTempAdsorded;
}

void UArmyEditorViewportClient::ApplyDeltaToActors(const FVector& InDrag, const FRotator& InRot, const FVector& InScale)
{
	if ((InDrag.IsZero() && InRot.IsZero() && InScale.IsZero()))
	{
		return;
	}

	FVector ModifiedScale = InScale;

	TArray<AArmyGroupActor*> ActorGroups;
	TArray<AActor*> SelectActors;
	FBox SelectActorsBox(ForceInit);
	GArmyEditor->GetSelectedActors(SelectActors);
	for (auto AIt : SelectActors)
	{
		SelectActorsBox += GetActorCollisionBox(AIt);
	}

	//SelectActorsBox = GArmyEditor->GetSelectedActorsBox();

	if (!bAdsorbed)
		FlowAxisBoxOfSelectedActors = SelectActorsBox;
	else
		FlowAxisBoxOfSelectedActors = FlowAxisBoxOfSelectedActors.MoveTo(FlowAxisBoxOfSelectedActors.GetCenter() + InDrag);

	// 	for (FSelectionIterator SelectedActorIt(GVRSEditor->GetSelectedActorIterator()); SelectedActorIt; ++SelectedActorIt)
	// 	{
	// 		AActor* Actor = static_cast<AActor*>(*SelectedActorIt);
	// 		checkSlow(Actor->IsA(AActor::StaticClass()));
	// 		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(Actor);
	// 		if (SMA)
	// 		{
	// 			FKAggregateGeom& agg = SMA->GetStaticMeshComponent()->GetBodySetup()->AggGeom;
	// 			SelectActorsBox += agg.CalcAABB(SMA->GetStaticMeshComponent()->ComponentToWorld);
	// 			SelectActors.Push(Actor);
	// 		}
	// 	}

	// Apply the deltas to any selected actors.
	for (FSelectionIterator SelectedActorIt(GArmyEditor->GetSelectedActorIterator()); SelectedActorIt; ++SelectedActorIt)
	{
		AActor* Actor = static_cast<AActor*>(*SelectedActorIt);
		checkSlow(Actor->IsA(AActor::StaticClass()));

		// Verify that the actor is in the same world as the viewport before moving it.
		// 		if (GEditor->PlayWorld)
		// 		{
		// 			if (bIsSimulateInEditorViewport)
		// 			{
		// 				// If the Actor's outer (level) outer (world) is not the PlayWorld then it cannot be moved in this viewport.
		// 				if (!(GEditor->PlayWorld == Actor->GetWorld()))
		// 				{
		// 					continue;
		// 				}
		// 			}
		// 			else if (!(GEditor->EditorWorld == Actor->GetWorld()))
		// 			{
		// 				continue;
		// 			}
		// 		}

		if (true)//!Actor->bLockLocation)
		{
			if (GArmyEditor->GetSelectedComponentCount() > 0)
			{
				// 				USelection* ComponentSelection = GEditor->GetSelectedComponents();
				// 
				// 				// Only move the parent-most component(s) that are selected 
				// 				// Otherwise, if both a parent and child are selected and the delta is applied to both, the child will actually move 2x delta
				// 				TInlineComponentArray<USceneComponent*> ComponentsToMove;
				// 				for (FSelectedEditableComponentIterator EditableComponentIt(GEditor->GetSelectedEditableComponentIterator()); EditableComponentIt; ++EditableComponentIt)
				// 				{
				// 					USceneComponent* SceneComponent = CastChecked<USceneComponent>(*EditableComponentIt);
				// 					if (SceneComponent)
				// 					{
				// 						USceneComponent* SelectedComponent = Cast<USceneComponent>(*EditableComponentIt);
				// 
				// 						// Check to see if any parent is selected
				// 						bool bParentAlsoSelected = false;
				// 						USceneComponent* Parent = SelectedComponent->GetAttachParent();
				// 						while (Parent != nullptr)
				// 						{
				// 							if (ComponentSelection->IsSelected(Parent))
				// 							{
				// 								bParentAlsoSelected = true;
				// 								break;
				// 							}
				// 
				// 							Parent = Parent->GetAttachParent();
				// 						}
				// 
				// 						// If no parent of this component is also in the selection set, move it!
				// 						if (!bParentAlsoSelected)
				// 						{
				// 							ComponentsToMove.Add(SelectedComponent);
				// 						}
				// 					}
				// 				}
				// 
				// 				// Now actually apply the delta to the appropriate component(s)
				// 				for (auto SceneComp : ComponentsToMove)
				// 				{
				// 					ApplyDeltaToComponent(SceneComp, InDrag, InRot, ModifiedScale);
				// 				}
			}
			else
			{
				AArmyGroupActor* ParentGroup = AArmyGroupActor::GetRootForActor(Actor, true, true);
				if (ParentGroup /*&& GVRSEditor->bGroupingActive*/)
				{
					ActorGroups.AddUnique(ParentGroup);
				}
				else
				{
					// Finally, verify that no actor in the parent hierarchy is also selected
					bool bHasParentInSelection = false;
					AActor* ParentActor = Actor->GetAttachParentActor();
					while (ParentActor != NULL && !bHasParentInSelection)
					{
						if (ParentActor->IsSelected())
						{
							bHasParentInSelection = true;
						}
						ParentActor = ParentActor->GetAttachParentActor();
					}
					if (!bHasParentInSelection)
					{
						//UpdateAutoRotation(Actor, SelectActors, SelectActorsBox, InDrag);
						if (!IsAdsorbDetect(Actor, SelectActors, SelectActorsBox, InDrag))
						{
							ApplyDeltaToActor(Actor, InDrag, InRot, ModifiedScale);
							bAdsorbed = false;
						}
						else
							bAdsorbed = true;
					}
				}
			}
		}
	}
	AArmyGroupActor::RemoveSubGroupsFromArray(ActorGroups);
	for (int32 ActorGroupsIndex = 0; ActorGroupsIndex < ActorGroups.Num(); ++ActorGroupsIndex)
	{
		ActorGroups[ActorGroupsIndex]->GroupApplyDelta(this, InDrag, InRot, ModifiedScale);
	}
}

void UArmyEditorViewportClient::ApplyDeltaToActor(AActor* InActor, const FVector& InDeltaDrag, const FRotator& InDeltaRot, const FVector& InDeltaScale)
{
	// If we are scaling, we may need to change the scaling factor a bit to properly align to the grid.

	FVector ModifiedDeltaScale = InDeltaScale;

	// we dont scale actors when we only have a very small scale change
	if (!InDeltaScale.IsNearlyZero())
	{
		if (true)
			//if (!GEditor->UsePercentageBasedScaling())
		{
			ModifyScale(InActor, ModifiedDeltaScale, false);
		}
	}
	else
	{
		ModifiedDeltaScale = FVector::ZeroVector;
	}

	GArmyEditor->ApplyDeltaToActor(
		InActor,
		true,
		&InDeltaDrag,
		&InDeltaRot,
		&ModifiedDeltaScale,
		IsAltPressed(),
		IsShiftPressed(),
		IsCtrlPressed());

	// Update the cameras from their locked actor (if any) only if the viewport is realtime enabled
	//UpdateLockedActorViewports(InActor, true);
}

void UArmyEditorViewportClient::ApplyDeltaToComponent(USceneComponent* InComponent, const FVector& InDeltaDrag, const FRotator& InDeltaRot, const FVector& InDeltaScale)
{
	int32 a = 0;
}

FBox UArmyEditorViewportClient::GetActorCollisionBox(AActor* _Actor) const
{
	FBox AggGeomBox(ForceInit);
	bool bFirst = true;
	AStaticMeshActor* SMA = Cast<AStaticMeshActor>(_Actor);

	bool bIsSkeletalMesh = false;
	FVector SkeletalBoxExtent = FVector::ZeroVector;
	UArmyResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	if (ResMgr)
	{
		FVRSObject* Obj = ResMgr->GetObjFromObjID(_Actor->GetObjID());
		if (Obj)
		{
			if (Obj->GetSkeletalMeshObject())
			{
				bIsSkeletalMesh = true;
				SkeletalBoxExtent = Obj->BoundExtent;
			}
		}
	}

	if (bIsSkeletalMesh)
	{
		FTransform Trans = _Actor->GetActorTransform();
		SkeletalBoxExtent *= Trans.GetScale3D();

		AggGeomBox += Trans.TransformPosition(-SkeletalBoxExtent);
		AggGeomBox +=Trans.TransformPosition(FVector(-SkeletalBoxExtent.X, SkeletalBoxExtent.Y, -SkeletalBoxExtent.Z));
		AggGeomBox +=Trans.TransformPosition(FVector(SkeletalBoxExtent.X, -SkeletalBoxExtent.Y, -SkeletalBoxExtent.Z));
		AggGeomBox +=Trans.TransformPosition(FVector(SkeletalBoxExtent.X, SkeletalBoxExtent.Y, -SkeletalBoxExtent.Z));
						  
		AggGeomBox +=Trans.TransformPosition(SkeletalBoxExtent);
		AggGeomBox +=Trans.TransformPosition(FVector(-SkeletalBoxExtent.X, SkeletalBoxExtent.Y, SkeletalBoxExtent.Z));
		AggGeomBox +=Trans.TransformPosition(FVector(SkeletalBoxExtent.X, -SkeletalBoxExtent.Y, SkeletalBoxExtent.Z));
		AggGeomBox +=Trans.TransformPosition(FVector(-SkeletalBoxExtent.X, -SkeletalBoxExtent.Y, SkeletalBoxExtent.Z));
	}
	else if (SMA)
	{
		UStaticMeshComponent* SMC = SMA->GetStaticMeshComponent();
		if (SMC->GetStaticMesh() && SMC->GetStaticMesh()->BodySetup)
		{
			AggGeomBox = SMC->GetStaticMesh()->BodySetup->AggGeom.CalcAABB(SMC->GetComponentToWorld());
		}
		else
		{
			AggGeomBox = SMA->GetComponentsBoundingBox();
		}
	}
	else
	{
		UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(_Actor->GetRootComponent());
		if (SMC)
		{
			if (SMC->GetStaticMesh()->BodySetup)
			{
				AggGeomBox = SMC->GetStaticMesh()->BodySetup->AggGeom.CalcAABB(SMC->GetComponentToWorld());
			}
		}

		TArray<USceneComponent*> ChildList;
		_Actor->GetRootComponent()->GetChildrenComponents(true, ChildList);
		for (int32 i = 0; i < ChildList.Num(); i++)
		{
			UStaticMeshComponent* SMC2 = Cast<UStaticMeshComponent>(ChildList[i]);
			if (SMC2 && SMC2->GetStaticMesh())
			{
				if (SMC2->GetStaticMesh()->BodySetup)
				{
					FBox TempBox = SMC2->GetStaticMesh()->BodySetup->AggGeom.CalcAABB(SMC2->GetComponentToWorld());
					if (bFirst)
					{
						bFirst = false;
						AggGeomBox = TempBox;
					}
					else
					{
						//找到新的最大点 最小点
						if (TempBox.Max.X > AggGeomBox.Max.X)
							AggGeomBox.Max.X = TempBox.Max.X;
						if (TempBox.Max.Y > AggGeomBox.Max.Y)
							AggGeomBox.Max.Y = TempBox.Max.Y;
						if (TempBox.Max.Z > AggGeomBox.Max.Z)
							AggGeomBox.Max.Z = TempBox.Max.Z;

						if (TempBox.Min.X < AggGeomBox.Min.X)
							AggGeomBox.Min.X = TempBox.Min.X;
						if (TempBox.Min.Y < AggGeomBox.Min.Y)
							AggGeomBox.Min.Y = TempBox.Min.Y;
						if (TempBox.Min.Z < AggGeomBox.Min.Z)
							AggGeomBox.Min.Z = TempBox.Min.Z;
					}
				}
			}
		}
	}

	return AggGeomBox;
}

#undef LOCTEXT_NAMESPACE 
