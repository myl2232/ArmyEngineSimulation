#include "UIMixin/SArmyWHCCabDoorUI.h"
#include "UIMixin/SArmyWHCModificationRangeUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCShapeDoorChangeList.h"
#include "WHCMode/XRWHCCabinetChangeList.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "../common/XRShapeUtilities.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Actor/XRActorConstant.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTAnimation.h"
#include "SCTCabinetShape.h"
#include "SCTDoorGroup.h"
#include "SCTSlidingDrawerShape.h"
#include "SCTShapeManager.h"
#include "Actor/SCTShapeActor.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

FWHCModeGlobalData::TCoverDoorTypeMap* GetDoorTypeMap(int32 InDoorType)
{
    switch (InDoorType)
    {
        case 1:
            return &FWHCModeGlobalData::SlidingDoorTypeMap;
        case 2:
            return &FWHCModeGlobalData::CoverDoorTypeMap;
        case 3:
            return &FWHCModeGlobalData::DrawerTypeMap;
        default:
            check(0);
    }
    return nullptr;
}

TSharedPtr<SWidget> FArmyWHCCabDoorUIBase::MakeWidget()
{
    TSharedPtr<SArmyWHCModeReplacingWidget> CabDoorReplacing;
    SAssignNew(CabDoorReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCCabDoorUIBase::Callback_ReplacingDoor)
        .OnButtonClicked_Raw(this, &FArmyWHCCabDoorUIBase::Callback_ReplacingDoor);
    CabDoorReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this]() -> const FSlateBrush* {
            if (IsValidSelection())
            {
                AXRShapeFrame *ShapeFrame = GetShapeFrame();
                if (ShapeFrame != nullptr)
                {
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
                    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
                    if (DoorGroup)
                        return FArmySlateModule::Get().WebImageCache->Download(DoorGroup->GetThumbnailUrl())->GetBrush();
                }
                return FCoreStyle::Get().GetDefaultBrush();
            }
            else
                return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    CabDoorReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            if (IsValidSelection())
            {
                AXRShapeFrame *ShapeFrame = GetShapeFrame();
                if (ShapeFrame != nullptr)
                {
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
                    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
                    if (DoorGroup)
                        return FText::FromString(DoorGroup->GetShapeName());
                }
                return FText();
            }
            else
            {
                return FText();
            }
        }
    ));
    SAssignNew(CabDoorCategoryComboBox, SArmyWHCModeNormalComboBox)
        .OptionSource(TArray<TSharedPtr<FString>> { MakeShareable(new FString(TEXT("全部"))) })
        .Value(TEXT("全部"))
        .OnValueChanged_Raw(this, &FArmyWHCCabDoorUIBase::Callback_DoorCategoryChanged);
    return MoveTemp(CabDoorReplacing);
}

void FArmyWHCCabDoorUIBase::LoadListData()
{
    if (!IsValidSelection())
        return;
    LoadDoorData();
}

void FArmyWHCCabDoorUIBase::Callback_ReplacingDoor()
{
    if (!IsValidSelection())
        return;

    FString DesireState(TEXT("门板型号"));
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    Container->ListPanel->SetListController(
        MakeReplacingListPanelContent()
    );

    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->SetCurrentState(DesireState);

    CurrentDoorTypeCategory = 0;
    Container->ClearReplacingList();
    LoadDoorCategoryData();
    LoadDoorData();
}

void FArmyWHCCabDoorUIBase::Callback_ReplacingDoorItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    FWHCModeGlobalData::TCoverDoorTypeMap * DoorTypeMapPtr = GetDoorTypeMap(CurrentDoorType);

    int32 Id = Container->GetReplacingIds()[InItemCode];

    const TSharedPtr<FCoverDoorTypeInfo> * DoorTypeInfoPtr = DoorTypeMapPtr->Find(Id);
    check(DoorTypeInfoPtr != nullptr);
    const TSharedPtr<FCoverDoorTypeInfo> & DoorTypeInfo = *DoorTypeInfoPtr;
    if (DoorTypeInfo->IsDoorTypeHasValidCache())
    {
        ReplacingDoorItemInternal(InItemCode, ShapeFrame->ShapeInRoomRef, DoorTypeInfo.Get());      
        Container->ListPanel->SetSelectedItem(InItemCode);
    }
    else if (DoorTypeInfo->RcState == ECabinetResourceState::CabRc_None)
    {
        SContentItem * Item = Container->GetListItem(InItemCode);
        if (Item == nullptr)
            return;
        Item->SetItemState(EPending);

        DoorTypeInfo->RcState = ECabinetResourceState::CabRc_FetchingJson;

        Container->RunHttpRequest(FString::Printf(TEXT("design/door/%d"), DoorTypeInfo->Id),
            TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
                [this, Id, DoorTypeMapPtr](const TSharedPtr<FJsonObject> &ResponseData)
                {
                    const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                    if (Data.IsValid())
                    {
                        FSCTShape *ReplacingDoorGroupShape = nullptr;
                        if (CurrentDoorType == 1) // 移门
                            ReplacingDoorGroupShape = new FSlidingDoor;
                        else if (CurrentDoorType == 2) // 掩门
                            ReplacingDoorGroupShape = new FSideHungDoor;
                        else if (CurrentDoorType == 3) // 抽面
                            ReplacingDoorGroupShape = new FDrawerDoorShape;
                        check(ReplacingDoorGroupShape != nullptr);
                        FSCTShapeManager::Get()->ParseChildrenShapes(Data);
                        ReplacingDoorGroupShape->ParseFromJson(Data);
                        FSCTShapeManager::Get()->ClearAllChildrenShapes();

                        // 获取需要下载的资源列表
                        TArray<FString> CurResourceUrls;
                        TArray<FString> CurFileCachePaths;
                        ReplacingDoorGroupShape->GetResourceUrls(CurResourceUrls);
                        ReplacingDoorGroupShape->GetFileCachePaths(CurFileCachePaths);
                        ReplacingDoorGroupShape->CheckResourceUrlsAndCachePaths(CurResourceUrls, CurFileCachePaths);

						TSharedPtr<FCoverDoorTypeInfo> DoorTypeInfo = DoorTypeMapPtr->FindRef(Id);
						if (DoorTypeInfo.IsValid())
						{
							for (auto& DoorType : DoorTypeInfo->ComponentArray)
							{
								if (!DoorType->TopResourceUrl.IsEmpty())
								{
									CurResourceUrls.Add(DoorType->TopResourceUrl);
									CurFileCachePaths.Add(DoorType->GetCachePath(1));
								}
								if (!DoorType->FrontResourceUrl.IsEmpty())
								{
									CurResourceUrls.Add(DoorType->FrontResourceUrl);
									CurFileCachePaths.Add(DoorType->GetCachePath(2));
								}
								if (!DoorType->TopResourceUrl.IsEmpty())
								{
									CurResourceUrls.Add(DoorType->SideResourceUrl);
									CurFileCachePaths.Add(DoorType->GetCachePath(3));
								}
							}
						}

                        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
                        for (int32 i = 0; i < CurResourceUrls.Num(); ++i)
                        {
                            WHC_ADD_DOWNLOAD_TASK(FileInfos, i, CurFileCachePaths[i], CurResourceUrls[i], FString());
                        }

                        SContentItem * Item = Container->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_DoorType);
                        if (Item)
                            Item->SetItemState(EDownloading);

                        DoorTypeInfo->DoorGroupShape = MakeShareable(ReplacingDoorGroupShape);
                        DoorTypeInfo->RcState = ECabinetResourceState::CabRc_FetchingRc;

                        if (FileInfos.Num() == 0)
                        {
                            SetDownloadDoorResState(false, Item, DoorTypeInfo.Get());
                            return;
                        }

                        if (CurrentDoorType == 1)
                            DoorTypeInfo->DownloadTaskFlag = FString::Printf(TEXT("SlidingDoor_%d"), DoorTypeInfo->Id);
                        else if (CurrentDoorType == 2)
                            DoorTypeInfo->DownloadTaskFlag = FString::Printf(TEXT("CoverDoor_%d"), DoorTypeInfo->Id);
                        else if (CurrentDoorType == 3)
                            DoorTypeInfo->DownloadTaskFlag = FString::Printf(TEXT("Drawer_%d"), DoorTypeInfo->Id);
                        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(DoorTypeInfo->DownloadTaskFlag, FileInfos);
                        NewTask->OnDownloadFinished.BindRaw(this, 
                            &FArmyWHCCabDoorUIBase::ReplacingDoorResourcesDownloadFinished, 
                            Id);
                    }
                }
            )
        );
    }
}

void FArmyWHCCabDoorUIBase::Callback_DoorCategoryChanged(const FString &InNewCategory)
{
    if (!IsValidSelection())
        return;

    for (auto & Category : DoorTypeCategoryArr)
    {
        if (Category.Value == InNewCategory)
        {
            CurrentDoorTypeCategory = Category.Key;
            CabDoorCategoryComboBox->SetValue(Category.Value);
            Container->ClearReplacingList();
            LoadDoorData();
            break;
        }
    }
}

void FArmyWHCCabDoorUIBase::LoadDoorData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
    if (DoorGroup == nullptr)
        return;

    FString QueryUrl;
    if (CurrentDoorType == 1) // 移门
    {
        check(DoorGroup->GetShapeType() == ST_SlidingDoor);
        FSlidingDoor *SlidingDoor = static_cast<FSlidingDoor*>(DoorGroup);
        QueryUrl = FString::Printf(
            TEXT("design/door/cabinetsProduct/%d/doors?categoryId=%d&doorType=%d&doorGroupNum=%d&current=%d&size=15"), 
            ShapeFrame->ShapeInRoomRef->Shape->GetShapeId(),
            CurrentDoorTypeCategory, 
            CurrentDoorType,
            SlidingDoor->GetDoorSheets().Num(),
            Container->GetPageIndexToLoad()
        );
    }
    else if (CurrentDoorType == 2) // 掩门
    {
        check(DoorGroup->GetShapeType() == ST_SideHungDoor);
        FSideHungDoor *SideHungDoor = static_cast<FSideHungDoor*>(DoorGroup);
        check(SideHungDoor->GetDoorSheetNum() > 0);
        FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor *DoorSheet = SideHungDoor->GetDoorSheets()[0].Get();
        EMetalsType HandleType = EMetalsType::MT_None;
        switch (SideHungDoor->GetHanleType())
        {
            case FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS:
                HandleType = EMetalsType::MT_SHAKE_HANDS;
                break;
            case FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND:
                HandleType = EMetalsType::MT_LATERAL_BOX_SHAKE_HAND;
                break;
            case FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE:
                HandleType = EMetalsType::MT_SEALING_SIDE_HANDLE;
                break;
        }
        QueryUrl = FString::Printf(
            TEXT("design/door/cabinetsProduct/%d/doors?categoryId=%d&doorType=%d&current=%d&size=15&width=%d&height=%d&openDoorDirection=%d&shakeHandsType=%d&shapeGuid=%s"), 
            ShapeFrame->ShapeInRoomRef->Shape->GetShapeId(),
            CurrentDoorTypeCategory, 
            CurrentDoorType,
            Container->GetPageIndexToLoad(),
            FMath::RoundToInt(DoorSheet->GetShapeWidth()), 
            FMath::RoundToInt(DoorSheet->GetShapeHeight()), 
            (int32)DoorSheet->GetOpenDoorDirection(),
            (int32)HandleType,
            *(SideHungDoor->GetShapeCopyFromGuid().ToString())
        );
    }
    else if (CurrentDoorType == 3) // 抽面
    {
        check(DoorGroup->GetShapeType() == ST_DrawerDoor);
        FDrawerDoorShape *DrawerDoor = static_cast<FDrawerDoorShape*>(DoorGroup);
        check(DrawerDoor->GetDrawDoorSheet().IsValid());
        FDrawerDoorShape::FVariableAreaDoorSheetForDrawer *DoorSheet = DrawerDoor->GetDrawDoorSheet().Get();
        EMetalsType HandleType = EMetalsType::MT_None;
        switch (DrawerDoor->GetDoorGroupHanleType())
        {
            case FSideHungDoor::EHandleMetalType::E_MT_SHAKE_HANDS:
                HandleType = EMetalsType::MT_SHAKE_HANDS;
                break;
            case FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND:
                HandleType = EMetalsType::MT_LATERAL_BOX_SHAKE_HAND;
                break;
            case FSideHungDoor::EHandleMetalType::E_MT_SEALING_SIDE_HANDLE:
                HandleType = EMetalsType::MT_SEALING_SIDE_HANDLE;
                break;
        }
        QueryUrl = FString::Printf(
            TEXT("design/door/cabinetsProduct/%d/doors?categoryId=%d&doorType=%d&current=%d&size=15&width=%d&height=%d&openDoorDirection=%d&shakeHandsType=%d&shapeGuid=%s"), 
            ShapeFrame->ShapeInRoomRef->Shape->GetShapeId(),
            CurrentDoorTypeCategory, 
            CurrentDoorType,
            Container->GetPageIndexToLoad(),
            FMath::RoundToInt(DoorSheet->GetShapeWidth()), 
            FMath::RoundToInt(DoorSheet->GetShapeHeight()), 
            (int32)DoorSheet->GetOpenDoorDirection(),
            (int32)HandleType,
            *(DrawerDoor->GetShapeCopyFromGuid().ToString())
        );
    }

    Container->RunHttpRequest(
        QueryUrl,
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
                
                    const TArray<TSharedPtr<FJsonValue>>* DoorTypeArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), DoorTypeArray))
                    {
                        int32 nCount = (*DoorTypeArray).Num();
                        int32 IndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = DoorTypeArray->operator[](i);
                            const TSharedPtr<FJsonObject> &DoorTypeJsonObject = Value->AsObject();

                            int32 Id = DoorTypeJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            FWHCModeGlobalData::TCoverDoorTypeMap * DoorTypeMapPtr = GetDoorTypeMap(CurrentDoorType);

                            if (DoorTypeMapPtr->Find(Id) == nullptr)
                            {
                                FCoverDoorTypeInfo * NewCoverDoorTypeInfo = new FCoverDoorTypeInfo;
                                NewCoverDoorTypeInfo->Id = Id;
                                const TArray<TSharedPtr<FJsonValue>> &OpenDoorDirs = DoorTypeJsonObject->GetArrayField(TEXT("openDoorDirections"));
                                for (const auto &OpenDoorDir : OpenDoorDirs)
                                    NewCoverDoorTypeInfo->SKUOpenDoorDirections.Emplace((int32)OpenDoorDir->AsNumber());
                                NewCoverDoorTypeInfo->Name = DoorTypeJsonObject->GetStringField(TEXT("name"));
                                NewCoverDoorTypeInfo->ThumbnailUrl = DoorTypeJsonObject->GetStringField(TEXT("thumbnailUrl"));

								const TSharedPtr<FJsonObject> ComponentJsonObj = DoorTypeJsonObject->GetObjectField(TEXT("viewLegend"));
								if (ComponentJsonObj.IsValid())
								{
									int32 ComponentId = ComponentJsonObj->GetIntegerField(TEXT("id"));
									int32 ComponentType = ComponentJsonObj->GetIntegerField(TEXT("type"));

									switch (ComponentType)
									{
									case 106://门型
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
										NewCoverDoorTypeInfo->ComponentArray.Add(AccessoryComponent);
									}
									break;
									default:
										break;
									}
								}
                                DoorTypeMapPtr->Emplace(Id, MakeShareable(NewCoverDoorTypeInfo));
                            }
                        }
                        FillDoorList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCCabDoorUIBase::FillDoorList(int32 InStart, int32 InCount)
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
    if (DoorGroup == nullptr)
        return;
    int32 DoorGroupId = DoorGroup->GetShapeId();

    FWHCModeGlobalData::TCoverDoorTypeMap * DoorTypeMapPtr = GetDoorTypeMap(CurrentDoorType);

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FCoverDoorTypeInfo> * DoorTypeInfoPtr = DoorTypeMapPtr->Find(CabIds[InStart + i]);
        check(DoorTypeInfoPtr != nullptr);
        const TSharedPtr<FCoverDoorTypeInfo> & DoorTypeInfo = *DoorTypeInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(DoorTypeInfo->ThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(!DoorTypeInfo->IsDoorTypeHasValidCache())
			.OnClicked_Raw(this, &FArmyWHCCabDoorUIBase::Callback_ReplacingDoorItem)
			.DisplayText(FText::FromString(DoorTypeInfo->Name));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_DoorType, DoorTypeInfo->Id));
	
        switch (DoorTypeInfo->RcState)
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

        if (DoorGroupId == DoorTypeInfo->Id)
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCCabDoorUIBase::ReplacingDoorResourcesDownloadFinished(bool bSucceeded, int32 InShapeId)
{
    FWHCModeGlobalData::TCoverDoorTypeMap * DoorTypeMapPtr = GetDoorTypeMap(CurrentDoorType);

    const TSharedPtr<FCoverDoorTypeInfo> * DoorTypeInfoPtr = DoorTypeMapPtr->Find(InShapeId);
    check(DoorTypeInfoPtr != nullptr);
    const TSharedPtr<FCoverDoorTypeInfo> & DoorTypeInfo = *DoorTypeInfoPtr;

    SContentItem * Item = Container->GetListItemMatchingType(InShapeId, EWHCModeAttrType::EWHCAttr_DoorType);
    SetDownloadDoorResState(bSucceeded, Item, DoorTypeInfo.Get());

	// if (!bSucceeded)
		// DoorTypeMapPtr->Remove(InShapeId);
}

void FArmyWHCCabDoorUIBase::LoadDoorCategoryData()
{
    if (DoorTypeCategoryArr.Num() == 0)
    {
        Container->RunHttpRequest(
            FString::Printf(TEXT("design/door/%d/category"), CurrentDoorType),
            TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
                [this](const TSharedPtr<FJsonObject> &ResponseData)
                {
                    const TArray<TSharedPtr<FJsonValue>> & Categories = ResponseData->GetArrayField(TEXT("data"));
                    DoorTypeCategoryArr.Emplace(TPairInitializer<int32, FString>(0, TEXT("全部")));
                    for (const auto & Category : Categories)
                    {
                        const TSharedPtr<FJsonObject> & CategoryJsonObj = Category->AsObject();
                        DoorTypeCategoryArr.Emplace(TPairInitializer<int32, FString>(CategoryJsonObj->GetIntegerField(TEXT("id")), CategoryJsonObj->GetStringField(TEXT("name"))));
                    }
                    FillDoorCategoryComboBox();
                }
            )
        );
    }
    else
        FillDoorCategoryComboBox();
}

void FArmyWHCCabDoorUIBase::FillDoorCategoryComboBox()
{
    TArray<TSharedPtr<FString>> OptionSource;
    for (const auto & OneCategory : DoorTypeCategoryArr)
        OptionSource.Emplace(MakeShareable(new FString(OneCategory.Value)));
    CabDoorCategoryComboBox->SetOptionSource(OptionSource);
}

TArray<FDoorGroup*> FArmyWHCCabDoorUIBase::GetDoorGroups(FShapeInRoom *InShapeInRoom) const
{
    FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());

    TArray<FDoorGroup*> DoorGroups;
    switch (CurrentDoorType)
    {
        case 1: // 移门
            XRShapeUtilities::GetCabinetDoors(CabShape, DoorGroups, FLAG_SLIDINGDOORS);
            break;
        case 2: // 掩门
            XRShapeUtilities::GetCabinetDoors(CabShape, DoorGroups, FLAG_SIDEHUNGDOORS);
            break;
        case 3: // 抽面
            XRShapeUtilities::GetCabinetDrawerDoors(CabShape, DoorGroups);
            break;
    }
        
    return MoveTemp(DoorGroups);
}

FDoorGroup* FArmyWHCCabDoorUIBase::GetFirstDoorGroup(FShapeInRoom *InShapeInRoom) const
{
    TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
    return DoorGroups.Num() > 0 ? DoorGroups[0] : nullptr;
}

FArmyWHCSingleDoorGroupChange* FArmyWHCCabDoorUIBase::GetOrCreateDoorChange(FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const
{
    FArmyWHCSingleDoorGroupChange * DoorChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP))
        {
            DoorChange = static_cast<FArmyWHCSingleDoorGroupChange*>(Change.Get());
            if (DoorChange->GetDoorGroupGuid() == InGuid)
                return DoorChange;
        }
    }

    DoorChange = new FArmyWHCSingleDoorGroupChange(InShapeInRoom->Shape.Get());
    DoorChange->SetDoorGroupGuid(InGuid);
    InShapeInRoom->ChangeList.Emplace(MakeShareable(DoorChange));

    return DoorChange;
}

FArmyWHCSingleDrawerGroupChange* FArmyWHCCabDoorUIBase::GetOrCreateDrawerChange(FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const
{
    FArmyWHCSingleDrawerGroupChange * DrawerChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP))
        {
            DrawerChange = static_cast<FArmyWHCSingleDrawerGroupChange*>(Change.Get());
            if (DrawerChange->GetDoorGroupGuid() == InGuid)
                return DrawerChange;
        }
    }

    DrawerChange = new FArmyWHCSingleDrawerGroupChange(InShapeInRoom->Shape.Get());
    DrawerChange->SetDoorGroupGuid(InGuid);
    InShapeInRoom->ChangeList.Emplace(MakeShareable(DrawerChange));

    return DrawerChange;
}

void FArmyWHCCabDoorUIBase::SetDownloadDoorResState(bool bSucceeded, SContentItem *UIItem, FCoverDoorTypeInfo *DataItem)
{
    if (UIItem != nullptr)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem != nullptr)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}

bool FArmyWHCCabDoorUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRShapeFrame>();
}

AXRShapeFrame* FArmyWHCCabDoorUI::GetShapeFrame() const
{
    return Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
}

void FArmyWHCCabDoorUI::ReplacingDoorItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, FCoverDoorTypeInfo *InDoorTypeInfo)
{
    if (Container->IsClickedSameReplacingItem(InItemCode))
        return;   

    TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);

    int32 CurrentDoorType = GetCurrentDoorType();
    if (CurrentDoorType == 1) // 移门替换
    {
        for (const auto &DoorGroup : DoorGroups)
        {
            FSlidingDoor * SlidingDoor = static_cast<FSlidingDoor *>(DoorGroup);
            FGuid DoorGroupGuid = SlidingDoor->GetShapeCopyFromGuid();
            const TArray<TSharedPtr<FSpaceDividDoorSheet>> & DoorSheets = SlidingDoor->GetDoorSheets();
            check(DoorSheets.Num() > 0);
            if (FSCTShapeUtilityTool::ReplaceDoorGroupAndKeepMetalTransform(DoorSheets[0].Get(), InDoorTypeInfo->DoorGroupShape.Get()).IsValid())
            {
                FArmyWHCSingleDoorGroupChange *Change = GetOrCreateDoorChange(InShapeInRoom, DoorGroupGuid);
                Change->SetDoorTypeTemplateId(InDoorTypeInfo->DoorGroupShape->GetShapeId());
            }
        }
    }
    else if (CurrentDoorType == 2) // 掩门替换
    {
        for (auto & Animation : InShapeInRoom->Animations)
            Animation->Stop();

        for (const auto &DoorGroup : DoorGroups)
        {
            FSideHungDoor * SideHungDoor = static_cast<FSideHungDoor *>(DoorGroup);
            FGuid DoorGroupGuid = SideHungDoor->GetShapeCopyFromGuid();
            const TArray<TSharedPtr<FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor>> & DoorSheets = SideHungDoor->GetDoorSheets();
            check(DoorSheets.Num() > 0);
            if (FSCTShapeUtilityTool::ReplaceDoorGroupAndKeepMetalTransform(DoorSheets[0].Get(), InDoorTypeInfo->DoorGroupShape.Get()).IsValid())
            {
                // 掩门替换成功后要把所有关于门板的修改都移除掉
                TArray<TSharedPtr<IArmyWHCModeShapeChange>> TempChangeList;
                for (const auto &Change : InShapeInRoom->ChangeList)
                {
                    if ((!Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL)) && 
                        (!Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE)) && 
                        (!Change->IsSameType(SHAPE_CHANGE_DOOR_OPEN_DIR)))
                        TempChangeList.Emplace(Change);
                }
                InShapeInRoom->ChangeList = MoveTemp(TempChangeList);

                FArmyWHCSingleDoorGroupChange *Change = GetOrCreateDoorChange(InShapeInRoom, DoorGroupGuid);
                Change->SetDoorTypeTemplateId(InDoorTypeInfo->DoorGroupShape->GetShapeId());
            }
        }

        InShapeInRoom->PrepareShapeAnimations();
    }
    else if (CurrentDoorType == 3) // 抽面替换
    {
        for (auto & Animation : InShapeInRoom->Animations)
            Animation->Stop();

        for (const auto &DoorGroup : DoorGroups)
        {
            FDrawerDoorShape * DrawerDoorShape = static_cast<FDrawerDoorShape*>(DoorGroup);
            FGuid DrawerDoorGuid = DrawerDoorShape->GetShapeCopyFromGuid();
            check(DrawerDoorShape->GetDrawDoorSheet().IsValid());
            if (FSCTShapeUtilityTool::ReplaceDoorGroupAndKeepMetalTransform(DrawerDoorShape->GetDrawDoorSheet().Get(), InDoorTypeInfo->DoorGroupShape.Get()).IsValid())
            {
                // 抽面替换成功后要把所有关于抽面的修改都移除掉
                TArray<TSharedPtr<IArmyWHCModeShapeChange>> TempChangeList;
                for (const auto &Change : InShapeInRoom->ChangeList)
                {
                    if ((!Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL)) && 
                        (!Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE)))
                        TempChangeList.Emplace(Change);
                }
                InShapeInRoom->ChangeList = MoveTemp(TempChangeList);

                FArmyWHCSingleDrawerGroupChange *Change = GetOrCreateDrawerChange(InShapeInRoom, DrawerDoorGuid);
                Change->SetDoorTypeTemplateId(InDoorTypeInfo->DoorGroupShape->GetShapeId());
            }
        }

        InShapeInRoom->PrepareShapeAnimations();
    }

    InShapeInRoom->MakeImmovable();
    FSCTShapeUtilityTool::SetActorTag(InShapeInRoom->ShapeFrame, XRActorTag::WHCActor);
}

TSharedRef<SWidget> FArmyWHCCabDoorUI::MakeReplacingListPanelContent()
{
    return 
    SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    .HAlign(HAlign_Left)
    .Padding(16.0f, 8.0f, 16.0f, 0.0f)
    [
        SNew(SBox)
        .WidthOverride(96.0f)
        .HeightOverride(32.0f)
        [
            CabDoorCategoryComboBox.ToSharedRef()
        ]
    ];
}

TSharedPtr<SWidget> FArmyWHCDoorUI::MakeWidget()
{
    MRangeUI = MakeShareable(TXRWHCAttrUIComponent<FArmyWHCModificationRangeUI>::MakeComponentInstance(Container, true));
    MRangeUI->MakeWidget();
    MRangeUI->SetMRange(SArmyWHCModeModificationRange::MR_All);
    return FArmyWHCCabDoorUIBase::MakeWidget();
}

bool FArmyWHCDoorUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

AXRShapeFrame* FArmyWHCDoorUI::GetShapeFrame() const
{
    return Container->CabinetOperationRef->GetShapeFrameFromSelected();
}

void FArmyWHCDoorUI::ReplacingDoorItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, FCoverDoorTypeInfo *InDoorTypeInfo)
{
    if (Container->IsClickedSameReplacingItem(InItemCode) && !MRangeUI->IsMRangeChanged())
        return;    
    MRangeUI->SyncMRange();
    switch ((SArmyWHCModeModificationRange::EModificationRange)MRangeUI->GetMRange())
    {
        case SArmyWHCModeModificationRange::MR_Single:
        {
            ReplacingSingleDoorOrDrawer(InShapeInRoom, Cast<ASCTShapeActor>(Container->LastWHCItemSelected)->GetShape(), InDoorTypeInfo);
            break;
        }
        case SArmyWHCModeModificationRange::MR_SingleCab:
        {
            ReplacingSingleCabDoorOrDrawer(InShapeInRoom, Cast<ASCTShapeActor>(Container->LastWHCItemSelected)->GetShape(), InDoorTypeInfo);
            break;
        }
        case SArmyWHCModeModificationRange::MR_SameType:
        {
            ReplacingSameTypeCabDoorOrDrawer(InShapeInRoom, InDoorTypeInfo);
            break;
        }
        case SArmyWHCModeModificationRange::MR_All:
        {
            ReplacingAllCabDoorOrDrawer(InShapeInRoom, InDoorTypeInfo);
            break;
        }
    }
    InShapeInRoom->MakeImmovable();
    FSCTShapeUtilityTool::SetActorTag(InShapeInRoom->ShapeFrame, XRActorTag::WHCActor);

    Container->NotifyAttrPanelNeedRebuild();
}

FSCTShape* FArmyWHCDoorUI::ReplacingDoorOrDrawer(FShapeInRoom *InShapeInRoom, FSCTShape *InDoorSheetToReplace, FCoverDoorTypeInfo *InDoorTypeInfo)
{
    FGuid ShapeGuid = InDoorSheetToReplace->GetParentShape()->GetShapeCopyFromGuid();

    int32 DoorType = GetCurrentDoorType();
    bool bIsSideHungDoor = DoorType == 2;

    // 检查SKU中设置的开门方向是否包含当前设置的开门方向，如果包含则不需要修改开门方向，否则使用SKUOpenDoorDirections
    // 中的第一个开门方向修改当前开门方向
    bool bNeedChangeOpenDir = false;
    if (bIsSideHungDoor)
    {
        FVariableAreaDoorSheet *OldDoorSheet = static_cast<FVariableAreaDoorSheet*>(InDoorSheetToReplace);
        FVariableAreaDoorSheet::EDoorOpenDirection OriginOpenDir = OldDoorSheet->GetOpenDoorDirection();
        bNeedChangeOpenDir = !InDoorTypeInfo->SKUOpenDoorDirections.ContainsByPredicate([OriginOpenDir](const int32 Data) -> bool {
            return Data == (int32)OriginOpenDir;
        });
    }

    TSharedPtr<FSCTShape> NewDoorSheet = FSCTShapeUtilityTool::ReplaceDoorGroupAndKeepMetalTransform(
        InDoorSheetToReplace, 
        InDoorTypeInfo->DoorGroupShape.Get()
    );
    if (!NewDoorSheet.IsValid())
        return nullptr;

    if (bIsSideHungDoor && InDoorTypeInfo->SKUOpenDoorDirections.Num() > 0)
    {
        TArray<FVariableAreaDoorSheet::EDoorOpenDirection> &AvailableOpenDirs = 
            StaticCastSharedPtr<FVariableAreaDoorSheet>(NewDoorSheet)->GetAvailableOpenDirections();
        AvailableOpenDirs.Empty();
        for (const auto &SKUOpenDir : InDoorTypeInfo->SKUOpenDoorDirections)
            AvailableOpenDirs.Emplace((FVariableAreaDoorSheet::EDoorOpenDirection)SKUOpenDir);
    }

    // 清空所有属于该门型的材质和拉手修改
    TArray<TSharedPtr<IArmyWHCModeShapeChange>> TempChangeList;
    for (const auto &Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL))
        {
            FArmyWHCSingleDoorGroupMtlChange * Temp = static_cast<FArmyWHCSingleDoorGroupMtlChange*>(Change.Get());
            if (Temp->GetDoorGroupGuid() != ShapeGuid)
                TempChangeList.Emplace(Change);
        }
        else if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE))
        {
            FArmyWHCSingleDoorGroupHandleChange * Temp = static_cast<FArmyWHCSingleDoorGroupHandleChange*>(Change.Get());
            if (Temp->GetDoorGroupGuid() != ShapeGuid)
                TempChangeList.Emplace(Change);
        }
        else if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL))
        {
            FArmyWHCSingleDrawerGroupMtlChange * Temp = static_cast<FArmyWHCSingleDrawerGroupMtlChange*>(Change.Get());
            if (Temp->GetDoorGroupGuid() != ShapeGuid)
                TempChangeList.Emplace(Change);
        }
        else if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE))
        {
            FArmyWHCSingleDrawerGroupHandleChange * Temp = static_cast<FArmyWHCSingleDrawerGroupHandleChange*>(Change.Get());
            if (Temp->GetDoorGroupGuid() != ShapeGuid)
                TempChangeList.Emplace(Change);
        }
        else
            TempChangeList.Emplace(Change);
    }
    InShapeInRoom->ChangeList = MoveTemp(TempChangeList);

    if (DoorType == 2)
    {
        FArmyWHCSingleDoorGroupChange *DoorGroupChange = GetOrCreateDoorChange(InShapeInRoom, ShapeGuid);
        DoorGroupChange->SetDoorTypeTemplateId(InDoorTypeInfo->Id);
    }
    else if (DoorType == 3)
    {
        FArmyWHCSingleDrawerGroupChange *DrawerGroupChange = GetOrCreateDrawerChange(InShapeInRoom, ShapeGuid);
        DrawerGroupChange->SetDoorTypeTemplateId(InDoorTypeInfo->Id);
    }

    if (bNeedChangeOpenDir && InDoorTypeInfo->SKUOpenDoorDirections.Num() > 0)
    {
        // 当柜子包含SKU开门方向，且替换前开门方向不包含在替换后开门方向中时，将开门方向更换为替换后开门方向的第一个
        check(DoorType == 2);

        // 修改开门方向
        TSharedPtr<FVariableAreaDoorSheet> VarAreaDoorSheet = StaticCastSharedPtr<FVariableAreaDoorSheet>(NewDoorSheet);
        VarAreaDoorSheet->SetOpenDoorDirection((FVariableAreaDoorSheet::EDoorOpenDirection)InDoorTypeInfo->SKUOpenDoorDirections[0]);

        FGuid Guid = VarAreaDoorSheet->GetShapeCopyFromGuid();

        FArmyWHCDoorOpenDirChange * OpenDirChange = nullptr;
        for (const auto & Change : InShapeInRoom->ChangeList)
        {
            if (Change->IsSameType(SHAPE_CHANGE_DOOR_OPEN_DIR))
            {
                FArmyWHCDoorOpenDirChange * Temp = static_cast<FArmyWHCDoorOpenDirChange*>(Change.Get());
                if (Temp->GetDoorSheetGuid() == Guid)
                {
                    OpenDirChange = Temp;
                    break;
                }
            }
        }
        if (OpenDirChange == nullptr)
        {
            OpenDirChange = new FArmyWHCDoorOpenDirChange(InShapeInRoom->Shape.Get());
            OpenDirChange->SetDoorSheetGuid(Guid);
            InShapeInRoom->ChangeList.Emplace(MakeShareable(OpenDirChange));
        }
        OpenDirChange->SetDoorOpenDir(InDoorTypeInfo->SKUOpenDoorDirections[0]);
    }

    return NewDoorSheet.Get();
}

void FArmyWHCDoorUI::ReplacingSingleDoorOrDrawer(FShapeInRoom *InShapeInRoom, FSCTShape *InShapeToProcess, FCoverDoorTypeInfo *InDoorTypeInfo)
{
    for (auto & Animation : InShapeInRoom->Animations)
        Animation->Stop();
    FSCTShape *NewDoorOrDrawer = nullptr;
    int32 DoorType = GetCurrentDoorType();
    if (DoorType == 2) // 掩门选中的是单个门板
        NewDoorOrDrawer = ReplacingDoorOrDrawer(InShapeInRoom, InShapeToProcess, InDoorTypeInfo);
    else if (DoorType == 3) // 抽面选中的是抽屉
    {
        FSlidingDrawerShape *SlidingDrawerShape = static_cast<FSlidingDrawerShape*>(InShapeToProcess);
        const TArray<TSharedPtr<FDrawerDoorShape>> &DrawerDoors = SlidingDrawerShape->GetDrawerDoorShapes();
        check(DrawerDoors.Num() > 0);
        check(DrawerDoors[0]->GetDrawDoorSheet().IsValid());
        NewDoorOrDrawer = ReplacingDoorOrDrawer(InShapeInRoom, DrawerDoors[0]->GetDrawDoorSheet().Get(), InDoorTypeInfo);
    }
    if (NewDoorOrDrawer == nullptr)
        return;
    InShapeInRoom->PrepareShapeAnimations();

    if (DoorType == 2) // 抽面替换选中的是抽屉，所以不用更新当前选中对象
    {
        Container->LastWHCItemSelected = NewDoorOrDrawer->GetShapeActor();
        Container->CabinetOperationRef->UpdateLastSelected(Container->LastWHCItemSelected);
    }
}

void FArmyWHCDoorUI::ReplacingSingleCabDoorOrDrawer(FShapeInRoom *InShapeInRoom, FSCTShape *InShapeToProcess, FCoverDoorTypeInfo *InDoorTypeInfo, bool bNeedUpdateSelected)
{
    for (auto & Animation : InShapeInRoom->Animations)
        Animation->Stop();
    FSCTShape *NewDoorOrDrawer = nullptr;
    int32 DoorType = GetCurrentDoorType();
    if (DoorType == 2) // 掩门选中的是单个门板
        NewDoorOrDrawer = ReplacingDoorOrDrawer(InShapeInRoom, InShapeToProcess, InDoorTypeInfo);
    else if (DoorType == 3) // 抽面选中的是抽屉
    {
        FSlidingDrawerShape *SlidingDrawerShape = static_cast<FSlidingDrawerShape*>(InShapeToProcess);
        const TArray<TSharedPtr<FDrawerDoorShape>> &DrawerDoors = SlidingDrawerShape->GetDrawerDoorShapes();
        check(DrawerDoors.Num() > 0);
        check(DrawerDoors[0]->GetDrawDoorSheet().IsValid());
        NewDoorOrDrawer = ReplacingDoorOrDrawer(InShapeInRoom, DrawerDoors[0]->GetDrawDoorSheet().Get(), InDoorTypeInfo);
    }

    if (bNeedUpdateSelected && DoorType == 2 && NewDoorOrDrawer != nullptr) // 抽面替换选中的是抽屉，所以不用更新当前选中对象
    {
        Container->LastWHCItemSelected = NewDoorOrDrawer->GetShapeActor();
        Container->CabinetOperationRef->UpdateLastSelected(Container->LastWHCItemSelected);
    }

    TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
    if (DoorType == 2)
    {
        for (auto &DoorGroup : DoorGroups)
        {
            if (DoorGroup->GetShapeId() != InDoorTypeInfo->Id)
            {
                FSideHungDoor *SideHungDoor = static_cast<FSideHungDoor*>(DoorGroup);
                check(SideHungDoor->GetDoorSheetNum() > 0);
                ReplacingDoorOrDrawer(InShapeInRoom, SideHungDoor->GetDoorSheets()[0].Get(), InDoorTypeInfo);
            }
        }
    }
    else if (DoorType == 3)
    {
        for (auto &DoorGroup : DoorGroups)
        {
            if (DoorGroup->GetShapeId() != InDoorTypeInfo->Id)
            {
                FDrawerDoorShape *DrawerDoor = static_cast<FDrawerDoorShape*>(DoorGroup);
                check(DrawerDoor->GetDrawDoorSheet().IsValid());
                ReplacingDoorOrDrawer(InShapeInRoom, DrawerDoor->GetDrawDoorSheet().Get(), InDoorTypeInfo);
            }
        }
    }

    InShapeInRoom->PrepareShapeAnimations();

    if (NewDoorOrDrawer != nullptr)
    {
        // 更换饰条材质
        ReplacingInsertionBoardMtl(InShapeInRoom, NewDoorOrDrawer);
        // 同步修改调整板材质
        InShapeInRoom->OnChangeDoorMtl();
    }
}

void FArmyWHCDoorUI::ReplacingSameTypeCabDoorOrDrawer(FShapeInRoom *InShapeInRoom, FCoverDoorTypeInfo *InDoorTypeInfo)
{
    ReplacingSingleCabDoorOrDrawer(InShapeInRoom, Cast<ASCTShapeActor>(Container->LastWHCItemSelected)->GetShape(), InDoorTypeInfo);

    int32 ShapeCategory = InShapeInRoom->Shape->GetShapeCategory();

    TArray<FDoorGroup*> DoorGroups;
    TArray<TSharedPtr<FShapeInRoom>> & PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();

    int32 DoorType = GetCurrentDoorType();
    for (auto &PlacedShape : PlacedShapes)
    {
        if (PlacedShape->Shape->GetShapeCategory() == ShapeCategory && PlacedShape.Get() != InShapeInRoom)
        {
            DoorGroups = GetDoorGroups(PlacedShape.Get());
            if (DoorGroups.Num() > 0 && DoorGroups[0]->GetShapeType() == InDoorTypeInfo->DoorGroupShape->GetShapeType())
            {
                if (DoorType == 2)
                {
                    FSideHungDoor * SideHungDoor = static_cast<FSideHungDoor*>(DoorGroups[0]);
                    check(SideHungDoor->GetDoorSheetNum() > 0);
                    ReplacingSingleCabDoorOrDrawer(PlacedShape.Get(), SideHungDoor->GetDoorSheets()[0].Get(), InDoorTypeInfo, false);
                }
                else if (DoorType == 3)
                {
                    FDrawerDoorShape * DrawerDoor = static_cast<FDrawerDoorShape*>(DoorGroups[0]);
                    check(DrawerDoor->GetDrawDoorSheet().IsValid());
                    ReplacingSingleCabDoorOrDrawer(PlacedShape.Get(), DrawerDoor->GetParentShape(), InDoorTypeInfo, false);
                }
            }
        }
    }
}

void FArmyWHCDoorUI::ReplacingAllCabDoorOrDrawer(FShapeInRoom *InShapeInRoom, FCoverDoorTypeInfo *InDoorTypeInfo)
{
    ReplacingSingleCabDoorOrDrawer(InShapeInRoom, Cast<ASCTShapeActor>(Container->LastWHCItemSelected)->GetShape(), InDoorTypeInfo);

    TArray<FDoorGroup*> DoorGroups;
    TArray<TSharedPtr<FShapeInRoom>> & PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();

    int32 DoorType = GetCurrentDoorType();
    for (auto &PlacedShape : PlacedShapes)
    {
        if (PlacedShape.Get() != InShapeInRoom)
        {
            DoorGroups = GetDoorGroups(PlacedShape.Get());
            if (DoorGroups.Num() > 0 && DoorGroups[0]->GetShapeType() == InDoorTypeInfo->DoorGroupShape->GetShapeType())
            {
                if (DoorType == 2)
                {
                    FSideHungDoor * SideHungDoor = static_cast<FSideHungDoor*>(DoorGroups[0]);
                    check(SideHungDoor->GetDoorSheetNum() > 0);
                    ReplacingSingleCabDoorOrDrawer(PlacedShape.Get(), SideHungDoor->GetDoorSheets()[0].Get(), InDoorTypeInfo, false);
                }
                else if (DoorType == 3)
                {
                    FDrawerDoorShape * DrawerDoor = static_cast<FDrawerDoorShape*>(DoorGroups[0]);
                    check(DrawerDoor->GetDrawDoorSheet().IsValid());
                    ReplacingSingleCabDoorOrDrawer(PlacedShape.Get(), DrawerDoor->GetParentShape(), InDoorTypeInfo, false);
                }
            }
        }
    }
}

void FArmyWHCDoorUI::ReplacingInsertionBoardMtl(FShapeInRoom *InShapeInRoom, FSCTShape *InDoorSheetToPeekMtl)
{
    FDoorSheetShapeBase * DoorSheet = static_cast<FDoorSheetShapeBase*>(InDoorSheetToPeekMtl);
    const FDoorSheetShapeBase::FDoorPakMetaData & MtlData = DoorSheet->GetMaterial();
    const TSharedPtr<FMtlInfo> * MtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(MtlData.ID);
    UMaterialInterface * Mtl = nullptr;
    if (MtlInfoPtr != nullptr)
    {
        const TSharedPtr<FMtlInfo> & MtlInfo = *MtlInfoPtr;
        if (MtlInfo->Mtl == nullptr)
        {
            Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
                FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, MtlData.ID), 
                MtlData.OptimizeParam);
        }
        else
            Mtl = MtlInfo->Mtl;
    }
    else
    {
        Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(
            FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, MtlData.ID), 
            MtlData.OptimizeParam);

        FMtlInfo * MtlInfo = new FMtlInfo;
        MtlInfo->Id = MtlData.ID;
        MtlInfo->MtlName = MtlData.Name;
        MtlInfo->MtlThumbnailUrl = MtlData.ThumbnailUrl;
        MtlInfo->MtlUrl = MtlData.Url;
        MtlInfo->MtlMd5 = MtlData.MD5;
        MtlInfo->MtlParam = MtlData.OptimizeParam;
        MtlInfo->Mtl = Mtl;
        FWHCModeGlobalData::CabinetMtlMap.Emplace(MtlData.ID, MakeShareable(MtlInfo));
    }
    if (Mtl != nullptr)
    {
        FSCTShapeUtilityTool::ReplacInsertionBoardMaterial(InShapeInRoom->Shape.Get(), &MtlData, Mtl);    

        FArmyWHCCabinetInsertionBoardMtlChange* InsertionBoardChange = GetOrCreateInsertionBoardMtlChange(InShapeInRoom);
        InsertionBoardChange->SetInsertionBoardMtlId(MtlData.ID);
    }
}

FArmyWHCCabinetInsertionBoardMtlChange* FArmyWHCDoorUI::GetOrCreateInsertionBoardMtlChange(FShapeInRoom *InShapeInRoom)
{
    FArmyWHCCabinetInsertionBoardMtlChange * InsertionBoardChange = nullptr;
    for (auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_CABINET_INSERTION_BOARD_MTL))
        {
            InsertionBoardChange = static_cast<FArmyWHCCabinetInsertionBoardMtlChange*>(Change.Get());
            break;
        }
    }
    if (InsertionBoardChange == nullptr)
    {
        InsertionBoardChange = new FArmyWHCCabinetInsertionBoardMtlChange(InShapeInRoom->Shape.Get());
        InShapeInRoom->ChangeList.Emplace(MakeShareable(InsertionBoardChange));
    }
    return InsertionBoardChange;
}

TSharedRef<SWidget> FArmyWHCDoorUI::MakeReplacingListPanelContent()
{
    return 
    SNew(SVerticalBox)
    + SVerticalBox::Slot()
    .AutoHeight()
    [
        MRangeUI->MakeWidget().ToSharedRef()
    ]
    + SVerticalBox::Slot()
    .AutoHeight()
    .HAlign(HAlign_Left)
    .Padding(16.0f, 8.0f, 16.0f, 0.0f)
    [
        SNew(SBox)
        .WidthOverride(96.0f)
        .HeightOverride(32.0f)
        [
            CabDoorCategoryComboBox.ToSharedRef()
        ]
    ];
}