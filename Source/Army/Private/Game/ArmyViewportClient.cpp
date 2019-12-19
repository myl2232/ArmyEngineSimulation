#include "ArmyViewportClient.h"
#include "ArmyDesignEditor.h"
#include "EngineUtils.h"
#include "ArmySceneData.h"
#include "ArmyGameInstance.h"
#include "ArmyObjectVisualizerManager.h"
#include "ArmyMouseDeltaTracker.h"
#include "ArmyEditorEngine.h"
#include "ArmyEditorModeManager.h"
#include "ArmyBaseArea.h"
#include "ArmyPanoramaModule.h"
#include "ArmyWindowActor.h"
#include "ArmyExtrusionActor.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyWorldManager.h"
#include "StaticLightingPrivate.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyPawn.h"
#include "ArmyPlayerController.h"

#define LOCTEXT_NAMESPACE "ArmyViewportClient"

UArmyViewportClient* GVC = nullptr;

UArmyViewportClient::UArmyViewportClient()
	: UArmyEditorViewportClient()
{
	GVC = this;
}

void UArmyViewportClient::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	Super::Draw(View, PDI);
	DrawXRObjectVisualizers(View, PDI);
	if (GGI->Window.IsValid() && GGI->Window->GetActiveController().IsValid())
	{
		GGI->Window->GetActiveController()->Draw(View, PDI);
	}
}

void UArmyViewportClient::Draw(FViewport* InViewport, FCanvas* SceneCanvas)
{
	Super::Draw(InViewport, SceneCanvas);

	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		Viewport,
		GetScene(),
		EngineShowFlags)
		.SetRealtimeUpdate(false));
	FSceneView* View = CalcSceneView(&ViewFamily);
	DrawXRObjectVisualizersHUD(InViewport, View, SceneCanvas);
	if (GGI->Window.IsValid() && GGI->Window->GetActiveController().IsValid())
	{
		GGI->Window->GetActiveController()->Draw(InViewport, View, SceneCanvas);
	}

	//测试显示变量
	/*AXRPawn* XRPawn = GXRPC->GetMyPawn();
	if (XRPawn)
	{
		FVector ActorLoc = XRPawn->GetActorLocation();
		FString Str = FString::Printf(TEXT("Test_HMDLoc：%.f, %.f, %.f"), XRPawn->Test_HMDLoc.X, XRPawn->Test_HMDLoc.Y, XRPawn->Test_HMDLoc.Z);
		FString Str2 = FString::Printf(TEXT("Test_HMDRot: %.f, %.f, %.f"), XRPawn->Test_HMDRot.Pitch, XRPawn->Test_HMDRot.Yaw, XRPawn->Test_HMDRot.Roll);
		FString Str3 = FString::Printf(TEXT("ActorLoc: %.f, %.f, %.f"), ActorLoc.X, ActorLoc.Y, ActorLoc.Z);
		SceneCanvas->DrawShadowedString(0, 810, *Str, GEngine->GetSmallFont(), FLinearColor::Red, 1.f);
		SceneCanvas->DrawShadowedString(0, 825, *Str2, GEngine->GetSmallFont(), FLinearColor::Red, 1.f);
		SceneCanvas->DrawShadowedString(0, 850, *Str3, GEngine->GetSmallFont(), FLinearColor::Red, 1.f);
	}*/

	//@打扮家 XRLightmass 渲染uv
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	if (SelectedActors.IsValidIndex(0) && FArmyWorldManager::Get().bShowLightmapUV)
	{
		//AXRWallActor* XA = Cast<AXRWallActor>(SelectedActors[0]);
		//if (XA)
		//{
		//	int32 GenerateType = -1;
		//	if (XA->AttachSurface.IsValid())
		//	{
		//		FArmyRoomSpaceArea* RSA = XA->AttachSurface->AsassignObj<FArmyRoomSpaceArea>();
		//		if (RSA)
		//		{
		//			GenerateType = RSA->GenerateFromObjectType;
		//		}
		//		else
		//		{
		//			FArmyHelpRectArea* RA = XA->AttachSurface->AsassignObj<FArmyHelpRectArea>();
		//			if (RA)
		//			{
		//				GenerateType = -2;
		//			}
		//		}
		//	}
		//	FString Str = FString::Printf(TEXT("ActorName: %s, Type: %d"), *XA->GetName(), GenerateType);
		//	SceneCanvas->DrawShadowedString(10, 10, *Str, GEngine->GetSmallFont(), FLinearColor::Red, 2.f);
		//}

		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(SelectedActors[0]);
		if (SMA)
		{
			TArray<FVector2D> SelectedEdgeTexCoords;
			DrawUVs(InViewport, SceneCanvas, 180, 0, FArmyWorldManager::Get().UVChannelToShow, SelectedEdgeTexCoords, SMA->GetStaticMeshComponent()->GetStaticMesh()->RenderData.Get(), NULL);
		}
		else
		{
			TArray<UActorComponent*> COMs = SelectedActors[0]->GetComponentsByClass(UXRProceduralMeshComponent::StaticClass());

			if (COMs.IsValidIndex(DrawSelectedUVComponentIndex))
			{
				UXRProceduralMeshComponent* GenCOM = Cast<UXRProceduralMeshComponent>(COMs[DrawSelectedUVComponentIndex]);

				FString Str = FString::Printf(TEXT("UVChannel: %d LightmapRes: %d"), GenCOM->LightMapResolution, FArmyWorldManager::Get().UVChannelToShow);
				SceneCanvas->DrawShadowedString(10, 10, *Str, GEngine->GetSmallFont(), FLinearColor::Red, 1.f);

				for (int32 i = 0; i < GenCOM->GetNumSections(); i++)
				{
					FArmyProcMeshSection* Sec = GenCOM->GetProcMeshSection(i);
					if (Sec)
					{
						Sec->ProcVertexBuffer;

						TArray<FVector2D> UVs;
						TArray<uint16> Indices;

						for (auto& It : Sec->ProcVertexBuffer)
						{
							UVs.Add(FArmyWorldManager::Get().UVChannelToShow == 0 ? It.UV : It.LightMapUV);
						}
						for (auto& It : Sec->ProcIndexBuffer)
						{
							Indices.Add(It);
						}
						DrawUVsEx(InViewport, SceneCanvas, 180, 0, 1, UVs, Indices);
					}
				}
			}
		}
	}
}

bool UArmyViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed /*= 1.f*/, bool bGamepad /*= false*/)
{
	if (GGI->Window.IsValid() && GGI->Window->GetActiveController().IsValid())
	{
		if (GGI->Window->GetActiveController()->InputKey(Viewport, Key, Event))
		{
			return true;
		}
		if (!LocalCamera)
		{
			return Super::InputKey(Viewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
		}
	}
	else if (XRObjectVisManager.HandleInputKey(this, Viewport, Key, Event))
	{
		return true;
	}

	return Super::InputKey(Viewport, ControllerId, Key, Event, AmountDepressed, bGamepad);
}

bool UArmyViewportClient::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	if (GGI->Window.IsValid() && GGI->Window->GetActiveController().IsValid())
	{
		if (GGI->Window->GetActiveController()->MouseDrag(Viewport, Key))
		{
			return true;
		}
		if (!LocalCamera)
		{
			return Super::InputAxis(Viewport, ControllerId, Key, Delta, DeltaTime, NumSamples, bGamepad);
		}
	}

	return Super::InputAxis(Viewport, ControllerId, Key, Delta, DeltaTime, NumSamples, bGamepad);
}

void UArmyViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	const FArmyViewportClick Click(&View, this, Key, Event, HitX, HitY);
	XRObjectVisManager.HandleClick(this,HitProxy,Click);

	if (GGI->Window.IsValid()&&GGI->Window->GetActiveController().IsValid())
	{
		GGI->Window->GetActiveController()->ProcessClick(View,HitProxy,Key,Event,HitX,HitY);
	}

	Super::ProcessClick(View,HitProxy,Key,Event,HitX,HitY);
}

bool UArmyViewportClient::InputWidgetDelta(FViewport* InViewport, EAxisList::Type InCurrentAxis, FVector& Drag, FRotator& Rot, FVector& Scale)
{
	if (XRObjectVisManager.HandleInputDelta(this, InViewport, Drag, Rot, Scale))
	{
		return true;
	}
	return Super::InputWidgetDelta(InViewport, InCurrentAxis, Drag, Rot, Scale);
}

float UArmyViewportClient::GetWallHeight()
{
	return FArmySceneData::Get()->WallHeight;
}

void UArmyViewportClient::Callback_PreSelectionNone()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	for (auto& It : SelectedActors)
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(It);
		if (SMA && SMA->Tags.Num() > 0 && SMA->Tags[0] == TEXT("MoveableMeshActor") && !FStaticLightingManager::Get()->IsValidLightMap())
		{
			SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
		}
		//如果是反射球，需要恢复为静态
		AXRReflectionCaptureActor* TheActor = Cast<AXRReflectionCaptureActor>(It);
		if (TheActor)
		{
			TheActor->ReflectionCaptureCOM->SetMobility(EComponentMobility::Static);
			TheActor->ReflectionCaptureCOM->SetCaptureIsDirty();
		}
	}
}

void UArmyViewportClient::Callback_SelectionNone()
{

}

void UArmyViewportClient::Callback_SelectionChanged(UObject* _NewSelectionObject)
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	for (auto& It : SelectedActors)
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(It);
		if (SMA && SMA->Tags.Num() > 0 && SMA->Tags[0] == TEXT("MoveableMeshActor"))
		{
			SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		}
		//如果是反射球，需要恢复为静态
		AXRReflectionCaptureActor* TheActor = Cast<AXRReflectionCaptureActor>(It);
		if (TheActor)
		{
			TheActor->ReflectionCaptureCOM->SetMobility(EComponentMobility::Movable);
			TheActor->ReflectionCaptureCOM->SetCaptureIsDirty();
		}
	}
}

void UArmyViewportClient::TrackingStopped()
{
	Super::TrackingStopped();
	/*if(MouseDeltaTracker->HasReceivedDelta ())
	{
		for(FSelectionIterator It (GXREditor->GetSelectedActorIterator ()); It; ++It)
		{
			AActor* Actor = static_cast<AActor*>(*It);
			checkSlow (Actor->IsA (AActor::StaticClass ()));
			Actor->PostEditMove (true);
			GXREditor->BroadcastEndObjectMovement (*Actor);
		}
	}*/
}

FArmyWidget::EWidgetMode UArmyViewportClient::GetWidgetMode() const
{
	if (XRObjectVisManager.IsActive() && XRObjectVisManager.IsVisualizingArchetype())
	{
		return FArmyWidget::WM_None;
	}
	return UArmyEditorViewportClient::GetWidgetMode();
}

FVector UArmyViewportClient::GetWidgetLocation() const
{
	FVector ComponentVisWidgetLocation;
	if (XRObjectVisManager.GetWidgetLocation(this, ComponentVisWidgetLocation))
	{
		return ComponentVisWidgetLocation;
	}
	return UArmyEditorViewportClient::GetWidgetLocation();
}


FMatrix UArmyViewportClient::GetWidgetCoordSystem() const
{
	FMatrix ComponentVisWidgetCoordSystem;
	if (XRObjectVisManager.GetCustomInputCoordinateSystem(this, ComponentVisWidgetCoordSystem))
	{
		return ComponentVisWidgetCoordSystem;
	}

	return UArmyEditorViewportClient::GetWidgetCoordSystem();
}


void UArmyViewportClient::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GGI->Window.IsValid() && GGI->Window->GetActiveController().IsValid())
	{
		GGI->Window->GetActiveController()->Tick(DeltaSeconds);
	}
}

void UArmyViewportClient::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	Super::MouseMove(Viewport, X, Y);

	if (GGI->Window.IsValid() && GGI->Window->GetActiveController().IsValid())
	{
		GGI->Window->GetActiveController()->MouseMove(Viewport, X, Y);
	}
}
void UArmyViewportClient::ProcessScreenShots(FViewport* Viewport)
{
	//@ 梁晓菲 全景图
	if (GIsHighResScreenshot)
	{
		FArmyPanoramaModule::Get().GetPanoramaController()->ProcessScreenShots(Viewport);
	}

	ScreenShotFun.ExecuteIfBound(Viewport);
}
FLinearColor UArmyViewportClient::GetBackgroundColor() const
{
	return NotShotBackColor ? FLinearColor(FColor(0, 0, 0, 0)) : Super::GetBackgroundColor();
}
bool UArmyViewportClient::LocalLineTraceSingleByChannel(FVector& OutHit, FVector& OutHitNormal, int32& HitObjectType/*摆放类型（1：地面，2：顶面：3：墙面）*/, const FVector& Start, const FVector& End)
{
	bool isHit = false;
	float MinDistance = (End - Start).Size();

	for (auto Obj : FArmySceneData::Get()->GetObjects(E_HardModel))
	{
		if (Obj.Pin()->GetType() > OT_TextureEditorBegin && Obj.Pin()->GetType() < OT_TextureEditorEnd)
		{
			if (Obj.Pin()->GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
			{
				FArmyBaseArea* PolygonArea = dynamic_cast<FArmyBaseArea*>(Obj.Pin().Get());

				if (PolygonArea)
				{
					//inter plolygon point
					FVector IntersectionPoint, IntersectionNormal;
					bool TempHit = PolygonArea->IsSelected(Start, End, IntersectionPoint, IntersectionNormal);

					if (TempHit)
					{
						float TempDistance = (IntersectionPoint - Start).Size();
						if (MinDistance > TempDistance)
						{
							isHit = TempHit;
							HitObjectType = PolygonArea->SurfaceType;
							MinDistance = TempDistance;
							OutHit = IntersectionPoint;
							OutHitNormal = IntersectionNormal;
						}
					}
				}
			}
		}
	}

	return isHit;
}
void UArmyViewportClient::LockViewPortClient(bool locked)
{
	LocalCamera = locked;
}

void UArmyViewportClient::SetDrawMode(EArmyDrawMode DrawMode)
{
	if (DrawMode == DM_2D)
	{
		//CurrentModel = DM_2D;
		SetShowGrid(true);
		//RectSelect->Start();
		SetViewportType(EXRLevelViewportType::LVT_OnlyCanvas);

		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			//bool Show = !Actor->Tags.Contains(TEXT("HideInList"));
			Actor->SetActorHiddenInGame(true);
		}
		EngineShowFlags.CompositeEditorPrimitives = true;
		auto MSAA = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MSAA.CompositingSampleCount"));
		MSAA->Set(8);

		//HomeModeler->BSPManager->HideModelGeometry();
	}
	else if (DrawMode == DM_3D)
	{
		//CurrentModel = DM_3D;
		SetShowGrid(false);
		//RectSelect->End();
		//RectSelect->Clear();
		SetViewportType(EXRLevelViewportType::LVT_Perspective);

		for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			if (Actor->Tags.Contains(TEXT("AreaHighLight"))
				|| Actor->Tags.Contains(TEXT("RectAreaHightActor"))
				|| Actor->Tags.Contains(TEXT("BreakPointReformHide")) //@郭子阳  断点改造隐藏
				|| Actor->ActorHasTag("WHCAlwaysHidden") //@zengy 木作模式始终隐藏的Actor标签
 				)
				Actor->SetActorHiddenInGame(true);
			else
				Actor->SetActorHiddenInGame(false);
		}
		EngineShowFlags.CompositeEditorPrimitives = false;
		//BuildAllBSP(HomeModeler);
	}
}

FVector2D UArmyViewportClient::GetCurrentMousePos()
{
	return FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY());
}



void UArmyViewportClient::ApplyDeltaToActor(AActor* InActor, const FVector& InDeltaDrag, const FRotator& InDeltaRot, const FVector& InDeltaScale)
{
	//AArmyExtrusionActor* tempExtruActor = Cast<AArmyExtrusionActor>(InActor);
	//AXRWindowActor* tempWindowActor = Cast<AXRWindowActor>(InActor);
	//AXRShapeActor* tempShapeActor = Cast<AXRShapeActor>(InActor);
	//if (tempExtruActor || tempWindowActor || tempShapeActor)
	//	return;
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

	GXREditor->ApplyDeltaToActor(
		InActor,
		true,
		&InDeltaDrag,
		&InDeltaRot,
		&ModifiedDeltaScale,
		IsAltPressed(),
		IsShiftPressed(),
		IsCtrlPressed());
}



void UArmyViewportClient::ZoomCanvas(const struct FArmyInputEventState& InputState)
{
	OnOrthoZoom(InputState);
}

void UArmyViewportClient::RegisterXRObjectVisualizer(EObjectType ObjectType, TSharedPtr<class FArmyObjectVisualizer> Visualizer)
{
	XRObjectVisualizerMap.Add(ObjectType, Visualizer);
	RegisteredXRObjectTypes.Add(ObjectType);
	if (Visualizer.IsValid())
		Visualizer->OnRegister();
}

TSharedPtr<class FArmyObjectVisualizer> UArmyViewportClient::FindXRObjectVisualizer(EObjectType ObjectType) const
{
	TSharedPtr<FArmyObjectVisualizer> Visualizer = NULL;
	const TSharedPtr<FArmyObjectVisualizer>* VisualizerPtr = XRObjectVisualizerMap.Find(ObjectType);
	if (VisualizerPtr != NULL)
	{
		Visualizer = *VisualizerPtr;
	}

	return Visualizer;
}

void UArmyViewportClient::DrawXRObjectVisualizers(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	for (FCachedXRObjectVisualizer& CachedVisualizer : VisualizersForSelection)
	{
		TSharedPtr<FArmyObject> ObjectPtr = CachedVisualizer.Component.Pin();
		CachedVisualizer.Visualizer->DrawVisualization(ObjectPtr.Get(), View, PDI);
	}
}

void UArmyViewportClient::DrawXRObjectVisualizersHUD(const FViewport* Viewport, const FSceneView* View, FCanvas* Canvas)
{
	for (FCachedXRObjectVisualizer& CachedVisualizer : VisualizersForSelection)
	{
		TSharedPtr<FArmyObject> ObjectPtr = CachedVisualizer.Component.Pin();
		CachedVisualizer.Visualizer->DrawVisualizationHUD(ObjectPtr.Get(), Viewport, View, Canvas);
	}
}

void UArmyViewportClient::UnregisterXRObjectVisualizer(EObjectType ObjectType)
{
	TSharedPtr<FArmyObjectVisualizer> Visualizer = FindXRObjectVisualizer(ObjectType);
	VisualizersForSelection.RemoveAll([&Visualizer](const auto& CachedComponentVisualizer)
	{
		return CachedComponentVisualizer.Visualizer == Visualizer;
	});

	XRObjectVisualizerMap.Remove(ObjectType);
}

void UArmyViewportClient::ClearVisualizersForSelection()
{
	VisualizersForSelection.Empty();
}

void UArmyViewportClient::AddVisualizersForSelection(TSharedPtr<class FArmyObject>& Object)
{
	TSharedPtr<FArmyObjectVisualizer> Visualizer = FindXRObjectVisualizer(Object->GetType());
	if (Visualizer.IsValid())
	{
		VisualizersForSelection.Add(FCachedXRObjectVisualizer(Object.Get(), Visualizer));
	}
}

#undef LOCTEXT_NAMESPACE