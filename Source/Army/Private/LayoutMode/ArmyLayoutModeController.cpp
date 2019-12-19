#include "ArmyLayoutModeController.h"
#include "SArmyOperationBrowser.h"
#include "ArmyAddWallOperation.h"
#include "ArmyAxisRuler.h"
#include "ArmyComponentOperation.h"
#include "ArmyHardwareOperation.h"
#include "ArmySceneData.h"
#include "ArmyLayerManager.h"
#include "ArmyLayoutModeCommands.h"
#include "ArmyLayoutModeDetail.h"
#include "ArmyModifyWallOperation.h"
#include "ArmyPackPipeOperation.h"
#include "ArmySingleDoorDetail.h"
#include "ArmyTextLabelOperation.h"
#include "Army3DManager.h"
#include "ArmyToolBarBuilder.h"
#include "ArmyFrameCommands.h"
#include "ArmyEditorEngine.h"
#include "ArmyGameInstance.h"
#include "ArmyRegionOperation.h"

enum ELayoutOperation
{
    LO_Hardware,
    LO_Component,
	LO_ModifyWall,
	LO_AddWall,
	LO_PackPipe,
	/** @纪仁泽  区域分割 */
	LO_SplitRegion,
	LO_TextLabel,
};

FArmyLayoutModeController::~FArmyLayoutModeController()
{
    FArmyLayoutModeCommands::Unregister();
    FArmySceneData::Get()->PreOperationDelegate.RemoveAll(this);
}

const EModelType FArmyLayoutModeController::GetDesignModelType()
{
    return IsDisplayDismantle() ? EModelType::E_ModifyModel : EModelType::E_LayoutModel;
}

void FArmyLayoutModeController::Init()
{
	FArmySceneData::Get()->PreOperationDelegate.AddRaw(this, &FArmyLayoutModeController::OnObjectOperation);

	FArmySceneData::ModifyMode_ModifyMultiDelegate.AddRaw(this, &FArmyLayoutModeController::OnModifyModeModifyOperation);

    FArmyLayoutModeCommands::Register();

    FArmyDesignModeController::Init();

	AxisRuler = MakeShareable(new FArmyAxisRuler());
	AxisRuler->Init(GVC->ViewportOverlayWidget);
}

void FArmyLayoutModeController::InitOperations()
{
    // @海涛 初始化操作类
	HardWareOperation = MakeShareable(new FArmyHardwareOperation(E_HomeModel));
	OperationMap.Add(LO_Hardware, HardWareOperation);
	OperationMap.Add(LO_ModifyWall, MakeShareable(new FArmyModifyWallOperation(E_HomeModel)));
	OperationMap.Add(LO_AddWall, MakeShareable(new FArmyAddWallOperation(E_HomeModel)));
	OperationMap.Add(LO_PackPipe, MakeShareable(new FArmyPackPipeOperation(E_HomeModel)));
	/** @纪仁泽 区域分割初始化操作 */
	OperationMap.Add(LO_SplitRegion, MakeShareable(new FArmyRegionOperation(E_HomeModel)));

    ComponentOperation = MakeShareable(new FArmyComponentOperation(E_HomeModel));
    OperationMap.Add(LO_Component, ComponentOperation);
	OperationMap.Add(LO_TextLabel, MakeShareable(new FArmyTextLabelOperation(E_HomeModel)));

	for (auto It : OperationMap)
	{
		It.Value->Init();
		It.Value->InitWidget(GVC->ViewportOverlayWidget);
		It.Value->EndOperationDelegate.BindRaw(this, &FArmyLayoutModeController::EndOperation);
	}

	GVC->AdsorbDetect.BindUObject(G3DM,&UXR3DManager::CaptureWallInfo);
}

void FArmyLayoutModeController::BeginMode()
{
    /**@欧石楠代理选中事件*/
    FArmyToolsModule::Get().GetRectSelectTool()->SelectedObjectsDelegate.BindRaw(this, &FArmyLayoutModeController::OnSelectionChanged);

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

	FArmyToolsModule::Get().GetRectSelectTool()->BIsLayOut = true;
}

bool FArmyLayoutModeController::EndMode()
{
	AxisRuler->SetBoundObj(nullptr);

	/**@欧石楠取消户型模式下的所有选中*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmyToolsModule::Get().GetRectSelectTool()->BIsLayOut = false;

	return true;
}

void FArmyLayoutModeController::EndOperation()
{
	AxisRuler->SetBoundObj(nullptr);
	FArmyDesignModeController::EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Start();
}

void FArmyLayoutModeController::CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles)
{
	for (auto LayerIt : FArmySceneData::Get()->bIsDisplayDismantle ? HomeDataLayers_Modify : HomeDataLayers_Layout)
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

TSharedPtr<SWidget> FArmyLayoutModeController::MakeLeftPanelWidget()
{
    TSharedPtr<SArmyOperationBrowser> HomeCategory = SNew(SArmyOperationBrowser);

    HomeCategory->BeginOperationCategory(TEXT("拆改"), FArmyStyle::Get().GetBrush("Category.Dismantle"));

    HomeCategory->AddOperationLabel_First(TEXT("墙体"));
    HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandNewWall, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnAddWallCommand));
    HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandDismantleWall, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnModifyWallCommand));	
	//HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandCurtainBox, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnComponentCommand, XRArgument().ArgInt32(OT_CurtainBox)));
	HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandPackPipe, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnPackPipeCommand));
	HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandNewPass, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnNewPassCommand));
	HomeCategory->BuildSubOperationCategory();

	HomeCategory->AddOperationLabel(TEXT("室内门"));
    HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandDoor, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnDoorCommand));
    HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandSlidingDoor, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnSlidingDoorCommand));
    HomeCategory->BuildSubOperationCategory();
	
	/** @纪仁泽 布局 - 区域布局暂时不上线*/
    //HomeCategory->BeginOperationCategory(TEXT("布局"), FArmyStyle::Get().GetBrush("Category.SplitRegion"));
    //HomeCategory->AddOperationLabel_First(TEXT("空间布局"));
    //HomeCategory->AddOperation(FArmyLayoutModeCommands::Get().CommandSplitRegion, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnSplitRegion));
    //HomeCategory->BuildSubOperationCategory();

    return HomeCategory;
}

TSharedPtr<SWidget> FArmyLayoutModeController::MakeRightPanelWidget()
{    
	DetailWidget = MakeShareable(new FArmyLayoutModeDetail());
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

TSharedPtr<SWidget> FArmyLayoutModeController::MakeToolBarWidget()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"

	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	FCanExecuteAction CheckHasSelectedXRObj = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedXRObj);

	//FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	//FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);

	ToolBarBuidler->AddButton(FArmyLayoutModeCommands::Get().CommandSave, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnSaveCommand));
	//ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandUndo, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnUndoCommand), CheckCanUndo);
	//ToolBarBuidler->AddButton(FArmyHomeModeCommands::Get().CommandRedo, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnRedoCommand), CheckCanRedo);
	ToolBarBuidler->AddButton(FArmyLayoutModeCommands::Get().CommandDelete, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::Delete), CheckHasSelectedXRObj);
	ToolBarBuidler->AddButton(FArmyLayoutModeCommands::Get().CommandEmpty, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnEmptyCommand));
	ToolBarBuidler->AddButton(FArmyLayoutModeCommands::Get().CommandDismantle, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnDisplayDismantle), FCanExecuteAction(), FIsActionChecked::CreateRaw(this, &FArmyLayoutModeController::IsDisplayDismantle));
	ToolBarBuidler->AddButton(FArmyLayoutModeCommands::Get().CommandAfterDismantle, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnDisplayAfterDismantle), FCanExecuteAction(), FIsActionChecked::CreateRaw(this, &FArmyLayoutModeController::IsDisplayAfterDismantle));
    //ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyLayoutModeController::OnDismantleStateCommand), LOCTEXT("DismantleState", "状态"), FSlateIcon("ArmyStyle", "LayoutMode.CommandDismantleCombo"));

	CommandList->Append(ToolBarBuidler->GetCommandList());

#undef LOCTEXT_NAMESPACE

    return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

void FArmyLayoutModeController::Draw(FViewport* InViewport, const FSceneView* InView, FCanvas* SceneCanvas)
{
    FArmyDesignModeController::Draw(InViewport, InView, SceneCanvas);

    for (auto LayerIt : FArmySceneData::Get()->bIsDisplayDismantle ? HomeDataLayers_Modify : HomeDataLayers_Layout)
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

void FArmyLayoutModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
    if (ReferenceImage.IsValid())
    {
        ReferenceImage.Pin()->Draw(PDI, View);
    }

    AxisRuler->Draw(PDI, View);
    FArmyDesignModeController::Draw(View, PDI);
	TArray<FObjectWeakPtr> DrawObjects;
	if (FArmySceneData::Get()->bIsDisplayDismantle)
	{
		DrawObjects = FArmySceneData::Get()->GetObjects(E_ModifyModel);
	}
	else
	{
		DrawObjects = FArmySceneData::Get()->GetObjects(E_LayoutModel);
	}
    
	for (auto ItObj : DrawObjects)
	{
		ItObj.Pin()->Draw(PDI, View);
	}
}

bool FArmyLayoutModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	/*if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		return FArmyDesignModeController::InputKey(Viewport, Key, Event);
	}*/
	if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
	{
		if ((Event == IE_Pressed || Event == IE_Released) && Key == EKeys::LeftMouseButton)
		{
			//单选物体不参与鼠标松开事件的逻辑，包括拖拽后的松开。
            if (Event == IE_Released && FArmyToolsModule::Get().GetRectSelectTool()->GetState() == 1)
            {				
				return false;
            }

			TArray<FObjectWeakPtr> AllObjects;
			for (auto LayerIt : FArmySceneData::Get()->bIsDisplayDismantle ? HomeDataLayers_Modify : HomeDataLayers_Layout)
			{
				if (LayerIt.Value.Pin()->IsVisible())
				{
					//拆改后状态只能选中空间
					if (!FArmySceneData::Get()->bIsDisplayDismantle)
					{
						AllObjects.Append(LayerIt.Value.Pin()->GetObjects(OT_InternalRoom));
						AllObjects.Append(LayerIt.Value.Pin()->GetObjects(OT_SplitLine));
						AllObjects.Append(LayerIt.Value.Pin()->GetObjects(OT_Region));
						AllObjects.Append(LayerIt.Value.Pin()->GetObjects(OT_TextLabel));
					}	
					else
					{
						for (auto Obj : LayerIt.Value.Pin()->GetObjects())
						{
							if (Obj->GetType() != OT_OutRoom &&
								Obj->GetType() != OT_ComponentBase)
							{
								AllObjects.Add(Obj);
							}
						}
					}
				}
			}

			//拣选的顺序  如果有添加的，请规划好在这个数组中的顺序，要考虑其他类型
			static TArray<EObjectType> ObjectTypeSortArray{
				OT_SlidingDoor,
				OT_Door,
				OT_Pass,
				OT_NewPass,
				OT_AddWall,
				OT_IndependentWall,
				OT_PackPipe,
				OT_TextLabel,
				OT_SplitLine,
				OT_Region,
				OT_InternalRoom
			};

            AllObjects.Sort([&](const FObjectWeakPtr A, const FObjectWeakPtr B) {
				for (auto ObjType : ObjectTypeSortArray)
				{
					if (A.Pin()->GetType() == ObjType)
					{
						return true;
					}
					else if ((B.Pin()->GetType() == ObjType))
					{
						return false;
					}
				}
				return false;
            });

			FArmyToolsModule::Get().GetRectSelectTool()->LeftMouseButtonPress(GVC, Viewport->GetMouseX(), Viewport->GetMouseY(), AllObjects);
			
			//只有presse事件才进入operation相关操作
			if (Event == IE_Pressed && !CurrentOperation.IsValid())
			{
				AxisRuler->SetBoundObj(nullptr);
				FArmyRectSelect::OperationInfo CurrentOperationInfo = FArmyToolsModule::Get().GetRectSelectTool()->GetCurrentOperationInfo();				

				/**@欧石楠 如果选中了控件，更新一遍捕捉信息*/
				if (FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects().Num() > 0)
				{
					TSharedPtr<FArmyObject> TempObj = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects()[0].Pin();
					if (TempObj->GetType() == OT_Window
						|| TempObj->GetType() == OT_FloorWindow
						|| TempObj->GetType() == OT_RectBayWindow
						|| TempObj->GetType() == OT_TrapeBayWindow
						|| TempObj->GetType() == OT_SecurityDoor
						|| TempObj->GetType() == OT_Pass
						|| TempObj->GetType() == OT_NewPass
						|| TempObj->GetType() == OT_Punch
						|| TempObj->GetType() == OT_Door 
						|| TempObj->GetType() == OT_SecurityDoor)
					{
						TSharedPtr<FArmyHardware> TempHW = StaticCastSharedPtr<FArmyHardware>(TempObj);
						HardWareOperation->ForceCaptureDoubleLine(TempHW, E_ModifyModel);
						TempHW->Update();
						if (TempObj->GetType() == OT_Door)
						{
							TSharedPtr<FArmySingleDoor> TempHWDoor = StaticCastSharedPtr<FArmySingleDoor>(TempHW);
							TempHWDoor->UpdateDoorHole();
						}
						else if (TempObj->GetType() == OT_SlidingDoor)
						{
							TSharedPtr<FArmySlidingDoor> TempHWSlidingDoor = StaticCastSharedPtr<FArmySlidingDoor>(TempHW);
							TempHWSlidingDoor->UpdateDoorHole();
						}

					}

					if (TempObj->GetType() == OT_TextLabel)
					{
						SetOperation(LO_TextLabel, XRArgument(1));
						CurrentOperation->SetSelected(TempObj, nullptr);											
					}
				}			

				if (CurrentOperationInfo.IsValid())
				{					
					switch (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType())
					{
					case OT_Door:
					case OT_SlidingDoor:
					case OT_NewPass:
					case OT_Pass:
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							SetOperation(LO_Hardware, XRArgument(OT_None));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
						}
						break;						
					case OT_CurtainBox:
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							SetOperation(LO_Component, XRArgument(1));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
							/**@欧石楠 只有选中结构控件并且选择移动操作点进行操作时才启用自动吸附*/
							if (ComponentOperation->GetSelectedComponent()->GetBaseEditPoint() == CurrentOperationInfo.CurrentOperationPoint)
							{
								AxisRuler->SetEnableAutoAttach(true);
							}
							FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();
						}
						break;
					/*case OT_TextLabel:
						if (CurrentOperationInfo.CurrentOperationObject.IsValid())
						{
							SetOperation(LO_TextLabel, XRArgument(1));
							CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), nullptr);
							GetDesignModelType() == E_ModifyModel ? FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast() : FArmySceneData::LayOutMode_ModifyMultiDelegate.Broadcast();
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
	/**@欧石楠 当松开鼠标时更新一次属性面板*/
	if (Event == IE_Released && Key == EKeys::LeftMouseButton)
	{		
		if (DetailWidget.IsValid())
		{
			DetailWidget->RefreshSelectedDetial();
		}
	}	

    // 更新空间间距
	if ((Event == IE_Pressed && (Key == EKeys::MouseScrollUp || Key == EKeys::MouseScrollDown)) 
		|| Event == IE_Released && Key == EKeys::MiddleMouseButton)
	{
		const TArray<FObjectWeakPtr> ObjArray = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects();
		for (auto It : ObjArray)
		{
            if (It.Pin()->GetType() == OT_ModifyWall)
			{
				TSharedPtr<FArmyModifyWall> ModifyWall = StaticCastSharedPtr<FArmyModifyWall>(It.Pin());
				if (ModifyWall.IsValid())
					ModifyWall->UpdateDashLine();
			}
		}
	}

	return FArmyDesignModeController::InputKey(Viewport, Key, Event);
}

void FArmyLayoutModeController::Tick(float DeltaSeconds)
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
void FArmyLayoutModeController::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
	{
		FArmyToolsModule::Get().GetRectSelectTool()->MouseMove(GVC, X, Y, GetDesignModelType() == E_ModifyModel ? AllCanHoverObjects : AllCanHoverObjectsDismantle);
	}
	if (FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture)
	{
		TArray<TSharedPtr<FArmyLine>> Lines;
		TArray<TSharedPtr<FArmyCircle>> Circles;
		TArray<TSharedPtr<FArmyPoint>> Points;
		CollectAllLinesAndPoints(Lines, Points, Circles);

		FArmyToolsModule::Get().GetMouseCaptureTool()->Capture(GVC, X, Y, Lines, Points);
	}

	if (CurrentOperation.IsValid())
	{
		CurrentOperation->MouseMove(GVC, Viewport, X, Y);
	}
}
void FArmyLayoutModeController::Clear()
{
	HomeDataLayers_Modify.Empty();
	HomeDataLayers_Layout.Empty();
    FArmySceneData::Get()->ClearModifyData();
	FArmySceneData::Get()->Clear(E_LayoutModel);

    // 还原墙线颜色和底图透明度
    //FArmySceneData::Get()->SetWallLinesColor(FLinearColor::White);
    //FArmySceneData::Get()->SetFacsimileTransparency(60);
}

void FArmyLayoutModeController::Load(TSharedPtr<FJsonObject> Data)
{
	if (!FArmySceneData::Get()->bHasOutWall)
		return;

	if (Data.IsValid())
	{
		//FArmySceneData::Get()->DeserializationDifferentMode(Data, E_HomeModel);
		FArmySceneData::Get()->DeserializationDifferentMode(Data, E_ModifyModel);
		FArmySceneData::Get()->DeserializationDifferentMode(Data, E_LayoutModel);
		// 初始化区域中的分割线
		InitSplitLineInRegion();
	}
}

bool FArmyLayoutModeController::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	if (!FArmySceneData::Get()->bHasOutWall)
		return false;

	TArray<FObjectWeakPtr> SplitLineArry;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, EObjectType::OT_SplitLine, SplitLineArry);

	FArmySceneData::Get()->SerializeToJsonDifferentMode(JsonWriter, E_ModifyModel);
	FArmySceneData::Get()->SerializeToJsonDifferentMode(JsonWriter, E_LayoutModel);

    return true;
}

void FArmyLayoutModeController::Delete()
{
    OnDeleteExcute();
}

void FArmyLayoutModeController::OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj,bool bTransaction/*=false*/)
{
	if (InArg._ArgUint32 == E_ModifyModel || InArg._ArgUint32 == E_LayoutModel)
	{
		if (InArg._ArgInt32 == 1)//add
		{
			FName LayerName = FArmyLayerManager::GetInstance()->ObjectLayerName(InObj->GetType());

			TWeakPtr<FArmyObjLayer> HomeDataLayer;
			if (InArg._ArgUint32 == E_ModifyModel)
				HomeDataLayer = HomeDataLayers_Modify.FindRef(LayerName);
			else
				HomeDataLayer = HomeDataLayers_Layout.FindRef(LayerName);
			if (HomeDataLayer.IsValid())
			{
				HomeDataLayer.Pin()->AddObject(InObj);
			}
			else
			{
				TSharedPtr<FArmyObjLayer> TempLayer = FArmyLayerManager::GetInstance()->CreateLayer(LayerName);
				TempLayer->AddObject(InObj);
				if (InArg._ArgUint32 == E_ModifyModel)
					HomeDataLayers_Modify.Add(LayerName, TempLayer.ToSharedRef());
				else
					HomeDataLayers_Layout.Add(LayerName, TempLayer.ToSharedRef());
			}

			/*@欧石楠添加全局可以捕获端点的obj**/
			switch (InObj->GetType())
			{
			case OT_AddWall:
				E_ModifyModel == InArg._ArgUint32 ? AllCanHoverObjects.Add(FArmyRectSelect::ObjectInfo(InObj, true)) : AllCanHoverObjectsDismantle.Add(FArmyRectSelect::ObjectInfo(InObj, true));
				break;

			case OT_Door:
			case OT_SlidingDoor:
			case OT_ModifyWall:
			case OT_TextLabel:
				E_ModifyModel == InArg._ArgUint32 ? AllCanHoverObjects.Add(FArmyRectSelect::ObjectInfo(InObj, false)) : AllCanHoverObjectsDismantle.Add(FArmyRectSelect::ObjectInfo(InObj, false));
				break;

			default:
				break;
			}

			// @欧石楠 更新墙线颜色
			if (InObj->GetType() == OT_InternalRoom ||
				InObj->GetType() == OT_OutRoom)
			{
				TSharedPtr<FArmyRoom> TempRoom = StaticCastSharedPtr<FArmyRoom>(InObj);
				if (TempRoom.IsValid())
				{
					TempRoom->SetWallLinesColor(FArmySceneData::Get()->GetWallLinesColor());
				}
			}

			/*@欧石楠 捕获控件关联的线跟房间**/
			if (InObj->GetType() == OT_Window
				|| InObj->GetType() == OT_FloorWindow
				|| InObj->GetType() == OT_RectBayWindow
				|| InObj->GetType() == OT_TrapeBayWindow
				|| InObj->GetType() == OT_Door
				|| InObj->GetType() == OT_SlidingDoor
				|| InObj->GetType() == OT_Pass
				|| InObj->GetType() == OT_NewPass
				|| InObj->GetType() == OT_DoorHole
				|| InObj->GetType() == OT_Punch)
			{
				TSharedPtr<FArmyHardware> TempHW = StaticCastSharedPtr<FArmyHardware>(InObj);
				HardWareOperation->ForceCaptureDoubleLine(TempHW, (EModelType)InArg._ArgUint32);
				TempHW->Update();
			}

			/**@欧石楠 添加控件时默认自动调用一次属性面板，用来初始化施工项*/
			if (DetailWidget.IsValid())
			{
				DetailWidget->ShowSelectedDetial(InObj);
				DetailWidget->ShowSelectedDetial(nullptr);
			}
		}
		else if (InArg._ArgInt32 == 0)//remove
		{
			/*@欧石楠移除全局可以捕获端点的obj**/
			TSharedPtr<FArmyFurniture> Fur = nullptr;
			AActor * RelatedActor = nullptr;
			switch (InObj->GetType())
			{
			case OT_Door:
			case OT_SlidingDoor:
			case OT_TextLabel:
				AllCanHoverObjects.Remove(FArmyRectSelect::ObjectInfo(InObj, false));
				AllCanHoverObjectsDismantle.Remove(FArmyRectSelect::ObjectInfo(InObj, false));
				break;
			case OT_ComponentBase:
				Fur = StaticCastSharedPtr<FArmyFurniture>(InObj);
				RelatedActor = Fur.IsValid() ? Fur->GetRelevanceActor() : nullptr;
				if (Fur->GetFurniturePro()->GetbIsPakModel() && RelatedActor && RelatedActor->IsValidLowLevel())
				{
					RelatedActor->Destroy();
				}
				else
				{
					for (int32 i = 0 ;i<Fur->GetChildren().Num();i++)
					{
						FObjectPtr Child = Fur->GetChildren()[i];
						if (Child.IsValid() && (Child->GetType() > OT_Preload_Begin || Child->GetType() < OT_Preload_End))
						{
							TSharedPtr<FArmyPipeline> Line = StaticCastSharedPtr<FArmyPipeline>(Child);
							if (Line.IsValid())
							{
								AActor* LineActor = Line->GetRelevanceActor();
								if (LineActor && LineActor->IsValidLowLevel())
								{
									GXREditor->OnLevelActorRemoved.ExecuteIfBound(LineActor);
								}
							}
						}
					}
					
				}
				break;
			default:
				break;
			}

			FArmyLayerManager::GetInstance()->RemoveFromLayer(InObj);
		}
	}
    else if (InArg._ArgUint32 == E_HomeModel && InObj->GetType() == OT_ReferenceImage) // @欧石楠 户型的底图在拆改模式下也需要绘制
    {
        if (InArg._ArgInt32 == 1)//add
        {
            ReferenceImage = InObj;
        }
        else if (InArg._ArgInt32 == 0)//remove
        {
            ReferenceImage = nullptr;
        }
    }
}

void FArmyLayoutModeController::OnSaveCommand()
{
    FArmyFrameCommands::OnMenuSave();
}

//void FArmyLayoutModeController::OnUndoCommand()
//{
//	FArmyFrameCommands::OnMenuUndo();	
//}
//
//void FArmyLayoutModeController::OnRedoCommand()
//{
//	FArmyFrameCommands::OnMenuRedo();		
//}

void FArmyLayoutModeController::OnEmptyCommand()
{
    // 户型模式下清空所有
	/**@欧石楠 当清空之前取消所有选中*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
    GGI->Window->PresentModalDialog(TEXT("是否清空？"), FSimpleDelegate::CreateRaw(this, &FArmyLayoutModeController::OnClear));
}

void FArmyLayoutModeController::OnAddWallCommand()
{
	/**@欧石楠 限制拆改后状态下无法操作拆改的相关功能*/
	if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改后状态下无法新建墙体！"));
		return;
	}
	/** @欧石楠 在开始一个新的operation之前清空选择*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
    SetOperation(LO_AddWall, XRArgument(1));
}

void FArmyLayoutModeController::OnModifyWallCommand()
{
	/**@欧石楠 限制拆改后状态下无法操作拆改的相关功能*/
	if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改后状态下无法拆除墙体！"));
		return;
	}
	/** @欧石楠 在开始一个新的operation之前清空选择*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
    SetOperation(LO_ModifyWall, XRArgument(1));
}

void FArmyLayoutModeController::OnPackPipeCommand()
{
	if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改后状态下无法构建包立管！"));
		return;
	}
	/** @欧石楠 在开始一个新的operation之前清空选择*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(LO_PackPipe, XRArgument(1));
}

void FArmyLayoutModeController::OnModifyModeModifyOperation()
{
	FArmySceneData::Get()->bUpdateModifyModeData = true;
	FArmySceneData::Get()->LoadDataFromJason = false;

	GGI->DesignEditor->OnBaseDataChanged(0);
}

void FArmyLayoutModeController::OnSplitRegion()
{
	/** @纪仁泽  限制拆改中状态下无法操作区域分割的相关功能 */
	if (FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改中状态下无法进行区域分割！"));
		return;
	}
	SetOperation(LO_SplitRegion, XRArgument(1));
}

void FArmyLayoutModeController::OnDoorCommand()
{
	/**@欧石楠 限制拆改后状态下无法操作拆改的相关功能*/
	if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改后状态下无法放置标准门！"));
		return;
	}
	/** @欧石楠 在开始一个新的operation之前清空选择*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(LO_Hardware, XRArgument(OT_Door));
}

void FArmyLayoutModeController::OnSlidingDoorCommand()
{
	/**@欧石楠 限制拆改后状态下无法操作拆改的相关功能*/
	if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改后状态下无法放置推拉门！"));
		return;
	}
	/** @欧石楠 在开始一个新的operation之前清空选择*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(LO_Hardware, XRArgument(OT_SlidingDoor));
}

void FArmyLayoutModeController::OnComponentCommand(XRArgument InArg)
{
	switch (InArg._ArgInt32)
	{	
	case OT_CurtainBox:
		SetOperation(LO_Component, XRArgument().ArgInt32(0).ArgUint32(OT_CurtainBox).ArgString(TEXT("Beam.dxf")).ArgFName(TEXT("窗帘盒")));
		break;	
	default:
		break;
	}
	/**@欧石楠 用于点击结构部件后立马启用标尺并启用自动吸附*/
	FArmyToolsModule::Get().GetRectSelectTool()->AddToSelectedObject(ComponentOperation->GetSelectedComponent());
	AxisRuler->SetBoundObj(ComponentOperation->GetSelectedComponent());
	AxisRuler->SetEnableAutoAttach(true);
}
void FArmyLayoutModeController::OnNewPassCommand()
{
	/**@欧石楠 限制拆改后状态下无法操作拆改的相关功能*/
	if (!FArmySceneData::Get()->bIsDisplayDismantle)
	{
		GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("拆改后状态下无法新开门洞！"));
		return;
	}
	/** @欧石楠 在开始一个新的operation之前清空选择*/
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	SetOperation(LO_Hardware, XRArgument(OT_NewPass));
}
//
//void FArmyLayoutModeController::OnPointLocationCommand(XRArgument InArg)
//{
//	switch (InArg._ArgInt32)
//	{
//	case OT_Drain_Point:
//		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(OT_Drain_Point).ArgString(TEXT("Location_DrainPoint.dxf")).ArgFName(TEXT("下水主管道")));
//		break;
//	case OT_Switch1O2_Point:
//		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(OT_Switch1O2_Point).ArgString(TEXT("Location_Switch1O2_Point.dxf")).ArgFName(TEXT("单联双控开关")));
//		break;
//	case OT_Closestool://坐便下水
//		SetOperation(HO_Component, XRArgument().ArgUint8(1).ArgInt32(0).ArgUint32(OT_Closestool).ArgString(TEXT("Location_Closestool.dxf")).ArgFName(TEXT("坐便下水")));
//		break;
//	default:
//		break;
//	}
//
//	TSharedPtr<FArmyFurniture> Furniture = ComponentOperation->GetSelectedComponent();
//	if (Furniture.IsValid())
//	{
//		ComponentOperation->ShowAxisRulerDelegate.BindRaw(this, &FArmyLayoutModeController::ShowAxisRuler);
//		Furniture->SetbUseCustomColor(false);
//		Furniture->SetCustomoColor(FLinearColor::White);
//		FArmyToolsModule::Get().GetRectSelectTool()->AddToSelectedObject(Furniture);
//		AxisRuler->SetBoundObj(nullptr);
//		AxisRuler->SetEnableAutoAttach(false);
//	}
//}

void FArmyLayoutModeController::OnSelectionChanged(TArray< TSharedPtr<FArmyObject> >& ObjArray)
{
	if (ObjArray.Num() == 1)
    {
		DetailWidget->ShowSelectedDetial(ObjArray[0]);
	}	
	else if (ObjArray.Num() > 0)
    {
		//TODO Muti selection function
		DetailWidget->ShowSelectedDetial(nullptr);
	}
	else
    {
		DetailWidget->ShowSelectedDetial(nullptr);
	}
}

void FArmyLayoutModeController::OnDeleteExcute()
{ 
	AxisRuler->SetBoundObj(nullptr);
	const TArray<FObjectWeakPtr>& SelectedObjects = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects();
	TArray<FArmyRectSelect::FPrimitiveInfo> SelectedPrimitives = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedPrimitives();
	if (SelectedObjects.Num() == 0 && SelectedPrimitives.Num() >= 0)
		return;

	//如果删除了拆改墙，判断是否有新建墙覆盖在上面，有则不能删除
	if (SelectedObjects.Num() > 0 && SelectedObjects[0].Pin()->GetType() == OT_ModifyWall)
	{
		TSharedPtr<FArmyModifyWall> SelectedModifyWall = StaticCastSharedPtr<FArmyModifyWall>(SelectedObjects[0].Pin());
		TArray<TSharedPtr<FArmyLine>> ModifyLines;
		SelectedModifyWall->GetLines(ModifyLines);
		TArray<TWeakPtr<FArmyObject>> AddWallList;
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_AddWall, AddWallList);
		FArmySceneData::Get()->GetObjects(E_ModifyModel, OT_IndependentWall, AddWallList);
		for (auto It : AddWallList)
		{
			TArray<TSharedPtr<FArmyLine>> AddWallLines;
			It.Pin()->GetLines(AddWallLines);
			for (auto ItLine : AddWallLines)
			{
				for (auto ItModifyLine : ModifyLines)
				{
					FVector CurrentIntersect;
					if (FArmyMath::CalculateLinesCross(ItLine->GetStart(), ItLine->GetEnd(),
						ItModifyLine->GetStart(), ItModifyLine->GetEnd(), CurrentIntersect))
					{
						GGI->Window->ShowMessage(EXRMessageType::MT_Warning, TEXT("请先删除覆盖在拆改墙上面的新建墙"));
						return;
					}
				}
				
			}
		}
	}

	SCOPE_TRANSACTION(TEXT("删除选中的物体"));

	// 如果选中的为一个分割线则进行分割线删除操作
	if (SelectedObjects.Num() == 1 && SelectedObjects[0].Pin()->GetType() == OT_SplitLine)
	{
		SetOperation(LO_SplitRegion, XRArgument(0));
		CurrentOperation->SetSelected(SelectedObjects[0].Pin(), MakeShareable(new FArmyLine()));
		return;
	}
	else
	{
		for (auto It : SelectedObjects)
		{
			if (It.Pin()->GetType() != OT_SplitLine)
			{
				// 不能删除分割线
				FArmySceneData::Get()->Delete(It.Pin());
			}	
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
		}
	}
	
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmySceneData::ModifyMode_ModifyMultiDelegate.Broadcast();

	GGI->Window->DismissModalDialog();
}

TSharedRef<SWidget> FArmyLayoutModeController::OnDismantleStateCommand()
{
    const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
    TSharedRef<FUICommandList> ActionList = MakeShareable(new FUICommandList);

	FCanExecuteAction DefaultCanExec = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::DefaultCanExecuteAction);
    FIsActionChecked IsDisplayDismantle = FIsActionChecked::CreateRaw(this, &FArmyLayoutModeController::IsDisplayDismantle);
    FIsActionChecked IsDisplayAfterDismantle = FIsActionChecked::CreateRaw(this, &FArmyLayoutModeController::IsDisplayAfterDismantle);

    ActionList->MapAction(FArmyLayoutModeCommands::Get().CommandDismantle, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnDisplayDismantle), DefaultCanExec, IsDisplayDismantle);
    ActionList->MapAction(FArmyLayoutModeCommands::Get().CommandAfterDismantle, FExecuteAction::CreateRaw(this, &FArmyLayoutModeController::OnDisplayAfterDismantle), DefaultCanExec, IsDisplayAfterDismantle);

    CommandList->Append(ActionList);

    FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
    MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");
    MenuBuilder.AddMenuEntry(FArmyLayoutModeCommands::Get().CommandDismantle);
    MenuBuilder.AddMenuEntry(FArmyLayoutModeCommands::Get().CommandAfterDismantle);

    return MenuBuilder.MakeWidget();
}

void FArmyLayoutModeController::OnDisplayDismantle()
{
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmySceneData::Get()->bIsDisplayDismantle = true;
}

void FArmyLayoutModeController::OnDisplayAfterDismantle()
{
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmySceneData::Get()->bIsDisplayDismantle = false;
	if (FArmySceneData::Get()->bUpdateModifyModeData)
	{
		FArmySceneData::CopyModeData(E_ModifyModel, E_LayoutModel, true);
		FArmySceneData::Get()->bUpdateModifyModeData = false;
		FArmySceneData::Get()->bUpdateLayoutModeData = true;
	}
}

void FArmyLayoutModeController::OnClear()
{

	if (FArmySceneData::Get()->bIsDisplayDismantle)
	{
		HomeDataLayers_Modify.Empty();
		HomeDataLayers_Layout.Empty();
        FArmySceneData::Get()->ClearModifyData();
		FArmySceneData::Get()->CopyModeData(E_HomeModel, E_ModifyModel, true);
		FArmySceneData::Get()->CopyModeData(E_ModifyModel, E_LayoutModel, true);

		FArmySceneData::Get()->bUpdateModifyModeData = true;
	}
	else
	{
		HomeDataLayers_Layout.Empty();
		FArmySceneData::Get()->CopyModeData(E_ModifyModel, E_LayoutModel, true);

		FArmySceneData::Get()->bUpdateLayoutModeData = true;
	}

    // 还原墙线颜色和底图透明度
    FArmySceneData::Get()->SetWallLinesColor(FLinearColor::White);
    FArmySceneData::Get()->SetFacsimileTransparency(60);
}

// 初始化区域中的分割线
void FArmyLayoutModeController::InitSplitLineInRegion()
{
	// 初始化区域中的分割线
	TArray<TWeakPtr<FArmyObject>>	  RegionArray;
	FArmySceneData::Get()->GetObjects(E_LayoutModel, OT_Region, RegionArray);
	for (auto &Region : RegionArray)
	{
		TSharedPtr<FArmyRegion> RegionObj = StaticCastSharedPtr<FArmyRegion>(Region.Pin());
		for (auto It : RegionObj->GetSplitLineIdList())
		{
			TWeakPtr<FArmyObject> SplineLine= FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, It);
			TSharedPtr<FArmySplitLine> SplineLineObj = StaticCastSharedPtr<FArmySplitLine>(SplineLine.Pin());
			RegionObj->AddSplitLine(SplineLineObj);
		}
	}
}