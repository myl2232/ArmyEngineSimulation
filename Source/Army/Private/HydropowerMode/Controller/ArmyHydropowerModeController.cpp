#include "ArmyHydropowerModeController.h"
#include "SArmyModelContentBrowser.h"
#include "ArmyViewportClient.h"
#include "ArmyDesignEditor.h"
#include "ArmyHydropowerModeCommands.h"
#include "SArmyHydropowerModeContentBrowser.h"
#include "ArmyLayerManager.h"
#include "SArmyObjectOutliner.h"
#include "ArmyHydropowerDetail.h"
#include "ArmyHydropowerSwitchConnect.h"
#include "ArmyHydropowerPowerSystem.h"
#include "ArmyAxisRuler3D.h"
#include "ArmyAxisRuler.h"
#include "ArmyCommonTypes.h"
#include "ArmyPipelineOperation.h"
#include "ArmyHydropowerComponentOperation.h"
#include "ArmySwitchConnectOperation.h"
#include "ArmyPowerSystemManager.h"
#include "ArmyObject.h"
#include "ArmyPipeline.h"
#include "ArmyPipePoint.h"
#include "ArmyFurniture.h"
#include "ArmyObjectVisualizer.h"
#include "ArmyPipelineVisualizer.h"
#include "ArmyPipePointVisualizer.h"
#include "ArmyPipelineActor.h"
#include "ArmyPipePointActor.h"
#include "ArmyFurnitureActor.h"
#include "Engine/Selection.h"
#include "ArmyEditorEngine.h"
#include "ArmyMouseCaptureManager.h"
#include "ArmyPlayerController.h"
#include "ArmyFrameCommands.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"
#include "Army3DManager.h"
#include "PhysicsEngine/BodySetup.h"
#include "EngineUtils.h"
#include "ArmyViewportClient.h"
#include "ArmyGameInstance.h"
#include "ArmyToolBarBuilder.h"
#include "ArmyResourceModule.h"
#include "ArmyCommonTools.h"
#include "ArmyActorItem.h"
#include "ArmyGroupActor.h"
#include "ArmyRoomSpaceArea.h"
#include "SArmyTextCheckBox.h"
#include "ArmyMath.h"
#include "ArmyHydropowerDataManager.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyAutoDesignPipeLine.h"
#include "ArmyActorVisitorFactory.h"
#include "Army/Public/Operation/XRHydropowerEditor/XRSwitchConnectOperation.h"
#include "ArmyAutoDesignPoint.h"
#include "ArmyRuler.h"
#include "ArmyHttpModule.h"
#include "SArmySceneOutliner.h"
#include "ArmyFurniture.h"
#include "ArmyAutoCad.h"
#include "SArmyCategoryOrganizationManage.h"
#include "SArmyCategoryOrganization.h"
#include "ArmyWorldManager.h"


using namespace FContentItemSpace;

#define  WallHeightTop  300

FArmyHydropowerModeController::~FArmyHydropowerModeController()
{
	//注销XRObject操作注册表
	TArray<EObjectType> Types = { OT_StrongElectricity_25,OT_StrongElectricity_4,
		OT_StrongElectricity_Single,OT_StrongElectricity_Double,
		OT_WeakElectricity_TV,OT_WeakElectricity_Net,
		OT_WeakElectricity_Phone,OT_ColdWaterTube,OT_HotWaterTube,OT_Drain,OT_ColdWaterTube_Linker,
		OT_HotWaterTube_Linker,OT_Drain_Linker };
	for (EObjectType ObjectType : Types)
	{
		GVC->UnregisterXRObjectVisualizer(ObjectType);
	}
	USelection::SelectNoneEvent.RemoveAll(this);
	USelection::SelectObjectEvent.RemoveAll(this);
	USelection::SelectionChangedEvent.RemoveAll(this);
	MaterialInsDy->RemoveFromRoot();
}

const EModelType FArmyHydropowerModeController::GetDesignModelType()
{
	return EModelType::E_HydropowerModel;
}

void FArmyHydropowerModeController::Init()
{
	CurrentOperationID = HO_NONE;
	// 注册命令代理
	FArmyHydropowerModeCommands::Register();

	FArmyDesignModeController::Init();
	USelection::SelectNoneEvent.AddRaw(this, &FArmyHydropowerModeController::OnSelectionNone);//没有选中actor代理
	USelection::SelectObjectEvent.AddRaw(this, &FArmyHydropowerModeController::OnSelectionChanged);//切换选中actor代理
	USelection::SelectionChangedEvent.AddRaw(this, &FArmyHydropowerModeController::OnSelectionChanged);//选中actor代理
	GXREditor->OnLevelActorRemoved.BindRaw(this, &FArmyHydropowerModeController::OnDeleteSelectionActor);//删除actor代理
	UMaterialInstance* MaterialInterface = FArmyEngineModule::Get().GetEngineResource()->GetLineMaterialIns();
	GVC->ProjectActorsIntoWorldDelegate.BindRaw(this, &FArmyHydropowerModeController::ProjectActorsIntoWorld);//拖入场景注册代理
	GVC->CreateNewDroppedEvent.BindRaw(this, &FArmyHydropowerModeController::AddToScene);//持续AddToScene代理
	GXREditor->OnActorMoved().AddRaw(this, &FArmyHydropowerModeController::OnActorMoved);// actor移动代理
	MaterialInsDy = UMaterialInstanceDynamic::Create(MaterialInterface, nullptr);
	if (MaterialInsDy && MaterialInsDy->IsValidLowLevel())
	{
		MaterialInsDy->AddToRoot();
	}

	//视图切换代理
	if (GXRPC)
	{
		GXRPC->OnViewChangedDelegate.AddRaw(this, &FArmyHydropowerModeController::OnViewModeChanged);
	}
	//注册xrobject观察者
	TArray<EObjectType> Types = { OT_StrongElectricity_25,OT_StrongElectricity_4,
		OT_StrongElectricity_Single,OT_StrongElectricity_Double,
		OT_WeakElectricity_TV,OT_WeakElectricity_Net,
		OT_WeakElectricity_Phone,OT_ColdWaterTube,OT_HotWaterTube,OT_Drain };
	for (int32 i = 0; i < Types.Num(); i++)
		GVC->RegisterXRObjectVisualizer(Types[i], MakeShareable(new FArmyPipelineVisualizer));
	GVC->RegisterXRObjectVisualizer(OT_ColdWaterTube_Linker, MakeShareable(new FArmyPipePointVisualizer));
	GVC->RegisterXRObjectVisualizer(OT_HotWaterTube_Linker, MakeShareable(new FArmyPipePointVisualizer));
	GVC->RegisterXRObjectVisualizer(OT_Drain_Linker, MakeShareable(new FArmyPipePointVisualizer));

	{
		//添加右侧物体列表目录路径
		TSharedPtr<FObjectGroupContentItem> ObjGroupItem = MakeShareable(new FObjectGroupContentItem);
		ObjGroupItem->ModelName = FArmyActorPath::GetHydropowerPath().ToString();
		ObjGroupItem->ModelID = 2;
		ObjGroupItem->VisibleInOtherModel = false;
		ObjGroupItem->ItemPath = FArmyActorPath::GetPipelinePath().ToString();
		SArmyModelContentBrowser::ObjectGroupCategoriesArray.Add(ObjGroupItem);
	}
	// 水电模式数据初始化
	FArmyHydropowerDataManager::Get()->Initialize();
	//配电箱系统设置管理类初始化
	PowerSystemManager = MakeShareable(new FArmyPowerSystemManager);

	TopViewIgnoreObjectArray.AddUnique(EC_Dewatering_Point);
	TopViewIgnoreObjectArray.AddUnique(EC_Water_Supply);
	TopViewIgnoreObjectArray.AddUnique(EC_Drain_Point);
	TopViewIgnoreObjectArray.AddUnique(EC_Water_Basin);
	TopViewIgnoreObjectArray.AddUnique(EC_Closestool);
}

void FArmyHydropowerModeController::InitOperations()
{
	//初始化操作类
	OperationMap.Add(HO_DrawLine, MakeShareable(new FArmyPipelineOperation(E_HydropowerModel, GVC->GetWorld())));
	OperationMap.Add(HO_Component, MakeShareable(new FArmyHydropowerComponentOperation(E_HydropowerModel)));
	TSharedPtr<FArmySwitchConnectOperation> SwitchContentOperation = MakeShareable(new FArmySwitchConnectOperation(E_HydropowerModel));
	OperationMap.Add(HO_SwitchConnect, SwitchContentOperation);
	SwitchContentOperation->OnBenginOrEndOperationDelegate.BindRaw(this, &FArmyHydropowerModeController::BeginSwitchOperation);
	if (HydropowerSwitchConnect.IsValid())
	{
		SwitchContentOperation->SetDetailView(HydropowerSwitchConnect);
	}
	for (auto It : OperationMap)
	{
		It.Value->Init();
		It.Value->InitWidget(GVC->ViewportOverlayWidget);
	}

	FArmyRuler::Get()->Show(false);

	FArmyRuler::Get()->AxisOperationDelegate.BindRaw(this, &FArmyHydropowerModeController::AxisOperation);
}

void FArmyHydropowerModeController::BeginMode()
{
	FArmyDesignModeController::BeginMode();

	// @zengy 重置套餐
	LeftPanel->OnBeginMode();

	CurrentOperationID = HO_NONE;

	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = FArmyUser::Get().CurProjectData->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	//获取项目选择的套餐，搜索默认选择项目套餐
	TSharedPtr<FContentItemSpace::FProjectRes> ProRes = StaticCastSharedPtr<FContentItemSpace::FProjectRes>(resArr[0]);

	TSharedPtr<FArmyKeyValue> defaultPackage = MakeShareable(new FArmyKeyValue(ProRes->SetMealId, ProRes->SetMealName));
	LeftPanel->SetDefaultPackage(defaultPackage);
	TSharedPtr<SArmyCategoryOrganization> CatOrganization = CategoryOrgManage->GetCurrentSelectCat();
	if (CatOrganization.IsValid())
		CatOrganization->SelectFirstCategory();
	else
		LeftPanel->RequestPackage(0);

	GVC->SetDrawMode(DM_3D);

	bViewTOP = GVC->GetViewportType() == EXRView_TOP;


	TSharedPtr<IArmyActorVisitor> WHCVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_WHCActor);
	WHCVisitor->Show(false);
	TSharedPtr<IArmyActorVisitor> HydropowerModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HydropowerMode);
	HydropowerModeAV->Show(true);

	FArmyRuler::Get()->Show(false);


	FArmyToolsModule::Get().GetMouseCaptureTool()->Reset();
	FArmyToolsModule::Get().GetMouseCaptureTool()->SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = false;
	GVC->PlaceActorToWorldDelegate.BindSP(LeftPanel.Get(), &SArmyHydropowerModeContentBrowser::OnPlaceActorToWorld);

	GVC->EnableBSPSelectAble(false);//不能选中bsp（已经不能再用）

	// 设置墙体、地面、顶面为半透明
	int32 Opcaital = FArmyHydropowerDataManager::Get()->GetCurrentOpcity();
	FArmySceneData::Get()->SetHomeOpacity(Opcaital / 100.f);


	//禁用鼠标滚轮旋转功能 2019.1.16 郭子阳
	GVC->CloseMouseRotate();

	//设置此模块默认显示的目录数据
	static bool FirstDefaultSelectCat = true;
	if (FirstDefaultSelectCat)
	{
		CategoryOrgManage->SetDefaultSelectOrgination(SArmyCategoryOrganization::OZ_CompanyData);
		FirstDefaultSelectCat = false;
	}

	/** @马云龙 水电模式自动切换模式 */
	FArmyWorldManager::Get().TryChangeEnvironmentModeConditionally(DesignMode::HydropowerMode);

	// 初始化配电箱自动生成回路的样本回路数据 2019.5.9
	FArmyHydropowerDataManager::Get()->InitPowerSystemLoopSamples();
}

bool FArmyHydropowerModeController::EndMode()
{
	FArmySceneData::Get()->EmptyGlobal();

	CurrentOperationID = HO_NONE;

	TSharedPtr<IArmyActorVisitor> WHCVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_WHCActor);
	WHCVisitor->Show(true);

	GXREditor->SelectNone(true, true);
	EndOperation();
	GVC->EnableBSPSelectAble(true);

	// 设置墙体、地面、顶面为半透明
	FArmySceneData::Get()->SetHomeOpacity(1.f);

	FArmyDesignModeController::EndMode();

	//恢复鼠标滚轮旋转功能 2019.1.16 郭子阳
	GVC->OpenMouseRotate();
	FArmyRuler::Get()->Show(false);
	return true;
}

TSharedPtr<SWidget> FArmyHydropowerModeController::MakeLeftPanelWidget()
{
	//企业素材
	CategoryOrgManage = SNew(SArmyCategoryOrganizationManage);

	LeftPanel = SNew(SArmyHydropowerModeContentBrowser)
		//.CategoryUrl("/api/mode_menu/elevation")
		.SearchUrl("/api/bim_goods/hydropower/query")
		.OnDelegate_3DTo2DGanged(FArmyFComponentResDelegate::CreateRaw(this, &FArmyHydropowerModeController::On3DTo2DGanged))
		.OnContentItemWithoutComponent(FArmyFComponentDelegate::CreateRaw(this,&FArmyHydropowerModeController::CreateObjForMeshActor))
		.ParentId(2);
	LeftPanel->ItemSelected.BindRaw(this, &FArmyHydropowerModeController::SelectContentItem);

	TSharedPtr<SArmyCategoryOrganization> CompanyDataWid = SNew(SArmyCategoryOrganization)
		.CategoryUrl("/api/mode_menu/hydropower")
		.ButtonImage(FArmyStyle::Get().GetBrush("Icon.companyCategry"))
		.CurrentBrowser(LeftPanel.ToSharedRef())
		.bResponseToSearch(true)
		.ECurrentOrganization(SArmyCategoryOrganization::EOragnization::OZ_CompanyData);

	//请求目录数据
	CompanyDataWid->RequestHardModeCategory();
	CategoryOrgManage->AddOrganization(CompanyDataWid);

	return CategoryOrgManage;
}

TSharedPtr<SWidget> FArmyHydropowerModeController::MakeRightPanelWidget()
{
	return CreateLayerOutliner();
}

TSharedPtr<SWidget> FArmyHydropowerModeController::MakeToolBarWidget()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"
	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	FCanExecuteAction CheckHasSelectedActor = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedActor);
	FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);
	FCanExecuteAction CheckCanGroup = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanGroup);
	FIsActionChecked IsOverallViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsOverallViewChecked);
	FIsActionChecked IsTopViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsTopViewChecked);
	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandSave, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnSaveCommand));
	
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandDelete, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnDeleteCommand), CheckHasSelectedActor);
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyCommandes), LOCTEXT("Emptyes", "清空"), FSlateIcon("ArmyStyle", "HydropowerMode.CommandEmpty"));
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandPowerSystem, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEnterPowerSystem));
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandExitSwitchConnect, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnChangeToSwtichConnectCommand, 0), FCanExecuteAction(), FIsActionChecked(), FIsActionButtonVisible::CreateRaw(this, &FArmyHydropowerModeController::IsSwitchConnect));
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandSwitchConnect, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnChangeToSwtichConnectCommand, 1), FCanExecuteAction(), FIsActionChecked(), FIsActionButtonVisible::CreateRaw(this, &FArmyHydropowerModeController::CanVisibleSwitchConnect));
	//#if SERVER_MODE == PLATFORM_STAGE
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandAutoDesignLine, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnDesignPipeLine));
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandAutoDesignPoint, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnAutoDesignPoint));
	//#endif		
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandTopView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuTopView), DefaultCanExec, IsTopViewChecked);
	ToolBarBuidler->AddButton(FArmyHydropowerModeCommands::Get().CommandOverallView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuOverallView), DefaultCanExec, IsOverallViewChecked);

	CommandList->Append(ToolBarBuidler->GetCommandList());
#undef LOCTEXT_NAMESPACE
	return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

TSharedPtr<SWidget> FArmyHydropowerModeController::MakeSettingBarWidget()
{
	return nullptr;
}

void FArmyHydropowerModeController::Draw(FViewport* InViewport, const FSceneView* InView, FCanvas* SceneCanvas)
{
	if (!IsCurrentModel)
		return;
	//	AxisRuler->Draw(InView,SceneCanvas);

	TArray<TWeakPtr<FArmyObject>> RoomList;
	//获取当前选中的墙面，墙面法线
	if (GVC != nullptr &&GVC->IsValidLowLevel())
	{
		FVector InLocation = FArmyMouseCaptureManager::Get()->GetCapturePoint();
		FVector InNormal = FArmyMouseCaptureManager::Get()->GetCaptureNormal();
		//	FVector InWallDiection = FArmyMouseCaptureManager::Get()->GetCaptureWallDirection(InLocation);

		FVector2D InLocation2D;
		GVC->WorldToPixel(InLocation, InLocation2D);
		TArray<TSharedPtr<FArmyRoomSpaceArea>> selectLists;
		FArmySceneData::Get()->ForceOnRoomAreas(InLocation, GVC, selectLists);
		if (selectLists.Num() != 0)
		{
			TWeakPtr<FArmyRoomSpaceArea> plane = selectLists[0];
			if (selectLists.Num() > 1)
			{
				for (int32 i = 0; i < selectLists.Num(); i++)
				{
					FVector planeNormal = selectLists[i]->GetPlaneNormal();
					if (!FArmyMath::AreLinesParallel(FVector::UpVector, planeNormal))
					{
						plane = selectLists[i];
						break;
					}
				}
			}

			InNormal = plane.Pin()->GetPlaneNormal();
			FArmyMouseCaptureManager::Get()->SetCaptureNormal(InNormal);
			RoomList.AddUnique(plane);
		}

		auto DroppingActor = Cast<AActor>(GVC->GetFirstDroppingObject());
		 if (GVC->GetIsDraggingWidget())
		{
			TArray<AActor*> OutActors;
			GXREditor->GetSelectedActors(OutActors);
			if (OutActors.Num())
			{
				DroppingActor = OutActors[0];
			}
		}

		FRotator AxisRotator(0, 0, 0);
		if (DroppingActor)
		{
			InLocation = DroppingActor->GetActorLocation();
			AxisRotator = DroppingActor->GetActorRotation();
		}
		
		auto info = FArmyMouseCaptureManager::Get()->GetCaptureInfo(InLocation);
		FArmyRuler::Get()->Update(info, bViewTOP, true, AxisRotator);
	}


}

void FArmyHydropowerModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	//AxisRuler->Draw (PDI,View);
	FArmyRuler::Get()->Draw(PDI, View);

	if (GVC != nullptr&&GVC->IsValidLowLevel())
		FArmyMouseCaptureManager::Get()->Draw(View, PDI);
	FArmyDesignModeController::Draw(View, PDI);
}

bool FArmyHydropowerModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (OperationMap.FindRef(HO_SwitchConnect) == CurrentOperation && Event == IE_Pressed && Key == EKeys::RightMouseButton)
	{
		return CurrentOperation->InputKey(GVC, Viewport, Key, Event);
	}
	return FArmyDesignModeController::InputKey(Viewport, Key, Event);
}

void FArmyHydropowerModeController::EndOperation()
{
	TArray<AActor*> OutActors;
	GXREditor->GetSelectedActors(OutActors);
	if (OutActors.Num() == 0)
		//	AxisRuler->SetAxisRulerDisplay(false,false,false);
	{
		FArmyRuler::Get()->Show(false);
	}
	if (CurrentOperation.IsValid())
		CurrentOperation->EndOperation();
	FArmyDesignModeController::EndOperation();
}

bool FArmyHydropowerModeController::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	if (Delta < 1.)
	{

	}
	return false;
}

void FArmyHydropowerModeController::Tick(float DeltaSeconds)
{
	FArmyDesignModeController::Tick(DeltaSeconds);
}


void FArmyHydropowerModeController::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	FArmyDesignModeController::MouseMove(Viewport, X, Y);
	//FArmyMouseCaptureManager::Get()->MouseMove(GVC,Viewport,X,Y);
	FArmyMouseCaptureManager::Get()->Capture(GVC, X, Y);

	HHitProxy* HitProxy = Viewport->GetHitProxy(X, Y);
	if (HitProxy&& HitProxy->IsA(HActor::StaticGetType()))
	{
		HActor* ActorProxy = (HActor*)HitProxy;
		AActor* Actor = ActorProxy->Actor;
		Actor->IsSelected();
		GXREditor->SetActorSelectionFlags(Actor);
	}
}

void FArmyHydropowerModeController::Clear()
{
	HydropowerDetail->Clear();
	// TODO:
	Cleanup(true);
}

void FArmyHydropowerModeController::Load(TSharedPtr<FJsonObject> Data)
{
	// TODO:
	int32 i = 0;

	Cleanup(false);

	PowerSystemManager->Load(Data);

	if (Data.IsValid())
	{
		TSharedPtr<FJsonObject> HardModeData = Data->GetObjectField("hydropowerMode");

		//读取移动家具信息
		TArray<FMoveableMeshSaveData> Save_MoveableMeshList;
		const TArray<TSharedPtr<FJsonValue>> MoveableMeshListData = HardModeData->GetArrayField(TEXT("moveableMeshList"));
		for (auto& It : MoveableMeshListData)
		{
			TSharedPtr<FJsonObject> ItObject = It->AsObject();
			if (ItObject.IsValid())
			{
				int32 ID = ItObject->GetIntegerField(TEXT("id"));
				FVector Location = FVector::ZeroVector;
				Location.InitFromString(ItObject->GetStringField(TEXT("location")));
				FRotator Rotation = FRotator::ZeroRotator;
				Rotation.InitFromString(ItObject->GetStringField(TEXT("rotation")));
				FVector Scale3D = FVector::ZeroVector;
				Scale3D.InitFromString(ItObject->GetStringField(TEXT("scale")));
				int32 ToggleMode = ItObject->GetIntegerField(TEXT("toggleMode"));
				int32 nAutoDesignFlag = ItObject->GetIntegerField(TEXT("AutoDesignFlag"));
				//@飞舞轻扬
				TSharedPtr<FArmyFurniture> NewFurniture = nullptr;
				if (ItObject->HasField("object"))
				{
					const TSharedPtr<FJsonObject> ObjectJson = ItObject->GetObjectField(TEXT("object"));
					NewFurniture = MakeShareable(new FArmyFurniture);
					NewFurniture->Deserialization(ObjectJson);
					//FArmySceneData::Get()->Add(NewFurniture,XRArgument(1).ArgUint32(E_HydropowerModel),nullptr,true);
				}
				Save_MoveableMeshList.Add(FMoveableMeshSaveData(ID, Location, Rotation, Scale3D, NewFurniture, ToggleMode, nAutoDesignFlag));

				//读取材质参数
				const TArray<TSharedPtr<FJsonValue>> JObject_MaterialList = ItObject->GetArrayField(TEXT("materialList"));
				for (auto& It2 : JObject_MaterialList)
				{
					TSharedPtr<FJsonObject> It2Object = It2->AsObject();
					if (It2Object.IsValid())
					{
						FString SlotName = It2Object->GetStringField(TEXT("sn"));
						int32 MatID = It2Object->GetIntegerField(TEXT("id"));
						FString Parameters = It2Object->GetStringField(TEXT("parameters"));
						float RotateUV = It2Object->GetNumberField(TEXT("uvr"));
						float OffsetU = It2Object->GetNumberField(TEXT("su"));
						float OffsetV = It2Object->GetNumberField(TEXT("sv"));
						float TilingU = It2Object->GetNumberField(TEXT("tu"));
						float TilingV = It2Object->GetNumberField(TEXT("tv"));
						bool bProjectiveUV = It2Object->GetIntegerField(TEXT("up")) == 1;

						Save_MoveableMeshList.Last().MaterialList.Add(
							FMeshMaterialData(SlotName, MatID, Parameters,
								RotateUV, OffsetU, OffsetV, TilingU, TilingV, bProjectiveUV)
						);
					}
				}
			}
		}

		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		ResMgr->CurWorld = GVC->GetWorld();
		// 加载模型
		for (auto& It : Save_MoveableMeshList)
		{
			AActor* CreatedActor = ResMgr->CreateActorFromID(GVC->GetWorld(), It.ItemID, "", It.Location, It.Rotation, It.Scale3D);
			if (CreatedActor)
			{
				if (It.AutoDesignFlag == 1 || It.AutoDesignFlag == 2 || It.AutoDesignFlag == 3)
				{
					CreatedActor->Tags.Add(XRActorTag::AutoDesignPoint);
				}

				FVRSObject* ResultObj = ResMgr->GetObjFromObjID(CreatedActor->GetObjID());
				FContentItemPtr ResultSynData = ResMgr->GetContentItemFromID(CreatedActor->GetSynID());

				if (ResultObj && ResultSynData.IsValid())
				{
					CreatedActor->SetFolderPath(FName(*(LeftPanel->FindObjectGroup(ResultSynData->CategryID))));
					CreatedActor->SetActorLabel(ResultSynData->Name);

					if (ResultSynData->bIsSale)
					{
						CreatedActor->Tags.Add(FName("IsSale"));
					}
					//查看模型上的材质参数是否需要保存
					UMeshComponent* TheMeshCOM = NULL;
					TArray<UMeshComponent*> TheMeshCOMList;
					TArray<UMaterialInterface*> Materials;
					TArray<FName> MaterialSlotNames;
					TInlineComponentArray<USceneComponent*> SceneComponents;
					CreatedActor->GetComponents(SceneComponents);
					//获取目标MeshComponent
					if (ResultObj->bNormalized)
					{
						for (USceneComponent* SceneComp : SceneComponents)
						{
							UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
							if (MeshCOM)
							{
								TheMeshCOM = MeshCOM;
								break;
							}
						}
					}
					//在目标MeshComponent上获取材质列表，插槽名称列表，解析材质参数并保存
					if (TheMeshCOM)
					{
						//如果MaterialList不为空，则创建材质，并附加到改模型上，一般用于硬装造型类的模型
						for (auto& It2 : It.MaterialList)
						{
							float RotateUV = It2.RotateUV;
							float OffsetU = It2.OffsetU;
							float OffsetV = It2.OffsetV;
							float TilingU = It2.TilingU;
							float TilingV = It2.TilingV;
							bool bUseProjectiveUV = It2.bProjectiveUV;
							int32 OutObjID = 0;
							int32 OutSynID = 0;
							FVRSObject* MatResultObj = ResMgr->LoadObjFromFileID(It2.PakID, OutObjID, OutSynID);
							if (MatResultObj)
							{
								//如果材质参数不为空，则转换并覆盖
								if (!It2.ParaStr.IsEmpty())
								{
									//把从xml载入的材质Json字符串转换为标准数组格式
									TArray<FModelMaterialData> LoadedModelMaterialData;
									if (ResMgr->ConvertJsonToModelMaterialList(true, It2.ParaStr, LoadedModelMaterialData))
									{
										ResMgr->OverrideMaterialData(MatResultObj->MaterialList[0], LoadedModelMaterialData[0]);
									}
								}
								//如果为空，则需要把材质参数恢复到默认值。因为有可能他的参数克隆自上一个载入的材质。
								else
								{
									for (auto& It : MatResultObj->MaterialList[0].ParaList)
									{
										//恢复默认值
										It.ResetToServer();
									}
									//并且把这些值设置到材质中
									ResMgr->OverrideMaterialData(MatResultObj->MaterialList[0], MatResultObj->MaterialList[0]);
								}

								UMaterialInterface* MI = Cast<UMaterialInterface>(MatResultObj->GetFirstObject());
								if (MI)
								{
									UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MI);
									if (MID)
									{
										MID->SetScalarParameterValue(TEXT("UseProjectiveUV"), bUseProjectiveUV ? 1.f : 0.f);
										ResMgr->SetMaterialOffsetU(MID, OffsetU);
										ResMgr->SetMaterialOffsetV(MID, OffsetV);
										ResMgr->SetMaterialTilingU(MID, TilingU);
										ResMgr->SetMaterialTilingV(MID, TilingV);
										ResMgr->SetMaterialRotation(MID, It2.RotateUV/*, true*/);
										TheMeshCOM->SetMaterialByName(FName(*It2.SlotName), MID);

										//只有非标准蓝图物体 才会执行，因为蓝图中可能有多个组件，所以把所有材质都加上去
										if (TheMeshCOMList.Num() > 0)
										{
											for (int32 k = 0; k < TheMeshCOMList.Num(); k++)
											{
												TheMeshCOMList[k]->SetMaterialByName(FName(*It2.SlotName), MID);
											}
										}
									}
								}
							}
						}
					}
					if (ResultSynData->GetComponent().IsValid())
					{
						ThreeTo2DGanged(ResultSynData->GetComponent(), CreatedActor, It.Object);
						FVector Location = CreatedActor->GetActorLocation();
						CreatedActor->SetActorRelativeLocation(Location);
					}
					else
					{
						//@郭子阳
						//没有构件就生成空构件
						CreateObjForMeshActor(ResultSynData, CreatedActor, It.Object, MI_HydropowerMode);
						FVector Location = CreatedActor->GetActorLocation();
						CreatedActor->SetActorRelativeLocation(Location);
					}
				}
			}
		}

		TSharedPtr<FArmyPipelineOperation> PipeLineOperation = StaticCastSharedPtr<FArmyPipelineOperation>(OperationMap[HO_DrawLine]);
		const TArray< TSharedPtr<FJsonValue> > HydropowerLines = HardModeData->GetArrayField("HydropowerLine");
		TArray<TSharedPtr<FArmyPipeline>> Lines;
		for (auto& LineJson : HydropowerLines)
		{
			TSharedPtr<FJsonObject> JObject = LineJson->AsObject();
			TSharedPtr<FArmyPipeline> Line = MakeShareable(new FArmyPipeline());
			Line->Deserialization(JObject);
			TWeakPtr<FArmyObject> LineObject = FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, Line->GetUniqueID());
			if (!LineObject.IsValid())
			{
				Line->PostEditDelegate.BindRaw(PipeLineOperation.Get(), &FArmyPipelineOperation::PostEditPointes);
				FArmySceneData::Get()->Add(Line, XRArgument(1).ArgUint32(E_HydropowerModel).ArgBoolean(false));
				Lines.AddUnique(Line);
			}
		}
		TArray<TSharedPtr<FArmyPipePoint>> Pointes;
		for (auto& LinePtr : Lines)
		{
			LinePtr->GeneratePipelineModel(GVC->GetWorld());
			Pointes.AddUnique(LinePtr->StartPoint);
			Pointes.AddUnique(LinePtr->EndPoint);
			/*
			LinePtr->UpdateWithPoints();*/
		}
		if (PipeLineOperation.IsValid())
		{
			PipeLineOperation->CreateBridgeBend(Pointes);
			PipeLineOperation->UpdateBridgeBend(Pointes);
		}

		Lines.Empty();
	}

	//@郭子阳
	if (Data.IsValid())
	{
		//兼容1.5版本之前的施工项数据
		TArray<FObjectWeakPtr> ObjArray = FArmySceneData::Get()->GetObjects(E_HydropowerModel);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_ComponentBase, ObjArray);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Component_None, ObjArray);
		for (auto ObjPtr : ObjArray)
		{
			FArmyFurniture* FurnitureObj = ObjPtr.Pin()->AsassignObj<FArmyFurniture>();
			if (FurnitureObj &&FurnitureObj->CanOutPutCalcList())
			{
				AActor* ObjActor = FurnitureObj->GetRelevanceActor();
				if (ObjActor
					&& (ObjActor->Tags.Contains("HydropowerActor") || ObjActor->Tags.Contains(XRActorTag::OriginalPoint)))
				{

					//纯移位施工项
					bool OffsetConstruction = FurnitureObj->IsOrignalPoint();
					if (OffsetConstruction)
					{
						ConstructionPatameters Parameters;
						Parameters.SetPureOrignalOffset((EComponentID)FurnitureObj->ComponentType);

						if (FurnitureObj->ConstructionData->bHasSetted)
						{
							XRConstructionManager::Get()->SaveConstructionData(FurnitureObj->GetUniqueID(), Parameters, FurnitureObj->ConstructionData);
						}

						XRConstructionManager::Get()->TryToFindConstructionData(FurnitureObj->GetUniqueID(), Parameters, FConstructionDataDelegate::CreateRaw(this, &FArmyHydropowerModeController::ConstructionCallBack));

						continue;
					}

					//非纯移位施工项

					//2.获取商品信息
					UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
					TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(FurnitureObj->GetRelevanceActor()->GetSynID());

					//1.获取施工项

					ConstructionPatameters Parameters;
					Parameters.SetNormalGoodsInfo(ActorItemInfo->ID);
					if (FurnitureObj->ConstructionData->bHasSetted)
					{
						XRConstructionManager::Get()->SaveConstructionData(FurnitureObj->GetUniqueID(), Parameters, FurnitureObj->ConstructionData);
					}

					XRConstructionManager::Get()->TryToFindConstructionData(FurnitureObj->GetUniqueID(), Parameters, FConstructionDataDelegate::CreateRaw(this, &FArmyHydropowerModeController::ConstructionCallBack));

				}
				else if (FArmyPipeline* PipeObj = ObjPtr.Pin()->AsassignObj<FArmyPipeline>())
				{
					if (PipeObj->GetLenght() <= 0 || PipeObj->ProductID == -1)
					{
						continue;
					}

					ConstructionPatameters Parameters;
					Parameters.SetNormalGoodsInfo(PipeObj->ProductID);
					if (FurnitureObj->ConstructionData->bHasSetted)
					{
						XRConstructionManager::Get()->SaveConstructionData(FurnitureObj->GetUniqueID(), Parameters, FurnitureObj->ConstructionData);
					}

					XRConstructionManager::Get()->TryToFindConstructionData(FurnitureObj->GetUniqueID(), Parameters, FConstructionDataDelegate::CreateRaw(this, &FArmyHydropowerModeController::ConstructionCallBack));

				}
				else if (FArmyPipePoint* PointObj = ObjPtr.Pin()->AsassignObj<FArmyPipePoint>())
				{
					AActor* ObjActor = PointObj->GetRelevanceActor();
					if (!ObjActor || ObjActor->Tags.Contains(XRActorTag::OriginalPoint))
					{
						continue;
					}

					ConstructionPatameters Parameters;
					Parameters.SetNormalGoodsInfo(PointObj->ProductID);
					if (FurnitureObj->ConstructionData->bHasSetted)
					{
						XRConstructionManager::Get()->SaveConstructionData(FurnitureObj->GetUniqueID(), Parameters, FurnitureObj->ConstructionData);
					}

					XRConstructionManager::Get()->TryToFindConstructionData(FurnitureObj->GetUniqueID(), Parameters, FConstructionDataDelegate::CreateRaw(this, &FArmyHydropowerModeController::ConstructionCallBack));
				}
			}
		}
	}
}

bool FArmyHydropowerModeController::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	// TODO:
	TArray<FMoveableMeshSaveData> Save_MoveableMeshList;
	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	// 遍历场景中所有的有效保存Actor
	for (TActorIterator<AXRFurnitureActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		// 查找Actor的ItemID
		AActor* TempActor = nullptr;
		for (int32 i = 0; i < ActorItr->Children.Num(); i++)
		{
			AActor* ActorItem = ActorItr->Children[i];
			if (ActorItem->ActorHasTag(XRActorTag::OriginalPoint))
			{
				break;//@ 原始点位不保存在水电模式中
			}

			if (ActorItem->Tags.Contains("HydropowerActor"))
			{
				TempActor = ActorItem;
				break;
			}
		}
		if (TempActor == nullptr)
			continue;
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMgr->GetContentItemFromID(TempActor->GetSynID());
		EActorType ActorType = ResMgr->GetActorType(TempActor);
		FVRSObject* ResultObj = ResMgr->GetObjFromObjID(TempActor->GetObjID());
		TSharedPtr<FArmyObject> Object = ActorItr->GetRelevanceObject().Pin();
		int32 ItemID = 0;
		if (ResultSynData.IsValid())
		{
			ItemID = ResultSynData->ID;
		}

		if (ActorType == EActorType::Moveable || ActorType == EActorType::Blueprint)
		{
			int32 AutoDesignFlag = TempActor->Tags.Contains(XRActorTag::AutoDesignPoint) ? 2 : 0;
			Save_MoveableMeshList.Add(FMoveableMeshSaveData(ItemID, TempActor->GetActorLocation(), TempActor->GetActorRotation(), TempActor->GetActorScale3D(), Object, -1, AutoDesignFlag));
			GGI->DesignEditor->SaveFileList.AddUnique(ItemID);

			// 查看模型上的材质参数是否需要保存
			UMeshComponent* TheMeshCOM = NULL;
			TArray<UMaterialInterface*> Materials;
			TArray<FName> MaterialSlotNames;
			TInlineComponentArray<USceneComponent*> SceneComponents;
			TempActor->GetComponents(SceneComponents);

			// 获取目标MeshComponent
			if (ResultObj->bNormalized)
			{
				for (USceneComponent* SceneComp : SceneComponents)
				{
					UMeshComponent* MeshCOM = Cast<UMeshComponent>(SceneComp);
					if (MeshCOM)
					{
						TheMeshCOM = MeshCOM;
						break;
					}
				}
			}

			// 在目标MeshComponent上获取材质列表，插槽名称列表，解析材质参数并保存
			if (TheMeshCOM)
			{
				Materials = TheMeshCOM->GetMaterials();
				MaterialSlotNames = TheMeshCOM->GetMaterialSlotNames();

				// 在ObjList中查找该材质，并解析它的材质参数，把发生变化的参数转换成Json字符串，保存到xml中。
				for (int32 i = 0; i < Materials.Num(); i++)
				{
					// 如果模型丢失材质，可能为NULL
					if (Materials[i] != NULL)
					{
						// 来自于刚刚贴上去的壁纸瓷砖类材质
						FVRSObject* MatResultObj = ResMgr->GetObjFromObjID(Materials[i]->GetObjID());
						TSharedPtr<FContentItemSpace::FContentItem> MatResultSynData = ResMgr->GetContentItemFromID(Materials[i]->GetSynID());
						if (MatResultObj && MatResultSynData.IsValid())
						{
							// 把材质参数转换为Json字符串
							FString ParaJsonStr = ResMgr->ConvertModelMaterialListToJson(MatResultObj->MaterialList, true);

							// 获取额外的几个重要的材质参数
							UMaterialInstanceDynamic* TheMID = MatResultObj->MaterialList[0].DIM;
							float RotateUV = ResMgr->GetMaterialRotation(TheMID);
							float OffsetU = ResMgr->GetMaterialOffsetU(TheMID);
							float OffsetV = ResMgr->GetMaterialOffsetV(TheMID);
							float TilingU = ResMgr->GetMaterialTilingU(TheMID);
							float TilingV = ResMgr->GetMaterialTilingV(TheMID);
							bool bUseProjective = ResMgr->GetMaterialUseProjectiveUV(TheMID);

							//保存到MaterialList中
							Save_MoveableMeshList.Last().MaterialList.Add(
								FMeshMaterialData(MaterialSlotNames[i].ToString(), MatResultSynData->ID, ParaJsonStr,
									RotateUV, OffsetU, OffsetV, TilingU, TilingV, bUseProjective
								)
							);
							GGI->DesignEditor->SaveFileList.AddUnique(MatResultSynData->ID);
						}
					}
				}
			}

		}
	}


	// 写入开始
	JsonWriter->WriteObjectStart(TEXT("hydropowerMode"));

	/** 资源版本 */
	JsonWriter->WriteValue(TEXT("version"), GResourceVersion);

	//移动家具位移信息
	JsonWriter->WriteArrayStart(TEXT("moveableMeshList"));
	for (auto& It : Save_MoveableMeshList)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue(TEXT("id"), It.ItemID);
		JsonWriter->WriteValue(TEXT("location"), It.Location.ToString());
		JsonWriter->WriteValue(TEXT("rotation"), It.Rotation.ToString());
		JsonWriter->WriteValue(TEXT("scale"), It.Scale3D.ToString());
		JsonWriter->WriteValue(TEXT("toggleMode"), It.DefaultToggleMode);
		JsonWriter->WriteValue(TEXT("AutoDesignFlag"), It.AutoDesignFlag);
		//写入材质参数 开始
		JsonWriter->WriteArrayStart(TEXT("materialList"));
		for (auto& It2 : It.MaterialList)
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue(TEXT("sn"), It2.SlotName);
			JsonWriter->WriteValue(TEXT("id"), It2.PakID);
			if (!It2.ParaStr.IsEmpty())
				JsonWriter->WriteValue(TEXT("parameters"), It2.ParaStr);
			if (It2.RotateUV != 0.f)
				JsonWriter->WriteValue(TEXT("uvr"), It2.RotateUV);
			if (It2.OffsetU != 0.f)
				JsonWriter->WriteValue(TEXT("su"), It2.OffsetU);
			if (It2.OffsetV != 0.f)
				JsonWriter->WriteValue(TEXT("sv"), It2.OffsetV);
			if (It2.TilingU != 0.f)
				JsonWriter->WriteValue(TEXT("tu"), It2.TilingU);
			if (It2.TilingV != 0.f)
				JsonWriter->WriteValue(TEXT("tv"), It2.TilingV);
			JsonWriter->WriteValue(TEXT("up"), It2.bProjectiveUV ? 1 : 0);
			JsonWriter->WriteObjectEnd();
		}
		JsonWriter->WriteArrayEnd();
		if (It.Object.IsValid())
		{
			JsonWriter->WriteObjectStart(TEXT("object"));
			It.Object->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
		}
		//写入材质参数 结束
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();



	//室内空间


	TArray<EObjectType> LineTypes = {
		 OT_StrongElectricity_25,            //强电2.5
		OT_StrongElectricity_4,            //强电4
		OT_StrongElectricity_Single,            //强电单控
		OT_StrongElectricity_Double,            //强电双控
		OT_WeakElectricity_TV,//弱电 电视线
		OT_WeakElectricity_Net,//弱电， 网线
		OT_WeakElectricity_Phone,//弱电，电话线
		OT_ColdWaterTube,//冷水管
		OT_HotWaterTube,//热水管
		OT_Drain,//排水管
	};
	TArray<EObjectType> LinkerTypes = {
		OT_StrongElectricity_25_Linker,            //强电2.5
		OT_StrongElectricity_4_Linker,            //强电4
		OT_StrongElectricity_Single_Linker,            //强电单控
		OT_StrongElectricity_Double_Linker,            //强电双控
		OT_WeakElectricity_TV_Linker,//弱电 电视线
		OT_WeakElectricity_Net_Linker,//弱电， 网线
		OT_WeakElectricity_Phone_Linker,//弱电，电话线
		OT_ColdWaterTube_Linker,//冷水管
		OT_HotWaterTube_Linker,//热水管
		OT_Drain_Linker,//排水管
	};
	TArray<FObjectWeakPtr> LinkerObjects, LineObjects;
	for (auto LinkerType : LinkerTypes)
		FArmySceneData::Get()->GetObjects(EModelType::E_HydropowerModel, LinkerType, LinkerObjects);
	for (auto LineType : LineTypes)
		FArmySceneData::Get()->GetObjects(EModelType::E_HydropowerModel, LineType, LineObjects);

	JsonWriter->WriteArrayStart(TEXT("HydropowerLine"));
	for (auto& Line : LineObjects)
	{
		JsonWriter->WriteObjectStart();
		Line.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//写入施工项数据
	JsonWriter->WriteArrayStart(TEXT("HydropowerConstructionData"));
	for (auto data : FArmySceneData::HyConstructionDataMap)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue<int32>(TEXT("ConstructionIndex"), data.Key);
		//JsonWriter->WriteValue(TEXT("ConstructionDatajson"), (*data.Value.Get()));
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();

	//--------------------
	JsonWriter->WriteObjectEnd();


	PowerSystemManager->Save(JsonWriter);

	JsonWriter->Close();

	return true;
}

void FArmyHydropowerModeController::Delete()
{
	{
		SCOPE_TRANSACTION(TEXT("删除水电模型"));

		TArray<AActor*> ActorsToDelete;
		GXREditor->CanDeleteSelectedActors(GVC->GetWorld(), false, true, &ActorsToDelete);
		TArray< TWeakPtr<FArmyObject> > ActorItems = FArmySceneData::Get()->GetObjects(E_HardModel);

		TArray<TWeakPtr<FArmyPipeline>> lines;
		for (auto& ActorIt : ActorsToDelete)
		{
			//删除普通模型
			for (auto& It : ActorItems)
			{
				if (It.Pin().IsValid() && It.Pin()->GetType() == EObjectType::OT_ActorItem)
				{
					TSharedPtr<FArmyActorItem> ActorItem = StaticCastSharedPtr<FArmyActorItem>(It.Pin());

					if (ActorItem->Actor == ActorIt)
					{
						FArmySceneData::Get()->Delete(ActorItem);
						break;
					}
				}
			}
			//删除管线的链接点
			if (ActorIt->IsA(AXRPipePointActor::StaticClass()))
			{
				AXRPipePointActor* PointActor = Cast<AXRPipePointActor>(ActorIt);
				TSharedPtr<FArmyPipePoint> Point = StaticCastSharedPtr<FArmyPipePoint>(PointActor->GetRelevanceObject().Pin());
				for (auto Line : Point->PipeLines)
				{
					lines.Add(Point->GetLine(Line));
				}
			}
			//删除线
			else if (ActorIt->IsA(AXRPipelineActor::StaticClass()))
			{
				AXRPipelineActor* LineActor = Cast<AXRPipelineActor>(ActorIt);
				TWeakPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(LineActor->GetRelevanceObject().Pin());
				lines.Add(Line);
			}

		}

		for (auto& Line : lines)
		{
			Line.Pin()->Delete();
		}
	}


	GXREditor->edactDeleteSelected(GVC->GetWorld());
}

void FArmyHydropowerModeController::OnActorMoved(AActor* InActor)
{
	//只有水电的模型和管线才能执行这步操作
	if (!InActor->Tags.Contains(TEXT("HydropowerActor")) || InActor->Tags.Contains(TEXT("HydropowerPipeActor")))
		return;


	TArray<AActor*> Actores, ModifyActor;
	if (InActor->IsA(AXRGroupActor::StaticClass()))
	{
		AXRGroupActor* GroupActor = Cast<AXRGroupActor>(InActor);
		GroupActor->GetGroupActors(Actores);
	}
	else
	{
		Actores.AddUnique(InActor);
	}

	for (auto& ActorItem : Actores)
	{
		bool bTPS = ActorItem->Tags.Contains("EXRView_TPS");
		AActor* Actor = ActorItem;
		ModifyActor.AddUnique(Actor);
	}

	if (ModifyActor.Num())
	{
		SCOPE_TRANSACTION(TEXT("修改模型位置、旋转、缩放"));
		TArray< TWeakPtr<FArmyObject> > ActorItems = FArmySceneData::Get()->GetObjects(E_OtherModel);
		for (auto& Actor : ModifyActor)
		{
			for (auto& It : ActorItems)
			{
				if (It.Pin().IsValid() && It.Pin()->GetType() == EObjectType::OT_ActorItem)
				{
					TSharedPtr<FArmyActorItem> ActorItem = StaticCastSharedPtr<FArmyActorItem>(It.Pin());

					// 如果Actor没有发生变换，则不记录修改
					if (ActorItem->Actor == Actor &&
						!(ActorItem->Location == Actor->GetActorLocation() &&
							ActorItem->Rotation == Actor->GetActorRotation() &&
							ActorItem->Scale == Actor->GetActorScale3D()))
					{
						ActorItem->Modify();
						break;
					}
				}
			}
		}
	}

	//@郭子阳 刷新右侧列表的施工项,因为移动了之后actor所在房间可能变化，施工项随之发生变化
	HydropowerDetail->ShowSelectedDetial(InActor);

}

bool FArmyHydropowerModeController::AxisOperation(FVector direction)
{
	AXRActor* ActorLine = GXREditor->GetSelectedActors()->GetBottom<AXRActor>();
	AActor* SelectActor = GXREditor->GetSelectedActors()->GetBottom<AActor>();
	if (SelectActor)
	{
		FVector location = SelectActor->GetActorLocation();
		SelectActor->SetActorLocation(location + direction);
		GXREditor->UpdatePivotLocationForSelection();

		AXRActor * ParentActor = Cast<AXRActor>(SelectActor->GetOwner());
		if (ParentActor)
		{

			OnActorMoved(SelectActor);
		}
		return true;
	}
	return false;
}

void FArmyHydropowerModeController::Empty(int32 IntVal)
{
	//更具类型清空管线
	TArray<TSharedPtr<FArmyPipeline>> Lines;
	TArray<TWeakPtr<FArmyObject>> data = FArmySceneData::Get()->GetObjects(E_HydropowerModel);
	for (int32 i = 0; i < data.Num(); i++)
	{
		TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(data[i].Pin());
		if (!Line.IsValid())
			continue;
		EObjectType ObjectType = Line->GetType();
		bool bCanEmpty = false;
		switch (IntVal)
		{
		case EE_Strong:
			bCanEmpty = (ObjectType == OT_StrongElectricity_25 || ObjectType == OT_StrongElectricity_4);
			break;
		case EE_Light:
			bCanEmpty = (ObjectType == OT_StrongElectricity_Double || ObjectType == OT_StrongElectricity_Single);
			break;
		case EE_Weak:
			bCanEmpty = (ObjectType == OT_WeakElectricity_TV || ObjectType == OT_WeakElectricity_Net || ObjectType == OT_WeakElectricity_Phone);
			break;
		case EE_ColdWater:
			bCanEmpty = (ObjectType == OT_ColdWaterTube);
			break;
		case EE_HotWater:
			bCanEmpty = (ObjectType == OT_HotWaterTube);
			break;
		case EE_Drain:
			bCanEmpty = (ObjectType == OT_Drain);
			break;
		case EE_Max:
			bCanEmpty = (ObjectType == OT_StrongElectricity_25 || ObjectType == OT_StrongElectricity_4) ||
				(ObjectType == OT_StrongElectricity_Double || ObjectType == OT_StrongElectricity_Single) ||
				(ObjectType == OT_HotWaterTube) || (ObjectType == OT_WeakElectricity_TV || ObjectType == OT_WeakElectricity_Net ||
					ObjectType == OT_WeakElectricity_Phone) || (ObjectType == OT_ColdWaterTube) || (ObjectType == OT_Drain);
			break;
		default:
			bCanEmpty = false;
			break;
		}
		if (!bCanEmpty)
			continue;
		Lines.AddUnique(Line);
	}
	for (auto& Line : Lines)
	{
		Line->DeleteSelf(true);
		FArmySceneData::Get()->Delete(Line);
	}
}

TSharedRef<SWidget> FArmyHydropowerModeController::OnEmptyCommandes()
{
	//放置光源前结束上一次操作
	EndOperation();


	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);
	//
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteStrongELe, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyStrongCommand));
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteLighting, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyLightCommand));
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteWeakELe, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyWeakCommand));
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteColdWater, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyColdWaterCommand));
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteHotWater, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyHotWaterCommand));
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteDrain, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyDrainCommand));
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandDeleteAll, FExecuteAction::CreateRaw(this, &FArmyHydropowerModeController::OnEmptyCommand));

	//ToolBarBuidler->AddButton (FArmyHydropowerModeCommands::Get ().CommandEmpty,FExecuteAction::CreateRaw (this,&FArmyHydropowerModeController::OnEmptyCommand));

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteStrongELe);
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteLighting);
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteWeakELe);

	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteColdWater);
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteHotWater);
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteDrain);
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandDeleteAll);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FArmyHydropowerModeController::OnViewComboCommand()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef<FUICommandList> ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	FIsActionChecked IsTopViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsTopViewChecked);
	FIsActionChecked IsCeilingViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsCeilingViewChecked);

	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandTopView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuTopView), DefaultCanExec, IsTopViewChecked);
	ActionList->MapAction(FArmyHydropowerModeCommands::Get().CommandCeilingView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuCeilingView), DefaultCanExec, IsCeilingViewChecked);

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandTopView);
	MenuBuilder.AddMenuEntry(FArmyHydropowerModeCommands::Get().CommandCeilingView);

	return MenuBuilder.MakeWidget();
}

void FArmyHydropowerModeController::Cleanup(bool bClearResource)
{
	Empty(EE_Max);
	for (TActorIterator<AActor> it(GVC->GetWorld()); it; ++it)
	{
		if (it->Tags.Contains("HydropowerActor"))
			it->Destroy();
	}
	FArmyAutoDesignPoint::GetInstance()->Empty();
	GVC->ClearVisualizersForSelection();
	TransMgr->Reset();//撤销重做
	PowerSystemManager->Cleanup();
}

bool FArmyHydropowerModeController::ShouldTick()
{
	GVC->GetViewportSize(ViewportSize);
	GVC->GetMousePosition(ViewportMousePos);
	if (ViewportSize.X == 0 || ViewportSize.Y == 0)
	{
		return false;
	}

	if (LastViewportMousePos == ViewportMousePos || ViewportMousePos < -ViewportSize)
	{
		return false;
	}

	LastViewportMousePos = ViewportMousePos;

	return true;
}

bool FArmyHydropowerModeController::IsMiddleButtonPressed() const
{
	return GVC->Viewport->KeyState(EKeys::MiddleMouseButton);
}

void FArmyHydropowerModeController::SelectContentItem()
{
	//选中商品列表Item
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = LeftPanel->GetSelectedContentItem();
	TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
	if (resArr.Num() < 1)
		return;
	TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes>(resArr[0]);
	switch (Res->Categry0Type) ///画线
	{
	case HO_DrawLine:
	{
		TSharedPtr<FArmyPipelineOperation> PipeLineOperation = StaticCastSharedPtr<FArmyPipelineOperation>(OperationMap.FindRef(HO_DrawLine));
		PipeLineOperation->SetCurrentItem(ContentItem);
	}
	break;
	default:
		break;
	}

	SetOperation(Res->Categry0Type);
	//	AxisRuler->SetAxisRulerDisplay(true,true,!bViewTOP);
	FArmyRuler::Get()->Show(true);
	if (Res->Categry0Type == HO_Component)
	{
		LeftPanel->AddToScene(ContentItem);//添加水电构件到场景
	}
}

void FArmyHydropowerModeController::OnSelectionNone()
{
//	FirstSelectedActor = nullptr;
	//FArmyRuler::Get()->Show(false);
	//AxisRuler->SetActor(nullptr);
	if (HO_DrawLine == GetCurrentOperationID())
		return;
	if (GetCurrentOperationID() == 0)
		if (GetCurrentOperationID() != HO_DrawLine)
			FArmyRuler::Get()->Show(false);
	//AxisRuler->SetAxisRulerDisplay(false,false,false)
	//	;

	GVC->ClearVisualizersForSelection();
	HydropowerDetail->ShowSelectedDetial(nullptr);
	FArmyMouseCaptureManager::Get()->SetSelectObject(nullptr);
	FArmyMouseCaptureManager::Get()->SetObjectType(OT_None);
}

void FArmyHydropowerModeController::OnSelectionChanged(UObject* NewSelectionObject)
{
	USelection* Selection = GXREditor->GetSelectedActors();

	if (!IsCurrentModel)
		return;
	if (HO_DrawLine == GetCurrentOperationID())
	{
		HydropowerDetail->ShowSelectedDetial(nullptr);
		return;
	}

	if (HO_SwitchConnect == GetCurrentOperationID())
	{

		//判断是不是在选择物体，而不是取消选择
		if (Selection->IsSelected(NewSelectionObject))
		{
			auto SCOperation = StaticCastSharedPtr<FArmySwitchConnectOperation>(CurrentOperation);
			if (SCOperation.IsValid())
			{
				SCOperation->OnSelectedSomething(NewSelectionObject);
			}
		}
	}
	GVC->ClearVisualizersForSelection();
	GVC->GetModeTools()->SetWidgetMode(FArmyWidget::EWidgetMode::WM_Translate);
	if (NewSelectionObject == Selection)
	{
		TSharedPtr<FArmyObject> ObjectFirst;
		TArray<AActor*> OutActors;
		GXREditor->GetSelectedActors(OutActors);

		if (OutActors.Num() <= 0)
		{
			FArmyRuler::Get()->Show(false);
//			FirstSelectedActor = nullptr;
			return;
		}
	//	FirstSelectedActor = OutActors[0];

		int32 index = 0;
		for (int32 i = 0; i < OutActors.Num(); i++)
		{
			AActor* Actor = OutActors[i];
			AXRShapeActor* ActorLine = Cast<AXRShapeActor>(Actor);//GXREditor->GetSelectedActors()->GetBottom<AXRActor> ();
			AActor* SelectActor = Actor;//GXREditor->GetSelectedActors ()->GetBottom<AActor> ();
			if (ActorLine) // 选中的如果是管线和管线上点
			{
				TSharedPtr<FArmyObject> Object = ActorLine->GetRelevanceObject().Pin();

				if (GetCurrentOperationID() != HO_DrawLine)
					//AxisRuler->SetAxisRulerDisplay(false, false, false)
					FArmyRuler::Get()->Show(false);
				//;
				if (index == 0)
					ObjectFirst = Object;
				if (Object.IsValid())
				{
					index++;
					GVC->AddVisualizersForSelection(Object);
					HydropowerDetail->ShowSelectedDetial(ObjectFirst);
					FArmyMouseCaptureManager::Get()->SetEnable(true);
					FArmyMouseCaptureManager::Get()->SetObjectType(ObjectFirst->GetType());
					FArmyMouseCaptureManager::Get()->SetSelectObject(ObjectFirst);
				}
			}
			else if (SelectActor)//普通模型
			{
				AXRActor * ParentActor = Cast<AXRActor>(SelectActor->GetOwner());


				if (ParentActor)
				{
					if (GetCurrentOperationID() == 0)
					{
						//@郭子阳 停用的点位 或者启用但不断点改的 不显示标尺
						bool  CanMove = false;
						bool  CanBPreform = false;
						bool  IsNativePoint = false;
						TSharedPtr<FArmyObject> Object = ParentActor->GetRelevanceObject().Pin();
						if (Object.IsValid())
						{
							auto furniture = StaticCastSharedPtr<FArmyFurniture>(Object);
							CanBPreform = furniture->CanNativeBPReform();
							CanMove = furniture->CanBPMove();
							IsNativePoint = furniture->IsOrignalPoint();
						}

						if (GetCurrentOperationID() != HO_DrawLine
							&& ((CanMove &&CanBPreform) || !CanBPreform)
							&&!((!CanBPreform) &&  IsNativePoint) //入户水点位是原始点位但没有原始点位断点改造
							)
						{
							FArmyRuler::Get()->Show(true);
						}
						else
						{
							FArmyRuler::Get()->Show(false);
						}

					}
					TSharedPtr<FArmyObject> Object = ParentActor->GetRelevanceObject().Pin();
					if (Object.IsValid() && ParentActor->IsA(AXRFurnitureActor::StaticClass()))
						HydropowerDetail->ShowSelectedDetial(SelectActor);

				}
				else
				{
					FArmyRuler::Get()->Show(false);
				}
			}
		}


	}


}

void FArmyHydropowerModeController::OnDeleteSelectionActor(AActor* Actor)
{
	TSharedPtr<FArmyPipelineOperation> PipeLineOperation = StaticCastSharedPtr<FArmyPipelineOperation>(OperationMap[HO_DrawLine]);
	PipeLineOperation->OnDeleteSelectionActor(Actor);
//	FirstSelectedActor = nullptr;
}

void FArmyHydropowerModeController::OnCopySelectionActorToNew(AActor* OldActor, AActor* NewActor)
{
}

void FArmyHydropowerModeController::OnLevelActorsAdded(AActor* InActor)
{

}

void FArmyHydropowerModeController::On3DTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes, AActor* InActor, EModeIdent InModeIdent)
{
	if (InModeIdent == EModeIdent::MI_HydropowerMode)
	{
		ThreeTo2DGanged(InRes, InActor);
		//	AxisRuler->SetActor(InActor);
	}
}
void FArmyHydropowerModeController::CreateObjForMeshActor(FContentItemPtr ContentItem, AActor* InActor, TSharedPtr<FArmyObject> Object , EModeIdent Mode)
{
	if (Mode != EModeIdent::MI_HydropowerMode
		|| (!(ContentItem.IsValid()&&( !ContentItem->GetComponent().IsValid())))
		)
	{
		return;
	}

	TSharedPtr<FArmyFurniture> EmptyFurniture = nullptr;// StaticCastSharedPtr<FArmyFurniture>(Object);
	if (!Object.IsValid())
	{
		EmptyFurniture = MakeShared<FArmyFurniture>();
	}
	else
	{
		EmptyFurniture= StaticCastSharedPtr<FArmyFurniture>(Object);
	}

	InActor->Tags.Add(XRActorTag::IgnoreCollision);
	InActor->Tags.Add("HydropowerActor");
	InActor->OnDestroyed.AddUniqueDynamic(G3DM, &UXR3DManager::OnOwnerDestroyed);
	InActor->GetRootComponent()->TransformUpdated.AddRaw(this, &FArmyHydropowerModeController::GangedTransformUpdated);

	EComponentID ComponentID = FArmyCommonTools::GetDefaultComponentLocalIDByCategoryID(ContentItem->codeStrId);

	//uint32 ObjectType;
	//FArmyCommonTools::ParseComponentToObjectType(ComponentID, ObjectType);

	EmptyFurniture->SetType(OT_Component_None);
	EmptyFurniture->SetPropertyFlag(FArmyObject::FLAG_POINTOBJ,true);
	EmptyFurniture->ComponentType = ComponentID;

	FVector Pos = InActor->GetActorLocation();
	float Hight = Pos.Z;
	Pos.Z = 0;

	EmptyFurniture->SetTransform(FTransform(InActor->GetActorRotation(), Pos, InActor->GetActorScale3D()));
	//@郭子阳 将新生成的构件放在拆改后模式下，以防止替换后原始点位在拆改后消失问题 //E_HydropowerModel  //E_LayoutModel
	FArmySceneData::Get()->Add(EmptyFurniture, XRArgument(1).ArgUint32(E_LayoutModel), nullptr, !EmptyFurniture.IsValid());
	EmptyFurniture->SetAltitude(Hight);

	G3DM->AssociatedObjectAndActor(EmptyFurniture, InActor, TEXT("HydropowerActor"));


		//// 添加cad绘制
		//FArmyAutoCad::Get()->AddComponent(CurrentObj->ComponentType, CurrentObj);
}

void FArmyHydropowerModeController::ThreeTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes, AActor* InActor, TSharedPtr<FArmyObject> Object)
{
	TArray<TSharedPtr<FArmyFurniture>> ObjectLayers;
	if (Object.IsValid())
	{
		ObjectLayers.Add(StaticCastSharedPtr<FArmyFurniture>(Object));
	}
	else
	{
		ObjectLayers = FArmyToolsModule::ParseDXF(InRes->FilePath);
	}

	{

		EXRViewMode CurrentViewMode = GXRPC->GetXRViewMode();
		//只有插座和开关需要平面视图的平面模型
		if (!TopViewIgnoreObjectArray.Contains(InRes->ComponentID))
		{
			switch (InRes->TypeID)
			{
			case Class_Switch:
			case Class_Socket:
			case Class_ElectricBoxH:
			case Class_ElectricBoxL:
			case Class_WaterRoutePoint:
			case Class_HotWaterPoint://热水点位
			case Class_HotAndColdWaterPoint://冷热水点位
			case Class_ColdWaterPoint://冷水点位
			case Class_FloordRainPoint://地漏下水
			case Class_ClosestoolRainPoint://马桶下水
			case Class_WashBasinRainPoint://水盆下水
			case Class_RecycledWaterPoint://中水点位
			case Class_HomeEntryWaterPoint://进户水
			{
				AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(InActor);
				if (StaticMeshActor)
				{
					bool bCreate = G3DM->CreateSurfaceComponent(InActor, InActor->GetActorLocation().Z);
					if (bCreate)
					{
						for (auto& ItemComp : InActor->GetComponents())
						{
							if (ItemComp->ComponentHasTag(TEXT("Transverse")))
							{
								UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(ItemComp);
								StaticMeshComp->SetHiddenInGame(CurrentViewMode != EXRView_TOP);
							}
						}
					}
				}
			}
			default:
				break;
			}
		}

		InActor->Tags.Add(XRActorTag::IgnoreCollision);
		InActor->OnDestroyed.AddUniqueDynamic(G3DM, &UXR3DManager::OnOwnerDestroyed);
		InActor->GetRootComponent()->TransformUpdated.AddRaw(this, &FArmyHydropowerModeController::GangedTransformUpdated);
	}
	if (ObjectLayers.Num() > 0)
	{
		TSharedPtr<FArmyFurniture>CurrentObj = ObjectLayers[0];
		uint32 ObjectType;
		FArmyCommonTools::ParseComponentToObjectType(InRes->ComponentID, ObjectType);

		// 添加正视图图例
		TSharedPtr<FResObj> FrontResObj = InRes->MapResObj.FindRef(E_Dxf_Front);
		if (FrontResObj.IsValid())
		{
			TSharedPtr<FArmyFurniture> FrontFurniture = FArmyToolsModule::GetFurnitureByDXF(FrontResObj->FilePath);
			if (FrontFurniture.IsValid())
			{
				CurrentObj->GetFurniturePro()->AddItemLegend(E_Dxf_Front, FrontFurniture);
			}
		}
		
		if (!Object.IsValid())
		{
			CurrentObj->SetType(OT_ComponentBase);
			CurrentObj->BelongClass = InRes->TypeID;
			CurrentObj->ComponentType = InRes->ComponentID;
			CurrentObj->Altitudes.Empty();
			CurrentObj->Altitudes.Append(InRes->Altitudes);
			CurrentObj->SwitchCoupletNum = InRes->SwitchCoupletNum;
		}

		FVector Pos = InActor->GetActorLocation();
		float Hight = Pos.Z;
		Pos.Z = 0;

		CurrentObj->SetTransform(FTransform(InActor->GetActorRotation(), Pos, InActor->GetActorScale3D()));
		//@郭子阳 将新生成的构件放在拆改后模式下，以防止替换后原始点位在拆改后消失问题 //E_HydropowerModel  //E_LayoutModel
		FArmySceneData::Get()->Add(CurrentObj, XRArgument(1).ArgUint32(E_LayoutModel), nullptr, !Object.IsValid());

		G3DM->AssociatedObjectAndActor(CurrentObj, InActor, TEXT("HydropowerActor"));

		CurrentObj->SetAltitude(Hight);

		// 添加cad绘制
		FArmyAutoCad::Get()->AddComponent(CurrentObj->ComponentType, CurrentObj);
	}
}


void FArmyHydropowerModeController::GangedTransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	EXRViewMode CurrentViewMode = GXRPC->GetXRViewMode();
	AActor* TheActor = InRootComponent->GetAttachmentRootActor();

	FVector Pos = TheActor->GetActorLocation();
	float Height = Pos.Z;
	Pos.Z = 0;
	AActor* TempActor = TheActor->GetOwner();
	AXRFurnitureActor* Owner = Cast<AXRFurnitureActor>(TempActor);

	if (Owner)
	{
		FVector WallNormal = Owner->GetNormal();
		FObjectWeakPtr RObj = Owner->GetRelevanceObject();
		if (RObj.IsValid())
		{
			TSharedPtr<FArmyFurniture> ComponentObj = StaticCastSharedPtr<FArmyFurniture>(RObj.Pin());
			/* @梁晓菲 图纸模式下，点位图例不应该跟着实际模型大小走，软装类控件跟着实际模型走*/
			if (ComponentObj->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
			{
				ComponentObj->SetTransform(FTransform(TheActor->GetActorRotation(), Pos, FVector(1, 1, 1)));
			}
			else
			{
				ComponentObj->SetTransform(FTransform(TheActor->GetActorRotation(), Pos, TheActor->GetActorScale3D()));
			}
			ComponentObj->SetAltitude(Height);
		}
	}
}

void FArmyHydropowerModeController::ProjectActorsIntoWorld(const TArray<AActor*>& Actors)
{
	FVector WallNormal = FArmyMouseCaptureManager::Get()->GetCaptureNormal();
	PlaceHydropowerActorIntoWorld(Actors, WallNormal);
}

void FArmyHydropowerModeController::PlaceHydropowerActorIntoWorld(const TArray<AActor*>& Actors, FVector WallNormal)
{
	if (WallNormal.ContainsNaN())
		return;

	if (Actors.Num() == 1)
	{
		if (!Actors[0]->IsA(AStaticMeshActor::StaticClass()))
			return;
		if (!Actors[0]->Tags.Contains("HydropowerActor"))
			return;
		AActor* TempActor = Actors[0]->GetOwner();

		FVector ThePos = Actors[0]->GetActorLocation();
		FVector Pos = ThePos;
		AXRFurnitureActor* FurnitureActor = Cast<AXRFurnitureActor>(TempActor);
		if (FurnitureActor && FurnitureActor->IsValidLowLevel())
		{
			FurnitureActor->SetNormal(WallNormal);
			if (bViewTOP)
			{
				FObjectWeakPtr RObj = FurnitureActor->GetRelevanceObject();
				TSharedPtr<FArmyFurniture> ComponentObj = StaticCastSharedPtr<FArmyFurniture>(RObj.Pin());
				int32 Altitude = ComponentObj->Altitudes.Num() ? ComponentObj->Altitudes[0] : 0.f;
				Pos.Z = Altitude / 10.f;
			}
		}
		Actors[0]->SetActorRelativeLocation(Pos);
	}
	else
	{
	}

	if (Actors.Num() == 1)
	{
		if (!Actors[0]->IsA(AStaticMeshActor::StaticClass()))
			return;
		if (!Actors[0]->Tags.Contains("HydropowerActor"))
			return;

		//请求施工项
		for (auto Actor : Actors)
		{
			RequestConstructionList(Actor);
		}
	}
}

void FArmyHydropowerModeController::AddToScene()
{
	TSharedPtr<FContentItemSpace::FContentItem> ContentItem = LeftPanel->GetSelectedContentItem();
	if (ContentItem.IsValid() && IsCurrentModel)
	{
		TArray<TSharedPtr<FContentItemSpace::FResObj> >resArr = ContentItem->GetResObjNoComponent();
		if (resArr.Num() < 1)
			return;
		TSharedPtr<FArmyPipeRes> Res = StaticCastSharedPtr<FArmyPipeRes>(resArr[0]);
		if (Res->Categry0Type == HO_Component)
		{
			if (GVC->DroppedObjects.Num() == 0)
			{
				FVector Location = FArmyMouseCaptureManager::Get()->GetCapturePoint();
				LeftPanel->AddToScene(ContentItem, Location);
			}
		}
	}
}

void FArmyHydropowerModeController::OnSaveCommand()
{
	FArmyFrameCommands::OnMenuSave();
}

void FArmyHydropowerModeController::OnUndoCommand()
{
	FArmyFrameCommands::OnMenuUndo();
}

void FArmyHydropowerModeController::OnChangeToSwtichConnectCommand(int32 bEnter)
{
	if (bEnter)
		SetOperation(HO_SwitchConnect);
	else
		EndOperation();
}

void FArmyHydropowerModeController::OnRedoCommand()
{
	FArmyFrameCommands::OnMenuRedo();
}

void FArmyHydropowerModeController::OnEmptyCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Clear));
}

bool FArmyHydropowerModeController::IsSwitchConnect()
{
	if (GGI->DesignEditor.IsValid() && GGI->DesignEditor->GetCurrentDesignMode() == DesignMode::HydropowerMode)
	{
		return GetCurrentOperationID() == HO_SwitchConnect;
	}
	return false;
}

bool FArmyHydropowerModeController::CanVisibleSwitchConnect()
{
	if (GGI->DesignEditor.IsValid() && GGI->DesignEditor->GetCurrentDesignMode() == DesignMode::HydropowerMode)
	{
		return GetCurrentOperationID() != HO_SwitchConnect;
	}
	return false;
}

void FArmyHydropowerModeController::OnEmptyStrongCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空强电线路？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Empty, (int32)FArmyHydropowerModeController::EE_Strong));
}

void FArmyHydropowerModeController::OnEmptyLightCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空照明线路？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Empty, (int32)FArmyHydropowerModeController::EE_Light));
}

void FArmyHydropowerModeController::OnEmptyWeakCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空弱电线路？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Empty, (int32)FArmyHydropowerModeController::EE_Weak));
}

void FArmyHydropowerModeController::OnEmptyColdWaterCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空冷水管线路？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Empty, (int32)FArmyHydropowerModeController::EE_ColdWater));
}

void FArmyHydropowerModeController::OnEmptyHotWaterCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空热水管线路？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Empty, (int32)FArmyHydropowerModeController::EE_HotWater));
}

void FArmyHydropowerModeController::OnEmptyDrainCommand()
{
	GGI->Window->PresentModalDialog(TEXT("是否清空排水管线路？"), FSimpleDelegate::CreateRaw(this, &FArmyHydropowerModeController::Empty, (int32)FArmyHydropowerModeController::EE_Drain));
}

void FArmyHydropowerModeController::OnDeleteCommand()
{
	Delete();
}

void FArmyHydropowerModeController::OnEnterPowerSystem()
{
	PowerSystemManager->ShowUI();
	// 初始化配电箱自动生成回路的样本回路数据 2019.5.9
	FArmyHydropowerDataManager::Get()->InitPowerSystemLoopSamples();
}

void FArmyHydropowerModeController::OnDesignPipeLine()
{

	TArray<TWeakPtr<FArmyObject>> Objectes;
	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_StrongElectricity_25, Objectes);
	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_StrongElectricity_Single, Objectes);
	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_StrongElectricity_4, Objectes);
	FArmySceneData::Get()->GetObjects(E_HydropowerModel, OT_StrongElectricity_Double, Objectes);
	auto func = [&]()
	{
		this->Empty(EE_Strong);
		this->Empty(EE_Light);
		FArmyAutoDesignPipeLine::GetInstance()->Empty();
		FArmyAutoDesignPipeLine::GetInstance()->AutoDesign(PowerSystemManager->GetCurrentPowerSystem());
	};
	if (Objectes.Num())
	{
		GGI->Window->PresentModalDialog(
			TEXT("智能布线会清空之前布置的管线，是否继续"),
			FSimpleDelegate::CreateLambda(func)
			, false
		);
	}
	else
	{
		func();
	}


}

void FArmyHydropowerModeController::OnReplaceCommand()
{

}

void FArmyHydropowerModeController::OnCopyCommand()
{

}

void FArmyHydropowerModeController::BeginSwitchOperation(bool bWasSucceed)
{
	if (bWasSucceed)
	{

		FArmyRuler::Get()->Show(false);

		MyWidgetSwitcher->SetActiveWidgetIndex(1);
	}
	else
	{
		MyWidgetSwitcher->SetActiveWidgetIndex(0);
	}
}

void FArmyHydropowerModeController::OnAutoDesignPoint()
{
	FArmyAutoDesignPoint::GetInstance()->AutoDesign();
}

void FArmyHydropowerModeController::OnViewModeChanged(int32 InNewValue, int32 InOldValue)
{
	//2d构件有没有清除
	static bool Mesh2DCleared = true;

	static auto Hide2DMesh = [](bool NewHidden) { 
		for (TActorIterator<AStaticMeshActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
		{
			AStaticMeshActor* Actor = *ActorItr;
			for (auto& Component : Actor->GetComponents())
			{
				UStaticMeshComponent* StaticComp = Cast<UStaticMeshComponent>(Component);
				if (StaticComp&&StaticComp->IsValidLowLevel())
				{
					bool bTransverse = StaticComp->GetName().Equals("Transverse");
					if (bTransverse)
					{
						StaticComp->SetHiddenInGame(NewHidden);
					}
				}
			}
		}
	};

	if (IsCurrentModel)
	{
		TSharedPtr<IArmyActorVisitor> TopModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TopHydropowerMode);

		if (InNewValue == EXRView_TOP)
		{
			bViewTOP = true;
			FArmyMouseCaptureManager::Get()->SetTopView(true);
			TopModeAV->Show(true);
		}
		else
		{
			bViewTOP = false;
			FArmyMouseCaptureManager::Get()->SetTopView(false);
			TopModeAV->Show(false);

			TSharedPtr<IArmyActorVisitor> HydropowerModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HydropowerMode);
			HydropowerModeAV->Show(true);
		}
		bool bHiddenInGame = InNewValue != EXRView_TOP;
		Hide2DMesh(bHiddenInGame);
		Mesh2DCleared = bHiddenInGame;
	}
	else
	{
		//@郭子阳
		//清楚2d构件
		if(!Mesh2DCleared)
		{
			Hide2DMesh(true);
			Mesh2DCleared = true;
		}

	}
}

TSharedRef<SWidget> FArmyHydropowerModeController::CreateLayerOutliner()
{
	HydropowerDetail = MakeShareable(new FArmyHydropowerDetail);

	return
		SNew(SSplitter)
		.Orientation(Orient_Vertical)

		+ SSplitter::Slot()
		.Value(TAttribute<float>(this, &FArmyHydropowerModeController::GetOutlinerWidgetHeight))
		.OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([this](float InValue) {
		OutlinerWidgetHeight = FMath::Clamp<float>(InValue, 112.f / 1012.f, 900.f / 1012.f);
	}))
		[
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
			.Text(FText::FromString(TEXT("物体列表")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		]
		]

	+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			GGI->DesignEditor->WorldOutliner.ToSharedRef()
		]
		]

	+ SSplitter::Slot()
		.Value(TAttribute<float>(this, &FArmyHydropowerModeController::GetPropertyWidgetHeight))
		.OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([this](float InValue) {
		PropertyWidgetHeight = FMath::Clamp<float>(InValue, 112.f / 1012.f, 900.f / 1012.f);
	}))
		[
			SAssignNew(MyWidgetSwitcher, SWidgetSwitcher)

			+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			CreateDetail()
		]

	+ SWidgetSwitcher::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		[
			CreateSwitchConnect()
		]
		];
}

TSharedRef<SWidget> FArmyHydropowerModeController::CreateDetail()
{
	SAssignNew(LayerOutliner1, SVerticalBox)
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
			.Text(FText::FromString(TEXT("属性")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		]

		]
	+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0, 0, 16, 0))
		[
			HydropowerDetail->CreateDetailWidget()
		];
	return LayerOutliner1.ToSharedRef();
}

TSharedRef<SWidget> FArmyHydropowerModeController::CreateSwitchConnect()
{
	HydropowerSwitchConnect = MakeShareable(new FArmyHydropowerSwitchConnect);
	SAssignNew(LayerOutliner2, SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		[

			SNew(SBox)
			.HeightOverride(32)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF212224"))
		.Padding(FMargin(16, 0, 0, 0))
		.VAlign(VAlign_Center)
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("灯控设置")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		]
		]

		]
	+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		.Padding(FMargin(0, 0, 16, 0))
		[
			HydropowerSwitchConnect->CreateDetailWidget()
		];
	return LayerOutliner2.ToSharedRef();

}

float FArmyHydropowerModeController::GetOutlinerWidgetHeight() const
{
	if (HydropowerDetail->GetDetailWidget()->GetVisibility() == EVisibility::Collapsed ||
		HydropowerDetail->GetDetailWidget()->GetVisibility() == EVisibility::Hidden)
	{
		return 1.f;
	}

	return OutlinerWidgetHeight;
}

float FArmyHydropowerModeController::GetPropertyWidgetHeight() const
{
	if (HydropowerDetail->GetDetailWidget()->GetVisibility() == EVisibility::Collapsed ||
		HydropowerDetail->GetDetailWidget()->GetVisibility() == EVisibility::Hidden)
	{
		return 0.f;
	}

	return PropertyWidgetHeight;
}

int32 FArmyHydropowerModeController::GetCurrentOperationID()
{
	auto key = OperationMap.FindKey(CurrentOperation);
	if (key)
	{
		return *key;
	}
	return 0;
}

void FArmyHydropowerModeController::SetLightsVisibility(EVisibility LightVisible)
{
	bool bLightVisible = (LightVisible == EVisibility::Visible) ? true : false;

	for (FActorIterator ActorIt(GVC->GetWorld()); ActorIt; ++ActorIt)
	{
		if (ActorIt->IsA(AXRLightActor::StaticClass()))
		{
			AXRLightActor* LightActor = Cast<AXRLightActor>(*ActorIt);
			LightActor->ShowLightIcon(bLightVisible);
		}
		else if (ActorIt->IsA(AXRReflectionCaptureActor::StaticClass()))
		{
			AXRReflectionCaptureActor* ReflectionCaptureActor = Cast<AXRReflectionCaptureActor>(*ActorIt);
			ReflectionCaptureActor->ShowRCIcon(bLightVisible);
		}
	}
}

/*替换操作
* @NewContentItem 新的商品
* @ ReplacedFurniture 被替换的构件
*/
void FArmyHydropowerModeController::Replace(FContentItemPtr NewContentItem, TSharedPtr<FArmyFurniture> ReplacedFurniture)
{
	//生成actor
	AActor * TempActor = Cast<AActor>(
		LeftPanel->AddToScene(NewContentItem
			, ReplacedFurniture->GetRelevanceActor()->GetActorLocation()
			, ReplacedFurniture->GetRelevanceActor()->GetActorRotation()
			, FVector(1)
			, true
		));

	FArmyFComponentResDelegate::CreateRaw(this, &FArmyHydropowerModeController::On3DTo2DGanged);
	AXRFurnitureActor* Actor = Cast<AXRFurnitureActor>(TempActor->GetOwner());
	TSharedPtr<FArmyFurniture> Newfurniture = StaticCastSharedPtr<FArmyFurniture>(Actor->GetRelevanceObject().Pin());

	if (!Newfurniture->OnFurnitureActorShowUpdated.IsBound())
	{
		Newfurniture->OnFurnitureActorShowUpdated.BindRaw(this, &FArmyHydropowerModeController::OnFurnitureActorShowUpdated);
	}

	if (!ReplacedFurniture->OnFurnitureActorShowUpdated.IsBound())
	{
		ReplacedFurniture->OnFurnitureActorShowUpdated.BindRaw(this, &FArmyHydropowerModeController::OnFurnitureActorShowUpdated);
	}

	//替换构件之间的联系
	auto UselessFurniture = ReplacedFurniture->GetRepresentedFurniture(true)->Replace(Newfurniture);


	//删除多余的构件
	if (UselessFurniture.IsValid())
	{
		UselessFurniture.Pin()->GetRelevanceActor()->Destroy();
	}
}

//停用点位
void FArmyHydropowerModeController::StopOriginalPoint(TSharedPtr<FArmyFurniture> StoppedFurniture)
{
	if (!StoppedFurniture->OnFurnitureActorShowUpdated.IsBound())
	{
		StoppedFurniture->OnFurnitureActorShowUpdated.BindRaw(this, &FArmyHydropowerModeController::OnFurnitureActorShowUpdated);
	}

	StoppedFurniture->GetRepresentedFurniture(true)->SetBPStop(true);
}

void FArmyHydropowerModeController::StartOriginalPoint(TSharedPtr<FArmyFurniture> StoppedFurniture)
{
	if (!StoppedFurniture->OnFurnitureActorShowUpdated.IsBound())
	{
		StoppedFurniture->OnFurnitureActorShowUpdated.BindRaw(this, &FArmyHydropowerModeController::OnFurnitureActorShowUpdated);
	}
	StoppedFurniture->GetRepresentedFurniture(true)->SetBPStop(false);
}

void FArmyHydropowerModeController::ApplyOffset(TSharedPtr<FArmyFurniture> OffsetFurniture, FVector offset)
{
	OffsetFurniture->SetOffset(offset);
}

void FArmyHydropowerModeController::StartBPReform(TSharedPtr<FArmyFurniture> StoppedFurniture)
{
	StoppedFurniture->StartReforming();
	FArmyRuler::Get()->Show(true);
}

void FArmyHydropowerModeController::StopBPReform(TSharedPtr<FArmyFurniture> StoppedFurniture)
{
	StoppedFurniture->StopReforming();
	FArmyRuler::Get()->Show(false);
}

void FArmyHydropowerModeController::OnFurnitureActorShowUpdated(TSharedRef<FArmyFurniture> Furniture, bool NewHide)
{
	Furniture->GetRelevanceActor()->SetActorHiddenInGame(NewHide);

	if (NewHide)
	{
		Furniture->GetRelevanceActor()->Tags.Add("HideInList");
	}
	else
	{
		Furniture->GetRelevanceActor()->Tags.Remove("HideInList");
	}
	auto Outliner = StaticCastSharedPtr<SArmySceneOutliner>(GGI->DesignEditor->WorldOutliner);
	Outliner->ShowOrHideLevelActor(Furniture->GetRelevanceActor(), NewHide);
	Outliner->Refresh(true);

	Furniture->GetRelevanceActor()->bIsSelectable = true;

	if (!NewHide)
	{//选中新构件actor
		HydropowerDetail->ShowSelectedDetial(Furniture->GetRelevanceActor());
	}
}

bool FArmyHydropowerModeController::RequestConstructionList(AActor * Actor)
{
	if (!Actor)
	{
		return false;
	}
	//之前是不是已经存储了这一类的施工项
	bool AlreadyHaveData = false;

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgr->GetContentItemFromID(Actor->GetSynID());
	FGuid ObjectID;
	ConstructionPatameters Parameters;
	//获取商品ID
	int32 GoodsID = -1;
	if (ActorItemInfo.IsValid())
	{
		GoodsID = ActorItemInfo->ID;
		AXRActor * parent = Cast<AXRActor>(Actor->GetOwner());
		if (parent != nullptr)
		{
			auto obj = parent->GetRelevanceObject().Pin();
			if (!obj.IsValid())
			{
				return false;
			}

			ObjectID = obj->GetUniqueID();

			auto FurnitureObj = StaticCastSharedPtr<FArmyFurniture>(obj);
			Parameters = FurnitureObj->GetConstructionParameter();
		} 
		else
		{
			return false;
		}
		
	}
	else
	{
		AXRActor * parent = Cast<AXRActor>(Actor);
		auto obj = parent->GetRelevanceObject().Pin();
		if (!obj.IsValid())
		{
			return false;
		}
		auto pipe = StaticCastSharedPtr<FArmyPipeline>(obj);

		if (pipe.IsValid())
		{
			GoodsID = pipe->ProductID;
			ObjectID = obj->GetUniqueID();
			Parameters.SetNormalGoodsInfo(GoodsID);
		}
		else
		{
			auto PipPoint = StaticCastSharedPtr<FArmyPipePoint>(obj);

			if (PipPoint.IsValid())
			{
				GoodsID = PipPoint->ProductID;
				ObjectID = obj->GetUniqueID();
				Parameters.SetNormalGoodsInfo(GoodsID);
			}
		}
	}

	if (GoodsID == -1)
	{
		return false;
	}
	
	XRConstructionManager::Get()->TryToFindConstructionData(ObjectID, Parameters, FConstructionDataDelegate::CreateRaw(this,&FArmyHydropowerModeController::ConstructionCallBack));
	return true;
}

bool FArmyHydropowerModeController::RequestConstructionList(TSharedPtr<FArmyFurniture> Furniture)
{
	XRConstructionManager::Get()->TryToFindConstructionData(Furniture->GetUniqueID(), Furniture->GetConstructionParameter(), FConstructionDataDelegate::CreateRaw(this, &FArmyHydropowerModeController::ConstructionCallBack));
	return true;
}



void FArmyHydropowerModeController::ConstructionCallBack(EConstructionRequeryResult ResultInfo, TSharedPtr<FJsonObject> ConstructionData, TSharedPtr<class FArmyConstructionItemInterface> CheckedData, ObjectConstructionKey Key)
{
	if (ResultInfo != EConstructionRequeryResult::Failed)
	{
		OnConstructionDataGot.ExecuteIfBound( ResultInfo,  ConstructionData, CheckedData,  Key);
	}
}


