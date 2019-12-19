#include "ArmyWHCabinetAutoMgr.h"
#include "ArmyWHCCookAssist.h"
#include "ArmyShapeTableActor.h"
#include "SCTResManager.h"
#include "MaterialManager.h"
#include "SCTCabinetShape.h"
#include "SCTSpaceShape.h"
#include "SCTDoorGroup.h"
#include "ArmyWHCModeCabinetOperation.h"
#include "StringConv.h"
#include "ArmyMath.h"
#include "common/XRSpline.h"
#include "Engine/StaticMeshActor.h"
#include "ArmyPillar.h"
#include "ArmyPackPipe.h"
#include "Engine/CollisionProfile.h"
#include "ArmySceneData.h"
#include "ArmyActorVisitorFactory.h"
#include "ArmyPolygonAssist.h"
#include "ArmyShapeFrameActor.h"
#include "SCTShapeActor.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "ArmyActorConstant.h"
#include "ArmyGameInstance.h"
#include "HttpMgr.h"

FArmyWHCabinetMgr* FArmyWHCabinetMgr::Singleton = nullptr;

FArmyWHCabinetMgr* FArmyWHCabinetMgr::Get()
{
	return Singleton;
}

FArmyWHCabinetMgr::FArmyWHCabinetMgr()
{
	Singleton = this;
	SetGenToeGap(true);
	SetGenTopGap(true);

	mGapMesh = nullptr;

	mFloorShapeHight = 10.0f;
}

FArmyWHCabinetMgr::~FArmyWHCabinetMgr()
{
	Singleton = nullptr;
}

bool FArmyWHCabinetMgr::Init()
{

	UObject *TempObj = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("Material'/Game/Material/Lit.Lit'"));
	if (TempObj && TempObj->IsA<UMaterialInterface>())
	{
		TempObj->AddToRoot();
		FMaterialManagerInstatnce::GetIns().GetMaterialManagerPtr()->SetLitLitMaterial(Cast<UMaterialInterface>(TempObj));
	}

	// PlatformInfo = MakeShareable(new FMtlInfo);
	DefaultPlatformMtl = LoadMtl("M_AiKongJian_181113066_418");
	//PlatformDefaultMtl = LoadMtl("M_AiKongJian_181113066_418"); 
	// PlatformInfo->Mtl = LoadMtl("M_AiKongJian_181113066_418");
	// PlatformMtl = LoadMtl("M_AiKongJian_181113066_418");
	DefaultToeMtl = LoadMtl("xn_170606016_C_418");
	//HangTrimMtl  = LoadMtl("M_AiKongJian_181113043_418");

	mTrimNames[ETrim_Toe] = GDefaultToeDxf;
	mTrimNames[ETrim_PlatformFront] = GDefaultFrontDxf;
	mTrimNames[ETrim_PlatformBack] = GDefaultBackDxf;
	mTrimNames[ETrim_Top] = GDefaultTopDxf;

	LoadMesh(TEXT("WHC/M_AiKongJian_181113002_418.pak"), TEXT("WHC/M_AiKongJian_181113002_418.json"));
	LoadMesh(TEXT("WHC/M_AiKongJian_181113007_418.pak"), TEXT("WHC/M_AiKongJian_181113007_418.json"));
	LoadMesh(TEXT("WHC/M_AiKongJian_181113012_418.pak"), TEXT("WHC/M_AiKongJian_181113012_418.json"));
	LoadMesh(TEXT("WHC/M_AiKongJian_181113011_418.pak"), TEXT("WHC/M_AiKongJian_181113011_418.json"));
	LoadMesh(TEXT("WHC/MATERIAL_1542592442974_3994.pak"), TEXT(""));
	LoadMesh(TEXT("WHC/400_600_418_sunboard.pak"), TEXT(""));
	mGapMesh = GetStaticMesh("WHC/400_600_418_sunboard.pak");
	if (mGapMesh)
		mSizeGapMesh = mGapMesh->GetBoundingBox().GetExtent();
	return true;
}
UMaterialInterface* FArmyWHCabinetMgr::LoadMtl(FString MtlName)
{
	FString ModelPakPath = FPaths::ProjectContentDir() / TEXT("WHC/")+ MtlName+".pak";
	FString ModelJsonPath = FPaths::ProjectContentDir() / TEXT("WHC/") + MtlName +".json";
	FString ParamJson;
	FFileHelper::LoadFileToString(ParamJson, *ModelJsonPath);
	UMaterialInterface* pMtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(ModelPakPath, ParamJson);
	if (pMtl != nullptr)
		pMtl->AddToRoot();
	mMaterials.Add(MtlName,pMtl);
	return pMtl;
}

void FArmyWHCabinetMgr::LoadMesh(const FString &InMeshFilePath, const FString &InMeshJson)
{
	FString ParamJson;
	FFileHelper::LoadFileToString(ParamJson, *(FPaths::ProjectContentDir() / InMeshJson));
	USCTXRResourceManager & ResMgr = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager();
	FSCTVRSObject *Object = ResMgr.LoadCustomObj(FPaths::ProjectContentDir() / InMeshFilePath, ParamJson);
	if (Object != nullptr)
		LoadedMeshMap.Add(InMeshFilePath, Object->SynID);
}

int32 FArmyWHCabinetMgr::GetMesh(const FString &InMeshFilePath)
{
	int32 *pSynID = LoadedMeshMap.Find(InMeshFilePath);
	return pSynID != nullptr ? *pSynID : -1;
}
UStaticMesh* FArmyWHCabinetMgr::GetStaticMesh(const FString &InMeshFilePath)
{
	int32 SynID = GetMesh(InMeshFilePath);
	if (SynID == -1)
		return nullptr;
	FSCTVRSObject *Object = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().GetObjFromObjID(SynID);
	if (Object==nullptr)
		return nullptr;
	return Object->GetLampMeshObject();
}
bool FArmyWHCabinetMgr::OnNewShape(FShapeInRoom *pShape)
{
	static int ShapeId = 0;
	pShape->mId = ++ShapeId;
	return true;
}
bool FArmyWHCabinetMgr::OnShapeInited(FShapeInRoom* pShape)
{
	if (pShape && pShape->mId == -1)
	{
		OnNewShape(pShape);
	}
	return true;
}
bool FArmyWHCabinetMgr::OnPalceShapeBegin(FShapeInRoom *pShape)
{
	if (pShape)
	{
		ETinyBoxType BoxType = (ETinyBoxType)pShape->Shape->GetShapeCategory();
		if (BoxType == eBoxHang || BoxType == eBoxHigh || BoxType == eBoxOnFloor)
		{
			DestroyHangTrim();
			DestroyHangGap();
		}
		else if (BoxType == eBoxFloor || BoxType == eBoxHigh)
		{	
			DestroyTable();
			DestroyFloorTrim();
		}	
		else if (BoxType == eBoxOther || BoxType == eBoxBathroomFloor || BoxType == eBoxBathroomHang || BoxType == eBoxBathroomStorage)
			pShape->DestroyPlatform();
	}
	return true;
}
bool FArmyWHCabinetMgr::OnRefreshShape(int32 iType)
{
	ETinyBoxType BoxType = (ETinyBoxType)iType;
	if (BoxType == eBoxHang || BoxType == eBoxHigh || BoxType == eBoxOnFloor)
		GenHangGapBoard();

	if (BoxType == eBoxFloor || BoxType == eBoxHigh)
	{
		GenTableBoard();
	}
	return true;
}
bool FArmyWHCabinetMgr::OnPalceShapeEnd(FShapeInRoom *pShape)
{
	if ( pShape )
	{
		ETinyBoxType BoxType = (ETinyBoxType)pShape->Shape->GetShapeCategory();
		if (BoxType == eBoxHang || BoxType == eBoxHigh || BoxType == eBoxOnFloor)
		{
			GenHangGapBoard();
		}
		if (BoxType == eBoxFloor || BoxType == eBoxHigh)
		{
			GenTableBoard();
		}
	}
	return true;
}
bool FArmyWHCabinetMgr::InitRoom()
{
	mRooms.Empty();

	TArray<FObjectWeakPtr> Objects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, Objects);
	TArray<FArmyRoom*> Rooms;
	for (auto &Object : Objects)
	{
		FArmyRoom *RoomObject = static_cast<FArmyRoom*>(Object.Pin().Get());
		const FGuid id = RoomObject->GetUniqueID();
		TArray<FVector> Points = RoomObject->GetWorldPoints(true);

		TSharedPtr<FArmyWHCPoly> pPoly = MakeShareable(new FArmyWHCPoly());
		pPoly->mGuid = id;
		pPoly->Init(Points);
		mRooms.Add(id, pPoly);
	}
	InitPillar(OT_Pillar);
	InitPillar(OT_AirFlue);
	//InitPillar(OT_PackPipe);
	return true;
}
void FArmyWHCabinetMgr::OnRefreshRoom()
{
	OnBeginMode();
}
bool FArmyWHCabinetMgr::OnBeginMode()
{
	// TODO: 需要增加一个标记，来标识是否需要在进入木作模式时重新生成台面等内容
	// 这个问题需要看户型的哪些数据会影响台面的生成
	CalculateFloorHeight();
	InitRoom();
	GroupShapeRoom();
	GenTableBoard();
	GenHangGapBoard();
	return true;
}

bool FArmyWHCabinetMgr::InitPillar(EObjectType eType)
{
	TArray<TWeakPtr<FArmyObject>> ObjectList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, eType, ObjectList);
	for (TWeakPtr<FArmyObject> IterCom : ObjectList)
	{
		FArmyWHCRect Rect; 
		TSharedPtr<FArmySimpleComponent> Pillar = StaticCastSharedPtr<FArmySimpleComponent>(IterCom.Pin());
		FVector Dir = Pillar->GetDirection();
		FVector2D Dir2d(Dir.X, Dir.Y);
		Dir2d.Normalize();
		Rect.Init(Pillar->GetPos(), Dir2d, FVector(Pillar->GetLength(), Pillar->GetWidth(),Pillar->GetHeight()) );

		for (auto& room:mRooms )
		{
			if ( room.Value->IsIn(Rect) )
				room.Value->AddRect(Pillar->GetUniqueID(), Rect);
		}
	}
	return true;
}
bool FArmyWHCabinetMgr::InitPackPipe()
{
	// TArray<TWeakPtr<FArmyObject>> ObjectList;
	// FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_PackPipe, ObjectList);
	// for (TWeakPtr<FArmyObject> IterCom : ObjectList)
	// {
	// 	TSharedPtr<FArmyPackPipe> PackPipe = StaticCastSharedPtr<FArmyPackPipe>(IterCom.Pin());
	// 	FBox Bound = PackPipe->GetBounds();
	// 	FVector Center = Bound.GetCenter();
	// }
	return false;
}
bool FArmyWHCabinetMgr::OnClear()
{
	GenTable(false);
	SetGenTop(false);
	SetGenToe(false);
	SetGenWaist(false);
	SetGenToeGap(true);
	SetGenTopGap(true);

	DestroyFloorGap();
	DestroyTable();
	DestroyFloorTrim();
	DestroyHangTrim();
	DestroyHangGap();
	return true;
}
bool FArmyWHCabinetMgr::OnEndMode()
{
	mRooms.Empty();
	return true;
}
bool FArmyWHCabinetMgr::GenTableBoard()
{
	DestroyFloorGap();
	DestroyTable();
	DestroyFloorTrim();

	TMap<int32, int32> CabinetTop;
	GroupCabinet(CabinetTop,true);

	for ( auto& room:mRooms )
	{
		for (auto& iTop : CabinetTop)
		{
			FArmyWHCCookAssist AssistGen;
			AssistGen.PrepareGenTable(IsGenToeGap(), room.Value.Get(), PlacedShapes, iTop.Key );
			AssistGen.GenGapMesh(
				FArmyWHCCookAssist::FGenGap::CreateLambda(
					[this](int32 nType, FVector2D StartPos, FVector2D EndPos, float fBottom, float fTop, UMaterialInstanceDynamic* pMtl) -> float {
						return GenGapActor(nType, StartPos, EndPos, fBottom, fTop, pMtl);
					}
				),
				FArmyWHCCookAssist::FGenBoardGap()
			);
			if (IsGenTable())
			{
				AssistGen.GenTableMesh(
					GetTableSplineElapse() * 10.0f,
					GetTableSplineWidth(),
					GetTableHeight(),
					FArmyWHCCookAssist::FGenTable::CreateRaw(this, &FArmyWHCabinetMgr::RefreshTableMesh),
					FArmyWHCCookAssist::FGenTrim::CreateRaw(this, &FArmyWHCabinetMgr::GenTrimActor),
					FArmyWHCCookAssist::FCollectTrimOutline::CreateRaw(this, &FArmyWHCabinetMgr::CollectTableTrimOutline)
				);

				// 当生成台面时，隐藏所有地柜图纸构件
				for (auto & PlacedShape : PlacedShapes)
				{
					if(PlacedShape->CabinetActor.IsValid())
					{
						XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = PlacedShape->CabinetActor;
						TSharedPtr<class FArmyFurniture> ShapeComponent = FurnitureComponent->Get();
						if (ShapeComponent.IsValid())
						{
							if (PlacedShape->Shape->GetShapeCategory() == (int32)ECabinetType::EType_OnGroundCab &&
								ShapeComponent.IsValid())
							{
								ShapeComponent->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, false);
							}
						}
					}
				}
			}
			else
			{
				// 当不生成台面时，显示所有地柜图纸构件
				for (auto & PlacedShape : PlacedShapes)
				{
					if (PlacedShape->CabinetActor.IsValid())
					{
						XRCabinetActorT<XRWHCabinetFurnitureComponent> FurnitureComponent = PlacedShape->CabinetActor;
						TSharedPtr<class FArmyFurniture> ShapeComponent = FurnitureComponent->Get();
						if (ShapeComponent.IsValid())
						{
							if (PlacedShape->Shape->GetShapeCategory() == (int32)ECabinetType::EType_OnGroundCab &&
								ShapeComponent.IsValid())
							{
								ShapeComponent->SetPropertyFlag(FArmyObject::FLAG_VISIBILITY, true);
							}
						}
					}
				}
			}

			if (IsGenToe())
			{
				AssistGen.GenFloorTrim(FArmyWHCCookAssist::FGenTrim::CreateRaw(this, &FArmyWHCabinetMgr::GenTrimActor));
			}
		}
	}

	return true;
}
bool FArmyWHCabinetMgr::CalculateFloorHeight()
{
	mFloorShapeHight = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap[ECabinetType::EType_OnGroundCab].AboveGround/* * 0.1f*/;
	mTableFrontExtrude = FWHCModeGlobalData::GlobalParams.PlatformBulge;
	for (auto &PlacedShape : PlacedShapes)
	{
		if (!(PlacedShape->Shape.IsValid() && PlacedShape->ShapeFrame != nullptr))
			continue;
		ETinyBoxType BoxType = (ETinyBoxType)PlacedShape->Shape->GetShapeCategory();
		if ( !(BoxType == eBoxFloor || BoxType == eBoxHigh) )
			continue;
		FVector ShapeLocation = PlacedShape->ShapeFrame->GetActorLocation();
		mFloorShapeHight = ShapeLocation.Z;
		break;
	}
	return true;
}
float FArmyWHCabinetMgr::GetAboveGround(int BoxType)
{
	auto& pp = FWHCModeGlobalData::GlobalParams;
	if (BoxType == eBoxFloor || BoxType == eBoxHigh)
		return mFloorShapeHight;
	else if (BoxType == eBoxHang)
		return FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap[ECabinetType::EType_HangCab].AboveGround + mFloorShapeHight - FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap[ECabinetType::EType_OnGroundCab].AboveGround;
	else if (BoxType == eBoxOnFloor)
		return FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap[ECabinetType::EType_OnCabCab].AboveGround + mFloorShapeHight - FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap[ECabinetType::EType_OnGroundCab].AboveGround;
	else
		return 0.0f;
}
bool FArmyWHCabinetMgr::OnResLoaded()
{
	CalculateFloorHeight();
	gFArmyWHCabinMgr->GenTableBoard();
	gFArmyWHCabinMgr->GenHangGapBoard();
	UpdateShapeAccessoryLocation();
	return true;
}
float FArmyWHCabinetMgr::GetTableHeight(bool bIngorShow)
{
	if (bIngorShow || IsGenTable())
	{	
		FTableInfo* pTableInfo = GetFrontInfo();
		if (pTableInfo)
			mTrimNames[1] = pTableInfo->GetCachedPath();

		FArmySplineRes* pSplineRes = gSplineResMgr.GetSplineRes(mTrimNames[1]);
		if (pSplineRes)
			return pSplineRes->mSize.Y;
	}
	return 0.0f;
}
float FArmyWHCabinetMgr::GetTableSplineWidth()
{
	FTableInfo* pTableInfo = GetFrontInfo();
	if (pTableInfo)
		mTrimNames[1] = pTableInfo->GetCachedPath();

	FArmySplineRes* pSplineRes = gSplineResMgr.GetSplineRes(mTrimNames[1]);
	if (pSplineRes)
		return pSplineRes->mSize.X;
	return 0.0f;
}

float FArmyWHCabinetMgr::GetTableSplineElapse()
{
	FTableInfo* pTableInfo = GetFrontInfo();
	if (pTableInfo)
		mTrimNames[1] = pTableInfo->GetCachedPath();

	FArmySplineRes* pSplineRes = gSplineResMgr.GetSplineRes(mTrimNames[1]);
	if (pSplineRes)
		return pSplineRes->mSize.X-mTableFrontExtrude;
	return 0.0f;
}
bool FArmyWHCabinetMgr::GroupCabinet(TMap<int32, int32>& CabinetTop, bool bFloor)
{
	CabinetTop.Empty();
	for (auto& Cabinet : PlacedShapes )
	{
		if (Cabinet->HasValidShape())
		{
			ETinyBoxType BoxType = (ETinyBoxType)Cabinet->Shape->GetShapeCategory();
			if (BoxType == eBoxOnFloor)
				continue;
			if (bFloor && BoxType == eBoxHang)
				continue;
			if (!bFloor && BoxType == eBoxFloor)
				continue;
			int32 iTop = int32(Cabinet->GetRect().mMaxZ*10.0f + 0.5f);
			CabinetTop.Add(iTop,true);	
		}
	}
	return true;
}
bool FArmyWHCabinetMgr::GenHangGapBoard()
{
	TMap<int32, int32> CabinetTop;
	GroupCabinet(CabinetTop,false);

	DestroyHangGap();
	DestroyHangTrim();
	for (auto& room : mRooms)
	{
		for (auto& iTop: CabinetTop )
		{
			FArmyWHCPoly * Room = room.Value.Get();

			// zengy modified 2019-2-13 
			// 去掉CookAssist中对于CabinetMgr的调用，将CookAssist作为纯粹的数据处理类
			FArmyWHCCookAssist AssistGen;
			AssistGen.PrepareGenHangGapBoard(IsGenTopGap(), room.Value.Get(), PlacedShapes, iTop.Key );
			AssistGen.GenGapMesh(
				FArmyWHCCookAssist::FGenGap::CreateLambda(
					[this](int32 nType, FVector2D StartPos, FVector2D EndPos, float fBottom, float fTop, UMaterialInstanceDynamic* pMtl) -> float {
						return GenGapActor(nType, StartPos, EndPos, fBottom, fTop, pMtl);
					}
				),
				FArmyWHCCookAssist::FGenBoardGap::CreateLambda(
					[this](const TArray<FPUVVertex>& Vers, const TArray<uint16>& Faces, UMaterialInstanceDynamic* pMtl) {
						GenGapActor(Vers, Faces, pMtl);
					}
				)
			);
			if (IsGenTop())
			{
				AssistGen.GenHangTrim(FArmyWHCCookAssist::FGenTrim::CreateRaw(this, &FArmyWHCabinetMgr::GenTrimActor));
			}
		}
	}
	return true;
}



FArmyWHCLineSeg* FArmyWHCabinetMgr::CollisionRoomSeg(FArmyWHCRect& rect, const FGuid& RoomId, int32 Idx, float OffsetX, float OffsetY)
{
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(RoomId);
	if (pPloy == nullptr)
		return nullptr;
	FArmyWHCLineSeg& Seg = (*pPloy)->GetSeg(Idx);
	if (rect.Collision(Seg, OffsetX, OffsetY))
		return &Seg;
	return nullptr;
}
FArmyWHCLineSeg* FArmyWHCabinetMgr::CollisionRoom(FArmyWHCRect& rect, const FGuid& RoomId, float OffsetX, float OffsetY)
{
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(RoomId);
	if (pPloy == nullptr)
		return nullptr;
	return (*pPloy)->Collision(rect, OffsetX, OffsetY);
}
FArmyWHCLineSeg* FArmyWHCabinetMgr::CollisionNextRoomSeg(FArmyWHCRect& rect, const FGuid& RoomId, int32 Idx, float OffsetX, float OffsetY)
{
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(RoomId);
	if (pPloy == nullptr)
		return nullptr;
	FArmyWHCLineSeg& Seg = (*pPloy)->GetNextSeg(Idx);
	if (rect.Collision(Seg, OffsetX, OffsetY))
		return &Seg;
	return nullptr;
}
FArmyWHCLineSeg* FArmyWHCabinetMgr::CollisionPreRoomSeg(FArmyWHCRect& rect, const FGuid& RoomId, int32 Idx, float OffsetX, float OffsetY)
{
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(RoomId);
	if (pPloy == nullptr)
		return nullptr;
	FArmyWHCLineSeg& Seg = (*pPloy)->GetPreSeg(Idx);
	if (rect.Collision(Seg, OffsetX, OffsetY))
		return &Seg;
	return nullptr;
}
bool FArmyWHCabinetMgr::Collision(FShapeInRoom *pShape, FArmyWHCRect& rect, FArmyWHCRect& RectOther, float Offset, const TArray<FShapeInRoom*> &IgnoredShapes)
{
	for (auto &p : PlacedShapes)
	{
		if (p.Get() == pShape)
			continue;
		if (IgnoredShapes.Num() > 0 && IgnoredShapes.ContainsByPredicate([&p](const FShapeInRoom* InData) { return p.Get() == InData; }))
			continue;
		RectOther = p->GetRect();
		if (rect.Collision(&RectOther, Offset))
			return true;
	}
	return false;
}
bool FArmyWHCabinetMgr::CollisionPillar(FArmyWHCRect& Rect, FArmyWHCRect* pRectRet /* =nullptr */, float Offset/* =0.0f */, FArmyWHCPoly* pRoom)
{
	if (pRoom == nullptr)
		pRoom = GetPolyByPos(Rect.mPos);
	if (pRoom)
	{
		FArmyWHCRect* pRect = pRoom->CollisonChild(Rect, Offset);
		if (pRect)
		{
			if (pRectRet)
				*pRectRet = *pRect;
			return true;
		}
	}
	return false;
}
bool FArmyWHCabinetMgr::Collision(FShapeInRoom* pShape, FArmyWHCRect* pRect, float Offset, bool bPillar, const TArray<FShapeInRoom*> &IgnoredShapes)
{
	FArmyWHCRect rect = pShape->GetRect();
	FArmyWHCRect RectOther;
	if (pRect == nullptr)
		pRect = &rect;
	if (Collision(pShape, *pRect, RectOther, Offset, IgnoredShapes))
		return true;
	if (CollisionRoom(*pRect, pShape->RoomAttachedIndex, Offset, Offset))
		return true;
	if (bPillar && CollisionPillar(*pRect, nullptr, Offset))
		return true;
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(pShape->RoomAttachedIndex);
	if (pPloy)
		return !(*pPloy)->IsIn(pRect->mPos);
	return false;
}
FVector2D FArmyWHCabinetMgr::GetNearSeg(FVector2D Pos, const FGuid& RoomId, int32& Idx, float& fDistance)
{
	Idx = -1;
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(RoomId);
	if (pPloy == nullptr)
		return FVector2D();
	return (*pPloy)->GetNearSeg(Pos, Idx, fDistance);
}

FArmyWHCPoly* FArmyWHCabinetMgr::GetPolyById(const FGuid& Id)
{
	TSharedPtr<FArmyWHCPoly>* pPloy = mRooms.Find(Id);
	if (pPloy == nullptr)
		return nullptr;
	return (*pPloy).Get();
}

FArmyWHCPoly* FArmyWHCabinetMgr::GetPolyByPos(FVector2D Pos)
{
	for (auto& p: mRooms )
	{
		if ( p.Value->IsIn(Pos) )
			return p.Value.Get();
	}
	return nullptr;
}

bool FArmyWHCabinetMgr::GroupShapeRoom()
{
	for (auto&p : PlacedShapes)
	{
		FArmyWHCRect Rect = p->GetRect();
		FArmyWHCPoly* pRoom = GetPolyByPos(Rect.mPos);
		if (pRoom)
			p->RoomAttachedIndex = pRoom->mGuid;
	}
	return true;
}
FArmyWHCPoly* FArmyWHCabinetMgr::CollisionRoom(FVector& PosStart, FVector& Dir, float InAboveGround, int32 &OutRoomIndex, FVector &OutPositionInRoom)
{
	OutRoomIndex = -1;
	FArmyWHCPoly* pNearestRoom = nullptr;
	float MinDistSqr = FLT_MAX;
	FVector CrossPos;
	for (auto& p : mRooms)
	{
		if (p.Value->RayCollisonFloor(PosStart, Dir, InAboveGround, CrossPos))
		{
			MinDistSqr = (CrossPos - PosStart).SizeSquared();
			pNearestRoom = p.Value.Get();
			OutPositionInRoom = CrossPos;
			break;
		}
	}
	for (auto& p : mRooms)
	{
		FArmyWHCLineSeg* Seg = p.Value->RayCollisonWall(PosStart, Dir, InAboveGround, FArmySceneData::WallHeight, CrossPos);
		if (Seg )
		{
			float DistSqr = (CrossPos - PosStart).SizeSquared();
			if (DistSqr < MinDistSqr)
			{
				pNearestRoom = p.Value.Get();
				MinDistSqr = DistSqr;
				OutRoomIndex = Seg->mIdx;
				OutPositionInRoom = CrossPos;
			}
		}
	}
	return pNearestRoom;
}


//
//CabComponentsPool& FArmyWHCabinetMgr::CabinetComponentsPool(){
//	return mCabComponetsPool;
//}
















//////////////////////////////////////////////////////////////////////////
bool FArmyWHCabinetMgr::OnUIAboveGroundChangeSingle(float fValue, FShapeInRoom* pShapeInRoom)
{
	// CHECK_ERROR(fValue >= 0 && fValue<=4000);
	// float fElapseHeight = fValue * 0.1f - pShapeInRoom->AboveGround;
	if (fValue * 0.1f > (FArmySceneData::WallHeight - pShapeInRoom->Shape->GetShapeHeight()*0.1f))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
		return false;
	}

	pShapeInRoom->AboveGround = fValue * 0.1f;
	pShapeInRoom->OnAboveFloorChange();
	OnRefreshShape((ETinyBoxType)pShapeInRoom->Shape->GetShapeCategory());
	return true;
}
bool FArmyWHCabinetMgr::OnUIAboveGroundChange(float fValue)
{
	float Delta = fValue * 0.1f - mFloorShapeHight;
	for (const auto &PlacedShape : PlacedShapes)
	{
		int32 ShapeCategory = PlacedShape->Shape->GetShapeCategory();
		if (ShapeCategory == (int32)ECabinetType::EType_HangCab || 
			ShapeCategory == (int32)ECabinetType::EType_OnGroundCab ||
			ShapeCategory == (int32)ECabinetType::EType_TallCab ||
			ShapeCategory == (int32)ECabinetType::EType_OnCabCab)
		{
			PlacedShape->AboveGround += Delta;
			PlacedShape->OnAboveFloorChange();
		}	
	}

	mFloorShapeHight = fValue * 0.1f;
	OnRefreshShape(eBoxFloor);
	OnRefreshShape(eBoxHang);
	return true;
}
bool FArmyWHCabinetMgr::OnUptValueCommitted(float fValue, FShapeInRoom* pShapeInRoom)
{
	float ElapseHeight = fValue;
	float fEndValue = pShapeInRoom->AboveGround + ElapseHeight;
	if (fEndValue < 0 )
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
		return false;
	}
	if (fEndValue > (FArmySceneData::WallHeight - pShapeInRoom->Shape->GetShapeHeight()*0.1f))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
		return false;
	}

	ETinyBoxType BoxType = (ETinyBoxType)pShapeInRoom->Shape->GetShapeCategory();
	if ( BoxType == eBoxHang || BoxType == eBoxOnFloor)
	{
		pShapeInRoom->AboveGround += ElapseHeight;
		pShapeInRoom->OnAboveFloorChange();
		OnRefreshShape(eBoxHang);
		return true;
	}
	for (auto &PlacedShape : PlacedShapes)
	{
		if (!(PlacedShape->Shape.IsValid() && PlacedShape->ShapeFrame != nullptr))
			continue;
		PlacedShape->AboveGround += ElapseHeight;
		PlacedShape->OnAboveFloorChange();		
	}
	if (BoxType == eBoxFloor || BoxType == eBoxHigh)
	{
		mFloorShapeHight += ElapseHeight;
		OnRefreshShape(eBoxFloor);
		OnRefreshShape(eBoxHang);
	}
	else
	{
		OnRefreshShape(eBoxHang);
	}
	return true;
}
bool FArmyWHCabinetMgr::OnFrontValueCommitted(float fValue, FShapeInRoom* pShapeInRoom)
{
	CHECK_ERROR(fValue >= -1000 && fValue <= 1000);
	FArmyWHCRect rect = pShapeInRoom->GetRect();
	FVector PosNew = fValue * 0.1f * rect.GetNor() + pShapeInRoom->ShapeFrame->GetActorLocation();
	rect = pShapeInRoom->GetRect(&PosNew);
	if (Collision(pShapeInRoom, &rect))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
		return false;
	}
	pShapeInRoom->ShapeFrame->SetActorLocation(PosNew);
	OnPalceShapeEnd(pShapeInRoom);
	return true;
}
bool FArmyWHCabinetMgr::OnRightValueCommitted(float fValue, FShapeInRoom* pShapeInRoom)
{
	CHECK_ERROR(fValue >= -1000 && fValue <= 1000);
	FArmyWHCRect rect = pShapeInRoom->GetRect();
	FVector PosNew = fValue * 0.1f * rect.GetDir() + pShapeInRoom->ShapeFrame->GetActorLocation();
	rect = pShapeInRoom->GetRect(&PosNew);
	if (Collision(pShapeInRoom, &rect))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("尺寸超出限制 请重新输入"));
		return false;
	}
	pShapeInRoom->ShapeFrame->SetActorLocation(PosNew);
	OnPalceShapeEnd(pShapeInRoom);
	return true;
}
bool FArmyWHCabinetMgr::AdjustCabinetNewWidth(FShapeInRoom* pShapeInRoom, float NewWidth)
{
	FArmyWHCRect rect = pShapeInRoom->GetRect(nullptr, &NewWidth);
	FArmyWHCRect rectOld = pShapeInRoom->GetRect();
	float xOffset = rect.mSizeHalf.X - rectOld.mSizeHalf.X;
	bool bCanAjust = true;
	do 
	{
		if (xOffset <= 0)
		{
			if (!Collision(pShapeInRoom, &rectOld, -0.0001f))
				break;

			rect.mPos = rectOld.mPos + xOffset*rect.mDir;
			if (Collision(pShapeInRoom, &rect, -0.0001f))
			{
				pShapeInRoom->SetTranslate(rect);
				break;
			}
			rect.mPos = rectOld.mPos - xOffset*rect.mDir;
			if (Collision(pShapeInRoom, &rect, -0.0001f))
			{
				pShapeInRoom->SetTranslate(rect);
				break;
			}
			break;
		}
		if (!Collision(pShapeInRoom, &rect, -0.0001f))
			break;

		rect.mPos = rectOld.mPos+xOffset*rect.mDir;
		if (!Collision(pShapeInRoom, &rect, -0.0001f))
		{
			pShapeInRoom->SetTranslate(rect);
			break;
		}
		rect.mPos = rectOld.mPos - xOffset*rect.mDir;
		if (!Collision(pShapeInRoom, &rect, -0.0001f))
		{
			pShapeInRoom->SetTranslate(rect);
			break;
		}
		bCanAjust = false;
	} while (false);

	if (!bCanAjust)
		return false;

	if (pShapeInRoom->Shape->SetShapeWidth(NewWidth))
	{
		float OldDepth = pShapeInRoom->Shape->GetShapeDepth();
		float OldHeight = pShapeInRoom->Shape->GetShapeHeight();
		XRCabinetActorT<XRWHCabinetRulerComponent> RulerComponent = pShapeInRoom->CabinetActor;
		TSharedPtr<class FShapeRuler> ShapeWidthRuler = RulerComponent->Get();
		if (ShapeWidthRuler.IsValid())
		{
			ShapeWidthRuler->SetRulerLength(NewWidth * 0.1f);
			ShapeWidthRuler->GetRulerActor()->SetActorRelativeLocation(FVector(-NewWidth * 0.05f, 0.0f, OldHeight * 0.1f));
		}
		pShapeInRoom->Shape->GetShapeActor()->SetActorRelativeLocation(FVector(-NewWidth * 0.05f, 0.0f, 0.0f));
		pShapeInRoom->ShapeFrame->RefreshFrame(NewWidth * 0.1f, OldDepth * 0.1f, OldHeight * 0.1f);
		XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = pShapeInRoom->CabinetActor;
		TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
		if (Accessory.IsValid() && Accessory->Actor != nullptr)
		{
			pShapeInRoom->UpdateAccessoryLocation();
		}
		return true;
	}
	else
		return false;
}
void FArmyWHCabinetMgr::LoadTopMtl()
{
	int32 decoratingId = gCabinMgr.GetTopInfo() ? gCabinMgr.GetTopInfo()->Id : 0;
	TSharedRef<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(
		FString::Printf(TEXT("design/decoratingPlate/materials?decoratingId=%d&categoryId=0"), decoratingId),
		TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", "application/json; charset=utf-8");
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
		{
			if (!FHttpMgr::Get()->CheckHttpResponseValid(ResponsePtr, bSucceeded))
				return;
			FString ErrorMsg;
			TSharedPtr<FJsonObject> ResponseData = FHttpMgr::Get()->GetContentAsJsonObject(ResponsePtr, &ErrorMsg);
			if (!ResponseData.IsValid())
				return;

			const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
			int32 nCount = Data->GetNumberField(TEXT("totalPage"));

			const TArray<TSharedPtr<FJsonValue>>* TopSections = nullptr;
			Data->TryGetArrayField(TEXT("list"), TopSections);
			for (auto& Section : *TopSections)
			{
				int32 Id = Section->AsObject()->GetNumberField(TEXT("id"));
				if (FWHCModeGlobalData::TopMtlMap.Find(Id) == nullptr)
				{
					FMtlInfo * NewPlatformInfo = new FMtlInfo;
					NewPlatformInfo->Id = Id;
					NewPlatformInfo->MtlName = Section->AsObject()->GetStringField(TEXT("name"));
					NewPlatformInfo->MtlThumbnailUrl = Section->AsObject()->GetStringField(TEXT("thumbnailUrl"));
					NewPlatformInfo->MtlUrl = Section->AsObject()->GetStringField(TEXT("pakUrl"));
					NewPlatformInfo->MtlMd5 = Section->AsObject()->GetStringField(TEXT("pakMd5"));
					NewPlatformInfo->MtlParam = Section->AsObject()->GetStringField(TEXT("optimizeParam"));
					FWHCModeGlobalData::TopMtlMap.Emplace(Id, MakeShareable(NewPlatformInfo));
					NewPlatformInfo->Mtl=FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(NewPlatformInfo->GetCachedPath(), NewPlatformInfo->MtlParam);
				}
			}
			gCabinMgr.OnRefreshShape(eBoxHang);
		}
	);
	HttpRequest->ProcessRequest();
}


const int32 CabComponentsPool::defaultCapacity = 2048;

CabComponentsPool::CabComponentsPool() {
	InstanceList.Reserve(defaultCapacity);
	FreeList.Reserve(defaultCapacity);
	FreeTransformList.Reserve(defaultCapacity);
}


void CabComponentsPool::RemoveComponent(int32 location, int32 signature) {
	Entry& instance = InstanceList[location];
	if (signature == instance.signature) {
		if (instance.signature < 0) {
			FreeTransformList.Push(location);
		}
		else{
			FreeList.Push(location);
		}
		++instance.signature;
		if(instance.tpComponent.IsValid())
			instance.tpComponent = nullptr;
	}
}
bool CabComponentsPool::IsTransformComponent(int64 hashCodes) {

	return hashCodes == GetTypeHash(XRWHTransform::typeName());
}

CabComponentsPool::Location CabComponentsPool::AddComponent(TSharedPtr<XRWHCabinetIComponent> tpComponent) {
	if (!tpComponent.IsValid()) {
		return{ 0 };
	}
	Location location = { 0 };
	{
		const int int32Max = 65535;   //32 bit computer
		const int int32Min = -int32Max - 1;
	
		if (IsTransformComponent(tpComponent->ClassHashName())) {
			if (FreeTransformList.Num()<=0) {
				location.value = InstanceList.Num();
				InstanceList.Push({ int32Min, nullptr });
			}
			else {
				location.value = FreeTransformList.Last();
				FreeTransformList.Pop();
			}
		}
		else {
			if (FreeList.Num() <= 0) {
				location.value = InstanceList.Num();
				InstanceList.Push({ 0, nullptr });
			}
			else {
				location.value = FreeList.Last();
				FreeList.Pop();
			}
		}
	}
	if (location.value == 14 || location.value == 4)
	{
		int32 vv = 22;
	}
	InstanceList[location.value].tpComponent = tpComponent;
	location.signatureCapture = InstanceList[location.value].signature;

	return location;
}

