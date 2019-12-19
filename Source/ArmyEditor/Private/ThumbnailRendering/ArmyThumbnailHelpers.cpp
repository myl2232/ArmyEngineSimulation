#include "ArmyThumbnailHelpers.h"
#include "Components/DirectionalLightComponent.h"
#include "ArmyEditorEngine.h"
#include "ArmyThumbnailManager.h"
/*
***************************************************************
FThumbnailPreviewScene
***************************************************************
*/

FArmyThumbnailPreviewScene::FArmyThumbnailPreviewScene()
	: FArmyPreviewScene(ConstructionValues()
		.SetLightRotation(FRotator(304.736, 39.84, 0))
		.SetSkyBrightness(1.69f)
		.SetCreatePhysicsScene(false)
		.SetTransactional(false))
{
	// A background sky sphere
	UStaticMeshComponent* BackgroundComponent = NewObject<UStaticMeshComponent>();
	BackgroundComponent->SetStaticMesh(GArmyEditor->GetThumbnailManager()->EditorSkySphere);
	const float SkySphereScale = 2000.0f;
	const FTransform BackgroundTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(SkySphereScale));
	FArmyPreviewScene::AddComponent(BackgroundComponent, BackgroundTransform);

	// Adjust the default light
	DirectionalLight->Intensity = 0.2f;

	// Add additional lights
	UDirectionalLightComponent* DirectionalLight2 = NewObject<UDirectionalLightComponent>();
	DirectionalLight->Intensity = 5.0f;
	AddComponent(DirectionalLight2, FTransform(FRotator(-40, -144.678, 0)));

	UDirectionalLightComponent* DirectionalLight3 = NewObject<UDirectionalLightComponent>();
	DirectionalLight->Intensity = 1.0f;
	AddComponent(DirectionalLight3, FTransform(FRotator(299.235, 144.993, 0)));

	// Add an infinite plane
	const float FloorPlaneScale = 10000.f;
	const FTransform FloorPlaneTransform(FRotator(-90.f, 0, 0), FVector::ZeroVector, FVector(FloorPlaneScale));
	UStaticMeshComponent* FloorPlaneComponent = NewObject<UStaticMeshComponent>();
	FloorPlaneComponent->SetStaticMesh(GArmyEditor->GetThumbnailManager()->EditorPlane);
	FloorPlaneComponent->SetMaterial(0, GArmyEditor->GetThumbnailManager()->FloorPlaneMaterial);
	FArmyPreviewScene::AddComponent(FloorPlaneComponent, FloorPlaneTransform);
}

void FArmyThumbnailPreviewScene::GetView(FSceneViewFamily* ViewFamily, int32 X, int32 Y, uint32 SizeX, uint32 SizeY) const
{
	check(ViewFamily);

	FIntRect ViewRect(
		FMath::Max<int32>(X, 0),
		FMath::Max<int32>(Y, 0),
		FMath::Max<int32>(X + SizeX, 0),
		FMath::Max<int32>(Y + SizeY, 0));

	if (ViewRect.Width() > 0 && ViewRect.Height() > 0)
	{
		const float FOVDegrees = 30.f;
		const float HalfFOVRadians = FMath::DegreesToRadians<float>(FOVDegrees) * 0.5f;
		static_assert((int32)ERHIZBuffer::IsInverted != 0, "Check NearPlane and Projection Matrix");
		const float NearPlane = 1.0f;
		FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
			HalfFOVRadians,
			1.0f,
			1.0f,
			NearPlane
		);

		FVector Origin(0);
		float OrbitPitch = 0;
		float OrbitYaw = 0;
		float OrbitZoom = 0;
		GetViewMatrixParameters(FOVDegrees, Origin, OrbitPitch, OrbitYaw, OrbitZoom);

		// Ensure a minimum camera distance to prevent problems with really small objects
		const float MinCameraDistance = 48;
		OrbitZoom = FMath::Max<float>(MinCameraDistance, OrbitZoom);

		const FRotator RotationOffsetToViewCenter(0.f, 90.f, 0.f);
		FMatrix ViewRotationMatrix = FRotationMatrix(FRotator(0, OrbitYaw, 0)) *
			FRotationMatrix(FRotator(0, 0, OrbitPitch)) *
			FTranslationMatrix(FVector(0, OrbitZoom, 0)) *
			FInverseRotationMatrix(RotationOffsetToViewCenter);

		ViewRotationMatrix = ViewRotationMatrix * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));

		Origin -= ViewRotationMatrix.InverseTransformPosition(FVector::ZeroVector);
		ViewRotationMatrix = ViewRotationMatrix.RemoveTranslation();

		FSceneViewInitOptions ViewInitOptions;
		ViewInitOptions.ViewFamily = ViewFamily;
		ViewInitOptions.SetViewRectangle(ViewRect);
		ViewInitOptions.ViewOrigin = -Origin;
		ViewInitOptions.ViewRotationMatrix = ViewRotationMatrix;
		ViewInitOptions.ProjectionMatrix = ProjectionMatrix;
		ViewInitOptions.BackgroundColor = FLinearColor::Black;

		FSceneView* NewView = new FSceneView(ViewInitOptions);

		ViewFamily->Views.Add(NewView);

		NewView->StartFinalPostprocessSettings(ViewInitOptions.ViewOrigin);
		NewView->EndFinalPostprocessSettings(ViewInitOptions);

		//FFinalPostProcessSettings::FCubemapEntry& CubemapEntry = *new(NewView->FinalPostProcessSettings.ContributingCubemaps) FFinalPostProcessSettings::FCubemapEntry;
		//CubemapEntry.AmbientCubemap = GUnrealEd->GetThumbnailManager()->AmbientCubemap;
		//const float AmbientCubemapIntensity = 1.69;
		//CubemapEntry.AmbientCubemapTintMulScaleValue = FLinearColor::White * AmbientCubemapIntensity;

		// Tell the texture streaming system about this thumbnail view, so the textures will stream in as needed
		// NOTE: Sizes may not actually be in screen space depending on how the thumbnail ends up stretched by the UI.  Not a big deal though.
		// NOTE: Textures still take a little time to stream if the view has not been re-rendered recently, so they may briefly appear blurry while mips are prepared
		// NOTE: Content Browser only renders thumbnails for loaded assets, and only when the mouse is over the panel. They'll be frozen in their last state while the mouse cursor is not over the panel.  This is for performance reasons
		//IStreamingManager::Get().AddViewInformation(Origin, SizeX, SizeX / FMath::Tan(FOVDegrees));
	}
}

float FArmyThumbnailPreviewScene::GetBoundsZOffset(const FBoxSphereBounds& Bounds) const
{
	// Return half the height of the bounds plus one to avoid ZFighting with the floor plane
	return Bounds.BoxExtent.Z + 1;
}


/*
***************************************************************
FMaterialThumbnailScene
***************************************************************
*/

FArmyMaterialThumbnailScene::FArmyMaterialThumbnailScene()
	: FArmyThumbnailPreviewScene()
{
	bForceAllUsedMipsResident = false;

	// Create preview actor
	// checked
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.bNoFail = true;
	SpawnInfo.ObjectFlags = RF_Transient;
	PreviewActor = GetWorld()->SpawnActor<AStaticMeshActor>(SpawnInfo);

	PreviewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	PreviewActor->GetStaticMeshComponent()->bSelectable = false;
	PreviewActor->SetActorEnableCollision(false);
}

void FArmyMaterialThumbnailScene::SetMaterialInterface(UMaterialInterface* InMaterial)
{
	check(PreviewActor);
	check(PreviewActor->GetStaticMeshComponent());

	if (InMaterial)
	{
		// The sphere is a little big, scale it down to 256x256x256
		FTransform Transform = FTransform::Identity;
		Transform.SetScale3D(FVector(0.8f));
		PreviewActor->GetStaticMeshComponent()->SetStaticMesh(GArmyEditor->GetThumbnailManager()->EditorSphere);
		PreviewActor->GetStaticMeshComponent()->SetRelativeTransform(Transform);
		PreviewActor->GetStaticMeshComponent()->UpdateBounds();

		// Center the mesh at the world origin then offset to put it on top of the plane
		const float BoundsZOffset = GetBoundsZOffset(PreviewActor->GetStaticMeshComponent()->Bounds);
		Transform.SetLocation(-PreviewActor->GetStaticMeshComponent()->Bounds.Origin + FVector(0, 0, BoundsZOffset));
		PreviewActor->GetStaticMeshComponent()->SetRelativeTransform(Transform);
	}

	PreviewActor->GetStaticMeshComponent()->SetMaterial(0, InMaterial);
	PreviewActor->GetStaticMeshComponent()->RecreateRenderState_Concurrent();
}

void FArmyMaterialThumbnailScene::GetViewMatrixParameters(const float InFOVDegrees, FVector& OutOrigin, float& OutOrbitPitch, float& OutOrbitYaw, float& OutOrbitZoom) const
{
	check(PreviewActor);
	check(PreviewActor->GetStaticMeshComponent());
	check(PreviewActor->GetStaticMeshComponent()->GetMaterial(0));

	// Fit the mesh in the view using the following formula
	// tan(HalfFOV) = Width/TargetCameraDistance
	const float HalfFOVRadians = FMath::DegreesToRadians<float>(InFOVDegrees) * 0.5f;
	// Add extra size to view slightly outside of the bounds to compensate for perspective
	const float BoundsMultiplier = 1.15f;
	const float HalfMeshSize = PreviewActor->GetStaticMeshComponent()->Bounds.SphereRadius * BoundsMultiplier;
	const float BoundsZOffset = GetBoundsZOffset(PreviewActor->GetStaticMeshComponent()->Bounds);
	const float TargetDistance = HalfMeshSize / FMath::Tan(HalfFOVRadians);

	OutOrigin = FVector(0, 0, -BoundsZOffset);
	OutOrbitPitch = -11.25;
	OutOrbitYaw = -157.5;
	OutOrbitZoom = TargetDistance + 0;
}

