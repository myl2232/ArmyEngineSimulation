#include "UIMixin/SArmyWHCCabUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCCabinetChangeList.h"
#include "../common/XRShapeUtilities.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Game/XRGameInstance.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTCabinetShape.h"
#include "SCTShapeManager.h"
#include "Actor/SCTShapeActor.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCCabUI::MakeWidget()
{
    TSharedPtr<SArmyWHCModeReplacingWidget> CabReplacing;
    SAssignNew(CabReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCCabUI::Callback_ReplacingCabinet)
        .OnButtonClicked_Raw(this, &FArmyWHCCabUI::Callback_ReplacingCabinet);
    CabReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush *>::Create(
        [this]() -> const FSlateBrush* {
            AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
            if (ShapeFrame != nullptr)
            {
                check(ShapeFrame->ShapeInRoomRef != nullptr);
                return FArmySlateModule::Get().WebImageCache->Download(ShapeFrame->ShapeInRoomRef->Shape->GetThumbnailUrl())->GetBrush();
            }
            else
            {
                return FCoreStyle::Get().GetDefaultBrush();
            }
        }
    ));
    CabReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
            if (ShapeFrame != nullptr)
            {
                check(ShapeFrame->ShapeInRoomRef != nullptr);
                return FText::FromString(ShapeFrame->ShapeInRoomRef->Shape->GetShapeName());
            }
            else
            {
                return FText();
            }
        }
    ));
    return MoveTemp(CabReplacing);
}

void FArmyWHCCabUI::LoadListData()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;
    LoadCabinetData();
}

FString FArmyWHCCabUI::MakeQueryUrl(int32 InType, int32 InWidth, int32 InDepth, int32 InHeight) const
{
    return FString::Printf(TEXT("design/cabinets/%d/pageList?type=%d&width=%d&height=%d&depth=%d&current=%d&size=15"), CurrentClassify, InType, InWidth, InHeight, InDepth, Container->GetPageIndexToLoad());
}

void FArmyWHCCabUI::ReplacingCabInternal(const TSharedPtr<class FSCTShape> &InTemplateShape, FShapeInRoom *InShapeInRoom, FCabinetInfo *InCabInfo, FCabinetWholeComponent *InCabWholeComponent)
{
    float OldShapeWidth = InShapeInRoom->Shape->GetShapeWidth();
    float OldShapeDepth = InShapeInRoom->Shape->GetShapeDepth();
    float OldShapeHeight = InShapeInRoom->Shape->GetShapeHeight();

    InShapeInRoom->DestroyAccessories();
    InShapeInRoom->DestroyShape();
    InShapeInRoom->DestroyPlatform();

    InShapeInRoom->Shape = FSCTShapeManager::Get()->CopyShapeToNew(InTemplateShape);
    
    // 更新柜子的标签
    InShapeInRoom->Labels = InCabInfo->Labels;

    InShapeInRoom->InitializeSelf(InCabWholeComponent->Cabinet.Get());
    InShapeInRoom->InitializeParams(InCabInfo);
    InShapeInRoom->SpawnShape();

    InShapeInRoom->Shape->SetShapeWidth(OldShapeWidth);
    InShapeInRoom->Shape->SetShapeDepth(OldShapeDepth);
    InShapeInRoom->Shape->SetShapeHeight(OldShapeHeight);

    InShapeInRoom->Shape->GetShapeActor()->SetActorRelativeLocation(FVector(-OldShapeWidth * 0.05f, 0.0f, 0.0f));

    if (InCabInfo->AccInfo.IsValid())
    {
        InShapeInRoom->InitializeAccessories(InCabInfo->AccInfo.Get(), InCabWholeComponent->Accessorys);
        InShapeInRoom->SpawnAccessories();
    }
    
    InShapeInRoom->SpawnPlatform();
    InShapeInRoom->UpdateComponents();

    // 准备柜子相关的动画
    InShapeInRoom->PrepareShapeAnimations();

    // 清空所有关于柜子的修改记录
    InShapeInRoom->ChangeList.Empty();

    // 重建放样线条
    FArmyWHCabinetMgr::Get()->OnRefreshShape(InShapeInRoom->Shape->GetShapeCategory());
}

void FArmyWHCCabUI::Callback_ReplacingCabinet()
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    FString DesireState(TEXT("柜体型号"));
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->SetCurrentState(DesireState);
    
    Container->ClearReplacingList();
    LoadCabinetData();
}

void FArmyWHCCabUI::Callback_ReplacingCabinetItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FShapeInRoom * RESTRICT ShapeInRoomRef = ShapeFrame->ShapeInRoomRef;

    TArray<int32> CabIds = Container->GetReplacingIds();

	FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(CurrentClassify);
	FCabinetInfo *CabinetInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(CabIds[InItemCode]));

    TSharedPtr<FCabinetWholeComponent> * Result = FWHCModeGlobalData::CabinetComponentMap.Find(CabIds[InItemCode]);
	check(Result != nullptr);
    TSharedPtr<FCabinetWholeComponent> & WholeComponent = *Result;

	if (CabinetInfo->IsCabinetHasValidCache())
	{
        if (Container->IsClickedSameReplacingItem(InItemCode))
            return;
          
        TSharedPtr<FSCTShape> *Result = FWHCModeGlobalData::CabinetTemplateMap.Find(CabinetInfo->Id);
        if (Result != nullptr)
        {
            ReplacingCabInternal(*Result, ShapeInRoomRef, CabinetInfo, WholeComponent.Get());
            Container->NotifyAttrPanelNeedRebuild();
            // 替换界面选中更新
            Container->ListPanel->SetSelectedItem(InItemCode);
        }
	}
	else if (CabinetInfo->CabRcState == ECabinetResourceState::CabRc_None) // 下载柜子Json数据
	{
		FString JsonFileCachePath = CabinetInfo->GetJsonFileCachePath();

		// 设置标记，用来在Tick函数中查找DownloadTask
		CabinetInfo->DownloadTaskFlag = JsonFileCachePath;
		// 更新状态为获取Json
		CabinetInfo->CabRcState = ECabinetResourceState::CabRc_FetchingJson;

		SContentItem *ContentItem = Container->GetListItem(InItemCode);
        if (ContentItem != nullptr)
		    ContentItem->SetItemState(EPending);

		// 最后添加下载任务，因为Http在另外一个线程处理，所以通过执行顺序严格控制状态同步
		TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, CabinetInfo->Id, JsonFileCachePath, CabinetInfo->JsonUrl, CabinetInfo->JsonMd5);

        // 添加柜子关联台面前沿后挡轮廓下载任务
		if (CabinetInfo->PlatformInfo.IsValid())
		{
			// 前沿轮廓
			if (CabinetInfo->PlatformInfo->FrontSections.Num() > 0)
			{
				const TSharedPtr<FTableInfo> &FrontSection = CabinetInfo->PlatformInfo->FrontSections[0];			
				WHC_ADD_DOWNLOAD_TASK(FileInfos, 
					FrontSection->Id, 
					FrontSection->GetCachedPath(),
					FrontSection->cadUrl, 
					FString());
			}
			// 后挡轮廓
			if (CabinetInfo->PlatformInfo->BackSections.Num() > 0)
			{
				const TSharedPtr<FTableInfo> &BackSection = CabinetInfo->PlatformInfo->BackSections[0];			
				WHC_ADD_DOWNLOAD_TASK(FileInfos, 
					BackSection->Id, 
					BackSection->GetCachedPath(),
					BackSection->cadUrl, 
					FString());
			}
		}

        if (WholeComponent->Cabinet.IsValid())
		{
			if (WholeComponent->Cabinet->TopResourceUrl.Len() > 0)
			{
				WHC_ADD_DOWNLOAD_TASK(FileInfos, WholeComponent->Cabinet->Id, WholeComponent->Cabinet->GetCachePath(1),
					WholeComponent->Cabinet->TopResourceUrl, WholeComponent->Cabinet->TopResourceMd5);
			}
			if (WholeComponent->Cabinet->FrontResourceUrl.Len() > 0)
			{
				WHC_ADD_DOWNLOAD_TASK(FileInfos, WholeComponent->Cabinet->Id, WholeComponent->Cabinet->GetCachePath(2),
					WholeComponent->Cabinet->FrontResourceUrl, WholeComponent->Cabinet->FrontResourceMd5);
			}
			if (WholeComponent->Cabinet->SideResourceUrl.Len() > 0)
			{
				WHC_ADD_DOWNLOAD_TASK(FileInfos, WholeComponent->Cabinet->Id, WholeComponent->Cabinet->GetCachePath(3),
					WholeComponent->Cabinet->SideResourceUrl, WholeComponent->Cabinet->SideResourceMd5);
			}
		}
		for (const auto & SubAcc : WholeComponent->Accessorys)
		{
			WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAcc->Id, SubAcc->GetCachePath(1), SubAcc->TopResourceUrl, SubAcc->TopResourceMd5);
			WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAcc->Id, SubAcc->GetCachePath(2), SubAcc->FrontResourceUrl, SubAcc->FrontResourceMd5);
			WHC_ADD_DOWNLOAD_TASK(FileInfos, SubAcc->Id, SubAcc->GetCachePath(3), SubAcc->SideResourceUrl, SubAcc->SideResourceMd5);
		}

		for (const auto & DoorSheet : WholeComponent->DoorSheets)
		{
			WHC_ADD_DOWNLOAD_TASK(FileInfos, DoorSheet->Id, DoorSheet->GetCachePath(1), DoorSheet->TopResourceUrl, DoorSheet->TopResourceMd5);
			WHC_ADD_DOWNLOAD_TASK(FileInfos, DoorSheet->Id, DoorSheet->GetCachePath(2), DoorSheet->FrontResourceUrl, DoorSheet->FrontResourceMd5);
			WHC_ADD_DOWNLOAD_TASK(FileInfos, DoorSheet->Id, DoorSheet->GetCachePath(3), DoorSheet->SideResourceUrl, DoorSheet->SideResourceMd5);
		}

        if (FileInfos.Num() == 0)
        {
            SetDownloadCabResState(false, ContentItem, CabinetInfo);
            return;
        }
		
		TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(FileInfos);
        NewTask->OnDownloadFinished.BindRaw(this, 
			&FArmyWHCCabUI::ReplacingCabinetJsonDownloadFinished, 
			JsonFileCachePath,
			CabinetInfo->Id);
	}
}

void FArmyWHCCabUI::LoadCabinetData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FShapeInRoom * RESTRICT ShapeInRoomRef = ShapeFrame->ShapeInRoomRef;

	int32 UrlType = ShapeInRoomRef->Shape->GetShapeCategory();
    float ShapeWidth = ShapeInRoomRef->Shape->GetShapeWidth();
    float ShapeDepth = ShapeInRoomRef->Shape->GetShapeDepth();
    float ShapeHeight = ShapeInRoomRef->Shape->GetShapeHeight();

    Container->RunHttpRequest(
        MakeQueryUrl(UrlType, FMath::RoundToInt(ShapeWidth), FMath::RoundToInt(ShapeHeight), FMath::RoundToInt(ShapeDepth)), 
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));

                    FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(CurrentClassify);

                    const TArray<TSharedPtr<FJsonValue>>* ShapeArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), ShapeArray))
                    {	
                        int32 nCount = (*ShapeArray).Num();
                        int32 nIndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = ShapeArray->operator[](i);
                            const TSharedPtr<FJsonObject> &ShapeJsonObject = Value->AsObject();

                            int32 Id = ShapeJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            FCabinetInfo *CabinetInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));
                            if (CabinetInfo == nullptr)
                            {
                                CabinetInfoArr.Emplace(Id,
                                    ShapeJsonObject->GetNumberField(TEXT("category")),
                                    ShapeJsonObject->GetStringField(TEXT("name")),
                                    ShapeJsonObject->GetStringField(TEXT("thumbnailUrl")),
                                    ShapeJsonObject->GetStringField(TEXT("jsonUrl")),
                                    ShapeJsonObject->GetStringField(TEXT("cabinetMd5"))
                                );
                                FCabinetInfo &NewCabInfo = CabinetInfoArr[CabinetInfoArr.Num() - 1];
                                NewCabInfo.WidthInfo.Min = ShapeJsonObject->GetNumberField(TEXT("minWidth"));
                                NewCabInfo.WidthInfo.Max = ShapeJsonObject->GetNumberField(TEXT("maxWidth"));
                                const TArray<TSharedPtr<FJsonValue>> &WidthIntervalsJsonArr = ShapeJsonObject->GetArrayField(TEXT("standardWidths"));
                                for (const auto &IntervalJson : WidthIntervalsJsonArr)
                                    NewCabInfo.WidthInfo.Intervals.Emplace(IntervalJson->AsNumber());
                                NewCabInfo.DepthInfo.Min = ShapeJsonObject->GetNumberField(TEXT("minDepth"));
                                NewCabInfo.DepthInfo.Max = ShapeJsonObject->GetNumberField(TEXT("maxDepth"));
                                const TArray<TSharedPtr<FJsonValue>> &DepthIntervalsJsonArr = ShapeJsonObject->GetArrayField(TEXT("standardDepths"));
                                for (const auto &IntervalJson : DepthIntervalsJsonArr)
                                    NewCabInfo.DepthInfo.Intervals.Emplace(IntervalJson->AsNumber());
                                NewCabInfo.HeightInfo.Min = ShapeJsonObject->GetNumberField(TEXT("minHeight"));
                                NewCabInfo.HeightInfo.Max = ShapeJsonObject->GetNumberField(TEXT("maxHeight"));
                                const TArray<TSharedPtr<FJsonValue>> &HeightIntervalsJsonArr = ShapeJsonObject->GetArrayField(TEXT("standardHeights"));
                                for (const auto &IntervalJson : HeightIntervalsJsonArr)
                                    NewCabInfo.HeightInfo.Intervals.Emplace(IntervalJson->AsNumber());
                                const TArray<TSharedPtr<FJsonValue>> &LabelsJsonArr = ShapeJsonObject->GetArrayField(TEXT("labels"));
                                for (const auto &LabelJson : LabelsJsonArr)
                                {
                                    const TSharedPtr<FJsonObject> &LabelJsonObj = LabelJson->AsObject();
                                    NewCabInfo.Labels.Emplace((ECabinetLabelType)LabelJsonObj->GetIntegerField(TEXT("id")));
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

                                FWHCModeGlobalData::CabinetComponentMap.Emplace(Id, MakeShareable(WholeComponent));
                            }
                        }
                        FillCabinetList(nIndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCCabUI::FillCabinetList(int32 InStart, int32 InCount)
{
    AXRShapeFrame *ShapeFrame = Container->CabinetOperationRef->GetShapeFrameFromSelected();
    if (ShapeFrame == nullptr)
        return;

    check(ShapeFrame->ShapeInRoomRef != nullptr);
    int32 CabId = ShapeFrame->ShapeInRoomRef->Shape->GetShapeId();

    TArray<int32> CabIds = Container->GetReplacingIds();
	const FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(CurrentClassify);

    int32 MatchingIndex = -1;
	for (int32 i = 0; i < InCount; ++i)              
	{
        const FCabinetInfo *CabinetInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(CabIds[InStart + i]));
        check(CabinetInfo != nullptr);
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(CabinetInfo->ThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(!CabinetInfo->IsCabinetHasValidCache())
			.OnClicked_Raw(this, &FArmyWHCCabUI::Callback_ReplacingCabinetItem)
			.DisplayText(FText::FromString(CabinetInfo->Name));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_Cab, CabinetInfo->Id));

		switch (CabinetInfo->CabRcState)
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
		
		Container->ListPanel->AddItem(Item); 

        if (CabId == CabinetInfo->Id)
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCCabUI::ReplacingCabinetJsonDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 Id)
{
    SContentItem *ContentItem = Container->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_Cab);

    FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(CurrentClassify);
    FCabinetInfo *CabinetInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(Id));
    check(CabinetInfo != nullptr);

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
				CabinetInfo->CabRcState = ECabinetResourceState::CabRc_FetchingRc;

				Shape->SetShapeId(CabinetInfo->Id);
				Shape->SetShapeName(CabinetInfo->Name);
				Shape->SetJsonFileUrl(CabinetInfo->JsonUrl);
				FWHCModeGlobalData::CabinetTemplateMap.Emplace(CabinetInfo->Id, MakeShareable(Shape));				


                if (ContentItem != nullptr)
                {
    				ContentItem->SetDownloadText(FText::FromString(FString::Printf(TEXT("%.2f%%"), 20.0f)));
	    			ContentItem->SetPercent(20.0f);
		    		ContentItem->SetItemState(EDownloading);
                }

				

				// 最后添加下载任务，因为Http在另外一个线程处理，所以通过执行顺序严格控制状态同步
				TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
				for (int32 i = 0; i < CurResourceUrls.Num(); ++i)
				{
                    WHC_ADD_DOWNLOAD_TASK(FileInfos, i, CurFileCachePaths[i], CurResourceUrls[i], FString());
				}
                if (FileInfos.Num() == 0)
                {
                    SetDownloadCabResState(false, ContentItem, CabinetInfo);
                    return;
                }
                // 设置标识，用来在Tick函数中查找DownloadTask
				CabinetInfo->DownloadTaskFlag = FString::Printf(TEXT("%d"), CabinetInfo->Id);

				TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(CabinetInfo->DownloadTaskFlag, FileInfos);
				NewTask->OnDownloadFinished.BindRaw(this, 
					&FArmyWHCCabUI::ReplacingCabinetResourcesDownloadFinished, 
					InJsonFileCachePath,
					CabinetInfo->Id);

				return;
			}
		}
    }

    SetDownloadCabResState(false, ContentItem, CabinetInfo);
}

void FArmyWHCCabUI::ReplacingCabinetResourcesDownloadFinished(bool bSucceeded, FString InJsonFileCachePath, int32 InShapeId)
{
    // 根据任务成功与否决定柜子最终状态
	TArray<int32> CabIds = Container->GetReplacingIds();
	FWHCModeGlobalData::TCabinetInfoArr &CabinetInfoArr = *FWHCModeGlobalData::CabinetInfoArrCollection.Find(CurrentClassify);
	FCabinetInfo *CabinetInfo = CabinetInfoArr.FindByPredicate(FCabinetInfoFunctor(InShapeId));
    check(CabinetInfo != nullptr);

	// 检查已完成下载了资源数据的柜子是否在界面列表中
    SContentItem * ContentItem = Container->GetListItemMatchingType(InShapeId, EWHCModeAttrType::EWHCAttr_Cab);

    SetDownloadCabResState(bSucceeded, ContentItem, CabinetInfo);

	if (!bSucceeded)
		FWHCModeGlobalData::CabinetTemplateMap.Remove(InShapeId);
}

void FArmyWHCCabUI::SetDownloadCabResState(bool bSucceeded, SContentItem *UIItem, FCabinetInfo *DataItem)
{
    if (UIItem != nullptr)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem != nullptr)
        DataItem->CabRcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}

FString FArmyWHCVentilatorCabUI::MakeQueryUrl(int32 InType, int32 InWidth, int32 InDepth, int32 InHeight) const
{
    return FString::Printf(TEXT("design/cabinets/%d/pageList?type=%d&width=%d&height=%d&depth=%d&labelId=4&current=%d&size=15"), GetCurrentClassify(), InType, InWidth, InHeight, InDepth, Container->GetPageIndexToLoad());
}

FString FArmyWHCBathroomCabUI::MakeQueryUrl(int32 InType, int32 InWidth, int32 InDepth, int32 InHeight) const
{
    return FString::Printf(TEXT("design/cabinets/%d/pageList?type=%d&current=%d&size=15"), GetCurrentClassify(), InType, Container->GetPageIndexToLoad());
}

void FArmyWHCBathroomCabUI::ReplacingCabInternal(const TSharedPtr<class FSCTShape> &InTemplateShape, FShapeInRoom *InShapeInRoom, FCabinetInfo *InCabInfo, FCabinetWholeComponent *InCabWholeComponent)
{
    FShapeInRoom TempShapeInRoom;
    TempShapeInRoom.RoomAttachedIndex = InShapeInRoom->RoomAttachedIndex;
    TempShapeInRoom.Shape = FSCTShapeManager::Get()->CopyShapeToNew(InTemplateShape);
    TempShapeInRoom.SpawnShapeFrame();
    TempShapeInRoom.ShapeFrame->SetActorLocation(InShapeInRoom->ShapeFrame->GetActorLocation());
    TempShapeInRoom.ShapeFrame->SetActorRotation(InShapeInRoom->ShapeFrame->GetActorRotation());

    FArmyWHCRect OldRect = InShapeInRoom->GetRect();
    float LeftDis = Container->CabinetOperationRef->GetSelectedShapeLeftMoveRange();
    LeftDis = LeftDis > 1e+20 ? 0.0f : LeftDis * 10.0f;
    float RightDis = Container->CabinetOperationRef->GetSelectedShapeRightMoveRange();
    RightDis = RightDis > 1e+20 ? 0.0f : RightDis * 10.0f;
    float SelfDis = InShapeInRoom->Shape->GetShapeWidth();

    auto AdjustShapeLocation = [](FShapeInRoom *InShapeInRoom, FCabinetInfo *InCabInfo, float LeftDis, float RightDis, float SelfDis) -> bool {
        float TotalDis = FMath::RoundToInt(LeftDis + RightDis + SelfDis);
        float NewShapeWidth = InShapeInRoom->Shape->GetShapeWidth();
        FVector Dir = InShapeInRoom->ShapeFrame->GetActorRotation().RotateVector(FVector::ForwardVector);
        if (TotalDis < NewShapeWidth)
        {
            if (InCabInfo->WidthInfo.Intervals.Num() > 0)
            {
                int32 IndexOfWidth = InCabInfo->WidthInfo.Intervals.IndexOfByPredicate(
                    [TotalDis](float InData) -> bool {
                        return !(InData > TotalDis);
                    }
                );
                if (IndexOfWidth == INDEX_NONE)
                    return false;
                NewShapeWidth = InCabInfo->WidthInfo.Intervals[IndexOfWidth];
            }
            else
            {
                if (TotalDis < InCabInfo->WidthInfo.Min || TotalDis > InCabInfo->WidthInfo.Max)
                    return false;
                NewShapeWidth = TotalDis;
            }
            InShapeInRoom->Shape->SetShapeWidth(NewShapeWidth);
        }
        float HalfNewWidth = NewShapeWidth * 0.5f;
        float HalfSelfDis = SelfDis * 0.5f;
        if (HalfNewWidth > LeftDis + HalfSelfDis)
            InShapeInRoom->ShapeFrame->SetActorLocation(InShapeInRoom->ShapeFrame->GetActorLocation() - Dir * (LeftDis + HalfSelfDis - HalfNewWidth) * 0.1f);
        else if (HalfNewWidth > RightDis + HalfSelfDis)
            InShapeInRoom->ShapeFrame->SetActorLocation(InShapeInRoom->ShapeFrame->GetActorLocation() + Dir * (RightDis + HalfSelfDis - HalfNewWidth) * 0.1f);
        return true;
    };

    if (AdjustShapeLocation(&TempShapeInRoom, InCabInfo, LeftDis, RightDis, SelfDis))
    {
        InShapeInRoom->DestroyAccessories();
        InShapeInRoom->DestroyShape();
        InShapeInRoom->DestroyPlatform();

        InShapeInRoom->Shape = TempShapeInRoom.Shape;

        // 更新柜子的标签
        InShapeInRoom->Labels = InCabInfo->Labels;

        InShapeInRoom->InitializeSelf(InCabWholeComponent->Cabinet.Get());
        InShapeInRoom->InitializeParams(InCabInfo);
        InShapeInRoom->SpawnShape();
        InShapeInRoom->ShapeFrame->RefreshFrame(
            InShapeInRoom->Shape->GetShapeWidth() * 0.1f, 
            InShapeInRoom->Shape->GetShapeDepth() * 0.1f,
            InShapeInRoom->Shape->GetShapeHeight() * 0.1f
        );
        InShapeInRoom->ShapeFrame->SetActorLocation(TempShapeInRoom.ShapeFrame->GetActorLocation());

        if (InCabInfo->AccInfo.IsValid())
        {
            InShapeInRoom->InitializeAccessories(InCabInfo->AccInfo.Get(), InCabWholeComponent->Accessorys);
            InShapeInRoom->SpawnAccessories();
        }
        
        InShapeInRoom->SpawnPlatform();
        InShapeInRoom->UpdateComponents();

        // 准备柜子相关的动画
        InShapeInRoom->PrepareShapeAnimations();

        // 清空所有关于柜子的修改记录
        InShapeInRoom->ChangeList.Empty();

        Container->CabinetOperationRef->CalculateShapeDistanceFromWall(InShapeInRoom);
        Container->CabinetOperationRef->UpdateHighlight();
    }
    
    
}