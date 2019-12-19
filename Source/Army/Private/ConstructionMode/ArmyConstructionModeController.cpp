#include "ArmyConstructionModeController.h"
#include "SArmyOperationBrowser.h"
#include "ArmyConstructionModeCommands.h"
#include "ArmyViewportClient.h"
#include "ArmyToolsModule.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "MultiBoxBuilder.h"

#include "ArmyToolBarBuilder.h"
#include "ArmyDimensionsOperation.h"
#include "ArmyDownLeadLabelOperation.h"
#include "ArmyEditObjectOperation.h"
#include "ArmyArcLineOperation.h"
#include "ArmyConstructionFrame.h"
#include "ArmyTextLabel.h"
#include "ArmyDownLeadLabel.h"
#include "ArmyObjAutoRuler.h"
#include "ArmyBaseArea.h"
#include "ArmyCompass.h"
#include "ArmyFrameCommands.h"
#include "ArmyConstructionFunction.h"
#include "ArmyGameInstance.h"
#include "SArmyGeneratePrint.h"
#include "SArmyProgressBar.h"
#include "SArmyDynamicFontSize.h"
#include "ArmySettings.h"
#include "ArmyRoom.h"
#include "ArmyDimensionDownLeadOperation.h"
#include "Kismet/GameplayStatics.h"
#include "ArmyPlayerController.h"
#include "ArmyTextLabelOperation.h"
#include "ArmyBoxChooseOperation.h"
#include "ArmyDownloadCad.h"
#include "ArmyBoardSplitOperation.h"
#include "SArmyHydropowerAutoDesignError.h"

//导出CAD

#include "WindowsFileUtilityFunctionLibrary.h"
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "WindowsDirectories.h"
#include "SCTResourceTools.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/PlatformFile.h"

#include "ArmyConstructionLayerManager.h"

#include "ArmySymbolLeadLine.h"

//#define DEBUG_WALLFACELAYER

FArmyConstructionModeController::~FArmyConstructionModeController()
{
	StartShotScreen = false;
	GVC->ScreenShotFun.Unbind();
	FArmyConstructionCommands::Unregister();

	FArmySceneData::Get()->PreOperationDelegate.RemoveAll(this);

}

const EModelType FArmyConstructionModeController::GetDesignModelType()
{
	return EModelType::E_ConstructionModel;
}

void FArmyConstructionModeController::Init()
{
	LayerManager = MakeShareable(new FArmyConstructionLayerManager);
	LocalFunction = MakeShareable(new FArmyConstructionFunction);

	LocalModelService = MakeShareable(new FArmyResourceService);
	LocalModelService->OnLoadConstructionFrame.BindRaw(this, &FArmyConstructionModeController::LoadConstructionFrame);
	LocalModelService->OnFrameFillContentCallBack.BindRaw(this, &FArmyConstructionModeController::FillConstructionFrame);
	LocalModelService->OnLoadConstructionLayersCallBack.BindRaw(this, &FArmyConstructionModeController::InitLayers);
	LocalModelService->OnUploadImageStep.BindRaw(this, &FArmyConstructionModeController::OnUploadImageStep);
	LocalModelService->OnLoadFrameLogo.BindRaw(this, &FArmyConstructionModeController::OnLoadFrameLogo);

	LocalModelService->RequestLoadConstructionFrameLogo();//全局请求一次图框logo
	//@ 梁晓菲 手动标尺，标注，图框，label，构件等可以被捕捉
	SelectedObjArray.Add(OT_ConstructionFrame);
	SelectedObjArray.Add(OT_DownLeadLabel);
	SelectedObjArray.Add(OT_Dimensions);
	SelectedObjArray.Add(OT_TextLabel);
	SelectedObjArray.Add(OT_ComponentBase);
	SelectedObjArray.Add(OT_Compass);
	SelectedObjArray.Add(OT_Entrance);
	//@ 梁晓菲 自动标尺可以被捕捉
	SelectedObjArray.Add(OT_InSideWallDimensions);
	SelectedObjArray.Add(OT_OutSideWallDimensions);
	SelectedObjArray.Add(OT_AddWallDimensions);
	SelectedObjArray.Add(OT_DeleteWallDimensions);
	//@飞舞轻扬 开关灯具控制图控制点
	SelectedObjArray.Add(OT_LampControlLine);
	//橱柜台面分割线
	SelectedObjArray.Add(OT_BoardSplitline);

	SAssignNew(DynamicFontSizeSetWidget, SArmyDynamicFontSize);

	GVC->ScreenShotFun = ScreenShotDelegate::CreateRaw(this, &FArmyConstructionModeController::OnScreenShot);

	FArmySceneData::Get()->PreOperationDelegate.AddRaw(this, &FArmyConstructionModeController::OnObjectOperation);

	LocalFunction->DelegateNewLayer.BindRaw(this, &FArmyConstructionModeController::DelegateNewLayer);
	// 注册命令代理
	FArmyConstructionCommands::Register();

	FArmyDesignModeController::Init();

	LastObjMap = GetAllObjectWithoutConstruct();
	DelegateInterface->OnCadDoneDelegate.BindRaw(this, &FArmyConstructionModeController::MoveZipToDir);

	IndexSubController = NewVC<FArmyConstructionIndexSubModeController>();
	IndexSubController->DelegateFloatButtonClicked.BindRaw(this, &FArmyConstructionModeController::IndexSubCommand);
}
void FArmyConstructionModeController::InitOperations()
{
	OperationMap.Add(HO_Dimensions, MakeShareable(new FArmyDimensionsOperation(E_ConstructionModel)));
	TSharedPtr<FArmyDownLeadLabelOperation> DownLabelOperation = MakeShareable(new FArmyDownLeadLabelOperation(E_ConstructionModel));
	DownLabelOperation->OnGetObjectArrayDelegate.BindRaw(this, &FArmyConstructionModeController::GetCurrentObjects);
	OperationMap.Add(HO_DownLeadLabel, DownLabelOperation);

	/*@梁晓菲*/
	OperationMap.Add(HO_DimensionDownLead, MakeShareable(new FArmyDimensionDownLeadOperation(E_ConstructionModel)));
	OperationMap.Add(HO_ObjectOperation, MakeShareable(new FArmyEditObjectOperation(E_ConstructionModel)));
	OperationMap.Add(HO_ARCLINE, MakeShareable(new FArmyArcLineOperation(E_ConstructionModel)));
	OperationMap.Add(HO_TextLabel, MakeShareable(new FArmyTextLabelOperation(E_ConstructionModel)));
	OperationMap.Add(HO_BoxChoose, MakeShareable(new FArmyBoxChooseOperation(E_ConstructionModel)));

	OperationMap.Add(HO_BoradSplit, MakeShareable(new FArmyBoardSplitOperation(E_ConstructionModel)));

	for (auto Operation : OperationMap)
	{
		Operation.Value->Init();
		Operation.Value->InitWidget(GVC->ViewportOverlayWidget);
		Operation.Value->EndOperationDelegate.BindRaw(this, &FArmyConstructionModeController::EndOperation);
	}
}
void FArmyConstructionModeController::BeginMode()
{
	FArmyDesignModeController::BeginMode();

	GVC->SetDrawMode(DM_2D);
	GVC->SetDrawGridVisibility(false);
	FArmyObject::SetDrawModel(MODE_CONSTRUCTION, true);

	FArmyToolsModule::Get().GetMouseCaptureTool()->Reset();
	FArmyToolsModule::Get().GetMouseCaptureTool()->SetRefCoordinateSystem(FVector(ForceInitToZero), FVector(1, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1));
	FArmyToolsModule::Get().GetMouseCaptureTool()->EnableCapture = false;

	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
	FArmyToolsModule::Get().GetRectSelectTool()->End();
	FArmyToolsModule::Get().GetRectSelectTool()->EnableMultipleSelect(true);
	FArmyToolsModule::Get().GetRectSelectTool()->Start();

	//对比上次数据，做对应户型的变化
	TMap<unsigned long long, HomeModelInfo> CurrentObjMap;
	CurrentObjMap = GetAllObjectWithoutConstruct();
	LocalFunction->ClassTypeList = CalcNeedChangeType(CheckDifferentObj(LastObjMap, CurrentObjMap));
	LastObjMap = CurrentObjMap;
	if (LocalFunction->ClassTypeList.Num() > 0 && !BeenUpdated)
	{
		//提示更新弹窗
		ShowUpdateTitle();
	}
	if (BeenUpdated)
	{
		OnUpdateDismensionCommand();
		BeenUpdated = false;
	}

	if (BaseDataChanged)
	{
		//提示更新
		//GGI->Window->ShowMessage(MT_Normal, TEXT("户型数据发生变化，需要更新图纸标尺信息！"));
	}

	FArmyToolsModule::Get().GetRectSelectTool()->BIsConstruction = true;// 图纸模式，关闭捕捉
	FArmySceneData::Get()->bIsDisplayDismantle = true;// true显示拆改中，false显示拆改后
	if (LayerManager->GetCurrentLayer().IsValid()) OnLayerChanged(LayerManager->GetCurrentLayer().Pin()->GetName());
}

bool FArmyConstructionModeController::EndMode()
{
	//关闭弹窗
	if (UpdateOverlay != nullptr)
	{
		UpdateOverlay->DetachWidget();
	}

	EndOperation();
	FArmyObject::SetDrawModel(MODE_CONSTRUCTION, false);
	FArmyObject::SetDrawModel(MODE_MODIFYADD, true);
	FArmyObject::SetDrawModel(MODE_MODIFYDELETE, true);
	FArmyObject::SetDrawModel(MODE_ADDWALLPOST, false);
	FArmyObject::SetDrawModel(MODE_DELETEWALLPOST, false);
	FArmyObject::SetDrawModel(MODE_LAMPSTRIP, false);
	FArmyObject::SetDrawModel(MODE_OTHERCEILINGOBJ, false);
	FArmyObject::SetDrawModel(MODE_DOORPASS, false);
	FArmyToolsModule::Get().GetRectSelectTool()->BIsConstruction = false;//非图纸模式，开启捕捉
	GVC->SetDrawGridVisibility(true);
	FArmyDesignModeController::EndMode();

	return true;
}
void FArmyConstructionModeController::EndOperation()
{
	FontSizeVisible = EVisibility::Collapsed;
	GVC->ViewportOverlayWidget->RemoveSlot(DynamicFontSizeSetWidget.ToSharedRef());
	FArmyDesignModeController::EndOperation();
	FArmyToolsModule::Get().GetRectSelectTool()->Start();
}

void FArmyConstructionModeController::Clear()
{
	// @ 梁晓菲 清空HomeData的LocalDatas数据
	//BeenUpdated = false;

	TMap<FObjectWeakPtr, FLayerInfo> TempObjMap = LocalDatas;
	for (auto& LocalIt : TempObjMap)
	{
		FArmySceneData::Get()->Delete(LocalIt.Key.Pin());
	}
	LocalDatas.Empty();
	LocalAutoObjectMap.Empty();

	if (CurrentFrame.IsValid())
	{
		CurrentFrame = NULL;
	}

	for (auto& LInfo : LocalFunction->CupboardTableLayerArray)
	{
		DeleteLayer(LInfo.LayerName);
	}
	LocalFunction->CupboardTableLayerArray.Empty();

	LayerFrameMap.Empty();

	//@马云龙 清空所有动态图层，如橱柜立面图，索引立面图
	for (auto& It : LayerManager->GetDynamicLayers())
	{
		if (It.Pin().IsValid())
		{
			DeleteLayer(It.Pin()->GetName());
		}
	}

	/*if(LayerOutLiner->RootTreeItems.Num() > 0) OnLayerSelect(LayerOutLiner->RootTreeItems[0],true);*/
}
void FArmyConstructionModeController::OnBaseDataChanged(int32 InDataType)
{
	if (InDataType == 0)
	{
		BaseDataChanged = true;
	}
}

void FArmyConstructionModeController::OnResetDismensionCommand()
{
	GGI->Window->DismissModalDialog();
	GGI->Window->PresentModalDialog(TEXT("重置标注，图纸标注会根据当前设计重新生成，是否继续？"),
			FSimpleDelegate::CreateLambda([this]() {
			//1、将全部的自动生成标注、Label、标识删除
			ClearAutoObjects();
			//2、存储之前需要改变的ClassType，并标记全部标注都刷新
			LocalFunction->ClassTypeList.Empty();
			LocalFunction->ClassTypeList = { Class_Other };
			//3、刷新标注
			LocalFunction->GenerateAutoDimensions();
			LocalFunction->GenerateAutoObjectRuler();
			//更新橱柜台面
			LocalFunction->GenerateAutoLayers();

			//4、还原之前需改变的ClassType
			LocalFunction->ClassTypeList.Empty();
			//5、关闭弹窗
			GGI->Window->DismissModalDialog();
		}),
        FSimpleDelegate(),
		FSimpleDelegate::CreateLambda([]() {
			GGI->Window->DismissModalDialog();
		}), TEXT("确定"), TEXT("取消"), false, TEXT("提醒")
	 );
}
void FArmyConstructionModeController::OnUpdateDismensionCommand()
{
	//清空需要改变的对象 
	ClearAutoObjectsWithClassType(LocalFunction->ClassTypeList);
	//刷新
	LocalFunction->GenerateAutoDimensions();
	LocalFunction->GenerateAutoObjectRuler();
	LocalFunction->GenerateAutoLayers();
	//已刷新后将类型制空
	LocalFunction->ClassTypeList.Empty();

	//取消显示弹窗
	if (UpdateOverlay != nullptr)
	{
		UpdateOverlay->DetachWidget();
	}
}
void FArmyConstructionModeController::GetCurrentObjects(TArray<FObjectWeakPtr>& OutObjArray)
{
	TWeakPtr<FArmyLayer> CurrentLayer = LayerManager->GetCurrentLayer();
	if (CurrentLayer.IsValid())
	{
		for (auto ClassIt : CurrentLayer.Pin()->GetAllClass())
		{
			if (ClassIt.Value->IsVisible())
			{
				OutObjArray.Append(ClassIt.Value->GetAllObjects());
			}
		}
	}
}
void FArmyConstructionModeController::CollectAllLinesAndPoints(TArray< TSharedPtr<FArmyLine> >& InOutLines, TArray< TSharedPtr<FArmyPoint> >& InOutPoints, TArray<TSharedPtr<FArmyCircle>>& InOutCircles)
{
	TWeakPtr<FArmyLayer> CurrentLayer = LayerManager->GetCurrentLayer();
	if (CurrentLayer.IsValid())
	{
		for (auto ClassIt : CurrentLayer.Pin()->GetAllClass())
		{
			if (ClassIt.Value->IsVisible())
			{
				for (FObjectPtr ObjectIt : ClassIt.Value->GetAllObjects())
				{
					if (CurrentOperation.IsValid() && CurrentOperation->GetOperationObject() == ObjectIt)
					{
						continue;
					}
					if (ObjectIt->GetPropertyFlag(FArmyObject::FLAG_VISIBILITY))
					{
						TArray< TSharedPtr<FArmyLine> > Lines;
						ObjectIt->GetLines(Lines, true);
						InOutLines.Append(Lines);

						TArray< TSharedPtr<FArmyCircle> > Circles;
						ObjectIt->GetCircleLines(Circles);
						InOutCircles.Append(Circles);

						TArray< TSharedPtr<FArmyPoint> > Points;
						ObjectIt->GetAlonePoints(Points);
						InOutPoints.Append(Points);
					}
				}
			}
		}
	}

	if (CurrentOperation.IsValid()) InOutLines.Append(CurrentOperation->GetLines());
}
TSharedPtr<SWidget> FArmyConstructionModeController::MakeLeftPanelWidget()
{
	TSharedPtr<SArmyOperationBrowser> HomeCategory = SNew(SArmyOperationBrowser);

	HomeCategory->BeginOperationCategory(TEXT("标尺"), FArmyStyle::Get().GetBrush("Icon.Ruler"));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDimension, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_Dimensions, XRArgument(0)));
	HomeCategory->BuildSubOperationCategory();

	HomeCategory->BeginOperationCategory(TEXT("标注"), FArmyStyle::Get().GetBrush("Icon.Note"));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDownLeadLabel_Size, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_DownLeadLabel, XRArgument(1).ArgUint32(1).ArgString(TEXT("规格："))));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDownLeadLabel_Material, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_DownLeadLabel, XRArgument(1).ArgUint32(2).ArgString(TEXT("材质："))));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDownLeadLabel_Text, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_DownLeadLabel, XRArgument(1).ArgUint32(3).ArgString(TEXT("注释："))));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDownLeadLabel_Craft, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_DownLeadLabel, XRArgument(1).ArgUint32(4).ArgString(TEXT("工艺："))));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDownLeadLabel_Height, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_DownLeadLabel, XRArgument(1).ArgUint32(5).ArgString(TEXT("高度："))));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandDownLeadLabel_CustomHeight, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCustomHeight, HO_ObjectOperation, XRArgument(1).ArgBoolean(true).ArgString(TEXT("NewObject"))));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandBoradSplitLine, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnCommandOperation, HO_BoradSplit, XRArgument(0)));
	HomeCategory->BuildSubOperationCategory();

	HomeCategory->BeginOperationCategory(TEXT("其他"), FArmyStyle::Get().GetBrush("Icon.Other"));
	HomeCategory->AddOperation(FArmyConstructionCommands::Get().CommandCompass, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnAddCompass));
	HomeCategory->BuildSubOperationCategory();

	return HomeCategory;
}
TSharedRef<SWidget> FArmyConstructionModeController::CreateLayerOutliner()
{
	SAssignNew(LayerOutLiner, SArmyObjectOutliner);
	LayerOutLiner->TreeType = OUTLINER_LAYERLIST;
	LayerOutLiner->TreeEventTrigger = OutlinerTreeEvent::CreateRaw(this, &FArmyConstructionModeController::OnLayerSelect);
	//LayerOutLiner->TreeRowModify = OutlinerTreeRowModify::CreateSP(this, &FArmyConstructionModeController::OnLayerNameModify);
	//LayerOutLiner->PopMenu = OutlinerPopMenu::CreateSP(this, &FArmyConstructionModeController::CreatePopMenu);
	LocalModelService->RequestConstructionLayers();
	return LayerOutLiner.ToSharedRef();
}
void FArmyConstructionModeController::OnAddCompass()
{
	TSharedPtr<FArmyCompass> Compass = MakeShareable(new FArmyCompass);
	FArmySceneData::Get()->Add(Compass, XRArgument(1).ArgUint32(E_ConstructionModel));

	SCOPE_TRANSACTION(TEXT("生成指北针"));
	Compass->Create();
	FTransform DefaultTrans;
	DefaultTrans.SetScale3D(FVector(1.f / 3.f, 1.f / 3.f, 1.f / 3.f));
	Compass->ApplyTransform(DefaultTrans);
	Compass->GetMoveOperationPoint();
	SetOperation(HO_ObjectOperation, XRArgument().ArgBoolean(true));

	if (CurrentOperation.IsValid())
	{
		CurrentOperation->SetSelected(Compass, Compass->GetMoveOperationPoint());
	}
}
void FArmyConstructionModeController::OnCommandOperation(EConstrctionOperation InOp, XRArgument InArg)
{
	SetOperation(InOp, InArg);
}

void FArmyConstructionModeController::OnCustomHeight(EConstrctionOperation InOp, XRArgument InArg /*= XRArgument()*/)
{
	SetOperation(HO_ObjectOperation, InArg);

	FVector2D MousePos(0, 0);
	FVector Pos(0, 0, 0);
	if (GVC->GetMousePosition(MousePos))
	{
		Pos = FVector(GVC->PixelToWorld(MousePos.X, MousePos.Y, 0));
	}

	TSharedPtr<FArmyTextLabel> TextLabel = MakeShareable(new FArmyTextLabel());
	TextLabel->Init(GVC->ViewportOverlayWidget);
	TextLabel->SetWorldPosition(Pos);
	TextLabel->SetLabelContent(FText::GetEmpty());
	TextLabel->SetTextColor(FLinearColor(0.5, 0.5, 0.5));
	TextLabel->SetLabelType(FArmyTextLabel::LabelType::LT_CustomHeight);

	CurrentOperation->SetSelected(TextLabel, TextLabel->GetEditPoint());
}

void FArmyConstructionModeController::OnLayerSelect(const SArmyObjectTreeItemPtr InItem, bool InV)
{
	if (InItem.IsValid())
	{
		OnLayerChanged(InItem->ModuleName);
	}
}

void FArmyConstructionModeController::OnSetFontHoverd()
{
	SetFontTextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF9800"));
}

void FArmyConstructionModeController::OnSetFontUnHoverd()
{
	SetFontTextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"));
}

//init layers init class setting - once
//auto ruler when change model
void FArmyConstructionModeController::InitLayers(const TSharedPtr<FJsonObject> InJsonObj)
{
	LayerOutLiner->EmptyTreeItems();

	if (InJsonObj.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>>* ConstructionLayerArray;
		InJsonObj->TryGetArrayField(TEXT("layerList"), ConstructionLayerArray);
		//遍历每个图层
		for (auto LayerIt : *ConstructionLayerArray)
		{
			TSharedPtr<FJsonObject> JsonLayer = LayerIt->AsObject();

			uint32 ShowContent,Type;

			FArmyLayer::FLayerType LayerType = FArmyLayer::LT_NONE;

			TWeakPtr<FArmyLayer> NewWeakLayer;
			if (JsonLayer->TryGetNumberField(TEXT("showType"), ShowContent))
			{
				if (ShowContent == 0)
				{
					LayerType = FArmyLayer::LT_S_ORIGINAL;//该图层显示的原始图层
				} 
			}
			if (JsonLayer->TryGetNumberField(TEXT("type"), Type))
			{
				if (Type == 3)
				{
					LayerType = FArmyLayer::LT_D_WALLINDEXEDITOR;//该图层立面索引编辑图层，该图层在立面索引编辑界面用，在施工图图层列表不显示
				}
			}
			if (LayerType == FArmyLayer::LT_D_WALLINDEXEDITOR)
			{
				TSharedPtr<FArmyLayer> IndexLayer = MakeShareable(new FArmyLayer(false));
				NewWeakLayer = IndexLayer;
				IndexSubController->SetDataContainner(IndexLayer);
				NewWeakLayer.Pin()->SetName(FName(*(JsonLayer->GetStringField(TEXT("name")))));
			}
			else
			{
				NewWeakLayer = NewLayer(FName(*(JsonLayer->GetStringField(TEXT("name")))));
			}

			NewWeakLayer.Pin()->SetLayerType(LayerType);

			bool PostWallModel = true;

			const TArray<TSharedPtr<FJsonValue>>* LayerClassArray;
			//每个图层需要显示的数据列表
			if (JsonLayer->TryGetArrayField(TEXT("categoryList"), LayerClassArray))
			{
				for (auto ClassIt : *LayerClassArray)
				{
					TSharedPtr<FJsonObject> JsonClass = ClassIt->AsObject();

					int32 ClassID = JsonClass->GetIntegerField(TEXT("id"));
					switch (ClassID)
					{
					case Class_AddWall:
						PostWallModel = false;
						NewWeakLayer.Pin()->SetFlag(NewWeakLayer.Pin()->GetFlag() | MODE_DELETEWALLPOST);//拆改墙绘制状态，用来区分户型拆改后的真实（拆改墙没有彩色及填充）
						InitClassVisible(NewWeakLayer, ObjectClassType(ClassID));
						break;
					case Class_BreakWall:
						PostWallModel = false;
						NewWeakLayer.Pin()->SetLayerType(FArmyLayer::LT_S_MODIFY);
						InitClassVisible(NewWeakLayer, ObjectClassType(ClassID));
						//newLayer.Pin()->SetFlag(newLayer.Pin()->GetFlag() | MODE_ADDWALLPOST);//拆改墙绘制状态，用来区分户型拆改后的真实（拆改墙没有彩色及填充）
						break;
					case Class_LampStrip://顶面灯带数据
						NewWeakLayer.Pin()->SetFlag(NewWeakLayer.Pin()->GetFlag() | MODE_LAMPSTRIP);
						InitClassVisible(NewWeakLayer, Class_Ceilinglayout);
						break;
					case Class_Ceilinglayout://顶面其他数据
						NewWeakLayer.Pin()->SetFlag(NewWeakLayer.Pin()->GetFlag() | MODE_OTHERCEILINGOBJ);
						InitClassVisible(NewWeakLayer, Class_Ceilinglayout);
						break;
					default:
						InitClassVisible(NewWeakLayer, ObjectClassType(ClassID));
						break;
					}
				}
			}
			if (!NewWeakLayer.Pin()->GetOrCreateClass(Class_Ceilinglayout, false).IsValid())
			{
				NewWeakLayer.Pin()->SetFlag(NewWeakLayer.Pin()->GetFlag() | MODE_OTHERCEILINGOBJ);
			}
			if (PostWallModel)
			{
				NewWeakLayer.Pin()->SetFlag(NewWeakLayer.Pin()->GetFlag() | MODE_ADDWALLPOST);//拆改墙绘制状态，用来区分户型拆改后的真实（拆改墙没有彩色及填充）
				NewWeakLayer.Pin()->SetFlag(NewWeakLayer.Pin()->GetFlag() | MODE_DELETEWALLPOST);//拆改墙绘制状态，用来区分户型拆改后的真实（拆改墙没有彩色及填充）
			}
			InitClassVisible(NewWeakLayer, Class_Other);
		}
	}
	//默认选中第一个图层
	if (LayerOutLiner->RootTreeItems.Num() > 0)
	{
		LayerOutLiner->SelectItem(LayerOutLiner->RootTreeItems.HeapTop());
	}
	if (!IsCurrentModel)
	{
		EndMode();
	}
}
TWeakPtr<FArmyLayer> FArmyConstructionModeController::DelegateNewLayer(const FName& InLayerName, bool IsStaticLayer,int32 InLayerType)
{
	TWeakPtr<FArmyLayer> TempNewLayer = NewLayer(InLayerName, IsStaticLayer);
	TempNewLayer.Pin()->SetLayerType(FArmyLayer::FLayerType(InLayerType));

	//if (FArmyLayer::FLayerType(InLayerType) == FArmyLayer::LT_D_CUPBOARDTABLE)
	//{
	TempNewLayer.Pin()->GetOrCreateClass(Class_BaseWall)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_Door)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_Pass)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_Window)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_Furniture)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_SCTCabinet)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_SCTHardware)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_Other)->SetVisible(true);
	TempNewLayer.Pin()->GetOrCreateClass(Class_ConstructionFrame)->SetVisible(true);
	//}

	//@马云龙 对于载入过程中创建的立面图，需要强制对某些物体做显示，这里的Visibility信息最好保存在Json，之后这段强制执行的代码就不需要了
	if (FArmyLayer::FLayerType(InLayerType) == FArmyLayer::LT_D_WALLFACADE)
	{
		TempNewLayer.Pin()->GetOrCreateClass(Class_Switch)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Socket)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ElectricBoxH)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ElectricBoxL)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Light)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_AirConditionerPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_WaterRoutePoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_HotWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_HotAndColdWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ColdWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_FloordRainPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ClosestoolRainPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_WashBasinRainPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_RecycledWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_HomeEntryWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Beam)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Socket_H)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Socket_L)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_DiversityWater)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_WaterPipe)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Heater)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_GasMeter)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_GasPipe)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_SCTHardware)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Walllayout)->SetVisible(true);
	}

	return TempNewLayer;
}
TWeakPtr<FArmyLayer> FArmyConstructionModeController::NewLayer(const FName& InLayerName,bool IsStaticLayer)
{
	TWeakPtr<FArmyLayer> TempNewLayer = LayerManager->GetLayer(InLayerName);
	if (TempNewLayer.IsValid()) return TempNewLayer;

	TempNewLayer = LayerManager->CreateLayer(InLayerName, IsStaticLayer);

	TSharedPtr<SArmyObjectTreeItem> LayerItem = MakeShareable(new SArmyObjectTreeItem);
	LayerItem->ModuleName = InLayerName;
	LayerItem->EditState = true;
	FPlatformMisc::CreateGuid(LayerItem->ItemID);
	LayerOutLiner->AddItemToTree(LayerItem);

	return TempNewLayer;
}
bool FArmyConstructionModeController::DeleteLayer(const FName& InLayerName)
{
	//目前只允许删除动态图层
	TWeakPtr<FArmyLayer> TheLayer = LayerManager->GetLayer(InLayerName);
	if (TheLayer.IsValid() && !TheLayer.Pin()->IsStaticLayer())
	{
		if (LayerManager->RemoveLayer(InLayerName))
		{
			for (auto ItemPtr : LayerOutLiner->RootTreeItems)
			{
				if (ItemPtr->ModuleName == InLayerName)
				{
					LayerOutLiner->RemoveItemFromTree(ItemPtr->ItemID);
					return true;
				}
			}
		}
	}
	return false;
}
void FArmyConstructionModeController::InitClassVisible(TWeakPtr<FArmyLayer> InLayer, ObjectClassType InClassType, bool bVisible)
{
	InLayer.Pin()->GetOrCreateClass(InClassType)->SetVisible(bVisible);
}
void FArmyConstructionModeController::OnObjectOperation(const XRArgument& InArg, FObjectPtr InObj, bool bTransaction/*=false*/)
{
	bool LayoutCurrentLayer = false;
	bool LayoutDynamicLayer = false;
	bool IgnoeLayerName = true;
	bool AutoData = false;

	TArray<FArmyLayer::FLayerType> InLayerTypeArray = {};

	if (InArg._ArgInt32 == 1)//add
	{
		bool IsHalfWall = false;
		if (InObj->GetType() == OT_AddWall)
		{
			IsHalfWall = InObj->AsassignObj<FArmyAddWall>()->GetIsHalfWall();
		}
		if ((InArg._ArgUint32 != E_ConstructionModel && 
			InObj->GetType() == OT_TextLabel && 
			InObj->AsassignObj<FArmyTextLabel>()->GetLabelType() == FArmyTextLabel::LT_SpaceName)||//其他模式的空间名称 不添加到施工图中，施工图会自动新生成一份数据。（涉及到移动和保存的操作）
			InObj->GetType() == OT_ReferenceImage ||//临摹图不需要在施工图显示
			(InArg._ArgUint32 != E_ModifyModel && ((InObj->GetType() == OT_AddWall && !IsHalfWall) || InObj->GetType() == OT_ModifyWall)))//拆改后的数据  不需要新增墙和拆改墙的数据(排除半墙)
		{
			return;
		}
		else if (InArg._ArgUint32 == E_HomeModel)//原始户型
		{
			InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
		}
		else if (InArg._ArgUint32 == E_ModifyModel)//拆改中的数据
		{
			FArmyHardware* HW = InObj->AsassignObj<FArmyHardware>();
			if (HW && HW->InWallType == 1)
			{
				InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
			}
			else
			{
				switch (InObj->GetType())
				{
				case OT_AddWall:
				case OT_IndependentWall:
				case OT_PackPipe:
				{
					InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
					break;
				}
				default:
					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
					break;
				}
			}
		}
		else if (InArg._ArgUint32 == E_LayoutModel)//拆改后
		{
			InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
		}
		else if (InArg._ArgUint32 == E_ConstructionModel)//图纸模式数据
		{
			LayoutCurrentLayer = InArg._ArgBoolean;
			LayoutDynamicLayer = InArg._ArgString.Contains(TEXT("DYNAMICLAYER"));

			IgnoeLayerName = InArg._ArgFName.IsNone() || !LayerManager->GetLayer(InArg._ArgFName).IsValid();//是否指定了图层
			AutoData = InArg._ArgString.Contains(TEXT("AUTO"));
			//添加到当前图层否则如果指定了对应图层的名称，则加入到对应的图层
			FName TheLayerName = LayoutCurrentLayer ? LayerManager->GetCurrentLayer().Pin()->GetName() : (IgnoeLayerName ? FName() : InArg._ArgFName);
			
			LocalDatas.Add(InObj, FLayerInfo(TheLayerName,InArg._ArgString, AutoData));

			if (AutoData)
			{
				LocalAutoObjectMap.FindOrAdd(InObj->GetType()).AddUnique(FAutoObjectInfo(InObj, InArg._ArgString, InArg._ArgFName));
			}

			if (InArg._ArgString.Contains(TEXT("ORIGIN")))
			{
				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
			}
			else if (InArg._ArgString.Contains(TEXT("LAYOUT")))
			{
				InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
			}
			else if (InArg._ArgString.Contains(TEXT("LAYERTYPE_CUPBOARDTABLE")))
			{
				InLayerTypeArray.AddUnique(FArmyLayer::LT_D_CUPBOARDTABLE);
			}
			else if (InArg._ArgString.Contains(TEXT("LAYERTYPE_WALLFACADE")))
			{
				InLayerTypeArray.AddUnique(FArmyLayer::LT_D_WALLFACADE);
			}
			else
			{
				InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
			}
		}
		else
		{
			InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
		}

		ObjectClassType ClassType = LocalFunction->GetClassTypeByObject(InObj);

		if (IsHalfWall && InArg._ArgUint32 == E_LayoutModel)
		{
			ClassType = Class_BaseWall;
		}

		if (LayoutCurrentLayer)
		{
			TSharedPtr<FArmyClass> NewClass = LayerManager->GetCurrentLayer().Pin()->GetOrCreateClass(ClassType);
			NewClass->AddObject(InObj);
		}
		else if (!IgnoeLayerName)
		{
			TSharedPtr<FArmyClass> NewClass = LayerManager->GetLayer(InArg._ArgFName).Pin()->GetOrCreateClass(ClassType);
			NewClass->AddObject(InObj);

			//每个图层的图框可能不一样，例如橱柜台面的图框
			if (InObj->GetType() == OT_ConstructionFrame)
			{
				LayerFrameMap.Add(InArg._ArgFName, StaticCastSharedPtr<FArmyConstructionFrame>(InObj));
			}
			if (InLayerTypeArray.Contains(FArmyLayer::LT_D_CUPBOARDTABLE))
			{
				LocalFunction->AddLayerFrameProperty(InArg._ArgFName, InObj);
			}
		}
		else
		{
			for (auto LayerIt : LayerManager->GetLayerMap())
			{
				TSharedPtr<FArmyLayer> TempLayer = LayerIt.Value;
				TSharedPtr<FArmyClass> AddWallClass = TempLayer->GetOrCreateClass(Class_AddWall, false);
				if (AddWallClass.IsValid() && AddWallClass->IsVisible() && ClassType == Class_BaseWall && IsHalfWall)
				{
					continue;//拆改后的新建半墙不需要在新建墙体图显示（否则重叠）
				}
				if (TempLayer.IsValid() && InLayerTypeArray.Contains(TempLayer->GetLayerType()) &&
					TempLayer->IsStaticLayer() == !LayoutDynamicLayer)
				{
					if (TempLayer->GetLayerType() == FArmyLayer::LT_D_CUPBOARDTABLE)
					{
						LocalFunction->AddLayerFrameProperty(LayerIt.Key,InObj);
					}
					TSharedPtr<FArmyClass> NewClass = TempLayer->GetOrCreateClass(ClassType, false);
					if (NewClass.IsValid())
					{
						NewClass->AddObject(InObj);

						//每个图层的图框可能不一样，例如橱柜台面的图框
						if (InObj->GetType() == OT_ConstructionFrame)
						{
							LayerFrameMap.Add(LayerIt.Key, StaticCastSharedPtr<FArmyConstructionFrame>(InObj));
						}
					}
				}
			}
		}

		if (InArg._ArgUint32 == E_ConstructionModel)
		{
			switch (InObj->GetType())
			{
			case OT_DownLeadLabel:
			case OT_InSideWallDimensions:
			case OT_OutSideWallDimensions:
			case OT_AddWallDimensions:
			case OT_DeleteWallDimensions:
			case OT_Dimensions:
				break;
			case OT_TextLabel:
			{
				AllCanHoverObjects.Add(FArmyRectSelect::ObjectInfo(InObj, false));
				break;
			}
			default:
				break;
			}
		}

		/////////////////////////////////////////////////////////
//		if (InArg._ArgUint32 == E_HomeModel)//原始户型
//		{
//			InLayerTypeArray.Empty();
//			InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
//			switch (InObj->GetType())
//			{
//			case OT_SecurityDoor:
//			{
//				//防盗门需要在拆除图中显示
//				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
//				break;
//			}
//			case OT_TextLabel:
//			{
//				//房间命名标签
//				FArmyTextLabel* TextLabel = InObj->AsassignObj<FArmyTextLabel>();
//				if (TextLabel->GetLabelType() == FArmyTextLabel::LT_SpaceName)
//				{
//					return;
//				}
//				break;
//			}
//			default:
//				break;
//			}
//		}
//		else if (InArg._ArgUint32 == E_ModifyModel)//拆改中的数据
//		{
//			InLayerTypeArray.Empty();
//			InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
//			switch (InObj->GetType())
//			{
//			case OT_AddWall:
//			case OT_IndependentWall:
//			case OT_PackPipe:
//			{
//				InLayerTypeArray.Empty();
//				InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
//				break;
//			}
//			case OT_TextLabel:
//			{
//				//房间命名标签
//				FArmyTextLabel* TextLabel = InObj->AsassignObj<FArmyTextLabel>();
//				if (TextLabel->GetLabelType() == FArmyTextLabel::LT_SpaceName)
//				{
//					//拆改中的房间名不需要显示在拆除图中
//					return;
//				}
//				break;
//			}
//			default:
//				break;
//			}
//		}
//		else if (InArg._ArgUint32 == E_LayoutModel)//拆改后
//		{
//			InLayerTypeArray.Empty();
//			InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
//			switch (InObj->GetType())
//			{
//			case OT_TextLabel:
//			{
//				//房间命名标签
//				FArmyTextLabel* TextLabel = InObj->AsassignObj<FArmyTextLabel>();
//				if (TextLabel->GetLabelType() == FArmyTextLabel::LT_SpaceName)
//				{
//					//拆改后的房间名不需要显示在拆除图中
//					return;
//				}
//				break;
//			}
//// 			case OT_AddWall:
//// 			case OT_IndependentWall:
//// 			case OT_PackPipe:
//// 			{
//// 				return;
//// 				break;
//// 			}
//			default:
//				break;
//			}
//		}
//		else if (InArg._ArgUint32 == E_ConstructionModel)//图纸模式数据
//		{
//			if (InArg._ArgString.Contains(TEXT("AUTO")))//目前自动生成的元素看做是所有图层都存在的
//			{
//				InLayerTypeArray.Empty();
//				LocalAutoObjectMap.FindOrAdd(InObj->GetType()).AddUnique(FAutoObjectInfo(InObj, InArg._ArgString, InArg._ArgFName));
//
//				if (InArg._ArgString.Contains(TEXT("ORIGIN")))
//				{
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
//				}
//				else if (InArg._ArgString.Contains(TEXT("LAYOUT")))
//				{
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
//				}
//				else if (InArg._ArgString.Contains(TEXT("LAYERTYPE_CUPBOARDTABLE")))
//				{
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_D_CUPBOARDTABLE);
//				}
//				else
//				{
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
//				}
//			}
//			
//			LayoutCurrentLayer = InArg._ArgBoolean;
//			LayoutDynamicLayer = InArg._ArgString.Contains(TEXT("DYNAMICLAYER"));
//
//			switch (InObj->GetType())
//			{
//			case OT_ReferenceImage:
//			{
//				return;
//			}
//			case OT_ConstructionFrame://图框
//			{
//				//CurrentFrame = StaticCastSharedPtr<FArmyConstructionFrame>(InObj);
//				if (InArg._ArgString.Contains(TEXT("LAYERTYPE_CUPBOARDTABLE")))
//				{
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_D_CUPBOARDTABLE);
//				}
//				else
//				{
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
//					InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
//				}
//				break;
//			}
//			case OT_Compass://指北针在各个图层上显示
//			{
//				InLayerTypeArray.AddUnique(FArmyLayer::LT_NONE);
//				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_ORIGINAL);
//				InLayerTypeArray.AddUnique(FArmyLayer::LT_S_MODIFY);
//				break;
//			}
//			default:
//				break;
//			}
//		}
//		else if (InArg._ArgUint32 != E_ModifyModel && (InObj->GetType() == OT_AddWall || InObj->GetType() == OT_ModifyWall))//拆改后的数据  不需要新增墙和拆改墙的数据
//		{
//			return;
//		}
//
//		ObjectClassType ClassType = LocalFunction->GetClassTypeByObject(InObj);
//		if (InArg._ArgUint32 == E_ConstructionModel)
//		{
//			LocalDatas.Add(InObj, LayoutCurrentLayer ? LayerManager->GetCurrentLayer().Pin()->GetName() : FName());
//		}
//		if (LayoutCurrentLayer)
//		{
//			TSharedPtr<FArmyClass> NewClass = LayerManager->GetCurrentLayer().Pin()->GetOrCreateClass(ClassType);
//			NewClass->AddObject(InObj);
//		}
//		else
//		{
//			bool IgnoeLayerName = InArg._ArgFName.IsNone() || !LayerManager->GetLayer(InArg._ArgFName).IsValid();
//			for (auto LayerIt : LayerManager->GetLayerMap())
//			{
//				TSharedPtr<FArmyLayer> TempLayer = LayerIt.Value;
//				if (TempLayer.IsValid() && InLayerTypeArray.Contains(TempLayer->GetLayerType()) &&
//					TempLayer->IsStaticLayer() == !LayoutDynamicLayer &&
//					(IgnoeLayerName || TempLayer->GetName() == InArg._ArgFName))
//				{
//					TSharedPtr<FArmyClass> NewClass = TempLayer->GetOrCreateClass(ClassType, false);
//					if (NewClass.IsValid())
//					{
//						NewClass->AddObject(InObj);
//
//						//每个图层的图框可能不一样，例如橱柜台面的图框
//						if (InObj->GetType() == OT_ConstructionFrame)
//						{
//							LayerFrameMap.Add(LayerIt.Key, StaticCastSharedPtr<FArmyConstructionFrame>(InObj));
//						}
//					}
//				}
//			}
//		}
//		if (InArg._ArgUint32 == E_ConstructionModel)
//		{
//			switch (InObj->GetType())
//			{
//			case OT_DownLeadLabel:
//			case OT_InSideWallDimensions:
//			case OT_OutSideWallDimensions:
//			case OT_AddWallDimensions:
//			case OT_DeleteWallDimensions:
//			case OT_Dimensions:
//				break;
//			case OT_TextLabel:
//			{
//				AllCanHoverObjects.Add(FArmyRectSelect::ObjectInfo(InObj, false));
//				break;
//			}
//			default:
//				break;
//			}
//		}
	}
	else if (InArg._ArgInt32 == 0)//remove
	{
		ObjectClassType ClassType = LocalFunction->GetClassTypeByObject(InObj);
		LocalDatas.Remove(InObj);
		if (LayoutCurrentLayer)
		{
			TSharedPtr<FArmyClass> NewClass = LayerManager->GetCurrentLayer().Pin()->GetOrCreateClass(ClassType);
			NewClass->RemoveObject(InObj);
		}
		else
		{
			for (auto LayerIt : LayerManager->GetLayerMap())
			{
				TSharedPtr<FArmyLayer> TempLayer = LayerIt.Value;
				if (TempLayer.IsValid())
				{
					TSharedPtr<FArmyClass> NewClass = TempLayer->GetOrCreateClass(ClassType, false);
					if (NewClass.IsValid())
					{
						NewClass->RemoveObject(InObj);
					}
				}
			}
		}
		switch (InObj->GetType())
		{
		case OT_DownLeadLabel:
		case OT_InSideWallDimensions:
		case OT_OutSideWallDimensions:
		case OT_AddWallDimensions:
		case OT_DeleteWallDimensions:
		case OT_Dimensions:
			break;
		case OT_TextLabel:
		{
			AllCanHoverObjects.Remove(FArmyRectSelect::ObjectInfo(InObj, false));

			//if (DemensionObjList.Contains(InObj))
			//{
			//	DemensionObjList.Remove(InObj);
			//}
			break;
		}
		default:
			break;
		}
	}
}

TSharedPtr<SWidget> FArmyConstructionModeController::MakeRightPanelWidget()
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
                    .Text(FText::FromString(TEXT("施工图管理")))
		            .TextStyle(FArmyStyle::Get(), "ArmyText_12")
		        ]
		    ]
		]

	    + SVerticalBox::Slot()
		.Padding(FMargin(0.0f, 8.0f, 0.0f, 0.0f))
		.VAlign(VAlign_Fill)
		[
			CreateLayerOutliner()
		];
}

TSharedPtr<SWidget> FArmyConstructionModeController::MakeToolBarWidget()
{
#define LOCTEXT_NAMESPACE "ToolBarWidget"
	TSharedPtr<FArmyToolBarBuilder> ToolBarBuidler = MakeShareable(new FArmyToolBarBuilder);

	FCanExecuteAction CheckHasSelectedXRObj = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckHasSelectedXRObj);

	//@梁晓菲
	FCanExecuteAction CheckCanUndo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanUndo);
	FCanExecuteAction CheckCanRedo = FCanExecuteAction::CreateStatic(&FArmyFrameCommands::CheckCanRedo);

	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandSave, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnSaveCommand));
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandDelete, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnDeleteCommand), CheckHasSelectedXRObj);
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandFacadeIndex, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnGenFacadeIndex));
	//ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandUndo, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnUndoCommand), CheckCanUndo);//@梁晓菲
	//ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandRedo, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnRedoCommand), CheckCanRedo);//@梁晓菲
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandLoadFrame, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnLoadFrameCommand));
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandSaveImage, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnSaveImageCommand));
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandExportCAD, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnExportCADDialog));
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandReset, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnResetDismensionCommand));
	ToolBarBuidler->AddButton(FArmyConstructionCommands::Get().CommandUpdate, FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnUpdateDismensionCommand));

	//ToolBarBuidler->AddComboButton(FOnGetContent::CreateRaw(this, &FArmyConstructionModeController::OnAutoRulerSelected), LOCTEXT("annotation", "尺寸线"), FSlateIcon("ArmyStyle", "ConstructionMode.OnAutoRulerCommand"));
#undef LOCTEXT_NAMESPACE
	return ToolBarBuidler->CreateToolBar(&FArmyStyle::Get(), "ToolBar");
}

TSharedPtr<SWidget> FArmyConstructionModeController::MakeSettingBarWidget()
{
	TextTypes.AddUnique(MakeShared<FString>(TEXT("标尺字号")));
	TextTypes.AddUnique(MakeShared<FString>(TEXT("标注字号")));
	CurrentTextStyle = TextTypes[0];
	for (int32 i = 6;i < 54;i += 2)
	{
		TextSizeList.AddUnique(MakeShared<int32>(i));
	}
	GVC->ViewportOverlayWidget->AddSlot().HAlign(HAlign_Left).VAlign(VAlign_Bottom).Padding(2, 0)
		[
			SAssignNew(FontSizeSetWidget, SBox)
			.WidthOverride(210)
		.HeightOverride(136)
		.Visibility_Raw(this, &FArmyConstructionModeController::VisibleFontSize)
		[
			SNew(SBorder)
			.Padding(FMargin(0))
		.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Top)
		.Padding(FMargin(16, 16, 16, 0))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		.Padding(0)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("类型")))
		]
	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Right)
		.Padding(0)
		[
			SNew(SBox)
			.WidthOverride(114)
		.HeightOverride(24)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.ContentPadding(FMargin(10, 5, 0, 0))
		.ForegroundColor(FLinearColor::White)
		.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
		.HasDownArrow(true)
		.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Modechange"))
		.OptionsSource(&TextTypes)
		.OnGenerateWidget_Raw(this, &FArmyConstructionModeController::GenerateTextTypeComboItem)
		.OnSelectionChanged_Raw(this, &FArmyConstructionModeController::HandleTextTypeComboChanged)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
		.Text_Raw(this, &FArmyConstructionModeController::GetTextTypeComboText)
		]
		]
		]
		]
	+ SVerticalBox::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Fill)
		.Padding(16, 8, 16, 0)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		.Padding(0)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("字体大小")))
		]
	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Right)
		.Padding(0)
		[
			SNew(SBox)
			.WidthOverride(114)
		.HeightOverride(24)
		[
			//SNew(STextBlock)
			SNew(SComboBox<TSharedPtr<int32>>)
			.ContentPadding(FMargin(10, 5, 0, 0))
		.ComboBoxStyle(&FArmyStyle::Get().GetWidgetStyle<FComboBoxStyle>("ComboBox.Black"))
		.ItemStyle(&FArmyStyle::Get().GetWidgetStyle<FTableRowStyle>("TableRow.Modechange"))
		.HasDownArrow(true)
		.MaxListHeight(100)
		//.CustomScrollbar(SNew(SScrollBar))
		.ForegroundColor(FLinearColor::White)
		.OptionsSource(&TextSizeList)
		.OnGenerateWidget_Raw(this, &FArmyConstructionModeController::GenerateFontSizeComboItem)
		.OnSelectionChanged_Raw(this, &FArmyConstructionModeController::HandleFontSizeComboChanged)
		[
			SNew(STextBlock)
			.ColorAndOpacity(FLinearColor::White)
		.Text_Raw(this, &FArmyConstructionModeController::GetFontSizeComboText)
		]
		]
		]
		]
	+ SVerticalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(0, 16, 16, 16)
		[
			SNew(SBox)
			.HeightOverride(30)
		.WidthOverride(80)
		[
			SNew(SButton)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.Gray.FF353638"))
		.OnClicked(FOnClicked::CreateRaw(this, &FArmyConstructionModeController::OnFontSizeRevert))
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("恢复默认")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
		.HighlightColor(FLinearColor(FColor(0XFFFFFFFF)))
		]
		]
		]
		]
		]
		];

	return
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Bottom)
		[
			SNew(SBox)
			.HeightOverride(40)
		[
			SNew(SBorder)
			.Padding(FMargin(0))
		.HAlign(HAlign_Left)
		.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
		.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.ContentPadding(FMargin(16, 0, 0, 0))
		.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("Button.None"))
		.OnClicked(FOnClicked::CreateRaw(this, &FArmyConstructionModeController::OnFontSizeSet))
		.OnHovered_Raw(this, &FArmyConstructionModeController::OnSetFontHoverd)
		.OnUnhovered_Raw(this, &FArmyConstructionModeController::OnSetFontUnHoverd)
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Content()
		[
			SAssignNew(SetFontTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("设置字体")))
		.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
		.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"))
		.HighlightColor(FLinearColor(FColor(0XFFFFFFFF)))
		]
		]
		]
		]
		];
}

TSharedRef<SWidget> FArmyConstructionModeController::GenerateTextTypeComboItem(TSharedPtr<FString> InItem)
{
	return 	SNew(STextBlock).Text(FText::FromString(*InItem.Get())).ColorAndOpacity(FLinearColor::White);
}
void FArmyConstructionModeController::HandleTextTypeComboChanged(TSharedPtr<FString> NewSelection, ESelectInfo::Type SelectInfo)
{
	CurrentTextStyle = NewSelection;
}
FText FArmyConstructionModeController::GetTextTypeComboText() const
{
	return FText::FromString(*CurrentTextStyle.Get());
}

TSharedRef<SWidget> FArmyConstructionModeController::GenerateFontSizeComboItem(TSharedPtr<int32> InItem)
{
	return 	SNew(STextBlock).Text(FText::AsNumber(*InItem.Get())).ColorAndOpacity(FLinearColor::White);
}
void FArmyConstructionModeController::HandleFontSizeComboChanged(TSharedPtr<int32> NewSelection, ESelectInfo::Type SelectInfo)
{
	if (CurrentTextStyle == TextTypes[0])
	{
		if (GXRSettings->GetSizeDimensionsFontSize() != *NewSelection.Get())
		{
			GXRSettings->SetSizeDimensionsFontSize(*NewSelection.Get());
			GXRSettings->SaveSettings();
		}
	}
	else
	{
		if (GXRSettings->GetAnnotationFontSize() != *NewSelection.Get())
		{
			GXRSettings->SetAnnotationFontSize(*NewSelection.Get());
			GXRSettings->SaveSettings();
		}
	}
}
FText FArmyConstructionModeController::GetFontSizeComboText() const
{
	if (CurrentTextStyle == TextTypes[0])
	{
		return FText::AsNumber(GXRSettings->GetSizeDimensionsFontSize());
	}
	else
	{
		return FText::AsNumber(GXRSettings->GetAnnotationFontSize());
	}
}
EVisibility FArmyConstructionModeController::VisibleFontSize() const
{
	return FontSizeVisible;
}
FReply FArmyConstructionModeController::OnFontSizeSet()
{
	FontSizeVisible = FontSizeVisible == EVisibility::Collapsed ? EVisibility::Visible : EVisibility::Collapsed;
	if (FontSizeVisible == EVisibility::Visible)
	{
		SetFontTextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFF9800"));
	}
	else
	{
		SetFontTextBlock->SetColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFB5B7BE"));
	}
	return FReply::Handled();
}
FReply FArmyConstructionModeController::OnFontSizeRevert()
{
	if (CurrentTextStyle == TextTypes[0])
	{
		GXRSettings->SetSizeDimensionsFontSize(12);
		GXRSettings->SaveSettings();
	}
	else
	{
		GXRSettings->SetAnnotationFontSize(12);
		GXRSettings->SaveSettings();
	}
	return FReply::Handled();
}
TSharedRef<SWidget> FArmyConstructionModeController::OnAutoRulerSelected()
{
	const TSharedRef<FExtender> MenuExtender = MakeShareable(new FExtender());
	TSharedRef< FUICommandList > ActionList = MakeShareable(new FUICommandList);
	FMenuBuilder MenuBuilder(true, ActionList, MenuExtender);
	MenuBuilder.SetStyle(&FArmyStyle::Get(), "Menu");

	MenuBuilder.AddMenuEntry(FArmyConstructionCommands::Get().CommandAutoRulerUp);
	MenuBuilder.AddMenuEntry(FArmyConstructionCommands::Get().CommandAutoRulerDown);
	MenuBuilder.AddMenuEntry(FArmyConstructionCommands::Get().CommandAutoRulerLeft);
	MenuBuilder.AddMenuEntry(FArmyConstructionCommands::Get().CommandAutoRulerRight);

	ActionList->MapAction(
		FArmyConstructionCommands::Get().CommandAutoRulerUp,
		FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnAutoRulerCommand, 1),
		FCanExecuteAction(),
		FIsActionChecked::CreateRaw(this, &FArmyConstructionModeController::IsAutoRulerChecked, 1)
	);
	ActionList->MapAction(
		FArmyConstructionCommands::Get().CommandAutoRulerDown,
		FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnAutoRulerCommand, 2),
		FCanExecuteAction(),
		FIsActionChecked::CreateRaw(this, &FArmyConstructionModeController::IsAutoRulerChecked, 2)
	);
	ActionList->MapAction(
		FArmyConstructionCommands::Get().CommandAutoRulerLeft,
		FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnAutoRulerCommand, 3),
		FCanExecuteAction(),
		FIsActionChecked::CreateRaw(this, &FArmyConstructionModeController::IsAutoRulerChecked, 3)
	);
	ActionList->MapAction(
		FArmyConstructionCommands::Get().CommandAutoRulerRight,
		FExecuteAction::CreateRaw(this, &FArmyConstructionModeController::OnAutoRulerCommand, 4),
		FCanExecuteAction(),
		FIsActionChecked::CreateRaw(this, &FArmyConstructionModeController::IsAutoRulerChecked, 4)
	);

	return MenuBuilder.MakeWidget();
}
void FArmyConstructionModeController::OnAutoRulerCommand(int32 InType)
{
	const TArray<FObjectWeakPtr> ObjArray = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects();
	if (ObjArray.Num() == 1)
	{
		TArray<FAutoObjectInfo>* RulerArray = LocalAutoObjectMap.Find(OT_ObjectRuler);
		if (RulerArray)
		{
			for (auto ObjRuler : *RulerArray)
			{
				FArmyObjAutoRuler* Ruler = ObjRuler.ObjPtr.Pin()->AsassignObj<FArmyObjAutoRuler>();
				if (Ruler)
				{
					if (Ruler->GetBoundObj() == ObjArray[0].Pin())
					{
						switch (InType)
						{
						case 1:
							Ruler->SetLineVisible(FArmyObjAutoRuler::YBACK, !Ruler->IsLineVisible(FArmyObjAutoRuler::YBACK));
							break;
						case 2:
							Ruler->SetLineVisible(FArmyObjAutoRuler::YFRONT, !Ruler->IsLineVisible(FArmyObjAutoRuler::YFRONT));
							break;
						case 3:
							Ruler->SetLineVisible(FArmyObjAutoRuler::XBACK, !Ruler->IsLineVisible(FArmyObjAutoRuler::XBACK));
							break;
						case 4:
							Ruler->SetLineVisible(FArmyObjAutoRuler::XFRONT, !Ruler->IsLineVisible(FArmyObjAutoRuler::XFRONT));
							break;
						case 6:
							Ruler->SetLineVisible(FArmyObjAutoRuler::WIDTH, !Ruler->IsLineVisible(FArmyObjAutoRuler::WIDTH));
							break;
						case 7:
							Ruler->SetLineVisible(FArmyObjAutoRuler::LENGTH, !Ruler->IsLineVisible(FArmyObjAutoRuler::LENGTH));
							break;
						default:
							break;
						}
						break;
					}
				}
			}
		}
	}
}
bool FArmyConstructionModeController::IsAutoRulerChecked(int32 InType)
{
	const TArray<FObjectWeakPtr> ObjArray = FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects();
	if (ObjArray.Num() == 1)
	{
		TArray<FAutoObjectInfo>* RulerArray = LocalAutoObjectMap.Find(OT_ObjectRuler);
		if (RulerArray)
		{
			for (auto ObjRuler : *RulerArray)
			{
				FArmyObjAutoRuler* Ruler = ObjRuler.ObjPtr.Pin()->AsassignObj<FArmyObjAutoRuler>();
				if (Ruler)
				{
					if (Ruler->GetBoundObj() == ObjArray[0].Pin())
					{
						switch (InType)
						{
						case 1:
							return Ruler->IsLineVisible(FArmyObjAutoRuler::YBACK);
						case 2:
							return Ruler->IsLineVisible(FArmyObjAutoRuler::YFRONT);
						case 3:
							return Ruler->IsLineVisible(FArmyObjAutoRuler::XBACK);
						case 4:
							return Ruler->IsLineVisible(FArmyObjAutoRuler::XFRONT);
						case 6:
							return Ruler->IsLineVisible(FArmyObjAutoRuler::WIDTH);
						case 7:
							return Ruler->IsLineVisible(FArmyObjAutoRuler::LENGTH);
						default:
							break;
						}
						break;
					}
				}
			}
		}
	}
	return false;
}
void FArmyConstructionModeController::InitConstructionFrame(TSharedPtr<FArmyConstructionFrame> InConstructionFrame,int32 InFrameType)
{
	if (InConstructionFrame.IsValid() && InFrameType == 1)//橱柜台面图框
	{
		float FontSize = 10;
		int32 CharNum = 2;

		FBox UserDefineBox = InConstructionFrame->GetAreaBox(FRAME_AREA_USERDEFINE);
		FVector BoxSize = UserDefineBox.GetSize() / 2;
		float YOffset = BoxSize.Y / 3;
		float XOffset = BoxSize.X / 7;
		float StartX = UserDefineBox.Min.X + 10 * XOffset;
		float StartY = UserDefineBox.Min.Y + 3 * YOffset;
		InConstructionFrame->SetEditorModel(true);
		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX, StartY, 0), FVector(UserDefineBox.Max.X, StartY, 0))));
		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX, StartY + YOffset, 0), FVector(UserDefineBox.Max.X, StartY + YOffset, 0))));
		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX, StartY + YOffset * 2, 0), FVector(UserDefineBox.Max.X, StartY + YOffset * 2, 0))));

		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX, StartY, 0), FVector(StartX, UserDefineBox.Max.Y, 0))));
		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX + XOffset, StartY, 0), FVector(StartX + XOffset, UserDefineBox.Max.Y, 0))));
		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX + XOffset * 2, StartY, 0), FVector(StartX + XOffset * 2, UserDefineBox.Max.Y, 0))));
		InConstructionFrame->AddFrameLine(MakeShareable(new FArmyLine(FVector(StartX + XOffset * 3, StartY, 0), FVector(StartX + XOffset * 3, UserDefineBox.Max.Y, 0))));

		CharNum = 2;
		TSharedPtr<FArmyRect> Rect1 = MakeShareable(new FArmyRect(FVector(StartX + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect1->XDirection = FVector(1, 0, 0);
		Rect1->YDirection = FVector(0, 1, 0);

		CharNum = 2;
		TSharedPtr<FArmyRect> Rect2 = MakeShareable(new FArmyRect(FVector(StartX + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect2->XDirection = FVector(1, 0, 0);
		Rect2->YDirection = FVector(0, 1, 0);

		CharNum = 2;
		TSharedPtr<FArmyRect> Rect3 = MakeShareable(new FArmyRect(FVector(StartX + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset * 2 + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect3->XDirection = FVector(1, 0, 0);
		Rect3->YDirection = FVector(0, 1, 0);

		CharNum = 4;
		TSharedPtr<FArmyRect> Rect4 = MakeShareable(new FArmyRect(FVector(StartX + XOffset + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect4->XDirection = FVector(1, 0, 0);
		Rect4->YDirection = FVector(0, 1, 0);

		CharNum = 8;
		TSharedPtr<FArmyRect> Rect5 = MakeShareable(new FArmyRect(FVector(StartX + XOffset + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect5->XDirection = FVector(1, 0, 0);
		Rect5->YDirection = FVector(0, 1, 0);

		CharNum = 4;
		TSharedPtr<FArmyRect> Rect6 = MakeShareable(new FArmyRect(FVector(StartX + XOffset + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset * 2 + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect6->XDirection = FVector(1, 0, 0);
		Rect6->YDirection = FVector(0, 1, 0);

		CharNum = 4;
		TSharedPtr<FArmyRect> Rect7 = MakeShareable(new FArmyRect(FVector(StartX + XOffset * 2 + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect7->XDirection = FVector(1, 0, 0);
		Rect7->YDirection = FVector(0, 1, 0);

		CharNum = 2;
		TSharedPtr<FArmyRect> Rect8 = MakeShareable(new FArmyRect(FVector(StartX + XOffset * 2 + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect8->XDirection = FVector(1, 0, 0);
		Rect8->YDirection = FVector(0, 1, 0);

		CharNum = 2;
		TSharedPtr<FArmyRect> Rect9 = MakeShareable(new FArmyRect(FVector(StartX + XOffset * 2 + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset * 2 + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect9->XDirection = FVector(1, 0, 0);
		Rect9->YDirection = FVector(0, 1, 0);

		CharNum = 8;
		TSharedPtr<FArmyRect> Rect10 = MakeShareable(new FArmyRect(FVector(StartX + XOffset * 3 + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect10->XDirection = FVector(1, 0, 0);
		Rect10->YDirection = FVector(0, 1, 0);

		CharNum = 8;
		TSharedPtr<FArmyRect> Rect11 = MakeShareable(new FArmyRect(FVector(StartX + XOffset * 3 + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect11->XDirection = FVector(1, 0, 0);
		Rect11->YDirection = FVector(0, 1, 0);

		CharNum = 4;
		TSharedPtr<FArmyRect> Rect12 = MakeShareable(new FArmyRect(FVector(StartX + XOffset * 3 + XOffset / 2 - FontSize * (CharNum / 2), StartY + YOffset * 2 + YOffset / 2 - FontSize / 2, 0), XOffset, YOffset));
		Rect12->XDirection = FVector(1, 0, 0);
		Rect12->YDirection = FVector(0, 1, 0);

		InConstructionFrame->AddFillArea(1001, Rect1, FontSize);
		InConstructionFrame->AddFillArea(1002, Rect2, FontSize);
		InConstructionFrame->AddFillArea(1003, Rect3, FontSize);
		
		InConstructionFrame->AddFillArea(1004, Rect4, FontSize);
		InConstructionFrame->AddFillArea(1005, Rect5, FontSize);
		InConstructionFrame->AddFillArea(1006, Rect6, FontSize);
		
		InConstructionFrame->AddFillArea(1007, Rect7, FontSize);
		InConstructionFrame->AddFillArea(1008, Rect8, FontSize);
		InConstructionFrame->AddFillArea(1009, Rect9, FontSize);
		
		InConstructionFrame->AddFillArea(1010, Rect10, FontSize);
		InConstructionFrame->AddFillArea(1011, Rect11, FontSize);
		InConstructionFrame->AddFillArea(1012, Rect12, FontSize);
		
		
		InConstructionFrame->FillAreaText(1001, TEXT("套餐"));
		InConstructionFrame->FillAreaText(1002, TEXT("前沿"));
		InConstructionFrame->FillAreaText(1003, TEXT("灶具"));
		
		InConstructionFrame->FillAreaText(1004, TEXT("套餐/风格"));
		InConstructionFrame->FillAreaText(1005, TEXT("前沿名称-高度"));
		InConstructionFrame->FillAreaText(1006, TEXT("灶具型号"));
		
		InConstructionFrame->FillAreaText(1007, TEXT("台面材质"));
		InConstructionFrame->FillAreaText(1008, TEXT("后档"));
		InConstructionFrame->FillAreaText(1009, TEXT("水槽"));
		
		InConstructionFrame->FillAreaText(1010, TEXT("台面名称-厚度"));
		InConstructionFrame->FillAreaText(1011, TEXT("后档名称-高度"));
		InConstructionFrame->FillAreaText(1012, TEXT("水槽型号"));
		
		InConstructionFrame->SetEditorModel(false);
	}
}
void FArmyConstructionModeController::OnLoadFrameLogo(const FString& InFilePath)
{
	TArray<TSharedPtr<FArmyFurniture>> CADLayers = FArmyToolsModule::Get().ParseDXF(InFilePath);
	if (CADLayers.Num() > 0)
	{
		if (FrameLogo != CADLayers[0] && FrameLogoFilePath != InFilePath)
		{
			FrameLogo = CADLayers[0];
			FrameLogoFilePath = InFilePath;
		}
	}
}
void FArmyConstructionModeController::LoadConstructionFrame(const FString& InFilePath)
{
	bool ExistStaticLayerFrame = false,ExsitCupboardTableLayerFrame = false;
	for (auto& FrameObj : LayerFrameMap)
	{
		TWeakPtr<FArmyLayer> Layer = LayerManager->GetLayer(FrameObj.Key);
		if (Layer.IsValid())
		{
			if (!ExistStaticLayerFrame)
			{
				ExistStaticLayerFrame = Layer.Pin()->IsStaticLayer();
			}
			for (auto& CupboardTableLayerInfo : LocalFunction->CupboardTableLayerArray)
			{
				if (CupboardTableLayerInfo.LayerName == FrameObj.Key)
				{
					ExsitCupboardTableLayerFrame = true;
				}
			}
		}
		if (ExistStaticLayerFrame && 
			ExsitCupboardTableLayerFrame)
		{
			return;
		}
	}

	FString Content;
	FFileHelper::LoadFileToString(Content, *InFilePath);

	TSharedPtr<FJsonObject> Data;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Content), Data))
	{
		const TArray< TSharedPtr<FJsonValue> >& JsonArray = Data->GetArrayField("FrameList");
		for (auto JsonV : JsonArray)
		{
			const TSharedPtr<FJsonObject> JsonObj = JsonV->AsObject();
			if (JsonObj.IsValid())
			{
				FBox TheMaxBox(ForceInitToZero);// = FArmySceneData::Get()->GetBounds();

				TSharedPtr<FArmyConstructionFrame> LoadFrame = MakeShareable(new FArmyConstructionFrame);

				LoadFrame->Deserialization(JsonObj);
				if (!ExistStaticLayerFrame)
				{
					CurrentFrame = LoadFrame.ToSharedRef();
					FArmySceneData::Get()->Add(LoadFrame, XRArgument(1).ArgUint32(E_ConstructionModel).ArgString(TEXT("LAYERTYPE_NONE")));
				}

				float XScale = 1;
				float YScale = 1;
				FBox MainArea = LoadFrame->GetAreaBox(FRAME_AREA_Main);
				if (MainArea.IsValid && TheMaxBox.IsValid)
				{
					XScale = (TheMaxBox.GetSize().X + 10) / MainArea.GetSize().X;
					YScale = (TheMaxBox.GetSize().Y + 10) / MainArea.GetSize().Y;
				}
				float Scale = FMath::Max(XScale, YScale);

				FVector MaxBoxCenter = TheMaxBox.GetCenter();
				FVector MainAreaCenter = MainArea.GetCenter();
				MaxBoxCenter.Z = 0;
				MainAreaCenter.Z = 0;
				FVector OffSet = MaxBoxCenter - MainAreaCenter;
				FTransform Transform;
				Transform.SetLocation(OffSet);
				Transform.SetScale3D(FVector(Scale, Scale, 1));
				LoadFrame->ApplyTransform(Transform);

				if (!ExsitCupboardTableLayerFrame)
				{
					for (auto& DLayerInfo : LocalFunction->CupboardTableLayerArray)//橱柜台面图框
					{
						TSharedPtr<FArmyConstructionFrame> TempFrame = MakeShareable(new FArmyConstructionFrame);

						TempFrame->Deserialization(JsonObj);
						TempFrame->SetFrameType(E_Frame_Cupboard);
						InitConstructionFrame(TempFrame, 1);
						FArmySceneData::Get()->Add(TempFrame, XRArgument(1).ArgUint32(E_ConstructionModel).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_CUPBOARDTABLE")));
					}
				}

				LocalModelService->RequestFrameFillInfo();

				SCOPE_TRANSACTION(TEXT("生成新图框"));//梁晓菲 撤销重做
				LoadFrame->Create();//梁晓菲 撤销重做
				break;
			}
		}
	}
}
void FArmyConstructionModeController::FillConstructionFrame()
{
	if (CurrentFrame.Pin().IsValid())
	{
		if (FrameLogo.IsValid())
		{
			CurrentFrame.Pin()->FillAreaDxf(FRAME_AREA_COMPANYNAME, FrameLogo);
		}
		else
		{
			FString FilePath = FPaths::ProjectContentDir() + "Assets/" + "loveSpaceLogo.dxf";
			TArray<TSharedPtr<FArmyFurniture>> CADLayers = FArmyToolsModule::Get().ParseDXF(FilePath);
			if (CADLayers.Num() > 0)
			{
				FrameLogo = CADLayers[0];
				CurrentFrame.Pin()->FillAreaDxf(FRAME_AREA_COMPANYNAME, FrameLogo);
			}
		}

		//FString Declare = TEXT("ALL DESIGN INTHIS DRAWNG ARE THE PROPERTY\nOF OUR COMPANY AND CANNOT BE USED\nWITHTHOUT THEIR WRITEN PERMISSION.") + FString("\n") +
		//	TEXT("此设计图纸之知识产权归本公司所有；") + FString("\n") +
		//	TEXT("任何复印或使用，均需获得书面批准。") + FString("\n") +
		//	TEXT("ALLMEASUREMENTS MUST BE CHECKD AND VARIF\nON SITE.") + FString("\n") +
		//	TEXT("所有于图纸上标示的尺寸，均需现场复核和修正。") + FString("\n") +
		//	TEXT("THE DRAWING IS NOT VALID FOR CONSTRUCTION\nUNLESS") + FString("\n") +
		//	TEXT("除有批核外，此图不可作为施工用。");

		FString Declare = TEXT("1.由于出图等原因，实际尺寸会与标注尺寸有差别，施工前应对所有的尺寸进行验证；") + FString("\n") +
			TEXT("2.当发现图纸中有错误、遗漏或不当之处时，需甲方与客户经理共同商讨解决，出具变更方可施工；") + FString("\n") +
			TEXT("3.所有用材严格按照工程报价单规定的材料，具体标准参照公司的施工规定；") + FString("\n") +
			TEXT("4.非本公司客户经理书面批准，不得随意将任何部分翻印，图纸版权归公司所有。");


		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_COMPANYNAME, TEXT(""));
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_DESCRIBE, TEXT(""));
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_DECLARATION, Declare);
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_CONTRACTNO, FArmyUser::Get().CurrentAppendInfo.ContractNo);
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_CUSTOMERNAME, FArmyUser::Get().CurrentAppendInfo.CustomerName);
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_PROJECTIONADDRESS, FArmyUser::Get().CurrentAppendInfo.HomeAddress);
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_CONSTRUCTIONNAME, LayerManager->GetCurrentLayer().Pin()->GetName().ToString());
		CurrentFrame.Pin()->FillAreaText(FRAME_AREA_DESIGNERNAME, FArmyUser::Get().CurrentAppendInfo.DesignerName);
		//CurrentFrame.Pin()->FillAreaText(FRAME_AREA_DRAWINGNO, TEXT("PM-05"));

		int32 Index = LocalFunction->CupboardTableLayerArray.Find(FArmyConstructionFunction::FConstructionPropertyInfo(LayerManager->GetCurrentLayer().Pin()->GetName()));
		if (Index != INDEX_NONE)//橱柜台面图框
		{
			//
			for (auto& V : LocalFunction->CupboardTableLayerArray[Index].AreaPropertyMap)
			{
				CurrentFrame.Pin()->FillAreaText(V.Key, V.Value);
			}
		}
	}
}

void FArmyConstructionModeController::AdjustViewportToShowAllContent(TWeakPtr<FArmyLayer> InCurrentLayer)
{
	auto TransformHomeFunc = [this](const FVector& CenterPos, float TY, float BY, float LX, float RX)
	{
		//0、定义外边框与出血值
		constexpr float OutsideValue = 30;//出血值
		TY -= OutsideValue;
		BY += OutsideValue;
		LX -= OutsideValue;
		RX += OutsideValue;
		FVector2D ViewportSize;
		//1、获取视口大小
		GVC->GetViewportSize(ViewportSize);
		//2、将图框居中
		GVC->SetViewLocation(CenterPos);
		//3、设置相机Zoom
		float YScale = (FMath::Abs(TY - BY)) / ViewportSize.Y;
		float XScale = (FMath::Abs(LX - RX)) / ViewportSize.X;
		float OrthoZoomScale = YScale > XScale ? ViewportSize.Y : ViewportSize.X;
		float TargetOrthZoom = YScale > XScale ?
			FMath::Abs(TY - BY) :
			FMath::Abs(LX - RX);
		if (TargetOrthZoom != 0 && OrthoZoomScale != 0)
		{
			GVC->SetOrthoZoom((TargetOrthZoom)* FMath::Max(ViewportSize.X, ViewportSize.Y) / OrthoZoomScale);
		}
	};

	FBox LayerContentBox = FBox(ForceInitToZero);

	//有图框，则根据图框获取包围盒大小
	if (CurrentFrame.IsValid())
	{
		FBox FrameBound = CurrentFrame.Pin()->GetBounds();
		FVector TargetCenterPos = FrameBound.GetCenter();
		TargetCenterPos.Z = 200;
		FVector BoundMin = FrameBound.Min;
		FVector BoundMax = FrameBound.Max;
		//定义外边框与出血值
		TransformHomeFunc(TargetCenterPos, MoveTemp(BoundMin.Y), MoveTemp(BoundMax.Y), MoveTemp(BoundMin.X), MoveTemp(BoundMax.X));
	}
	else
	{
		for (auto ClassIt : InCurrentLayer.Pin()->GetAllClass())
		{
			for (auto Obj : ClassIt.Value->GetAllObjects())
			{
				LayerContentBox += Obj->GetBounds();
			}
		}

		float Offset = 20.f;
		float TY = LayerContentBox.Min.Y;
		float BY = LayerContentBox.Max.Y;
		float LX = LayerContentBox.Min.X;
		float RX = LayerContentBox.Max.X;

		TY -= Offset;
		BY += Offset;
		LX -= Offset;
		RX += Offset;

		FVector TempCenter = LayerContentBox.GetCenter();
		TempCenter.Z = 200;
		TransformHomeFunc(TempCenter, MoveTemp(TY), MoveTemp(BY), MoveTemp(LX), MoveTemp(RX));
	}
}

void FArmyConstructionModeController::ShotCommand(bool InStart)
{
	if (InStart)
	{
		EndOperation();
		//LayerManager->GetLayerMap().GetKeys(ShotLayerNames);
		CurrentShotIndex = 0;
		//解決高亮状态下生成的第一张是高亮状态下的图纸
		if (ShotLayerNames.Num() > 0)
		{
			//截取选中的第一张图纸
			//可以按需求设定截取图纸的顺序，只需对ShotLayerNames数组进行排序即可
			OnLayerChanged(ShotLayerNames[CurrentShotIndex]);
		}

		StartShotScreen = true;
		GVC->NotShotBackColor = true;
		GVC->bDrawAxes = false;
		GIsHighResScreenshot = true;

		GVC->GetViewportSize(ViewportSize);

		GScreenshotResolutionX = GXRSettings->GetScreenshotResolutionX();
		GScreenshotResolutionY = GScreenshotResolutionX * ViewportSize.Y / ViewportSize.X;
	}
	else
	{
		StartShotScreen = false;
		GVC->NotShotBackColor = false;
		GVC->bDrawAxes = true;
		GVC->SetShowGrid(false);

		//切换回到高亮状态下的图纸画面
		OnLayerChanged(CurrentLayerName);
	}
}
void FArmyConstructionModeController::OnLayerChanged(const FName& InLayerName)
{
	//将当前的施工图纸名称存储起来
	CurrentLayerName = InLayerName;
	LayerManager->SetCurrentLayer(InLayerName);
	CurrentFrame = LayerFrameMap.FindRef(InLayerName);
	FillConstructionFrame();

	if (IsCurrentModel)
	{
		TWeakPtr<FArmyLayer> CurrentLayer = LayerManager->GetCurrentLayer();
		if (CurrentLayer.IsValid())
		{
			//没有门则显示门洞
			TSharedPtr<FArmyClass> ClassPtr = CurrentLayer.Pin()->GetOrCreateClass(Class_Door);
			if (ClassPtr.IsValid() && ClassPtr->IsVisible())
			{
				FArmyObject::SetDrawModel(MODE_DOORPASS, false);
			}
			else
			{
				FArmyObject::SetDrawModel(MODE_DOORPASS, true);
			}

			//新建墙
			if (CurrentLayer.Pin()->GetOrCreateClass(Class_AddWall)->IsVisible())
			{
				FArmyObject::SetDrawModel(MODE_MODIFYADD, true);
			}
			else
			{
				FArmyObject::SetDrawModel(MODE_MODIFYADD, false);
			}
			//删除墙
			if (CurrentLayer.Pin()->GetOrCreateClass(Class_BreakWall)->IsVisible())
			{
				FArmyObject::SetDrawModel(MODE_MODIFYDELETE, true);
			}
			else
			{
				FArmyObject::SetDrawModel(MODE_MODIFYDELETE, false);
			}
			////////////////////////////////////////////////////////
			FArmyObject::SetDrawModel(MODE_ADDWALLPOST, CurrentLayer.Pin()->GetFlag() & MODE_ADDWALLPOST ? true : false);
			FArmyObject::SetDrawModel(MODE_DELETEWALLPOST, CurrentLayer.Pin()->GetFlag() & MODE_DELETEWALLPOST ? true : false);
			////////////////////////////////////////////////////////
			FArmyObject::SetDrawModel(MODE_LAMPSTRIP, CurrentLayer.Pin()->GetFlag() & MODE_LAMPSTRIP ? true : false);
			FArmyObject::SetDrawModel(MODE_OTHERCEILINGOBJ, CurrentLayer.Pin()->GetFlag() & MODE_OTHERCEILINGOBJ ? true : false);
		}
	}

	//@马云龙 自动适配视口
	AdjustViewportToShowAllContent(LayerManager->GetCurrentLayer());
}

//开始截取屏幕中的图纸
void FArmyConstructionModeController::OnScreenShot(FViewport* InViewport)
{
	if (StartShotScreen && InViewport->GetSizeXY().X == GScreenshotResolutionX && InViewport->GetSizeXY().Y == GScreenshotResolutionY)
	{
		int32 X = GScreenshotResolutionX;
		int32 Y = GScreenshotResolutionY;
		FIntRect InRect(0, 0, X, Y);

		TArray<FColor> RawData;

		GetViewportScreenShot(InViewport, RawData, InRect);
		for (auto& C : RawData)
		{
			if (C == GVC->GetBackgroundColor().ToFColor(true) || (C.R < 50 && C.G < 50 && C.B < 50))
			{
				C.R = 255;
				C.G = 255;
				C.B = 255;
				C.A = 255;
			}
			else if (C.R == C.G && C.R == C.B && C.R > 10)
			{
				C.R = 0;
				C.G = 0;
				C.B = 0;
				C.A = 255;
			}
		}
		//ShotCommand(false);

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

		ImageWrapper->SetRaw(RawData.GetData(), RawData.GetAllocatedSize(), InRect.Width(), InRect.Height(), ERGBFormat::BGRA, 8);
		const TArray<uint8>& PNGData = ImageWrapper->GetCompressed();
		//FFileHelper::SaveArrayToFile(PNGData, *FString(TEXT("D:\\") + FString::FromInt(CurrentShotIndex) + TEXT(".jpg")));
		LocalModelService->UploadConstructionImage(LayerManager->GetCurrentLayer().Pin()->GetName(), PNGData);
		ImageWrapper.Reset();

		CurrentShotIndex++;
	}
	else if (StartShotScreen && !GIsHighResScreenshot)
	{
		if (ShotLayerNames.Num() <= CurrentShotIndex)
		{
			//关闭截图
			ShotCommand(false);
		}
		else
		{
			//切换图纸，准备截取下一张图纸
			GIsHighResScreenshot = true;
			const FName& LayerName = ShotLayerNames[CurrentShotIndex];
			OnLayerChanged(LayerName);
		}
	}
}
void FArmyConstructionModeController::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	LayerManager->Draw(PDI, View);
	FArmyDesignModeController::Draw(View, PDI);
}
void FArmyConstructionModeController::Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas)
{
	LayerManager->DrawHUD(GVC, InViewport, View, SceneCanvas);
	FArmyDesignModeController::Draw(InViewport, View, SceneCanvas);
}
bool FArmyConstructionModeController::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	auto GetAllObjects = [this, Viewport](const bool IsPressCtl) {
		{
			TArray<FObjectWeakPtr> LayerObjects, AllObjects;
			TWeakPtr<FArmyLayer> CurrentLayer = LayerManager->GetCurrentLayer();
			if (CurrentLayer.IsValid())
			{
				for (auto ClassIt : CurrentLayer.Pin()->GetAllClass())
				{
					if (ClassIt.Value->IsVisible())
					{
						LayerObjects.Append(ClassIt.Value->GetAllObjects());
					}
				}
			}
			for (auto ObjIt : LayerObjects)
			{
				EObjectType objtype = ObjIt.Pin()->GetType();

				if (LocalDatas.Contains(ObjIt) && SelectedObjArray.Contains(objtype))
				{
					AllObjects.Add(ObjIt);
				}
			}

			FArmyToolsModule::Get().GetRectSelectTool()->LeftMouseButtonPress(GVC, Viewport->GetMouseX(), Viewport->GetMouseY(), AllObjects, IsPressCtl);
		}
	};

	if (Event == IE_DoubleClick)
	{
		if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
		{
			for (auto Obj : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
			{
				if (Obj.Pin().IsValid())
				{
					switch (Obj.Pin()->GetType())
					{
					case OT_DownLeadLabel:
					{
						FVector CurrentWorldP = GVC->PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0);
						CurrentWorldP.Z = 0.0f;
						if (Obj.Pin()->IsSelected(CurrentWorldP, GVC))
						{
							FArmyDownLeadLabel* TempLabel = Obj.Pin()->AsassignObj<FArmyDownLeadLabel>();
							if (TempLabel)
							{
								DynamicFontSizeSetWidget->SetBoundObj(Obj);
								TempLabel->AttachDynamicWidget(DynamicFontSizeSetWidget);
								TempLabel->StartEditText();
							}
							return false;
						}
					}
					case OT_Dimensions:
					{
						FArmyDimensions* SelectedDimension = Obj.Pin()->AsassignObj<FArmyDimensions>();
						if (SelectedDimension->ClassType == -1)
						{
							FVector CurrentWorldP = GVC->PixelToWorld(Viewport->GetMouseX(), Viewport->GetMouseY(), 0);
							CurrentWorldP.Z = 0.0f;
							if (Obj.Pin()->IsSelected(CurrentWorldP, GVC))
							{
								FArmyDimensions* TempDimension = Obj.Pin()->AsassignObj<FArmyDimensions>();
								if (TempDimension)
								{
									GVC->ViewportOverlayWidget->RemoveSlot(DynamicFontSizeSetWidget.ToSharedRef());
									DynamicFontSizeSetWidget->SetBoundObj(Obj);
									GVC->ViewportOverlayWidget->AddSlot().HAlign(HAlign_Center).VAlign(VAlign_Center)[DynamicFontSizeSetWidget.ToSharedRef()];

									FVector2D PixPos;
									FVector2D ViewportSize;
									GVC->GetViewportSize(ViewportSize);
									GVC->WorldToPixel(TempDimension->TextControlPoint->Pos, PixPos);
									PixPos -= ViewportSize / 2;

									DynamicFontSizeSetWidget->SetRenderTransform(FSlateRenderTransform(PixPos));
								}
								return false;
							}
							break;
						}
					}
					case OT_InSideWallDimensions:
					case OT_OutSideWallDimensions:
					case OT_AddWallDimensions:
					case OT_DeleteWallDimensions:
					{
						FArmyDimensions* SelectedDimension = Obj.Pin()->AsassignObj<FArmyDimensions>();
						SelectedDimension->SetForceUseLeadLine();
						return false;
					}
					default:
						break;
					}
				}
			}
		}
	}
	else if (Event == IE_Pressed)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
			{
				PressPos = FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY());

				if (!IsPressCtl)
				{
					FArmyRectSelect::OperationInfo CurrentOperationInfo = FArmyToolsModule::Get().GetRectSelectTool()->GetCurrentOperationInfo();
					if (CurrentOperationInfo.CurrentOperationObject.IsValid() && 
						!CurrentOperationInfo.CurrentOperationPoint.IsValid() &&
						FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects().Num() >= 1)
					{
						//框选中多个obj
						SetOperation(HO_BoxChoose, XRArgument());
						for (FObjectWeakPtr Obj : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
						{
							CurrentOperation->SetSelected(Obj.Pin(), CurrentOperationInfo.CurrentOperationPoint);
						}
						return false;
					}
					else if (CurrentOperationInfo.IsValid())
					{
						
						if (CurrentOperationInfo.CurrentOperationPoint.IsValid())
						{
							if (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_TextLabel)
							{
								//SetOperation(HO_TextLabel, XRArgument());
								SetOperation(HO_BoxChoose, XRArgument());
								for (FObjectWeakPtr Obj : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
								{
									CurrentOperation->SetSelected(Obj.Pin(), CurrentOperationInfo.CurrentOperationPoint);
								}
								//CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
								return false;
							}
							else if (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_DownLeadLabel)
							{
								SetOperation(HO_DownLeadLabel, XRArgument(2).ArgUint32(1));
								CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
								return false;
							}
							else if (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_ConstructionFrame
								//|| CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_TextLabel
								|| CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_Compass
								|| CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_Entrance)
							{
								SetOperation(HO_ObjectOperation, XRArgument().ArgBoolean(true));
								CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
								return false;
							}
							else if (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_InSideWallDimensions
								|| CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_OutSideWallDimensions
								|| CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_AddWallDimensions
								|| CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_DeleteWallDimensions)
							{
								SetOperation(HO_DimensionDownLead, XRArgument(2).ArgUint32(1));
								CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
								return false;
							}
							//吊顶标注
							//开关标注
							//插座标注
							else if (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_Dimensions)
							{
								FArmyDimensions* Dimensions = CurrentOperationInfo.CurrentOperationObject.Pin()->AsassignObj<FArmyDimensions>();
								if (Dimensions/*&&Dimensions->ClassType != -1*/)//类型为-1表示为手动标注类型
								{
									SetOperation(HO_DimensionDownLead, XRArgument(2).ArgUint32(1));
									CurrentOperation->SetSelected(CurrentOperationInfo.CurrentOperationObject.Pin(), CurrentOperationInfo.CurrentOperationPoint);
									return false;
								}
							}
							else if (CurrentOperationInfo.CurrentOperationObject.Pin()->GetType() == OT_LampControlLine)
							{
								SetOperation(HO_ARCLINE, XRArgument().ArgBoolean(true));
								TWeakPtr<FArmyLampControlLines> ControllerLines = StaticCastSharedPtr<FArmyLampControlLines>(CurrentOperationInfo.CurrentOperationObject.Pin());
								CurrentOperation->SetSelectedPrimitive(ControllerLines.Pin()->GetSelectArcLine(), ControllerLines.Pin()->GetEditPoint());
								return false;
							}
						}
					}
				}
				GetAllObjects(IsPressCtl);
			}
		}
		else if (Key == EKeys::Delete)
		{
			/*@梁晓菲 如果框选到的有OT_TextLabel或者OT_ComponentBase，那么删除快捷键无效*/
			bool tempBSelectedTextLabel = false;
			for (auto& It : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
			{
				if (It.Pin()->GetType() == EObjectType::OT_TextLabel)
				{
					//自动生成的室内净高标注支持删除
					FArmyTextLabel* TextLabel = It.Pin()->AsassignObj<FArmyTextLabel>();
					if (TextLabel&&TextLabel->GetLabelType() == FArmyTextLabel::LT_SpaceHeight)
					{
						continue;
					}
				}
				if (It.Pin()->GetType() == EObjectType::OT_TextLabel || It.Pin()->GetType() == EObjectType::OT_ComponentBase)
				{
					tempBSelectedTextLabel = true;
				}
			}
			if (!tempBSelectedTextLabel)
			{
				DeleteSelectedObjects();
			}
		}
		else if (Key == EKeys::LeftControl || Key == EKeys::RightControl)
		{
			//多选
			IsPressCtl = true;
			FArmyToolsModule::Get().GetRectSelectTool()->SetPickOne(false);
		}
	}
	else if (Event == IE_Released)
	{
		if (Key == EKeys::LeftMouseButton)
		{
			if (FArmyToolsModule::Get().GetRectSelectTool()->GetState() > 0)
			{
				ReleasePos = FVector2D(Viewport->GetMouseX(), Viewport->GetMouseY());

				if (FVector2D::Distance(PressPos, ReleasePos) > 0.1)//框选
				{
					GetAllObjects(IsPressCtl);
				}
				else
				{
					//解除了框选时失灵问题
					FArmyToolsModule::Get().GetRectSelectTool()->SetState(1);
				}
			}
		}
		else if (Key == EKeys::LeftControl || Key == EKeys::RightControl)
		{
			IsPressCtl = false;
			//单选
			FArmyToolsModule::Get().GetRectSelectTool()->SetPickOne(true);
		}
	}
	return FArmyDesignModeController::InputKey(Viewport, Key, Event);
}

void FArmyConstructionModeController::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	FArmyDesignModeController::MouseMove(Viewport, X, Y);

	// 	static FObjectPtr CurrentHoverObj = NULL;
	// 	FObjectPtr TempHoverObj = NULL;
	// 
	// 	for (FObjectPtr ObjPtr : DemensionObjList)
	// 	{
	// 		if (ObjPtr.IsValid() && ObjPtr->GetState() != EObjectState::OS_Selected)
	// 		{
	// 			if (ObjPtr->Hover(FVector(GVC->PixelToWorld(X, Y, 0)), GVC))
	// 			{
	// 				TempHoverObj = ObjPtr;
	// 				break;
	// 			}
	// 		}
	// 	}
	// 	
	// 	if (CurrentHoverObj != TempHoverObj)
	// 	{
	// 		if (TempHoverObj.IsValid())
	// 		{
	// 			TempHoverObj->SetState(EObjectState::OS_Hovered);
	// 		}
	// 		if (CurrentHoverObj.IsValid() && CurrentHoverObj->GetState() != EObjectState::OS_Selected)
	// 		{
	// 			CurrentHoverObj->SetState(EObjectState::OS_Normal);
	// 		}
	// 		CurrentHoverObj = TempHoverObj;
	// 	}
}

bool FArmyConstructionModeController::MouseDrag(FViewport* Viewport, FKey Key)
{
	return FArmyDesignModeController::MouseDrag(Viewport, Key);

	
}

void FArmyConstructionModeController::Tick(float DeltaSeconds)
{
	if (LayerManager->GetCurrentLayer().IsValid())
	{
		TArray<TSharedPtr<FArmyObject>> Objects;
		for (auto ClassIt : LayerManager->GetCurrentLayer().Pin()->GetAllClass())
		{
			if (ClassIt.Value->IsVisible())
			{
				Objects.Append(ClassIt.Value->GetAllObjects());
			}
		}
		for (auto Ruler : LocalAutoObjectMap.FindRef(OT_ObjectRuler))
		{
			FArmyObjAutoRuler* AutoRuler = Ruler.ObjPtr.Pin()->AsassignObj<FArmyObjAutoRuler>();
			if (AutoRuler)
			{
				AutoRuler->Update(Objects);
			}
		}
	}
	FArmyDesignModeController::Tick(DeltaSeconds);
}
void FArmyConstructionModeController::Load(TSharedPtr<FJsonObject> Data)
{
	//LocalFunction->GenerateAutoDimensions();
	//LocalFunction->GenerateAutoObjectRuler();

	if (Data.IsValid())
	{
		BeenUpdated = true;
		TSharedPtr<FJsonObject> ConstructionModeData = Data->GetObjectField("ConstructionModel");

		const TArray< TSharedPtr<FJsonValue> > Layers = ConstructionModeData->GetArrayField(TEXT("Layer"));
		for (auto& LayerIt : Layers)
		{
			TSharedPtr<FJsonObject> JObject = LayerIt->AsObject();
			FName LayerName = FName(*(JObject->GetStringField("Name")));

			bool IsStaticLayer = true;
			JObject->TryGetBoolField("IsStaticLayer",IsStaticLayer);
			int32 LayerType = -1;
			JObject->TryGetNumberField("LayerType", LayerType);
			
			if (!IsStaticLayer)
			{
				DelegateNewLayer(LayerName, false, LayerType);
			}

			LayerManager->SetCurrentLayer(LayerName);
			
			for (auto& AloneObjIt : JObject->GetArrayField("ObjectList"))
			{
				TSharedPtr<FJsonObject> AloneJsonObj = AloneObjIt->AsObject();
				if (AloneJsonObj.IsValid())
				{
					FString ClassName = AloneJsonObj->GetStringField(TEXT("RegisterClass"));
					FObjectPtr ObjPtr = ClassFactory::GetInstance(ClassName);
					if (ObjPtr.IsValid())
					{
						ObjPtr->Deserialization(AloneJsonObj);
						FArmySceneData::Get()->Add(ObjPtr, XRArgument(1).ArgBoolean(true).ArgUint32(E_ConstructionModel));
					}
				}
			}
		}

		const TArray< TSharedPtr<FJsonValue> > DynamicData = ConstructionModeData->GetArrayField("DynamicData");
		for (auto JsonObjIt : DynamicData)
		{
			TSharedPtr<FJsonObject> DynamicJsonObj = JsonObjIt->AsObject();

			if (DynamicJsonObj.IsValid())
			{
				FString AutoTag = TEXT("AUTO"),StrLayerName = TEXT("");
				DynamicJsonObj->TryGetStringField(TEXT("AutoTag"), AutoTag);//自动生成的数据标识
				bool PointLayer = DynamicJsonObj->TryGetStringField(TEXT("LayerName"), StrLayerName);
				
				FString ObjType = DynamicJsonObj->GetStringField("Type");
				if (ObjType == TEXT("OT_ObjectRuler"))
				{
					FString ObjID = DynamicJsonObj->GetStringField("BindObject");
					FGuid TempID;
					FGuid::Parse(ObjID, TempID);

					////////////////////////////////////////////////////////////////////////////////////////change at 2018-10-25 start
					FObjectWeakPtr BindObj = FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, TempID);

					if (BindObj.IsValid())
					{
						TSharedPtr<FArmyObjAutoRuler> ObjRuler = MakeShareable(new FArmyObjAutoRuler());
						ObjRuler->SetBoundObj(BindObj.Pin());
						ObjRuler->SetAutoCheck(true);
						FArmySceneData::Get()->Add(ObjRuler, XRArgument(1).ArgString(AutoTag).ArgUint32(E_ConstructionModel));
					}
					////////////////////////////////////////////////////////////////////////////////////////change at 2018-10-25 end

					//for (auto AutoRulerIt : LocalAutoObjectMap.FindRef(OT_ObjectRuler))
					//{
					//	FArmyObjAutoRuler* AutoRuler = AutoRulerIt.Pin()->AsassignObj<FArmyObjAutoRuler>();
					//	if (AutoRuler)
					//	{
					//		FObjectPtr BoundObject = AutoRuler->GetBoundObj();
					//		if (BoundObject.IsValid())
					//		{
					//			if (BoundObject->GetUniqueID() == TempID)
					//			{
					//				AutoRuler->SetLineVisible(FArmyObjAutoRuler::XBACK, DynamicJsonObj->GetBoolField("LeftRuler"));
					//				AutoRuler->SetLineVisible(FArmyObjAutoRuler::XFRONT, DynamicJsonObj->GetBoolField("RightRuler"));
					//				AutoRuler->SetLineVisible(FArmyObjAutoRuler::YFRONT, DynamicJsonObj->GetBoolField("FrontRuler"));
					//				AutoRuler->SetLineVisible(FArmyObjAutoRuler::YBACK, DynamicJsonObj->GetBoolField("BackRuler"));
					//				AutoRuler->SetLineVisible(FArmyObjAutoRuler::WIDTH, DynamicJsonObj->GetBoolField("WidthRuler"));
					//				AutoRuler->SetLineVisible(FArmyObjAutoRuler::LENGTH, DynamicJsonObj->GetBoolField("LenthRuler"));
					//				break;
					//			}
					//		}
					//	}
					//}
				}
				else if (ObjType == TEXT("OT_TextLabel"))
				{
					FString ClassName = DynamicJsonObj->GetStringField(TEXT("RegisterClass"));
					if (ClassName.IsEmpty()) continue;
					FObjectPtr ObjPtr = ClassFactory::GetInstance(ClassName);
					if (ObjPtr.IsValid())
					{
						ObjPtr->Deserialization(DynamicJsonObj);
						FArmyTextLabel* TextLabel = ObjPtr->AsassignObj<FArmyTextLabel>();
						if (TextLabel)
						{
							FObjectPtr BindObj;
							int32 Tag = -1;
							TextLabel->GetObjTag(BindObj, Tag);
							FObjectWeakPtr NewBindObj = FArmySceneData::Get()->GetObjectByGuidAlone(TextLabel->GetBindObjGuid());
							if (NewBindObj.IsValid())
							{
								TextLabel->SetObjTag(NewBindObj.Pin(), Tag);
							}
						}
						FArmySceneData::Get()->Add(ObjPtr, XRArgument(1).ArgString(AutoTag).ArgUint32(E_ConstructionModel));

						SCOPE_TRANSACTION(TEXT("变换"));
						ObjPtr->Modify();
					}

					//FString ObjID = DynamicJsonObj->GetStringField("BindObject");
					//int32 JsonObjTag = DynamicJsonObj->GetIntegerField("ObjectTag");

					//FGuid TempID;
					//FGuid::Parse(ObjID, TempID);

					//FVector LabelPos;
					//LabelPos.InitFromString(DynamicJsonObj->GetStringField("LabelPos"));

					//for (auto TextLabelIt : LocalAutoObjectMap.FindRef(OT_TextLabel))
					//{
					//	FArmyTextLabel* TextLabel = TextLabelIt.Pin()->AsassignObj<FArmyTextLabel>();
					//	if (TextLabel)
					//	{
					//		FObjectPtr BindObj;
					//		int32 ObjTag = -1;
					//		TextLabel->GetObjTag(BindObj, ObjTag);
					//		if (BindObj->GetUniqueID() == TempID && JsonObjTag == ObjTag)
					//		{
					//			FVector LabelPos;
					//			LabelPos.InitFromString(DynamicJsonObj->GetStringField("LabelPos"));
					//			TextLabel->SetWorldPosition(LabelPos);
					//			break;
					//		}
					//	}
					//}
				}
				else
				{
					FString ClassName = DynamicJsonObj->GetStringField(TEXT("RegisterClass"));
					FObjectPtr ObjPtr = ClassFactory::GetInstance(ClassName);
					ObjPtr->Deserialization(DynamicJsonObj);
					if (ObjPtr.IsValid())
					{
						if (PointLayer)//所在图层名称
						{
							FArmySceneData::Get()->Add(ObjPtr, XRArgument(1).ArgString(AutoTag).ArgFName(FName(*StrLayerName)).ArgUint32(E_ConstructionModel));
						}
						else
						{
							FArmySceneData::Get()->Add(ObjPtr, XRArgument(1).ArgString(AutoTag).ArgUint32(E_ConstructionModel));
						}
					}
				}
			}
		}

		const TArray< TSharedPtr<FJsonValue> > GlobalDatas = ConstructionModeData->GetArrayField(TEXT("GlobalData"));
		for (auto& ObjIt : GlobalDatas)
		{
			TSharedPtr<FJsonObject> JObject = ObjIt->AsObject();
			FString Param;
			JObject->TryGetStringField("LocalData_Param", Param);
			FString ClassName = JObject->GetStringField(TEXT("RegisterClass"));
			FObjectPtr ObjPtr = ClassFactory::GetInstance(ClassName);
			if (ObjPtr.IsValid())
			{
				ObjPtr->Deserialization(JObject);
				FArmySceneData::Get()->Add(ObjPtr, XRArgument(1).ArgUint32(E_ConstructionModel).ArgString(Param));
			}
		}

		const TArray< TSharedPtr<FJsonValue> > IndexSymbolGroupsData = ConstructionModeData->GetArrayField(TEXT("IndexSymbolGroupData"));
		for (auto SymbolGroupIt : IndexSymbolGroupsData)
		{
			TSharedPtr<FJsonObject> JObject = SymbolGroupIt->AsObject();
			FString RoomID;
			JObject->TryGetStringField("RoomID", RoomID);
			FGuid TempID;
			FGuid::Parse(RoomID, TempID);

			TSharedPtr<FArmyRoom> CurrentRoom = StaticCastSharedPtr<FArmyRoom>(FArmySceneData::Get()->GetObjectByGuid(E_HomeModel, TempID).Pin());
			FBox roomBounds = CurrentRoom->GetBounds();
			FVector CenterPos = roomBounds.GetCenter();
			FVector EditPointPos;
			if (JObject->GetStringField("EditPointPos").IsEmpty())
			{
				EditPointPos = CenterPos;
			}
			else
			{
				EditPointPos.InitFromString(JObject->GetStringField("EditPointPos"));
			}
			IndexSubController->AddSymbolGroupData(TempID,MakeShareable(new FIndexSymbolGroup(EditPointPos)));
			IndexSubController->GetSymbolGroupMap().Find(TempID)->Get()->SymbolGroupLeadLine->Deserialization(JObject);
			
			TSharedPtr<FJsonObject> LeftRectRegionJobj = JObject->GetObjectField(TEXT("LeftRectRegion"));
			IndexSubController->GetSymbolGroupMap().Find(TempID)->Get()->SymbolLeftRectRegion->Deserialization(LeftRectRegionJobj);
			TSharedPtr<FJsonObject> RightRectRegionJobj = JObject->GetObjectField(TEXT("RightRectRegion"));
			IndexSubController->GetSymbolGroupMap().Find(TempID)->Get()->SymbolRightRectRegion->Deserialization(RightRectRegionJobj);
			TSharedPtr<FJsonObject> TopRectRegionJobj = JObject->GetObjectField(TEXT("TopRectRegion"));
			IndexSubController->GetSymbolGroupMap().Find(TempID)->Get()->SymbolTopRectRegion->Deserialization(TopRectRegionJobj);
			TSharedPtr<FJsonObject> BottomRectRegionJobj = JObject->GetObjectField(TEXT("BottomRectRegion"));
			IndexSubController->GetSymbolGroupMap().Find(TempID)->Get()->SymbolBottomRectRegion->Deserialization(BottomRectRegionJobj);
			
			IndexSubController->UpdateSymbol();	
		}
		// 设置最新的数据
		LastObjMap = GetAllObjectWithoutConstruct();
	}
	LayerManager->SetCurrentLayer(CurrentLayerName);
}

bool FArmyConstructionModeController::Save(TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > JsonWriter)
{
	JsonWriter->WriteObjectStart(TEXT("ConstructionModel"));//1

	JsonWriter->WriteArrayStart(TEXT("DynamicData"));
	for (auto& DataArray : LocalAutoObjectMap)
	{
		for (auto AutoData : DataArray.Value)
		{
			switch (AutoData.ObjPtr.Pin()->GetType())
			{
			case OT_ObjectRuler:
			{
				FArmyObjAutoRuler* ObjRuler = AutoData.ObjPtr.Pin()->AsassignObj<FArmyObjAutoRuler>();
				if (ObjRuler)
				{
					FObjectPtr ObjPtr = ObjRuler->GetBoundObj();
					if (ObjPtr.IsValid())
					{
						JsonWriter->WriteObjectStart();//6
						JsonWriter->WriteValue("AutoTag", AutoData.ObjTag);//自动生成的数据标识
						JsonWriter->WriteValue("Type", TEXT("OT_ObjectRuler"));
						JsonWriter->WriteValue("BindObject", ObjPtr->GetUniqueID().ToString());

						JsonWriter->WriteValue("LeftRuler", ObjRuler->IsLineVisible(FArmyObjAutoRuler::XBACK));
						JsonWriter->WriteValue("RightRuler", ObjRuler->IsLineVisible(FArmyObjAutoRuler::XFRONT));
						JsonWriter->WriteValue("FrontRuler", ObjRuler->IsLineVisible(FArmyObjAutoRuler::YBACK));
						JsonWriter->WriteValue("BackRuler", ObjRuler->IsLineVisible(FArmyObjAutoRuler::YFRONT));
						JsonWriter->WriteValue("WidthRuler", ObjRuler->IsLineVisible(FArmyObjAutoRuler::WIDTH));
						JsonWriter->WriteValue("LenthRuler", ObjRuler->IsLineVisible(FArmyObjAutoRuler::LENGTH));
						JsonWriter->WriteObjectEnd();//6
					}
				}
			}
			break;
			//case OT_TextLabel:
			//{
			//	FArmyTextLabel* ObjTextLabel = AutoData.Pin()->AsassignObj<FArmyTextLabel>();
			//	FObjectPtr LabelObj;
			//	int32 LabelObjTag = -1;
			//	ObjTextLabel->GetObjTag(LabelObj, LabelObjTag);
			//	if (LabelObj.IsValid())
			//	{
			//		JsonWriter->WriteObjectStart();//6
			//		JsonWriter->WriteValue("Type", TEXT("OT_TextLabel"));
			//		JsonWriter->WriteValue("BindObject", LabelObj->GetUniqueID().ToString());
			//		JsonWriter->WriteValue("ObjectTag", LabelObjTag);

			//		JsonWriter->WriteValue("LabelPos", ObjTextLabel->GetWorldPosition().ToString());
			//		JsonWriter->WriteObjectEnd();//6
			//	}
			//}
			//	break;
			default:
				JsonWriter->WriteObjectStart();
				JsonWriter->WriteValue("AutoTag", AutoData.ObjTag);//自动生成的数据标识
				if(!AutoData.LayerName.IsNone()) 
					JsonWriter->WriteValue("LayerName", AutoData.LayerName.ToString());//所在图层名称
				AutoData.ObjPtr.Pin()->SerializeToJson(JsonWriter);
				JsonWriter->WriteObjectEnd();
				break;
			}
		}
	}
	JsonWriter->WriteArrayEnd(); //DynamicData

	JsonWriter->WriteArrayStart(TEXT("Layer"));

	//@马云龙 先遍历所有图层，优先保存到Json中，因为在载入的时候，会边读取Layer信息，边创建，这样可以保证Layer的顺序是跟保存前一致的。
	//如果没有这一步，则会按照后面的LocalDatas里的Object所在Layer来决定Json中Layer保存的顺序
	for (auto& It : LayerOutLiner->RootTreeItems)
	{
		TWeakPtr<FArmyLayer> TheLayer = LayerManager->GetLayer(It->ModuleName);
		if (TheLayer.IsValid() && TheLayer.Pin().IsValid())
		{
			JsonWriter->WriteObjectStart();
			JsonWriter->WriteValue(TEXT("Name"), It->ModuleName.ToString());
			JsonWriter->WriteValue(TEXT("IsStaticLayer"), TheLayer.Pin()->IsStaticLayer());
			JsonWriter->WriteValue(TEXT("LayerType"), TheLayer.Pin()->GetLayerType());
			JsonWriter->WriteObjectEnd();
		}
	}

	FName TempLayerName;
	bool NeaLayerStart = false;

	struct FNameEqual
	{
		FORCEINLINE bool operator()(const FLayerInfo& A, const FLayerInfo& B) const
		{
			return B.LayerName == A.LayerName;
		}
	};
	LocalDatas.ValueSort(FNameEqual());
	TArray<TPair<FObjectWeakPtr,FString>> GlobalObjectArray;
	for (auto ObjIt : LocalDatas)
	{
		TWeakPtr<FArmyLayer> TheLayer = LayerManager->GetLayer(ObjIt.Value.LayerName);
		if (TheLayer.IsValid())
		{
			if (TempLayerName != ObjIt.Value.LayerName)
			{
				TempLayerName = ObjIt.Value.LayerName;
				if (NeaLayerStart)
				{
					JsonWriter->WriteArrayEnd();
					JsonWriter->WriteObjectEnd();
				}
				NeaLayerStart = true;
				JsonWriter->WriteObjectStart();
				JsonWriter->WriteValue(TEXT("Name"), TempLayerName.ToString());
				JsonWriter->WriteValue(TEXT("IsStaticLayer"), TheLayer.Pin()->IsStaticLayer());
				JsonWriter->WriteValue(TEXT("LayerType"), TheLayer.Pin()->GetLayerType());
				JsonWriter->WriteArrayStart(TEXT("ObjectList"));
			}
			if (!ObjIt.Value.AutoData)
			{
				JsonWriter->WriteObjectStart();
				ObjIt.Key.Pin()->SerializeToJson(JsonWriter);
				JsonWriter->WriteObjectEnd();
			}
		}
		else
		{
			EObjectType ObjType = ObjIt.Key.Pin()->GetType();
			if (ObjType == OT_ComponentBase) continue;//构件由软装模式下的模型关联生成，无需在此保存与加载
			const TArray<FAutoObjectInfo>* AutoObjArray = LocalAutoObjectMap.Find(ObjType);
			if (!(AutoObjArray && AutoObjArray->Contains(FAutoObjectInfo(ObjIt.Key))))
			{
				GlobalObjectArray.AddUnique(TPair<FObjectWeakPtr,FString>(ObjIt.Key,ObjIt.Value.ObjTag));
			}
		}
	}
	if (NeaLayerStart)
	{
		JsonWriter->WriteArrayEnd();
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd();//Layer End

	JsonWriter->WriteArrayStart(TEXT("GlobalData"));
	for (auto ObjIt : GlobalObjectArray)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("LocalData_Param", ObjIt.Value);
		ObjIt.Key.Pin()->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();
	}
	JsonWriter->WriteArrayEnd(); // GlobalData End

	JsonWriter->WriteArrayStart(TEXT("IndexSymbolGroupData"));
	
	//遍历所有符号组，提取数据
	TMap<FGuid, TSharedPtr<FIndexSymbolGroup>> IndexSymbolGroups =  IndexSubController->GetSymbolGroupMap();
	for (auto SymbolGroup : IndexSymbolGroups)
	{
		JsonWriter->WriteObjectStart();
		JsonWriter->WriteValue("RoomID", SymbolGroup.Key.ToString());
		JsonWriter->WriteValue("EditPointPos", SymbolGroup.Value->EditPoint->GetPos().ToString());
		SymbolGroup.Value->SymbolGroupLeadLine->SerializeToJson(JsonWriter);

		JsonWriter->WriteObjectStart("LeftRectRegion");
		SymbolGroup.Value->SymbolLeftRectRegion->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart("RightRectRegion");
		SymbolGroup.Value->SymbolRightRectRegion->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart("TopRectRegion");
		SymbolGroup.Value->SymbolTopRectRegion->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectStart("BottomRectRegion");
		SymbolGroup.Value->SymbolBottomRectRegion->SerializeToJson(JsonWriter);
		JsonWriter->WriteObjectEnd();

		JsonWriter->WriteObjectEnd();
	}

	JsonWriter->WriteArrayEnd(); // IndexSymbolGroupData End
	JsonWriter->WriteObjectEnd();//1

	return true;
}
void FArmyConstructionModeController::Delete()
{
	DeleteSelectedObjects();
}
void FArmyConstructionModeController::DeleteSelectedObjects()
{
	SCOPE_TRANSACTION(TEXT("图纸模式删除"));
	for (auto ObjectIt : FArmyToolsModule::Get().GetRectSelectTool()->GetSelectedObjects())
	{
		if (!ObjectIt.Pin().IsValid() || ObjectIt.Pin()->GetType() == OT_ComponentBase) continue;

		else if (ObjectIt.Pin()->GetType() == EObjectType::OT_InSideWallDimensions
			|| ObjectIt.Pin()->GetType() == EObjectType::OT_OutSideWallDimensions
			|| ObjectIt.Pin()->GetType() == EObjectType::OT_AddWallDimensions
			|| ObjectIt.Pin()->GetType() == EObjectType::OT_DeleteWallDimensions)
		{
			FArmyDimensions* SelectedDimension = ObjectIt.Pin()->AsassignObj<FArmyDimensions>();
			SelectedDimension->StopUseLeadLine();
			continue;
		}
		//吊顶标注
		//开关标注
		//插座标注
		else if (ObjectIt.Pin()->GetType() == EObjectType::OT_Dimensions)
		{
			FArmyDimensions* Demensions = ObjectIt.Pin()->AsassignObj<FArmyDimensions>();
			if (Demensions && Demensions->ClassType != -1)//等于-1是的手动标注的类型
			{
				FArmyDimensions* SelectedDimension = ObjectIt.Pin()->AsassignObj<FArmyDimensions>();
				SelectedDimension->StopUseLeadLine();
				continue;
			}
		}

		//删除图框
		if (ObjectIt.IsValid() && ObjectIt.Pin()->GetType() == EObjectType::OT_ConstructionFrame)
		{
			CurrentFrame = NULL;

			TArray<FName> LayerNameArray;
			LayerFrameMap.GetKeys(LayerNameArray);
			for (auto& LName : LayerNameArray)
			{
				TWeakPtr<FArmyConstructionFrame> F = LayerFrameMap.FindRef(LName);
				if (F == ObjectIt)
				{
					LayerFrameMap.Remove(LName);
				}
			}
		}

		const TArray<FAutoObjectInfo>* ObjArray = LocalAutoObjectMap.Find(ObjectIt.Pin()->GetType());
		if (ObjArray)
		{
			if (ObjArray->Contains(FAutoObjectInfo(ObjectIt)))
			{
				FArmySceneData::Get()->Delete(ObjectIt.Pin());
				continue;
			}
		}
		if (LocalDatas.Contains(ObjectIt))
		{
			FArmySceneData::Get()->Delete(ObjectIt.Pin());
		}
	}
	FArmyToolsModule::Get().GetRectSelectTool()->Clear();
}
void FArmyConstructionModeController::ClearAutoObjects()
{
	for (auto & ArrayIt : LocalAutoObjectMap)
	{
		for (auto ObjIt : ArrayIt.Value)
		{
			if (ObjIt.ObjPtr.IsValid())
			{
				FArmySceneData::Get()->Delete(ObjIt.ObjPtr.Pin());
			}
		}
	}
	LocalAutoObjectMap.Empty();
}

void FArmyConstructionModeController::OnSaveCommand()
{
	FArmyFrameCommands::OnMenuSave();
}
void FArmyConstructionModeController::OnDeleteCommand()
{
	Delete();
}

void FArmyConstructionModeController::OnUndoCommand()
{
	FArmyFrameCommands::OnMenuUndo();
}

void FArmyConstructionModeController::OnRedoCommand()
{
	FArmyFrameCommands::OnMenuRedo();
}

void FArmyConstructionModeController::OnLoadFrameCommand()
{
	LocalModelService->DownLoadConstructionFrame();
}
void FArmyConstructionModeController::OnUploadImageStep(const int32 InStep, const FString& InLayerName)
{
	if (ProgressBarDialog.IsValid())
	{
		ProgressBarDialog->SetCurrentLabel(FText::FromString(InLayerName));
		ProgressBarDialog->SetCurrentText(FText::FromString(FString::FromInt(InStep) + TEXT("/") + FString::FromInt(ShotLayerNames.Num())));
		if (InStep == ShotLayerNames.Num())
		{
			ProgressBarDialog = NULL;
			GGI->Window->DismissModalDialog();
		}
		else
		{
			ProgressBarDialog->SetCurrentPercent(float(InStep) / ShotLayerNames.Num());
		}
	}
}
void FArmyConstructionModeController::OnSaveImageCommand()
{
	//auto TransformHomeFunc = [this](const FVector& CenterPos, float TY, float BY, float LX, float RX)
	//{
	//	//0、定义外边框与出血值
	//	constexpr float OutsideValue = 30;//出血值
	//	TY -= OutsideValue;
	//	BY += OutsideValue;
	//	LX -= OutsideValue;
	//	RX += OutsideValue;
	//	//1、获取视口大小
	//	GVC->GetViewportSize(ViewportSize);
	//	//2、将图框居中
	//	GVC->SetViewLocation(CenterPos);
	//	//3、设置相机Zoom
	//	float YScale = (FMath::Abs(TY - BY)) / ViewportSize.Y;
	//	float XScale = (FMath::Abs(LX - RX)) / ViewportSize.X;
	//	float OrthoZoomScale = YScale > XScale ? ViewportSize.Y : ViewportSize.X;
	//	float TargetOrthZoom = YScale > XScale ?
	//		FMath::Abs(TY - BY) :
	//		FMath::Abs(LX - RX);
	//	if (TargetOrthZoom != 0 && OrthoZoomScale != 0)
	//	{
	//		GVC->SetOrthoZoom((TargetOrthZoom)* FMath::Max(ViewportSize.X, ViewportSize.Y) / OrthoZoomScale);
	//	}
	//};

	////有图框，则根据图框获取包围盒大小
	//if (CurrentFrame.IsValid())
	//{
	//	FBox FrameBound = CurrentFrame.Pin()->GetBounds();
	//	const FVector& TargetCenterPos = FrameBound.GetCenter();

	//	FVector BoundMin = FrameBound.Min;
	//	FVector BoundMax = FrameBound.Max;
	//	//定义外边框与出血值
	//	TransformHomeFunc(TargetCenterPos, MoveTemp(BoundMin.Y), MoveTemp(BoundMax.Y), MoveTemp(BoundMin.X), MoveTemp(BoundMax.X));
	//}
	////无图框，根据三层标注最外围为界
	//else
	//{
	//	if (LocalFunction->TY==0 &&
	//		LocalFunction->BY==0 &&
	//		LocalFunction->LX==0 &&
	//		LocalFunction->RX==0)
	//	{
	//		//刷新
	//		OnUpdateDismensionCommand();
	//	}
	//	//定义外边框与出血值
	//	float TY = LocalFunction->TY - LocalFunction->FirstDimensionOffset;
	//	float BY = LocalFunction->BY + LocalFunction->FirstDimensionOffset;
	//	float LX = LocalFunction->LX - LocalFunction->FirstDimensionOffset;
	//	float RX = LocalFunction->RX + LocalFunction->FirstDimensionOffset;
	//	TransformHomeFunc(LocalFunction->BoundBox.GetCenter(), MoveTemp(TY), MoveTemp(BY), MoveTemp(LX), MoveTemp(RX));
	//}
	TSharedPtr<SArmyGeneratePrint> GeneratePrintDialog = SNew(SArmyGeneratePrint).OnGeneratePrintStart(FOnGeneratePrintStart::CreateRaw(this, &FArmyConstructionModeController::OnGeneratePrintStart));

	TArray<FName> AllLayers;
	LayerManager->GetLayerMap().GetKeys(AllLayers);
	GeneratePrintDialog->InitContent(AllLayers);

	GGI->Window->DismissModalDialog();
	GGI->Window->PresentModalDialog(TEXT("生成图纸"), GeneratePrintDialog.ToSharedRef());
}
void FArmyConstructionModeController::IndexSubCommand(int32 InType)
{
	if (InType == 0)
	{
		IndexSubController->EndMode();
		GGI->Window->DismissModalViewController();
	}
	else
	{
		TArray<FName> LayerNameArray;
		TArray<FObjectPtr> TempObjectArray;
		for (auto It : LayerManager->GetLayerMap())
		{
			if (It.Value->GetLayerType() == FArmyLayer::LT_D_WALLFACADE && !It.Value->IsStaticLayer() /*&& It.Key != TEXT("立面索引图")*/)
			{
				LayerNameArray.AddUnique(It.Key);
				for (auto ClassIt : It.Value->GetAllClass())
				{
					for (auto Obj : ClassIt.Value->GetAllObjects())
					{
						TempObjectArray.Add(Obj);
					}
				}
			}
		}
		for (auto& ObjPtr : TempObjectArray)
		{
			FArmySceneData::Get()->Delete(ObjPtr);
		}
		for (auto& LayerName : LayerNameArray)
		{
			DeleteLayer(LayerName);
		}

		auto GenerateFaceLayer = [&](const FName& InLayerName)
		{
			TWeakPtr<FArmyLayer> TempNewLayer = NewLayer(InLayerName, false);
			TempNewLayer.Pin()->SetLayerType(FArmyLayer::FLayerType(FArmyLayer::LT_D_WALLFACADE));

			TempNewLayer.Pin()->GetOrCreateClass(Class_BaseWall)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Door)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Pass)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Window)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Furniture)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Switch)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Socket)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_ElectricBoxH)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_ElectricBoxL)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Light)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_AirConditionerPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_WaterRoutePoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_HotWaterPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_HotAndColdWaterPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_ColdWaterPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_FloordRainPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_ClosestoolRainPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_WashBasinRainPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_RecycledWaterPoint)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_HomeEntryWaterPoint)->SetVisible(true);

			TempNewLayer.Pin()->GetOrCreateClass(Class_Beam)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Socket_H)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Socket_L)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_DiversityWater)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_WaterPipe)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Heater)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_GasMeter)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_GasPipe)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_SCTCabinet)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_SCTHardware)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Walllayout)->SetVisible(true);
			TempNewLayer.Pin()->GetOrCreateClass(Class_Other)->SetVisible(true);
		};

		FName LayerName = IndexSubController->GetDataContainner()->GetName();//立面索引图
		TWeakPtr<FArmyLayer> TempNewLayer = NewLayer(LayerName, false);
		TempNewLayer.Pin()->SetLayerType(FArmyLayer::FLayerType(FArmyLayer::LT_D_WALLFACADE));
		for (auto Class : IndexSubController->GetDataContainner()->GetAllClass())
		{
			TempNewLayer.Pin()->GetOrCreateClass(Class.Key)->SetVisible(Class.Value->IsVisible());
		}

		for (auto ObjPtr : FArmySceneData::Get()->GetObjects(E_LayoutModel))
		{
			FArmySceneData::Get()->Add(ObjPtr.Pin()->CopySelf(true), XRArgument(1).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
		}
		TArray<FString> CharArray{ TEXT("A"),TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E"), TEXT("F"), TEXT("G"), TEXT("H"), TEXT("I"), TEXT("J"), TEXT("K") };

		TArray<FString> RoomTemplateArray{ TEXT("客餐厅"),TEXT("客厅"),TEXT("餐厅"),TEXT("主卧"),TEXT("次卧"),TEXT("客卧"),TEXT("书房"),TEXT("儿童房"),TEXT("储物间"),TEXT("玄关"),TEXT("走廊"),TEXT("过道"),TEXT("厨房"),TEXT("卫生间"),TEXT("主卫"),TEXT("客卫"),TEXT("阳台"),TEXT("洗衣间"),TEXT("未命名") };

		struct FRoomNameRangeInfo
		{
			FRoomNameRangeInfo(FObjectWeakPtr InRoom, int32 InKey) :RoomObjPtr(InRoom), SortKey(InKey) {}
			FObjectWeakPtr RoomObjPtr;
			int32 SortKey = -1;
			int32 SortKey2 = 0;

			bool operator==(const FRoomNameRangeInfo& InOther)const
			{
				return SortKey == InOther.SortKey && SortKey2 == InOther.SortKey2;
			}
		};

		const TMap<FGuid, TSharedPtr<FIndexSymbolGroup>>& CurrentGroupMap = IndexSubController->GetSymbolGroupMap();
		TArray<FRoomNameRangeInfo> RoomSortArray;
		for (auto& GroupIt : CurrentGroupMap)
		{
			FObjectWeakPtr ObjPtr = FArmySceneData::Get()->GetObjectByGuid(E_LayoutModel, GroupIt.Key);

			if (ObjPtr.Pin()->GetType() == OT_InternalRoom)
			{
				FArmyRoom* Room = ObjPtr.Pin()->AsassignObj<FArmyRoom>();
				if (Room)
				{
					for (int32 i = 0; i < RoomTemplateArray.Num(); i++)
					{
						if (Room->GetSpaceName().Contains(RoomTemplateArray[i]))
						{
							//@马云龙  当RoomSpaceName为 客餐厅、客餐厅1、客餐厅2， 且RoomTemplateArray[i]为餐厅时，就会出现错误的判断，此处特意避免这种情况
							if (Room->GetSpaceName().Contains(TEXT("客餐厅")) && RoomTemplateArray[i] == TEXT("餐厅"))
								continue;

							FRoomNameRangeInfo NameInfo(ObjPtr, i);
							while (RoomSortArray.Find(NameInfo) != INDEX_NONE)
							{
								NameInfo.SortKey2++;
							}
							RoomSortArray.Add(NameInfo);
						}
					}
				}
			}
		}

		RoomSortArray.Sort([&](const FRoomNameRangeInfo& A, const FRoomNameRangeInfo& B) {
			if (A.SortKey < B.SortKey)
			{
				return true;
			}
			else if (A.SortKey == B.SortKey)
			{
				return A.SortKey2 < B.SortKey2;
			}

			return false;
		});

		auto GenFaceLayerContent = [&](const FString& FaceText,const FString& OwnerText,const TSharedPtr<FArmySymbol> InTemplateSymbol) {
			TSharedPtr<FArmySymbol> VSymbol = StaticCastSharedPtr<FArmySymbol>(InTemplateSymbol->CopySelf());
			VSymbol->SetText(FaceText);
			VSymbol->SetTextSize(5);
			VSymbol->SetText(OwnerText, FArmySymbol::ETT_OWNERTEXT);
			VSymbol->SetTextSize(5, FArmySymbol::ETT_OWNERTEXT);
			VSymbol->SetDrawSpliteLine(true);
			FArmySceneData::Get()->Add(VSymbol, XRArgument(1).ArgString(TEXT("DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));

			FName FaceLayerName(*(OwnerText + FaceText));
			GenerateFaceLayer(FaceLayerName);//生成立面图层
			return FaceLayerName;
		};

		int32 FaceNameId = 0;
		for (int32 i = 0; i < RoomSortArray.Num(); ++i)
		{
			const FRoomNameRangeInfo& RoomInfo = RoomSortArray[i];
			TSharedPtr<FIndexSymbolGroup> CurrentGroup = CurrentGroupMap.FindRef(RoomInfo.RoomObjPtr.Pin()->GetUniqueID());
			FArmyRoom* Room = RoomInfo.RoomObjPtr.Pin()->AsassignObj<FArmyRoom>();
			if (Room && CurrentGroup.IsValid())
			{
				int32 SymbolIndex = 0;
				if (CurrentGroup->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_TOP))
				{
					FString OwnerText = TEXT("E-");
					OwnerText += (FaceNameId >= 9 ? FString::FromInt(++FaceNameId) : TEXT("0") + FString::FromInt(++FaceNameId));
					FName FaceLayerName = GenFaceLayerContent(CharArray[SymbolIndex], OwnerText, CurrentGroup->SymbolTop);

					FPlane CurrentPlane(CurrentGroup->SymbolTopRectRegion->GetMainLine()->GetLineEditPoint()->Pos, FVector(0,-1,0));
					LocalFunction->GenerateWallFacadeLayer(FaceLayerName, StaticCastSharedPtr<FArmyRoom>(RoomInfo.RoomObjPtr.Pin()), CurrentPlane, CurrentGroup->SymbolTopRectRegion->GetBounds());

					SymbolIndex++;
				}
				if (CurrentGroup->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_RIGHT))
				{
					FString OwnerText = TEXT("E-");
					OwnerText += (FaceNameId >= 9 ? FString::FromInt(++FaceNameId) : TEXT("0") + FString::FromInt(++FaceNameId));
					FName FaceLayerName = GenFaceLayerContent(CharArray[SymbolIndex], OwnerText, CurrentGroup->SymbolRight);

					FPlane CurrentPlane(CurrentGroup->SymbolTopRectRegion->GetMainLine()->GetLineEditPoint()->Pos, FVector(1, 0, 0));
					LocalFunction->GenerateWallFacadeLayer(FaceLayerName, StaticCastSharedPtr<FArmyRoom>(RoomInfo.RoomObjPtr.Pin()), CurrentPlane, CurrentGroup->SymbolRightRectRegion->GetBounds());

					SymbolIndex++;
				}
				if (CurrentGroup->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_BOTTOM))
				{
					FString OwnerText = TEXT("E-");
					OwnerText += (FaceNameId >= 9 ? FString::FromInt(++FaceNameId) : TEXT("0") + FString::FromInt(++FaceNameId));
					FName FaceLayerName = GenFaceLayerContent(CharArray[SymbolIndex], OwnerText, CurrentGroup->SymbolBottom);

					FPlane CurrentPlane(CurrentGroup->SymbolTopRectRegion->GetMainLine()->GetLineEditPoint()->Pos, FVector(0, 1, 0));
					LocalFunction->GenerateWallFacadeLayer(FaceLayerName, StaticCastSharedPtr<FArmyRoom>(RoomInfo.RoomObjPtr.Pin()), CurrentPlane, CurrentGroup->SymbolBottomRectRegion->GetBounds());

					SymbolIndex++;
				}
				if (CurrentGroup->SymbolGroupLeadLine->GetTargetType(FArmySymbolLeadLine::EST_LEFT))
				{
					FString OwnerText = TEXT("E-");
					OwnerText += (FaceNameId >= 9 ? FString::FromInt(++FaceNameId) : TEXT("0") + FString::FromInt(++FaceNameId));
					FName FaceLayerName = GenFaceLayerContent(CharArray[SymbolIndex], OwnerText, CurrentGroup->SymbolLeft);

					FPlane CurrentPlane(CurrentGroup->SymbolTopRectRegion->GetMainLine()->GetLineEditPoint()->Pos, FVector(-1, 0, 0));
					LocalFunction->GenerateWallFacadeLayer(FaceLayerName, StaticCastSharedPtr<FArmyRoom>(RoomInfo.RoomObjPtr.Pin()), CurrentPlane, CurrentGroup->SymbolLeftRectRegion->GetBounds());

					SymbolIndex++;
				}
				if (SymbolIndex > 0)
				{
					TSharedPtr<FArmySymbolLeadLine> SymboLine = StaticCastSharedPtr<FArmySymbolLeadLine>(CurrentGroup->SymbolGroupLeadLine->CopySelf());
					SymboLine->SetEditorModel(false);
					FArmySceneData::Get()->Add(SymboLine, XRArgument(1).ArgString(TEXT("DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
				}
			}
		}

		//@马云龙 默认选中立面索引图
		LayerOutLiner->SelectItem(TEXT("立面索引图"));

		IndexSubController->EndMode();
		GGI->Window->DismissModalViewController();
	}
}
void FArmyConstructionModeController::OnGenFacadeIndex()
{
	IndexSubController->BeginMode();
	//FGuid RoomID;
	//FPlatformMisc::CreateGuid(RoomID);
	//IndexSubController->AddSymbolGroupData(RoomID,MakeShareable(new FIndexSymbolGroup(FVector(100,0,0))));

	IndexSubController->UpdateSymbol();
	GGI->Window->PresentModalViewController(IndexSubController);


#ifdef DEBUG_WALLFACELAYER
	TArray<FName> LayerNameArray;
	TArray<FObjectPtr> TempObjectArray;
	for (auto It : LayerManager->GetLayerMap())
	{
		if (It.Value->GetLayerType() == FArmyLayer::LT_D_WALLFACADE && !It.Value->IsStaticLayer() /*&& It.Key != TEXT("立面索引图")*/)
		{
			LayerNameArray.AddUnique(It.Key);
			for (auto ClassIt : It.Value->GetAllClass())
			{
				for (auto Obj : ClassIt.Value->GetAllObjects())
				{
					TempObjectArray.Add(Obj);
				}
			}
		}
	}
	for (auto& ObjPtr : TempObjectArray)
	{
		FArmySceneData::Get()->Delete(ObjPtr);
	}
	for (auto& LayerName : LayerNameArray)
	{
		DeleteLayer(LayerName);
	}

	auto GenerateFaceLayer = [&](const FName& InLayerName)
	{
		TWeakPtr<FArmyLayer> TempNewLayer = NewLayer(InLayerName, false);
		TempNewLayer.Pin()->SetLayerType(FArmyLayer::FLayerType(FArmyLayer::LT_D_WALLFACADE));

		TempNewLayer.Pin()->GetOrCreateClass(Class_BaseWall)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Door)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Pass)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Window)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Furniture)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Switch)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Socket)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ElectricBoxH)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ElectricBoxL)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Light)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_AirConditionerPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_WaterRoutePoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_HotWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_HotAndColdWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ColdWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_FloordRainPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_ClosestoolRainPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_WashBasinRainPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_RecycledWaterPoint)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_HomeEntryWaterPoint)->SetVisible(true);

		TempNewLayer.Pin()->GetOrCreateClass(Class_Beam)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Socket_H)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Socket_L)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_DiversityWater)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_WaterPipe)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Heater)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_GasMeter)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_GasPipe)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_SCTCabinet)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_SCTHardware)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Walllayout)->SetVisible(true);
		TempNewLayer.Pin()->GetOrCreateClass(Class_Other)->SetVisible(true);
	};

	FName LayerName(TEXT("立面索引图"));
	GenerateFaceLayer(LayerName);
	for (auto ObjPtr : FArmySceneData::Get()->GetObjects(E_LayoutModel))
	{
		FArmySceneData::Get()->Add(ObjPtr.Pin()->CopySelf(true), XRArgument(1).ArgString(TEXT("DYNAMICLAYER/LAYERTYPE_WALLFACADE")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));
	}
	TArray<FString> CharArray{ TEXT("A"),TEXT("B"), TEXT("C"), TEXT("D"), TEXT("E"), TEXT("F"), TEXT("G"), TEXT("H"), TEXT("I"), TEXT("J"), TEXT("K") };

	TArray<FString> RoomTemplateArray{ TEXT("客餐厅"),TEXT("客厅"),TEXT("餐厅"),TEXT("主卧"),TEXT("次卧"),TEXT("客卧"),TEXT("书房"),TEXT("儿童房"),TEXT("储物间"),TEXT("玄关"),TEXT("走廊"),TEXT("过道"),TEXT("厨房"),TEXT("卫生间"),TEXT("主卫"),TEXT("客卫"),TEXT("阳台"),TEXT("洗衣间"),TEXT("未命名") };

	struct FRoomNameRangeInfo
	{
		FRoomNameRangeInfo(FObjectWeakPtr InRoom, int32 InKey) :RoomObjPtr(InRoom), SortKey(InKey){}
		FObjectWeakPtr RoomObjPtr;
		int32 SortKey = -1;
		int32 SortKey2 = 0;

		bool operator==(const FRoomNameRangeInfo& InOther)const
		{
			return SortKey == InOther.SortKey && SortKey2 == InOther.SortKey2;
		}
	};
	TArray<FRoomNameRangeInfo> RoomSortArray;
	for (auto ObjPtr : FArmySceneData::Get()->GetObjects(E_LayoutModel))
	{
		if (ObjPtr.Pin()->GetType() == OT_InternalRoom)
		{
			FArmyRoom* Room = ObjPtr.Pin()->AsassignObj<FArmyRoom>();
			if (Room)
			{
				for (int32 i = 0; i < RoomTemplateArray.Num(); i++)
				{
					if (Room->GetSpaceName().Contains(RoomTemplateArray[i]))
					{
						//@马云龙  当RoomSpaceName为 客餐厅、客餐厅1、客餐厅2， 且RoomTemplateArray[i]为餐厅时，就会出现错误的判断，此处特意避免这种情况
						if (Room->GetSpaceName().Contains(TEXT("客餐厅")) && RoomTemplateArray[i] == TEXT("餐厅"))
							continue;

						FRoomNameRangeInfo NameInfo(ObjPtr, i);
						while (RoomSortArray.Find(NameInfo) != INDEX_NONE)
						{
							NameInfo.SortKey2++;
						}
						RoomSortArray.Add(NameInfo);
					}
				}
			}
		}
	}
	RoomSortArray.Sort([&](const FRoomNameRangeInfo& A,const FRoomNameRangeInfo& B) {
		if (A.SortKey < B.SortKey)
		{
			return true;
		}
		else if (A.SortKey == B.SortKey)
		{
			return A.SortKey2 < B.SortKey2;
		}

		return false;
	});

	int32 FaceNameId = 0;
	for (int32 i = 0;i < RoomSortArray.Num();++i)
	{
		const FRoomNameRangeInfo& RoomInfo = RoomSortArray[i];
		FArmyRoom* Room = RoomInfo.RoomObjPtr.Pin()->AsassignObj<FArmyRoom>();
		if (Room)
		{
			//获取房间的中心点
			FBox RoomBox = Room->GetBounds();
			FVector IndexPos = RoomBox.GetCenter();
			float MinDis = FLT_MAX;
			FVector FirstProjPos = FVector::ZeroVector;
			FVector SecondProjPos = FVector::ZeroVector;
			if (!Room->IsPointInRoom(IndexPos))
			{
				Room->CalPointToRoomLineMinDis(IndexPos, FirstProjPos, SecondProjPos);
				IndexPos = (FirstProjPos + SecondProjPos) / 2;
			}
			FVector BaseDir(0, -30, 0);
			for (int32 j = 0; j < 4; j++)
			{
				FString OwnerText = TEXT("E-");
				OwnerText += (FaceNameId >= 9 ? FString::FromInt(++FaceNameId) : TEXT("0") + FString::FromInt(++FaceNameId));

				FRotator R(0, j * 90, 0);
				FVector RDir = R.RotateVector(BaseDir);
				FVector CurrentPos = IndexPos + RDir;
				TSharedPtr<FArmySymbol> VSymbol = MakeShareable(new FArmySymbol);
				VSymbol->SetText(CharArray[j]);
				VSymbol->SetTextSize(5);
				VSymbol->SetText(OwnerText, FArmySymbol::ETT_OWNERTEXT);
				VSymbol->SetTextSize(5, FArmySymbol::ETT_OWNERTEXT);
				VSymbol->SetDrawSpliteLine(true);
				VSymbol->ApplyTransform(FTransform(R, CurrentPos, FVector(1, 1, 1)));
				FArmySceneData::Get()->Add(VSymbol, XRArgument(1).ArgString(TEXT("DYNAMICLAYER")).ArgFName(LayerName).ArgUint32(E_ConstructionModel));

				FName FaceLayerName(*(OwnerText + CharArray[j]));
				GenerateFaceLayer(FaceLayerName);//生成立面图层

				// FVector PlaneBasePos = IndexPos - RDir.GetSafeNormal() * RoomBox.GetSize().Size();//这里以后需要变动
				// @zengy 使用计算出的IndexPos作为观察平面的基准点
				FPlane CurrentPlane(IndexPos, RDir.GetSafeNormal());
				LocalFunction->GenerateWallFacadeLayer(FaceLayerName, StaticCastSharedPtr<FArmyRoom>(RoomInfo.RoomObjPtr.Pin()), CurrentPlane);
			}
		}
	}

	//@马云龙 默认选中立面索引图
	LayerOutLiner->SelectItem(TEXT("立面索引图"));

#endif // DEBUG_WALLFACELAYER
}

void FArmyConstructionModeController::OnExportCAD(const TArray<FName>& CADChosenList)
{
	// 如果没有选择则不执行
	if (CADChosenList.Num() < 1)
	{
		return;
	}
	//唤出windows选择目录窗口，并保存 当前选择的目录
	FString TimePath = FSCTResourceTools::GetYMDSTimeString();
	FString ZipName = TEXT("施工图") + TimePath + TEXT(".zip");
	FString CadPath = "";

	GGI->Window->DismissModalDialog();
	GGI->Window->ShowThrobber(MAKE_TEXT("CAD施工图生成中..."));
	FSlateApplication::Get().Tick();
	// 是否包含两种图框
	TMap<EConstructionFrameType, TSharedPtr<class FArmyConstructionFrame>> FrameMap;
	for (auto It : LayerFrameMap)
	{
		if (FrameMap.Contains(It.Value.Pin()->GetFrameType()))
		{
			continue;
		}
		else
		{
			FrameMap.Add(It.Value.Pin()->GetFrameType(), It.Value.Pin());
		}
	}

		// 没有图框
	if (!FrameMap.Contains(E_Frame_Default))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("缺失图框，CAD施工图生成失败！"));
		return;
	}
	if (FArmyDownloadCad::Get()->CreateCad(FrameMap, LayerManager, TimePath, CADChosenList,CadPath))
	{
		GGI->Window->HideThrobber();
		FString LastUsedPath = FWindowsDirectories::Get().GetLastDirectory(ELastWindowsDirectory::UNR);
		FString ExportFilename;

		//若支持多种解压方式/*|*.fbx|Object (*.obj)|*.obj|Unreal Text (*.t3d)|*.t3d|Stereo Litho (*.stl)|*.stl|LOD Export (*.lod.obj)|*.lod.obj"*/
		if (GreatPlanFileDialog::SaveFile(NSLOCTEXT("UnrealEd", "Export", "Export").ToString(), TEXT("ZIP (*.zip)|*.zip"), LastUsedPath, FPaths::GetBaseFilename(ZipName), ExportFilename))
		{
			if (ExportFilename.IsEmpty())
			{
				return;
			}

			//初始化SZlib压缩用
			FZipFileFunctionLibrary::Startup();
			//压缩包的格式
			EZipUtilityCompressionFormat format = COMPRESSION_FORMAT_ZIP;
			//压缩方式
			TEnumAsByte<EZipUtilityCompressionLevel> Level = COMPRESSION_LEVEL_FAST;
			
			//原路径
			FString LastArchivePath = CadPath+ TEXT(".zip");
			//设置转移路径
			DelegateInterface->SetFromTo(LastArchivePath, ExportFilename);
			// 设置待删除文件夹
			DelegateInterface->SetDirPath(CadPath);
			//压缩方法
			ZipLibrary->Zip(CadPath, DelegateInterface, format, Level);
		}
	}
	else
	{
		GGI->Window->HideThrobber();
		GGI->Window->ShowMessage(MT_Warning, TEXT("CAD施工图生成失败"));
	}
}


void FArmyConstructionModeController::OnExportCADDialog()
{

	// 判断是否有图框
	if (LayerFrameMap.Num() == 0)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请生成图框！"));
		return;
	}


	// 是否包含两种图框
	TMap<EConstructionFrameType, TSharedPtr<class FArmyConstructionFrame>> FrameMap;
	for (auto It : LayerFrameMap)
	{
		if (FrameMap.Contains(It.Value.Pin()->GetFrameType()))
		{
			continue;
		}
		else
		{
			FrameMap.Add(It.Value.Pin()->GetFrameType(), It.Value.Pin());
		}
	}

	if (!FrameMap.Contains(E_Frame_Default))
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请生成图框！"));
		return;
	}

	//if (!FrameMap.Contains(E_Frame_Cupboard))
	//{
	//	GGI->Window->ShowMessage(MT_Warning, TEXT("请生成橱柜图纸图框！"));
	//	return;
	//}


	TSharedPtr<SArmyGeneratePrint> ExportCADDialog = SNew(SArmyGeneratePrint).OnGeneratePrintStart(FOnGeneratePrintStart::CreateRaw(this, &FArmyConstructionModeController::OnExportCAD));
	if (!LayerManager.IsValid())
	{
		return;
	}

	TMap<FName, TSharedPtr<FArmyLayer>> LayersMap = LayerManager->GetLayerMap();
	TArray<FName> AllLayers;
	for (auto It : LayersMap)
	{
		// 初始化cad不包含立面图
		if (It.Value.IsValid() && It.Value->GetLayerType() != FArmyLayer::LT_D_WALLFACADE)
		{
			AllLayers.Add(It.Key);
		}
		//// 初始化包含厨房
		//if (It.Value.IsValid())
		//{
		//	AllLayers.Add(It.Key);
		//}
	}

	
	ExportCADDialog->InitContent(AllLayers);

	GGI->Window->DismissModalDialog();
	GGI->Window->PresentModalDialog(TEXT("导出CAD图纸"), ExportCADDialog.ToSharedRef());
}

void FArmyConstructionModeController::OnGeneratePrintStart(const TArray<FName>& InLayerArray)
{
	//TArray<FObjectWeakPtr> RoomList;
	//FArmySceneData::Get()->GetObjects(E_HomeModel, OT_OutRoom, RoomList);
	//for (auto It: RoomList)
	//{
	//	TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
	//	TArray< TSharedPtr<FArmyLine> > Lines;
	//	Room->GetLines(Lines);
	//	for (auto &It : Lines)
	//	{
	//		It->SetLineWidth(WALLLINEWIDTH);
	//	}
	//}
	//RoomList.Reset();
	//FArmySceneData::Get()->GetObjects(E_HomeModel, OT_InternalRoom, RoomList);
	//for (auto It : RoomList)
	//{
	//	TSharedPtr<FArmyRoom> Room = StaticCastSharedPtr<FArmyRoom>(It.Pin());
	//	TArray< TSharedPtr<FArmyLine> > Lines;
	//	Room->GetLines(Lines);
	//	for (auto &It : Lines)
	//	{
	//		It->SetLineWidth(WALLLINEWIDTH);
	//	}
	//}

	if (InLayerArray.Num() > 0)
	{
		if (LocalModelService->SetUploadNum(InLayerArray.Num()))
		{
			ShotLayerNames = InLayerArray;

			ProgressBarDialog = SNew(SArmyProgressBar).OnCanceled(FSimpleDelegate::CreateRaw(this, &FArmyConstructionModeController::OnCancelUploadImage));
			GGI->Window->DismissModalDialog();
			GGI->Window->PresentModalDialog(ProgressBarDialog.ToSharedRef());

			ProgressBarDialog->SetCurrentLabel(FText::FromName(ShotLayerNames.HeapTop()));
			ProgressBarDialog->SetCurrentText(FText::FromString(TEXT("0/") + FString::FromInt(ShotLayerNames.Num())));
			ProgressBarDialog->SetCurrentPercent(0);

			ShotCommand(true);
		}
	}
}
void FArmyConstructionModeController::OnCancelUploadImage()
{
	LocalModelService->CancelUploadImage();
	ProgressBarDialog = NULL;
	GGI->Window->DismissModalDialog();
	ShotCommand(false);
}

void FArmyConstructionModeController::ShowUpdateTitle()
{
	GVC->ViewportOverlayWidget->AddSlot()
		.Expose(UpdateOverlay)
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(0.0f,76.0f,215.0f,0.0f)
		[
			SNew(SBox)
			.WidthOverride(348.0f)
			.HeightOverride(131.0f)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
					.HeightOverride(7.0f)
					[
						SNew(SImage)
						.Image(FArmyStyle::Get().GetBrush("Icon.Triangle"))
					]
				]
				+SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SNew(SBorder)
					.Padding(0.0f)
					.BorderBackgroundColor(FArmyStyle::Get().GetColor("Color.Gray.FF2A2B2E"))
					.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						.HAlign(HAlign_Right)
						.Padding(0.0f,5.0f,5.0f,0.0f)
						[
							SNew(SBox)
							.WidthOverride(30.0f)
							.HeightOverride(30.0f)
							.Padding(0.0f)
							[
								SNew(SButton)
								.ButtonStyle(&FArmyStyle::Get().GetWidgetStyle<FButtonStyle>("BaseButtonStyle.White"))
								.ButtonColorAndOpacity(FLinearColor(FColor(0xFF2A2B2E)))
								.OnClicked(FOnClicked::CreateLambda([this]() {
									UpdateOverlay->DetachWidget();
									return FReply::Handled();
								}))
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								.ContentPadding(FMargin(6.0f, 6.0f))
								[
									//关闭
									SNew(SImage)
									.Image(FArmyStyle::Get().GetBrush("Icon.Close"))
								]
							]
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(20.0f,1.0f,0.0f,0.0f)
						[
							SNew(SHorizontalBox)
							+SHorizontalBox::Slot()
							.Padding(0.0f, 0.0f, 8.0f, 0.0f)
							.AutoWidth()
							[
								SNew(SBox)
								.WidthOverride(24.0f)
								.HeightOverride(24.0f)
								[
									SNew(SImage)
									.Image(FArmyStyle::Get().GetBrush("Icon.Update"))
									.ColorAndOpacity(FArmyStyle::Get().GetColor("Color.FFFFFFFF"))
								]
							]
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("请更新标注")))
								.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_16"))
								.ColorAndOpacity(FLinearColor(FColor(0xFFFFFFFF)))
							]
						]
						+SVerticalBox::Slot()
						.AutoHeight()
						.Padding(52.0f,12.0f,0.0f,0.0f)
						[
							SNew(STextBlock)
							.Text(FText::FromString(TEXT("设计发生变化，请点击【更新标注】更新至最新状态")))
							.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
							.ColorAndOpacity(FLinearColor(FColor(0xFFC8C9CC)))
						]
					]
				]
			]
		];
}

void FArmyConstructionModeController::MoveZipToDir(const FString& InDirPath, const FString& From, const FString& To)
{
	// 转移压缩文件
	ZipLibrary->MoveFileTo(From, To);
	// 删除源文件
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.DirectoryExists(*InDirPath))
	{
		PlatformFile.DeleteDirectoryRecursively(*InDirPath);
	}

	GGI->Window->ShowMessage(MT_Success, TEXT("导出成功！"));
}

TMap<unsigned long long, HomeModelInfo> FArmyConstructionModeController::GetAllObjectWithoutConstruct() const
{
	TMap<unsigned long long, HomeModelInfo> HomeModelInfoMap;
	for (auto LayerMap : LayerManager->GetLayerMap())
	{
		if (LayerMap.Value.IsValid())
		{
			for (auto ClassIt : LayerMap.Value->GetAllClass())
			{
				if (ClassIt.Value->IsVisible())
				{
					for (TSharedPtr<FArmyObject> Obj : ClassIt.Value->GetAllObjects())
					{
						unsigned long long PtrVal = (unsigned long long)(Obj.Get());
						if (!HomeModelInfoMap.Contains(PtrVal))
						{
							EObjectType ObjType = Obj->GetType();
							switch (ObjType)
							{
							case OT_InternalRoom:
							case OT_OutRoom:
							{
								FBox Box = Obj->GetBounds();
								HomeModelInfoMap.Add(PtrVal, HomeModelInfo(ObjType).ArgBox(Box));
								break;
							}
							case OT_Wall:
							case OT_ArcWall:
							case OT_BaseWall:
							{
								break;
							}
							case OT_Pillar:
							case OT_AirFlue:
							{
								FBox Box = Obj->GetBounds();
								HomeModelInfoMap.Add(PtrVal, HomeModelInfo(ObjType).ArgBox(Box));
								break;
							}
							case OT_Beam:
							{
								break;
							}
							case OT_ModifyWall:
							{
								break;
							}
							case OT_AddWall:
							{
								FBox Box = Obj->GetBounds();
								HomeModelInfoMap.Add(PtrVal, HomeModelInfo(ObjType).ArgBox(Box));
								break;
							}
							case OT_CurtainBox:
							{
								break;
							}
							case OT_RoomSpaceArea:
							case OT_BaseBoard:
							case OT_WaterKnifeArea:
							case OT_FreePolygonArea:// 自由多边形绘制区域
							case OT_BridgeStoneArea://过门石区域
							{
								break;
							}
							case OT_PolygonArea:
							case OT_CircleArea:
							case OT_RectArea:
							{
								FBox Box = Obj->GetBounds();
								HomeModelInfoMap.Add(PtrVal, HomeModelInfo(ObjType).ArgBox(Box));
								break;
							}
							case OT_Door:
							case OT_SlidingDoor:
							{
								break;
							}
							case OT_Window:
							case OT_FloorWindow:
							case OT_RectBayWindow:
							case OT_TrapeBayWindow:
							case OT_CornerBayWindow:
							{
								break;
							}
							case OT_SecurityDoor:
							{
								break;
							}
							case OT_Pass:
							case OT_DoorHole:
							{
								break;
							}
							case OT_Dimensions:
							{
								break;
							}
							case OT_InSideWallDimensions:
							{
								break;
							}
							case OT_OutSideWallDimensions:
							{
								break;
							}
							case OT_AddWallDimensions:
							{
								break;
							}
							case OT_DeleteWallDimensions:
							{
								break;
							}
							case OT_LampControlLine:
							{
								break;
							}
							case OT_ComponentBase:
							{
								TSharedPtr<FArmyFurniture> Furniture = StaticCastSharedPtr<FArmyFurniture>(Obj);
								switch (Furniture->BelongClass)
								{
								case Class_Switch:
								{
									ObjType = OT_Switch;
									break;
								}
								case Class_Socket:
								case Class_ElectricBoxH:
								case Class_ElectricBoxL:
								case Class_Socket_H:
								case Class_Socket_L:
								{
									ObjType = OT_Socket_Begin;
									break;
								}
								case Class_Light:
								{
									ObjType = OT_Lamp_Ceiling_Point;
									break;
								}
								case Class_HotWaterPoint://热水点位
								case Class_HotAndColdWaterPoint://冷热水点位
								case Class_ColdWaterPoint://冷水点位
								case Class_FloordRainPoint://地漏下水
								case Class_ClosestoolRainPoint://马桶下水
								case Class_WashBasinRainPoint://水盆下水
								case Class_RecycledWaterPoint://中水点位
								case Class_HomeEntryWaterPoint://进户水
								case Class_WaterRoutePoint:
								{
									ObjType = OT_Water_Point;
									break;
								}
								default:
									break;
								}
								HomeModelInfoMap.Add(PtrVal, HomeModelInfo(ObjType).ArgPosition(Obj->GetBasePos()));
								break;
							}
							case OT_StrongElectricity_25:
							case OT_StrongElectricity_4:
							case OT_StrongElectricity_Single:
							case OT_StrongElectricity_Double:
							case OT_WeakElectricity_TV:
							case OT_WeakElectricity_Net:
							case OT_WeakElectricity_Phone:
							case OT_StrongElectricity_25_Linker:
							case OT_StrongElectricity_4_Linker:
							case OT_StrongElectricity_Single_Linker:
							case OT_StrongElectricity_Double_Linker:
							case OT_WeakElectricity_TV_Linker:
							case OT_WeakElectricity_Net_Linker:
							case OT_WeakElectricity_Phone_Linker:
							{
								break;
							}
							case OT_TextLabel:
							{
								break;
							}
							case OT_ConstructionFrame:
							case OT_Compass:
							{
								break;
							}
							default:
							{
								break;
							}
							}
						}
					}
				}
			}
		}
	}

	TArray<FObjectWeakPtr> CupboardTableArray;
	FArmySceneData::Get()->GetObjects(E_WHCModel, OT_CupboardTable, CupboardTableArray);
	for (auto TableObj : CupboardTableArray)
	{
		unsigned long long PtrVal = (unsigned long long)(TableObj.Pin().Get());
		if (!HomeModelInfoMap.Contains(PtrVal))
		{
			EObjectType ObjType = TableObj.Pin()->GetType();
			switch (ObjType)
			{
			case OT_CupboardTable:
			{
				FBox Box = TableObj.Pin()->GetBounds();
				HomeModelInfoMap.Add(PtrVal, HomeModelInfo(ObjType).ArgBox(Box));
				break;
			}
			default:
				break;
			}
		}
	}
	return MoveTemp(HomeModelInfoMap);
}

TArray<EObjectType> FArmyConstructionModeController::CheckDifferentObj(TMap<unsigned long long, HomeModelInfo> LastObjMap, TMap<unsigned long long, HomeModelInfo> CurrentObjMap) const
{
	TArray<EObjectType> ObjTypeList;
	for (auto CurrentObj : CurrentObjMap)
	{
		if (LastObjMap.Contains(CurrentObj.Key))
		{
			//判断是否发生改变
			HomeModelInfo const* Info = LastObjMap.Find(CurrentObj.Key);
			if (Info != nullptr)
			{
				if (*Info == (CurrentObj.Value))
				{
				}
				else
				{
					//改变
					ObjTypeList.AddUnique(CurrentObj.Value._ArgObjType);
				}
			}
		}
		else
		{
			//增加
			ObjTypeList.AddUnique(CurrentObj.Value._ArgObjType);
		}
	}
	for (auto LastObj : LastObjMap)
	{
		if (CurrentObjMap.Contains(LastObj.Key))
		{
		}
		else
		{
			//删除
			ObjTypeList.AddUnique(LastObj.Value._ArgObjType);
		}
	}
	return MoveTemp(ObjTypeList);
}

TArray<ObjectClassType> FArmyConstructionModeController::CalcNeedChangeType(TArray<EObjectType> InObjTypeList) const
{
	//户型模式（原始和拆改）
	TMap<EObjectType, TArray<ObjectClassType>> ObjToTypesMap;
	ObjToTypesMap.Emplace(OT_OutRoom, TArray<ObjectClassType>{ Class_Other });
	ObjToTypesMap.Emplace(OT_InternalRoom, TArray<ObjectClassType>{ Class_Other });
	ObjToTypesMap.Emplace(OT_AddWall, TArray<ObjectClassType>{Class_AnnotationAddWall});

	//吊顶区域
	ObjToTypesMap.Emplace(OT_RectArea, TArray<ObjectClassType>{Class_Floorlayout, Class_Walllayout, Class_Beam, Class_AnnotationBeam, Class_AnnotationCeilingObj});
	ObjToTypesMap.Emplace(OT_CircleArea, TArray<ObjectClassType>{Class_Floorlayout, Class_Walllayout, Class_Beam, Class_AnnotationBeam, Class_AnnotationCeilingObj});
	ObjToTypesMap.Emplace(OT_PolygonArea, TArray<ObjectClassType>{Class_Floorlayout, Class_Walllayout, Class_Beam, Class_AnnotationBeam, Class_AnnotationCeilingObj});

	//构件
	ObjToTypesMap.Emplace(OT_ComponentBase, TArray<ObjectClassType>{ Class_AnnotationSwitch, Class_AnnotationSocket, Class_AnnotationElectricBoxH, Class_AnnotationElectricBoxL, Class_AnnotationAirConditionerPoint, Class_AnnotationFurniture,
		Class_AnnotationSocket_H, Class_AnnotationSocket_L, Class_AnnotationWaterPoint, Class_AnnotationSewerPipe, Class_AnnotationLamp, Class_AnnotationHeater, Class_AnnotationGas});
	ObjToTypesMap.Emplace(OT_Switch, TArray<ObjectClassType>{ Class_AnnotationSwitch});
	ObjToTypesMap.Emplace(OT_Socket_Begin, TArray<ObjectClassType>{ Class_AnnotationSocket, Class_AnnotationElectricBoxH, Class_AnnotationElectricBoxL, Class_AnnotationSocket_H, Class_AnnotationSocket_L});
	ObjToTypesMap.Emplace(OT_Water_Point, TArray<ObjectClassType>{ Class_AnnotationWaterPoint});
	ObjToTypesMap.Emplace(OT_Lamp_Ceiling_Point, TArray<ObjectClassType>{ Class_AnnotationLamp});

	ObjToTypesMap.Emplace(OT_CupboardTable, TArray<ObjectClassType>{ Class_SCTCabinet});

	TArray<ObjectClassType> ClassTypeList;
	for (EObjectType ObjectType : InObjTypeList)
	{
		if (ObjToTypesMap.Contains(ObjectType))
		{
			ClassTypeList.Append(*(ObjToTypesMap.Find(ObjectType)));
		}
	}

	return MoveTemp(ClassTypeList);
}

void FArmyConstructionModeController::ClearAutoObjectsWithClassType(TArray<ObjectClassType> ClassTypeList)
{
	if (ClassTypeList.Contains(Class_Other))
	{
		ClearAutoObjects();
		return;
	}

	TArray<EObjectType> ObjTypeList;
	for (auto & ArrayIt : LocalAutoObjectMap)
	{
		TArray<FAutoObjectInfo> ObjInfoList;
		for (auto ObjIt : ArrayIt.Value)
		{
			if (ObjIt.ObjPtr.IsValid())
			{
				if (ClassTypeList.Contains(LocalFunction->GetClassTypeByObject(ObjIt.ObjPtr.Pin(), 2)))
				{
					ObjInfoList.Emplace(ObjIt);
					FArmySceneData::Get()->Delete(ObjIt.ObjPtr.Pin());
				}
			}
		}
		for (FAutoObjectInfo ObjInfo : ObjInfoList)
		{
			ArrayIt.Value.Remove(ObjInfo);
		}
	}
}