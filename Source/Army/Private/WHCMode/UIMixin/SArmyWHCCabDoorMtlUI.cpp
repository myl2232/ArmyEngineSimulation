#include "UIMixin/SArmyWHCCabDoorMtlUI.h"
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
#include "Data/WHCModeData/XRWHCModeData.h"

#include "SCTCabinetShape.h"
#include "SCTDoorGroup.h"
#include "SCTSlidingDrawerShape.h"
#include "Actor/SCTShapeActor.h"
#include "SCTResManager.h"
#include "MaterialManager.h"
#include "UtilityTools/SCTShapeUtilityTools.h"

#include "WebImageCache.h"

TSharedPtr<SWidget> FArmyWHCCabDoorMtlUIBase::MakeWidget()
{
    TSharedPtr<SArmyWHCModeReplacingWidget> DoorMtlReplacing;
    SAssignNew(DoorMtlReplacing, SArmyWHCModeReplacingWidget)
        .ButtonImage(FArmyStyle::Get().GetBrush("WHCMode.Replace"))
        .OnWholeWidgetClicked_Raw(this, &FArmyWHCCabDoorMtlUIBase::Callback_ReplacingDoorMtl)
        .OnButtonClicked_Raw(this, &FArmyWHCCabDoorMtlUIBase::Callback_ReplacingDoorMtl);
    DoorMtlReplacing->SetReplacingItemThumbnail(TAttribute<const FSlateBrush*>::Create(
        [this]() -> const FSlateBrush* {
            if (IsValidSelection())
            {
                AXRShapeFrame *ShapeFrame = GetShapeFrame();
                if (ShapeFrame != nullptr)
                {
                    check(ShapeFrame->ShapeInRoomRef != nullptr);
                
                    FDoorSheetShapeBase *DoorSheet = GetFirstDoorSheet(ShapeFrame->ShapeInRoomRef);
                    if (DoorSheet)
                        return FArmySlateModule::Get().WebImageCache->Download(DoorSheet->GetMaterial().ThumbnailUrl)->GetBrush();
                }
                return FCoreStyle::Get().GetDefaultBrush();    
            }
            else
                return FCoreStyle::Get().GetDefaultBrush();
        }
    ));
    DoorMtlReplacing->SetReplacingItemText(TAttribute<FText>::Create(
        [this]() -> FText {
            if (IsValidSelection())
            {
                AXRShapeFrame *ShapeFrame = GetShapeFrame();
                if (ShapeFrame != nullptr)
                {
                    check(ShapeFrame->ShapeInRoomRef != nullptr);

                    FDoorSheetShapeBase *DoorSheet = GetFirstDoorSheet(ShapeFrame->ShapeInRoomRef);
                    if (DoorSheet)
                        return FText::FromString(DoorSheet->GetMaterial().Name);
                }
                return FText();    
            }
            else
                return FText();
        }
    ));
    SAssignNew(DoorMtlCategoryComboBox, SArmyWHCModeNormalComboBox)
        .OptionSource(TArray<TSharedPtr<FString>> { MakeShareable(new FString(TEXT("全部"))) })
        .Value(TEXT("全部"))
        .OnValueChanged_Raw(this, &FArmyWHCCabDoorMtlUIBase::Callback_DoorMtlCategoryChanged);
    return MoveTemp(DoorMtlReplacing);
}

void FArmyWHCCabDoorMtlUIBase::LoadListData()
{
    if (!IsValidSelection())
        return;
    LoadDoorMtlData();
}

void FArmyWHCCabDoorMtlUIBase::Callback_ReplacingDoorMtl()
{
    if (!IsValidSelection())
        return;

    FString DesireState(TEXT("门板材质"));
    if (Container->IsCurrentState(DesireState))
        return;
    
    Container->ListPanel->ClearListController();
    Container->ListPanel->SetListController(
        MakeReplacingListPanelContent()
    );

    if (Container->ReplacingSlot->GetWidget() == SNullWidget::NullWidget)
        Container->SetReplacingPanel(Container->ListPanel.ToSharedRef());
    
    Container->SetCurrentState(DesireState);

    CurrentDoorMtlCategory = 0;
    Container->ClearReplacingList();
    LoadDoorMtlCategoryData();
    LoadDoorMtlData();
}

void FArmyWHCCabDoorMtlUIBase::Callback_ReplacingDoorMtlItem(int32 InItemCode)
{
    if (!Container->CheckIsValidReplacingIndex(InItemCode))
        return;
    if (!IsValidSelection())
        return;

    int32 Id = Container->GetReplacingIds()[InItemCode];
    const TSharedPtr<FMtlInfo> * DoorMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
    check(DoorMtlPtr != nullptr);
    const TSharedPtr<FMtlInfo> & DoorMtl = *DoorMtlPtr;
    if (DoorMtl->RcState == ECabinetResourceState::CabRc_None)
    {
        FString CachePath = FSCTShapeUtilityTool::GetFullCachePathByMaterialTypeAndID(EMaterialType::Board_Material, Id);

        TArray<FDownloadSpace::FDownloadFileInfo> FileInfos;
        WHC_ADD_DOWNLOAD_TASK(FileInfos, Id, CachePath, DoorMtl->MtlUrl, DoorMtl->MtlMd5);

        SContentItem * Item = Container->GetListItem(InItemCode);
        if (FileInfos.Num() == 0)
        {
            SetDownloadDoorMtlResState(false, Item, DoorMtl.Get());
            return;
        }

        if (Item == nullptr)
            return;
        Item->SetItemState(EDownloading);

        DoorMtl->RcState = ECabinetResourceState::CabRc_FetchingRc;

        TSharedPtr<FDownloadFileSet> NewTask = FArmyDownloadModule::Get().GetDownloadManager()->AddMultiTask(DoorMtl->MtlUrl, FileInfos);
        NewTask->OnDownloadFinished.BindLambda(
            [this](bool bSucceeded, FString InCachePath, int32 Id)
            {
                SContentItem * Item = Container->GetListItemMatchingType(Id, EWHCModeAttrType::EWHCAttr_DoorMtl);

                const TSharedPtr<FMtlInfo> * DoorMtlPtr = FWHCModeGlobalData::CabinetMtlMap.Find(Id);
                check(DoorMtlPtr != nullptr);
                const TSharedPtr<FMtlInfo> & DoorMtl = *DoorMtlPtr;

                if (bSucceeded)
                {
                    DoorMtl->Mtl = FSCTXRResourceManagerInstatnce::GetIns().GetResourceManager().CreateCustomMaterial(InCachePath, DoorMtl->MtlParam);
                    if (DoorMtl->Mtl != nullptr)
                    {
                        DoorMtl->Mtl->AddToRoot();
                        SetDownloadDoorMtlResState(true, Item, DoorMtl.Get());
                        return;
                    }
                }

                SetDownloadDoorMtlResState(false, Item, DoorMtl.Get());
            },
            CachePath,
            Id
        );
    }
    else if (DoorMtl->RcState == ECabinetResourceState::CabRc_Complete)
    {
        AXRShapeFrame *ShapeFrame = GetShapeFrame();
        check(ShapeFrame->ShapeInRoomRef != nullptr);
        ReplacingDoorMtlItemInternal(InItemCode, ShapeFrame->ShapeInRoomRef, DoorMtl.Get());
    }
}

void FArmyWHCCabDoorMtlUIBase::Callback_DoorMtlCategoryChanged(const FString &InNewCategory)
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);
    FDoorSheetShapeBase *DoorSheet = GetFirstDoorSheet(ShapeFrame->ShapeInRoomRef);
    if (DoorSheet == nullptr)
        return;

    int32 Id = DoorSheet->GetShapeId();

    TArray<TPair<int32, FString>> * DoorMtlCategory = DoorMtlCategoryMap.Find(Id);
    check(DoorMtlCategory != nullptr);
    for (auto & Category : *DoorMtlCategory)
    {
        if (Category.Value == InNewCategory)
        {
            CurrentDoorMtlCategory = Category.Key;
            DoorMtlCategoryComboBox->SetValue(Category.Value);
            Container->ClearReplacingList();
            LoadDoorMtlData();
            break;
        }
    }
}

void FArmyWHCCabDoorMtlUIBase::LoadDoorMtlData()
{
    if (Container->CheckNoMoreData())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    FDoorSheetShapeBase *DoorSheet = GetFirstDoorSheet(ShapeFrame->ShapeInRoomRef);
    if (DoorSheet == nullptr)
        return;
    int32 Id = DoorSheet->GetShapeId();
    FGuid Guid = DoorSheet->GetParentShape()->GetShapeCopyFromGuid();

    Container->RunHttpRequest(
        FString::Printf(TEXT("design/door/cabinetsProduct/%d/%d/materials?categoryId=%d&current=%d&size=15&shapeGuid=%s"), 
            ShapeFrame->ShapeInRoomRef->Shape->GetShapeId(), 
            Id, 
            CurrentDoorMtlCategory, 
            Container->GetPageIndexToLoad(),
            *(Guid.ToString())
        ),
        TBaseDelegate<void, const TSharedPtr<FJsonObject>&>::CreateLambda(
            [this](const TSharedPtr<FJsonObject> &ResponseData)
            {
                const TSharedPtr<FJsonObject> &Data = ResponseData->GetObjectField(TEXT("data"));
                if (Data.IsValid())
                {
                    Container->SetReplacingTotalPage(Data->GetNumberField(TEXT("totalPage")));
                
                    const TArray<TSharedPtr<FJsonValue>>* DoorMtlArray = nullptr;
                    if (Data->TryGetArrayField(TEXT("list"), DoorMtlArray))
                    {
                        int32 nCount = (*DoorMtlArray).Num();
                        int32 IndexStart = Container->GetReplacingIds().Num();
                        for (int32 i = 0; i < nCount; ++i)
                        {
                            const TSharedPtr<FJsonValue> &Value = DoorMtlArray->operator[](i);
                            const TSharedPtr<FJsonObject> &DoorMtlJsonObject = Value->AsObject();

                            int32 Id = DoorMtlJsonObject->GetNumberField(TEXT("id"));
                            Container->AddReplacingId(Id);

                            if (FWHCModeGlobalData::CabinetMtlMap.Find(Id) == nullptr)
                            {
                                FMtlInfo * NewDoorMtlInfo = new FMtlInfo;
                                NewDoorMtlInfo->Deserialize(DoorMtlJsonObject);
                                FWHCModeGlobalData::CabinetMtlMap.Emplace(Id, MakeShareable(NewDoorMtlInfo));
                            }
                        }
                        FillDoorMtlList(IndexStart, nCount);
                    }
                }
            }
        )
    );
}

void FArmyWHCCabDoorMtlUIBase::FillDoorMtlList(int32 InStart, int32 InCount)
{
    if (!IsValidSelection())
        return;

    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    FDoorSheetShapeBase *DoorSheet = GetFirstDoorSheet(ShapeFrame->ShapeInRoomRef);
    if (DoorSheet == nullptr)
        return;
    int32 MtlId = DoorSheet->GetMaterial().ID;

    int32 MatchingIndex = -1;
    TArray<int32> CabIds = Container->GetReplacingIds();
    for (int32 i = 0; i < InCount; ++i)
	{
        const TSharedPtr<FMtlInfo> * DoorMtlInfoPtr = FWHCModeGlobalData::CabinetMtlMap.Find(CabIds[InStart + i]);
        check(DoorMtlInfoPtr != nullptr);
        const TSharedPtr<FMtlInfo> & DoorMtlInfo = *DoorMtlInfoPtr;
		TSharedPtr<SContentItem> Item = SNew(SContentItem)
            .ItemCode(InStart + i)
            .ItemWidth(112.0f)
            .ItemHeight(132.0f)
			.TextStyle(&FArmyStyle::Get().GetWidgetStyle<FTextBlockStyle>("ArmyText_12"))
			.TextColor(FArmyStyle::Get().GetColor("Color.FF969799"))
			.ThumbnailURL(DoorMtlInfo->MtlThumbnailUrl)
			.ThumbnailImage(SNew(SImage).Image(FArmyStyle::Get().GetBrush("Icon.DefaultImage_ContentBrowser")))//梁晓菲 默认图
			.bShowDownloadTips(DoorMtlInfo->Mtl == nullptr)
			.OnClicked_Raw(this, &FArmyWHCCabDoorMtlUIBase::Callback_ReplacingDoorMtlItem)
			.DisplayText(FText::FromString(DoorMtlInfo->MtlName));
        Item->AddMetadata<FWHCModeAttrTypeInfo>(MakeShared<FWHCModeAttrTypeInfo>(EWHCModeAttrType::EWHCAttr_DoorMtl, DoorMtlInfo->Id));
	
        switch (DoorMtlInfo->RcState)
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

        if (MtlId == DoorMtlInfo->Id)
            MatchingIndex = InStart + i;
	}

    Container->ListPanel->SetSelectedItem(MatchingIndex);
	Container->ListPanel->Container->ResetScrollNotify();
}

void FArmyWHCCabDoorMtlUIBase::LoadDoorMtlCategoryData()
{
    AXRShapeFrame *ShapeFrame = GetShapeFrame();
    check(ShapeFrame->ShapeInRoomRef != nullptr);

    FDoorSheetShapeBase *DoorSheet = GetFirstDoorSheet(ShapeFrame->ShapeInRoomRef);
    if (DoorSheet == nullptr)
        return;
    int32 Id = DoorSheet->GetShapeId();

    TArray<TPair<int32, FString>> * DoorMtlCategory = DoorMtlCategoryMap.Find(Id);
    if (DoorMtlCategory == nullptr)
    {
        Container->RunHttpRequest(FString::Printf(TEXT("design/door/%d/materials/category"), Id),
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
                    DoorMtlCategoryMap.Emplace(Id, CategoryStrings);
                    FillDoorMtlCategoryComboBox(CategoryStrings);
                }
            )
        );
    }
    else
        FillDoorMtlCategoryComboBox(*DoorMtlCategory);
}

void FArmyWHCCabDoorMtlUIBase::FillDoorMtlCategoryComboBox(const TArray<TPair<int32, FString>> &InDoorMtlCategory)
{
    TArray<TSharedPtr<FString>> OptionSource;
    for (const auto & OneCategory : InDoorMtlCategory)
        OptionSource.Emplace(MakeShareable(new FString(OneCategory.Value)));
    DoorMtlCategoryComboBox->SetOptionSource(OptionSource);
}

TArray<FDoorGroup*> FArmyWHCCabDoorMtlUIBase::GetDoorGroups(FShapeInRoom *InShapeInRoom) const
{
    FCabinetShape *CabShape = static_cast<FCabinetShape*>(InShapeInRoom->Shape.Get());

    TArray<FDoorGroup*> DoorGroups;
    XRShapeUtilities::GetCabinetDoors(CabShape, DoorGroups, FLAG_SIDEHUNGDOORS);
    TArray<FDoorGroup*> DrawerDoorGroups;
    XRShapeUtilities::GetCabinetDrawerDoors(CabShape, DrawerDoorGroups);
    DoorGroups.Append(DrawerDoorGroups);
    return MoveTemp(DoorGroups);
}

FDoorSheetShapeBase* FArmyWHCCabDoorMtlUIBase::GetFirstDoorSheet(FShapeInRoom *InShapeInRoom) const
{
    TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
    if (DoorGroups.Num() > 0)
    {
        FDoorGroup *DoorGroup = DoorGroups[0];
        switch (DoorGroup->GetShapeType())
        {
            case ST_SideHungDoor:
            {
                FSideHungDoor *SideHungDoor = static_cast<FSideHungDoor*>(DoorGroup);
                return SideHungDoor->GetDoorSheetNum() > 0 ? SideHungDoor->GetDoorSheets()[0].Get() : nullptr;
            }
            case ST_DrawerDoor:
            {
                FDrawerDoorShape *DrawerDoor = static_cast<FDrawerDoorShape*>(DoorGroup);
                return DrawerDoor->GetDrawDoorSheet().Get();
            }
            default:
                return nullptr;
        }
    }
    else
        return nullptr; 
}

FArmyWHCSingleDoorGroupMtlChange* FArmyWHCCabDoorMtlUIBase::GetOrCreateDoorMtlChange(FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const
{
    FArmyWHCSingleDoorGroupMtlChange * DoorMtlChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DOOR_GROUP_MTL))
        {
            DoorMtlChange = static_cast<FArmyWHCSingleDoorGroupMtlChange*>(Change.Get());
            if (DoorMtlChange->GetDoorGroupGuid() == InGuid)
                return DoorMtlChange;
        }
    }

    DoorMtlChange = new FArmyWHCSingleDoorGroupMtlChange(InShapeInRoom->Shape.Get());
    DoorMtlChange->SetDoorGroupGuid(InGuid);
    InShapeInRoom->ChangeList.Emplace(MakeShareable(DoorMtlChange));

    return DoorMtlChange;
}

FArmyWHCSingleDrawerGroupMtlChange* FArmyWHCCabDoorMtlUIBase::GetOrCreateDrawerMtlChange(FShapeInRoom *InShapeInRoom, const FGuid &InGuid) const
{
    FArmyWHCSingleDrawerGroupMtlChange * DrawerMtlChange = nullptr;
    for (const auto & Change : InShapeInRoom->ChangeList)
    {
        if (Change->IsSameType(SHAPE_CHANGE_SINGLE_DRAWER_GROUP_MTL))
        {
            DrawerMtlChange = static_cast<FArmyWHCSingleDrawerGroupMtlChange*>(Change.Get());
            if (DrawerMtlChange->GetDoorGroupGuid() == InGuid)
                return DrawerMtlChange;
        }
    }

    DrawerMtlChange = new FArmyWHCSingleDrawerGroupMtlChange(InShapeInRoom->Shape.Get());
    DrawerMtlChange->SetDoorGroupGuid(InGuid);
    InShapeInRoom->ChangeList.Emplace(MakeShareable(DrawerMtlChange));

    return DrawerMtlChange;
}

void FArmyWHCCabDoorMtlUIBase::SetDownloadDoorMtlResState(bool bSucceeded, SContentItem *UIItem, FMtlInfo *DataItem)
{
    if (UIItem)
        UIItem->SetItemState(bSucceeded ? EReady : ENotReady);
    if (DataItem)
        DataItem->RcState = bSucceeded ? ECabinetResourceState::CabRc_Complete : ECabinetResourceState::CabRc_None;
}

bool FArmyWHCCabDoorMtlUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<AXRShapeFrame>();
}

AXRShapeFrame* FArmyWHCCabDoorMtlUI::GetShapeFrame() const
{
    return Cast<AXRShapeFrame>(Container->LastWHCItemSelected);
}

void FArmyWHCCabDoorMtlUI::ReplacingDoorMtlItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    if (Container->IsClickedSameReplacingItem(InItemCode))
        return;

    FDoorSheetShapeBase * DoorSheet = GetFirstDoorSheet(InShapeInRoom);
    if (DoorSheet && DoorSheet->HasReplaceMaterial())
    {
        TSharedPtr<FCommonPakData> PakData = XRShapeUtilities::CreateMtlPakData(InMtlInfo);
        TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
        for (auto &DoorGroup : DoorGroups)
        {
            FSCTShapeUtilityTool::ReplaceDoorGroupMaterial(DoorGroup, PakData.Get(), InMtlInfo->Mtl);
            EShapeType DoorType = DoorGroup->GetShapeType();
            if (DoorType == ST_SideHungDoor)
            {
                FArmyWHCSingleDoorGroupMtlChange *DoorMtlChange = GetOrCreateDoorMtlChange(InShapeInRoom, DoorGroup->GetShapeCopyFromGuid());
                DoorMtlChange->SetDoorMtlId(InMtlInfo->Id);
            }
            else if (DoorType == ST_DrawerDoor)
            {
                FArmyWHCSingleDrawerGroupMtlChange *DrawerDoorMtlChange = GetOrCreateDrawerMtlChange(InShapeInRoom, DoorGroup->GetShapeCopyFromGuid());
                DrawerDoorMtlChange->SetDoorMtlId(InMtlInfo->Id);
            }
        }    
        Container->ListPanel->SetSelectedItem(InItemCode);
    }
}

TSharedRef<SWidget> FArmyWHCCabDoorMtlUI::MakeReplacingListPanelContent()
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
            DoorMtlCategoryComboBox.ToSharedRef()
        ]
    ];   
}

TSharedPtr<SWidget> FArmyWHCDoorMtlUI::MakeWidget()
{
    MRangeUI = MakeShareable(TXRWHCAttrUIComponent<FArmyWHCModificationRangeUI>::MakeComponentInstance(Container, true));
    MRangeUI->MakeWidget();
    MRangeUI->SetMRange(SArmyWHCModeModificationRange::MR_All);
    return FArmyWHCCabDoorMtlUIBase::MakeWidget();
}

bool FArmyWHCDoorMtlUI::IsValidSelection() const
{
    return Container->IsWHCItemSelected<ASCTShapeActor>();
}

AXRShapeFrame* FArmyWHCDoorMtlUI::GetShapeFrame() const
{
    return Container->CabinetOperationRef->GetShapeFrameFromSelected();
}

void FArmyWHCDoorMtlUI::ReplacingDoorMtlItemInternal(int32 InItemCode, FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    if (Container->IsClickedSameReplacingItem(InItemCode) && !MRangeUI->IsMRangeChanged())
        return;    
    MRangeUI->SyncMRange();
    switch ((SArmyWHCModeModificationRange::EModificationRange)MRangeUI->GetMRange())
    {
        case SArmyWHCModeModificationRange::MR_Single:
        {
            ReplacingSingleDoorOrDrawerMtl(InShapeInRoom, InMtlInfo);
            break;
        }
        case SArmyWHCModeModificationRange::MR_SingleCab:
        {
            ReplacingSingleCabDoorOrDrawerMtl(InShapeInRoom, InMtlInfo);
            break;
        }
        case SArmyWHCModeModificationRange::MR_SameType:
        {
            ReplacingSameTypeCabDoorOrDrawerMtl(InShapeInRoom, InMtlInfo);
            break;
        }
        case SArmyWHCModeModificationRange::MR_All:
        {
            ReplacingAllCabDoorOrDrawerMtl(InShapeInRoom, InMtlInfo);
            break;
        }
    }
    Container->ListPanel->SetSelectedItem(InItemCode);
}

TSharedRef<SWidget> FArmyWHCDoorMtlUI::MakeReplacingListPanelContent()
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
            DoorMtlCategoryComboBox.ToSharedRef()
        ]
    ];
}

void FArmyWHCDoorMtlUI::ReplacingDoorMtl(FShapeInRoom *InShapeInRoom, FSCTShape *InDoorGroupToReplace, FMtlInfo *InMtlInfo)
{
    TSharedPtr<FCommonPakData> PakData = XRShapeUtilities::CreateMtlPakData(InMtlInfo);
    FSCTShapeUtilityTool::ReplaceDoorGroupMaterial(InDoorGroupToReplace, PakData.Get(), InMtlInfo->Mtl);

    const FGuid &ShapeGuid = InDoorGroupToReplace->GetShapeCopyFromGuid();

    EShapeType DoorGroupType = InDoorGroupToReplace->GetShapeType();
    if (DoorGroupType == ST_SideHungDoor)
    {
        FArmyWHCSingleDoorGroupMtlChange *Change = GetOrCreateDoorMtlChange(InShapeInRoom, ShapeGuid);
        Change->SetDoorMtlId(InMtlInfo->Id);
    }
    else if (DoorGroupType == ST_DrawerDoor)
    {
        FArmyWHCSingleDrawerGroupMtlChange *Change = GetOrCreateDrawerMtlChange(InShapeInRoom, ShapeGuid);
        Change->SetDoorMtlId(InMtlInfo->Id);
    }
}

void FArmyWHCDoorMtlUI::ReplacingSingleDoorOrDrawerMtl(FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
        ReplacingDoorMtl(InShapeInRoom, DoorGroupShape, InMtlInfo);
}

void FArmyWHCDoorMtlUI::ReplacingSingleCabDoorOrDrawerMtl(FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        int32 Id = DoorGroupShape->GetShapeId();

        TArray<FDoorGroup*> DoorGroups = GetDoorGroups(InShapeInRoom);
        for (auto &DoorGroup : DoorGroups)
        {
            if (DoorGroup->GetShapeId() == Id)
                ReplacingDoorMtl(InShapeInRoom, DoorGroup, InMtlInfo);
        }
    
        // 更换饰条材质
        ReplacingInsertionBoardMtl(InShapeInRoom, InMtlInfo);
        
        // 通知门板材质变化
        InShapeInRoom->OnChangeDoorMtl();    
    }
}

void FArmyWHCDoorMtlUI::ReplacingSameTypeCabDoorOrDrawerMtl(FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        int32 Id = DoorGroupShape->GetShapeId();        
        int32 ShapeCategory = InShapeInRoom->Shape->GetShapeCategory();

        TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
        for (auto &PlacedShape : PlacedShapes)
        {
            if (PlacedShape->Shape->GetShapeCategory() == ShapeCategory)
            {
                TArray<FDoorGroup*> DoorGroups = GetDoorGroups(PlacedShape.Get());
                if (DoorGroups.Num() > 0)
                {
                    for (auto &DoorGroup : DoorGroups)
                    {
                        if (DoorGroup->GetShapeId() == Id)
                            ReplacingDoorMtl(PlacedShape.Get(), DoorGroup, InMtlInfo);
                    }
                }

                ReplacingInsertionBoardMtl(PlacedShape.Get(), InMtlInfo);   
            }
        }
    }
}

void FArmyWHCDoorMtlUI::ReplacingAllCabDoorOrDrawerMtl(FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    FSCTShape *DoorGroupShape = GetCurrentDoorGroup();
    if (DoorGroupShape != nullptr)
    {
        int32 Id = DoorGroupShape->GetShapeId();        

        TArray<TSharedPtr<FShapeInRoom>> &PlacedShapes = FArmyWHCabinetMgr::Get()->GetPlaceShapes();
        for (auto &PlacedShape : PlacedShapes)
        {
            TArray<FDoorGroup*> DoorGroups = GetDoorGroups(PlacedShape.Get());
            if (DoorGroups.Num() > 0)
            {
                for (auto &DoorGroup : DoorGroups)
                {
                    if (DoorGroup->GetShapeId() == Id)
                        ReplacingDoorMtl(PlacedShape.Get(), DoorGroup, InMtlInfo);
                }               
            }

            ReplacingInsertionBoardMtl(PlacedShape.Get(), InMtlInfo);
        }
    }   
}

void FArmyWHCDoorMtlUI::ReplacingInsertionBoardMtl(FShapeInRoom *InShapeInRoom, FMtlInfo *InMtlInfo)
{
    TSharedPtr<FCommonPakData> MtlData = XRShapeUtilities::CreateMtlPakData(InMtlInfo);
    FSCTShapeUtilityTool::ReplacInsertionBoardMaterial(InShapeInRoom->Shape.Get(), MtlData.Get(), InMtlInfo->Mtl);    

    FArmyWHCCabinetInsertionBoardMtlChange* InsertionBoardChange = GetOrCreateInsertionBoardMtlChange(InShapeInRoom);
    InsertionBoardChange->SetInsertionBoardMtlId(MtlData->ID);
}

FArmyWHCCabinetInsertionBoardMtlChange* FArmyWHCDoorMtlUI::GetOrCreateInsertionBoardMtlChange(FShapeInRoom *InShapeInRoom)
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

FSCTShape* FArmyWHCDoorMtlUI::GetCurrentDoorGroup()
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

FSCTShape* FArmyWHCDoorMtlUI::GetDoorSheet(FDoorGroup *InDoorGroup)
{
    EShapeType DoorType = InDoorGroup->GetShapeType();
    if (DoorType == ST_SideHungDoor)
    {
        FSideHungDoor *SideHungDoor = static_cast<FSideHungDoor*>(InDoorGroup);
        const TArray<TSharedPtr<FSideHungDoor::FVariableAreaDoorSheetForSideHungDoor>> & DoorSheets = SideHungDoor->GetDoorSheets();
        check(DoorSheets.Num() > 0);
        return DoorSheets[0].Get();
    }
    else if (DoorType == ST_DrawerDoor)
    {
        FDrawerDoorShape *DrawerDoor = static_cast<FDrawerDoorShape*>(InDoorGroup);
        TSharedPtr<FDrawerDoorShape::FVariableAreaDoorSheetForDrawer> DrawerDoorSheet = DrawerDoor->GetDrawDoorSheet();
        check(DrawerDoorSheet.IsValid());
        return DrawerDoorSheet.Get();
    }   
    else
        return nullptr;
}