#include "ArmyHomeModeController.h"
#include "SArmyOperationBrowser.h"
#include "ArmyHomeModeCommands.h"
#include "ArmyViewportClient.h"
#include "ArmyMouseCapture.h"
#include "ArmyRectSelect.h"
#include "ArmyToolsModule.h"
#include "ArmyRoomOperation.h"
#include "ArmyOffsetOperation.h"
#include "ArmyComponentOperation.h"
#include "SArmyObjectOutliner.h"
#include "ArmyLayerManager.h"
#include "MultiBoxBuilder.h"
#include "UICommandList.h"
#include "SArmyDetailComponent.h"
#include "ArmyGameInstance.h"
#include "SArmyWallSetting.h"
#include "SButton.h"
#include "ArmyFacsimileOperation.h"
#include "ArmyHardwareOperation.h"
#include "ArmyFrameCommands.h"
#include "ArmySingleDoor.h"
#include "ArmySlidingDoor.h"
#include "ArmyFloorWindow.h"
#include "ArmyRectBayWindow.h"
#include "ArmyTrapeBayWindow.h"
#include "ArmyCornerBayWindow.h"
#include "ArmyPass.h"
#include "ArmyToolsModule.h"
#include "ArmyRectSelect.h"
#include "ArmyHomeModeDetail.h"
#include "ArmyClipper.h"
#include "ArmySceneData.h"
#include "ArmyReferenceImage.h"
#include "ArmyMath.h"
#include "ArmyModifyWallOperation.h"
#include "ArmyAddWallOperation.h"
#include "ArmyModifyWall.h"
#include "ArmyAddWall.h"
#include "SArmyGenerateOutWall.h"
#include "ArmySingleDoorDetail.h"
#include "ArmyAxisRuler.h"
#include "ArmyPunch.h"
#include "ArmySimpleComponentOperation.h"
#include "ArmySecurityDoor.h"
#include "ArmyBeam.h"
#include "ArmyWallLine.h"
#include "ArmyRoom.h"
#include "ArmyPillar.h"
#include "ArmyAirFlue.h"
#include "ArmyBearingWallOperation.h"
#include "ArmyTextLabelOperation.h"
#include "ArmyActorConstant.h"
#include "Army3DManager.h"
#include "ArmyToolBarBuilder.h"

#define REIMPORT_FACSIMILE_MSG TEXT("导入的底图将替换原有底图，确认导入底图?")

enum EHomeOperation
{
	HO_Room = 0,
	HO_RectRoom,
	HO_OffsetLine,
	HO_Facsimile,
	HO_Hardware,
	HO_Component,
	HO_SimpleComponent,
	HO_BearingWall,//承重墙
	HO_TextLabel//文本
};

FArmyHomeModeController::~FArmyHomeModeController()
{
	FArmyHomeModeCommands::Unregister();

	FArmySceneData::Get()->PreOperationDelegate.RemoveAll(this);
}

const EModelType FArmyHomeModeController::GetDesignModelType()
{
	return EModelType::E_HomeModel;
}

void FArmyHomeModeController::Init()
{
	FArmySceneData::Get()->PreOperationDelegate.AddRaw(this, &FArmyHomeModeController::OnObjectOperation);

	FArmySceneData::HomeMode_ModifyMultiDelegate.AddRaw(this, &FArmyHomeModeController::OnHomeModeModifyOperation);

	// 注册命令代理
	FArmyHomeModeCommands::Register();

	FArmyDesignModeController::Init();

    //HomeModeCommandList = MakeShareable(new FUICommandList);

	AxisRuler = MakeShareable(new FArmyAxisRuler());
	AxisRuler->Init(GVC->ViewportOverlayWidget);
	//@ 点位输入框数值越界后输入框内容重置操作
	AxisRuler->AxisOperationDelegate.BindRaw(this, &FArmyHomeModeController::PreCalTransformIllegalCommon);

	LockWidget = CreateLockWidget();
	GVC->ViewportOverlayWidget->AddSlot()
		.HAlign(EHorizontalAlignment::HAlign_Right)
		.VAlign(EVerticalAlignment::VAlign_Top)
		.Padding(38, 40)
		[
			LockWidget->AsShared()
		];

	//@ 将点位节点加入物体列表中，暂时放置在水电模式下
	TSharedPtr<FObjectGroupContentItem> ObjGroupItem = MakeShareable(new FObjectGroupContentItem);
	ObjGroupItem->ModelName = FArmyActorPath::GetHydropowerPath().ToString();
	ObjGroupItem->ModelID = 2;
	ObjGroupItem->VisibleInOtherModel = true;
	ObjGroupItem->ItemPath = FArmyActorPath::GetOriginalPointPath().ToString();
	SArmyModelContentBrowser::ObjectGroupCategoriesArray.Add(ObjGroupItem);
}

void FArmyHomeModeController::InitOperations()
{
	// @海涛 初始化操作类
	RoomOperation = MakeShareable(new FArmyRoomOperation(E_HomeModel));
	OperationMap.Add(HO_Room, RoomOperation);
	OperationMap.Add(HO_OffsetLine, MakeShareable(new FArmyOffsetOperation(E_HomeModel)));
	OperationMap.Add(HO_Facsimile, MakeShareable(new FArmyFacsimileOperation(E_HomeModel)));
	HardWareOperation = MakeShareable(new FArmyHardwareOperation(E_HomeModel));
	OperationMap.Add(HO_Hardware, HardWareOperation);
	ComponentOperation = MakeShareable(new FArmyComponentOperation(E_HomeModel));
	OperationMap.Add(HO_Component, ComponentOperation);
	OperationMap.Add(HO_SimpleComponent, MakeShareable(new FArmySimpleComponentOperation(E_HomeModel)));
	OperationMap.Add(HO_BearingWall, MakeShareable(new FArmyBearingWallOperation(E_HomeModel)));
	OperationMap.Add(HO_TextLabel, MakeShareable(new FArmyTextLabelOperation(E_HomeModel)));
	for (auto It : OperationMap)
	{
		It.Value->Init();
		It.Value->InitWidget(GVC->ViewportOverlayWidget);
		It.Value->EndOperationDelegate.BindRaw(this, &FArmyHomeModeController::EndOperation);
	}

	GVC->AdsorbDetect.BindUObject(G3DM, &UXR3DManager::CaptureWallInfo);
}

void FArmyHomeModeController::BeginMode()
{
	/**@欧石楠代理选中事件*/
	FArmyToolsModule::Get().GetRectSelectTool()->SelectedObjectsDelegate.BindRaw(this, &FArmyHomeModeController::OnSelectionChanged);

	FArmyObject::SetDrawModel(MODE_MODIFYADD, true);
	FArmyObject::SetDrawModel(MODE_MODIFYDELETE, true);
	FArmyObject::SetDrawModel(MODE_ADDWALLPOST, false);
	FArmyObject::SetDrawModel(MODE_DELETEWALLPOST, false);
	GVC->SetDrawMode(DM_2D);

	/**	设置捕捉的相对平面，在该平面上所有的物体用的是相对该平面的相对坐标*/
	FArmyToolsModule::Get().GetMouseCaptureTool()->Reset();
	FArmyToolsModule::Get().GetMouseCaptureTool()->SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = false;

	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetRectSelectTool()->EnableMultipleSelect(true);
	FArmyToolsModule::Get().GetRectSelectTool()->Start();

	bSwitchMode = false;
	if (LockWidget.IsValid() && !GGI->DesignEditor->bReloading)
	{
		LockWidget->SetVisibility(bLockedHome ? EVisibility::Visible : EVisibility::Hidden);
	}
}

bool FArmyHomeModeController::EndMode()
{
	if (!GGI->DesignEditor->bReloading)
	{
		/**@欧石楠  检查是否已经生成外墙来决定能否切换模式*/
		if (!FArmySceneData::Get()->bHasOutWall)
		{
			GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("原始户型绘制未完成，请生成外墙后切换！"));
			return false;
		}

		/**@欧石楠  检查是否全部房间都已经命名或者选择了直接切换来决定要不要提示未命名问题*/
		if (!CheckRoomName() && !bSwitchMode)
		{
			GGI->Window->PresentModalDialog(
                TEXT("有空间尚未命名！"),
                FSimpleDelegate::CreateRaw(this, &FArmyHomeModeController::OnNotNameSpace),
                FSimpleDelegate(),
				FSimpleDelegate::CreateRaw(this, &FArmyHomeModeController::OnNameSpace),
                TEXT("暂不命名"),
                TEXT("去命名"),
                true);
			return false;
		}
		SetLockHome(true);
		LockWidget->SetVisibility(EVisibility::Hidden);
	}

	AxisRuler->SetBoundObj(nullptr);

	/**@欧石楠取消户型模式下的所有选中*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();

	//清空已选中的空间线的标尺
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		Room->HiddenRulerLine();
	}

    // @欧石楠 切换模式时截取户型图
    GGI->DesignEditor->TakeHomeScreenshot();

	FArmyDesignModeController::EndMode();

	return true;
}

void FArmyHomeModeController::EndOperation()
{
	//AxisRuler->SetBoundObj(nullptr);
	FArmyDesignModeController::EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Start();
}

void FArmyHomeModeController::CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles)
{
	for (auto LayerIt : HomeDataLayers)
	{
		if (LayerIt.Value.Pin()->IsVisible())
		{
			for (auto ObjIt : LayerIt.Value.Pin()->GetObjects())
			{
				if (ObjIt->GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
				{
					TArray< TSharedPtr<FArmyLine> > Lines;
					ObjIt->GetLines(Lines);
					InOutLines.Append(Lines);

					TArray< TSharedPtr<FArmyCircle> > Circles;
					ObjIt->GetCircleLines(Circles);
					InOutCircles.Append(Circles);

					TArray< TSharedPtr<FArmyPoint> > Points;
					ObjIt->GetAlonePoints(Points);
					InOutPoints.Append(Points);

					if (CurrentOperation.IsValid() && CurrentOperation->GetOperationObject() == ObjIt)
					{
						for (auto L : Lines)
						{
							if (L->GetStartPointer() == CurrentOperation->GetOperationPoint() || L->GetEndPointer() == CurrentOperation->GetOperationPoint())
							{
								InOutLines.Remove(L);
							}
						}

						for (auto P : Points)
						{
							if (P == CurrentOperation->GetOperationPoint() || P == CurrentOperation->GetOperationPoint())
							{
								InOutPoints.Remove(P);
							}
						}
					}
				}
			}
		}
	}

	if (CurrentOperation.IsValid()) InOutLines.Append(CurrentOperation->GetLines());
}

TSharedPtr<SWidget> FArmyHomeModeController::MakeLeftPanelWidget()
{
	TSharedPtr<SArmyOperationBrowser> HomeCategory = SNew(SArmyOperationBrowser);

	HomeCategory->BeginOperationCategory(TEXT("建筑"), FArmyStyle::Get().GetBrush("Category.Building"));

	HomeCategory->AddOperationLabel_First(TEXT("墙线"));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandInternalWall, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnInternalWallCommand));
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandBearingWall, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnBearingWallCommand));
    HomeCategory->BuildSubOperationCategory();

    HomeCategory->AddOperationLabel(TEXT("结构"));
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandPass, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPassCommand));
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandBeam, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnComponentCommand, XRArgument().ArgInt32(OT_Beam)));
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandPillar, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnComponentCommand, XRArgument().ArgInt32(OT_Pillar)));
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandAirFlue, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnComponentCommand, XRArgument().ArgInt32(OT_AirFlue)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandAirLouver, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnAirLouverCommand));
	
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandBalconyPunch, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnBalconyPunchCommand));
    HomeCategory->BuildSubOperationCategory();

	HomeCategory->BeginOperationCategory(TEXT("门窗"), FArmyStyle::Get().GetBrush("Category.Window"));

	HomeCategory->AddOperationLabel_First(TEXT("室外门"));
    HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandSecurityDoor, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnSecurityDoorCommand));
    HomeCategory->BuildSubOperationCategory();

    HomeCategory->AddOperationLabel(TEXT("窗户"));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandWindow, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnWindowCommand));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandFloorWindow, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnFloorWindowCommand));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandBayWindow, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnBayWindowCommand));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandTrapezoidBayWindow, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnTrapezoidBayWindowCommand));
    HomeCategory->BuildSubOperationCategory();

	HomeCategory->BeginOperationCategory(TEXT("点位"), FArmyStyle::Get().GetBrush("Category.PointLocation"));

	HomeCategory->AddOperationLabel_First(TEXT("电路点位"));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandHighElvBox, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_EleBoxH_Point)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandLowElvBox, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_EleBoxL_Point)));
    HomeCategory->BuildSubOperationCategory();

    HomeCategory->AddOperationLabel(TEXT("水路点位"));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandEnterRoomWater, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Water_Supply)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandDrainPoint, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Drain_Point)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandClosestool, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Closestool)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandBasin, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Basin)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandWaterHotPoint, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Water_Hot_Point)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandWaterChilledPoint, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Water_Chilled_Point)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandWaterNormalPoint, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Water_Normal_Point)));
    HomeCategory->BuildSubOperationCategory();

    HomeCategory->AddOperationLabel(TEXT("燃气点位"));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandGasMeter, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Gas_Meter)));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandGasMainPipe, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Gas_MainPipe)));
    HomeCategory->BuildSubOperationCategory();

    HomeCategory->AddOperationLabel(TEXT("暖通点位"));
	HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandWaterSeparatorPoint, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Water_Separator_Point)));
	//HomeCategory->AddOperation(FArmyHomeModeCommands::Get().CommandWitch1O2Point, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnPointLocationCommand, XRArgument().ArgInt32(EC_Switch1O2_Point)));
    HomeCategory->BuildSubOperationCategory();

    CommandList->Append(HomeCategory->GetCommandList());

	return HomeCategory;
}

TSharedPtr<SWidget> FArmyHomeModeController::MakeRightPanelWidget()
{
	DetailWidget = MakeShareable(new FArmyHomeModeDetail());

	return
		SNew(SOverlay)

		+ SOverlay::Slot()
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
			            .Text(FText::FromString(TEXT("属性")))
		                .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		            ]
		        ]
		    ]

	        + SVerticalBox::Slot()
		    .VAlign(VAlign_Fill)
		    .Padding(FMargin(0, 0, 16, 0))
		    [
			    DetailWidget->CreateDetailWidget()
		    ]
		];
}

TSharedPtr<SWidget> FArmyHomeModeController::MakeToolBarWidget()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"

	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	FCanExecuteAction CheckHasSelectedXRObj = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedXRObj);
	FCanExecuteAction CheckCanDeleteOutWall = FCanExecuteAction::CreateRaw(this, &FArmyHomeModeController::CheckCanDeleteOutWall);
	FCanExecuteAction CheckCanCreateOutWall = FCanExecuteAction::CreateRaw(this, &FArmyHomeModeController::CheckCanCreateOutWall);

	FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);

	ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandSave, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnSaveCommand));
	//ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandUndo, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnUndoCommand), CheckCanUndo);
	//ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandRedo, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnRedoCommand), CheckCanRedo);
	ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandDelete, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::Delete), CheckHasSelectedXRObj);
	ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandEmpty, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnEmptyCommand));
    ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyHomeModeController::OnImportCommand), LOCTEXT("Import", "导入"), FSlateIcon("ArmyStyle", "HomeMode.CommandImport"));
	ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandOffset, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnOffsetCommand), CheckCanCreateOutWall);
	ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandCreateOutWall, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnOutWallCommand), CheckCanCreateOutWall);
	ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandDeleteOutWall, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnDeleteOutWallCommand), CheckCanDeleteOutWall);

	CommandList->Append(ToolBarBuidler->GetCommandList());

#undef LOCTEXT_NAMESPACE

	return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

void FArmyHomeModeController::Draw(FViewport* InViewport, const FSceneView* InView, FCanvas* SceneCanvas)
{
	FArmyDesignModeController::Draw(InViewport, InView, SceneCanvas);

	for (auto LayerIt : HomeDataLayers)
	{
		if (LayerIt.Value.Pin()->IsVisible())
		{
			for (auto ObjIt : LayerIt.Value.Pin()->GetObjects())
			{
				ObjIt->DrawHUD(GVC, InViewport, InView, SceneCanvas);
			}
		}
	}
}

void FArmyHomeModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	AxisRuler->Draw(PDI, View);
	FArmyDesignModeController::Draw(View, PDI);
	for (auto LayerIt : HomeDataLayers)
	{
		if (LayerIt.Value.Pin()->IsVisible())
		{
			for (auto ObjIt : LayerIt.Value.Pin()->GetObjects())
			{
				ObjIt->Draw(PDI, View);
			}
		}
	}
}

bool FArmyHomeModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (bLockedHome)
	{
		return FArmyDesignModeController::InputKey(Viewport, Key, Event);
	}
	if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
	{
		if ((Event == IE_Pressed || Event == IE_Released) && Key == EKeys::LeftMouseButton)
		{
			// 单选物体不参与鼠标松开事件的逻辑，包括拖拽后的松开
			if (Event == IE_Released && FArmyToolsModule::Get().GetRectSelectTool()->GetState() == 1)
			{
				return false;
			}

			TArray<FObjectWeakPtr> AllObjects;
			for (auto LayerIt : HomeDataLayers)
			{
				if (LayerIt.Value.Pin()->IsVisible())
				{
					AllObjects.Append(LayerIt.Value.Pin()->GetObjects());
				}
			}

			// 不让外墙参与拣选
			for (auto ObjectIt : AllObjects)
			{
				if (ObjectIt.Pin()->GetType() == OT_OutRoom)
				{
					AllObjects.Remove(ObjectIt);
					break;
				}
			}

			AllObjects.Sort([](const FObjectWeakPtr A, const FObjectWeakPtr B)
			{ return (A.Pin()->GetType() == OT_InternalRoom) ? false :
				((B.Pin()->GetType() == OT_InternalRoom) ? true : false); });

			FArmyToolsModule::Get().GetRectSelectTool()->LeftMouseButtonPress(GVC, Viewport->GetMouseX(), Viewport->GetMouseY(), AllObjects);

			//只有presse事件才进入operation相关操作
			if (Event == IE_Pressed && !CurrentOperation.IsValid())
			{
				AxisRuler->SetBoundObj(nullptr);
				FArmyRectSelect::OperationInfo CurrentOperationInfo = FArmyToolsModule::Get().GetRectSelectTool()->GetCurrentOperationInfo();

				/**@欧石楠 如果选中了控件，更新一遍捕捉信息*/
				if (FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects().Num() > 0)
				{
					TSharedPtr<FArmyObject> TempObj = CurrentOperationInfo.CurrentOperationObject.Pin();
					if (TempObj.IsValid() && (TempObj->GetType() == OT_Window
						|| TempObj->GetType() == OT_FloorWindow
						|| TempObj->GetType() == OT_RectBayWindow
						|| TempObj->GetType() == OT_TrapeBayWindow
						|| TempObj->GetType() == OT_SecurityDoor
						|| TempObj->GetType() == OT_Pass
						|| TempObj->GetType() == OT_Punch))
					{
						TSharedPtr<FArmyHardware> TempHW = StaticCastSharedPtr<FArmyHardware>(TempObj);
						HardWareOperation->ForceCaptureDoubleLine(TempHW);
						TempHW->Update();
						if (TempObj->GetType() == OT_SecurityDoor)
						{
							TSharedPtr<FArmySingleDoor> TempHWDoor = StaticCastSharedPtr<FArmySingleDoor>(TempHW);
							TempHWDoor->UpdateDoorHole();
						}

					}

					TSharedPtr<FArmyObject> TempFurObj = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects()[0].Pin();
					if (TempFurObj->GetType() == OT_ComponentBase)
					{
						TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(TempFurObj);
						if (Furniture.IsValid() && Furniture->GetPlaceLocationFlag() == EPLF_ROOMSPACE)
						{
							AxisRuler->SetBoundObj(TempFurObj);
						}
					}

					if (TempFurObj->GetType() == OT_TextLabel)
					{

						SetOperation(HO_TextLabel, XRArgument(1));
						CurrentOperation->SetSelected(TempFurObj, nullptr);
						FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
					}
				}

				if (CurrentOperationInfo.IsValid())
				{
					switch (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType())
					{
					case OT_InternalRoom:
					case OT_OutRoom:
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							SetOperation(HO_Room, XRArgument(2));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
							FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
						}
						return false;

					case OT_Window:
					case OT_FloorWindow:
					case OT_RectBayWindow:
					case OT_TrapeBayWindow:
					case OT_CornerBayWindow:
					case OT_SecurityDoor:
					case OT_Pass:
					case OT_Punch:
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							SetOperation(HO_Hardware, XRArgument(OT_None));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
						}
						break;

					case OT_Beam:
					case OT_Pillar:
					case OT_AirFlue:
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							SetOperation(HO_SimpleComponent, XRArgument(OT_None));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
							FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
						}
						break;
					case OT_ComponentBase:
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							SetOperation(HO_Component, XRArgument(1));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
							/**只有选中结构控件并且选择移动操作点进行操作时才启用自动吸附*/
							if (ComponentOperation->GetSelectedComponent()->GetBaseEditPoint() == CurrentOperationInfo.CurrentOperationPoint)
							{
								AxisRuler->SetEnableAutoAttach(false);
							}

							FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
						}
						break;
					/*case OT_TextLabel:
						if (CurrentOperationInfo.CurrentOperationObject.IsValid())
						{
							SetOperation(HO_TextLabel, XRArgument(1));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), nullptr);
							FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
						}
						break;*/
					default:
						break;
					}
				}
			}
			else
			{
				if (AxisRuler->GetBoundObj().IsValid())
				{
					AxisRuler->SetEnableAutoAttach(false);
				}
			}
		}
	}

	// 更新空间间距
	/*if ((Event == IE_Pressed && (Key == EKeys::MouseScrollUp || Key == EKeys::MouseScrollDown))
		|| Event == IE_Released && Key == EKeys::MiddleMouseButton)
	{
		const TArray<FObjectWeakPtr> ObjArray = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects();
		for (auto It : ObjArray)
		{
			if (It.Pin()->GetType() == OT_InternalRoom)
			{
				TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
				if (Room.IsValid())
				{
					Room->UpdateSpacing();
				}
			}
		}
	}*/

	if (Key == EKeys::RightMouseButton || Key == EKeys::Escape)
	{
		AxisRuler->SetBoundObj(nullptr);
	}
	return FArmyDesignModeController::InputKey(Viewport, Key, Event);
}

void FArmyHomeModeController::Tick(float DeltaSeconds)
{
	if (AxisRuler->GetBoundObj().IsValid())
	{
		TArray<TWeakPtr<FArmyObject>> objects;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, objects);
		AxisRuler->Update(GVC, objects);
	}
	if (ShouldTick())
	{
		FArmyDesignModeController::Tick(DeltaSeconds);
	}
}

void FArmyHomeModeController::Clear()
{
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	{
		SCOPE_TRANSACTION(TEXT("户型模式的清空"));

		HomeDataLayers.Empty();
		FArmySceneData::Get()->Clear(E_HomeModel);
		FArmySceneData::Get()->Clear(E_ModifyModel);
		FArmySceneData::Get()->Clear(E_LayoutModel);
		FArmySceneData::Get()->ClearHardModeData();

        // 还原墙线颜色和底图透明度
        FArmySceneData::Get()->SetWallLinesColor(FLinearColor::White);
        FArmySceneData::Get()->SetFacsimileTransparency(60);
	}

	//为了防止死循环
	if (CurrentOperation.IsValid())
	{
		CurrentOperation->EndOperation();
	}
	else
	{
		EndOperation();
	}
	//FArmySceneData::Get()->ClearModifyMap();
	bRebuildBspWall = true;
	FArmySceneData::Get()->bHasOutWall = false;
	FArmySceneData::Get()->SetHasOutWall(FArmySceneData::Get()->bHasOutWall);

	SetLockHome(false);
}

void FArmyHomeModeController::Load(TSharedPtr<FJsonObject> Data)
{
	Clear();

	if (Data.IsValid())
	{
		FArmySceneData::Get()->DeserializationDifferentMode(Data, E_HomeModel);
	}

	FArmySceneData::Get()->bRebuild3DModel = true;
	FArmySceneData::Get()->SetHasOutWall(FArmySceneData::Get()->bHasOutWall);

	if (FArmySceneData::Get()->bHasOutWall)
	{
		SetLockHome(true);
		if (LockWidget.IsValid())
		{
			LockWidget->SetVisibility(bLockedHome ? EVisibility::Visible : EVisibility::Hidden);
		}
	}
}

bool FArmyHomeModeController::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	FArmySceneData::Get()->SerializeToJsonDifferentMode(JsonWriter, E_HomeModel);

	return true;
}

void FArmyHomeModeController::Delete()
{
	for (auto Obj : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
	{
		if (Obj.Pin()->GetType() == OT_InternalRoom)
		{
			//@ 有外墙时不能删除内部空间
			if (!FArmySceneData::Get()->bHasOutWall)
			{
				GGI->Window->PresentModalDialog(TEXT("区域删除后，相关联的门窗会同时删除。是否继续？"),
					FSimpleDelegate::CreateRaw(this, &FArmyHomeModeController::OnDeleteExcute));
				bDeletingRoom = true;
				return;
			}
			else
			{
				return;
			}
			
		}
	}

	OnDeleteExcute();
}

void FArmyHomeModeController::OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj, bool bTransaction/*=false*/)
{
	if (InArg._ArgUint32 == E_HomeModel)
	{
		if (InArg._ArgInt32 == 1)//add
		{
			FName LayerName = FArmyLayerManager::GetInstance()->ObjectLayerName(InObj->GetType());
			TWeakPtr<FArmyObjLayer> HomeDataLayer = HomeDataLayers.FindRef(LayerName);
			if (HomeDataLayer.IsValid())
			{
				HomeDataLayer.Pin()->AddObject(InObj);
			}
			else
			{
				TSharedPtr<FArmyObjLayer> TempLayer = FArmyLayerManager::GetInstance()->CreateLayer(LayerName);
				TempLayer->AddObject(InObj);
				HomeDataLayers.Add(LayerName, TempLayer.ToSharedRef());
			}

			/*@欧石楠添加全局可以捕获端点的obj**/
			switch (InObj->GetType())
			{
			case OT_InternalRoom:
			case OT_OutRoom:
			case OT_AddWall:
				AllCanHoverObjects.Add(FArmyRectSelect::ObjectInfo(InObj, true));
				break;
			case OT_ArcWindow:
			case OT_Door:
			case OT_SlidingDoor:
			case OT_Pass:
			case OT_Punch:
			case OT_Window:
			case OT_FloorWindow:
			case OT_RectBayWindow:
			case OT_TrapeBayWindow:
			case OT_CornerBayWindow:
			case OT_CrossDoorStone:
			case OT_ModifyWall:
			//case OT_ComponentBase:
			case OT_TextLabel:
				AllCanHoverObjects.Add(FArmyRectSelect::ObjectInfo(InObj, false));
				break;
			default:
				break;
			}
			/*@欧石楠 捕获控件关联的线跟房间**/
			if (InObj->GetType() == OT_Window
				|| InObj->GetType() == OT_FloorWindow
				|| InObj->GetType() == OT_RectBayWindow
				|| InObj->GetType() == OT_TrapeBayWindow
				|| InObj->GetType() == OT_Door
				|| InObj->GetType() == OT_SlidingDoor
				|| InObj->GetType() == OT_Pass
				|| InObj->GetType() == OT_DoorHole
				|| InObj->GetType() == OT_Punch
				|| InObj->GetType() == OT_SecurityDoor)
			{
				TSharedPtr<FArmyHardware> TempHW = StaticCastSharedPtr<FArmyHardware>(InObj);
				HardWareOperation->ForceCaptureDoubleLine(TempHW);
				TempHW->Update();
			}
		}
		else if (InArg._ArgInt32 == 0)//remove
		{
			/*@欧石楠移除全局可以捕获端点的obj**/
			switch (InObj->GetType())
			{
			case OT_InternalRoom:
			case OT_OutRoom:
				AllCanHoverObjects.Remove(FArmyRectSelect::ObjectInfo(InObj, true));
				break;
			case OT_ArcWindow:
			case OT_Door:
			case OT_SlidingDoor:
			case OT_Pass:
			case OT_Punch:
			case OT_Window:
			case OT_FloorWindow:
			case OT_RectBayWindow:
			case OT_TrapeBayWindow:
			case OT_CornerBayWindow:
			case OT_CrossDoorStone:
			//case OT_ComponentBase:
			case OT_TextLabel:
				AllCanHoverObjects.Remove(FArmyRectSelect::ObjectInfo(InObj, false));
				break;
			default:
				break;
			}
			FArmyLayerManager::GetInstance()->RemoveFromLayer(InObj);
		}
	}
}

void FArmyHomeModeController::OnHomeModeModifyOperation()
{
	FArmySceneData::Get()->bUpdateHomeModeData = true;
	FArmySceneData::Get()->bUpdateModifyModeData = true;
	FArmySceneData::Get()->bUpdateLayoutModeData = true;

	FArmySceneData::Get()->LoadDataFromJason = false;
	GGI->DesignEditor->OnBaseDataChanged(0);
}

void FArmyHomeModeController::OnSaveCommand()
{
	FArmyFrameCommands::OnMenuSave();
}

void FArmyHomeModeController::OnUndoCommand()
{
	FArmyFrameCommands::OnMenuUndo();
}

void FArmyHomeModeController::OnRedoCommand()
{
	FArmyFrameCommands::OnMenuRedo();
}

void FArmyHomeModeController::OnImportFacsimile()
{
    if (FArmySceneData::Get()->GetFacsimile().IsValid())
    {
        FSimpleDelegate OnConfirmReimport;
        OnConfirmReimport.BindLambda([this]() {
            GGI->Window->DismissModalDialog();
            // 关闭提示框后Tick一下，立即生效
            FSlateApplication::Get().Tick();
            SetOperation(HO_Facsimile);
        });
        GGI->Window->PresentModalDialog(REIMPORT_FACSIMILE_MSG, OnConfirmReimport);
    }
    else
    {
        SetOperation(HO_Facsimile);
    }
}

void FArmyHomeModeController::OnSetFacsimileScale()
{
    SetOperation(HO_Facsimile, XRArgument().ArgBoolean(true));
}

bool FArmyHomeModeController::CanSetFacsimileScale()
{
    return FArmySceneData::Get()->GetFacsimile().IsValid();
}

void FArmyHomeModeController::OnEmptyCommand()
{
	// 户型模式下清空所有
	GGI->Window->PresentModalDialog(TEXT("户型模式下清空方案中所有内容，是否继续？"), FSimpleDelegate::CreateRaw(GGI->DesignEditor.Get(), &FArmyDesignEditor::ClearAll));
}

TSharedRef<SWidget> FArmyHomeModeController::OnImportCommand()
{
    const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
    TSharedRef<FUICommandList> ActionList = MakeShareable(new FUICommandList);

    FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);
    FCanExecuteAction CanSetFacsimileScale = FCanExecuteAction::CreateRaw(this, &FArmyHomeModeController::CanSetFacsimileScale);

    ActionList->MapAction(FArmyHomeModeCommands::Get().CommandImportFacsimile, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnImportFacsimile), DefaultCanExec);
    ActionList->MapAction(FArmyHomeModeCommands::Get().CommandSetFacsimileScale, FExecuteAction::CreateRaw(this, &FArmyHomeModeController::OnSetFacsimileScale), CanSetFacsimileScale);

    CommandList->Append(ActionList);

    FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
    MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");
    MenuBuilder.AddMenuEntry(FArmyHomeModeCommands::Get().CommandImportFacsimile);
    MenuBuilder.AddMenuEntry(FArmyHomeModeCommands::Get().CommandSetFacsimileScale);

    return MenuBuilder.MakeWidget();
}

void FArmyHomeModeController::OnOffsetCommand()
{
	SetOperation(HO_OffsetLine);
}

void FArmyHomeModeController::OnOutWallCommand()
{
	if (GetCurrentOperationType() == HO_Room)
	{
		TSharedPtr<FArmyRoomOperation> RoomOperation = StaticCastSharedPtr<FArmyRoomOperation>(CurrentOperation);
		if (RoomOperation.IsValid() && RoomOperation->GetPrePoint().IsValid())
		{
			TSharedPtr<FArmyRoom> Room = FArmySceneData::Get()->GetRoomFromPoint(RoomOperation->GetPrePoint());
			if (!Room.IsValid())
			{
				GGI->Window->ShowMessage(MT_Warning, TEXT("绘制户型过程中，无法生成外墙！"));
				return;
			}
		}
	}

	//首先判断是否有没闭合的空间
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	if (RoomList.Num() == 0)
		return;

	bool AllClosed = true;
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (!Room->IsClosed())
		{
			TArray<TSharedPtr<FArmyEditPoint>> CurrentPoints = Room->GetPoints();
			GGI->Window->ShowMessage(MT_Warning, TEXT("有未闭合空间，无法生成外墙！"));
			return;
		}
	}

	/**@欧石楠 生成外墙时厚度输入框*/
	if (GGI)
	{
		if (!GenerateOutWallWidget.IsValid())
		{
			SAssignNew(GenerateOutWallWidget, SArmyGenerateOutWall);
		}
		GGI->Window->PresentModalDialog(TEXT("提示"), GenerateOutWallWidget->AsShared(), FSimpleDelegate::CreateRaw(this, &FArmyHomeModeController::OnConfirmGenerateOutWallClicked), false);
		if (bDeletingRoom)
		{
			OnConfirmGenerateOutWallClicked();
			bDeletingRoom = false;
		}

		// @zengy 保证生成外墙的动作能够触发立面模式重新构建三维墙面
		// 否则当只绘制了内墙线保存方案后，再打开方案生成外墙，进入立面模式
		// 就没有墙了
		FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
	}
}

void FArmyHomeModeController::OnDeleteOutWallCommand()
{
	GGI->Window->PresentModalDialog(TEXT("删除外墙时，关联的门窗、阳台将一同删除，是否继续？"),
		FSimpleDelegate::CreateRaw(this, &FArmyHomeModeController::OnDeleteOutWallExcute));
}

void FArmyHomeModeController::OnDeleteOutWallExcute()
{
	//删除立面的所有面片
	//FArmySceneData::Get()->ClearHardModeData();

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
	if (RoomList.Num() > 0)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[0].Pin());
		if (Room.IsValid())
		{
			Room->DeleteRelatedObj();
		}
		FArmySceneData::Get()->Delete(RoomList[0].Pin());
	}

	FArmySceneData::Get()->bHasOutWall = false;
	FArmySceneData::Get()->SetHasOutWall(FArmySceneData::Get()->bHasOutWall);
	FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();
}

void FArmyHomeModeController::OnInternalWallCommand()
{
	SetOperation(HO_Room, XRArgument(1));
}

void FArmyHomeModeController::OnBalconyPunchCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_Punch));
}

void FArmyHomeModeController::OnBearingWallCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_BearingWall, XRArgument(1));
}

bool FArmyHomeModeController::CheckRoomName()
{
	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	for (auto It : RoomList)
	{
		TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
		if (Room.IsValid())
		{
			if (Room->GetSpaceName() == TEXT("未命名"))
			{
				return false;
			}
		}
	}
	return true;
}

void FArmyHomeModeController::SetLockHome(bool bLocked)
{
	bLockedHome = bLocked;
	HideDesignPanel(bLockedHome);
	FArmyToolsModule::Get().GetRectSelectTool()->SetEnableGlobalHover(!bLocked);
	if (!bLockedHome)
	{
		LockWidget->SetVisibility(EVisibility::Hidden);
	}
}

void FArmyHomeModeController::OnPassCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_Pass));
}

void FArmyHomeModeController::OnSecurityDoorCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_SecurityDoor));
}

void FArmyHomeModeController::OnWindowCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_Window));
}

void FArmyHomeModeController::OnFloorWindowCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_FloorWindow));
}

void FArmyHomeModeController::OnBayWindowCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_RectBayWindow));
}

void FArmyHomeModeController::OnCornerBayWindowCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_CornerBayWindow));
}

void FArmyHomeModeController::OnTrapezoidBayWindowCommand()
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_TrapeBayWindow));
}

void FArmyHomeModeController::OnAirLouverCommand()
{
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(HO_Hardware, XRArgument(OT_AirLouver));
}

void FArmyHomeModeController::OnComponentCommand(XRArgument InArg)
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();

	switch (InArg._ArgInt32)
	{
	case OT_Beam:
		SetOperation(HO_SimpleComponent, XRArgument().ArgInt32(OT_Beam).ArgFName(TEXT("横梁")));
		return;
	case OT_Pillar:
		SetOperation(HO_SimpleComponent, XRArgument().ArgInt32(OT_Pillar).ArgFName(TEXT("柱子")));
		return;
	case OT_AirFlue://测
		SetOperation(HO_SimpleComponent, XRArgument().ArgInt32(OT_AirFlue).ArgFName(TEXT("风道")));
		return;
		break;
	default:
		break;
	}
	/**@欧石楠 用于点击结构部件后立马启用标尺并启用自动吸附*/
	FArmyToolsModule::Get().GetRectSelectTool()->AddToSelectedObject(ComponentOperation->GetSelectedComponent());
	AxisRuler->SetBoundObj(ComponentOperation->GetSelectedComponent());
	AxisRuler->SetEnableAutoAttach(true);
}

void FArmyHomeModeController::OnPointLocationCommand(XRArgument InArg)
{
	//@ 激活工具前，取消当前视口数据的选中状态
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();

	switch (InArg._ArgInt32)
	{
	case EC_Drain_Point:
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Drain_Point).ArgString(TEXT("Location_DrainPoint.dxf")).ArgFName(TEXT("下水主管道")));
		break;
	case EC_Switch1O2_Point:
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Switch1O2_Point).ArgString(TEXT("Location_WaterChilledPoint.dxf"/*"Location_Switch1O2_Point.dxf"*/)).ArgFName(TEXT("单联双控开关")));
		break;
	case EC_Closestool://坐便下水
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Closestool).ArgString(TEXT("Location_Closestool.dxf")).ArgFName(TEXT("坐便下水")));
		break;
	case EC_EleBoxH_Point://强电箱
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_EleBoxH_Point).ArgString(TEXT("Location_EleBoxHPoint.dxf")).ArgFName(TEXT("强电箱")));
		break;
	case EC_Gas_MainPipe://燃气主管
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Gas_MainPipe).ArgString(TEXT("Location_GasMainPipe.dxf")).ArgFName(TEXT("燃气主管")));
		break;
	case EC_Gas_Meter://燃气表
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Gas_Meter).ArgString(TEXT("Location_GasMeter.dxf")).ArgFName(TEXT("燃气表")));
		break;
	case EC_Water_Hot_Point://热水点位
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Water_Hot_Point).ArgString(TEXT("Location_WaterHotPoint.dxf")).ArgFName(TEXT("热水点位")));
		break;
	case EC_Water_Chilled_Point://冷水点位
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Water_Chilled_Point).ArgString(TEXT("Location_WaterChilledPoint.dxf")).ArgFName(TEXT("冷水点位")));
		break;
	case EC_Water_Normal_Point://中水点位
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Water_Normal_Point).ArgString(TEXT("Location_WaterNormalPoint.dxf")).ArgFName(TEXT("中水点位")));
		break;
	case EC_Water_Separator_Point://分集水器
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Water_Separator_Point).ArgString(TEXT("Location_WaterSeparatorPoint.dxf")).ArgFName(TEXT("分集水器")));
		break;
	case EC_Basin://地漏下水
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Basin).ArgString(TEXT("Location_Basin.dxf")).ArgFName(TEXT("地漏下水")));
		break;
	case EC_EleBoxL_Point://弱电箱
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_EleBoxL_Point).ArgString(TEXT("Location_EleBoxLPoint.dxf")).ArgFName(TEXT("弱电箱")));
		break;
	case EC_Water_Supply://入户水点位
		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(EC_Water_Supply).ArgString(TEXT("Location_EnterRoomWater.dxf")).ArgFName(TEXT("入户水点位")));
		break;
	default:
		break;
	}

	TSharedPtr<FArmyFurniture> Furniture = ComponentOperation->GetSelectedComponent();
	if (Furniture.IsValid())
	{
		ComponentOperation->ShowAxisRulerDelegate.BindRaw(this, &FArmyHomeModeController::ShowAxisRuler);
		Furniture->SetbUseCustomColor(false);
		Furniture->SetCustomoColor(FLinearColor::White);
		Furniture->SetBreakPointType(EBreakPointType::NativePoint);

		//FArmyToolsModule::Get().GetRectSelectTool()->AddToSelectedObject(Furniture);
		AxisRuler->SetBoundObj(nullptr);
		AxisRuler->SetEnableAutoAttach(false);
	}
}

void FArmyHomeModeController::OnSelectionChanged(TArray< TSharedPtr<FArmyObject> >& ObjArray)
{
    // 防止注销后登录，打开方案崩溃
    if (!DetailWidget.IsValid())
    {
        return;
    }

	if (ObjArray.Num() == 1) {
		DetailWidget->ShowSelectedDetial(ObjArray[0]);
	}
	else if (ObjArray.Num() > 0) {
		//TODO Muti selection function
		DetailWidget->ShowSelectedDetial(nullptr);
	}
	else {
		DetailWidget->ShowSelectedDetial(nullptr);
	}
}

void FArmyHomeModeController::OnDeleteExcute()
{
	AxisRuler->SetBoundObj(nullptr);
	const TArray<FObjectWeakPtr>& SelectedObjects = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects();
	TArray<FArmyRectSelect::FPrimitiveInfo> SelectedPrimitives = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedPrimitives();
	if (SelectedObjects.Num() == 0 && SelectedPrimitives.Num() == 0)
		return;
	else if (SelectedObjects.Num() == 0 && SelectedPrimitives.Num() > 0 && FArmySceneData::Get()->bHasOutWall)
		return;

	SCOPE_TRANSACTION(TEXT("删除选中的物体"));

	for (auto It : SelectedObjects)
	{
		if (It.Pin()->GetType() == OT_InternalRoom || It.Pin()->GetType() == OT_OutRoom)
		{
			TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
			if (Room.IsValid())
			{
				Room->DeleteRelatedObj();
			}
		}
		FArmySceneData::Get()->Delete(It.Pin());
	}

	for (auto It : SelectedPrimitives)
	{
        // @欧石楠 这个地方强转判断类型的写法是错的！类型不一致也会强转成功
		TSharedPtr<FArmyLine> CurrentLine = StaticCastSharedPtr<FArmyLine>(It.Primitive);
        if (!CurrentLine.IsValid())
        {
			continue;
        }

		TSharedPtr<FArmyRoom> CurrentRoom = FArmySceneData::Get()->GetRoomFromLine(CurrentLine);
		CurrentRoom->RemoveLine(CurrentLine);

		TArray<TSharedPtr<class FArmyWallLine>> WallLines = CurrentRoom->GetWallLines();
		if (WallLines.Num() == 0)
		{
			FArmySceneData::Get()->Delete(CurrentRoom);
		}
		else /** @欧石楠 如果房间还存在则检查是否因删除线条产生了另外一个房价*/
		{
			TArray<FVector> RoomPoints = CurrentRoom->GetAnotherRoomPoints();
			if (RoomPoints.Num() > 0)
			{
				for (auto It : RoomPoints)
				{
					RoomOperation->AddLinePoint(It);
				}
				RoomOperation->EndOperation();
				CurrentRoom->ClearAnotherRoomPoints();
			}
		}
	}
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();

	FArmySceneData::HomeMode_ModifyMultiDelegate.Broadcast();

	bRebuildBspWall = true;

	TArray<TWeakPtr<FArmyObject>> RoomList;
	FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	if (bDeletingRoom && FArmySceneData::Get()->bHasOutWall && RoomList.Num() > 0)//有内墙线区域时，自动生成外墙
		OnOutWallCommand();
	else if (FArmySceneData::Get()->bHasOutWall && RoomList.Num() == 0)//没有内墙区域时，外墙也删除
	{
		RoomList.Empty();
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
		if (RoomList.Num() > 0)
		{
			FArmySceneData::Get()->Delete(RoomList[0].Pin());
			FArmySceneData::Get()->bHasOutWall = false;
			FArmySceneData::Get()->SetHasOutWall(FArmySceneData::Get()->bHasOutWall);
		}
	}

	GGI->Window->DismissModalDialog();
}

void FArmyHomeModeController::OnConfirmGenerateOutWallClicked()
{
	if (GenerateOutWallWidget.IsValid() || bDeletingRoom)
	{
		GGI->Window->ShowMessage(MT_Normal, TEXT("可通过【删除外墙】命令一键删除外墙"));

		GenerateOutWallWidget->OnConfirmClicked();
		GGI->Window->DismissModalDialog();

		int WallThickness = GenerateOutWallWidget->GetCachedValue() / 10;
		FArmySceneData::OutWallThickness = GenerateOutWallWidget->GetCachedValue();

		TArray<TWeakPtr<FArmyObject>> RoomList;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);

		//取消所有选中状态
		FArmyToolsModule::Get().GetRectSelectTool()->Clear();

		//删除已有的外墙
		TArray<TWeakPtr<FArmyObject>> OutRoomList;
		FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, OutRoomList);
		if (OutRoomList.Num() == 1)
			FArmySceneData::Get()->Delete(OutRoomList[0].Pin());

        //根据布尔运算找出&的多边形区域，再挤出
        Clipper c;
        TArray<FVector> OutLinePoints;
        Paths PrePointsPath;

        TSharedPtr<FArmyRoom> Room0 = StaticCastSharedPtr<FArmyRoom>(RoomList.Pop().Pin());
        TArray<FVector> PointListPre = FArmyMath::Extrude3D(Room0->GetWorldPoints(true), -WallThickness);

        int32 Index = RoomList.Num() - 1;
        while (RoomList.Num() > 0 && Index >= 0)
        {
            TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(RoomList[Index].Pin());
            TArray<FVector> PointListCurrent = FArmyMath::Extrude3D(Room->GetWorldPoints(true), -WallThickness);

            if (FArmyMath::ArePolysIntersection(PointListPre, PointListCurrent, true))
            {
                Paths ClipperPath;
                Path RoomPath;
                for (int i = 0; i < PointListPre.Num(); i++)
                {
                    RoomPath.push_back(IntPoint(PointListPre[i].X * 1000, PointListPre[i].Y * 1000));
                }
                ClipperPath.push_back(RoomPath);
                c.AddPaths(ClipperPath, ptSubject, true);

                ClipperPath.clear();
                RoomPath.clear();
                for (int i = 0; i < PointListCurrent.Num(); i++)
                {
                    RoomPath.push_back(IntPoint(PointListCurrent[i].X * 1000, PointListCurrent[i].Y * 1000));
                }
                ClipperPath.push_back(RoomPath);
                c.AddPaths(ClipperPath, ptClip, true);

                Paths ResultPoints;
                if (c.Execute(ctUnion, ResultPoints, pftEvenOdd, pftEvenOdd))
                {
                    // @欧石楠 防止在做布尔运算的时候出现挖洞的情况
                    int32 MaxSize = 0;
                    int32 MaxSizeIndex = 0;
                    for (int32 i = 0; i < ResultPoints.size(); ++i)
                    {
                        if (ResultPoints[i].size() > MaxSize)
                        {
                            MaxSize = ResultPoints[i].size();
                            MaxSizeIndex = i;
                        }
                    }

                    OutLinePoints.Empty();
                    Path ResultPath = ResultPoints[MaxSizeIndex];
                    for (auto& It : ResultPath)
                    {
                        OutLinePoints.Push(FVector(It.X / 1000.f, It.Y / 1000.f, 0));
                    }

                    PointListPre = OutLinePoints;

                    c.Clear();
                }

                RoomList.RemoveAt(Index);
                Index = RoomList.Num() - 1;
            }
            else
            {
                --Index;
            }
        }

        if (OutLinePoints.Num() == 0)
        {
            OutLinePoints = PointListPre;
        }

		TArray<FVector> UseOutLinePoints;
		int32 IndexTag = 0;
		//重新排序，将小于50mm的点放到第一个
		for (int i = 0; i < OutLinePoints.Num(); i++)
		{
			float LineLength = (OutLinePoints[i] - OutLinePoints[i == OutLinePoints.Num() - 1 ? 0 : i + 1]).Size();
			if (LineLength < 5)
			{
				IndexTag = i;
				break;
			}
			else
				UseOutLinePoints.Push(OutLinePoints[i]);
		}
		//将小于50mm的外墙线去掉
		if (IndexTag > 0)
		{
			int32 NextIndex = 0;
			for (int i = IndexTag; i < OutLinePoints.Num(); i++)
			{
				UseOutLinePoints.Insert(OutLinePoints[i], NextIndex++);
			}

			if (UseOutLinePoints.Num() > 3)
			{
				FVector PreLine = UseOutLinePoints[0] - UseOutLinePoints[UseOutLinePoints.Num() - 1];
				FVector NextLine = UseOutLinePoints[2] - UseOutLinePoints[3];
				FVector2D IntersectePoint2D;
				if (FArmyMath::Line2DIntersection(FVector2D(UseOutLinePoints[0]), FVector2D(UseOutLinePoints[UseOutLinePoints.Num() - 1]),
					FVector2D(UseOutLinePoints[2]), FVector2D(UseOutLinePoints[3]), IntersectePoint2D))
				{
					FVector IntersectePoint(IntersectePoint2D, UseOutLinePoints[0].Z);
					UseOutLinePoints.RemoveAt(0, 3);
					UseOutLinePoints.Insert(IntersectePoint, 0);
				}
			}
		}
		else
			UseOutLinePoints = OutLinePoints;

		TSharedPtr<FArmyRoom> OutRoom = MakeShareable(new FArmyRoom);
		OutRoom->SetType(OT_OutRoom);
		TSharedPtr<FArmyEditPoint> FirstPoint = MakeShareable(new FArmyEditPoint(UseOutLinePoints[0]));
		TSharedPtr<FArmyEditPoint> PointPre = FirstPoint;
		for (int i = 1; i <= UseOutLinePoints.Num(); i++)
		{
			TSharedPtr<FArmyEditPoint> PointCurrent;
			if (i == UseOutLinePoints.Num())
				PointCurrent = FirstPoint;
			else
				PointCurrent = MakeShareable(new FArmyEditPoint(UseOutLinePoints[i]));
			TSharedPtr<FArmyWallLine> Line = MakeShareable(new FArmyWallLine());
			Line->GetCoreLine()->SetStartPointer(PointPre);
			Line->GetCoreLine()->SetEndPointer(PointCurrent);
			Line->SetWidth(WALLLINEWIDTH);
			if (PointPre->GetReferenceNum() < 2)
				PointPre->AddReferenceLine(Line->GetCoreLine());
			if (PointCurrent->GetReferenceNum() < 2)
				PointCurrent->AddReferenceLine(Line->GetCoreLine());
			OutRoom->AddLine(Line);
			PointPre = PointCurrent;
		}
		FArmySceneData::Get()->Add(OutRoom, 0);

		FArmySceneData::Get()->bHasOutWall = true;
		FArmySceneData::Get()->SetHasOutWall(FArmySceneData::Get()->bHasOutWall);
		
		if (CurrentOperation.IsValid())
			CurrentOperation->EndOperation();
	}
}

void FArmyHomeModeController::OnNotNameSpace()
{
	bSwitchMode = true;
	GGI->Window->DismissModalDialog();
	GGI->DesignEditor->DesignTitleBar.Pin()->OnDesignModeChanged(MakeShareable(new FString(GGI->DesignEditor->GetCheckDesignMode())), ESelectInfo::OnMouseClick);
}

void FArmyHomeModeController::OnNameSpace()
{
	bSwitchMode = false;
	GGI->Window->DismissModalDialog();
}

void FArmyHomeModeController::ShowAxisRuler(TSharedPtr<FArmyObject> obj, bool bShow, bool bAutoAttach)
{
	if (bShow)
	{
		AxisRuler->SetBoundObj(obj);
		AxisRuler->SetEnableAutoAttach(bAutoAttach);
	}
	else
	{
		AxisRuler->SetBoundObj(nullptr);
		AxisRuler->SetEnableAutoAttach(bAutoAttach);

	}
}

bool FArmyHomeModeController::PreCalTransformIllegalCommon(TSharedPtr<FArmyObject> SelectedObj, FTransform trans, int32 Flag)
{
	TSharedPtr<FArmyFurniture> Fur = StaticCastSharedPtr<FArmyFurniture>(SelectedObj);
	if (Fur.IsValid())
	{
		return Fur->PreCalTransformIllegal(trans, Flag);
	}
	return false;
}

TSharedPtr<SWidget> FArmyHomeModeController::CreateLockWidget()
{
	return
		SNew(SBox)
		.Visibility(EVisibility::Hidden)
		.WidthOverride(120)
		.HeightOverride(44)
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		.Content()
		[
			SNew(SButton)
			.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Orange"))
		.OnClicked(this, &FArmyHomeModeController::OnUnlockClicked)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(SImage)
			.Image(FArmyStyle::Get().GetBrush("Icon.Unlock"))
		]
	+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(5, 0, 0, 0)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.VAlign(EVerticalAlignment::VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("解锁户型")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_14"))
		]
		]
		];
}

FReply FArmyHomeModeController::OnUnlockClicked()
{
	GGI->Window->PresentModalDialog(TEXT("解锁后修改原始户型会删除其他模式设计数据，是否解锁？"),
		FSimpleDelegate::CreateRaw(this, &FArmyHomeModeController::OnConfirmUnlockHome));
	return FReply::Handled();
}

void FArmyHomeModeController::OnConfirmUnlockHome()
{
	SetLockHome(false);
}
