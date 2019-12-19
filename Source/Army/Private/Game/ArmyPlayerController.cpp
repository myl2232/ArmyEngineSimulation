#include "ArmyPlayerController.h"
#include "EngineUtils.h"
#include "IArmyTrackingSystem.h"
#include "IHeadMountedDisplay.h"
#include "SViewport.h"
#include "SteamVRFunctionLibrary.h"
#include "ArmyResourceModule.h"
#include "ArmyEditorEngine.h"
#include "ArmyEditor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Runtime/Engine/Classes/Engine/PostProcessVolume.h"
#include "ArmyEngineModule.h"
#include "ArmyViewportClient.h"
#include "Scalability.h"
#include "Runtime/Engine/Classes/Engine/DirectionalLight.h"
#include "Runtime/Engine/Classes/Components/DirectionalLightComponent.h"
#include "ArmySceneData.h"
#include "Army3DManager.h"
#include "ArmyGameInstance.h"
#include "ArmyActorVisitorFactory.h"

//@打扮家 XRLightmass
//#include "Editor/UnrealEd/Public/EditorBuildUtils.h"
#include "UObject/UObjectIterator.h"
#include "UObject/ObjectMacros.h"
#include "Components/LightComponentBase.h"
#include "Components/SkyLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/SphereReflectionCapture.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/MapBuildDataRegistry.h"
//#include "Interfaces/ITargetPlatform.h"
//#include "Interfaces/ITargetPlatformManagerModule.h"
//#include "MeshUtilities.h"
//#include "DerivedDataCacheInterface.h"
#include "ArmyWallActor.h"
#include "ArmyStaticLighting/Public/LayoutUV.h"
#include "ArmyStaticLighting/Public/XRMeshUtilities.h"
#include "ArmyStaticLighting/Public/StaticLightingPrivate.h"
//#include "ArmyGenMeshComponent.h"
#include "ArmyPointLightActor.h"
#include "ArmySpotLightActor.h"
#include "LightingBuildOptions.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "RenderingThread.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineModule.h"
#include "Lightmass/LightmassImportanceVolume.h"
#include "ComponentRecreateRenderStateContext.h"
#include "ArmyProceduralMeshComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Runtime/Engine/Classes/Materials/MaterialInterface.h"
#include "ArmyHardModeController.h"
#include "ArmyReflectionCaptureActor.h"
#include "ComponentReregisterContext.h"
#include "ArmyWorldManager.h"

AXRPlayerController* GXRPC = nullptr;

AXRPlayerController::AXRPlayerController()
{
	bPostInit = false;
	bVRModeEnabled = false;
	bVRTestModeEnabled = false;
	bVRTeleportEnabled = false;
	HTCFocusedActorDuaration = 0.f;
	GXRPC = this;
	CurrentViewMode = EXRView_FPS;

	TypeArray.Add(OT_InternalRoom);
	TypeArray.Add(OT_OutRoom);
	TypeArray.Add(OT_ArcWindow);
	TypeArray.Add(OT_Window);
	TypeArray.Add(OT_FloorWindow);
	TypeArray.Add(OT_RectBayWindow);
	TypeArray.Add(OT_TrapeBayWindow);
	TypeArray.Add(OT_CornerBayWindow);
	TypeArray.Add(OT_Door);
	TypeArray.Add(OT_SecurityDoor);
	TypeArray.Add(OT_SlidingDoor);
	TypeArray.Add(OT_Pass);
	TypeArray.Add(OT_NewPass);
	TypeArray.Add(OT_Beam);
	TypeArray.Add(OT_Pillar);
	TypeArray.Add(OT_AirFlue);
	TypeArray.Add(OT_IndependentWall);
	TypeArray.Add(OT_PackPipe);
}

void AXRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	this->SetActorTickEnabled(true);
	this->bBlockInput = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;

	FreezeDefaultLevelActor();

	ConsoleCommand("r.DistanceFieldAO 0");
	ConsoleCommand("disablescreenmessages");
	ConsoleCommand("showflag.ScreenSpaceReflections 1");
	ConsoleCommand("showflag.CompositeEditorPrimitives 1");
}

void AXRPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (!bPostInit)
	{
		bPostInit = true;
		PostInit();

	}

	TickViveController();
}

void AXRPlayerController::PostInit()
{
}

void AXRPlayerController::TickViveController()
{
	HTCInteractiveActor = NULL;

	if (bVRModeEnabled || bVRTestModeEnabled)
	{
		FVector Loc, Loc1;
		FRotator Rot, Rot1;
		FQuat Qut;
		bVRTeleportEnabled = false;
		if (GetMyPawn()->LeftArmMesh)
		{
			Loc = GetPawn()->GetActorLocation();
			Loc.Z -= GetMyPawn()->HalfCapsuleHeight;
			USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Left, Loc1, Rot1);
			GetMyPawn()->LeftArmMesh->SetWorldLocationAndRotation(Loc + Loc1, Rot1.Quaternion());
		}
		if (GetMyPawn()->RightArmMesh)
		{
			Loc = GetPawn()->GetActorLocation();
			Loc.Z -= GetMyPawn()->HalfCapsuleHeight;
			USteamVRFunctionLibrary::GetHandPositionAndOrientation(0, EControllerHand::Right, Loc1, Rot1);
			if (bVRTestModeEnabled)
			{
				Rot = GetControlRotation();
				GetMyPawn()->RightArmMesh->SetWorldRotation(Rot.Quaternion());
			}
			else
			{
				GetMyPawn()->RightArmMesh->SetWorldLocationAndRotation(Loc + Loc1, Rot1.Quaternion());
			}
		}

		//手柄检测物体
		FHitResult OutHit(ForceInit);
		static FName TraceTag = FName(TEXT("SteamControllerPick"));
		FCollisionQueryParams TraceParams(TraceTag, true, this);
		TraceParams.AddIgnoredActor(GetPawn());
		if (HTCInteractiveActorIgnored)
		{
			TraceParams.AddIgnoredActor(HTCInteractiveActorIgnored);
			HTCFocusedActorDuaration -= GetWorld()->GetDeltaSeconds();
			if (HTCFocusedActorDuaration < 0.f)
			{
				HTCFocusedActorDuaration = 0.f;
				HTCInteractiveActorIgnored = NULL;
			}
		}

		FVector TraceStart = GetMyPawn()->RightArmMesh->GetComponentLocation();
		FVector TraceEnd = TraceStart + GetMyPawn()->RightArmMesh->GetComponentRotation().Vector() * 65535;
		bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);
		if (bHit && OutHit.bBlockingHit)
		{
			UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
			AActor* HitActor = OutHit.GetActor();
			EActorType ActorType = ResMgr->GetActorType(HitActor);
			float CapsuleHalfHeight = GetMyPawn()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

			if (ActorType == EActorType::Blueprint || ActorType == EActorType::Moveable)
			{
				//如果是骨骼模型，并且是蓝图物体，则有可能是门
				bool bIsSkeletalMesh = false;
				FVRSObject* ResultObj = ResMgr->GetObjFromObjID(HitActor->GetObjID());
				if (ResultObj)
				{
					bIsSkeletalMesh = ResultObj->GetSkeletalMeshObject() != NULL;
				}

				if (ActorType == EActorType::Blueprint)
				{
					//查看是否为可以交互的物体
					UFunction* Function = HitActor->FindFunction(FName(TEXT("OnToggle")));
					if (Function && !HitActor->bHidden)
					{
						HTCInteractiveActor = HitActor;

						if (bIsSkeletalMesh)
						{
							//记录当前手柄停留在该物体上面的时间，如果超过一定时间，则关闭门的交互碰撞，这样才可以进入房间。
							HTCFocusedActorDuaration += GetWorld()->GetDeltaSeconds();
							if (HTCInteractiveActorIgnored != HitActor)
							{
								HTCInteractiveActorIgnored = NULL;
							}
							if (HTCFocusedActorDuaration > 1.f)
							{
								HTCInteractiveActorIgnored = HitActor;
							}
						}

						DrawDebugLine(GetWorld(), TraceStart, OutHit.ImpactPoint, FColor::Purple, false, -1.f, 0, 1.f);
						DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 16.f, FColor::Orange, false, -1.f);
					}
					//可以直接瞬移到较矮的交互物体上面
					else
					{
						if (OutHit.ImpactPoint.Z - GetPawn()->GetActorLocation().Z - GetMyPawn()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() <= 50.f)
						{
							bVRTeleportEnabled = true;
							VRTeleportLoc = OutHit.ImpactPoint + FVector(0, 0, 5 + CapsuleHalfHeight);
							DrawDebugLine(GetWorld(), TraceStart, OutHit.ImpactPoint, FColor::Green, false, -1.f, 0, 1.f);

							/*TeleportLocation->SetActorHiddenInGame(false);
							FRotator Rot2 = GetVRSP()->GetActorRotation();
							Rot2.Roll = 0;
							Rot2.Pitch = 0;
							TeleportLocation->SetActorLocationAndRotation(OutHit.ImpactPoint, Rot2);*/
						}
						else
						{
							DrawDebugLine(GetWorld(), TraceStart, OutHit.ImpactPoint, FColor::Red, false, -1.f, 0, 1.f);
						}
					}
				}
				//可以直接瞬移到较矮的家具上面
				else if (ActorType == EActorType::Moveable)
				{
					if (OutHit.ImpactPoint.Z - GetPawn()->GetActorLocation().Z + GetMyPawn()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() <= 30.f)
					{
						bVRTeleportEnabled = true;
						VRTeleportLoc = OutHit.ImpactPoint + FVector(0, 0, 5 + CapsuleHalfHeight);
						DrawDebugLine(GetWorld(), TraceStart, OutHit.ImpactPoint, FColor::Green, false, -1.f, 0, 1.f);
					}
					else
					{
						DrawDebugLine(GetWorld(), TraceStart, OutHit.ImpactPoint, FColor::Red, false, -1.f, 0, 1.f);
					}
				}
			}
			else
			{
				//碰撞点的法线方向朝上才能移动
				if (FVector::DotProduct(OutHit.ImpactNormal, FVector(0, 0, 1)) > 0.7f)
				{
					bVRTeleportEnabled = true;
					VRTeleportLoc = OutHit.ImpactPoint + FVector(0, 0, 5 + CapsuleHalfHeight);
					DrawDebugLine(GetWorld(), TraceStart, OutHit.ImpactPoint, FColor::Green, false, -1.f, 0, 1.f);
					DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 8.f, FColor::Green, false, -1.f);
				}
				else
				{
					DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, -1.f, 0, 1.f);
				}
			}
		}
		else
		{
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, -1.f, 0, 1.f);
		}
	}
}

AXRPawn * AXRPlayerController::GetMyPawn()
{
	if (MyPawn)
		return MyPawn;
	MyPawn = Cast<AXRPawn>(GetPawn());
	if (MyPawn)
		return MyPawn;
	return NULL;
}

TSharedPtr<class FArmyHardModeController> AXRPlayerController::GetHardPC()
{
	if (HardPC.IsValid())
	{
		return HardPC;
	}
	else
	{
		HardPC = GGI->DesignEditor->GetModeController<FArmyHardModeController>(DesignMode::HardMode);
		//HardPC = StaticCastSharedPtr<FArmyHardModeController>(GGI->DesignEditor->CurrentController);
		return HardPC;
	}
}

bool AXRPlayerController::IsVRMode()
{
	return bVRModeEnabled;
}

bool AXRPlayerController::EnableVRMode(bool _bEnable)
{
	if (_bEnable)
	{
		this->SetActorTickEnabled(true);
		this->bBlockInput = false;
	}
	else
	{
		this->SetActorTickEnabled(false);
		this->bBlockInput = true;
	}

	TSharedPtr<SWindow> RootWindow = GEngine->GameViewport->GetWindow();

	TSharedPtr<SOverlay> WindowOverlay = RootWindow->GetWindowOverlay();
	bVRModeEnabled = false;
	if (_bEnable)
	{
		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDEnabled())
		{
			GVC->bUserPlayerControllerView = true;
			GEngine->StereoRenderingDevice->EnableStereo(true);
			GEngine->GetGameViewportWidget()->SetRenderDirectlyToWindow(true);
			GEngine->XRSystem->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
			FVector PawnLoc = GetPawn()->GetActorLocation();
			PawnLoc.Z -= GetMyPawn()->HalfCapsuleHeight;
			GetMyPawn()->LeftArmMesh->SetVisibility(true);
			GetMyPawn()->RightArmMesh->SetVisibility(true);
			GetPawn()->SetActorLocation(PawnLoc);
			GetPawn()->BaseEyeHeight = 0;
			if (WindowOverlay.IsValid())
			{
				WindowOverlay->GetChildren()->GetChildAt(0)->SetVisibility(EVisibility::Hidden);
				WindowOverlay->GetChildren()->GetChildAt(1)->SetVisibility(EVisibility::Hidden);
			}
			GGI->Window->SetViewportFullscreen(true);
			ConsoleCommand(TEXT("r.screenpercentage 150"));

			bVRModeEnabled = true;
			return true;
		}
	}
	else
	{
		if (GEngine->XRSystem.IsValid() && GEngine->XRSystem->GetHMDDevice() && GEngine->XRSystem->GetHMDDevice()->IsHMDEnabled())
		{
			GEngine->StereoRenderingDevice->EnableStereo(false);
		}

		GEngine->GetGameViewportWidget()->SetRenderDirectlyToWindow(false);
		GetPawn()->BaseEyeHeight = 62;
		GetMyPawn()->LeftArmMesh->SetVisibility(false);
		GetMyPawn()->RightArmMesh->SetVisibility(false);
		if (WindowOverlay.IsValid())
		{
			WindowOverlay->GetChildren()->GetChildAt(0)->SetVisibility(EVisibility::SelfHitTestInvisible);
			WindowOverlay->GetChildren()->GetChildAt(1)->SetVisibility(EVisibility::SelfHitTestInvisible);
		}

		GGI->Window->SetViewportFullscreen(false);
		ConsoleCommand(TEXT("r.screenpercentage 100"));
		GVC->bUserPlayerControllerView = false;

		bVRModeEnabled = false;
		return true;
	}
	return false;
}

void AXRPlayerController::OnExitFullScreenMode()
{
	GVC->SetGameViewportFullScreenMode(false);

	if (IsVRMode())
	{
		EnableVRMode(false);
	}
}

void AXRPlayerController::SetQualityLevels(int32 _Level)
{
	Scalability::FQualityLevels CurLevels = Scalability::GetQualityLevels();
	CurLevels.SetFromSingleQualityLevel(_Level);
	CurLevels.EffectsQuality = 3;
	Scalability::SetQualityLevels(CurLevels);

	//精美画质
	if (_Level == 3)
		ShowDirectionalLight(true);
	//流畅画质将关闭太阳光，开启环境光照亮
	else
		ShowDirectionalLight(false);
}

int32 AXRPlayerController::GetQualityLevels()
{
	Scalability::FQualityLevels CurLevels = Scalability::GetQualityLevels();
	int32 Level = CurLevels.GetSingleQualityLevel();
	return Level;
}

bool AXRPlayerController::IsHightQualityLevel()
{
	Scalability::FQualityLevels CurLevels = Scalability::GetQualityLevels();
	if (CurLevels.GetSingleQualityLevel() == 3)
		return true;

	return false;
}

void AXRPlayerController::ShowDirectionalLight(bool _bShow)
{
	for (auto& It : DirectionalLightCOMs)
	{
		It->SetVisibility(_bShow);
	}
	if (_bShow)
	{
		for (auto& It : DirectionalLightList)
		{
			It.Value->GetLightComponent()->SetIntensity(It.Key);
		}
	}
	else
	{
		for (auto& It : DirectionalLightList)
		{
			It.Value->GetLightComponent()->SetIntensity(0);
		}
	}
}

void AXRPlayerController::SetXRViewMode(EXRViewMode _Mode)
{
	if (_Mode == EXRView_FPS) {
		GVC->SetViewportType(EXRLevelViewportType::LVT_Perspective);
		//if (IsHightQualityLevel()) {
		//	ShowDirectionalLight(true);
		//}		
		////开启光照
		//ConsoleCommand(TEXT("ShowFlag.Lighting 1"));
		////开启高光和反射
		//ConsoleCommand(TEXT("ShowFlag.ScreenSpaceReflections 1"));
		//ConsoleCommand(TEXT("ShowFlag.Specular 1"));		
	}
	else if (_Mode == EXRView_TOP) {
		GVC->SetViewportType(EXRLevelViewportType::LVT_OrthoXY);
		////关闭光照
		//ShowDirectionalLight(false);
		//ConsoleCommand(TEXT("ShowFlag.Lighting 0"));
		////关闭高光和反射
		//ConsoleCommand(TEXT("ShowFlag.ScreenSpaceReflections 0"));
		//ConsoleCommand(TEXT("ShowFlag.Specular 0"));
		//FArmySceneData::Get()->SetHiddenRoof();		
	}
	else if (_Mode == EXRView_CEILING) {
		GVC->SetViewportType(EXRLevelViewportType::LVT_OrthoXY);
		////关闭光照
		//ShowDirectionalLight(false);
		//ConsoleCommand(TEXT("ShowFlag.Lighting 0"));
		////关闭高光和反射
		//ConsoleCommand(TEXT("ShowFlag.ScreenSpaceReflections 0"));
		//ConsoleCommand(TEXT("ShowFlag.Specular 0"));
		//FArmySceneData::Get()->SetHiddenRoof();
	}
	else if (_Mode == EXRView_TPS) {
		GVC->SetViewportType(EXRLevelViewportType::LVT_Perspective);
		if (IsHightQualityLevel()) {
			ShowDirectionalLight(true);
		}
		//开启光照
		ConsoleCommand(TEXT("ShowFlag.Lighting 1"));
		//开启高光和反射
		ConsoleCommand(TEXT("ShowFlag.ScreenSpaceReflections 1"));
		ConsoleCommand(TEXT("ShowFlag.Specular 1"));
	}

	OnViewChanged(_Mode, CurrentViewMode);
	CurrentViewMode = _Mode;
}

void AXRPlayerController::SetupInputComponent()
{
	static bool bBindingsAdded = false;
	if (!bBindingsAdded)
	{
		bBindingsAdded = true;
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("LeftMouseButton", EKeys::LeftMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("RightMouseButton", EKeys::RightMouseButton));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ExportDAE", EKeys::P));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Interactive_HTC_Left", EKeys::MotionController_Left_Trigger));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Interactive_HTC_Right", EKeys::MotionController_Right_Trigger));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Interactive_HTC_Left", EKeys::Nine));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Interactive_HTC_Right", EKeys::Zero));

		//ESC和Enter键都可以退出全屏（VR）模式
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Key_ExitFullScreenMode", EKeys::Enter));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Key_ExitFullScreenMode", EKeys::Escape));
	}

	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("Interactive_HTC_Left", IE_Pressed, this, &AXRPlayerController::OnViveLeftTrigger);
	InputComponent->BindAction("Interactive_HTC_Right", IE_Pressed, this, &AXRPlayerController::OnViveRightTrigger);
	InputComponent->BindAction("Key_ExitFullScreenMode", IE_Pressed, this, &AXRPlayerController::OnExitFullScreenMode);
}

void AXRPlayerController::UpdateRotation(float DeltaTime)
{
	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	FRotator ViewRotation = GetControlRotation();

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}

	SetControlRotation(ViewRotation);

	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}

void AXRPlayerController::EXE_Stereo(bool bEnable)
{
	EnableVRMode(bEnable);
}

void AXRPlayerController::EXE_SetVRTestModeEnabled(bool bEnable)
{
	bVRTestModeEnabled = bEnable;

	if (bVRTestModeEnabled)
	{
		this->SetActorTickEnabled(true);
		this->bBlockInput = false;
		GVC->bUserPlayerControllerView = true;
		GetMyPawn()->LeftArmMesh->SetVisibility(true);
		GetMyPawn()->RightArmMesh->SetVisibility(true);
	}
	else
	{
		this->SetActorTickEnabled(false);
		this->bBlockInput = true;
		GVC->bUserPlayerControllerView = false;
		GetMyPawn()->LeftArmMesh->SetVisibility(false);
		GetMyPawn()->RightArmMesh->SetVisibility(false);
	}
}

void AXRPlayerController::EXE_SetCustomCursor(int32 _Mode)
{
	if (_Mode == 0)
	{
		GVC->CurCursor = EMouseCursor::Custom;
		GVC->SetCustomCursorWidget(SNew(SButton));
	}
	else if (_Mode == 1)
	{
		GVC->CurCursor = EMouseCursor::GrabHand;
	}
}

void AXRPlayerController::EXE_TestJson()
{
	FString SyncBody;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&SyncBody);
	JsonWriter->WriteObjectStart();

	JsonWriter->WriteArrayStart("0");
	JsonWriter->WriteValue(5);
	JsonWriter->WriteValue(6);
	JsonWriter->WriteValue(7);
	JsonWriter->WriteValue(8);
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart("2");
	JsonWriter->WriteValue(10);
	JsonWriter->WriteValue(11);
	JsonWriter->WriteValue(12);
	JsonWriter->WriteValue(13);
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	int32 a = 0;
}

void AXRPlayerController::EXE_ViewMode(int32 InMode)
{
	for (TActorIterator<APostProcessVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		APostProcessVolume* PPV = *ActorItr;
		//全局后处理线框材质				
		if (InMode == 1) {
			PPV->AddOrUpdateBlendable(FArmyEngineModule::Get().GetEngineResource()->GetPM_Outline());
		}
		else {
			PPV->Settings.RemoveBlendable(FArmyEngineModule::Get().GetEngineResource()->GetPM_Outline());
		}
	}
}

void AXRPlayerController::EXE_BuildStaticLighting(int32 InQuality)
{
	FStaticLightingManager::Get()->BuildStaticLighting(InQuality);
}

void AXRPlayerController::EXE_ClearStaticLighting()
{
	FStaticLightingManager::Get()->ClearStaticLighting();
}

void AXRPlayerController::EXE_SaveStaticLighting()
{
	FPlatformFileManager::Get().SetPlatformFile(IPlatformFile::GetPlatformPhysical());
	UObject* Obj = StaticFindObject(UPackage::StaticClass(), NULL, TEXT("/Game/XRCommon/Map/DIYTemplate_Grass_BuiltData"));
	if (Obj)
	{
		UPackage* Pkg = Cast<UPackage>(Obj);
		if (Pkg)
		{
			Pkg->SetPackageFlags(PKG_FilterEditorOnly);
			FString SavePath = FPaths::ProjectContentDir() + TEXT("DBJCache/LevelPlan/LevelBuildData001.uasset");
			//IPlatformFile::GetPlatformPhysical().DeleteFile(*SavePath);
			//FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*SavePath);
			const FSavePackageResultStruct Result = UPackage::Save(Pkg, NULL, RF_Standalone, *SavePath);
			int32 a = 0;
		}
	}
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	FPlatformFileManager::Get().SetPlatformFile(*ResMgr->LocalPlatformFile);
} 

void AXRPlayerController::EXE_LoadStaticLighting()
{
	FPlatformFileManager::Get().SetPlatformFile(IPlatformFile::GetPlatformPhysical());

	UPackage* ResultPackage = LoadPackageFromRuntime(NULL, TEXT("/Game/DBJCache/LevelPlan/LevelBuildData001"), RF_NoFlags);
	UObject* ResultObj = StaticFindObjectFast(UObject::StaticClass(), ResultPackage, TEXT("DIYTemplate_Grass_BuiltData"));

	if (ResultObj)
	{
		if (UMapBuildDataRegistry* TmpMapData = Cast<UMapBuildDataRegistry>(ResultObj))
		{
			ULevel* TheLevel = GetWorld()->GetCurrentLevel();
			TheLevel->MapBuildData = TmpMapData;
			TheLevel->InitializeRenderingResources();
			FGlobalComponentRecreateRenderStateContext RecreateRenderState;
		}
	}

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	FPlatformFileManager::Get().SetPlatformFile(*ResMgr->LocalPlatformFile);
}

void AXRPlayerController::EXE_LoadStaticLightingEx()
{
	//GetHardPC()->SetStaticLightEnvironment();
	FArmyWorldManager::Get().SetEnvironmentMode(false);

	FStaticLightingManager::Get()->GatherStaticLightingInfo();

	FPlatformFileManager::Get().SetPlatformFile(IPlatformFile::GetPlatformPhysical());

	UPackage* ResultPackage = LoadPackageFromRuntime(NULL, TEXT("/Game/DBJCache/LevelPlan/LevelBuildData001"), RF_NoFlags);
	UObject* ResultObj = StaticFindObjectFast(UObject::StaticClass(), ResultPackage, TEXT("DIYTemplate_Grass_BuiltData"));

	if (ResultObj)
	{
		if (UMapBuildDataRegistry* TmpMapData = Cast<UMapBuildDataRegistry>(ResultObj))
		{
			ULevel* TheLevel = GetWorld()->GetCurrentLevel();
			TheLevel->MapBuildData = TmpMapData;
			TheLevel->InitializeRenderingResources();
			FGlobalComponentRecreateRenderStateContext RecreateRenderState;
		}
	}

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	FPlatformFileManager::Get().SetPlatformFile(*ResMgr->LocalPlatformFile);

	//刷新检测光源状态
	TArray<ULightComponent*> LightCOMList;
	for (TObjectIterator<ULightComponent> It; It; ++It)
	{
		if (!((*It)->GetFlags() & RF_ArchetypeObject))
		{
			(*It)->RecreateRenderState_Concurrent();
			LightCOMList.Add(*It);
		}
	}
	int32 a = 0;
}

void AXRPlayerController::EXE_LoadPackageTest()
{
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	FPlatformFileManager::Get().SetPlatformFile(IPlatformFile::GetPlatformPhysical());
	UPackage* TmpPackage2 = LoadPackageFromRuntime(NULL, TEXT("/Game/DBJCache/LevelPlan/LevelBuildData001"), RF_NoFlags);
	UObject* Result = StaticFindObjectFast(UObject::StaticClass(), TmpPackage2, TEXT("DIYTemplate_Grass_BuiltData"));
	return;
}

void AXRPlayerController::EXE_UnloadLightMapDataPackage()
{
	//IPlatformFile::GetPlatformPhysical().clo
	GetWorld()->GetCurrentLevel()->MapBuildData->InvalidateStaticLighting(GetWorld());
	GetWorld()->GetCurrentLevel()->MapBuildData = NULL;

	UObject* Pkg = StaticFindObject(UPackage::StaticClass(), NULL, TEXT("/Game/XRCommon/Map/DIYTemplate_Grass_BuiltData"));
	if (Pkg)
	{
		Pkg->ConditionalBeginDestroy();
	}
}

void AXRPlayerController::EXE_RenderSomethingToRGB()
{
	UTexture2D* Tex = NULL;// GetWorld()->GetCurrentLevel()->MapBuildData->GetLightMapTexture();
	if (!Tex)
		return;

	UTextureRenderTarget2D* RenderTarget = NULL;
	FCanvas* Canvas = nullptr;

	if (RenderTarget == NULL)
	{
		RenderTarget = NewObject<UTextureRenderTarget2D>();
		check(RenderTarget);
		RenderTarget->AddToRoot();
		RenderTarget->ClearColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
		RenderTarget->InitCustomFormat(128, 128, PF_FloatRGBA, false);

		Canvas = new FCanvas(RenderTarget->GameThread_GetRenderTargetResource(), NULL, 0, 0, 0, ERHIFeatureLevel::SM5);
		check(Canvas);
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND(
		InitializeSystemTextures,
		{
			GetRendererModule().InitializeSystemTextures(RHICmdList);
		});

	Canvas->SetRenderTarget_GameThread(RenderTarget->GameThread_GetRenderTargetResource());
	Canvas->Clear(FLinearColor(0, 0, 0, 0));
	FCanvasTileItem TileItem(FVector2D(0.0f, 0.0f), Tex->Resource, FVector2D(1024, 1024), FLinearColor(1,1,1,1));
	TileItem.bFreezeTime = true;
	Canvas->DrawItem(TileItem);
	Canvas->Flush_GameThread();
	FlushRenderingCommands();
	Canvas->SetRenderTarget_GameThread(NULL);
	FlushRenderingCommands();

	//TestMaterialSamples.Reset();
	//bool bResult = RenderTarget->GameThread_GetRenderTargetResource()->ReadFloat16Pixels(TestMaterialSamples);
	int32 a = 0;
}

void AXRPlayerController::EXE_CreateFileTest()
{
	//FString SavePath = FPaths::ProjectContentDir() + TEXT("DBJCache/LevelPlan/LevelBuildData001.uasset");
	//TestHandle = IPlatformFile::GetPlatformPhysical().OpenAsyncRead(*SavePath);

	FPlatformFileManager::Get().SetPlatformFile(IPlatformFile::GetPlatformPhysical());
	TestPackage = LoadPackageFromRuntime(NULL, TEXT("/Game/DBJCache/LevelPlan/LevelBuildData001"), RF_NoFlags);
}

void AXRPlayerController::EXE_CloseFileTest()
{
	/*if (TestHandle)
	{
		delete TestHandle;
		TestHandle = nullptr;
	}*/

	if (TestPackage)
	{
		TestPackage->ConditionalBeginDestroy();
		TestPackage = NULL;
	}
}

//void AXRPlayerController::EXE_LoadBuildMapDataPackageInEditor()
//{
//	FString BuildDataPath = FPaths::ProjectContentDir() + "DBJCache/LevelPlan/LevelBuildData001.uasset";
//	UPackage* TmpPackage = LoadPackage(NULL, *BuildDataPath, RF_NoFlags);
//	if (TmpPackage)
//	{
//		UObject* TmpObj = TmpPackage->LinkerLoad->ExportMap[39].Object;
//		if (TmpObj)
//		{
//			if (UMapBuildDataRegistry* TmpMapData = Cast<UMapBuildDataRegistry>(TmpObj))
//			{
//				ULevel* TheLevel = GetWorld()->GetCurrentLevel();
//
//				TheLevel->MapBuildData = TmpMapData;
//				//TheLevel->OnApplyNewLightingData(true);
//				TheLevel->InitializeRenderingResources();
//				GetWorld()->UpdateWorldComponents(true, true);
//			}
//		}
//	}
//}

void AXRPlayerController::EXE_ComputeSelectedMeshUV2InEditor(bool bAutoWrap)
{
	FArmyWorldManager::Get().bShowLightmapUV = true;
	FArmyWorldManager::Get().UVChannelToShow = 1;

	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	if (SelectedActors.IsValidIndex(0))
	{
		TArray<UActorComponent*> COMs = SelectedActors[0]->GetComponentsByClass(UXRProceduralMeshComponent::StaticClass());
		if (COMs.IsValidIndex(0))
		{
			for (auto& It : COMs)
			{
				UXRProceduralMeshComponent* MeshCOM = Cast<UXRProceduralMeshComponent>(It);
				if (MeshCOM)
				{
					FRawMesh RawMesh;

					FTransform RootTransform = SelectedActors[0]->GetTransform();
					FMatrix WorldToRoot = RootTransform.ToMatrixWithScale().Inverse();
					FMatrix ComponentToWorld = MeshCOM->GetComponentTransform().ToMatrixWithScale() * WorldToRoot;

					for (int32 i = 0; i < MeshCOM->GetNumSections(); i++)
					{
						FArmyProcMeshSection* Sec = MeshCOM->GetProcMeshSection(i);
						const int32 BaseVertexIndex = RawMesh.VertexPositions.Num();

						const TArray<FArmyProcMeshVertex>& VerticeBuffer = Sec->ProcVertexBuffer;
						const TArray<int32>& IndexBuffer = Sec->ProcIndexBuffer;

						for (auto& It : VerticeBuffer)
						{
							RawMesh.VertexPositions.Add(ComponentToWorld.InverseTransformPosition(It.Position));
						}

						for (auto& It : IndexBuffer)
						{
							RawMesh.WedgeIndices.Add(It + BaseVertexIndex);
							RawMesh.WedgeTangentX.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentX));
							RawMesh.WedgeTangentY.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentY));
							RawMesh.WedgeTangentZ.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentZ));
							RawMesh.WedgeTexCoords[0].Add(VerticeBuffer[It].UV);
							RawMesh.WedgeColors.Add(FColor(255, 255, 255, 255));
						}

						uint32 TriNum = Sec->ProcIndexBuffer.Num() / 3;
						// copy face info
						for (uint32 TriIndex = 0; TriIndex < TriNum; TriIndex++)
						{
							RawMesh.FaceMaterialIndices.Add(0 + i);
							RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
						}
					}

					FVector PlaneNormal = FVector(0,0,1);
					AXRWallActor* WA = Cast<AXRWallActor>(SelectedActors[0]);
					FMatrix Mat = FMatrix::Identity;
					if (WA)
					{
						Mat = FMatrix(
							FVector(WA->AttachSurface->GetXDir()),
							FVector(WA->AttachSurface->GetYDir()),
							FVector(WA->AttachSurface->GetPlaneNormal()),
							FVector(0,0,0)
						);
						Mat = Mat.Inverse();
					}

					FArmyMeshUtilities::Get().BuildMeshLightMapUV(RawMesh, 256, Mat, bAutoWrap);

					//把计算好的LightMapUV填充会ProceduralMeshComponent中
					if (RawMesh.WedgeTexCoords[0].Num() == RawMesh.WedgeTexCoords[1].Num())
					{
						int32 BaseIndex = 0;
						for (int32 i = 0; i < MeshCOM->GetNumSections(); i++)
						{
							FArmyProcMeshSection* Sec = MeshCOM->GetProcMeshSection(i);
							TArray<FArmyProcMeshVertex>& VerticeBuffer = Sec->ProcVertexBuffer;
							const TArray<int32>& IndexBuffer = Sec->ProcIndexBuffer;

							for (int32 j = 0; j < IndexBuffer.Num(); j++)
							{
								VerticeBuffer[IndexBuffer[j]].LightMapUV = RawMesh.WedgeTexCoords[1][j + BaseIndex];
							}

							BaseIndex += IndexBuffer.Num();
						}
					}
				}
			}
		}
	}
}

void AXRPlayerController::EXE_ComputeLevelUV2InEditor()
{
	for (TObjectIterator<UXRProceduralMeshComponent> ObjIt; ObjIt; ++ObjIt)
	{
		UXRProceduralMeshComponent* MeshCOM = *ObjIt;
		//非CDO，非销毁状态的Component，将参与计算
		if (MeshCOM && !(MeshCOM->GetFlags() & RF_ArchetypeObject) && !(MeshCOM->GetFlags() &RF_BeginDestroyed))
		{
			FRawMesh RawMesh;

			FTransform RootTransform = MeshCOM->GetOwner()->GetTransform();
			FMatrix WorldToRoot = RootTransform.ToMatrixWithScale().Inverse();
			FMatrix ComponentToWorld = MeshCOM->GetComponentTransform().ToMatrixWithScale() * WorldToRoot;

			for (int32 i = 0; i < MeshCOM->GetNumSections(); i++)
			{
				FArmyProcMeshSection* Sec = MeshCOM->GetProcMeshSection(i);
				const int32 BaseVertexIndex = RawMesh.VertexPositions.Num();

				const TArray<FArmyProcMeshVertex>& VerticeBuffer = Sec->ProcVertexBuffer;
				const TArray<int32>& IndexBuffer = Sec->ProcIndexBuffer;

				for (auto& It : VerticeBuffer)
				{
					RawMesh.VertexPositions.Add(ComponentToWorld.InverseTransformPosition(It.Position));
				}

				for (auto& It : IndexBuffer)
				{
					RawMesh.WedgeIndices.Add(It + BaseVertexIndex);
					RawMesh.WedgeTangentX.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentX));
					RawMesh.WedgeTangentY.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentY));
					RawMesh.WedgeTangentZ.Add(ComponentToWorld.TransformVector(VerticeBuffer[It].TangentZ));
					RawMesh.WedgeTexCoords[0].Add(VerticeBuffer[It].UV);
					RawMesh.WedgeColors.Add(FColor(255, 255, 255, 255));
				}

				uint32 TriNum = Sec->ProcIndexBuffer.Num() / 3;
				// copy face info
				for (uint32 TriIndex = 0; TriIndex < TriNum; TriIndex++)
				{
					RawMesh.FaceMaterialIndices.Add(0 + i);
					RawMesh.FaceSmoothingMasks.Add(0); // Assume this is ignored as bRecomputeNormals is false
				}
			}

			FVector PlaneNormal = FVector(0, 0, 1);
			AXRWallActor* WA = Cast<AXRWallActor>(MeshCOM->GetOwner());
			FMatrix Mat = FMatrix::Identity;
			if (WA)
			{
				Mat = FMatrix(
					FVector(WA->AttachSurface->GetXDir()),
					FVector(WA->AttachSurface->GetYDir()),
					FVector(WA->AttachSurface->GetPlaneNormal()),
					FVector(0, 0, 0)
				);
				Mat = Mat.Inverse();
			}

			FArmyMeshUtilities::Get().BuildMeshLightMapUV(RawMesh, 256, Mat, false);

			//把计算好的LightMapUV填充会ProceduralMeshComponent中
			if (RawMesh.WedgeTexCoords[0].Num() == RawMesh.WedgeTexCoords[1].Num())
			{
				int32 BaseIndex = 0;
				for (int32 i = 0; i < MeshCOM->GetNumSections(); i++)
				{
					FArmyProcMeshSection* Sec = MeshCOM->GetProcMeshSection(i);
					TArray<FArmyProcMeshVertex>& VerticeBuffer = Sec->ProcVertexBuffer;
					const TArray<int32>& IndexBuffer = Sec->ProcIndexBuffer;

					for (int32 j = 0; j < IndexBuffer.Num(); j++)
					{
						VerticeBuffer[IndexBuffer[j]].LightMapUV = RawMesh.WedgeTexCoords[1][j + BaseIndex];
					}

					BaseIndex += IndexBuffer.Num();
				}
			}
		}
	}
}

void AXRPlayerController::EXE_DeleteSelectedActor()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	for(auto& It : SelectedActors)
	{
		GetWorld()->DestroyActor(It,false,false);
	}
}

void AXRPlayerController::EXE_MoveSelectedActor(float InX, float InY, float InZ)
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	for (auto& It : SelectedActors)
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(It);
		if (SMA)
		{
			SMA->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			SMA->AddActorWorldOffset(FVector(InX,InY,InZ));
		}
	}
}

void AXRPlayerController::EXE_SetDrawUVChannel(int32 InDrawUVChannel)
{
	FArmyWorldManager::Get().UVChannelToShow = InDrawUVChannel;

	/*UArmyEditorViewportClient* VC = Cast<UArmyEditorViewportClient>(GetWorld()->GetGameViewport());
	if (VC)
	{
		VC->DrawSelectedUVChannel = InDrawUVChannel;
	}*/
}

void AXRPlayerController::EXE_SetDrawUVComponentIndex(int32 InDrawUVComponentIndex)
{
	UArmyEditorViewportClient* VC = Cast<UArmyEditorViewportClient>(GetWorld()->GetGameViewport());
	if (VC)
	{
		VC->DrawSelectedUVComponentIndex = InDrawUVComponentIndex;
	}
}

void AXRPlayerController::EXE_AddDrawDebugPoint(float InX, float InY, float InZ)
{
	DrawDebugPoint(GetWorld(), FVector(InX, InY, InZ), 16.f, FColor::Red, true, -1.f);
}

void AXRPlayerController::EXE_SetLightMobility(int32 InLightType, int32 InMobilityType)
{
	EComponentMobility::Type NewMobility = EComponentMobility::Movable;

	if (InMobilityType == 0)
		NewMobility = EComponentMobility::Movable;
	else if (InMobilityType == 1)
		NewMobility = EComponentMobility::Static;
	else if (InMobilityType == 2)
		NewMobility = EComponentMobility::Stationary;

	if (InLightType == 0)
	{
		for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ADirectionalLight* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->GetLightComponent()->SetMobility(NewMobility);
			}
		}
	}
	else if (InLightType == 1)
	{
		for (TActorIterator<ASkyLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ASkyLight* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->GetLightComponent()->SetMobility(NewMobility);
			}
		}
	}
}

void AXRPlayerController::EXE_SetLightVisibility(int32 InType, bool InNewVisibility)
{
	if (InType == 0)
	{
		for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ADirectionalLight* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->GetLightComponent()->SetVisibility(InNewVisibility);
			}
		}
	}
	else if (InType == 1)
	{
		for (TActorIterator<ASkyLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ASkyLight* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->GetLightComponent()->SetVisibility(InNewVisibility);
			}
		}
	}
}

void AXRPlayerController::EXE_SetSelectedLightRadius(float InNewRadius)
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	for (auto& It : SelectedActors)
	{
		AXRPointLightActor* PA = Cast<AXRPointLightActor>(It);
		AXRSpotLightActor* PA2 = Cast<AXRSpotLightActor>(It);
		if (PA)
		{
			PA->PointLightCOM->SetMobility(EComponentMobility::Movable);
			PA->PointLightCOM->SetAttenuationRadius(InNewRadius);
			PA->PointLightCOM->SetMobility(EComponentMobility::Static);
		}
		else if (PA2)
		{
			PA2->SpotLightCOM->SetMobility(EComponentMobility::Movable);
			PA2->SpotLightCOM->SetAttenuationRadius(InNewRadius);
			PA2->SpotLightCOM->SetMobility(EComponentMobility::Static);
		}
	}
}

void AXRPlayerController::EXE_SetSelectedLightIntensity(float InIntensity)
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	for (auto& It : SelectedActors)
	{
		AXRPointLightActor* PA = Cast<AXRPointLightActor>(It);
		AXRSpotLightActor* PA2 = Cast<AXRSpotLightActor>(It);
		if (PA)
		{
			PA->PointLightCOM->SetMobility(EComponentMobility::Movable);
			PA->PointLightCOM->SetIntensity(InIntensity);
			PA->PointLightCOM->SetMobility(EComponentMobility::Static);
		}
		else if (PA2)
		{
			PA2->SpotLightCOM->SetMobility(EComponentMobility::Movable);
			PA2->SpotLightCOM->SetIntensity(InIntensity);
			PA2->SpotLightCOM->SetMobility(EComponentMobility::Static);
		}
	}
}

void AXRPlayerController::EXE_SetLightAffectWorld(int32 InType, bool InNewAffectWorld)
{
	if (InType == 0)
	{
		for (TActorIterator<ADirectionalLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ADirectionalLight* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->GetLightComponent()->bAffectsWorld = InNewAffectWorld;
			}
		}
	}
	else if (InType == 1)
	{
		for (TActorIterator<ASkyLight> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ASkyLight* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->GetLightComponent()->bAffectsWorld = InNewAffectWorld;
			}
		}
	}
}

void AXRPlayerController::EXE_GetAllComponents()
{
	TArray<UActorComponent*> Coms;
	for (TObjectIterator<UActorComponent> ComItr; ComItr; ++ComItr)
	{
		Coms.Add(*ComItr);
	}

	TArray<UMaterialInterface*> MIs;
	for (TObjectIterator<UMaterialInterface> ComItr; ComItr; ++ComItr)
	{
		MIs.Add(*ComItr);
	}
	int32 a = 0;
}

void AXRPlayerController::EXE_DeleteActor(int32 InType)
{
	if (InType == 0)
	{
		for (TActorIterator<ALightmassImportanceVolume> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ALightmassImportanceVolume* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->Destroy();
			}
		}
	}
	else if (InType == 1)
	{
		for (TActorIterator<ASphereReflectionCapture> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ASphereReflectionCapture* TheActor = *ActorItr;
			if (TheActor)
			{
				TheActor->Destroy();
			}
		}
	}
}

void AXRPlayerController::EXE_SetCubeMobility(int32 InType)
{
	for (TActorIterator<AStaticMeshActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AStaticMeshActor* TheActor = *ActorItr;
		if (TheActor->GetName().Contains("Cube"))
		{
			TheActor->SetMobility((EComponentMobility::Type)InType);
		}
	}
}

void AXRPlayerController::EXE_GenerateTestMesh()
{
	AXRShapeActor* TempActor = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0,0,0), FRotator::ZeroRotator);
	TempActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	
	TArray<FVector> VertexBuffer;
	//上
	VertexBuffer.Add(FVector(0, 100, 100));//0
	VertexBuffer.Add(FVector(100, 100, 100));//1
	VertexBuffer.Add(FVector(100, 0, 100));//2
	VertexBuffer.Add(FVector(0, 0, 100));//3
	//下
	VertexBuffer.Add(FVector(100, 100, 80));//4
	VertexBuffer.Add(FVector(0, 100, 80));//5
	VertexBuffer.Add(FVector(0, 0, 80));//6
	VertexBuffer.Add(FVector(100, 0, 80));//7
	//左
	VertexBuffer.Add(FVector(0, 100, 100));//8
	VertexBuffer.Add(FVector(0, 0, 100));//9
	VertexBuffer.Add(FVector(0, 0, 80));//10
	VertexBuffer.Add(FVector(0, 100, 80));//11
	//右
	VertexBuffer.Add(FVector(100, 0, 100));//12
	VertexBuffer.Add(FVector(100, 100, 100));//13
	VertexBuffer.Add(FVector(100, 100, 80));//14
	VertexBuffer.Add(FVector(100, 0, 80));//15
	//前
	VertexBuffer.Add(FVector(0, 0, 100));//16
	VertexBuffer.Add(FVector(100, 0, 100));//17
	VertexBuffer.Add(FVector(100, 0, 80));//18
	VertexBuffer.Add(FVector(0, 0, 80));//19
	//后
	VertexBuffer.Add(FVector(100, 100, 100));//20
	VertexBuffer.Add(FVector(0, 100, 100));//21
	VertexBuffer.Add(FVector(0, 100, 80));//22
	VertexBuffer.Add(FVector(100, 100, 80));//23

	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));

	UVs.Add(FVector2D(0, 0));
	UVs.Add(FVector2D(1, 0));
	UVs.Add(FVector2D(1, 1));
	UVs.Add(FVector2D(0, 1));
	
	
	TArray<int32> IndexBuffer;
	//上
	IndexBuffer.Add(0);
	IndexBuffer.Add(1);
	IndexBuffer.Add(2);
	IndexBuffer.Add(0);
	IndexBuffer.Add(2);
	IndexBuffer.Add(3);
	//下
	IndexBuffer.Add(4);
	IndexBuffer.Add(5);
	IndexBuffer.Add(6);
	IndexBuffer.Add(4);
	IndexBuffer.Add(6);
	IndexBuffer.Add(7);
	//左
	IndexBuffer.Add(8);
	IndexBuffer.Add(9);
	IndexBuffer.Add(10);
	IndexBuffer.Add(8);
	IndexBuffer.Add(10);
	IndexBuffer.Add(11);
	//右
	IndexBuffer.Add(12);
	IndexBuffer.Add(13);
	IndexBuffer.Add(14);
	IndexBuffer.Add(12);
	IndexBuffer.Add(14);
	IndexBuffer.Add(15);
	//前
	IndexBuffer.Add(16);
	IndexBuffer.Add(17);
	IndexBuffer.Add(18);
	IndexBuffer.Add(16);
	IndexBuffer.Add(18);
	IndexBuffer.Add(19);
	//后
	IndexBuffer.Add(20);
	IndexBuffer.Add(21);
	IndexBuffer.Add(22);
	IndexBuffer.Add(20);
	IndexBuffer.Add(22);
	IndexBuffer.Add(23);

	TempActor->SetGeometryBuffer(VertexBuffer, UVs, IndexBuffer);
}

void AXRPlayerController::EXE_GenerateTestMeshWithTools()
{
	//TestActor = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	//TestActor->Tags.Add("TestActor");
	//TestActor2 = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	//TestActor2->Tags.Add("TestActor1");
	////TestActor3 = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	////TestActor3->Tags.Add("TestActor2");

	//TempActor = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	//TempActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());

	//TArray<FVector> ShapePoints;
	//ShapePoints.Add(FVector(25, 75, 10));//0
	//ShapePoints.Add(FVector(25, 25, 10));//1
	//ShapePoints.Add(FVector(75, 25, 10));//2
	//ShapePoints.Add(FVector(75, 75, 10));//3
	//TempActor->AddBoardPoints(ShapePoints, 50);
	//TempActor->UpdateAllVetexBufferIndexBuffer();

	//TArray<FVector> ShapePoints2;
	//ShapePoints2.Add(FVector(0, 100, 0));//0
	//ShapePoints2.Add(FVector(0, 0, 0));//1
	//ShapePoints2.Add(FVector(100, 0, 0));//2
	//ShapePoints2.Add(FVector(100, 100, 0));//3
	//TempActor->AddBoardPoints3(ShapePoints2, 1);

	//TempActor->BoardMesh2 = NewObject<UXRGenMeshComponent>(TempActor);
	//TempActor->BoardMesh2->RegisterComponent();
	//TempActor->BoardMesh2->AttachToComponent(TempActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	//TempActor->BoardMesh2->SetMaterial(0, TempActor->BoardMesh->GetMaterial(0));
	//TempActor->BoardMesh2->SetGenerateMeshGeomInfo(TempActor->TotalVertexs2, TempActor->TotalTriangles2);
	//TempActor->BoardMesh2->LightMapResolution = 128;





	//TArray<FVector> ShapePoints;
	//ShapePoints.Add(FVector(-5, 10, 95));//0
	//ShapePoints.Add(FVector(-5, -10, 95));//1
	//ShapePoints.Add(FVector(5, -10, 95));//2
	//ShapePoints.Add(FVector(5, 10, 95));//3
	//TempActor->AddBoardPoints(ShapePoints, 50);
	//TempActor->UpdateAllVetexBufferIndexBuffer();

	//TArray<FVector> ShapePoints2;
	//ShapePoints2.Add(FVector(-10, 70, 90));//0
	//ShapePoints2.Add(FVector(-10, -70, 90));//1
	//ShapePoints2.Add(FVector(10, -70, 90));//2
	//ShapePoints2.Add(FVector(10, 70, 90));//3
	//TempActor->AddBoardPoints2(ShapePoints2, 5);

	//TArray<FVector> ShapePoints3;
	//ShapePoints3.Add(FVector(12, 70, 90));//0
	//ShapePoints3.Add(FVector(12, -70, 90));//1
	//ShapePoints3.Add(FVector(32, -70, 90));//2
	//ShapePoints3.Add(FVector(32, 70, 90));//3
	//TempActor->AddBoardPoints3(ShapePoints3, 5);

	//TArray<FArmyGenMeshVertex> VertexBuffer;
	//TArray<FArmyGenMeshTriangle> IndexBuffer;
	//TriangluatePoints(ShapePoints, 20.f, VertexBuffer, IndexBuffer);
	//TempActor->SetGeometryBufferDirectly(VertexBuffer, IndexBuffer);
}

void AXRPlayerController::EXE_GenerateTestMeshWithTools2()
{
	//AXRShapeActor* TempActor = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	//TempActor->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	//TArray<FVector> ShapePoints;
	//ShapePoints.Add(FVector(-80, 70, 90));//0
	//ShapePoints.Add(FVector(-80, -70, 90));//1
	//ShapePoints.Add(FVector(-60, -70, 90));//2
	//ShapePoints.Add(FVector(-60, 70, 90));//3
	//TempActor->AddBoardPoints(ShapePoints, 5);
	//TempActor->UpdateAllVetexBufferIndexBuffer();

	//AXRShapeActor* TempActor2 = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	//TempActor2->SetMaterial(FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	//TArray<FVector> ShapePoints2;
	//ShapePoints2.Add(FVector(-75, 10, 95));//0
	//ShapePoints2.Add(FVector(-75, -10, 95));//1
	//ShapePoints2.Add(FVector(-65, -10, 95));//2
	//ShapePoints2.Add(FVector(-65, 10, 95));//3
	//TempActor2->AddBoardPoints(ShapePoints2, 50);
	//TempActor2->UpdateAllVetexBufferIndexBuffer();
}

void AXRPlayerController::EXE_TestCreateProcMesh()
{
	//创建依附的Actor
	TempActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
	//USceneComponent* SceneComponent = NewObject<USceneComponent>(TempActor, "SceneComponent");
	//SceneComponent->RegisterComponent();
	//SceneComponent->AttachToComponent(TempActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	//创建组件
	UXRProceduralMeshComponent* ProcMeshComponent = NewObject<UXRProceduralMeshComponent>(TempActor, "TestProcMeshComponent");
	ProcMeshComponent->RegisterComponent();
	ProcMeshComponent->AttachToComponent(TempActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	TempActor->SetRootComponent(ProcMeshComponent);
	//填充轮廓线
	TArray<FVector> OutlineVetices;
	OutlineVetices.Add(FVector(0, 0, 10));//0
	OutlineVetices.Add(FVector(10, 0, 10));//1
	OutlineVetices.Add(FVector(10, 10, 10));//2
	OutlineVetices.Add(FVector(40, 10, 10));//3
	OutlineVetices.Add(FVector(40, 0, 10));//4
	OutlineVetices.Add(FVector(50, 0, 10));//5
	OutlineVetices.Add(FVector(50, 50, 10));//6
	OutlineVetices.Add(FVector(40, 50, 10));//7
	OutlineVetices.Add(FVector(40, 40, 10));//8
	OutlineVetices.Add(FVector(10, 40, 10));//9
	OutlineVetices.Add(FVector(10, 50, 10));//10
	OutlineVetices.Add(FVector(0, 50, 10));//10
	ProcMeshComponent->CreateMeshSection_Extrude(0, OutlineVetices, 10.f);

	//TArray<FVector> OutlineVetices2;
	//OutlineVetices2.Add(FVector(60, 50, 50));//0
	//OutlineVetices2.Add(FVector(60, 0, 50));//1
	//OutlineVetices2.Add(FVector(110, 0, 50));//2
	//OutlineVetices2.Add(FVector(110, 50, 50));//3
	//ProcMeshComponent->CreateMeshSection_Extrude(1, OutlineVetices2, 50.f);

	//设置材质
	ProcMeshComponent->SetMaterial(0, FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
	//UMaterial* M_Test = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/XRCommon/Material/Default/M_Concrete_Poured2.M_Concrete_Poured2"), NULL, LOAD_None, NULL));
	//ProcMeshComponent->SetMaterial(1, FArmyEngineModule::Get().GetEngineResource()->GetWallMaterial());
}

void AXRPlayerController::EXE_TestCreateOrUpdateProcMesh(int32 InSectionIndex)
{
	if (TempActor)
	{
		TArray<USceneComponent*> SceneComponents;
		TempActor->GetComponents(SceneComponents);
		for (auto& It : SceneComponents)
		{
			UXRProceduralMeshComponent* ProcCOM = Cast<UXRProceduralMeshComponent>(It);
			if (ProcCOM)
			{
				TArray<FVector> OutlineVetices;
				OutlineVetices.Add(FVector(0, 50, 10));//0
				OutlineVetices.Add(FVector(0, 0, 10));//1
				OutlineVetices.Add(FVector(50, 0, 10));//2
				OutlineVetices.Add(FVector(50, 50, 10));//3

				TArray<FArmyProcMeshVertex> SectionVertices;
				TArray<FArmyProcMeshTriangle> SectionTriangles;

				UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, 50, SectionVertices, SectionTriangles);

				ProcCOM->CreateMeshSection(InSectionIndex, SectionVertices, SectionTriangles, true, true);
			}
		}
	}
}

void AXRPlayerController::EXE_TestTransform()
{
	TSubclassOf<class AXRPointLightActor> SpotLightClass = StaticLoadClass(AXRSpotLightActor::StaticClass(), NULL, TEXT("/Game/XRCommon/Blueprint/BP_SpotLight.BP_SpotLight_C"), NULL, LOAD_None, NULL);
	UObject* ClassObj = StaticLoadClass(UObject::StaticClass(), NULL, TEXT("/Game/XRCommon/Blueprint/BP_Test.BP_Test_C"));
	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(ClassObj);
	AActor* TestActor = GetWorld()->SpawnActor<AActor>(BPClass, FVector(0,0,100), FRotator::ZeroRotator);
	if (TestActor)
	{
		FTransform RootTransform = TestActor->GetTransform();
		FMatrix RootMatrixNoScale = RootTransform.ToMatrixNoScale();
		FMatrix RootMatrix = RootTransform.ToMatrixWithScale();
		FMatrix WorldToRoot = RootMatrix.Inverse();
		
		TArray<USceneComponent*> Components;
		TestActor->GetComponents(Components);
		UStaticMeshComponent* SMC = NULL;
		for (auto& It : Components)
			SMC = Cast<UStaticMeshComponent>(It);

		FMatrix ComponentMatrix = SMC->GetComponentTransform().ToMatrixWithScale();
		FMatrix ComponentToWorld = SMC->GetComponentTransform().ToMatrixWithScale() * WorldToRoot;

		FVector A1 = FVector(0,0,0);

		FVector A2 = ComponentToWorld.TransformPosition(A1);
		FVector A3 = ComponentToWorld.InverseTransformPosition(A1);
	
		FVector B1 = FVector(1, 0, 0);
		FVector B2 = ComponentToWorld.TransformVector(B1);

		return;
	}
}

void AXRPlayerController::EXE_InvalidateSelectedLighting()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	for (auto& It : SelectedActors)
	{
		TArray<USceneComponent*> COMs;
		It->GetComponents(COMs);
		
		for (USceneComponent* It : COMs)
		{
			UXRProceduralMeshComponent* ProcCOM = Cast<UXRProceduralMeshComponent>(It);
			if (ProcCOM)
			{
				ProcCOM->InvalidateLightingCache();
				return;
			}
		}
	}
}

void AXRPlayerController::EXE_SetSelectedActorComponentVisibility(int32 ChildComponentIndex, bool bVisible)
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);
	for (auto& It : SelectedActors)
	{
		TArray<USceneComponent*> COMs;
		It->GetComponents(COMs);

		if (COMs.IsValidIndex(ChildComponentIndex))
		{
			COMs[ChildComponentIndex]->SetVisibility(bVisible);
		}
	}
}

void AXRPlayerController::EXE_SetSelectedLightMobility(bool bMoveable)
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.IsValidIndex(0))
	{
		TArray<USceneComponent*> COMs;
		SelectedActors[0]->GetComponents(COMs);

		for (USceneComponent* It : COMs)
		{
			ULightComponent* LCom = Cast<ULightComponent>(It);
			if(LCom)
				LCom->SetMobility(bMoveable ? EComponentMobility::Movable : EComponentMobility::Static);
		}
	}
}

void AXRPlayerController::EXE_SetEnvironmentMode(bool bDynamic)
{
	FArmyWorldManager::Get().SetEnvironmentMode(bDynamic);
}

void AXRPlayerController::EXE_SetNotificationPercent(int32 InPercent)
{
	if (!LightBuildNotification.IsValid())
	{
		FNotificationInfo Info(FText::FromString(TEXT("Test Percent")));
		Info.bFireAndForget = false;
		Info.bUseThrobber = true;
		LightBuildNotification = FSlateNotificationManager::Get().AddNotification(Info);
		LightBuildNotification.Pin()->SetCompletionState(SNotificationItem::CS_Pending);
	}
	
	FText Text = FText::FromString(FString::Printf(TEXT("Percent: %d"), InPercent));
	LightBuildNotification.Pin()->SetText(Text);
}

void AXRPlayerController::EXE_CollectActors()
{
	TArray<AXRWallActor*> WindowSides;
	TArray<AXRWallActor*> HelpAreaSides;
	TArray<AXRWallActor*> HoleSides;

	for (TActorIterator<AXRWallActor> It(GetWorld()); It; ++It)
	{
		AXRWallActor* XA = (*It);
		if (XA->AttachSurface.IsValid())
		{
			FArmyRoomSpaceArea* RSA = XA->AttachSurface->AsassignObj<FArmyRoomSpaceArea>();
			if (RSA)
			{
				if (RSA->GenerateFromObjectType == OT_Window ||
					RSA->GenerateFromObjectType == OT_FloorWindow ||
					RSA->GenerateFromObjectType == OT_RectBayWindow ||
					RSA->GenerateFromObjectType == OT_TrapeBayWindow ||
					RSA->GenerateFromObjectType == OT_CornerBayWindow
					)
				{
					WindowSides.Add(XA);
					continue;
					//RSA->SurfaceType == 0 1 2
					// if(RSA->AttachRoomID == )
				}
				else if (RSA->GenerateFromObjectType == OT_Door ||
					RSA->GenerateFromObjectType == OT_FloorWindow ||
					RSA->GenerateFromObjectType == OT_RectBayWindow ||
					RSA->GenerateFromObjectType == OT_TrapeBayWindow ||
					RSA->GenerateFromObjectType == OT_CornerBayWindow
					)
				{
					WindowSides.Add(XA);
					continue;
					//RSA->SurfaceType == 0 1 2
					// if(RSA->AttachRoomID == )
				}

			}
			FArmyHelpRectArea* RA = XA->AttachSurface->AsassignObj<FArmyHelpRectArea>();
			if (RA)
			{
				HelpAreaSides.Add(XA);
				continue;
			}
		}
	}

	int32 a = 0;
}

void AXRPlayerController::EXE_SetCubesDynamic()
{
	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		if (It->GetName().Contains(TEXT("Cube")))
		{
			It->bIsSelectable = true;
			//It->GetStaticMeshComponent()->SetCastShadow(false);
			//It->SetMobility(EComponentMobility::Movable);

			It->Tags.Reset();
			It->Tags.Add(FName("MoveableMeshActor"));
		}
	}
}

void AXRPlayerController::EXE_SetMaterialToSelected()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.IsValidIndex(0))
	{
		AStaticMeshActor* SMA = Cast<AStaticMeshActor>(SelectedActors[0]);
		if (SMA)
		{
			UMaterial* MI = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/XRCommon/Material/Default/M_Test2.M_Test2"), NULL, LOAD_None, NULL));
			SMA->GetStaticMeshComponent()->SetMaterial(0, MI);
		}
	}
}

void AXRPlayerController::EXE_HoldMap()
{
	ULevel* TheLevel = GetWorld()->GetCurrentLevel();
	TmpMapData = TheLevel->MapBuildData;
}

void AXRPlayerController::EXE_CreateDoorBlock()
{
	TArray<FObjectWeakPtr> DoorList;
	TArray<FObjectWeakPtr> DoorList2;
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_Door, DoorList);
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_SecurityDoor, DoorList2);

	DoorList.Append(DoorList2);

	for (auto Obj : DoorList)
	{
		FArmyHardware* Door = Obj.Pin()->AsassignObj<FArmyHardware>();
		if (Door)
		{
			AXRShapeActor* TempActor = GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
			UMaterial* M_Wall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/XRCommon/Material/Default/M_Test1.M_Test1"), NULL, LOAD_None, NULL));
			TempActor->SetMaterial(M_Wall);

			if (TempActor)
			{
				UXRProceduralMeshComponent* ProcCOM = TempActor->GetMeshComponent();
				if (ProcCOM)
				{
					FBox Bound = Door->GetBounds();
					FVector A = Bound.Min;
					FVector B = FVector(A.X, Bound.Max.Y, 0);
					FVector C = FVector(Bound.Max.X, Bound.Max.Y, 0);
					FVector D = FVector(Bound.Max.X, A.Y, 0);

					TArray<FVector> OutlineVetices;
					OutlineVetices.Add(A);
					OutlineVetices.Add(B);
					OutlineVetices.Add(C);
					OutlineVetices.Add(D);

					TArray<FArmyProcMeshVertex> SectionVertices;
					TArray<FArmyProcMeshTriangle> SectionTriangles;

					UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, -Door->GetHeight(), SectionVertices, SectionTriangles);

					ProcCOM->CreateMeshSection(0, SectionVertices, SectionTriangles, true, true);
					ProcCOM->SetLightmapResolution(4);
					ProcCOM->bCastHiddenShadow = true;
					TempActor->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

void AXRPlayerController::EXE_UpdateSelectedRS()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.IsValidIndex(0))
	{
		AXRReflectionCaptureActor* RCA = Cast<AXRReflectionCaptureActor>(SelectedActors[0]);
		if (RCA)
		{
			RCA->ReflectionCaptureCOM->DestroyRenderState_Concurrent();
			RCA->ReflectionCaptureCOM->CreateRenderState_Concurrent();
			RCA->ReflectionCaptureCOM->SetCaptureIsDirty();
			//FGlobalComponentRecreateRenderStateContext RecreateRenderState;
		}
	}
}

void AXRPlayerController::EXE_SetRSMovable()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.IsValidIndex(0))
	{
		AXRReflectionCaptureActor* RCA = Cast<AXRReflectionCaptureActor>(SelectedActors[0]);
		if (RCA)
		{
			RCA->ReflectionCaptureCOM->SetMobility(EComponentMobility::Movable);
		}
	}
}

void AXRPlayerController::EXE_SetRSStatic()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.IsValidIndex(0))
	{
		AXRReflectionCaptureActor* RCA = Cast<AXRReflectionCaptureActor>(SelectedActors[0]);
		if (RCA)
		{
			RCA->ReflectionCaptureCOM->SetMobility(EComponentMobility::Static);
		}
	}
}

void AXRPlayerController::EXE_UpdateReflectionCaptures()
{
	for (TObjectIterator<UStaticMeshComponent> It; It; ++It)
	{
		It->SetMobility(EComponentMobility::Static);
		FComponentReregisterContext ReregisterContext(*It);
	}

	for (TObjectIterator<USkeletalMeshComponent> It; It; ++It)
	{
		It->SetMobility(EComponentMobility::Static);
		FComponentReregisterContext ReregisterContext(*It);
	}
	GGI->GetWorld()->UpdateAllReflectionCaptures();
}

void AXRPlayerController::EXE_RecreateMeshState()
{
	for (TObjectIterator<UStaticMeshComponent> It; It; ++It)
	{
		It->SetMobility(EComponentMobility::Movable);
		FComponentReregisterContext ReregisterContext(*It);
	}

	for (TObjectIterator<USkeletalMeshComponent> It; It; ++It)
	{
		It->SetMobility(EComponentMobility::Movable);
		FComponentReregisterContext ReregisterContext(*It);
	}
}

void AXRPlayerController::EXE_GenerateDoorBlock()
{
	TArray<FObjectWeakPtr> DoorList;
	TArray<FObjectWeakPtr> DoorList2;
	//FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_Door, DoorList);
	FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, OT_SecurityDoor, DoorList2);

	DoorList.Append(DoorList2);

	for (auto Obj : DoorList)
	{
		FArmyHardware* Door = Obj.Pin()->AsassignObj<FArmyHardware>();
		if (Door)
		{
			AXRShapeActor* TempActor = GWorld->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector(0, 0, 0), FRotator::ZeroRotator);
			UMaterial* M_Wall = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), NULL, TEXT("/Game/XRCommon/Material/Default/M_Test1.M_Test1"), NULL, LOAD_None, NULL));

			if (TempActor)
			{
				UXRProceduralMeshComponent* ProcCOM = TempActor->GetMeshComponent();
				if (ProcCOM)
				{
					ProcCOM->Rename(TEXT("DoorBlockComponent"));
					ProcCOM->bBuildStaticLighting = true;
					ProcCOM->SetLightmapResolution(1024);
					ProcCOM->bCastHiddenShadow = true;

					// V2-----------------------------V3
					//  |							   |
					//	|  V7----------------------V6  |
					//	|	|						|  |
					//	|   |						|  |
					//  |   |                       |  |
					//  V1--V0          Pos        V5--V4

					FVector XDir = FVector(1, 0, 0);
					FVector YDir = FVector(0, 1, 0);
					FVector Origin = FVector(0, 0, 0);

					TArray<FVector> Verticies;
					Verticies.SetNum(8);

					Verticies[0] = Origin - XDir * Door->GetLength();
					Verticies[1] = Verticies[0] - XDir * 20.f;
					Verticies[2] = Verticies[1] + YDir * 100.f;
					Verticies[7] = Verticies[0] + YDir * 80.f;

					Verticies[5] = Origin + XDir * Door->GetLength();
					Verticies[4] = Verticies[5] + XDir * 20.f;
					Verticies[3] = Verticies[4] + YDir * 100.f;
					Verticies[6] = Verticies[5] + YDir * 80.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i].Z = -50.f;

					FRotator Rot = Door->GetDirection().Rotation();
					Rot.Yaw -= 90;
					FTransform Trans = FTransform(Rot.Quaternion(), Door->GetPos(), FVector(1, 1, 1));

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i] = Trans.TransformPosition(Verticies[i]);

					TArray<FVector> OutlineVetices;
					for (auto It : Verticies)
						OutlineVetices.Add(It);

					TArray<FArmyProcMeshVertex> SectionVertices;
					TArray<FArmyProcMeshTriangle> SectionTriangles;

					UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, -Door->GetHeight() - 100.f, SectionVertices, SectionTriangles);
					ProcCOM->CreateMeshSection(0, SectionVertices, SectionTriangles, true, true);

					//顶盖
					//		V1----------------------V2
					//		|						|
					//		|						|
					//		|                       |
					//		V0----------Pos---------V3
					Verticies.Reset();
					Verticies.SetNum(4);
					OutlineVetices.Reset();

					Verticies[0] = Origin - XDir * Door->GetLength();
					Verticies[1] = Verticies[0] + YDir * 80.f;
					Verticies[3] = Origin + XDir * Door->GetLength();
					Verticies[2] = Verticies[3] + YDir * 80.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i].Z = Door->GetHeight() + 50.f - 20.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i] = Trans.TransformPosition(Verticies[i]);

					for (auto It : Verticies)
						OutlineVetices.Add(It);

					UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, -20.f, SectionVertices, SectionTriangles);
					ProcCOM->CreateMeshSection(1, SectionVertices, SectionTriangles, true, true);

					//底盖
					//		V1----------------------V2
					//		|						|
					//		|						|
					//		|                       |
					//		V0----------Pos---------V3
					Verticies.Reset();
					Verticies.SetNum(4);
					OutlineVetices.Reset();

					Verticies[0] = Origin - XDir * (Door->GetLength() + 20);
					Verticies[1] = Verticies[0] + YDir * 100.f;
					Verticies[3] = Origin + XDir * (Door->GetLength() + 20);
					Verticies[2] = Verticies[3] + YDir * 100.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i].Z = -70.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i] = Trans.TransformPosition(Verticies[i]);

					for (auto It : Verticies)
						OutlineVetices.Add(It);

					UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, -20.f, SectionVertices, SectionTriangles);
					ProcCOM->CreateMeshSection(2, SectionVertices, SectionTriangles, true, true);

					//侧视图
					//	*-----------*
					//	| *---------|
					//	| |
					//	| |
					//	| |
					//	| |
					//	| |
					//	| |
					//	| |		 *------*
					//	| |		 |		| <==========过门石
					//	| |		 *------*
					//	| |			*---*
					//	| |			|	| <==========底部封底侧面，位于过门石下面的缝隙
					//	| |			|	|
					//	| *---------|	|
					//	*-----------V0--V3
					//
					//
					//
					//
					//顶视图
					//
					//
					//				*----------* <==========过门石
					//	V0----------|----pos---|----------V1
					//	|			|		   |		  |
					//	|			*----------*		  |
					//	V3--------------------------------V2						^ YDir
					//																|
					//																|
					//--------------------------------------------------------------------->XDir

					Verticies.Reset();
					Verticies.SetNum(4);
					OutlineVetices.Reset();

					Verticies[0] = Origin - XDir * (Door->GetLength() + 20);
					Verticies[1] = Origin + XDir * (Door->GetLength() + 20);
					Verticies[2] = Verticies[1] - YDir * 20.f;
					Verticies[3] = Verticies[0] - YDir * 20.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i].Z = -70.f;

					for (int32 i = 0; i < Verticies.Num(); i++)
						Verticies[i] = Trans.TransformPosition(Verticies[i]);

					for (auto It : Verticies)
						OutlineVetices.Add(It);

					UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, -70.f, SectionVertices, SectionTriangles);
					ProcCOM->CreateMeshSection(3, SectionVertices, SectionTriangles, true, true);

					ProcCOM->SetMaterial(0, M_Wall);
					ProcCOM->SetMaterial(1, M_Wall);
					ProcCOM->SetMaterial(2, M_Wall);
					ProcCOM->SetMaterial(3, M_Wall);
					//TempActor->SetActorHiddenInGame(true);
					//DoorBlocks.Add(TempActor);
				}
			}
		}
	}
}

void AXRPlayerController::EXE_InvalidateSelected()
{
	TArray<AActor*> SelectedActors;
	GXREditor->GetSelectedActors(SelectedActors);

	if (SelectedActors.IsValidIndex(0))
	{
		TArray<USceneComponent*> SceneComponents;
		SelectedActors[0]->GetComponents(SceneComponents);
		for (auto& It : SceneComponents)
		{
			UXRProceduralMeshComponent* ProcCOM = Cast<UXRProceduralMeshComponent>(It);
			if (ProcCOM)
			{
				ProcCOM->InvalidateLightingCacheDetailed(true, false);
			}
		}
	}
}

void AXRPlayerController::EXE_ShowAdvancedWorldSettings(bool bVisible /*= true*/)
{
	FArmyWorldManager::Get().bShowAdvancedWorldSettings = bVisible;
}

void AXRPlayerController::EXE_SetAllPointLightIntensity(float InValue)
{
	for (TActorIterator<AXRPointLightActor> It(GetWorld()); It; ++It)
	{
		It->PointLightCOM->SetIntensity(InValue);
	}
}

void AXRPlayerController::EXE_SetAllSpotLightIntensity(float InValue)
{
	for (TActorIterator<AXRSpotLightActor> It(GetWorld()); It; ++It)
	{
		It->SpotLightCOM->SetIntensity(InValue);
	}
}

void AXRPlayerController::EXE_SetQualityLevel(int32 InLevel)
{
	Scalability::FQualityLevels CurLevels = Scalability::GetQualityLevels();
	CurLevels.SetFromSingleQualityLevel(InLevel);
	Scalability::SetQualityLevels(CurLevels);
}

void AXRPlayerController::EXE_SetAllStaticMeshMobility(bool bDynamic)
{
	for (TActorIterator<AStaticMeshActor> It(GWorld); It; ++It)
	{
		if (It && It->Tags.Num() > 0 && It->Tags[0] == TEXT("MoveableMeshActor"))
		{
			It->GetStaticMeshComponent()->SetMobility(bDynamic ? EComponentMobility::Movable:EComponentMobility::Static);
		}
	}
}

void AXRPlayerController::EXE_DirectionalLight_Intensity(float InValue)
{
	FArmyWorldManager::Get().DirectionalLight_Intensity(InValue);
}

void AXRPlayerController::EXE_DirectionalLight_Color(int32 R, int32 G, int32 B)
{
	FArmyWorldManager::Get().DirectionalLight_Color(R,G,B);
}

void AXRPlayerController::EXE_DirectionalLight_IndirectIntensity(float InValue)
{
	FArmyWorldManager::Get().DirectionalLight_IndirectIntensity(InValue);
}

void AXRPlayerController::EXE_DirectionalLight_SourceAngle(float InValue)
{
	FArmyWorldManager::Get().DirectionalLight_SourceAngle(InValue);
}

void AXRPlayerController::EXE_DirectionalLight_ShadowExponent(float InValue)
{
	FArmyWorldManager::Get().DirectionalLight_ShadowExponent(InValue);
}

void AXRPlayerController::EXE_DirectionalLight_Rotation(int32 InPitch, int32 InYaw, int32 InRoll)
{
	FArmyWorldManager::Get().DirectionalLight_Rotation(InPitch, InYaw, InRoll);
}

void AXRPlayerController::EXE_Postprocess_AutoExposure(float InMin, float InMax, float InUp, float InDown)
{
	FArmyWorldManager::Get().Postprocess_AutoExposure(InMin, InMax, InUp, InDown);
}

void AXRPlayerController::EXE_Postprocess_AutoExposureBias(float InValue)
{
	FArmyWorldManager::Get().Postprocess_AutoExposureBias(InValue);
}

void AXRPlayerController::EXE_Postprocess_IndirectLightingIntensity(float InValue)
{
	FArmyWorldManager::Get().Postprocess_IndirectLightingIntensity(InValue);
}

void AXRPlayerController::EXE_WorldSetting_StaticLightingLevelScale(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_StaticLightingLevelScale(InValue);
}

void AXRPlayerController::EXE_WorldSetting_NumIndirectLightingBounces(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_NumIndirectLightingBounces(InValue);
}

void AXRPlayerController::EXE_WorldSetting_NumSkyLightingBounces(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_NumSkyLightingBounces(InValue);
}

void AXRPlayerController::EXE_WorldSetting_IndirectLightingQuality(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_IndirectLightingQuality(InValue);
}

void AXRPlayerController::EXE_WorldSetting_IndirectLightingSmoothness(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_IndirectLightingSmoothness(InValue);
}

void AXRPlayerController::EXE_WorldSetting_EnvironmentColor(int32 R, int32 G, int32 B)
{
	FArmyWorldManager::Get().WorldSetting_EnvironmentColor(R,G,B);
}

void AXRPlayerController::EXE_WorldSetting_EnvironmentIntensity(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_EnvironmentIntensity(InValue);
}

void AXRPlayerController::EXE_WorldSetting_DiffuseBoost(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_DiffuseBoost(InValue);
}

void AXRPlayerController::EXE_WorldSetting_VolumetricLightmapDetailCellSize(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_VolumetricLightmapDetailCellSize(InValue);
}

void AXRPlayerController::EXE_WorldSetting_VolumetricLightmapMaximumBirckMemoryMb(float InValue)
{
	FArmyWorldManager::Get().WorldSetting_VolumetricLightmapMaximumBirckMemoryMb(InValue);
}

void AXRPlayerController::EXE_WorldSetting_AO(float InDirectillOFraction, float InIndirectillOFraction, float InOExponent, float InFullyOSimplesFraction, float InMaxODistance)
{
	FArmyWorldManager::Get().WorldSetting_AO(InDirectillOFraction, InIndirectillOFraction, InOExponent, InFullyOSimplesFraction, InMaxODistance);
}

void AXRPlayerController::EXE_WorldSetting_SSR(float InIntensity, float InQuality, float InMaxRoughness)
{
	FArmyWorldManager::Get().WorldSetting_SSR(InIntensity, InQuality, InMaxRoughness);
}

void AXRPlayerController::EXE_CeilingLampIntensity(float InValue)
{
	FArmyWorldManager::Get().CeilingLampIntensity(InValue);
}

void AXRPlayerController::MoveForward(float Val)
{
	if (Val != 0.f)
	{
		int32 a = 0;
		int32 b = 0;
	}
}

void AXRPlayerController::OnViewChanged(int32 InNewValue, int32 InOldValue)
{
	switch (InNewValue)
	{
	case EXRView_TOP:
	case EXRView_CEILING:
	{
		if (InNewValue == EXRView_CEILING)
		{
			FArmyObject::SetDrawModel(MODE_CEILING, true);
			FArmyObject::SetDrawModel(MODE_TOPVIEW, false);
		}
		else
		{
			FArmyObject::SetDrawModel(MODE_TOPVIEW, true);
			FArmyObject::SetDrawModel(MODE_CEILING, false);
		}

		TArray<TWeakPtr<FArmyObject>> TempObjs = FArmySceneData::Get()->GetObjects(E_LayoutModel);
		TArray<TWeakPtr<FArmyObject>> ResultObjs;
		for (auto It : TempObjs)
		{
			if (TypeArray.Contains(It.Pin()->GetType()))
			{
				AActor* RelActor = It.Pin()->GetRelevanceActor();
				if (RelActor && RelActor->IsValidLowLevel())
				{
					RelActor->SetActorHiddenInGame(true);
				}
				ResultObjs.Add(It);
			}
		}
		TArray<TWeakPtr<FArmyObject>> TempTotalAddWall;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, TempTotalAddWall);
		for (TWeakPtr<FArmyObject> iter : TempTotalAddWall)
		{
			FArmyAddWall* tempAddWall = iter.Pin()->AsassignObj<FArmyAddWall>();
			if (tempAddWall &&tempAddWall->GetIsHalfWall())
			{
				ResultObjs.Emplace(iter);
			}
		}
		
		FArmySceneData::Get()->AddToGlobal(ResultObjs);

		FArmyWorldManager::Get().SetOutdoorMeshVisibile(false);
	}
	break;

	default:
	{
		FArmyObject::SetDrawModel(MODE_TOPVIEW, false);
		FArmyObject::SetDrawModel(MODE_CEILING, false);
		TArray<TWeakPtr<FArmyObject>> TempObjs = FArmySceneData::Get()->GetObjects(E_LayoutModel);
		for (auto It : TempObjs)
		{
			if (TypeArray.Contains(It.Pin()->GetType()))
			{
				AActor* RelActor = It.Pin()->GetRelevanceActor();
				if (RelActor && RelActor->IsValidLowLevel())
				{
					RelActor->SetActorHiddenInGame(false);
				}
			}
		}
		FArmySceneData::Get()->EmptyGlobal();

		FArmyWorldManager::Get().SetOutdoorMeshVisibile(true);
	}
	break;
	}

	// 取消选中
	GXREditor->SelectNone(true, true, false);
	OnViewChangedDelegate.Broadcast(InNewValue, InOldValue);
}

void AXRPlayerController::OnViveLeftTrigger()
{
	int32 a = 0;
}

void AXRPlayerController::OnViveRightTrigger()
{
	if (bVRModeEnabled || bVRTestModeEnabled)
	{
		if (bVRTeleportEnabled)
		{
			FRotator Rot = GetMyPawn()->GetActorRotation();
			GetMyPawn()->SetActorLocationAndRotation(VRTeleportLoc, Rot, false);
			GetMyPawn()->GetCharacterMovement()->bJustTeleported = true;
			//UGameplayStatics::GetPlayerCameraManager(this, 0)->StartCameraFade(1, 0, 0.5, FLinearColor::Black);
		}
		else if (HTCInteractiveActor)
		{
			UFunction* Function = HTCInteractiveActor->FindFunction(FName(TEXT("OnToggle")));
			if (Function)
			{
				HTCInteractiveActor->ProcessEvent(Function, nullptr);
			}
		}
	}
}

void AXRPlayerController::FreezeDefaultLevelActor()
{
	for (FActorIterator ActorIt(GetWorld()); ActorIt; ++ActorIt)
	{
		ActorIt->bIsSelectable = false;
	}
}
