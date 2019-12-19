
#include "ArmyWHCabinetComponent.h"
#include "SCTResManager.h"
#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyWHCabinet.h"

XRWHCabinetIComponent::~XRWHCabinetIComponent(){
}

XRWHCabinetAccessoryComponent::XRWHCabinetAccessoryComponent() {
	ComponentClassName = XRWHCabinetAccessoryComponent::typeName();
	IsValidComponent = true;
}
XRWHCabinetAccessoryComponent::~XRWHCabinetAccessoryComponent() {

}

void XRWHCabinetAccessoryComponent::InitializeObject(FCabinetWholeAccInfo *InCabAccInfo,
	const TArray<TSharedPtr<FCabinetComponent>>& InAccessories) {
	check(nullptr != InCabAccInfo);
	//check( _actorBase.IsValid());
	
	if (!CabRoom()) {
		return;
	}
	// 将附属件信息复制一份，放到FShapeAccessory中
	// 原因：附属件可能会关联子部件，附属件和子部件都可以独立更换，更换完毕后要同步CabinetAccInfo的信息
	// 而且每个柜子实例是否替换附属件都是不确定的。如果直接将指针传递过来，当有些柜子修改了附属件，再添加
	// 新柜子的时候就会生成替换后的附属件。要保证原始柜子信息中的附属件信息是不变的，所以柜子实例会复制一份
	// 附属件信息

	Accessory = MakeShareable(CabRoom()->MakeShapeAccessory(InCabAccInfo->Self.Get()));
	check(Accessory.IsValid());
	UpdateAccessoryLocation();

	for (const auto & SubAccInfo : InCabAccInfo->SubAccInfos)
		Accessory->AttachedSubAccessories.Emplace(MakeShareable(CabRoom()->MakeShapeAccessory(SubAccInfo.Get())));

	if (Accessory->AttachedSubAccessories.Num() > 0)
	{
		if (CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_Basin) ||
			CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_BathroomBasin)) // 水槽
		{
			Accessory->AttachedSubAccessories[0]->CabAccInfo->bBoundToSocket = true;
		}
		else if (CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_Cooker) ||
			CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator)) // 灶具和油烟机
		{
			Accessory->AttachedSubAccessories[0]->CabAccInfo->bBoundToSocket = false;
		}
	}

	// 初始化构件
	for (auto Component : InAccessories)
	{
		if (InCabAccInfo->Self->Id == Component->Id && Component->Type == 105)
		{
			CabRoom()->MakeComponent(Accessory->Component, Component->GetCachePath(1), OT_ComponentBase, 105);
			break;
		}
	}
}

void XRWHCabinetAccessoryComponent::SpawnAccessories() {
	if (!Accessory.IsValid())
		return;
	
	USCTXRResourceManager & ResMgr = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager();
	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();

	bool bRet = Accessory->SpawnActor(CabRoom()->ShapeFrame);
	if (bRet)
	{
		FSCTVRSObject *Object = Accessory->GetResourceObj();
		int32 IterCount = Accessory->AttachedSubAccessories.Num();
		for (int32 i = 0; i < IterCount; ++i)
		{
			if (Accessory->AttachedSubAccessories[i]->CabAccInfo->bBoundToSocket)
			{
				// 为了防止Socket有数组越界，所以使用了两者较小的值当作Socket的索引
				int32 SocketIndex = FMath::Min(i, Object->SocketList.Num() - 1);
				if (SocketIndex < 0)
					continue;
				Accessory->AttachedSubAccessories[i]->CabAccInfo->RelativeLocation = 
					Object->SocketList[SocketIndex].Location;
				Accessory->AttachedSubAccessories[i]->SpawnActor(Accessory->Actor);
			}
			else
				Accessory->AttachedSubAccessories[i]->SpawnActor(Accessory->Actor);
		}

		// 将构件添加到HomeData中
		if (Accessory->Component.IsValid())
		{
			FArmySceneData::Get()->Add(Accessory->Component, XRArgument(1).ArgUint32(E_WHCModel));
			for (const auto & SubAccessory : Accessory->AttachedSubAccessories)
			{
				if (SubAccessory->Component.IsValid())
					FArmySceneData::Get()->Add(SubAccessory->Component, XRArgument(1).ArgUint32(E_WHCModel));
			}
		}
	}
}

void XRWHCabinetAccessoryComponent::Destory() {
	if (Accessory.IsValid())
	{
		Accessory.Reset();
		Accessory = nullptr;
	}
}
/* <summary>
/// 从actor 添加了该组件
/// </summary>
*/
FShapeAccessory* XRWHCabinetAccessoryComponent::AddSubAccessory(FCabinetAccInfo *InAccInfo,
	EMetalsType InMetalType, bool bBoundToSocket) {
	check(Accessory.IsValid());
	check(nullptr != InAccInfo);

	FShapeAccessory * NewSubAccessory = CabRoom()->MakeShapeAccessory(InAccInfo);
	NewSubAccessory->CabAccInfo->Type = (uint8)InMetalType;
	NewSubAccessory->CabAccInfo->bBoundToSocket = bBoundToSocket;
	Accessory->AttachedSubAccessories.Emplace(MakeShareable(NewSubAccessory));
	// TODO: 临时增加的对于烟机构件的逻辑
	if (InMetalType == EMetalsType::MT_FLUE_GAS_TURBINE)
	{
		FString DefaultComponent = FPaths::ProjectContentDir() / TEXT("WHC/ventilator.dxf");
		if (InAccInfo->ComponentArray.Num() > 0)
			DefaultComponent = InAccInfo->ComponentArray[0]->GetCachePath(1);
		CabRoom()->MakeComponent(NewSubAccessory->Component, DefaultComponent, OT_ComponentBase, 105);
	}
	return NewSubAccessory;
}
/* <summary>
/// 从actor 添加了该组件
/// </summary>
*/
FShapeAccessory* XRWHCabinetAccessoryComponent::ReplaceSubAccessory(FCabinetAccInfo *InOldAccInfo,
	FCabinetAccInfo *InNewAccInfo, EMetalsType InMetalType, bool bBoundToSocket) {
	check(Accessory.IsValid());
	check(nullptr != InOldAccInfo);
	check(nullptr != InNewAccInfo);

	int32 Id = InOldAccInfo->Id;
	int32 Index = Accessory->AttachedSubAccessories.IndexOfByPredicate(
		[Id](const TSharedPtr<FShapeAccessory> &SubAccessory) { return SubAccessory->CabAccInfo->Id == Id; }
	);
	check(Index != INDEX_NONE);

	FShapeAccessory * NewSubAccessory = CabRoom()->MakeShapeAccessory(InNewAccInfo);
	NewSubAccessory->CabAccInfo->Type = (uint8)InMetalType;
	NewSubAccessory->CabAccInfo->bBoundToSocket = bBoundToSocket;
	Accessory->AttachedSubAccessories[Index] = MakeShareable(NewSubAccessory);
	return NewSubAccessory;
}

void XRWHCabinetAccessoryComponent::SpawnSubAccessory(FShapeAccessory *InAccessory, 
	const FVector &InRelativeLocation) {
	if (Accessory.IsValid() && Accessory->Actor != nullptr)
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
	}
}

void XRWHCabinetAccessoryComponent::UpdateAccessoryLocation() {
	check(Accessory.IsValid());
	if (CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_Basin) || CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_Cooker))
	{
		Accessory->CabAccInfo->RelativeLocation.Set(0.0f, CabRoom()->Shape->GetShapeDepth() * 0.05f,
			CabRoom()->Shape->GetShapeHeight() * 0.1f + FArmyWHCabinetMgr::Get()->GetTableHeight(true));
	}
	else if (CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_BathroomBasin))
	{
		Accessory->CabAccInfo->RelativeLocation.Set(0.0f, CabRoom()->Shape->GetShapeDepth() * 0.05f,
			CabRoom()->Shape->GetShapeHeight() * 0.1f);
	}
	else if (CabRoom()->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
	{
		Accessory->CabAccInfo->RelativeLocation.Set(0.0f, 0.0f, 0.0f);
	}

	if (Accessory->Actor != nullptr)
	{
		Accessory->Actor->SetActorRelativeLocation(Accessory->CabAccInfo->RelativeLocation);
		Accessory->UpdateComponent();
	}
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////


XRWHCabinetRulerComponent::XRWHCabinetRulerComponent() {
	ComponentClassName = XRWHCabinetRulerComponent::typeName();
}

void XRWHCabinetRulerComponent::Destory() {
	if (ShapeWidthRuler.IsValid())
	{
		ShapeWidthRuler->GetRulerActor()->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		ShapeWidthRuler.Reset();
	}
}

void XRWHCabinetRulerComponent::SpawnShapeWidthRuler() {
	float ShapeWidth = CabRoom()->Shape->GetShapeWidth() * 0.1f;
	float ShapeHeight = CabRoom()->Shape->GetShapeHeight() * 0.1f;

	ShapeWidthRuler = MakeShareable(new FShapeRuler(XRWHCMode::ERulerAlignDirection::RAD_X, GVC->ViewportOverlayWidget));
	ShapeWidthRuler->SetRulerAlignment(XRWHCMode::ERulerAlignment::RA_Bottom);
	ShapeWidthRuler->SetRulerLength(ShapeWidth);
	ShapeWidthRuler->SetRulerValueIsReadOnly(true);
	AShapeRulerActor *RulerActor = ShapeWidthRuler->GetRulerActor();
	RulerActor->SetActorRelativeLocation(FVector(-ShapeWidth * 0.5f, 0.0f, ShapeHeight));
	RulerActor->AttachToActor(CabRoom()->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
	RefreshShapeWidthRuler();
}

void XRWHCabinetRulerComponent::RefreshShapeWidthRuler()
{
	if (ShapeWidthRuler.IsValid())
	{
		FArmyWHCRect rect = CabRoom()->GetRect();
		ShapeWidthRuler->SetRulerLength(rect.mSizeHalf.X * 2);
		AShapeRulerActor *RulerActor = ShapeWidthRuler->GetRulerActor();
		RulerActor->SetActorRelativeLocation(FVector(-rect.mSizeHalf.X + (CabRoom()->GetSunBoardWidth(1) - 
			CabRoom()->GetSunBoardWidth(0)) * 0.5f, 0.0f, rect.SizezHalf()*2.0f));
	}
}

XRWHCabinetLegsComponent::XRWHCabinetLegsComponent() {
	ComponentClassName = XRWHCabinetLegsComponent::typeName();
}

void XRWHCabinetLegsComponent::Destory() {
	for (auto &Leg : ShapeLegs)
		Leg->Destroy();
	ShapeLegs.Empty();
}

void XRWHCabinetLegsComponent::SpawnObject() {
}

void XRWHCabinetLegsComponent::SpawnShapeLegs() {

	int32 SynID = FArmyWHCabinetMgr::Get()->GetMesh(TEXT("WHC/MATERIAL_1542592442974_3994.pak"));
	int32 ShapeCategory = CabRoom()->Shape->GetShapeCategory();
	// 只有橱柜地柜和橱柜高柜会生成调整脚
	if (SynID != -1 && (ShapeCategory == (int32)ECabinetType::EType_OnGroundCab ||
		ShapeCategory == (int32)ECabinetType::EType_TallCab))
	{
		USCTXRResourceManager & ResMgr = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager();
		FSCTVRSObject *Object = ResMgr.GetObjFromObjID(SynID);
		if (Object != nullptr)
		{
			float ShapeWidth = CabRoom()->Shape->GetShapeWidth() * 0.1f;
			float ShapeDepth = CabRoom()->Shape->GetShapeDepth() * 0.1f;

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
			float fScale = 0.5f*CabRoom()->AboveGround / LegMeshExtent.Z;
			UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
			for (int32 i = 0; i < Outline.Num(); ++i)
			{
				FVector LastDir = Outline[i] - Outline[XRWHCMode::RoundIndex(i, -1, Outline.Num())];
				LastDir.Normalize();
				FVector LastRight = FVector::CrossProduct(FVector::UpVector, LastDir);

				FVector CurrentDir = Outline[XRWHCMode::RoundIndex(i, 1, Outline.Num())] - Outline[i];
				CurrentDir.Normalize();
				FVector CurrentRight = FVector::CrossProduct(FVector::UpVector, CurrentDir);

				AStaticMeshActor *Leg = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
				Leg->bIsSelectable = 0;
				Leg->Tags.Emplace(XRActorTag::Immovable);
				Leg->Tags.Emplace(XRActorTag::WHCActor);
				UStaticMeshComponent *SMComp = Leg->GetStaticMeshComponent();
				SMComp->SetMobility(EComponentMobility::Movable);
				SMComp->SetStaticMesh(LegMesh);
				for (const auto &IterRef : Object->MaterialList)
					SMComp->SetMaterialByName(FName(*IterRef.SlotName), IterRef.DIM);
				Leg->AttachToActor(CabRoom()->ShapeFrame, FAttachmentTransformRules::KeepRelativeTransform);
				Leg->SetActorRelativeLocation(Outline[i] + LastRight * EdgeOffset + CurrentRight * EdgeOffset + Offset - FVector(0.0f, 0.0f, CabRoom()->AboveGround));
				Leg->SetActorRelativeScale3D(FVector(1.0f, 1.0f, fScale));
				ShapeLegs.Emplace(Leg);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
/* <summary>
/// PlatformInfo组件类
/// </summary>
*/
//////////////////////////////////////////////////////////////////////////
XRWHCabinetPlatformInfoComponent::XRWHCabinetPlatformInfoComponent(){
	ComponentClassName = XRWHCabinetPlatformInfoComponent::typeName();
}

void XRWHCabinetPlatformInfoComponent::Destory() {

}

void XRWHCabinetPlatformInfoComponent::SpawnObject(const TSharedPtr<FPlatformInfo> &InPlatformInfo) {
	PlatformInfo = InPlatformInfo;
	IsValidComponent = true;
}

void XRWHCabinetPlatformInfoComponent::SpawnPlatform() {
	if (!IsValidComponent)
		return;
	if (!CabRoom())
		return;
	if (PlatformInfo.IsValid() &&
		PlatformInfo->FrontSections.Num() > 0 &&
		PlatformInfo->BackSections.Num() > 0)
	{
		CabRoom()->DestroyPlatform();

		const TSharedPtr<FArmyWHCPoly> * PolyPtr = FArmyWHCabinetMgr::Get()->GetRooms().Find(CabRoom()->RoomAttachedIndex);
		if (PolyPtr != nullptr)
		{
			FArmySplineResMgr &SplineResMgr = FArmySplineResMgr::Instance();
			FArmySplineRes * SplineRes = SplineResMgr.GetSplineRes(PlatformInfo->FrontSections[0]->GetCachedPath());

			FArmyWHCCookAssist AssistGen;
			AssistGen.PrepareGenTable(true, (*PolyPtr).Get(), TArray<FShapeInRoom*>{CabRoom()}, CabRoom()->GetRect().mMaxZ * 10.0f, true);
			AssistGen.GenTableMesh(
				SplineRes != nullptr ? (SplineRes->mSize.X * 10.0f - 20.0f) : 0.0f,
				SplineRes != nullptr ? SplineRes->mSize.X : 0.0f,
				SplineRes != nullptr ? SplineRes->mSize.Y : 0.0f,
				FArmyWHCCookAssist::FGenTable::CreateLambda(
					[this](const TArray<FPUVVertex>& Vertices, const TArray<uint16>& Indices) {
				UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
				CabRoom()->Platform = World->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
				CabRoom()->Platform->Tags.Emplace(XRActorTag::Immovable);
				CabRoom()->Platform->Tags.AddUnique(XRActorTag::WHCTable);
				CabRoom()->Platform->SetFolderPath(FArmyActorPath::GetWHCTablePath());
				CabRoom()->Platform->SetIsSingleTable(true);
				CabRoom()->Platform->RefreshMesh(Vertices, Indices);
				if (PlatformInfo->PlatformMtl.IsValid())
				{
					if (PlatformInfo->PlatformMtl->Mtl == nullptr)
					{
						PlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(PlatformInfo->PlatformMtl->GetCachedPath(), PlatformInfo->PlatformMtl->MtlParam);
						if (PlatformInfo->PlatformMtl->Mtl != nullptr)
							PlatformInfo->PlatformMtl->RcState = ECabinetResourceState::CabRc_Complete;
					}
					CabRoom()->Platform->SetMaterial(PlatformInfo->PlatformMtl->Mtl);
				}
			}
				),
				FArmyWHCCookAssist::FGenTrim::CreateLambda(
					[this](int32 TrimType, const TArray<FVector2D>& Vers, bool bIsClose, float PlaceHigh) {
				AXRShapeTableActor* pActor = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
				TArray<FPUVVertex> FuVers; TArray<uint16> Faces;
				FString TrimName;
				switch (TrimType)
				{
				case 1: // 前挡
				{
					TrimName = PlatformInfo->FrontSections[0]->GetCachedPath();
					CabRoom()->PlatformFrontSections.Emplace(pActor);
					break;
				}
				case 2: // 后挡
				{
					TrimName = PlatformInfo->BackSections[0]->GetCachedPath();
					CabRoom()->PlatformBackSections.Emplace(pActor);
					break;
				}
				}
				FArmySplineRes* SplineRes = FArmySplineResMgr::Instance().CreateSpline(TrimName, const_cast<TArray<FVector2D>&>(Vers), bIsClose, FuVers, Faces);
				if (SplineRes == nullptr)
					return;
				pActor->Tags.Emplace(XRActorTag::Immovable);
				pActor->SetIsSingleTable(true);
				pActor->RefreshMesh(FuVers, Faces);
				pActor->SetActorLocation(FVector(0, 0, PlaceHigh));

				pActor->Tags.AddUnique(XRActorTag::WHCTable);
				pActor->SetFolderPath(FArmyActorPath::GetWHCTablePath());
				if (PlatformInfo->PlatformMtl.IsValid())
				{
					if (PlatformInfo->PlatformMtl->Mtl == nullptr)
					{
						PlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(PlatformInfo->PlatformMtl->GetCachedPath(), PlatformInfo->PlatformMtl->MtlParam);
						if (PlatformInfo->PlatformMtl->Mtl != nullptr)
							PlatformInfo->PlatformMtl->RcState = ECabinetResourceState::CabRc_Complete;
					}
					pActor->SetMaterial(PlatformInfo->PlatformMtl->Mtl);
				}

			}
				),
				FArmyWHCCookAssist::FCollectTrimOutline()
				);
		}
	}
}


//////////////////////////////////////////////////////////////////////////
/* <summary>
/// PlatformInfo组件类
/// </summary>
*/
//////////////////////////////////////////////////////////////////////////
XRWHCabinetFurnitureComponent::XRWHCabinetFurnitureComponent() {
	ComponentClassName = XRWHCabinetFurnitureComponent::typeName();
}

void XRWHCabinetFurnitureComponent::InitializeObject(FCabinetComponent *InCabComp) {
	// 初始化柜子构件
	if (InCabComp != nullptr)
		CabRoom()->MakeComponent(ShapeComponent, InCabComp->GetCachePath(1), OT_ComponentBase, InCabComp->Type);
}

void XRWHCabinetFurnitureComponent::SpawnObject() {
	if (!IsValidComponent)
		return;
}

void XRWHCabinetFurnitureComponent::Destory() {
	if (ShapeComponent.IsValid())
	{
		FArmySceneData::Get()->Delete(ShapeComponent);
		ShapeComponent.Reset();
	}
}

void XRWHCabinetFurnitureComponent::UpdateComponents() {
	if (ShapeComponent.IsValid())
	{
		FVector Bound = ShapeComponent->GetPreBounds().GetSize();
		FRotator Rotation = CabRoom()->ShapeFrame->GetActorRotation();
		FVector Right = Rotation.RotateVector(FVector::RightVector);
		float ShapeDepth = CabRoom()->Shape->GetShapeDepth() * 0.1f;
		ShapeComponent->SetTransform(
			FTransform(
				Rotation,
				CabRoom()->ShapeFrame->GetActorLocation() + Right * ShapeDepth * 0.5f,
				FVector(FMath::IsNearlyZero(Bound.X) ? 1.0f : CabRoom()->Shape->GetShapeWidth() * 0.1f / Bound.X, FMath::IsNearlyZero(Bound.Y) ? 1.0f : ShapeDepth / Bound.Y, 1.0f)
			)
		);
	}
}


//////////////////////////////////////////////////////////////////////////
/* <summary>
/// Frontsections组件类
/// </summary>
*/
//////////////////////////////////////////////////////////////////////////
XRWHCabinetFrontSectionsComponent::XRWHCabinetFrontSectionsComponent() {
	ComponentClassName = XRWHCabinetFrontSectionsComponent::typeName();
}

void XRWHCabinetFrontSectionsComponent::InitializeObject() {

}

void XRWHCabinetFrontSectionsComponent::SpawnPlatform() {
	if (!IsValidComponent)
		return;
}


//////////////////////////////////////////////////////////////////////////
/* <summary>
/// Backsections组件类
/// </summary>
*/
//////////////////////////////////////////////////////////////////////////
XRWHCabinetBackSectionsComponent::XRWHCabinetBackSectionsComponent() {
	ComponentClassName = XRWHCabinetBackSectionsComponent::typeName();
}

void XRWHCabinetBackSectionsComponent::InitializeObject(){

}

void XRWHCabinetBackSectionsComponent::SpawnPlatform() {
	if (!IsValidComponent)
		return;
}


XRCabinetActor::XRCabinetActor()
	: location(-1)
	, signature(0) {
}

XRCabinetActor::XRCabinetActor(TSharedPtr<XRWHCabinetIComponent> tpComponent)
	: location(-1)
	, signature(0)
{
	RepetBinding(tpComponent);
}

XRCabinetActor& XRCabinetActor::operator=(TSharedPtr<XRWHCabinetIComponent> tpComponent) {
	RepetBinding(tpComponent);
	return *this;
}

/*/ <summary>
/// 初始化所有Cabinet需求组件
/// </summary>
*/
void XRCabinetActor::InitCabinetComponents(TSharedPtr<FShapeInRoom> sharePtrRoom) {

}

TSharedPtr<XRWHCabinetIComponent> XRCabinetActor::Value() const {
	return GetValue(gCabinMgr.CabinetComponentsPool());
}

inline
TSharedPtr<XRWHCabinetIComponent> XRCabinetActor::GetValue(CabComponentsPool& pool) const {
	if (location != -1) {
		CabComponentsPool::Entry& entry = pool.Get(location);
		if (signature == entry.signature) {
			return entry.tpComponent;
		}
	}
	return nullptr;
}

void XRCabinetActor::Create(TSharedPtr<XRWHCabinetIComponent> tpComponent) {
	XRWHTransform* trans = Transform();
	if (trans) {
		XRCabinetActor entity = tpComponent;
		trans->ArrComponentList.Push(entity);
		tpComponent->cabTransform = trans->Self();
		trans->HashCodeTrans = HashCode();
	}
}

void XRCabinetActor::CreateTransform() {
	TSharedPtr<XRWHTransform> CabinetIComponent = MakeShareable(new XRWHTransform());
	XRCabinetActor entity = CabinetIComponent;
	CabinetIComponent->HashCodeTrans = HashCode();
	location = entity.location;
	signature = entity.signature;
}

void XRCabinetActor::RepetBinding(TSharedPtr<XRWHCabinetIComponent> tpComponent) {
	if (Value() != tpComponent) {
		if (tpComponent.IsValid()) {
			CabComponentsPool::Location newLocation = { 0 };
			newLocation = gCabinMgr.CabinetComponentsPool().AddComponent(tpComponent);
			location = newLocation.value;
			signature = newLocation.signatureCapture;
			tpComponent->self = *this;
		}
		else {
			location = -1;
			signature = 0;
		}
	}
}

bool XRCabinetActor::GenCabinetComponents(eCabinetComponentType eccType, FShapeInRoom* sharePtrRoom) {
	XRWHCabinetIComponent* CabinetIComponent = nullptr;
	bool beInvalidComponent = false;
	switch (eccType) {
#define CASE_TYPE(TYPE,OCOBJ)\
	case eCabinetComponentType::CCT_##TYPE:\
        {\
			if(nullptr ==CabinetIComponent)\
			{\
				beInvalidComponent  =  CheckEnableCreateComponent<typename XRWHCabinet##OCOBJ##Component>();\
				if (beInvalidComponent)\
				{\
					CabinetIComponent = new XRWHCabinet##OCOBJ##Component(); \
					CabinetIComponent->AddFromActor(sharePtrRoom);\
					Create(MakeShareable(CabinetIComponent));\
				}\
			}\
		}\
		break;\

			CASE_TYPE(ACCESSORY, Accessory)
			CASE_TYPE(RULER, Ruler)
			CASE_TYPE(PLATFORM, PlatformInfo)
			CASE_TYPE(LEGS, Legs)
			CASE_TYPE(FArmyFURNITURE, Furniture)
			

#undef  CASE_TYPE
	default:
		if (nullptr != CabinetIComponent) {
			CabinetIComponent = nullptr;
		}
		break;
	}
	return true;
}

void XRCabinetActor::RemoveAt(int64 hashCodes) {
	XRWHTransform* trans = Transform();
	if (trans) {
		auto IndexFun = [&hashCodes](TArray<XRCabinetActor>& _arr)-> int {
			int32 index = 0;
			for (const XRCabinetActor& val : _arr)
			{
				if (val.HashCode() == hashCodes) {
					return index;
				}
				index++;
			}
			return -1;
		};

		int32 index = IndexFun(trans->ArrComponentList);
		if (trans->ArrComponentList.IsValidIndex(index)) {
			XRCabinetActor& refActor = trans->ArrComponentList[index];
			if (refActor.LocationID() != -1) {
				//TSharedPtr<XRWHCabinetIComponent> tpComponent = 
				gCabinMgr.CabinetComponentsPool().RemoveComponent(refActor.LocationID(),refActor.Signature());
				/*if (tpComponent.IsValid()) {
					tpComponent = nullptr;
				}*/
			}
			trans->ArrComponentList.RemoveAt(index);
		}
	}
}

void XRCabinetActor::Remove() {
	XRWHTransform* trans = Transform();
	if(trans)
	{
		for (const XRCabinetActor& val : trans->ArrComponentList) {
			if (val.LocationID() != -1) {
				gCabinMgr.CabinetComponentsPool().RemoveComponent(val.LocationID(), val.Signature());
			}
		}
		trans->ArrComponentList.Empty();
		//delete transform
		gCabinMgr.CabinetComponentsPool().RemoveComponent(trans->Self().LocationID(), trans->Self().Signature());
	}
}


bool XRCabinetActor::Have(int64 hashName) {

	auto iComponent = Find(hashName);
	if (iComponent.IsValid())
		return true;

	return false;
}

XRWHTransform* XRCabinetActor::Transform() {
	TSharedPtr<XRWHCabinetIComponent> tpComponent = Value();
	if (tpComponent.IsValid() && 
		tpComponent->ClassHashName() == tpComponent->Self().HashClassType<XRWHTransform>()) {
		return (dynamic_cast<XRWHTransform*>(tpComponent.Get()));
	}
	return nullptr;
}
template <class _TComponent> inline
bool XRCabinetActor::CheckEnableCreateComponent() {
	TSharedPtr<XRWHCabinetIComponent> tpComponent = Find(HashClassType<_TComponent>());
	if (tpComponent.IsValid())
		return false;
	
	return true;
}

template <class _TComponent> inline
TSharedPtr<XRWHCabinetIComponent> XRCabinetActor::Component() {
	TSharedPtr<XRWHCabinetIComponent> tpComponent = Value();
	if (tpComponent.IsValid()) {
		if (IsComponent<XRWHTransform>()) {
			auto trans = dynamic_cast<XRWHTransform*>(tpComponent.Get());
			if (trans->ArrComponentList.Num() <= 0) {
				return nullptr;
			}
			return Find(HashClassType<_TComponent>());
		}
		else {
			return tpComponent;
		}
	}
	return nullptr;
}

template <class _TComponent> inline
bool XRCabinetActor::IsComponent()  {
	TSharedPtr<XRWHCabinetIComponent> tpComponent = this->Component<_TComponent>();
	if (tpComponent.IsValid() && tpComponent->ClassHashName() == HashClassType<_TComponent>()) {
		return  true;
	}
	return false;
}

template <> inline
bool XRCabinetActor::IsComponent<XRWHTransform>() {
	return signature < 0;
}

template <class _TComponent>
 _TComponent* XRCabinetActor::AsComponent() {
	TSharedPtr<XRWHCabinetIComponent> c = this->Component<_TComponent>();
	if(c.IsValid())
		return dynamic_cast<_TComponent*>(c.Get());

	return dynamic_cast<_TComponent*>(this->Transform());
}

template <class _TComponent>
_TComponent* XRCabinetActor::AsComponent(int index) {

	return nullptr;
}

template <class _TComponent>
int64 XRCabinetActor::HashClassType() {
	//_TComponent classType;
	return GetTypeHash(_TComponent::typeName());
}

int64 XRCabinetActor::HashCode() const {
	return (int)(((int64)signature << 32) | (int64)location);
}


TSharedPtr<XRWHCabinetIComponent> XRCabinetActor::Find(uint64 hashValue) {
	XRWHTransform* trans = Transform();
	if (trans) {
		using TPTYPE = TSharedPtr<XRWHCabinetIComponent>;
		auto findComponent = [&hashValue](TArray<XRCabinetActor>& _arr)-> TPTYPE {
			bool result = false;
			for (const XRCabinetActor& val : _arr)
			{
				if (val.Value().IsValid())
				{
					if (val.Value()->ClassHashName() == hashValue) {
						result = true;
						return val.Value();
					}
				}
			}
			if (!result)
				GGI->Window->ShowMessage(MT_Warning, TEXT("查找組件失敗！"));
			return nullptr;
		};
		auto tpComponent = findComponent(trans->ArrComponentList);

		if (tpComponent.IsValid()) {
			return tpComponent;
		}
	}
	return nullptr;
}

template <class _TComponent>
XRCabinetActorT<_TComponent>::XRCabinetActorT() : XRCabinetActor() {
}

template <class _TComponent>
XRCabinetActorT<_TComponent>::XRCabinetActorT(TSharedPtr<XRCabinetActor> actor) : XRCabinetActor(*actor.Get()) {
}

template <class _TComponent>
 _TComponent* XRCabinetActorT<_TComponent>::GetComponent() {
	return XRCabinetActor::AsComponent<_TComponent>();
}

template <class _TComponent>
 _TComponent* XRCabinetActorT<_TComponent>::operator->() {
	return GetComponent();
}

