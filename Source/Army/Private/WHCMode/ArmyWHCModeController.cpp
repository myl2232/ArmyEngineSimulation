#include "ArmyWHCModeController.h"
#include "SArmyWHCModeContentBrowser.h"
#include "SArmyWHCModeAttrPanel.h"
#include "ArmyWHCModeCommands.h"
#include "ArmyWHCModeCabinetOperation.h"
#include "ArmyFrameCommands.h"
#include "ArmyWHCModeGlobalParams.h"
#include "ArmyWHCShapeDoorChangeList.h"
#include "ArmyWHCCabinetChangeList.h"
#include "ArmyShapeFrameActor.h"
#include "ArmyShapeTableActor.h"
#include "ArmyWHCabinetAutoMgr.h"

#include "Game/XRGameInstance.h"
#include "Game/XRViewportClient.h"
#include "Actor/XRActorConstant.h"
#include "ArmyToolBarBuilder.h"
#include "Model/XRHomeData.h"
#include "Data/WHCModeData/XRWHCModeData.h"
#include "Data/XRRoom.h"
#include "ArmyActorVisitorFactory.h"

#include "SArmyDetailInfoWidget.h"

#include "SContentItem.h"
#include "ArmyDownloadModule.h"

#include "ArmyHttpModule.h"

#include "SCTCabinetShape.h"
#include "SCTDoorGroup.h"
#include "UtilityTools/SCTAccessShapeForBim.h"
#include "SCTShapeManager.h"
#include "SCTResManager.h"
#include "MaterialManager.h"
#include "Actor/SCTShapeActor.h"
#include "UtilityTools/SCTShapeUtilityTools.h"
#include "SCTGlobalDataForShape.h"
#include "HttpMgr.h"

#include "Modules/ModuleManager.h"

#include "Misc/Base64.h"
#include "Frame/XRDesignEditor.h"

#include "ArmyWorldManager.h"

#include "SArmyCabinetTypeItem.h"
#include "SArmyCabinetItem.h"
#include "SArmyAutoDesignSolution.h"
#include "SArmyKitchenTypeItem.h"
#include "SArmyKitchenItem.h"

FArmyWHCModeController::FArmyWHCModeController()
{
	FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("SCTTess"));
	FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("SCTShapeModule"));

	CabinetOperation = MakeShareable(new FArmyWHCModeCabinetOperation);
	mCablinMgr = MakeShareable(new FArmyWHCabinetMgr);
}

FArmyWHCModeController::~FArmyWHCModeController()
{
	FWHCModeGlobalData::SelectionState = 0;

	CabinetOperation.Reset();
	mCablinMgr.Reset();
	FArmyWHCModeCommands::Unregister();
}

const EModelType FArmyWHCModeController::GetDesignModelType()
{
    return EModelType::E_WHCModel;
}

void FArmyWHCModeController::Init()
{
	FGlobalDataForShape::GetInsPtr()->SetRootDir(FPaths::ProjectContentDir() / TEXT("WHC"));

	FHttpMgr::Get()->SetDomainName(FArmyHttpModule::Get().GetWHCAddress());

	FArmyWHCModeCommands::Register();
	FArmyDesignModeController::Init();

	FArmyWHCModeGlobalParams QueryGlobalParams;
	QueryGlobalParams.Query();

	CurrentDisplayingCategoryIndex = 0;

	// 当有多个柜子分类时需要重设数组的数量
	FWHCModeGlobalData::CabinetInfoArrCollection.Emplace((int32)ECabinetClassify::EClassify_Cabinet, FWHCModeGlobalData::TCabinetInfoArr());
	FWHCModeGlobalData::CabinetInfoArrCollection.Emplace((int32)ECabinetClassify::EClassify_Wardrobe, FWHCModeGlobalData::TCabinetInfoArr());
	FWHCModeGlobalData::CabinetInfoArrCollection.Emplace((int32)ECabinetClassify::EClassify_Other, FWHCModeGlobalData::TCabinetInfoArr());
	FWHCModeGlobalData::CabinetInfoArrCollection.Emplace((int32)ECabinetClassify::EClassify_BathroomCab, FWHCModeGlobalData::TCabinetInfoArr());

	FArmyWHCabinetMgr::Get()->Init();

	FArmySceneData::Get()->ChangedHomeDataDelegate.AddRaw(gFArmyWHCabinMgr, &FArmyWHCabinetMgr::OnRefreshRoom);//原始户型发生修改，需要更新
}

void FArmyWHCModeController::Quit()
{
	if (CabinetOperation.IsValid())
		CabinetOperation->OnQuit();
}

void FArmyWHCModeController::BeginMode()
{
	GVC->SetDrawMode(DM_3D);
	GVC->bForceHiddenWidget = true;

    TSharedPtr<IArmyActorVisitor> WHCAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_WHCMode);
    WHCAV->Show(true);

	CabinetOperation->BeginMode();

	ContentBrowser->BeginMode();
	ShapeDetailInfo->BeginMode();

	// int32 CabType = (int32)ECabinetType::EType_Cab;
	FWHCModeGlobalData::CabinetListInfo.Reset();
	QueryCabinetList(FWHCModeGlobalData::CurrentWHCClassify, 0);
	gFArmyWHCabinMgr->OnBeginMode();

	FArmyWorldManager::Get().TryChangeEnvironmentModeConditionally(DesignMode::WHCMode);
}

bool FArmyWHCModeController::EndMode()
{
	GVC->bForceHiddenWidget = false;

	ContentBrowser->ClearSelection();
	CabinetOperation->EndMode();
	gFArmyWHCabinMgr->OnEndMode();

	ShapeDetailInfo->EndMode();

	return true;
}

void FArmyWHCModeController::Load(TSharedPtr<FJsonObject> Data)
{
	if (!Data.IsValid())
		return;
	FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get();

	const TSharedPtr<FJsonObject> &WHCModeJsonObj = Data->GetObjectField(TEXT("WHCMode"));
	const TArray<TSharedPtr<FJsonValue>> &CabTemplates = WHCModeJsonObj->GetArrayField(TEXT("cab_templates"));
	if (CabTemplates.Num() > 0)
	{
		TArray<FSCTShape*> ShapeTemplates;
		TMap<int32, FCabinetWholeComponent*> ShapeWholeComponents;
		// 解析柜子模板数据
		for (const auto &CabTemplate : CabTemplates)
		{
			const TSharedPtr<FJsonObject> &CabTemplateJsonObj = CabTemplate->AsObject();
			int32 Id = CabTemplateJsonObj->GetIntegerField(TEXT("id"));
			// 检查柜子模板是否已存在，如果不存在就读取保存的数据并解析
			TSharedPtr<FSCTShape> *ShapeTemplate = FWHCModeGlobalData::CabinetTemplateMap.Find(Id);
			if (ShapeTemplate == nullptr)
			{
				FCabinetShape *NewTemplate = new FCabinetShape;
                FSCTShapeManager::Get()->ParseChildrenShapes(CabTemplateJsonObj);
				NewTemplate->ParseFromJson(CabTemplateJsonObj);
				FSCTShapeManager::Get()->ClearAllChildrenShapes();

				FWHCModeGlobalData::CabinetTemplateMap.Emplace(Id, MakeShareable(NewTemplate));
				ShapeTemplates.Emplace(NewTemplate);
			}
			else
				ShapeTemplates.Emplace(ShapeTemplate->Get());

			TSharedPtr<FCabinetWholeComponent> * WholeComponent = FWHCModeGlobalData::CabinetComponentMap.Find(Id);
			if (WholeComponent == nullptr)
			{
				FCabinetWholeComponent * NewWholeComponent = new FCabinetWholeComponent;
				// 解析和柜子模板对应的构件数据
				NewWholeComponent->Cabinet = MakeShareable(new FCabinetComponent);
				NewWholeComponent->Cabinet->Id = CabTemplateJsonObj->GetIntegerField(TEXT("component_id"));
				NewWholeComponent->Cabinet->Type = CabTemplateJsonObj->GetIntegerField(TEXT("component_type"));
				NewWholeComponent->Cabinet->TopResourceUrl = CabTemplateJsonObj->GetStringField(TEXT("component_url"));
				NewWholeComponent->Cabinet->TopResourceMd5 = CabTemplateJsonObj->GetStringField(TEXT("component_md5"));

				NewWholeComponent->Cabinet->FrontResourceUrl = CabTemplateJsonObj->GetStringField(TEXT("component_Fronturl"));
				NewWholeComponent->Cabinet->FrontResourceMd5 = CabTemplateJsonObj->GetStringField(TEXT("component_Frontmd5"));

				NewWholeComponent->Cabinet->SideResourceUrl = CabTemplateJsonObj->GetStringField(TEXT("component_Sideurl"));
				NewWholeComponent->Cabinet->SideResourceMd5 = CabTemplateJsonObj->GetStringField(TEXT("component_Sidemd5"));
				// 解析和柜子模板对应的附属件数据
				const TArray<TSharedPtr<FJsonValue>>* AccCompJsonObjs = nullptr;
				if (CabTemplateJsonObj->TryGetArrayField(TEXT("acc_accessorycomponents"), AccCompJsonObjs))
				{
					for (auto CompJson : *AccCompJsonObjs)
					{
						TSharedPtr<FJsonObject> CompObj = CompJson->AsObject();
						TSharedPtr<FCabinetComponent> Accessory = MakeShareable(new FCabinetComponent);
						Accessory->Id = CompObj->GetIntegerField(TEXT("id"));
						Accessory->Type = CompObj->GetIntegerField(TEXT("type"));
						Accessory->TopResourceUrl = CompObj->GetStringField(TEXT("Topurl"));
						Accessory->TopResourceMd5 = CompObj->GetStringField(TEXT("Topmd5"));
						Accessory->FrontResourceUrl = CompObj->GetStringField(TEXT("Fronturl"));
						Accessory->FrontResourceMd5 = CompObj->GetStringField(TEXT("Frontmd5"));
						Accessory->SideResourceUrl = CompObj->GetStringField(TEXT("Sideurl"));
						Accessory->SideResourceMd5 = CompObj->GetStringField(TEXT("Sidemd5"));
						NewWholeComponent->Accessorys.Add(Accessory);
					}
				}
				//解析门板构件
				const TArray<TSharedPtr<FJsonValue>>* DoorSheetCompJsonObjs = nullptr;
				if (CabTemplateJsonObj->TryGetArrayField(TEXT("acc_doorsheetcomponents"), DoorSheetCompJsonObjs))
				{
					for (auto CompJson : *DoorSheetCompJsonObjs)
					{
						TSharedPtr<FJsonObject> CompObj = CompJson->AsObject();
						TSharedPtr<FCabinetComponent> DoorSheetComp = MakeShareable(new FCabinetComponent);
						DoorSheetComp->Id = CompObj->GetIntegerField(TEXT("id"));
						DoorSheetComp->Type = CompObj->GetIntegerField(TEXT("type"));
						DoorSheetComp->TopResourceUrl = CompObj->GetStringField(TEXT("Topurl"));
						DoorSheetComp->TopResourceMd5 = CompObj->GetStringField(TEXT("Topmd5"));
						DoorSheetComp->FrontResourceUrl = CompObj->GetStringField(TEXT("Fronturl"));
						DoorSheetComp->FrontResourceMd5 = CompObj->GetStringField(TEXT("Frontmd5"));
						DoorSheetComp->SideResourceUrl = CompObj->GetStringField(TEXT("Sideurl"));
						DoorSheetComp->SideResourceMd5 = CompObj->GetStringField(TEXT("Sidemd5"));
						NewWholeComponent->DoorSheets.Add(DoorSheetComp);
					}
				}
				FWHCModeGlobalData::CabinetComponentMap.Emplace(Id, MakeShareable(NewWholeComponent));
				ShapeWholeComponents.Emplace(Id, NewWholeComponent);
			}
			else
				ShapeWholeComponents.Emplace(Id, WholeComponent->Get());
			
		}

		// 解析移门门型模板数据
		TArray<FSCTShape*> SlidingDoorShapeTemplates;
		const TArray<TSharedPtr<FJsonValue>> & SlidingDoorTemplates = WHCModeJsonObj->GetArrayField(TEXT("sliding_door_templates"));
		for (const auto & SlidingDoorTemplate : SlidingDoorTemplates)
		{
			const TSharedPtr<FJsonObject> & SlidingDoorTemplateObj = SlidingDoorTemplate->AsObject();

			int32 Id = SlidingDoorTemplateObj->GetIntegerField(TEXT("id"));
			TSharedPtr<FCoverDoorTypeInfo> * SlidingDoorTypeInfoPtr = FWHCModeGlobalData::SlidingDoorTypeMap.Find(Id);
			if (SlidingDoorTypeInfoPtr == nullptr)
			{
				FSlidingDoor * SlidingDoor = new FSlidingDoor;
				FSCTShapeManager::Get()->ParseChildrenShapes(SlidingDoorTemplateObj);
				SlidingDoor->ParseFromJson(SlidingDoorTemplateObj);
				FSCTShapeManager::Get()->ClearAllChildrenShapes();

				FCoverDoorTypeInfo * NewSlidingDoorTypeInfo = new FCoverDoorTypeInfo;
				NewSlidingDoorTypeInfo->Id = Id;
				NewSlidingDoorTypeInfo->Name = SlidingDoor->GetShapeName();
				NewSlidingDoorTypeInfo->ThumbnailUrl = SlidingDoor->GetThumbnailUrl();
				NewSlidingDoorTypeInfo->DoorGroupShape = MakeShareable(SlidingDoor);
				FWHCModeGlobalData::SlidingDoorTypeMap.Emplace(Id, MakeShareable(NewSlidingDoorTypeInfo));

				SlidingDoorShapeTemplates.Emplace(SlidingDoor);
			}
			else
				SlidingDoorShapeTemplates.Emplace((*SlidingDoorTypeInfoPtr)->DoorGroupShape.Get());			
		}

		// 解析掩门门型模板数据
		TArray<FSCTShape*> CoverDoorShapeTemplates;
		const TArray<TSharedPtr<FJsonValue>> & CoverDoorTemplates = WHCModeJsonObj->GetArrayField(TEXT("cover_door_templates"));
		for (const auto & CoverDoorTemplate : CoverDoorTemplates)
		{
			const TSharedPtr<FJsonObject> & CoverDoorTemplateObj = CoverDoorTemplate->AsObject();

			int32 Id = CoverDoorTemplateObj->GetIntegerField(TEXT("id"));
			TSharedPtr<FCoverDoorTypeInfo> * CoverDoorTypeInfoPtr = FWHCModeGlobalData::CoverDoorTypeMap.Find(Id);
			if (CoverDoorTypeInfoPtr == nullptr)
			{
				FSideHungDoor * SideHungDoor = new FSideHungDoor;
				FSCTShapeManager::Get()->ParseChildrenShapes(CoverDoorTemplateObj);
				SideHungDoor->ParseFromJson(CoverDoorTemplateObj);
				FSCTShapeManager::Get()->ClearAllChildrenShapes();

				FCoverDoorTypeInfo * NewCoverDoorTypeInfo = new FCoverDoorTypeInfo;
				NewCoverDoorTypeInfo->Id = Id;
				NewCoverDoorTypeInfo->Name = SideHungDoor->GetShapeName();
				NewCoverDoorTypeInfo->ThumbnailUrl = SideHungDoor->GetThumbnailUrl();
				NewCoverDoorTypeInfo->DoorGroupShape = MakeShareable(SideHungDoor);
				FWHCModeGlobalData::CoverDoorTypeMap.Emplace(Id, MakeShareable(NewCoverDoorTypeInfo));

				CoverDoorShapeTemplates.Emplace(SideHungDoor);
			}
			else
				CoverDoorShapeTemplates.Emplace((*CoverDoorTypeInfoPtr)->DoorGroupShape.Get());			
		}

		// 解析抽面模板数据
		TArray<FSCTShape*> DrawerShapeTemplates;
		const TArray<TSharedPtr<FJsonValue>> & DrawerTemplates = WHCModeJsonObj->GetArrayField(TEXT("drawer_templates"));
		for (const auto & DrawerTemplate : DrawerTemplates)
		{
			const TSharedPtr<FJsonObject> & DrawerTemplateObj = DrawerTemplate->AsObject();

			int32 Id = DrawerTemplateObj->GetIntegerField(TEXT("id"));
			TSharedPtr<FCoverDoorTypeInfo> * DrawerTypeInfoPtr = FWHCModeGlobalData::DrawerTypeMap.Find(Id);
			if (DrawerTypeInfoPtr == nullptr)
			{
				FDrawerDoorShape * DrawerDoor = new FDrawerDoorShape;
				FSCTShapeManager::Get()->ParseChildrenShapes(DrawerTemplateObj);
				DrawerDoor->ParseFromJson(DrawerTemplateObj);
				FSCTShapeManager::Get()->ClearAllChildrenShapes();

				FCoverDoorTypeInfo * NewDrawerTypeInfo = new FCoverDoorTypeInfo;
				NewDrawerTypeInfo->Id = Id;
				NewDrawerTypeInfo->Name = DrawerDoor->GetShapeName();
				NewDrawerTypeInfo->ThumbnailUrl = DrawerDoor->GetThumbnailUrl();
				NewDrawerTypeInfo->DoorGroupShape = MakeShareable(DrawerDoor);
				FWHCModeGlobalData::DrawerTypeMap.Emplace(Id, MakeShareable(NewDrawerTypeInfo));

				DrawerShapeTemplates.Emplace(DrawerDoor);
			}
			else
				DrawerShapeTemplates.Emplace((*DrawerTypeInfoPtr)->DoorGroupShape.Get());			
		}

		// 解析替换的柜子材质数据
		TArray<FMtlInfo*> ReplacedCabMtls;
		const TArray<TSharedPtr<FJsonValue>> & ReplacedCabMtlsJsonArr = WHCModeJsonObj->GetArrayField(TEXT("replaced_materials"));
		for (const auto & ReplacedCabMtlJson : ReplacedCabMtlsJsonArr)
		{
			const TSharedPtr<FJsonObject> & ReplacedCabMtlJsonObj = ReplacedCabMtlJson->AsObject();

			int32 Id = ReplacedCabMtlJsonObj->GetIntegerField(TEXT("id"));
			TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
			if (MtlInfoPtr == nullptr)
			{
				FMtlInfo * NewMtlInfo = new FMtlInfo;
				NewMtlInfo->Deserialize(ReplacedCabMtlJsonObj);

				FWHCModeGlobalData::CabinetMtlMap.Emplace(Id, MakeShareable(NewMtlInfo));
				ReplacedCabMtls.Emplace(NewMtlInfo);
			}
			else
				ReplacedCabMtls.Emplace((*MtlInfoPtr).Get());
		}

		// 解析替换的柜子五金数据
		TArray<FStandaloneCabAccInfo*> ReplacedCabAccs;
		const TArray<TSharedPtr<FJsonValue>> & ReplacedCabAccsJsonArr = WHCModeJsonObj->GetArrayField(TEXT("replaced_accessories"));
		for (const auto & ReplacedCabAccJson : ReplacedCabAccsJsonArr)
		{
			const TSharedPtr<FJsonObject> & ReplacedCabAccJsonObj = ReplacedCabAccJson->AsObject();

			int32 Id = ReplacedCabAccJsonObj->GetIntegerField(TEXT("id"));
			TSharedPtr<FStandaloneCabAccInfo> * CabAccInfoPtr = FWHCModeGlobalData::StandaloneAccessoryMap.Find(Id);
			if (CabAccInfoPtr == nullptr)
			{
				FAccessShapeForBim *AccForBim = new FAccessShapeForBim;
				AccForBim->ParseFromJson(ReplacedCabAccJsonObj);
				FStandaloneCabAccInfo * AccInfo = new FStandaloneCabAccInfo;
				AccInfo->AccShape = MakeShareable(AccForBim);
				FWHCModeGlobalData::StandaloneAccessoryMap.Emplace(Id, MakeShareable(AccInfo));
				ReplacedCabAccs.Emplace(AccInfo);
			}
			else
				ReplacedCabAccs.Emplace((*CabAccInfoPtr).Get());
		}

		const TArray<TSharedPtr<FJsonValue>> &CabPlatforms = WHCModeJsonObj->GetArrayField(TEXT("cab_platforms"));
		for (const auto &CabPlatform : CabPlatforms)
		{
			const TSharedPtr<FJsonObject> &CabPlatformObj = CabPlatform->AsObject();
			int32 Id = CabPlatformObj->GetIntegerField(TEXT("id"));
			if (!FWHCModeGlobalData::PlatformMap.Contains(Id))
			{
				FPlatformInfo *NewPlatformInfo = new FPlatformInfo;
				NewPlatformInfo->Deserialize(CabPlatformObj);
				FWHCModeGlobalData::PlatformMap.Emplace(Id, MakeShareable(NewPlatformInfo));
			}
		}

		TSet<int32> CabPlatformIds;
		TArray<TPair<int32, int32>> CabVisibilityFlags;
		TArray<TPair<int32, int32>> OverlappedPairs, AlignedPairs;
		TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = CabMgr->GetPlaceShapes();
		// 解析柜子实例
		const TArray<TSharedPtr<FJsonValue>> &CabinetInstances = WHCModeJsonObj->GetArrayField(TEXT("cabinets"));
		int32 CabIndex = 0;
		for (const auto &CabinetInstance : CabinetInstances)
		{
			const TSharedPtr<FJsonObject> &CabInstJsonObj = CabinetInstance->AsObject();
			int32 Id = CabInstJsonObj->GetIntegerField(TEXT("id"));
			// 查找实例应该使用的模板
			TSharedPtr<FSCTShape> *ShapeTemplate = FWHCModeGlobalData::CabinetTemplateMap.Find(Id);
			if (ShapeTemplate != nullptr)
			{
				FShapeInRoom *NewShape = new FShapeInRoom;
				int32 CabVisibility = CabInstJsonObj->GetIntegerField(TEXT("visibility"));
				FGuid::Parse(CabInstJsonObj->GetStringField(TEXT("room_guid")), NewShape->RoomAttachedIndex);
				NewShape->WallAttachedIndex = CabInstJsonObj->GetIntegerField(TEXT("wall_index"));
				NewShape->Shape = FSCTShapeManager::Get()->CopyShapeToNew(*ShapeTemplate);

				float Width = CabInstJsonObj->GetNumberField(TEXT("width"));
				NewShape->Shape->SetShapeWidth(Width);
				float Depth = CabInstJsonObj->GetNumberField(TEXT("depth"));
				NewShape->Shape->SetShapeDepth(Depth);
				float Height = CabInstJsonObj->GetNumberField(TEXT("height"));
				NewShape->Shape->SetShapeHeight(Height);

				double HollowWidth = 0.0, HollowDepth = 0.0, HollowHeight = 0.0;
				if (CabInstJsonObj->TryGetNumberField(TEXT("hollowWidth"), HollowWidth) &&
					CabInstJsonObj->TryGetNumberField(TEXT("hollowDepth"), HollowDepth) &&
					CabInstJsonObj->TryGetNumberField(TEXT("hollowHeight"), HollowHeight))
				{
					FSCTShapeUtilityTool::ResizeSmokeCabinetSpaceRangeWithExpectValue(NewShape->Shape.Get(), HollowWidth, HollowDepth, HollowHeight);
				}

				const TArray<TSharedPtr<FJsonValue>> &LabelsJson = CabInstJsonObj->GetArrayField(TEXT("cabinet_labels"));
				for (const auto &LabelJson : LabelsJson)
					NewShape->Labels.Emplace((ECabinetLabelType)((int32)LabelJson->AsNumber()));

				const TSharedPtr<FJsonObject> * AccessoryJson = nullptr;
				int32 AccVisibility = 0;
				if (CabInstJsonObj->TryGetObjectField(TEXT("cabinet_accessory"), AccessoryJson))
				{
					AccVisibility = (*AccessoryJson)->GetIntegerField(TEXT("visibility"));
					TSharedPtr<FCabinetWholeAccInfo> CabAccInfo = MakeShareable(new FCabinetWholeAccInfo);
					CabAccInfo->Deserialize(*AccessoryJson);
					TArray<TSharedPtr<FCabinetComponent>> Accessories;
					NewShape->InitializeAccessories(CabAccInfo.Get(), Accessories);
				}

				const TArray<TSharedPtr<FJsonValue>> &LocationJson = CabInstJsonObj->GetArrayField(TEXT("location"));
				FVector Location(
					LocationJson[0]->AsNumber(),
					LocationJson[1]->AsNumber(),
					LocationJson[2]->AsNumber()
				);
				const TArray<TSharedPtr<FJsonValue>> &RotationJson = CabInstJsonObj->GetArrayField(TEXT("rotation"));
				FRotator Rotation(
					RotationJson[0]->AsNumber(),
					RotationJson[1]->AsNumber(),
					RotationJson[2]->AsNumber()
				);
				NewShape->AboveGround = Location.Z;

				NewShape->SpawnShapeFrame();
				// NewShape->SpawnShapeWidthRuler();

				NewShape->ShapeFrame->SetActorLocation(Location);
				NewShape->ShapeFrame->SetActorRotation(Rotation);

				const TArray<TSharedPtr<FJsonValue>> & outer_boards = CabInstJsonObj->GetArrayField(TEXT("outer_boards"));
				for (int i = 0; i < outer_boards.Num(); ++i)
				{
					const auto &board = outer_boards[i]->AsObject();
					FShapeInRoom::FDecBoardInfo DecBoardInfo;
					// FSCTShapeUtilityTool::FVeneerdSudeBoardInfo BoardInfo;
					DecBoardInfo.Face = board->GetIntegerField(TEXT("position"));
					DecBoardInfo.Spawned = board->GetIntegerField(TEXT("valid"));
					DecBoardInfo.MtlId = board->GetIntegerField(TEXT("material_id"));
					NewShape->DecBoardInfos.Emplace(DecBoardInfo);
				}

				const TArray<TSharedPtr<FJsonValue>> * ChangeListJsonArr = nullptr;
				if (CabInstJsonObj->TryGetArrayField(TEXT("change_list"), ChangeListJsonArr))
				{
					for (const auto &ChangeJson : *ChangeListJsonArr)
					{
						const TSharedPtr<FJsonObject> & ChangeJsonObj = ChangeJson->AsObject();
						IArmyWHCModeShapeChange * ChangeInst = IArmyWHCModeShapeChange::CreateInstance(ChangeJsonObj->GetStringField(TEXT("type")), NewShape->Shape.Get());
						ChangeInst->Deserialize(ChangeJsonObj);
						NewShape->ChangeList.Emplace(MakeShareable(ChangeInst));
					}
				}

				if (CabInstJsonObj->HasField(TEXT("platform_id")))
				{
					int32 PlatformId = CabInstJsonObj->GetIntegerField(TEXT("platform_id"));
					const TSharedPtr<FPlatformInfo> *PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(PlatformId);
					check(PlatformInfoPtr != nullptr);
					NewShape->InitializePlatform(*PlatformInfoPtr);
					CabPlatformIds.Emplace(PlatformId);
				}

				if (CabinetOperation->IsFrontBoardCabinet(NewShape))
					CabinetOperation->AddFrontBoardCabinet(NewShape);

				int32 PassiveIndex = -1;
				if (CabInstJsonObj->TryGetNumberField(TEXT("overlapped_passive"), PassiveIndex))
					OverlappedPairs.Emplace(TPairInitializer<int32, int32>(CabIndex, PassiveIndex));
				else if (CabInstJsonObj->TryGetNumberField(TEXT("aligned_passive"), PassiveIndex))
					AlignedPairs.Emplace(TPairInitializer<int32, int32>(CabIndex, PassiveIndex));

				PlacedShapes.Emplace(MakeShareable(NewShape));

				CabVisibilityFlags.Emplace(TPairInitializer<int32, int32>(CabVisibility, AccVisibility));

				++CabIndex;
			}
		}

		// 台面
		const TSharedPtr<FJsonObject> & PlatformJsonObj = WHCModeJsonObj->GetObjectField(TEXT("platform"));
		CabMgr->SetGenTable(PlatformJsonObj->GetIntegerField(TEXT("valid")) != 0);
		int32 Id = PlatformJsonObj->GetIntegerField(TEXT("id"));
		FPlatformInfo * UsedPlatformInfo = nullptr; 
		FTableInfo * UsedFrontInfo = nullptr;
		if (Id > 0)
		{
			const TSharedPtr<FPlatformInfo> * PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
			if (PlatformInfoPtr == nullptr)
			{
				FPlatformInfo * PlatformInfo = new FPlatformInfo;
				PlatformInfo->Deserialize(PlatformJsonObj);
				UsedPlatformInfo = PlatformInfo;
				FWHCModeGlobalData::PlatformMap.Emplace(PlatformInfo->Id, MakeShareable(PlatformInfo));
			}
			else
				UsedPlatformInfo = (*PlatformInfoPtr).Get();

			int32 FrontSectionId = PlatformJsonObj->GetIntegerField(TEXT("front_shape_id"));
			if (FrontSectionId > 0)
			{
				const TSharedPtr<FTableInfo> * TableInfoPtr = UsedPlatformInfo->FrontSections.FindByPredicate(
					[FrontSectionId](const TSharedPtr<FTableInfo> TableInfo)
					{ return FrontSectionId == TableInfo->Id; }
				);
				check(TableInfoPtr != nullptr);
				UsedFrontInfo = (*TableInfoPtr).Get();
			}
		}

		// 踢脚板
		const TSharedPtr<FJsonObject> & ToeJsonObj = WHCModeJsonObj->GetObjectField(TEXT("toe"));
		CabMgr->SetGenToe(ToeJsonObj->GetIntegerField(TEXT("valid")) != 0);

		const TSharedPtr<FJsonObject> * ToeMtlJsonObj = nullptr;
		int32 ToeMtlId = 0;
		FMtlInfo * UsedToeMtlInfo = nullptr;
		if (ToeJsonObj->TryGetObjectField(TEXT("material"), ToeMtlJsonObj))
		{
			const TSharedPtr<FJsonObject> & ToeMtlJsonObjRef = *ToeMtlJsonObj;
			ToeMtlId = ToeMtlJsonObjRef->GetIntegerField(TEXT("id"));
			const TSharedPtr<FMtlInfo> * ToeMtlInfoPtr = FWHCModeGlobalData::ToeMtlMap.Find(ToeMtlId);
			if (ToeMtlInfoPtr == nullptr)
			{
				FMtlInfo * NewMtlInfo = new FMtlInfo;
				NewMtlInfo->Deserialize(ToeMtlJsonObjRef);
				FWHCModeGlobalData::ToeMtlMap.Emplace(ToeMtlId, MakeShareable(NewMtlInfo));
				UsedToeMtlInfo = NewMtlInfo;
			}
			else
				UsedToeMtlInfo = (*ToeMtlInfoPtr).Get();
		}

		const TSharedPtr<FJsonObject> * ToeShapeJsonObj = nullptr;
		FTableInfo * UsedToeInfo = nullptr;
		if (ToeJsonObj->TryGetObjectField(TEXT("shape"), ToeShapeJsonObj))
		{
			const TSharedPtr<FJsonObject> & ToeShapeJsonObjRef = *ToeShapeJsonObj;
			int32 TempId = ToeShapeJsonObjRef->GetIntegerField(TEXT("id"));
			const TSharedPtr<FTableInfo> * TableInfoPtr = FWHCModeGlobalData::ToeSections.Find(TempId);		
			if (TableInfoPtr == nullptr)
			{
				FTableInfo * NewTableInfo = new FTableInfo;
				NewTableInfo->Deserialize(ToeShapeJsonObjRef);
				NewTableInfo->mtlId = ToeMtlId;
				FWHCModeGlobalData::ToeSections.Emplace(TempId, MakeShareable(NewTableInfo));
				UsedToeInfo = NewTableInfo;
			}
			else
				UsedToeInfo = (*TableInfoPtr).Get();
		}

		// 上线条
		const TSharedPtr<FJsonObject> & TopJsonObj = WHCModeJsonObj->GetObjectField(TEXT("upper_line"));
		CabMgr->SetGenTop(TopJsonObj->GetIntegerField(TEXT("valid")) != 0);
		
		FMtlInfo * UsedTopMtlInfo = nullptr;
		const TSharedPtr<FJsonObject> * TopMtlJsonObj = nullptr;
		int32 TopMtlId = 0;
		if (TopJsonObj->TryGetObjectField(TEXT("material"), TopMtlJsonObj))
		{
			const TSharedPtr<FJsonObject> & TopMtlJsonObjRef = *TopMtlJsonObj;
			TopMtlId = TopMtlJsonObjRef->GetIntegerField(TEXT("id"));
			const TSharedPtr<FMtlInfo> * TopMtlInfoPtr = FWHCModeGlobalData::TopMtlMap.Find(TopMtlId);
			if (TopMtlInfoPtr == nullptr)
			{
				FMtlInfo * NewMtlInfo = new FMtlInfo;
				NewMtlInfo->Deserialize(TopMtlJsonObjRef);
				FWHCModeGlobalData::TopMtlMap.Emplace(TopMtlId, MakeShareable(NewMtlInfo));
				UsedTopMtlInfo = NewMtlInfo;
			}
			else
				UsedTopMtlInfo = (*TopMtlInfoPtr).Get();
		}

		const TSharedPtr<FJsonObject> * TopShapeJsonObj = nullptr;
		FTableInfo * UsedTopInfo = nullptr;
		if (TopJsonObj->TryGetObjectField(TEXT("shape"), TopShapeJsonObj))
		{
			const TSharedPtr<FJsonObject> & TopShapeJsonObjRef = *TopShapeJsonObj;
			int32 TempId = TopShapeJsonObjRef->GetIntegerField(TEXT("id"));
			const TSharedPtr<FTableInfo> * TableInfoPtr = FWHCModeGlobalData::TopSections.Find(TempId);
			if (TableInfoPtr == nullptr)
			{
				FTableInfo * NewTableInfo = new FTableInfo;
				NewTableInfo->Deserialize(TopShapeJsonObjRef);
				NewTableInfo->mtlId = TopMtlId;
				FWHCModeGlobalData::TopSections.Emplace(TempId, MakeShareable(NewTableInfo));
				UsedTopInfo = NewTableInfo;
			}
			else
				UsedTopInfo = (*TableInfoPtr).Get();
		}

		CabMgr->SetGenWaist((WHCModeJsonObj->GetObjectField(TEXT("lower_line"))->GetIntegerField(TEXT("valid"))) != 0);

		TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;

		struct FFindSameResourceFunctor {
			FString CheckResource;
			FFindSameResourceFunctor(const FString & InCheckResource) : CheckResource(InCheckResource) {}
			bool operator() (const FDownloadSpace::FDownloadFileInfo & FileInfo) const
			{
				return FileInfo.FilePath == CheckResource;
			}
		};

		// 柜子资源
		for (const auto & ShapeTemplate : ShapeTemplates)
		{
			if (ShapeTemplate == nullptr)
				continue;

			TArray<FString> ResourceUrls;
			TArray<FString> FileCachePaths;
			ShapeTemplate->GetResourceUrls(ResourceUrls);
            ShapeTemplate->GetFileCachePaths(FileCachePaths);
            ShapeTemplate->CheckResourceUrlsAndCachePaths(ResourceUrls, FileCachePaths);

			for (int32 i = 0; i < ResourceUrls.Num(); ++i)
			{
				if (FileInfos.FindByPredicate(FFindSameResourceFunctor(FileCachePaths[i])) == nullptr)
				{
					FileInfos.Emplace(
						i, 
						FileCachePaths[i],
						ResourceUrls[i],
						FString(),
						true
					);
				}
			}
		}

		// 移门门板资源
		for (const auto & SlidingDoorShapeTemplate : SlidingDoorShapeTemplates)
		{
			if (SlidingDoorShapeTemplate == nullptr)
				continue;
			
			TArray<FString> ResourceUrls;
			TArray<FString> FileCachePaths;
			SlidingDoorShapeTemplate->GetResourceUrls(ResourceUrls);
            SlidingDoorShapeTemplate->GetFileCachePaths(FileCachePaths);
            SlidingDoorShapeTemplate->CheckResourceUrlsAndCachePaths(ResourceUrls, FileCachePaths);

			for (int32 i = 0; i < ResourceUrls.Num(); ++i)
			{
				if (FileInfos.FindByPredicate(FFindSameResourceFunctor(FileCachePaths[i])) == nullptr)
				{
					FileInfos.Emplace(
						i, 
						FileCachePaths[i],
						ResourceUrls[i],
						FString(),
						true
					);
				}
			}
		}

		// 掩门门板资源
		for (const auto & CoverDoorShapeTemplate : CoverDoorShapeTemplates)
		{
			if (CoverDoorShapeTemplate == nullptr)
				continue;
			
			TArray<FString> ResourceUrls;
			TArray<FString> FileCachePaths;
			CoverDoorShapeTemplate->GetResourceUrls(ResourceUrls);
            CoverDoorShapeTemplate->GetFileCachePaths(FileCachePaths);
            CoverDoorShapeTemplate->CheckResourceUrlsAndCachePaths(ResourceUrls, FileCachePaths);

			for (int32 i = 0; i < ResourceUrls.Num(); ++i)
			{
				if (FileInfos.FindByPredicate(FFindSameResourceFunctor(FileCachePaths[i])) == nullptr)
				{
					FileInfos.Emplace(
						i, 
						FileCachePaths[i],
						ResourceUrls[i],
						FString(),
						true
					);
				}
			}
		}

		// 抽面资源
		for (const auto & DrawerShapeTemplate : DrawerShapeTemplates)
		{
			if (DrawerShapeTemplate == nullptr)
				continue;

			TArray<FString> ResourceUrls;
			TArray<FString> FileCachePaths;
			DrawerShapeTemplate->GetResourceUrls(ResourceUrls);
            DrawerShapeTemplate->GetFileCachePaths(FileCachePaths);
            DrawerShapeTemplate->CheckResourceUrlsAndCachePaths(ResourceUrls, FileCachePaths);

			for (int32 i = 0; i < ResourceUrls.Num(); ++i)
			{
				if (FileInfos.FindByPredicate(FFindSameResourceFunctor(FileCachePaths[i])) == nullptr)
				{
					FileInfos.Emplace(
						i, 
						FileCachePaths[i],
						ResourceUrls[i],
						FString(),
						true
					);
				}
			}
		}

		// 柜子附属件资源
		for (auto & PlacedShape : PlacedShapes)
		{
			if (!PlacedShape->CabinetActor.IsValid())
				continue;
			XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PlacedShape->CabinetActor;
			TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
			if (Accessory.IsValid())
			{
				FString CachePath = Accessory->CabAccInfo->GetCachePath();
				if (FileInfos.FindByPredicate(FFindSameResourceFunctor(CachePath)) == nullptr)
				{
					FileInfos.Emplace(
						Accessory->CabAccInfo->Id, 
						CachePath,
						Accessory->CabAccInfo->PakUrl,
						Accessory->CabAccInfo->PakMd5,
						false
					);
				}
				for (const auto & SubAcc : Accessory->AttachedSubAccessories)
				{
					CachePath = SubAcc->CabAccInfo->GetCachePath();
					if (FileInfos.FindByPredicate(FFindSameResourceFunctor(CachePath)) == nullptr)
					{
						FileInfos.Emplace(
							SubAcc->CabAccInfo->Id, 
							CachePath,
							SubAcc->CabAccInfo->PakUrl,
							SubAcc->CabAccInfo->PakMd5,
							false
						);
					}
				}
			}
		}

		// 柜子材质资源
		for (const auto & ReplacedCabMtl : ReplacedCabMtls)
		{
			if (ReplacedCabMtl == nullptr)
				continue;
			FString CachePath = FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, ReplacedCabMtl->Id);
			if (FileInfos.FindByPredicate(FFindSameResourceFunctor(CachePath)) == nullptr)
			{
				FileInfos.Emplace(
					ReplacedCabMtl->Id,
					CachePath,
					ReplacedCabMtl->MtlUrl,
					ReplacedCabMtl->MtlMd5,
					false
				);
			}
		}

		// 柜子五金资源
		for (const auto & ReplacedCabAcc : ReplacedCabAccs)
		{
			if (ReplacedCabAcc == nullptr)
				continue;
			TArray<FString> ResourceUrls;
			TArray<FString> FileCachePaths;
			ReplacedCabAcc->AccShape->GetResourceUrls(ResourceUrls);
			ReplacedCabAcc->AccShape->GetFileCachePaths(FileCachePaths);
			ReplacedCabAcc->AccShape->CheckResourceUrlsAndCachePaths(ResourceUrls, FileCachePaths);

			for (int32 i = 0; i < ResourceUrls.Num(); ++i)
			{
				if (FileInfos.FindByPredicate(FFindSameResourceFunctor(FileCachePaths[i])) == nullptr)
				{
					FileInfos.Emplace(
						i, 
						FileCachePaths[i],
						ResourceUrls[i],
						FString(),
						true
					);
				}
			}
		}

		// 橱柜台面资源
		if (UsedPlatformInfo != nullptr)
		{
			FileInfos.Emplace(
				UsedPlatformInfo->PlatformMtl->Id, 
				UsedPlatformInfo->PlatformMtl->GetCachedPath(),
				UsedPlatformInfo->PlatformMtl->MtlUrl,
				UsedPlatformInfo->PlatformMtl->MtlMd5,
				false
			);
			if (UsedFrontInfo != nullptr)
			{
				FileInfos.Emplace(
					UsedFrontInfo->Id,
					UsedFrontInfo->GetCachedPath(),
					UsedFrontInfo->cadUrl,
					FString(),
					true
				);
			}
		}

		// 柜子台面资源
		TSet<int32>::TIterator PlatformIdIter(CabPlatformIds);
		for (; PlatformIdIter; ++PlatformIdIter)
		{
			int32 Id = *PlatformIdIter;
			const TSharedPtr<FPlatformInfo> *PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
			check(PlatformInfoPtr != nullptr);
			const TSharedPtr<FPlatformInfo> &PlatformInfo = *PlatformInfoPtr;
			if (PlatformInfo->PlatformMtl.IsValid())
			{
				FileInfos.Emplace(
					PlatformInfo->PlatformMtl->Id, 
					PlatformInfo->PlatformMtl->GetCachedPath(),
					PlatformInfo->PlatformMtl->MtlUrl,
					PlatformInfo->PlatformMtl->MtlMd5,
					false
				);
			}
			if (PlatformInfo->FrontSections.Num() > 0)
			{
				const TSharedPtr<FTableInfo> &TableInfo = PlatformInfo->FrontSections[0];
				FileInfos.Emplace(
					TableInfo->Id, 
					TableInfo->GetCachedPath(),
					TableInfo->cadUrl,
					FString(),
					true
				);
			}
			if (PlatformInfo->BackSections.Num() > 0)
			{
				const TSharedPtr<FTableInfo> &TableInfo = PlatformInfo->BackSections[0];
				FileInfos.Emplace(
					TableInfo->Id, 
					TableInfo->GetCachedPath(),
					TableInfo->cadUrl,
					FString(),
					true
				);
			}
		}

		// 踢脚板资源
		if (UsedToeInfo != nullptr)
		{
			FileInfos.Emplace(
				UsedToeInfo->Id, 
				UsedToeInfo->GetCachedPath(),
				UsedToeInfo->cadUrl,
				FString(),
				true
			);
			if (UsedToeMtlInfo != nullptr)
			{
				FileInfos.Emplace(
					UsedToeMtlInfo->Id, 
					UsedToeMtlInfo->GetCachedPath(),
					UsedToeMtlInfo->MtlUrl,
					UsedToeMtlInfo->MtlMd5,
					false
				);
			}
		}

		// 上线条资源
		if (UsedTopInfo != nullptr)
		{
			FileInfos.Emplace(
				UsedTopInfo->Id, 
				UsedTopInfo->GetCachedPath(),
				UsedTopInfo->cadUrl,
				FString(),
				true
			);
			if (UsedTopMtlInfo != nullptr)
			{
				FileInfos.Emplace(
					UsedTopMtlInfo->Id, 
					UsedTopMtlInfo->GetCachedPath(),
					UsedTopMtlInfo->MtlUrl,
					UsedTopMtlInfo->MtlMd5,
					false
				);
			}
		}

		// 构件资源
		TMap<int32, FCabinetWholeComponent*>::TIterator CompIter(ShapeWholeComponents);
		for (; CompIter; ++CompIter)
		{
			// 添加柜子所需构件下载任务
			FCabinetWholeComponent * WholeComponent = CompIter.Value();
			if (WholeComponent->Cabinet.IsValid())
			{
				if (WholeComponent->Cabinet->TopResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(WholeComponent->Cabinet->GetCachePath(1))) == nullptr)
				{
					FileInfos.Emplace(WholeComponent->Cabinet->Id,
						WholeComponent->Cabinet->GetCachePath(1),
						WholeComponent->Cabinet->TopResourceUrl,
						WholeComponent->Cabinet->TopResourceMd5,
						false);
				}
				if (WholeComponent->Cabinet->FrontResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(WholeComponent->Cabinet->GetCachePath(2))) == nullptr)
				{
					FileInfos.Emplace(WholeComponent->Cabinet->Id,
						WholeComponent->Cabinet->GetCachePath(2),
						WholeComponent->Cabinet->FrontResourceUrl,
						WholeComponent->Cabinet->FrontResourceMd5,
						false);
				}
				if (WholeComponent->Cabinet->SideResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(WholeComponent->Cabinet->GetCachePath(3))) == nullptr)
				{
					FileInfos.Emplace(WholeComponent->Cabinet->Id,
						WholeComponent->Cabinet->GetCachePath(3),
						WholeComponent->Cabinet->SideResourceUrl,
						WholeComponent->Cabinet->SideResourceMd5,
						false);
				}
			}
			// 添加柜子附属品构件下载任务
			if (WholeComponent->Accessorys.Num() > 0)
			{
				for (auto Accessory : WholeComponent->Accessorys)
				{
					if (Accessory->TopResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(Accessory->GetCachePath(1))) == nullptr)
					{
						FileInfos.Emplace(Accessory->Id,
							Accessory->GetCachePath(1),
							Accessory->TopResourceUrl,
							Accessory->TopResourceMd5,
							false);
					}
					if (Accessory->FrontResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(Accessory->GetCachePath(2))) == nullptr)
					{
						FileInfos.Emplace(Accessory->Id,
							Accessory->GetCachePath(2),
							Accessory->FrontResourceUrl,
							Accessory->FrontResourceMd5,
							false);
					}
					if (Accessory->SideResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(Accessory->GetCachePath(3))) == nullptr)
					{
						FileInfos.Emplace(Accessory->Id,
							Accessory->GetCachePath(3),
							Accessory->SideResourceUrl,
							Accessory->SideResourceMd5,
							false);
					}
				}
			}
			// 添加柜子门板构件下载任务
			if (WholeComponent->DoorSheets.Num() > 0)
			{
				for (auto DoorSheet : WholeComponent->DoorSheets)
				{
					if (DoorSheet->TopResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(DoorSheet->GetCachePath(1))) == nullptr)
					{
						FileInfos.Emplace(DoorSheet->Id,
							DoorSheet->GetCachePath(1),
							DoorSheet->TopResourceUrl,
							DoorSheet->TopResourceMd5,
							false);
					}
					if (DoorSheet->FrontResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(DoorSheet->GetCachePath(2))) == nullptr)
					{
						FileInfos.Emplace(DoorSheet->Id,
							DoorSheet->GetCachePath(2),
							DoorSheet->FrontResourceUrl,
							DoorSheet->FrontResourceMd5,
							false);
					}
					if (DoorSheet->SideResourceUrl.Len() > 0 && FileInfos.FindByPredicate(FFindSameResourceFunctor(DoorSheet->GetCachePath(3))) == nullptr)
					{
						FileInfos.Emplace(DoorSheet->Id,
							DoorSheet->GetCachePath(3),
							DoorSheet->SideResourceUrl,
							DoorSheet->SideResourceMd5,
							false);
					}
				}
			}
		}

		GGI->Window->AlwaysShowThrobber(FText::FromString(TEXT("加载木作数据")));
		TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(FileInfos);
		NewTask->OnDownloadFinished.BindLambda(
			[this, CabVisibilityFlags, UsedPlatformInfo, UsedFrontInfo, UsedToeInfo, UsedToeMtlInfo, UsedTopInfo, UsedTopMtlInfo, ShapeWholeComponents, OverlappedPairs, AlignedPairs](bool bSucceeded) {
				if (bSucceeded)
				{
					FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get();
					CabMgr->InitRoom();
					CabMgr->ResetPlatformInfo();
					CabMgr->ResetFrontTrim();
					CabMgr->ResetTopTrim();
					CabMgr->ResetToeTrim();

					int32 Index = 0;
					TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = CabMgr->GetPlaceShapes();
					for (auto &PlacedShape : PlacedShapes)
					{
						int32 Id = PlacedShape->Shape->GetShapeId();
						FCabinetWholeComponent * const * CabWholeComp = ShapeWholeComponents.Find(Id);

						TArray<FShapeInRoom::FDecBoardInfo> TempInfos = MoveTemp(PlacedShape->DecBoardInfos);

						PlacedShape->InitializeSelf((*CabWholeComp)->Cabinet.Get());
						PlacedShape->SpawnShape();

						// 执行柜子的所有修改
						for (auto & Change : PlacedShape->ChangeList)
							Change->ProcessChange();
						
						PlacedShape->PrepareShapeAnimations();

						CabMgr->OnShapeInited(PlacedShape.Get());

						// 处理见光板
						for (const auto &TempInfo : TempInfos)
						{
							if (TempInfo.Spawned)
							{
								PlacedShape->SetSunBoard(TempInfo.Face);
								const TSharedPtr<FMtlInfo> *MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(TempInfo.MtlId);
								if (MtlInfoPtr != nullptr)
									PlacedShape->ChangeSunBoardMtl((*MtlInfoPtr).Get());
							}
							else
								PlacedShape->UnsetSunBoard(TempInfo.Face);
						}

						PlacedShape->SpawnAccessories();
						PlacedShape->SpawnShapeLegs();
						PlacedShape->SpawnPlatform();
						PlacedShape->UpdateComponents();

						PlacedShape->MakeImmovable();
						FSCTShapeUtilityTool::SetActorTag(PlacedShape->Shape->GetShapeActor(), XRActorTag::WHCActor);

						FCabinetShape *CabShape = static_cast<FCabinetShape*>(PlacedShape->Shape.Get());
						if (CabVisibilityFlags[Index].Key == 1)
						{
							CabShape->HiddenCabinetShapeActors(true);
							FSCTShapeUtilityTool::SetActorTag(CabShape->GetShapeActor(), XRActorTag::WHCAlwaysHidden);
						}
						if (PlacedShape->CabinetActor.IsValid())
						{
							XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PlacedShape->CabinetActor;
							TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
							if (Accessory.IsValid() && Accessory->Actor != nullptr && CabVisibilityFlags[Index].Value == 1)
							{
								Accessory->Actor->SetActorHiddenInGame(true);
								Accessory->Actor->Tags.Emplace(XRActorTag::WHCAlwaysHidden);
								for (auto &SubAcc : Accessory->AttachedSubAccessories)
								{
									if (SubAcc->Actor != nullptr)
									{
										SubAcc->Actor->SetActorHiddenInGame(true);
										SubAcc->Actor->Tags.Emplace(XRActorTag::WHCAlwaysHidden);
									}
								}
							}
						}

						++Index;
					}

					if (UsedPlatformInfo != nullptr)
					{
						CabMgr->SetPlatformInfo(UsedPlatformInfo);
						if (UsedPlatformInfo->PlatformMtl->Mtl == nullptr)
						{
							UsedPlatformInfo->PlatformMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
								UsedPlatformInfo->PlatformMtl->GetCachedPath(), UsedPlatformInfo->PlatformMtl->MtlParam
							);
						}

						if (UsedFrontInfo != nullptr)
						{
							CabMgr->SetFrontTrim(UsedFrontInfo);
						}
					}

					if (UsedToeInfo != nullptr)
					{
						CabMgr->SetToeTrim(UsedToeInfo);
						if (UsedToeMtlInfo != nullptr)
						{
							if (UsedToeMtlInfo->Mtl == nullptr)
							{
								UsedToeMtlInfo->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
									UsedToeMtlInfo->GetCachedPath(), UsedToeMtlInfo->MtlParam
								);
							}
						}
					}

					if (UsedTopInfo != nullptr)
					{
						CabMgr->SetTopTrim(UsedTopInfo);
						if (UsedTopMtlInfo != nullptr)
						{
							if (UsedTopMtlInfo->Mtl == nullptr)
							{
								UsedTopMtlInfo->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
									UsedTopMtlInfo->GetCachedPath(), UsedTopMtlInfo->MtlParam
								);
							}
						}
					}

					for (const auto &OverlappedPair : OverlappedPairs)
						CabinetOperation->AddOverlappedPair(PlacedShapes[OverlappedPair.Key].Get(), PlacedShapes[OverlappedPair.Value].Get());
					for (const auto &AlignedPair : AlignedPairs)
						CabinetOperation->AddAlignedPair(PlacedShapes[AlignedPair.Key].Get(), PlacedShapes[AlignedPair.Value].Get());

					CabMgr->OnResLoaded();

					if (UsedPlatformInfo != nullptr && UsedPlatformInfo->PlatformMtl->Mtl != nullptr)
						CabMgr->RefreshPlatformMtl();
					if (UsedToeMtlInfo != nullptr && UsedToeMtlInfo->Mtl != nullptr)
						CabMgr->SetToeTrimMtl(UsedToeMtlInfo);
					if (UsedTopMtlInfo != nullptr && UsedTopMtlInfo->Mtl != nullptr)
						CabMgr->SetTopTrimMtl(UsedTopMtlInfo);
						
				}
				GGI->Window->AlwaysHideThrobber();
                
                // 加载完木作数据需要隐藏，否则会在户型模式下显示
                GVC->SetDrawMode(DM_2D);
			}
		);
	}
}

bool FArmyWHCModeController::Save(TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter)
{
	FArmyWHCabinetMgr * RESTRICT CabMgr = FArmyWHCabinetMgr::Get();

	JsonWriter->WriteObjectStart(TEXT("WHCMode"));

	TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = CabMgr->GetPlaceShapes();

	// 保存柜子模板
	TArray<int32> PlacedTemplateIds;
	JsonWriter->WriteArrayStart(TEXT("cab_templates"));
	for (auto &PlacedShape : PlacedShapes)
	{
		int32 Id = (int32)PlacedShape->Shape->GetShapeId();
		if (!PlacedTemplateIds.Contains(Id))
		{
			TSharedPtr<FSCTShape> *ShapeTemplate = FWHCModeGlobalData::CabinetTemplateMap.Find(Id);
			if (ShapeTemplate != nullptr)
			{
				JsonWriter->WriteObjectStart();

				// 柜子完整数据
				(*ShapeTemplate)->SaveToJson(JsonWriter);

				// 柜子构件信息
				TSharedPtr<FCabinetWholeComponent> * Result = FWHCModeGlobalData::CabinetComponentMap.Find(Id);
				if (Result != nullptr)
				{
					TSharedPtr<FCabinetWholeComponent> & WholeComponent = *Result;
					// 柜子自身构件信息
					if (WholeComponent->Cabinet.IsValid())
					{
						JsonWriter->WriteValue(TEXT("component_id"), WholeComponent->Cabinet->Id);
						JsonWriter->WriteValue(TEXT("component_type"), WholeComponent->Cabinet->Type);
						JsonWriter->WriteValue(TEXT("component_url"), WholeComponent->Cabinet->TopResourceUrl);
						JsonWriter->WriteValue(TEXT("component_md5"), WholeComponent->Cabinet->TopResourceMd5);
						JsonWriter->WriteValue(TEXT("component_Fronturl"), WholeComponent->Cabinet->FrontResourceUrl);
						JsonWriter->WriteValue(TEXT("component_Frontmd5"), WholeComponent->Cabinet->FrontResourceMd5);
						JsonWriter->WriteValue(TEXT("component_Sideurl"), WholeComponent->Cabinet->SideResourceUrl);
						JsonWriter->WriteValue(TEXT("component_Sidemd5"), WholeComponent->Cabinet->SideResourceMd5);
					}

					// 柜子附属件构建信息
					JsonWriter->WriteArrayStart(TEXT("acc_accessorycomponents"));
					for (auto Accessory : WholeComponent->Accessorys)
					{
						JsonWriter->WriteObjectStart();
						JsonWriter->WriteValue(TEXT("id"), Accessory->Id);
						JsonWriter->WriteValue(TEXT("type"), Accessory->Type);
						JsonWriter->WriteValue(TEXT("Topurl"), Accessory->TopResourceUrl);
						JsonWriter->WriteValue(TEXT("Topmd5"), Accessory->TopResourceMd5);
						JsonWriter->WriteValue(TEXT("Fronturl"), Accessory->FrontResourceUrl);
						JsonWriter->WriteValue(TEXT("Frontmd5"), Accessory->FrontResourceMd5);
						JsonWriter->WriteValue(TEXT("Sideurl"), Accessory->SideResourceUrl);
						JsonWriter->WriteValue(TEXT("Sidemd5"), Accessory->SideResourceMd5);
						JsonWriter->WriteObjectEnd();
					}
					JsonWriter->WriteArrayEnd();

					// 柜子门板构件信息
					JsonWriter->WriteArrayStart(TEXT("acc_doorsheetcomponents"));
					for (auto DoorSheet : WholeComponent->DoorSheets)
					{
						JsonWriter->WriteObjectStart();
						JsonWriter->WriteValue(TEXT("id"), DoorSheet->Id);
						JsonWriter->WriteValue(TEXT("type"), DoorSheet->Type);
						JsonWriter->WriteValue(TEXT("Topurl"), DoorSheet->TopResourceUrl);
						JsonWriter->WriteValue(TEXT("Topmd5"), DoorSheet->TopResourceMd5);
						JsonWriter->WriteValue(TEXT("Fronturl"), DoorSheet->FrontResourceUrl);
						JsonWriter->WriteValue(TEXT("Frontmd5"), DoorSheet->FrontResourceMd5);
						JsonWriter->WriteValue(TEXT("Sideurl"), DoorSheet->SideResourceUrl);
						JsonWriter->WriteValue(TEXT("Sidemd5"), DoorSheet->SideResourceMd5);
						JsonWriter->WriteObjectEnd();
					}
					JsonWriter->WriteArrayEnd();
					//if (WholeComponent->Accessory.IsValid())
					//{
					//	JsonWriter->WriteObjectStart(TEXT("acc_component"));

					//	JsonWriter->WriteValue(TEXT("id"), WholeComponent->Accessory->Self.Id);
					//	JsonWriter->WriteValue(TEXT("type"), WholeComponent->Accessory->Self.Type);
					//	JsonWriter->WriteValue(TEXT("url"), WholeComponent->Accessory->Self.TopResourceUrl);
					//	JsonWriter->WriteValue(TEXT("md5"), WholeComponent->Accessory->Self.TopResourceMd5);

					//	JsonWriter->WriteArrayStart(TEXT("sub_acc_components"));
					//	for (const auto & SubAcc : WholeComponent->Accessory->Subs)
					//	{
					//		JsonWriter->WriteObjectStart();
					//		JsonWriter->WriteValue(TEXT("id"), SubAcc.Id);
					//		JsonWriter->WriteValue(TEXT("type"), SubAcc.Type);
					//		JsonWriter->WriteValue(TEXT("url"), SubAcc.ResourceUrl);
					//		JsonWriter->WriteValue(TEXT("md5"), SubAcc.ResourceMd5);
					//		JsonWriter->WriteObjectEnd();
					//	}
					//	JsonWriter->WriteArrayEnd();

					//	JsonWriter->WriteObjectEnd();
					//}
				}

				JsonWriter->WriteObjectEnd();
				PlacedTemplateIds.Emplace(Id);
			}
		}
	}
	JsonWriter->WriteArrayEnd();

	// 保存移门板模板
	TArray<int32> SlidingDoorTemplateIds;
	JsonWriter->WriteArrayStart(TEXT("sliding_door_templates"));
	for (const auto &PlacedShape : PlacedShapes)
	{
		int32 Id = 0;
		for (const auto &Change : PlacedShape->ChangeList)
		{
			if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP))
			{
				FArmyWHCSingleDoorGroupChange * SingleDoorGroupChange = static_cast<FArmyWHCSingleDoorGroupChange*>(Change.Get());
				Id = SingleDoorGroupChange->GetDoorTypeTemplateId();
			}

			if (Id > 0 && !SlidingDoorTemplateIds.Contains(Id))
			{
				TSharedPtr<FCoverDoorTypeInfo> * SlidingDoorTypeTemplate = FWHCModeGlobalData::SlidingDoorTypeMap.Find(Id);
				if (SlidingDoorTypeTemplate != nullptr)
				{
					(*SlidingDoorTypeTemplate)->DoorGroupShape->SaveToJson(JsonWriter);
					SlidingDoorTemplateIds.Emplace(Id);
				}
			}
		}
	}
	JsonWriter->WriteArrayEnd();

	// 保存掩门门板模板
	TArray<int32> CoverDoorTemplateIds;
	JsonWriter->WriteArrayStart(TEXT("cover_door_templates"));
	for (const auto &PlacedShape : PlacedShapes)
	{
		int32 Id = 0;
		for (const auto &Change : PlacedShape->ChangeList)
		{
			if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP))
			{
				FArmyWHCSingleDoorGroupChange * SingleDoorGroupChange = static_cast<FArmyWHCSingleDoorGroupChange*>(Change.Get());
				Id = SingleDoorGroupChange->GetDoorTypeTemplateId();
			}

			if (Id > 0 && !CoverDoorTemplateIds.Contains(Id))
			{
				TSharedPtr<FCoverDoorTypeInfo> * CoverDoorTypeTemplate = FWHCModeGlobalData::CoverDoorTypeMap.Find(Id);
				if (CoverDoorTypeTemplate != nullptr)
				{
					JsonWriter->WriteObjectStart();
					(*CoverDoorTypeTemplate)->DoorGroupShape->SaveToJson(JsonWriter);
					JsonWriter->WriteObjectEnd();
					CoverDoorTemplateIds.Emplace(Id);
				}
			}
		}
	}
	JsonWriter->WriteArrayEnd();

	// 保存抽面模板
	TArray<int32> DrawerTemplateIds;
	JsonWriter->WriteArrayStart(TEXT("drawer_templates"));
	for (const auto &PlacedShape : PlacedShapes)
	{
		int32 Id = 0;
		for (const auto &Change : PlacedShape->ChangeList)
		{
			if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP))
			{
				FArmyWHCSingleDrawerGroupChange * SingleDrawerGroupChange = static_cast<FArmyWHCSingleDrawerGroupChange*>(Change.Get());
				Id = SingleDrawerGroupChange->GetDoorTypeTemplateId();
			}

			if (Id > 0 && !DrawerTemplateIds.Contains(Id))
			{
				TSharedPtr<FCoverDoorTypeInfo> * DrawerTypeTemplate = FWHCModeGlobalData::DrawerTypeMap.Find(Id);
				if (DrawerTypeTemplate != nullptr)
				{
					JsonWriter->WriteObjectStart();
					(*DrawerTypeTemplate)->DoorGroupShape->SaveToJson(JsonWriter);
					JsonWriter->WriteObjectEnd();
					DrawerTemplateIds.Emplace(Id);
				}
			}
		}
	}
	JsonWriter->WriteArrayEnd();

	// 保存替换的柜体材质信息
	TArray<int32> ReplacedMtlIds;
	JsonWriter->WriteArrayStart(TEXT("replaced_materials"));
	for (const auto &PlacedShape : PlacedShapes)
	{
		int32 Id = 0;
		for (const auto &Change : PlacedShape->ChangeList)
		{
			if (Change->IsSameType(SHAPE_CHANGE_CABINET_MTL))
			{
				FArmyWHCCabinetMtlChange * CabMtlChange = static_cast<FArmyWHCCabinetMtlChange*>(Change.Get());
				Id = CabMtlChange->GetCabinetMtlId();
			}
			else if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL))
			{
				FArmyWHCSingleDoorGroupMtlChange * SingleDoorMtlChange = static_cast<FArmyWHCSingleDoorGroupMtlChange*>(Change.Get());
				Id = SingleDoorMtlChange->GetDoorMtlId();
			}
			else if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL))
			{
				FArmyWHCSingleDrawerGroupMtlChange * SingleDrawerMtlChange = static_cast<FArmyWHCSingleDrawerGroupMtlChange*>(Change.Get());
				Id = SingleDrawerMtlChange->GetDoorMtlId();
			}
			else if (Change->IsSameType(SHAPE_CHANGE_CABINET_INSERTION_BOARD_MTL))
			{
				FArmyWHCCabinetInsertionBoardMtlChange * InsertionBoardMtlChange = static_cast<FArmyWHCCabinetInsertionBoardMtlChange*>(Change.Get());
				Id = InsertionBoardMtlChange->GetInsertionBoardMtlId();
			}

			if (Id > 0 && !ReplacedMtlIds.Contains(Id))
			{
				TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
				if (MtlInfoPtr != nullptr)
				{
					TSharedPtr<FMtlInfo> &MtlInfo = *MtlInfoPtr;
					JsonWriter->WriteObjectStart();
					MtlInfo->Serialize(JsonWriter);
					JsonWriter->WriteObjectEnd();
					ReplacedMtlIds.Emplace(Id);
				}
			}
		}

		// 见光板使用的材质
		for (const auto &DecBoardInfo : PlacedShape->DecBoardInfos)
		{
			if (DecBoardInfo.MtlId > 0 && !ReplacedMtlIds.Contains(DecBoardInfo.MtlId))
			{
				TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(DecBoardInfo.MtlId);
				if (MtlInfoPtr != nullptr)
				{
					TSharedPtr<FMtlInfo> &MtlInfo = *MtlInfoPtr;
					JsonWriter->WriteObjectStart();
					MtlInfo->Serialize(JsonWriter);
					JsonWriter->WriteObjectEnd();
					ReplacedMtlIds.Emplace(DecBoardInfo.MtlId);
				}	
			}
		}
	}
	JsonWriter->WriteArrayEnd();

	// 保存可替换五金信息
	TArray<int32> StandaloneAccIds;
	JsonWriter->WriteArrayStart(TEXT("replaced_accessories"));
	for (const auto &PlacedShape : PlacedShapes)
	{
		int32 Id = 0;
		for (const auto &Change : PlacedShape->ChangeList)
		{
			if (Change->IsSameType(SHAPE_CHANGE_EMBBED_ELEC_DEV))
			{
				FArmyWHCEmbbedElecDevChange *EmbbedElecDevChange = static_cast<FArmyWHCEmbbedElecDevChange*>(Change.Get());
				Id = EmbbedElecDevChange->GetElecDevId();
			}

			if (Id > 0 && !StandaloneAccIds.Contains(Id))
			{
				TSharedPtr<FStandaloneCabAccInfo> *StandaloneAccInfo = FWHCModeGlobalData::StandaloneAccessoryMap.Find(Id);
				if (StandaloneAccInfo != nullptr)
				{
					JsonWriter->WriteObjectStart();
					(*StandaloneAccInfo)->AccShape->SaveToJson(JsonWriter);
					JsonWriter->WriteObjectEnd();

					StandaloneAccIds.Emplace(Id);
				}
			}
		}
	}
	JsonWriter->WriteArrayEnd();

	// 保存每个柜子实例
	TSet<int32> SingleCabPlatformIds;
	JsonWriter->WriteArrayStart("cabinets");
	for (auto &PlacedShape : PlacedShapes)
	{
		JsonWriter->WriteObjectStart();

		// 如果显示柜子，标记为0，为了兼容旧方案
		JsonWriter->WriteValue(TEXT("visibility"), PlacedShape->HasValidShape() ? 0 : 1);
		JsonWriter->WriteValue(TEXT("id"), (int32)PlacedShape->Shape->GetShapeId());
		JsonWriter->WriteValue(TEXT("room_guid"), PlacedShape->RoomAttachedIndex.ToString());
		JsonWriter->WriteValue(TEXT("wall_index"), PlacedShape->WallAttachedIndex);

		JsonWriter->WriteArrayStart(TEXT("location"));
		FVector Location = PlacedShape->ShapeFrame->GetActorLocation();
		JsonWriter->WriteValue(Location.X);
		JsonWriter->WriteValue(Location.Y);
		JsonWriter->WriteValue(Location.Z);
		JsonWriter->WriteArrayEnd();

		JsonWriter->WriteArrayStart(TEXT("rotation"));
		FRotator Rotation = PlacedShape->ShapeFrame->GetActorRotation();
		JsonWriter->WriteValue(Rotation.Pitch);
		JsonWriter->WriteValue(Rotation.Yaw);
		JsonWriter->WriteValue(Rotation.Roll);
		JsonWriter->WriteArrayEnd();

		// 尺寸
		JsonWriter->WriteValue(TEXT("width"), PlacedShape->Shape->GetShapeWidth());
		JsonWriter->WriteValue(TEXT("depth"), PlacedShape->Shape->GetShapeDepth());
		JsonWriter->WriteValue(TEXT("height"), PlacedShape->Shape->GetShapeHeight());

		TPair<TTuple<float, float, float>, TArray<float>> WidthInfo, DepthInfo, HeightInfo;
		if (FSCTShapeUtilityTool::GetSmokeCabinetSpaceRange(PlacedShape->Shape.Get(), WidthInfo, DepthInfo, HeightInfo))
		{
			JsonWriter->WriteValue(TEXT("hollowWidth"), WidthInfo.Key.Get<2>());
			JsonWriter->WriteValue(TEXT("hollowDepth"), DepthInfo.Key.Get<2>());
			JsonWriter->WriteValue(TEXT("hollowHeight"), HeightInfo.Key.Get<2>());
		}

		// 柜子标签
		JsonWriter->WriteArrayStart(TEXT("cabinet_labels"));
		for (const auto &Label : PlacedShape->Labels)
			JsonWriter->WriteValue((int32)Label);
		JsonWriter->WriteArrayEnd();
		if (PlacedShape->CabinetActor.IsValid())
		{
			XRCabinetActorT<XRWHCabinetAccessoryComponent> AccessoryComponent = PlacedShape->CabinetActor;
			TSharedPtr<FShapeAccessory> Accessory = AccessoryComponent->Get();
			// 柜子附件
			if (Accessory.IsValid())
			{
				JsonWriter->WriteObjectStart(TEXT("cabinet_accessory"));
				JsonWriter->WriteValue(TEXT("visibility"), PlacedShape->HasValidAccessory() ? 0 : 1);
				Accessory->CabAccInfo->Serialize(JsonWriter);
				JsonWriter->WriteArrayStart(TEXT("defaultParts"));
				for (auto &SubAcc : Accessory->AttachedSubAccessories)
				{
					JsonWriter->WriteObjectStart();
					SubAcc->CabAccInfo->Serialize(JsonWriter);
					JsonWriter->WriteObjectEnd();
				}
				JsonWriter->WriteArrayEnd();
				JsonWriter->WriteObjectEnd();
			}
		}

		JsonWriter->WriteArrayStart(TEXT("outer_boards"));
		for (const auto &BoardInfo : PlacedShape->DecBoardInfos)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue(TEXT("position"), BoardInfo.Face);
			JsonWriter->WriteValue(TEXT("valid"), BoardInfo.Spawned);
			if (BoardInfo.Spawned)
			{
				JsonWriter->WriteValue(TEXT("material_id"), BoardInfo.MtlId);
				// JsonWriter->WriteObjectStart(TEXT("material"));
				// JsonWriter->WriteValue(TEXT("id"), BoardInfo.MaterialId);
				// JsonWriter->WriteValue(TEXT("name"), BoardInfo.MaterialName);
				// JsonWriter->WriteValue(TEXT("thumbnail_url"), BoardInfo.MaterialThumbnailUrl);
				// JsonWriter->WriteValue(TEXT("url"), BoardInfo.PakUrl);
				// JsonWriter->WriteValue(TEXT("md5"), BoardInfo.PakMd5);
				// JsonWriter->WriteValue(TEXT("param"), BoardInfo.MaterialOptimiztionParam);
				// JsonWriter->WriteObjectEnd();
			}
			JsonWriter->WriteArrayStart(TEXT("offsets"));
			for (int32 j = 0; j < 4; ++j)
				JsonWriter->WriteValue(0);
			JsonWriter->WriteArrayEnd();
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();

		// 调整板，占位
		JsonWriter->WriteObjectStart(TEXT("adjust_boards"));
		// 左侧
		JsonWriter->WriteObjectStart(TEXT("left_side"));
		JsonWriter->WriteValue(TEXT("front_shape"), FString());
		JsonWriter->WriteValue(TEXT("top_valid"), 0);
		JsonWriter->WriteValue(TEXT("bottom_valid"), 0);
		JsonWriter->WriteObjectEnd();
		// 右侧
		JsonWriter->WriteObjectStart(TEXT("right_side"));
		JsonWriter->WriteValue(TEXT("front_shape"), FString());
		JsonWriter->WriteValue(TEXT("top_valid"), 0);
		JsonWriter->WriteValue(TEXT("bottom_valid"), 0);
		JsonWriter->WriteObjectEnd();
		JsonWriter->WriteObjectEnd();

		// 柜子中被替换过的项目，目前占位
		JsonWriter->WriteArrayStart(TEXT("change_list"));
		for (auto & Change : PlacedShape->ChangeList)
			Change->Serialize(JsonWriter);
		JsonWriter->WriteArrayEnd();
		if (PlacedShape->CabinetActor.IsValid())
		{
			XRCabinetActorT<XRWHCabinetPlatformInfoComponent> PlatformInfoComponent = PlacedShape->CabinetActor;
			TSharedPtr<FPlatformInfo> PlatformInfo = PlatformInfoComponent->Get();

			// 单独台面
			if (PlatformInfo.IsValid())
			{
				SingleCabPlatformIds.Emplace(PlatformInfo->Id);
				JsonWriter->WriteValue(TEXT("platform_id"), PlatformInfo->Id);
			}
		}

		// 覆盖组和对齐组
		if (PlacedShape->Labels.Contains(ECabinetLabelType::ELabelType_Ventilator))
		{
			if (FShapeInRoom *PassiveOverlapped = CabinetOperation->GetOverlappedPassive(PlacedShape.Get()))
			{
				int32 Index = PlacedShapes.IndexOfByPredicate([PassiveOverlapped](const TSharedPtr<FShapeInRoom> &InData) -> bool {
					return InData.Get() == PassiveOverlapped;
				});
				JsonWriter->WriteValue(TEXT("overlapped_passive"), Index);
			}
			else if (FShapeInRoom *PassiveAligned = CabinetOperation->GetAlignedPassive(PlacedShape.Get()))
			{
				int32 Index = PlacedShapes.IndexOfByPredicate([PassiveAligned](const TSharedPtr<FShapeInRoom> &InData) -> bool {
					return InData.Get() == PassiveAligned;
				});
				JsonWriter->WriteValue(TEXT("aligned_passive"), Index);
			}
		}

		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	// 橱柜台面
	JsonWriter->WriteObjectStart(TEXT("platform"));
	JsonWriter->WriteValue(TEXT("valid"), CabMgr->IsGenTable() ? 1 : 0);
	FPlatformInfo * PlatformInfo = CabMgr->GetPlatformInfo();
	int32 GlobalPlatformId = 0;
	if (PlatformInfo != nullptr)
	{
		PlatformInfo->Serialize(JsonWriter);

		FTableInfo * TableInfo = CabMgr->GetFrontInfo();
		JsonWriter->WriteValue(TEXT("front_shape_id"), TableInfo ? TableInfo->Id : 0);

		GlobalPlatformId = PlatformInfo->Id;
	}
	else
	{
		JsonWriter->WriteValue(TEXT("id"), GlobalPlatformId);
	}
	JsonWriter->WriteObjectEnd();

	// 某些柜子单独生成的台面
	JsonWriter->WriteArrayStart(TEXT("cab_platforms"));
	TSet<int32>::TIterator Iter(SingleCabPlatformIds);
	for (; Iter; ++Iter)
	{
		int32 Id = *Iter;
		if (Id != GlobalPlatformId)
		{
			const TSharedPtr<FPlatformInfo> *PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(Id);
			check(PlatformInfoPtr != nullptr);
			JsonWriter->WriteObjectStart();
			(*PlatformInfoPtr)->Serialize(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteArrayEnd();

	// 踢脚线
	JsonWriter->WriteObjectStart(TEXT("toe"));
	JsonWriter->WriteValue(TEXT("valid"), CabMgr->IsGenToe() ? 1 : 0);
	FTableInfo * ToeInfo = CabMgr->GetToeInfo();
	if (ToeInfo != nullptr)
	{
		JsonWriter->WriteObjectStart(TEXT("shape"));
		ToeInfo->Serialize(JsonWriter);
		JsonWriter->WriteObjectEnd();

		const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::ToeMtlMap.Find(ToeInfo->mtlId);
		if (MtlInfoPtr != nullptr)
		{
			const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
			JsonWriter->WriteObjectStart(TEXT("material"));
			MtlInfo->Serialize(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteObjectEnd();

	// 上线条
	JsonWriter->WriteObjectStart(TEXT("upper_line"));
	JsonWriter->WriteValue(TEXT("valid"), CabMgr->IsGenTop() ? 1 : 0);
	FTableInfo * TopInfo = CabMgr->GetTopInfo();
	if (TopInfo != nullptr)
	{
		JsonWriter->WriteObjectStart(TEXT("shape"));
		TopInfo->Serialize(JsonWriter);
		JsonWriter->WriteObjectEnd();

		const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::TopMtlMap.Find(TopInfo->mtlId);
		if (MtlInfoPtr != nullptr)
		{
			const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
			JsonWriter->WriteObjectStart(TEXT("material"));
			MtlInfo->Serialize(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
	}
	JsonWriter->WriteObjectEnd();

	// 下线条
	JsonWriter->WriteObjectStart(TEXT("lower_line"));
	JsonWriter->WriteValue(TEXT("valid"), CabMgr->IsGenWaist() ? 1 : 0);
	JsonWriter->WriteValue(TEXT("shape"), FString());
	JsonWriter->WriteValue(TEXT("material"), FString());
	JsonWriter->WriteObjectEnd();

	JsonWriter->WriteObjectEnd();

	ShapeDetailInfo->CloseReplacingPanel();

	return true;
}

void FArmyWHCModeController::Clear()
{
	ContentBrowser->ClearSelection();
	ShapeDetailInfo->Callback_WHCItemDeleted();
	CabinetOperation->ClearScene();

	TArray<FObjectWeakPtr> Objects;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, Objects);
	if (Objects.Num() == 0)
	{
		FArmyWHCabinetMgr::Get()->ResetToeTrim();
	}
}

void FArmyWHCModeController::Delete()
{
	CabinetOperation->Callback_DeleteSelected();
	ShapeDetailInfo->Callback_WHCItemDeleted();
}

TSharedPtr<SWidget> FArmyWHCModeController::MakeLeftPanelWidget() 
{
    SAssignNew(ContentBrowser, SArmyWHCModeContentBrowser)
	.OnClickedTopCategory_Raw(this, &FArmyWHCModeController::Callback_ClickedTopCategory)
	.OnClickedCategory_Raw(this, &FArmyWHCModeController::Callback_ClickedCategory)
	.OnLoadMore_Raw(this, &FArmyWHCModeController::Callback_ItemListLoadMore)
	.OnSearch_Raw(this, &FArmyWHCModeController::Callback_Search)

	.StartAutoDesign_Raw(this, &FArmyWHCModeController::Callback_StartAutoDesign)
	.CloseCabinetTypeChoicePage_Raw(this, &FArmyWHCModeController::Callback_CloseCabinetTypeChoicePage)
	.WhenSelectCabinetTypeDone_Raw(this, &FArmyWHCModeController::Callback_WhenSelectCabinetTypeDone)
	//.SelectedCabinetType_Raw(this, &FArmyWHCModeController::Callback_SelectedCabinetType)
	//.SelectedCabinetItemType_Raw(this, &FArmyWHCModeController::Callback_SelectedCabinetItemType)
	//.OnHoverInformationCabinetType_Raw(this, &FArmyWHCModeController::Callback_OnHoverInformationCabinetType)
	.OnSelectedLType_Raw(this, &FArmyWHCModeController::Callback_OnSelectedLType)
	.OnSelectedHorizontalLineType_Raw(this, &FArmyWHCModeController::Callback_OnSelectedHorizontalLineType)
	.OnSelectedUType_Raw(this, &FArmyWHCModeController::Callback_OnSelectedUType)
	.OnSelectedDoubleHorizontalLineType_Raw(this, &FArmyWHCModeController::Callback_OnSelectedDoubleHorizontalLineType)
	//.SelectedKitchenType_Raw(this, &FArmyWHCModeController::Callback_SelectedKitchenType)
	.ReturnToCabinetTypeChoicePage_Raw(this, &FArmyWHCModeController::Callback_ReturnToCabinetTypeChoicePage)
	.WhenSelectKitchenTypeDone_Raw(this, &FArmyWHCModeController::Callback_WhenSelectKitchenTypeDone)
	.CabinetBeSelectedClicked_Raw(this, &FArmyWHCModeController::Callback_CabinetBeSelectedClicked)
	//.ApplaySolution_Raw(this, &FArmyWHCModeController::Callback_ApplaySolution)
	.ReturnToKitchenTypeChoicePage_Raw(this, &FArmyWHCModeController::Callback_ReturnToKitchenTypeChoicePage)
	.CloseAutoDesignMainPage_Raw(this, &FArmyWHCModeController::Callback_CloseAutoDesignMainPage)
	.CloseCabinetItemChoicePage_Raw(this, &FArmyWHCModeController::Callback_CloseCabinetItemChoicePage)
	//.SelectedCabinetItem_Raw(this, &FArmyWHCModeController::Callback_SelectedCabinetItem)
	//.OnButtonKitchenItemClicked_Raw(this, &FArmyWHCModeController::Callback_OnButtonKitchenItemClicked)
	.OnKitchenChoiceDone_Raw(this, &FArmyWHCModeController::Callback_OnKitchenChoiceDone)
	.OnKitchenChoiceCancel_Raw(this, &FArmyWHCModeController::Callback_OnKitchenChoiceCancel);
    return ContentBrowser;
}

TSharedPtr<SWidget> FArmyWHCModeController::MakeRightPanelWidget()
{
    SAssignNew(ShapeDetailInfo, SArmyWHCModeAttrPanelContainer).CabinetOperation(CabinetOperation.Get());
	ShapeDetailInfo->Initialize();
	CabinetOperation->GetWHCItemSelected().BindSP(ShapeDetailInfo.ToSharedRef(), &SArmyWHCModeAttrPanelContainer::Callback_WHCItemSelected);

    return ShapeDetailInfo;
}

TSharedPtr<SWidget> FArmyWHCModeController::MakeToolBarWidget()
{
	return MakeToolBarWidgetForCupboardCabs();
}

TSharedPtr<SWidget> FArmyWHCModeController::MakeSettingBarWidget()
{
    return SNullWidget::NullWidget;
}

void FArmyWHCModeController::Draw(const FSceneView *SceneView, FPrimitiveDrawInterface* PDI)
{
	CabinetOperation->Draw(SceneView, PDI);
}

bool FArmyWHCModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	FArmyDesignModeController::InputKey(Viewport, Key, Event);

	FString KeyStr = Key.ToString();
	if (KeyStr == "LeftMouseButton")
	{
		if (Event == IE_Pressed)
			CabinetOperation->Callback_LMousePressed(FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY()));
		else
			CabinetOperation->Callback_LMouseReleased(FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY()));
	}
	else if (KeyStr == "RightMouseButton")
	{
		if (Event == IE_Pressed)
			CabinetOperation->Callback_RMousePressed(FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY()));
		else
			CabinetOperation->Callback_RMouseReleased(FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY()));
	}
	else if (KeyStr == "Delete")
	{
		if (Event == IE_Released)
			Delete();
	}
	else
	{
		if (Event == IE_Pressed)
			CabinetOperation->Callback_KeyPressed(KeyStr);
		else
			CabinetOperation->Callback_KeyReleased(KeyStr);
	}
	return false;
}

void FArmyWHCModeController::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{

}

void FArmyWHCModeController::Tick(float DeltaSeconds)
{
	FArmyDesignModeController::Tick(DeltaSeconds);

	CabinetOperation->UpdateShapeRulerPosition();
	CabinetOperation->UpdateShapeDistanceRulerPosition();

	const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);
	const TArray<int32> &CabIds = FWHCModeGlobalData::CabinetListInfo.CabinetIds;
	for (int32 i = 0; i < ContentBrowser->GetItems().Num(); ++i)
	{
		SContentItem *ContentItem = ContentBrowser->GetItems()[i].Get();
		if (ContentItem->GetItemState() == EDownloading)
		{
			int32 Id = CabIds[i];
			const FCabinetInfo &CabinetInfo = *CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));
			TSharedPtr<FDownloadFileSet> TheTask = FArmyDownloadModule::Get().GetDownloadManager()->GetTask(CabinetInfo.DownloadTaskFlag);
            if (TheTask.IsValid())
            {
                // 设置下载进度
                float Progress = TheTask->GetProgress();
                ContentItem->SetDownloadText(FText::FromString(FString::Printf(TEXT("%.2f%%"), 20.0f + Progress * 0.8f)));
                ContentItem->SetPercent(Progress);

				// GEngine->AddOnScreenDebugMessage(i, 3000.0f, FColor::Red, FString::Printf(TEXT("Rc downloading %d id %d progress %.2f"), i + 1, Id, Progress), false, FVector2D::UnitVector * 2.0f);
            }
		}
	}
}

void FArmyWHCModeController::MouseMove(FViewport *Viewport, int32 X, int32 Y)
{
	FArmyDesignModeController::MouseMove(Viewport, X, Y);
	CabinetOperation->Callback_MouseMove(FVector2D(X, Y));
}

bool FArmyWHCModeController::MouseDrag(FViewport* Viewport, FKey Key)
{
	FArmyDesignModeController::MouseDrag(Viewport, Key);
	return CabinetOperation->Callback_MouseCapturedMove(FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY()));
}

void FArmyWHCModeController::Callback_SaveCommand()
{
	FArmyFrameCommands::OnMenuSave();
}

void FArmyWHCModeController::Callback_DeleteCommand()
{
	Delete();
}

TSharedRef<SWidget> FArmyWHCModeController::Callback_HideCommandForCupboard()
{
	//EndOperation();

	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	ActionList->MapAction(FArmyWHCModeCommands::Get().HideCabinetDoorCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideCabinetDoorCommand), 
		DefaultCanExec, 
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_DoorGroup) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().HideOnGroundCabCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideOnGroundCabCommand),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_OnGroundCab) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().HideHangingCabCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideHangCabCommand),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_HangCab) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().HideFurnitureCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideFurniture),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Furniture) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().HideHydropowerCommand,
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideHydropower),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Hydropower) != 0; }));

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideCabinetDoorCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideOnGroundCabCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideHangingCabCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideFurnitureCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideHydropowerCommand);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "WHCMenu");

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FArmyWHCModeController::Callback_HideCommandForWardrobe()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	ActionList->MapAction(FArmyWHCModeCommands::Get().HideFurnitureCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideFurniture),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Furniture) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().HideHydropowerCommand,
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideHydropower),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Hydropower) != 0; }));

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideFurnitureCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideHydropowerCommand);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "WHCMenu");

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FArmyWHCModeController::Callback_HideCommandForOther()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	ActionList->MapAction(FArmyWHCModeCommands::Get().HideFurnitureCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideFurniture),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Furniture) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().HideHydropowerCommand,
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideHydropower),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Hydropower) != 0; }));

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideFurnitureCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().HideHydropowerCommand);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "WHCMenu");

	return MenuBuilder.MakeWidget();
}

void FArmyWHCModeController::Callback_HideCabinetDoorCommand()
{
	CabinetOperation->ShowCabinetDoors((CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_DoorGroup) != 0);
}

void FArmyWHCModeController::Callback_HideOnGroundCabCommand()
{
	CabinetOperation->ShowOnGroundCabinets((CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_OnGroundCab) != 0);
}

void FArmyWHCModeController::Callback_HideHangCabCommand()
{
	CabinetOperation->ShowHangCabinets((CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_HangCab) != 0);
}

void FArmyWHCModeController::Callback_HideFurniture()
{
	CabinetOperation->ShowFurniture((CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Furniture) != 0);
}

void FArmyWHCModeController::Callback_HideHydropower()
{
	CabinetOperation->ShowHydropower((CabinetOperation->GetHideCommandFlag() & FArmyWHCModeCabinetOperation::EHideCF_Hydropower) != 0);
}

void FArmyWHCModeController::Callback_StyleCommand()
{

}

TSharedRef<SWidget> FArmyWHCModeController::Callback_RulerCommand()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	ActionList->MapAction(FArmyWHCModeCommands::Get().CabinetRulerCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideCabinetRulerCommand), 
		DefaultCanExec, 
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetRulerVisibilityFlag() & FArmyWHCModeCabinetOperation::ERulerVF_Self) != 0; }));
	ActionList->MapAction(FArmyWHCModeCommands::Get().EnvRulerCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_HideEnvRulerCommand),
		DefaultCanExec,
		FIsActionChecked::CreateLambda([this]()->bool { return (CabinetOperation->GetRulerVisibilityFlag() & FArmyWHCModeCabinetOperation::ERulerVF_Env) != 0; }));

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().CabinetRulerCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().EnvRulerCommand);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "WHCMenu");

	return MenuBuilder.MakeWidget();
}

void FArmyWHCModeController::Callback_HideCabinetRulerCommand()
{
	int32 RulerCommandFlag = CabinetOperation->GetRulerVisibilityFlag();
	if ((RulerCommandFlag & FArmyWHCModeCabinetOperation::ERulerVF_Self) != 0)
	{
		CabinetOperation->ShowShapeWidthRuler(false);
	}
	else
	{
		CabinetOperation->ShowShapeWidthRuler(true);
	}
}

void FArmyWHCModeController::Callback_HideEnvRulerCommand()
{
	int32 RulerCommandFlag = CabinetOperation->GetRulerVisibilityFlag();
	if ((RulerCommandFlag & FArmyWHCModeCabinetOperation::ERulerVF_Env) != 0)
	{
		CabinetOperation->ShowShapeDistanceRuler(false);
	}
	else
	{
		CabinetOperation->ShowShapeDistanceRuler(true);
	}
}

TSharedRef<SWidget> FArmyWHCModeController::Callback_GeneratingCommand()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	ActionList->MapAction(FArmyWHCModeCommands::Get().PlatformGeneratingCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_GeneratingPlatformCommand));
	ActionList->MapAction(FArmyWHCModeCommands::Get().ToeGeneratingCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_GeneratingToeCommand));
	ActionList->MapAction(FArmyWHCModeCommands::Get().TopBlockerGeneratingCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_GeneratingTopBlockerCommand));
	ActionList->MapAction(FArmyWHCModeCommands::Get().BottomBlockerGeneratingCommand,
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_GeneratingBottomBlockerCommand),
		FCanExecuteAction::CreateLambda([](){ return false; }));

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().PlatformGeneratingCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().ToeGeneratingCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().TopBlockerGeneratingCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().BottomBlockerGeneratingCommand);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	return MenuBuilder.MakeWidget();
}

void FArmyWHCModeController::Callback_GeneratingPlatformCommand()
{
	FArmyWHCabinetMgr::Get()->GenTable(true);
}

void FArmyWHCModeController::Callback_GeneratingToeCommand()
{
	FArmyWHCabinetMgr::Get()->GenFloorTrim(true);
}

void FArmyWHCModeController::Callback_GeneratingTopBlockerCommand()
{
	FArmyWHCabinetMgr::Get()->GenHangTrim(true);
}

void FArmyWHCModeController::Callback_GeneratingBottomBlockerCommand()
{

}

TSharedRef<SWidget> FArmyWHCModeController::Callback_ClearSceneCommand()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	ActionList->MapAction(FArmyWHCModeCommands::Get().ClearCabinetCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_ClearCabinetCommand));
	ActionList->MapAction(FArmyWHCModeCommands::Get().ClearWardrobeCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_ClearWardrobeCommand));
	ActionList->MapAction(FArmyWHCModeCommands::Get().ClearOtherCommand, 
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_ClearOtherCommand));
	ActionList->MapAction(FArmyWHCModeCommands::Get().ClearAllCommand,
		FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_ClearAllCommand));

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().ClearCabinetCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().ClearWardrobeCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().ClearOtherCommand);
	MenuBuilder.AddMenuEntry(FArmyWHCModeCommands::Get().ClearAllCommand);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	return MenuBuilder.MakeWidget();
}

void FArmyWHCModeController::Callback_ClearCabinetCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空橱柜？"), FSimpleDelegate::CreateRaw(this, &FArmyWHCModeController::ClearCabinet));
}

void FArmyWHCModeController::Callback_ClearWardrobeCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空衣柜？"), FSimpleDelegate::CreateRaw(this, &FArmyWHCModeController::ClearWardrobe));
}

void FArmyWHCModeController::Callback_ClearOtherCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空其它柜子？"), FSimpleDelegate::CreateRaw(this, &FArmyWHCModeController::ClearOther));
}

void FArmyWHCModeController::Callback_ClearAllCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空所有？"), FSimpleDelegate::CreateRaw(this, &FArmyWHCModeController::Clear));	
}

void FArmyWHCModeController::Callback_TopdownViewCommand()
{

}

void FArmyWHCModeController::Callback_RegularViewCommand()
{

}

void FArmyWHCModeController::Callback_ClickedTopCategory(int32 Key, int32 Value)
{
	ContentBrowser->HideCategoryList();
	ContentBrowser->ClearItems();
	FWHCModeGlobalData::CabinetListInfo.Reset();
	FWHCModeGlobalData::CurrentWHCClassify = Value;
	
	QueryCabinetList(Value, 0);
}

void FArmyWHCModeController::Callback_ClickedCategory(int32 Key, int32 Value)
{
	int32 Classify = GetTopCategory(Value);
	if (Classify != -1)
	{
		ContentBrowser->HideCategoryList();
		ContentBrowser->ClearItems();
		FWHCModeGlobalData::CabinetListInfo.Reset();
		FWHCModeGlobalData::CurrentWHCClassify = Classify;
		
		QueryCabinetList(Classify, Value);
	}
}

void FArmyWHCModeController::Callback_ItemListLoadMore()
{
	QueryCabinetList(FWHCModeGlobalData::CurrentWHCClassify, CurrentDisplayingCategoryIndex, CurrentSearchKeyword);
}

void FArmyWHCModeController::Callback_ItemClicked(int32 InItemCode)
{
	const TArray<int32> &CabIds = FWHCModeGlobalData::CabinetListInfo.CabinetIds;
	check(InItemCode >= 0 && InItemCode < CabIds.Num());

	FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);
	FCabinetInfo *CabinetInfoPtr = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(CabIds[InItemCode]));
	check(CabinetInfoPtr != nullptr);
	FCabinetInfo &CabinetInfo = *CabinetInfoPtr;

	TSharedPtr<FCabinetWholeComponent> * Result = FWHCModeGlobalData::CabinetComponentMap.Find(CabIds[InItemCode]);
	check(Result != nullptr);
	TSharedPtr<FCabinetWholeComponent> & WholeComponent = *Result;

	if (CabinetInfo.IsCabinetHasValidCache())
	{
		SContentItem *ContentItem = ContentBrowser->GetItem(InItemCode);
		if (ContentItem->GetItemState() != EReady)
			ContentItem->SetItemState(EReady);
			
		// 选择了某个柜子进行摆放
		CabinetOperation->BeginOperation(&CabinetInfo, WholeComponent.Get());
	}
	else if (CabinetInfo.CabRcState == ECabinetResourceState::CabRc_None) // 下载柜子Json数据
	{
		FString JsonFileCachePath = CabinetInfo.GetJsonFileCachePath();

		// 设置标记，用来在Tick函数中查找DownloadTask
		CabinetInfo.DownloadTaskFlag = JsonFileCachePath;
		// 更新状态为获取Json
		CabinetInfo.CabRcState = ECabinetResourceState::CabRc_FetchingJson;

		SContentItem *ContentItem = GetListItem(CabinetInfo.Id);
		if (ContentItem != nullptr)
			ContentItem->SetItemState(EPending);

		/////////////////////////////////////////////////////////////////////////////////
		// 最后添加Json和构件下载任务，因为Http在另外一个线程处理，所以通过执行顺序严格控制状态同步

		// 添加柜子Json下载任务
		TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
		WHC_ADD_DOWNLOAD_TASK(FileInfos, CabinetInfo.Id, JsonFileCachePath, CabinetInfo.JsonUrl, CabinetInfo.JsonMd5);
		// {
		// 	FDownloadSpace::FDownloadFileInfo(
		// 		CabinetInfo.Id, 
		// 		JsonFileCachePath,
		// 		CabinetInfo.JsonUrl,
		// 		FString(),
		// 		true
		// 	)
		// };	

		// 添加柜子关联台面前沿后挡轮廓下载任务
		if (CabinetInfo.PlatformInfo.IsValid())
		{
			// 前沿轮廓
			if (CabinetInfo.PlatformInfo->FrontSections.Num() > 0)
			{
				const TSharedPtr<FTableInfo> &FrontSection = CabinetInfo.PlatformInfo->FrontSections[0];			
				WHC_ADD_DOWNLOAD_TASK(FileInfos, 
					FrontSection->Id, 
					FrontSection->GetCachedPath(),
					FrontSection->cadUrl, 
					FString());
			}
			// 后挡轮廓
			if (CabinetInfo.PlatformInfo->BackSections.Num() > 0)
			{
				const TSharedPtr<FTableInfo> &BackSection = CabinetInfo.PlatformInfo->BackSections[0];			
				WHC_ADD_DOWNLOAD_TASK(FileInfos, 
					BackSection->Id, 
					BackSection->GetCachedPath(),
					BackSection->cadUrl, 
					FString());
			}
		}

		// 添加柜子所需构件下载任务
		if (WholeComponent->Cabinet.IsValid())
		{
			if (WholeComponent->Cabinet->TopResourceUrl.Len() > 0)
			{
				// FileInfos.Emplace(WholeComponent->Cabinet->Id,
				// 	WholeComponent->Cabinet->GetCachePath(1),
				// 	WholeComponent->Cabinet->TopResourceUrl,
				// 	WholeComponent->Cabinet->TopResourceMd5,
				// 	false);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, WholeComponent->Cabinet->Id, WholeComponent->Cabinet->GetCachePath(1),
					WholeComponent->Cabinet->TopResourceUrl, WholeComponent->Cabinet->TopResourceMd5);
			}
			if (WholeComponent->Cabinet->FrontResourceUrl.Len() > 0)
			{
				// FileInfos.Emplace(WholeComponent->Cabinet->Id,
				// 	WholeComponent->Cabinet->GetCachePath(2),
				// 	WholeComponent->Cabinet->FrontResourceUrl,
				// 	WholeComponent->Cabinet->FrontResourceMd5,
				// 	false);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, WholeComponent->Cabinet->Id, WholeComponent->Cabinet->GetCachePath(2),
					WholeComponent->Cabinet->FrontResourceUrl, WholeComponent->Cabinet->FrontResourceMd5);
			}
			if (WholeComponent->Cabinet->SideResourceUrl.Len() > 0)
			{
				// FileInfos.Emplace(WholeComponent->Cabinet->Id,
				// 	WholeComponent->Cabinet->GetCachePath(3),
				// 	WholeComponent->Cabinet->SideResourceUrl,
				// 	WholeComponent->Cabinet->SideResourceMd5,
				// 	false);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, WholeComponent->Cabinet->Id, WholeComponent->Cabinet->GetCachePath(3),
					WholeComponent->Cabinet->SideResourceUrl, WholeComponent->Cabinet->SideResourceMd5);
			}
		}
		// 添加柜子附属品构件下载任务
		if (WholeComponent->Accessorys.Num() > 0)
		{
			for (const auto & SubAcc : WholeComponent->Accessorys)
			{
				// if (Accessory->TopResourceUrl.Len() > 0)
				// {
				// 	FileInfos.Emplace(Accessory->Id,
				// 		Accessory->GetCachePath(1),
				// 		Accessory->TopResourceUrl,
				// 		Accessory->TopResourceMd5,
				// 		false);
				// }
				// if (Accessory->FrontResourceUrl.Len() > 0)
				// {
				// 	FileInfos.Emplace(Accessory->Id,
				// 		Accessory->GetCachePath(2),
				// 		Accessory->FrontResourceUrl,
				// 		Accessory->FrontResourceMd5,
				// 		false);
				// }
				// if (Accessory->SideResourceUrl.Len() > 0)
				// {
				// 	FileInfos.Emplace(Accessory->Id,
				// 		Accessory->GetCachePath(3),
				// 		Accessory->SideResourceUrl,
				// 		Accessory->SideResourceMd5,
				// 		false);
				// }
				WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAcc->Id, SubAcc->GetCachePath(1), SubAcc->TopResourceUrl, SubAcc->TopResourceMd5);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAcc->Id, SubAcc->GetCachePath(2), SubAcc->FrontResourceUrl, SubAcc->FrontResourceMd5);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAcc->Id, SubAcc->GetCachePath(3), SubAcc->SideResourceUrl, SubAcc->SideResourceMd5);
			}
		}
		// 添加柜子门板构件下载任务
		if (WholeComponent->DoorSheets.Num() > 0)
		{
			for (const auto & DoorSheet : WholeComponent->DoorSheets)
			{
				// if (DoorSheet->TopResourceUrl.Len() > 0)
				// {
				// 	FileInfos.Emplace(DoorSheet->Id,
				// 		DoorSheet->GetCachePath(1),
				// 		DoorSheet->TopResourceUrl,
				// 		DoorSheet->TopResourceMd5,
				// 		false);
				// }
				// if (DoorSheet->FrontResourceUrl.Len() > 0)
				// {
				// 	FileInfos.Emplace(DoorSheet->Id,
				// 		DoorSheet->GetCachePath(2),
				// 		DoorSheet->FrontResourceUrl,
				// 		DoorSheet->FrontResourceMd5,
				// 		false);
				// }
				// if (DoorSheet->SideResourceUrl.Len() > 0)
				// {
				// 	FileInfos.Emplace(DoorSheet->Id,
				// 		DoorSheet->GetCachePath(3),
				// 		DoorSheet->SideResourceUrl,
				// 		DoorSheet->SideResourceMd5,
				// 		false);
				// }
				WHC_ADD_DOWNLOAD_TASK(FileInfos, DoorSheet->Id, DoorSheet->GetCachePath(1), DoorSheet->TopResourceUrl, DoorSheet->TopResourceMd5);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, DoorSheet->Id, DoorSheet->GetCachePath(2), DoorSheet->FrontResourceUrl, DoorSheet->FrontResourceMd5);
				WHC_ADD_DOWNLOAD_TASK(FileInfos, DoorSheet->Id, DoorSheet->GetCachePath(3), DoorSheet->SideResourceUrl, DoorSheet->SideResourceMd5);
			}
		}
		/////////////////////////////////////////////////////////////////////////////////

		TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(FileInfos);
        NewTask->OnDownloadFinished.BindRaw(this, 
			&FArmyWHCModeController::Callback_CabJsonDownloadFinished, 
			JsonFileCachePath, 
			CabinetInfo.Id);
	}
}

void FArmyWHCModeController::Callback_ItemDetailInfoHovered(int32 InItemCode)
{
	check(InItemCode < ContentBrowser->GetItems().Num());

	const TSharedPtr<SContentItem> &ContentItem = ContentBrowser->GetItems()[InItemCode];

	// 这段代码来自SArmyModelContentBrowser的OnContentItemInfoHovered
	FVector2D ItemAbsolutePosition = ContentItem->GetUIGeo().GetAbsolutePosition();
	if (ItemAbsolutePosition.Y < 1.0f)
		return;
	FVector2D ContentBrowserAbsolutePosition = ContentBrowser->GetCachedGeometry().GetAbsolutePosition();
	float HeightAboveItem = ItemAbsolutePosition.Y - ContentBrowserAbsolutePosition.Y + 7.0f;
	FVector2D GVCAbsolutionPosition = GVC->ViewportOverlayWidget->GetCachedGeometry().GetAbsolutePosition();

	FVector2D PixPos(4,0);
	if (HeightAboveItem + 444.0f >= GVC->ViewportOverlayWidget->GetCachedGeometry().GetLocalSize().Y)
		PixPos.Y = GVC->ViewportOverlayWidget->GetCachedGeometry().GetLocalSize().Y - 448.0f;
	else
		PixPos.Y = HeightAboveItem;

	if (ContentItemDetailInfoWidget.IsValid())
	{
		GVC->ViewportOverlayWidget->RemoveSlot(ContentItemDetailInfoWidget.ToSharedRef());
		ContentItemDetailInfoWidget.Reset();
	}

	const TArray<int32> &CabIds = FWHCModeGlobalData::CabinetListInfo.CabinetIds;
	const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);
	const FCabinetInfo &CabinetInfo = *CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(CabIds[InItemCode]));
	
	ContentItemDetailInfoWidget = 
		SNew(SArmyDetailInfoWidget)
		.Size(300.0f)
		.ItemHeight(418.0f)
		.AlignCenter(true)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
		.TextColor(FLinearColor::White)
		.ThumbnailURL(CabinetInfo.ThumbnailUrl)
		.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))
		.DisplayText(FText::FromString(CabinetInfo.Name));

	FString CabTypeText(TEXT("未知"));
	ECabinetType CabType = (ECabinetType)CabinetInfo.Type;
	switch (CabType)
	{
		case ECabinetType::EType_HangCab:
			CabTypeText = TEXT("吊柜");
			break;
		case ECabinetType::EType_OnCabCab:
			CabTypeText = TEXT("台上柜");
			break;
		case ECabinetType::EType_OnGroundCab:
			CabTypeText = TEXT("地柜");
			break;
		case ECabinetType::EType_TallCab:
			CabTypeText = TEXT("高柜");
			break;
	}
	ContentItemDetailInfoWidget->AddItem(
		SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("类型: %s"), *CabTypeText)))   
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
	);
	ContentItemDetailInfoWidget->AddItem(
		SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("尺寸: 宽%d*深%d*高%d"), FMath::RoundToInt(CabinetInfo.WidthInfo.Current), FMath::RoundToInt(CabinetInfo.DepthInfo.Current), FMath::RoundToInt(CabinetInfo.HeightInfo.Current))))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
	);
	ContentItemDetailInfoWidget->AddItem(
		SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("宽度: %d ~ %dmm"), FMath::RoundToInt(CabinetInfo.WidthInfo.Min), FMath::RoundToInt(CabinetInfo.WidthInfo.Max))))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
	);
	ContentItemDetailInfoWidget->AddItem(
		SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("深度: %d ~ %dmm"), FMath::RoundToInt(CabinetInfo.DepthInfo.Min), FMath::RoundToInt(CabinetInfo.DepthInfo.Max))))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
	);
	ContentItemDetailInfoWidget->AddItem(
		SNew(STextBlock)
		.Text(FText::FromString(FString::Printf(TEXT("高度: %d ~ %dmm"), FMath::RoundToInt(CabinetInfo.HeightInfo.Min), FMath::RoundToInt(CabinetInfo.HeightInfo.Max))))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FF969799"))
	);

	ContentItemDetailInfoWidget->SetRenderTransform(FSlateRenderTransform(PixPos));
	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(0, 0, 0, 0)
		[
			ContentItemDetailInfoWidget.ToSharedRef()
		];
}

void FArmyWHCModeController::Callback_ItemDetailInfoUnhovered(int32 InItemCode)
{
	check(InItemCode < ContentBrowser->GetItems().Num());
	if (ContentItemDetailInfoWidget.IsValid())
		GVC->ViewportOverlayWidget->RemoveSlot(ContentItemDetailInfoWidget.ToSharedRef());
}

void FArmyWHCModeController::Callback_Search(const FString &InSearchText)
{
	FTCHARToUTF8 Converter(*InSearchText);

	TArray<uint8> Data;
	Data.SetNum(Converter.Length());
	FMemory::Memcpy(Data.GetData(), Converter.Get(), Converter.Length());
	CurrentSearchKeyword.Empty();
	for (const auto &OneData : Data)
	{
		CurrentSearchKeyword += FString::Printf(TEXT("%%%X"), OneData);
	}

	ContentBrowser->ClearItems();
	FWHCModeGlobalData::CabinetListInfo.Reset();
	QueryCabinetList(FWHCModeGlobalData::CurrentWHCClassify, CurrentDisplayingCategoryIndex, CurrentSearchKeyword);
}

void FArmyWHCModeController::Callback_CabJsonDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 Id)
{
	// 检查已完成下载了Json数据的柜子是否在界面列表中
	SContentItem *ContentItemPtr = GetListItem(Id);

	FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);
	FCabinetInfo &CabinetInfo = *CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));

	if (bSucceeded)
	{	
		FString FileContent;
		if (FFileHelper::LoadFileToString(FileContent, *InJsonFileCachePath))
		{
			// 解析型录数据
			TSharedPtr<FJsonObject> ShapeJsonData;
            if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(FileContent), ShapeJsonData))
            {
				FCabinetShape *Shape = new FCabinetShape;
                FSCTShapeManager::Get()->ParseChildrenShapes(ShapeJsonData);
				Shape->ParseFromJson(ShapeJsonData);
				FSCTShapeManager::Get()->ClearAllChildrenShapes();

				// 获取需要下载的资源列表
				TArray<FString> CurResourceUrls;
				TArray<FString> CurFileCachePaths;
				Shape->GetResourceUrls(CurResourceUrls);
				Shape->GetFileCachePaths(CurFileCachePaths);
				Shape->CheckResourceUrlsAndCachePaths(CurResourceUrls, CurFileCachePaths);

				// 切换状态为获取柜子所需的模型材质资源
				CabinetInfo.CabRcState = ECabinetResourceState::CabRc_FetchingRc;

				Shape->SetShapeId(CabinetInfo.Id);
				Shape->SetShapeName(CabinetInfo.Name);
				
				Shape->SetJsonFileUrl(CabinetInfo.JsonUrl);
				FWHCModeGlobalData::CabinetTemplateMap.Emplace(CabinetInfo.Id, MakeShareable(Shape));				
				

				// 最后添加下载任务，因为Http在另外一个线程处理，所以通过执行顺序严格控制状态同步
				TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
				for (int32 i = 0; i < CurResourceUrls.Num(); ++i)
				{
					FileInfos.Emplace(
						i, 
						CurFileCachePaths[i],
						CurResourceUrls[i],
						FString(),
						true                 
					);
				}

				// 柜子关联台面的材质下载
				if (CabinetInfo.PlatformInfo.IsValid())
				{
					if (CabinetInfo.PlatformInfo->PlatformMtl.IsValid())
					{
						WHC_ADD_DOWNLOAD_TASK(FileInfos, 
							CabinetInfo.PlatformInfo->Id, 
							CabinetInfo.PlatformInfo->PlatformMtl->GetCachedPath(),
							CabinetInfo.PlatformInfo->PlatformMtl->MtlUrl, 
							CabinetInfo.PlatformInfo->PlatformMtl->MtlMd5);
					}
				}

				// 添加柜子附属件下载任务
				if (CabinetInfo.AccInfo.IsValid())
				{
					FileInfos.Emplace(
						CabinetInfo.AccInfo->Self->Id,
						CabinetInfo.AccInfo->Self->GetCachePath(),
						CabinetInfo.AccInfo->Self->PakUrl,
						CabinetInfo.AccInfo->Self->PakMd5,
						false
					);
					for (const auto & SubAcc : CabinetInfo.AccInfo->SubAccInfos)
					{
						FileInfos.Emplace(
							SubAcc->Id,
							SubAcc->GetCachePath(),
							SubAcc->PakUrl,
							SubAcc->PakMd5,
							false
						);
					}
				}

				// 设置标识，用来在Tick函数中查找DownloadTask
				CabinetInfo.DownloadTaskFlag = FString::Printf(TEXT("%d"), CabinetInfo.Id);

				TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(CabinetInfo.DownloadTaskFlag, FileInfos);
				NewTask->OnDownloadFinished.BindRaw(this, 
					&FArmyWHCModeController::Callback_CabResourcesDownloadFinished, 
					InJsonFileCachePath,
					CabinetInfo.Id);

				// Json数据加载到了内存，并解析完成，可以再更新一次进度
				if (ContentItemPtr != nullptr)
				{
					ContentItemPtr->SetDownloadText(FText::FromString(FString::Printf(TEXT("%.2f%%"), 20.0f)));
					ContentItemPtr->SetPercent(20.0f);
					ContentItemPtr->SetItemState(EDownloading);
				}

				return;
			}
		}
	}

	CabinetInfo.CabRcState = ECabinetResourceState::CabRc_None;

	if (ContentItemPtr != nullptr)
		ContentItemPtr->SetItemState(ENotReady);
}

void FArmyWHCModeController::Callback_CabResourcesDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 InShapeId)
{
	// 根据任务成功与否决定柜子最终状态
	const TArray<int32> &CabIds = FWHCModeGlobalData::CabinetListInfo.CabinetIds;
	FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);
	FCabinetInfo * CabinetInfoPtr = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(InShapeId));
	check(CabinetInfoPtr != nullptr);
	FCabinetInfo & CabinetInfo = *CabinetInfoPtr;
	CabinetInfo.CabRcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;

	// 检查已完成下载了资源数据的柜子是否在界面列表中
	SContentItem *ContentItemPtr = GetListItem(InShapeId);
	if (ContentItemPtr != nullptr)
		ContentItemPtr->SetItemState(bSucceeded ? EReady : ENotReady);

	if (!bSucceeded)
		FWHCModeGlobalData::CabinetTemplateMap.Remove(InShapeId);
}

void FArmyWHCModeController::QueryCabinetList(int32 InClassify, int32 InType, const FString &InSearchText)
{
	// 获取当前要请求的页数
	int32 UrlCurrent = FWHCModeGlobalData::CabinetListInfo.CabinetIds.Num() / 20 + 1;
	// 如果超过了总页数，则不再请求
	if (FWHCModeGlobalData::CabinetListInfo.TotalDataPage != 0 && (FWHCModeGlobalData::CabinetListInfo.CabinetIds.Num() % 20 != 0))
		return;

	CurrentDisplayingCategoryIndex = InType;

	int32 UrlType = InType;
	int32 UrlCategoryId = 0;
	if ((InType & 0xFFC00000) != 0) // Has subtype
	{
		UrlCategoryId = UrlType & 0x003FFFFF;
		UrlType = UrlType >> 22;
	}
	FString Url;
	if (InSearchText.IsEmpty())
		Url = FString::Printf(TEXT("design/cabinets/%d/pageList?type=%d&categoryId=%d&current=%d&size=20"), InClassify, UrlType, UrlCategoryId, UrlCurrent);
	else
		Url = FString::Printf(TEXT("design/cabinets/%d/pageList?type=%d&keywords=%s&categoryId=%d&current=%d&size=20"), InClassify, UrlType, *InSearchText, UrlCategoryId, UrlCurrent);
	
	TSharedRef<IHttpRequest> HttpRequest = FHttpMgr::Get()->CreateHttpRequest(
		Url, 
		TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", "application/json; charset=utf-8");
	HttpRequest->SetHeader("xloginid", FArmyHttpModule::Get().GetLoginId());
    HttpRequest->SetHeader("xtoken", FArmyHttpModule::Get().GetToken());
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, InType](FHttpRequestPtr RequestPtr, FHttpResponsePtr ResponsePtr, bool bSucceeded)
		{
			if (!FHttpMgr::Get()->CheckHttpResponseValid(ResponsePtr, bSucceeded))
			{
				ContentBrowser->ShowNoItem();
				return;
			}
			FString ErrorMsg;
			TSharedPtr<FJsonObject> ResponseData = FHttpMgr::Get()->GetContentAsJsonObject(ResponsePtr, &ErrorMsg);
			if (!ResponseData.IsValid())
			{
				ContentBrowser->ShowNoItem();
				return;
			}

			FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);

			const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
			FWHCModeGlobalData::CabinetListInfo.TotalDataPage = Data->GetNumberField(TEXT("totalPage"));

			const TArray<TSharedPtr<FJsonValue>>* ShapeArray = nullptr;
			if (Data->TryGetArrayField(TEXT("list"), ShapeArray))
			{	
				int32 nCount = (*ShapeArray).Num();
				int32 nIndexStart = FWHCModeGlobalData::CabinetListInfo.CabinetIds.Num();
				for (int32 i = 0; i < nCount; ++i)
				{
					const TSharedPtr<FJsonValue> &Value = ShapeArray->operator[](i);
					const TSharedPtr<FJsonObject> &ShapeJsonObject = Value->AsObject();

					int32 Id = ShapeJsonObject->GetNumberField(TEXT("id"));
					FWHCModeGlobalData::CabinetListInfo.CabinetIds.Emplace(Id);

					FCabinetInfo *CabinetInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));
					if (CabinetInfo == nullptr)
					{
						int32 CabType = ShapeJsonObject->GetNumberField(TEXT("category"));
						CabinetInfoArr.Emplace(
							Id,
							CabType,
							ShapeJsonObject->GetStringField(TEXT("name")),
							ShapeJsonObject->GetStringField(TEXT("thumbnailUrl")),
							ShapeJsonObject->GetStringField(TEXT("jsonUrl")),
							ShapeJsonObject->GetStringField(TEXT("cabinetMd5"))
						);
						FCabinetInfo &NewCabInfo = CabinetInfoArr[CabinetInfoArr.Num() - 1];
						FWHCGlobalParams::FWHCCabGlobalParams * CabGlobalParams = FWHCModeGlobalData::GlobalParams.CabGlobalParamsMap.Find((ECabinetType)CabType);
						if (CabGlobalParams != nullptr)
							NewCabInfo.AboveGround = CabGlobalParams->AboveGround;
						else
							NewCabInfo.AboveGround = ShapeJsonObject->GetNumberField(TEXT("heightToGround"));
						NewCabInfo.WidthInfo.Current = ShapeJsonObject->GetNumberField(TEXT("currentWidth"));
						NewCabInfo.WidthInfo.Min = ShapeJsonObject->GetNumberField(TEXT("minWidth"));
						NewCabInfo.WidthInfo.Max = ShapeJsonObject->GetNumberField(TEXT("maxWidth"));
						const TArray<TSharedPtr<FJsonValue>> &WidthIntervalsJsonArr = ShapeJsonObject->GetArrayField(TEXT("standardWidths"));
						for (const auto &IntervalJson : WidthIntervalsJsonArr)
							NewCabInfo.WidthInfo.Intervals.Emplace(IntervalJson->AsNumber());
						NewCabInfo.DepthInfo.Current = ShapeJsonObject->GetNumberField(TEXT("currentDepth"));
						NewCabInfo.DepthInfo.Min = ShapeJsonObject->GetNumberField(TEXT("minDepth"));
						NewCabInfo.DepthInfo.Max = ShapeJsonObject->GetNumberField(TEXT("maxDepth"));
						const TArray<TSharedPtr<FJsonValue>> &DepthIntervalsJsonArr = ShapeJsonObject->GetArrayField(TEXT("standardDepths"));
						for (const auto &IntervalJson : DepthIntervalsJsonArr)
							NewCabInfo.DepthInfo.Intervals.Emplace(IntervalJson->AsNumber());
						NewCabInfo.HeightInfo.Current = ShapeJsonObject->GetNumberField(TEXT("currentHeight"));
						NewCabInfo.HeightInfo.Min = ShapeJsonObject->GetNumberField(TEXT("minHeight"));
						NewCabInfo.HeightInfo.Max = ShapeJsonObject->GetNumberField(TEXT("maxHeight"));
						const TArray<TSharedPtr<FJsonValue>> &HeightIntervalsJsonArr = ShapeJsonObject->GetArrayField(TEXT("standardHeights"));
						for (const auto &IntervalJson : HeightIntervalsJsonArr)
							NewCabInfo.HeightInfo.Intervals.Emplace(IntervalJson->AsNumber());
						const TSharedPtr<FJsonObject> *ParamJsonObjPtr = nullptr;
						if (ShapeJsonObject->TryGetObjectField(TEXT("params"), ParamJsonObjPtr))
						{
							const TSharedPtr<FJsonObject> *RangeHoodObjPtr = nullptr;
							if ((*ParamJsonObjPtr)->TryGetObjectField(TEXT("rangeHood"), RangeHoodObjPtr))
							{
								const TSharedPtr<FJsonObject> &RangeHoodObj = *RangeHoodObjPtr;

								FDimInfo RangeHoodWidthInfo;
								RangeHoodWidthInfo.Current = RangeHoodObj->GetNumberField(TEXT("hollowWidth"));
								RangeHoodWidthInfo.Min = RangeHoodObj->GetNumberField(TEXT("minHollowWidth"));
								RangeHoodWidthInfo.Max = RangeHoodObj->GetNumberField(TEXT("maxHollowWidth"));
								const TArray<TSharedPtr<FJsonValue>> *StandardHollowWidthArr = nullptr;
								if (RangeHoodObj->TryGetArrayField(TEXT("hollowStandardWidth"), StandardHollowWidthArr))
								{
									for (const auto &IntervalJson : (*StandardHollowWidthArr))
										RangeHoodWidthInfo.Intervals.Emplace(IntervalJson->AsNumber());
								}
								NewCabInfo.OtherDimInfo.Emplace("VentilatorWidth", MoveTemp(RangeHoodWidthInfo));

								FDimInfo RangeHoodDepthInfo;
								RangeHoodDepthInfo.Current = RangeHoodObj->GetNumberField(TEXT("hollowDepth"));
								RangeHoodDepthInfo.Min = RangeHoodObj->GetNumberField(TEXT("minHollowDepth"));
								RangeHoodDepthInfo.Max = RangeHoodObj->GetNumberField(TEXT("maxHollowDepth"));
								const TArray<TSharedPtr<FJsonValue>> *StandardHollowDepthArr = nullptr;
								if (RangeHoodObj->TryGetArrayField(TEXT("hollowStandardDepth"), StandardHollowDepthArr))
								{
									for (const auto &IntervalJson : (*StandardHollowDepthArr))
										RangeHoodDepthInfo.Intervals.Emplace(IntervalJson->AsNumber());
								}
								NewCabInfo.OtherDimInfo.Emplace("VentilatorDepth", MoveTemp(RangeHoodDepthInfo));
							}
						}
						const TArray<TSharedPtr<FJsonValue>> &LabelsJsonArr = ShapeJsonObject->GetArrayField(TEXT("labels"));
						for (const auto &LabelJson : LabelsJsonArr)
						{
							const TSharedPtr<FJsonObject> &LabelJsonObj = LabelJson->AsObject();
							NewCabInfo.Labels.Emplace((ECabinetLabelType)LabelJsonObj->GetIntegerField(TEXT("id")));
						}

						// 获取柜子绑定的默认台面
						const TSharedPtr<FJsonObject> * DefaultWorkTop = nullptr;
						if (ShapeJsonObject->TryGetObjectField(TEXT("worktop"), DefaultWorkTop))
						{
							int32 PlatformId = (*DefaultWorkTop)->GetIntegerField(TEXT("id"));
							const TSharedPtr<FPlatformInfo> *PlatformInfoPtr = FWHCModeGlobalData::PlatformMap.Find(PlatformId);
							if (PlatformInfoPtr == nullptr)
							{
								NewCabInfo.PlatformInfo = MakeShareable(new FPlatformInfo);
								NewCabInfo.PlatformInfo->Deserialize(*DefaultWorkTop);
								FWHCModeGlobalData::PlatformMap.Emplace(PlatformId, NewCabInfo.PlatformInfo);
							}
							else
								NewCabInfo.PlatformInfo = *PlatformInfoPtr;
						}

						const TSharedPtr<FJsonObject> * DefaultAcc = nullptr;
						// ShapeJsonObject->GetObjectField(TEXT("defaultMetals"));
						
						if (ShapeJsonObject->TryGetObjectField(TEXT("defaultMetals"), DefaultAcc))
						{
							NewCabInfo.AccInfo = MakeShareable(new FCabinetWholeAccInfo);
							NewCabInfo.AccInfo->Deserialize(*DefaultAcc);
						}

						//添加了其他五金的构件图例  
						FCabinetWholeComponent * WholeComponent = new FCabinetWholeComponent;

						const TArray<TSharedPtr<FJsonValue>>* ComponentJsonArr = nullptr;
						if (ShapeJsonObject->TryGetArrayField(TEXT("viewLegends"), ComponentJsonArr))
						{
							for (auto JsonValue : *ComponentJsonArr)
							{
								const TSharedPtr<FJsonObject> &ComponentJsonObj = JsonValue->AsObject();
								if (ComponentJsonObj.IsValid())
								{
									int32 ComponentId = ComponentJsonObj->GetIntegerField(TEXT("id"));
									int32 ComponentType = ComponentJsonObj->GetIntegerField(TEXT("type"));

									switch (ComponentType)
									{
									case 104://柜类
									{
										WholeComponent->Cabinet = MakeShareable(new FCabinetComponent);
										WholeComponent->Cabinet->Id = ComponentId;
										WholeComponent->Cabinet->Type = ComponentType;
										WholeComponent->Cabinet->TopResourceUrl = ComponentJsonObj->GetStringField(TEXT("topViewLegend"));
										WholeComponent->Cabinet->TopResourceMd5 = ComponentJsonObj->GetStringField(TEXT("topViewLegendMd5"));
										WholeComponent->Cabinet->FrontResourceUrl = ComponentJsonObj->GetStringField(TEXT("frontViewLegend"));
										WholeComponent->Cabinet->FrontResourceMd5 = ComponentJsonObj->GetStringField(TEXT("frontViewLegendMd5"));
										WholeComponent->Cabinet->SideResourceUrl = ComponentJsonObj->GetStringField(TEXT("sideViewLegend"));
										WholeComponent->Cabinet->SideResourceMd5 = ComponentJsonObj->GetStringField(TEXT("sideViewLegendMd5"));
									}
										break;
									case 105://五金/电器类
									{
										TSharedPtr<FCabinetComponent> AccessoryComponent = MakeShareable(new FCabinetComponent);
										AccessoryComponent->Id = ComponentJsonObj->GetIntegerField(TEXT("id"));
										AccessoryComponent->Type = ComponentJsonObj->GetIntegerField(TEXT("type"));
										AccessoryComponent->TopResourceUrl = ComponentJsonObj->GetStringField(TEXT("topViewLegend"));
										AccessoryComponent->TopResourceMd5 = ComponentJsonObj->GetStringField(TEXT("topViewLegendMd5"));
										AccessoryComponent->FrontResourceUrl = ComponentJsonObj->GetStringField(TEXT("frontViewLegend"));
										AccessoryComponent->FrontResourceMd5 = ComponentJsonObj->GetStringField(TEXT("frontViewLegendMd5"));
										AccessoryComponent->SideResourceUrl = ComponentJsonObj->GetStringField(TEXT("sideViewLegend"));
										AccessoryComponent->SideResourceMd5 = ComponentJsonObj->GetStringField(TEXT("sideViewLegendMd5"));
										WholeComponent->Accessorys.Add(AccessoryComponent);
									}
										break;
									case 106://门板
									{
										TSharedPtr<FCabinetComponent> DoorSheetComponent = MakeShareable(new FCabinetComponent);
										DoorSheetComponent->Id = ComponentJsonObj->GetIntegerField(TEXT("id"));
										DoorSheetComponent->Type = ComponentJsonObj->GetIntegerField(TEXT("type"));
										DoorSheetComponent->TopResourceUrl = ComponentJsonObj->GetStringField(TEXT("topViewLegend"));
										DoorSheetComponent->TopResourceMd5 = ComponentJsonObj->GetStringField(TEXT("topViewLegendMd5"));
										DoorSheetComponent->FrontResourceUrl = ComponentJsonObj->GetStringField(TEXT("frontViewLegend"));
										DoorSheetComponent->FrontResourceMd5 = ComponentJsonObj->GetStringField(TEXT("frontViewLegendMd5"));
										DoorSheetComponent->SideResourceUrl = ComponentJsonObj->GetStringField(TEXT("sideViewLegend"));
										DoorSheetComponent->SideResourceMd5 = ComponentJsonObj->GetStringField(TEXT("sideViewLegendMd5"));
										WholeComponent->DoorSheets.Add(DoorSheetComponent);
									}
									break;
									default:
										break;
									}
								}
							}
						}

						//old core
						//FCabinetWholeComponent * WholeComponent = new FCabinetWholeComponent;
						
						//if (ComponentJson.IsValid())
						//{
						//	WholeComponent->Cabinet = MakeShareable(new FCabinetComponent);
						//	WholeComponent->Cabinet->Id = ComponentJson->GetIntegerField(TEXT("id"));
						//	WholeComponent->Cabinet->Type = ComponentJson->GetIntegerField(TEXT("type"));
						//	WholeComponent->Cabinet->ResourceUrl = ComponentJson->GetStringField(TEXT("topViewLegend"));
						//	WholeComponent->Cabinet->ResourceMd5 = ComponentJson->GetStringField(TEXT("topViewLegendMd5"));
						//}

						FWHCModeGlobalData::CabinetComponentMap.Emplace(Id, MakeShareable(WholeComponent));
					}
				}

				if (nCount == 0 && nIndexStart == 0)
					ContentBrowser->ShowNoItem();
				else
					FillCabinetList(InType, nIndexStart, nCount);

				

				return;
			}

			// CurrentDisplayingCategoryIndex = -1;
			ContentBrowser->ShowNoItem();
		}
	);
	HttpRequest->ProcessRequest();
}

void FArmyWHCModeController::FillCabinetList(int32 InCategory, int32 StartIndex, int32 InCount)
{
	ContentBrowser->ShowItemList();

	const TArray<int32> &CabIds = FWHCModeGlobalData::CabinetListInfo.CabinetIds;
	const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(FWHCModeGlobalData::CurrentWHCClassify);

	for (int32 i = 0; i < InCount; ++i)
	{
		const FCabinetInfo *CabinetInfoPtr = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(CabIds[StartIndex + i]));
		check(CabinetInfoPtr != nullptr);
		const FCabinetInfo &CabinetInfo = *CabinetInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(StartIndex + i)
            .ItemWidth(106)
            .ItemHeight(130)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
			.ThumbnailURL(CabinetInfo.ThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(!CabinetInfo.IsCabinetHasValidCache())
			.OnClicked_Raw(this, &FArmyWHCModeController::Callback_ItemClicked)
			.DisplayText(FText::FromString(CabinetInfo.Name))
			.DetailInfo(EVisibility::Visible)
			.OnInfoHovered_Raw(this, &FArmyWHCModeController::Callback_ItemDetailInfoHovered)
			.OnInfoUnhovered_Raw(this, &FArmyWHCModeController::Callback_ItemDetailInfoUnhovered);
		Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_Cab, CabinetInfo.Id));

		switch (CabinetInfo.CabRcState)
		{
			case ECabinetResourceState::CabRc_None:
				Item->SetItemState(ENotReady);
				break;
			case ECabinetResourceState::CabRc_FetchingJson:
				Item->SetItemState(EPending);
				break;
			case ECabinetResourceState::CabRc_FetchingRc:
				Item->SetItemState(EDownloading);
				break;
			case ECabinetResourceState::CabRc_Complete:
				Item->SetItemState(EReady);
				break;
		}
		
		ContentBrowser->AddItem(Item); 
	}

	ContentBrowser->ResetScroll();
}

// int32 FArmyWHCModeController::GetItemIndexFromJsonFile(const FString &InJsonFileCachePath) const
// {
// 	const TArray<int32> &CabIds = FWHCModeGlobalData::CabinetListInfo.CabinetIds;
// 	const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find((int32)ECabinetType::EType_Cab);

// 	for (int32 i = 0; i < CabIds.Num(); ++i)
// 	{
// 		const FCabinetInfo *CabInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(CabIds[i]));
// 		if (InJsonFileCachePath == CabInfo->GetJsonFileCachePath())
// 			return i;
// 	}
// 	return -1;
// }

FReply FArmyWHCModeController::Callback_StartAutoDesign()
{
	//检测柜体库中是否有橱柜

	//检测是否有厨房
	bool bHasValidKitchens = false;

	if (bHasValidKitchens)
	{
		//没有厨房，提示命名新的厨房
	}
	else
	{
		//存在一个有效的厨房

		//存在多个厨房,提示选择一个厨房

		//测试代码
		ContentBrowser->KitchenListWidget->ClearChildren();	
		int32 KitchenNum = 2; 
		for (int i = 0; i < KitchenNum; ++i)
		{
			ContentBrowser->KitchenListWidget->AddSlot().Padding(FMargin(0.f,4.f))
			[SNew(SArmyKitchenItem)];
		}		
		GGI->Window->PresentModalDialog(TEXT("提醒"), ContentBrowser->MultiKitchenChoicePageBorder.ToSharedRef());
	}

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_CloseCabinetTypeChoicePage(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_WhenSelectCabinetTypeDone()
{
	//测试，填充空白控件------------
	ContentBrowser->KitchenTypeListWidget->ClearChildren();
	int32 SolutionNum = 20;
	TArray<TSharedPtr<SArmyKitchenTypeItem>> TemList; TemList.SetNum(SolutionNum);

	for (int i = 0; i < SolutionNum; ++i)
	{
		SAssignNew(TemList[i], SArmyKitchenTypeItem);
		ContentBrowser->KitchenTypeListWidget->AddItem(TemList[i]);
	}
	ContentBrowser->AutoDesignBreadcrumTrail->PushCrumb(FText::FromString(TEXT("智能设计")), FString("AutoDesign"));
	ContentBrowser->AutoDesignBreadcrumTrail->PushCrumb(FText::FromString(TEXT("选型")), FString("ChoiceType"));

	//-----------------------------

	/*切换页面*/
	ContentBrowser->ItemListContainerSlot->DetachWidget();

	ContentBrowser->ItemListContainerSlot->operator[](ContentBrowser->AutoDesignMainPageBorder.ToSharedRef());

	ContentBrowser->AutoDesignMainSlot->DetachWidget();

	ContentBrowser->AutoDesignMainSlot->operator[](ContentBrowser->KitchenTypeChoicePageBox.ToSharedRef());

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_SelectedCabinetType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_SelectedCabinetItemType()
{
	//添加空白控件用于测试

	ContentBrowser->CabineItemListWidget->ClearChildren();

	int32 WidgetNum = 20;
	TArray<TSharedPtr<SArmyCabinetItem>> CabinetItems; CabinetItems.SetNum(WidgetNum);

	for (int32 Index = 0; Index < WidgetNum; ++Index)
	{
		SAssignNew(CabinetItems[Index], SArmyCabinetItem).bCanSelectType(false);

		ContentBrowser->CabineItemListWidget->AddItem(CabinetItems[Index]);
	}

	ContentBrowser->CabinetItemPageTitle->SetText(FText::FromString(TEXT("电器列表")));

	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(0, 0, 0, 0)
		[
			ContentBrowser->SelectCabinetItemPageBorder.ToSharedRef()
		];

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_OnHoverInformationCabinetType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_OnSelectedLType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_OnSelectedHorizontalLineType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_OnSelectedUType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_OnSelectedDoubleHorizontalLineType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_SelectedKitchenType(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_ReturnToCabinetTypeChoicePage()
{
	//切换橱柜选型页面

	//测试代码，填充空白控件------------
	ContentBrowser->CabinetTypeItems.Empty();
	ContentBrowser->CabineTypetListWidget->ClearChildren();
	int32 CabinetTypeNumber = 4;
	int32 CabineNumber = 6;

	ContentBrowser->CabinetTypeItems.SetNum(CabinetTypeNumber);
	for (int i = 0; i < CabinetTypeNumber; ++i)
	{
		SAssignNew(ContentBrowser->CabinetTypeItems[i], SArmyCabinetTypeItem);

		ContentBrowser->CabinetTypeItems[i]->CabinetItems.AddZeroed(CabineNumber);
		for (int j = 0; j < CabineNumber; ++j)
		{
			SAssignNew(ContentBrowser->CabinetTypeItems[i]->CabinetItems[j], SArmyCabinetItem);
			ContentBrowser->CabinetTypeItems[i]->AddItem(ContentBrowser->CabinetTypeItems[i]->CabinetItems[j]);
		}

		ContentBrowser->CabineTypetListWidget->AddSlot()
			[
				ContentBrowser->CabinetTypeItems[i].ToSharedRef()
			];
	}
	//-----------------------------

	/*切换页面*/
	ContentBrowser->ItemListContainerSlot->DetachWidget();

	ContentBrowser->ItemListContainerSlot->operator[](ContentBrowser->SelectCabinetPageBorder.ToSharedRef());

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_WhenSelectKitchenTypeDone()
{
	//测试，填充空白控件------------
	ContentBrowser->SolutionListWidget->ClearChildren();
	int32 SolutionNum = 20;
	TArray<TSharedPtr<SArmyAutoDesignSolution>> TemList; TemList.SetNum(SolutionNum);

	for (int i = 0; i < SolutionNum; ++i)
	{
		SAssignNew(TemList[i], SArmyAutoDesignSolution);
		ContentBrowser->SolutionListWidget->AddItem(TemList[i]);
	}

	ContentBrowser->AutoDesignBreadcrumTrail->PushCrumb(FText::FromString(TEXT("L字型")), FString("TypeL"));

	//-----------------------------

	/*切换页面*/
	ContentBrowser->ItemListContainerSlot->DetachWidget();

	ContentBrowser->ItemListContainerSlot->operator[](ContentBrowser->AutoDesignMainPageBorder.ToSharedRef());

	ContentBrowser->AutoDesignMainSlot->DetachWidget();

	ContentBrowser->AutoDesignMainSlot->operator[](ContentBrowser->SolutionShowPageBox.ToSharedRef());

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_CabinetBeSelectedClicked()
{
	ContentBrowser->CabineItemListWidget->ClearChildren();

	int32 WidgetNum = 20;
	TArray<TSharedPtr<SArmyCabinetItem>> CabinetItems; CabinetItems.SetNum(WidgetNum);

	for (int32 Index = 0; Index < WidgetNum; ++Index)
	{
		SAssignNew(CabinetItems[Index], SArmyCabinetItem).bCanSelectType(false);

		ContentBrowser->CabineItemListWidget->AddItem(CabinetItems[Index]);
	}

	ContentBrowser->CabinetItemPageTitle->SetText(FText::FromString(TEXT("已选商品")));

	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(0, 0, 0, 0)
		[
			ContentBrowser->SelectCabinetItemPageBorder.ToSharedRef()
		];
	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_ApplaySolution(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_ReturnToKitchenTypeChoicePage()
{
	//测试，填充空白控件------------
	ContentBrowser->KitchenTypeListWidget->ClearChildren();
	int32 SolutionNum = 20;
	TArray<TSharedPtr<SArmyKitchenTypeItem>> TemList; TemList.SetNum(SolutionNum);

	for (int i = 0; i < SolutionNum; ++i)
	{
		SAssignNew(TemList[i], SArmyKitchenTypeItem);
		ContentBrowser->KitchenTypeListWidget->AddItem(TemList[i]);
	}
	ContentBrowser->AutoDesignBreadcrumTrail->PushCrumb(FText::FromString(TEXT("智能设计")), FString("AutoDesign"));
	ContentBrowser->AutoDesignBreadcrumTrail->PushCrumb(FText::FromString(TEXT("选型")), FString("ChoiceType"));

	//-----------------------------

	/*切换页面*/
	ContentBrowser->AutoDesignMainSlot->DetachWidget();

	ContentBrowser->AutoDesignMainSlot->operator[](ContentBrowser->KitchenTypeChoicePageBox.ToSharedRef());

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_CloseAutoDesignMainPage(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_CloseCabinetItemChoicePage()
{
	if (ContentBrowser->SelectCabinetItemPageBorder.IsValid())
		GVC->ViewportOverlayWidget->RemoveSlot(ContentBrowser->SelectCabinetItemPageBorder.ToSharedRef());

	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_SelectedCabinetItem()
{
	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_OnButtonKitchenItemClicked(){return FReply::Handled();}

FReply FArmyWHCModeController::Callback_OnKitchenChoiceDone()
{
	//切换橱柜选型页面

	//测试代码，填充空白控件------------
	ContentBrowser->CabinetTypeItems.Empty();
	ContentBrowser->CabineTypetListWidget->ClearChildren();
	int32 CabinetTypeNumber = 4;
	int32 CabineNumber = 6;

	ContentBrowser->CabinetTypeItems.SetNum(CabinetTypeNumber);
	for (int i = 0; i < CabinetTypeNumber; ++i)
	{
		SAssignNew(ContentBrowser->CabinetTypeItems[i], SArmyCabinetTypeItem);

		ContentBrowser->CabinetTypeItems[i]->CabinetItems.AddZeroed(CabineNumber);
		for (int j = 0; j < CabineNumber; ++j)
		{
			SAssignNew(ContentBrowser->CabinetTypeItems[i]->CabinetItems[j], SArmyCabinetItem);
			ContentBrowser->CabinetTypeItems[i]->AddItem(ContentBrowser->CabinetTypeItems[i]->CabinetItems[j]);
		}

		ContentBrowser->CabineTypetListWidget->AddSlot()
			[
				ContentBrowser->CabinetTypeItems[i].ToSharedRef()
			];
	}
	//-----------------------------

	/*切换页面*/
	ContentBrowser->ItemListContainerSlot->DetachWidget();

	ContentBrowser->ItemListContainerSlot->operator[](ContentBrowser->SelectCabinetPageBorder.ToSharedRef());


	GGI->Window->DismissModalDialog();
	return FReply::Handled();
}

FReply FArmyWHCModeController::Callback_OnKitchenChoiceCancel()
{
	GGI->Window->DismissModalDialog();
	return FReply::Handled();
}

int32 FArmyWHCModeController::GetTopCategory(int32 InType) const
{
	if ((InType & 0xFFC00000) != 0) // Has subtype
		InType = InType >> 22;
	switch ((ECabinetType)InType)
	{
		case ECabinetType::EType_Other: // 其它类型
			return 0;
		case ECabinetType::EType_CoverDoorWardrobe:
		case ECabinetType::EType_SlidingDoorWardrobe:
		case ECabinetType::EType_TopCab:
			return 1;
		case ECabinetType::EType_HangCab:
		case ECabinetType::EType_OnGroundCab:
		case ECabinetType::EType_TallCab:
		case ECabinetType::EType_OnCabCab:
			return 2;
		case ECabinetType::EType_BathroomOnGroundCab:
		case ECabinetType::EType_BathroomHangCab:
		case ECabinetType::EType_BathroomStorageCab:
			return 4;
		default:
			return -1;
	}
}

SContentItem* FArmyWHCModeController::GetListItem(int32 Id) const
{
	for (const auto & Item : ContentBrowser->GetItems())
	{
		TSharedPtr<FWHCModeAttrTypeInfo> AttrTypeInfo = Item->GetMetaData<FWHCModeAttrTypeInfo>();
		if (AttrTypeInfo.IsValid() && AttrTypeInfo->Id == Id)
			return Item.Get();
	}
	return nullptr;
}

TSharedPtr<SWidget> FArmyWHCModeController::MakeToolBarWidgetForCupboardCabs()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"

	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	ToolBarBuidler->AddButton(FArmyWHCModeCommands::Get().SaveCommand, FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_SaveCommand));
	ToolBarBuidler->AddButton(FArmyWHCModeCommands::Get().DeleteCommand, FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_DeleteCommand),
		FCanExecuteAction::CreateLambda(
			[]() { return FWHCModeGlobalData::SelectionState == 1; }
		)
	);
	// ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyWHCModeController::Callback_ClearSceneCommand), LOCTEXT("ClearScene", "清空"), FSlateIcon("ArmyStyle", "WHCMode.ClearSceneCommand"));
	ToolBarBuidler->AddButton(FArmyWHCModeCommands::Get().ClearSceneCommand, FExecuteAction::CreateLambda(
		[this]()
		{
			GGI->Window->PresentModalDialog(TEXT("是否清空所有？"), FSimpleDelegate::CreateRaw(this, &FArmyWHCModeController::Clear));
		}
	));
	// ToolBarBuidler->AddButton(FArmyWHCModeCommands::Get().StyleCommand, FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_StyleCommand), 
	// 	FCanExecuteAction::CreateLambda(
	// 		[]() { return false; }
	// 	)
	// );
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyWHCModeController::Callback_RulerCommand), LOCTEXT("Ruler", "标尺"), FSlateIcon("ArmyStyle", "WHCMode.RulerCommand"));
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyWHCModeController::Callback_GeneratingCommand), LOCTEXT("Generating", "生成"), FSlateIcon("ArmyStyle", "WHCMode.GeneratingCommand"));
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyWHCModeController::Callback_HideCommandForCupboard), LOCTEXT("Hide", "隐藏"), FSlateIcon("ArmyStyle", "WHCMode.HideCommand"));
	// ToolBarBuidler->AddButton(FArmyWHCModeCommands::Get().TopdownViewCommand, FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_TopdownViewCommand), 
	// 	FCanExecuteAction::CreateLambda(
	// 		[]() { return false; }
	// 	)
	// );
	// ToolBarBuidler->AddButton(FArmyWHCModeCommands::Get().RegularViewCommand, FExecuteAction::CreateRaw(this, &FArmyWHCModeController::Callback_RegularViewCommand), 
	// 	FCanExecuteAction::CreateLambda(
	// 		[]() { return false; }
	// 	)
	// );

	CommandList->Append(ToolBarBuidler->GetCommandList());

#undef LOCTEXT_NAMESPACE

 	return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

void FArmyWHCModeController::ClearCabinet()
{
	CabinetOperation->ClearCabinet();
}

void FArmyWHCModeController::ClearWardrobe()
{
	CabinetOperation->ClearWardrobe();
}

void FArmyWHCModeController::ClearOther()
{
	CabinetOperation->ClearOther();
}

void FArmyWHCModeController::Callback_HideSliderDoorCommand() {

}
void FArmyWHCModeController::Callback_HideDrawerFaceDoorCommand() {

}