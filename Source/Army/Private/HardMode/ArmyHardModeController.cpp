#include "ArmyHardModeController.h"
#include "ArmyPlayerController.h"
#include "SArmyHardModeContentBrowser.h"
#include "ArmyViewportClient.h"
#include "ArmyDesignEditor.h"
#include "ArmyReplaceTextureOperation.h"
#include "ArmyRectAreaOperation.h"
#include "ArmyCircleAreaOperation.h"
#include "ArmyRegularPolygonAreaOperation.h"
#include "ArmyBaseArea.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyFreePolygonArea.h"
#include "ArmyDetailBuilder.h"
#include "SWidgetSwitcher.h"
#include "ArmyHardModeDetail.h"
#include "ArmyCircleArea.h"
#include "ArmyRectArea.h"
#include "ArmyRegularPolygonArea.h"
#include "ArmyFrameCommands.h"
#include "ArmyExtrusionActor.h"
#include "ArmyPointLightActor.h"
#include "ArmySpotLightActor.h"
#include "ArmyReflectionCaptureActor.h"
#include "ArmyActorItem.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "PhysicsEngine/BodySetup.h"
#include "ArmyGameInstance.h"
#include "ArmyGroupActor.h"
#include "ArmyShapeBoardActor.h"
#include "ArmyWallActor.h"
#include "ArmyHardModeCommands.h"
#include "ArmyEditorEngine.h"
#include "ArmyHardModeFloorController.h"
#include "ArmyHardModeWallController.h"
#include "ArmyHardModeCeilingController.h"
#include "ArmyUser.h"
#include "ArmyToolBarBuilder.h"
#include "EngineUtils.h"
#include "ArmyResourceModule.h"
#include "ArmyGlobalActionCallBack.h"
#include "JsonResourceParser.h"
#include "Army3DManager.h"
#include "ArmyAxisRulerActor.h"
#include "ArmyActorVisitorFactory.h"
#include "ArmyActorConstant.h"
#include "ArmyDesignPackage.h"
#include "ArmyPackageDownloader.h"
#include "ArmyAutoDesignModel.h"
//#include "AutoDesignEngine.h"
#include "ArmyAutoAccessory.h"
#include "ArmyHttpModule.h"
#include "ArmyRoomEntity.h"
#include "ArmyStaticLighting/Public/StaticLightingPrivate.h"
#include "Engine/PostProcessVolume.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SphereReflectionCapture.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "SArmyStaticLightingSettings.h"
#include "SArmyCategoryOrganizationManage.h"
#include "SArmyCategoryOrganization.h"
#include "SArmyMasterPlanContentBrowser.h"
#include "ArmyApplyMasterPlanManage.h"
#include "ArmyAutoCad.h"
#include "ComponentReregisterContext.h"
#include "ArmyEngineTools.h"
#include "ArmyWorldManager.h"
#include "ArmyMaterialBrushOperation.h"
#include "ArmyHardModeCommonData.h"
FArmyHardModeController::~FArmyHardModeController()
{
	FArmySceneData::Get()->PreOperationDelegate.RemoveAll(this);
	USelection::SelectObjectEvent.RemoveAll(this);
	HightLightActor->Destroy();
	HightLightActor = NULL;

}

const EModelType FArmyHardModeController::GetDesignModelType()
{
	return EModelType::E_HardModel;
}

void FArmyHardModeController::Init()
{
	if (!HightLightActor)
	{
		FActorSpawnParameters ActorParam;
		ActorParam.Name = FName(TEXT("NOLIST-1"));

		HightLightActor = GVC->GetWorld()->SpawnActor<AXRShapeActor>(AXRShapeActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, ActorParam);
	}
	MI_BackGroundColor = UMaterialInstanceDynamic::Create(FArmyEngineModule::Get().GetEngineResource()->GetDefaultHightLightMaterial(), NULL);
	MI_BackGroundColor->AddToRoot();
	MI_BackGroundColor->SetVectorParameterValue("MainColor", FLinearColor::White);
	MI_BackGroundColor->SetScalarParameterValue("Alpha", 0.0f);
	HightLightActor->SetMaterial(MI_BackGroundColor);
	HightLightActor->Tags.Add(TEXT("AreaHighLight"));
	HightLightActor->SetActorHiddenInGame(true);
	FArmySceneData::Get()->PreOperationDelegate.AddRaw(this, &FArmyHardModeController::OnObjectOperation);

	// 注册命令代理
	FArmyHardModeCommands::Register();

	// 绑定代理
	GXRPC->OnViewChangedDelegate.AddRaw(this, &FArmyHardModeController::OnViewModeChanged);
	/**@欧石楠 增加选空*/
	USelection::SelectNoneEvent.AddRaw(this, &FArmyHardModeController::OnSelectionNone);
	USelection::SelectObjectEvent.AddRaw(this, &FArmyHardModeController::OnSelectionChanged);
	GXREditor->OnActorMoved().AddRaw(this, &FArmyHardModeController::OnActorMoved);
	GXREditor->RightSelectedEvent.BindRaw(this, &FArmyHardModeController::OnRightSelected);

	FArmyGlobalActionCallBack::Get().OnGlobalAction.AddRaw(this, &FArmyHardModeController::OnGloabalAction);

	FArmyDesignModeController::Init();

	HardModeFloorController = NewVC<FArmyHardModeFloorController>();
	HardModeWallController = NewVC<FArmyHardModeWallController>();
	HardModeCeilingController = NewVC<FArmyHardModeCeilingController>();
	HardModeFloorController->FreeViewModeCallback.BindRaw(this, &FArmyHardModeController::CameraReturnCallBack);
	HardModeWallController->FreeViewModeCallback.BindRaw(this, &FArmyHardModeController::CameraReturnCallBack);
	HardModeCeilingController->FreeViewModeCallback.BindRaw(this, &FArmyHardModeController::CameraReturnCallBack);
	HardModeFloorController->HightLightActor = HightLightActor;
	HardModeCeilingController->HightLightActor = HightLightActor;
	HardModeWallController->HightLightActor = HightLightActor;

	HardModeFloorController->PostInit();
	HardModeCeilingController->PostInit();
	HardModeWallController->PostInit();

	ActorAxisRuler = MakeShareable(new FArmyAxisRulerActor);
	ActorAxisRuler->Init(GVC->ViewportOverlayWidget);
	GVC->OnViewTypeChanged.AddRaw(ActorAxisRuler.Get(), &FArmyAxisRulerActor::OnViewTypeChanged);

	HardModeDetail->EditAreaDelegate.AddRaw(this, &FArmyHardModeController::LockCamera);

	//@马云龙
	HardModeGlobalCommandList = MakeShareable(new FUICommandList);
	HardModeGlobalCommandList->MapAction(FArmyHardModeCommands::Get().CommandInteractiveHovered, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnInteractiveHovered));
	CommandList->Append(HardModeGlobalCommandList.ToSharedRef());

	//FArmyUser::Get().OnConstructionWallDataReady.BindRaw(this, &FArmyHardModeController::OnConstructionWallReady);
	//FArmyUser::Get().OnConstructionTopDataReady.BindRaw(this, &FArmyHardModeController::OnConstructionRoofReady);
	//FArmyUser::Get().OnConstructionFloorDataReady.BindRaw(this, &FArmyHardModeController::OnConstructionFloorReady);
	//应用大师方案
	FArmyApplyMasterPlanManage::Get()->ApplayMasterPlanDelegate.BindRaw(this, &FArmyHardModeController::OnApplayMasterPlan);

	GGI->DesignEditor->AllControllersLoadCompleted.AddRaw(this, &FArmyHardModeController::AllControllersLoadCompleted);
	
	FArmySceneData::Get()->ChangedHomeDataDelegate.AddRaw(this, &FArmyHardModeController::OnHomeRebuild);//原始户型发生修改，需要更新

	DelayPostBeginModeTickCount = 0;

	FArmyWorldManager::Get().SetEnvironmentMode(true);
}

void FArmyHardModeController::InitOperations()
{
	TSharedPtr<FArmyReplaceTextureOperation> tempReplace = MakeShareable(new FArmyReplaceTextureOperation(E_HardModel));
	OperationMap.Add((uint8)EHardModeOperation::HO_Replace, tempReplace);
	

	{
		//extern TSharedPtr<FArmyMaterialBrushOperation> HardModCommonOperations::OP_MaterialBrush;
		//TSharedPtr<FArmyMaterialBrushOperation> Operation = MakeShared< FArmyMaterialBrushOperation>(E_HardModel);
		OperationMap.Add((uint8)EHardModeOperation::HO_MaterialBrush, HardModCommonOperations::OP_MaterialBrush);
	}

	//{

	//	TSharedPtr<FArmyMaterialBrushOperation> Operation = MakeShared< FArmyMaterialBrushOperation>(E_HardModel);
	//	OperationMap.Add((uint8)EHardModeOperation::HO_MaterialBrush, Operation);
	//}

	for (auto It : OperationMap)
	{
		It.Value->Init();
		It.Value->InitWidget(GVC->ViewportOverlayWidget);
		It.Value->EndOperationDelegate.BindRaw(this, &FArmyHardModeController::EndOperation);
	}

}

void FArmyHardModeController::BeginMode()
{
	FArmyDesignModeController::BeginMode();

	// @zengy 重置套餐
	LeftPanel->OnBeginMode();

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

	TSharedPtr<IArmyActorVisitor> HardModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardMode);
	HardModeAV->Show(true);

	FArmyToolsModule::Get().GetMouseCaptureTool()->Reset();
	FArmyToolsModule::Get().GetMouseCaptureTool()->SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = false;

	GVC->PlaceActorToWorldDelegate.BindSP(LeftPanel.Get(), &SArmyHardModeContentBrowser::OnPlaceActorToWorld);


	//进入立面模式，默认是未选中任何物体，此时属性面板不显示（光照烘焙成功情况下除外，要显示全局设置）
	if (!FStaticLightingManager::Get()->IsValidLightMap())
		HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);


	TArray<FObjectWeakPtr> TotalAreas;
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_CircleArea, TotalAreas);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_RectArea, TotalAreas);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_PolygonArea, TotalAreas);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_FreePolygonArea, TotalAreas);

    for (auto iter : TotalAreas)
    {
        TSharedPtr<FArmyBaseArea> room = StaticCastSharedPtr<FArmyBaseArea>(iter.Pin());
        if (room.IsValid())
        {
            if (room->GetLampSlotActor())
            {
                room->GetLampSlotActor()->SetActorHiddenInGame(true);
            }
        }
    }

	//设置此模块默认显示的目录数据
	static bool FirstDefaultSelectCat = true;
	if (FirstDefaultSelectCat)
	{
		CategoryOrgManage->SetDefaultSelectOrgination(SArmyCategoryOrganization::OZ_MasterPlan);
        CategoryOrgManage->GetCurrentSelectCat()->GetCurrentBrowser().Pin()->ReqRootContentItems();
		FirstDefaultSelectCat = false;
	}

	/* @梁晓菲 进入立面模式默认隐藏光源图标*/
	SetLightsVisibility(EVisibility::Hidden);

	/** @马云龙 每次进入该模式都会触发一次PostTick，用来延迟更新反射球 */
	DelayPostBeginModeTickCount = 0;

	/** @马云龙 水电模式自动切换模式 */
	FArmyWorldManager::Get().TryChangeEnvironmentModeConditionally(DesignMode::HardMode);

	////@郭子阳 自动进入材质刷
	//TSharedPtr<FArmyMaterialBrushOperation>	MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation>(*OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush));
	//if (MatBrush->GetRebeginAtStart())
	//{
	//	SetOperation((uint8)EHardModeOperation::HO_MaterialBrush);
	//}
}

bool FArmyHardModeController::EndMode()
{
	if (CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_Replace))
	{
		TSharedPtr<FArmyMaterialBrushOperation>	MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation>(*OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush));
		MatBrush->SetRebeginAtStart();
		CurrentOperation ->EndOperation();
	}

	if (CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush))
	{
		CurrentOperation->EndOperation();
	}

	if (CurrentEditMode == HO_FloorEdit)
	{
		HardModeFloorController->EndMode();
	}
	else if (CurrentEditMode == HO_WallEdit)
	{
		HardModeWallController->EndMode();
	}
	else if (CurrentEditMode == HO_RoofEdit)
	{
		HardModeCeilingController->EndMode();
	}

    TempInteractiveActor = nullptr;
	HardModeDetail->SetSelectObject(NULL);
	ActorAxisRuler->SetAxisVisible(FArmyAxisRulerActor::AXIS_ALL, false);

	FArmyDesignModeController::EndMode();

	return true;
}

void FArmyHardModeController::EndOperation()
{


	FArmyDesignModeController::EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Start();
}

TSharedPtr<SWidget> FArmyHardModeController::MakeLeftPanelWidget()
{
	CategoryOrgManage = SNew(SArmyCategoryOrganizationManage);

	//大师方案
	TSharedPtr<SArmyMasterPlanContentBrowser> MasterContentBrowser = SNew(SArmyMasterPlanContentBrowser);
	TSharedPtr<SArmyCategoryOrganization> MasterPlanWid = SNew(SArmyCategoryOrganization)
		.ButtonImage(FArmyStyle::Get().GetBrush("Icon.masterPlan"))
		.CurrentBrowser(MasterContentBrowser.ToSharedRef())
		.bRequestNoMenuData(true)
		.ECurrentOrganization(SArmyCategoryOrganization::EOragnization::OZ_MasterPlan);

	//请求目录数据
	//MasterPlanWid->RequestHardModeCategory();
	CategoryOrgManage->AddOrganization(MasterPlanWid);

	//企业素材
	LeftPanel = SNew(SArmyHardModeContentBrowser)
		//.CategoryUrl("/api/mode_menu/elevation")
		.SearchUrl("/api/bim_goods/query")
		.OnDelegate_3DTo2DGanged(FArmyFComponentResDelegate::CreateRaw(this, &FArmyHardModeController::On3DTo2DGanged))
		.ParentId(1);
	LeftPanel->SetSubMode(ESubMode::SM_None);
	//LeftPanel->RequestCategory();
	LeftPanel->ItemSelected.BindRaw(this, &FArmyHardModeController::ReplaceTextureOperation);
	LeftPanel->PlaceModelEvent.BindRaw(this, &FArmyHardModeController::EndOperation);

	TSharedPtr<SArmyCategoryOrganization> CompanyDataWid = SNew(SArmyCategoryOrganization)
		.CategoryUrl("/api/mode_menu/elevation")
		.ButtonImage(FArmyStyle::Get().GetBrush("Icon.companyCategry"))
		.CurrentBrowser(LeftPanel.ToSharedRef())
		.bResponseToSearch(true)
		.ECurrentOrganization(SArmyCategoryOrganization::EOragnization::OZ_CompanyData);
	
	//请求目录数据
	CompanyDataWid->RequestHardModeCategory();
	CategoryOrgManage->AddOrganization(CompanyDataWid);

	return CategoryOrgManage;

}

TSharedPtr<SWidget> FArmyHardModeController::MakeRightPanelWidget()
{
	HardModeDetail = MakeShareable(new FArmyHardModeDetail());

	return
		SNew(SOverlay)

		+ SOverlay::Slot()
		[
            SNew(SSplitter)
            .Orientation(Orient_Vertical)

            + SSplitter::Slot()
            .Value(TAttribute<float>(this, &FArmyHardModeController::GetOutlinerWidgetHeight))
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
		                    .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		                ]
		            ]
		        ]

	            + SVerticalBox::Slot()
		        .VAlign(VAlign_Fill)
                .FillHeight(1.f)
		        [
                    GGI->DesignEditor->WorldOutliner.ToSharedRef()
		        ]
            ]

            + SSplitter::Slot()
            .Value(TAttribute<float>(this, &FArmyHardModeController::GetPropertyWidgetHeight))
            .OnSlotResized(SSplitter::FOnSlotResized::CreateLambda([this](float InValue) {
                PropertyWidgetHeight = FMath::Clamp<float>(InValue, 112.f / 1012.f, 900.f / 1012.f);
            }))
            [
                HardModeDetail->CreateDetailWidget()
            ]
        ];
}

TSharedPtr<SWidget> FArmyHardModeController::MakeToolBarWidget()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"
	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	FCanExecuteAction CheckHasSelectedAnyObj = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedAnyObj);
	FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);
	FIsActionChecked IsOverallViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsOverallViewChecked);
	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandSave, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnSaveCommand));
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandUndo, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnUndoCommand), CheckCanUndo);
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandRedo, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnRedoCommand), CheckCanRedo);
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandDelete, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnDeleteCommand), CheckHasSelectedAnyObj);
	//ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandEmpty, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnEmptyPlanCommand));
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHardModeController::OnEmptyCommand), LOCTEXT("Empty", "清空"), FSlateIcon("ArmyStyle", "HardMode.CommandEmpty"));
	
	//ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHardModeController::OnMaterialBrushCommand), LOCTEXT("MaterialBrush", "材质刷"), FSlateIcon("ArmyStyle", "HardMode.CommandEmpty"));
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandMaterialBrush, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnMaterialBrushCommand));


	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHardModeController::OnLightCommand), LOCTEXT("Light", "光源"), FSlateIcon("ArmyStyle", "HardMode.OnLightCommand"));
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHardModeController::OnDisplayCommand), LOCTEXT("Display", "隐藏"), FSlateIcon("ArmyStyle", "HomeMode.CommandDisplay"));
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandBuildStaticLighting, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnBuildStaticLightingCommand));
	ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHardModeController::OnViewComboCommand), LOCTEXT("ViewCombo", "平面视图"), FSlateIcon("ArmyStyle", "HardMode.CommandViewCombo"));
	ToolBarBuidler->AddButton(FArmyHardModeCommands::Get().CommandHardOverallView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuOverallView), DefaultCanExec, IsOverallViewChecked);

#undef LOCTEXT_NAMESPACE

return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

TSharedPtr<SWidget> FArmyHardModeController::MakeSettingBarWidget()
{
	return nullptr;
}

void FArmyHardModeController::Draw(FViewport* InViewport, const FSceneView* InView, FCanvas* SceneCanvas)
{

}

void FArmyHardModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	ActorAxisRuler->Draw(PDI, View);
	FArmyDesignModeController::Draw(View, PDI);
	TArray<TWeakPtr<FArmyObject>> HardModels = FArmySceneData::Get()->GetObjects(E_HardModel);
	for (TWeakPtr<FArmyObject> iter : HardModels)
	{
		iter.Pin()->Draw(PDI, View);
	}
}

bool FArmyHardModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (Event == IE_DoubleClick&&Key == EKeys::LeftMouseButton)
	{
		if (!FArmyObject::GetDrawModel(MODE_TOPVIEW) && !FArmyObject::GetDrawModel(MODE_CEILING))
		{
			////保留当前的材质刷
			//if (CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_Replace)
			//	&& FormerOperation.IsValid() && FormerOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush)
			//	)
			//{
			//	TSharedPtr<FArmyMaterialBrushOperation>	MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation>(*OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush));
			//	MatBrush->SetRebeginAtStart();
			//}
			LockCameraToEditSurface();
		
		}
	}
	else if (Key == EKeys::Escape || Key == EKeys::RightMouseButton)
	{
		if (HightLightActor)
			HightLightActor->SetActorHiddenInGame(true);
		if (CurrentOperation.IsValid())
		{
			CurrentOperation->EndOperation();
		}
	}
	return FArmyDesignModeController::InputKey(Viewport, Key, Event);
}

void FArmyHardModeController::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	bool HasSetSelectedArea = false;

	if (HitProxy && Key == EKeys::LeftMouseButton)
	{
		if (HitProxy->Priority >= 0 && HitProxy->OrthoPriority >= 0)
		{
			if (HitProxy->IsA(HActor::StaticGetType()))
			{
				HActor* actorProxy = (HActor*)(HitProxy);
				AXRWallActor* wallActor = Cast<AXRWallActor>(actorProxy->Actor);
				AArmyExtrusionActor* ExtrusionActor = Cast<AArmyExtrusionActor>(actorProxy->Actor);
				
				if (wallActor)
				{
					SelectedArea = wallActor->AttachSurface;
					HasSetSelectedArea = true;
					FArmyDesignModeController::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
					HardModeDetail->SetSelectObject(wallActor->AttachSurface, wallActor->GetName().Contains("ORIGNALWALL"));
				}
				else if (ExtrusionActor)
				{
					FArmyDesignModeController::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
					HardModeDetail->ShowSelectedDetial(actorProxy->Actor);
				}
				else
				{
					HardModeDetail->ShowSelectedDetial(actorProxy->Actor);
					HightLightActor->SetActorHiddenInGame(true);
				}
			}
		}
	}
	else if (HitProxy && (Key == EKeys::RightMouseButton || Key == EKeys::Escape))
	{
		HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);
		GXREditor->SelectNone(true, true, false);
	}

	if (!HasSetSelectedArea)
	{
		SelectedArea = nullptr;
	}
}

void FArmyHardModeController::Tick(float DeltaSeconds)
{
	ActorAxisRuler->Update(GVC);
	FArmyDesignModeController::Tick(DeltaSeconds);

	TickTraceInteractiveActors();


	if (DelayPostBeginModeTickCount < 4)
	{
		DelayPostBeginModeTickCount++;
	}
	if (DelayPostBeginModeTickCount == 4)
	{
		DelayPostBeginModeTickCount++;
		DelayUpdate();
	}
}

void FArmyHardModeController::Clear()
{
	//加载新方案前清空当前方案数据
	Cleanup(true, true);
	//清空光照数据
	ClearStaticLighitngData();
	if (HardModeDetail.IsValid())
		HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);

}

void FArmyHardModeController::Load(TSharedPtr<FJsonObject> Data)
{
	Cleanup(false, true);

	//@打扮家 XRLightmass 临时 加载方案不生成任何墙体
	//return;
	if (!Data.IsValid())
	{
		return;
	}

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
	ResMgr->CurWorld = GVC->GetWorld();

	FArmySceneData::Get()->LoadDataFromJason = true;
	TSharedPtr<FJsonObject> HardModeData;
	if (Data.IsValid())
	{
		HardModeData = Data->GetObjectField("hardMode");

		const TArray<TSharedPtr<FJsonValue>> HardModeDataRoom = HardModeData->GetArrayField(TEXT("hardMode_RoomSpaces"));
		TArray<TSharedPtr<FArmyRoomSpaceArea>> TRoomSpaceArea;
		for (auto& It : HardModeDataRoom)
		{
			TSharedPtr<FArmyRoomSpaceArea> ResultRoomArea = MakeShareable(new FArmyRoomSpaceArea());
			ResultRoomArea->Deserialization(It->AsObject());
			XRArgument arg;
			arg._ArgUint32 = E_HardModel;
			FArmySceneData::Get()->Add(ResultRoomArea, arg);
			TRoomSpaceArea.Add(ResultRoomArea);
		}

		const TArray<TSharedPtr<FJsonValue>> HardModeDataCircles = HardModeData->GetArrayField(TEXT("hardMode_Circles"));
		for (auto& It : HardModeDataCircles)
		{
			TSharedPtr<FArmyCircleArea> ResultArea = MakeShareable(new FArmyCircleArea());
			ResultArea->Deserialization(It->AsObject());
			FGuid id = ResultArea->GetParentGuid();
			for (auto RoomSpaceIt : TRoomSpaceArea)
			{
				if (RoomSpaceIt->GetUniqueID() == id)
				{
					XRArgument arg;
					arg._ArgUint32 = E_HardModel;
					FArmySceneData::Get()->Add(ResultArea, arg, RoomSpaceIt);
				};
			}

		}

		const TArray<TSharedPtr<FJsonValue>> HardModeDataRect = HardModeData->GetArrayField(TEXT("hardMode_Rects"));
		for (auto& It : HardModeDataRect)
		{
			TSharedPtr<FArmyRectArea> ResultArea = MakeShareable(new FArmyRectArea());
			ResultArea->Deserialization(It->AsObject());
			FGuid id = ResultArea->GetParentGuid();
			for (auto RoomSpaceIt : TRoomSpaceArea)
			{
				if (RoomSpaceIt->GetUniqueID() == id)
				{
					XRArgument arg;
					arg._ArgUint32 = E_HardModel;
					FArmySceneData::Get()->Add(ResultArea, arg, RoomSpaceIt);
				};
			}
		}

		const TArray<TSharedPtr<FJsonValue>> HardModeDataRegular = HardModeData->GetArrayField(TEXT("hardMode_Polygons"));
		for (auto& It : HardModeDataRegular)
		{
			TSharedPtr<FArmyRegularPolygonArea> ResultArea = MakeShareable(new FArmyRegularPolygonArea());
			ResultArea->Deserialization(It->AsObject());
			FGuid id = ResultArea->GetParentGuid();
			for (auto RoomSpaceIt : TRoomSpaceArea)
			{
				if (RoomSpaceIt->GetUniqueID() == id)
				{
					XRArgument arg;
					arg._ArgUint32 = E_HardModel;
					FArmySceneData::Get()->Add(ResultArea, arg, RoomSpaceIt);
				};
			}
		}

		const TArray<TSharedPtr<FJsonValue>> HardModeDataFreePolygon = HardModeData->GetArrayField(TEXT("hardMode_FreePolygons"));
		for (auto& It : HardModeDataFreePolygon)
		{
			TSharedPtr<FArmyFreePolygonArea> ResultArea = MakeShareable(new FArmyFreePolygonArea());
			ResultArea->Deserialization(It->AsObject());
			FGuid id = ResultArea->GetParentGuid();
			for (auto RoomSpaceIt : TRoomSpaceArea)
			{
				if (RoomSpaceIt->GetUniqueID() == id)
				{
					XRArgument arg;
					arg._ArgUint32 = E_HardModel;
					FArmySceneData::Get()->Add(ResultArea, arg, RoomSpaceIt);
				};
			}
		}

        //@郭子阳 递归的生成侧面
        std::function<void(TSharedPtr<FArmyBaseArea>)>
            GenerateSideArea = [&GenerateSideArea](TSharedPtr<FArmyBaseArea> InArea) {
            for (auto & SubArea : InArea->GetEditAreas())
            {
                GenerateSideArea(SubArea);
            }
            InArea->RefreshExtrusionPlane();
        };

        for (auto RoomSpaceIt : TRoomSpaceArea)
        {
            GenerateSideArea(StaticCastSharedPtr<FArmyBaseArea>(RoomSpaceIt));
        }

		//@郭子阳 递归的生成灯槽
        std::function<void(TSharedPtr<FArmyBaseArea>)>
            GenerateLamp = [&GenerateLamp](TSharedPtr<FArmyBaseArea> InArea) {
            InArea->GenerateLampSlot(GVC->GetWorld(), InArea->LampContentItem, InArea->GetLampSlotWidth(), true);

            for (auto & SubArea : InArea->GetEditAreas())
            {
                GenerateLamp(SubArea);
            }
        };

        for (auto RoomSpaceIt : TRoomSpaceArea)
        {
            GenerateLamp(StaticCastSharedPtr<FArmyBaseArea>(RoomSpaceIt));
        }
		
		////@郭子阳 刷新所有面的顶点
		//for (auto RoomSpaceIt : TRoomSpaceArea)
		//{
		//	RoomSpaceIt->RefreshPolyVertices();
		//}

	}

	//打扮家 XRLightmass 临时 加载方案时不生成外墙
	// 计算外墙体
	TArray<TWeakPtr<FArmyObject>> TotalOutRooms;
	TArray<TWeakPtr<FArmyObject>> TotalInnearRooms;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_OutRoom, TotalOutRooms);
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_InternalRoom, TotalInnearRooms);
	if (TotalOutRooms.Num() == 1)
	{
		TSharedPtr<FArmyRoom> TempOutRoom = StaticCastSharedPtr<FArmyRoom>(TotalOutRooms[0].Pin());
		TArray<TArray<FVector>> TotalInnerVerts;
		for (TWeakPtr<FArmyObject> iter : TotalInnearRooms)
		{
			TSharedPtr<FArmyRoom> TempInner = StaticCastSharedPtr<FArmyRoom>(iter.Pin());
			const TArray<FVector>& tempVerts = TempInner->GetWorldPoints(true);
			TotalInnerVerts.Push(tempVerts);
		}
		const TArray<FVector>& TotalHouseVerts = TempOutRoom->GetWorldPoints(true);
		FArmySceneData::Get()->UpdateOutWallInfo(TempOutRoom);
		FArmySceneData::Get()->CaculateSolidWall(TotalHouseVerts, TotalInnerVerts);
	}

	FString OutterWallLightMapIDStr;
	if (HardModeData->TryGetStringField("OutterWallLightMapID", OutterWallLightMapIDStr))
	{
		FGuid::Parse(OutterWallLightMapIDStr, FArmySceneData::Get()->OutterWallLightMapID);
	}
	

	/** @马云龙 尝试从Json中加载光照图文件的名称，用来对比本地存在的文件，如果不同，则视为发生了变化，删除 */
	if (!HardModeData->TryGetStringField("LevelDataFileName", LoadedLevelDataFilePath))
		LoadedLevelDataFilePath = "";

	LoadModel(HardModeData);

	LoadWorldSettings(HardModeData);
}

bool FArmyHardModeController::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	EndOperation();
	
	TArray<TWeakPtr<FArmyObject>> RoomSpaceAreaObjects;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, RoomSpaceAreaObjects);
	//原始户型刚设计后没有转3D时直接保存，要经过面片运算，防止加载户型时没有面片数据
	if (RoomSpaceAreaObjects.Num() == 0 && FArmySceneData::Get()->bHasOutWall)
		FArmySceneData::Get()->ConvertRoomInfoToWallInfo();
	//加载的户型，有了原有的面片。回到原始户型修改后，保存，要经过一步面片计算
	else if (RoomSpaceAreaObjects.Num() > 0 && (FArmySceneData::Get()->bUpdateHomeModeData || FArmySceneData::Get()->bUpdateModifyModeData))
		FArmySceneData::Get()->ConvertRoomInfoToWallInfo();

	/**　＠梁晓菲　保存的时候，施工项可能尚未根据UI更新数据，所以先隐藏，根据UI保存数据，这个过程会导致施工项UI被清空，所以隐藏，以后重新点击会重新读取数据 */
	if (HardModeDetail.IsValid())
	{
		HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);
		HardModeDetail->ExeBeforeClearDetail();
	}

	// 写入开始
	JsonWriter->WriteObjectStart(TEXT("hardMode"));

	/** 资源版本 */
	JsonWriter->WriteValue(TEXT("version"), GResourceVersion);

	TArray<FObjectWeakPtr> AllCircle;
	TArray<FObjectWeakPtr> AllRect;
	TArray<FObjectWeakPtr> AllPolygon;
	TArray<FObjectWeakPtr> AllRoomSpace;
	TArray<FObjectWeakPtr> AllFreePolygon;
	TArray<FObjectWeakPtr> AllBridgeStone;
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_CircleArea, AllCircle);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_RectArea, AllRect);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_PolygonArea, AllPolygon);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_FreePolygonArea, AllFreePolygon);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_BridgeStoneArea, AllBridgeStone);
	FArmySceneData::Get()->GetObjects(EModelType::E_HardModel, EObjectType::OT_RoomSpaceArea, AllRoomSpace);


	for (auto WindowIt : AllCircle)
	{
		//添加纹理iD到保存列表
		TSharedPtr<FArmyCircleArea> room = StaticCastSharedPtr<FArmyCircleArea>(WindowIt.Pin());
		SaveFileListID(WindowIt.Pin());
	}

	//@郭子阳
	//序列化绘制区域
	JsonWriter->WriteArrayStart(TEXT("hardMode_Circles"));
	for (auto RoomSpaceObj : AllRoomSpace)
	{
		auto RoomSpaceArea=StaticCastSharedPtr<FArmyBaseArea>(RoomSpaceObj.Pin());
		
		for (auto& SubArea : RoomSpaceArea->GetEditAreas())
		{
			if (SubArea->GetDrawAreaType() == EDrawAreaType::Circle)
			{
				JsonWriter->WriteObjectStart();
				SubArea->SerializeToJson(JsonWriter);
				JsonWriter->WriteObjectEnd();
			}
		}
	}
	JsonWriter->WriteArrayEnd();


	for (auto WindowIt : AllRect)
	{
		//添加纹理iD到保存列表
		TSharedPtr<FArmyRectArea> room = StaticCastSharedPtr<FArmyRectArea>(WindowIt.Pin());
		SaveFileListID(WindowIt.Pin());
		//添加角砖ID
		room->GetCornerBrickID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(room->GetCornerBrickID()) : 0;
		room->GetMainBrickID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(room->GetMainBrickID()) : 0;
		//添加造型线ID
		room->GetSkitLineGoodsID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(room->GetSkitLineGoodsID()) : 0;

	}


	JsonWriter->WriteArrayStart(TEXT("hardMode_Rects"));
		for (auto RoomSpaceObj : AllRoomSpace)
		{
			auto RoomSpaceArea = StaticCastSharedPtr<FArmyBaseArea>(RoomSpaceObj.Pin());

			for (auto& SubArea : RoomSpaceArea->GetEditAreas())
			{
				if (SubArea->GetDrawAreaType() == EDrawAreaType::Rectangle)
				{
					JsonWriter->WriteObjectStart();
					SubArea->SerializeToJson(JsonWriter);
					JsonWriter->WriteObjectEnd();
				}
			}
		}	  
	JsonWriter->WriteArrayEnd();


	for (auto WindowIt : AllPolygon)
	{
		//添加纹理iD到保存列表
		TSharedPtr<FArmyRegularPolygonArea> room = StaticCastSharedPtr<FArmyRegularPolygonArea>(WindowIt.Pin());
		SaveFileListID(WindowIt.Pin());
		//添加角砖ID
		room->GetMainBrickID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(room->GetMainBrickID()) : 0;
	}

	JsonWriter->WriteArrayStart(TEXT("hardMode_Polygons"));
		for (auto RoomSpaceObj : AllRoomSpace)
		{
			auto RoomSpaceArea = StaticCastSharedPtr<FArmyBaseArea>(RoomSpaceObj.Pin());

			for (auto& SubArea : RoomSpaceArea->GetEditAreas())
			{
				if (SubArea->GetDrawAreaType() == EDrawAreaType::RegularPolygon)
				{
					JsonWriter->WriteObjectStart();
					SubArea->SerializeToJson(JsonWriter);
					JsonWriter->WriteObjectEnd();
				}
			}
		}
	JsonWriter->WriteArrayEnd();


	for (auto WindowIt : AllFreePolygon)
	{
		//添加纹理iD到保存列表
		TSharedPtr<FArmyFreePolygonArea> room = StaticCastSharedPtr<FArmyFreePolygonArea>(WindowIt.Pin());
		SaveFileListID(WindowIt.Pin());
		//添加角砖ID
		room->GetMainBrickID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(room->GetMainBrickID()) : 0;
	}



	JsonWriter->WriteArrayStart(TEXT("hardMode_FreePolygons"));
		for (auto RoomSpaceObj : AllRoomSpace)
		{
			auto RoomSpaceArea = StaticCastSharedPtr<FArmyBaseArea>(RoomSpaceObj.Pin());

			for (auto& SubArea : RoomSpaceArea->GetEditAreas())
			{
				if (SubArea->GetDrawAreaType() == EDrawAreaType::CuastomPolygon)
				{
					JsonWriter->WriteObjectStart();
					SubArea->SerializeToJson(JsonWriter);
					JsonWriter->WriteObjectEnd();
				}
			}
		}
	JsonWriter->WriteArrayEnd();



	// 添加过门石
	JsonWriter->WriteArrayStart(TEXT("hardMode_BridgeStone"));
	for (auto WindowIt : AllBridgeStone)
	{
		JsonWriter->WriteObjectStart();
		WindowIt.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		//添加纹理iD到保存列表
		SaveFileListID(WindowIt.Pin());
	}
	JsonWriter->WriteArrayEnd();

	JsonWriter->WriteArrayStart(TEXT("hardMode_RoomSpaces"));
	for (auto WindowIt : AllRoomSpace)
	{
		JsonWriter->WriteObjectStart();
		WindowIt.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
		//添加纹理iD到保存列表
		TSharedPtr<FArmyRoomSpaceArea> room = StaticCastSharedPtr<FArmyRoomSpaceArea>(WindowIt.Pin());
		SaveFileListID(WindowIt.Pin());
		//添加踢脚线ID
		room->GetSkitLineGoodsID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(room->GetSkitLineGoodsID()) : 0;
	}
	JsonWriter->WriteArrayEnd();

	//单独保存外墙光照图ID
	if (FArmySceneData::Get()->TotalOutRoomActor)
		JsonWriter->WriteValue("OutterWallLightMapID", FArmySceneData::Get()->TotalOutRoomActor->GetLightMapGUIDFromSMC().ToString());

	SaveModel(JsonWriter, GGI->DesignEditor->SaveFileList);

	SaveMapBuildData(JsonWriter);

	SaveWorldSettings(JsonWriter);

	//写入结束
	JsonWriter->WriteObjectEnd();

	return true;
}

bool FArmyHardModeController::SaveFileListID(const TSharedPtr<FArmyObject> obj) const
{
	TSharedPtr<FArmyBaseArea> area = StaticCastSharedPtr<FArmyBaseArea>(obj);
	TSharedPtr<FArmyBaseEditStyle> style = area->GetStyle();
	if (style.IsValid())
	{
		style->GetGoodsID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(style->GetGoodsID()) : 0;
		style->GetSecondGoodID() > 0 ? GGI->DesignEditor->SaveFileList.AddUnique(style->GetSecondGoodID()) : 0;
	}
	return true;
}

bool FArmyHardModeController::CalModelMinDis(AActor* InputActor,
	TSharedPtr<FArmyFurniture> ComponentObj,
	TMap < FString/*方向*/, TMap<TSharedPtr<FArmyLine>/*内墙线*/, TSharedPtr<FArmyLine>/*最短距离线*/ > > &InputmapXYMinDis,
	TMap < FString/*方向*/, TMap<TSharedPtr<FArmyLine>/*内墙线*/, TSharedPtr<FArmyLine>/*最短距离线*/ > > &OutmapXYMinDis)
{
	TMap < FString, TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> > tempMap;
	FBox ActorBox;
	FVector Location = FVector::ZeroVector;
	FString ActorType = TEXT("");
	TArray<float> XYMinArr;
	XYMinArr.Add(MAX_flt);
	XYMinArr.Add(MAX_flt);
	if (ComponentObj.IsValid() && InputmapXYMinDis.Num() == 2)
	{
		ActorBox = ComponentObj->GetBounds();
		Location = ComponentObj->GetBasePos();
		ActorType = TEXT("Component");
		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator XMinIt(*InputmapXYMinDis.Find(TEXT("XMin")));
		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator YMinIt(*InputmapXYMinDis.Find(TEXT("YMin")));
		TSharedPtr<FArmyLine> XInnerLine = XMinIt.Key();
		TSharedPtr<FArmyLine> YInnerLine = YMinIt.Key();

		CalPointToLineDis(ActorBox, XInnerLine, OutmapXYMinDis, XYMinArr);
		CalPointToLineDis(ActorBox, YInnerLine, OutmapXYMinDis, XYMinArr);
		tempMap = OutmapXYMinDis;

	}
	else if (InputActor != nullptr && InputActor->IsValidLowLevel())
	{
		ActorBox = GVC->GetActorCollisionBox(InputActor);
		Location = InputActor->GetActorLocation();
		ActorType = TEXT("Model");

		TArray<FObjectWeakPtr> homeData;
		FArmySceneData::Get()->GetObjects(EModelType::E_HomeModel, EObjectType::OT_InternalRoom, homeData);

		for (auto room : homeData)
		{
			TSharedPtr<FArmyRoom> innerRoom = StaticCastSharedPtr<FArmyRoom>(room.Pin());
			if (innerRoom->IsPointInRoom(Location))
			{
				TArray<TSharedPtr<FArmyLine> > lines;
				innerRoom->GetLines(lines);
				for (auto L : lines)
				{
					TSharedPtr<FArmyLine> line = StaticCastSharedPtr<FArmyLine>(L);
					CalPointToLineDis(ActorBox, line, InputmapXYMinDis, XYMinArr);
				}
			}
		}

		tempMap = InputmapXYMinDis;
	}
	else
	{
		return false;
	}
	if (tempMap.Num() == 2)
	{
		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator XMinIt(*tempMap.Find(TEXT("XMin")));
		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator YMinIt(*tempMap.Find(TEXT("YMin")));
		//UE_LOG(LogTexture, Warning, TEXT("Minimum size of %s distance wall. Param: minx = %f,minY = %f"), *ActorType, XMinIt.Value()->Size(), YMinIt.Value()->Size());
	}
	return true;
}

bool FArmyHardModeController::CalPointToLineDis(FBox ActorBox, TSharedPtr<FArmyLine>line,
	TMap < FString, TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine> > > &OutmapXYMinDis,
	TArray<float> & XYMinArr)
{
	//存储X\Y各方向最小距离和相关的内墙线
	FVector XLeftStart(ActorBox.Min.X, ActorBox.GetCenter().Y, 0);
	FVector XLeftEnd(ActorBox.Min.X - 10000000, ActorBox.GetCenter().Y, 0);

	FVector XRightStart(ActorBox.Max.X, ActorBox.GetCenter().Y, 0);
	FVector XRightEnd(ActorBox.Max.X + 10000000, ActorBox.GetCenter().Y, 0);

	FVector YBottomStart(ActorBox.GetCenter().X, ActorBox.Min.Y, 0);
	FVector YBottomEnd(ActorBox.GetCenter().X, ActorBox.Min.Y - 10000000, 0);

	FVector YTopStart(ActorBox.GetCenter().X, ActorBox.Max.Y, 0);
	FVector YTopEnd(ActorBox.GetCenter().X, ActorBox.Max.Y + 10000000, 0);
	FVector outLeftPoint;
	FVector outRightPoint;
	FVector outTopPoint;
	FVector outBottomPoint;

	bool result = FArmyMath::CalculateLinesIntersection(XLeftStart, XLeftEnd, line->GetStart(), line->GetEnd(), outLeftPoint);
	if (result)
	{
		float Leftdis = FVector::Dist2D(outLeftPoint, XLeftStart);
		if (Leftdis < XYMinArr[0])
		{
			TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> temp;
			temp.Add(line, MakeShareable(new FArmyLine(XLeftStart, outLeftPoint)));
			OutmapXYMinDis.Add("XMin", temp);
			XYMinArr[0] = Leftdis;
		}
	}

	result = FArmyMath::CalculateLinesIntersection(XRightStart, XRightEnd, line->GetStart(), line->GetEnd(), outRightPoint);
	if (result)
	{
		float Rightdis = FVector::Dist2D(outRightPoint, XRightStart);

		if (Rightdis < XYMinArr[0])
		{
			TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> temp;
			temp.Add(line, MakeShareable(new FArmyLine(XRightStart, outRightPoint)));
			OutmapXYMinDis.Add("XMin", temp);
			XYMinArr[0] = Rightdis;

		}
	}

	result = FArmyMath::CalculateLinesIntersection(YTopStart, YTopEnd, line->GetStart(), line->GetEnd(), outTopPoint);
	if (result)
	{
		float Topdis = FVector::Dist2D(outTopPoint, YTopStart);

		if (Topdis < XYMinArr[1])
		{
			TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> temp;
			temp.Add(line, MakeShareable(new FArmyLine(YTopStart, outTopPoint)));
			OutmapXYMinDis.Add("YMin", temp);
			XYMinArr[1] = Topdis;
		}
	}

	result = FArmyMath::CalculateLinesIntersection(YBottomStart, YBottomEnd, line->GetStart(), line->GetEnd(), outBottomPoint);
	if (result)
	{
		float Bottomdis = FVector::Dist2D(outBottomPoint, YBottomStart);

		if (Bottomdis < XYMinArr[1])
		{
			TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> temp;
			temp.Add(line, MakeShareable(new FArmyLine(YBottomStart, outBottomPoint)));
			OutmapXYMinDis.Add("YMin", temp);
			XYMinArr[1] = Bottomdis;
		}
	}
	return true;
}
void FArmyHardModeController::TickTraceInteractiveActors()
{
	GGI->Window->ShowInteractiveMessage(false);

	static FName TraceTag = FName(TEXT("Detect"));
	FCollisionQueryParams TraceParams(TraceTag, false, GXRPC);
	FCollisionQueryParams TraceParamsComplex(TraceTag, true, GXRPC);
	TraceParams.AddIgnoredActor(GXRPC->GetPawn());
	TraceParams.bTraceAsyncScene = true;
	TraceParamsComplex.AddIgnoredActor(GXRPC->GetPawn());
	TraceParamsComplex.bTraceAsyncScene = true;

	FVector TraceStart, TraceEnd, WorldOrigin, WorldDirection;
	FHitResult Hit(ForceInit);
	//-----------------------------------------------------------ScreenToWorld
	FVector2D MousePos;
	GVC->GetMousePosition(MousePos);
	GVC->DeprojectFVector2D(MousePos, WorldOrigin, WorldDirection);
	TraceStart = WorldOrigin;
	TraceEnd = TraceStart + WorldDirection*10000.f;
	//-----------------------------------------------------------AddingModel Trace
	bool bHit = false;
	//检测可交互的物体
	bHit = GXRPC->GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParamsComplex);
	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			EActorType ActorType = FArmyResourceModule::Get().GetResourceManager()->GetActorType(HitActor);
			if (ActorType == EActorType::Blueprint)
			{
				UFunction* Function = HitActor->FindFunction(FName(TEXT("OnToggle")));
				if (Function && !HitActor->bHidden)
				{
					TempInteractiveActor = HitActor;
					GGI->Window->ShowInteractiveMessage(true);
					return;
				}
			}
		}
	}

    // @欧石楠 如果没有检测到可交互物体，需要置空
    TempInteractiveActor = nullptr;
}

void FArmyHardModeController::OnInteractiveHovered()
{
	if (TempInteractiveActor)
	{
		//如果是工具上传的可交互物体，则需要执行InitActor函数
		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		FVRSObject* ResultObj = ResMgr->GetObjFromObjID(TempInteractiveActor->GetObjID());
		if (ResultObj->GetSkeletalMeshObject())
		{
			UFunction* InitFunction = TempInteractiveActor->FindFunction(FName(TEXT("InitActor")));
			if (InitFunction)
			{
				struct FInitActorPars
				{
					USkeletalMesh* SkObj;
					UAnimSequence* AnimObj;
					FVector BoundOrigin;
					FVector BoundExtent;
				};
				FInitActorPars Pars;
				Pars.SkObj = ResultObj->GetSkeletalMeshObject();
				Pars.AnimObj = ResultObj->GetAnimSequenceObject();
				Pars.BoundOrigin = ResultObj->BoundOrigin;
				Pars.BoundExtent = ResultObj->BoundExtent;
				TempInteractiveActor->ProcessEvent(InitFunction, &Pars);
			}
		}
		else if (ResultObj->GetLampMeshObject())
		{
			UFunction* InitFunction = TempInteractiveActor->FindFunction(FName(TEXT("InitActor")));
			if (InitFunction)
			{
				struct FInitActorPars
				{
					UStaticMesh* LampObj;
					FLinearColor LightColor;
					float AttenuationRadius;
					float PointLightIntensity;
					float SpotLightIntensity;
					float SpotLightConeAngle;
					TArray<FTransform> PointLightTransforms;
					TArray<FTransform> SpotLightTransforms;
				};
				FInitActorPars Pars;
				Pars.LampObj = ResultObj->GetLampMeshObject();
				Pars.LightColor = ResultObj->LampColor;//FLinearColor(1.f, 0.871f, 0.718f);
				Pars.AttenuationRadius = 1000.f;
				Pars.PointLightIntensity = ResultObj->LampIntensity;
				Pars.SpotLightIntensity = ResultObj->LampIntensity / 120.f;
				Pars.SpotLightConeAngle = 80.f;
				Pars.PointLightTransforms = ResultObj->PointArray;
				Pars.SpotLightTransforms = ResultObj->SpotArray;
				TempInteractiveActor->ProcessEvent(InitFunction, &Pars);
			}
		}
		//执行OnToggle函数
		UFunction* ToggleFunction = TempInteractiveActor->FindFunction(FName(TEXT("OnToggle")));
		if (ToggleFunction)
		{
			TempInteractiveActor->ProcessEvent(ToggleFunction, nullptr);
		}
	}
}

void FArmyHardModeController::SetLightsVisibility(EVisibility LightVisible)
{
	bool bLightVisible = (LightVisible == EVisibility::Visible) ? true : false;
	bIsDisplayLightIconChecked = bLightVisible;

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

void FArmyHardModeController::Delete()
{
	SCOPE_TRANSACTION(TEXT("删除软装模型"));

	TArray<AActor*> ActorsToDelete;
	GXREditor->CanDeleteSelectedActors(GVC->GetWorld(), false, true, &ActorsToDelete);
	TArray< TWeakPtr<FArmyObject> > ActorItems = FArmySceneData::Get()->GetObjects(E_HardModel);
	for (auto& It : ActorItems)
	{
		if (It.Pin().IsValid() && It.Pin()->GetType() == EObjectType::OT_ActorItem)
		{
			TSharedPtr<FArmyActorItem> ActorItem = StaticCastSharedPtr<FArmyActorItem>(It.Pin());
			for (auto& ActorIt : ActorsToDelete)
			{
				if (ActorItem->Actor == ActorIt)
				{
					FArmySceneData::Get()->Delete(ActorItem);
					break;
				}
			}
		}
	}

	TArray<AActor*> SeletecdActors;
	GXREditor->GetSelectedActors(SeletecdActors);
	for (AActor* iter : SeletecdActors)
	{
		AArmyExtrusionActor* skitActor = Cast<AArmyExtrusionActor>(iter);
		AXRWallActor* OrignalWallActor = Cast<AXRWallActor>(iter);
		if (skitActor)
		{
			if (skitActor->AttachSurfaceArea.IsValid())
			{
				/** @梁晓菲 删除该Actor关联的施工项*/
				FArmySceneData::Get()->DeleteActorConstructionItemData(skitActor->GetUniqueID());
				//if (skitActor->AttachSurfaceArea.Pin()->GetType() == OT_RoomSpaceArea)
				//{
					TSharedPtr<FArmyRoomSpaceArea> roomSpace = StaticCastSharedPtr<FArmyRoomSpaceArea>(skitActor->AttachSurfaceArea.Pin());
					if (roomSpace->GenerateFromObjectType == OT_Pass || roomSpace->GenerateFromObjectType == OT_NewPass ||
						roomSpace->GenerateFromObjectType == OT_Door || roomSpace->GenerateFromObjectType == OT_SecurityDoor ||
						roomSpace->GenerateFromObjectType == OT_SlidingDoor)
					{
						/* @梁晓菲 删除扣条*/
						roomSpace->RemoveBuckleLine(skitActor);
					}
					else
					{
						roomSpace->RemoveSkitLineActor(skitActor);
					}
				//}
				if (skitActor->Tags.Contains(TEXT("WallMoldingLine")))
				{
					skitActor->AttachSurfaceArea.Pin()->RemoveWallModingLine();
				}
				if (skitActor->Tags.Contains("LampSlotActor"))
				{
					skitActor->AttachSurfaceArea.Pin()->RemoveLampSlot(skitActor);
				}
			}
		}
		if (OrignalWallActor)
		{
			if (OrignalWallActor->AttachSurface.IsValid())
			{
				if (OrignalWallActor->AttachSurface->GetType() == OT_RoomSpaceArea)
				{
					TSharedPtr<FArmyRoomSpaceArea> roomSpace = StaticCastSharedPtr<FArmyRoomSpaceArea>(OrignalWallActor->AttachSurface);
					roomSpace->ResetSurfaceStyle(true);
				}
				else if (OrignalWallActor->AttachSurface->GetType() == OT_CircleArea ||
					OrignalWallActor->AttachSurface->GetType() == OT_FreePolygonArea ||
					OrignalWallActor->AttachSurface->GetType() == OT_RectArea ||
					OrignalWallActor->AttachSurface->GetType() == OT_PolygonArea ||
					OrignalWallActor->AttachSurface->GetType() == OT_BodaArea)
				{
					if (OrignalWallActor->AttachSurface->GetType() == OT_BodaArea)
					{
						TSharedPtr<FArmyBodaArea> tempBodaArea = StaticCastSharedPtr<FArmyBodaArea>(OrignalWallActor->AttachSurface);
						TSharedPtr<FArmyRectArea> tempRectArea = StaticCastSharedPtr<FArmyRectArea>(tempBodaArea->AttachParentArea);
						tempRectArea->DeleteBodaArea();
					}
					else
						if (OrignalWallActor->AttachSurface->IsPlotArea())
						{
							OrignalWallActor->AttachSurface->MoveChildAreaToParent();
						}
						FArmySceneData::Get()->Delete(OrignalWallActor->AttachSurface);
				}
			}
		}
	}
	if (HardModeDetail.IsValid())
		HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);
	GXREditor->edactDeleteSelected(GVC->GetWorld());
}

void FArmyHardModeController::ReplaceTextureOperation()
{
	SetOperation((uint8)EHardModeOperation::HO_Replace);

	TSharedPtr<FArmyReplaceTextureOperation>	tempOperation = StaticCastSharedPtr<FArmyReplaceTextureOperation>(CurrentOperation);
	tempOperation->SetCurrentItem(LeftPanel->GetSelectedContentItem());
	//tempOperation->SetCurrentSelectItem(LeftPanel->GetSelectItem());


}

void FArmyHardModeController::CameraReturnCallBack()
{
	GVC->SetViewportType(EXRLevelViewportType::LVT_Perspective);
	GVC->LockViewPortClient(false);
	GVC->SetViewLocation(LastCameraViewLocation);
	GVC->SetViewRotation(LastCameraViewRotator);
	// 显示硬装所有数据
	DisplyOrHidden(false);
	HightLightActor->SetActorHiddenInGame(true);
	CurrentEditMode = HO_FreeEdit;
	FArmyObject::SetDrawModel(MODE_CEILING, false);
	GVC->bForceHiddenWidget = false;
	MI_BackGroundColor->SetScalarParameterValue("Alpha", 0.0f);
	ActorAxisRuler->SetAxisVisible(FArmyAxisRulerActor::AXIS_ALL, true);
	HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);

	TArray<TWeakPtr<FArmyObject>> TotalSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, TotalSurfaces);
	for (TWeakPtr<FArmyObject> iter : TotalSurfaces)
	{
		FArmyRoomSpaceArea* tempRoom = iter.Pin()->AsassignObj<FArmyRoomSpaceArea>();
		if (tempRoom)
			tempRoom->SetOriginalActorVisible(true);
	}
	FArmySceneData::Get()->Set3DRoomVisible(true);
	
	//显示外景
	FArmyWorldManager::Get().SetOutdoorMeshVisibile(true);
}

void FArmyHardModeController::DisplyOrHidden(bool bDisplay)
{
	if (!bDisplay)
	{
		TSharedPtr<IArmyActorVisitor> HardModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardMode);
		HardModeAV->Show(true);
	}

	TSharedPtr<IArmyActorVisitor> HardSubModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardSubMode);
	HardSubModeAV->Show(bDisplay);
}

void FArmyHardModeController::OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj, bool bTransaction/*=false*/)
{
	if (InArg._ArgUint32 == E_HardModel)
	{
		if (InArg._ArgInt32 == 1)
		{
			if (InObj->GetParents().Num() > 0)
			{
				TWeakPtr<FArmyObject> parent = InObj->GetParents()[0];
				FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
				TSharedPtr<FArmyBaseArea> InEditAreas = StaticCastSharedPtr<FArmyBaseArea>(InObj);
				if (InEditAreas.IsValid())
					temp->AddArea(InEditAreas);
			}

		}
		else
		{
			if (InObj->GetParents().Num() != 0)
			{
				TWeakPtr<FArmyObject> parent = InObj->GetParents()[0];
				if (!parent.IsValid())
					return;
				FArmyBaseArea* temp = parent.Pin()->AsassignObj<FArmyBaseArea>();
				TSharedPtr<FArmyBaseArea> InEditAreas = StaticCastSharedPtr<FArmyBaseArea>(InObj);
				if (InEditAreas.IsValid())
					temp->RemoveArea(InEditAreas);
			}

		}

	}
}



void FArmyHardModeController::CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles)
{

}

void FArmyHardModeController::LockCameraToEditSurface()
{
	if (GVC->ViewportType != EXRLevelViewportType::LVT_Perspective) return;
	TArray<float> Dists;
	TArray<TSharedPtr<FArmyRoomSpaceArea>> selectLists = FArmySceneData::Get()->ForceSelectRoomAreas(FVector(GVC->GetCurrentMousePos(), 0.0), GVC, Dists);
	if (selectLists.Num() == 0)
		return;
	LockCameraToEditSurface(selectLists[0]);
	MI_BackGroundColor->SetScalarParameterValue("Alpha", 0.3f);
}

bool FArmyHardModeController::LockCameraToEditSurface(TSharedPtr<FArmyObject> editArea)
{

	// @梁晓菲 进入子模式隐藏软装和木作物体，锁定外墙
	TSharedPtr<IArmyActorVisitor> OuterWallVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_OuterWall);
	OuterWallVisitor->Lock(true);
	TSharedPtr<IArmyActorVisitor> WHCActorVisitor = FArmyActorVisitorFactory::Get().CreateVisitor(AV_WHCActor);
	WHCActorVisitor->Show(false);

	CurrentEditSurface = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea);
	if (!CurrentEditSurface.IsValid())
		return false;
	FVector normal = CurrentEditSurface->GetPlaneNormal();
	FVector lockAtCenter = CurrentEditSurface->GetPlaneCenter();
	LastCameraViewLocation = GVC->GetViewLocation();
	LastCameraViewRotator = GVC->GetViewRotation();

	if (CurrentEditSurface->SurfaceType == 0)
		CurrentEditMode = HO_FloorEdit;
	else if (CurrentEditSurface->SurfaceType == 2)
		CurrentEditMode = HO_RoofEdit;
	else
		CurrentEditMode = HO_WallEdit;

	FVector xDir = CurrentEditSurface->GetXDir();
	FVector	yDir = CurrentEditSurface->GetYDir();
	FVector centerPos = CurrentEditSurface->GetPlaneCenter();
	FVector ViewLocation = lockAtCenter + normal * 200.0f;

	if (CurrentEditMode == HO_WallEdit)
	{
		GVC->SetViewportType(EXRLevelViewportType::LVT_OrthoFreelook);

		FVector ViewLookAt(1, 0, 0), ViewRight(0, 1, 0), ViewUp(0, 0, 1);
		FVector RefViewLookAt(normal * -1), RefViewRight(xDir), RefViewUp(yDir * -1);

		FQuat Qt1 = FQuat::FindBetweenNormals(ViewLookAt, RefViewLookAt);
		FVector TempRight = Qt1 * ViewRight;
		FVector PreRight = Qt1.Inverse() * RefViewRight;
		FQuat Qt2 = FQuat::FindBetweenNormals(ViewRight, PreRight);
		GVC->SetLookAtLocation(lockAtCenter);
		GVC->SetViewLocation(ViewLocation);
		GVC->SetViewRotation((Qt1 * Qt2).Rotator());
		FBox box(CurrentEditSurface->GetOutArea()->Vertices);
		float OrthZoom = FMath::Max(box.GetSize().X, box.GetSize().Y);
		FVector2D tempViewPortSize;
		GVC->GetViewportSize(tempViewPortSize);

		//GVC->SetViewLocation(centerPos);
		GVC->SetViewLocation(ViewLocation);

		OrthZoom = OrthZoom* FMath::Max(tempViewPortSize.X, tempViewPortSize.Y) / 720;
		if (OrthZoom != 0.0f)
		{
			GVC->SetOrthoZoom(OrthZoom);
		}
	}
	else
	{
		GVC->SetViewportType(EXRLevelViewportType::LVT_OrthoXY);
		FBox box(CurrentEditSurface->GetOutArea()->Vertices);
		float OrthZoom = FMath::Max(box.GetSize().X, box.GetSize().Y);
		FVector2D tempViewPortSize;
		GVC->GetViewportSize(tempViewPortSize);

		//GVC->SetViewLocation(centerPos);
		GVC->SetViewLocation(ViewLocation);

		OrthZoom = OrthZoom* FMath::Max(tempViewPortSize.X, tempViewPortSize.Y) / 720;
		if (OrthZoom != 0.0f)
		{
			GVC->SetOrthoZoom(OrthZoom);
		}
	}

	FArmyToolsModule::Get().GetMouseCaptureTool()->SetRefCoordinateSystem(centerPos, xDir, yDir, normal);


	//@郭子阳 在进入子模式后重新进入材质刷操作
	if ((CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_Replace)
		&& FormerOperation.IsValid() && FormerOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush))
		|| CurrentOperation.IsValid() && CurrentOperation == OperationMap.FindRef((uint8)EHardModeOperation::HO_MaterialBrush)
		)
	{
		TSharedPtr<FArmyMaterialBrushOperation>	MatBrush = StaticCastSharedPtr<FArmyMaterialBrushOperation>(*OperationMap.Find((uint8)EHardModeOperation::HO_MaterialBrush));
		MatBrush->SetRebeginAtStart();
		SetOperation((uint8)EHardModeOperation::HO_None);
	}


	if (CurrentEditSurface->SurportPlaceArea == 0)
	{
		HardModeFloorController->BeginMode();
		HardModeFloorController->SetCurrentEditSurface(CurrentEditSurface);
		//进入地面模式瀑布流默认显示所有瓷砖数据
		//HardModeFloorController->GetHardModeContentBrowser()->RequestContentItems(0, 6);
		GGI->Window->PresentModalViewController(HardModeFloorController);
	}
	else if (CurrentEditSurface->SurportPlaceArea == 1)
	{
		HardModeWallController->BeginMode();
		HardModeWallController->SetCurrentEditSurface(CurrentEditSurface);
		//进入墙面模式瀑布流默认显示所有墙砖数据
	//	HardModeFloorController->GetHardModeContentBrowser()->RequestContentItems(0, 22);
		GGI->Window->PresentModalViewController(HardModeWallController);
	}
	else if (CurrentEditSurface->SurportPlaceArea == 2)
	{
		HardModeCeilingController->BeginMode();
		HardModeCeilingController->SetCurrentEditSurface(CurrentEditSurface);
		//进入顶面模式瀑布流默认显示所有石膏板吊顶数据
		//HardModeFloorController->GetHardModeContentBrowser()->RequestContentItems(0, 41);
		GGI->Window->PresentModalViewController(HardModeCeilingController);
	}
	GVC->LockViewPortClient(true);
	// 隐藏所有actor
	DisplyOrHidden(true);
	// 显示当前墙面上关联的门窗模型
	if (CurrentEditSurface->SurfaceType == 1)
	{
		for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			for (FString& nameIter : CurrentEditSurface->CurrentAttachWindowName)
			{
				if (Actor->GetActorLabel() == nameIter)
				{
					Actor->SetActorHiddenInGame(false);
				}
			}
		}
	}
	// 掩藏所有原始面
	TArray<TWeakPtr<FArmyObject>> TotalSurfaces;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, TotalSurfaces);
	for (TWeakPtr<FArmyObject> iter : TotalSurfaces)
	{
		TSharedPtr<FArmyRoomSpaceArea> tempRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		tempRoom->SetOriginalActorVisible(false);
	}
	FArmySceneData::Get()->Set3DRoomVisible(false);
	CurrentEditSurface->SetActorVisible(true);
	FArmyObject::SetDrawModel(MODE_CEILING, true);
	GVC->bForceHiddenWidget = true;
	ActorAxisRuler->SetAxisVisible(FArmyAxisRulerActor::AXIS_ALL, false);

	//隐藏外景
	FArmyWorldManager::Get().SetOutdoorMeshVisibile(false);
	return true;
}

void FArmyHardModeController::OnSaveCommand()
{
	EndOperation();
	FArmyFrameCommands::OnMenuSave();
}

void FArmyHardModeController::OnUndoCommand()
{
	EndOperation();
	FArmyFrameCommands::OnMenuUndo();
}

void FArmyHardModeController::OnRedoCommand()
{
	EndOperation();
	FArmyFrameCommands::OnMenuRedo();

}

void FArmyHardModeController::OnEmptyPlanCommand()
{
	EndOperation();
	GGI->Window->PresentModalDialog(TEXT("是否清空所有？"), FSimpleDelegate::CreateRaw(this, &FArmyHardModeController::ResetDefaultStyle));
}

void FArmyHardModeController::OnEmptyStaticLightingCommand()
{
	EndOperation();
	GGI->Window->PresentModalDialog(TEXT("是否清空渲染效果？"), FSimpleDelegate::CreateRaw(this, &FArmyHardModeController::ClearStaticLighitngData));
}

void FArmyHardModeController::OnDeleteCommand()
{
	EndOperation();
	Delete();
}

void FArmyHardModeController::OnReplaceCommand()
{
	//LeftPanel->ShowReplaceData();
}

void FArmyHardModeController::OnCopyCommand()
{

}

void FArmyHardModeController::OnBuildStaticLightingCommand()
{
	EndOperation();
	if (GGI) 
	{
		TSharedPtr<SArmyStaticLightingSettings> NewWidget = SNew(SArmyStaticLightingSettings)
			.OnGenerateClickedDelegate(this, &FArmyHardModeController::BuildStaticMeshLighting)
			.OnBuildFinishedDelegate(this, &FArmyHardModeController::BuildStaticLightingFinished);
		NewWidget->InitializeSettings();

		GGI->Window->PresentModalDialog(TEXT("全屋渲染"), NewWidget->AsShared());
	}
} 

TSharedRef<SWidget> FArmyHardModeController::OnEmptyCommand()
{
	EndOperation();
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	ActionList->MapAction(FArmyHardModeCommands::Get().CommandEmptyPlan, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnEmptyPlanCommand));
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandEmptyStaticLighting, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnEmptyStaticLightingCommand));

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandEmptyPlan);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandEmptyStaticLighting);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FArmyHardModeController::OnLightCommand()
{
	//放置光源前结束上一次操作
	EndOperation();
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);

	ActionList->MapAction(FArmyHardModeCommands::Get().CommandPointLight, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnPointLightCommand));
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandSpotLight, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnSpotLightCommand));
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandReflectionCapture, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnReflectionCaptureCommand));

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);

	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandPointLight);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandSpotLight);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandReflectionCapture);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	return MenuBuilder.MakeWidget();
}

void FArmyHardModeController::OnMaterialBrushCommand()
{
	EndOperation();
	SetOperation((uint8)EHardModeOperation::HO_MaterialBrush);
	/*const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef<FUICommandList> ActionList = MakeShareable(new FUICommandList);

	ActionList->MapAction(FArmyHardModeCommands::Get().CommandMaterialBrush, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnDisplayLightIconCommand));


	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandMaterialBrush);

	return MenuBuilder.MakeWidget();*/
}

TSharedRef<SWidget> FArmyHardModeController::OnDisplayCommand()
{
	EndOperation();
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef<FUICommandList> ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);
	FIsActionChecked IsDisplayLightIconChecked = FIsActionChecked::CreateRaw(this, &FArmyHardModeController::IsDisplayLightIconChecked);

	ActionList->MapAction(FArmyHardModeCommands::Get().CommandDisplayLightIcon, FExecuteAction::CreateRaw(this, &FArmyHardModeController::OnDisplayLightIconCommand), DefaultCanExec, IsDisplayLightIconChecked);

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandDisplayLightIcon);

	return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget> FArmyHardModeController::OnViewComboCommand()
{
	EndOperation();
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef<FUICommandList> ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);

	FIsActionChecked IsTopViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsTopViewChecked);
	FIsActionChecked IsCeilingViewChecked = FIsActionChecked::CreateStatic(&FArmyFrameCommands::IsCeilingViewChecked);

	ActionList->MapAction(FArmyHardModeCommands::Get().CommandHardTopView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuTopView), DefaultCanExec, IsTopViewChecked);
	ActionList->MapAction(FArmyHardModeCommands::Get().CommandHardCeilingView, FExecuteAction::CreateStatic(&FArmyFrameCommands::OnMenuCeilingView), DefaultCanExec, IsCeilingViewChecked);

	CommandList->Append(ActionList);

	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandHardTopView);
	MenuBuilder.AddMenuEntry(FArmyHardModeCommands::Get().CommandHardCeilingView);

	return MenuBuilder.MakeWidget();
}

bool FArmyHardModeController::ShouldTick()
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

//void FArmyHardModeController::OnConstructionWallReady()
//{
//	if (HardModeDetail.IsValid())
//	{
//		HardModeDetail->OnConstructionWallReady();
//	}
//}

//void FArmyHardModeController::OnConstructionRoofReady()
//{
//	if (HardModeDetail.IsValid())
//	{
//		HardModeDetail->OnConstructionRoofReady();
//	}
//}

//void FArmyHardModeController::OnConstructionFloorReady()
//{
//	if (HardModeDetail.IsValid())
//	{
//		HardModeDetail->OnConstructionFloorReady();
//	}
//}

//void FArmyHardModeController::ReqConstructionData(int32 GoodsId)
//{
//	FString url = FString::Printf(TEXT("/api/quota/goods/%d"), GoodsId);
//	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(url, FArmyHttpRequestCompleteDelegate::CreateRaw(this, &FArmyHardModeController::ResConstructionData, GoodsId));
//	Request->ProcessRequest();
//}

//void FArmyHardModeController::ResConstructionData(FArmyHttpResponse Response, int32 GoodsId)
//{
//	if (Response.bWasSuccessful && Response.Data.IsValid())
//	{
//		FArmySceneData::Get()->AddItemIDConstructionData(GoodsId, Response.Data);
//	}
//}

void FArmyHardModeController::SaveWorldSettings(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	if (FArmyWorldManager::Get().PPV_StaticLighting)
	{
		JsonWriter->WriteObjectStart(TEXT("WorldSettings"));

		JsonWriter->WriteValue(TEXT("BloomIntensity"), FArmyWorldManager::Get().PPV_StaticLighting->Settings.BloomIntensity);
		JsonWriter->WriteValue(TEXT("AutoExposureBias"), FArmyWorldManager::Get().PPV_StaticLighting->Settings.AutoExposureBias);

		JsonWriter->WriteObjectEnd();
	}
}

void FArmyHardModeController::LoadWorldSettings(TSharedPtr<FJsonObject> Data)
{
	if (FArmyWorldManager::Get().PPV_StaticLighting)
	{
		const TSharedPtr<FJsonObject>* SoftModeData = nullptr;
		if (Data->TryGetObjectField("WorldSettings", SoftModeData))
		{
			FString BloomIntensity;
			FString AutoExposureBias;

			if ((*SoftModeData)->TryGetStringField("BloomIntensity", BloomIntensity))
				FArmyWorldManager::Get().PPV_StaticLighting->Settings.BloomIntensity = FCString::Atof(*BloomIntensity);
			if ((*SoftModeData)->TryGetStringField("AutoExposureBias", AutoExposureBias))
				FArmyWorldManager::Get().PPV_StaticLighting->Settings.AutoExposureBias = FCString::Atof(*AutoExposureBias);
		}
	}
}

void FArmyHardModeController::ResetDefaultStyle()
{
	ClearStaticLighitngData();
	TransMgr->Reset();
	Cleanup(true, false);
	if (HardModeDetail.IsValid())
		HardModeDetail->SetPropertyPanelVisibility(EVisibility::Collapsed);
	if (HightLightActor)
	{
		HightLightActor->SetActorHiddenInGame(true);
	}
}

void FArmyHardModeController::BuildStaticMeshLighting(int32 InQuality)
{
	//标清
	if (InQuality == 1)
	{
		GWorld->GetWorldSettings()->LightmassSettings.StaticLightingLevelScale = 1.f;
		GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingQuality = 1.f;
		GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingSmoothness = 1.f;
	}
	//高清
	else if (InQuality == 3)
	{
		GWorld->GetWorldSettings()->LightmassSettings.StaticLightingLevelScale = 0.85f;
		GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingQuality = 1.5f;
		GWorld->GetWorldSettings()->LightmassSettings.IndirectLightingSmoothness = 0.95f;
	}

	//GGI->Window->DismissModalDialog();
	//FSlateApplication::Get().Tick();

	for (TActorIterator<AStaticMeshActor> It(GWorld); It; ++It)
	{
		if (It && It->Tags.Num() > 0 && It->Tags[0] == TEXT("MoveableMeshActor"))
		{
			It->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
		}
	}

	FArmyWorldManager::Get().SetEnvironmentMode(false);
	GenerateDoorBlocks();

	for (TActorIterator<AStaticMeshActor> It(GWorld); It; ++It)
	{
		if (It && It->Tags.Num() > 0 && It->Tags[0] == TEXT("MoveableMeshActor"))
		{
			It->GetStaticMeshComponent()->SetMobility(FArmyWorldManager::Get().bBuildStaticMesh ? EComponentMobility::Static : EComponentMobility::Movable);
			It->GetStaticMeshComponent()->bOverrideLightMapRes = 1;
			It->GetStaticMeshComponent()->OverriddenLightMapRes = 1;
		}
	}

	for (TActorIterator<AStaticMeshActor> It(GWorld); It; ++It)
	{
		if (It && It->Tags.Num() > 0 && It->Tags[0] == TEXT("MoveableMeshActor"))
		{
			for (UMaterialInterface* It : It->GetStaticMeshComponent()->OverrideMaterials)
			{
				UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(It);
				if (MID)
				{
					MID->SetScalarParameterValue("Shadow_ON", FArmyWorldManager::Get().bBuildStaticMesh ? 0.f : 1.f);
				}
			}
		}
	}

	FStaticLightingManager::Get()->BuildStaticLighting(InQuality);
}

void FArmyHardModeController::BuildStaticLightingFinished(bool bInSuccess)
{
	if (bInSuccess)
	{
		HardModeDetail->TryToggleWorldSettings();

		for (TActorIterator<AStaticMeshActor> It(GWorld); It; ++It)
		{
			if (It && It->Tags.Num() > 0 && It->Tags[0] == TEXT("MoveableMeshActor"))
			{
				It->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			}
		}

		UpdateReflection();
	}
	else
		ClearStaticLighitngData();
}

void FArmyHardModeController::ClearStaticLighitngData()
{
	FStaticLightingManager::Get()->ClearStaticLighting();
	FArmyWorldManager::Get().SetEnvironmentMode(true);
	//SetDynamicLightEnvironment();
	ClearDoorBlocks();
	HardModeDetail->TryToggleWorldSettings();
}

void FArmyHardModeController::ClearDoorBlocks()
{
	for (AXRShapeActor* It : DoorBlocks)
	{
		if (It)
		{
			It->Destroy();
		}
	}

	DoorBlocks.Reset();
}

void FArmyHardModeController::GenerateDoorBlocks()
{
	ClearDoorBlocks();

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
					ProcCOM->SetLightmapResolution(64);
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

					UXRProceduralMeshComponent::Triangulate_Extrusion(OutlineVetices, -60.f, SectionVertices, SectionTriangles);
					ProcCOM->CreateMeshSection(3, SectionVertices, SectionTriangles, true, true);

					ProcCOM->SetMaterial(0, M_Wall);
					ProcCOM->SetMaterial(1, M_Wall);
					ProcCOM->SetMaterial(2, M_Wall);
					ProcCOM->SetMaterial(3, M_Wall);
					TempActor->SetActorHiddenInGame(true);
					DoorBlocks.Add(TempActor);
				}
			}
		}
	}
}

void FArmyHardModeController::AllControllersLoadCompleted()
{
	// @zengy 当未切换过立面模式时，TotalOutRoomActor为空，进行判定
	if (FArmySceneData::Get()->TotalOutRoomActor != nullptr)
	{
		FArmySceneData::Get()->TotalOutRoomActor->LightMapID = FArmySceneData::Get()->OutterWallLightMapID;
	}
	TryLoadMapBuildData();
}

void FArmyHardModeController::DelayUpdate()
{
	UpdateReflection();
}

void FArmyHardModeController::UpdateReflection()
{
	GGI->GetWorld()->UpdateAllReflectionCaptures();
	GGI->GetWorld()->UpdateAllSkyCaptures();
}

void FArmyHardModeController::OnHomeRebuild()
{
	ClearStaticLighitngData();
}

void FArmyHardModeController::Cleanup(bool bClearResource, bool bClearRoomSpace)
{
	TArray<TWeakPtr<FArmyObject>> SelectedRoomLists;

	TArray<TWeakPtr<FArmyObject>> SelectedBridgeStoneArea;
	TArray<TWeakPtr<FArmyObject>> TotalEditAreas;
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RoomSpaceArea, SelectedRoomLists);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_RectArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_CircleArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_PolygonArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_FreePolygonArea, TotalEditAreas);
	FArmySceneData::Get()->GetObjects(E_HardModel, OT_BridgeStoneArea, SelectedBridgeStoneArea);

	for (TWeakPtr<FArmyObject > iter : SelectedRoomLists)
	{
		TSharedPtr<FArmyRoomSpaceArea> temp = StaticCastSharedPtr<FArmyRoomSpaceArea>(iter.Pin());
		if (bClearRoomSpace)
		{
			//@郭子阳
			//递归地删除绘制区域
			std::function<void(TSharedPtr<FArmyBaseArea>& Area)>
				DeleteSubAreas = [&DeleteSubAreas](TSharedPtr<FArmyBaseArea> & Area) {
				for (auto & SubArea : Area->GetEditAreas())
				{
					DeleteSubAreas(SubArea);
				}
				//Area->RoomEditAreas.Empty();

				FArmySceneData::Get()->Delete(Area);
			};

			for (auto& SubArea : temp->GetEditAreas())
			{
				DeleteSubAreas(SubArea);
			}
			temp->RoomEditAreas.Empty();

			FArmySceneData::Get()->Delete(iter.Pin(), true);
		}
		else
		{
			temp->ResetSurfaceStyle(true);
			HardModeDetail->ExeBeforeClearDetail();//先存档
			// 给撤销重做增加一条记录
			TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter =
				TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&temp->RecordBeforeModify);
			JsonWriter->WriteObjectStart();
			temp->SerializeToJson(JsonWriter);
			JsonWriter->WriteObjectEnd();
			JsonWriter->Close();
			temp->Record = temp->RecordBeforeModify;
		}
	}
	//for (TWeakPtr<FArmyObject> iter : TotalEditAreas)
	//{
	//	FArmySceneData::Get()->Delete(iter.Pin(), true);
	//}
	for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->Tags.Num() > 0 &&
			(ActorItr->Tags[0] == "MoveableMeshActor" || ActorItr->Tags[0] == "EnvironmentAsset" || ActorItr->Tags[0] == "BlueprintVRSActor") &&
			!ActorItr->Tags.Contains("HydropowerActor") &&
			!ActorItr->Tags.Contains("RectAreaHightActor") &&
			!ActorItr->Tags.Contains("AreaHighLight") &&
			!ActorItr->ActorHasTag(XRActorTag::OriginalPoint) &&
			!ActorItr->ActorHasTag(XRActorTag::Door))
		{
			//FArmySceneData::Get()->DeleteActorConstructionItemData(ActorItr->GetUniqueID());
			ActorItr->Destroy();
		}
	}

    if (bClearResource)
    {
        // 将室内外门还原成白模
        FContentItemPtr CommonSingleDoorItem = FArmyResourceModule::Get().GetResourceManager()->GetContentItemFromItemID(EResourceType::BlueprintClass, -2);
        TArray< TWeakPtr<FArmyObject> > SingleDoors;
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Door, SingleDoors);
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SecurityDoor, SingleDoors);
        for (auto It : SingleDoors)
        {
            TSharedPtr<FArmySingleDoor> Door = StaticCastSharedPtr<FArmySingleDoor>(It.Pin());
            if (Door.IsValid())
            {
                Door->ReplaceGoods(CommonSingleDoorItem, GGI->GetWorld());
            }
        }

        // 将推拉门还原成白模
        FContentItemPtr CommonSlidingDoorItem = FArmyResourceModule::Get().GetResourceManager()->GetContentItemFromItemID(EResourceType::BlueprintClass, -1);
        TArray< TWeakPtr<FArmyObject> > SlidingDoors;
        FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_SlidingDoor, SlidingDoors);
        for (auto It : SlidingDoors)
        {
            TSharedPtr<FArmySlidingDoor> Door = StaticCastSharedPtr<FArmySlidingDoor>(It.Pin());
            if (Door.IsValid())
            {
                Door->ReplaceGoods(CommonSlidingDoorItem, GGI->GetWorld());
            }
        }

		//@郭子阳
		// 将窗台石还原成白模
		TArray<FObjectWeakPtr> LayOutObjects;
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Window, LayOutObjects);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_FloorWindow, LayOutObjects);
		for (auto It : LayOutObjects)
		{
			TSharedPtr<FArmyWindow> WindowObj = StaticCastSharedPtr<FArmyWindow>(It.Pin());
			WindowObj->SetContentItem(nullptr);

		}
		LayOutObjects.Empty();
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_RectBayWindow, LayOutObjects);
		FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_TrapeBayWindow, LayOutObjects);
		for (auto It : LayOutObjects)
		{
			TSharedPtr<FArmyRectBayWindow> WindowObj = StaticCastSharedPtr<FArmyRectBayWindow>(It.Pin());
			WindowObj->SetContentItem(nullptr);
		}


    }
}

void FArmyHardModeController::OnRightSelected()
{
	//ShowRightMenu(true);
}

void FArmyHardModeController::ShowRightMenu(bool bShow)
{
	//if (bShow)
	//{
	//	TArray<AActor*> SelectedActors;
	//	bool bValidActors = true;
	//	bool bValidMaterial = false;
	//	GXREditor->GetSelectedActors(SelectedActors);
	//	for (auto& It : SelectedActors)
	//	{
	//		EActorType ActorType = FArmyResourceModule::Get().GetResourceManager()->GetActorType(It);
	//		if (ActorType != EActorType::Blueprint && ActorType != EActorType::Moveable&& ActorType != EActorType::PipeLine && ActorType != EActorType::EnvironmentAsset)
	//		{
	//			bValidActors = false;
	//			break;
	//		}
	//	}
	//	if (!bValidActors && GXREditor->IsMaterialSelected())
	//	{
	//		if (GXREditor->SelectedMaterial)
	//		{
	//			UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(GXREditor->SelectedMaterial);
	//			if (MID)
	//			{
	//				bValidMaterial = true;
	//			}
	//		}
	//	}
	//	const bool bInShouldCloseWindowAfterMenuSelection = true;
	//	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterMenuSelection, FArmyGlobalActionCallBack::Get().GetGlobalCommandsList());
	//	if (bValidActors)
	//	{
	//		//显示右键菜单，绑定删除操作
	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuCopy);
	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuPaste);
	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuDelete);
	//		MenuBuilder.AddMenuSeparator();

	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuMirrorX);
	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuMirrorY);

	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuGroup);
	//		MenuBuilder.AddMenuEntry(FArmyFrameCommands::Get().CommandMenuUnGroup);
 //           MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	//		FSlateApplication::Get().PushMenu(GGI->Window.ToSharedRef(), FWidgetPath(), MenuBuilder.MakeWidget(), FSlateApplication::Get().GetCursorPos(), FPopupTransitionEffect::ContextMenu);
	//	}

	//}
}

void FArmyHardModeController::OnActorMoved(AActor* InActor)
{
	if (InActor->Tags.Contains(TEXT("HydropowerActor")) || InActor->Tags.Contains(TEXT("HydropowerPipeActor")))
	{
		return;
	}


	TArray<AActor*> Actores;
	if (InActor->IsA(AXRGroupActor::StaticClass()))
	{
		AXRGroupActor* GroupActor = Cast<AXRGroupActor>(InActor);
		TArray<AActor*> TempActores;
		GroupActor->GetGroupActors(TempActores);
		for (auto& TempActor : TempActores)
		{
			if (TempActor->Tags.Contains(TEXT("HydropowerActor")) || TempActor->Tags.Contains(TEXT("HydropowerPipeActor")))
				continue;
			Actores.AddUnique(TempActor);
		}
	}
	else
	{
		Actores.AddUnique(InActor);
	}
	if (Actores.Num())
	{
		if (Actores.Num() == 1)
		{
			if (Actores[0]->Tags.Contains(TEXT("AreaHighLight")))
				return;
		}
		SCOPE_TRANSACTION(TEXT("修改模型位置、旋转、缩放"));
		TArray< TWeakPtr<FArmyObject> > ActorItems = FArmySceneData::Get()->GetObjects(E_OtherModel);
		for (auto& Actor : Actores)
		{
			for (auto& It : ActorItems)
			{
				if (It.Pin().IsValid() && It.Pin()->GetType() == EObjectType::OT_ActorItem)
				{
					TSharedPtr<FArmyActorItem> ActorItem = StaticCastSharedPtr<FArmyActorItem>(It.Pin());
					if (ActorItem->Actor == Actor &&
						!(ActorItem->Location == Actor->GetActorLocation() &&
							ActorItem->Rotation == Actor->GetActorRotation() &&
							ActorItem->Scale == Actor->GetActorScale3D()))
					{
						ActorItem->Modify();
						break;
					}
					// 如果Actor没有发生变换，则不记录修改
				}
			}
		}
	}

	//@郭子阳 刷新右侧列表的施工项,因为移动了之后actor所在房间可能变化，施工项随之发生变化
	if (!InActor->Tags.Contains("Immovable"))
	{
		HardModeDetail->ShowSelectedDetial(InActor);
	}
}
void FArmyHardModeController::OnViewModeChanged(int32 InNewValue, int32 InOldValue)
{
	if (IsCurrentModel)
	{
		// 切换模式隐藏光源图标
		SetLightsVisibility(EVisibility::Hidden);

		TSharedPtr<IArmyActorVisitor> TopModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_TopHardMode);

		if (InNewValue == EXRView_TOP)
		{
			TopModeAV->Show(true);
		}
		else if (InNewValue == EXRView_CEILING)
		{
			/* @梁晓菲 立面模式顶面视图显示半透明蒙版*/
			FArmySceneData::Get()->ShowHardModeCeilingOpacity();
			TSharedPtr<IArmyActorVisitor> HardModeCeilingModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardModeCeilingMode);
			HardModeCeilingModeAV->Show(true);
		}
		else
		{
			TopModeAV->Show(false);

			TSharedPtr<IArmyActorVisitor> HardModeAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_HardMode);
			HardModeAV->Show(true);
		}
	}
}
void FArmyHardModeController::OnSelectionChanged(UObject* NewSelection)
{
	/*if (IsCurrentModel)
		HardModeDetail->ShowSelectedDetial(NewSelection);*/
}

void FArmyHardModeController::OnSelectionNone()
{
	if (HardModeDetail.IsValid())
		HardModeDetail->ShowSelectedDetial(nullptr);
}

void FArmyHardModeController::SaveModel(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter, TArray<int32>& Save_FileList)
{
	TArray<FMoveableMeshSaveData> Save_MoveableMeshList;
	TArray<AXRPointLightActor*> Save_PointLightList;
	TArray<AXRSpotLightActor*> Save_SpotLightList;
	TArray<AXRReflectionCaptureActor*> Save_ReflectionCaptureList;
	//TArray<AXRLevelAssetBoardActor*> Save_LevelAssetBoardList;
	//TMap<FString, FMeshMaterialData> Save_BSPMaterialList;

	UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();

	// 遍历场景中所有的有效保存Actor
	for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* TempActor = *ActorItr;
		if (TempActor->Tags.Contains("HydropowerActor") || TempActor->Tags.Contains("HydropowerPipeActor") || TempActor->Tags.Contains(XRActorTag::OriginalPoint))
		{
			continue;
		}

		// 查找Actor的ItemID
		TSharedPtr<FContentItemSpace::FContentItem> ResultSynData = ResMgr->GetContentItemFromID((*ActorItr)->GetSynID());
		EActorType ActorType = ResMgr->GetActorType(*ActorItr);
		FVRSObject* ResultObj = ResMgr->GetObjFromObjID((*ActorItr)->GetObjID());

		int32 ItemID = 0;
		if (ResultSynData.IsValid())
		{
			ItemID = ResultSynData->ID;
		}

		if (ActorType == EActorType::Moveable || ActorType == EActorType::Blueprint)
		{
			AXRActor* Owner = Cast<AXRActor>(ActorItr->GetOwner());
			TSharedPtr<FArmyObject> Object = nullptr;
			if (Owner&&Owner->IsValidLowLevel())
			{
				Object = Owner->GetRelevanceObject().Pin();
			}
			Save_MoveableMeshList.Add(FMoveableMeshSaveData(ItemID, ActorItr->GetActorLocation(), ActorItr->GetActorRotation(), ActorItr->GetActorScale3D(), Object));
			Save_FileList.AddUnique(ItemID);

			// 查看模型上的材质参数是否需要保存
			UMeshComponent* TheMeshCOM = NULL;
			TArray<UMaterialInterface*> Materials;
			TArray<FName> MaterialSlotNames;
			TInlineComponentArray<USceneComponent*> SceneComponents;
			(*ActorItr)->GetComponents(SceneComponents);

			if (!ResultObj)
			{
				continue;
			}
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
							Save_FileList.AddUnique(MatResultSynData->ID);
						}
					}
				}
			}
		}
		else if (ActorType == EActorType::EnvironmentAsset)
		{
			AXRPointLightActor* TargetActor = Cast<AXRPointLightActor>(*ActorItr);
			if (TargetActor)
			{
				Save_PointLightList.Add(TargetActor);
				continue;
			}

			AXRSpotLightActor* TargetActor2 = Cast<AXRSpotLightActor>(*ActorItr);
			if (TargetActor2)
			{
				Save_SpotLightList.Add(TargetActor2);
				continue;
			}

			AXRReflectionCaptureActor* TargetActor3 = Cast<AXRReflectionCaptureActor>(*ActorItr);
			if (TargetActor3)
			{
				Save_ReflectionCaptureList.Add(TargetActor3);
				continue;
			}

			//AXRLevelAssetBoardActor* TargetActor4 = Cast<AXRLevelAssetBoardActor>(*ActorItr);
			//if (TargetActor4)
			//{
			//	Save_LevelAssetBoardList.Add(TargetActor4);
			//	continue;
			//}
		}
	}

	// 写入开始
	//JsonWriter->WriteObjectStart(TEXT("softMode"));

	// 版本号
	//JsonWriter->WriteValue(TEXT("version"), GResourceVersion);

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

	// 点光源
	JsonWriter->WriteArrayStart(TEXT("pointLightList"));
	for (auto& It : Save_PointLightList)
	{
		FJsonResourceParer::Write_PointLight(JsonWriter, It, NULL);
	}
	JsonWriter->WriteArrayEnd();

	// 射灯
	JsonWriter->WriteArrayStart(TEXT("spotLightList"));
	for (auto& It : Save_SpotLightList)
	{
		FJsonResourceParer::Write_SpotLight(JsonWriter, It, NULL);
	}
	JsonWriter->WriteArrayEnd();

	// 反射球
	JsonWriter->WriteArrayStart(TEXT("reflectionSphereList"));
	for (auto& It : Save_ReflectionCaptureList)
	{
		FJsonResourceParer::Write_ReflectionCapture(JsonWriter, It, NULL);
	}
	JsonWriter->WriteArrayEnd();

	//@打扮家 XRLightmass 序列换 静态平行光
	JsonWriter->WriteValue("StaticSunLightMapID", FArmyWorldManager::Get().StaticSunLight->GetLightComponent()->LightGuid.ToString());

	// 写入结束
	//JsonWriter->WriteObjectEnd();
}

void FArmyHardModeController::LoadModel(TSharedPtr<FJsonObject> SoftModeData)
{
	if (SoftModeData.IsValid())
	{
		// 点光源
		const TArray<TSharedPtr<FJsonValue>> PointLightListData = SoftModeData->GetArrayField(TEXT("pointLightList"));
		for (auto& It : PointLightListData)
		{
			FJsonResourceParer::Read_PointLight(GVC->GetWorld(), It->AsObject());
		}

		TArray<FMoveableMeshSaveData> Save_MoveableMeshList;

		//TSharedPtr<FJsonObject> SoftModeData = Data->GetObjectField("softMode");

		//读取移动家具信息
		const TArray<TSharedPtr<FJsonValue>> MoveableMeshListData = SoftModeData->GetArrayField(TEXT("moveableMeshList"));
		for (auto& It : MoveableMeshListData)
		{
			TSharedPtr<FJsonObject> ItObject = It->AsObject();
			if (ItObject.IsValid())
			{
				int32 ID = ItObject->GetIntegerField(TEXT("id"));

                // 手动生成的模型需要略过，否则会在此重复生成相同模型
                if (FArmySceneData::Get()->IgnoredModelIndexes.Contains(ID))
                {
                    continue;
                }

				FVector Location = FVector::ZeroVector;
				Location.InitFromString(ItObject->GetStringField(TEXT("location")));
				FRotator Rotation = FRotator::ZeroRotator;
				Rotation.InitFromString(ItObject->GetStringField(TEXT("rotation")));
				FVector Scale3D = FVector::ZeroVector;
				Scale3D.InitFromString(ItObject->GetStringField(TEXT("scale")));
				int32 ToggleMode = ItObject->GetIntegerField(TEXT("toggleMode"));
				//@飞舞轻扬
				TSharedPtr<FArmyFurniture> NewFurniture = nullptr;
				if (ItObject->HasField("object"))
				{
					const TSharedPtr<FJsonObject> ObjectJson = ItObject->GetObjectField(TEXT("object"));
					NewFurniture = MakeShareable(new FArmyFurniture);
					NewFurniture->Deserialization(ObjectJson);
					//FArmySceneData::Get()->Add(NewFurniture,XRArgument(1).ArgUint32(E_HydropowerModel),nullptr,true);
				}
				Save_MoveableMeshList.Add(FMoveableMeshSaveData(ID, Location, Rotation, Scale3D, NewFurniture, ToggleMode));

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

		

		// 射灯
		const TArray<TSharedPtr<FJsonValue>> SpotLightListData = SoftModeData->GetArrayField(TEXT("spotLightList"));
		for (auto& It : SpotLightListData)
		{
			FJsonResourceParer::Read_SpotLight(GVC->GetWorld(), It->AsObject());
		}

		// 反射球
		const TArray<TSharedPtr<FJsonValue>> ReflectionCaptureListData = SoftModeData->GetArrayField(TEXT("reflectionSphereList"));
		for (auto& It : ReflectionCaptureListData)
		{
			FJsonResourceParer::Read_ReflectionCapture(GVC->GetWorld(), It->AsObject());
		}

		UXRResourceManager* ResMgr = FArmyResourceModule::Get().GetResourceManager();
		//ResMgr->CurWorld = GVC->GetWorld();

		// 加载模型
		for (auto& It : Save_MoveableMeshList)
		{
			AActor* CreatedActor = ResMgr->CreateActorFromID(GVC->GetWorld(), It.ItemID, "", It.Location, It.Rotation, It.Scale3D);
			if (CreatedActor)
			{
				FVRSObject* ResultObj = ResMgr->GetObjFromObjID(CreatedActor->GetObjID());
				FContentItemPtr ResultSynData = ResMgr->GetContentItemFromID(CreatedActor->GetSynID());
				CreatedActor->SetFolderPath(FName(*(LeftPanel->FindObjectGroup(ResultSynData->CategryID))));
				CreatedActor->SetActorLabel(ResultSynData->Name);
				if (ResultObj && ResultSynData.IsValid())
				{
					if (ResultSynData->bIsSale)
					{
						CreatedActor->Tags.Add(FName("IsSale"));
					}
					if (ResultSynData->ItemFlag& EContentItemFlag::PF_OrignalCenter)
					{
						CreatedActor->Tags.Add(FName("POINTOBJECT"));
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
									for (auto& ParaIt : MatResultObj->MaterialList[0].ParaList)
									{
										//恢复默认值
										ParaIt.ResetToServer();
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
					}
				}
				/** @梁晓菲 加载方案时获取模型的施工项数据*/
				//TSharedPtr<FJsonObject> TempJsonObj = FArmySceneData::Get()->GetJsonDataByItemID(It.ItemID);
				//if (!TempJsonObj.IsValid())
				//{
				//	//ReqConstructionData(It.ItemID);
				//}
			}
		}

		TArray<AActor*> ActorList;
		// 遍历场景中所有的有效保存Actor
		for (TActorIterator<AActor> ActorItr(GVC->GetWorld()); ActorItr; ++ActorItr)
		{
			AActor* TempActor = *ActorItr;
			if (TempActor->Tags.Contains("HydropowerActor") || TempActor->Tags.Contains("HydropowerPipeActor") || TempActor->Tags.Contains(XRActorTag::OriginalPoint))
			{
				continue;
			}

			EActorType ActorType = FArmyResourceModule::Get().GetResourceManager()->GetActorType(*ActorItr);
			if (ActorType == EActorType::Moveable || ActorType == EActorType::Blueprint)
			{
				ActorList.Add(TempActor);
			}
		}

		for (auto It : ActorList)
		{
			UXRResourceManager* ResMgrObj = FArmyResourceModule::Get().GetResourceManager();
			TSharedPtr<FContentItemSpace::FContentItem> ActorItemInfo = ResMgrObj->GetContentItemFromID(It->GetSynID());
			TSharedPtr<FArmyConstructionItemInterface> TempConstructionData = FArmySceneData::Get()->GetConstructionItemDataByActorVector(It->GetActorLocation(), ActorItemInfo->ID);
			if (TempConstructionData.IsValid())
			{
				FArmySceneData::Get()->AddActorConstructionItemData(It->GetUniqueID(), TempConstructionData);
			}
			else
			{
				FArmySceneData::Get()->AddActorConstructionItemData(It->GetUniqueID(), MakeShareable(new FArmyConstructionItemInterface));
			}
		}

		//@打扮家 XRLightmass 序列化 静态平行光
		FString StaticSunLightMapIDStr = "";
		StaticSunLightMapIDStr = SoftModeData->GetStringField("StaticSunLightMapID");
		FGuid ID;
		FGuid::Parse(StaticSunLightMapIDStr, ID);
		if (ID.IsValid())
		{
			FArmyWorldManager::Get().StaticSunLight->GetLightComponent()->LightGuid = ID;
		}
	}
}

void FArmyHardModeController::On3DTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes, AActor* InActor, EModeIdent InModeIdent)
{
	if (InModeIdent == EModeIdent::MI_SoftHardMode)
	{
		ThreeTo2DGanged(InRes, InActor);
	}
}

void FArmyHardModeController::OnGloabalAction()
{
	TSharedPtr<FArmyDesignModeController> CurrentController = nullptr;
	switch (CurrentEditMode)
	{
	case HardModeEditMode::HO_FreeEdit:
		CurrentController = this->AsShared();
		break;
	case HardModeEditMode::HO_FloorEdit:
		CurrentController = HardModeFloorController;
		break;
	case HardModeEditMode::HO_WallEdit:
		CurrentController = HardModeWallController;
		break;
	case HardModeEditMode::HO_RoofEdit:
		CurrentController = HardModeCeilingController;
		break;
	}

	if (CurrentController->IsCurrentOperration((uint8)EHardModeOperation::HO_Replace)
		|| CurrentController->IsCurrentOperration((uint8)EHardModeOperation::HO_MaterialBrush)
		)
	{
		CurrentController->GetCurrentOperation()->EndOperation();
	}
}

void FArmyHardModeController::ThreeTo2DGanged(TSharedPtr<FContentItemSpace::FComponentRes> InRes, AActor* InActor, TSharedPtr<FArmyObject> Object /*= nullptr*/)
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


	if (ObjectLayers.Num() > 0)
	{
		TSharedPtr<FArmyFurniture>CurrentObj = Object.IsValid() ? StaticCastSharedPtr<FArmyFurniture>(Object) : ObjectLayers[0];
		CurrentObj->SetType(OT_ComponentBase);
		CurrentObj->BelongClass = InRes->TypeID;
		CurrentObj->ComponentType = InRes->ComponentID;
		switch (InRes->TypeID)
		{
		case 8://开关
		case 9://插座
			CurrentObj->SetAltitude(135);//默认开关1350毫米的高度
			break;
		default:
			break;
		}

		FVector Pos = InActor->GetActorLocation();
		Pos.Z = 0;
		FBox SizeBox = CurrentObj->GetPreBounds();
		float XSize = SizeBox.GetSize().X * 10;
		float YSize = SizeBox.GetSize().Y * 10;

		FVector ComponentScale(1, 1, 1);
		if (XSize <= 0 || YSize <= 0)
		{
			ComponentScale = InActor->GetActorScale3D();
			ComponentScale.Z = 1;
		}
		else
		{
			ComponentScale.X = InRes->ProductLength / XSize;
			ComponentScale.Y = InRes->ProductWidth / YSize;
		}
		//else if (XSize >= YSize)
		//{
		//	ComponentScale.X = InRes->ProductLength / XSize;
		//	ComponentScale.Y = InRes->ProductWidth / YSize;
		//}
		//else
		//{
		//	ComponentScale.X = InRes->ProductWidth / XSize;
		//	ComponentScale.Y = InRes->ProductLength / YSize;
		//}
		CurrentObj->SetTransform(FTransform(InActor->GetActorRotation(), Pos, ComponentScale));

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

		FArmySceneData::Get()->Add(CurrentObj, XRArgument(1).ArgUint32(E_ConstructionModel), nullptr, true);

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.Name = FName(*(TEXT("NOLIST-") + FGuid::NewGuid().ToString()));
		AXRActor* Owner = GVC->GetWorld()->SpawnActor<AXRActor>(SpawnInfo);
		InActor->SetOwner(Owner);
		Owner->SetRelevanceObject(CurrentObj);
		CurrentObj->SetRelevanceActor(InActor);
		//InActor->OnDestroyed.Remove(G3DM,FName(TEXT("")));
		InActor->OnDestroyed.AddUniqueDynamic(G3DM, &UXR3DManager::OnOwnerDestroyed);
		InActor->GetRootComponent()->TransformUpdated.AddRaw(this, &FArmyHardModeController::GangedTransformUpdated);
		GangedTransformUpdated(InActor->GetRootComponent(), EUpdateTransformFlags::None, ETeleportType::None);
		// 添加cad绘制
		FArmyAutoCad::Get()->AddComponent(CurrentObj->ComponentType, CurrentObj);
	
		//@郭子阳 请求施工项
		//int32 TempItemID = FArmyResourceModule::Get().GetResourceManager()->GetItemIDFromActor(InActor);
		XRConstructionManager::Get()->TryToFindConstructionData(CurrentObj->GetUniqueID(), CurrentObj->GetConstructionParameter(), nullptr);
		
	}
}

void FArmyHardModeController::GangedTransformUpdated(USceneComponent* InRootComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	AActor* TheActor = InRootComponent->GetAttachmentRootActor();
	AXRActor* Owner = Cast<AXRActor>(TheActor->GetOwner());
	if (Owner)
	{
		FObjectWeakPtr RObj = Owner->GetRelevanceObject();
		if (RObj.IsValid())
		{
			FVector Pos = TheActor->GetActorLocation();
			float Height = Pos.Z;
			Pos.Z = 0;

			TSharedPtr<FArmyFurniture> ComponentObj = StaticCastSharedPtr<FArmyFurniture>(RObj.Pin());
			ComponentObj->SetAltitude(Height);

			/* @梁晓菲 图纸模式下，点位图例不应该跟着实际模型大小走，软装类控件跟着实际模型走*/
			if (ComponentObj->GetPropertyFlag(FArmyObject::FLAG_POINTOBJ))
			{
				ComponentObj->SetTransform(FTransform(TheActor->GetActorRotation(), Pos, FVector(1, 1, 1)));
			}
			else
			{
				FVector Scale3D = TheActor->GetActorScale3D();
				Scale3D.X = Scale3D.X > 0 ? 1 : -1;
				Scale3D.Y = Scale3D.Y > 0 ? 1 : -1;
				Scale3D.Z = 1;

				ComponentObj->SetTransform(FTransform(TheActor->GetActorRotation(), Pos, ComponentObj->LocalTransform.GetScale3D() * Scale3D));


				//
				TArray<FObjectWeakPtr> RoomArray;
				FArmySceneData::Get()->GetObjects(EModelType::E_LayoutModel, EObjectType::OT_InternalRoom, RoomArray);
				for (auto R : RoomArray)
				{
					TSharedPtr<FArmyRoom> RPtr = StaticCastSharedPtr<FArmyRoom>(R.Pin());
					if (RPtr->IsPointInRoom(Pos))
					{
						TArray<TSharedPtr<FArmyLine>> RoomLineArray;
						RPtr->GetLines(RoomLineArray);

						FBox ObjBox = ComponentObj->GetBounds();
						ObjBox.Min.Z = 0;
						ObjBox.Max.Z = 0;

						TArray<FVector> ObjBoxVArray{ ObjBox.Min,ObjBox.Max,FVector(ObjBox.Min.X, ObjBox.Max.Y, 0),FVector(ObjBox.Max.X, ObjBox.Min.Y, 0) };

						FBox ActorBox = GVC->GetActorCollisionBox(TheActor);
						ActorBox.Min.Z = 0;
						ActorBox.Max.Z = 0;
						TArray<FVector> ActorBoxVArray{ ActorBox.Min,ActorBox.Max,FVector(ActorBox.Min.X, ActorBox.Max.Y, 0),FVector(ActorBox.Max.X, ActorBox.Min.Y, 0) };

						int32 MinVIndex = INDEX_NONE, MinIndex1 = INDEX_NONE, MinIndex2 = INDEX_NONE;
						float MinDistance1 = 9999999, MinDistance2 = 9999999;
						float MinArea = MinDistance1 * MinDistance2;
						FVector MinDirection1(0, 0, 0);
						FVector MinDirection2(0, 0, 0);

						for (int32 i = 0; i < ActorBoxVArray.Num(); ++i)
						{
							const FVector& V = ActorBoxVArray[i];

							int32 Index1 = -1, Index2 = -1;
							float Distance1 = 9999999, Distance2 = 9999999;
							FVector Direction1(0, 0, 0);
							FVector Direction2(0, 0, 0);

							for (int32 j = 0; j < RoomLineArray.Num(); ++j)
							{
								const TSharedPtr<FArmyLine> L = RoomLineArray[j];

								FVector ClosePoint = FMath::ClosestPointOnInfiniteLine(L->GetStart(), L->GetEnd(), V);
								FVector LDir = (ClosePoint - V).GetSafeNormal();

								bool Parallel = ((LDir + Direction1).Size() < 0.001 || (LDir - Direction1).Size() < 0.001);
								float Len = (ClosePoint - V).Size();
								if (Len < Distance1)
								{
									if (!Direction1.IsZero())
									{
										if (Distance1 < Distance2 && !Parallel)
										{
											Index2 = Index1;
											Distance2 = Distance1;
											Direction2 = Direction1;
										}
									}
									Distance1 = Len;
									Direction1 = LDir;
									Index1 = j;
								}
								else if (Len < Distance2 && !Parallel)
								{
									Distance2 = Len;
									Direction2 = LDir;
									Index2 = j;
								}
							}
							float CurrentArea = Distance1 * Distance2;
							if (CurrentArea < MinArea)
							{
								MinVIndex = i;
								MinIndex1 = Index1;
								MinIndex2 = Index2;
								MinDistance1 = Distance1;
								MinDistance2 = Distance2;
								MinArea = CurrentArea;
								MinDirection1 = Direction1;
								MinDirection2 = Direction2;
							}
						}
						///////////////////////
						if (MinIndex1 != INDEX_NONE && MinIndex2 != INDEX_NONE)
						{
							const TSharedPtr<FArmyLine> L1 = RoomLineArray[MinIndex1];
							const TSharedPtr<FArmyLine> L2 = RoomLineArray[MinIndex2];

							FVector V = ObjBoxVArray[MinVIndex];

							FVector ClosePoint1 = FMath::ClosestPointOnInfiniteLine(L1->GetStart(), L1->GetEnd(), V);
							FVector ClosePoint2 = FMath::ClosestPointOnInfiniteLine(L2->GetStart(), L2->GetEnd(), V);

							float Dis1 = (ClosePoint1 - V).Size();
							float Dis2 = (ClosePoint2 - V).Size();

							FVector Dir1 = (ClosePoint1 - V).GetSafeNormal();
							FVector Dir2 = (ClosePoint2 - V).GetSafeNormal();

							FVector Offset(0, 0, 0);
							if ((Dir1 - MinDirection1).Size() < 0.5)
							{
								Offset += MinDirection1 * (Dis1 - MinDistance1);
							}
							else
							{
								Offset += Dir1 * (Dis1 + MinDistance1);
							}

							if ((Dir2 - MinDirection2).Size() < 0.5)
							{
								Offset += MinDirection2 * (Dis2 - MinDistance2);
							}
							else
							{
								Offset += Dir2 * (Dis2 + MinDistance2);
							}
							ComponentObj->ApplyTransform(FTransform(Offset));
						}
						break;
					}
				}





				//TMap<FString, TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> > mapModelXYMinDis;
				//TMap<FString, TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>> > mapComponentXYMinDis;
				//FVector PosOffset = FVector::ZeroVector;

				//CalModelMinDis(TheActor, nullptr, mapModelXYMinDis, mapComponentXYMinDis);

				//if (ComponentObj.IsValid())
				//{
				//	CalModelMinDis(nullptr, ComponentObj, mapModelXYMinDis, mapComponentXYMinDis);

				//	if (mapModelXYMinDis.Num() == 2 && mapComponentXYMinDis.Num() == 2)
				//	{
				//		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator XModelMinIt(*mapModelXYMinDis.Find(TEXT("XMin")));
				//		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator YModelMinIt(*mapModelXYMinDis.Find(TEXT("YMin")));
				//		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator XComMinIt(*mapComponentXYMinDis.Find(TEXT("XMin")));
				//		TMap<TSharedPtr<FArmyLine>, TSharedPtr<FArmyLine>>::TConstIterator YComMinIt(*mapComponentXYMinDis.Find(TEXT("YMin")));
				//		TSharedPtr<FArmyLine>XModelLine = XModelMinIt.Value();
				//		TSharedPtr<FArmyLine>YModelLine = YModelMinIt.Value();
				//		TSharedPtr<FArmyLine>XComLine = XComMinIt.Value();
				//		TSharedPtr<FArmyLine>YComLine = YComMinIt.Value();
				//		PosOffset.X = XModelLine->GetStart().X - XComLine->GetStart().X;
				//		PosOffset.Y = YModelLine->GetStart().Y - YComLine->GetStart().Y;
				//		ComponentObj->ApplyTransform(FTransform(PosOffset));
				//	}
				//}
			}
		}
	}
}

bool FArmyHardModeController::SaveMapBuildData(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	bool SaveResult = false;
	FString TempFilePath = FString::Printf(TEXT("%sTempLevelData.uasset"), *FResTools::GetLevelPlanDir());
	IPlatformFile::GetPlatformPhysical().DeleteFile(*TempFilePath);

	if (FStaticLightingManager::Get()->SaveMapBuildData(TempFilePath))
	{
		//删除所有与该方案相关的光照图文件
		FString FileNameMatchedID = FString::Printf(TEXT("%d_*LevelData.uasset"), FArmyUser::Get().GetCurPlanID());
		TArray<FString> Filenames;
		IFileManager::Get().FindFilesRecursive(Filenames, *FResTools::GetLevelPlanDir(), *FileNameMatchedID, true, false);
		for (auto It : Filenames)
		{
			IPlatformFile::GetPlatformPhysical().DeleteFile(*It);
		}

		FString NewGUID = FGuid::NewGuid().ToString();
		FString FileName = FString::Printf(TEXT("%d_%s_LevelData.uasset"), FArmyUser::Get().GetCurPlanID(), *NewGUID);
		FString FinalFilePath = FString::Printf(TEXT("%s%s"), *FResTools::GetLevelPlanDir(), *FileName);
		if (IPlatformFile::GetPlatformPhysical().MoveFile(*FinalFilePath, *TempFilePath))
		{
			JsonWriter->WriteValue("LevelDataFileName", FileName);
			SaveResult = true;
		}
	}
	//如果前面if中保存失败，再看是不是当前是否已加载了渲染数据文件，不要忘记把渲染文件名存入方案Json中
	else if (!LoadedLevelDataFilePath.IsEmpty() && FStaticLightingManager::Get()->IsValidLightMap())
	{
		JsonWriter->WriteValue("LevelDataFileName", LoadedLevelDataFilePath);
	}
	else
	{
		//删除所有与该方案相关的光照图文件
		FString FileNameMatchedID = FString::Printf(TEXT("%d_*LevelData.uasset"), FArmyUser::Get().GetCurPlanID());
		TArray<FString> Filenames;
		IFileManager::Get().FindFilesRecursive(Filenames, *FResTools::GetLevelPlanDir(), *FileNameMatchedID, true, false);
		for (auto It : Filenames)
		{
			IPlatformFile::GetPlatformPhysical().DeleteFile(*It);
		}
	}

	return SaveResult;
}

void FArmyHardModeController::LoadMapBuildData(FString InFilePath)
{
	if (FStaticLightingManager::Get()->LoadMapBuildData(InFilePath))
	{
		FArmyWorldManager::Get().SetEnvironmentMode(false);

		for (TActorIterator<AStaticMeshActor> It(GWorld); It; ++It)
		{
			if (It && It->Tags.Num() > 0 && It->Tags[0] == TEXT("MoveableMeshActor"))
			{
				for (UMaterialInterface* It : It->GetStaticMeshComponent()->OverrideMaterials)
				{
					UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(It);
					if (MID)
					{
						MID->SetScalarParameterValue("Shadow_ON", FArmyWorldManager::Get().bBuildStaticMesh ? 0.f : 1.f);
					}
				}
			}
		}
	}
	//SetStaticLightEnvironment();
}

void FArmyHardModeController::TryLoadMapBuildData()
{
	//查找与方案匹配的渲染数据文件
	FString FileNameMatchedID = FString::Printf(TEXT("%d_*LevelData.uasset"), FArmyUser::Get().GetCurPlanID());
	TArray<FString> Filenames;
	IFileManager::Get().FindFilesRecursive(Filenames, *FResTools::GetLevelPlanDir(), *FileNameMatchedID, true, false);

	//该方案有关联过渲染数据文件
	if (!LoadedLevelDataFilePath.IsEmpty())
	{
		//查找到了
		if (Filenames.Num() > 0)
		{
			//看下版本如果太低，则弹框提示清除掉
			int32 VersionInt = FArmyUser::Get().GetApplicationVersionInt();
			if (VersionInt < LightmassLastSupportedVersion)
			{
				FSimpleDelegate OnConfirm;
				OnConfirm.BindLambda([this, Filenames]() {
					GGI->Window->DismissModalDialog();
					ClearStaticLighitngData();
					for (auto It : Filenames)
						IPlatformFile::GetPlatformPhysical().DeleteFile(*It);
				});

				FSimpleDelegate OnCancel;
				OnCancel.BindLambda([this, Filenames]() {
					GGI->Window->DismissModalDialog();
					LoadMapBuildData(Filenames[0]);
				});

				GGI->Window->PresentModalDialog(TEXT("当前方案的渲染效果数据已不兼容此版本，会引起错误的效果，是否清空渲染效果（方案不会影响）？"),
					OnConfirm, OnCancel, OnCancel,
					TEXT("清空渲染效果"), TEXT("不清空"), false);
			}
			//对比本地文件名与方案中记录的文件名，如果一致，尝试加载
			else if (Filenames[0].Contains(LoadedLevelDataFilePath))
			{
				LoadMapBuildData(Filenames[0]);
			}
			//如果不一致，则弹框提示删除
			else
			{
				FSimpleDelegate OnConfirm;
				OnConfirm.BindLambda([this, Filenames]() {
					GGI->Window->DismissModalDialog();
					ClearStaticLighitngData();
					for (auto It : Filenames)
						IPlatformFile::GetPlatformPhysical().DeleteFile(*It);
				});

				FSimpleDelegate OnCancel;
				OnCancel.BindLambda([this, Filenames]() {
					GGI->Window->DismissModalDialog();
					LoadMapBuildData(Filenames[0]);
				});

				GGI->Window->PresentModalDialog(TEXT("当前方案已发生变化，之前渲染过的效果将会出错，是否清空渲染效果（方案不会清空）？"),
					OnConfirm, OnCancel, OnCancel,
					TEXT("清空渲染效果"), TEXT("不清空"), false);
			}
		}
		//没找到，顶部给出警告
		else
		{
			//GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("当前方案关联的渲染效果数据丢失！"));
		}

	}
	//方案未关联任何渲染数据文件，则尝试清空已有的本地渲染文件
	else
	{
		for (auto It : Filenames)
			IPlatformFile::GetPlatformPhysical().DeleteFile(*It);
	}
}

void FArmyHardModeController::OnPointLightCommand()
{
	AXRPointLightActor* NewActor = FArmyResourceModule::Get().GetResourceManager()->AddPointLight(GVC->GetWorld());
	GVC->PlaceActor(NewActor);
}

void FArmyHardModeController::OnSpotLightCommand()
{
	AXRSpotLightActor* NewActor = FArmyResourceModule::Get().GetResourceManager()->AddSpotLight(GVC->GetWorld());
	GVC->PlaceActor(NewActor);
}

void FArmyHardModeController::OnReflectionCaptureCommand()
{
	AXRReflectionCaptureActor* NewActor = FArmyResourceModule::Get().GetResourceManager()->AddReflectionSphere(GVC->GetWorld());
	GVC->PlaceActor(NewActor);
}

void FArmyHardModeController::OnDisplayLightIconCommand()
{
	bIsDisplayLightIconChecked = !bIsDisplayLightIconChecked;

	// 设置是否显示光源图标和反射球图标
	TSharedPtr<IArmyActorVisitor> LightIconAV = FArmyActorVisitorFactory::Get().CreateVisitor(AV_LightIcon);
	LightIconAV->Show(bIsDisplayLightIconChecked);
}

bool FArmyHardModeController::IsDisplayLightIconChecked()
{
	return bIsDisplayLightIconChecked;
}

void FArmyHardModeController::LockCamera(const XRArgument& InArg, FObjectPtr editArea, bool bTransaction/*=false*/)
{
	if (editArea.IsValid())
	{
		if (editArea->GetType() == OT_RoomSpaceArea)
			LockCameraToEditSurface(editArea);
		else
		{
			if (editArea->GetParents().Num() > 0)
			{
				TSharedPtr<FArmyRoomSpaceArea> AttachRoom = StaticCastSharedPtr<FArmyRoomSpaceArea>(editArea->GetParents()[0].Pin());
				if (AttachRoom.IsValid())
					LockCameraToEditSurface(AttachRoom);
			}
		}

		SetOperation((uint8)EHardModeOperation::HO_None);
	}
}

/**@常远 初始化颜志包中模型的基本信息 */
void FArmyHardModeController::InitDesignPackageMap(TArray<TSharedPtr<class FContentItemSpace::FContentItem>> InContentItemList, TMap<int32, TSharedPtr<class FArmyDesignPackage>> InDesignPackageMap)
{
	//auto contentItem = Downloader->GetPakageContentItems();
	for (auto Iter = InDesignPackageMap.CreateIterator(); Iter; ++Iter)
	{
		TSharedPtr<FArmyDesignPackage> TmpPackage = Iter->Value;
		if (!TmpPackage.IsValid())
		{
			continue;
		}

		for (auto &It : TmpPackage->GetAllModelList())
		{
			for (auto ContentItem : InContentItemList)
			{
				if (ContentItem.IsValid() && It->ItemID == ContentItem->ID)
				{
					FVector ItemVecotr = FArmyDataTools::GetContextItemSize(ContentItem);
					It->Width = ItemVecotr.Y;
					It->Height = ItemVecotr.Z;
					It->Length = ItemVecotr.X;
					It->FurContentItem = ContentItem;
					break;
				}
			}
		}
	}
}

float FArmyHardModeController::GetOutlinerWidgetHeight() const
{
    if (HardModeDetail->GetPropertyPanelVisibility() == EVisibility::Collapsed ||
        HardModeDetail->GetPropertyPanelVisibility() == EVisibility::Hidden)
    {
        return 1.f;
    }

    return OutlinerWidgetHeight;
}

float FArmyHardModeController::GetPropertyWidgetHeight() const
{
    if (HardModeDetail->GetPropertyPanelVisibility() == EVisibility::Collapsed ||
        HardModeDetail->GetPropertyPanelVisibility() == EVisibility::Hidden)
    {
        return 0.f;
    }

    return PropertyWidgetHeight;
}

// 应用大师方案相应函数
void FArmyHardModeController::OnApplayMasterPlan(TSharedPtr<FContentItemSpace::FContentItem> InContentItem, ApplayExtentS InApplayExtent)
{
	TSharedPtr<SArmyContentBrowser> ContentB = CategoryOrgManage->GetCurrentSelectCat()->GetCurrentBrowser().Pin();
	TSharedPtr<SArmyMasterPlanContentBrowser> MasterB = StaticCastSharedPtr<SArmyMasterPlanContentBrowser>(ContentB);
	if (MasterB.IsValid())
	{
		FArmyApplyMasterPlanManage::OnMasterPlanDownLoadFinishedDelegate.BindSP(this,&FArmyHardModeController::OnApplayMasterPlanNoDownload, InApplayExtent);
		MasterB->DownloadDataFromContentItem(InContentItem,InApplayExtent);
	}
}

// 应用大师方案，不包含下载
void FArmyHardModeController::OnApplayMasterPlanNoDownload(bool bDownloadSuccess, TSharedPtr<FContentItemSpace::FContentItem> InContentItem, ApplayExtentS InApplayExtent)
{
	/** 大师方案统计 */
	FString url = FString::Printf(TEXT("/api/aiPlans/%d/clickApply"), InContentItem->ID);
	FArmyHttpRequestCompleteDelegate CompleteDelegate;
	IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest(url, CompleteDelegate);
	Request->ProcessRequest();

	TArray<TSharedPtr<FResObj> > ResArr = InContentItem->GetResObjNoComponent();
	TSharedPtr<FArmyGreatPlanRes> GreatPlanRes = StaticCastSharedPtr<FArmyGreatPlanRes>(ResArr[0]);
	FString PlanStr = GreatPlanRes->DesignPackageStr;

	TArray<TSharedPtr<FContentItemSpace::FContentItem>> contentItemArr;
	TSharedPtr<SArmyContentBrowser> ContentB = CategoryOrgManage->GetCurrentSelectCat()->GetCurrentBrowser().Pin();
	TSharedPtr<SArmyMasterPlanContentBrowser> MasterB = StaticCastSharedPtr<SArmyMasterPlanContentBrowser>(ContentB);
	if (MasterB.IsValid())
	{
		contentItemArr = MasterB->GetSelectedMasterContentItem()->GetMasterPlanContentItemArr();
	}

	// 获得该方案相应的颜值包
	TMap<int32, TSharedPtr<FArmyDesignPackage>> CurrentDesignPackageMap = FArmyAutoDesignModel::Get()->GetDesignPackageMap(PlanStr);
	// 初始化颜值包中每个模型的信息
	InitDesignPackageMap(contentItemArr, CurrentDesignPackageMap);

	AutoDesignInfo DesignInfo = AutoDesignInfo(InApplayExtent.Auxiliary, InApplayExtent.SoftModel, InApplayExtent.HardModel,InApplayExtent.RoomIdList.Num() == 0);
	DesignInfo.RoomGuidList = InApplayExtent.RoomIdList;
	FArmyAutoDesignModel::Get()->AutoDesignExecute(CurrentDesignPackageMap, DesignInfo);
	GGI->Window->ShowMessage(MT_Success, TEXT("大师方案应用完成"));

}
