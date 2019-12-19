#include "ArmyWHCabinet.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyWHCCookAssist.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyShapeTableActor.h"
#include "ArmyShapeRuler.h"
#include "ArmyWHCShapeChangeList.h"
#include "common/XRMathFunc.h"
#include "common/XRPolygonAssist.h"
#include "common/XRShapeUtilities.h"
#include "common/XRSpline.h"

#include "SCTResManager.h"
#include "MaterialManager.h"
#include "SCTCabinetShape.h"
#include "SCTSpaceShape.h"
#include "SCTFrameShape.h"
#include "SCTDoorGroup.h"
#include "SCTSlidingDrawerShape.h"
#include "SCTEmbededElectricalGroup.h"
#include "SCTAnimation.h"
#include "SCTShapeManager.h"
#include "Actor/SCTShapeActor.h"
#include "Actor/VertexType.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

// #include "StringConv.h"
#include "ArmyMath.h"
#include "ArmyActorConstant.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"
#include "ArmyWallActor.h"
#include "Model/XRHomeData.h"
#include "Data/XRFurniture.h"

#include "Engine/StaticMeshActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionQueryParams.h"
// #include "ArmyWHCShapeDoorChangeList.h"

FShapeAccessory::~FShapeAccessory()
{
	AttachedSubAccessories.Empty();

	if (Actor != nullptr)
	{
		Actor->Destroy();
		Actor = nullptr;
	}

	if (Component.IsValid())
	{
		FArmySceneData::Get()->Delete(Component);
		Component.Reset();
	}
}

bool FShapeAccessory::SpawnActor(AActor *InParent)
{
	FSCTVRSObject *Object = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().LoadCustomObj(
		CabAccInfo->GetCachePath(), CabAccInfo->OptimizedParam);
	if (Object != nullptr)
	{
		Actor = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AXRElecDeviceActor>(AXRElecDeviceActor::StaticClass());
		check(Actor != nullptr);
		Actor->ShapeAccRef = this;
		Actor->SetType((EMetalsType)CabAccInfo->Type);
		Actor->Tags.Emplace(XRActorTag::Immovable);
		Actor->Tags.Emplace(XRActorTag::WHCActor);
		UStaticMeshComponent *SMComp = Actor->GetStaticMeshComponent();
		SMComp->SetMobility(EComponentMobility::Movable);
		UStaticMesh *StaticMesh = Object->GetLampMeshObject();
		SMComp->SetStaticMesh(StaticMesh);
		for (const auto &IterRef: Object->MaterialList)
			SMComp->SetMaterialByName(FName(*IterRef.SlotName), IterRef.DIM);
		Actor->AttachToActor(InParent, FAttachmentTransformRules::KeepRelativeTransform);
		Actor->SetActorRelativeLocation(CabAccInfo->RelativeLocation);
		Actor->SetActorScale3D((FVector(CabAccInfo->WidthInfo.Current, CabAccInfo->DepthInfo.Current, CabAccInfo->HeightInfo.Current) / 10.0f) / StaticMesh->GetBoundingBox().GetSize());
		return true;
	}
	else
		return false;
}

FSCTVRSObject* FShapeAccessory::GetResourceObj()
{
	return FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().LoadCustomObj(CabAccInfo->GetCachePath(), CabAccInfo->OptimizedParam);
}

void FShapeAccessory::UpdateComponent()
{
	if (Actor != nullptr && Component.IsValid())
	{
		Component->SetTransform(
			FTransform(Actor->GetActorRotation(), Actor->GetActorLocation(), FVector::OneVector)
		);

		for (auto & SubAcc : AttachedSubAccessories)
			SubAcc->UpdateComponent();
	}
}

FShapeInRoom::FShapeInRoom()
{
	mId = -1;
}

FShapeInRoom::~FShapeInRoom()
{
	// 删除构件
	DestroyShape();
	// 删除附属件
	DestroyAccessories();
	// 删除调整脚
	DestroyShapeLegs();
	// 删除关联台面
	DestroyPlatform();
	// 删除宽度标尺
	DestroyRuler();
	// 删除碰撞盒
	if (ShapeFrame != nullptr)
	{
		ShapeFrame->Destroy();
		ShapeFrame = nullptr;
	}
	if (CabinetActor.IsValid())
	{
		CabinetActor->Remove();
		CabinetActor = nullptr;
	}
}

void FShapeInRoom::Initialize(FCabinetInfo *InCabInfo, FCabinetWholeComponent *InCabWholeComponent)
{
	Labels = InCabInfo->Labels;
	// @zengy 特殊判断是否使用全局离地高度
	if (InCabInfo->Type == (int32)ECabinetType::EType_HangCab || 
		InCabInfo->Type == (int32)ECabinetType::EType_OnGroundCab || 
		InCabInfo->Type == (int32)ECabinetType::EType_TallCab ||
		InCabInfo->Type == (int32)ECabinetType::EType_OnCabCab)
		AboveGround = gCabinMgr.GetAboveGround(InCabInfo->Type); 
	else 
		AboveGround = InCabInfo->AboveGround * 0.1f;
	// 初始化櫃子組建
	CabinetActor = MakeShareable(new XRCabinetActor());
	if (CabinetActor.IsValid()) {
		CabinetActor->CreateTransform();
		//CabinetActor->InitCabinetComponents(this);
	}
	// 默认关联台面
	InitializePlatform(InCabInfo->PlatformInfo);

	// 柜子构件
	InitializeSelf(InCabWholeComponent->Cabinet.Get());
	// 柜子附属件构件
	if (InCabInfo->AccInfo.IsValid())
		InitializeAccessories(InCabInfo->AccInfo.Get(), InCabWholeComponent->Accessorys);

}

void FShapeInRoom::InitializeParams(FCabinetInfo *InCabInfo)
{
	if (InCabInfo->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
    {
    	// 特殊逻辑，烟机柜的尺寸要保存到Shape的参数中
        AddOrSetParam(PARAM_NAME_CAB_WIDTH_INTERVALS, InCabInfo->WidthInfo.Intervals);
        AddOrSetParam(PARAM_NAME_CAB_DEPTH_INTERVALS, InCabInfo->DepthInfo.Intervals);
        AddOrSetParam(PARAM_NAME_CAB_HEIGHT_INTERVALS, InCabInfo->HeightInfo.Intervals);
        FDimInfo *VentilatorWidthInfo = InCabInfo->OtherDimInfo.Find("VentilatorWidth");
        if (VentilatorWidthInfo != nullptr)
        {
            AddOrSetParam(PARAM_NAME_CAB_MAX_VENTILATOR_WIDTH, VentilatorWidthInfo->Max);
            AddOrSetParam(PARAM_NAME_CAB_MIN_VENTILATOR_WIDTH, VentilatorWidthInfo->Min);
            AddOrSetParam(PARAM_NAME_CAB_VENTILATOR_WIDTH_INTERVALS, VentilatorWidthInfo->Intervals);
        }
    }
}


void FShapeInRoom::InitializeSelf(FCabinetComponent *InCabComp)
{
	//// 初始化柜子构件
	//if (InCabComp != nullptr)
	//	MakeComponent(ShapeComponent, InCabComp->GetCachePath(1), OT_ComponentBase, InCabComp->Type);
	if (CabinetActor.IsValid())
	{
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetFurnitureComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_FArmyFURNITURE, this);
		}
		XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = CabinetActor;
		FurnitureComponent->InitializeObject(InCabComp);
	}

}

void FShapeInRoom::InitializeAccessories(FCabinetWholeAccInfo *InCabAccInfo, const TArray<TSharedPtr<FCabinetComponent>>& InAccessories)
{
	//// 初始化附属件
	//check(InCabAccInfo != nullptr);

	//// 将附属件信息复制一份，放到FShapeAccessory中
	//// 原因：附属件可能会关联子部件，附属件和子部件都可以独立更换，更换完毕后要同步CabinetAccInfo的信息
	//// 而且每个柜子实例是否替换附属件都是不确定的。如果直接将指针传递过来，当有些柜子修改了附属件，再添加
	//// 新柜子的时候就会生成替换后的附属件。要保证原始柜子信息中的附属件信息是不变的，所以柜子实例会复制一份
	//// 附属件信息
	//Accessory = MakeShareable(MakeShapeAccessory(InCabAccInfo->Self.Get()));
	//check(Accessory.IsValid());
	//UpdateAccessoryLocation();

	//for (const auto & SubAccInfo : InCabAccInfo->SubAccInfos)
	//	Accessory->AttachedSubAccessories.Emplace(MakeShareable(MakeShapeAccessory(SubAccInfo.Get())));

	//if (Accessory->AttachedSubAccessories.Num() > 0)
	//{
	//	if (Labels.Contains(ECabinetLabelType::ELabelType_Basin) || 
	//		Labels.Contains(ECabinetLabelType::ELabelType_BathroomBasin)) // 水槽
	//	{
	//		Accessory->AttachedSubAccessories[0]->CabAccInfo->bBoundToSocket = true;
	//	}
	//	else if (Labels.Contains(ECabinetLabelType::ELabelType_Cooker) || 
	//		Labels.Contains(ECabinetLabelType::ELabelType_Ventilator)) // 灶具和油烟机
	//	{
	//		Accessory->AttachedSubAccessories[0]->CabAccInfo->bBoundToSocket = false;
	//	}
	//}

	//// 初始化构件
	//for (auto Component : InAccessories)
	//{
	//	if (InCabAccInfo->Self->Id == Component->Id && Component->Type == 105)
	//	{
	//		MakeComponent(Accessory->Component, Component->GetCachePath(1), OT_ComponentBase, 105);
	//		break;
	//	}
	//}
	if (CabinetActor.IsValid()) {
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetAccessoryComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_ACCESSORY, this);
		}
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		AccessoryComponent->InitializeObject(InCabAccInfo, InAccessories);
	}
}

void FShapeInRoom::InitializePlatform(const TSharedPtr<FPlatformInfo> &InPlatformInfo)
{
	if (InPlatformInfo.IsValid())
	{
		if (CabinetActor.IsValid())
		{
			if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetPlatformInfoComponent>()))
			{
				auto ICabinetComponent = CabinetActor->GenCabinetComponents(
					ECC_TYPE::CCT_PLATFORM, this);
			}
			XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = CabinetActor;
			PlatformInfoComponent->SpawnObject(InPlatformInfo);
		}
	}
}

FShapeAccessory* FShapeInRoom::AddSubAccessory(FCabinetAccInfo *InAccInfo, EMetalsType InMetalType, bool bBoundToSocket)
{
	//check(Accessory.IsValid());
	//check(InAccInfo != nullptr);

	//FShapeAccessory * NewSubAccessory = MakeShapeAccessory(InAccInfo);
	//NewSubAccessory->CabAccInfo->Type = (uint8)InMetalType;
	//NewSubAccessory->CabAccInfo->bBoundToSocket = bBoundToSocket;
	//Accessory->AttachedSubAccessories.Emplace(MakeShareable(NewSubAccessory));
	//// TODO: 临时增加的对于烟机构件的逻辑
	//if (InMetalType == EMetalsType::MT_FLUE_GAS_TURBINE)
	//{
	//	FString DefaultComponent = FPaths::ProjectContentDir() / TEXT("WHC/ventilator.dxf");
	//	if (InAccInfo->ComponentArray.Num() > 0)
	//		DefaultComponent = InAccInfo->ComponentArray[0]->GetCachePath(1);
	//	MakeComponent(NewSubAccessory->Component, DefaultComponent, OT_ComponentBase, 105);
	//}
	//return NewSubAccessory;

	return nullptr;
}

void FShapeInRoom::AddAndSpawnSubAccessory(FCabinetAccInfo *InAccInfo, 
	EMetalsType InMetalType, bool bBoundToSocket, FVector& RelLocation){
	if (CabinetActor.IsValid()) {
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		//auto ICabinetComponent = CabinetActor->GetCabinetPartsComponents(ECC_TYPE::CCT_ACCESSORY);
		 {
			FShapeAccessory* SubAccessory = AccessoryComponent->AddSubAccessory(InAccInfo, 
				(EMetalsType)InAccInfo->Type, bBoundToSocket);
			if (nullptr != SubAccessory)
				AccessoryComponent->SpawnSubAccessory(SubAccessory, RelLocation);
		}
	}
}

FShapeAccessory* FShapeInRoom::ReplaceSubAccessory(FCabinetAccInfo *InOldAccInfo, FCabinetAccInfo *InNewAccInfo, EMetalsType InMetalType, bool bBoundToSocket){
	/*check(Accessory.IsValid());
	check(InOldAccInfo != nullptr);
	check(InNewAccInfo != nullptr);

	int32 Id = InOldAccInfo->Id;
	int32 Index = Accessory->AttachedSubAccessories.IndexOfByPredicate(
		[Id](const TSharedPtr<FShapeAccessory> &SubAccessory) { return SubAccessory->CabAccInfo->Id == Id; }
	);
	check(Index != INDEX_NONE);

	FShapeAccessory * NewSubAccessory = MakeShapeAccessory(InNewAccInfo);
	NewSubAccessory->CabAccInfo->Type = (uint8)InMetalType;
	NewSubAccessory->CabAccInfo->bBoundToSocket = bBoundToSocket;
	Accessory->AttachedSubAccessories[Index] = MakeShareable(NewSubAccessory);
	return NewSubAccessory;*/

	return nullptr;
}

void FShapeInRoom::ReplaceAndSpawnSubAccessory(FCabinetAccInfo *InOldAccInfo, 
	FCabinetAccInfo *InNewAccInfo, EMetalsType InMetalType, bool bBoundToSocket, FVector& RelLocation) {
	if (CabinetActor.IsValid()) {
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		 {
			FShapeAccessory* SubAccessory = AccessoryComponent->ReplaceSubAccessory(InOldAccInfo,
				InNewAccInfo, InMetalType,  bBoundToSocket);
			if(nullptr != SubAccessory)
				AccessoryComponent->SpawnSubAccessory(SubAccessory, RelLocation);
		}
	}
}

void FShapeInRoom::DeleteSubAccessory(int32 InIndex)
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		check(Accessory.IsValid());
		check(InIndex >= 0 && InIndex < Accessory->AttachedSubAccessories.Num());
		Accessory->AttachedSubAccessories.RemoveAt(InIndex);
	}
}

void FShapeInRoom::UpdateAccessoryLocation()
{
	/*check(Accessory.IsValid());
	if (Labels.Contains(ECabinetLabelType::ELabelType_Basin) || Labels.Contains(ECabinetLabelType::ELabelType_Cooker))
	{
		Accessory->CabAccInfo->RelativeLocation.Set(0.0f, Shape->GetShapeDepth() * 0.05f, 
            Shape->GetShapeHeight() * 0.1f + FArmyWHCabinetMgr::Get()->GetTableHeight(true));
	}
	else if (Labels.Contains(ECabinetLabelType::ELabelType_BathroomBasin))	
	{
		Accessory->CabAccInfo->RelativeLocation.Set(0.0f, Shape->GetShapeDepth() * 0.05f, 
            Shape->GetShapeHeight() * 0.1f);
	}
	else if (Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
	{
		Accessory->CabAccInfo->RelativeLocation.Set(0.0f, 0.0f, 0.0f);
	}

	if (Accessory->Actor != nullptr)
	{
		Accessory->Actor->SetActorRelativeLocation(Accessory->CabAccInfo->RelativeLocation);
		Accessory->UpdateComponent();
	}*/
}

void FShapeInRoom::Spawn()
{
	SpawnShapeFrame();
	SpawnAccessories();
	SpawnShapeLegs();
	SpawnShape();
	SpawnShapeWidthRuler();
	
}

void FShapeInRoom::SpawnShapeFrame()
{
	ShapeFrame = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AXRShapeFrame>(AXRShapeFrame::StaticClass(), 
		FVector::ZeroVector,
		FRotator::ZeroRotator);
	ShapeFrame->Tags.Emplace(XRActorTag::WHCActor);
    ShapeFrame->SetFolderPath(FArmyActorPath::GetWHCCabinetPath());
	float ShapeWidth = Shape->GetShapeWidth() * 0.1f;
	float ShapeDepth = Shape->GetShapeDepth() * 0.1f;
	float ShapeHeight = Shape->GetShapeHeight() * 0.1f;
	ShapeFrame->ShapeInRoomRef = this;
	ShapeFrame->RefreshFrame(ShapeWidth, ShapeDepth, ShapeHeight);
}

void FShapeInRoom::SpawnShapeWidthRuler()
{
	/*float ShapeWidth = Shape->GetShapeWidth() * 0.1f;
	float ShapeHeight = Shape->GetShapeHeight() * 0.1f;

	ShapeWidthRuler = MakeShareable(new FShapeRuler(XRWHCMode::ERulerAlignDirection::RAD_X, GVC->ViewportOverlayWidget));
	ShapeWidthRuler->SetRulerAlignment(XRWHCMode::ERulerAlignment::RA_Bottom);
	ShapeWidthRuler->SetRulerLength(ShapeWidth);
	ShapeWidthRuler->SetRulerValueIsReadOnly(true);
	AShapeRulerActor *RulerActor = ShapeWidthRuler->GetRulerActor();
	RulerActor->SetActorRelativeLocation(FVector(-ShapeWidth * 0.5f, 0.0f, ShapeHeight));
	RulerActor->AttachToActor(ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RefreshShapeWidthRuler();*/

	if (CabinetActor.IsValid())
	{
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetRulerComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_RULER, this);
		}

		XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = CabinetActor;
		RulerComponent->SpawnShapeWidthRuler();
	}
}

void FShapeInRoom::SpawnAccessories()
{
	//if (!Accessory.IsValid())
	//	return;
	//
	//USCTXRResourceManager & ResMgr = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager();
	//UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	//bool bRet = Accessory->SpawnActor(ShapeFrame);
	//if (bRet)
	//{
	//	FSCTVRSObject *Object = Accessory->GetResourceObj();
	//	int32 IterCount = Accessory->AttachedSubAccessories.Num();
	//	for (int32 i = 0; i < IterCount; ++i)
	//	{
	//		if (Accessory->AttachedSubAccessories[i]->CabAccInfo->bBoundToSocket)
	//		{
	//			// 为了防止Socket有数组越界，所以使用了两者较小的值当作Socket的索引
	//			int32 SocketIndex = FMath::Min(i, Object->SocketList.Num() - 1);
	//			if (SocketIndex < 0)
	//				continue;
	//			Accessory->AttachedSubAccessories[i]->CabAccInfo->RelativeLocation = Object->SocketList[SocketIndex].Location;
	//			Accessory->AttachedSubAccessories[i]->SpawnActor(Accessory->Actor);
	//		}
	//		else
	//			Accessory->AttachedSubAccessories[i]->SpawnActor(Accessory->Actor);
	//	}

	//	// 将构件添加到HomeData中
	//	if (Accessory->Component.IsValid())
	//	{
	//		FArmySceneData::Get()->Add(Accessory->Component, XRArgument(1).ArgUint32(E_WHCModel));
	//		for (const auto & SubAccessory : Accessory->AttachedSubAccessories)
	//		{
	//			if (SubAccessory->Component.IsValid())
	//				FArmySceneData::Get()->Add(SubAccessory->Component, XRArgument(1).ArgUint32(E_WHCModel));
	//		}
	//	}
	//}
	if (CabinetActor.IsValid())
	{
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetAccessoryComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_ACCESSORY, this);
		}
		
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		AccessoryComponent->SpawnAccessories();
	}
}

void FShapeInRoom::SpawnSubAccessory(FShapeAccessory *InAccessory, const FVector &InRelativeLocation)
{
	/*if (Accessory.IsValid() && Accessory->Actor != nullptr)
	{
		int32 Index = Accessory->AttachedSubAccessories.IndexOfByPredicate(
			[InAccessory](const TSharedPtr<FShapeAccessory> &Accessory) { return InAccessory == Accessory.Get(); }
		);
		if (Index != INDEX_NONE)
		{
			bool bCanSpawn = true;
			FVector RelLocation;
			if (InAccessory->CabAccInfo->bBoundToSocket)
			{
				FSCTVRSObject *ResObj = Accessory->GetResourceObj();
				bCanSpawn = ResObj->SocketList.Num() > Index;
				if (bCanSpawn)
					RelLocation = ResObj->SocketList[Index].Location;
			}
			else
				RelLocation = InRelativeLocation;

			if (bCanSpawn)
			{
				InAccessory->CabAccInfo->RelativeLocation = RelLocation;	
				Accessory->AttachedSubAccessories[Index]->SpawnActor(Accessory->Actor);

				if (InAccessory->Component.IsValid())
					FArmySceneData::Get()->Add(InAccessory->Component, XRArgument(1).ArgUint32(E_WHCModel));
			}
		}
	}*/
}

void FShapeInRoom::SpawnShapeLegs()
{
	//int32 SynID = FArmyWHCabinetMgr::Get()->GetMesh(TEXT("WHC/MATERIAL_1542592442974_3994.pak"));
	//int32 ShapeCategory = Shape->GetShapeCategory();
	//// 只有橱柜地柜和橱柜高柜会生成调整脚
	//if (SynID != -1 && (ShapeCategory == (int32)ECabinetType::EType_OnGroundCab || ShapeCategory == (int32)ECabinetType::EType_TallCab))
	//{
	//	USCTXRResourceManager & ResMgr = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager();
	//	FSCTVRSObject *Object = ResMgr.GetObjFromObjID(SynID);
	//	if (Object != nullptr)
	//	{
	//		float ShapeWidth = Shape->GetShapeWidth() * 0.1f;
	//		float ShapeDepth = Shape->GetShapeDepth() * 0.1f;
	//		
	//		TArray<FVector> Outline {
	//			FVector(0.0f, 0.0f, 0.0f),
	//			FVector(ShapeWidth, 0.0f, 0.0f),
	//			FVector(ShapeWidth, ShapeDepth, 0.0f),
	//			FVector(0.0f, ShapeDepth, 0.0f)
	//		};
	//		FVector Offset(-ShapeWidth * 0.5f, 0.0f, 0.0f);
	//		UStaticMesh *LegMesh = Object->GetLampMeshObject();
	//		FVector LegMeshExtent = LegMesh->GetBoundingBox().GetExtent();
	//		float EdgeOffset = FMath::Max<float>(LegMeshExtent.X, LegMeshExtent.Y) + 7.0f;
	//		float fScale = 0.5f*AboveGround / LegMeshExtent.Z;
	//		UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	//		if (!CabinetActor.IsValid())
	//			return;
	//
	//		for (int32 i = 0; i < Outline.Num(); ++i)
	//		{
	//			FVector LastDir = Outline[i] - Outline[XRWHCMode::RoundIndex(i, -1, Outline.Num())];
	//			LastDir.Normalize();
	//			FVector LastRight = FVector::CrossProduct(FVector::UpVector, LastDir);

	//			FVector CurrentDir = Outline[XRWHCMode::RoundIndex(i, 1, Outline.Num())] - Outline[i];
	//			CurrentDir.Normalize();
	//			FVector CurrentRight = FVector::CrossProduct(FVector::UpVector, CurrentDir);

	//			AStaticMeshActor *Leg = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	//			Leg->bIsSelectable = 0;
	//			Leg->Tags.Emplace(XRActorTag::Immovable);
	//			Leg->Tags.Emplace(XRActorTag::WHCActor);
	//			UStaticMeshComponent *SMComp = Leg->GetStaticMeshComponent();
	//			SMComp->SetMobility(EComponentMobility::Movable);
	//			SMComp->SetStaticMesh(LegMesh);
	//			for (const auto &IterRef: Object->MaterialList)
	//				SMComp->SetMaterialByName(FName(*IterRef.SlotName), IterRef.DIM);
	//			Leg->AttachToActor(ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	//			Leg->SetActorRelativeLocation(Outline[i] + LastRight * EdgeOffset + CurrentRight * EdgeOffset + Offset - FVector(0.0f, 0.0f, AboveGround));
	//			Leg->SetActorRelativeScale3D(FVector(1.0f, 1.0f, fScale));
	//			ShapeLegs.Emplace(Leg);
	//		}
	//	}
	//}
	if (CabinetActor.IsValid()) {
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetLegsComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_LEGS, this);
		}
		XRCabinetActorT<XRWHCabinetLegsComponent> LegsComponent = CabinetActor;
		LegsComponent->SpawnShapeLegs();
	}
}

void FShapeInRoom::SpawnShape()
{
	ASCTShapeActor *ShapeActor = Shape->SpawnShapeActor();
	FSCTShapeUtilityTool::SetActorTag(ShapeActor, XRActorTag::WHCActor);
	ShapeActor->SetActorRelativeLocation(FVector(-Shape->GetShapeWidth() * 0.05f, 0.0f, 0.0f));
	ShapeActor->AttachToActor(ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	Shape->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);

	MakeImmovable();

	// 更新门板材质信息
	CacheMtl(CachedDoorMtl, LoadDoorMtl());
	// 更新柜子底板
	CacheMtl(CachedBoardMtl, LoadBottomBoardMtl());
	// 更新饰条材质
	CacheMtl(CachedInsertionBoardMtl, LoadInsertionBoardMtl());
	if (CabinetActor.IsValid())
	{
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetFurnitureComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_FArmyFURNITURE, this);
		}
		XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = CabinetActor;
		TSharedPtr<class FArmyFurniture> ShapeComponent = FurnitureComponent->Get();
		if (ShapeComponent.IsValid())
		{
			FArmySceneData::Get()->Add(ShapeComponent, XRArgument(1).ArgUint32(E_WHCModel));
		}
	}
	// 更新见光板信息
	UpdateDecBoardInfo();	
}

void FShapeInRoom::SpawnPlatform()
{
	//XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = CabinetActor;
	//TSharedPtr<FPlatformInfo> PlatformInfo = PlatformInfoComponent->Get();
	//if (PlatformInfo.IsValid() && 
	//	PlatformInfo->FrontSections.Num() > 0 && 
	//	PlatformInfo->BackSections.Num() > 0)
	//{
	//	DestroyPlatform();

	//	const TSharedPtr<FArmyWHCPoly> * PolyPtr = FArmyWHCabinetMgr::Get()->GetRooms().Find(RoomAttachedIndex);
	//	if (PolyPtr != nullptr)
	//	{
	//		FArmySplineResMgr &SplineResMgr = FArmySplineResMgr::Instance();
	//		FArmySplineRes * SplineRes = SplineResMgr.GetSplineRes(PlatformInfo->FrontSections[0]->GetCachedPath());

	//		FArmyWHCCookAssist AssistGen;
	//		AssistGen.PrepareGenTable(true, (*PolyPtr).Get(), TArray<FShapeInRoom*>{this}, GetRect().mMaxZ * 10.0f, true);
	//		AssistGen.GenTableMesh(
	//			SplineRes != nullptr ? (SplineRes->mSize.X * 10.0f - 20.0f) : 0.0f,
	//			SplineRes != nullptr ? SplineRes->mSize.X : 0.0f,
	//			SplineRes != nullptr ? SplineRes->mSize.Y : 0.0f,
	//			FArmyWHCCookAssist::FGenTable::CreateLambda(
	//				[this](const TArray<FPUVVertex>& Vertices, const TArray<uint16>& Indices) {	
	//					UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	//					Platform = World->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
	//					Platform->Tags.Emplace(XRActorTag::Immovable);
	//					Platform->Tags.AddUnique(XRActorTag::WHCTable);
	//					Platform->SetFolderPath(FArmyActorPath::GetWHCTablePath());
	//					Platform->SetIsSingleTable(true);
	//					Platform->RefreshMesh(Vertices, Indices);
	//					if (PlatformInfo->PlatformMtl.IsValid())
	//					{
	//						if (PlatformInfo->PlatformMtl->Mtl == nullptr)
	//						{
	//							PlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(PlatformInfo->PlatformMtl->GetCachedPath(), PlatformInfo->PlatformMtl->MtlParam);
	//							if (PlatformInfo->PlatformMtl->Mtl != nullptr)
	//								PlatformInfo->PlatformMtl->RcState = ECabinetResourceState::CabRc_Complete;
	//						}
	//						Platform->SetMaterial(PlatformInfo->PlatformMtl->Mtl);
	//					}
	//				}
	//			),
	//			FArmyWHCCookAssist::FGenTrim::CreateLambda(
	//				[this](int32 TrimType, const TArray<FVector2D>& Vers, bool bIsClose, float PlaceHigh) {
	//					AXRShapeTableActor* pActor = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
	//					TArray<FPUVVertex> FuVers; TArray<uint16> Faces;
	//					FString TrimName;
	//					switch (TrimType)
	//					{
	//						case 1: // 前挡
	//						{
	//							TrimName = PlatformInfo->FrontSections[0]->GetCachedPath();
	//							PlatformFrontSections.Emplace(pActor);
	//							break;
	//						}
	//						case 2: // 后挡
	//						{
	//							TrimName = PlatformInfo->BackSections[0]->GetCachedPath();
	//							PlatformBackSections.Emplace(pActor);
	//							break;
	//						}
	//					}
	//					FArmySplineRes* SplineRes = FArmySplineResMgr::Instance().CreateSpline(TrimName, const_cast<TArray<FVector2D>&>(Vers), bIsClose, FuVers, Faces);
	//					if (SplineRes == nullptr)
	//						return;
	//					pActor->Tags.Emplace(XRActorTag::Immovable);
	//					pActor->SetIsSingleTable(true);
	//					pActor->RefreshMesh(FuVers, Faces);
	//					pActor->SetActorLocation(FVector(0, 0, PlaceHigh));

	//					pActor->Tags.AddUnique(XRActorTag::WHCTable);
	//					pActor->SetFolderPath(FArmyActorPath::GetWHCTablePath());
	//					if (PlatformInfo->PlatformMtl.IsValid())
	//					{
	//						if (PlatformInfo->PlatformMtl->Mtl == nullptr)
	//						{
	//							PlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(PlatformInfo->PlatformMtl->GetCachedPath(), PlatformInfo->PlatformMtl->MtlParam);
	//							if (PlatformInfo->PlatformMtl->Mtl != nullptr)
	//								PlatformInfo->PlatformMtl->RcState = ECabinetResourceState::CabRc_Complete;
	//						}
	//						pActor->SetMaterial(PlatformInfo->PlatformMtl->Mtl);
	//					}

	//				}
	//			),
	//			FArmyWHCCookAssist::FCollectTrimOutline()
	//		);
	//	}
	//}
	if (CabinetActor.IsValid())
	{
		if (!CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetPlatformInfoComponent>()))
		{
			auto ICabinetComponent = CabinetActor->GenCabinetComponents(
				ECC_TYPE::CCT_PLATFORM, this);
		}
		XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformComponent = CabinetActor;
		PlatformComponent->SpawnPlatform();
	}
}


void FShapeInRoom::DestroyAccessories()
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		if (CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetAccessoryComponent>()))
		{
			AccessoryComponent->Destory();
			CabinetActor->RemoveAt(AccessoryComponent->Self().HashCode());
		}
	}
}

void FShapeInRoom::DestroyShapeLegs()
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetLegsComponent> LegsComponent = CabinetActor;
		if (CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetLegsComponent>()))
		{
			LegsComponent->Destory();
			CabinetActor->RemoveAt(LegsComponent->Self().HashCode());
		}
	}
}

void FShapeInRoom::DestroyShape()
{
	// 清空见光板信息
	DecBoardInfos.Empty();

	// 删除构件
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = CabinetActor;
		if (CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetFurnitureComponent>()))
		{
			FurnitureComponent->Destory();
			CabinetActor->RemoveAt(FurnitureComponent->Self().HashCode());
		}
	}

	// 删除柜子
	Shape.Reset();
}

void FShapeInRoom::DestroyPlatform()
{
	if (Platform != nullptr)
	{
		Platform->Destroy();
		Platform = nullptr;
	}

	for (auto &PlatformFrontSection : PlatformFrontSections)
		PlatformFrontSection->Destroy();
	PlatformFrontSections.Empty();

	for (auto &PlatformBackSection : PlatformBackSections)
		PlatformBackSection->Destroy();
	PlatformBackSections.Empty();

}
void FShapeInRoom::DestroyRuler()
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = CabinetActor;
		if (CabinetActor->Have(CabinetActor->HashClassType<XRWHCabinetRulerComponent>()))
		{
			RulerComponent->Destory();
			CabinetActor->RemoveAt(RulerComponent->Self().HashCode());
		}
	}
}

bool FShapeInRoom::HasValidShape() const
{
	AActor *ShapeActor = Shape->GetShapeActor();
	return ShapeActor != nullptr ? (!ShapeActor->ActorHasTag(XRActorTag::WHCAlwaysHidden)) : false;
}

bool FShapeInRoom::HasValidAccessory() const
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		return Accessory.IsValid() && Accessory->Actor != nullptr && (!Accessory->Actor->ActorHasTag(XRActorTag::WHCAlwaysHidden));
	}
	return false;
}


bool FShapeInRoom::IsPlatformSpawned() const
{
	return Platform != nullptr;
}

void FShapeInRoom::ShowShapeLegs(bool bShow)
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetLegsComponent> LegsComponent = CabinetActor;
		TArray<class AStaticMeshActor*>& ShapeLegs = LegsComponent->Get();

		for (auto &ShapeLeg : ShapeLegs)
			ShapeLeg->SetActorHiddenInGame(!bShow);
	}
}

void FShapeInRoom::ShowAccessories(bool bShow)
{
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		// 隐藏附属配件
		if (Accessory.IsValid())
		{
			if (Accessory->Actor != nullptr)
				Accessory->Actor->SetActorHiddenInGame(!bShow);

			for (auto & SubAccessory : Accessory->AttachedSubAccessories)
			{
				if (SubAccessory->Actor != nullptr)
					SubAccessory->Actor->SetActorHiddenInGame(!bShow);
			}
		}
	}
}

void FShapeInRoom::UpdateComponents()
{
	// 柜子构件
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = CabinetActor;
		TSharedPtr<class FArmyFurniture> ShapeComponent = FurnitureComponent->Get();
		if (ShapeComponent.IsValid())
		{
			FVector Bound = ShapeComponent->GetPreBounds().GetSize();
			FRotator Rotation = ShapeFrame->GetActorRotation();
			FVector Right = Rotation.RotateVector(FVector::RightVector);
			float ShapeDepth = Shape->GetShapeDepth() * 0.1f;
			ShapeComponent->SetTransform(
				FTransform(
					Rotation,
					ShapeFrame->GetActorLocation() + Right * ShapeDepth * 0.5f,
					FVector(FMath::IsNearlyZero(Bound.X) ? 1.0f : Shape->GetShapeWidth() * 0.1f / Bound.X, FMath::IsNearlyZero(Bound.Y) ? 1.0f : ShapeDepth / Bound.Y, 1.0f)
				)
			);
		}
	}
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		// 附属物构件更新
		if (Accessory.IsValid())
			Accessory->UpdateComponent();
	}
}

void FShapeInRoom::OnAboveFloorChange()
{
	FVector Pos = ShapeFrame->GetActorLocation();
	Pos.Z = AboveGround;
	ShapeFrame->SetActorLocation(Pos);

	int32 SynID = FArmyWHCabinetMgr::Get()->GetMesh(TEXT("WHC/MATERIAL_1542592442974_3994.pak"));
	int32 ShapeCategory = Shape->GetShapeCategory();
	if (SynID != -1 && (ShapeCategory == (int32)ECabinetType::EType_OnGroundCab || ShapeCategory == (int32)ECabinetType::EType_TallCab))
	{
		USCTXRResourceManager & ResMgr = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager();
		FSCTVRSObject *Object = ResMgr.GetObjFromObjID(SynID);
		if (Object != nullptr)
		{
			float ShapeWidth = Shape->GetShapeWidth() * 0.1f;
			float ShapeDepth = Shape->GetShapeDepth() * 0.1f;

			TArray<FVector> Outline{
				FVector(0.0f, 0.0f, 0.0f),
				FVector(ShapeWidth, 0.0f, 0.0f),
				FVector(ShapeWidth, ShapeDepth, 0.0f),
				FVector(0.0f, ShapeDepth, 0.0f)
			};
			FVector Offset(-ShapeWidth * 0.5f, 0.0f, 0.0f);
			UStaticMesh *LegMesh = Object->GetLampMeshObject();
			FVector LegMeshExtent = LegMesh->GetBoundingBox().GetExtent();
			float EdgeOffset = FMath::Max<float>(LegMeshExtent.X, LegMeshExtent.Y) + 7.0f;
			float fScale = 0.5f*AboveGround / LegMeshExtent.Z;
			if (CabinetActor.IsValid())
			{
				XRCabinetActorT<XRWHCabinetLegsComponent> LegsComponent = CabinetActor;
				TArray<class AStaticMeshActor*>& ShapeLegs = LegsComponent->Get();
				for (int32 i = 0; i < Outline.Num(); ++i)
				{
					FVector LastDir = Outline[i] - Outline[XRWHCMode::RoundIndex(i, -1, Outline.Num())];
					LastDir.Normalize();
					FVector LastRight = FVector::CrossProduct(FVector::UpVector, LastDir);

					FVector CurrentDir = Outline[XRWHCMode::RoundIndex(i, 1, Outline.Num())] - Outline[i];
					CurrentDir.Normalize();
					FVector CurrentRight = FVector::CrossProduct(FVector::UpVector, CurrentDir);

					AStaticMeshActor *Leg = ShapeLegs[i];
					Leg->SetActorRelativeLocation(Outline[i] + LastRight * EdgeOffset + CurrentRight * EdgeOffset + Offset - FVector(0.0f, 0.0f, AboveGround));
					Leg->SetActorRelativeScale3D(FVector(1.0f, 1.0f, fScale));
				}
			}
		}
	}
	// if (Accessory.IsValid() && Accessory->Actor != nullptr)
	// {
	// 	bool bIncludeTableHeight = ShapeCategory == (int32)ECabinetType::EType_OnGroundCab;
	// 	FVector RelLocation(0.0f, Shape->GetShapeDepth() * 0.05f, 
	// 		Shape->GetShapeHeight() * 0.1f + (bIncludeTableHeight ? gCabinMgr.GetTableHeight(true) : 0.0f));
	// 	Accessory->Actor->SetActorRelativeLocation(RelLocation);
	// 	Accessory->CabAccInfo->RelativeLocation = RelLocation;
	// }
}

bool FShapeInRoom::SetSunBoard(int32 nFace)
{
	FSCTShapeUtilityTool::ModifyCabinetVeneerdSudeBoardActorValid(Shape.Get(), (FSCTShapeUtilityTool::EPosition)nFace, true);

	FArmyWHCRect rect = GetRect();
	if (gCabinMgr.Collision(this, &rect, 0.0f, true))
	{
		UnsetSunBoard(nFace);
		return false;
	}

	// 重新查找见光板
	UpdateDecBoardInfo();
	RefreshShapeWidthRuler();
	return true;
}

void FShapeInRoom::UnsetSunBoard(int32 nFace)
{
	FSCTShapeUtilityTool::ModifyCabinetVeneerdSudeBoardActorValid(Shape.Get(), (FSCTShapeUtilityTool::EPosition)nFace, false);
	// 重新查找见光板
	UpdateDecBoardInfo();
	RefreshShapeWidthRuler();
}

void FShapeInRoom::ChangeSunBoardMtl(FMtlInfo *InMtlInfo)
{
	TSharedPtr<FCommonPakData> PakData = XRShapeUtilities::CreateMtlPakData(InMtlInfo);
	if (InMtlInfo->Mtl == nullptr)
	{
		XRShapeUtilities::LoadMtl(InMtlInfo);
		if (InMtlInfo->Mtl == nullptr)
			return;
	}

	FSCTShapeUtilityTool::ReplaceCabinetVeneerdSudeBoardMaterial(Shape.Get(), 
		PakData.Get(), InMtlInfo->Mtl);
	// 重新查找见光板
	UpdateDecBoardInfo();
}

bool FShapeInRoom::IsSunBoard(int32 nFace) const
{
	for (const auto &BoardInfo : DecBoardInfos)
	{
		if (nFace == BoardInfo.Face)
			return true;
	}
	return false;
}

float FShapeInRoom::GetSunBoardWidth(int32 nFace) const
{
	for (const auto &BoardInfo : DecBoardInfos)
	{
		if (nFace == BoardInfo.Face)
			return BoardInfo.BoardValue * 0.1f;
	}
	return 0.0f;
}

bool FShapeInRoom::CanGenGlobalTableBoardAndToe() const
{
	if (Shape.IsValid())
	{
		int32 ShapeCategory = Shape->GetShapeCategory();
		return (ShapeCategory == (int32)ECabinetType::EType_OnGroundCab) || (ShapeCategory == (int32)ECabinetType::EType_TallCab);
	}
	else
	{
		return false;
	}
}

void FShapeInRoom::PrepareShapeAnimations()
{
	Animations.Empty();

	FCabinetShape *pCab = static_cast<FCabinetShape*>(Shape.Get());
	const TSharedPtr<FSpaceShape>& pSpaceShape = pCab->GetTopSpaceShape();
	TDoubleLinkedList< TSharedPtr<FSpaceShape> > Spaces;
	Spaces.AddTail(pSpaceShape);
	do
	{
		auto pHead = Spaces.GetHead();
		TSharedPtr<FSpaceShape> pSpace = pHead->GetValue();
		Spaces.RemoveNode(pHead);

		TSharedPtr<FDoorGroup> pDoorGroup = pSpace->GetDoorShape();

		if (pDoorGroup.IsValid() && pDoorGroup->GetShapeType() == EShapeType::ST_SideHungDoor)
		{
			FSideHungDoor* pSideHungDoor = static_cast<FSideHungDoor*>(pDoorGroup.Get());
			const TArray<TSharedPtr<FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor>>& DoorSheets = pSideHungDoor->GetDoorSheets();
			int32 DoorSheetNum = DoorSheets.Num();
			if (DoorSheetNum <= 0)
				continue;

			if (DoorSheetNum == 1) {
				// 1 左开，2右开
				// 设置每一个门的一个animation
				FVariableAreaDoorSheet::EDoorOpenDirection openDir = DoorSheets[0]->GetOpenDoorDirection();

				// 管理内存！
				if (openDir == FVariableAreaDoorSheet::EDoorOpenDirection::E_Left) {
					FLeftOpenDoorAnimation *Animation = new FLeftOpenDoorAnimation;
					Animation->InitDefault(DoorSheets[0].Get());
					Animation->SetAnimationParams(90.0f);
					Animations.Emplace(MakeShareable(Animation));
				}
				else if (openDir == FVariableAreaDoorSheet::EDoorOpenDirection::E_Right) {
					FRightOpenDoorAnimation *Animation = new FRightOpenDoorAnimation;
					Animation->InitDefault(DoorSheets[0].Get());
					Animation->SetAnimationParams(90.0f);
					Animations.Emplace(MakeShareable(Animation));
				}
				else if (openDir == FVariableAreaDoorSheet::EDoorOpenDirection::E_Top) {
					FTopOpenDoorAnimation *Animation = new FTopOpenDoorAnimation;
					Animation->InitDefault(DoorSheets[0].Get());
					Animation->SetAnimationParams(90.0f);
					Animations.Emplace(MakeShareable(Animation));
				}
			}
			else if (DoorSheetNum == 2) {

				FLeftOpenDoorAnimation *AnimationLeft = new FLeftOpenDoorAnimation;
				AnimationLeft->InitDefault(DoorSheets[0].Get());
				AnimationLeft->SetAnimationParams(90.0f);
				Animations.Emplace(MakeShareable(AnimationLeft));

				FRightOpenDoorAnimation *AnimationRight = new FRightOpenDoorAnimation;
				AnimationRight->InitDefault(DoorSheets[1].Get());
				AnimationRight->SetAnimationParams(90.0f);
				Animations.Emplace(MakeShareable(AnimationRight));
	
			}
		}

		TSharedPtr<FDrawerGroupShape> pDrawerGroup = pSpace->GetCoveredDrawerGroup();
		if (pDrawerGroup.IsValid() && pDrawerGroup->GetShapeType() == EShapeType::ST_DrawerGroup) {
			TSharedPtr<FSlidingDrawerShape> PDrawer = pDrawerGroup->GetPrimitiveDrawer();
			TArray<TSharedPtr<FSlidingDrawerShape>> Drawers = pDrawerGroup->GetCopyDrawerShapes();
			Drawers.Emplace(PDrawer);
			for (const auto & Drawer : Drawers)
			{
				FDrawerAnimation * DrawerAnimation = new FDrawerAnimation;
				DrawerAnimation->InitDefault(Drawer.Get());
				Animations.Emplace(MakeShareable(DrawerAnimation));
			}
		}

		auto pInside = pSpace->GetInsideSpace();
		if (pInside.IsValid())
			Spaces.AddTail(pInside);
		for (auto& p : pSpace->GetChildSpaceShapes())
			Spaces.AddTail(p);

	} while (Spaces.Num() > 0);
}

TSharedPtr<FAnimation> FShapeInRoom::FindShapeAnimation(FSCTShape *InShape) const
{
	for (const auto &Animation : Animations)
	{
		if (Animation->ShapeRef == InShape)
			return Animation;
	}
	return TSharedPtr<FAnimation>();
}

UMaterialInterface* FShapeInRoom::GetDoorMtl() const
{
	return CachedDoorMtl;
}

UMaterialInterface* FShapeInRoom::GetInsertionBoardMtl() const
{
	return CachedInsertionBoardMtl;
}

UMaterialInterface* FShapeInRoom::GetBottomMtl() const
{
	return CachedBoardMtl;
}

void FShapeInRoom::OnChangeDoorMtl()
{
	CacheMtl(CachedDoorMtl, LoadDoorMtl());
	gCabinMgr.OnRefreshShape(Shape->GetShapeCategory());
}

void FShapeInRoom::MakeImmovable()
{
	ASCTShapeActor * ShapeActor = Shape->GetShapeActor();
	if (!ShapeActor->ActorHasTag(XRActorTag::Immovable))
		ShapeActor->Tags.Emplace(XRActorTag::Immovable);

	TArray<AActor*> AttachedActors;
	ShapeActor->GetAttachedActors(AttachedActors);
	for (auto &AttachedActor : AttachedActors)
		MakeImmovableInternal(AttachedActor);
}

void FShapeInRoom::MakeImmovableInternal(AActor *InActor)
{
	InActor->bIsSelectable = 0;
	TArray<AActor*> AttachedActors;
	InActor->GetAttachedActors(AttachedActors);
	for (auto &AttachedActor : AttachedActors)
		MakeImmovableInternal(AttachedActor);
}

FArmyWHCRect FShapeInRoom::GetRect(FVector* NewPos/* =nullptr */, float* fNewHalfWidth)
{
	FArmyWHCRect Rect;
	Rect.mShapeInRoom = this;
	FVector Pos;
	if (NewPos)
		Pos = *NewPos;
	else
		Pos = ShapeFrame->GetActorLocation();

	FRotator ShapeRotation = ShapeFrame->GetActorRotation();
	float fAngle = ShapeRotation.Yaw*PI / 180.0f;
	FVector2D Dir(FMath::Cos(fAngle), FMath::Sin(fAngle));

	FVector Size(Shape->GetShapeWidth() * 0.1f,Shape->GetShapeDepth() * 0.1f, Shape->GetShapeHeight()*0.1f);
	if (fNewHalfWidth)
		Size.X = *fNewHalfWidth*0.1f;
	Rect.Init(Pos, Dir, Size, FVector2D(0, -1));
	Rect.Extern(0, 0, GetSunBoardWidth(1), GetSunBoardWidth(0));

	return MoveTemp(Rect);
}

bool FShapeInRoom::SetTranslate(FArmyWHCRect& Rect)
{
	FVector Pos = Rect.GetPos(FVector(0, -1,-1), 0, 0, GetSunBoardWidth(1), GetSunBoardWidth(0));
	ShapeFrame->SetActorLocation(Pos);
	ShapeFrame->SetActorRotation(Rect.GetRotation());
	return true;
}

float FShapeInRoom::GetLeftDist(FShapeInRoom* pIgnoredActor)
{
	FArmyWHCRect CabinetRect = GetRect();

	float HalfShapeWidth = CabinetRect.mSizeHalf.X;
	float HalfShapeDepth = CabinetRect.mSizeHalf.Y;
	float HalfShapeHeight = CabinetRect.SizezHalf();

	FVector ShapeLocation = CabinetRect.GetPos(FVector(0, -1, -1));
	FRotator ShapeRotation = ShapeFrame->GetActorRotation();

	FVector ShapeForward = ShapeRotation.RotateVector(FVector::ForwardVector);
	FVector ShapeRight = ShapeRotation.RotateVector(FVector::RightVector);

	FVector LeftWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXLeft, FArmyWHCRect::eYFront, FArmyWHCRect::eZTop);

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	auto WallDistanceCompare = [](float InSrc, float InDest) -> bool {
		if (FMath::IsNearlyEqual(InSrc, InDest, 0.1f))
			return false;
		else
			return true;
	};
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	if (pIgnoredActor)
		QueryParams.AddIgnoredActor(pIgnoredActor->ShapeFrame);
	QueryParams.AddIgnoredActor(ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	bool LeftHasBlocking = false;
	FHitResult HitResultLeft;
	if (World->SweepSingleByChannel(HitResultLeft,
		LeftWallIntersectionStart,
		LeftWallIntersectionStart - ShapeForward * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_GameTraceChannel1,
		FCollisionShape::MakeBox(FVector(0.1f, HalfShapeDepth, HalfShapeHeight)),
		QueryParams))
	{
		AActor *LeftHitActor = HitResultLeft.GetActor();
		if (LeftHitActor != nullptr && LeftHitActor->IsA<AXRShapeFrame>())
		{
			return HitResultLeft.Distance;
		}
	}

	FVector Intersection;
	FVector LeftDir = -CabinetRect.GetDir();

	FArmyWHCPoly* pPoly = gCabinMgr.GetPolyById(RoomAttachedIndex);
	if (pPoly == nullptr)
		return 0.0f;
	if (pPoly->RayCollisonWall(LeftWallIntersectionStart, LeftDir, 0, FArmySceneData::WallHeight, Intersection))
	{
		float Length = (Intersection - LeftWallIntersectionStart).Size();
		return Length;
	}
	return 0.0f;
}

float FShapeInRoom::GetRightDist(FShapeInRoom* pIgnoredActor)
{
	FArmyWHCRect CabinetRect = GetRect();

	float HalfShapeWidth = CabinetRect.mSizeHalf.X;
	float HalfShapeDepth = CabinetRect.mSizeHalf.Y;
	float HalfShapeHeight = CabinetRect.SizezHalf();

	FVector ShapeLocation = CabinetRect.GetPos(FVector(0, -1, -1));
	FRotator ShapeRotation = ShapeFrame->GetActorRotation();

	FVector ShapeForward = ShapeRotation.RotateVector(FVector::ForwardVector);
	FVector ShapeRight = ShapeRotation.RotateVector(FVector::RightVector);

	FVector RightWallIntersectionStart = CabinetRect.GetPos(FArmyWHCRect::eXRight, FArmyWHCRect::eYFront, FArmyWHCRect::eZTop);
	FVector RightDir = CabinetRect.GetDir();

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	if (pIgnoredActor)
		QueryParams.AddIgnoredActor(pIgnoredActor->ShapeFrame);
	QueryParams.AddIgnoredActor(ShapeFrame);
	QueryParams.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GVC->GetWorld(), 0));

	FHitResult HitResultRight;
	if (World->SweepSingleByChannel(HitResultRight,
		RightWallIntersectionStart,
		RightWallIntersectionStart + ShapeForward * 10000.0f,
		ShapeRotation.Quaternion(),
		ECC_GameTraceChannel1,
		FCollisionShape::MakeBox(FVector(0.1f, HalfShapeDepth, HalfShapeHeight)),
		QueryParams))
	{
		AActor *RightHitActor = HitResultRight.GetActor();
		if (RightHitActor != nullptr && RightHitActor->IsA<AXRShapeFrame>())
		{
			return HitResultRight.Distance;
		}
	}

	FVector Intersection;
	FArmyWHCPoly* pPoly = gCabinMgr.GetPolyById(RoomAttachedIndex);
	if (pPoly == nullptr)
		return 0.0f;

	if (pPoly->RayCollisonWall(RightWallIntersectionStart, RightDir, 0, FArmySceneData::WallHeight, Intersection))
	{
		float Length = (Intersection - RightWallIntersectionStart).Size();
		return Length;
	}
	return 0.0f;
}

float FShapeInRoom::GetMatchWidth(float NewWidth)
{
	FCabinetShape * CabShape = static_cast<FCabinetShape*>(Shape.Get());

	//TSharedPtr<FNumberRangeAttri> RangeWidthAttri = StaticCastSharedPtr<FNumberRangeAttri>(CabShape->GetShapeWidthAttri());
	//if (NewWidth >= RangeWidthAttri->GetMinValue() && NewWidth <= RangeWidthAttri->GetMaxValue())
	//{
	//	return NewWidth;
	//}

	TArray<float> StandardWidths = CabShape->GetStandardWidths();
	if ( StandardWidths.Num()>0 )
	{
		if (NewWidth >= StandardWidths[StandardWidths.Num() - 1]+200 )
		{
			return 0.0f;
		}
		for ( int i= StandardWidths.Num()-1; i>=0;--i )
		{
			if (NewWidth >= StandardWidths[i])
				return StandardWidths[i];
		}
		return 0.0f;
	}
	return 0.0f;
}

bool FShapeInRoom::SetNewWidth(float NewWidth)
{
	if (FMath::Abs(Shape->GetShapeWidth() - NewWidth) < 1.0f)
		return false;
	Shape->SetShapeWidth(NewWidth);
	float OldDepth = Shape->GetShapeDepth();
	float OldHeight = Shape->GetShapeHeight();
	if (CabinetActor.IsValid())
	{
		XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = CabinetActor;
		TSharedPtr<class FShapeRuler> ShapeWidthRuler = RulerComponent->Get();
		if (ShapeWidthRuler.IsValid())
		{
			ShapeWidthRuler->SetRulerLength(NewWidth * 0.1f);
			ShapeWidthRuler->GetRulerActor()->SetActorRelativeLocation(FVector(-NewWidth * 0.05f, 0.0f, OldHeight * 0.1f));
		}
		Shape->GetShapeActor()->SetActorRelativeLocation(FVector(-NewWidth * 0.05f, 0.0f, 0.0f));
		ShapeFrame->RefreshFrame(NewWidth * 0.1f, OldDepth * 0.1f, OldHeight * 0.1f);

		RefreshShapeWidthRuler();
		// 更新调整脚
		DestroyShapeLegs();
		SpawnShapeLegs();
		// 更新构件
		UpdateComponents();
	}
	return true;
}

UMaterialInterface* FShapeInRoom::LoadDoorMtl()
{
	int32 MtlId = 0;
	FString MtlParam;

	FCabinetShape* pCab = (FCabinetShape*)(Shape.Get());
	const TSharedPtr<FSpaceShape>& pSpaceShape = pCab->GetTopSpaceShape();
	TDoubleLinkedList< TSharedPtr<FSpaceShape> > Spaces;
	Spaces.AddTail(pSpaceShape);
	do
	{
		auto pHead = Spaces.GetHead();
		TSharedPtr<FSpaceShape> pSpace = pHead->GetValue();
		Spaces.RemoveNode(pHead);

		TSharedPtr<FDoorGroup> pDoorGroup = pSpace->GetDoorShape();
		if (pDoorGroup.IsValid())
		{
			if (pDoorGroup->GetShapeType() == EShapeType::ST_SlidingDoor)
			{
				FSlidingDoor* pSlidingDoor = (FSlidingDoor*)pDoorGroup.Get();
				const TArray<TSharedPtr<FSpaceDividDoorSheet>>& DoorSheets = pSlidingDoor->GetDoorSheets();
				if (DoorSheets.Num() > 0)
				{
					FDoorSheetShapeBase::FDoorPakMetaData &MtlData = DoorSheets[0]->GetMaterial();
					MtlId = MtlData.ID;
					MtlParam = MtlData.OptimizeParam;
					break;
				}
			}
			else if (pDoorGroup->GetShapeType() == EShapeType::ST_SideHungDoor)
			{
				FSideHungDoor* pSideHungDoor = (FSideHungDoor*)pDoorGroup.Get();
				const TArray<TSharedPtr<FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor>>& DoorSheets = pSideHungDoor->GetDoorSheets();
				if (DoorSheets.Num() > 0)
				{
					FDoorSheetShapeBase::FDoorPakMetaData &MtlData = DoorSheets[0]->GetMaterial();
					MtlId = MtlData.ID;
					MtlParam = MtlData.OptimizeParam;
					break;
				}
			}
		}

		TSharedPtr<FDrawerGroupShape> pDrawerGroup = pSpace->GetCoveredDrawerGroup();
		if (pDrawerGroup.IsValid())
		{
			TSharedPtr<FSlidingDrawerShape> SlidingDrawShape = pDrawerGroup->GetPrimitiveDrawer();
			if ( SlidingDrawShape.IsValid() )
			{
				const TArray<TSharedPtr<FDrawerDoorShape>> & DoorSheets = SlidingDrawShape->GetDrawerDoorShapes();
				if (DoorSheets.Num() > 0)
				{
					TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> DoorSheetDrawer = DoorSheets[0]->GetDrawDoorSheet();
					FDoorSheetShapeBase::FDoorPakMetaData &MtlData = DoorSheetDrawer->GetMaterial();
					MtlId = MtlData.ID;
					MtlParam = MtlData.OptimizeParam;
					break;
				}
			}
		}

		auto pInside = pSpace->GetInsideSpace();
		if (pInside.IsValid())
			Spaces.AddTail(pInside);
		for (auto& p : pSpace->GetChildSpaceShapes())
			Spaces.AddTail(p);
	} while (Spaces.Num() > 0);

	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetMaterialInterfaceWithParam(EMaterialType::Board_Material, FString::FromInt(MtlId), MtlParam));
	// if ( MID == nullptr )
	// {
	// 	for (const auto &Change : ChangeList)
	// 	{
	// 		if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL))
	// 		{
	// 			FArmyWHCSingleDoorGroupMtlChange * Temp = static_cast<FArmyWHCSingleDoorGroupMtlChange*>(Change.Get());
	// 			int32 Id = Temp->GetDoorMtlId();

	// 			const TSharedPtr<FMtlInfo>* Mtl = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
	// 			if (Mtl && (*Mtl)->Mtl)
	// 			{
	// 				MID=(UMaterialInstanceDynamic*)((*Mtl)->Mtl);
	// 				break;
	// 			}
	// 		}
	// 	}
	// }
	if (MID)
	{
		UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(MID->Parent, nullptr);
		NewMID->CopyParameterOverrides(MID);
		return NewMID;
	}
	return nullptr;
}

UMaterialInterface* FShapeInRoom::LoadBottomBoardMtl()
{
	int32 MtlId = 0;
	FString MtlParam;

	FCabinetShape* pCab = (FCabinetShape*)(Shape.Get());
	const TSharedPtr<FSpaceShape>& pSpaceShape = pCab->GetTopSpaceShape();
	TDoubleLinkedList< TSharedPtr<FSpaceShape> > Spaces;
	Spaces.AddTail(pSpaceShape);
	do
	{
		auto pHead = Spaces.GetHead();
		TSharedPtr<FSpaceShape> pSpace = pHead->GetValue();
		Spaces.RemoveNode(pHead);
		if ( pSpace->GetFrameShape().IsValid() )
		{
			TSharedPtr<FBoardShape> pBoardShape = pSpace->GetFrameShape()->GetFrameBoardShape(1, 0);
			if (pBoardShape.IsValid())
			{
				const FPakMetaData &MtlData = pBoardShape->GetMaterialData();
				MtlId = MtlData.ID;
				MtlParam = MtlData.OptimizeParam;
				break;
			}
		}
		auto pInside = pSpace->GetInsideSpace();
		if (pInside.IsValid())
			Spaces.AddTail(pInside);
		for (auto& p : pSpace->GetChildSpaceShapes())
			Spaces.AddTail(p);
	} while (Spaces.Num() > 0);

	UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetMaterialInterfaceWithParam(EMaterialType::Board_Material, FString::FromInt(MtlId), MtlParam));
	if (MID)
	{
		UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(MID->Parent, nullptr);
		NewMID->CopyParameterOverrides(MID);
		return NewMID;
	}
	return nullptr;
}

UMaterialInterface* FShapeInRoom::LoadInsertionBoardMtl()
{
	TArray<FInsertionBoard*> InsertionBoards;
	FSCTShapeUtilityTool::GetCabinetAllInsertionBoard(Shape.Get(), InsertionBoards);
	if (InsertionBoards.Num() > 0)
	{
		const FInsertionBoard::FInsertBoardMaterial &MtlData = InsertionBoards[0]->GetMaterial();
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->GetMaterialInterfaceWithParam(EMaterialType::Board_Material, FString::FromInt(MtlData.ID), MtlData.OptimizeParam));
		if (MID)
		{
			UMaterialInstanceDynamic* NewMID = UMaterialInstanceDynamic::Create(MID->Parent, nullptr);
			NewMID->CopyParameterOverrides(MID);
			return NewMID;
		}
	}
	return nullptr;
}

void FShapeInRoom::CacheMtl(UMaterialInterface *&InOutCacheSlot, UMaterialInterface *InMtlToCache)
{
	if (InOutCacheSlot != nullptr)
		InOutCacheSlot->RemoveFromRoot();
	InOutCacheSlot = InMtlToCache;
	if (InOutCacheSlot != nullptr)
		InOutCacheSlot->AddToRoot();
}

FShapeAccessory* FShapeInRoom::MakeShapeAccessory(FCabinetAccInfo *InAccInfo)
{
	FShapeAccessory * NewAccessory = new FShapeAccessory;
	NewAccessory->CabAccInfo = MakeShareable(new FCabinetAccInfo);
	*NewAccessory->CabAccInfo = *InAccInfo;
	return NewAccessory;
}

void FShapeInRoom::MakeComponent(TSharedPtr<class FArmyFurniture> &OutFurniture, const FString &InComponentPath, EObjectType InObjType, uint32 InBelongClass)
{
	TArray<TSharedPtr<FArmyFurniture>> ObjectLayers = FArmyToolsModule::ParseDXF(InComponentPath);
	if (ObjectLayers.Num() > 0)
	{
		OutFurniture = ObjectLayers[0];
		OutFurniture->SetType(InObjType);
		OutFurniture->BelongClass = InBelongClass;
	}
}

void FShapeInRoom::RefreshShapeWidthRuler()
{
	/*if (ShapeWidthRuler.IsValid())
	{
		FArmyWHCRect rect = GetRect();
		ShapeWidthRuler->SetRulerLength(rect.mSizeHalf.X*2);
		AShapeRulerActor *RulerActor = ShapeWidthRuler->GetRulerActor();
		RulerActor->SetActorRelativeLocation(FVector(-rect.mSizeHalf.X+(GetSunBoardWidth(1)-GetSunBoardWidth(0)) * 0.5f, 0.0f, rect.SizezHalf()*2.0f));
	}*/
}

void FShapeInRoom::UpdateDecBoardInfo()
{
	DecBoardInfos.Empty();
	TArray<FSCTShapeUtilityTool::FVeneerdSudeBoardInfo> BoardInfos = FSCTShapeUtilityTool::GetCabinetAllAddedVeneerdSudeBoards(Shape.Get());
	for (const auto &BoardInfo : BoardInfos)
	{
		FDecBoardInfo DecBoardInfo;
		DecBoardInfo.Face = (int32)BoardInfo.Position;
		DecBoardInfo.Spawned = BoardInfo.IsActorValid ? 1 : 0;
		DecBoardInfo.BoardValue = BoardInfo.BoardValue;
		DecBoardInfo.MtlId = BoardInfo.MaterialId;
		if (!FWHCModeGlobalData::CabinetMtlMap.Contains(DecBoardInfo.MtlId))
		{
			FMtlInfo *MtlInfo = new FMtlInfo;
			MtlInfo->Id = BoardInfo.MaterialId;
			MtlInfo->MtlName = BoardInfo.MaterialName;
			MtlInfo->MtlThumbnailUrl = BoardInfo.MaterialThumbnailUrl;
			MtlInfo->MtlUrl = BoardInfo.PakUrl;
			MtlInfo->MtlMd5 = BoardInfo.PakMd5;
			MtlInfo->MtlParam = BoardInfo.MaterialOptimiztionParam;
			// MtlInfo->RcState = ECabinetResourceState::CabRc_Complete;
			FWHCModeGlobalData::CabinetMtlMap.Emplace(MtlInfo->Id, MakeShareable(MtlInfo));
		}
		DecBoardInfos.Emplace(DecBoardInfo);
	}	
}