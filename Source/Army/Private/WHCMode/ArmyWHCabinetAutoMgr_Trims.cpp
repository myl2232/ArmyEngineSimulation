#include "ArmyWHCabinetAutoMgr.h"
#include "Data/XRWHCTableObject.h"
#include "common/XRSpline.h"
#include "Actor/XRActorConstant.h"
#include "ArmyActor/XRActor.h"
#include "Model/XRHomeData.h"
#include "Engine/Engine.h"
#include "Engine/CollisionProfile.h"
#include "WHCMode/common/XRPolygonAssist.h"

void FArmyWHCabinetMgr::RefreshTableMesh(const TArray<FPUVVertex>& Vertices, const TArray<uint16>& Indices)
{
	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	AXRShapeTableActor* pTableActor = World->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
	pTableActor->Tags.Emplace(XRActorTag::Immovable);
	pTableActor->Tags.AddUnique(XRActorTag::WHCTable);
    pTableActor->SetFolderPath(FArmyActorPath::GetWHCTablePath());
	pTableActor->RefreshMesh(Vertices, Indices);
	pTableActor->SetMaterial(GetTableMtl());

	

	// AXRActor * TableOwner = World->SpawnActor<AXRActor>(AXRActor::StaticClass());
	// TableOwner->SetRelevanceObject(TableObject);
	// pTableActor->SetOwner(TableOwner);

	mTableActors.Add(pTableActor);
}

void FArmyWHCabinetMgr::CollectTableTrimOutline(const TArray<FArmyWHCTableObject::FWHCTableBatchData>& InBatchDataArray)
{
	if (InBatchDataArray.Num() == 0) return;
	TSharedPtr<FArmyWHCTableObject> TableObject = MakeShareable(new FArmyWHCTableObject);

	TArray<TWeakPtr<FArmyFurniture>> ComponentArray;
	for (auto Ptr : GetPlaceShapes())
	{
		if (!Ptr->CabinetActor.IsValid())
			return;
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = Ptr->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid())
		{
			ComponentArray.Add(Accessory->Component);

			switch (EMetalsType(Accessory->CabAccInfo->Type))
			{
			case EMetalsType::MT_WATER_CHANNE:
				TableObject->TableInfo.SinkTypeArray.Add(Accessory->CabAccInfo->Name);
				break;
			case EMetalsType::MT_KITCHEN_RANGE:
				TableObject->TableInfo.StoveTypeArray.Add(Accessory->CabAccInfo->Name);
				break;
			default:
				break;
			}
		}
	}

	
	TableObject->SetTableOutline(InBatchDataArray, GetTableSplineWidth() * 10.0f, ComponentArray);
	TableObject->SetType(OT_CupboardTable);
	if (FArmyWHCabinetMgr::Get()->GetPlatformInfo())
	{
		TableObject->TableInfo.BoardMaterial = FArmyWHCabinetMgr::Get()->GetPlatformInfo()->PlatformMtl->MtlName;
		if (FArmyWHCabinetMgr::Get()->GetPlatformInfo()->FrontSections.Num() >0)
		{
			TableObject->TableInfo.FrontEdgeName = FArmyWHCabinetMgr::Get()->GetPlatformInfo()->FrontSections[0]->Name;
			TableObject->TableInfo.FrontEdgeHeight = FArmyWHCabinetMgr::Get()->GetPlatformInfo()->FrontSections[0]->CurrentHeight;
		}
		if (FArmyWHCabinetMgr::Get()->GetPlatformInfo()->BackSections.Num() >0)
		{
			TableObject->TableInfo.BackEdgeName = FArmyWHCabinetMgr::Get()->GetPlatformInfo()->BackSections[0]->Name;
			TableObject->TableInfo.BackEdgeHeight = FArmyWHCabinetMgr::Get()->GetPlatformInfo()->BackSections[0]->CurrentHeight;
		}
	}
	
	FArmySceneData::Get()->Add(TableObject, XRArgument(1).ArgUint32(E_WHCModel));

	TableObjects.Emplace(TableObject);
}
void FArmyWHCabinetMgr::GetGapActors(int32 InType/*-1: all 0: floor 1: hang*/, TArray<AXRWhcGapActor*>& OutArray)
{
	switch (InType)
	{
	case -1:
	{
		OutArray.Append(mGapFloors);
		OutArray.Append(mGapHangs);
	}
	break;
	case 0:
		OutArray.Append(mGapFloors);
		break;
	case 1:
		OutArray.Append(mGapHangs);
		break;
	default:
		break;
	}
}
void FArmyWHCabinetMgr::GetTrimPath(int32 InType, TArray<FArmyWHCabinetMgr::FTrimInfo>& OutPaths) const
{
	TrimPathMap.MultiFind(ETrimType(InType),OutPaths);
}
float FArmyWHCabinetMgr::GenGapActor(int nType, FVector2D StartPos, FVector2D EndPos, float fBottom, float fTop, UMaterialInstanceDynamic* pMtl)
{
    CHECK_ERROR(mGapMesh);

	AXRWhcGapActor* pActor = GenGapActor(StartPos, EndPos, (fTop-fBottom), fBottom, pMtl);

	pActor->Tags.Emplace(TEXT("Immovable"));
	if (nType == 0)
	{
		pActor->Tags.AddUnique("FloorGap");
		mGapFloors.Add(pActor);
	}
	else
	{
		pActor->Tags.AddUnique("HangGap");
		mGapHangs.Add(pActor);
	}
	return -pActor->GetActorScale3D().Y;
}

void FArmyWHCabinetMgr::GenGapActor(const TArray<FPUVVertex>& Vers, const TArray<uint16>& Faces, UMaterialInstanceDynamic* pMtl)
{
	AXRShapeTableActor* pGapActor = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
	pGapActor->Tags.Emplace(XRActorTag::Immovable);
	pGapActor->RefreshMesh(Vers, Faces);
	pGapActor->SetMaterial(pMtl);
	Cast<UMeshComponent>(pGapActor->GetRootComponent())->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	mGapActors.Add(pGapActor);
}

void FArmyWHCabinetMgr::GenTrimActor(int32 TrimType, const TArray<FVector2D>& Vers, bool bIsClose, float PlaceHigh)
{
	TArray<FPUVVertex> FuVers; TArray<uint16> Faces;
	FArmySplineRes* pSplineRes = FArmySplineResMgr::Instance().CreateSpline(mTrimNames[TrimType], const_cast<TArray<FVector2D>&>(Vers), bIsClose, FuVers, Faces);
	if (pSplineRes == nullptr)
		return;
	
	AXRShapeTableActor* pActor = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World()->SpawnActor<AXRShapeTableActor>(AXRShapeTableActor::StaticClass());
	pActor->Tags.Emplace(XRActorTag::Immovable);
	pActor->RefreshMesh(FuVers, Faces);
	pActor->SetActorLocation(FVector(0, 0, PlaceHigh));

	if (TrimType == ETrim_Toe)
	{
		float fScaleZ = mFloorShapeHight / pSplineRes->mSize.Y;
		pActor->SetActorScale3D(FVector(1, 1, fScaleZ));
		pActor->Tags.AddUnique(XRActorTag::WHCFloor);
        pActor->SetFolderPath(FArmyActorPath::GetWHCFloorPath());
		pActor->SetMaterial(GetToeMtl());
	}
	else if (TrimType == ETrim_Top)
	{
		pActor->Tags.AddUnique(XRActorTag::WHCHang);
        pActor->SetFolderPath(FArmyActorPath::GetWHCHangPath());
		pActor->SetMaterial(GetHangOrHighDoorMtl());
	}
	else
	{
		pActor->Tags.AddUnique(XRActorTag::WHCTable);
        pActor->SetFolderPath(FArmyActorPath::GetWHCTablePath());
		pActor->SetMaterial(GetTableMtl());
	}
	TrimPathMap.Add(ETrimType(TrimType), FTrimInfo(pActor,Vers));
	mTrimActors[TrimType].Add(pActor);
}
const TArray<AXRShapeTableActor*>& FArmyWHCabinetMgr::GetTrimActors(int32 InType) const
{
	return mTrimActors[InType];
}
void FArmyWHCabinetMgr::SetPlatformInfo(FPlatformInfo* InPlatformInfo)
{
	mPlatformId = InPlatformInfo->Id;
}

void FArmyWHCabinetMgr::ResetPlatformInfo()
{
	mPlatformId = mFrontTrimId = mBackTrimId = 0;
    mTrimNames[ETrim_PlatformFront] = GDefaultFrontDxf;
    mTrimNames[ETrim_PlatformBack] = GDefaultBackDxf;
}

void FArmyWHCabinetMgr::SetFrontTrim(FTableInfo* pTableInfo)
{
	mFrontTrimId = pTableInfo->Id;
	mTrimNames[ETrim_PlatformFront] = pTableInfo->GetCachedPath();
}

void FArmyWHCabinetMgr::ResetFrontTrim()
{
	mFrontTrimId = 0;
	mTrimNames[ETrim_PlatformFront] = GDefaultFrontDxf;	
}

void FArmyWHCabinetMgr::SetTopTrim(FTableInfo* pTableInfo)
{
	mTopTrimId = pTableInfo->Id;
	mTrimNames[ETrim_Top] = pTableInfo->GetCachedPath();
}

void FArmyWHCabinetMgr::ResetTopTrim()
{
    mTopTrimId = mTopMtlId = 0;
    mTrimNames[ETrim_Top] = GDefaultTopDxf;
}

void FArmyWHCabinetMgr::SetToeTrim(FTableInfo* pTableInfo)
{
	mToeTrimId = pTableInfo->Id;
	mTrimNames[ETrim_Toe] = pTableInfo->GetCachedPath();
}

void FArmyWHCabinetMgr::ResetToeTrim()
{
	mToeTrimId = mToeMtlId = 0;
    mTrimNames[ETrim_Toe] = GDefaultToeDxf;
}

void FArmyWHCabinetMgr::RefreshPlatformMtl()
{
	UMaterialInterface *Mtl = GetTableMtl();
	for (auto & TableActor: mTableActors)
		TableActor->SetMaterial(Mtl);
	for (int32 i = 0; i < mTrimActors[ETrim_PlatformFront].Num(); ++i)
		mTrimActors[ETrim_PlatformFront][i]->SetMaterial(Mtl);
	for (int32 i = 0; i < mTrimActors[ETrim_PlatformBack].Num(); ++i)
		mTrimActors[ETrim_PlatformBack][i]->SetMaterial(Mtl);
}

void FArmyWHCabinetMgr::SetTopTrimMtl(FMtlInfo* InTopInfo)
{
	mTopMtlId = InTopInfo->Id;
	for (int32 i = 0; i < mTrimActors[ETrim_Top].Num(); ++i)
		mTrimActors[ETrim_Top][i]->SetMaterial(InTopInfo->Mtl);
}

void FArmyWHCabinetMgr::SetToeTrimMtl(FMtlInfo* InToeInfo)
{
	mToeMtlId = InToeInfo->Id;
	for (int32 i = 0; i < mTrimActors[ETrim_Toe].Num(); ++i)
		mTrimActors[ETrim_Toe][i]->SetMaterial(InToeInfo->Mtl);
}

FPlatformInfo* FArmyWHCabinetMgr::GetPlatformInfo() const
{
	const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(mPlatformId);
	return PlatformInfoPtr ? (*PlatformInfoPtr).Get() : nullptr;
}

FTableInfo* FArmyWHCabinetMgr::GetFrontInfo() const
{
	const TSharedPtr<FPlatformInfo>* pPlatformInfo = FWHCModeGlobalData::PlatformMap.Find(mPlatformId);
	if (pPlatformInfo)
	{
		const TSharedPtr<FTableInfo> * TableInfo = (*pPlatformInfo)->FrontSections.FindByPredicate(
			[this](const TSharedPtr<FTableInfo> & TableInfo) { return mFrontTrimId == TableInfo->Id; }
		);
		if (TableInfo)
			return (*TableInfo).Get();
	}
	return nullptr;
}

FTableInfo* FArmyWHCabinetMgr::GetBackInfo() const
{
	return nullptr;
}

FTableInfo* FArmyWHCabinetMgr::GetTopInfo() const
{
	const TSharedPtr<FTableInfo> * TableInfoPtr = FWHCModeGlobalData::TopSections.Find(mTopTrimId);
	if (TableInfoPtr != nullptr)
	{
		if (mTopMtlId != 0) // 材质ID不为0代表进行过材质更换
			(*TableInfoPtr).Get()->mtlId = mTopMtlId;
		return (*TableInfoPtr).Get();
	}
	else
		return nullptr;
}

FTableInfo* FArmyWHCabinetMgr::GetToeInfo() const
{
	const TSharedPtr<FTableInfo> * TableInfoPtr = FWHCModeGlobalData::ToeSections.Find(mToeTrimId);
	if (TableInfoPtr != nullptr)
	{
		if (mToeMtlId != 0) // 材质ID不为0代表进行过材质更换
			(*TableInfoPtr).Get()->mtlId = mToeMtlId;
		return (*TableInfoPtr).Get();
	}
	else
		return nullptr;
}

void FArmyWHCabinetMgr::GenTable(bool bVisble)
{
	if (bVisble == IsGenTable())
		return;
	SetGenTable(bVisble);
	GenTableBoard();
}

void FArmyWHCabinetMgr::GenFloorTrim(bool bVisble)
{
	if (bVisble == IsGenToe())
		return;
	SetGenToe(bVisble);
	GenTableBoard();
}

void FArmyWHCabinetMgr::GenHangTrim(bool bVisble)
{
	if (bVisble == IsGenTop())
		return;
	SetGenTop(bVisble);
	GenHangGapBoard();
}

void FArmyWHCabinetMgr::GenFloorGap(bool bVisble)
{
	if (bVisble == IsGenToeGap())
		return;
	SetGenToeGap(bVisble);
	GenTableBoard();
}

void FArmyWHCabinetMgr::GenHangGap(bool bVisble)
{
	if (bVisble == IsGenTopGap())
		return;
	SetGenTopGap(bVisble);
	GenHangGapBoard();
}

void FArmyWHCabinetMgr::SetTableVisible(bool bVisible)
{
	for (auto &TableActor : mTableActors)
	{
		TableActor->SetActorHiddenInGame(!bVisible);
	}
}

void FArmyWHCabinetMgr::SetTrimVisible(ETrimType TrimType, bool bVisible)
{
	for (auto &TrimActor : mTrimActors[TrimType])
	{
		TrimActor->SetActorHiddenInGame(!bVisible);
	}
}

void FArmyWHCabinetMgr::SetFloorGapVisible(bool bVisible)
{
	for (auto &FloorGapActor : mGapFloors)
	{
		FloorGapActor->SetActorHiddenInGame(!bVisible);
	}
}

void FArmyWHCabinetMgr::SetHangGapVisible(bool bVisible)
{
	for (auto &HangGapActor : mGapHangs)
	{
		HangGapActor->SetActorHiddenInGame(!bVisible);
	}
}

void FArmyWHCabinetMgr::SetGenTable(bool bGenTable)
{
	if (bGenTable)
		GenFlag |= WHC_GEN_TABLE_FLAG;
	else
		GenFlag &= ~WHC_GEN_TABLE_FLAG;
}

bool FArmyWHCabinetMgr::IsGenTable() const
{
	return (GenFlag & WHC_GEN_TABLE_FLAG) != 0;
}

void FArmyWHCabinetMgr::SetGenTop(bool bGenTop)
{
	if (bGenTop)
		GenFlag |= WHC_GEN_TOP_FLAG;
	else
		GenFlag &= ~WHC_GEN_TOP_FLAG;
}

bool FArmyWHCabinetMgr::IsGenTop() const
{
	return (GenFlag & WHC_GEN_TOP_FLAG) != 0;
}

void FArmyWHCabinetMgr::SetGenToe(bool bGenToe)
{
	if (bGenToe)
		GenFlag |= WHC_GEN_TOE_FLAG;
	else
		GenFlag &= ~WHC_GEN_TOE_FLAG;
}

bool FArmyWHCabinetMgr::IsGenToe() const
{
	return (GenFlag & WHC_GEN_TOE_FLAG) != 0;
}

void FArmyWHCabinetMgr::SetGenWaist(bool bGenWaist)
{
	if (bGenWaist)
		GenFlag |= WHC_GEN_WAIST_FLAG;
	else
		GenFlag &= ~WHC_GEN_WAIST_FLAG;
}

bool FArmyWHCabinetMgr::IsGenWaist() const
{
	return (GenFlag & WHC_GEN_WAIST_FLAG) != 0;
}

void FArmyWHCabinetMgr::SetGenToeGap(bool bGenToeGap)
{
	if (bGenToeGap)
		GenFlag |= WHC_GEN_TOE_GAP_FLAG;
	else
		GenFlag &= ~WHC_GEN_TOE_GAP_FLAG;
}

bool FArmyWHCabinetMgr::IsGenToeGap() const
{
	return (GenFlag & WHC_GEN_TOE_GAP_FLAG) != 0;
}

void FArmyWHCabinetMgr::SetGenTopGap(bool bGenTopGap)
{
	if (bGenTopGap)
		GenFlag |= WHC_GEN_TOP_GAP_FLAG;
	else
		GenFlag &= ~WHC_GEN_TOP_GAP_FLAG;
}

bool FArmyWHCabinetMgr::IsGenTopGap() const
{
	return (GenFlag & WHC_GEN_TOP_GAP_FLAG) != 0;
}

void FArmyWHCabinetMgr::UpdateShapeAccessoryLocation()
{
    for (const auto & p : PlacedShapes)
	{
		if (p->CabinetActor.IsValid())
		{
			XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = p->CabinetActor;
			TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
			if (Accessory.IsValid() && Accessory->Actor != nullptr)
			{
				p->UpdateAccessoryLocation();
			}
		}
	}
}

UMaterialInterface* FArmyWHCabinetMgr::GetHangOrHighDoorMtl() const
{
	const TSharedPtr<FMtlInfo>* Mtl = FWHCModeGlobalData::TopMtlMap.Find(mTopMtlId);
	if (Mtl)
		return (*Mtl)->Mtl;

	for (const auto &PlacedShape : PlacedShapes)
	{
		ECabinetType eType = (ECabinetType)PlacedShape->Shape->GetShapeCategory();
		if (eType == ECabinetType::EType_HangCab || eType == ECabinetType::EType_TallCab || eType == ECabinetType::EType_OnCabCab)
			return PlacedShape->GetDoorMtl();
	}
	return nullptr;
}

UMaterialInterface* FArmyWHCabinetMgr::GetTableMtl() const
{
	UMaterialInterface *Mtl = nullptr;
	const TSharedPtr<FPlatformInfo> * PlatformInfo = FWHCModeGlobalData::PlatformMap.Find(mPlatformId);
	if (PlatformInfo && (*PlatformInfo)->PlatformMtl->Mtl != nullptr)
		Mtl = (*PlatformInfo)->PlatformMtl->Mtl;
	else
		Mtl = DefaultPlatformMtl;
	return Mtl;
}

UMaterialInterface* FArmyWHCabinetMgr::GetToeMtl() const
{
    if (mToeMtlId > 0)
    {
        const TSharedPtr<FMtlInfo> * MtlInfo = FWHCModeGlobalData::ToeMtlMap.Find(mToeMtlId);
        check(MtlInfo != nullptr);
        return (*MtlInfo)->Mtl;
    }
    else
        return DefaultToeMtl;
}

AXRWhcGapActor* FArmyWHCabinetMgr::GenGapActor(FVector2D StartPos, FVector2D EndPos, float Height, float zStart, UMaterialInstanceDynamic* pMtl)
{
	static int GapActorId = 0;
    FActorSpawnParameters SpawnParams;
	FString TempName = FString::Printf(TEXT("NOLIST - WHCGapActor_%d"), ++GapActorId);
    SpawnParams.Name = FName(*TempName);

	UWorld *World = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport)->World();
	AXRWhcGapActor* pActor = World->SpawnActor<AXRWhcGapActor>(AXRWhcGapActor::StaticClass(), SpawnParams);
	UStaticMeshComponent *pSMComp = pActor->GetStaticMeshComponent();
	pSMComp->SetMobility(EComponentMobility::Movable);
	pSMComp->SetStaticMesh(mGapMesh);
	pSMComp->SetMaterial(0, pMtl);
	pSMComp->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	FVector2D End2Star = (EndPos - StartPos);
	float xWidth = End2Star.Size()*0.5f;
	float ScaleX = xWidth / mSizeGapMesh.X;
	float ScaleZ = Height*0.5f / mSizeGapMesh.Z;
	float ScaleY = -1.0f;
	// zengy modified
	// 提高浮点数比较精度
	int32 xWidthx10 = FMath::RoundToInt(xWidth * 10.0f);
	int32 VerticalRangeXx10 = FMath::RoundToInt(FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.X*5.f);
	int32 VerticalRangeYx10 = FMath::RoundToInt(FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.Y*5.f);
	// if (!(FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.X*0.5f <= xWidth&&xWidth <= FWHCModeGlobalData::GlobalParams.AdjustmentVerticalRange.Y*0.5f))
	if (!(VerticalRangeXx10 <= xWidthx10 && xWidthx10 <= VerticalRangeYx10))
		ScaleY = -FWHCModeGlobalData::GlobalParams.AdjustmentLateralDepth/ mSizeGapMesh.Y;

	pActor->SetActorScale3D( FVector(ScaleX, ScaleY, ScaleZ) );

	float angle = FMath::Atan2(End2Star.Y, End2Star.X);
	FRotator r(0, angle * 180 / PI, 0);
	pActor->SetActorRotation(r);

	FVector Pos = FVector(StartPos.X, StartPos.Y, zStart);
	pActor->SetActorLocation(Pos);

	if ( pMtl )
	{
		pMtl->SetScalarParameterValue(FName(TEXT("Diffuse_UV_TilingU")), ScaleX);
		pMtl->SetScalarParameterValue(FName(TEXT("Diffuse_UV_TilingV")), ScaleZ);
		pMtl->SetScalarParameterValue(FName(TEXT("NormalMap_UV_Tiling_U")), ScaleX);
		pMtl->SetScalarParameterValue(FName(TEXT("NormalMap_UV_Tiling_V")), ScaleZ);
		pMtl->SetScalarParameterValue(TEXT("Diffuse_UV_Rotate"), 90.0f);
	}
	return pActor;
}


void FArmyWHCabinetMgr::DestroyTable()
{
	for (auto& p : mTableActors)
	{
		p->Destroy();
	}
	mTableActors.Empty();
	for (int i = 1; i < 3; ++i)
	{
		for (auto& p : mTrimActors[i])
		{
			p->Destroy();
		}
		mTrimActors[i].Empty();
		TrimPathMap.Remove(ETrimType(i));
	}

	const TSharedRef<FArmySceneData> &HomeData = FArmySceneData::Get();
	for (const auto &TableObject : TableObjects)
		HomeData->Delete(TableObject.Pin());
	TableObjects.Empty();
}

void FArmyWHCabinetMgr::DestroyFloorTrim()
{
	for (auto& p : mTrimActors[0])
	{
		p->Destroy();
	}
	mTrimActors[0].Empty();
	TrimPathMap.Remove(ETrim_Toe);
}

void FArmyWHCabinetMgr::DestroyFloorGap()
{
	for (auto& p : mGapFloors)
	{
		p->Destroy();
	}
	mGapFloors.Empty();
}

void FArmyWHCabinetMgr::DestroyHangGap()
{
	for (auto& p : mGapHangs)
	{
		p->Destroy();
	}
	mGapHangs.Empty();

	for (auto& p : mGapActors)
	{
		p->Destroy();
	}
	mGapActors.Empty();
}

void FArmyWHCabinetMgr::DestroyHangTrim()
{
	for (auto& p : mTrimActors[3])
	{
		p->Destroy();
	}
	mTrimActors[3].Empty();
	TrimPathMap.Remove(ETrim_Top);
}