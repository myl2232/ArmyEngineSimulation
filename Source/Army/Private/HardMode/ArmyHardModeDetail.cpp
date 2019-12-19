#include "ArmyHardModeDetail.h"
#include "ArmyHydropowerDetail.h"
#include "SWidgetSwitcher.h"
#include "ArmyBaseEditStyle.h"
#include "ArmyPipePointActor.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "SArmyDetailComponent.h"
#include "SColorPicker.h"
#include "STextBlockImageSpinbox.h"
#include "Components/SphereReflectionCaptureComponent.h"
#include "SArmyComboBox.h"
#include "ArmyContinueStyle.h"
#include "ArmyWhirlwindStyle.h"
#include "ArmyIntervalStyle.h"
#include "ArmySeamlessStyle.h"
#include "ArmyCrossStyle.h"
#include "ArmyWorkerStyle.h"
#include "ArmyContinueStyle.h"
#include "ArmyHerringBoneStyle.h"
#include "ArmyTrapezoidStyle.h"
#include "ArmyPointLightActor.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyBaseArea.h"
#include "ArmyGameInstance.h"
#include "ArmyEditorEngine.h"
#include "ArmyWindowActor.h"
#include "ArmyFurnitureActor.h"
#include "ArmyFurniture.h"
#include "Resmanager.h"
#include "ArmyResourceModule.h"
#include "SArmyHardModeReplaceLists.h"
#include "ArmyActorConstant.h"
#include "ArmyExtrusionActor.h"
#include "ArmyUser.h"
#include "SArmyMulitCategory.h"
#include "FArmyConstructionItemInterface.h"
#include "ArmySlateModule.h"
#include "Runtime/Online/ImageDownload/Public/WebImageCache.h"
#include "ArmyHttpModule.h"
#include "ArmyReplaceTextureOperation.h"
#include "ArmyStaticLighting/Public/StaticLightingPrivate.h"
#include "ArmyConstructionManager.h"
#include "ConstructionData/IArmyConstructionHelper.h"
FArmyHardModeDetail::FArmyHardModeDetail()
{
	Array_Paving = MakeShareable(new FArmyComboBoxArray());
	cbxWindowStoneVisibility = ECheckBoxState::Checked;
	//初始化铺法UI
	SAssignNew(PavingMethodModify, SArmyPropertyComboBox)
		.OptionsSource(Array_Paving);

	//ConstructionWallDetail = MakeShareable(new SArmyMulitCategory);
	//ConstructionRoofDetail = MakeShareable(new SArmyMulitCategory);
	//ConstructionFloorDetail = MakeShareable(new SArmyMulitCategory);
	ConstructionDetail = MakeShareable(new SArmyMulitCategory);

	ConstructionDetail->SetAutoAssignValue(false);

	LightMobilityTypeList = MakeShareable(new FArmyComboBoxArray);
	LightMobilityTypeList->Add(MakeShareable(new FArmyKeyValue(0, TEXT("静态光"))));
	LightMobilityTypeList->Add(MakeShareable(new FArmyKeyValue(1, TEXT("动态光"))));
}

FArmyHardModeDetail::~FArmyHardModeDetail()
{
}

void FArmyHardModeDetail::UpdateConstructionUI(int32 PDTType, TSharedPtr<FJsonObject> InJsonData)
{
	//测试代码输出房间名
	if (InJsonData.IsValid()) 
	{
		auto Room = SelectedRoom;
		if (Room.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("面所在房间名： %s"), *Room->GetSpaceName())
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("面所在房间名： 未找到"))
		}
	}

	CurrentJsonData = InJsonData;
	ExeBeforeClearDetail();

	int32 SpaceID = -1;
	if (SelectedRoom.IsValid())
	{
		SpaceID = SelectedRoom->GetSpaceId();
	}
	

	ConstructionDetail->Init(InJsonData, SpaceID);

	detailType = DetailType::Index_None;

	switch (PDTType)
	{
	case PDT_FloorTile:
		CreateDetailFloorTile((EWallType)selectedObject->SurportPlaceArea);
		break;
	case PDT_WaterKnife:
		CreateDetailWaterKnife();
		break;
	case PDT_WaveLine:
		CreateDetailWaveLine();
		break;
	case PDT_DXFModel:
		CreateDetailDXFModel();
		detailType = DT_ExtrusionLine;
		break;
	case PDT_WallPaint:
		CreateDetailWallPaint((EWallType)selectedObject->SurportPlaceArea);
		break;
	case PDT_WallPaper:
		CreateDetailWallPaper((EWallType)selectedObject->SurportPlaceArea);
		break;
	case PDT_AluminousGussetPlate:
		CreateDetailAluminousGussetPlate();
		break;
	case PDT_PAKModel:
		CreateDetailPAKModel();
		detailType = DetailType::DT_Actor;
		break;
	case PDT_LampSlot:
		CreateDetailLampSlot();
		detailType = DT_ExtrusionLine;
		break;
	case PDT_PointLight:
		CreateDetailPointLight();
		break;
	case PDT_SpotLight:
		CreateDetailSpotLight();
		break;
	case PDT_ReflectionSphere:
		CreateDetailReflectionSphere();
		break;
	case PDT_WindowNormal:
		CreateDetailWindowNormal();
		detailType = DetailType::DT_NormalWindowStone;
		break;
	case PDT_WindowRectBay:
		CreateDetailWindowRectBay();
		detailType = DetailType::DT_RectWindowStone;
		break;
	case PDT_HYDROPOWERMODE:
		CreateOutletDetail();
		break;
	case PDT_DXFMODELRectLAMPSLOT:
		CreateHasLightDXFDetail();
		break;
	case PDT_Door:
		CreateDoorDetail();
		detailType = DetailType::DT_Door;
		break;
	case PDT_Pass:
		CreatePassDetail();
		detailType = DT_Pass;
		break;
	case PDT_RoughWall:
		CreateRoughWallDetail();
		detailType = DetailType::DT_RoughWall;
		break;
	case PDT_RoughRoof:
		CreateRoughRoofDetail();
		detailType = DetailType::DT_RoughRoof;
		break;
	case PDT_RoughFloor:
		CreateRoughFloorDetail();
		detailType = DetailType::DT_RoughFloor;
		break;
		//绘制区域额毛坯墙
	case PDT_PlotArea_RoughWall:
		CreatePlotAreaRoughWallDetail();
		detailType = DetailType::DT_RoughWall;
		break;
		//绘制区域毛坯顶
	case PDT_PlotArea_RoughRoof:
		CreatePlotAreaRoughRoofDetail();
		detailType = DetailType::DT_RoughRoof;
		break;
		//绘制区域毛坯地
	case PDT_PlotArea_RoughFloor:
		CreatePlotAreaRoughFloorDetail();
		detailType = DetailType::DT_RoughFloor;
		break;
	default:
		break;
	}

	if (detailType == DetailType::Index_None)
	{
		if (selectedObject.IsValid())
		{
			detailType = selectedObject->GetType() == OT_BodaArea ? DT_Boda : DT_Paste;
		}
		else
		{
			detailType = PDTType == PDT_DXFModel ? DT_Extrusion : DT_Actor;
		}
	}
	ExeWhileShowDetail(detailType);
}

void FArmyHardModeDetail::ConstructionCallBack(EConstructionRequeryResult ResultInfo, TSharedPtr<FJsonObject> ConstructionData, TSharedPtr<class FArmyConstructionItemInterface> CheckedData, ObjectConstructionKey Key, EPropertyDetailType DetailType, TSharedPtr<FArmyObject> Obj)
{

	if (!selectedObject.IsValid()
		&& (!SelectedUObject || !SelectedUObject->IsValidLowLevel()))
	{
		return;
	}

	switch (ResultInfo)
	{
	case EConstructionRequeryResult::Succeed:
		UpdateConstructionUI(DetailType, ConstructionData);
		break;
	case EConstructionRequeryResult::Failed:
		UpdateConstructionUI(DetailType, nullptr);
		break;
	default:
		break;
	}


	TSharedPtr<SWidget> *TempWidget;
	TempWidget = SWidgetMap.Find(DetailType);
	if (TempWidget)
	{
		PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
	}

}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDetailWidget()
{
	SAssignNew(PropertyPanel, SWidgetSwitcher);

	//创建瓷砖、地板、墙砖、墙板属性面板 0
	CreateDetailFloorTile();

	//水刀拼花 1
	CreateDetailWaterKnife();

	//波打线 2
	CreateDetailWaveLine();


	//踢脚线、造型线、顶角线 3
	CreateDetailDXFModel();


	//墙漆 4
	CreateDetailWallPaint();


	//壁纸 、背景墙 5
	CreateDetailWallPaper();


	//铝扣板 6
	CreateDetailAluminousGussetPlate();


	//灯、浴霸、排风扇 7
	CreateDetailPAKModel();


	//灯槽 8
	CreateDetailLampSlot();


	//点光源 9
	CreateDetailPointLight();

	//射灯 10
	CreateDetailSpotLight();

	//反射球 11
	CreateDetailReflectionSphere();

	//普通窗户 12
	CreateDetailWindowNormal();

	//矩形飘窗 13
	CreateDetailWindowRectBay();

	//水电点位 14
	CreateOutletDetail();

	// 15
	CreateHasLightDXFDetail();

	//门 16
	CreateDoorDetail();

	//垭口 
	CreatePassDetail();

	//@郭子阳
	CreatePlotAreaRoughWallDetail();
	CreatePlotAreaRoughFloorDetail();
	CreatePlotAreaRoughRoofDetail();

	CreateRoughWallDetail();

	CreateRoughRoofDetail();

	CreateRoughFloorDetail();

	//@马云龙
	CreateWorldSettingsDetail();

	HardModePropertyPanel = CreateHardModePropertyPanel();
	return HardModePropertyPanel.ToSharedRef();
}

void FArmyHardModeDetail::ShowSelectedDetial(UObject* InSelectedObject)
{
	//@郭子阳 清空当前房间
	SelectedRoom = nullptr;

	if (WindowRectBayMatReplaceList.IsValid())
	{
		GGI->WindowOverlay->RemoveSlot(WindowRectBayMatReplaceList.ToSharedRef());
		WindowRectBayMatReplaceList = nullptr;
	}

	//@郭子阳
	CurrentCheckData = nullptr; //清空勾选数据

	if (InSelectedObject)
	{
		/**@欧石楠 如果上一个选中的物体是普通actor，且不是同一个actor 则将其设置为静态*/
		if (SelectedUObject != InSelectedObject)
		{

			// 当选中门后又选中别的对象，隐藏门的替换列表
			if (DoorGoodsReplaceList.IsValid())
			{
				GGI->WindowOverlay->RemoveSlot(DoorGoodsReplaceList.ToSharedRef());
				DoorGoodsReplaceList = nullptr;
			}
			if (PassGoodsReplaceList.IsValid())
			{
				GGI->WindowOverlay->RemoveSlot(PassGoodsReplaceList.ToSharedRef());
				PassGoodsReplaceList = nullptr;
			}
		}

		SelectedUObject = InSelectedObject;
		selectedObject = nullptr;//同一时刻只能存在一种选择对象

		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
		AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
		AXRReflectionCaptureActor* ReflectionCaptureActor = Cast<AXRReflectionCaptureActor>(SelectedUObject);
		AXRWindowActor* WindowActor = Cast<AXRWindowActor>(SelectedUObject);
		AXRFurnitureActor* ParentActor = Cast<AXRFurnitureActor>(SelectedActor->GetOwner());
		AArmyExtrusionActor* ExtrusionActor = Cast<AArmyExtrusionActor>(SelectedUObject);

		// 对于不能选中的物体不显示属性界面
		if (SelectedActor && SelectedActor->IsSelectable())
		{
			if (SelectedActor->Tags.Contains(TEXT("AreaHighLight")))
				return;

			// 门的属性面板需要单独显示
 			if (SelectedActor->ActorHasTag(XRActorTag::Door) ||
				SelectedActor->ActorHasTag(XRActorTag::SecurityDoor))
			{
				CachedName = SelectedActor->GetActorLabel();
				FObjectWeakPtr DoorObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
				if (DoorObj.IsValid() &&
					(DoorObj.Pin()->GetType() == EObjectType::OT_SecurityDoor ||
						DoorObj.Pin()->GetType() == EObjectType::OT_Door ||
						DoorObj.Pin()->GetType() == EObjectType::OT_SlidingDoor))
				{
					TSharedPtr<FArmyHardware> DoorHardware = StaticCastSharedPtr<FArmyHardware>(DoorObj.Pin());
					CachedScaleY = DoorHardware->GetLength() * 10;
					CachedScaleZ = DoorHardware->GetHeight() * 10;
				}
				SetPropertyPanelVisibility(EVisibility::Visible);
				int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
				CurrentDetailType = EPropertyDetailType::PDT_Door;


				//@郭子阳
				//获取施工项
				if (DoorObj.IsValid())
				{
					switch (DoorObj.Pin()->GetType())
					{
					case  EObjectType::OT_SecurityDoor:
					case  EObjectType::OT_Door:
					{
						auto Door = StaticCastSharedPtr<FArmySingleDoor>(DoorObj.Pin());

						SelectedRoom = Door->GetRoom();
						if (Door->HasGoods())
						{
							XRConstructionManager::Get()->TryToFindConstructionData(Door->GetUniqueID(), *Door->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, DoorObj.Pin()));
						}
						else
						{
							UpdateConstructionUI(EPropertyDetailType::PDT_Door, nullptr);
						}
					}
						break;
					
					case  EObjectType::OT_SlidingDoor:
					{
						auto Door = StaticCastSharedPtr<FArmySlidingDoor>(DoorObj.Pin());
						SelectedRoom = Door->GetRoom();
						if (Door->HasGoods())
						{
							XRConstructionManager::Get()->TryToFindConstructionData(Door->GetUniqueID(), *Door->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, DoorObj.Pin()));
						}
						else
						{
							UpdateConstructionUI(EPropertyDetailType::PDT_Door, nullptr);
						}
					}
						break;
					}
				}

				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_Door);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}

				// @欧石楠 显示名字的时候不需要显示NOLIST
				CachedName.RemoveFromStart(TEXT("NOLIST-"));

				return;
			}
			//垭口属性
			else if (SelectedActor->ActorHasTag(XRActorTag::Pass)  )
			{
				CachedName = SelectedActor->GetActorLabel();
				FObjectWeakPtr PassObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
				if (PassObj.IsValid() && (PassObj.Pin()->GetType() == EObjectType::OT_Pass || PassObj.Pin()->GetType() == EObjectType::OT_NewPass))
				{
					TSharedPtr<FArmyHardware> DoorHardware = StaticCastSharedPtr<FArmyHardware>(PassObj.Pin());
					CachedScaleY = DoorHardware->GetLength() * 10;
					CachedScaleZ = DoorHardware->GetHeight() * 10;
				}
				SetPropertyPanelVisibility(EVisibility::Visible);
				int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
				
				CurrentDetailType = EPropertyDetailType::PDT_Pass;
				//@郭子阳
				//获取施工项
				if (PassObj.IsValid())
				{
					switch (PassObj.Pin()->GetType())
					{
					case  EObjectType::OT_NewPass:
					{
						auto Obj = StaticCastSharedPtr<FArmyNewPass>(PassObj.Pin());
						SelectedRoom = Obj->GetRoom();
						if (Obj->HasGoods())
						{
							XRConstructionManager::Get()->TryToFindConstructionData(Obj->GetUniqueID(), *Obj->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, PassObj.Pin()));
						}
						else
						{
							UpdateConstructionUI(CurrentDetailType, nullptr);
						}
					}
					break;
					}
				}
				//UpdateConstructionUI(EPropertyDetailType::PDT_Pass, FArmySceneData::Get()->GetJsonDataByItemID(TempItemID));
				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(CurrentDetailType);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}
				return;
			}

			for (auto detailMapItem : DetailBuilderMap)
			{
				TSharedPtr<class FArmyDetailBuilder> detailItem = detailMapItem.Value;
				TMap<FName, TSharedPtr<FArmyDetailNode>> node = detailItem->GetChildrenNodes();
				TSharedPtr<FArmyDetailNode> RaiseNodeX = nullptr;
				TSharedPtr<FArmyDetailNode> RaiseNodeY = nullptr;
				TSharedPtr<FArmyDetailNode> RaiseNodeZ = nullptr;
				for (auto& It : node)
				{
					RaiseNodeX = It.Value->FindChildNode(FName(TEXT("ScaleX")));
					RaiseNodeY = It.Value->FindChildNode(FName(TEXT("ScaleY")));
					RaiseNodeZ = It.Value->FindChildNode(FName(TEXT("ScaleZ")));
					if (SelectedActor->ActorHasTag(FName("IsSale")))
					{
						SetDetailNodeProperty(RaiseNodeX, true, false);
						SetDetailNodeProperty(RaiseNodeY, true, false);
						SetDetailNodeProperty(RaiseNodeZ, true, false);
					}
					else
					{
						SetDetailNodeProperty(RaiseNodeX, false, true);
						SetDetailNodeProperty(RaiseNodeY, false, true);
						SetDetailNodeProperty(RaiseNodeZ, false, true);
					}
				}
			}
			SetPropertyPanelVisibility(EVisibility::Visible);
			if (PointLightActor)
			{
				CachedLight3DIntensity = PointLightActor->PointLightCOM->Intensity;
				CachedLight3DRadius = PointLightActor->PointLightCOM->AttenuationRadius;
				CachedLight3DSourceRadius = PointLightActor->PointLightCOM->SourceRadius;
				CachedLight3DSourceLength = PointLightActor->PointLightCOM->SourceLength;
				CachedLight3DColor = PointLightActor->PointLightCOM->LightColor;
				BorderColor = PointLightActor->PointLightCOM->LightColor;
				CachedLight3DEnable = (ECheckBoxState)PointLightActor->PointLightCOM->bVisible;
				CachedIndirectLightingIntensity = PointLightActor->PointLightCOM->IndirectLightingIntensity;
				CacheLightMobility = PointLightActor->GetRootComponent()->Mobility == EComponentMobility::Static ? TEXT("静态光") : TEXT("动态光");
				PointLightActor->PointLightCOM->CastShadows ? CachedLight3DCastShadow = ECheckBoxState::Checked : CachedLight3DCastShadow = ECheckBoxState::Unchecked;

				int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
				UpdateConstructionUI(EPropertyDetailType::PDT_PointLight, FArmySceneData::Get()->GetJsonDataByItemID(TempItemID));
				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_PointLight);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}
			}
			else if (SpotLightActor)
			{
				CachedLight3DIntensity = SpotLightActor->SpotLightCOM->Intensity;
				CachedLight3DRadius = SpotLightActor->SpotLightCOM->AttenuationRadius;
				CachedLight3DSourceRadius = SpotLightActor->SpotLightCOM->SourceRadius;
				CachedLight3DSourceLength = SpotLightActor->SpotLightCOM->SourceLength;
				CachedSpotLight3DInnerAngle = SpotLightActor->SpotLightCOM->InnerConeAngle;
				CachedSpotLight3DOuterAngle = SpotLightActor->SpotLightCOM->OuterConeAngle;
				CachedLight3DColor = SpotLightActor->SpotLightCOM->LightColor;
				BorderColor = SpotLightActor->SpotLightCOM->LightColor;
				CachedLight3DEnable = (ECheckBoxState)SpotLightActor->SpotLightCOM->bVisible;
				CachedIndirectLightingIntensity = SpotLightActor->SpotLightCOM->IndirectLightingIntensity;
				CacheLightMobility = SpotLightActor->GetRootComponent()->Mobility == EComponentMobility::Static ? TEXT("静态光") : TEXT("动态光");
				SpotLightActor->SpotLightCOM->CastShadows ? CachedLight3DCastShadow = ECheckBoxState::Checked : CachedLight3DCastShadow = ECheckBoxState::Unchecked;

				int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
				UpdateConstructionUI(EPropertyDetailType::PDT_SpotLight, FArmySceneData::Get()->GetJsonDataByItemID(TempItemID));
				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_SpotLight);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}
			}
			else if (ReflectionCaptureActor)
			{
				CachedReflectionSphereRadius = ReflectionCaptureActor->ReflectionCaptureCOM->InfluenceRadius;
				CachedReflectionSphereBrightness = ReflectionCaptureActor->ReflectionCaptureCOM->Brightness;
				int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
				UpdateConstructionUI(EPropertyDetailType::PDT_ReflectionSphere, FArmySceneData::Get()->GetJsonDataByItemID(TempItemID));
				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_ReflectionSphere);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}
			}
			else if (WindowActor)
			{
				for (auto detailMapItem : DetailBuilderMap)
				{
					TSharedPtr<class FArmyDetailBuilder> detailItem = detailMapItem.Value;
					TMap<FName, TSharedPtr<FArmyDetailNode>> node = detailItem->GetChildrenNodes();
					TSharedPtr<FArmyDetailNode> WindowLocationZ = nullptr;
					TSharedPtr<FArmyDetailNode> RaiseNodeX = nullptr;
					TSharedPtr<FArmyDetailNode> RaiseNodeY = nullptr;
					TSharedPtr<FArmyDetailNode> RaiseNodeZ = nullptr;
					for (auto& It : node)
					{
						RaiseNodeX = It.Value->FindChildNode(FName(TEXT("WindowScaleX")));
						RaiseNodeY = It.Value->FindChildNode(FName(TEXT("WindowScaleY")));
						RaiseNodeZ = It.Value->FindChildNode(FName(TEXT("WindowScaleZ")));
						WindowLocationZ = It.Value->FindChildNode(FName(TEXT("WindowLocationZ")));

						SetDetailNodeProperty(RaiseNodeX, true, false);
						SetDetailNodeProperty(RaiseNodeY, true, false);
						SetDetailNodeProperty(RaiseNodeZ, true, false);
						SetDetailNodeProperty(WindowLocationZ, true, false);

					}
				}

				if (WindowActor->CurrentWindowType == WindowActorType::RECTBAY_WINDOW
					|| WindowActor->CurrentWindowType == WindowActorType::TRAPEBAY_WINDOW)
				{
					int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
					CurrentDetailType = EPropertyDetailType::PDT_WindowRectBay;

				}
				else
				{
					int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
					CurrentDetailType = EPropertyDetailType::PDT_WindowNormal;

				}
				//是不是非飘窗
				bool IsNormalWindow = !(WindowActor->AttachSurface->GetType() == EObjectType::OT_RectBayWindow
					|| WindowActor->AttachSurface->GetType() == EObjectType::OT_TrapeBayWindow);

				if (IsNormalWindow)
				{
					auto WindowObj = StaticCastSharedPtr<FArmyWindow>(WindowActor->AttachSurface);
					SelectedRoom = WindowObj->GetRoom();
					if (WindowObj->GetContentItem().IsValid())
					{
						CachedName = WindowObj->GetContentItem()->Name;
						//请求施工项
						ConstructionPatameters Parameter;
						Parameter.SetNormalGoodsInfo(WindowObj->GetContentItem()->ID, WindowObj->GetRoomSpaceID());
						XRConstructionManager::Get()->TryToFindConstructionData(WindowObj->GetUniqueID(), Parameter, FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(WindowObj)));

					}
					else
					{
						//清空施工项UI
						UpdateConstructionUI(CurrentDetailType, nullptr);
					}
					detailType = DT_NormalWindowStone;
				}
				else
				{
					auto WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(WindowActor->AttachSurface);
					SelectedRoom = WindowObj->GetRoom();
					if (WindowObj->GetContentItem().IsValid())
					{
						CachedName = WindowObj->GetContentItem()->Name;
						//请求施工项
						ConstructionPatameters Parameter;
						Parameter.SetNormalGoodsInfo(WindowObj->GetContentItem()->ID, WindowObj->GetRoomSpaceID());
						XRConstructionManager::Get()->TryToFindConstructionData(WindowObj->GetUniqueID(), Parameter, FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(WindowObj)));
					}
					else
					{
						//清空施工项UI
						UpdateConstructionUI(CurrentDetailType, nullptr);
					}
					detailType = DT_RectWindowStone;
				}

				//UpdateConstructionUI(EPropertyDetailType::PDT_WindowRectBay, FArmySceneData::Get()->GetJsonDataByItemID(TempItemID));

				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(CurrentDetailType);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}

				//ExeWhileShowDetail(detailType);

			}
			//@郭子阳 不能点击水电点位，屏蔽相关代码
			//else if (ParentActor)
			//{
			//	InitGroundHeightArray();
			//	int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
			//	UpdateConstructionUI(EPropertyDetailType::PDT_HYDROPOWERMODE, FArmySceneData::Get()->GetJsonDataByItemID(TempItemID));
			//	TSharedPtr<SWidget> *TempWidget;
			//	TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_HYDROPOWERMODE);
			//	if (TempWidget)
			//	{
			//		PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
			//	}
			//}
			else if (ExtrusionActor)//@梁晓菲 放样Actor属性
			{
				TMap<FName, TSharedPtr<FArmyDetailNode>> node = DetailDXFModel->GetChildrenNodes();
				TSharedPtr<FArmyDetailNode> RaiseNodeX = nullptr;
				TSharedPtr<FArmyDetailNode> RaiseNodeY = nullptr;
				TSharedPtr<FArmyDetailNode> RaiseNodeZ = nullptr;
				for (auto& It : node)
				{
					RaiseNodeX = It.Value->FindChildNode(FName(TEXT("Length")));
					RaiseNodeY = It.Value->FindChildNode(FName(TEXT("Width")));
					RaiseNodeZ = It.Value->FindChildNode(FName(TEXT("Height")));
					if (SelectedActor->ActorHasTag(FName("IsSale")))
					{
						SetDetailNodeProperty(RaiseNodeX, true, false);
						SetDetailNodeProperty(RaiseNodeY, true, false);
						SetDetailNodeProperty(RaiseNodeZ, true, false);
					}
					else
					{
						SetDetailNodeProperty(RaiseNodeX, true, false);
						SetDetailNodeProperty(RaiseNodeY, true, false);
						SetDetailNodeProperty(RaiseNodeZ, true, false);
					}
				}
		
				CurrentDetailType = EPropertyDetailType::PDT_DXFModel;

				SelectedRoom = ExtrusionActor->GetRoom();

				if (ExtrusionActor->Tags.Contains(TEXT("LampSlotActor"))
					&& ExtrusionActor->AttachSurfaceArea.Pin()->LampContentItem.IsValid()
					)
				{
					CurrentDetailType = EPropertyDetailType::PDT_LampSlot;
					//获取灯槽施工项
					ConstructionPatameters Parameter;
					Parameter.SetNormalGoodsInfo(ExtrusionActor->AttachSurfaceArea.Pin()->LampContentItem->ID);
					XRConstructionManager::Get()->TryToFindConstructionData(ExtrusionActor->AttachSurfaceArea.Pin()->GetUniqueID(), Parameter, FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(ExtrusionActor->AttachSurfaceArea.Pin())));

					detailType = DT_ExtrusionLine;

				}
				else if(ExtrusionActor->AttachSurfaceArea.IsValid())
				{
					XRConstructionManager::Get()->TryToFindConstructionData(ExtrusionActor->AttachSurfaceArea.Pin()->GetUniqueID(), ExtrusionActor->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(ExtrusionActor->AttachSurfaceArea.Pin())));
					detailType = DT_ExtrusionLine;
	
				}

				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(CurrentDetailType);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}
			}
			else
			{
				auto XRActor = Cast<AXRActor>(SelectedActor->GetOwner());
				if (XRActor && XRActor->IsValidLowLevel())
				{
					auto FurnitureObj = StaticCastSharedPtr<FArmyFurniture>(XRActor->GetRelevanceObject().Pin());
					SelectedRoom = FurnitureObj->GetRoom();
					int32 ItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(SelectedActor);
					CurrentDetailType = EPropertyDetailType::PDT_PAKModel;

					// 施工项	
					XRConstructionManager::Get()->TryToFindConstructionData(FurnitureObj->GetUniqueID(), FurnitureObj->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(FurnitureObj)));
				}
				else
				{
					UpdateConstructionUI(EPropertyDetailType::PDT_PAKModel, nullptr);
				}
				
				detailType = DT_Actor;

				TSharedPtr<SWidget> *TempWidget;
				TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_PAKModel);
				if (TempWidget)
				{
					PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
				}
			}
			if (WindowActor)
			{
				//CachedName = WindowActor->WindowStoneName;
				CachedScaleX = WindowActor->BayWindowDepth;
				CachedScaleY = WindowActor->WinowLength*10.f;
				CachedScaleZ = WindowActor->WindowHeight*10.0f;
				CachedWindowRaise = WindowActor->WindowOffsetGroundHeight*10.0f;
			}
			/* @梁晓菲 放样尺寸属性显示设置*/
			else if (ExtrusionActor)
			{
				CachedName = ExtrusionActor->dxfName;
				CachedScaleX = ExtrusionActor->dxfLength;
				CachedScaleY = ExtrusionActor->dxfWidth;
				CachedScaleZ = ExtrusionActor->dxfHeight;
			}
			else
			{
				CachedName = SelectedActor->GetName();
				FBox aggBox = GVC->GetActorCollisionBox(SelectedActor);
				FVector size = aggBox.GetSize();
				CachedLocationZ = FMath::CeilToInt((aggBox.GetCenter().Z - size.Z / 2) * 10.0f);
				CachedRotationZ = SelectedActor->GetActorRotation().Yaw;

				FVector aggBoxDefault = CalcActorDefaultSize(SelectedActor);
				FVector Scale3D = SelectedActor->GetActorScale3D();
				float NewScaleX = FMath::Abs(aggBoxDefault.X * Scale3D.X * 10.0f);
				if (FMath::IsNearlyZero(CachedScaleX) || !FMath::IsNearlyEqual(CachedScaleX, NewScaleX))
				{
					CachedScaleX = NewScaleX;
				}
				float NewScaleY = FMath::Abs(aggBoxDefault.Y * Scale3D.Y * 10.0f);
				if (FMath::IsNearlyZero(CachedScaleY) || !FMath::IsNearlyEqual(CachedScaleY, NewScaleY))
				{
					CachedScaleY = NewScaleY;
				}
	
				CachedScaleZ = FMath::Abs(size.Z * Scale3D.Z * 10.f);
			}
		}
		else
		{
			SetPropertyPanelVisibility(EVisibility::Collapsed);
		}
	}
	else
	{
		// 不选中物体时，隐藏替换面板
		if (DoorGoodsReplaceList.IsValid())
		{
			GGI->WindowOverlay->RemoveSlot(DoorGoodsReplaceList.ToSharedRef());
			DoorGoodsReplaceList = nullptr;
		}
		if (PassGoodsReplaceList.IsValid())
		{
			GGI->WindowOverlay->RemoveSlot(PassGoodsReplaceList.ToSharedRef());
			PassGoodsReplaceList = nullptr;
		}

		SelectedUObject = InSelectedObject;
		selectedObject = nullptr;//同一时刻只能存在一种选择对象

		//@马云龙 未选中任何物体的时候 显示世界属性
		TryToggleWorldSettings();
	}
}

TSharedPtr<SWidgetSwitcher> FArmyHardModeDetail::GetPropertyPanel()
{
	return PropertyPanel;
}

TSharedPtr<FArmyBaseArea> FArmyHardModeDetail::GetSelectObject()
{
	return selectedObject;
}

TSharedPtr<class FArmyFurniture> FArmyHardModeDetail::GetSelectFurniture()
{
	if (SelectedUObject && SelectedUObject->IsValidLowLevel())
	{
		AActor* SelfActor = Cast<AActor>(SelectedUObject);
		if (SelfActor&&SelfActor->IsValidLowLevel())
		{
			AXRFurnitureActor* ParentActor = Cast<AXRFurnitureActor>(SelfActor->GetOwner());
			if (ParentActor&& ParentActor->IsValidLowLevel())
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ParentActor->GetRelevanceObject().Pin());
				return Furniture;
			}
		}
	}
	return nullptr;
}

void FArmyHardModeDetail::SetSelectObject(TSharedPtr<FArmyBaseArea> _SelectObject, bool isOrignalsurface)
{
	SelectedRoom = nullptr;

	//测试代码，输出面所在的房间名
	if (_SelectObject.IsValid())
	{
		auto Room = _SelectObject->GetRoom();
		if (Room.IsValid())
		{
			UE_LOG(LogTemp,Warning,TEXT("面所在房间名： %s"), *Room->GetSpaceName())
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("面所在房间名： 未找到"))
		}
	}

	if (WindowRectBayMatReplaceList.IsValid())
	{
		GGI->WindowOverlay->RemoveSlot(WindowRectBayMatReplaceList.ToSharedRef());
		WindowRectBayMatReplaceList = nullptr;
	}

	// 不选中物体或者选中的物体是不是门时，隐藏替换面板
	if (DoorGoodsReplaceList.IsValid())
	{
		GGI->WindowOverlay->RemoveSlot(DoorGoodsReplaceList.ToSharedRef());
		DoorGoodsReplaceList = nullptr;
	}
	if (PassGoodsReplaceList.IsValid())
	{
		GGI->WindowOverlay->RemoveSlot(PassGoodsReplaceList.ToSharedRef());
		PassGoodsReplaceList = nullptr;
	}

	selectedObject = _SelectObject;
	if (!_SelectObject.IsValid())
	{
		SetPropertyPanelVisibility(EVisibility::Collapsed);
		return;
	}
	else
	{
		SelectedRoom = _SelectObject->GetRoom();
	}
	TSharedPtr<FArmyBaseEditStyle> editStyle = nullptr;

	if (isOrignalsurface)
	{
		//@郭子阳 获得原始墙面的style
		editStyle = MakeShared<FArmySeamlessStyle>();
	}
	else
	{
		editStyle = _SelectObject->GetStyle();
	}
	this->IsOrignalsurface = isOrignalsurface;

	if (!editStyle.IsValid())
	{
		SetPropertyPanelVisibility(EVisibility::Collapsed);
		return;
	}

	TSharedPtr<FContentItemSpace::FContentItem> GoodItem = editStyle->GetCurrentItem();
	if (GoodItem.IsValid())
	{
		CachedName = GoodItem->Name;
	}
	else
		CachedName = " ";/* @梁晓菲 先清空，不然会记录上一次的名字*/
						 //改变属性时，将3D模型的选择集设置为空
	EStyleType type = editStyle->GetEditType();
	SelectedUObject = nullptr;

	if (_SelectObject->GetType() == OT_BodaArea)
	{
		SetPropertyPanelVisibility(EVisibility::Visible);
			CurrentDetailType = EPropertyDetailType::PDT_WaveLine;
		if (GoodItem.IsValid())
		{
			//请求并展示施工项
			ConstructionPatameters Parameter;
			bool HasHung = false;

			auto BodaArea = StaticCastSharedPtr<FArmyBodaArea>(selectedObject);
			XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), BodaArea->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(selectedObject)));
		}
		TSharedPtr<SWidget> *TempWidget;
		TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WaveLine);
		if (TempWidget)
		{
			PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
		}
		return;
	}

	for (auto detailMapItem : DetailBuilderMap)
	{
		TSharedPtr<class FArmyDetailBuilder> detailItem = detailMapItem.Value;
		TMap<FName, TSharedPtr<FArmyDetailNode>> node = detailItem->GetChildrenNodes();
		TSharedPtr<FArmyDetailNode> RaiseNodeX = nullptr;
		TSharedPtr<FArmyDetailNode> RaiseNodeY = nullptr;
		TSharedPtr<FArmyDetailNode> RaiseNodeZ = nullptr;
		TSharedPtr<FArmyDetailNode> MaterialLength = nullptr;
		TSharedPtr<FArmyDetailNode> MaterialWidth = nullptr;
		for (auto& It : node)
		{
			RaiseNodeX = It.Value->FindChildNode(FName(TEXT("ScaleX")));
			RaiseNodeY = It.Value->FindChildNode(FName(TEXT("ScaleY")));
			RaiseNodeZ = It.Value->FindChildNode(FName(TEXT("ScaleZ")));
			MaterialLength = It.Value->FindChildNode(FName(TEXT("MaterialLength")));
			MaterialWidth = It.Value->FindChildNode(FName(TEXT("MaterialWidth")));
			SetDetailNodeProperty(RaiseNodeX, true, false);
			SetDetailNodeProperty(RaiseNodeY, true, false);
			SetDetailNodeProperty(RaiseNodeZ, true, false);
			SetDetailNodeProperty(MaterialLength, true, false);
			SetDetailNodeProperty(MaterialWidth, true, false);
		}
	}

	SetPropertyPanelVisibility(EVisibility::Visible);
	ShowPavingMethod(_SelectObject);

	for (auto detailMapItem : DetailBuilderMap)
	{
		TSharedPtr<class FArmyDetailBuilder> detailItem = detailMapItem.Value;
		TMap<FName, TSharedPtr<FArmyDetailNode>> node = detailItem->GetChildrenNodes();
		TSharedPtr<FArmyDetailNode> EditAreaNode = nullptr;
		for (auto& It : node)
		{
			EditAreaNode = It.Value->FindChildNode(FName(TEXT("EditArea")));
			if (EditAreaNode.IsValid())
			{
				EditAreaNode->SetNodeVisibility(EVisibility::Visible);
				break;
			}
		}
		if (selectedObject->SurportPlaceArea == 2)
		{
			SetEditAreaText(FText::FromString(TEXT("编辑顶面")));
			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(selectedObject);
			if (tempRoom->GenerateFromObjectType == OT_Beam
				|| selectedObject->GetType() == OT_HelpRectArea
				|| selectedObject->GetType() == OT_ClinderArea)
			{
				if (EditAreaNode.IsValid())
				{
					EditAreaNode->SetNodeVisibility(EVisibility::Collapsed);
				}
			}
		}
		else if (selectedObject->SurportPlaceArea == 1)
		{
			SetEditAreaText(FText::FromString(TEXT("编辑墙面")));
		}
		else if (selectedObject->SurportPlaceArea == 0)
		{
			SetEditAreaText(FText::FromString(TEXT("编辑地面")));
			TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(selectedObject);
			if (selectedObject->GetType() == OT_HelpRectArea || selectedObject->GetType() == OT_ClinderArea)
			{
				if (EditAreaNode.IsValid())
				{
					EditAreaNode->SetNodeVisibility(EVisibility::Collapsed);
				}
			}
		}
	}

	//@郭子阳
	CurrentCheckData = nullptr; //清空勾选数据
	//处理未铺贴的原始面（原始墙顶地和原始绘制区域）
	if (!GoodItem.IsValid())
	{
		//是不是绘制区域
		bool IsPlotArea = selectedObject->IsPlotArea();
		EWallType wallType = (EWallType)selectedObject->SurfaceType;
  	//保存之前的施工项
		ConstructionDetail->RefreshCheckData(nullptr);
		//清空UI
		ConstructionDetail->Init(nullptr);
		//获取detail类型
		switch (wallType)
		{
		case EWallType::Floor:
			if (IsPlotArea)
			{
				CurrentDetailType = EPropertyDetailType::PDT_PlotArea_RoughFloor;
			}
			else
			{
				CurrentDetailType = EPropertyDetailType::PDT_RoughFloor;
			}
			detailType = DetailType::DT_RoughFloor;
			break;
		case EWallType::Roof:
			if (IsPlotArea)
			{
				CurrentDetailType = EPropertyDetailType::PDT_PlotArea_RoughRoof;
			}
			else
			{
				CurrentDetailType = EPropertyDetailType::PDT_RoughRoof;
			}
			detailType = DetailType::DT_RoughRoof;
			break;
		case EWallType::Wall:
			if (IsPlotArea)
			{
				CurrentDetailType = EPropertyDetailType::PDT_PlotArea_RoughWall;
			}
			else
			{
				CurrentDetailType = EPropertyDetailType::PDT_RoughWall;
			}
			detailType = DetailType::DT_RoughWall;
			break;
		}

		//请求施工项
		int32 SpaceID = -1;
		auto Room = selectedObject->GetRoom();
		if (Room.IsValid())
		{
			SpaceID = Room->GetSpaceId();
		}
		ConstructionPatameters Parameter;
		Parameter.SetOriginalSurface(wallType, selectedObject->GetExtrusionHeight() != 0, SpaceID);

		XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), Parameter, FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(selectedObject)));

		ExeWhileShowDetail(detailType);

		//设置选择的UI
		TSharedPtr<SWidget> *TempWidget;
		TempWidget = SWidgetMap.Find(CurrentDetailType);
		if (TempWidget)
		{
			PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
		}
		SetSelecteAlignmentType(selectedObject->GetStyle()->GetAlignType());
		return;
	}

	detailType = DetailType::Index_None;
	switch (type)
	{
	case S_ContinueStyle:
	case S_SlopeContinueStyle:
	case S_HerringBoneStyle:
	case S_TrapeZoidStyle:
	case S_WhirlwindStyle:
	case S_WorkerStyle:
	case S_CrossStyle:
	case S_IntervalStyle:
	{
		if (GoodItem.IsValid())
		{
			CurrentDetailType = EPropertyDetailType::PDT_FloorTile;
			BorderColor = selectedObject->GetGapColor();
			if (GoodItem.IsValid())
			{
				XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(selectedObject)));

			}
			TSharedPtr<SWidget> *TempWidget;
			TempWidget = SWidgetMap.Find(CurrentDetailType);
			if (TempWidget)
			{
				PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
			}
			SetSelecteAlignmentType(selectedObject->GetStyle()->GetAlignType());
		}

		break;
	}
	case S_SeamlessStyle:
		if ((!isOrignalsurface) && GoodItem.IsValid() && GoodItem->codeStrId.Equals(TEXT("GC0294")))
		{
			WallPaintBorderColor = editStyle->GetWallPaintColor();
			if (GoodItem.IsValid())
			{
				XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, EPropertyDetailType::PDT_WallPaint, StaticCastSharedPtr<FArmyObject>(selectedObject)));
			}
			TSharedPtr<SWidget> *TempWidget;
			TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaint);
			if (TempWidget)
			{
				PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
			}
		}
		else if (
			!FArmyObject::GetDrawModel(MODE_TOPVIEW)
			&& selectedObject->SurportPlaceArea == 1)
		{
			TSharedPtr<FArmyRoomSpaceArea> TempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(selectedObject);
			if (TempRoom->GenerateFromObjectType == OT_InternalRoom
				|| TempRoom->GenerateFromObjectType == OT_AirFlue
				|| TempRoom->GenerateFromObjectType == OT_Pillar
				|| TempRoom->GenerateFromObjectType == OT_PackPipe
				|| TempRoom->GenerateFromObjectType == OT_WallLine
				|| TempRoom->GenerateFromObjectType == OT_Window
				|| TempRoom->GenerateFromObjectType == OT_FloorWindow
				|| TempRoom->GenerateFromObjectType == OT_ArcWindow
				|| TempRoom->GenerateFromObjectType == OT_CornerBayWindow
				|| TempRoom->GenerateFromObjectType == OT_RectBayWindow
				|| TempRoom->GenerateFromObjectType == OT_TrapeBayWindow
				|| TempRoom->GenerateFromObjectType == OT_AddWall
				)
			{
					//@郭子阳
					CurrentDetailType = EPropertyDetailType::PDT_WallPaper;
					XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, EPropertyDetailType::PDT_WallPaper, StaticCastSharedPtr<FArmyObject>(selectedObject)));

					TSharedPtr<SWidget> *TempWidget;
					TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
					if (TempWidget)
					{

						PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
					}
			}
			else
			{
					//@郭子阳
					CurrentDetailType = EPropertyDetailType::PDT_WallPaper;
		
					XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, EPropertyDetailType::PDT_WallPaper, StaticCastSharedPtr<FArmyObject>(selectedObject)));

					TSharedPtr<SWidget> *TempWidget;
					TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
					if (TempWidget)
					{
						PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
					}
			}
		}
		else if (
			!FArmyObject::GetDrawModel(MODE_TOPVIEW)
			&& selectedObject->SurportPlaceArea == 0)
		{
			TSharedPtr<FArmyRoomSpaceArea> TempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(selectedObject);
			if (TempRoom->GenerateFromObjectType == OT_InternalRoom || TempRoom->GenerateFromObjectType == OT_FloorWindow)
			{
					//@郭子阳
					CurrentDetailType = EPropertyDetailType::PDT_WallPaper;
					TSharedPtr<SWidget> *TempWidget;
					TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
					if (TempWidget)
					{
						PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
					}
			}
			else
			{

					//@郭子阳
					CurrentDetailType = EPropertyDetailType::PDT_WallPaper;
					XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, EPropertyDetailType::PDT_WallPaper, StaticCastSharedPtr<FArmyObject>(selectedObject)));

					TSharedPtr<SWidget> *TempWidget;
					TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
					if (TempWidget)
					{
						PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
					}
			}
		}
		else if (!FArmyObject::GetDrawModel(MODE_TOPVIEW)
			&& selectedObject->SurportPlaceArea == 2)
		{
			TSharedPtr<FArmyRoomSpaceArea> TempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(selectedObject);
			if (TempRoom->GenerateFromObjectType == OT_InternalRoom || TempRoom->GenerateFromObjectType == OT_Beam)
			{
		
					XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, EPropertyDetailType::PDT_WallPaper, StaticCastSharedPtr<FArmyObject>(selectedObject)));

					TSharedPtr<SWidget> *TempWidget;
					TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
					if (TempWidget)
					{
						PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
					}
			}
			else
			{
					//@郭子阳
					CurrentDetailType = EPropertyDetailType::PDT_WallPaper;
	
					UpdateConstructionUI(EPropertyDetailType::PDT_WallPaper, XRConstructionManager::Get()->GetJsonData(selectedObject->GetMatConstructionParameter()));

					TSharedPtr<SWidget> *TempWidget;
					TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
					if (TempWidget)
					{
						PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
					}
			}
		}

		else
		{
			CurrentDetailType = EPropertyDetailType::PDT_WallPaper;

				XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, EPropertyDetailType::PDT_WallPaper, StaticCastSharedPtr<FArmyObject>(selectedObject)));

			TSharedPtr<SWidget> *TempWidget;
			TempWidget = SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper);
			if (TempWidget)
			{
				PropertyPanel->SetActiveWidget(TempWidget->Get()->AsShared());
			}
		}
		break;
	default:
	{
		SetPropertyPanelVisibility(EVisibility::Collapsed);
		break;
	}
	}
}

void FArmyHardModeDetail::SetPropertyPanelVisibility(EVisibility _Visibility)
{
	HardModePropertyPanel->SetVisibility(_Visibility);
}

void FArmyHardModeDetail::SetEditAreaVisibility(const EVisibility _EditAreaVisibility)
{
	EditAreaVisibility = _EditAreaVisibility;
}

TSharedRef<class SEditableTextBox> FArmyHardModeDetail::CreateMaterialName()
{
	TAttribute<FText> MaterialNameText = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(this, &FArmyHardModeDetail::GetName));
	TSharedRef<SEditableTextBox> MaterialNameWidget =
		SAssignNew(MaterialName, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(MaterialNameText)
		.IsEnabled(false)
		.IsReadOnly(true)//设置只读
		;
	// @zengy 增加ToolTip
	AddToolTipForWidget(MaterialNameWidget, MaterialNameText);
	return MaterialNameWidget;
}

TSharedRef<STextBlockImageSpinbox<float>> FArmyHardModeDetail::CreateGap()
{
	return
		SAssignNew(Gap, STextBlockImageSpinbox<float>)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Value(this, &FArmyHardModeDetail::GetGapWidth)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnGapWidthChanged)
		.MinValue(0.0)
		.MaxValue(20.0)
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		.MinChangedValue(0.1)//设置微调数值
		;
}

TSharedRef<STextBlockImageSpinbox<int>> FArmyHardModeDetail::CreateAngle()
{
	return
		SAssignNew(Angle, STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetRotationAngle)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnRotationAngleChanged)
		.MinValue(-359.0)
		.MaxValue(359.0)
		.MinChangedValue(1)//设置微调数值
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		;
}

TSharedRef<STextBlockImageSpinbox<int>> FArmyHardModeDetail::CreateRaise()
{
	return
		SAssignNew(Raise, STextBlockImageSpinbox<int>)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Value(this, &FArmyHardModeDetail::GetRaise)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnRaiseChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.MinChangedValue(1)//设置微调数值
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		;
}
TSharedRef<SWidget> FArmyHardModeDetail::CreateWindowRaise()
{

	return
		SNew(SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.SelectAllTextWhenFocused(true)
		.Text(this, &FArmyHardModeDetail::GetWindowRaise)
		.OnTextCommitted_Raw(this, &FArmyHardModeDetail::OnWindowRaiseCommited)
		//.IsReadOnly(true)//设置只读
		;
}

TSharedRef<STextBlockImageSpinbox<int>> FArmyHardModeDetail::CreateDeviationX()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetXDirOffset)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnXDirOffsetChanged)
		.MinValue(-1000.0)
		.MaxValue(1000.0)
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		.MinChangedValue(1)//设置微调数值
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		;
}

TSharedRef<STextBlockImageSpinbox<int>> FArmyHardModeDetail::CreateDeviationY()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetYDirOffset)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnYDirOffsetChanged)
		.MinValue(-1000.0)
		.MaxValue(1000.0)
		.Font(FSlateFontInfo("PingFangSC-Regular", 12))
		.MinChangedValue(1)//设置微调数值
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		;
}

void FArmyHardModeDetail::CreateEditArea(class FArmyDetailNode & _DetailNode)
{
	_DetailNode.AddChildNode("EditArea")
		.WholeContentOverride()
		[
			SNew(SBox)
			.Visibility(this, &FArmyHardModeDetail::GetEditAreaVisibility)
		.WidthOverride(200)
		.HeightOverride(48)//32+16
		.VAlign(VAlign_Bottom)
		[
			SNew(SBox)
			.Visibility(this, &FArmyHardModeDetail::GetEditAreaVisibility)
		.WidthOverride(200)
		.HeightOverride(32)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
		.OnClicked(this, &FArmyHardModeDetail::OnEditAreaClicked)
		[
			SNew(STextBlock)
			.Text(this, &FArmyHardModeDetail::GetEditAreaText)
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		]

		]
		];
}

void FArmyHardModeDetail::CreateApplyToAllWall(TSharedPtr<FArmyDetailBuilder>& Builder)
{
	FArmyDetailNode& CategoryNode = Builder->AddCategory("ApplyToRoomCategory", TEXT("该分类名称不可见"));


	CategoryNode//.AddChildNode("ApplyToRoomNode")
		.WholeContentOverride()
		[
			SNew(SBox)
			.Visibility(this, &FArmyHardModeDetail::GetEditAreaVisibility)
		.WidthOverride(200)
		.HeightOverride(48)//32+16
		.VAlign(VAlign_Bottom)
		[
			SNew(SBox)
			.Visibility(this, &FArmyHardModeDetail::GetEditAreaVisibility)
		.WidthOverride(200)
		.HeightOverride(32)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))

		.OnClicked(this, &FArmyHardModeDetail::ApplyToAllWall)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("应用到该房间所有墙面")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		]

		]
		];
}

TSharedRef<class SWidget> FArmyHardModeDetail::CreateGridPanel()
{

	return
		SNew(SGridPanel)

		+ SGridPanel::Slot(0, 0)
		.Padding(FMargin(0, 0, 9, 0))
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::LeftTop)
		[
			SAssignNew(Image_Left_Top, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(1, 0)
		.Padding(FMargin(0, 0, 9, 0))
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::MidTop)
		[
			SAssignNew(Image_Mid_Top, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(2, 0)
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::RightTop)
		[
			SAssignNew(Image_Right_Top, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(0, 1)
		.Padding(FMargin(0, 9, 9, 0))
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::LeftMid)
		[
			SAssignNew(Image_Left_Mid, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(1, 1)
		.Padding(FMargin(0, 9, 9, 0))
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::MidMid)
		[
			SAssignNew(Image_Mid_Mid, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(2, 1)
		.Padding(FMargin(0, 9, 0, 0))
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::RightMid)
		[
			SAssignNew(Image_Right_Mid, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(0, 2)
		.Padding(FMargin(0, 9, 9, 0)).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::LeftDown)
		[
			SAssignNew(Image_Left_Down, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(1, 2)
		.Padding(FMargin(0, 9, 9, 0))
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::MidDown)
		[
			SAssignNew(Image_Mid_Down, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]

	+ SGridPanel::Slot(2, 2)
		.Padding(FMargin(0, 9, 0, 0)).HAlign(HAlign_Fill).VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.WidthOverride(24)
		.HeightOverride(24)
		[
			SNew(SButton)
			.ContentPadding(FMargin(0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.FF4D4E52"))
		.OnClicked(this, &FArmyHardModeDetail::OnAlignClicked, AlignmentType::RightDown)
		[
			SAssignNew(Image_Right_Down, SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.None"))
		]
		]
		]
	;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateWallPaintColor()
{
	return
		SNew(SButton)
		.OnClicked(this, &FArmyHardModeDetail::OnWallPaintColorClicked)
		.ContentPadding(FMargin(0))
		.Content()
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
		.BorderBackgroundColor(this, &FArmyHardModeDetail::GetWallPaintBorderColor)
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		];
}

TSharedRef<class SWidget> FArmyHardModeDetail::CreateColorName()
{

	return
		SAssignNew(ColorNameWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.SelectAllTextWhenFocused(true)
		.Text(this, &FArmyHardModeDetail::GetWallPaintColorValue)

		.OnTextCommitted_Lambda([this](const FText & InText, ETextCommit::Type CommitType)
	{
		if (selectedObject.IsValid())
		{
			if (selectedObject->GetStyle()->GetEditType() == S_SeamlessStyle)
			{
				TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(selectedObject->GetStyle());
				if (tempSeamless->SeamlesStyleType == 1)
				{
					if (CommitType == ETextCommit::OnEnter)
					{
						tempSeamless->ColorPaintValueKey = InText.ToString();
					}
				}
			}
		}
	})
		;
}

FText FArmyHardModeDetail::GetWallPaintColorValue() const
{
	if (selectedObject.IsValid())
	{
		if (selectedObject->GetStyle()->GetEditType() == S_SeamlessStyle)
		{
			TSharedPtr<FArmySeamlessStyle> tempSeamless = StaticCastSharedPtr<FArmySeamlessStyle>(selectedObject->GetStyle());
			if (tempSeamless->SeamlesStyleType == 1)
			{
				return FText::FromString(tempSeamless->ColorPaintValueKey);
			}
		}
	}
	return FText();
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightIntensity()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetLight3DIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnLight3DIntensityChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightRadius()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetLight3DRadius)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnLight3DRadiusChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightSourceRadius()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetLight3DSourceRadius)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnLight3DSourceRadiusChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightSourceLength()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetLight3DSourceLength)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnLight3DSourceLengthChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateSpotLightInnerAngle()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetSpotLight3DInnerAngle)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnSpotLight3DInnerAngleChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateSpotLightOuterAngle()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetSpotLight3DOuterAngle)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnSpotLight3DOuterAngleChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightColor()
{
	return
		SNew(SButton)
		.OnClicked(this, &FArmyHardModeDetail::OnColorClicked)
		.ContentPadding(FMargin(0))
		.Content()
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
		.BorderBackgroundColor(this, &FArmyHardModeDetail::GetBorderColor)
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		];
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightEnable()
{
	return
		SNew(SCheckBox)
		.IsChecked(this, &FArmyHardModeDetail::GetLight3DEnable)
		.OnCheckStateChanged(this, &FArmyHardModeDetail::OnLight3DEnableChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"));
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightCastShadow()
{
	return
		SNew(SCheckBox)
		.IsChecked(this, &FArmyHardModeDetail::GetLight3DCastShadow)
		.OnCheckStateChanged(this, &FArmyHardModeDetail::OnLight3DCastShadowStateChanged)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FCheckBoxStyle>("SingleCheckBox"));
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateIndirectLightingIntensity()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetIndirectLightingIntensity)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnIndirectLightingIntensityChanged)
		.MinValue(0.0)
		.MaxValue(10.f)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.f)//设置微调数值
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateLightMobilityTypeWidget()
{
	return
		SAssignNew(LightMobilityTypeComboBox, SArmyPropertyComboBox)
		.OptionsSource(LightMobilityTypeList)
		.OnSelectionChanged(this, &FArmyHardModeDetail::OnLightMobilityTypeChanged)
		.Value(this, &FArmyHardModeDetail::GetLightMobilityType);
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateLightMobilityTypeNameWidget()
{
	return

		SNew(SBox)
		.HeightOverride(24)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("光源类型")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 1)))
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(FMargin(2, 0, 0, 0))
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.ToolTip"))
		.ToolTipText(FText::FromString(TEXT("静态光参与全屋渲染，动态光不参与高级渲染")))
		]
		];
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateReflectionSphereRadius()
{
	return
		SNew(STextBlockImageSpinbox<int>)
		.Value(this, &FArmyHardModeDetail::GetReflectionSphereRadius)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnReflectionSphereRadiusChanged)
		.MinValue(0.0)
		.MaxValue(10000.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(1)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateReflectionSphereBrightness()
{
	return
		SNew(STextBlockImageSpinbox<float>)
		.Value(this, &FArmyHardModeDetail::GetReflectionSphereBrightness)
		.OnValueCommitted(this, &FArmyHardModeDetail::OnReflectionSphereBrightnessChanged)
		.MinValue(0.0)
		.MaxValue(10.0)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FSpinBoxStyle>("SpinBoxStyle"))
		.Font(FSlateFontInfo("PingFangSC-Regular", 10))
		.MinChangedValue(0.01)//设置微调数值
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreatePavingMethodWidget()
{
	return
		SAssignNew(PavingMethodModify, SArmyPropertyComboBox)
		.OptionsSource(Array_Paving)
		.OnSelectionChanged(this, &FArmyHardModeDetail::OnSlectedPavingMethodChanged)
		.Value(this, &FArmyHardModeDetail::GetPavingMethodName);
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateWindowStoneMaterialGoods()
{
	return
		SNew(SArmyReplaceButton).OnClicked_Lambda([this]() {
		if (!WindowRectBayMatReplaceList.IsValid())
		{
			int32 SelectedStoneId = -1;
			AXRWindowActor* WinActor = Cast<AXRWindowActor>(SelectedUObject);
			if (WinActor && WinActor->IsValidLowLevel())
			{
				SelectedStoneId = WinActor->GetSynID();
			}

			FString categoreCode = "GC0026";
			FString title = _T("窗台石");
			SAssignNew(WindowRectBayMatReplaceList, SArmyReplaceList)
				.Title(FText::FromString(title))
				.CategoryCode(categoreCode)
				.Visibility(EVisibility::Visible)
				.NoResourceTipText(MAKE_TEXT("未找到符合条件的窗台石"))
				.DefaultGoodsId(SelectedStoneId)
				.OnClose_Lambda([this]() {
				GGI->WindowOverlay->RemoveSlot(WindowRectBayMatReplaceList.ToSharedRef());
				WindowRectBayMatReplaceList = nullptr;
			})
				.OnReplace_Lambda([this](FContentItemPtr ContentItem) {
				if (ContentItem.IsValid())
				{
					TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
					if (resArr.Num() >= 1)
					{
						FString _FilePath = resArr[0]->FilePath;
						if (FPaths::FileExists(*_FilePath))//文件不存在直接返回
						{
							UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
							UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(ContentItem);
							if (mat)
							{
								UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);
								AXRWindowActor * winActor = dynamic_cast<AXRWindowActor *>(SelectedUObject);
								winActor->SetSynID(mat->GetSynID());
								winActor->SetWindowBoardMaterial(mat);
								int32 SpaceID = -1;
								{
									TSharedPtr<FArmyObject> Obj = nullptr;

									//是不是非飘窗
									bool IsNormalWindow = !(winActor->AttachSurface->GetType() == EObjectType::OT_RectBayWindow
										|| winActor->AttachSurface->GetType() == EObjectType::OT_TrapeBayWindow);

									if (IsNormalWindow)
									{
										auto WindowObj = StaticCastSharedPtr<FArmyWindow>(winActor->AttachSurface);
										WindowObj->SetContentItem(ContentItem);
										Obj = WindowObj;
										SpaceID = WindowObj->GetRoomSpaceID();
									}
									else
									{
										auto WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(winActor->AttachSurface);
										WindowObj->SetContentItem(ContentItem);
										Obj = WindowObj;
										SpaceID = WindowObj->GetRoomSpaceID();
									}

									FGuid Id = Obj->GetUniqueID();
									//@郭子阳
									//请求施工项
									ConstructionPatameters Parameter;
									Parameter.SetNormalGoodsInfo(ContentItem->ID, SpaceID);
									XRConstructionManager::Get()->TryToFindConstructionData(Id, Parameter, FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, Obj));
									ExeWhileShowDetail(detailType);
								}
								/* @梁晓菲 窗台石名字*/
								winActor->WindowStoneName = ContentItem->Name;
								CachedName = winActor->WindowStoneName;
							}
							else
							{
								GGI->Window->ShowMessage(MT_Warning, TEXT("材质球无效"));
							}
						}
					}
				}
			});
			GGI->WindowOverlay->AddSlot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Top)
				.Padding(0, 550, 242, 0)
				[
					WindowRectBayMatReplaceList.ToSharedRef()
				];
		}
		else
		{
			GGI->WindowOverlay->RemoveSlot(WindowRectBayMatReplaceList.ToSharedRef());
			WindowRectBayMatReplaceList = NULL;
		}
		return FReply::Handled();
	})
		.ThumbnailBrush_Lambda([this]() {
		AXRWindowActor * WinActor = Cast<AXRWindowActor>(SelectedUObject);
		if (WinActor && WinActor->IsValidLowLevel())
		{
			TSharedPtr<FContentItem> WindowContentItem = FArmyResourceModule::Get().GetResourceManager()->GetContentItemFromID(WinActor->GetSynID());
			{
				//是不是非飘窗
				bool IsNormalWindow = !(WinActor->AttachSurface->GetType() == EObjectType::OT_RectBayWindow
					|| WinActor->AttachSurface->GetType() == EObjectType::OT_TrapeBayWindow);

				if (IsNormalWindow)
				{
					auto WindowObj = StaticCastSharedPtr<FArmyWindow>(WinActor->AttachSurface);
					WindowContentItem = WindowObj->GetContentItem();

				}
				else
				{
					auto WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(WinActor->AttachSurface);
					WindowContentItem = WindowObj->GetContentItem();
				}

			}

			if (WindowContentItem.IsValid())
			{
				return FArmySlateModule::Get().WebImageCache->Download(WindowContentItem->ThumbnailURL).Get().Attr().Get();
			}
		}

		return FArmyStyle::Get().GetBrush("Icon.DefaultImage");
	});
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateLightCheckBoxDetail()
{
	return
		SNew(SCheckBox)
		.Style(FArmyStyle::Get(), "SingleCheckBox")
		.IsChecked(this, &FArmyHardModeDetail::GetLightCheckBoxState)
		.OnCheckStateChanged(this, &FArmyHardModeDetail::OnListCheckBoxStateChanged);
}

ECheckBoxState FArmyHardModeDetail::GetLightCheckBoxState() const
{
	AArmyExtrusionActor* lampSlotActor = Cast<AArmyExtrusionActor>(SelectedUObject);
	if (lampSlotActor)
	{
		if (lampSlotActor->AttachSurfaceArea.IsValid())
		{
			if (lampSlotActor->AttachSurfaceArea.Pin()->GetType() == OT_RectArea)
			{
				TSharedPtr<FArmyRectArea> tempRectArea = StaticCastSharedPtr<FArmyRectArea>(lampSlotActor->AttachSurfaceArea.Pin());
				return tempRectArea->HasLight() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			}

		}
	}
	return ECheckBoxState::Unchecked;
}

void FArmyHardModeDetail::OnListCheckBoxStateChanged(ECheckBoxState InNewState)
{
	AArmyExtrusionActor* lampSlotActor = Cast<AArmyExtrusionActor>(SelectedUObject);
	if (lampSlotActor)
	{
		if (lampSlotActor->AttachSurfaceArea.IsValid())
		{
			if (lampSlotActor->AttachSurfaceArea.Pin()->GetType() == OT_RectArea)
			{

				TSharedPtr<FArmyRectArea> tempRectArea = StaticCastSharedPtr<FArmyRectArea>(lampSlotActor->AttachSurfaceArea.Pin());
				if (InNewState == ECheckBoxState::Checked)
				{
					tempRectArea->GenerateAllLights();
				}
				else
				{
					tempRectArea->DestroyAllLights();
				}
			}
		}
	}
}

TSharedRef<class SEditableTextBox> FArmyHardModeDetail::CreateRoughName()
{
	return
		SAssignNew(MaterialName, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyHardModeDetail::GetRoughName)
		.IsEnabled(true)
		.IsReadOnly(true)//设置只读
		;
}

TSharedRef<SWidget> FArmyHardModeDetail::CreateDoorGoods()
{
	return
		SAssignNew(DoorReplaceButton, SArmyReplaceButton)
		.OnClicked_Lambda([this]() {
		if (!DoorGoodsReplaceList.IsValid())
		{
			FString CategoryCode, Title;
			int32 SelectedDoorId = -1;

			FObjectWeakPtr DoorObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
			if (DoorObj.IsValid())
			{
				if (DoorObj.Pin()->GetType() == OT_Door)
				{
					CategoryCode = "GC0234";
					Title = TEXT("标准门");
					TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(DoorObj.Pin());
					SelectedDoorId = Door->GetDoorSaleID();
				}
				else if (DoorObj.Pin()->GetType() == OT_SecurityDoor)
				{
					CategoryCode = "GC0237";
					Title = TEXT("防盗门");
					TSharedPtr<FArmySecurityDoor> Door = StaticCastSharedPtr<FArmySecurityDoor>(DoorObj.Pin());
					SelectedDoorId = Door->GetDoorSaleID();
				}
				else if (DoorObj.Pin()->GetType() == OT_SlidingDoor)
				{
					CategoryCode = "GC0240";
					Title = TEXT("推拉门");
					TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(DoorObj.Pin());
					SelectedDoorId = Door->GetDoorSaleID();
				}
			}

			SAssignNew(DoorGoodsReplaceList, SArmyDoorReplaceList)
				.Title(FText::FromString(Title))
				.CategoryCode(CategoryCode)
				.Visibility(EVisibility::Visible)
				.NoResourceTipText(MAKE_TEXT("未找到符合条件的门"))
				.DefaultGoodsId(SelectedDoorId)
				.OnClose_Lambda([this]() {
				GGI->WindowOverlay->RemoveSlot(DoorGoodsReplaceList.ToSharedRef());
				DoorGoodsReplaceList = nullptr;
			})
				.OnReplace_Lambda([this](FContentItemPtr ContentItem) {
				FObjectWeakPtr DoorObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
				if (DoorObj.IsValid())
				{
					if (DoorObj.Pin()->GetType() == OT_Door || DoorObj.Pin()->GetType() == OT_SecurityDoor)
					{
						TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(DoorObj.Pin());
						Door->ReplaceGoods(ContentItem, GGI->GetWorld());
						
						//刷新施工项
						XRConstructionManager::Get()->TryToFindConstructionData(Door->GetUniqueID(), *Door->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, DoorObj.Pin()));
					}
					else if (DoorObj.Pin()->GetType() == OT_SlidingDoor)
					{
						TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(DoorObj.Pin());
						Door->ReplaceGoods(ContentItem, GGI->GetWorld());
				
						//刷新施工项
						XRConstructionManager::Get()->TryToFindConstructionData(Door->GetUniqueID(), *Door->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, DoorObj.Pin()));
					}
				}
			});
			DoorGoodsReplaceList->SetFilterSize(CachedScaleY, CachedScaleZ);

			GGI->WindowOverlay->AddSlot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Top)
				.Padding(0, 550, 242, 0)
				[
					DoorGoodsReplaceList.ToSharedRef()
				];
		}
		else
		{
			GGI->WindowOverlay->RemoveSlot(DoorGoodsReplaceList.ToSharedRef());
			DoorGoodsReplaceList = nullptr;
		}

		return FReply::Handled();
	})
		.ThumbnailBrush_Lambda([this]() {
		FObjectWeakPtr DoorObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
		if (DoorObj.IsValid())
		{
			if (DoorObj.Pin()->GetType() == OT_Door || DoorObj.Pin()->GetType() == OT_SecurityDoor)
			{
				TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(DoorObj.Pin());
				if (!Door->GetThumbnailUrl().IsEmpty())
				{
					return FArmySlateModule::Get().WebImageCache->Download(Door->GetThumbnailUrl()).Get().Attr().Get();
				}
			}
			else if (DoorObj.Pin()->GetType() == OT_SlidingDoor)
			{
				TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(DoorObj.Pin());
				if (!Door->GetThumbnailUrl().IsEmpty())
				{
					return FArmySlateModule::Get().WebImageCache->Download(Door->GetThumbnailUrl()).Get().Attr().Get();
				}
			}
		}

		return FArmyStyle::Get().GetBrush("Icon.DefaultImage");
	});
}

TSharedRef<SWidget> FArmyHardModeDetail::CreatePassGoods()
{
	return
		SNew(SArmyReplaceButton)
		.OnClicked_Lambda([this]() {
		if (!PassGoodsReplaceList.IsValid())
		{
			FString CategoryCode, Title;
			int32 SelectedPassId = -1;

			FObjectWeakPtr PassObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
			if (PassObj.IsValid())
			{
				if (PassObj.Pin()->GetType() == OT_Pass)
				{
					CategoryCode = "GC0669";
					Title = TEXT("垭口");
					TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(PassObj.Pin());
					SelectedPassId = Pass->GetSaleID();
				}
				else if (PassObj.Pin()->GetType() == OT_NewPass)
				{
					CategoryCode = "GC0669";
					Title = TEXT("垭口");
					TSharedPtr<FArmyNewPass> NewPass = StaticCastSharedPtr<FArmyNewPass>(PassObj.Pin());
					SelectedPassId = NewPass->GetSaleID();
				}
			}

			SAssignNew(PassGoodsReplaceList, SArmyDoorReplaceList)
				.Title(FText::FromString(Title))
				.CategoryCode(CategoryCode)
				.Visibility(EVisibility::Visible)
				.NoResourceTipText(MAKE_TEXT("未找到符合条件的垭口"))
				.DefaultGoodsId(SelectedPassId)
				.OnClose_Lambda([this]() {
				GGI->WindowOverlay->RemoveSlot(PassGoodsReplaceList.ToSharedRef());
				PassGoodsReplaceList = nullptr;
			})
				.OnReplace_Lambda([this](FContentItemPtr ContentItem) {
				FObjectWeakPtr PassObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
				if (PassObj.IsValid())
				{
					if (PassObj.Pin()->GetType() == OT_Pass)
					{
						TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(PassObj.Pin());
						Pass->ReplaceGoods(ContentItem, GGI->GetWorld());
						//刷新施工项
						XRConstructionManager::Get()->TryToFindConstructionData(Pass->GetUniqueID(), *Pass->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, PassObj.Pin()));
					}
					else if (PassObj.Pin()->GetType() == OT_NewPass)
					{
						TSharedPtr<FArmyNewPass> Pass = StaticCastSharedPtr<FArmyNewPass>(PassObj.Pin());
						Pass->ReplaceGoods(ContentItem, GGI->GetWorld());
						//刷新施工项
						XRConstructionManager::Get()->TryToFindConstructionData(Pass->GetUniqueID(), *Pass->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, PassObj.Pin()));
					}
				}
			});
			PassGoodsReplaceList->SetFilterSize(CachedScaleY, CachedScaleZ);

			GGI->WindowOverlay->AddSlot()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Top)
				.Padding(0, 550, 242, 0)
				[
					PassGoodsReplaceList.ToSharedRef()
				];
		}
		else
		{
			GGI->WindowOverlay->RemoveSlot(PassGoodsReplaceList.ToSharedRef());
			PassGoodsReplaceList = nullptr;
		}

		return FReply::Handled();
	})
		.ThumbnailBrush_Lambda([this]() {
		FObjectWeakPtr PassObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
		if (PassObj.IsValid())
		{
			if (PassObj.Pin()->GetType() == OT_Pass)
			{
				TSharedPtr<FArmyPass> Pass = StaticCastSharedPtr<FArmyPass>(PassObj.Pin());
				if (!Pass->GetThumbnailUrl().IsEmpty())
				{
					return FArmySlateModule::Get().WebImageCache->Download(Pass->GetThumbnailUrl()).Get().Attr().Get();
				}
			}
			else if (PassObj.Pin()->GetType() == OT_NewPass)
			{
				TSharedPtr<FArmyNewPass> NewPass = StaticCastSharedPtr<FArmyNewPass>(PassObj.Pin());
				if (!NewPass->GetThumbnailUrl().IsEmpty())
				{
					return FArmySlateModule::Get().WebImageCache->Download(NewPass->GetThumbnailUrl()).Get().Attr().Get();
				}
			}
		}

		return FArmyStyle::Get().GetBrush("Icon.DefaultImage");
	});
}

FText FArmyHardModeDetail::GetScaleY() const
{
	float width = 0.f;
	if (selectedObject.IsValid())
	{
		width = selectedObject->GetStyle()->GetMainTexHeight();
		return FText::FromString(FString::FromInt(width));
	}
	else if (SelectedUObject != nullptr)
	{
		return FText::FromString(FString::FromInt(CachedScaleY));
	}
	return FText::FromString(FString::FromInt(width));
}

FText FArmyHardModeDetail::GetScaleZ() const
{
	float ScaleZ = 0.f;

	if (SelectedUObject != nullptr)
	{
		return FText::FromString(FString::FromInt(CachedScaleZ));
	}
	return FText::FromString(FString::FromInt(ScaleZ));
}

FText FArmyHardModeDetail::GetRoughLength() const
{
	float Length = 0.f;
	if (selectedObject.IsValid())
	{
		float Area = FArmyMath::CalcPolyArea(selectedObject->GetOutArea()->Vertices);
		Length = 10.0f*Area / FArmySceneData::Get()->WallHeight;
	}
	return FText::FromString(FString::FromInt(Length));
}

FText FArmyHardModeDetail::GetWallHeight() const
{
	return FText::FromString(FString::FromInt(FArmySceneData::Get()->WallHeight * 10));
}

FText FArmyHardModeDetail::GetArea() const
{
	float Area = 0.f;
	float InnerArea = 0.f;
	if (selectedObject.IsValid())
	{
		Area = FArmyMath::CalcPolyArea(selectedObject->GetInnerArea()->Vertices) / 10000;
		TSharedPtr<FArmyBaseArea> SelectedArea = StaticCastSharedPtr<FArmyBaseArea>(selectedObject);

		return FText::FromString(FString::Printf(TEXT("%.2f"), SelectedArea->GetAreaDimensionWithVisibility()));
	}
	return FText::FromString(FString::Printf(TEXT("%.2f"), Area));
}

FText FArmyHardModeDetail::GetScaleX() const
{
	float length = 0.f;
	if (selectedObject.IsValid())
	{
		length = selectedObject->GetStyle()->GetMainTexWidth();
		return FText::FromString(FString::FromInt(length));
	}
	else if (SelectedUObject != nullptr)
	{
		return FText::FromString(FString::FromInt(CachedScaleX));
	}
	return FText::FromString(FString::FromInt(length));
}

void FArmyHardModeDetail::CreateDetailFloorTile(EWallType WallType)
{
	DetailFloorTile = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailFloorTile->AddCategory("FloorTilePropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialLength", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialWidth", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("DeviationX", TEXT("X偏移(mm)"), CreateDeviationX()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("DeviationY", TEXT("Y偏移(mm)"), CreateDeviationY()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateAngle()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Gap", TEXT("缝隙(mm)"), CreateGap()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightColor", TEXT("缝隙颜色"), CreateLightColor()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	if (Array_Paving->Array.Num() > 0)
	{
		TileAndFloorPropertyCategory.AddChildNode("PvaingMethod", TEXT("铺法"), CreatePavingMethodWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	}
	CreateRaiseDetailUI(TileAndFloorPropertyCategory, WallType);

	CreateEditArea(TileAndFloorPropertyCategory);

	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailFloorTile->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}

	if (WallType == EWallType::Wall)
	{
		CreateApplyToAllWall(DetailFloorTile);
	}

	DetailBuilderMap.Add(EPropertyDetailType::PDT_FloorTile, DetailFloorTile);
	SWidgetMap.Add(EPropertyDetailType::PDT_FloorTile, DetailFloorTile->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_FloorTile)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailFloorTile->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailWaterKnife()
{
	DetailWaterKnife = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailWaterKnife->AddCategory("WaterKnifePropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialLength", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialWidth", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateAngle()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailWaterKnife->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_WaterKnife, DetailWaterKnife);
	SWidgetMap.Add(EPropertyDetailType::PDT_WaterKnife, DetailWaterKnife->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WaterKnife)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailWaterKnife->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailWaveLine()
{
	DetailWaveLine = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailWaveLine->AddCategory("WaveLinePropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialLength", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialWidth", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Gap", TEXT("缝隙(mm)"), CreateGap()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailWaveLine->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_WaveLine, DetailWaveLine);
	SWidgetMap.Add(EPropertyDetailType::PDT_WaveLine, DetailWaveLine->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WaveLine)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailWaveLine->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailDXFModel()
{
	DetailDXFModel = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailDXFModel->AddCategory("DXFModelPropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailDXFModel->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_DXFModel, DetailDXFModel);
	SWidgetMap.Add(EPropertyDetailType::PDT_DXFModel, DetailDXFModel->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_DXFModel)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailDXFModel->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailWallPaint(EWallType WallType)
{
	DetailWallPaint = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailWallPaint->AddCategory("WallPaintPropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Color", TEXT("颜色"), CreateWallPaintColor()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("ColorName", TEXT("厂家色号"), CreateColorName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
    TileAndFloorPropertyCategory.AddChildNode("Raise", TEXT("挤出(mm)"), CreateRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CreateRaiseDetailUI(TileAndFloorPropertyCategory, WallType);
	CreateEditArea(TileAndFloorPropertyCategory);
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailWallPaint->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}

	if (WallType == EWallType::Wall)
	{
		CreateApplyToAllWall(DetailWallPaint);
	}

	DetailBuilderMap.Add(EPropertyDetailType::PDT_WallPaint, DetailWallPaint);
	SWidgetMap.Add(EPropertyDetailType::PDT_WallPaint, DetailWallPaint->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WallPaint)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailWallPaint->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailWallPaper(EWallType WallType)
{
	DetailWallPaper = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailWallPaper->AddCategory("WallPaperPropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialLength", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialWidth", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("DeviationX", TEXT("X偏移(mm)"), CreateDeviationX()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("DeviationY", TEXT("Y偏移(mm)"), CreateDeviationY()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
    TileAndFloorPropertyCategory.AddChildNode("Raise", TEXT("挤出(mm)"), CreateRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	CreateRaiseDetailUI(TileAndFloorPropertyCategory, WallType);
	CreateEditArea(TileAndFloorPropertyCategory);

	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailWallPaper->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}

	if (WallType == EWallType::Wall)
	{
		CreateApplyToAllWall(DetailWallPaper);
	}

	DetailBuilderMap.Add(EPropertyDetailType::PDT_WallPaper, DetailWallPaper);
	SWidgetMap.Add(EPropertyDetailType::PDT_WallPaper, DetailWallPaper->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WallPaper)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailWallPaper->GetTreeView());
}

//铝扣板
void FArmyHardModeDetail::CreateDetailAluminousGussetPlate()
{
	DetailAluminousGussetPlate = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailAluminousGussetPlate->AddCategory("AluminousGussetPlatePropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialLength", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("MaterialWidth", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Angle", TEXT("角度(°)"), CreateAngle()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CreateRaiseDetailUI(TileAndFloorPropertyCategory, EWallType::Roof);
	CreateEditArea(TileAndFloorPropertyCategory);
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailAluminousGussetPlate->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_AluminousGussetPlate, DetailAluminousGussetPlate);
	SWidgetMap.Add(EPropertyDetailType::PDT_AluminousGussetPlate, DetailAluminousGussetPlate->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_AluminousGussetPlate)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailAluminousGussetPlate->GetTreeView());
}

//灯、浴霸、排风扇以及3D模型
void FArmyHardModeDetail::CreateDetailPAKModel()
{
	DetailPAKModel = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& TransformCategory = DetailPAKModel->AddCategory("PAKModelPropertyCategory", TEXT("属性"));

	TransformCategory.AddChildNode("Name", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TransformCategory.AddChildNode("ScaleX", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TransformCategory.AddChildNode("ScaleY", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TransformCategory.AddChildNode("ScaleZ", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TransformCategory.AddChildNode("RotationZ", TEXT("角度(°)"), CreateAngle()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TransformCategory.AddChildNode("LocationZ", TEXT("离地高度(mm)"), CreateRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailPAKModel->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_PAKModel, DetailPAKModel);
	SWidgetMap.Add(EPropertyDetailType::PDT_PAKModel, DetailPAKModel->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_PAKModel)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailPAKModel->GetTreeView());
}

//灯槽
void FArmyHardModeDetail::CreateDetailLampSlot()
{
	DetailLampSlot = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailLampSlot->AddCategory("LampSlotPropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailLampSlot->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_LampSlot, DetailLampSlot);
	SWidgetMap.Add(EPropertyDetailType::PDT_LampSlot, DetailLampSlot->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_LampSlot)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailLampSlot->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailPointLight()
{
	DetailPointLight = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailPointLight->AddCategory("TilePropertyCategory", TEXT("属性"));

	TileAndFloorPropertyCategory.AddChildNode("PointLightIntensity", TEXT("强度"), CreateLightIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightRadius", TEXT("辐射半径"), CreateLightRadius()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightSourceRadius", TEXT("光源半径"), CreateLightSourceRadius()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightSourceLength", TEXT("光源长度"), CreateLightSourceLength()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightIndirectLightingIntensity", TEXT("间接光强度"), CreateIndirectLightingIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightColor", TEXT("颜色"), CreateLightColor()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	//TileAndFloorPropertyCategory.AddChildNode("PointLightCastShadow", TEXT("投影"), CreateLightCastShadow()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("PointLightEnable", TEXT("启用"), CreateLightEnable()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DetailBuilderMap.Add(EPropertyDetailType::PDT_PointLight, DetailPointLight);
	SWidgetMap.Add(EPropertyDetailType::PDT_PointLight, DetailPointLight->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_PointLight)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::CreateDetailSpotLight()
{
	DetailSpotLight = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailSpotLight->AddCategory("TilePropertyCategory", TEXT("属性"));

	TileAndFloorPropertyCategory.AddChildNode("SpotLightIntensity", TEXT("亮度"), CreateLightIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightRadius", TEXT("半径"), CreateLightRadius()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightSourceRadius", TEXT("光源半径"), CreateLightSourceRadius()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightSourceLength", TEXT("光源长度"), CreateLightSourceLength()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightInnerAngle", TEXT("内径"), CreateSpotLightInnerAngle()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightOuterAngle", TEXT("外径"), CreateSpotLightOuterAngle()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightIndirectLightingIntensity", TEXT("间接光强度"), CreateIndirectLightingIntensity()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightColor", TEXT("颜色"), CreateLightColor()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	//TileAndFloorPropertyCategory.AddChildNode("SpotLightCastShadow", TEXT("投影"), CreateLightCastShadow()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("SpotLightEnable", TEXT("启用"), CreateLightEnable()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DetailBuilderMap.Add(EPropertyDetailType::PDT_SpotLight, DetailSpotLight);
	SWidgetMap.Add(EPropertyDetailType::PDT_SpotLight, DetailSpotLight->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_SpotLight)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::CreateDetailReflectionSphere()
{
	DetailReflectionCapture = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailReflectionCapture->AddCategory("TilePropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("ReflectionSphereRadius", TEXT("影响范围"), CreateReflectionSphereRadius()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("ReflectionSphereBrightness", TEXT("亮度"), CreateReflectionSphereBrightness()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DetailBuilderMap.Add(EPropertyDetailType::PDT_ReflectionSphere, DetailReflectionCapture);
	SWidgetMap.Add(EPropertyDetailType::PDT_ReflectionSphere, DetailReflectionCapture->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_ReflectionSphere)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::CreateDetailWindowNormal()
{
	DetailWindowNormal = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& WindowCategory = DetailWindowNormal->AddCategory("WindowNormalModelPropertyCategory", TEXT("属性"));

	WindowCategory.AddChildNode("Name", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowScaleZ", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowScaleY", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowLocationZ", TEXT("离地高度(mm)"), CreateWindowRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowColor", TEXT("窗台石材质"), CreateWindowStoneMaterialGoods()).ValueContentOverride().Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailWindowNormal->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_WindowNormal, DetailWindowNormal);
	SWidgetMap.Add(EPropertyDetailType::PDT_WindowNormal, DetailWindowNormal->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WindowNormal)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailWindowNormal->GetTreeView());
}

void FArmyHardModeDetail::CreateDetailWindowRectBay()
{
	DetailWindowRectBay = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& WindowCategory = DetailWindowRectBay->AddCategory("WindowRectBayModelPropertyCategory", TEXT("属性"));

	WindowCategory.AddChildNode("Name", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowScaleZ", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowScaleY", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowScaleX", TEXT("深度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowLocationZ", TEXT("离地高度(mm)"), CreateWindowRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("WindowColor", TEXT("窗台石材质"), CreateWindowStoneMaterialGoods()).ValueContentOverride().Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailWindowRectBay->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_WindowRectBay, DetailWindowRectBay);
	SWidgetMap.Add(EPropertyDetailType::PDT_WindowRectBay, DetailWindowRectBay->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_WindowRectBay)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailWindowRectBay->GetTreeView());
}

void FArmyHardModeDetail::CreateOutletDetail()
{
	OutletDetailBuilder = MakeShareable(new FArmyDetailBuilder);
	FArmyDetailNode& WindowCategory = OutletDetailBuilder->AddCategory("WindowWaterModelPropertyCategory", TEXT("属性"));

	WindowCategory.AddChildNode("ComponentName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("GroundHeight", TEXT("离地高度(mm)"), CreateLocationZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	WindowCategory.AddChildNode("Refrom", TEXT("断点改造"), CreateRefromWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = OutletDetailBuilder->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_HYDROPOWERMODE, OutletDetailBuilder);
	SWidgetMap.Add(EPropertyDetailType::PDT_HYDROPOWERMODE, OutletDetailBuilder->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_HYDROPOWERMODE)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(OutletDetailBuilder->GetTreeView());
}

void FArmyHardModeDetail::CreateHasLightDXFDetail()
{
	DetailDXFModel = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& TileAndFloorPropertyCategory = DetailDXFModel->AddCategory("DXFModelPropertyCategory", TEXT("属性"));
	TileAndFloorPropertyCategory.AddChildNode("MaterialName", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateScaleXWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	TileAndFloorPropertyCategory.AddChildNode("HasLight", TEXT("灯带"), CreateLightCheckBoxDetail()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailDXFModel->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_DXFMODELRectLAMPSLOT, DetailDXFModel);
	SWidgetMap.Add(EPropertyDetailType::PDT_DXFMODELRectLAMPSLOT, DetailDXFModel->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_DXFMODELRectLAMPSLOT)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailDXFModel->GetTreeView());
}

void FArmyHardModeDetail::CreateDoorDetail()
{
	DetailDoor = MakeShareable(new FArmyDetailBuilder);

	FArmyDetailNode& DoorPropertyCategory = DetailDoor->AddCategory("SecurityDoorPropertyCategory", TEXT("属性"));
	DoorPropertyCategory.AddChildNode("Name", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DoorPropertyCategory.AddChildNode("Style", TEXT("商品"), CreateDoorGoods()).ValueContentOverride().Padding(FMargin(0, 0, 8, 0));
	DoorPropertyCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget(true)).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DoorPropertyCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget(true)).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailDoor->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_Door, DetailDoor);
	SWidgetMap.Add(EPropertyDetailType::PDT_Door, DetailDoor->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_Door)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailDoor->GetTreeView());
}

void FArmyHardModeDetail::CreatePassDetail()
{
	DetailPass = MakeShareable(new FArmyDetailBuilder);

	FArmyDetailNode& DoorPropertyCategory = DetailPass->AddCategory("PassPropertyCategory", TEXT("属性"));
	DoorPropertyCategory.AddChildNode("Name", TEXT("名称"), CreateMaterialName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DoorPropertyCategory.AddChildNode("Style", TEXT("商品"), CreatePassGoods()).ValueContentOverride().Padding(FMargin(0, 0, 8, 0));
	DoorPropertyCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateScaleZWidget(true)).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	DoorPropertyCategory.AddChildNode("Width", TEXT("宽度(mm)"), CreateScaleYWidget(true)).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailPass->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_Pass, DetailPass);
	SWidgetMap.Add(EPropertyDetailType::PDT_Pass, DetailPass->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_Pass)->Get()->AsShared()
		];
	ConstructionDetail->RefreshItemExpand(DetailPass->GetTreeView());
}



void FArmyHardModeDetail::CreateRoughWallDetail()
{
	DetailRoughWall = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& RoughWallPropertyCategory = DetailRoughWall->AddCategory("RoughWallPropertyCategory", TEXT("属性"));
	RoughWallPropertyCategory.AddChildNode("RoughName", TEXT("名称"), CreateRoughName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	RoughWallPropertyCategory.AddChildNode("Length", TEXT("长度(mm)"), CreateRoughLengthWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	RoughWallPropertyCategory.AddChildNode("Height", TEXT("高度(mm)"), CreateRoughHeightWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	RoughWallPropertyCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CreateEditArea(RoughWallPropertyCategory);

	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailRoughWall->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	CreateApplyToAllWall(DetailRoughWall);
	DetailBuilderMap.Add(EPropertyDetailType::PDT_RoughWall, DetailRoughWall);
	SWidgetMap.Add(EPropertyDetailType::PDT_RoughWall, DetailRoughWall->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_RoughWall)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::CreateRoughFloorDetail()
{
	DetailRoughFloor = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& RoughFloorPropertyCategory = DetailRoughFloor->AddCategory("RoughFloorPropertyCategory", TEXT("属性"));
	RoughFloorPropertyCategory.AddChildNode("RoughName", TEXT("名称"), CreateRoughName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	RoughFloorPropertyCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CreateRaiseDetailUI(RoughFloorPropertyCategory, EWallType::Floor);
	CreateEditArea(RoughFloorPropertyCategory);
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailRoughFloor->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_RoughFloor, DetailRoughFloor);
	SWidgetMap.Add(EPropertyDetailType::PDT_RoughFloor, DetailRoughFloor->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_RoughFloor)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::CreateRoughRoofDetail()
{
	DetailRoughRoof = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& RoughRoofPropertyCategory = DetailRoughRoof->AddCategory("RoughRoofPropertyCategory", TEXT("属性"));
	RoughRoofPropertyCategory.AddChildNode("RoughName", TEXT("名称"), CreateRoughName()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	RoughRoofPropertyCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	if(selectedObject.IsValid()
		&& selectedObject->GetType() != OT_HelpRectArea
		&& selectedObject->GetType() != OT_ClinderArea)
	{
		CreateRaiseDetailUI(RoughRoofPropertyCategory, EWallType::Roof);
	}
		CreateEditArea(RoughRoofPropertyCategory);

	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailRoughRoof->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_RoughRoof, DetailRoughRoof);
	SWidgetMap.Add(EPropertyDetailType::PDT_RoughRoof, DetailRoughRoof->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_RoughRoof)->Get()->AsShared()
		];
}

void FArmyHardModeDetail::CreatePlotAreaRoughWallDetail()
{
	DetailRoughWall = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& RoughWallPropertyCategory = DetailRoughWall->AddCategory("RoughWallPropertyCategory", TEXT("属性"));
	RoughWallPropertyCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
    RoughWallPropertyCategory.AddChildNode("Raise", TEXT("挤出(mm)"), CreateRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailRoughWall->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_PlotArea_RoughWall, DetailRoughWall);
	SWidgetMap.Add(EPropertyDetailType::PDT_PlotArea_RoughWall, DetailRoughWall->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_PlotArea_RoughWall)->Get()->AsShared()
		];
}
void FArmyHardModeDetail::CreatePlotAreaRoughFloorDetail()
{
	DetailRoughFloor = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& RoughFloorPropertyCategory = DetailRoughFloor->AddCategory("RoughFloorPropertyCategory", TEXT("属性"));
	RoughFloorPropertyCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	CreateRaiseDetailUI(RoughFloorPropertyCategory, EWallType::Floor);

	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailRoughFloor->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_PlotArea_RoughFloor, DetailRoughFloor);
	SWidgetMap.Add(EPropertyDetailType::PDT_PlotArea_RoughFloor, DetailRoughFloor->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_PlotArea_RoughFloor)->Get()->AsShared()
		];
}
void FArmyHardModeDetail::CreatePlotAreaRoughRoofDetail()
{
	DetailRoughRoof = MakeShareable(new FArmyDetailBuilder());

	FArmyDetailNode& RoughRoofPropertyCategory = DetailRoughRoof->AddCategory("RoughRoofPropertyCategory", TEXT("属性"));
	RoughRoofPropertyCategory.AddChildNode("Area", TEXT("面积(㎡)"), CreateAreaWidget()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
	RoughRoofPropertyCategory.AddChildNode("Raise", TEXT("下吊高度(mm)"), CreateRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));

	//CreateEditArea(RoughRoofPropertyCategory);
	if (ConstructionDetail->GetDetailNodes().Num() > 0)
	{
		FArmyDetailNode& ConstructionItemCategory = DetailRoughRoof->AddCategory("ConstructionItem", TEXT("施工项"));
		for (auto It : ConstructionDetail->GetDetailNodes())
		{
			ConstructionItemCategory.AddChildNode(It);
		}
	}
	DetailBuilderMap.Add(EPropertyDetailType::PDT_PlotArea_RoughRoof, DetailRoughRoof);
	SWidgetMap.Add(EPropertyDetailType::PDT_PlotArea_RoughRoof, DetailRoughRoof->BuildDetail());
	PropertyPanel->AddSlot()
		[
			SWidgetMap.Find(EPropertyDetailType::PDT_PlotArea_RoughRoof)->Get()->AsShared()
		];

}

void FArmyHardModeDetail::CreateRaiseDetailUI(FArmyDetailNode &DetailNode, EWallType WallType)
{
	FString RaiseNodeName;
	if (WallType == EWallType::Roof)
		RaiseNodeName = TEXT("下吊高度(mm)");
	else if (WallType == EWallType::Floor)
		RaiseNodeName = TEXT("抬高(mm)");
	if (!RaiseNodeName.IsEmpty())
		DetailNode.AddChildNode("Raise", RaiseNodeName, CreateRaise()).ValueContentOverride().HAlign(HAlign_Fill).Padding(FMargin(0, 0, 8, 0));
}

TSharedRef<SWidget>  FArmyHardModeDetail::CreateHardModePropertyPanel()
{
	return
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[
			SNew(SBox)
			.HeightOverride(32)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		.Padding(FMargin(16, 0, 0, 0))
		.VAlign(VAlign_Center)
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("物体详情")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
		]
		]

	+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			PropertyPanel.ToSharedRef()
		];
}

float FArmyHardModeDetail::GetGapWidth() const
{
	if (selectedObject.IsValid())
	{
		return selectedObject->GetStyle()->GetInternalDist() * 10;
	}
	return 0.f;
}

void FArmyHardModeDetail::OnGapWidthChanged(float InHeight, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter && selectedObject.IsValid())
	{
		SCOPE_TRANSACTION(TEXT("修改缝隙宽度"));
		//@王志超 2018.12.20 波打线属性
		if (selectedObject->GetType() == OT_BodaArea)
		{
			TSharedPtr<FArmyBodaArea> tempBoda = StaticCastSharedPtr<FArmyBodaArea>(selectedObject);
			tempBoda->UpdateBodaAreaBrickDist(InHeight / 10.0f);
		}
		else
			selectedObject->GetStyle()->SetInternalDist(InHeight / 10);
		selectedObject->Modify();
	}
}

int FArmyHardModeDetail::GetRotationAngle() const
{
	if (selectedObject.IsValid())
	{
		return selectedObject->GetStyle()->GetRotationAngle();
	}
	else if (SelectedUObject != nullptr)
	{
		return FMath::CeilToInt(CachedRotationZ);
	}
	return 0.f;
}

void FArmyHardModeDetail::OnRotationAngleChanged(int InHeight, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (SelectedUObject != nullptr && SelectedUObject->IsValidLowLevel())
		{
			CachedRotationZ = InHeight;

			AActor* SelectedActor = Cast<AActor>(SelectedUObject);
			AArmyExtrusionActor* extrusionActor = Cast<AArmyExtrusionActor>(SelectedUObject);
			if (SelectedActor&&extrusionActor == NULL)
			{
				FRotator Rotation = SelectedActor->GetActorRotation();
				if (Rotation.Yaw != InHeight)
				{
					Rotation.Yaw = InHeight;
					RotateActor(SelectedActor, Rotation);
				}
			}
		}
		else if (selectedObject.IsValid())
		{
			SCOPE_TRANSACTION(TEXT("修改旋转角度"));
			selectedObject->GetStyle()->SetRotationAngle(InHeight);
			selectedObject->Modify();
		}
	}
}

int FArmyHardModeDetail::GetRaise() const
{
	if (selectedObject.IsValid())
	{
		return selectedObject->GetExtrusionHeight() * 10.0f;
	}
	else if (SelectedUObject != nullptr)
	{
		return CachedLocationZ;
	}
	return 0.f;
}

void FArmyHardModeDetail::OnRaiseChanged(int InHeight, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (SelectedUObject != nullptr && SelectedUObject->IsValidLowLevel())
		{
			int32 NewLocationZ = InHeight;
			if (CachedLocationZ != InHeight)
			{
				CachedLocationZ = InHeight;

				AActor* SelectedActor = Cast<AActor>(SelectedUObject);
				AArmyExtrusionActor* extrusionActor = Cast<AArmyExtrusionActor>(SelectedUObject);

				if (SelectedActor&&extrusionActor == NULL)
				{
					FVector Location = SelectedActor->GetActorLocation();
					//mm转成cm
					Location.Z = InHeight / 10.f;
					TransformActor(SelectedActor, Location);
				}
			}
		}
		else if (selectedObject.IsValid())
		{
            selectedObject->SetExtrusionHeight(InHeight / 10.0f);

            if (selectedObject->GetStyle(M_InnearArea)->HasGoodID())
			{
				XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(selectedObject)));
			}
			else
			{
				bool HasHung = false;
				if (selectedObject->SurfaceType == 2 && InHeight != 0)
				{
					HasHung = true;
				}

				//请求施工项
				ConstructionPatameters Parameter;
				Parameter.SetOriginalSurface((EWallType)selectedObject->SurportPlaceArea, HasHung);
				XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), Parameter, FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(selectedObject)));
			}
		}
	}
}

int FArmyHardModeDetail::GetXDirOffset() const
{
	if (selectedObject.IsValid())
	{
		return selectedObject->GetStyle()->GetXDirOffset();
	}
	return 0.f;
}

void FArmyHardModeDetail::OnXDirOffsetChanged(int InHeight, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		SCOPE_TRANSACTION(TEXT("修改x偏移"));

		selectedObject->GetStyle()->SetXDirOffset(InHeight);
		selectedObject->Modify();
	}
}

int FArmyHardModeDetail::GetYDirOffset() const
{
	if (selectedObject.IsValid())
	{
		return selectedObject->GetStyle()->GetYDirOffset();
	}
	return 0.f;
}

void FArmyHardModeDetail::OnYDirOffsetChanged(int InHeight, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		SCOPE_TRANSACTION(TEXT("修改y偏移"));

		selectedObject->GetStyle()->SetYDirOffset(InHeight);
		selectedObject->Modify();
	}
}

FText FArmyHardModeDetail::GetWindowRaise() const
{
	float length = 0.f;
	if (SelectedUObject != nullptr)
	{
		return FText::FromString(FString::FromInt(CachedWindowRaise));
	}
	return FText::FromString(FString::FromInt(length));
}

ECheckBoxState FArmyHardModeDetail::GetReform() const
{
	if (SelectedUObject&&SelectedUObject->IsValidLowLevel())
	{
		AActor* SelfActor = Cast<AActor>(SelectedUObject);
		if (SelfActor&&SelfActor->IsValidLowLevel())
		{
			AXRFurnitureActor* ParentActor = Cast<AXRFurnitureActor>(SelfActor->GetOwner());
			if (ParentActor&& ParentActor->IsValidLowLevel())
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ParentActor->GetRelevanceObject().Pin());
				if (Furniture.IsValid() && Furniture->GetType() == OT_ComponentBase)
				{
					return  Furniture->bReform ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
				}
			}
		}

	}
	return ECheckBoxState::Unchecked;

}

void FArmyHardModeDetail::CheckReform(ECheckBoxState InState)
{
	if (SelectedUObject&&SelectedUObject->IsValidLowLevel())
	{
		AActor* SelfActor = Cast<AActor>(SelectedUObject);
		if (SelfActor&&SelfActor->IsValidLowLevel())
		{
			AXRFurnitureActor* ParentActor = Cast<AXRFurnitureActor>(SelfActor->GetOwner());
			if (ParentActor&& ParentActor->IsValidLowLevel())
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ParentActor->GetRelevanceObject().Pin());
				if (Furniture.IsValid() && Furniture->GetType() == OT_ComponentBase)
				{
					Furniture->bReform = (InState == ECheckBoxState::Checked);
				}
			}
		}

	}
}

FText FArmyHardModeDetail::GetValueText() const
{
	if (SelectedUObject&&SelectedUObject->IsValidLowLevel())
	{
		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		FVector Location = SelectedActor->GetActorLocation();
		int32 Value = Location.Z;
		FText Text = FText::FromString(FString::FormatAsNumber(Value * 10));
		return Text;
	}
	return FText();
}

void FArmyHardModeDetail::OnGroundHeightChanged(const FText& text)
{
	float Value = FCString::Atof(*text.ToString());
	TSharedPtr<FArmyFurniture> SelectFurniture = GetSelectFurniture();
	if (SelectFurniture.IsValid())
	{
		SelectFurniture->SetAltitude(Value / 10.f);
		SelectFurniture->UpdateData();
	}
	if (SelectedUObject)
	{
		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		FVector Location = SelectedActor->GetActorLocation();
		Location.Z = Value / 10.f;
		SelectedActor->SetActorLocation(Location);
	}
}

void FArmyHardModeDetail::OnGroundCommitted(const FText& InText, const ETextCommit::Type InTextAction)
{
	if (InTextAction == ETextCommit::OnEnter)
	{
		float Value = FCString::Atof(*InText.ToString());
		TSharedPtr<FArmyFurniture> SelectFurniture = GetSelectFurniture();
		if (SelectFurniture.IsValid() && SelectFurniture.IsValid())
		{
			SelectFurniture->SetAltitude(Value / 10.f);
			SelectFurniture->UpdateData();
		}
		if (SelectedUObject->IsValidLowLevel())
		{
			AActor* SelectedActor = Cast<AActor>(SelectedUObject);
			FVector Location = SelectedActor->GetActorLocation();
			Location.Z = Value / 10.f;
			SelectedActor->SetActorLocation(Location);
		}
	}
}

FReply FArmyHardModeDetail::OnAlignClicked(AlignmentType AlignType)
{
	if (!selectedObject.IsValid())
	{
		return  FReply::Handled();
	}
	selectedObject->GetStyle()->SetAlignType(AlignType);
	//图标
	SetSelecteAlignmentType(AlignType);
	return FReply::Handled();
}

void FArmyHardModeDetail::SetSelecteAlignmentType(AlignmentType AlignType)
{
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateScaleXWidget()
{
	return
		SAssignNew(LengthWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.SelectAllTextWhenFocused(true)
		.Text(this, &FArmyHardModeDetail::GetScaleX)
		.OnTextCommitted_Raw(this, &FArmyHardModeDetail::OnScaleXCommited)
		//.IsReadOnly(true)//设置只读
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateScaleYWidget(bool bReadOnly)
{
	return
		SAssignNew(WidthWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.SelectAllTextWhenFocused(true)
		.Text(this, &FArmyHardModeDetail::GetScaleY)
		.OnTextCommitted_Raw(this, &FArmyHardModeDetail::OnScaleYCommited)
		.IsReadOnly(bReadOnly)//设置只读
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateScaleZWidget(bool bReadOnly)
{
	return
		SAssignNew(HeightWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.SelectAllTextWhenFocused(true)
		.Text(this, &FArmyHardModeDetail::GetScaleZ)
		.OnTextCommitted_Raw(this, &FArmyHardModeDetail::OnScaleZCommited)
		.IsReadOnly(bReadOnly)//设置只读
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateRoughLengthWidget()
{
	return
		SAssignNew(LengthWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyHardModeDetail::GetRoughLength)
		.IsReadOnly(true)//设置只读
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateRoughHeightWidget()
{
	return
		SAssignNew(HeightWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.Text(this, &FArmyHardModeDetail::GetWallHeight)
		.IsReadOnly(true)//设置只读
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateAreaWidget()
{
	return
		SAssignNew(AreaWidget, SEditableTextBox)
		.Style(&FArmyStyle::Get().GetWidgetStyle<FEditableTextBoxStyle>("EditableTextBox"))
		.SelectAllTextWhenFocused(true)
		.Text(this, &FArmyHardModeDetail::GetArea)
		.IsReadOnly(true)//设置只读
		;
}

FReply FArmyHardModeDetail::ApplyToAllWall()
{
	if (!dynamic_cast<FArmyRoomSpaceArea*>(selectedObject.Get()))
	{
		return FReply::Unhandled();;
	}
	//找到所在的房间
	TSharedPtr<FArmyRoomSpaceArea>  SelectedWall = StaticCastSharedPtr<FArmyRoomSpaceArea>(selectedObject);
	TArray<TWeakPtr<FArmyObject>> AllSurface;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, AllSurface);
	TArray<TWeakPtr<FArmyObject>> WindowList;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, WindowList);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_CornerBayWindow, WindowList);

	struct WindowInfo
	{
		//窗户
		TSharedPtr<FArmyHardware> Window = nullptr;
		//房间
		TSharedPtr<FArmyRoom> Room = nullptr;
		//与窗户关联的面
		TArray<TSharedPtr<FArmyRoomSpaceArea>> Surfaces;
		//被窗台石压住的面
		TSharedPtr<FArmyRoomSpaceArea> CoveredSurface = nullptr;
	};
	TArray<WindowInfo> WindowInfoes;

	//所有房间
	TArray<TWeakPtr<FArmyObject>> AllInnerRoom;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, AllInnerRoom);

	for (const auto& Window : WindowList)
	{
		WindowInfo Info;
		Info.Window = StaticCastSharedPtr<FArmyHardware>(Window.Pin());

		//获取与窗户关联的房间
		{
			bool HaveRoom1 = false;
			bool HaveRoom2 = false;

			auto Normal = Info.Window->GetDirection().GetSafeNormal();
			auto& Pos = Info.Window->GetPos();
			FVector P1, P2;
			P1 = (FArmySceneData::OutWallThickness / 2 + 5)*Normal + Pos;
			P2 = -(FArmySceneData::OutWallThickness / 2 + 5)*Normal + Pos;

			for (const auto& RoomObj : AllInnerRoom)
			{
				auto Room = StaticCastSharedPtr<FArmyRoom>(RoomObj.Pin());

				if (!HaveRoom1)
				{
					HaveRoom1 = Room->IsPointInRoom(P1) || Room->IsPointInRoom(P2);
					Info.Room = Room;
				}
				else if (Room->IsPointInRoom(P1) || Room->IsPointInRoom(P2))
				{
					//关联多个房间的窗户不予考虑
					Info.Room = nullptr;
				}
			}
		}

		//获取是否有窗台石
		bool HaveStone = false;
		switch (Info.Window->GetType())
		{
		case OT_Window:
		case OT_FloorWindow:
		{
			auto SubTypeWindow = StaticCastSharedPtr<FArmyWindow>(Info.Window);
			HaveStone = SubTypeWindow->GetIfGenerateWindowStone();
		}
		break;
		case OT_RectBayWindow:
		case OT_TrapeBayWindow:
		{
			auto SubTypeWindow = StaticCastSharedPtr<FArmyRectBayWindow>(Info.Window);
			HaveStone = SubTypeWindow->GetIfGenerateWindowStone();
		}
		break;
		default:
			break;
		}

		for (const auto& Surface : AllSurface)
		{
			auto CurrentSurface = StaticCastSharedPtr<FArmyRoomSpaceArea>(Surface.Pin());
			if (CurrentSurface->AttachRoomID == Window.Pin()->GetUniqueID().ToString())
			{
				Info.Surfaces.Add(CurrentSurface);

				if (HaveStone
					&& (!Info.CoveredSurface.IsValid())
					&& FMath::IsNearlyEqual(CurrentSurface->PlaneToTranlate(CurrentSurface->GetPlaneCenter()).Z, Info.Window->GetHeightToFloor(), 1)
					)
				{
					Info.CoveredSurface = CurrentSurface;
				};
			}
		}
		WindowInfoes.Add(Info);
	}
	/**寻找Obj所在房间*/
	FArmyRoom* SelectedRoom = nullptr;
	//房间所有的area;
	TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomAreas;

	struct CopyInfo
	{
		TSharedPtr<FArmyRoomSpaceArea> RoomArea;
		bool CopyMat;
		bool CopyConstructData;
	};
	TArray<CopyInfo> CopyInfoes;
	for (const auto& Room : AllInnerRoom)
	{
		FArmyRoom* currentRoom = Room.Pin()->AsassignObj<FArmyRoom>();
		CopyInfoes.Empty();

		TArray<TSharedPtr<FArmyRoomSpaceArea>> RoomAreas;
		//获得内墙面
		RoomAreas.Append(FArmySceneData::Get()->GetRoomAttachedSurfacesWidthRoomID(currentRoom->GetUniqueID().ToString()));
		//获得矮墙面
		RoomAreas.Append(FArmySceneData::Get()->GetRoomAttachedComponentSurfaceWithRoomId(currentRoom->GetUniqueID().ToString()));

		for (const auto& RoomArea : RoomAreas)
		{
			CopyInfo Info;
			Info.RoomArea = RoomArea;
			Info.CopyMat = true;
			Info.CopyConstructData = true;
			CopyInfoes.Add(Info);
		}

		//窗户的墙面
		for (const auto& WindowInfo : WindowInfoes)
		{
			if (!(WindowInfo.Room.IsValid() && WindowInfo.Room->GetUniqueID() == currentRoom->GetUniqueID()))
			{
				continue;
			}

			for (const auto & Surface : WindowInfo.Surfaces)
			{
				CopyInfo Info;
				Info.RoomArea = Surface;
				Info.CopyMat = !(WindowInfo.CoveredSurface.IsValid() && Surface->GetUniqueID() == WindowInfo.CoveredSurface->GetUniqueID());
				Info.CopyConstructData = true;
				CopyInfoes.Add(Info);
			}
		}

		if (CopyInfoes.FindByPredicate([&](const CopyInfo& CopyInfo)->bool {
			return CopyInfo.RoomArea->GetUniqueID() == SelectedWall->GetUniqueID(); })
			)
		{
			SelectedRoom = currentRoom;
			break;
		}
	}

	if (!SelectedRoom)
	{
		return FReply::Unhandled();
	}

	//复制MatStyle和施工项
	auto CopyMaterialAndConstructionData = [&](TSharedPtr<FArmyRoomSpaceArea> Source, TSharedPtr<FArmyRoomSpaceArea> Destination, bool CopyMat, bool CopyConstructionData) {

		if (Source == Destination)
		{
			return;
		}

		bool NeedCopyMaterial = CopyMat
			&& (!IsOrignalsurface)
			&& Source->GetMatStyle().IsValid()
			&& Source->GetMatStyle()->HasGoodID();


		if (NeedCopyMaterial)
		{
			FArmyReplaceTextureOperation::CopyEditAreaSytle(Source, Destination);
		}

		if (CopyConstructionData)
		{
			if (NeedCopyMaterial)
			{
				//复制商品施工项
				Destination->GetStyle()->ConstructionItemData->SetConstructionItemCheckedId(Source->GetStyle()->ConstructionItemData->CheckedId);
				XRConstructionManager::Get()->SaveConstructionData(Destination->GetUniqueID(), selectedObject->GetMatConstructionParameter(), XRConstructionManager::Get()->GetSavedCheckedData(Source->GetUniqueID(), selectedObject->GetMatConstructionParameter()));
			}
			else
			{
				//复制原始墙面施工项选项
				EWallType wallType = (EWallType)Source->SurfaceType;
				ConstructionPatameters Parameter;
				Parameter.SetOriginalSurface(wallType, Source->GetExtrusionHeight() != 0,Source->GetRoomSpaceID());

				XRConstructionManager::Get()->SaveConstructionData(
					Destination->GetUniqueID()
					, Parameter
					, XRConstructionManager::Get()->GetSavedCheckedData(Source->GetUniqueID(), Parameter)
				);
			}

		}
	};

	//保存施工项
	ExeBeforeClearDetail();
	for (const auto& CopyInfo : CopyInfoes)
	{
		if (SelectedWall->SurportPlaceArea == CopyInfo.RoomArea->SurportPlaceArea)
		{
			//复制
			CopyMaterialAndConstructionData(SelectedWall, CopyInfo.RoomArea, CopyInfo.CopyMat, CopyInfo.CopyConstructData);
		}
	}

	return FReply::Handled();
}

void FArmyHardModeDetail::TransformActor(AActor* Actor, const FVector& NewLocation)
{
	FTransform NewTransform = Actor->GetActorTransform();
	NewTransform.SetLocation(NewLocation);
	Actor->SetActorTransform(NewTransform);
	GXREditor->UpdatePivotLocationForSelection();
}

void FArmyHardModeDetail::RotateActor(AActor* Actor, const FRotator& NewRotation)
{
	FTransform NewTransform = Actor->GetActorTransform();
	NewTransform.SetRotation(FQuat(NewRotation));
	Actor->SetActorTransform(NewTransform);
	GXREditor->UpdatePivotLocationForSelection();
}

void FArmyHardModeDetail::ScaleActor(AActor* Actor, const FVector& NewScale)
{
	FTransform NewTransform = Actor->GetActorTransform();
	NewTransform.SetScale3D(NewScale);
	Actor->SetActorTransform(NewTransform);
	GXREditor->UpdatePivotLocationForSelection();
}

FText FArmyHardModeDetail::GetName() const
{
	if (selectedObject.IsValid() || SelectedUObject != nullptr)
	{
		return  FText::FromString(CachedName);
	}
	return FText::FromString("");
}

FText FArmyHardModeDetail::GetRoughName() const
{
	FString Name = " ";
	if (selectedObject.IsValid())
	{
		if (selectedObject->SurportPlaceArea == 0)
			Name = TEXT("地面");
		else if (selectedObject->SurportPlaceArea == 1)
			Name = TEXT("墙面");
		else
			Name = TEXT("顶面");
	}
	return  FText::FromString(Name);
}

void FArmyHardModeDetail::OnScaleXCommited(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (SelectedUObject != nullptr && SelectedUObject->IsValidLowLevel())
		{
			float NewScaleX = FCString::Atof(*InText.ToString());
			CachedScaleX = NewScaleX;
			//mm转成cm
			NewScaleX /= 10.f;
			AActor* SelectedActor = Cast<AActor>(SelectedUObject);

			if (SelectedActor)
			{
				float Scale = 1.f;
				float TempCheck = CalcActorDefaultSize(SelectedActor).X;
				if (TempCheck == 0) {
					return;
				}
				Scale = NewScaleX / TempCheck;
				FVector ActorScale = SelectedActor->GetActorScale3D();
				if (ActorScale.X < 0.f)
					Scale = -Scale;

				if (ActorScale.X != Scale)
				{
					ActorScale.X = Scale;
					ScaleActor(SelectedActor, ActorScale);
				}
			}
		}

	}
}

void FArmyHardModeDetail::OnScaleYCommited(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (SelectedUObject != nullptr && SelectedUObject->IsValidLowLevel())
		{
			float NewScaleY = FCString::Atof(*InText.ToString());
			CachedScaleY = NewScaleY;
			//mm转成cm
			NewScaleY /= 10.f;
			AActor* SelectedActor = Cast<AActor>(SelectedUObject);

			if (SelectedActor)
			{
				float Scale = 1.f;
				float TempCheck = CalcActorDefaultSize(SelectedActor).Y;
				if (TempCheck == 0) {
					return;
				}
				Scale = NewScaleY / TempCheck;
				FVector ActorScale = SelectedActor->GetActorScale3D();
				if (ActorScale.Y < 0.f)
					Scale = -Scale;

				if (ActorScale.Y != Scale)
				{
					ActorScale.Y = Scale;
					ScaleActor(SelectedActor, ActorScale);
				}
			}
		}
	}
}

void FArmyHardModeDetail::OnScaleZCommited(const  FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (SelectedUObject != nullptr && SelectedUObject->IsValidLowLevel())
		{
			float NewScaleZ = FCString::Atof(*InText.ToString());
			CachedScaleZ = NewScaleZ;
			//mm转成cm
			NewScaleZ /= 10.f;
			AActor* SelectedActor = Cast<AActor>(SelectedUObject);

			if (SelectedActor)
			{
				float Scale = 1.f;
				float TempCheck = CalcActorDefaultSize(SelectedActor).Z;
				if (TempCheck == 0) {
					return;
				}
				Scale = NewScaleZ / TempCheck;
				FVector ActorScale = SelectedActor->GetActorScale3D();
				if (ActorScale.Z < 0.f)
					Scale = -Scale;

				if (ActorScale.Z != Scale)
				{
					ActorScale.Z = Scale;
					ScaleActor(SelectedActor, ActorScale);
				}
			}
		}
	}
}

void FArmyHardModeDetail::OnWindowRaiseCommited(const FText & InText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (SelectedUObject != nullptr && SelectedUObject->IsValidLowLevel())
		{
			float NewScaleY = FCString::Atof(*InText.ToString());
			CachedWindowRaise = NewScaleY;
			//mm转成cm
			NewScaleY /= 10.f;
			AActor* SelectedActor = Cast<AActor>(SelectedUObject);

			if (SelectedActor)
			{
				float Scale = 1.f;
				float TempCheck = CalcActorDefaultSize(SelectedActor).Y;
				if (TempCheck == 0) {
					return;
				}
				Scale = NewScaleY / TempCheck;
				FVector ActorScale = SelectedActor->GetActorScale3D();
				if (ActorScale.Y < 0.f)
					Scale = -Scale;

				if (ActorScale.Y != Scale)
				{
					ActorScale.Y = Scale;
					ScaleActor(SelectedActor, ActorScale);
				}
			}
		}
	}
}

void FArmyHardModeDetail::OnLight3DCastShadowStateChanged(ECheckBoxState InNewState)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedLight3DCastShadow = InNewState;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetCastShadows(InNewState == ECheckBoxState::Checked);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetCastShadows(InNewState == ECheckBoxState::Checked);

	}
}

void FArmyHardModeDetail::OnLight3DIntensityChanged(int Intensity, ETextCommit::Type CommitType)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedLight3DIntensity = Intensity;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetIntensity(Intensity);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetIntensity(Intensity);

	}
}

void FArmyHardModeDetail::OnLight3DRadiusChanged(int InValue, ETextCommit::Type CommitType)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedLight3DRadius = InValue;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetAttenuationRadius(InValue);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetAttenuationRadius(InValue);

	}
}

void FArmyHardModeDetail::OnLight3DSourceRadiusChanged(int InValue, ETextCommit::Type CommitType)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedLight3DSourceRadius = InValue;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetSourceRadius(InValue);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetSourceRadius(InValue);

	}
}

void FArmyHardModeDetail::OnLight3DSourceLengthChanged(int InValue, ETextCommit::Type CommitType)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedLight3DSourceLength = InValue;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetSourceLength(InValue);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetSourceLength(InValue);

	}
}

void FArmyHardModeDetail::OnSpotLight3DInnerAngleChanged(int InValue, ETextCommit::Type CommitType)
{
	CachedSpotLight3DInnerAngle = InValue;
	AXRSpotLightActor* LightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	if (LightActor)
	{
		LightActor->SpotLightCOM->SetInnerConeAngle(InValue);
	}
}

void FArmyHardModeDetail::OnSpotLight3DOuterAngleChanged(int InValue, ETextCommit::Type CommitType)
{
	CachedSpotLight3DOuterAngle = InValue;
	AXRSpotLightActor* LightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	if (LightActor)
	{
		LightActor->SpotLightCOM->SetOuterConeAngle(InValue);
	}
}

FReply FArmyHardModeDetail::OnColorClicked()
{
	FColorPickerArgs PickerArgs;
	PickerArgs.InitialColorOverride = BorderColor;
	PickerArgs.InitialColorOverride.A = 1.f;
	PickerArgs.bUseAlpha = false;
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FArmyHardModeDetail::OnLight3DColorChanged);
	OpenColorPicker(PickerArgs);

	return FReply::Handled();
}

void FArmyHardModeDetail::OnLight3DColorChanged(const FLinearColor InColor)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	BorderColor = InColor;
	CachedLight3DColor = InColor;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetLightColor(InColor);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetLightColor(InColor);
	}
	else if (selectedObject.IsValid())
	{
		selectedObject->SetGapColor(InColor);
	}
}

void FArmyHardModeDetail::OnLight3DEnableChanged(ECheckBoxState InNewState)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedLight3DEnable = InNewState;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->SetVisibility(InNewState == ECheckBoxState::Checked);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->SetVisibility(InNewState == ECheckBoxState::Checked);

	}
}

void FArmyHardModeDetail::OnIndirectLightingIntensityChanged(float InValue, ETextCommit::Type CommitType)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CachedIndirectLightingIntensity = InValue;
	if (PointLightActor)
	{
		PointLightActor->PointLightCOM->IndirectLightingIntensity = InValue;
	}
	else if (SpotLightActor)
	{
		SpotLightActor->SpotLightCOM->IndirectLightingIntensity = InValue;

	}
}

void FArmyHardModeDetail::OnLightMobilityTypeChanged(const int32 Key, const FString& Value)
{
	AXRPointLightActor* PointLightActor = Cast<AXRPointLightActor>(SelectedUObject);
	AXRSpotLightActor* SpotLightActor = Cast<AXRSpotLightActor>(SelectedUObject);
	CacheLightMobility = Value;
	if (PointLightActor)
	{
		PointLightActor->GetRootComponent()->SetMobilityAllChildren(CacheLightMobility == TEXT("静态光") ? EComponentMobility::Static : EComponentMobility::Movable);
	}
	else if (SpotLightActor)
	{
		SpotLightActor->GetRootComponent()->SetMobilityAllChildren(CacheLightMobility == TEXT("静态光") ? EComponentMobility::Static : EComponentMobility::Movable);
	}
}

void FArmyHardModeDetail::OnReflectionSphereRadiusChanged(int InValue, ETextCommit::Type CommitType)
{
	CachedReflectionSphereRadius = InValue;
	AXRReflectionCaptureActor* TheActor = Cast<AXRReflectionCaptureActor>(SelectedUObject);
	if (TheActor)
	{
		TheActor->SetRadius(InValue);
	}
}

void FArmyHardModeDetail::OnReflectionSphereBrightnessChanged(float InValue, ETextCommit::Type CommitType)
{
	CachedReflectionSphereBrightness = InValue;
	AXRReflectionCaptureActor* TheActor = Cast<AXRReflectionCaptureActor>(SelectedUObject);
	if (TheActor)
	{
		TheActor->SetBrightness(InValue);
	}
}

void FArmyHardModeDetail::OnCheckedChange(ECheckBoxState NewCheckedState)
{
	AXRWindowActor* windowActor = Cast<AXRWindowActor>(SelectedUObject);
	if (!windowActor)
		return;
	TSharedPtr<FArmyDetailNode> node = nullptr;
	if (windowActor->CurrentWindowType == FLOOR_WINDOW || windowActor->CurrentWindowType == NORMAL_WINDOW)
	{
		node = DetailWindowNormal->EditCategory(FName(TEXT("WindowNormalModelPropertyCategory")));
	}
	else
	{
		node = DetailWindowRectBay->EditCategory(FName(TEXT("WindowRectBayModelPropertyCategory")));
	}
	if (NewCheckedState == ECheckBoxState::Checked)
	{
		TSharedPtr<FArmyDetailNode>WinNode = node->FindChildNode(FName(TEXT("WindowColor")));
		if (WinNode.IsValid())
		{
			WinNode->SetNodeVisibility(EVisibility::Visible);
		}
		cbxWindowStoneVisibility = ECheckBoxState::Checked;
	}
	else
	{
		TSharedPtr<FArmyDetailNode>WinNode = node->FindChildNode(FName(TEXT("WindowColor")));
		if (WinNode.IsValid())
		{
			WinNode->SetNodeVisibility(EVisibility::Collapsed);
		}
		cbxWindowStoneVisibility = ECheckBoxState::Unchecked;
	}
}

ECheckBoxState FArmyHardModeDetail::GetWindowStoneWidEnable()const
{
	return cbxWindowStoneVisibility;
}

FReply FArmyHardModeDetail::OnEditAreaClicked()
{
	XRArgument Param(0);
	EditAreaDelegate.Broadcast(Param, selectedObject, false);
	return FReply::Handled();
}

TSharedPtr<FArmyBaseEditStyle> FArmyHardModeDetail::GetBaseEditStyle(const int32 key)
{
	switch (key)
	{
	case 1:
	case 6:
		return MakeShareable(new FArmyContinueStyle());
		// @zengy 2是工字铺
	case 2:
		return MakeShareable(new FArmyWorkerStyle());
		// @zengy 7变成了斜铺
	case 7:
		return MakeShareable(new FArmySlopeContinueStyle());
	case 3:
		return MakeShareable(new FArmyWhirlwindStyle());
	case 4:
		return MakeShareable(new FArmyHerringBoneStyle());
	case 5:
		return MakeShareable(new FArmyTrapezoidStyle());
	case 12://壁纸  无铺法
		return MakeShareable(new FArmySeamlessStyle());
	default:
		break;
	}
	return nullptr;
}

void FArmyHardModeDetail::OnSlectedPavingMethodChanged(const int32 Key, const FString& Value)
{
	//@ 梁晓菲 选择“无其他铺法”时不做反应
	if (selectedObject.IsValid() && Key != 9999)
	{
		SelectPavingName = FText::FromString(Value);
		TSharedPtr<FArmyBaseEditStyle> editStyle = selectedObject->GetStyle();
		EStyleType type = editStyle->GetEditType();
		TSharedPtr<FArmyBaseEditStyle> MatStyleNew = GetBaseEditStyle(Key);
		if (!MatStyleNew.IsValid())
			return;
		if (type == MatStyleNew->GetEditType())
			return;

		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		UMaterialInterface * mat = ResMgr->CreateContentItemMaterial(editStyle->GetCurrentItem());
		UMaterialInstanceDynamic* MID = dynamic_cast<UMaterialInstanceDynamic*>(mat);

		MatStyleNew->SetMainMaterial(MID, editStyle->GetMainTexHeight(), editStyle->GetMainTexWidth());
		MatStyleNew->SetMainTextureLocalPath(editStyle->GetMainTextureUrl());

		MatStyleNew->SetCategoryID(editStyle->GetCategoryID());
		MatStyleNew->SetGoodsID(editStyle->GetGoodsID());
		MatStyleNew->SetPlaneInfo(selectedObject->GetPlaneCenter(), selectedObject->GetXDir(), selectedObject->GetYDir());
		MatStyleNew->SetCurrentItem(editStyle->GetCurrentItem());


		selectedObject->SetStyle(MatStyleNew);
		selectedObject->Modify();

		XRConstructionManager::Get()->TryToFindConstructionData(selectedObject->GetUniqueID(), selectedObject->GetMatConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHardModeDetail::ConstructionCallBack, CurrentDetailType, StaticCastSharedPtr<FArmyObject>(selectedObject)));
	}
}

bool FArmyHardModeDetail::ShowPavingMethod(TSharedPtr<FArmyBaseArea> SelectArea)
{
	if (!SelectArea.IsValid())
		return false;
	TSharedPtr<FArmyBaseEditStyle> style = SelectArea->GetStyle();
	TSharedPtr<FContentItemSpace::FContentItem> GoodItem = style->GetCurrentItem();
	EStyleType Styletype = style->GetEditType();
	if (!GoodItem.IsValid())
		return false;
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = GoodItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return false;
	TSharedPtr<FArmyHardModeRes> tempRes = StaticCastSharedPtr<FArmyHardModeRes>(resArr[0]);
	if (!tempRes.IsValid())
		return false;
	TArray<TSharedPtr<FArmyKeyValue> > pavingArr;
	CategryApplicationType pavingType = CAT_None;

	pavingArr = tempRes->PavingMethodArr;
	pavingType = CategryApplicationType(tempRes->DefaultPavingMethod);

	Array_Paving->Array.Empty();
	TArray<TSharedPtr<FArmyKeyValue> > * arr = &(Array_Paving->Array);
	if (pavingArr.Num() > 0)
	{
		Array_Paving->Array.Reset();
		for (auto& it : pavingArr)
		{

			(*arr).AddUnique(it);
		}
	}

	if (Array_Paving->Array.Num() > 0 && pavingType != CAT_WallPaper)
	{
		FString tempName = ConversionStringByStyle(Styletype);
		SelectPavingName = FText::FromString(tempName/*Array_Paving->Array[0]->Value*/);
		TSharedPtr<FArmyKeyValue> item = Array_Paving->FindByValue(tempName);
		if (item.IsValid())
		{
			PavingMethodModify->SetSelectedItem(item);
		}
	}
	else
	{
		Array_Paving->Array.Reset();
		Array_Paving->Array.AddUnique(MakeShareable(new FArmyKeyValue(9999, TEXT("无关联铺法"))));
		SelectPavingName = FText::FromString(Array_Paving->Array[0]->Value);
	}

	PavingMethodModify->RefreshOptions();
	return true;
}

void FArmyHardModeDetail::SetDetailNodeProperty(TSharedPtr<FArmyDetailNode> detailNode, bool isReadOnly, bool isEnabled)
{
	if (detailNode.Get() != nullptr && detailNode.IsValid())
	{
		FArmyDetailLayout valueLayout = detailNode->ValueContentOverride();
		TSharedPtr<SWidget> wid = valueLayout.GetWidget();
		TSharedPtr<SEditableTextBox> TextBox = StaticCastSharedPtr<SEditableTextBox>(wid);
		if (TextBox.IsValid())
		{
			TextBox->SetIsReadOnly(isReadOnly);
			TextBox->SetEnabled(isEnabled);
		}
	}
}

void FArmyHardModeDetail::InitGroundHeightArray()
{
	GroundHeightArray.Reset();
	if (SelectedUObject&&SelectedUObject->IsValidLowLevel())
	{
		AActor* SelfActor = Cast<AActor>(SelectedUObject);
		if (SelfActor&&SelfActor->IsValidLowLevel())
		{
			AXRFurnitureActor* ParentActor = Cast<AXRFurnitureActor>(SelfActor->GetOwner());
			if (ParentActor&& ParentActor->IsValidLowLevel())
			{
				TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(ParentActor->GetRelevanceObject().Pin());
				if (Furniture.IsValid())
				{
					TArray<float> Data = Furniture->Altitudes;
					for (int32 i = 0; i < Data.Num(); i++)
						GroundHeightArray.Add(MakeShareable(new float(Data[i])));
				}
			}
		}
	}
}

FString FArmyHardModeDetail::ConversionStringByStyle(EStyleType _Style)
{
	switch (_Style)
	{
	case S_ContinueStyle:
	{
		return TEXT("连续直铺");
	}
	case S_HerringBoneStyle:
	{
		return TEXT("人字铺");
	}
	case S_TrapeZoidStyle:
	{
		return TEXT("三六九铺");
	}
	case S_WhirlwindStyle:
	{
		return TEXT("旋风铺");
	}
	case S_WorkerStyle:
	{
		return TEXT("工字铺");
	}
	case S_SeamlessStyle:
	{
		return TEXT("无缝铺");
	}
	case S_SlopeContinueStyle:
	{
		return TEXT("斜铺");
	}
	default:
		return FString();
	}
}

FReply FArmyHardModeDetail::OnWallPaintColorClicked()
{
	FColorPickerArgs PickerArgs;
	PickerArgs.InitialColorOverride = WallPaintBorderColor;
	PickerArgs.InitialColorOverride.A = 1.f;
	PickerArgs.bUseAlpha = false;
	PickerArgs.OnColorCommitted = FOnLinearColorValueChanged::CreateSP(this, &FArmyHardModeDetail::OnWallPaintColorChanged);
	OpenColorPicker(PickerArgs);
	return FReply::Handled();
}

void FArmyHardModeDetail::OnWallPaintColorChanged(const FLinearColor InColor)
{
	TSharedPtr<FArmyBaseEditStyle> editStyle = selectedObject->GetStyle();
	editStyle->SetWallPaintColor(InColor);
	WallPaintBorderColor = InColor;
}



void FArmyHardModeDetail::ExeBeforeClearDetail()
{
	CurrentJsonData = nullptr;
	ConstructionDetail->RefreshCheckData(nullptr);
}

void FArmyHardModeDetail::ExeWhileShowDetail(int32 ConstructionType)
{
	AActor* SelectedActor = Cast<AActor>(SelectedUObject);
	if (selectedObject.IsValid() && ConstructionType != DT_Boda)
	{
		TSharedPtr<FArmyBaseArea> tempArea = StaticCastSharedPtr<FArmyBaseArea>(selectedObject);
		if (tempArea.IsValid())
		{

			if (ConstructionType == DT_RoughFloor
				|| ConstructionType == DT_RoughWall
				|| ConstructionType == DT_RoughRoof)

			{
				ConstructionPatameters Parameter;
				Parameter.SetOriginalSurface((EWallType)selectedObject->SurportPlaceArea, selectedObject->GetExtrusionHeight() != 0, selectedObject->GetRoomSpaceID());
				CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(tempArea->GetUniqueID(), Parameter);
				ConstructionDetail->RefreshCheckData(CurrentCheckData);
			}
			else if (ConstructionType == DT_Paste)
			{
				if (tempArea->GetStyle(M_InnearArea).IsValid())
				{

					CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(tempArea->GetUniqueID(), selectedObject->GetMatConstructionParameter());
					ConstructionDetail->RefreshCheckData(CurrentCheckData);
				}
			}
		}
	}
	else if (selectedObject.IsValid() && ConstructionType == DT_Boda)
	{
		TSharedPtr<FArmyBodaArea> tempArea = StaticCastSharedPtr<FArmyBodaArea>(selectedObject);
		if (tempArea->GetBodaMatStyle().IsValid())
		{
			CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(tempArea->GetUniqueID(), tempArea->GetMatConstructionParameter());
			ConstructionDetail->RefreshCheckData(CurrentCheckData);

		}
	}
	else if (SelectedUObject && SelectedUObject->IsValidLowLevel() && ConstructionType == DT_Actor)
	{
		AActor* SelectedActor = Cast<AActor>(SelectedUObject);
		auto XRActor = Cast<AXRActor>(SelectedActor->GetOwner());
		if (XRActor)
		{
			auto FurnitureObj = StaticCastSharedPtr<FArmyFurniture>(XRActor->GetRelevanceObject().Pin());

			CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(FurnitureObj->GetUniqueID(), FurnitureObj->GetConstructionParameter());

			ConstructionDetail->RefreshCheckData(FArmySceneData::Get()->GetConstructionItemDataByActorUniqueID(SelectedActor->GetUniqueID()));
		}
	}
	else if (SelectedUObject && SelectedUObject->IsValidLowLevel() && ConstructionType == DT_NormalWindowStone)
	{
		//窗台石
		AXRWindowActor* WindowActor = Cast<AXRWindowActor>(SelectedUObject);
		auto WindowObj = StaticCastSharedPtr<FArmyWindow>(WindowActor->AttachSurface);

		if (WindowObj->GetContentItem().IsValid())
		{
			ConstructionPatameters Parameter;
			Parameter.SetNormalGoodsInfo(WindowObj->GetContentItem()->ID, WindowObj->GetRoomSpaceID());
			CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(WindowObj->GetUniqueID(), Parameter);
			ConstructionDetail->RefreshCheckData(CurrentCheckData);
		}
		else
		{
			ConstructionDetail->RefreshCheckData(nullptr);
		}

	}
	else if (SelectedUObject && SelectedUObject->IsValidLowLevel() && ConstructionType == DT_RectWindowStone)
	{
		//窗台石
		AXRWindowActor* WindowActor = Cast<AXRWindowActor>(SelectedUObject);
		auto WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(WindowActor->AttachSurface);

		if (WindowObj->GetContentItem().IsValid())
		{
			ConstructionPatameters Parameter;
			Parameter.SetNormalGoodsInfo(WindowObj->GetContentItem()->ID, WindowObj->GetRoomSpaceID());
			CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(WindowObj->GetUniqueID(), Parameter);
			ConstructionDetail->RefreshCheckData(CurrentCheckData);
		}
		else
		{
			ConstructionDetail->RefreshCheckData(nullptr);
		}

	}
	else if (SelectedActor && SelectedActor->IsValidLowLevel() &&
		(SelectedActor->ActorHasTag(XRActorTag::Door) ||
			SelectedActor->ActorHasTag(XRActorTag::SecurityDoor)))
	{
		//门
		CachedName = SelectedActor->GetActorLabel();
		FObjectWeakPtr DoorObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
		if (DoorObj.IsValid())
		{
			TSharedPtr< ConstructionPatameters>  P;
			switch (DoorObj.Pin()->GetType())
			{
			case EObjectType::OT_Door:
			case EObjectType::OT_SecurityDoor:
			{
				auto Door = StaticCastSharedPtr<FArmySingleDoor>(DoorObj.Pin());
				if (Door->HasGoods())
				{
					P = Door->GetConstructionParameter();
				}
			}
			break;
			case EObjectType::OT_SlidingDoor:
			{
				auto Door = StaticCastSharedPtr<FArmySlidingDoor>(DoorObj.Pin());
				if (Door->HasGoods())
				{
					P = Door->GetConstructionParameter();
				}
			}
			break;
			}

			if (P.IsValid())
			{
				CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(DoorObj.Pin()->GetUniqueID(), *P);
				ConstructionDetail->RefreshCheckData(CurrentCheckData);
			}
			else
			{
				ConstructionDetail->RefreshCheckData(nullptr);
			}
		}

	}
	else if (SelectedActor && SelectedActor->IsValidLowLevel() && SelectedActor->ActorHasTag(XRActorTag::Pass))
	{
		CachedName = SelectedActor->GetActorLabel();
		FObjectWeakPtr PassObj = FArmySceneData::Get()->GetObjectByName(E_LayoutModel, CachedName);
		if (PassObj.IsValid())
		{
			TSharedPtr< ConstructionPatameters>  P;
			switch (PassObj.Pin()->GetType())
			{
			case EObjectType::OT_Pass:
			{

				auto Pass = StaticCastSharedPtr<FArmyPass>(PassObj.Pin());
				if (Pass->HasGoods())
				{
					P = Pass->GetConstructionParameter();
				}
			}
			break;
			case EObjectType::OT_NewPass:
			{

				auto Pass = StaticCastSharedPtr<FArmyNewPass>(PassObj.Pin());
				if (Pass->HasGoods())
				{
					P = Pass->GetConstructionParameter();
				}
			}
			break;
			}
			if (P.IsValid())
			{
				CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(PassObj.Pin()->GetUniqueID(), *P);
				ConstructionDetail->RefreshCheckData(CurrentCheckData);
			}
			else
			{
				ConstructionDetail->RefreshCheckData(nullptr);
			}
		}
	}
	else if (SelectedUObject && SelectedUObject->IsValidLowLevel() && ConstructionType == DT_ExtrusionLine)
	{
		//放样类
		AArmyExtrusionActor* ExtrusionActor = Cast<AArmyExtrusionActor>(SelectedUObject);
		auto ExtrusionArea = StaticCastSharedPtr<FArmyBaseArea>(ExtrusionActor->AttachSurfaceArea.Pin());

		if (ExtrusionActor && ExtrusionArea.IsValid())
		{
			CurrentCheckData = XRConstructionManager::Get()->GetSavedCheckedData(ExtrusionArea->GetUniqueID(), ExtrusionActor->GetConstructionParameter());
			ConstructionDetail->RefreshCheckData(CurrentCheckData);
		}
	}

	else if (SelectedUObject && SelectedUObject->IsValidLowLevel() && ConstructionType == DT_Extrusion)
	{
		AArmyExtrusionActor* SelectedActor = Cast<AArmyExtrusionActor>(SelectedUObject);
		ConstructionDetail->RefreshCheckData(FArmySceneData::Get()->GetConstructionItemDataByExtrusionUniqueCode(SelectedActor->UniqueCodeExtrusion));
	}

	ConstructionDetail->ConstructionStateChangeDelegate.Unbind();
	ConstructionDetail->ConstructionStateChangeDelegate.BindLambda([this]() {
		if (!CurrentCheckData.IsValid())
		{
			return;
		}
		CurrentCheckData->SetConstructionItemCheckedId(ConstructionDetail->GetCheckedData());
	});
	/*让施工项面板根据勾选自动展开*/
	auto Builder = DetailBuilderMap.Find((int32)CurrentDetailType);
	if (Builder && (*Builder).IsValid())
	{
		ConstructionDetail->RefreshItemExpand((*Builder)->GetTreeView());
	}
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateLocationZWidget()
{
	return
		SNew(SDropDownTextBox)
		.Text(this, &FArmyHardModeDetail::GetValueText)
		.OptionsSource(&GroundHeightArray)
		.OnTextChanged(this, &FArmyHardModeDetail::OnGroundHeightChanged)
		.OnTextCommitted(this, &FArmyHardModeDetail::OnGroundCommitted)
		;
}

TSharedPtr<SWidget> FArmyHardModeDetail::CreateRefromWidget()
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &FArmyHardModeDetail::CheckReform)
		.IsChecked_Raw(this, &FArmyHardModeDetail::GetReform)
		.CheckedImage(FArmyStyle::Get().GetBrush("Icon.Checkbox_active"))
		.CheckedPressedImage(FArmyStyle::Get().GetBrush("Icon.Checkbox_active"))
		.CheckedHoveredImage(FArmyStyle::Get().GetBrush("Icon.Checkbox_active"))
		.UncheckedImage(FArmyStyle::Get().GetBrush("Icon.Checkbox_normal"))
		.UncheckedPressedImage(FArmyStyle::Get().GetBrush("Icon.Checkbox_hover"))
		.UncheckedHoveredImage(FArmyStyle::Get().GetBrush("Icon.Checkbox_hover"))
		]
	+ SHorizontalBox::Slot()
		.Padding(FMargin(10, 0, 0, 0))
		.FillWidth(1.f)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("")))
		.TextStyle(FArmyStyle::Get(), "ArmyText_12")
		]
	;
}
