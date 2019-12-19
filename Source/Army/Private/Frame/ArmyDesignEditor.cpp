#include "ArmyDesignEditor.h"
#include "ArmyHomeModeController.h"
#include "ArmyHardModeController.h"
#include "ArmyLayoutModeController.h"
#include "ArmyConstructionModeController.h"
#include "ArmyHydropowerModeController.h"
#include "ArmyWHCModeController.h"
#include "ArmyFrameCommands.h"
#include "ArmyObject.h"
#include "ArmyEditorEngine.h"
#include "ArmyRoom.h"
#include "ArmyRoomSpaceArea.h"
#include "ArmyConstructionQuantity.h"
#include "ArmyEngineTools.h"
#include "ArmyPipeline.h"
#include "ArmyPipePoint.h"
#include "ArmyFurniture.h"

#include "ArmyResourceModule.h"
#include "JsonResourceParser.h"
#include "ArmyPlayerController.h"
#include "EngineUtils.h"
#include "Engine/Selection.h"
#include "JsonObjectConverter.h"
#include "ArmyFurnitureActor.h"
#include "ArmyPipelineActor.h"
#include "ArmyPipePointActor.h"

#include "SArmyModelContentBrowser.h"
#include "ArmyGameInstance.h"
#include "ArmyCommonTools.h"
#include "ArmyDownloadModule.h"
#include "ArmyActorConstant.h"
#include "ArmyHttpModule.h"
#include "ArmyDataTools.h"
#include "SArmySavePlan.h"
#include "SArmySaveAsPlan.h"
#include "ArmyAutoSave.h"
#include "Frame/SArmyPlanPrice.h"
using namespace FContentItemSpace;

FArmyDesignEditor::FArmyDesignEditor()
	: CurrentController(nullptr)
    , AutoSave(MakeShareable(new FArmyAutoSave))
{
}

FArmyDesignEditor::~FArmyDesignEditor()
{
	ViewControllers.Empty();
}

void FArmyDesignEditor::PreInit()
{
	
	FArmyCommonTools::InitStaticData();
	FArmyDataTools::InitStaticData();
	FArmyObject::OnGetObjectByID.BindStatic(&FArmyDesignEditor::GetObjectByGuidAlone);

	ConstructWorldOutliner();

	// 初始化视图控制器
	TSharedPtr<FArmyHomeModeController> HomeModeController = NewVC<FArmyHomeModeController>();
	ViewControllers.Add(DesignMode::HomeMode, HomeModeController);
	TSharedPtr<FArmyLayoutModeController> LayoutModeController = NewVC<FArmyLayoutModeController>();
	ViewControllers.Add(DesignMode::LayoutMode, LayoutModeController);
	TSharedPtr<FArmyHardModeController> HardModeController = NewVC<FArmyHardModeController>();
	ViewControllers.Add(DesignMode::HardMode, HardModeController);
	TSharedPtr<FArmyHydropowerModeController> HydropowerModeController = NewVC<FArmyHydropowerModeController>();
	ViewControllers.Add(DesignMode::HydropowerMode, HydropowerModeController);
	TSharedPtr<FArmyWHCModeController> WHCController = NewVC<FArmyWHCModeController>();
	ViewControllers.Add(DesignMode::WHCMode, WHCController);
	TSharedPtr<FArmyConstructionModeController> ConstrctionController = NewVC<FArmyConstructionModeController>();
	ViewControllers.Add(DesignMode::ConstrctionMode, ConstrctionController);

	OnBaseDataChangedDelegate.AddRaw(ConstrctionController.Get(), &FArmyConstructionModeController::OnBaseDataChanged);
}

void FArmyDesignEditor::Init()
{
	// 设置系统标题栏样式
	GGI->Window->GetSystemTitleBar()->SetStyle(FArmySystemTitleBarStyle(true, true, true, 24));
    GGI->Window->GetSystemTitleBar()->ShowGotoHomePageButton(true);

    SAssignNew(SavePlanWidget, SArmySavePlan);
    SAssignNew(SaveAsPlanWidget, SArmySaveAsPlan);

	FArmySceneData::Get()->Init(GVC->GetWorld());

	FArmyViewController::Init();

	//默认开启端点和辅助线捕捉
	OpenDefaultCaptureModel();

	FArmyToolsModule::Get().GetRectSelectTool()->SetSelectUnit(FArmyRectSelect::Unit_Object);

	for (int32 ObjType = OT_None; ObjType < OT_TheEnd; ++ObjType)
	{
		FArmyToolsModule::Get().GetRectSelectTool()->AddFiltType(EObjectType(ObjType));
	}

	Reload();
}
void FArmyDesignEditor::OnBaseDataChanged(int32 InDataType)
{
	OnBaseDataChangedDelegate.Broadcast(InDataType);
}
FObjectWeakPtr FArmyDesignEditor::GetObjectByGuidAlone(const FGuid& InGuid)
{
	return FArmySceneData::Get()->GetObjectByGuidAlone(InGuid);
}
TSharedPtr<SWidget> FArmyDesignEditor::MakeCustomTitleBarWidget()
{
	return
		SAssignNew(DesignTitleBar, SArmyDesignTitleBar)
		.OwnerEditorPtr(this->AsShared());
}

TSharedPtr<SWidget> FArmyDesignEditor::MakeContentWidget()
{
	return SAssignNew(DesignFrame, SArmyDesignFrame);
}

void FArmyDesignEditor::Draw(const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	FArmySceneData::Get()->DrawGlobalObject(PDI, View);

	if (CurrentController.IsValid())
	{
		CurrentController->Draw(View, PDI);
	}
}

void FArmyDesignEditor::Draw(FViewport* InViewport, const FSceneView* View, FCanvas* SceneCanvas)
{
	if (CurrentController.IsValid())
	{
		CurrentController->Draw(InViewport, View, SceneCanvas);
	}
}

bool FArmyDesignEditor::InputKey(FViewport* Viewport, FKey Key, EInputEvent Event)
{
	if (CurrentController.IsValid())
	{
		return CurrentController->InputKey(Viewport, Key, Event);
	}

	return false;
}

bool FArmyDesignEditor::InputAxis(FViewport* Viewport, int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	if (CurrentController.IsValid())
	{
		return CurrentController->InputAxis(Viewport, ControllerId, Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	return false;
}

void FArmyDesignEditor::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	if (CurrentController.IsValid())
	{
		CurrentController->ProcessClick(View, HitProxy, Key, Event, HitX, HitY);
	}
}

void FArmyDesignEditor::Tick(float DeltaSeconds)
{
	if (CurrentController.IsValid())
	{
		CurrentController->Tick(DeltaSeconds);
	}
}

void FArmyDesignEditor::MouseMove(FViewport* Viewport, int32 X, int32 Y)
{
	if (CurrentController.IsValid())
	{
		CurrentController->MouseMove(Viewport, X, Y);
	}
}

bool FArmyDesignEditor::MouseDrag(FViewport* Viewport, FKey Key)
{
	if (CurrentController.IsValid())
	{
		return CurrentController->MouseDrag(Viewport, Key);
	}

	return true;
}

TSharedPtr<FUICommandList> FArmyDesignEditor::GetCommandList()
{
	if (CurrentController.IsValid())
	{
		return CurrentController->GetCommandList();
	}

	return nullptr;
}

void FArmyDesignEditor::Reload()
{
	bReloading = true;

	GGI->Window->DismissAllModalViewControllers();
	WorldOutliner->Refresh(true);

    FArmyActorLabel::ResetIndexes();

	// 设置户型模式为默认模式
	if (DesignTitleBar.IsValid())
	{
		DesignTitleBar.Pin()->InitDesignModeList();
	}

	// 解决切换方案崩溃的bug
	//GGI->ResetTransactions();

    AutoSave->StartTimer();

	bReloading = false;
}

void FArmyDesignEditor::ClearAll()
{
	for (auto& It : ViewControllers)
	{
		It.Value->Clear();
		if (It.Value->TransMgr.IsValid())
		{
			It.Value->TransMgr->Reset();
		}
	}
}

const TArray< TSharedPtr<FString> > FArmyDesignEditor::GetModeNameArray()
{
	TArray< TSharedPtr<FString> > Results;

	TArray<FString> ModeNameArray;
	ViewControllers.GetKeys(ModeNameArray);

	for (auto& It : ModeNameArray)
	{
		Results.Add(MakeShareable(new FString(It)));
	}

	return Results;
}

bool FArmyDesignEditor::SetCurrentDesignMode(const FString& ModeName)
{
	/**@欧石楠 记录模式名称*/
	CheckModeString = ModeName;

	// 结束当前模式
	if (CurrentController.IsValid())
	{

		if (!CurrentController->EndMode())
		{
			return false;
		}
		else
		{
			/**@欧石楠重置视图模式到透视图*/
			if (GXRPC) {
				GXRPC->SetXRViewMode(EXRView_FPS);
			}
		}

		if (CurrentController == *ViewControllers.Find(DesignMode::LayoutMode) && ModeName.Equals(DesignMode::ConstrctionMode))
		{
			if (FArmySceneData::Get()->bIsDisplayDismantle == true)
			{
				FArmySceneData::Get()->bIsDisplayDismantle = false;
				if (FArmySceneData::Get()->bUpdateModifyModeData)
				{
					FArmySceneData::Get()->CopyModeData(E_ModifyModel, E_LayoutModel, true);
					FArmySceneData::Get()->bUpdateModifyModeData = false;
					FArmySceneData::Get()->bUpdateLayoutModeData = true;
				}
			}
		}

		if (CurrentController == *ViewControllers.Find(DesignMode::HomeMode) && ModeName.Equals(DesignMode::LayoutMode))
		{
			OnCopyHomeDataDelegate.ExecuteIfBound();
		}

		if ((CurrentController == *ViewControllers.Find(DesignMode::HomeMode) && !ModeName.Equals(DesignMode::LayoutMode)) ||
			(CurrentController == *ViewControllers.Find(DesignMode::LayoutMode) && !ModeName.Equals(DesignMode::HomeMode)))
		{
			OnCreateHomeModelDelegate.ExecuteIfBound();
		}
	}

	CurrentController = *ViewControllers.Find(ModeName);
	if (CurrentController.IsValid())
	{
		//在切换模式前取消已选中的物体
		FArmyToolsModule::Get().GetRectSelectTool()->Clear();

		//在切换模式前取消已选中的物体
		GXREditor->SelectNone(true, true);

		/** @欧石楠 记录当前模式名称*/
		CurrentModeString = ModeName;

		// 显示当前模式界面
		ContentWidget = CurrentController->ContentWidget;
		GGI->Window->SetWindowContent(ContentWidget);

		// 开始新模式
		CurrentController->BeginMode();

		// 设置撤销重做管理器
		TransMgr = CurrentController->TransMgr;

		// 切换模式后重新聚焦
		FSlateApplication::Get().SetAllUserFocusToGameViewport();
		FSlateApplication::Get().ClearKeyboardFocus();

		return true;
	}
	return false;
}

//template<class ControllerType>
//TSharedPtr<ControllerType> FArmyDesignEditor::GetModeController(FString InModeName)
//{
//	for (auto& It : ViewControllers)
//	{
//		if (It.Key== InModeName)
//		{
//			return StaticCastSharedPtr<ControllerType>(It.Value);
//		}
//	}
//
//	return nullptr;
//}

bool FArmyDesignEditor::CollectPlanData(TArray<uint8>& OutHomeData, TArray<uint8>& OutPlanData)
{
    // 方案数据
	FString PlanJStr;
	TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > PlanJsonWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&PlanJStr);
	SaveFileList.Empty();
	// 开始写入方案数据
	PlanJsonWriter->WriteObjectStart();

    // 写入文件版本号
    PlanJsonWriter->WriteValue("Version", *FArmyUser::Get().GetApplicationVersion());

	bool bResult = false;

	// 收集需要保存的数据
	for (auto& It : ViewControllers)
	{
		if (It.Key.Equals(DesignMode::HomeMode))
		{
			// 户型数据
			FString HomeJStr;
			TSharedRef< TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > > HomeJsonWriter = TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> >::Create(&HomeJStr);

			// 开始写入户型数据
			HomeJsonWriter->WriteObjectStart();

            // 写入文件版本号
            HomeJsonWriter->WriteValue("Version", *FArmyUser::Get().GetApplicationVersion());

			bResult = It.Value->Save(HomeJsonWriter);

			// 结束写入户型数据
			HomeJsonWriter->WriteObjectEnd();
			HomeJsonWriter->Close();

			// 如果保存失败则返回
			if (!bResult)
			{
				return false;
			}

			// 写入流
			FTCHARToUTF8 Converter(*HomeJStr);
			OutHomeData.SetNum(Converter.Length());
			FMemory::Memcpy(OutHomeData.GetData(), Converter.Get(), Converter.Length());
		}
		else
		{
			bResult = It.Value->Save(PlanJsonWriter);
		}
	}

	// 需要下载的文件ID列表
	PlanJsonWriter->WriteArrayStart(TEXT("fileList"));
	if (SaveFileList.Num() > 0)
	{
		//写入模型文件ID列表
		PlanJsonWriter->WriteObjectStart();
		PlanJsonWriter->WriteValue(TEXT("resourceType"), 2);
		PlanJsonWriter->WriteArrayStart("ids");
        for (auto& It : SaveFileList)
        {
            if (It >= 0)
            {
                PlanJsonWriter->WriteValue(It);
            }
        }
		PlanJsonWriter->WriteArrayEnd();
		PlanJsonWriter->WriteObjectEnd();
	}
	PlanJsonWriter->WriteArrayEnd();

	// 结束写入方案数据
	PlanJsonWriter->WriteObjectEnd();
	PlanJsonWriter->Close();

	// 如果保存失败则返回
	if (!bResult)
	{
		return false;
	}

	FTCHARToUTF8 Converter(*PlanJStr);
	OutPlanData.SetNum(Converter.Length());
	FMemory::Memcpy(OutPlanData.GetData(), Converter.Get(), Converter.Length());

	return true;
}

void FArmyDesignEditor::SavePlan()
{
    // @欧石楠 如果户型缩略图没有截取，有可能是在户型模式下保存且没有切换过模式，需要自动截取一下
    if (CurrentController->GetDesignModelType() == E_HomeModel)
    {
        SavePlanWidget->PreReqSavePlan(true);
    }
    else
    {
        SavePlanWidget->PreReqSavePlan(false);
    }
    AutoSave->StartTimer();
}

void FArmyDesignEditor::SaveAsPlan()
{
    FSimpleDelegate OnSaveAsPlan;
    OnSaveAsPlan.BindLambda([this]() {
        SaveAsPlanWidget->PreReqSavePlan();
        AutoSave->StartTimer();
    });

    SaveAsPlanWidget->TakePlanScreentshot();
    GGI->Window->PresentModalDialog(TEXT("方案另存为"), SaveAsPlanWidget->AsShared(), OnSaveAsPlan, false);
}

void FArmyDesignEditor::LoadHome(const FString& FilePath)
{
    // @欧石楠 先清空户型
	FArmyDesignModeControllerPtr HomeVC = *ViewControllers.Find(DesignMode::HomeMode);
    HomeVC->Clear();

    if (FilePath.IsEmpty())
    {
        return;
    }

    // 从文件流中载入数据
    FString Content;
    FFileHelper::LoadFileToString(Content, *FilePath);

	// 解析数据
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Content), JsonObject))
	{
		/** @马云龙 解析Json中的Version字符串，并转为int，比如 1.3.0 -> 130 */
		FString JsonVersion;
		if (JsonObject->TryGetStringField("Version", JsonVersion))
			FArmyUser::Get().CurHomeVersion = FArmyCommonTools::ConvertVersionStrToInt(JsonVersion);

		HomeVC->Load(JsonObject);
	}
	else
	{
		HomeVC->Load(nullptr);
	}
}

void FArmyDesignEditor::LoadPlan(const FString& FilePath)
{
    // 先清空所有
    for (auto& It : ViewControllers)
    {
        if (!It.Key.Equals(DesignMode::HomeMode))
        {
            It.Value->Clear();
        }
    }

    if (FilePath.IsEmpty())
    {
        return;
    }

	// 从文件流中载入数据
	FString Content;
	FFileHelper::LoadFileToString(Content, *FilePath);

	// 解析数据
	TSharedPtr<FJsonObject> JsonObject;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Content), JsonObject))
	{
		/** @马云龙 解析Json中的Version字符串，并转为int，比如 1.3.0 -> 130 */
		FString JsonVersion;
		if (JsonObject->TryGetStringField("Version", JsonVersion))
			FArmyUser::Get().CurPlanVersion = FArmyCommonTools::ConvertVersionStrToInt(JsonVersion);

		for (auto& It : ViewControllers)
		{
			if (!It.Key.Equals(DesignMode::HomeMode))
			{
				It.Value->Load(JsonObject);
			}
		}
	}
	else
	{
		for (auto& It : ViewControllers)
		{
			if (!It.Key.Equals(DesignMode::HomeMode))
			{
				It.Value->Load(nullptr);
			}
		}
	}

	//@打扮家 XRLightmass 临时 加载方案不生成任何窗户
	// 创建3D模型
	FArmySceneData::Get()->GenerateHardwareModel();
	FArmySceneData::Get()->GeneratePointPositionModel();

	Reload();

	/** @马云龙 场景载入完毕的代理 */
	AllControllersLoadCompleted.Broadcast();
}

void FArmyDesignEditor::ReqSyncPlan(const FString& FilePath)
{
    NeedToUpdateFiles.Reset();

    TArray<int32> FileListArray;

	// 从文件流中载入数据
	FString Content;
	FFileHelper::LoadFileToString(Content, *FilePath);

    TSharedPtr<FJsonObject> JsonObject;
    if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(Content), JsonObject))
    {
        TArray< TSharedPtr<FJsonValue> > FileListArrayData = JsonObject->GetArrayField("fileList");
        for (auto It : FileListArrayData)
        {
            TSharedPtr<FJsonObject> FileObj = It->AsObject();
            TArray< TSharedPtr<FJsonValue> > IdsArrayData = FileObj->GetArrayField("ids");
            for (auto It : IdsArrayData)
            {
                FileListArray.Add(It->AsNumber());
            }
        }
    }

    if (FileListArray.Num() > 0)
    {
        IdsArray.Reset();
        IdsArray.AddDefaulted(FileListArray.Num() / 200 + 1);
        int32 Index = 0;
        for (int32 i = 0; i < FileListArray.Num(); ++i)
        {
            IdsArray[Index].Append(FString::FromInt(FileListArray[i]));
            if (i == 0 || i % 200 != 0)
            {
                if (i < FileListArray.Num() - 1)
                {
                    IdsArray[Index].Append(",");
                }
            }
            else
            {
                ++Index;
            }
        }

        for (int32 i = 0; i < IdsArray.Num(); ++i)
        {
            FString Ids = IdsArray[i];

            if (!Ids.IsEmpty())
            {
                if (i == 0)
                {
                    TAttribute<FText> SyncProgressTextAttr = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FArmyDesignEditor::GetSyncProgressText));
                    GGI->Window->ShowThrobber(SyncProgressTextAttr);
                }

                FArmyHttpRequestCompleteDelegate CompleteDelegate;
                CompleteDelegate.BindRaw(this, &FArmyDesignEditor::ResSyncPlan, Ids);
                FString Url = FString::Printf(TEXT("/api/plans/load?ids=%s"), *Ids);
                IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(Url, CompleteDelegate);
                Request->ProcessRequest();
            }
        }
    }
    else
    {
        OnSyncFinished.ExecuteIfBound(true);
    }
}

void FArmyDesignEditor::ResSyncPlan(FArmyHttpResponse Response, FString InIds)
{
    IdsArray.Remove(InIds);

    if (Response.bWasSuccessful)
    {
        const TArray< TSharedPtr<FJsonValue> > JArray = Response.Data->GetArrayField("data");

        // 解析返回的列表
        for (auto& It : JArray)
        {
            const TSharedPtr<FJsonObject> JObject2 = It->AsObject()->GetObjectField("data");

            bool IsHydropower = false;
            const TArray<TSharedPtr<FJsonValue>>* VArray = nullptr;
            if (JObject2->TryGetArrayField("goodsClassify", VArray))
            {
                for (int32 i = 0; i < VArray->Num(); ++i)
                {
                    if ((*VArray)[i]->AsNumber() == 2)
                    {
                        IsHydropower = true;
                        break;
                    }
                }
            }

            if (IsHydropower)
            {
                FContentItemPtr Item = FArmyCommonTools::ParseHydropowerContentItemFromJson(JObject2);
                if (Item.IsValid())
                {
                    Item->ModeIdent = MI_HydropowerMode;
                    SynContentItems.Add(Item);
                }
            }
            else
            {
                FContentItemPtr Item = FArmyCommonTools::ParseHardModeContemItemFromJson(JObject2);
                if (Item.IsValid())
                {
                    Item->ModeIdent = MI_SoftHardMode;
                    SynContentItems.Add(Item);
                }
            }
        }

        for (auto&It : SynContentItems)
        {
            if (!It.IsValid())
                continue;
            TArray< TSharedPtr<FResObj> >resArr = It->GetResObjNoComponent();
            for (auto FileIt : resArr)
            {
                // 如果NetMD5或者URL为空，有可能是下架产品，不再作为下载对象
                if (FileIt->FileMD5 == TEXT("") || FileIt->FileURL == TEXT(""))
                {
                    continue;
                }

                EFileExistenceState FileState = FArmyResourceModule::Get().GetResourceManager()->CheckFileExistState(FileIt->FilePath, FileIt->FileMD5);
                if (FileState == EFileExistenceState::Complete || FileState == EFileExistenceState::PendingDownload)
                {
                    continue;
                }

                NeedToUpdateFiles.Add(FDownloadSpace::FDownloadFileInfo(It->ID, FileIt->FilePath, FileIt->FileURL, FileIt->FileMD5));
            }
        }

        // 创建下载任务
        if (NeedToUpdateFiles.Num() > 0)
        {
            if (IdsArray.Num() == 0)
            {
                SyncTasks = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(NeedToUpdateFiles);
                SyncTasks->OnDownloadFinished = OnSyncFinished;
            }
        }
        else
        {
            OnSyncFinished.ExecuteIfBound(true);
        }
    }
    else
    {
        if (Response.Message.IsEmpty())
        {
            GGI->Window->ShowMessage(MT_Warning, TEXT("方案同步失败"));
        }
        else
        {
            GGI->Window->ShowMessage(MT_Warning, Response.Message);
        }
        
        GGI->Window->HideThrobber();
    }
}


void FArmyDesignEditor::OnShowPlanPrice()
{
	FOnUploadComplete OnUploaded;
	OnUploaded.BindLambda([this](TSharedPtr<FAliyunOssResponse> Response)
		{
			if (Response->bWasSuccessful)
			{
				//发送方案报价请求
				FArmyHttpRequestCompleteDelegate CompleteDelegate;
				CompleteDelegate.BindRaw(this, &FArmyDesignEditor::OnPlanPriceResponsed);
				FString url= FString::Printf(TEXT("/api/dosage-lists/valuation?planId=%d&&dataUrl=%s"), FArmyUser::Get().GetCurPlanID(),* Response->CdnUrl);
				IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJGetRequest(url, CompleteDelegate);
				Request->ProcessRequest();
				return;
			}
		}
	);
	ExportConstructionListWithoutSave(OnUploaded);

}
void FArmyDesignEditor::OnPlanPriceResponsed(FArmyHttpResponse Response)
{
	if (Response.bWasSuccessful)
	{
		 
		TSharedPtr<FArmyPlanPrice> P = MakeShared<FArmyPlanPrice>();// = nullptr;// = new FArmyPlanPrice();
		auto Data = Response.Data->GetObjectField(TEXT("data"));
		FJsonObjectConverter::JsonObjectToUStruct<FArmyPlanPrice>(Data.ToSharedRef(),P.Get());
 		if (!PlanPriceUI.IsValid())
		{
			PlanPriceUI = SNew(SArmyPlanPrice);
			PlanPriceUI->OnShowDetail.BindRaw(this, &FArmyDesignEditor::ExportConstructionList);
		}
		PlanPriceUI->SetDataSource(P);
		PlanPriceUI->Show();
	}
	//else
	//{
	//	if (!PlanPriceUI.IsValid())
	//	{
	//		PlanPriceUI = SNew(SArmyPlanPrice);
	//		GGI->WindowOverlay->AddSlot()
	//			.HAlign(HAlign_Center)
	//			.VAlign(VAlign_Center)
	//			//.Padding(0, 550, 242, 0)
	//			[
	//				PlanPriceUI.ToSharedRef()
	//			];
	//	}
	//	PlanPriceUI->SetVisibility(EVisibility::Visible);
	//}

}

void FArmyDesignEditor::ExportConstructionList()
{
	bool bNeedSavePlan = false;

	if (FArmyUser::Get().CurHomeData.IsValid())
	{
		TArray< TSharedPtr<FResObj> > ResArr = FArmyUser::Get().CurHomeData->GetResObjNoComponent();
		if (ResArr.Num() > 0 && !ResArr[0].IsValid())
		{
			bNeedSavePlan = true;
		}
	}
	else
	{
		bNeedSavePlan = true;
	}

	if (bNeedSavePlan)
	{
		GGI->Window->ShowMessage(MT_Warning, TEXT("请先保存户型!"));
		return;
	}

	// 导出清单前先保存
	FArmyFrameCommands::OnMenuSave();

	FArmyConstructionQuantity ConstructionQuantity;
	FArmySceneData::Get()->CollectConstructionQuantity(ConstructionQuantity);

	FString JStr;
	FJsonObjectConverter::UStructToJsonObjectString(ConstructionQuantity, JStr);
	JStr.ReplaceInline(TEXT("iD"), TEXT("id"), ESearchCase::CaseSensitive);


	//@郭子阳
	//查看输出清单
	UE_LOG(LogTemp,Warning,TEXT("导出清单： %s"),*JStr)


	TArray<uint8> CQData;
	FTCHARToUTF8 Converter(*JStr);
	CQData.SetNum(Converter.Length());
	FMemory::Memcpy(CQData.GetData(), Converter.Get(), Converter.Length());

	// 上传施工算量json文件
	ConstructionQuantityFileName = FString::Printf(TEXT("temp/%s/%s.json"), *FArmyEngineTools::GetYMTimeString(), *FArmyEngineTools::GetTimeStampString());

	FOnUploadComplete Delegate_UploadConstructionQuantityFile;
	Delegate_UploadConstructionQuantityFile.BindRaw(this, &FArmyDesignEditor::ResUploadConstructionQuantityFile);
	FAliyunOss::Get().PutObject(EOssDir::OD_Resource, ConstructionQuantityFileName, CQData, Delegate_UploadConstructionQuantityFile);

	GGI->Window->ShowThrobber(MAKE_TEXT("生成施工算量..."));
}

void FArmyDesignEditor::ExportConstructionListWithoutSave(FOnUploadComplete OnUploaded)
{
	FArmyConstructionQuantity ConstructionQuantity;
	FArmySceneData::Get()->CollectConstructionQuantity(ConstructionQuantity);

	FString JStr;
	FJsonObjectConverter::UStructToJsonObjectString(ConstructionQuantity, JStr);
	JStr.ReplaceInline(TEXT("iD"), TEXT("id"), ESearchCase::CaseSensitive);


	TArray<uint8> CQData;
	FTCHARToUTF8 Converter(*JStr);
	CQData.SetNum(Converter.Length());
	FMemory::Memcpy(CQData.GetData(), Converter.Get(), Converter.Length());

	// 上传施工算量json文件
	ConstructionQuantityFileName = FString::Printf(TEXT("temp/%s/%s.json"), *FArmyEngineTools::GetYMTimeString(), *FArmyEngineTools::GetTimeStampString());

	//FOnUploadComplete Delegate_UploadConstructionQuantityFile;
	//OnUploaded.BindRaw(this, &FArmyDesignEditor::ResUploadConstructionQuantityFile);
	FAliyunOss::Get().PutObject(EOssDir::OD_Resource, ConstructionQuantityFileName, CQData, OnUploaded);
}


void FArmyDesignEditor::OpenDefaultCaptureModel()
{
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_VertexCaptrue;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Auxiliary;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Midpoint;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_IntersectionPoint;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Perpendicular;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Closest;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_Ortho;
	FArmyToolsModule::Get().GetMouseCaptureTool()->CaptureModel |= FArmyMouseCapture::Model_LineRange;
}

void FArmyDesignEditor::ConstructWorldOutliner()
{
	SArmyModelContentBrowser::RequestObjectListGroup();
	FArmyInitializationOptions InitOptions;
	{
		InitOptions.RepresentingWorld = GVC->GetWorld();
		InitOptions.RepresentingClient = GVC;
	}
	WorldOutliner = FArmySceneOutlinerModule::Get().CreateSceneOutliner(InitOptions);
}

void FArmyDesignEditor::ResUploadConstructionQuantityFile(TSharedPtr<FAliyunOssResponse> Response)
{
	if (Response->bWasSuccessful)
	{
		ReqSubmitConstructionQuantity(Response->CdnUrl);
		return;
	}

	GGI->Window->HideThrobber();
	GGI->Window->ShowMessage(MT_Warning, TEXT("生成失败！"));
}

void FArmyDesignEditor::ReqSubmitConstructionQuantity(const FString& CQFileUrl)
{
	FString JStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&JStr);

	JsonWriter->WriteObjectStart();
    JsonWriter->WriteValue("planId", FString::FromInt(FArmyUser::Get().GetCurPlanID()));
	JsonWriter->WriteValue("dataUrl", *CQFileUrl);
	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

    FArmyHttpRequestCompleteDelegate CompleteDelegate;
    CompleteDelegate.BindRaw(this, &FArmyDesignEditor::ResSubmitConstructionQuantity);
    IArmyHttpRequestPtr Request = FArmyHttpModule::Get().CreateDBJPostRequest("/api/dosage-lists", CompleteDelegate, JStr);
    
	

	Request->ProcessRequest();
}

void FArmyDesignEditor::ResSubmitConstructionQuantity(FArmyHttpResponse Response)
{
	GGI->Window->HideThrobber();

    if (Response.bWasSuccessful)
    {
        // 显示施工算量清单
        FString Url = FString::Printf(TEXT("%s/?token=%s#/personal/myProj/%d"), *FArmyHttpModule::Get().GetBackendUrl(), *FArmyHttpModule::Get().GetToken(), FArmyUser::Get().GetCurProjectID());
        FPlatformProcess::LaunchURL(*Url, NULL, NULL);
        GGI->Window->ShowMessage(MT_Success, TEXT("导出施工算量成功"));
    }
    else
    {
        GGI->Window->ShowMessage(MT_Warning, TEXT("导出施工算量失败"));
    }
}

void FArmyDesignEditor::LoadCommonModel(const FString& PakFileName, const FString& JsonFileName, const int32 ItemId)
{
	FContentItemPtr DoorItem = MakeShareable(new FContentItem());
	DoorItem->ID = ItemId;
	DoorItem->ResourceType = EResourceType::BlueprintClass;
	TSharedPtr<FContentItemSpace::FModelRes> DoorModelRes = MakeShareable(new FContentItemSpace::FModelRes());
	DoorModelRes->FileName = PakFileName;
	DoorModelRes->FilePath = FArmyCommonTools::GetCommonDir() + PakFileName;

	// 解析优化参数
	FString OptimizeParamJsonPath = FArmyCommonTools::GetCommonDir() + JsonFileName;
	FString OptimizeParam;
	FFileHelper::LoadFileToString(OptimizeParam, *OptimizeParamJsonPath);
	TSharedPtr<FJsonObject> JOptimizeParam;
	if (FJsonSerializer::Deserialize(TJsonReaderFactory<TCHAR>::Create(OptimizeParam), JOptimizeParam))
	{
		DoorModelRes->MaterialParameter = JOptimizeParam->GetStringField("materialParameter");
		DoorModelRes->LightParameter = JOptimizeParam->GetStringField("lightParameter");
	}

	DoorItem->ResObjArr.Add(DoorModelRes);

	SynContentItems.Add(DoorItem);
}

void FArmyDesignEditor::LoadCommonResource()
{
	LoadCommonModel("SingleDoor_Model.pak", "SingleDoor_OptimizeParam.json", -2);
	LoadCommonModel("SlidingDoor_Model.pak", "SlidingDoor_OptimizeParam.json", -1);
}

void FArmyDesignEditor::TakeHomeScreenshot()
{
    if (SavePlanWidget.IsValid())
    {
        SavePlanWidget->TakeHomeScreenshot();
    }
    if (SaveAsPlanWidget.IsValid())
    {
        SaveAsPlanWidget->TakeHomeScreenshot();
    }
}

void FArmyDesignEditor::Quit()
{
    AutoSave->StopTimer();
    AlreadyInited = false;

	for (auto& It : ViewControllers)
	{
		It.Value->Quit();
	}
}

void FArmyDesignEditor::UpdateSyncProgress()
{
	if (SyncTasks.IsValid())
	{
		SyncProgress = SyncTasks->GetProgress();
	}
}
