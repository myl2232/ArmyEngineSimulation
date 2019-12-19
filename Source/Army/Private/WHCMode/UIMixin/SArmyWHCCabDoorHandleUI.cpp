#include "UIMixin/SArmyWHCCabDoorHandleUI.h"
#include "UIMixin/SArmyWHCModificationRangeUI.h"
#include "WHCMode/SArmyWHCModeAttrPanel.h"
#include "WHCMode/SArmyWHCModeListPanel.h"
#include "WHCMode/XRShapeFrameActor.h"
#include "WHCMode/XRWHCabinet.h"
#include "WHCMode/XRWHCModeCabinetOperation.h"
#include "WHCMode/XRWHCShapeDoorChangeList.h"
#include "WHCMode/XRWHCabinetAutoMgr.h"
#include "../common/XRShapeUtilities.h"

#include "ArmyStyle.h"
#include "SContentItem.h"
#include "SScrollWrapBox.h"
#include "ArmySlateModule.h"
#include "ArmyDownloadModule.h"
#include "Actor/XRActorConstant.h"
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTCabinetShape.h"
#include "SCTDoorGroup.h"
#include "SCTSlidingDrawerShape.h"
#include "Actor/SCTShapeActor.h"
#include "SCTAnimation.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCCabDoorHandleUIBase::MakeWidget()
{
    TSharedPtr<SArmyWHCModeReplacingWidget> HandleReplacing;
    SAssignNew(HandleReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCCabDoorHandleUIBase::Callback_ReplacingHandle)
        .OnButtonClicked_Raw(this, &FArmyWHCCabDoorHandleUIBase::Callback_ReplacingHandle);
    HandleReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this]() -> const FSlateBrush* {
            if (IsValidSelection())
            {
                AXRShapeFrame *ShapeFrame = GetShapeFrame();
                if (ShapeFrame != nullptr)
                {
                    check(ShapeFrame->ShapeInRoomRef != nullptr);

                    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
                    int32 HandleId = GetCurrentHandleId(DoorGroup);
                    const TMap<int32, TSharedPtr<FSCTShape>> *OptionMapPtr = GetHandleOptionMap(DoorGroup);
                    if (HandleId > 0 && OptionMapPtr)
                    {
                        const TSharedPtr<FSCTShape> * HandleShapePtr = OptionMapPtr->Find(HandleId);
                        check(HandleShapePtr != nullptr);
                        return FArmySlateModule::Get().WebImageCache->Download((*HandleShapePtr)->GetThumbnailUrl())->GetBrush();
                    }
                }
                return FCoreStyle::Get().GetDefaultBrush();    
            }
            else
                return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    HandleReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            if (IsValidSelection())
            {
                AXRShapeFrame *ShapeFrame = GetShapeFrame();
                if (ShapeFrame != nullptr)
                {
                    check(ShapeFrame->ShapeInRoomRef);

                    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
                    int32 HandleId = GetCurrentHandleId(DoorGroup);
                    const TMap<int32, TSharedPtr<FSCTShape>> *OptionMapPtr = GetHandleOptionMap(DoorGroup);
                    if (HandleId > 0 && OptionMapPtr)
                    {
                        const TSharedPtr<FSCTShape> * HandleShapePtr = OptionMapPtr->Find(HandleId);
                        check(HandleShapePtr != nullptr);
                        return FText::FromString((*HandleShapePtr)->GetShapeName());
                    }
                }
                return FText();    
            }
            else
                return FText();
        }
    ));
    SAssignNew(DoorHandleCategoryComboBox, SArmyWHCModeNormalComboBox)
        .OptionSource(TArray<TSharedPtr<FString>> { MakeShareable(new FString(TEXT("全部"))) })
        .Value(TEXT("全部"))
        .OnValueChanged_Raw(this, &FArmyWHCCabDoorHandleUIBase::Callback_DoorHandleCategoryChanged);
    return MoveTemp(HandleReplacing);
}

void FArmyWHCCabDoorHandleUIBase::LoadListData()
{
    if (!IsValidSelection())
        return;
    LoadDoorHandleData();
}

void FArmyWHCCabDoorHandleUIBase::Callback_ReplacingHandle()
{
    if (!IsValidSelection())
        return;

    FString DesireState(TEXT("拉手型号"));
    if (Container->IsCurrentState(DesireState))
        return;

    Container->ListPanel->ClearListController();
    Container->ListPanel->SetListController(
        MakeReplacingListPanelContent()
    );

    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->ListPanel->SetCurrentState(DesireState);

    CurrentDoorHandleCategory = 0;
    Container->ClearReplacingList();
    LoadDoorHandleCategoryData();
    LoadDoorHandleData();
}

void FArmyWHCCabDoorHandleUIBase::Callback_ReplacingDoorHandleItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;

    if (!IsValidSelection())
        return;

    int32 Id = Container->GetReplacingIds()[InItemCode];
	SContentItem * Item = Container->GetListItem(InItemCode);
	if (Item == nullptr)
		return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

	if (Item->GetItemState() == EReady)
	{
        ReplacingDoorHandleItemInternal(InItemCode, ShapeFrame->ShapeInRoomRef, Id);
        Container->ListPanel->SetSelectedItem(InItemCode);
	}
	else
	{
		int32 ShapeId = ShapeFrame->ShapeInRoomRef->Shape->GetShapeId();
		TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
		TSharedPtr<FCabinetWholeComponent> WholeComponent = FWHCModeGlobalData::CabinetComponentMap.FindRef(ShapeId);
		if (WholeComponent.IsValid())
		{
			for (auto Component : WholeComponent->Accessorys)
			{
				if (Component->Id == Id)
				{
					bool TopExist = true, FrontExist = true, SideExist = true;
					if (!Component->TopResourceUrl.IsEmpty())
					{
						WHC_ADD_DOWNLOAD_TASK(FileInfos, Component->Id, Component->GetCachePath(1), Component->TopResourceUrl, Component->TopResourceMd5);
					}
					if (!Component->FrontResourceUrl.IsEmpty())
					{
						WHC_ADD_DOWNLOAD_TASK(FileInfos, Component->Id, Component->GetCachePath(2), Component->FrontResourceUrl, Component->FrontResourceMd5);
					}
					if (!Component->TopResourceUrl.IsEmpty())
					{
						WHC_ADD_DOWNLOAD_TASK(FileInfos, Component->Id, Component->GetCachePath(3), Component->SideResourceUrl, Component->SideResourceMd5);
					}
					Item->SetItemState(EDownloading);
					break;
				}
			}
		}
		if (FileInfos.Num() == 0)
		{
			SetDownloadDoorHandleResState(false, Item);
			return;
		}

		FString Flag = FString::Printf(TEXT("CoverDoorHandle_%d"), Id);
		TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(Flag, FileInfos);
		NewTask->OnDownloadFinished.BindRaw(this,
			&FArmyWHCCabDoorHandleUIBase::ReplacingDoorHandleResourcesDownloadFinished,
			Id);
	}
}

void FArmyWHCCabDoorHandleUIBase::Callback_DoorHandleCategoryChanged(const FString &InNewCategory)
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
    int32 Id = GetDoorIdForHandle(DoorGroup);

    TArray<TPair<int32, FString>> * DoorHandleCategory = DoorHandleCategoryMap.Find(Id);
    check(DoorHandleCategory != nullptr);
    for (auto & Category : *DoorHandleCategory)
    {
        if (Category.Value == InNewCategory)
        {
            CurrentDoorHandleCategory = Category.Key;
            DoorHandleCategoryComboBox->SetValue(Category.Value);
            Container->ClearReplacingList();
            LoadDoorHandleData();
            break;
        }
    }
}

void FArmyWHCCabDoorHandleUIBase::LoadDoorHandleData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    int32 ShapeId = ShapeFrame->ShapeInRoomRef->Shape->GetShapeId();
    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
    int32 Id = GetDoorIdForHandle(DoorGroup);
    if (Id <= 0)
        return;

    Container->RunHttpRequest(
        FString::Printf(TEXT("design/door/%d/handle?categoryId=%d&current=%d&size=15"), Id, CurrentDoorHandleCategory, Container->GetPageIndexToLoad()),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this, ShapeId](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
            
                    const TArray<TSharedPtr<FJsonValue>>* DoorHandleArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), DoorHandleArray))
                    {
                        int32 nCount = (*DoorHandleArray).Num();
                        int32 nRealCount = 0;
                        int32 IndexStart = Container->GetReplacingIds().Num();

                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = DoorHandleArray->operator[](i);
                            const TSharedPtr<FJsonObject> &DoorHandleJsonObject = Value->AsObject();

                            int32 Id = DoorHandleJsonObject->GetNumberField(TEXT("id"));
                            int32 Category = DoorHandleJsonObject->GetIntegerField(TEXT("category"));
                            if (Category != (int32)EMetalsType::MT_LATERAL_BOX_SHAKE_HAND)
                            {
                                Container->AddReplacingId(Id);
                                ++nRealCount;
                            }

							const TSharedPtr<FJsonObject> ComponentJsonObj = DoorHandleJsonObject->GetObjectField(TEXT("viewLegend"));
							if (ComponentJsonObj.IsValid())
							{
								int32 ComponentId = ComponentJsonObj->GetIntegerField(TEXT("id"));
								int32 ComponentType = ComponentJsonObj->GetIntegerField(TEXT("type"));

								switch (ComponentType)
								{
								case 105:// 五金/电器类
								{
									TSharedPtr<FCabinetComponent> ComponentObj = MakeShareable(new FCabinetComponent);
									ComponentObj->Id = ComponentJsonObj->GetIntegerField(TEXT("id"));
									ComponentObj->Type = ComponentJsonObj->GetIntegerField(TEXT("type"));
									ComponentObj->TopResourceUrl = ComponentJsonObj->GetStringField(TEXT("topViewLegend"));
									ComponentObj->TopResourceMd5 = ComponentJsonObj->GetStringField(TEXT("topViewLegendMd5"));
									ComponentObj->FrontResourceUrl = ComponentJsonObj->GetStringField(TEXT("frontViewLegend"));
									ComponentObj->FrontResourceMd5 = ComponentJsonObj->GetStringField(TEXT("frontViewLegendMd5"));
									ComponentObj->SideResourceUrl = ComponentJsonObj->GetStringField(TEXT("sideViewLegend"));
									ComponentObj->SideResourceMd5 = ComponentJsonObj->GetStringField(TEXT("sideViewLegendMd5"));

									TSharedPtr<FCabinetWholeComponent> WholeComponent = FWHCModeGlobalData::CabinetComponentMap.FindRef(ShapeId);
									if (WholeComponent.IsValid())
									{
										WholeComponent->Accessorys.AddUnique(ComponentObj);
									}
								}
								break;
								default:
									break;
								}
							}
                        }
                        FillDoorHandleList(IndexStart, nRealCount, ShapeId);
                    }
                }
            }
        )
    );
}

void FArmyWHCCabDoorHandleUIBase::FillDoorHandleList(int32 InStart, int32 InCount, int64 InShapeId)
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
    int32 Id = GetCurrentHandleId(DoorGroup);
    const TMap<int32, TSharedPtr<FSCTShape>> * OptionShapeMapPtr = GetHandleOptionMap(DoorGroup);
    if (Id <= 0 || OptionShapeMapPtr == nullptr)
        return;

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FSCTShape> * DoorHandlePtr = OptionShapeMapPtr->Find(CabIds[InStart + i]);
        if (DoorHandlePtr == nullptr)
            continue;
        const TSharedPtr<FSCTShape> & DoorHandle = *DoorHandlePtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(DoorHandle->GetThumbnailUrl())
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(false)
			.OnClicked_Raw(this, &FArmyWHCCabDoorHandleUIBase::Callback_ReplacingDoorHandleItem)
			.DisplayText(FText::FromString(DoorHandle->GetShapeName()));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_DoorHandle, DoorHandle->GetShapeId()));
	
		TSharedPtr<FCabinetWholeComponent> WholeComponent = FWHCModeGlobalData::CabinetComponentMap.FindRef(InShapeId);
		if (WholeComponent.IsValid())
		{
			for (auto Component : WholeComponent->Accessorys)
			{
				if (Component->Id == DoorHandle->GetShapeId())
				{
					bool TopExist = true, FrontExist = true, SideExist = true;
					if (!Component->TopResourceUrl.IsEmpty())
					{
						TopExist = FPaths::FileExists(Component->GetCachePath(1));
					}
					if (!Component->FrontResourceUrl.IsEmpty())
					{
						FrontExist = FPaths::FileExists(Component->GetCachePath(2));
					}
					if (!Component->TopResourceUrl.IsEmpty())
					{
						SideExist = FPaths::FileExists(Component->GetCachePath(3));
					}

					if (TopExist && FrontExist && SideExist)
					{
						Item->SetItemState(EReady);
					}
					else
					{
						Item->SetItemState(ENotReady);
					}
					break;
				}
			}
		}

        //Item->SetItemState(EReady);
		Container->ListPanel->AddItem(Item); 

        if (Id == DoorHandle->GetShapeId())
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCCabDoorHandleUIBase::LoadDoorHandleCategoryData()
{
    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FDoorGroup *DoorGroup = GetFirstDoorGroup(ShapeFrame->ShapeInRoomRef);
    int32 Id = GetDoorIdForHandle(DoorGroup);
    if (Id <= 0)
        return;

    TArray<TPair<int32, FString>> * DoorHandleCategory = DoorHandleCategoryMap.Find(Id);
    if (DoorHandleCategory == nullptr)
    {
        Container->RunHttpRequest(
            FString::Printf(TEXT("design/door/%d/handle/category"), Id),
            TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
                [this, Id](const TSharedPtr<FJsonObject> &ResponseData)
                {
                    const TArray<TSharedPtr<FJsonValue>> & Categories = ResponseData->GetArrayField(TEXT("data"));
                    TArray<TPair<int32, FString>> CategoryStrings;
                    CategoryStrings.Emplace(TPairInitializer<int32, FString>(0, TEXT("全部")));
                    for (const auto & Category : Categories)
                    {
                        const TSharedPtr<FJsonObject> & CategoryJsonObj = Category->AsObject();
                        CategoryStrings.Emplace(TPairInitializer<int32, FString>(CategoryJsonObj->GetIntegerField(TEXT("id")), CategoryJsonObj->GetStringField(TEXT("name"))));
                    }
                    DoorHandleCategoryMap.Emplace(Id, CategoryStrings);
                    FillDoorHandleCategoryComboBox(CategoryStrings);
                }
            )
        );
    }
    else
        FillDoorHandleCategoryComboBox(*DoorHandleCategory);
}

void FArmyWHCCabDoorHandleUIBase::FillDoorHandleCategoryComboBox(const TArray<TPair<int32, FString>> &InDoorHandleCategory)
{
    TArray<TSharedPtr<FString>> OptionSource;
    for (const auto & OneCategory : InDoorHandleCategory)
        OptionSource.Emplace(MakeShareable(new FString(OneCategory.Value)));
    DoorHandleCategoryComboBox->SetOptionSource(OptionSource);
}

void FArmyWHCCabDoorHandleUIBase::ReplacingDoorHandleResourcesDownloadFinished(bool bSucceeded, int32 InShapeId)
{
	SContentItem * Item = Container->GetListItemMatchingType(InShapeId, EWHCModeAttrType::EWHCAttr_DoorHandle);
	SetDownloadDoorHandleResState(bSucceeded, Item);
}

TArray<FDoorGroup*> FArmyWHCCabDoorHandleUIBase::GetDoorGroups(FShapeInRoom *InShapeInRoom) const
{
    FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());

    TArray<FDoorGroup*> DoorGroups;
    XRShapeUtilities::GetCabinetDoors(CabShape, DoorGroups, FLAG_SIDEHUNGDOORS);
    TArray<FDoorGroup*> DrawerDoorGroups;
    XRShapeUtilities::GetCabinetDrawerDoors(CabShape, DrawerDoorGroups);
    DoorGroups.Append(DrawerDoorGroups);
    return MoveTemp(DoorGroups);
}

FDoorGroup* FArmyWHCCabDoorHandleUIBase::GetFirstDoorGroup(FShapeInRoom *InShapeInRoom) const
{
    TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
    return DoorGroups.Num() > 0 ? DoorGroups[0] : nullptr;
}

int32 FArmyWHCCabDoorHandleUIBase::GetDoorIdForHandle(FDoorGroup *InDoorGroup) const
{
    if (InDoorGroup == nullptr)
        return 0;

    switch (InDoorGroup->GetShapeType())
    {
        case ST_SideHungDoor:
        {
            FSideHungDoor * SideHungDoor = static_cast<FSideHungDoor*>(InDoorGroup);
            FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor *DoorSheet = SideHungDoor->GetDoorSheetNum() > 0 ? SideHungDoor->GetDoorSheets()[0].Get() : nullptr;
            if (DoorSheet)
                return SideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND ? SideHungDoor->GetShapeId() : DoorSheet->GetShapeId();
            else
                return 0;
        }
        case ST_DrawerDoor:
        {
            FDrawerDoorShape * DrawerDoor = static_cast<FDrawerDoorShape*>(InDoorGroup);
            TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> DrawerDoorSheet = DrawerDoor->GetDrawDoorSheet();
            if (DrawerDoorSheet.IsValid())
                return DrawerDoor->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND ? DrawerDoor->GetShapeId() : DrawerDoorSheet->GetShapeId(); 
            else
                return 0;
        }
        default:
            return 0;
    }
}

int32 FArmyWHCCabDoorHandleUIBase::GetCurrentHandleId(FDoorGroup *InDoorGroup) const
{
    if (InDoorGroup == nullptr)
        return 0;

    switch (InDoorGroup->GetShapeType())
    {
        case ST_SideHungDoor:
        {
            FSideHungDoor * SideHungDoor = static_cast<FSideHungDoor*>(InDoorGroup);
            FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor *DoorSheet = SideHungDoor->GetDoorSheetNum() > 0 ? SideHungDoor->GetDoorSheets()[0].Get() : nullptr;
            if (DoorSheet)
                return SideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND ? SideHungDoor->GetCurrentHandleInfo().ID : DoorSheet->GetCurrentHandleInfo().ID;
            else
                return 0;
        }
        case ST_DrawerDoor:
        {
            FDrawerDoorShape * DrawerDoor = static_cast<FDrawerDoorShape*>(InDoorGroup);
            TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> DrawerDoorSheet = DrawerDoor->GetDrawDoorSheet();
            if (DrawerDoorSheet.IsValid())
                return DrawerDoor->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND ? DrawerDoor->GetDoorGroupCurrentHandleInfo().ID : DrawerDoorSheet->GetDefaultHandleInfo().ID; 
            else
                return 0;
        }
        default:
            return 0;
    }
}

const TMap<int32, TSharedPtr<FSCTShape>>* FArmyWHCCabDoorHandleUIBase::GetHandleOptionMap(FDoorGroup *InDoorGroup) const
{
    if (InDoorGroup == nullptr)
        return nullptr;

    switch (InDoorGroup->GetShapeType())
    {
        case ST_SideHungDoor:
        {
            FSideHungDoor * SideHungDoor = static_cast<FSideHungDoor*>(InDoorGroup);
            if (SideHungDoor->GetHanleType() == FSideHungDoor::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
                return &SideHungDoor->GetOptionShapeMap();
            else
            {
                FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor *DoorSheet = SideHungDoor->GetDoorSheetNum() > 0 ? SideHungDoor->GetDoorSheets()[0].Get() : nullptr;
                return DoorSheet ? &DoorSheet->GetOptionShapeMap() : nullptr;
            }
        }
        case ST_DrawerDoor:
        {
            FDrawerDoorShape * DrawerDoor = static_cast<FDrawerDoorShape*>(InDoorGroup);
            if (DrawerDoor->GetDoorGroupHanleType() == FDrawerDoorShape::EHandleMetalType::E_MT_LATERAL_BOX_SHAKE_HAND)
                return &DrawerDoor->GetOptionShapeMap();
            else
            {
                TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> DrawerDoorSheet = DrawerDoor->GetDrawDoorSheet();
                return DrawerDoorSheet.IsValid() ? &DrawerDoorSheet->GetOptionShapeMap() : nullptr;
            }
        }
        default:
            return nullptr;
    }
}

FArmyWHCSingleDoorGroupHandleChange* FArmyWHCCabDoorHandleUIBase::GetOrCreateDoorHandleChange(FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const
{
    FArmyWHCSingleDoorGroupHandleChange * DoorHandleChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_HANDLE))
        {
            DoorHandleChange = static_cast<FArmyWHCSingleDoorGroupHandleChange*>(Change.Get());
            if (DoorHandleChange->GetDoorGroupGuid() == InGuid)
                return DoorHandleChange;
        }
    }

    DoorHandleChange = new FArmyWHCSingleDoorGroupHandleChange(InShapeInRoom->Shape.Get());
    DoorHandleChange->SetDoorGroupGuid(InGuid);
    InShapeInRoom->ChangeList.Emplace(MakeShareable(DoorHandleChange));

    return DoorHandleChange;
}

FArmyWHCSingleDrawerGroupHandleChange* FArmyWHCCabDoorHandleUIBase::GetOrCreateDrawerHandleChange(FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const
{
    FArmyWHCSingleDrawerGroupHandleChange * DrawerHandleChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_HANDLE))
        {
            DrawerHandleChange = static_cast<FArmyWHCSingleDrawerGroupHandleChange*>(Change.Get());
            if (DrawerHandleChange->GetDoorGroupGuid() == InGuid)
                return DrawerHandleChange;
        }
    }

    DrawerHandleChange = new FArmyWHCSingleDrawerGroupHandleChange(InShapeInRoom->Shape.Get());
    DrawerHandleChange->SetDoorGroupGuid(InGuid);
    InShapeInRoom->ChangeList.Emplace(MakeShareable(DrawerHandleChange));

    return DrawerHandleChange;
}

void FArmyWHCCabDoorHandleUIBase::SetDownloadDoorHandleResState(bool bSucceeded, SContentItem *UIItem)
{
    if (UIItem)
		UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
}

bool FArmyWHCCabDoorHandleUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRShapeFrame>();
}

AXRShapeFrame* FArmyWHCCabDoorHandleUI::GetShapeFrame() const
{
    return Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
}

void FArmyWHCCabDoorHandleUI::ReplacingDoorHandleItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, int32 Id)
{
    if (Container->IsClickedSameReplacingItem(InItemCode))
        return;

    for (auto &Animation : InShapeInRoom->Animations)
        Animation->Stop();
    TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
    for (auto &DoorGroup : DoorGroups)
    {  
        if (FSCTShapeUtilityTool::ReplaceDoorGroupMetal(DoorGroup, Id))
        {
            EShapeType Type = DoorGroup->GetShapeType();
            if (Type == ST_SideHungDoor)
            {
                FArmyWHCSingleDoorGroupHandleChange *HandleChange = GetOrCreateDoorHandleChange(InShapeInRoom, DoorGroup->GetShapeCopyFromGuid());
                HandleChange->SetHandleId(Id);    
            }
            else if (Type == ST_DrawerDoor)
            {
                FArmyWHCSingleDrawerGroupHandleChange *HandleChange = GetOrCreateDrawerHandleChange(InShapeInRoom, DoorGroup->GetShapeCopyFromGuid());
                HandleChange->SetHandleId(Id);      
            }
        }
    }
    InShapeInRoom->PrepareShapeAnimations();

    InShapeInRoom->MakeImmovable();
    FSCTShapeUtilityTool::SetActorTag(InShapeInRoom->ShapeFrame, XRActorTag::WHCActor);
}

TSharedRef<SWidget> FArmyWHCCabDoorHandleUI::MakeReplacingListPanelContent()
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
            DoorHandleCategoryComboBox.ToSharedRef()
        ]
    ];
}

TSharedPtr<SWidget> FArmyWHCDoorHandleUI::MakeWidget()
{
    MRangeUI = MakeShareable(TXRWHCAttrUIComponent<FArmyWHCModificationRangeUI>::MakeComponentInstance(Container, true));
    MRangeUI->MakeWidget();
    MRangeUI->SetMRange(SArmyWHCModeModificationRange::MR_All);
    return FArmyWHCCabDoorHandleUIBase::MakeWidget();
}

bool FArmyWHCDoorHandleUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

AXRShapeFrame* FArmyWHCDoorHandleUI::GetShapeFrame() const
{
    return Container->CabinetOperationRef->GetShapeFrameFromSelected();
}

void FArmyWHCDoorHandleUI::ReplacingDoorHandleItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, int32 Id)
{
    if (Container->IsClickedSameReplacingItem(InItemCode) && !MRangeUI->IsMRangeChanged())
        return;    
    MRangeUI->SyncMRange();
    switch ((SArmyWHCModeModificationRange::EModificationRange)MRangeUI->GetMRange())
    {
        case SArmyWHCModeModificationRange::MR_Single:
        {
            ReplacingSingleDoorOrDrawerHandle(InShapeInRoom, Id);
            break;
        }
        case SArmyWHCModeModificationRange::MR_SingleCab:
        {
            ReplacingSingleCabDoorOrDrawerHandle(InShapeInRoom, Id);
            break;
        }
        case SArmyWHCModeModificationRange::MR_SameType:
        {
            ReplacingSameTypeCabDoorOrDrawerHandle(InShapeInRoom, Id);
            break;
        }
        case SArmyWHCModeModificationRange::MR_All:
        {
            ReplacingAllCabDoorOrDrawerHandle(InShapeInRoom, Id);
            break;
        }
    }
}

TSharedRef<SWidget> FArmyWHCDoorHandleUI::MakeReplacingListPanelContent()
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
            DoorHandleCategoryComboBox.ToSharedRef()
        ]
    ];
}

void FArmyWHCDoorHandleUI::ReplacingDoorHandle(FShapeInRoom *InShapeInRoom, FSCTShape *InDoorGroupToReplace, int32 Id)
{
    if (FSCTShapeUtilityTool::ReplaceDoorGroupMetal(InDoorGroupToReplace, Id))
    {
        const FGuid &ShapeGuid = InDoorGroupToReplace->GetShapeCopyFromGuid();

        EShapeType DoorGroupType = InDoorGroupToReplace->GetShapeType();
        if (DoorGroupType == ST_SideHungDoor)
        {
            FArmyWHCSingleDoorGroupHandleChange *Change = GetOrCreateDoorHandleChange(InShapeInRoom, ShapeGuid);
            Change->SetHandleId(Id);
        }
        else if (DoorGroupType == ST_DrawerDoor)
        {
            FArmyWHCSingleDrawerGroupHandleChange *Change = GetOrCreateDrawerHandleChange(InShapeInRoom, ShapeGuid);
            Change->SetHandleId(Id);
        }   
    }
}

void FArmyWHCDoorHandleUI::ReplacingSingleDoorOrDrawerHandle(FShapeInRoom *InShapeInRoom, int32 Id)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        for (auto &Animation : InShapeInRoom->Animations)
            Animation->Stop();

        ReplacingDoorHandle(InShapeInRoom, DoorGroupShape, Id);

        InShapeInRoom->PrepareShapeAnimations();

        InShapeInRoom->MakeImmovable();
        FSCTShapeUtilityTool::SetActorTag(InShapeInRoom->ShapeFrame, XRActorTag::WHCActor);
    }
}

void FArmyWHCDoorHandleUI::ReplacingSingleCabDoorOrDrawerHandle(FShapeInRoom *InShapeInRoom, int32 Id)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        int32 DoorGroupId = DoorGroupShape->GetShapeId();

        for (auto &Animation : InShapeInRoom->Animations)
            Animation->Stop();

        TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
        for (auto &DoorGroup : DoorGroups)
        {
            if (DoorGroup->GetShapeId() == DoorGroupId)
                ReplacingDoorHandle(InShapeInRoom, DoorGroup, Id);
        }

        InShapeInRoom->PrepareShapeAnimations();

        InShapeInRoom->MakeImmovable();
        FSCTShapeUtilityTool::SetActorTag(InShapeInRoom->ShapeFrame, XRActorTag::WHCActor);
    }
}

void FArmyWHCDoorHandleUI::ReplacingSameTypeCabDoorOrDrawerHandle(FShapeInRoom *InShapeInRoom, int32 Id)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        int32 DoorGroupId = DoorGroupShape->GetShapeId();        
        int32 ShapeCategory = InShapeInRoom->Shape->GetShapeCategory();

        TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
        for (auto &PlacedShape : PlacedShapes)
        {
            if (PlacedShape->Shape->GetShapeCategory() == ShapeCategory)
            {
                for (auto &Animation : InShapeInRoom->Animations)
                    Animation->Stop();

                TArray<FDoorGroup*> DoorGroups = GetDoorGroups(PlacedShape.Get());
                for (auto &DoorGroup : DoorGroups)
                {
                    if (DoorGroup->GetShapeId() == DoorGroupId)
                        ReplacingDoorHandle(PlacedShape.Get(), DoorGroup, Id);
                }    

                PlacedShape->PrepareShapeAnimations();

                PlacedShape->MakeImmovable();
                FSCTShapeUtilityTool::SetActorTag(PlacedShape->ShapeFrame, XRActorTag::WHCActor);
            }
        }
    }
}

void FArmyWHCDoorHandleUI::ReplacingAllCabDoorOrDrawerHandle(FShapeInRoom *InShapeInRoom, int32 Id)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        int32 DoorGroupId = DoorGroupShape->GetShapeId();        

        TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
        for (auto &PlacedShape : PlacedShapes)
        {
            for (auto &Animation : InShapeInRoom->Animations)
                Animation->Stop();

            TArray<FDoorGroup*> DoorGroups = GetDoorGroups(PlacedShape.Get());
            for (auto &DoorGroup : DoorGroups)
            {
                if (DoorGroup->GetShapeId() == DoorGroupId)
                    ReplacingDoorHandle(PlacedShape.Get(), DoorGroup, Id);
            }    

            PlacedShape->PrepareShapeAnimations();

            PlacedShape->MakeImmovable();
            FSCTShapeUtilityTool::SetActorTag(PlacedShape->ShapeFrame, XRActorTag::WHCActor);
        }
    }   
}

FSCTShape* FArmyWHCDoorHandleUI::GetCurrentDoorGroup()
{
    ASCTShapeActor *ShapeActor = Cast<ASCTShapeActor>(Container->LastWHCItemSelected);
    EShapeType Type = ShapeActor->GetShape()->GetShapeType();
    if (Type == ST_VariableAreaDoorSheet_ForSideHungDoor) // 掩门选中的是门板
        return ShapeActor->GetShape()->GetParentShape();
    else if (Type == ST_SlidingDrawer) // 抽面替换材质选中的是抽屉
    {
        FSlidingDrawerShape *SlidingDrawerShape = static_cast<FSlidingDrawerShape*>(ShapeActor->GetShape());
        const TArray<TSharedPtr<FDrawerDoorShape>> &DrawerDoors = SlidingDrawerShape->GetDrawerDoorShapes();
        check(DrawerDoors.Num() > 0);
        return DrawerDoors[0].Get();
    }
    return nullptr;
}