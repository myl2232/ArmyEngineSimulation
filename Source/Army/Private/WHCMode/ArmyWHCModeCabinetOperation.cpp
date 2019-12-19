#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyWHCCabinetChangeList.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyShapeTableActor.h"
#include "ArmyShapeRuler.h"
#include "common/XRShapeUtilities.h"
#include "ArmyMathFunc.h"

#include "Engine/StaticMesh.h"
#include "SCTCabinetShape.h"
#include "SCTModelShape.h"
#include "SCTDoorGroup.h"
#include "SCTSlidingDrawerShape.h"
#include "SCTEmbededElectricalGroup.h"
#include "SCTSpaceShape.h"
#include "SCTShapeManager.h"
#include "SCTResManager.h"
#include "SCTAnimation.h"
#include "UtilityTools/SCTShapeUtilityTools.h"
#include "Actor/SCTShapeActor.h"
#include "Actor/SCTShapeBoxActor.h"
#include "Actor/TypedPrimitive.h"

#include "Model/XRHomeData.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "Data/XRRoom.h"
#include "Data/XRConstructionQuantity.h"
#include "ArmyActor/XRWallActor.h" 
#include "Data/HardModeData/EditArea/XRBaseArea.h"
#include "Data/XRFurniture.h"
#include "ArmyToolsModule.h"
#include "Frame/SArmyWindow.h"
#include "Frame/XRDesignEditor.h"
#include "Game/XRGameInstance.h"
#include "Actor/XRActorConstant.h"
#include "ArmyActorVisitorFactory.h"
#include "ArmyEditorViewportClient.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/CollisionProfile.h"
#include "ArmyViewportClient.h"
#include "Engine/StaticMeshActor.h"
#include "HitProxies.h"
#include "UnrealClient.h"
#include "EngineUtils.h"

FArmyWHCModeCabinetOperation::FArmyWHCModeCabinetOperation()
	: OpState(ECabinetOperationState::Idle)
	, MouseButtonFlag(0)
	, RulerVisibilityFlag(0)
	, RulerCreationFlag(0)
	, HideCommandFlag(0)
	, PlacingShape(nullptr)
	, LastSelected(nullptr)
	, LastChildItemSelected(nullptr)
	, mbFirstMoveShape(false)
	, SelectedWireframe(nullptr)
{
	FArmySceneData::Get()->GetWHCModeListData.BindRaw(this, &FArmyWHCModeCabinetOperation::CabinetList);

	MoveCommandList.Emplace(MakeShareable(new FRegularMoveCommand(this)));
	MoveCommandList.Emplace(MakeShareable(new FVentilatorCabOverlappedCommand(this)));
	MoveCommandList.Emplace(MakeShareable(new FVentilatorCabAlignedCommand(this)));
}

FArmyWHCModeCabinetOperation::~FArmyWHCModeCabinetOperation()
{
	ClearScene();
}

void FArmyWHCModeCabinetOperation::OnQuit()
{
	GGI->Window->ShowInteractiveMessage(false);
}

void FArmyWHCModeCabinetOperation::BeginMode()
{
	if (OpState == ECabinetOperationState::AboutToPlaceCabinet ||
		OpState == ECabinetOperationState::PlacingCabinet)
		OpState = ECabinetOperationState::Idle;
	else if (FArmyWHCabinetMgr::Get()->GetPlaceShapes().Num() > 0)
		OpState = ECabinetOperationState::CabinetPlaced;

	RulerVisibilityFlag |= ERulerVF_Env;

	ShowFurniture((HideCommandFlag & EHideCF_Furniture) == 0);
	ShowHydropower((HideCommandFlag & EHideCF_Hydropower) == 0);

	SpawnShapeWidthRulers();
	SpawnShapeDistanceRulers();

	UWorld * World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	SelectedWireframe = World->SpawnActor<ASCTWireframeActor>(ASCTWireframeActor::StaticClass());
	SelectedWireframe->bIsSelectable = 0;
	SelectedWireframe->SetActorHiddenInGame(true);
	SelectedWireframe->SetIsIsDynamicScaleLineSizeFollowCamera(false);
	SelectedWireframe->SetLineWidth(0.2f);
	SelectedWireframe->Tags.Emplace(XRActorTag::WHCActor);

	UObject* MtlObj = StaticLoadObject(UObject::StaticClass(), NULL, TEXT("Material'/Game/XRCommon/Material/M_ForegroundBaseColorUnlit.M_ForegroundBaseColorUnlit'"));
	if (MtlObj && MtlObj->IsA<UMaterialInterface>())
	{
		UMaterialInstanceDynamic * MtlDyn = UMaterialInstanceDynamic::Create(Cast<UMaterialInterface>(MtlObj), nullptr);
		MtlDyn->SetVectorParameterValue("BaseColor", FLinearColor::Yellow);
		SelectedWireframe->SetLineMaterial(MtlDyn);
	}
	SelectedWireframe->SetActorHiddenInGame(true);
}

void FArmyWHCModeCabinetOperation::EndMode()
{
	RulerVisibilityFlag = 0;
	DestroyShapeWidthRulers();
	DestroyShapeDistanceRulers();

	HideCommandFlag &= ~EHideCF_DoorGroup;
	HideCommandFlag &= ~EHideCF_OnGroundCab;
	HideCommandFlag &= ~EHideCF_HangCab;
	ShowCabinetDoors(true);
	ShowOnGroundCabinets(true, true);
	ShowHangCabinets(true, true);

	ResetOperation();

	SelectedWireframe->Destroy();
	SelectedWireframe = nullptr;

	GGI->Window->ShowInteractiveMessage(false);
	GGI->Window->HideMessage();
}

void FArmyWHCModeCabinetOperation::BeginOperation(FCabinetInfo *InCabInfo, FCabinetWholeComponent *InCabWholeComponent)
{
	ResetOperation();
	OpState = ECabinetOperationState::Idle;
	TSharedPtr<FSCTShape> *Result = FWHCModeGlobalData::CabinetTemplateMap.Find(InCabInfo->Id);
	if (Result != nullptr)
	{
		PlacingShape = new FShapeInRoom;
		PlacingShape->Shape = FSCTShapeManager::Get()->CopyShapeToNew(*Result);
		PlacingShape->Shape->SetShapeWidth(InCabInfo->WidthInfo.Current);
		PlacingShape->Shape->SetShapeDepth(InCabInfo->DepthInfo.Current);
		PlacingShape->Shape->SetShapeHeight(InCabInfo->HeightInfo.Current);
		PlacingShape->Initialize(InCabInfo, InCabWholeComponent);

		PlacingShape->InitializeParams(InCabInfo);

		OpState = ECabinetOperationState::AboutToPlaceCabinet;

		GGI->Window->ShowMessageWithoutTimeout(MT_Normal, TEXT("柜子生成中"));
	}
}

void FArmyWHCModeCabinetOperation::ResetOperation()
{
	MouseButtonFlag = 0;

	if (PlacingShape != nullptr)
	{
		if (IsOverlappedPairByActive(PlacingShape))
			RemoveOverlappedPairByActive(PlacingShape);
		if (IsAlignedPairByActive(PlacingShape))
			RemoveAlignedPairByActive(PlacingShape);
		delete PlacingShape;
		PlacingShape = nullptr;
	}
	if (LastSelected)
	{
		if (LastSelected->GetClass() == AXRShapeTableActor::StaticClass())
		{
			((AXRShapeTableActor*)LastSelected)->SetIsSelected(false);
		}
		else if (LastSelected->GetClass() == AXRWhcGapActor::StaticClass())
		{
			((AXRWhcGapActor*)LastSelected)->SetIsSelected(false);
		}
		else if (LastSelected->GetClass() == AXRElecDeviceActor::StaticClass())
		{
			((AXRElecDeviceActor*)LastSelected)->SetIsSelected(false);
		}
	}
	LastSelected = nullptr;
	LastChildItemSelected = nullptr;
}

void FArmyWHCModeCabinetOperation::ClearScene()
{
	OpState = ECabinetOperationState::Idle;
	RulerVisibilityFlag = 0;
	HideCommandFlag = 0;
	DestroyShapeWidthRulers();
	DestroyShapeDistanceRulers();
	ResetOperation();
	FArmyWHPlaceShapes.Empty();
	OverlappedMap.Empty();
	AlignedMap.Empty();
	gFArmyWHCabinMgr->OnClear();
	FrontBoardCabModifier.ClearFrontBoardCabs();
	if (SelectedWireframe != nullptr)
		SelectedWireframe->SetActorHiddenInGame(true);

	GGI->Window->ShowInteractiveMessage(false);
}

void FArmyWHCModeCabinetOperation::ClearCabinet()
{

}

void FArmyWHCModeCabinetOperation::ClearWardrobe()
{

}

void FArmyWHCModeCabinetOperation::ClearOther()
{

}

void FArmyWHCModeCabinetOperation::Draw(const FSceneView *View, FPrimitiveDrawInterface *PDI)
{
	AXRShapeFrame *ShapeFrame = GetShapeFrameFromSelected();
	if (ShapeFrame == nullptr && PlacingShape == nullptr)
		return;

	FShapeInRoom * RESTRICT CurrentShapeInRoom = nullptr;
	if (PlacingShape != nullptr)
		CurrentShapeInRoom = PlacingShape;
	else if (ShapeFrame != nullptr)
		CurrentShapeInRoom = ShapeFrame->ShapeInRoomRef;

	check(CurrentShapeInRoom != nullptr);

	TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
	if (FShapeInRoom *const *PassiveShapePtr = OverlappedMap.Find(CurrentShapeInRoom))
	{
		const FShapeInRoom *PassiveShape = *PassiveShapePtr;
		FVector YDir = CurrentShapeInRoom->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector);
		FVector Start = CurrentShapeInRoom->ShapeFrame->GetActorLocation() + YDir * CurrentShapeInRoom->Shape->GetShapeDepth() * 0.1f;
		Start.Z = FArmySceneData::Get()->WallHeight;
		FVector End = Start;
		End.Z = 0.0f;
		DrawDashedLine(PDI, Start, End, FLinearColor(0.0f, 1.0f, 1.0f), 2.0f, SDPG_Foreground, 0.0f);

		YDir = PassiveShape->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector);
		// Start = PassiveShape->ShapeFrame->GetActorLocation() + YDir * PassiveShape->Shape->GetShapeDepth() * 0.1f;
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PassiveShape->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		Start = Accessory->Actor->GetActorLocation();
		Start.Z = FArmySceneData::Get()->WallHeight;
		End = Start;
		End.Z = 0.0f;
		PDI->DrawLine(Start, End, FLinearColor(1.0f, 0.0f, 0.0f), SDPG_Foreground, 0.0f, 0.0f);
	}
	else if (const FAlignedShapeInfo *AlignedInfoPtr = AlignedMap.Find(CurrentShapeInRoom))
	{
		FVector YDir = CurrentShapeInRoom->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector);
		FVector Start = CurrentShapeInRoom->ShapeFrame->GetActorLocation() + YDir * CurrentShapeInRoom->Shape->GetShapeDepth() * 0.1f;
		Start.Z = FArmySceneData::Get()->WallHeight;
		FVector End = Start;
		End.Z = 0.0f;
		DrawDashedLine(PDI, Start, End, FLinearColor(0.0f, 1.0f, 1.0f), 2.0f, SDPG_Foreground, 0.0f);
	}
}

void FArmyWHCModeCabinetOperation::ShowShapeWidthRuler(bool bShow)
{
	if (bShow)
		RulerVisibilityFlag |= ERulerVF_Self;
	else
		RulerVisibilityFlag &= ~ERulerVF_Self;


	for (auto &ShapeInRoom : FArmyWHPlaceShapes)
	{
		XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = ShapeInRoom->CabinetActor;
		TSharedPtr<class FShapeRuler> ShapeWidthRuler = RulerComponent->Get();
		if (ShapeWidthRuler.IsValid())
			ShapeWidthRuler->ShowRuler(bShow);
	}
}

void FArmyWHCModeCabinetOperation::UpdateShapeRulerPosition()
{
	if ((RulerVisibilityFlag & ERulerVF_Self) != 0)
	{
		for (auto &ShapeInRoom : FArmyWHPlaceShapes)
		{
			XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = ShapeInRoom->CabinetActor;
			TSharedPtr<class FShapeRuler> ShapeWidthRuler = RulerComponent->Get();
			ShapeWidthRuler->UpdateRulerPositionPerFrame();
		}
	}
}

void FArmyWHCModeCabinetOperation::SpawnShapeWidthRulers()
{
	for (auto &ShapeInRoom : FArmyWHPlaceShapes)
	{
		ShapeInRoom->SpawnShapeWidthRuler();
	}

	RulerCreationFlag |= ERulerVF_Self;
}

void FArmyWHCModeCabinetOperation::DestroyShapeWidthRulers()
{
	for (auto &ShapeInRoom : FArmyWHPlaceShapes)
	{
		if (ShapeInRoom->CabinetActor.IsValid())
		{
			XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = ShapeInRoom->CabinetActor;
			TSharedPtr<class FShapeRuler> ShapeWidthRuler = RulerComponent->Get();
			if (ShapeWidthRuler.IsValid())
			{
				ShapeWidthRuler->GetRulerActor()->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
				ShapeWidthRuler.Reset();
			}
		}
	}

	RulerCreationFlag &= ~ERulerVF_Self;
}

void FArmyWHCModeCabinetOperation::ShowShapeDistanceRuler(bool bShow)
{
	if (bShow)
		RulerVisibilityFlag |= ERulerVF_Env;
	else
		RulerVisibilityFlag &= ~ERulerVF_Env;

	if ((RulerCreationFlag & ERulerVF_Env) == 0)
		SpawnShapeDistanceRulers();

	AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected();
	if (ShapeFrame != nullptr)
	{
		if (bShow)
		{
			// 当要显示距离标尺之前，要重设距离标尺所绑定到的父ShapeFrame
			SwitchDistanceRulerBinding(ShapeFrame->ShapeInRoomRef);
			CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		}
		ShowShapeDistanceRulerInternal(bShow);
	}
}

void FArmyWHCModeCabinetOperation::UpdateShapeDistanceRulerPosition()
{
	if ((RulerVisibilityFlag & ERulerVF_Env) != 0)
	{
		LeftDistanceRuler->UpdateRulerPositionPerFrame();
		RightDistanceRuler->UpdateRulerPositionPerFrame();
		BackDistanceRuler->UpdateRulerPositionPerFrame();
		RoofDistanceRuler->UpdateRulerPositionPerFrame();
		FloorDistanceRuler->UpdateRulerPositionPerFrame();

		LeftObjDistanceRuler->UpdateRulerPositionPerFrame();
		RightObjDistanceRuler->UpdateRulerPositionPerFrame();
		BackObjDistanceRuler->UpdateRulerPositionPerFrame();
		RoofObjDistanceRuler->UpdateRulerPositionPerFrame();
		FloorObjDistanceRuler->UpdateRulerPositionPerFrame();
	}
}

void FArmyWHCModeCabinetOperation::SpawnShapeDistanceRulers()
{
#define SPAWN_DISTANCE_RULER(RulerObj, AlignDirection, Function) \
    if (!RulerObj.IsValid()) { \
        RulerObj = MakeShareable(new FShapeRuler(AlignDirection, GVC->ViewportOverlayWidget)); \
        RulerObj->SetRulerAlignment(XRWHCMode::ERulerAlignment::RA_Center); \
        RulerObj->OnRulerLengthChanged.BindRaw(this, &Function); \
    }   

	SPAWN_DISTANCE_RULER(LeftDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_X, FArmyWHCModeCabinetOperation::Callback_LeftRulerLengthChanged)
		SPAWN_DISTANCE_RULER(RightDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_X, FArmyWHCModeCabinetOperation::Callback_RightRulerLengthChanged)
		SPAWN_DISTANCE_RULER(RoofDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_Z, FArmyWHCModeCabinetOperation::Callback_RoofRulerLengthChanged)
		SPAWN_DISTANCE_RULER(FloorDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_Z, FArmyWHCModeCabinetOperation::Callback_FloorRulerLengthChanged)
		SPAWN_DISTANCE_RULER(BackDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_Y, FArmyWHCModeCabinetOperation::Callback_BackRulerLengthChanged)

		SPAWN_DISTANCE_RULER(LeftObjDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_X, FArmyWHCModeCabinetOperation::Callback_LeftObjRulerLengthChanged)
		SPAWN_DISTANCE_RULER(RightObjDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_X, FArmyWHCModeCabinetOperation::Callback_RightObjRulerLengthChanged)
		SPAWN_DISTANCE_RULER(BackObjDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_Y, FArmyWHCModeCabinetOperation::Callback_BackObjRulerLengthChanged)
		SPAWN_DISTANCE_RULER(RoofObjDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_Z, FArmyWHCModeCabinetOperation::Callback_RoofObjRulerLengthChanged)
		SPAWN_DISTANCE_RULER(FloorObjDistanceRuler, XRWHCMode::ERulerAlignDirection::RAD_Z, FArmyWHCModeCabinetOperation::Callback_FloorObjRulerLengthChanged)

		RulerCreationFlag |= ERulerVF_Env;
}

void FArmyWHCModeCabinetOperation::DestroyShapeDistanceRulers()
{
	ClearDistanceRulerBinding();
	LeftDistanceRuler.Reset();
	RightDistanceRuler.Reset();
	RoofDistanceRuler.Reset();
	FloorDistanceRuler.Reset();
	BackDistanceRuler.Reset();
	LeftObjDistanceRuler.Reset();
	RightObjDistanceRuler.Reset();
	BackObjDistanceRuler.Reset();
	RoofObjDistanceRuler.Reset();
	FloorObjDistanceRuler.Reset();

	RulerCreationFlag &= ~ERulerVF_Env;
}

float FArmyWHCModeCabinetOperation::GetSelectedShapeLeftMoveRange() const
{
	// float ObjRange = MAX_FLT;
	// if (LeftObjDistanceRuler.IsValid() && LeftObjDistanceRuler->IsRulerVisible())
	//     ObjRange = LeftObjDistanceRuler->GetRulerLength();
	// float Range = MAX_FLT;
	// if (LeftDistanceRuler.IsValid() && LeftDistanceRuler->IsRulerVisible())
	//     Range = LeftDistanceRuler->GetRulerLength();
	return FMath::Min(LeftDistanceCache, LeftObjDistanceCache);
}

float FArmyWHCModeCabinetOperation::GetSelectedShapeRightMoveRange() const
{
	// float ObjRange = MAX_FLT;
	// if (RightObjDistanceRuler.IsValid() && RightObjDistanceRuler->IsRulerVisible())
	// ObjRange = RightObjDistanceRuler->GetRulerLength();
	// float Range = MAX_FLT;
	// if (RightDistanceRuler.IsValid() && RightDistanceRuler->IsRulerVisible())
	// Range = RightDistanceRuler->GetRulerLength();
	return FMath::Min(RightDistanceCache, RightObjDistanceCache);
}

float FArmyWHCModeCabinetOperation::GetSelectedShapeBackRange() const
{
	// float ObjRange = MAX_FLT;
	// if (BackObjDistanceRuler.IsValid() && BackObjDistanceRuler->IsRulerVisible())
	// ObjRange = BackObjDistanceRuler->GetRulerLength();
	// float Range = MAX_FLT;
	// if (BackDistanceRuler.IsValid() && BackDistanceRuler->IsRulerVisible())
	// Range = BackDistanceRuler->GetRulerLength();
	return FMath::Min(BackDistanceCache, BackObjDistanceCache);
}

void FArmyWHCModeCabinetOperation::MoveShapeLeft(float Delta)
{
	AXRShapeFrame *ShapeFrame = GetShapeFrameFromSelected();
	if (ShapeFrame != nullptr)
	{
		FRotator Rotator = ShapeFrame->GetActorRotation();
		FVector Dir = Rotator.RotateVector(FVector::ForwardVector);

		FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();

		const TSharedPtr<FArmyWHCPoly> *PolyRoom = CabMgr->GetRooms().Find(ShapeFrame->ShapeInRoomRef->RoomAttachedIndex);
		check(PolyRoom != nullptr);
		TArray< TSharedPtr<FShapeInRoom> > &PlacedShapes = CabMgr->GetPlaceShapes();

		FRegularMoveCommand MoveCommand(this);
		MoveCommand.ProcessCommand((*PolyRoom).Get(),
			ShapeFrame->GetActorLocation() - Dir * Delta,
			ShapeFrame->ShapeInRoomRef,
			PlacedShapes);

		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		if (FShapeInRoom *AlignedPassiveShape = GetAlignedPassive(ShapeFrame->ShapeInRoomRef))
		{
			// 如果当前柜子和某个柜子对齐，移除其对齐关系，转为重叠关系
			RemoveAlignedPair(ShapeFrame->ShapeInRoomRef);
			AddOverlappedPair(ShapeFrame->ShapeInRoomRef, AlignedPassiveShape);
		}
		else
		{
			// 如果未和某个柜子对齐，重新检查对齐关系
			if (!CheckAligned(ShapeFrame->ShapeInRoomRef, PlacedShapes, KINDA_SMALL_NUMBER))
				CheckOverlapped(ShapeFrame->ShapeInRoomRef, PlacedShapes);
		}

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal((RulerVisibilityFlag & ERulerVF_Env) != 0);

		CabMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		if (FShapeInRoom *AlignedActiveShape = GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
			CabMgr->OnPalceShapeEnd(AlignedActiveShape);

		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);

		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::MoveShapeRight(float Delta)
{
	AXRShapeFrame *ShapeFrame = GetShapeFrameFromSelected();
	if (ShapeFrame != nullptr)
	{
		FRotator Rotator = ShapeFrame->GetActorRotation();
		FVector Dir = Rotator.RotateVector(FVector::ForwardVector);

		FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();

		const TSharedPtr<FArmyWHCPoly> *PolyRoom = CabMgr->GetRooms().Find(ShapeFrame->ShapeInRoomRef->RoomAttachedIndex);
		check(PolyRoom != nullptr);
		TArray< TSharedPtr<FShapeInRoom> > &PlacedShapes = CabMgr->GetPlaceShapes();

		FRegularMoveCommand MoveCommand(this);
		MoveCommand.ProcessCommand((*PolyRoom).Get(),
			ShapeFrame->GetActorLocation() + Dir * Delta,
			ShapeFrame->ShapeInRoomRef,
			PlacedShapes);

		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		if (FShapeInRoom *AlignedPassiveShape = GetAlignedPassive(ShapeFrame->ShapeInRoomRef))
		{
			// 如果当前柜子和某个柜子对齐，移除其对齐关系，转为重叠关系
			RemoveAlignedPair(ShapeFrame->ShapeInRoomRef);
			AddOverlappedPair(ShapeFrame->ShapeInRoomRef, AlignedPassiveShape);
		}
		else
		{
			// 如果未和某个柜子对齐，重新检查对齐关系
			if (!CheckAligned(ShapeFrame->ShapeInRoomRef, PlacedShapes, KINDA_SMALL_NUMBER))
				CheckOverlapped(ShapeFrame->ShapeInRoomRef, PlacedShapes);
		}

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal((RulerVisibilityFlag & ERulerVF_Env) != 0);

		CabMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		if (FShapeInRoom *AlignedActiveShape = GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
			CabMgr->OnPalceShapeEnd(AlignedActiveShape);

		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);

		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::MoveShapeBack(float Delta)
{
	AXRShapeFrame *ShapeFrame = GetShapeFrameFromSelected();
	if (ShapeFrame != nullptr)
	{
		FRotator Rotator = ShapeFrame->GetActorRotation();
		FVector Dir = Rotator.RotateVector(FVector::RightVector);

		FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();

		const TSharedPtr<FArmyWHCPoly> *PolyRoom = CabMgr->GetRooms().Find(ShapeFrame->ShapeInRoomRef->RoomAttachedIndex);
		check(PolyRoom != nullptr);
		TArray< TSharedPtr<FShapeInRoom> > &PlacedShapes = CabMgr->GetPlaceShapes();

		FRegularMoveCommand MoveCommand(this);
		MoveCommand.ProcessCommand((*PolyRoom).Get(),
			ShapeFrame->GetActorLocation() + Dir * Delta,
			ShapeFrame->ShapeInRoomRef,
			PlacedShapes);

		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		// 处理覆盖组和对齐组
		if (FShapeInRoom *PassiveShape = GetAlignedPassive(ShapeFrame->ShapeInRoomRef))
		{
			FArmyWHCRect ActiveRect = ShapeFrame->ShapeInRoomRef->GetRect();
			ActiveRect.mMinZ = ActiveRect.mMaxZ = 0.0f;
			FArmyWHCRect PassiveRect = PassiveShape->GetRect();
			PassiveRect.mMinZ = PassiveRect.mMaxZ = 0.0f;
			if (!ActiveRect.Collision(&PassiveRect))
			{
				RemoveOverlappedPair(ShapeFrame->ShapeInRoomRef);
				RemoveAlignedPair(ShapeFrame->ShapeInRoomRef);
			}
		}
		else
		{
			if (CheckOverlapped(ShapeFrame->ShapeInRoomRef, PlacedShapes))
				CheckAligned(ShapeFrame->ShapeInRoomRef, PlacedShapes, KINDA_SMALL_NUMBER);
		}

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal((RulerVisibilityFlag & ERulerVF_Env) != 0);

		CabMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		if (FShapeInRoom *AlignedActiveShape = GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
			CabMgr->OnPalceShapeEnd(AlignedActiveShape);

		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);

		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::ShowCabinetDoors(bool bShow)
{
	if (bShow)
		HideCommandFlag &= ~EHideCF_DoorGroup;
	else
		HideCommandFlag |= EHideCF_DoorGroup;

	for (auto &PlacedShape : FArmyWHCabinetMgr::Get()->GetPlaceShapes())
	{
		if (!PlacedShape->HasValidShape())
			continue;

		TSharedPtr<FCabinetShape> CabinetShape = StaticCastSharedPtr<FCabinetShape>(PlacedShape->Shape);
		int32 ShapeCategory = CabinetShape->GetShapeCategory();
		// 要显示门的时候，只有特定的柜子类型显示的时候，才会显示门
		if (ShapeCategory == (int32)ECabinetType::EType_OnGroundCab || ShapeCategory == (int32)ECabinetType::EType_TallCab)
		{
			CabinetShape->ShowDoorGroup(bShow && (HideCommandFlag & EHideCF_OnGroundCab) == 0);
			CabinetShape->ShowDrawer(bShow && (HideCommandFlag & EHideCF_OnGroundCab) == 0);
		}
		else if (ShapeCategory == (int32)ECabinetType::EType_HangCab || ShapeCategory == (int32)ECabinetType::EType_OnCabCab)
		{
			CabinetShape->ShowDoorGroup(bShow && (HideCommandFlag & EHideCF_HangCab) == 0);
			CabinetShape->ShowDrawer(bShow && (HideCommandFlag & EHideCF_HangCab) == 0);
		}
	}
}

void FArmyWHCModeCabinetOperation::ShowOnGroundCabinets(bool bShow, bool bChangeMode)
{
	if (bShow)
		HideCommandFlag &= ~EHideCF_OnGroundCab;
	else
		HideCommandFlag |= EHideCF_OnGroundCab;

	FArmyWHCabinetMgr * RESTRICT Mgr = FArmyWHCabinetMgr::Get();
	for (auto &PlacedShape : Mgr->GetPlaceShapes())
	{
		TSharedPtr<FCabinetShape> CabinetShape = StaticCastSharedPtr<FCabinetShape>(PlacedShape->Shape);
		int32 ShapeCategory = CabinetShape->GetShapeCategory();
		if (ShapeCategory == (int32)ECabinetType::EType_OnGroundCab)
		{
			if (PlacedShape->HasValidShape())
			{
				CabinetShape->HiddenCabinetShapeActors(!bShow);
				if (bShow && (HideCommandFlag & EHideCF_DoorGroup) != 0) // 要显示柜子，但门处于隐藏状态，再将门隐藏
					ShowCabinetDoors(false);
				PlacedShape->ShowShapeLegs(bShow);
			}
			if (PlacedShape->HasValidAccessory())
				PlacedShape->ShowAccessories(bShow);

		}
	}
	if (!bChangeMode && Mgr->IsGenTable())
	{
		Mgr->SetTableVisible(bShow);
		Mgr->SetTrimVisible(FArmyWHCabinetMgr::ETrim_Toe, bShow);
		Mgr->SetTrimVisible(FArmyWHCabinetMgr::ETrim_PlatformFront, bShow);
		Mgr->SetTrimVisible(FArmyWHCabinetMgr::ETrim_PlatformBack, bShow);
		Mgr->SetFloorGapVisible(bShow);
	}
}

void FArmyWHCModeCabinetOperation::ShowHangCabinets(bool bShow, bool bChangeMode)
{
	if (bShow)
		HideCommandFlag &= ~EHideCF_HangCab;
	else
		HideCommandFlag |= EHideCF_HangCab;

	FArmyWHCabinetMgr * RESTRICT Mgr = FArmyWHCabinetMgr::Get();
	for (auto &PlacedShape : Mgr->GetPlaceShapes())
	{
		TSharedPtr<FCabinetShape> CabinetShape = StaticCastSharedPtr<FCabinetShape>(PlacedShape->Shape);
		int32 ShapeCategory = CabinetShape->GetShapeCategory();
		if (ShapeCategory == (int32)ECabinetType::EType_HangCab || ShapeCategory == (int32)ECabinetType::EType_OnCabCab)
		{
			if (PlacedShape->HasValidShape())
			{
				CabinetShape->HiddenCabinetShapeActors(!bShow);
				if (bShow && (HideCommandFlag & EHideCF_DoorGroup) != 0) // 要显示柜子，但门处于隐藏状态，再将门隐藏
					ShowCabinetDoors(false);
				PlacedShape->ShowShapeLegs(bShow);
			}
			if (PlacedShape->HasValidAccessory())
				PlacedShape->ShowAccessories(bShow);

		}
	}
	if (!bChangeMode && Mgr->IsGenTop())
	{
		Mgr->SetTrimVisible(FArmyWHCabinetMgr::ETrim_Top, bShow);
		Mgr->SetHangGapVisible(bShow);
	}
}

void FArmyWHCModeCabinetOperation::ShowFurniture(bool bShow)
{
	if (bShow)
		HideCommandFlag &= ~EHideCF_Furniture;
	else
		HideCommandFlag |= EHideCF_Furniture;

	GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetSoftPath(), false, bShow, false);
}

void FArmyWHCModeCabinetOperation::ShowHydropower(bool bShow)
{
	if (bShow)
		HideCommandFlag &= ~EHideCF_Hydropower;
	else
		HideCommandFlag |= EHideCF_Hydropower;

	GGI->DesignEditor->WorldOutliner->CloseExpander(FArmyActorPath::GetHydropowerPath(), false, bShow, false);
	//@郭子阳 在立面模式和木作模式隐藏所有停用的原始点位，原始点位在E_LayoutModel
	if (bShow)
	{
		TArray<FObjectWeakPtr> ObjArray;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, ObjArray);
		for (auto ObjPtr : ObjArray)
		{
			FArmyFurniture* FurnitureObj = ObjPtr.Pin()->AsassignObj<FArmyFurniture>();
			if (FurnitureObj &&FurnitureObj->GetPropertyFlag(FArmyObject::PropertyType::FLAG_STOPPED))
			{
				if (FurnitureObj->GetRelevanceActor())
				{
					FurnitureObj->GetRelevanceActor()->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

bool FArmyWHCModeCabinetOperation::IsAnyWHCItemSelected() const
{
	return LastSelected != nullptr &&
		(LastSelected->IsA<AXRShapeFrame>() || LastSelected->IsA<AXRShapeTableActor>() || LastSelected->IsA<ASCTShapeActor>());
}

void FArmyWHCModeCabinetOperation::AddFrontBoardCabinet(FShapeInRoom *InCabinet)
{
	FrontBoardCabModifier.AddFrontBoardCab(InCabinet);
}

bool FArmyWHCModeCabinetOperation::IsFrontBoardCabinet(FShapeInRoom *InCabinet) const
{
	return FrontBoardCabModifier.IsFrontBoardCab(InCabinet);
}

void FArmyWHCModeCabinetOperation::TryModifySelectedCabinet()
{
	if (LastSelected != nullptr && LastSelected->IsA<AXRShapeFrame>())
	{
		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
	}
}

void FArmyWHCModeCabinetOperation::Callback_LMousePressed(const FVector2D &InMousePosition)
{

	switch (OpState)
	{
	case ECabinetOperationState::CabinetPlaced:
	{
		TArray<HActor*> HActorArr = GetHActorArray(InMousePosition.X, InMousePosition.Y);

		// 选择顺序优先级从低到高是柜体、台面/踢脚线/上线条/见光板、柜子中的某个组成部分
		AActor *TempFrameActor = ShapeFrameHitTest(HActorArr);
		AActor *TempAccessoryActor = ShapeTableHitTest(HActorArr);
		AActor *TempShapeActor = nullptr;
		AXRShapeFrame * ShapeFrame = nullptr;
		// zengy 2019-5-9 如果选中的柜子是橱柜系列，才会检查是否选中了附属件或门板
		if (TempFrameActor != nullptr)
		{
			ShapeFrame = Cast<AXRShapeFrame>(TempFrameActor);
			int32 ShapeCategory = ShapeFrame->ShapeInRoomRef->Shape->GetShapeCategory();
			if (ShapeCategory == (int32)ECabinetType::EType_HangCab ||
				ShapeCategory == (int32)ECabinetType::EType_OnGroundCab ||
				ShapeCategory == (int32)ECabinetType::EType_TallCab ||
				ShapeCategory == (int32)ECabinetType::EType_OnCabCab)
			{
				TempAccessoryActor = ShapeAccessoryHitTest(HActorArr);
				TempShapeActor = ShapeActorHitTest(HActorArr);
			}
		}

		if (LastSelected)
		{
			if (LastSelected->IsA<AXRShapeTableActor>())
				Cast<AXRShapeTableActor>(LastSelected)->SetIsSelected(false);
			else if (LastSelected->IsA<AXRWhcGapActor>())
				Cast<AXRWhcGapActor>(LastSelected)->SetIsSelected(false);
			else if (LastSelected->IsA<AXRElecDeviceActor>())
				Cast<AXRElecDeviceActor>(LastSelected)->SetIsSelected(false);
		}

		if (TempShapeActor != nullptr)
		{
			if (LastChildItemSelected == TempShapeActor) // 选择的是同一个子对象
			{
				if (LastSelected == TempShapeActor) // 上一次选中的也是这个子对象
					LastSelected = TempFrameActor; // 切换为选中柜子
				else if (LastSelected == TempFrameActor) // 上一次选中的是柜子
					LastSelected = TempShapeActor; // 切换为选中子对象
			}
			else // 如果不是同一扇门，直接切换门选择
			{
				LastSelected = TempShapeActor;
				LastChildItemSelected = TempShapeActor;
			}
		}
		else if (TempAccessoryActor != nullptr)
		{
			if (TempAccessoryActor->IsA<AXRShapeTableActor>())
				Cast<AXRShapeTableActor>(TempAccessoryActor)->SetIsSelected(true);
			else if (TempAccessoryActor->IsA<AXRWhcGapActor>())
				Cast<AXRWhcGapActor>(TempAccessoryActor)->SetIsSelected(true);
			else if (TempAccessoryActor->IsA<AXRElecDeviceActor>())
				Cast<AXRElecDeviceActor>(TempAccessoryActor)->SetIsSelected(true);

			LastSelected = TempAccessoryActor;
			LastChildItemSelected = nullptr;
		}
		else if (TempFrameActor != nullptr)
		{
			LastSelected = TempFrameActor;
			LastChildItemSelected = nullptr;
		}
		else
		{
			LastSelected = nullptr;
			LastChildItemSelected = nullptr;
		}

		HighlightSelected();

		ShapeFrame = GetShapeFrameFromSelected();
		if (ShapeFrame != nullptr)
		{
			CachedShapePosition = ShapeFrame->GetActorLocation();
			CachedShapeRotation = ShapeFrame->GetActorRotation();
			if ((RulerVisibilityFlag & ERulerVF_Env) != 0)
			{
				// 如果需要显示距离标尺，首先要绑定到正确的ShapeFrame上
				SwitchDistanceRulerBinding(ShapeFrame->ShapeInRoomRef);
				CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
				ShowShapeDistanceRulerInternal(true);
			}

			mbFirstMoveShape = true;
		}
		else
			ShowShapeDistanceRulerInternal(false);

		break;
	}
	}

	MouseButtonFlag |= 0x01;
}

void FArmyWHCModeCabinetOperation::Callback_LMouseReleased(const FVector2D &InMousePosition)
{
	switch (OpState)
	{
	case ECabinetOperationState::PlacingCabinet:
	{
		if (PlaceShapeRect(PlacingShape, InMousePosition))
		{
			FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();
			// 取入柜子时如果要放置柜子的时刻和其它柜子有冲突，则不能放置
			if (!CabMgr->Collision(PlacingShape, nullptr, -0.001f))
			{
				// 显示宽度标尺
				XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = PlacingShape->CabinetActor;
				TSharedPtr<class FShapeRuler> ShapeWidthRuler = RulerComponent->Get();
				if (ShapeWidthRuler.IsValid())
					ShapeWidthRuler->ShowRuler((RulerVisibilityFlag & ERulerVF_Self) != 0);

				// 如果显示距离标尺，则计算标尺并显示
				if ((RulerVisibilityFlag & ERulerVF_Env) != 0)
				{
					// 切换柜子距离标尺的绑定关系
					SwitchDistanceRulerBinding(PlacingShape);
					CalculateShapeDistanceFromWall(PlacingShape);
					ShowShapeDistanceRulerInternal(true);
				}

				PlacingShape->SpawnPlatform();
				// 准备动画
				PlacingShape->PrepareShapeAnimations();
				// 构件更新
				PlacingShape->UpdateComponents();

				FArmyWHPlaceShapes.Emplace(MakeShareable(PlacingShape));
				LastSelected = PlacingShape->ShapeFrame;
				CabMgr->OnPalceShapeEnd(PlacingShape);
				TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.AddCabinet(PlacingShape);
				for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
					MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
				PlacingShape = nullptr;

				HighlightSelected();

				OpState = ECabinetOperationState::CabinetPlaced;

				OnWHCItemSelected.ExecuteIfBound(LastSelected);
			}
			else
				GGI->Window->ShowMessage(MT_Warning, TEXT("发生重叠，请重新放置"));
		}
		break;
	}
	case ECabinetOperationState::CabinetPlaced:
	{
		// 要放置的位置和其它柜子重叠，回退到移动开始的位置
		if (LastSelected != nullptr)
		{
			AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected();
			if (ShapeFrame != nullptr)
			{
				FArmyWHCabinetMgr *CabMgr = FArmyWHCabinetMgr::Get();
				if (CabMgr->Collision(ShapeFrame->ShapeInRoomRef, nullptr, -0.001f))
				{
					FVector2D Delta(EForceInit::ForceInitToZero);
					if (FShapeInRoom *AlignedActiveShape = GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
					{
						FVector ActiveLocation = AlignedActiveShape->ShapeFrame->GetActorLocation();
						FVector PassiveLocation = ShapeFrame->GetActorLocation();
						FRotator Rotation = ShapeFrame->GetActorRotation();
						FVector Dir = Rotation.RotateVector(FVector::ForwardVector);
						FVector Nor = Rotation.RotateVector(FVector::RightVector);
						FVector Delta = ActiveLocation - PassiveLocation;
						FVector2D Delta2D(FVector::DotProduct(Delta, Dir), FVector::DotProduct(Delta, Nor));

						float Z = ActiveLocation.Z;
						Dir = CachedShapeRotation.RotateVector(FVector::ForwardVector);
						Nor = CachedShapeRotation.RotateVector(FVector::RightVector);
						ActiveLocation = CachedShapePosition + Dir * Delta2D.X + Nor * Delta2D.Y;
						ActiveLocation.Z = Z;
						AlignedActiveShape->ShapeFrame->SetActorLocation(ActiveLocation);
						AlignedActiveShape->ShapeFrame->SetActorRotation(CachedShapeRotation);
					}
					ShapeFrame->SetActorLocation(CachedShapePosition);
					ShapeFrame->SetActorRotation(CachedShapeRotation);

					GGI->Window->ShowMessage(MT_Warning, TEXT("发生重叠，请重新放置"));
				}

				ShapeFrame->ShapeInRoomRef->UpdateComponents();

				// 避免单纯的点选对象时也重新生成各种造型，造成台面、上线条和踢脚线等闪烁
				if (!mbFirstMoveShape)
				{
					ShapeFrame->ShapeInRoomRef->SpawnPlatform();
					CabMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
					// 如果移动的柜子是某个对齐组的PassiveShape，则需要更新其ActiveShape，以便同时更新放样类线条
					if (FShapeInRoom *AlignedActiveShape = GetAlignedActiveShapeByPassive(ShapeFrame->ShapeInRoomRef))
						CabMgr->OnPalceShapeEnd(AlignedActiveShape);
					TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
					for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
						MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
					UpdateHighlight();
				}
			}

			OnWHCItemSelected.ExecuteIfBound(LastSelected);
		}
		else
			OnWHCItemSelected.ExecuteIfBound(nullptr);


		break;
	}
	}

	MouseButtonFlag &= ~0x01;


	mbFirstMoveShape = false;
}

void FArmyWHCModeCabinetOperation::Callback_RMousePressed(const FVector2D &InMousePosition)
{

}

void FArmyWHCModeCabinetOperation::Callback_RMouseReleased(const FVector2D &InMousePosition)
{
	switch (OpState)
	{
	case ECabinetOperationState::AboutToPlaceCabinet:
	case ECabinetOperationState::PlacingCabinet:
	{
		ResetOperation();
		if (FArmyWHCabinetMgr::Get()->GetPlaceShapes().Num() > 0)
			OpState = ECabinetOperationState::CabinetPlaced;
		else
			OpState = ECabinetOperationState::Idle;
		GGI->Window->HideMessage();
		break;
	}
	}
}

void FArmyWHCModeCabinetOperation::Callback_KeyPressed(const FString &InKeyStr)
{
	if (InKeyStr == "F")
	{

	}
}

void FArmyWHCModeCabinetOperation::Callback_KeyReleased(const FString &InKeyStr)
{
	if (InKeyStr == "F")
	{
		AXRShapeFrame *ShapeFrame = GetShapeFrameFromSelected();
		if (ShapeFrame == nullptr)
			return;

		bool bPlaySingleAnimation = false;

		if (LastSelected != nullptr && LastSelected->IsA<ASCTShapeActor>())
		{
			ASCTShapeActor *SelectedShapeActor = Cast<ASCTShapeActor>(LastSelected);
			// 控制指定的某扇门
			FSCTShape *Shape = SelectedShapeActor->GetShape();
			EShapeType ShapeType = Shape->GetShapeType();
			if (ShapeType == ST_VariableAreaDoorSheet_ForSideHungDoor ||
				ShapeType == ST_SlidingDrawer)
			{
				bPlaySingleAnimation = true;
				TSharedPtr<FAnimation> Animation = ShapeFrame->ShapeInRoomRef->FindShapeAnimation(Shape);
				if (Animation.IsValid())
					Animation->Start();
			}
		}

		if (ShapeFrame && !bPlaySingleAnimation)
		{
			int32 StateCount = 0;
			for (const auto &Animation : ShapeFrame->ShapeInRoomRef->Animations)
			{
				if (!Animation->IsInitState())
					++StateCount;
			}
			if (StateCount > 0 && StateCount < ShapeFrame->ShapeInRoomRef->Animations.Num())
			{
				for (const auto &Animation : ShapeFrame->ShapeInRoomRef->Animations)
					Animation->Stop();
			}
			else
			{
				for (const auto &Animation : ShapeFrame->ShapeInRoomRef->Animations)
					Animation->Start();
			}
		}
	}
}

void FArmyWHCModeCabinetOperation::Callback_MouseMove(const FVector2D &InMousePosition)
{
	switch (OpState)
	{
	case ECabinetOperationState::AboutToPlaceCabinet:
	{
		if (PlaceShapeRect(PlacingShape, InMousePosition))
			OpState = ECabinetOperationState::PlacingCabinet;
		break;
	}
	case ECabinetOperationState::PlacingCabinet:
	{
		PlaceShapeRect(PlacingShape, InMousePosition);
		break;
	}
	}
}

bool FArmyWHCModeCabinetOperation::Callback_MouseCapturedMove(const FVector2D &InMousePosition)
{
	switch (OpState)
	{
	case ECabinetOperationState::CabinetPlaced:
	{
		AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected();
		if ((MouseButtonFlag & 0x01) != 0 && ShapeFrame != nullptr)
		{
			FVector2D ScreenPos = InMousePosition;
			if (mbFirstMoveShape)
			{
				gFArmyWHCabinMgr->OnPalceShapeBegin(ShapeFrame->ShapeInRoomRef);
				FVector Pos(ShapeFrame->GetActorLocation().X, ShapeFrame->GetActorLocation().Y, ShapeFrame->ShapeInRoomRef->AboveGround);
				GVC->WorldToPixel(Pos, ScreenPos);
				GVC->Viewport->SetMouse(ScreenPos.X, ScreenPos.Y);
				mbFirstMoveShape = false;
			}
			PlaceShapeRect(ShapeFrame->ShapeInRoomRef, ScreenPos);
			if ((RulerVisibilityFlag & ERulerVF_Env) != 0) // 在移动过程中实时更新距离标尺的值
			{
				CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
				ShowShapeDistanceRulerInternal(true);
			}
			// UpdateHighlight();
			return true;
		}
		break;
	}
	}
	mbFirstMoveShape = false;
	return false;
}

void FArmyWHCModeCabinetOperation::Callback_DeleteSelected()
{
	if (LastSelected != nullptr)
	{
		FArmyWHCabinetMgr * RESTRICT Mgr = gFArmyWHCabinMgr;
		if (AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected())
		{
			// 隐藏距离标尺
			ClearDistanceRulerBinding();
			ShowShapeDistanceRulerInternal(false);
			// 从已放置的柜子列表中删除当前选中的柜子
			TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = Mgr->GetPlaceShapes();
			ECabinetType ShapeType = ECabinetType::EType_Unknown;
			PlacedShapes.RemoveAll([this, ShapeFrame, &ShapeType](TSharedPtr<FShapeInRoom> &InItem) -> bool {
				bool bSameOne = ShapeFrame->ShapeInRoomRef == InItem.Get();
				if (bSameOne)
				{
					ShapeType = (ECabinetType)InItem->Shape->GetShapeCategory();
					LastSelected = nullptr;
					LastChildItemSelected = nullptr;

					// 如果选中的是门板导致的柜子删除，需要隐藏交互信息
					GGI->Window->ShowInteractiveMessage(false);
				}
				return bSameOne;
			});
			// 根据柜子类型决定是否需要更新台面、踢脚线或调整板
			Mgr->OnRefreshShape((int32)ShapeType);
			FrontBoardCabModifier.DeleteCabinet(ShapeFrame->ShapeInRoomRef);
			if (IsBelongedToAnyOverlappedPair(ShapeFrame->ShapeInRoomRef))
				RemoveOverlappedPair(ShapeFrame->ShapeInRoomRef);
			else if (IsBelongedToAnyAlignedPair(ShapeFrame->ShapeInRoomRef))
				RemoveAlignedPair(ShapeFrame->ShapeInRoomRef);
		}
		else if (LastSelected->IsA<AXRShapeTableActor>())
		{
			if (LastSelected->ActorHasTag(XRActorTag::WHCTable)) // 台面
			{
				Mgr->GenTable(false);
				LastSelected = nullptr;
			}
			else if (LastSelected->ActorHasTag(XRActorTag::WHCFloor)) // 踢脚板
			{
				Mgr->GenFloorTrim(false);
				LastSelected = nullptr;
			}
			else if (LastSelected->ActorHasTag(XRActorTag::WHCHang)) // 上线条
			{
				Mgr->GenHangTrim(false);
				LastSelected = nullptr;
			}
		}
		//      else if (LastSelected->IsA<AXRWhcGapActor>())
		//      {
		//          AXRWhcGapActor *GapActor = Cast<AXRWhcGapActor>(LastSelected);
		// if (GapActor->mCabinet)
		// {
		// 	// GapActor->mCabinet->SetSunBoard(GapActor->mGapIdx,"");
		// 	Mgr->OnRefreshShape(GapActor->mCabinet->Shape->GetShapeCategory());
		// }
		//          LastSelected = nullptr;
		//      }

		SelectedWireframe->SetActorHiddenInGame(true);
	}
}

AXRShapeFrame* FArmyWHCModeCabinetOperation::GetShapeFrameFromSelected() const
{
	if (LastSelected != nullptr)
	{
		if (LastSelected->IsA<AXRShapeFrame>())
			return Cast<AXRShapeFrame>(LastSelected);
		else if (LastSelected->IsA<ASCTShapeActor>() || LastSelected->IsA<AXRElecDeviceActor>())
		{
			AActor * AttachParent = LastSelected->GetAttachParentActor();
			while (AttachParent != nullptr && !AttachParent->IsA<AXRShapeFrame>())
				AttachParent = AttachParent->GetAttachParentActor();
			if (AttachParent != nullptr)
				return Cast<AXRShapeFrame>(AttachParent);
		}
	}
	return nullptr;
}

TArray<FArmyRoom*> FArmyWHCModeCabinetOperation::GetRooms(const FString &InRoomLabel) const
{
	TArray<FObjectWeakPtr> Objects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, Objects);
	TArray<FArmyRoom*> Rooms;
	for (auto &Object : Objects)
	{
		FArmyRoom *RoomObject = static_cast<FArmyRoom*>(Object.Pin().Get());
		if (RoomObject->GetSpaceName() == InRoomLabel)
			Rooms.Emplace(RoomObject);
	}
	return MoveTemp(Rooms);
}

bool FArmyWHCModeCabinetOperation::PlaceShape(FShapeInRoom *InShape, const FVector2D &InMousePosition)
{
	constexpr float AdhereThreshold = 55.0f; // 柜子和墙的吸附阈值
	constexpr float AdhereCabThreshold = 55.0f; // 柜子和柜子的吸附阈值

	if (Kitchens.Num() == 0)
		return false;

	int32 RoomIndex = -1;
	FVector ShapePosition;
	if (!To3DPosition(InMousePosition, Kitchens, InShape->AboveGround, RoomIndex, ShapePosition))
		return false;


	FTransform ShapeTransform = InShape->ShapeFrame == nullptr ? FTransform::Identity : InShape->ShapeFrame->GetActorTransform();

	float ShapeWidth = InShape->Shape->GetShapeWidth() * 0.1f; // To centimeter
	float ShapeDepth = InShape->Shape->GetShapeDepth() * 0.1f; // To centimeter
	float ShapeHeight = InShape->Shape->GetShapeHeight() * 0.1f; // To centimeter

																 // 柜子已经和某面墙吸附
	if (InShape->WallAttachedIndex != -1)
	{
		check(InShape->ShapeFrame != nullptr);

		FArmyRoom **Kitchen = Kitchens.FindByPredicate([InShape](FArmyRoom *InRoom) -> bool {
			return InRoom->GetUniqueID() == InShape->RoomAttachedIndex;
		});
		if (Kitchen == nullptr)
			return false;
		TArray<FVector> KitchenPoints = (*Kitchen)->GetWorldPoints(true);
		int32 NumWall = KitchenPoints.Num();

		const FVector &WallStartPoint = KitchenPoints[InShape->WallAttachedIndex];
		const FVector &WallEndPoint = KitchenPoints[XRWHCMode::RoundIndex(InShape->WallAttachedIndex, 1, KitchenPoints.Num())];
		FPlane KitchenWallPlane(WallEndPoint,
			WallStartPoint,
			WallStartPoint + FVector::UpVector * FArmySceneData::WallHeight);
		float Distance = KitchenWallPlane.PlaneDot(ShapePosition);
		bool bFarEnoughFromWall = XRWHCMode::FloatGreaterEqual(Distance, AdhereThreshold);
		if (!bFarEnoughFromWall)
		{
			// 如果柜子仍旧和墙吸附，计算当前位置点投影到墙上的位置，做为柜子新的位置
			FVector WallDir = WallEndPoint - WallStartPoint;
			WallDir.Normalize();
			FVector WallRight = FVector::CrossProduct(FVector::UpVector, WallDir);
			ShapePosition -= WallRight * Distance;
			//ShapeRotator = InShape->ShapeFrame->GetActorRotation();

			// 在吸附时考虑和两侧柜子的碰撞
			PlaceShapeAgainstOtherShapes(InShape, ShapePosition);

			// 在吸附时考虑和两侧墙和柱子的碰撞
			PlaceShapeAgainstWall(InShape, ShapePosition);

			ShapeTransform.SetTranslation(ShapePosition);
		}
		else
			InShape->WallAttachedIndex = -1;
	}

	// 当帧处理未吸附情况
	if (InShape->WallAttachedIndex == -1)
	{
		ShapeTransform.SetTranslation(ShapePosition);

		InShape->RoomAttachedIndex = Kitchens[RoomIndex]->GetUniqueID();

		TArray<FVector> KitchenPoints = Kitchens[RoomIndex]->GetWorldPoints(true);
		int32 NumWall = KitchenPoints.Num();

		// 获取Shape关联的Actor
		if (InShape->ShapeFrame == nullptr)
		{
			InShape->Spawn();
			gFArmyWHCabinMgr->OnShapeInited(InShape);

			GGI->Window->HideMessage();
		}

		// 检测柜子和墙的吸附
		for (int32 i = 0; i < NumWall; ++i)
		{
			const FVector &WallStartPoint = KitchenPoints[i];
			const FVector &WallEndPoint = KitchenPoints[XRWHCMode::RoundIndex(i, 1, NumWall)];
			FPlane KitchenWallPlane(WallEndPoint,
				WallStartPoint,
				WallStartPoint + FVector::UpVector * FArmySceneData::WallHeight);

			float Distance = KitchenWallPlane.PlaneDot(ShapePosition);
			if (XRWHCMode::FloatLess(Distance, AdhereThreshold)) // 和墙能够吸附
			{
				FVector WallDir = WallEndPoint - WallStartPoint;
				WallDir.Normalize();
				FVector WallRight = FVector::CrossProduct(FVector::UpVector, WallDir);
				ShapePosition -= WallRight * Distance;

				ShapeTransform = FTransform(WallDir, WallRight, FVector::UpVector, ShapePosition);

				InShape->WallAttachedIndex = i;

				break;
			}
		}

		if (InShape->WallAttachedIndex == -1) // 如果仍旧未和墙吸附，检测是否和场景中的某个柜子可以产生吸附
		{
			for (auto &PlacedShape : FArmyWHPlaceShapes)
			{
				if (InShape != PlacedShape.Get())
				{
					float OtherShapeHalfWidth = PlacedShape->Shape->GetShapeWidth() * 0.05f;
					float OtherShapeDepth = PlacedShape->Shape->GetShapeDepth() * 0.1f;
					float OtherShapeHeight = PlacedShape->Shape->GetShapeHeight() * 0.1f;

					FVector OtherShapeLocation = PlacedShape->ShapeFrame->GetActorLocation();
					FRotator OtherShapeRotation = PlacedShape->ShapeFrame->GetActorRotation();

					FTransform Transform(OtherShapeRotation.Quaternion(), OtherShapeLocation);
					FVector OtherShapeLeftLocation = Transform.TransformPositionNoScale(FVector(-OtherShapeHalfWidth, 0.0f, 0.0f));
					FVector OtherShapeRightLocation = Transform.TransformPositionNoScale(FVector(OtherShapeHalfWidth, 0.0f, 0.0f));
					FVector AlignedDir = OtherShapeRightLocation - OtherShapeLeftLocation;
					AlignedDir.Normalize();

					// 先检测左侧
					TArray<FVector> SidePoints{
						OtherShapeLeftLocation,
						Transform.TransformPositionNoScale(FVector(-OtherShapeHalfWidth, OtherShapeDepth, 0.0f)),
						Transform.TransformPositionNoScale(FVector(-OtherShapeHalfWidth, 0.0f, OtherShapeHeight)),
						Transform.TransformPositionNoScale(FVector(-OtherShapeHalfWidth, OtherShapeDepth, OtherShapeHeight))
					};

					bool bAttachedToOtherShape = false;

					for (const auto &SidePoint : SidePoints)
					{
						// 任意一个点和鼠标点的距离小于阈值
						if (XRWHCMode::FloatLess((SidePoint - ShapePosition).Size(), AdhereCabThreshold))
						{
							ShapePosition = OtherShapeLeftLocation - AlignedDir * ShapeWidth * 0.5f;
							//ShapeRotator = OtherShapeRotation;
							ShapeTransform.SetTranslation(ShapePosition);
							ShapeTransform.SetRotation(OtherShapeRotation.Quaternion());

							bAttachedToOtherShape = true;

							break;
						}
					}

					if (bAttachedToOtherShape)
					{
						InShape->WallAttachedIndex = PlacedShape->WallAttachedIndex;
						break;
					}

					// 再检查右侧
					SidePoints[0] = OtherShapeRightLocation;
					SidePoints[1] = Transform.TransformPositionNoScale(FVector(OtherShapeHalfWidth, OtherShapeDepth, 0.0f));
					SidePoints[2] = Transform.TransformPositionNoScale(FVector(OtherShapeHalfWidth, 0.0f, OtherShapeHeight));
					SidePoints[3] = Transform.TransformPositionNoScale(FVector(OtherShapeHalfWidth, OtherShapeDepth, OtherShapeHeight));

					for (const auto &SidePoint : SidePoints)
					{
						// 任意一个点和鼠标点的距离小于阈值
						if (XRWHCMode::FloatLess((SidePoint - ShapePosition).Size(), AdhereCabThreshold))
						{
							ShapePosition = OtherShapeRightLocation + AlignedDir * ShapeWidth * 0.5f;
							//ShapeRotator = OtherShapeRotation;
							ShapeTransform.SetTranslation(ShapePosition);
							ShapeTransform.SetRotation(OtherShapeRotation.Quaternion());

							bAttachedToOtherShape = true;

							break;
						}
					}

					if (bAttachedToOtherShape)
					{
						InShape->WallAttachedIndex = PlacedShape->WallAttachedIndex;
						break;
					}
				}
			}
		}
	}

	// 设置柜子最终的位置和旋转
	InShape->ShapeFrame->SetActorTransform(ShapeTransform);

	return true;
}

void FArmyWHCModeCabinetOperation::PlaceShapeAgainstOtherShapes(FShapeInRoom *InTestShape, FVector &OutPosition)
{
	FArmyWHCRect rect = InTestShape->GetRect();

	float TestShapeWidth = rect.mSizeHalf.X;
	float TestShapeDepth = rect.mSizeHalf.Y;
	float TestShapeHeight = rect.SizezHalf();

	FVector TestShapeLocation = InTestShape->ShapeFrame->GetActorLocation();
	FRotator TestShapeRotation = InTestShape->ShapeFrame->GetActorRotation();

	FTransform TestShapeTransform(TestShapeRotation.Quaternion(), TestShapeLocation);
	FVector TestShapeOutline[3] = {
		TestShapeTransform.TransformPositionNoScale(FVector(-TestShapeWidth, 0.0f, 0.0f)),
		TestShapeTransform.TransformPositionNoScale(FVector(TestShapeWidth, 0.0f, 0.0f)),
		TestShapeTransform.TransformPositionNoScale(FVector(-TestShapeWidth, TestShapeDepth, 0.0f))
	};

	FVector TestShapeForwardDir = TestShapeOutline[1] - TestShapeOutline[0];
	TestShapeForwardDir.Normalize();
	FVector TestShapeRightDir = TestShapeOutline[2] - TestShapeOutline[0];
	TestShapeRightDir.Normalize();

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(InTestShape->ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	FHitResult HitResultRight;
	FVector SweepRightStart = TestShapeLocation + TestShapeForwardDir * TestShapeWidth + TestShapeRightDir * TestShapeDepth + FVector::UpVector * TestShapeHeight * 0.5f;
	if (World->SweepSingleByChannel(HitResultRight,
		SweepRightStart,
		SweepRightStart + TestShapeForwardDir * 10000.0f,
		TestShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(0.1f, TestShapeDepth, TestShapeHeight)),
		QueryParams))
	{
		AActor *HitActor = HitResultRight.Actor.Get();
		if (HitActor && HitActor->IsA<AXRShapeFrame>())
		{
			AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(HitActor);
			FShapeInRoom *HitShape = ShapeFrame->ShapeInRoomRef;
			if (CheckTwoShapeOverlapped(InTestShape, OutPosition, HitShape, 0.0f))
			{
				if (HitResultRight.bStartPenetrating == 0)
					OutPosition = HitResultRight.Location - TestShapeForwardDir * TestShapeWidth * 0.5f - TestShapeRightDir * TestShapeDepth * 0.5f - FVector::UpVector * TestShapeHeight * 0.5f;
				else
					OutPosition = TestShapeLocation;
				return;
			}
		}
	}

	FHitResult HitResultLeft;
	FVector SweepLeftStart = TestShapeLocation - TestShapeForwardDir * TestShapeWidth * 0.5f + TestShapeRightDir * TestShapeDepth * 0.5f + FVector::UpVector * TestShapeHeight * 0.5f;
	if (World->SweepSingleByChannel(HitResultLeft,
		SweepLeftStart,
		SweepLeftStart - TestShapeForwardDir * 10000.0f,
		TestShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(0.1f, TestShapeDepth * 0.5f, TestShapeHeight * 0.5f)),
		QueryParams))
	{
		AActor *HitActor = HitResultLeft.Actor.Get();
		if (HitActor && HitActor->IsA<AXRShapeFrame>())
		{
			AXRShapeFrame *ShapeFrame = Cast<AXRShapeFrame>(HitActor);
			FShapeInRoom *HitShape = ShapeFrame->ShapeInRoomRef;
			if (CheckTwoShapeOverlapped(InTestShape, OutPosition, HitShape, 0.0f))
			{
				if (HitResultLeft.bStartPenetrating == 0)
					OutPosition = HitResultLeft.Location + TestShapeForwardDir * TestShapeWidth * 0.5f - TestShapeRightDir * TestShapeDepth * 0.5f - FVector::UpVector * TestShapeHeight * 0.5f;
				else
					OutPosition = TestShapeLocation;
				return;
			}
		}
	}

	for (const auto &PlacedShape : FArmyWHPlaceShapes)
	{
		if (InTestShape != PlacedShape.Get() &&
			CheckTwoShapeOverlapped(InTestShape, OutPosition, PlacedShape.Get(), 0.0f))
		{
			OutPosition = TestShapeLocation;
			break;
		}
	}
}

void FArmyWHCModeCabinetOperation::PlaceShapeAgainstWall(FShapeInRoom *InTestShape, FVector &OutPosition)
{
	FArmyWHCRect rect = InTestShape->GetRect();

	float TestShapeWidth = rect.mSizeHalf.X;// InTestShape->Shape->GetShapeWidth() * 0.1f;
	float TestShapeDepth = rect.mSizeHalf.Y;// InTestShape->Shape->GetShapeDepth() * 0.1f;
	float TestShapeHeight = rect.SizezHalf();// ->Shape->GetShapeHeight() * 0.1f;

	FVector TestShapeLocation = rect.GetPos(FVector(0, -1, -1));// (InTestShape->ShapeFrame->GetActorLocation();
	FRotator TestShapeRotation = rect.GetRotation();// InTestShape->ShapeFrame->GetActorRotation();

	FTransform TestShapeTransform(TestShapeRotation.Quaternion(), TestShapeLocation);
	FVector TestShapeOutline[3] = {
		TestShapeTransform.TransformPositionNoScale(FVector(-TestShapeWidth * 0.5f, 0.0f, 0.0f)),
		TestShapeTransform.TransformPositionNoScale(FVector(TestShapeWidth * 0.5f, 0.0f, 0.0f)),
		TestShapeTransform.TransformPositionNoScale(FVector(-TestShapeWidth * 0.5f, TestShapeDepth, 0.0f))
	};

	FVector TestShapeForwardDir = TestShapeOutline[1] - TestShapeOutline[0];
	TestShapeForwardDir.Normalize();
	FVector TestShapeRightDir = TestShapeOutline[2] - TestShapeOutline[0];
	TestShapeRightDir.Normalize();

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(InTestShape->ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	// FHitResult HitResultRight;
	TArray<FHitResult> HitResultsRight;
	FVector SweepRightStart = TestShapeLocation + TestShapeForwardDir * TestShapeWidth * 0.5f + TestShapeRightDir * TestShapeDepth * 0.5f + FVector::UpVector * TestShapeHeight * 0.5f;
	if (World->SweepMultiByChannel(HitResultsRight,
		SweepRightStart,
		SweepRightStart + TestShapeForwardDir * 10000.0f,
		TestShapeRotation.Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(FVector(0.01f, (TestShapeDepth - 0.2f) * 0.5f, (TestShapeHeight - 0.2f) * 0.5f)),
		QueryParams))
	{
		for (const auto &HitResult : HitResultsRight)
		{
			AActor *HitActor = HitResult.GetActor();
			if (HitActor && HitActor->IsA<AXRWallActor>())
			{
				AXRWallActor *WallActor = Cast<AXRWallActor>(HitActor);
				if (WallActor->AttachSurface.IsValid() &&
					WallActor->AttachSurface->SurfaceType == 1 &&
					!XRWHCMode::CheckDoublication(TestShapeRightDir, WallActor->AttachSurface->GetPlaneNormal())) // 碰到了墙
				{
					// FPlane WallPlane(WallActor->AttachSurface->GetPlaneCenter(), WallActor->AttachSurface->GetPlaneNormal());
					if (CheckShapePenetrateWall(InTestShape, OutPosition, WallActor))
					{
						OutPosition = HitResult.Location - TestShapeForwardDir * TestShapeWidth * 0.5f - TestShapeRightDir * TestShapeDepth * 0.5f - FVector::UpVector * TestShapeHeight * 0.5f;
						return;
					}
				}
			}
		}
	}

	// FHitResult HitResultLeft; 
	TArray<FHitResult> HitResultsLeft;
	FVector SweepLeftStart = TestShapeLocation - TestShapeForwardDir * TestShapeWidth * 0.5f + TestShapeRightDir * TestShapeDepth * 0.5f + FVector::UpVector * TestShapeHeight * 0.5f;
	if (World->SweepMultiByChannel(HitResultsLeft,
		SweepLeftStart,
		SweepLeftStart - TestShapeForwardDir * 10000.0f,
		TestShapeRotation.Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(FVector(0.01f, TestShapeDepth * 0.49f, TestShapeHeight * 0.49f)),
		QueryParams))
	{
		for (const auto &HitResult : HitResultsLeft)
		{
			AActor *HitActor = HitResult.GetActor();
			if (HitActor && HitActor->IsA<AXRWallActor>())
			{
				AXRWallActor *WallActor = Cast<AXRWallActor>(HitActor);
				if (WallActor->AttachSurface.IsValid() &&
					WallActor->AttachSurface->SurfaceType == 1 &&
					!XRWHCMode::CheckDoublication(TestShapeRightDir, WallActor->AttachSurface->GetPlaneNormal())) // 碰到了墙
				{
					// FPlane WallPlane(WallActor->AttachSurface->GetPlaneCenter(), WallActor->AttachSurface->GetPlaneNormal());
					if (CheckShapePenetrateWall(InTestShape, OutPosition, WallActor))
					{
						OutPosition = HitResult.Location + TestShapeForwardDir * TestShapeWidth * 0.5f - TestShapeRightDir * TestShapeDepth * 0.5f - FVector::UpVector * TestShapeHeight * 0.5f;
						return;
					}
				}
			}
		}
	}
}

bool FArmyWHCModeCabinetOperation::To3DPosition(const FVector2D &InMousePosition, const TArray<class FArmyRoom*> &InRooms, float InAboveGround, int32 &OutRoomIndex, FVector &OutPositionInRoom) const
{
	bool bHitGroundOrWall = false;
	TArray<FVector> RoomPoints;

	FVector WorldPoint, WorldDir;
	GVC->DeprojectFVector2D(InMousePosition, WorldPoint, WorldDir);

	// 查找鼠标点和地面的相交点
	for (int32 i = 0; i < InRooms.Num(); ++i)
	{
		RoomPoints = InRooms[i]->GetWorldPoints(true);
		if (RoomPoints.Num() < 2)
			continue;

		for (auto &RoomPoint : RoomPoints)
			RoomPoint.Z = InAboveGround;
		FPlane RoomGroundPlane(RoomPoints[0], RoomPoints[1], RoomPoints[2]);
		FVector Intersection = FMath::LinePlaneIntersection(WorldPoint, WorldPoint + WorldDir * 10000.0f, RoomGroundPlane);
		// 计算相交点在WorldDir上的投影距离，只有投影距离为正值(即地面在视线里，而不是背面)时才认为相交成功
		float ProjectDis = FVector::DotProduct(Intersection - WorldPoint, WorldDir);
		if (XRWHCMode::FloatGreater(ProjectDis, 0.0f) && InRooms[i]->IsPointInRoom(Intersection))
		{
			OutPositionInRoom = Intersection;
			OutRoomIndex = i;
			return true;
		}
		else // 如果未和地面相交再检查是否和这个房间的墙相交
		{
			for (int32 j = 0; j < RoomPoints.Num(); ++j)
			{
				const FVector &WallStartPoint = RoomPoints[j];
				const FVector &WallEndPoint = RoomPoints[XRWHCMode::RoundIndex(j, 1, RoomPoints.Num())];
				FPlane RoomWallPlane(WallEndPoint,
					WallStartPoint,
					WallStartPoint + FVector::UpVector * FArmySceneData::WallHeight);
				FVector Intersection = FMath::LinePlaneIntersection(WorldPoint, WorldPoint + WorldDir * 10000.0f, RoomWallPlane);
				float ProjectDis = FVector::DotProduct(Intersection - WorldPoint, WorldDir);
				if (XRWHCMode::FloatLess(ProjectDis, 0.0f)) // 相交的墙面在视线的背后，跳过这种情况
					continue;

				FVector WallDir = WallEndPoint - WallStartPoint;
				float WallLen = WallDir.Size();
				WallDir.Normalize();
				ProjectDis = FVector::DotProduct(Intersection - WallStartPoint, WallDir);
				// 检测相交点是否在墙面内
				if (XRWHCMode::FloatGreater(ProjectDis, 0.0f) &&
					XRWHCMode::FloatLess(ProjectDis, WallLen) &&
					XRWHCMode::FloatGreater(Intersection.Z, 0.0f) &&
					XRWHCMode::FloatLess(Intersection.Z, FArmySceneData::WallHeight))
				{
					OutPositionInRoom = Intersection;
					OutPositionInRoom.Z = InAboveGround;
					OutRoomIndex = i;
					return true;
				}
			}
		}
	}

	return false;
}

bool FArmyWHCModeCabinetOperation::CheckShapeOverlapped(FShapeInRoom *InTestShape) const
{
	float OffsetLength = -0.1f;
	for (const auto &PlacedShape : FArmyWHPlaceShapes)
	{
		FVector Location = InTestShape->ShapeFrame->GetActorLocation();
		if (InTestShape != PlacedShape.Get() &&
			CheckTwoShapeOverlapped(InTestShape, Location, PlacedShape.Get(), OffsetLength))
			return true;
	}

	FArmyWHCRect rect = InTestShape->GetRect();

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(InTestShape->ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));
	if (World->OverlapMultiByChannel(OverlapResults,
		rect.GetPos(),
		rect.GetRotation().Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(rect.SizeHalf(OffsetLength)),
		QueryParams))
	{
		bool bValidOverlap = false;
		for (const auto &OverlapResult : OverlapResults)
		{
			AActor *OverlapActor = OverlapResult.GetActor();
			if (OverlapActor != nullptr)
			{
				if (OverlapActor->IsA<AXRWallActor>())
				{
					AXRWallActor *OverlapWallActor = Cast<AXRWallActor>(OverlapActor);
					// TODO: 原始墙和外墙也是无效的Overlap，临时解决方案
					// 柜子吸附到的墙面是无效的Overlap，排除这面墙是为了避免精度造成的错误Overlap
					if (OverlapWallActor->Tags.Contains(FName(TEXT("OriginalWall"))) ||
						OverlapWallActor->Tags.Contains(FName(TEXT("OuterWall"))) ||
						(OverlapWallActor->AttachSurface.IsValid() &&
							XRWHCMode::CheckDoublication(OverlapWallActor->AttachSurface->GetPlaneNormal(), rect.GetRotation().RotateVector(FVector::RightVector))))
					{
						bValidOverlap = false;
						continue;
					}
				}
				else if (OverlapActor->IsBasedOnActor(InTestShape->ShapeFrame))
				{
					bValidOverlap = false;
					continue;
				}
				else if (OverlapActor->ActorHasTag(XRActorTag::IgnoreCollision))
				{
					bValidOverlap = false;
					continue;
				}

				bValidOverlap = true;
				break;
			}
		}

		return bValidOverlap;
	}
	else
		return false;
}

bool FArmyWHCModeCabinetOperation::CheckTwoShapeOverlapped(FShapeInRoom *InTestShape, FVector &InTestPosition, FShapeInRoom *InOtherShape, float OffsetLength) const
{
	FArmyWHCRect testRect = InTestShape->GetRect(&InTestPosition);
	FArmyWHCRect otherRect = InOtherShape->GetRect();

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(InTestShape->ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	TArray<FOverlapResult> OverlapResults;
	if (World->OverlapMultiByChannel(OverlapResults,
		testRect.GetPos(),
		testRect.GetRotation().Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(testRect.SizeHalf(OffsetLength)),
		QueryParams))
	{
		for (const auto &OverlapResult : OverlapResults)
		{
			AActor *OverlapActor = OverlapResult.GetActor();
			if (OverlapActor != nullptr && OverlapActor == InOtherShape->ShapeFrame)
				return true;
		}
		return false;
	}
	else
		return false;
}

bool FArmyWHCModeCabinetOperation::CheckShapePenetrateWall(FShapeInRoom *InTestShape, FVector &InTestPosition, AXRWallActor *InWallActor) const
{
	FArmyWHCRect rect = InTestShape->GetRect(&InTestPosition);

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(InTestShape->ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	TArray<FOverlapResult> OverlapResults;
	if (World->OverlapMultiByChannel(OverlapResults, rect.GetPos(),
		rect.GetRotation().Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(FVector(rect.mSizeHalf.X, rect.mSizeHalf.Y, rect.SizezHalf())),
		QueryParams))
	{
		for (const auto &OverlapResult : OverlapResults)
		{
			AActor *OverlapActor = OverlapResult.GetActor();
			if (OverlapActor != nullptr && OverlapActor == InWallActor)
				return true;
		}
	}
	return false;
}

AActor* FArmyWHCModeCabinetOperation::ShapeFrameHitTest(const TArray<HActor*> &InHActorArr) const
{
	for (const auto &HActor : InHActorArr)
	{
		if (HActor->Actor->IsA<ASCTShapeActor>() || HActor->Actor->IsA<AXRElecDeviceActor>())
		{
			AActor *ParentActor = HActor->Actor->GetAttachParentActor();
			while (ParentActor && !ParentActor->IsA<AXRShapeFrame>())
				ParentActor = ParentActor->GetAttachParentActor();
			if (ParentActor != nullptr)
				return ParentActor;
		}
	}
	return nullptr;
}

AActor* FArmyWHCModeCabinetOperation::ShapeActorHitTest(const TArray<HActor*> &InHActorArr) const
{
	for (const auto &HActor : InHActorArr)
	{
		if (HActor->Actor->IsA<ASCTShapeActor>())
		{
			ASCTShapeActor * ShapeActor = Cast<ASCTShapeActor>(HActor->Actor);
			FSCTShape *Shape = ShapeActor->GetShape();
			if (Shape != nullptr)
			{
				switch (Shape->GetShapeType())
				{
				case ST_Decoration:
				{
					ASCTShapeActor * AttachActor = ShapeActor->GetActorAttachTo();
					while (AttachActor && AttachActor->GetShape() == nullptr)
					{
						AttachActor = AttachActor->GetActorAttachTo();
					}

					if (AttachActor)
					{
						switch (AttachActor->GetShape()->GetShapeType())
						{
						case ST_VariableAreaDoorSheet_ForSideHungDoor:
							return AttachActor;
						case ST_VariableAreaDoorSheet_ForDrawerDoor:
							return AttachActor->GetActorAttachTo()->GetActorAttachTo(); // 返回抽屉
						default:
							continue;
						}
					}
					break;
				}
				case ST_Accessory:
				{
					if (Shape->GetShapeCategory() == (int32)EMetalsType::MT_EMBEDDED_APPLIANCE)
						return ShapeActor;
					break;
				}
				}
			}
		}
	}
	return nullptr;
}

AActor* FArmyWHCModeCabinetOperation::ShapeAccessoryHitTest(const TArray<HActor*> &InHActorArr) const
{
	for (const auto &HActor : InHActorArr)
	{
		if (HActor->Actor->IsA<AXRElecDeviceActor>())
			return HActor->Actor;
	}
	return nullptr;
}

AActor* FArmyWHCModeCabinetOperation::ShapeTableHitTest(const TArray<HActor*> &InHActorArr) const
{
	for (const auto &HActor : InHActorArr)
	{
		// if (HActor->Actor->IsA<AXRWhcGapActor>())
		// return HActor->Actor;
		if (HActor->Actor->IsA<AXRShapeTableActor>())
		{
			AXRShapeTableActor *ShapeTableActor = Cast<AXRShapeTableActor>(HActor->Actor);
			if (!ShapeTableActor->IsSingleTable())
				return ShapeTableActor;
		}
	}
	return nullptr;
}

void FArmyWHCModeCabinetOperation::CalculateShapeDistanceFromWall(FShapeInRoom *InShape)
{
	if ((RulerCreationFlag & ERulerVF_Env) == 0 || (RulerVisibilityFlag & ERulerVF_Env) == 0)
		return;
	FArmyWHCRect CabinetRect = InShape->GetRect();
	FArmyWHCPoly* pPoly = gCabinMgr.GetPolyById(InShape->RoomAttachedIndex);
	if (pPoly == nullptr)
		return;

	float HalfShapeWidth = CabinetRect.mSizeHalf.X;
	float HalfShapeDepth = CabinetRect.mSizeHalf.Y;
	float HalfShapeHeight = CabinetRect.SizezHalf();
	// 目前只有宽度方向会有变化
	float OriginalHalfWidth = InShape->Shape->GetShapeWidth() * 0.05f;
	float LeftDecBoard = InShape->GetSunBoardWidth(0);
	float RightDecBoard = InShape->GetSunBoardWidth(1);

	//left
	FVector Intersection;
	FVector LeftWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXLeft, FArmyWHCRect::eYFront, FArmyWHCRect::eZTop);
	FVector LeftDir = -CabinetRect.GetDir();
	LeftDistanceCache = MAX_FLT;
	if (pPoly->RayCollisonWall(LeftWallIntersectionStart, LeftDir, 0, FArmySceneData::WallHeight, Intersection))
	{
		float Length = (Intersection - LeftWallIntersectionStart).Size();
		LeftDistanceCache = Length;
		UpdateRuler(LeftDistanceRuler.Get(), Length, FVector::RightVector * HalfShapeDepth * 2.0f + FVector::UpVector * HalfShapeHeight * 2.0f - FVector::ForwardVector * (Length + OriginalHalfWidth + LeftDecBoard));
	}
	else
	{
		UpdateRuler(LeftDistanceRuler.Get(), 0.0f, FVector::ZeroVector);
	}
	//right
	FVector RightWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXRight, FArmyWHCRect::eYFront, FArmyWHCRect::eZTop);
	FVector RightDir = CabinetRect.GetDir();
	RightDistanceCache = MAX_FLT;
	if (pPoly->RayCollisonWall(RightWallIntersectionStart, RightDir, 0, FArmySceneData::WallHeight, Intersection))
	{
		float Length = (Intersection - RightWallIntersectionStart).Size();
		RightDistanceCache = Length;
		UpdateRuler(RightDistanceRuler.Get(), Length, FVector::ForwardVector * (OriginalHalfWidth + RightDecBoard) + FVector::RightVector * HalfShapeDepth * 2.0f + FVector::UpVector * HalfShapeHeight * 2.0f);
	}
	else
	{
		UpdateRuler(RightDistanceRuler.Get(), 0.0f, FVector::ZeroVector);
	}
	//back
	FVector BackWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXRight, FArmyWHCRect::eYBack, FArmyWHCRect::eZTop);
	FVector BackDir = -CabinetRect.GetNor();
	BackDistanceCache = MAX_FLT;
	if (pPoly->RayCollisonWall(BackWallIntersectionStart, BackDir, 0, FArmySceneData::WallHeight, Intersection))
	{
		float Length = (Intersection - BackWallIntersectionStart).Size();
		BackDistanceCache = Length;
		UpdateRuler(BackDistanceRuler.Get(), Length, FVector::ForwardVector * (OriginalHalfWidth + RightDecBoard) + FVector::UpVector * HalfShapeHeight * 2.0f - FVector::RightVector * Length);
	}
	else
	{
		UpdateRuler(BackDistanceRuler.Get(), 0.0f, FVector::ZeroVector);
	}

	float TableExtern = 0.0f;
	if (InShape->Shape->GetShapeCategory() == int32(ECabinetType::EType_OnGroundCab))
		TableExtern = gCabinMgr.GetTableHeight();
	//top

	UpdateRuler(RoofDistanceRuler.Get(),
		FArmySceneData::WallHeight - InShape->AboveGround - HalfShapeHeight * 2.0f - TableExtern,
		FVector::ForwardVector * (OriginalHalfWidth + RightDecBoard) + FVector::RightVector * HalfShapeDepth * 2.0f + FVector::UpVector * (HalfShapeHeight * 2.0f + TableExtern));
	//bottom
	UpdateRuler(FloorDistanceRuler.Get(),
		InShape->AboveGround,
		FVector::ForwardVector * (OriginalHalfWidth + RightDecBoard) + FVector::RightVector * HalfShapeDepth * 2.0f - FVector::UpVector * InShape->AboveGround);

	// 和物体之间的标尺		
	FVector ShapeLocation = CabinetRect.GetPos(FVector(0, -1, -1));
	FRotator ShapeRotation = InShape->ShapeFrame->GetActorRotation();

	FVector ShapeForward = ShapeRotation.RotateVector(FVector::ForwardVector);
	FVector ShapeRight = ShapeRotation.RotateVector(FVector::RightVector);

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	auto WallDistanceCompare = [](float InSrc, float InDest) -> bool {
		if (FMath::IsNearlyEqual(InSrc, InDest, 0.1f))
			return false;
		else
			return true;
	};
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(InShape->ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	bool LeftHasBlocking = false;
	LeftObjDistanceCache = MAX_FLT;
	LeftWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXLeft, FArmyWHCRect::eYCentry, FArmyWHCRect::eZCentry);
	FHitResult HitResultLeft;
	if (World->SweepSingleByChannel(HitResultLeft,
		LeftWallIntersectionStart,
		LeftWallIntersectionStart - ShapeForward * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(0.1f, HalfShapeDepth, HalfShapeHeight)),
		QueryParams))
	{
		AActor *LeftHitActor = HitResultLeft.GetActor();
		if (LeftHitActor != nullptr && LeftHitActor->IsA<AXRShapeFrame>())
		{
			AXRShapeFrame* pFrame = (AXRShapeFrame*)LeftHitActor;
			LeftObjDistanceCache = HitResultLeft.Distance + 0.1f;
			UpdateRuler(LeftObjDistanceRuler.Get(), LeftObjDistanceCache, FVector::UpVector * HalfShapeHeight * 2.0f - FVector::ForwardVector * (OriginalHalfWidth + HitResultLeft.Distance + LeftDecBoard));
			LeftHasBlocking = true;
		}
	}
	TArray<FHitResult> Hits;
	if (World->SweepMultiByChannel(Hits,
		LeftWallIntersectionStart,
		LeftWallIntersectionStart - ShapeForward * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(FVector(0.1f, HalfShapeDepth, HalfShapeHeight)),
		QueryParams))
	{
		for (auto &Hit : Hits)
		{
			AActor *LeftHitActor = Hit.GetActor();
			if (LeftHitActor != nullptr && LeftHitActor->IsA<AXRWallActor>())
			{
				AXRWallActor *WallActor = Cast<AXRWallActor>(LeftHitActor);
				float RulerLength = Hit.Distance + 0.1f;
				if (WallActor->AttachSurface.IsValid() &&
					!XRWHCMode::CheckDoublication(WallActor->AttachSurface->GetPlaneNormal(), InShape->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector)) &&
					WallDistanceCompare(RulerLength, LeftDistanceRuler->GetRulerLength()))
				{
					LeftObjDistanceCache = FMath::Min(LeftObjDistanceCache, RulerLength);
					UpdateRuler(LeftObjDistanceRuler.Get(), LeftObjDistanceCache, FVector::UpVector * HalfShapeHeight * 2.0f - FVector::ForwardVector * (OriginalHalfWidth + LeftObjDistanceCache + LeftDecBoard));
					LeftHasBlocking = true;
				}
			}
		}
	}
	if (!LeftHasBlocking)
	{
		UpdateRuler(LeftObjDistanceRuler.Get(), 0.0f, FVector::ZeroVector);
	}

	bool RightHasBlocking = false;
	RightObjDistanceCache = MAX_FLT;
	RightWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXRight, FArmyWHCRect::eYCentry, FArmyWHCRect::eZCentry);
	FHitResult HitResultRight;
	if (World->SweepSingleByChannel(HitResultRight,
		RightWallIntersectionStart,
		RightWallIntersectionStart + ShapeForward * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(0.1f, HalfShapeDepth, HalfShapeHeight)),
		QueryParams))
	{
		AActor *RightHitActor = HitResultRight.GetActor();
		if (RightHitActor != nullptr && RightHitActor->IsA<AXRShapeFrame>())
		{
			AXRShapeFrame* pFrame = (AXRShapeFrame*)RightHitActor;
			RightObjDistanceCache = HitResultRight.Distance + 0.1f;
			UpdateRuler(RightObjDistanceRuler.Get(), RightObjDistanceCache, FVector::UpVector * HalfShapeHeight * 2.0f + FVector::ForwardVector * (OriginalHalfWidth + RightDecBoard));
			RightHasBlocking = true;
		}
	}
	if (World->SweepMultiByChannel(Hits,
		RightWallIntersectionStart,
		RightWallIntersectionStart + ShapeForward * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(FVector(0.1f, HalfShapeDepth, HalfShapeHeight)),
		QueryParams))
	{
		for (auto &Hit : Hits)
		{
			AActor *RightHitActor = Hit.GetActor();
			if (RightHitActor != nullptr && RightHitActor->IsA<AXRWallActor>())
			{
				AXRWallActor *WallActor = Cast<AXRWallActor>(RightHitActor);
				float RulerLength = Hit.Distance + 0.1f;
				if (WallActor->AttachSurface.IsValid() &&
					!XRWHCMode::CheckDoublication(WallActor->AttachSurface->GetPlaneNormal(), InShape->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector)) &&
					WallDistanceCompare(RulerLength, RightDistanceRuler->GetRulerLength()))
				{
					RightObjDistanceCache = FMath::Min(RightObjDistanceCache, RulerLength);
					UpdateRuler(RightObjDistanceRuler.Get(), RightObjDistanceCache, FVector::UpVector * HalfShapeHeight * 2.0f + FVector::ForwardVector * (OriginalHalfWidth + RightDecBoard));
					RightHasBlocking = true;
				}
			}
		}
	}
	if (!RightHasBlocking)
	{
		UpdateRuler(RightObjDistanceRuler.Get(), 0.0f, FVector::ZeroVector);
	}

	FHitResult HitResultBack;
	bool BackHasBlocking = false;
	BackObjDistanceCache = MAX_FLT;
	BackWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXCentry, FArmyWHCRect::eYBack, FArmyWHCRect::eZCentry);
	if (World->SweepSingleByChannel(HitResultBack,
		BackWallIntersectionStart,
		BackWallIntersectionStart - ShapeRight * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(HalfShapeWidth, 0.1f, HalfShapeHeight)),
		QueryParams))
	{
		AActor *BackHitActor = HitResultBack.GetActor();
		if (BackHitActor != nullptr && BackHitActor->IsA<AXRShapeFrame>())
		{
			BackObjDistanceCache = HitResultBack.Distance;
			UpdateRuler(BackObjDistanceRuler.Get(), BackObjDistanceCache, FVector::UpVector * HalfShapeHeight * 2.0f - FVector::RightVector * HitResultBack.Distance - FVector::ForwardVector * (OriginalHalfWidth + LeftDecBoard));
			BackHasBlocking = true;
		}
	}
	if (World->SweepMultiByChannel(Hits,
		BackWallIntersectionStart,
		BackWallIntersectionStart - ShapeRight * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WorldDynamic,
		FCollisionShape::MakeBox(FVector(HalfShapeWidth, 0.1f, HalfShapeHeight)),
		QueryParams))
	{
		for (auto &Hit : Hits)
		{
			AActor *BackHitActor = Hit.GetActor();
			if (BackHitActor != nullptr && BackHitActor->IsA<AXRWallActor>())
			{
				AXRWallActor *WallActor = Cast<AXRWallActor>(BackHitActor);
				float RulerLength = Hit.Distance + 0.1f;
				if (WallActor->AttachSurface.IsValid() &&
					!XRWHCMode::CheckDoublication(WallActor->AttachSurface->GetPlaneNormal(), InShape->ShapeFrame->GetActorRotation().RotateVector(FVector::RightVector)) &&
					WallDistanceCompare(RulerLength, BackDistanceRuler->GetRulerLength()))
				{
					BackObjDistanceCache = FMath::Min(BackObjDistanceCache, RulerLength);
					UpdateRuler(BackObjDistanceRuler.Get(), BackObjDistanceCache, FVector::UpVector * HalfShapeHeight * 2.0f - FVector::RightVector * BackObjDistanceCache - FVector::ForwardVector * (OriginalHalfWidth + LeftDecBoard));
					BackHasBlocking = true;
				}
			}
		}
	}
	if (!BackHasBlocking)
	{
		UpdateRuler(BackObjDistanceRuler.Get(), 0.0f, FVector::ZeroVector);
	}

	FVector FloorWallIntersectionStart = ShapeLocation + ShapeRight * HalfShapeDepth;
	FVector RoofWallIntersectionStart = FloorWallIntersectionStart + FVector::UpVector * HalfShapeHeight * 2.0f;

	FHitResult HitResultRoof;
	if (World->SweepSingleByChannel(HitResultRoof,
		RoofWallIntersectionStart,
		RoofWallIntersectionStart + FVector::UpVector * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(HalfShapeWidth, HalfShapeDepth, 0.1f)),
		QueryParams))
	{
		if (HitResultRoof.GetActor() && HitResultRoof.GetActor()->IsA<AXRShapeFrame>())
		{
			RoofObjDistanceRuler->SetRulerLength(HitResultRoof.Distance);
			RoofObjDistanceRuler->SetRulerLocation(FVector::RightVector * HalfShapeDepth * 2.0f + FVector::UpVector * HalfShapeHeight * 2.0f - FVector::ForwardVector * (OriginalHalfWidth + LeftDecBoard));
		}
		else
			RoofObjDistanceRuler->SetRulerLength(0.0f);
	}
	else
		RoofObjDistanceRuler->SetRulerLength(0.0f);

	FHitResult HitResultFloor;
	if (World->SweepSingleByChannel(HitResultFloor,
		FloorWallIntersectionStart,
		FloorWallIntersectionStart - FVector::UpVector * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_WHCShape,
		FCollisionShape::MakeBox(FVector(HalfShapeWidth, HalfShapeDepth, 0.1f)),
		QueryParams))
	{
		if (HitResultFloor.GetActor() && HitResultFloor.GetActor()->IsA<AXRShapeFrame>())
		{
			float TableExtern = 0.0f;
			if (((AXRShapeFrame*)HitResultFloor.GetActor())->ShapeInRoomRef->Shape->GetShapeCategory() == int32(ECabinetType::EType_OnGroundCab))
				TableExtern = gCabinMgr.GetTableHeight();

			FloorObjDistanceRuler->SetRulerLength(HitResultFloor.Distance - TableExtern);
			FloorObjDistanceRuler->SetRulerLocation(FVector::RightVector * HalfShapeDepth * 2.0f - FVector::ForwardVector * (OriginalHalfWidth + LeftDecBoard) - FVector::UpVector * (HitResultFloor.Distance - TableExtern));
		}
		else
			FloorObjDistanceRuler->SetRulerLength(0.0f);
	}
	else
		FloorObjDistanceRuler->SetRulerLength(0.0f);

}

void FArmyWHCModeCabinetOperation::UpdateLastSelected(AActor *InNewLastSelected)
{
	if (InNewLastSelected && (!InNewLastSelected->IsA<AXRElecDeviceActor>()))
	{
		SelectedWireframe->AttachToActor(InNewLastSelected, FAttachmentTransformRules::KeepRelativeTransform);
		SelectedWireframe->SetActorRelativeLocation(FVector::ZeroVector);
		SelectedWireframe->SetActorRelativeRotation(FRotator::ZeroRotator);
	}

	LastSelected = InNewLastSelected;
	LastChildItemSelected = InNewLastSelected;
}

void FArmyWHCModeCabinetOperation::ClearSelection()
{
	LastSelected = LastChildItemSelected = nullptr;
	HighlightSelected();
}

void FArmyWHCModeCabinetOperation::ClearDistanceRulerBinding()
{
#define DETACH_RULER(RulerObj) \
    if (RulerObj.IsValid()) \
        RulerObj->GetRulerActor()->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	DETACH_RULER(LeftDistanceRuler)
		DETACH_RULER(RightDistanceRuler)
		DETACH_RULER(BackDistanceRuler)
		DETACH_RULER(RoofDistanceRuler)
		DETACH_RULER(FloorDistanceRuler)

		DETACH_RULER(LeftObjDistanceRuler)
		DETACH_RULER(RightObjDistanceRuler)
		DETACH_RULER(BackObjDistanceRuler)
		DETACH_RULER(RoofObjDistanceRuler)
		DETACH_RULER(FloorObjDistanceRuler)
}

void FArmyWHCModeCabinetOperation::SwitchDistanceRulerBinding(FShapeInRoom *InNewShape)
{
	ClearDistanceRulerBinding();

	LeftDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RightDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	BackDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RoofDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	FloorDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);

	LeftObjDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RightObjDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	BackObjDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RoofObjDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	FloorObjDistanceRuler->GetRulerActor()->AttachToActor(InNewShape->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);

	// 特殊逻辑，地柜和高柜不允许在标尺上修改数值
	int32 Category = InNewShape->Shape->GetShapeCategory();
	if (Category == (int32)ECabinetType::EType_OnGroundCab || Category == (int32)ECabinetType::EType_TallCab)
	{
		RoofDistanceRuler->SetRulerValueIsReadOnly(true);
		FloorDistanceRuler->SetRulerValueIsReadOnly(true);
		RoofObjDistanceRuler->SetRulerValueIsReadOnly(true);
		FloorObjDistanceRuler->SetRulerValueIsReadOnly(true);
	}
	else
	{
		RoofDistanceRuler->SetRulerValueIsReadOnly(false);
		FloorDistanceRuler->SetRulerValueIsReadOnly(false);
		RoofObjDistanceRuler->SetRulerValueIsReadOnly(false);
		FloorObjDistanceRuler->SetRulerValueIsReadOnly(false);
	}
}

void FArmyWHCModeCabinetOperation::ShowShapeDistanceRulerInternal(bool bShow)
{
	if (LeftDistanceRuler.IsValid())
		LeftDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(LeftDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (RightDistanceRuler.IsValid())
		RightDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(RightDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (BackDistanceRuler.IsValid())
		BackDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(BackDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (RoofDistanceRuler.IsValid())
		RoofDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(RoofDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (FloorDistanceRuler.IsValid())
		FloorDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(FloorDistanceRuler->GetRulerLength(), 0.0f, 0.1f));

	if (LeftObjDistanceRuler.IsValid())
		LeftObjDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(LeftObjDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (RightObjDistanceRuler.IsValid())
		RightObjDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(RightObjDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (BackObjDistanceRuler.IsValid())
		BackObjDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(BackObjDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (RoofObjDistanceRuler.IsValid())
		RoofObjDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(RoofObjDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
	if (FloorObjDistanceRuler.IsValid())
		FloorObjDistanceRuler->ShowRuler(bShow && XRWHCMode::FloatGreater(FloorObjDistanceRuler->GetRulerLength(), 0.0f, 0.1f));
}

TArray<HActor*> FArmyWHCModeCabinetOperation::GetHActorArray(int32 X, int32 Y) const
{
	const int32 HitProxySize = 5;
	int32	MinX = X - HitProxySize,
		MinY = Y - HitProxySize,
		MaxX = X + HitProxySize,
		MaxY = Y + HitProxySize;

	FIntPoint VPSize = GVC->Viewport->GetSizeXY();

	// Clip the region to the viewport bounds.
	MinX = FMath::Clamp(MinX, 0, VPSize.X - 1);
	MinY = FMath::Clamp(MinY, 0, VPSize.Y - 1);
	MaxX = FMath::Clamp(MaxX, 0, VPSize.X - 1);
	MaxY = FMath::Clamp(MaxY, 0, VPSize.Y - 1);

	int32 TestSizeX = MaxX - MinX + 1;
	int32 TestSizeY = MaxY - MinY + 1;

	if (TestSizeX > 0 && TestSizeY > 0)
	{
		TArray<HHitProxy*> ProxyMap;
		GVC->Viewport->GetHitProxyMap(FIntRect(MinX, MinY, MaxX + 1, MaxY + 1), ProxyMap);
		check(ProxyMap.Num() == TestSizeX * TestSizeY);
		TArray<HActor*> RetHActorMap;
		for (int32 TestY = 0; TestY < TestSizeY; ++TestY)
		{
			for (int32 TestX = 0; TestX < TestSizeX; ++TestX)
			{
				HHitProxy * TestProxy = ProxyMap[TestY * TestSizeX + TestX];
				if (TestProxy != nullptr && TestProxy->IsA(HActor::StaticGetType()))
					RetHActorMap.Emplace(static_cast<HActor*>(TestProxy));
			}
		}
		return MoveTemp(RetHActorMap);
	}

	return TArray<HActor*>();
}

void FArmyWHCModeCabinetOperation::HighlightSelected()
{
	if (LastSelected == nullptr)
		SelectedWireframe->SetActorHiddenInGame(true);
	else if (LastSelected->IsA<AXRShapeFrame>())
	{
		AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(LastSelected);
		DisplayHighlight(ShapeFrame->ShapeInRoomRef->Shape.Get(),
			ShapeFrame,
			-FVector::ForwardVector * ShapeFrame->ShapeInRoomRef->Shape->GetShapeWidth() * 0.05f,
			FRotator::ZeroRotator
		);
	}
	else if (LastSelected->IsA<ASCTShapeActor>())
	{
		SelectedWireframe->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

		ASCTShapeActor * ShapeActor = Cast<ASCTShapeActor>(LastSelected);
		EShapeType ShapeType = ShapeActor->GetShape()->GetShapeType();
		switch (ShapeType)
		{
		case ST_VariableAreaDoorSheet_ForSideHungDoor:
		case ST_SlidingDrawer:
			DisplayHighlight(ShapeActor->GetShape(),
				ShapeActor,
				FVector::ZeroVector,
				FRotator::ZeroRotator
			);
			break;
		case ST_Accessory:
			DisplayHighlight(ShapeActor->GetShape(),
				ShapeActor,
				FVector(-ShapeActor->GetShape()->GetShapeWidth() * 0.05f, -ShapeActor->GetShape()->GetShapeDepth() * 0.05f, 0.0f),
				FRotator::ZeroRotator
			);
			break;
		}
	}
	else
		SelectedWireframe->SetActorHiddenInGame(true);
}

void FArmyWHCModeCabinetOperation::DisplayHighlight(FSCTShape *InShape, AActor *ActorAttachedTo, const FVector &RelLocation, const FRotator &RelRotation)
{
	SelectedWireframe->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SelectedWireframe->SetShape(InShape);
	SelectedWireframe->SetActorDimension(
		InShape->GetShapeWidth(),
		InShape->GetShapeDepth(),
		InShape->GetShapeHeight()
	);
	SelectedWireframe->AttachToActor(ActorAttachedTo, FAttachmentTransformRules::KeepRelativeTransform);
	SelectedWireframe->SetActorRelativeLocation(RelLocation);
	SelectedWireframe->SetActorRelativeRotation(RelRotation);
	SelectedWireframe->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	SelectedWireframe->SetActorHiddenInGame(false);
}

void FArmyWHCModeCabinetOperation::MakeFrontBoardCabChangeItem(FShapeInRoom *InShape, int32 InNumDoors, float InStartPoint, float InEndPoint)
{
	int32 Index = InShape->ChangeList.IndexOfByPredicate([](const TSharedPtr<IArmyWHCModeShapeChange> &Data) -> bool {
		return Data->IsSameType(SHAPE_CHANGE_FRONT_BOARD_CAB);
	});
	FArmyWHCFrontBoardCabinetChange *FrontBoardCabChange = nullptr;
	if (Index == INDEX_NONE)
	{
		FrontBoardCabChange = new FArmyWHCFrontBoardCabinetChange(InShape->Shape.Get());
		InShape->ChangeList.Emplace(MakeShareable(FrontBoardCabChange));
	}
	else if (Index >= 0 && Index < InShape->ChangeList.Num() - 1)
	{
		// 插脚柜的修改不是最后一个修改项
		InShape->ChangeList.RemoveAt(Index);
		FrontBoardCabChange = new FArmyWHCFrontBoardCabinetChange(InShape->Shape.Get());
		InShape->ChangeList.Emplace(MakeShareable(FrontBoardCabChange));
	}
	else
		FrontBoardCabChange = StaticCastSharedPtr<FArmyWHCFrontBoardCabinetChange>(InShape->ChangeList[Index]).Get();
	FrontBoardCabChange->SetNumDoors(InNumDoors);
	FrontBoardCabChange->SetDoorPoints(InStartPoint, InEndPoint);
}

void FArmyWHCModeCabinetOperation::UpdateHighlight()
{
	if (LastSelected != nullptr/* && SelectedWireframe->bHidden == 0*/)
	{
		if (LastSelected->IsA<AXRShapeFrame>())
		{
			AXRShapeFrame * ShapeFrame = Cast<AXRShapeFrame>(LastSelected);
			DisplayHighlight(ShapeFrame->ShapeInRoomRef->Shape.Get(),
				ShapeFrame,
				-FVector::ForwardVector * ShapeFrame->ShapeInRoomRef->Shape->GetShapeWidth() * 0.05f,
				FRotator::ZeroRotator
			);
		}
		else if (LastSelected->IsA<ASCTShapeActor>())
		{
			ASCTShapeActor * ShapeActor = Cast<ASCTShapeActor>(LastSelected);
			EShapeType ShapeType = ShapeActor->GetShape()->GetShapeType();
			switch (ShapeType)
			{
			case ST_VariableAreaDoorSheet_ForSideHungDoor:
			case ST_SlidingDrawer:
				DisplayHighlight(ShapeActor->GetShape(),
					ShapeActor,
					FVector::ZeroVector,
					FRotator::ZeroRotator);
				break;
			case ST_Accessory:
				DisplayHighlight(ShapeActor->GetShape(),
					ShapeActor,
					FVector(-ShapeActor->GetShape()->GetShapeWidth() * 0.05f, -ShapeActor->GetShape()->GetShapeDepth() * 0.05f, 0.0f),
					FRotator::ZeroRotator
				);
				break;
			}
		}
	}
}

void FArmyWHCModeCabinetOperation::CabinetList(FArmyWHCModeListData& OutWHCModeData, int32 InType)
{
	struct FStatElems
	{
		void operator() (const TSharedPtr<FCabinetShape> &InCabShape, const TSharedPtr<FSpaceShape> & InSpaceShape, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			StatDoors(InCabShape, InSpaceShape, OutListData);
			StatDrawers(InCabShape, InSpaceShape, OutListData);

			FStatElems StatElems;

			const TArray<TSharedPtr<FSpaceShape>> & ChildSpaces = InSpaceShape->GetChildSpaceShapes();
			if (ChildSpaces.Num() > 0)
			{
				for (const auto & ChildSpace : ChildSpaces)
					StatElems(InCabShape, ChildSpace, OutListData);
			}
			else if (InSpaceShape->GetInsideSpace().IsValid())
			{
				const TArray<TSharedPtr<FSpaceShape>> & ChildInsideSpaces = InSpaceShape->GetInsideSpace()->GetChildSpaceShapes();
				for (const auto & ChildInsideSpace : ChildInsideSpaces)
					StatElems(InCabShape, ChildInsideSpace, OutListData);
			}
		}

		void StatLateralHandle(FDoorGroup *DoorGroup, int32 CurrentHandleId, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			const TSharedPtr<FSCTShape> * OptionShapePtr = DoorGroup->GetOptionShapeMap().Find(CurrentHandleId);
			if (OptionShapePtr != nullptr)
			{
				const TSharedPtr<FSCTShape> & OptionShape = *OptionShapePtr;

				FString BrandName, VersionName;
				if (OptionShape->GetShapeType() == ST_Accessory)
				{
					TSharedPtr<FAccessoryShape> AccShape = StaticCastSharedPtr<FAccessoryShape>(OptionShape);
					BrandName = AccShape->GetBrandName();
					VersionName = AccShape->GetProductVersion();
				}

				FArmyWHCListCabinetElemData ListElemData;
				ListElemData.Id = OptionShape->GetShapeId();
				ListElemData.Name = OptionShape->GetShapeName();
				ListElemData.Type = OptionShape->GetShapeType();
				ListElemData.Code = OptionShape->GetShapeCode();
				ListElemData.ThumbnailUrl = OptionShape->GetThumbnailUrl();
				ListElemData.BrandName = BrandName;
				ListElemData.Version = VersionName;
				ListElemData.Wdh = FString::Printf(TEXT("%d*%d*%d"),
					FMath::RoundToInt(DoorGroup->GetShapeWidth()),
					FMath::RoundToInt(OptionShape->GetShapeDepth()),
					FMath::RoundToInt(OptionShape->GetShapeHeight()));
				ListElemData.Quantity = DoorGroup->GetShapeWidth() * 0.001f;
				ListElemData.QuantityStr = FString::Printf(TEXT("%.3f"), ListElemData.Quantity);
				ListElemData.QuantityUnit = TEXT("米");
				OutListData.Emplace(ListElemData);
			}
		}

		// TODO: CurrentHandleType EHandleMetalType枚举提到类外层
		void StatOtherHandle(FDoorGroup *SideHungDoor, FVariableAreaDoorSheet *DoorSheet, int8 CurrentHandleType, int32 CurrentHandleId, int32 AddCount, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			const TSharedPtr<FSCTShape> * OptionShapePtr = DoorSheet->GetOptionShapeMap().Find(CurrentHandleId);
			if (OptionShapePtr != nullptr)
			{
				const TSharedPtr<FSCTShape> & OptionShape = *OptionShapePtr;

				const FString & HandleName = OptionShape->GetShapeName();
				FString BrandName, VersionName, WDH, QuantityUnit;
				float Width = 0.0f;
				float Quantity = 0.0f;

				if (OptionShape->GetShapeType() == ST_Accessory)
				{
					TSharedPtr<FAccessoryShape> AccShape = StaticCastSharedPtr<FAccessoryShape>(OptionShape);
					BrandName = AccShape->GetBrandName();
					VersionName = AccShape->GetProductVersion();
				}

				switch (CurrentHandleType)
				{
				case 0:
					Width = OptionShape->GetShapeWidth();
					Quantity = (float)AddCount;
					QuantityUnit = TEXT("个");
					break;
				case 2:
					Width = DoorSheet->GetShapeWidth();
					Quantity = Width * 0.001f;
					QuantityUnit = TEXT("米");
					break;
				}
				WDH = FString::Printf(TEXT("%d*%d*%d"),
					FMath::RoundToInt(Width),
					FMath::RoundToInt(OptionShape->GetShapeDepth()),
					FMath::RoundToInt(OptionShape->GetShapeHeight()));

				FArmyWHCListCabinetElemData * ListElemDataPtr = OutListData.FindByPredicate(
					[&HandleName, &BrandName, &VersionName, &WDH](FArmyWHCListCabinetElemData &Item)
				{
					return HandleName == Item.Name &&
						BrandName == Item.BrandName &&
						VersionName == Item.Version &&
						WDH == Item.Wdh;
				}
				);

				if (ListElemDataPtr == nullptr)
				{
					FArmyWHCListCabinetElemData ListElemData;
					ListElemData.Id = OptionShape->GetShapeId();
					ListElemData.Name = HandleName;
					ListElemData.Type = OptionShape->GetShapeType();
					ListElemData.Code = OptionShape->GetShapeCode();
					ListElemData.ThumbnailUrl = OptionShape->GetThumbnailUrl();
					ListElemData.BrandName = BrandName;
					ListElemData.Version = VersionName;
					ListElemData.Wdh = WDH;
					ListElemData.Quantity = Quantity;
					ListElemData.QuantityUnit = QuantityUnit;
					switch (CurrentHandleType)
					{
					case 0:
						ListElemData.QuantityStr = FString::Printf(TEXT("%d"), (int32)Quantity);
						break;
					case 2:
						ListElemData.QuantityStr = FString::Printf(TEXT("%.3f"), Quantity);
						break;
					}
					OutListData.Emplace(ListElemData);
				}
				else
				{
					ListElemDataPtr->Quantity += Quantity;
					switch (CurrentHandleType)
					{
					case 0:
						ListElemDataPtr->QuantityStr = FString::Printf(TEXT("%d"), (int32)ListElemDataPtr->Quantity);
						break;
					case 2:
						ListElemDataPtr->QuantityStr = FString::Printf(TEXT("%.3f"), ListElemDataPtr->Quantity);
						break;
					}
				}


			}
		}

		void StatDoors(const TSharedPtr<FCabinetShape> &InCabShape, const TSharedPtr<FSpaceShape> & InSpaceShape, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			TSharedPtr<FDoorGroup> pDoorGroup = InSpaceShape->GetDoorShape();
			if (!pDoorGroup.IsValid())
			{
				if (InSpaceShape->GetInsideSpace().IsValid())
					pDoorGroup = InSpaceShape->GetInsideSpace()->GetDoorShape();
			}
			if (pDoorGroup.IsValid())
			{
				switch (pDoorGroup->GetShapeType())
				{
				case ST_SideHungDoor:
				{
					TSharedPtr<FSideHungDoor> SideHungDoor = StaticCastSharedPtr<FSideHungDoor>(pDoorGroup);
					// 统计箱体拉手
					bool IsUsingLateralHandle = SideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND;
					if (IsUsingLateralHandle)
						StatLateralHandle(SideHungDoor.Get(), SideHungDoor->GetCurrentHandleInfo().ID, OutListData);

					int32 NumDoors = SideHungDoor->GetDoorSheetNum();
					if (NumDoors > 0)
					{
						const auto & DoorSheet = SideHungDoor->GetDoorSheets()[0];
						const FString & DoorSheetName = DoorSheet->GetShapeName();
						const FDoorSheetShapeBase::FDoorPakMetaData & MtlInfo = DoorSheet->GetMaterial();
						const FString & SubstrateName = DoorSheet->GetSubstrateName();
						const FString & MtlName = MtlInfo.Name;
						FString WDH = FString::Printf(TEXT("%d*%d*%d"),
							FMath::RoundToInt(DoorSheet->GetShapeWidth()),
							FMath::RoundToInt(DoorSheet->GetShapeDepth()),
							FMath::RoundToInt(DoorSheet->GetShapeHeight()));

						FString OpenDirStr(TEXT("未知"));
						if (NumDoors == 1)
						{
							FVariableAreaDoorSheet::EDoorOpenDirection OpenDir = DoorSheet->GetOpenDoorDirection();
							switch (OpenDir)
							{
							case FVariableAreaDoorSheet::EDoorOpenDirection::E_None:
								OpenDirStr = TEXT("不开");
								break;
							case FVariableAreaDoorSheet::EDoorOpenDirection::E_Left:
								OpenDirStr = TEXT("左开");
								break;
							case FVariableAreaDoorSheet::EDoorOpenDirection::E_Right:
								OpenDirStr = TEXT("右开");
								break;
							case FVariableAreaDoorSheet::EDoorOpenDirection::E_Top:
								OpenDirStr = TEXT("上翻");
								break;
							}
						}
						else if (NumDoors == 2)
						{
							OpenDirStr = TEXT("对开");
						}

						FArmyWHCListCabinetElemData ListElemData;
						ListElemData.Id = DoorSheet->GetShapeId();
						ListElemData.Name = DoorSheetName;
						ListElemData.Type = DoorSheet->GetShapeType();
						ListElemData.SingleType = 1; // 标识是一个门板
						ListElemData.Code = DoorSheet->GetShapeCode();
						ListElemData.ThumbnailUrl = DoorSheet->GetThumbnailUrl();
						ListElemData.SubstrateName = SubstrateName;
						ListElemData.MaterialId = MtlInfo.ID;
						ListElemData.MaterialName = MtlName;
						ListElemData.Wdh = WDH;
						ListElemData.Quantity = NumDoors;
						ListElemData.QuantityStr = FString::Printf(TEXT("%d"), NumDoors);
						ListElemData.QuantityUnit = TEXT("扇");
						if (!OpenDirStr.IsEmpty())
							ListElemData.OpenDoorDirection.Emplace(OpenDirStr);
						OutListData.Emplace(ListElemData);

						for (const auto & DoorSheet : SideHungDoor->GetDoorSheets())
						{
							// 统计其它拉手
							if ((!IsUsingLateralHandle) && (SideHungDoor->GetHanleType() != FSideHungDoor::EHandleMetalType::E_MT_None))
							{
								StatOtherHandle(SideHungDoor.Get(),
									DoorSheet.Get(),
									(int8)SideHungDoor->GetHanleType(),
									DoorSheet->GetCurrentHandleInfo().ID,
									1,
									OutListData);
							}
						}
					}
					break;
				}
				case ST_SlidingDoor:
				{
					TSharedPtr<FSlidingDoor> SlidingDoor = StaticCastSharedPtr<FSlidingDoor>(pDoorGroup);
					int32 NumOriginSheets = SlidingDoor->GetOriginDoorSheetNum();
					const TArray<TSharedPtr<FSpaceDividDoorSheet>>& DoorSheets = SlidingDoor->GetDoorSheets();
					int32 NumSheets = DoorSheets.Num();
					check(NumSheets % NumOriginSheets == 0);
					int32 Scale = NumSheets / NumOriginSheets;
					for (int32 i = 0; i < NumOriginSheets; ++i)
					{
						const TSharedPtr<FSpaceDividDoorSheet> &DoorSheet = DoorSheets[i];
						FArmyWHCListCabinetElemData ListElemData;
						ListElemData.Id = DoorSheet->GetShapeId();
						ListElemData.Name = DoorSheet->GetShapeName();
						ListElemData.Type = DoorSheet->GetShapeType();
						ListElemData.SingleType = 1; // 标识是一个门板
						ListElemData.Code = DoorSheet->GetShapeCode();
						ListElemData.ThumbnailUrl = DoorSheet->GetThumbnailUrl();
						ListElemData.Quantity = Scale;
						ListElemData.QuantityStr = FString::Printf(TEXT("%d"), Scale);
						ListElemData.QuantityUnit = TEXT("扇");
						OutListData.Emplace(ListElemData);
					}
					// 导轨信息
					TArray<FAccessoryShape*> Slidingways;
					FSCTShapeUtilityTool::GetCabinetSlidingDoorSlidways(InCabShape.Get(), Slidingways);
					for (const auto &Slidingway : Slidingways)
					{
						FArmyWHCListCabinetElemData ListElemData;
						ListElemData.Id = Slidingway->GetShapeId();
						ListElemData.Name = Slidingway->GetShapeName();
						ListElemData.Type = Slidingway->GetShapeType();
						ListElemData.SingleType = 0; // 标识是一个门板
						ListElemData.Code = Slidingway->GetShapeCode();
						ListElemData.ThumbnailUrl = Slidingway->GetThumbnailUrl();
						ListElemData.BrandName = Slidingway->GetBrandName();
						ListElemData.Version = Slidingway->GetProductVersion();
						ListElemData.Wdh = FString::Printf(TEXT("%d*%d*%d"), FMath::RoundToInt(Slidingway->GetShapeWidth()), FMath::RoundToInt(Slidingway->GetShapeDepth()), FMath::RoundToInt(Slidingway->GetShapeHeight()));
						ListElemData.Quantity = 1;
						ListElemData.QuantityStr = TEXT("1");
						ListElemData.QuantityUnit = TEXT("条");
						OutListData.Emplace(ListElemData);
					}
					break;
				}
				}
			}
		}

		void StatDrawers(const TSharedPtr<FCabinetShape> &InCabShape, const TSharedPtr<FSpaceShape> & InSpaceShape, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			TSharedPtr<FDrawerGroupShape> DrawerGroup = InSpaceShape->GetCoveredDrawerGroup();
			if (!DrawerGroup.IsValid())
			{
				if (InSpaceShape->GetInsideSpace().IsValid())
					DrawerGroup = InSpaceShape->GetInsideSpace()->GetCoveredDrawerGroup();
			}
			if (DrawerGroup.IsValid())
			{
				TSharedPtr<FSlidingDrawerShape> Drawer = DrawerGroup->GetPrimitiveDrawer();
				const TArray<TSharedPtr<FDrawerDoorShape>> & DrawerDoors = Drawer->GetDrawerDoorShapes();
				if (DrawerDoors.Num() > 0)
				{
					const TSharedPtr<FDrawerDoorShape> & DrawerDoor = DrawerDoors[0];
					const FString & DrawerDoorName = DrawerDoor->GetShapeName();
					const FDoorSheetShapeBase::FDoorPakMetaData & MtlInfo = DrawerDoor->GetMaterial();
					const FString & SubstrateName = DrawerDoor->GetSubstrateName();
					const FString & MtlName = MtlInfo.Name;
					FString WDH = FString::Printf(TEXT("%d*%d*%d"),
						FMath::RoundToInt(DrawerDoor->GetShapeWidth()),
						FMath::RoundToInt(DrawerDoor->GetShapeDepth()),
						FMath::RoundToInt(DrawerDoor->GetShapeHeight()));

					FArmyWHCListCabinetElemData ListElemData;
					ListElemData.Id = DrawerDoor->GetShapeId();
					ListElemData.Name = DrawerDoorName;
					ListElemData.Type = DrawerDoor->GetShapeType();
					ListElemData.SingleType = 1; // 标识是一个门板
					ListElemData.Code = DrawerDoor->GetShapeCode();
					ListElemData.ThumbnailUrl = DrawerDoor->GetThumbnailUrl();
					ListElemData.SubstrateName = SubstrateName;
					ListElemData.MaterialId = MtlInfo.ID;
					ListElemData.MaterialName = MtlName;
					ListElemData.Wdh = WDH;
					ListElemData.Quantity = 1; // DrawerGroup->GetCurrentCopyCount();
					ListElemData.QuantityStr = TEXT("1"); // FString::Printf(TEXT("%d"), DrawerGroup->GetCurrentCopyCount());
					ListElemData.QuantityUnit = TEXT("个");
					for (int32 i = 0; i < DrawerGroup->GetCurrentCopyCount(); ++i)
						OutListData.Add(ListElemData);

					bool IsUsingLateralHandle = DrawerDoor->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND;
					if (IsUsingLateralHandle)
						StatLateralHandle(DrawerDoor.Get(), DrawerDoor->GetDoorGroupCurrentHandleInfo().ID, OutListData);
					else if (DrawerDoor->GetDoorGroupHanleType() != FDrawerDoorShape::EHandleMetalType::E_MT_None)
					{
						TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> DrawerSheet = DrawerDoor->GetDrawDoorSheet();
						StatOtherHandle(DrawerDoor.Get(),
							DrawerSheet.Get(),
							(int8)DrawerDoor->GetDoorGroupHanleType(),
							DrawerSheet->GetDefaultHandleInfo().ID,
							DrawerGroup->GetCurrentCopyCount(),
							OutListData);
					}
				}
			}
		}
	};

	FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get();

	TArray<TArray<FShapeInRoom*>> CabArrs;
	TArray<TSharedPtr<FShapeInRoom>> & PlacedShapes = CabMgr->GetPlaceShapes();
	// 清单按照地柜、吊柜和高柜的顺序导出
	if (InType == (int32)ECabinetClassify::EClassify_Other)
	{
		TArray<FShapeInRoom*> OtherCabs;
		for (auto & PlacedShape : PlacedShapes)
		{
			ECabinetType CabType = (ECabinetType)PlacedShape->Shape->GetShapeCategory();
			switch (CabType)
			{
			case ECabinetType::EType_Other:
				OtherCabs.Emplace(PlacedShape.Get());
				break;
			}
		}
		CabArrs.Emplace(OtherCabs);
	}
	else if (InType == (int32)ECabinetClassify::EClassify_Wardrobe)
	{
		TArray<FShapeInRoom*> CoverDoorWardrobes;
		TArray<FShapeInRoom*> SlidingDoorWardrobes;
		TArray<FShapeInRoom*> TopWardrobes;
		for (auto & PlacedShape : PlacedShapes)
		{
			ECabinetType CabType = (ECabinetType)PlacedShape->Shape->GetShapeCategory();
			switch (CabType)
			{
			case ECabinetType::EType_CoverDoorWardrobe:
				CoverDoorWardrobes.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_SlidingDoorWardrobe:
				SlidingDoorWardrobes.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_TopCab:
				TopWardrobes.Emplace(PlacedShape.Get());
				break;
			}
		}
		CabArrs.Emplace(CoverDoorWardrobes);
		CabArrs.Emplace(SlidingDoorWardrobes);
		CabArrs.Emplace(TopWardrobes);
	}
	else if (InType == (int32)ECabinetClassify::EClassify_Cabinet)
	{
		TArray<FShapeInRoom*> OnGroundCabs;
		TArray<FShapeInRoom*> HangCabs;
		TArray<FShapeInRoom*> OnCabCabs;
		TArray<FShapeInRoom*> TallCabs;
		for (auto & PlacedShape : PlacedShapes)
		{
			ECabinetType CabType = (ECabinetType)PlacedShape->Shape->GetShapeCategory();
			switch (CabType)
			{
			case ECabinetType::EType_OnGroundCab:
				OnGroundCabs.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_HangCab:
				HangCabs.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_OnCabCab:
				OnCabCabs.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_TallCab:
				TallCabs.Emplace(PlacedShape.Get());
				break;
			}
		}
		CabArrs.Emplace(OnGroundCabs);
		CabArrs.Emplace(HangCabs);
		CabArrs.Emplace(OnCabCabs);
		CabArrs.Emplace(TallCabs);
	}
	else if (InType == (int32)ECabinetClassify::EClassify_BathroomCab)
	{
		TArray<FShapeInRoom*> OnGroundCabs;
		TArray<FShapeInRoom*> HangCabs;
		TArray<FShapeInRoom*> StorageCabs;
		for (auto & PlacedShape : PlacedShapes)
		{
			ECabinetType CabType = (ECabinetType)PlacedShape->Shape->GetShapeCategory();
			switch (CabType)
			{
			case ECabinetType::EType_BathroomOnGroundCab:
				OnGroundCabs.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_BathroomHangCab:
				HangCabs.Emplace(PlacedShape.Get());
				break;
			case ECabinetType::EType_BathroomStorageCab:
				StorageCabs.Emplace(PlacedShape.Get());
				break;
			}
		}
		CabArrs.Emplace(OnGroundCabs);
		CabArrs.Emplace(HangCabs);
		CabArrs.Emplace(StorageCabs);
	}

	struct FStatCab
	{
		FStatElems StatElems;

		void operator() (FShapeInRoom *InShapeInRoom, const TArray<FObjectWeakPtr> & InRooms, FArmyWHCModeListData& OutWHCModeData) const
		{
			FArmyWHCListCabinetData ListCabinetData;

			TSharedPtr<FCabinetShape> CabShape = StaticCastSharedPtr<FCabinetShape>(InShapeInRoom->Shape);
			const TSharedPtr<FSpaceShape> &TopSpaceShape = CabShape->GetTopSpaceShape();
			FString SubstrateName, MtlName;
			int32 MtlId = 0;
			for (const auto &Change : InShapeInRoom->ChangeList)
			{
				if (Change->IsSameType(SHAPE_CHANGE_CABINET_MTL))
				{
					FArmyWHCCabinetMtlChange *CabMtlChange = static_cast<FArmyWHCCabinetMtlChange*>(Change.Get());
					MtlId = CabMtlChange->GetCabinetMtlId();
					break;
				}
			}

			TSharedPtr<FBoardShape> BoardShape = XRShapeUtilities::GetCabinetFrameBoard(CabShape.Get(), 2);
			if (MtlId > 0)
			{
				const TSharedPtr<FMtlInfo> *MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(MtlId);
				if (MtlInfoPtr)
				{
					const TSharedPtr<FMtlInfo> &MtlInfo = *MtlInfoPtr;
					MtlName = MtlInfo->MtlName;
				}
			}
			else if (BoardShape.IsValid())
			{
				MtlId = BoardShape->GetMaterialData().ID;
				MtlName = BoardShape->GetMaterialData().Name;
			}

			if (BoardShape.IsValid())
			{
				SubstrateName = BoardShape->GetSubstrateName();
			}

			ListCabinetData.Id = InShapeInRoom->Shape->GetShapeId();
			ListCabinetData.Name = InShapeInRoom->Shape->GetShapeName();
			ListCabinetData.Type = InShapeInRoom->Shape->GetShapeType();
			ListCabinetData.Category = InShapeInRoom->Shape->GetShapeCategory();
			ListCabinetData.Code = InShapeInRoom->Shape->GetShapeCode();
			ListCabinetData.ThumbnailUrl = InShapeInRoom->Shape->GetThumbnailUrl();
			ListCabinetData.SubstrateName = SubstrateName;
			ListCabinetData.MaterialId = MtlId;
			ListCabinetData.MaterialName = MtlName;
			ListCabinetData.Wdh = FString::Printf(TEXT("%d*%d*%d"),
				FMath::RoundToInt(InShapeInRoom->Shape->GetShapeWidth()),
				FMath::RoundToInt(InShapeInRoom->Shape->GetShapeDepth()),
				FMath::RoundToInt(InShapeInRoom->Shape->GetShapeHeight()));
			ListCabinetData.Quantity = 1;
			ListCabinetData.QuantityUnit = TEXT("个");

			TPair<TTuple<float, float, float>, TArray<float>> WidthInfo, DepthInfo, HeightInfo;
			if (FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(InShapeInRoom->Shape.Get(),
				WidthInfo, DepthInfo, HeightInfo))
				ListCabinetData.HollowWidth = WidthInfo.Key.Get<2>();
			else
				ListCabinetData.HollowWidth = 0.0f;

			int32 RoomIndex = 0;
			for (const auto & Object : InRooms)
			{
				if (RoomIndex == 21)
					RoomIndex = 24;
				else if (RoomIndex == 25)
					RoomIndex = 26;

				TSharedPtr<FArmyObject> ObjectPtr = Object.Pin();
				if (ObjectPtr->GetUniqueID() == InShapeInRoom->RoomAttachedIndex)
				{
					TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(ObjectPtr);
					ListCabinetData.SpaceName = Room->GetSpaceName();
					ListCabinetData.VrSpaceId = RoomIndex; // Room->GetSpaceId();
					break;
				}

				++RoomIndex;
			}

			// 统计门和抽屉
			StatElems(CabShape, CabShape->GetTopSpaceShape(), ListCabinetData.Cabinetelems);

			//统计抽屉五金
			StatDrawerAccess(CabShape, ListCabinetData.Cabinetelems);

			// 统计饰条
			StatInsertionBoards(CabShape, ListCabinetData.Cabinetelems);

			// 统计DoorItems
			StatDoorItems(CabShape, ListCabinetData.DoorItems);

			// 统计Label
			for (const auto &Label : InShapeInRoom->Labels)
			{
				if (Label == ECabinetLabelType::ELabelType_Basin)
					ListCabinetData.CabinetLabels.Emplace(3);
				else if (Label == ECabinetLabelType::ELabelType_Cooker)
					ListCabinetData.CabinetLabels.Emplace(4);
			}
			bool bOutSingleDoor = false, bOutDoubleDoor = false, bOutCurIsSingleDoor = false, bOutCurIsLeft = false;
			float OutDoorSheetWidth = 0.0f;
			if (FSCTShapeUtilityTool::GetFrontBoardCabinetDoorModifyInfo(InShapeInRoom->Shape.Get(), bOutSingleDoor, bOutDoubleDoor, OutDoorSheetWidth, bOutCurIsSingleDoor, bOutCurIsLeft))
			{
				ListCabinetData.CabinetLabels.Emplace(bOutCurIsLeft ? 1 : 2);
			}

			OutWHCModeData.Cabinets.Emplace(ListCabinetData);
		}

		void StatInsertionBoards(const TSharedPtr<FCabinetShape> & InCabShape, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			TArray<FInsertionBoard*> InsertionBoards;
			FSCTShapeUtilityTool::GetCabinetAllInsertionBoard(InCabShape.Get(), InsertionBoards);
			for (const auto & InsertionBoard : InsertionBoards)
			{
				FArmyWHCListCabinetElemData ListElemData;
				ListElemData.Id = InsertionBoard->GetShapeId();
				ListElemData.Name = InsertionBoard->GetShapeName();
				ListElemData.Type = InsertionBoard->GetShapeType();
				ListElemData.SingleType = 1; // 标识是一个门板
				ListElemData.Code = InsertionBoard->GetShapeCode();
				ListElemData.ThumbnailUrl = InsertionBoard->GetThumbnailUrl();
				ListElemData.SubstrateName = InsertionBoard->GetSubstrateName();
				const FInsertionBoard::FInsertBoardMaterial & MtlInfo = InsertionBoard->GetMaterial();
				ListElemData.MaterialId = MtlInfo.ID;
				ListElemData.MaterialName = MtlInfo.Name;
				ListElemData.Wdh = FString::Printf(TEXT("%d*%d*%d"), FMath::RoundToInt(InsertionBoard->GetShapeWidth()), FMath::RoundToInt(InsertionBoard->GetShapeDepth()), FMath::RoundToInt(InsertionBoard->GetShapeHeight()));
				ListElemData.Quantity = 1;
				ListElemData.QuantityStr = TEXT("1");
				ListElemData.QuantityUnit = TEXT("条");
				OutListData.Emplace(ListElemData);
			}
		}
		void StatDrawerAccess(const TSharedPtr<FSCTShape> & InSpaceShape, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			TArray<TArray<FAccessoryShape*>> OutSlidingDrawerAccess;
			FSCTShapeUtilityTool::GetCabinetSlideringDrawerAccess(InSpaceShape.Get(), OutSlidingDrawerAccess);

			TMap<int64, int32> IDIndexMap;
			for (auto& AccessoryArray : OutSlidingDrawerAccess)
			{
				for (auto Accessory : AccessoryArray)
				{
					int32 Index = IDIndexMap.Contains(Accessory->GetShapeId()) ? IDIndexMap.FindRef(Accessory->GetShapeId()) : INDEX_NONE;

					if (Index == INDEX_NONE)
					{
						FString WDH = FString::Printf(TEXT("%d*%d*%d"),
							FMath::RoundToInt(Accessory->GetShapeWidth()),
							FMath::RoundToInt(Accessory->GetShapeDepth()),
							FMath::RoundToInt(Accessory->GetShapeHeight()));

						FArmyWHCListCabinetElemData ListElemData;
						ListElemData.Id = Accessory->GetShapeId();
						ListElemData.Name = Accessory->GetShapeName();
						ListElemData.Type = Accessory->GetShapeType();
						ListElemData.SingleType = 0;
						ListElemData.Code = Accessory->GetShapeCode();
						ListElemData.ThumbnailUrl = Accessory->GetThumbnailUrl();
						ListElemData.BrandName = Accessory->GetBrandName();
						ListElemData.Version = Accessory->GetProductVersion();
						ListElemData.Wdh = WDH;
						ListElemData.Quantity = 1;
						ListElemData.QuantityStr = FString::FromInt(ListElemData.Quantity);
						ListElemData.QuantityUnit = TEXT("个");

						Index = OutListData.Emplace(ListElemData);
						IDIndexMap.Add(Accessory->GetShapeId(), Index);
					}
					else
					{
						FArmyWHCListCabinetElemData& ListElemData = OutListData[Index];
						ListElemData.Quantity++;
						ListElemData.QuantityStr = FString::FromInt(ListElemData.Quantity);
					}
				}
			}
		}
		void StatDoorItems(const TSharedPtr<FCabinetShape> & InCabShape, TArray<FArmyWHCListCabinetDoorItem> &OutListData) const
		{
			TArray<TPair<FSCTShape*, int32>> DoorItems;
			FSCTShapeUtilityTool::GetCabinetAllDoorSequenceNum(InCabShape.Get(), DoorItems);
			for (auto &DoorItem : DoorItems)
			{
				FArmyWHCListCabinetDoorItem ListDoorItem;
				FSCTShape *Shape = DoorItem.Key;
				ListDoorItem.Id = Shape->GetShapeId();

				if (Shape->GetShapeType() == ST_SideHungDoor)
				{
					FSideHungDoor* SideHungDoorShape = StaticCast<FSideHungDoor*>(Shape);
					if (SideHungDoorShape->GetDoorSheetNum() == 0)
						continue;
					const TSharedPtr<FVariableAreaDoorSheet> &DoorSheet = SideHungDoorShape->GetDoorSheets()[0];
					FVariableAreaDoorSheet::EDoorOpenDirection DoorDirection = DoorSheet->GetOpenDoorDirection();
					if (SideHungDoorShape->GetDoorSheetNum() == 1)
					{
						ListDoorItem.Type = DoorDirection == FVariableAreaDoorSheet::EDoorOpenDirection::E_Top ? 3 : 2;
						ListDoorItem.OpenDoorDirection = StaticCast<int32>(DoorDirection);
					}
					else if (SideHungDoorShape->GetDoorSheetNum() == 2)
					{
						ListDoorItem.Type = 1;
						ListDoorItem.OpenDoorDirection = 4; // 代表对开
					}
					ListDoorItem.MaterialId = DoorSheet->GetMaterial().ID;
					OutListData.Emplace(ListDoorItem);
				}
				else if (Shape->GetShapeType() == ST_DrawerDoor)
				{
					FDrawerDoorShape * DrawerDoorShape = StaticCast<FDrawerDoorShape*>(Shape);
					TSharedPtr<FVariableAreaDoorSheet> DoorSheet = DrawerDoorShape->GetDrawDoorSheet();
					if (!DoorSheet.IsValid())
						continue;
					ListDoorItem.Type = 4;
					ListDoorItem.MaterialId = DoorSheet->GetMaterial().ID;
					OutListData.Emplace(ListDoorItem);
				}
			}
		}
	};

	TArray<FObjectWeakPtr> Objects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, Objects);

	FStatCab StatCab;
	for (const auto & CabArr : CabArrs)
	{
		for (auto & Cab : CabArr)
		{
			if (Cab->HasValidShape())
				StatCab(Cab, Objects, OutWHCModeData);
		}
	}

	struct FStatAccessory
	{
		void operator() (const TSharedPtr<FShapeAccessory> &InAccessory, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			if (InAccessory.IsValid())
			{
				const FString &Name = InAccessory->CabAccInfo->Name;
				const FString &BrandName = InAccessory->CabAccInfo->BrandName;
				const FString &Version = InAccessory->CabAccInfo->Version;
				FString Wdh = FString::Printf(TEXT("%d*%d*%d"),
					FMath::RoundToInt(InAccessory->CabAccInfo->WidthInfo.Current),
					FMath::RoundToInt(InAccessory->CabAccInfo->DepthInfo.Current),
					FMath::RoundToInt(InAccessory->CabAccInfo->HeightInfo.Current)
				);
				FArmyWHCListCabinetElemData *ElemDataPtr = OutListData.FindByPredicate([Name, BrandName, Version, &Wdh](const FArmyWHCListCabinetElemData &Data) -> bool {
					return Name == Data.Name && BrandName == Data.BrandName && Version == Data.Version && Wdh == Data.Wdh;
				});
				if (ElemDataPtr)
				{
					++ElemDataPtr->Quantity;
					ElemDataPtr->QuantityStr = FString::Printf(TEXT("%d"), FMath::RoundToInt(ElemDataPtr->Quantity));
				}
				else
				{
					FArmyWHCListCabinetElemData Data;
					Data.Id = InAccessory->CabAccInfo->Id;
					Data.Name = Name;
					Data.Code = InAccessory->CabAccInfo->Code;
					Data.Type = (int32)InAccessory->CabAccInfo->Type;
					Data.ThumbnailUrl = InAccessory->CabAccInfo->ThumbnailUrl;
					Data.BrandName = BrandName;
					Data.Version = Version;
					Data.Wdh = Wdh;
					Data.Quantity = 1;
					Data.QuantityStr = TEXT("1");
					Data.QuantityUnit = TEXT("个");
					OutListData.Emplace(Data);
				}

				FStatAccessory StatAccessory;
				for (const auto &SubAcc : InAccessory->AttachedSubAccessories)
					StatAccessory(SubAcc, OutListData);
			}
		}
	};

	struct FStatStandaloneAccessory
	{
		void operator() (const TSharedPtr<FSCTShape> & InCabShape, TArray<FArmyWHCListCabinetElemData> &OutListData) const
		{
			TArray<FAccessoryShape*> EmbbedElecDevices;
			FSCTShapeUtilityTool::GetCabinetAllEmbedElectrical(InCabShape.Get(), EmbbedElecDevices);
			for (auto &EmbbedElecDev : EmbbedElecDevices)
			{
				const FString &Name = EmbbedElecDev->GetShapeName();
				const FString &BrandName = EmbbedElecDev->GetBrandName();
				const FString &Version = EmbbedElecDev->GetProductVersion();
				FString Wdh = FString::Printf(TEXT("%d*%d*%d"),
					FMath::RoundToInt(EmbbedElecDev->GetShapeWidth()),
					FMath::RoundToInt(EmbbedElecDev->GetShapeDepth()),
					FMath::RoundToInt(EmbbedElecDev->GetShapeHeight())
				);
				FArmyWHCListCabinetElemData *ElemDataPtr = OutListData.FindByPredicate([Name, BrandName, Version, &Wdh](const FArmyWHCListCabinetElemData &Data) -> bool {
					return Name == Data.Name && BrandName == Data.BrandName && Version == Data.Version && Wdh == Data.Wdh;
				});
				if (ElemDataPtr)
				{
					++ElemDataPtr->Quantity;
					ElemDataPtr->QuantityStr = FString::Printf(TEXT("%d"), FMath::RoundToInt(ElemDataPtr->Quantity));
				}
				else
				{
					FArmyWHCListCabinetElemData ElemData;
					ElemData.Id = EmbbedElecDev->GetShapeId();
					ElemData.Name = Name;
					ElemData.Type = EmbbedElecDev->GetShapeType();
					ElemData.Code = EmbbedElecDev->GetShapeCode();
					ElemData.ThumbnailUrl = EmbbedElecDev->GetThumbnailUrl();
					ElemData.BrandName = BrandName;
					ElemData.Version = Version;
					ElemData.Wdh = Wdh;
					ElemData.Quantity = 1;
					ElemData.QuantityStr = TEXT("1");
					ElemData.QuantityUnit = TEXT("个");
					OutListData.Emplace(ElemData);
				}
			}
		}
	};

	// 统计水槽、灶具等信息
	FStatAccessory StatAccessory;
	FStatStandaloneAccessory StatStandaloneAccessory;
	for (const auto & CabArr : CabArrs)
	{
		for (auto & Cab : CabArr)
		{
			if (Cab->HasValidAccessory())
			{
				if (Cab->CabinetActor.IsValid())
				{
					XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = Cab->CabinetActor;
					TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
					if (Accessory.IsValid()) {
						StatAccessory(Accessory, OutWHCModeData.Kitchens);
					}
				}
			}
			StatStandaloneAccessory(Cab->Shape, OutWHCModeData.Kitchens);
		}
	}

	// 临时统计台面信息
	FPlatformInfo * PlatformInfo = FArmyWHCabinetMgr::Get()->GetPlatformInfo();
	if (PlatformInfo != nullptr && PlatformInfo->PlatformMtl.IsValid())
	{
		OutWHCModeData.Worktop.MaterialId = PlatformInfo->PlatformMtl->Id;
		OutWHCModeData.Worktop.MaterialName = PlatformInfo->PlatformMtl->MtlName;
	}
}

void FArmyWHCModeCabinetOperation::AddOverlappedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape)
{
	if (!IsOverlappedPair(InActiveShape, InPassiveShape))
	{
		OverlappedMap.Emplace(InActiveShape, InPassiveShape);
	}
}

void FArmyWHCModeCabinetOperation::RemoveOverlappedPairByActive(FShapeInRoom *InActiveShape)
{
	OverlappedMap.Remove(InActiveShape);
}

void FArmyWHCModeCabinetOperation::RemoveOverlappedPair(FShapeInRoom *InShape)
{
	if (IsOverlappedPairByActive(InShape))
	{
		RemoveOverlappedPairByActive(InShape);
		return;
	}
	TMap<FShapeInRoom*, FShapeInRoom*>::TIterator Iter(OverlappedMap);
	for (; Iter; ++Iter)
	{
		if (Iter.Value() == InShape)
		{
			RemoveOverlappedPairByActive(Iter.Key());
			break;
		}
	}
}

bool FArmyWHCModeCabinetOperation::IsOverlappedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape) const
{
	FShapeInRoom *const *Passive = OverlappedMap.Find(InActiveShape);
	return Passive != nullptr ? (*Passive) == InPassiveShape : false;
}

bool FArmyWHCModeCabinetOperation::IsOverlappedPairByActive(FShapeInRoom *InActiveShape) const
{
	return OverlappedMap.Contains(InActiveShape);
}

bool FArmyWHCModeCabinetOperation::IsOverlappedPairByPassive(FShapeInRoom *InPassiveShape) const
{
	TMap<FShapeInRoom*, FShapeInRoom*>::TConstIterator Iter(OverlappedMap);
	for (; Iter; ++Iter)
	{
		if (Iter.Value() == InPassiveShape)
			return true;
	}
	return false;
}

bool FArmyWHCModeCabinetOperation::IsBelongedToAnyOverlappedPair(FShapeInRoom *InShape) const
{
	return IsOverlappedPairByActive(InShape) || IsOverlappedPairByPassive(InShape);
}

FShapeInRoom* FArmyWHCModeCabinetOperation::GetOverlappedPassive(FShapeInRoom *InActiveShape) const
{
	const FShapeInRoom *const *Passive = OverlappedMap.Find(InActiveShape);
	return Passive != nullptr ? const_cast<FShapeInRoom*>(*Passive) : nullptr;
}

void FArmyWHCModeCabinetOperation::AddAlignedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape)
{
	if (!IsAlignedPair(InActiveShape, InPassiveShape))
	{
		FAlignedShapeInfo AlignedInfo;
		AlignedInfo.PassiveShape = InPassiveShape;
		AlignedInfo.RelativeOffset = FVector::ZeroVector;
		AlignedMap.Emplace(InActiveShape, MoveTemp(AlignedInfo));
	}
}

void FArmyWHCModeCabinetOperation::RemoveAlignedPairByActive(FShapeInRoom *InActiveShape)
{
	AlignedMap.Remove(InActiveShape);
}

void FArmyWHCModeCabinetOperation::RemoveAlignedPair(FShapeInRoom *InShape)
{
	if (IsAlignedPairByActive(InShape))
	{
		RemoveAlignedPairByActive(InShape);
		return;
	}
	TMap<FShapeInRoom*, FAlignedShapeInfo>::TIterator Iter(AlignedMap);
	for (; Iter; ++Iter)
	{
		if (Iter.Value().PassiveShape == InShape)
		{
			RemoveAlignedPairByActive(Iter.Key());
			break;
		}
	}
}

bool FArmyWHCModeCabinetOperation::IsAlignedPair(FShapeInRoom *InActiveShape, FShapeInRoom *InPassiveShape) const
{
	const FAlignedShapeInfo *AlignedInfo = AlignedMap.Find(InActiveShape);
	return AlignedInfo != nullptr && AlignedInfo->PassiveShape == InPassiveShape;
}

bool FArmyWHCModeCabinetOperation::IsAlignedPairByActive(FShapeInRoom *InActiveShape) const
{
	return AlignedMap.Contains(InActiveShape);
}

bool FArmyWHCModeCabinetOperation::IsAlignedPairByPassive(FShapeInRoom *InPassiveShape) const
{
	TMap<FShapeInRoom*, FAlignedShapeInfo>::TConstIterator Iter(AlignedMap);
	for (; Iter; ++Iter)
	{
		if (Iter.Value().PassiveShape == InPassiveShape)
			return true;
	}
	return false;
}

bool FArmyWHCModeCabinetOperation::IsBelongedToAnyAlignedPair(FShapeInRoom *InShape) const
{
	return IsAlignedPairByActive(InShape) || IsAlignedPairByPassive(InShape);
}

FShapeInRoom* FArmyWHCModeCabinetOperation::GetAlignedPassive(FShapeInRoom *InActiveShape) const
{
	const FAlignedShapeInfo *AlignedInfo = AlignedMap.Find(InActiveShape);
	return AlignedInfo != nullptr ? AlignedInfo->PassiveShape : nullptr;
}

FShapeInRoom* FArmyWHCModeCabinetOperation::GetAlignedActiveShapeByPassive(FShapeInRoom *InPassiveShape) const
{
	TMap<FShapeInRoom*, FAlignedShapeInfo>::TConstIterator Iter(AlignedMap);
	for (; Iter; ++Iter)
	{
		if (Iter.Value().PassiveShape == InPassiveShape)
			return const_cast<FShapeInRoom*>(Iter.Key());
	}
	return nullptr;
}

const FArmyWHCModeCabinetOperation::FAlignedShapeInfo* FArmyWHCModeCabinetOperation::GetAlignedShapeInfoByPassive(FShapeInRoom *InPassiveShape) const
{
	TMap<FShapeInRoom*, FAlignedShapeInfo>::TConstIterator Iter(AlignedMap);
	for (; Iter; ++Iter)
	{
		if (Iter.Value().PassiveShape == InPassiveShape)
			return &Iter.Value();
	}
	return nullptr;
}

FShapeInRoom* FArmyWHCModeCabinetOperation::CheckOverlapped(FShapeInRoom *InActiveShape, const TArray<TSharedPtr<FShapeInRoom>> &InPlacedShapes, const FVector *InDesiredPosition /* = nullptr */)
{
	if (InActiveShape->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
	{
		for (const auto &PlacedShape : InPlacedShapes)
		{
			if (InActiveShape != PlacedShape.Get())
			{
				if (PlacedShape->CabinetActor.IsValid())
				{
					XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PlacedShape->CabinetActor;
					TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
					if (PlacedShape->Labels.Contains(ECabinetLabelType::ELabelType_Cooker) &&
						Accessory.IsValid() &&
						Accessory->Actor != nullptr)
					{
						FArmyWHCRect MyRect = InActiveShape->GetRect(InDesiredPosition != nullptr ? const_cast<FVector*>(InDesiredPosition) : nullptr);
						MyRect.mMinZ = MyRect.mMaxZ = 0.0f;

						FArmyWHCRect OtherRect = PlacedShape->GetRect();
						FVector RelativeLocation = Accessory->Actor->GetRootComponent()->GetRelativeTransform().GetLocation();
						OtherRect.mPos = OtherRect.mPos + OtherRect.mDir * RelativeLocation.X;
						OtherRect.mPosOrgin = OtherRect.mPosOrgin + OtherRect.mDir * RelativeLocation.X;
						OtherRect.mMinZ = OtherRect.mMaxZ = 0.0f;

						if (MyRect.Collision(&OtherRect))
						{
							// 移动的柜子和遍历到的柜子不在一个覆盖组内
							if (IsOverlappedPairByPassive(PlacedShape.Get()) && (!IsOverlappedPair(InActiveShape, PlacedShape.Get())))
								continue;
							// 移动的柜子和遍历到的柜子不在一个对齐组内
							if (IsBelongedToAnyAlignedPair(PlacedShape.Get()) && (!IsAlignedPair(InActiveShape, PlacedShape.Get())))
								continue;
							RemoveAlignedPairByActive(InActiveShape);
							AddOverlappedPair(InActiveShape, PlacedShape.Get());
							return PlacedShape.Get();
						}
					}
				}
			}
			// 当前移动的柜子未和任何柜子产生碰撞，则移除当前柜子的覆盖组
			RemoveOverlappedPairByActive(InActiveShape);
		}
	}
	return nullptr;
}
FShapeInRoom* FArmyWHCModeCabinetOperation::CheckAligned(FShapeInRoom *InActiveShape, const TArray<TSharedPtr<FShapeInRoom>> &InPlacedShapes, float Tolerance, const FVector *InDesiredPosition /* = nullptr */)
{
	if (InActiveShape->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
	{
		for (const auto &PlacedShape : InPlacedShapes)
		{
			if (InActiveShape != PlacedShape.Get())
			{
				XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PlacedShape->CabinetActor;
				TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
				if (PlacedShape->Labels.Contains(ECabinetLabelType::ELabelType_Cooker) &&
					Accessory.IsValid() &&
					Accessory->Actor != nullptr)
				{
					FArmyWHCRect MyRect = InActiveShape->GetRect(InDesiredPosition != nullptr ? const_cast<FVector*>(InDesiredPosition) : nullptr);

					FArmyWHCRect OtherRect = PlacedShape->GetRect();
					FVector RelativeLocation = Accessory->Actor->GetRootComponent()->GetRelativeTransform().GetLocation();
					OtherRect.mPos = OtherRect.mPos + OtherRect.mDir * RelativeLocation.X;
					OtherRect.mPosOrgin = OtherRect.mPosOrgin + OtherRect.mDir * RelativeLocation.X;

					// 如果两个柜子是一个覆盖组，才会进行对齐检测
					if (IsOverlappedPair(InActiveShape, PlacedShape.Get()) &&
						(FMath::Abs(MyRect.mPos.X - OtherRect.mPos.X) < Tolerance || FMath::Abs(MyRect.mPos.Y - OtherRect.mPos.Y) < Tolerance))
					{
						RemoveOverlappedPairByActive(InActiveShape);
						AddAlignedPair(InActiveShape, PlacedShape.Get());
						return PlacedShape.Get();
					}
				}
			}
		}
	}
	return nullptr;
}

void FArmyWHCModeCabinetOperation::Callback_LeftRulerLengthChanged(const FString &InStr)
{
	float NewLength = FCString::Atof(*InStr) * 0.1f;
	MoveShapeLeft(LeftDistanceRuler->GetRulerLength() - NewLength);
}

void FArmyWHCModeCabinetOperation::Callback_RightRulerLengthChanged(const FString &InStr)
{
	float NewLength = FCString::Atof(*InStr) * 0.1f;
	MoveShapeRight(RightDistanceRuler->GetRulerLength() - NewLength);
}

void FArmyWHCModeCabinetOperation::Callback_BackRulerLengthChanged(const FString &InStr)
{
	float NewLength = FCString::Atof(*InStr) * 0.1f;
	MoveShapeBack(NewLength - BackDistanceRuler->GetRulerLength());
}

void FArmyWHCModeCabinetOperation::Callback_RoofRulerLengthChanged(const FString &InStr)
{
	if (AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected())
	{
		float NewLength = FCString::Atof(*InStr) * 0.1f;
		float Delta = NewLength - RoofDistanceRuler->GetRulerLength();
		FVector OldLocation = ShapeFrame->GetActorLocation();
		OldLocation.Z -= Delta;
		ShapeFrame->ShapeInRoomRef->AboveGround -= Delta;
		ShapeFrame->SetActorLocation(OldLocation);
		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal(true);
		gFArmyWHCabinMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::Callback_FloorRulerLengthChanged(const FString &InStr)
{
	if (AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected())
	{
		float NewLength = FCString::Atof(*InStr);
		gFArmyWHCabinMgr->OnUIAboveGroundChangeSingle(NewLength, ShapeFrame->ShapeInRoomRef);
		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal(true);
		gFArmyWHCabinMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::Callback_LeftObjRulerLengthChanged(const FString &InStr)
{
	float NewLength = FCString::Atof(*InStr) * 0.1f;
	MoveShapeLeft(LeftObjDistanceRuler->GetRulerLength() - NewLength);
}

void FArmyWHCModeCabinetOperation::Callback_RightObjRulerLengthChanged(const FString &InStr)
{
	float NewLength = FCString::Atof(*InStr) * 0.1f;
	MoveShapeRight(RightObjDistanceRuler->GetRulerLength() - NewLength);
}

void FArmyWHCModeCabinetOperation::Callback_BackObjRulerLengthChanged(const FString &InStr)
{
	float NewLength = FCString::Atof(*InStr) * 0.1f;
	MoveShapeBack(NewLength - BackObjDistanceRuler->GetRulerLength());
}

void FArmyWHCModeCabinetOperation::Callback_RoofObjRulerLengthChanged(const FString &InStr)
{
	if (AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected())
	{
		float NewLength = FCString::Atof(*InStr) * 0.1f;
		float Delta = NewLength - RoofObjDistanceRuler->GetRulerLength();
		FVector OldLocation = ShapeFrame->GetActorLocation();
		OldLocation.Z -= Delta;
		ShapeFrame->SetActorLocation(OldLocation);
		ShapeFrame->ShapeInRoomRef->AboveGround -= Delta;
		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal(true);
		gFArmyWHCabinMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::Callback_FloorObjRulerLengthChanged(const FString &InStr)
{
	if (AXRShapeFrame * ShapeFrame = GetShapeFrameFromSelected())
	{
		float NewLength = FCString::Atof(*InStr) * 0.1f;
		float Delta = NewLength - FloorObjDistanceRuler->GetRulerLength();
		FVector OldLocation = ShapeFrame->GetActorLocation();
		OldLocation.Z += Delta;
		ShapeFrame->SetActorLocation(OldLocation);
		ShapeFrame->ShapeInRoomRef->AboveGround += Delta;
		// 独立台面
		ShapeFrame->ShapeInRoomRef->SpawnPlatform();

		CalculateShapeDistanceFromWall(ShapeFrame->ShapeInRoomRef);
		ShowShapeDistanceRulerInternal(true);
		gFArmyWHCabinMgr->OnPalceShapeEnd(ShapeFrame->ShapeInRoomRef);
		TArray<FFrontBoardCabInfo> ChangedFrontBoardCabs = FrontBoardCabModifier.MoveCabinet();
		for (const auto &ChangedFrontBoardCab : ChangedFrontBoardCabs)
			MakeFrontBoardCabChangeItem(ChangedFrontBoardCab.FrontBoardCab, ChangedFrontBoardCab.NumDoors, ChangedFrontBoardCab.StartPoint, ChangedFrontBoardCab.EndPoint);
		UpdateHighlight();
	}
}

void FArmyWHCModeCabinetOperation::UpdateRuler(FShapeRuler *InRuler, float InLength, const FVector &InRulerLocation)
{
	if ((RulerVisibilityFlag & ERulerVF_Env) != 0)
	{
		if (FMath::IsNearlyZero(InLength, 0.1f))
		{
			InRuler->SetRulerLength(0.0f);
			InRuler->ShowRuler(false);
			return;
		}

		InRuler->SetRulerLength(InLength);
		InRuler->SetRulerLocation(InRulerLocation);
		if (!InRuler->IsRulerVisible())
			InRuler->ShowRuler(true);
	}
}

//////////////////////////////////////////////////////////////////////////
bool FArmyWHCModeCabinetOperation::PlaceShapeRect(FShapeInRoom *InShape, const FVector2D &InMousePosition)
{
	constexpr float AdhereThreshold = 20.0f;

	FVector WorldPoint, WorldDir;
	GVC->DeprojectFVector2D(InMousePosition, WorldPoint, WorldDir);

	int32 RoomIndex = -1;
	FVector ShapePosition;
	// 检查摆放点是否在某个房间内
	FArmyWHCPoly* polyRoom = gCabinMgr.CollisionRoom(WorldPoint, WorldDir, 0.0f, RoomIndex, ShapePosition);
	if (polyRoom == nullptr)
		return false;

	ShapePosition.Z = InShape->AboveGround;

	if (InShape->ShapeFrame == nullptr)
	{
		InShape->RoomAttachedIndex = polyRoom->mGuid;
		InShape->Spawn();
		gFArmyWHCabinMgr->OnShapeInited(InShape);

		GGI->Window->HideMessage();
	}

	if (InShape->RoomAttachedIndex != polyRoom->mGuid)
	{
		InShape->RoomAttachedIndex = polyRoom->mGuid;
		InShape->WallAttachedIndex = -1;
	}

	TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
	for (auto &Command : MoveCommandList)
	{
		Command->ProcessCommand(polyRoom, ShapePosition, InShape, PlacedShapes);
	}

	return true;
}